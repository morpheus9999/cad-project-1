/* 
 * File:   FileReader.cpp
 * Author: guilhermerodrigues
 * 
 * Created on March 25, 2011, 2:02 PM
 */

#include "FileReader.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

#define FILEPATH "dataset/sm_rules.csv"

using namespace std;

FileReader::FileReader() {
}

FileReader::FileReader(const FileReader& orig) {
}

FileReader::~FileReader() {
}

void FileReader::read() {
    int fd;
    int i;
    int *map;
    struct stat buffer;
    int status;

    fd = open(FILEPATH, O_RDONLY);
    status = fstat(fd, &buffer);

    if (fd == -1) {
	perror("Error opening file for reading");
	exit(EXIT_FAILURE);
    }

    //map = mmap(NULL, buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);
    
    if (map == MAP_FAILED) {
	close(fd);
	perror("Error mmapping the file");
	exit(EXIT_FAILURE);
    }
    
    cout << map[0] << endl;

    close(fd);
}