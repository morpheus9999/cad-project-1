/* 
 * File:   SMSolution.h
 * Author: guilhermerodrigues
 *
 * Created on March 26, 2011, 11:17 PM
 */

#ifndef SMSOLUTION_H
#define	SMSOLUTION_H

#include "FileHandler.h"

#include <set>

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

class SMSolution {
public:
    SMSolution();
    SMSolution(const SMSolution& orig);
    virtual ~SMSolution();

    void execute();

private:
    FileHandler fileHandler;
    StateNode finalState[NUM_CLASS];
    multiset<StateNode*, StateCompare> startIndex[INPUT_SIZE];

    void executeSM(StateNode* state, cell_array input);
    void printSM(StateNode* state, int d);
    void buildStateMachine(cell_vector* ruleSet);
};

#endif	/* SMSOLUTION_H */

