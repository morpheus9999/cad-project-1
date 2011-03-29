/* 
 * File:   FileHandler.h
 * Author: guilhermerodrigues
 *
 * Created on March 26, 2011, 9:24 PM
 */

#ifndef FILEHANDLER_H
#define	FILEHANDLER_H

#include <vector>
#include <list>
#include <algorithm>

#define RULE_SIZE   11
#define INPUT_SIZE  10

#define RULE_NUM    2000000
#define INPUT_NUM   1000000

#define MAX_NUMBER  10000

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

typedef short*  cell_array;
typedef short   cell_value;
typedef vector<short*> cell_vector;

struct LoadedFile {
    cell_array memoryBlock;
    cell_vector* workVector;
    unsigned int size;

    list< pair<cell_array, cell_value> > output;
};

class FileHandler {
public:
    FileHandler();
    FileHandler(const FileHandler& orig);
    virtual ~FileHandler();

    cell_vector* readRuleFile(const char* FileName);
    cell_vector* readInputFile(const char* FileName);
    
    void manageOutputOf(cell_vector* workPointer);

    void addOutput(cell_array input, cell_value classf);

    unsigned long int getMemoryUsed();

private:
    LoadedFile* readFile(const char* FileName, int row_size, int vector_size, int number_size);

    /* Atributes */
    LoadedFile* ruleHandler;
    vector<LoadedFile*> inputHandler;
};

#endif	/* FILEHANDLER_H */

