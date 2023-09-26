# Computer Architecture- Pipeline and Cache

## Pipeline
A. 5 Stage Basic MIPS pipeline without bypassing
=====================================================================
Implemented in C++ the basic 5 stage MIPS pipeline — IF (Instruction Fetch), ID (Instruction Decode), EX
(ALU execute), MEM (memory read/write) and WB (write back to register).
Our program has 32 registers as a data structure. The control signals, input and output for each
pipeline stage is implemented as a struct, with a separate function to implement the working of the
stage. The function for EX stage has support for some basic ALU operations like add, sub, addi, sll,
srl, and, or, slt etc. assuming instructions have only those ALU operations.
WB should happen in the first half cycle, and the rest of the stages in the second half cycle. Cycles can
be implemented as loop iterations. Total cycles will depend on data dependencies among different
instructions.
Output should print cycle number, first vs. second half cycle in that cycle number and what each
pipeline stage is doing in that cycle.
Implement this program in a C++ file with the name 5stage.cpp. You can test this with instruction pairs,
as discussed in class. Check total cycles taken by the simulator, vs. calculated with pen and paper. Some
examples of instruction pairs are given below:
(i) add $1, $2, $3; lw $4, 8($1)
(ii) lw $1, 8($2); lw $4, 8($1)
(iii) lw $1, 8($2); sw $1, 8($3)
(iv) lw $1, 8($2); add $4, $1, $3
=====================================================================
B. 5 Stage Basic MIPS pipeline with bypassing
=====================================================================
Implemented 5stage_bypass.cpp, where we forward input to the EX/MEM stages, not just from L3, but
from L4 and L5. Test this program also with instruction pairs as above, and note the improvements in total
cycles compared to no bypassing.
=====================================================================
C. 7-9 Stage pipeline without and with bypassing
=====================================================================
Implemented the 7-9 stage pipeline, with 7 stages for ALU operations and 9 stages for
load-store operations. Without bypassing source file should be named 79stage.cpp and with bypassing
source file should be named 79stage_bypass.cpp. Test this program also with instruction pairs as
above, and note the improvements/degradations in total cycles compared to 5 stage pipeline..
=====================================================================
D. Branch prediction for control hazard
=====================================================================
Implemented a branch predictor with 2-bit saturating counters and branch history registers (BHR).
Use 2^14 2-bit saturating counters indexed using 14 LSBs of each branch instruction and a 2 bit
Branch History Register (BHR) that records the outcomes of the previous 2 branches.
branchtrace is a trace of branch instructions consisting of the PC at which each branch occurs,
and whether the branch is Taken(1) or Not Taken(0).
Predict varying the start state of (a) each 2-bit saturating counter between 00,01,10,11 and (b)
each BHR also as 00,01,10,11. Predict using only saturating counters, only BHRs and some
ways of combining the two. For each of these variations, your output file should indicate, for
each branch in the trace, whether it was predicted as Taken(1) or Not Taken(0). Compute
branch prediction accuracy of each output file, as (#correct predictions/#total predictions).
Include these branch prediction accuracies in a table in the PDF report above. Each row in the
table should mention the prediction strategy and the corresponding accuracy.


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

![image](https://github.com/Fireborn25/Quine-McCluskey_Simulation/assets/97736756/74f00338-3e87-4815-81f3-33c99395b0e1)

![image](https://github.com/Fireborn25/Quine-McCluskey_Simulation/assets/97736756/c15cb624-d790-4b6a-8381-dce740dcf955)

![image](https://github.com/Fireborn25/Quine-McCluskey_Simulation/assets/97736756/f362c877-7d0e-42d3-af3a-baa4eecc7201)




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
