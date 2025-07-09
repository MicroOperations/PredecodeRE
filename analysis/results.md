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

- Tlb evictions usually dont cause predecode cache evictions, 
  however on very few occasions they can

- Setting memory to uncacheable in pte prevents it from being 
  put into the predecode cache

- Disabling caching in cr0 control reg disables predecode cache

- The predecode cache isn't coherent with the l1i cache nor the itlb

- The predecode cache was found to be shared between both physical cores on 
  the Intel celeron n4020

**Covert channel**

- Since we dont exactly know the length of what is being cached, its kinda hard to know how to 
  cause aliasing within specific parts of the predecode cache to then create a covert channel POC. 
  However, it should hypothetically be possible to create a covert channel which survives tlb and cache 
  flushes by leveraging the predecode cache.