
# HW5
Weihang Qin
Liam Goodwin

## Hit Rate and choice of parameters

We generate a request by the following procedure:

1. choose a random key represented by a random number from 1 to 1000
2. choose a random operation from GET, SET , and DEL.
3. If the operation is GET or DEL, perform the operation on the generated key.
4. If the operation is SET, generate a random value of size 1 with probability 0.9, size 10 with probability 0.09 and size 100 with probability 0.01.

The proportion of GET,SET,DEL and of value sizes are analogous to that from the paper. Key reuse is captured by the fact that random value is repeated with some chance. 

That done, we adjust the maxmem parameter on the server side to get the desired hit rate of 0.8.  In our final version maxmem is set to 2200. This maintains a long-term hit rate of 0.81. (long-term meaning we are not counting the obligatory misses when the cache is empty)


##  Baseline latency and throughput

In this part we measured the latency and throughput of http requests under the parameters specified above. 

Our histogram of average request time in milliseconds for a hundred measurements is plotted below. (Note that there were two extreme outliers 32 and 41 that were not shown in the graph. One possible reason is that they are the SET requests with long values. )


![Histogram of 100 measures](https://github.com/squinkums/CSSystemsHW5/blob/master/Plot.png?raw=true)

The 95th-percentile latency is 10 milliseconds and the mean throughput is  124.844 requests/sec. 

## Sensitivity
Note: before each test we first prepare the cache with enough pre-loaded key-val pairs to keep consistency.
### maxmem
varying maxmem is tricky in that to maintain the hit rate of 0.8, we need to adjust either the key_pool or the value sizes as well. For me,  I adjusted the key_pool in proportion to the maxmem adjustment. Perhaps not surprisingly, the latency and throughput did not change much (11, 123.76). 

Adjusting the value sizes will perhaps change the measurements, but I did not test it that way.
### max_load_factor
Theoretically, adjusting max_load_factor will produce a few extreme outliers, when the request is waiting for the server cache to resize. Otherwise decreasing max_load_factor will theoretically decrease the collision chance and hence reduce latency. This is indeed true in our measurements taken when max_load_factor is 


### evictor
Not using an evictor( i.e. the cache becomes static when maxmem reached) is not practical but a curious way to measure performance. 

Interestingly, the latency/throughput is further reduced to (7,174.52). This is perhaps the 'bottomline' time it takes to process a request since the cache literally does not need to perform any operation. 


### System
The above measurements is taken in a windows environment, the measurements in a linux context is (9, 225.734)


