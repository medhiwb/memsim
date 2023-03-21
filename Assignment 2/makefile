memsim: memsim.o fifo.o lru.o vms.o
	g++ -std=c++11 memsim.o fifo.o lru.cpp vms.o -o memsim

memsim.o: memsim.cpp
	g++ -c -std=c++11 memsim.cpp

fifo.o: fifo.cpp memory.hpp
	g++ -c -std=c++11 fifo.cpp

lru.o: lru.cpp memory.hpp
	g++ -c -std=c++11 lru.cpp

vms.o: vms.cpp memory.hpp
	g++ -c -std=c++11 vms.cpp

clean:
	rm *.o memsim