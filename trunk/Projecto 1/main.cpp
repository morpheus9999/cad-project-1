/* 
 * File:   main1.cpp
 * Author: jojo
 *
 * Created on 25 de Março de 2011, 1:43
 */

#include <cstdlib>
#include <iostream>
#include <time.h>

// TODO : remove useless libraries <algorithm>
#include <algorithm>

#include "def.h"
#include "FileHandler.h"
#include "SMSolution.h"

using namespace std;

inline bool compArray (int *a, int *b) {
    for (int i = 0; i < 10; i++) {
        if (!(a == 0 || b==0) && a[i] != b[i])
            return a[i] < b[i];
    }

    return false;
}

bool passesRule(int* input, int* rule) {
    
    for (int i = 0; i < 10; i++) {
        if(rule[i] != 0 && rule[i] != input[i])
            return false;
    }
    
    return true;
}

int main (int argc, const char * argv[])
{

#ifdef GRAVE
    SMSolution sml;
    sml.execute();
    return 0;
#endif

    FileHandler fh;

    vector<int*>* ruleSet;
    ruleSet = fh.readRuleFile("dataset/sm_rules.csv");

    vector<int*>* input;
    input = fh.readInputFile("dataset/sm_input.csv");

    vector<int*>::iterator input_it = input->begin();
    vector<int*>::iterator rule_it;
    
    clock_t s = clock();
    while(input_it < input->end()) {
        rule_it = ruleSet->begin();

        while (rule_it < ruleSet->end()) {
            if (passesRule(*input_it, *rule_it)) {

                for (int i = 0; i < 10; i++) {
                    cout << (*input_it)[i] << ",";
                }

                cout << (*rule_it)[RULE_SIZE - 1] << endl;
            }
            rule_it++;
        }
        
        input_it++;
    }
    cout << "\nSearch took: " << (((double)clock() - s) / CLOCKS_PER_SEC) << endl;
    return 0;
}

