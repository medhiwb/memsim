// Memory Virtualizaiton Project - FIFO Algorithm

#include <deque>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <string.h>
#include <algorithm>

#include "memory.hpp"

using namespace std;
const int PAGE_SIZE = 4096;

void fifo (char *tracefile, int memory_frames, bool debug) {
    int reads = 0, writes = 0, traces = 0;

    deque<int> page_table;
    deque<char> operation;

    FILE *FILE = fopen(tracefile, "r");
    if (!FILE) {
        printf("Error opening trace file: %s\n", tracefile);
        exit(1);
    }

    char line[100];
    while (fgets(line, sizeof(line), FILE) != NULL) {
        unsigned long long address; char rw; // address = trace, rw = R or W from trace file
        sscanf(line, "%llx %c", &address, &rw);
        int page_number = address / PAGE_SIZE;
        traces++;

        if (page_table.empty() || (!page_table.empty() && page_table.size() < memory_frames)) { // if the page table is empty or not full
            if (find(page_table.begin(), page_table.end(), page_number) != page_table.end()) { // if the page number IS in the deque
                if (rw == 'W') {
                    int index = std::distance(page_table.begin(), find(page_table.begin(), page_table.end(), page_number)); // finds last occurrence of page number and gives index
                    operation.erase(operation.begin() + index); // remove the operation from the operation deque
                    operation.insert(operation.begin() + index, rw); //add the new operation to the operation deque
                } 
            } else { // if the page number is NOT in the deque
                page_table.push_back(page_number); // adds page number to back of deque
                operation.push_back(rw); // adds RW operation to back of deque
                reads++;
            }
        } else if (!page_table.empty() && page_table.size() == memory_frames) { // if the page table is full 
            if (find(page_table.begin(), page_table.end(), page_number) != page_table.end()) { // if the page number IS in the deque
                if (rw == 'W') {
                    int index = std::distance(page_table.begin(), find(page_table.begin(), page_table.end(), page_number)); // finds last occurrence of page number and gives index
                    operation.erase(operation.begin() + index); // remove the operation from the operation deque
                    operation.insert(operation.begin() + index, rw); //add the new operation to the operation deque
                } else {
                    continue;
                }
            } else if (find(page_table.begin(), page_table.end(), page_number) == page_table.end()) {  // if the page number is NOT in the deque
                if (operation.front() == 'W') {
                    writes++;
                } 
                reads++;
                page_table.pop_front();
                operation.pop_front();
                page_table.push_back(page_number);
                operation.push_back(rw);
            }
        }
        if (debug)
        {
            cout << "Address: " << address << " Operation: " << rw << endl;
        }
    }

    cout << "Number of traces: " << traces << endl;
    cout << "Number of memory frames: " << memory_frames << endl;
    cout << "Number of reads: " << reads << endl;
    cout << "Number of writes: " << writes << endl;
}
