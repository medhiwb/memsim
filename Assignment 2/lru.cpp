// Memory Virtualizaiton Project - LRU Algorithm

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

void lru (char *tracefile, int memory_frames, bool debug) {
    int reads = 0, writes = 0, traces = 0;

    vector<int> page_table;
    vector<char> operation;

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
                vector<int>::iterator it = find(page_table.begin(), page_table.end(), page_number);
                if (operation.at(std::distance(page_table.begin(), it)) == 'W') {
                    rw = 'W';
                }
                int index = std::distance(page_table.begin(), find(page_table.begin(), page_table.end(), page_number)); // finds last occurrence of page number and gives index
                page_table.erase(it);
                operation.erase(operation.begin() + index); // remove the operation from the operation deque
                page_table.insert(page_table.begin(), page_number);
                operation.insert(operation.begin(), rw);
            } else { // if the page number is NOT in the deque
                page_table.insert(page_table.begin(), page_number); // adds page number to front of deque
                operation.insert(operation.begin(), rw); // adds RW operation to front of deque
                reads++;
            }

        } else if (!page_table.empty() && page_table.size() == memory_frames) { // if the page table is full 
            if (find(page_table.begin(), page_table.end(), page_number) != page_table.end()) { // if the page number IS in the deque
                vector<int>::iterator it = find(page_table.begin(), page_table.end(), page_number);
                if (operation.at(std::distance(page_table.begin(), it)) == 'W') {
                    rw = 'W';
                    //writes++;
                }
                int index = std::distance(page_table.begin(), find(page_table.begin(), page_table.end(), page_number)); // finds last occurrence of page number and gives index
                page_table.erase(it);
                operation.erase(operation.begin() + std::distance(page_table.begin(), it));
                //operation.insert(operation.begin() + index, rw);
                page_table.insert(page_table.begin(), page_number); // adds page number to front of deque
                operation.insert(operation.begin(), rw); // adds RW operation to front of deque
            } else if (find(page_table.begin(), page_table.end(), page_number) == page_table.end()) {  // if the page number is NOT in the deque
                if (operation.back() == 'W') {
                    writes++;
                } 
                reads++;
                page_table.pop_back();
                operation.pop_back();
                page_table.insert(page_table.begin(), page_number); // adds page number to front of deque
                operation.insert(operation.begin(), rw); // adds RW operation to front of deque
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
