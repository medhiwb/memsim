// Memory Virtualizaiton Project

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

int main (int argc, char *argv []) {

    char *file;
    int numframes, perc;

    if (argc < 5 || argc > 7) {
        cout << "Enter line in format of: memsim <tracefile> <nframes> <lru|fifo|vms> <p> <debug|quiet>";
        exit(1);
    } 
    
    if (argc == 5) {
        file = argv[1];
        numframes = stoi(argv[2]);
        if ((strcmp(argv[3], "fifo") == 0) && (strcmp(argv[4], "quiet")) == 0) {
            fifo(file, numframes, false);
        } else if (strcmp(argv[3], "fifo") && (strcmp(argv[4], "debug")) == 0) {
            fifo(file, numframes, true);
        } 
        
        if ((strcmp(argv[3], "lru") == 0) && (strcmp(argv[4], "quiet")) == 0) {
            lru(file, numframes, false);
        } else if ((strcmp(argv[3], "lru") == 0) && (strcmp(argv[4], "debug")) == 0) {
            lru(file, numframes, true);
        }
    }  
    
    if (argc == 6) {
        file = argv[1];
        numframes = stoi(argv[2]);
        perc = stoi(argv[4]);
        if ((strcmp(argv[3], "vms") == 0) && (strcmp(argv[5], "quiet")) == 0) {
            vms(file, numframes, perc, false);
        } else if ((strcmp(argv[3], "vms") == 0) && (strcmp(argv[5], "debug")) == 0) {
            vms(file, numframes, perc, true);
        } 
    }

    return 0;
}