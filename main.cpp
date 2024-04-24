#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <chrono>
#include <random>

#include <numa.h>

#include <unistd.h>

struct link_t
{
	link_t * next;                       // Pointer to the next element
	char     pad[64 - sizeof(link_t *)]; // Padding to fill a 64B cache line
};

float numa_latency(const int t_node, const int m_node, const size_t size)
{
	static constexpr size_t its   = 1000000;
	static constexpr float  its_f = its * 1.0;

	const size_t n_elems = size / sizeof(link_t);

	numa_run_on_node(t_node);

	link_t * list = (link_t *) numa_alloc_onnode(size, m_node);

	if (list == nullptr)
	{
		fprintf(stderr, "Cannot allocate %lu bytes on node %d\n", size, m_node);
		exit(EXIT_FAILURE);
	}

	// Populate the list
	for (size_t i = 0; i < n_elems; ++i)
	{
		list[i].next = list + i + 1;
	}
	list[n_elems - 1].next = list;

	// Shuffle the list
	std::shuffle(list, list + n_elems, std::mt19937(std::random_device()()));

	// Traverse the list
	const auto start = std::chrono::high_resolution_clock::now();

	link_t * node = &list[0];
	for (size_t i = 0; i < its; ++i)
	{
		node = node->next;
	}

	const auto end      = std::chrono::high_resolution_clock::now();
	const auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

	numa_free(list, sizeof(link_t));

	return static_cast<float>(total_ns) / static_cast<float>(its);
}

size_t print_caches();

int main(const int argc, const char * argv[])
{
	if (numa_available() == -1)
	{
		fprintf(stderr, "NUMA not available\n");
		exit(EXIT_FAILURE);
	}

	const size_t l3_size = print_caches();

	size_t size = 4 * l3_size;

	if (argc > 1) { size = atoll(argv[1]); }

	printf("Allocating %lu KB\n", size / 1024);

	const int max_node = numa_max_node();

	printf("\t");
	for (int node = 0; node <= max_node; ++node)
	{
		printf("Node %d\t", node);
	}
	printf("\n");

	for (int t_node = 0; t_node <= max_node; ++t_node)
	{
		printf("Node %d\t", t_node);
		fflush(stdout);
		for (int m_node = 0; m_node <= max_node; ++m_node)
		{
			const float ns = numa_latency(t_node, m_node, size);
			printf("%.1f\t", ns);
			fflush(stdout);
		}
		printf("\n");
		fflush(stdout);
	}

	return EXIT_SUCCESS;
}

size_t print_caches()
{
	const int l1_size      = sysconf(_SC_LEVEL1_DCACHE_SIZE);
	const int l1_line_size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
	const int l1_assoc     = sysconf(_SC_LEVEL1_DCACHE_ASSOC);

	const int l2_size      = sysconf(_SC_LEVEL2_CACHE_SIZE);
	const int l2_line_size = sysconf(_SC_LEVEL2_CACHE_LINESIZE);
	const int l2_assoc     = sysconf(_SC_LEVEL2_CACHE_ASSOC);

	const int l3_size      = sysconf(_SC_LEVEL3_CACHE_SIZE);
	const int l3_line_size = sysconf(_SC_LEVEL3_CACHE_LINESIZE);
	const int l3_assoc     = sysconf(_SC_LEVEL3_CACHE_ASSOC);

	printf("L1: %d KB, %d lines, %d-way\n", l1_size / 1024, l1_line_size, l1_assoc);
	printf("L2: %d KB, %d lines, %d-way\n", l2_size / 1024, l2_line_size, l2_assoc);
	printf("L3: %d KB, %d lines, %d-way\n", l3_size / 1024, l3_line_size, l3_assoc);

	return l3_size;
}