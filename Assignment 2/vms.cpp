// Memory Virtualizaiton Project - VMS Algorithm

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

void vms (char *tracefile, int memory_frames, int percentage, bool debug) {
    int reads = 0, writes = 0, traces = 0;
    FILE *FILE = fopen(tracefile, "r");
    if (!FILE) {
        printf("Error opening trace file: %s\n", tracefile);
        exit(1);
    }

    if (percentage == 0) {
        fifo(tracefile, memory_frames, debug);
        exit(1);
    } else if (percentage == 100) {
        lru(tracefile, memory_frames, debug);
        exit(1);
    }

    deque<int> fifo_table;
    vector<int> lru_table;
    deque<char> fifo_operations;
    vector<char> lru_operations;
    int secondarybufsize = (memory_frames * percentage) / 100;
    int primarybufsize = (memory_frames - secondarybufsize);

    char line[100];
    while (fgets(line, sizeof(line), FILE) != NULL) {
        unsigned long long address; char rw; // address = trace, rw = R or W from trace file
        sscanf(line, "%llx %c", &address, &rw);
        int page_number = address / PAGE_SIZE;
        traces++;

        deque<int>::iterator infifo = find(fifo_table.begin(), fifo_table.end(), page_number);
        vector<int>::iterator inlru = find(lru_table.begin(), lru_table.end(), page_number);

        if ((fifo_table.size() < primarybufsize)) { // if the fifo table is not full
            if (infifo != fifo_table.end()) { // if the page number IS in the fifo table
                if (rw == 'W') { // if the current's page operation is W, we have to find it's previous occurrence in deque and change it's operation
                    int index = std::distance(fifo_table.begin(), find(fifo_table.begin(), fifo_table.end(), page_number)); // finds last occurrence of page number and gives index
                    fifo_operations.erase(fifo_operations.begin() + index); // remove the operation from the operation deque
                    fifo_operations.insert(fifo_operations.begin() + index, rw); //add the new operation to the operation deque
                } 
            } else { // page is NOT in the fifo table
                fifo_table.push_back(page_number); // adds page number to back of fifo
                fifo_operations.push_back(rw); // adds RW operation to back of deque
                reads++;
            }
        } else { // if fifo table IS full
            if (infifo == fifo_table.end()) { //if page table is NOT in fifo table
                if ((lru_table.size() < secondarybufsize)) { // if lru table is empty or not full
                    if (inlru == lru_table.end()) { // if the page number is NOT in the lru table
                        lru_table.insert(lru_table.begin(), fifo_table.front()); // taking element from fifo and putting it in lru table
                        lru_operations.insert(lru_operations.begin(), fifo_operations.front()); // doing same for its RW operation
                        fifo_table.pop_front(); // remove fifo's top element from table. it is now in lru's table
                        fifo_operations.pop_front(); // removes fifo's top element operation from table
                        fifo_table.push_back(page_number); // putting new page number in fifo table
                        fifo_operations.push_back(rw); // putting new operation in fifo table
                        reads++;
                    } else { // if the page number is in the lru table
                        vector<int>::iterator it = find(lru_table.begin(), lru_table.end(), page_number);
                        if (lru_operations.at(std::distance(lru_table.begin(), it)) == 'W') {
                            rw = 'W';
                        }
                        lru_table.erase(it); // removes last occurrence of page
                        lru_operations.erase(lru_operations.begin() + std::distance(lru_table.begin(), it)); // removes the last occurence of that page's operation
                        lru_table.insert(lru_table.begin(), fifo_table.front()); // adds the oldest fifo page number to the lru page table
                        lru_operations.insert(lru_operations.begin(), fifo_operations.front()); // adds oldest fifo page number op to lru table
                        fifo_table.pop_front(); 
                        fifo_operations.pop_front();
                        fifo_table.push_back(page_number); // adds new page to fifo table
                        fifo_operations.push_back(rw); // adds new page's op to fifo table
                    } 
                } else { // if lru table is full
                    if (inlru == lru_table.end()) { // if the page number is NOT in the lru table 
                        if (lru_operations.back() == 'W') {
                            writes++;
                        } 
                        reads++;
                        lru_table.pop_back(); // removes oldest page in lru table
                        lru_operations.pop_back(); // removes oldest page's op in lru table
                        lru_table.insert(lru_table.begin(), fifo_table.front()); // taking element from fifo and putting it in lru table
                        lru_operations.insert(lru_operations.begin(), fifo_operations.front()); // doing same for its RW operation
                        fifo_table.pop_front(); // removes oldest page in fifo deque
                        fifo_operations.pop_front(); // removes oldest page's op in fifo deque
                        fifo_table.push_back(page_number);
                        fifo_operations.push_back(rw);
        
                    } else { // if page number is in lru table
                        vector<int>::iterator it = find(lru_table.begin(), lru_table.end(), page_number);
                        if (lru_operations.at(std::distance(lru_table.begin(), it)) == 'W') {
                            rw = 'W';
                        }
                        int index = std::distance(lru_table.begin(), find(lru_table.begin(), lru_table.end(), page_number));
                        lru_table.erase(it); // removes last occurrence of page
                        lru_operations.erase(lru_operations.begin() + std::distance(lru_table.begin(), it)); // removes the last occurence of that page's operation
                        lru_table.insert(lru_table.begin(), fifo_table.front()); // taking element from fifo and putting it in lru table
                        lru_operations.insert(lru_operations.begin(), fifo_operations.front()); // doing same for its RW operation
                        fifo_table.pop_front(); // removes oldest page in fifo deque
                        fifo_operations.pop_front(); // removes oldest page's op in fifo deque
                        fifo_table.push_back(page_number);
                        fifo_operations.push_back(rw);
                    }
                }
            } else { // if page number is neither in fifo or lru table
                if (rw == 'W') { // if the current's page operation is W, we have to find it's previous occurrence in deque and change it's operation
                    int index = std::distance(fifo_table.begin(), find(fifo_table.begin(), fifo_table.end(), page_number)); // finds last occurrence of page number and gives index
                    fifo_operations.erase(fifo_operations.begin() + index); // remove the operation from the operation deque
                    fifo_operations.insert(fifo_operations.begin() + index, rw); //add the new operation to the operation deque
                } 
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




// #include <deque>
// #include <iostream>
// #include <algorithm>
// #include "memory.hpp"
// #include <vector>
// #include <stdio.h>

// #define MAX_LINE 100
// #define PAGE_SIZE 4096

// using namespace std;

// struct Results {
//     int disk_reads;
//     int disk_writes;
//     int events;
// };

// void vms(char *tracefile, int nframes, int percent) {
//     struct Results results = {0, 0, 0};  

//     deque<int> fifo_table;
//     deque<char> fifo_op;
//     vector<int> lru_table;
//     vector<char> lru_op;
    
//     int secondary = (nframes * percent) / 100;
//     int primary = nframes - secondary;
    
//     FILE *FILE = fopen(tracefile, "r");
//     if (!FILE) {
//         printf("Error: Could not open trace file %s\n", tracefile);
//         exit(1);
//     }
//     char line[MAX_LINE];
//     while (fgets(line, sizeof(line), FILE) != NULL) {
//         char op;
//         unsigned long long address;
//         sscanf(line, "%llx %c", &address, &op);
//         int page_number = address / PAGE_SIZE;
        
//         results.events++;
//         if (fifo_table.size() <= primary) { //if the page table is not full
//             deque<int>::iterator it = find(fifo_table.begin(), fifo_table.end(), page_number); //find the page number in the page table
//             if ( it == fifo_table.end()) { //if the page number is not in the page table
//                 fifo_table.push_back(page_number); //add the page number to the page table
//                 fifo_op.push_back(op); //add the fifo_op to the fifo_op deque
//                 results.disk_reads++; //add 1 to the number of reads
//             }
//             else { //if its in the page table
//                 if (op == 'W') { //if the fifo_op is write
//                     int index = std::distance(fifo_table.begin(), it); //get the index of the page number in the page table
//                     fifo_op.erase(fifo_op.begin() + index); //remove the fifo_op from the fifo_op deque
//                     fifo_op.insert(fifo_op.begin() + index, op); //add the new fifo_op to the fifo_op deque
//                 }
//             }
//         }
//         else { //if the fifo page table is full
//             if (find(fifo_table.begin(), fifo_table.end(), page_number) == fifo_table.end()) { //if the page number is not in the fifo page table
//                 if (lru_table.size() <= secondary) { //if the lru page table is not full
//                     if(find(lru_table.begin(), lru_table.end(), page_number) == lru_table.end()) { //if the page number is not in the lru page table
//                         lru_table.push_back(fifo_table.front()); //add the oldest fifo page number to the lru page table
//                         lru_op.push_back(fifo_op.front()); //add the oldest fifo_op to the lru_op deque
//                         fifo_table.pop_front(); //remove the oldest fifo page number from the fifo page table
//                         fifo_op.pop_front(); //remove the oldest fifo_op from the fifo_op deque
//                         fifo_table.push_back(page_number); //add the new page number to the fifo page table
//                         fifo_op.push_back(op); //add the new fifo_op to the fifo_op deque
//                         results.disk_reads++; //add 1 to the number of reads
//                     }
//                     else { //if the page number is in the lru page table
//                         vector<int>::iterator it = find(lru_table.begin(), lru_table.end(), page_number); //find the page number in the lru page table
//                         if (lru_op.at(std::distance(lru_table.begin(), it)) == 'W') { //if the operation is write in the lru operation deque
//                              op = 'W'; //change the new operation to write
//                         }
//                         lru_table.erase(it); //remove the page number from the lru page table
//                         lru_op.erase(lru_op.begin() + std::distance(lru_table.begin(), it)); //remove the lru_op from the lru_op deque
//                         lru_table.push_back(fifo_table.front()); //add the oldest fifo page number to the lru page table
//                         lru_op.push_back(fifo_op.front()); //add the oldest fifo_op to the lru_op deque
//                         fifo_table.pop_front(); //remove the oldest fifo page number from the fifo page table
//                         fifo_op.pop_front(); //remove the oldest fifo_op from the fifo_op deque
//                         fifo_table.push_back(page_number); //add the new page number to the fifo page table
//                         fifo_op.push_back(op); //add the new fifo_op to the fifo_op deque
//                     }
//                 }
//                 else { //if the lru page table is full
//                     if(find(lru_table.begin(), lru_table.end(), page_number) == lru_table.end()) { //if the page number is not in the lru page table
//                         //we need to evict the oldest page in the lru page table and add the oldest page in the fifo page table to the lru page table
//                         //and if the lru_op of the evicted page is write, we need to write to disk
//                         if (lru_op.front() == 'W') { //if the lru_op is write
//                             results.disk_writes++; //add 1 to the number of writes
//                         }
//                         lru_table.erase(lru_table.begin()); //remove the oldest page number from the lru page table
//                         lru_op.erase(lru_op.begin()); //remove the oldest lru_op from the lru_op deque
//                         lru_table.push_back(fifo_table.front()); //add the oldest fifo page number to the lru page table
//                         lru_op.push_back(fifo_op.front()); //add the oldest fifo_op to the lru_op deque
//                         fifo_table.pop_front(); //remove the oldest fifo page number from the fifo page table
//                         fifo_op.pop_front(); //remove the oldest fifo_op from the fifo_op deque
//                         fifo_table.push_back(page_number); //add the new page number to the fifo page table
//                         fifo_op.push_back(op); //add the new fifo_op to the fifo_op deque
//                         results.disk_reads++; //add 1 to the number of reads
//                     }
//                     else { //if the page number is in the lru page table
//                         vector<int>::iterator it = find(lru_table.begin(), lru_table.end(), page_number); //find the page number in the lru page table
//                         if (lru_op.at(std::distance(lru_table.begin(), it)) == 'W') { //if the operation is write in the lru operation deque
//                              op = 'W'; //change the new operation to write
//                         }
//                         lru_table.erase(it); //remove the page number from the lru page table
//                         lru_op.erase(lru_op.begin() + std::distance(lru_table.begin(), it)); //remove the lru_op from the lru_op deque
//                         lru_table.push_back(fifo_table.front()); //add the oldest fifo page number to the lru page table
//                         lru_op.push_back(fifo_op.front()); //add the oldest fifo_op to the lru_op deque
//                         fifo_table.pop_front(); //remove the oldest fifo page number from the fifo page table
//                         fifo_op.pop_front(); //remove the oldest fifo_op from the fifo_op deque
//                         fifo_table.push_back(page_number); //add the new page number to the fifo page table
//                         fifo_op.push_back(op); //add the new fifo_op to the fifo_op deque
//                     }
//                 }
//             }
//             else { //if it is in the fifo page table
//                 if (op == 'W') { //if the fifo_op is write
//                     int index = std::distance(fifo_table.begin(), find(fifo_table.begin(), fifo_table.end(), page_number)); //get the index of the page number in the page table
//                     fifo_op.erase(fifo_op.begin() + index); //remove the fifo_op from the fifo_op deque
//                     fifo_op.insert(fifo_op.begin() + index, op); //add the new fifo_op to the fifo_op deque
//                 }
//             }
//         }
//         // if (debugMode)
//         // {
//         //     std::cout << "Address: " << address << " Operation: " << op << std::endl;
//         // }
//     }
//     std::cout << "Total event traces: " << results.events << std::endl;
//     std::cout << "Total Disk reads: " << results.disk_reads << std::endl;
//     std::cout << "Total Disk writes: " << results.disk_writes << std::endl;
//     fclose(FILE);
// }