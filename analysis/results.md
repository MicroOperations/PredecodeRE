# Environment:

- All tests were conducted on an intel celeron n4020 processor

- Driver was ran on linux kernel v6.1.0

- Source code can be found in the src directory, and you can follow
  each stage of the analysis by going through the commit history

# Findings

**Predecode cache utilisation in 6 steps:**

  1. Instruction fetch

  2. Check if instruction contains an LCP

  3. If it does then continue with step 4, otherwise skip to step 6

  4. Conduct predecode cache lookup to gather predecode related metadata such as 
     instruction boundaries and stuff like that, processor predicts this as the predecode 
     related data, I don't know how it finds out when a misprediction occurs, but it does

  5. If misprediction occurs update the predecode cache with the correct instruction 
     predecode related metadata

  6. Issue relevant data onto the instruction queue

**Coherency:**

- Cache evictions usually dont cause predecode cache evictions

- Tlb evictions usually dont cause predecode cache evictions, however on very few occasions they can

- Setting memory to uncacheable in pte prevents it from being put into the predecode cache

- Disabling caching in cr0 control reg disables predecode cache

- The predecode cache isn't coherent with the l1i cache nor the itlb

- The predecode cache was found to be shared between both physical cores on 
  the Intel celeron n4020

**Security**

- Hypothetically it should be possible to leverage the predecode cache to create an eviction based covert 
  channel that survives tlb and cache flushes, it would be interesting to see someone build off my 
  work here and implement something of the sort.

- The predecode cache could be used as a side channel to leak data however it wouldn't be a very good one 
  whatsoever, it would probably allow you to infer where in memory instructions with lcp's are being executed, 
  however if you're aiming to infer instruction execution the icache or measuring port contention 
  (on processors with SMT) would be better options.

# Conclusion

The absence of a micro op cache is due to goldmont and goldmont plus being minimal power optimised 
microarchitectures. This leaves room for optimisations in relation to the fetch + decode stages, with goldmont 
and goldmont plus, they implemented a predecode cache which allows for caching predecode related data
(instruction boundaries etc) for instructions with lcp's, which are known to be problematic for predecoders. When 
an instruction with an lcp is being predecoded, the processor looks up its predecode data within the predecode cache.
If there is a miss it'll predecode the instruction and place its predecode data within the predecode cache. 
If there is a hit, since the predecode cache isn't coherent with the l1i cache nor the itlb 
(which I would imagine is to limit complexity and power consumption), the processor has to figure out the real 
predecode data due to it potentially being stale, so you could say that the data from the predecode cache is used 
to 'predict' predecode data relating to the instruction for an lcp. If the data is no longer valid, likely due to self 
modifying code, it'll update the predecode cache with the correct data. after the predecoding stage the instruction is 
put onto the instruction queue.
