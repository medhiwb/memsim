// Memory Virtualizaiton Project - Header file for the algorithms

#ifndef MEMORY_HPP 
#define MEMORY_HPP

#include <unordered_map>
#include <deque>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;

void fifo (char *tracefile, int memory_frames, bool debug);
void lru (char *tracefile, int memory_frames, bool debug);
void vms (char *tracefile, int memory_frames, int percentage, bool debug);

void testvms (char *tracefile, int memory_frames, int percentage);

#endif