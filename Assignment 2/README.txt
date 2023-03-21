Type "make" into command line. It should compile all files but in the case it doesn't enter 
"g++ -std=c++11 -o memsim memsim.cpp fifo.cpp lru.cpp vms.cpp"

Then to run program, enter command starting with ".\memsim" like in the test cases.
Example commands: "./memsim bzip.trace 64 vms 0 quiet"
                 or "./memsim bzip.trace 64 lru quiet"

If you want to recompile for whatever reason, enter "make clean" and then enter "make" again.