# HSA microbenchmarks

This repository contains a set of microbenchmarks for determining the hardware characteristics of HSA devices.

Details for Kaveri (AMD A10-7850K) can be found via a blog post at ...

## Caches
This folder contains code and data (for AMD A10-7850K) for determining the cache capacity, cache associativity, cache miss latencies, etc. Additionally, this same code can be used to determine the TLB size/associativity/miss latency/etc.

## Requirements to run
  * HSA Runtime and Drivers from AMD
    * See http://github.com/HSAFoundation for installation details
  * These projects use the SNACK framework to execute on the HSA device
    * SNACK is part of the CLOC project from the HSA Foundation (found in the repo above)
  * I've only tested this code on Linux. The code should be mostly OS agnostic, however.

# License
This code is free for anyone to use / modify. This work has been completed as part of my PhD studies at UW-Madison under the guidance of Mark Hill and David Wood.
More about us can be found at  
http://cs.wisc.edu/multifacet  
http://cs.wisc.edu/~powerjg  
http://cs.wisc.edu/~markhill  
http://cs.wisc.edu/~david
