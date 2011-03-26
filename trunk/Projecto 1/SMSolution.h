/* 
 * File:   SMSolution.h
 * Author: guilhermerodrigues
 *
 * Created on March 26, 2011, 11:17 PM
 */

#ifndef SMSOLUTION_H
#define	SMSOLUTION_H

#include "FileHandler.h"

struct StateMachine {
    short value;
    short index;
    
    bool isFinal; //When accepted next state value atribute holds classification

    vector<StateMachine> nextState;
};

class SMSolution {
public:
    SMSolution();
    SMSolution(const SMSolution& orig);
    virtual ~SMSolution();

    void execute();

private:

};

#endif	/* SMSOLUTION_H */

