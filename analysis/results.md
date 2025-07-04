# Findings

**Predecode cache utilisation in 6 steps:**

  1. Instruction fetch

  2. Check if instruction contains an LCP

  3. If it does then continue with step 4, otherwise skip to step 6

  4. Conduct predecode cache lookup to gather predecode related metadata, processor
     predicts this as the predecode related data, I don't know how it finds out 
     when a misprediction occurs, but it does

  5. If misprediction occurs update the predecode cache with the correct instruction 
     predecode related metadata

  6. Issue relevant data onto the instruction queue

**Coherency:**

- Cache evictions usually dont cause predecode cache evictions

- Tlb evictions usually dont cause predecode cache evictions

- Disabling caching in cr0 control reg disables predecode cache

- The predecode cache isn't coherent with the l1i cache nor the itlb

**Predecode cache organisation**
