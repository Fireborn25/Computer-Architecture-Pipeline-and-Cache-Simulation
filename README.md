# Computer Architecture- Pipeline and Cache

## Cache
We design a simulator for an arbitrary n-way set associative cache in C++. The simulator implements 2 level caches, L1 and L2. The Simulator reads trace files and assigns requests to the L1
cache. L1 cache sends read/write requests to the L2 cache. L2 cache interacts with DRAM. L1 and L2 caches keep track of their own counters i.e. reads, writes, misses, hits etc. At the end of
the simulation, for a given trace file, our program should print the stats for both caches on the console, as follows:
i. number of L1 reads: ____________
ii. number of L1 read misses:____________
iii. number of L1 writes: ____________
iv. number of L1 write misses:____________
v. L1 miss rate: ____________
vi. number of writebacks from L1 memory: _____________
vii. number of L2 reads: _____________
viii. number of L2 read misses:____________
ix. number of L2 writes: _____________
x. number of L2 write misses:_____________
xi. L2 miss rate: _____________
xii. number of writebacks from L2 memory:______________
Configurable Parameters
● BLOCKSIZE: Number of bytes in a block/line.
● L1_Size: Total bytes of data storage for L1 Cache.
● L1_Assoc: Associativity of L1 Cache.
● L2_Size: Total bytes of data storage for L2 Cache.
● L2_Assoc: Associativity of L2 Cache.
For example: “./cache_simulate 64 1024 2 65536 8 memory_trace_files/trace1.txt” means the
following values for the configurable parameters.
BLOCKSIZE: 64
L1_SIZE: 1024
L1_ASSOC: 2
L2_SIZE: 65536
L2_ASSOC: 8
All parameters should be power of 2.
Non-configurable Parameters
● Cache eviction policy: Simulator uses least replacement policy(LRU) - this means you
need to keep track of which block was used least recently
● Cache write policy: Simulator uses Write-back Write-allocate(WBWA) policy - this means

### Observations
1. When Block size increases, generally access time decreases as now we have more space for storing addresses with
different tag bits in a single set but as the sizes keep increasing, we see a decrease in access time as more eviction occurs
due to which we need to bring data again and again
2. With increasing size of L1, and keeping block size, L1 associativity, L2 size and L2 associativity fixed at 64, 2, 65536,
8, access time decreases as the number of sets increases which leads to storing more address and helps in reducing
eviction, that is reducing conflict misses
3. with increasing L1 associativity for trace5,6,7,8 access time decreases sharply while for tarce1,2,3,4 decreases gradually
4. with increasing L2 size for trace14568 time access more or less remains same but for trace27 it decreases gradually , it
do reduces conflict misses
5. with increasing L2 associativity for trace5-6 access time is approximately same while for remaining it shows either
remains constant or gradually decreases
you need to keep track of dirty blocks, so that they are written back to the level n+1 of
memory hierarchy, before eviction from level n
