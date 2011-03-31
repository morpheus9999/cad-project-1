/* 
 * File:   SMSolution.cpp
 * Author: guilhermerodrigues
 * 
 * Created on March 26, 2011, 11:17 PM
 */

#include "SMSolution.h"
#include <iostream>
#include <pthread.h>
#include <omp.h>

#define LOCK(mutex)     pthread_mutex_lock((mutex))
#define UNLOCK(mutex)   pthread_mutex_unlock((mutex))


using namespace std;

void* InitiateThread(void* p) {
    SMSolution *tp = (SMSolution *) p;

    tp->thread_work();

    pthread_exit(NULL);
}

SMSolution::SMSolution() {
    hasZeroRule = false;
    zeroClass = -1;
}

SMSolution::SMSolution(const SMSolution& orig) {
}

SMSolution::~SMSolution() {
    pthread_mutex_destroy(&mutex_ID);
}

void SMSolution::execute() {
    cell_vector* ruleSet;
    ruleSet = fileHandler.readRuleFile("dataset/sm_rules.csv");
//    ruleSet = fileHandler.readRuleFile("dataset/THE_PROBLEM/rules2M.csv");

    inputSet = fileHandler.readInputFile("dataset/THE_PROBLEM/trans_day_1.csv");
//    inputSet = fileHandler.readInputFile("dataset/sm_input.csv");

    buildStateMachine(ruleSet);

    //    for(int i=0; i<INPUT_SIZE; i++) {
    //        vector<StateNode*>::iterator it = startIndex[i].begin();
    //        while(it != startIndex[i].end()) {
    //            printSM(*it, i);
    //            it++;
    //        }
    //    }

    clock_t s = clock();

#ifdef SERIAL
    cell_vector::iterator input_it;

    vector<StateNode*>::iterator it;
    pair<vector<StateNode*>::iterator, vector<StateNode*>::iterator> ret;

    StateNode cmp;
    StateNode *state;
    
    for (input_it = inputSet->begin(); input_it < inputSet->end(); input_it++) {
        
        for (int i = 0; i < INPUT_SIZE; i++) {
            cmp.value = (*input_it)[i];

            ret = equal_range(startIndex[i].begin(), startIndex[i].end(), &cmp, compareObj);

            for (it = ret.first; it != ret.second; ++it) {
                state = (*it)->next;

                if((state->next != NULL) && (state->value != (*input_it)[state->index])) {
                    continue;
                }

                while (true) {
                    if (state->next != NULL) {
                        if (state->value != (*input_it)[state->index]) {
                            break;
                        }else {
                            if (state->next->next != NULL) {
                                if (state->next->value != (*input_it)[state->next->index]) {
                                    break;

                                } else {
                                    state = state->next;
                                    continue;
                                }
                            } else {
                                fileHandler.addOutput((*input_it), state->value);
                                break;
                            }
                        }
                    } else {
                        fileHandler.addOutput((*input_it), state->value);
                        break;
                    }
                }
//                state = (*it)->next;
//
//                while (true) {
//                    if (state->next != NULL) {
//                        if (state->value != (*input_it)[state->index]) {
//                            break;
//                        } else {
//                            state = state->next;
//                        }
//                    } else {
//                        fileHandler.addOutput(*input_it, state->value, 0);
//                        break;
//                    }
//                }
            }
        }
        if (hasZeroRule)
            fileHandler.addOutput( *input_it, zeroClass);
    }
//                if((state->next != NULL) && (state->value != (*input_it)[state->index])) {
//                    continue;
//                }
//
//                while (true) {
//                    if (state->next != NULL) {
//                        if (state->value != (*input_it)[state->index]) {
//                            break;
//                        }else {
//                            if (state->next->next != NULL) {
//                                if (state->next->value != (*input_it)[state->next->index]) {
//                                    break;
//
//                                } else {
//                                    state = state->next;
//                                    continue;
//                                }
//                            } else {
//                                fileHandler.addOutput((*input_it), state->value);
//                                break;
//                            }
//                        }
//                    } else {
//                        fileHandler.addOutput((*input_it), state->value);
//                        break;
//                    }
//                }
#endif
    
#ifdef OPENMP

    SMSolution* ptr;
#pragma omp parallel default(none) num_threads(NUM_THREADS) \
shared(ptr)
    {
        ptr->openMP_work();
    }
#endif

#ifdef PTHREADS
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    int rc;
    long t;
    void *status;

    work_ID = 0;

    // Create mutex for work ID access
    pthread_mutex_init(&mutex_ID, NULL);

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (t = 0; t < NUM_THREADS; t++) {
        rc = pthread_create(&thread[t], &attr, InitiateThread, (void *) this);
        if (rc) {
            printf("ERROR; return code from pthread_create() is % d\n", rc);
            exit(-1);
        }
    }

    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for (t = 0; t < NUM_THREADS; t++) {
        rc = pthread_join(thread[t], &status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is % d\n", rc);
            exit(-1);
        }
    }
#endif

    cout << "\nExecuting all inputs took: " << (((double)clock() - s) / CLOCKS_PER_SEC) << endl;

    cout << "\nFile Handler memory used: " << fileHandler.getMemoryUsed() /(float) (1048576) << " MB\n";

    fileHandler.manageOutputOf(inputSet);

#ifdef PTHREADS
    pthread_exit(NULL);
#endif
}

void SMSolution::openMP_work() {
    int my_id = omp_get_thread_num();
    cell_vector::iterator input_it = inputSet->begin() + my_id;
    printf("openMP process %d has begun\n",my_id);
    vector<StateNode*>::iterator it;
    pair<vector<StateNode*>::iterator, vector<StateNode*>::iterator> ret;

    StateNode cmp;
    StateNode *state;

    while (input_it < inputSet->end()) {
        for (int i = 0; i < INPUT_SIZE; i++) {
            cmp.value = (*input_it)[i];

            ret = equal_range(startIndex[i].begin(), startIndex[i].end(), &cmp, compareObj);

            for (it = ret.first; it != ret.second; ++it) {
                state = (*it)->next;

                while (true) {
                    if (state->next != NULL) {
                        if (state->value != (*input_it)[state->index]) {
                            break;
                        } else {
                            state = state->next;
                        }
                    } else {
                        fileHandler.addOutput((*input_it), state->value, my_id);
                        break;
                    }
                }
            }
        }

        input_it += NUM_THREADS;
    }

    if (hasZeroRule) {
        for(input_it = inputSet->begin()+my_id; input_it < inputSet->end(); input_it+=NUM_THREADS) {
            fileHandler.addOutput( *input_it, zeroClass, my_id);
        }
    }
}

void SMSolution::thread_work() {
    int my_id;
    int num_worked = 0;
    LOCK(&mutex_ID);
    my_id = work_ID++;
    cout << "Thread "<< my_id << " reporting in" << endl;
    UNLOCK(&mutex_ID);

    cell_vector::iterator input_it = inputSet->begin()+my_id;

    vector<StateNode*>::iterator it;
    pair<vector<StateNode*>::iterator, vector<StateNode*>::iterator> ret;

    StateNode cmp;
    StateNode *state;
    
    clock_t s = clock();
    
    while (input_it < inputSet->end()) {
        for (int i = 0; i < INPUT_SIZE; i++) {
            cmp.value = (*input_it)[i];

            ret = equal_range (startIndex[i].begin(), startIndex[i].end(), &cmp, compareObj);

            for (it=ret.first; it!=ret.second; ++it) {
                state = (*it)->next;

                while (true) {
                    if (state->next != NULL) {
                        if (state->value != (*input_it)[state->index]) {
                            break;
                        }else {
                            state = state->next;
                        }
                    } else {
                        fileHandler.addOutput((*input_it), state->value, my_id);
                        break;
                    }
                }
            }
        }

        num_worked++;
        input_it+=NUM_THREADS;
    }
    
    if (hasZeroRule) {
        for(input_it = inputSet->begin()+my_id; input_it < inputSet->end(); input_it+=NUM_THREADS) {
            fileHandler.addOutput( *input_it, zeroClass, my_id);
        }
    }

    LOCK(&mutex_ID);
    cout << "Thread " << my_id << " handled " << num_worked << " inputs";
    cout << "\nAnd took: " << (((double)clock() - s) / CLOCKS_PER_SEC) << endl;
    UNLOCK(&mutex_ID);
    
    pthread_exit((void*) 0);
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

    vector<StateNode*>::iterator it;

    int countIdx[INPUT_SIZE];
    int sizes, nulls;

    sizes = 0, nulls = 0;
    countIdx[0] = countIdx[1] = countIdx[2] = countIdx[3] = countIdx[4] = 0;
    countIdx[5] = countIdx[6] = countIdx[7] = countIdx[8] = countIdx[9] = 0;

    // startIndex[INPUT_SIZE];
    StateNode *ptr;

    while (rule_it < ruleSet->end()) {
        ptr = NULL;
        for (int i = 0; i < INPUT_SIZE; i++) {
            if ((*rule_it)[i] != 0) {
                countIdx[i]++;
                sizes++;
                StateNode *newState = new StateNode;
                newState->index = i;
                newState->value = (*rule_it)[i];

                if (ptr != NULL) {
                    ptr->next = newState;
                } else {
                    startIndex[i].push_back(newState);
                }

                ptr = newState;
            }
        }
        
        if (ptr != NULL) {
            StateNode *newState = new StateNode;
            newState->index = -1;
            newState->value = (*rule_it)[INPUT_SIZE];
            newState->next = NULL;

            ptr->next = newState;
        } else {
            nulls++;
            hasZeroRule = true;
            zeroClass = (*rule_it)[INPUT_SIZE];
        }

        rule_it++;
    }

    for(int i=0; i<INPUT_SIZE; i++) 
        cout << countIdx[i] << " ";
    
    cout << endl << " average size of rules: " << sizes / (float) 2000000 << endl;
    cout << endl << " number of empty rules: " << nulls << endl;

    for(int i=0; i<INPUT_SIZE; i++) {
        sort(startIndex[i].begin(), startIndex[i].end(), compareObj);
        i++;
        sort(startIndex[i].begin(), startIndex[i].end(), compareObj);
    }

    cout << "\nState machine build took: " << (((double)clock() - s) / CLOCKS_PER_SEC) << endl;
}