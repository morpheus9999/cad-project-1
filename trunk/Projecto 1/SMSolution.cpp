/* 
 * File:   SMSolution.cpp
 * Author: guilhermerodrigues
 * 
 * Created on March 26, 2011, 11:17 PM
 */

#include "SMSolution.h"

SMSolution::SMSolution() {
}

SMSolution::SMSolution(const SMSolution& orig) {
}

SMSolution::~SMSolution() {
}

void SMSolution::execute() {
    FileHandler fh;

    vector<int*>* ruleSet;
    ruleSet = fh.readRuleFile("dataset/sm_rules.csv");

    vector<int*>* input;
    input = fh.readInputFile("dataset/sm_input.csv");

    
}
