/* 
 * File:   FileHandler.cpp
 * Author: guilhermerodrigues
 * 
 * Created on March 26, 2011, 9:24 PM
 */

#include "FileHandler.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fstream>
#include <fcntl.h>

// TODO : remove useless libraries <iostream>
#include <iostream>

FileHandler::FileHandler() {
}

FileHandler::FileHandler(const FileHandler& orig) {
}

vector<int*>* FileHandler::readRuleFile(const char* FileName) {
    return readFile(FileName, RULE_SIZE, RULE_NUM, RULE_NUM*RULE_SIZE);
}

vector<int*>* FileHandler::readInputFile(const char* FileName) {
    return readFile(FileName, INPUT_SIZE, INPUT_NUM, INPUT_NUM*INPUT_SIZE);
}

unsigned long int FileHandler::getMemoryUsed() {
    vector<LoadedFile*>::iterator it = memoryHandler.begin();
    
    unsigned long int totalSize = 0;

    while(it < memoryHandler.end()) {
        totalSize += (*it)->size * sizeof(int);
        totalSize += (*it)->workVector->size() * sizeof(int*);
        totalSize += sizeof(unsigned int);

        it++;
    }
}

vector<int*>* FileHandler::readFile(const char* FileName, int row_size, int vector_size, int number_size) {

    clock_t s = clock();

    vector<int*>* rules = new vector<int*>();
    rules->reserve(vector_size);

    int fd;
    int i;
    char *map;
    struct stat buffer;
    int status;

    fd = open(FileName, O_RDONLY);
    status = fstat(fd, &buffer);

    if (fd == -1) {
	perror("Error opening file for reading");
	exit(EXIT_FAILURE);
    }

    map = static_cast<char*>(mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0));

    if (map == MAP_FAILED) {
	close(fd);
	perror("Error mmapping the file");
	exit(EXIT_FAILURE);
    }

    char *last, *cell;

    cell = strtok_r(map, ",", &last);

    int *array = new int[number_size];
    int ruleIndex = 0, ruleStart;

    while (cell) {
        ruleStart = ruleIndex;

        ATOI(cell, array[ruleIndex]);
        //ruleSet[ruleIndex] = atoi(cell);
        //cout << (*array)[ruleIndex] << " ";
        ruleIndex++;

        for (i = 1; i < (row_size-1); i++, ruleIndex++) {
            //ruleSet[ruleIndex] = atoi(strtok_r(NULL, ",", &last));
            cell = strtok_r(NULL, ",", &last);
            ATOI(cell, array[ruleIndex]);
            //cout << (*array)[ruleIndex] << " ";
        }

        //ruleSet[ruleIndex] = atoi(strtok_r(NULL, "\n", &last));
        cell = strtok_r(NULL, "\n", &last);
        ATOI(cell, array[ruleIndex]);
        //cout << (*array)[ruleIndex] << endl;

        rules->push_back(&array[ruleStart]);
        ruleIndex++;

        cell = strtok_r(NULL, ",", &last);
    }

    close(fd);
    munmap(map, sizeof(map));

    LoadedFile *newFile  = new LoadedFile;

    newFile->size        = number_size;
    newFile->memoryBlock = array;
    newFile->workVector  = rules;
    
    memoryHandler.push_back(newFile);

    cout << "\nRead took: " << (((double)clock() - s) / CLOCKS_PER_SEC) << endl;

    return rules;
}

FileHandler::~FileHandler() {
    vector<LoadedFile*>::iterator it = memoryHandler.begin();

    while(it < memoryHandler.end()) {
        delete (*it)->memoryBlock;
        delete (*it)->workVector;
        delete (*it);
        
        it++;
    }

    memoryHandler.clear();
}

