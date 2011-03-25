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

#include "FileReader.h"

#define INPUT_SIZE 10

using namespace std;

vector<int*> rules;

const char* FileName = "dataset/sm_rules.csv";

int main (int argc, const char * argv[])
{
    /*
    FileReader fr;
    fr.read();
    return 0;
    */
    ifstream inFile (FileName);
    string line, item;

    if(!inFile.good()) {
        cout << "Failed to load file\n";
        return 0;
    }

    int* newRule;
    int rulecount = 0, i;

    clock_t s = clock();

    while (getline (inFile, line))
    {
        istringstream linestream(line);

        newRule = new int[INPUT_SIZE+1];

        cout << "Rule " << rulecount++ << " (" ;

        for (i=0; i<INPUT_SIZE; i++) {
            getline (linestream, item, ',');
            newRule[i] = atoi(item.c_str());
            cout << " " << atoi(item.c_str());
        }

        getline (linestream, item, ',');

        newRule[INPUT_SIZE] = atoi(item.c_str());
        
        rules.push_back(newRule);

        cout << ") Category: " << atoi(item.c_str()) << endl;
    }
    return 0;
}

