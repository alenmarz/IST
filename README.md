# Parallel Batched Interpolation Search Tree

1. Build OpenCilk from source files using the guide: https://github.com/OpenCilk/infrastructure/blob/release/INSTALLING.md

2. Pull the required git submodules: 

`git submodule update --init --recursive`

3. Remove the following from *pbbs-pctl/bench/include/bench.hpp:110:3*:

`__cilkrts_set_param("nworkers", std::to_string(proc).c_str());`

4. Make sure you have installed grepftools: https://github.com/gperftools/gperftools/blob/master/INSTALL

5. Compile the project:

```~/study/build/bin/clang++ -g -std=c++11 -O2 -I ~/study/IST/lib/pctl/include -I ~/study/IST/lib/chunkedseq/include -I ~/study/IST/lib/pbbs-pctl/include -I ~/study/IST/lib/quickcheck/quickcheck -I ~/study/IST/lib/cmdline/include -I ~/study/IST/lib/pbbs-pctl/bench/include -I ~/study/IST/lib/pbbs-pctl/bench/include/generators -I ~/study/IST/lib/pbbs-include/ -I ~/study/IST/ -DUSE_CILK_PLUS_RUNTIME -fopencilk -lopencilk -ltcmalloc ~/study/IST/main.cpp -o IST```

6. Run it:
`CILK_NWORKERS=10 ./IST`
