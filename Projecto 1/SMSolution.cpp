/* 
 * File:   SMSolution.cpp
 * Author: guilhermerodrigues
 * 
 * Created on March 26, 2011, 11:17 PM
 */

#include "SMSolution.h"
#include <iostream>

#define TIME_DEBUG

using namespace std;

SMSolution::SMSolution() {
}

SMSolution::SMSolution(const SMSolution& orig) {
}

SMSolution::~SMSolution() {
}

void SMSolution::execute() {
    cell_vector* ruleSet;
    ruleSet = fileHandler.readRuleFile("dataset/sm_rules.csv");

    cell_vector* input;
    input = fileHandler.readInputFile("dataset/THE_PROBLEM/trans_day_1.csv");
//    input = fileHandler.readInputFile("dataset/sm_input2.csv");

    buildStateMachine(ruleSet);
//
//    for(int i=0; i<INPUT_SIZE; i++) {
//        multiset<StateNode*>::iterator it = startIndex[i].begin();
//        while(it != startIndex[i].end()) {
//            printSM(*it, i);
//            it++;
//        }
//    }

    /********************************** CASO 00000000000 !!!! !!! !!!!! !   *************************/
    /********************************** CASO 00000000000 !!!! !!! !!!!! !   *************************/
    /********************************** CASO 00000000000 !!!! !!! !!!!! !   *************************/
    /********************************** CASO 00000000000 !!!! !!! !!!!! !   *************************/
    /********************************** CASO 00000000000 !!!! !!! !!!!! !   *************************/
    /********************************** CASO 00000000000 !!!! !!! !!!!! !   *************************/


    cell_vector::iterator input_it = input->begin();

    multiset<StateNode*, StateCompare>::iterator it;
    pair<multiset<StateNode*, StateCompare>::iterator,multiset<StateNode*, StateCompare>::iterator> ret;

    StateNode cmp;

    clock_t s = clock();
    int k=1;
    while (input_it < input->end()) {
        for (int i = 0; i < INPUT_SIZE; i++) {
            //cout << "current index, value: " << i << ", " << (*input_it)[i] << endl;
            cmp.value = (*input_it)[i];
            
            ret = startIndex[i].equal_range(&cmp);
            for (it=ret.first; it!=ret.second; ++it) {
                //cout << "checking :" << (*it)->value << endl;
                executeSM(*it, *input_it);
            }
        }

        k++;
        input_it++;
    }
    cout << "\nExecuting all inputs took: " << (((double)clock() - s) / CLOCKS_PER_SEC) << endl;

    cout << "\nFile Handler memory used: " << fileHandler.getMemoryUsed() /(float) (1048576) << " MB\n";

    fileHandler.manageOutputOf(input);
}

void SMSolution::executeSM(StateNode *state, cell_array input) {
    
    if (state->next != NULL) {
        if (state->value == input[state->index]) {
            executeSM(state->next, input);
        }
    } else {
        fileHandler.addOutput(input, state->value);
    }
}

void SMSolution::printSM(StateNode* sm, int d) {
    
    for (int i = 0; i < d; i++) {
        cout << "  ";
    }

    cout << sm->index << " " << sm->value << endl;

    if(sm->next != NULL)
        printSM(sm->next, d+1);

}

void SMSolution::buildStateMachine(cell_vector* ruleSet) {

    cell_vector::iterator rule_it = ruleSet->begin();
    // 980000,744000,744000,744000,720000,720000,716000,712000,712000,708000,
    // 3.75

    clock_t s = clock();

    multiset<StateNode*, StateCompare>::iterator it;

    int k = 1;

    // startIndex[INPUT_SIZE];
    StateNode *ptr;

    while (rule_it < ruleSet->end()) {
        ptr = NULL;
        for (int i = 0; i < INPUT_SIZE; i++) {
            if ((*rule_it)[i] != 0) {

                StateNode *newState = new StateNode;
                newState->index = i;
                newState->value = (*rule_it)[i];

                if (ptr != NULL) {
                    ptr->next = newState;
                } else {
                    startIndex[i].insert(newState);
                }

                ptr = newState;
            }
        }

        StateNode *newState = new StateNode;
        newState->index = 0;
        newState->value = (*rule_it)[INPUT_SIZE];
        newState->next = NULL;

        ptr->next = newState;

        rule_it++;
        k++;
    }

    cout << "\nState machine build took: " << (((double)clock() - s) / CLOCKS_PER_SEC) << endl;
}