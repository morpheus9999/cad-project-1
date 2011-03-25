/* 
 * File:   FileReader.cpp
 * Author: guilhermerodrigues
 * 
 * Created on March 25, 2011, 2:02 PM
 */

#include "FileReader.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>

#define INPUT_SIZE 10

using namespace std;

FileReader::FileReader() {
}

FileReader::FileReader(const FileReader& orig) {
}

FileReader::~FileReader() {
}

vector<int*>* FileReader::read(const char* FileName) {
    vector<int*>* rules = new vector<int*>();

    clock_t s = clock();
    int fd;
    int i, l;
    char *map;
    struct stat buffer;
    int status;

    fd = open(FileName, O_RDONLY);
    status = fstat(fd, &buffer);

    if (fd == -1) {
	perror("Error opening file for reading");
	exit(EXIT_FAILURE);
    }

    map = (char*) mmap(NULL, buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);
    
    stringstream is;
    
    if (map == MAP_FAILED) {
	close(fd);
	perror("Error mmapping the file");
	exit(EXIT_FAILURE);
    }

    is.str(map);
    
    cout << is.good() << endl;

    int* newRule;
    char k;

    for (int j = 0; j < 4; j++) {
        newRule = new int[INPUT_SIZE+1];
        for (int i = 0; i < 10; i++) {
            is >> newRule[i] >> k;
        }
        is >> newRule[10];
        rules->push_back(newRule);
    }

    close(fd);
    munmap(map, sizeof(map));
    cout << "\nTime took: " << (((double)clock() - s) / CLOCKS_PER_SEC) << endl; 
}