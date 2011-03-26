/* 
 * File:   FileHandler.h
 * Author: guilhermerodrigues
 *
 * Created on March 26, 2011, 9:24 PM
 */

#ifndef FILEHANDLER_H
#define	FILEHANDLER_H

#include <vector>

#define RULE_SIZE   11
#define INPUT_SIZE  10

#define RULE_NUM    2000000
#define INPUT_NUM   4

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

struct LoadedFile {
    int* memoryBlock;
    vector<int*>* workVector;
    unsigned int size;
};

class FileHandler {
public:
    FileHandler();
    FileHandler(const FileHandler& orig);
    virtual ~FileHandler();

    vector<int*>* readRuleFile(const char* FileName);
    vector<int*>* readInputFile(const char* FileName);
    
    unsigned long int getMemoryUsed();

private:
    vector<int*>* readFile(const char* FileName, int row_size, int vector_size, int number_size);

    /* Atributes */
    vector<LoadedFile*> memoryHandler;
};

#endif	/* FILEHANDLER_H */

