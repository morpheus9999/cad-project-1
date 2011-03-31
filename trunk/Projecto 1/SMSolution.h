/* 
 * File:   SMSolution.h
 * Author: guilhermerodrigues
 *
 * Created on March 26, 2011, 11:17 PM
 */

#ifndef SMSOLUTION_H
#define	SMSOLUTION_H

#include "FileHandler.h"

#define NUM_CLASS 100

struct StateNode {
    cell_value index;
    cell_value value;

    StateNode* next;
};

struct StateCompare {
    bool operator() (const StateNode* lhs, const StateNode* rhs) const {
        return lhs->value < rhs->value;
    }
};

void* InitiateThread(void* p);

class SMSolution {
public:
    SMSolution();
    SMSolution(const SMSolution& orig);
    virtual ~SMSolution();

    void execute();
    void thread_work();
    void openMP_work();

    void addZeroRuleOutput(cell_array input);

private:
    int work_ID;
    pthread_mutex_t mutex_ID;

    bool hasZeroRule;
    cell_value zeroClass;
    StateCompare compareObj;

    cell_vector* inputSet;

    FileHandler fileHandler;
    StateNode finalState[NUM_CLASS];
    vector<StateNode*> startIndex[INPUT_SIZE];
    
    void printSM(StateNode* state, int d);
    void buildStateMachine(cell_vector* ruleSet);
};

#endif	/* SMSOLUTION_H */

