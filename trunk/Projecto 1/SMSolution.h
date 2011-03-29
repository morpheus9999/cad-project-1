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
    unsigned short index;
    unsigned short value;

    StateNode* next;
};

struct StateCompare {
    bool operator() (const StateNode* lhs, const StateNode* rhs) const {
        return lhs->value < rhs->value;
    }
};

struct cellCmp {
  bool operator() (int *a, int *b) { 
      for (int i = 0; i < 10; i++) {
        if (!(a == 0 || b == 0) && a[i] != b[i])
            return a[i] < b[i];
        }

    return false;
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

    void executeSM(StateNode* state, int* input);
    void printSM(StateNode* state, int d);
    void buildStateMachine(vector<int*>* ruleSet);
};

#endif	/* SMSOLUTION_H */

