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
    ruleHandler = NULL;
}

FileHandler::FileHandler(const FileHandler& orig) {
}

void FileHandler::addOutput(int* input, short classf) {
    inputHandler[0]->output.push_back(pair<int*, short>(input, classf));
}

vector<int*>* FileHandler::readRuleFile(const char* FileName) {
    LoadedFile* newRule = readFile(FileName, RULE_SIZE, RULE_NUM, RULE_NUM*RULE_SIZE);
    ruleHandler = newRule;

    return newRule->workVector;
}

vector<int*>* FileHandler::readInputFile(const char* FileName) {
    LoadedFile* newInput = readFile(FileName, INPUT_SIZE, INPUT_NUM, INPUT_NUM*INPUT_SIZE);
    inputHandler.push_back(newInput);

    return newInput->workVector;
}

unsigned long int FileHandler::getMemoryUsed() {
    vector<LoadedFile*>::iterator it = inputHandler.begin();
    
    unsigned long int totalSize = 0;

    while(it < inputHandler.end()) {
        totalSize += (*it)->size * sizeof(int);
        totalSize += (*it)->workVector->size() * sizeof(int*);
        totalSize += (*it)->output.size() * sizeof( pair<int*, short> );
        totalSize += sizeof(unsigned int);

        it++;
    }

    if (ruleHandler != NULL) {
        totalSize += ruleHandler->size * sizeof (int);
        totalSize += ruleHandler->workVector->size() * sizeof (int*);
        totalSize += ruleHandler->output.size() * sizeof ( pair<int*, short>);
        totalSize += sizeof (unsigned int);
    }

    return totalSize;
}

void FileHandler::manageOutputOf(vector<int*>* workPointer) {
    if (inputHandler[0]->workVector == workPointer) {

        char  lookupTable[MAX_NUMBER+1][7];
        short lookupSizes[MAX_NUMBER+1];

        clock_t s = clock();

        for(int i=0; i<MAX_NUMBER; i+=5) {
            lookupSizes[i] = sprintf(lookupTable[i], "%d", i);
            lookupSizes[i+1] = sprintf(lookupTable[i+1], "%d", i+1);
            lookupSizes[i+2] = sprintf(lookupTable[i+2], "%d", i+2);
            lookupSizes[i+3] = sprintf(lookupTable[i+3], "%d", i+3);
            lookupSizes[i+4] = sprintf(lookupTable[i+4], "%d", i+4);
        }

        lookupSizes[MAX_NUMBER] = sprintf(lookupTable[MAX_NUMBER], "%d", MAX_NUMBER);

        LoadedFile* file = inputHandler[0];
        
        int fd, result;
        char *map;

        int fileSize = file->output.size() * RULE_SIZE * 6 * sizeof(char);

        fd = open("dataSet/testData.csv", O_RDWR | O_CREAT, (mode_t)0600);

        if (fd == -1) {
            perror("Error opening file for writing");
            exit(EXIT_FAILURE);
        }
        
        result = lseek(fd, fileSize-1, SEEK_SET);
        if (result == -1) {
            close(fd);
            perror("Error calling lseek() to 'stretch' the file");
            exit(EXIT_FAILURE);
        }

        result = write(fd, "", 1);
        if (result != 1) {
            close(fd);
            perror("Error writing last byte of the file");
            exit(EXIT_FAILURE);
        }

        map = static_cast<char*> (mmap(NULL, fileSize, PROT_WRITE, MAP_SHARED, fd, 0));

        if (map == MAP_FAILED) {
            close(fd);
            perror("Error mmapping the file");
            exit(EXIT_FAILURE);
        }

        int idx=0;
        int* t_id;
        list< pair<int*, short> >::iterator it = file->output.begin();
        
        cout << "Extra memory reserved: " << fileSize/(float) (1048576) << " MB\n";

        while (it != file->output.end()) {

            t_id = &((*it).first[0]);
            memcpy ( &map[idx], lookupTable[*t_id], lookupSizes[*t_id] );
            idx += lookupSizes[*t_id];
            map[idx++] = ',';
            t_id = &((*it).first[1]);
            memcpy ( &map[idx], lookupTable[*t_id], lookupSizes[*t_id] );
            idx += lookupSizes[*t_id];
            map[idx++] = ',';
            t_id = &((*it).first[2]);
            memcpy ( &map[idx], lookupTable[*t_id], lookupSizes[*t_id] );
            idx += lookupSizes[*t_id];
            map[idx++] = ',';
            t_id = &((*it).first[3]);
            memcpy ( &map[idx], lookupTable[*t_id], lookupSizes[*t_id] );
            idx += lookupSizes[*t_id];
            map[idx++] = ',';
            t_id = &((*it).first[4]);
            memcpy ( &map[idx], lookupTable[*t_id], lookupSizes[*t_id] );
            idx += lookupSizes[*t_id];
            map[idx++] = ',';
            t_id = &((*it).first[5]);
            memcpy ( &map[idx], lookupTable[*t_id], lookupSizes[*t_id] );
            idx += lookupSizes[*t_id];
            map[idx++] = ',';
            t_id = &((*it).first[6]);
            memcpy ( &map[idx], lookupTable[*t_id], lookupSizes[*t_id] );
            idx += lookupSizes[*t_id];
            map[idx++] = ',';
            t_id = &((*it).first[7]);
            memcpy ( &map[idx], lookupTable[*t_id], lookupSizes[*t_id] );
            idx += lookupSizes[*t_id];
            map[idx++] = ',';
            t_id = &((*it).first[8]);
            memcpy ( &map[idx], lookupTable[*t_id], lookupSizes[*t_id] );
            idx += lookupSizes[*t_id];
            map[idx++] = ',';
            t_id = &((*it).first[9]);
            memcpy ( &map[idx], lookupTable[*t_id], lookupSizes[*t_id] );
            idx += lookupSizes[*t_id];
            map[idx++] = ',';
            memcpy ( &map[idx], lookupTable[(*it).second], lookupSizes[(*it).second] );
            idx += lookupSizes[(*it).second];
            map[idx++] = '\n';
            
            it++;
        }
        map[idx] = '\0';
        close(fd);
        munmap(map, sizeof (map));

        cout << "\nWrite took: " << (((double) clock() - s) / CLOCKS_PER_SEC) << endl;
    }
}

LoadedFile* FileHandler::readFile(const char* FileName, int row_size, int vector_size, int number_size) {

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

    cout << "\nRead took: " << (((double)clock() - s) / CLOCKS_PER_SEC) << endl;

    return newFile;
}

FileHandler::~FileHandler() {
    vector<LoadedFile*>::iterator it = inputHandler.begin();

    while(it < inputHandler.end()) {
        delete (*it)->memoryBlock;
        delete (*it)->workVector;
        delete (*it);
        
        it++;
    }

    inputHandler.clear();

//    if ( ruleHandler != NULL ) {
//        delete (*it)->memoryBlock;
//        delete (*it)->workVector;
//        delete (*it);
//
//        (*it)->output.clear();
//    }
}

