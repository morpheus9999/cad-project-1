/* 
 * File:   main1.cpp
 * Author: jojo
 *
 * Created on 25 de Março de 2011, 1:43
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <algorithm>

#define INPUT_SIZE 10

#define ATOI(str, result) \
do{ \
char *lptr = str; \
result = 0; \
while (1) \
{ \
if ((*lptr >= '0') && (*lptr <= '9')) \
{ \
result *= 10; \
result += *lptr - '0'; \
lptr++; \
} \
else \
{ \
break; \
} \
} \
}while(0)

using namespace std;

vector<int*> rules;

const char* FileName = "dataset/sm_rules.csv";

bool compArray (int *a,int *b);
vector<int*>* readFile(const char*, int);

int main (int argc, const char * argv[])
{
    
    readFile(FileName, 2000000);
    return 0;
}

inline bool compArray (int *a, int *b) {
    for (int i = 0; i < 10; i++) {
        if (!(a == 0 || b==0) && a[i] != b[i])
            return a[i] < b[i];
    }
    
    return false;
}

vector<int*>* readFile(const char* FileName, int size) {

    clock_t s = clock();
    
    vector<int*>* rules = new vector<int*>();
    rules->reserve(size);

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

    map = static_cast<char*>(mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0));

    if (map == MAP_FAILED) {
	close(fd);
	perror("Error mmapping the file");
	exit(EXIT_FAILURE);
    }

    char *last, *cell;

    cell = strtok_r(map, ",", &last);

    int* ruleSet = new int[size*11];
    int ruleIndex = 0, ruleStart;

    while (cell) {
        ruleStart = ruleIndex;

        ATOI(cell, ruleSet[ruleIndex]);
        //ruleSet[ruleIndex] = atoi(cell);
        ruleIndex++;

        for (i = 1; i < 10; i++, ruleIndex++) {
            //ruleSet[ruleIndex] = atoi(strtok_r(NULL, ",", &last));
            cell = strtok_r(NULL, ",", &last);
            ATOI(cell, ruleSet[ruleIndex]);
        }
        
        //ruleSet[ruleIndex] = atoi(strtok_r(NULL, "\n", &last));
        cell = strtok_r(NULL, "\n", &last);
        ATOI(cell, ruleSet[ruleIndex]);
        rules->push_back(&ruleSet[ruleStart]);
        ruleIndex++;
        
        cell = strtok_r(NULL, ",", &last);
    }

    close(fd);
    munmap(map, sizeof(map));

    sort(rules->begin(), rules->end(), compArray);

    cout << "\nTime took: " << (((double)clock() - s) / CLOCKS_PER_SEC) << endl;
}
