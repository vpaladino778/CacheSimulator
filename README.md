# CacheSimulator
Determines if cache hit or cache miss depending on inputted cache parameters.

## How to use:
The first four lines of input contain the fundamental cache parameters.
1. S - Number of sets
2. E - Number of lines per set
3. B - Block size (in bytes)
4. m - Number of physical (main memory) address bits
5. The 5th line is the replacement policy (LFU or LRU)
6. The last two lines represent the hit time and the miss penalty respectively. Both are integers

### Example Usage:

This gives us a cache with 4 sets, each set has only 1 line and a block size of 8. The main memory address has 8 bits. This cache
uses a 'Least Frequently Used' replacement policy. The cache hit time is 1 and the cache miss pentality is 30.
```
4
1
8
8
LFU
1
30
20
22
24
26
28
2a
30
```
Below is the corresponding output:
 ```
20 M
22 H
24 H
26 H
28 M
2a H
30 M
```
