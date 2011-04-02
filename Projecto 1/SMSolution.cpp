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
#include <cmath>

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
//    ruleSet = fileHandler.readRuleFile("dataset/sm_rules.csv");
        ruleSet = fileHandler.readRuleFile("dataset/THE_PROBLEM/rules2M.csv");
//    ruleSet = fileHandler.readRuleFile("dataset/xs_rules.csv");

    inputSet = fileHandler.readInputFile("dataset/THE_PROBLEM/trans_day_2.csv");
//        inputSet = fileHandler.readInputFile("dataset/sm_input.csv");
//    inputSet = fileHandler.readInputFile("dataset/xs_input.csv");

    buildStateMachine(ruleSet);

//    map<cell_value, ContainFirst*>::iterator pit;
//    vector<StateNode*>* vptr;
//        for(int i=0; i<INPUT_SIZE; i++) {
//            for(pit = mappedIndexes[i].begin(); pit != mappedIndexes[i].end(); pit++) {
//                cout << (*pit).first<< endl;
//                vector<StateNode*>::iterator it;
//                for(int j=0; j<INPUT_SIZE; j++) {
//                    vptr = (*pit).second->next;
//                    it = vptr[j].begin();
//                    while(it != vptr[j].end()) {
//                        printSM(*it, i+1);
//                        it++;
//                    }
//                }
//            }
//            cout << endl;
//        }

    clock_t s = clock();

#ifdef SERIAL
    cell_vector::iterator input_it;

    map< cell_value, ContainFirst* >::iterator mit;

    vector<StateNode*>::iterator it;
    pair<vector<StateNode*>::iterator, vector<StateNode*>::iterator> ret;

    StateNode cmp;
    StateNode *state;
    ContainFirst *fs;

//    for (int i = 0; i < INPUT_SIZE; i++) {
//        for (it = startIndex[i].begin(); it < startIndex[i].end(); it++) {
//            cell_array ca = (*ruleSet)[(*it)->index];
//            for (int j = 0; j < INPUT_SIZE + 1; j++) {
//                cout << ca[j] << " ";
//            }
//            cout << endl;
//        }
//    }
//    int nbc, nc, nb, ncy, nwcy;
//    nbc = nc = nb = ncy = nwcy = 0;

    for (input_it = inputSet->begin(); input_it < inputSet->end(); input_it++) {

        for (int i = 0; i < INPUT_SIZE; i++) {
            cmp.value = (*input_it)[i];
//            cout << "Looking for: " << cmp.value << endl;
//            ncy++;
            mit = mappedIndexes[i].find(cmp.value);

//            nbc++;
            if (mit != mappedIndexes[i].end()) {
//                cout << "  match found: " << (*mit).first << endl;
                fs = (*mit).second;
                for (int j=i+1; j < INPUT_SIZE; j++) {

                    cmp.value = (*input_it)[j];
//                    ncy+=2;

                    ret = equal_range(fs->next[j].begin(), fs->next[j].end(), &cmp, compareObj);

                    for (it = ret.first; it != ret.second; ++it) {

                        state = (*it)->next;
//                        nbc++;
//                        cout << "    comparing against: " << state->value << endl;
                        while (true) {
//                            nwcy++;
                            if (state->next != NULL) {
                                if (state->value != (*input_it)[state->index]) {
//                                    nb++;
                                    break;
                                } else {
                                    state = state->next;
                                }
                            } else {
//                                nb++;
                                fileHandler.addOutput(*input_it, state->value);
                                break;
                            }
                        }
                    }
                }

                it = fs->next[i].begin();
//                nbc++;
                while (it < fs->next[i].end() && (*it)->index < 0) {
//                    nbc++;
//                    nwcy++;
                    fileHandler.addOutput(*input_it, (*it)->value);
                    it++;
                }
            }
        }
        
        if (hasZeroRule)
            fileHandler.addOutput(*input_it, zeroClass);
    }
//    printf("boolean Comp: %d\nContinues: %d\nBreaks: %d\nBinary searches: %d\nWhile cycles: %d\n", nbc, nc, nb, ncy, nwcy);
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
        omp_lock_t mutex;
        omp_init_lock(&mutex);
        
#pragma omp parallel default(shared) num_threads(NUM_THREADS)
    {
        int my_id = omp_get_thread_num();
        cell_vector::iterator input_it = inputSet->begin() + my_id;
        printf("openMP process %d has begun\n", my_id);
        vector<StateNode*>::iterator it;
        pair<vector<StateNode*>::iterator, vector<StateNode*>::iterator> ret;
        double opmpStart = omp_get_wtime();

        StateNode cmp;
        StateNode *state;
        int total=0, k=0;
        int workDiv = 1000000 / NUM_THREADS;

        while (input_it < inputSet->end()) {
//        while (k<workDiv) {
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
//                omp_set_lock(&mutex);
//                cout << " openMP thread " << my_id << endl;
//                for(int i=0; i<10; i++) {
//                    cout << (*input_it)[i] << " ";
//                }
//                cout << endl;
//                omp_unset_lock(&mutex);
            k++;
            input_it += NUM_THREADS;
        }

        if (hasZeroRule) {
            for (input_it = inputSet->begin() + my_id; input_it < inputSet->end(); input_it += NUM_THREADS) {
                fileHandler.addOutput(*input_it, zeroClass, my_id);
            }
        }
        omp_set_lock(&mutex);
        cout << "my Id: " << my_id << ": " << omp_get_wtime() - opmpStart << endl;
        omp_unset_lock(&mutex);
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

    cout << "\nExecuting all inputs took: " << (((double) clock() - s) / CLOCKS_PER_SEC) << endl;

    cout << "\nFile Handler memory used: " << fileHandler.getMemoryUsed() / (float) (1048576) << " MB\n";

    fileHandler.manageOutputOf(inputSet);

#ifdef PTHREADS
    pthread_exit(NULL);
#endif
}

void SMSolution::openMP_work(int id) {
    int my_id = id;
    cell_vector::iterator input_it = inputSet->begin() + my_id;
    printf("openMP process %d has begun\n", my_id);
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
        for (input_it = inputSet->begin() + my_id; input_it < inputSet->end(); input_it += NUM_THREADS) {
            fileHandler.addOutput(*input_it, zeroClass, my_id);
        }
    }
}

void SMSolution::thread_work() {
    int my_id;
    int num_worked = 0;
    LOCK(&mutex_ID);
    my_id = work_ID++;
    cout << "Thread " << my_id << " reporting in" << endl;
    UNLOCK(&mutex_ID);

    cell_vector::iterator input_it = inputSet->begin() + my_id;

    vector<StateNode*>::iterator it;
    pair<vector<StateNode*>::iterator, vector<StateNode*>::iterator> ret;

    StateNode cmp;
    StateNode *state;

    clock_t s = clock();

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

        num_worked++;
        input_it += NUM_THREADS;
    }

    if (hasZeroRule) {
        for (input_it = inputSet->begin() + my_id; input_it < inputSet->end(); input_it += NUM_THREADS) {
            fileHandler.addOutput(*input_it, zeroClass, my_id);
        }
    }

    LOCK(&mutex_ID);
    cout << "Thread " << my_id << " handled " << num_worked << " inputs";
    cout << "\nAnd took: " << (((double) clock() - s) / CLOCKS_PER_SEC) << endl;
    UNLOCK(&mutex_ID);

    pthread_exit((void*) 0);
}

void SMSolution::printSM(StateNode* sm, int d) {
    
    for (int i = 0; i < d; i++) {
        cout << "  ";
    }

    cout << sm->index << " " << sm->value << endl;

    if (sm->next != NULL)
        printSM(sm->next, d + 1);

}

void SMSolution::buildStateMachine(cell_vector* ruleSet) {

    cell_vector::iterator rule_it = ruleSet->begin();
    // 980000,744000,744000,744000,720000,720000,716000,712000,712000,708000,
    // 3.75

    clock_t s = clock();

//    int countIdx[INPUT_SIZE];
//    int sizes, nulls;
//    sizes = 0, nulls = 0;
//    countIdx[0] = countIdx[1] = countIdx[2] = countIdx[3] = countIdx[4] = 0;
//    countIdx[5] = countIdx[6] = countIdx[7] = countIdx[8] = countIdx[9] = 0;

    // startIndex[INPUT_SIZE];
    StateNode *ptr;
//    bool hasIndex[10][10000];
    short depth;

    while (rule_it < ruleSet->end()) {
        ptr = NULL;
        depth = 0;
        for (int i = 0; i < INPUT_SIZE; i++) {
            if ((*rule_it)[i] != 0) {
//                countIdx[i]++;
//                sizes++;
                depth++;
                StateNode *newState = new StateNode;
//                if(i==9 && (*rule_it)[i] == 5620)
//                    int b=0;
                newState->index = i;
                newState->value = (*rule_it)[i];

                if (ptr!=NULL) {
                    if (depth==2) {
//                        printf("pushed %d, in vector[%d] of key: %d on index %d\n", newState->value, i, ptr->value, ptr->index);
                        ((mappedIndexes[ptr->index])[ptr->value])->next[i].push_back(newState);
                        delete ptr;
                    } else {
//                        printf("added %d to the tail of %d\n", newState->value, ptr->value);
                        ptr->next = newState;
                    }
                } else {
//                    bool fe = hasIndex[i][newState->value];
//                    if ( !hasIndex[i][newState->value] ) {
//                        if(i==9 && newState->value == 5620)
//                            int gj = 0;
//                        hasIndex[i][newState->value] = 1;
//                        //printf("inserted %d on index %d\n", newState->value, i);
                        mappedIndexes[i].insert(pair< cell_value, ContainFirst* >(newState->value, new ContainFirst));
//                    }
                }

                ptr = newState;
            }
        }

        if (ptr != NULL) {
//            int c[10];
//            for (int i = 0; i < INPUT_SIZE; i++) {
//                c[i] = (*rule_it)[i];
//            }
            StateNode *newState = new StateNode;
            newState->index = -1;
            newState->value = (*rule_it)[INPUT_SIZE];

            if (depth > 1) {
                newState->next = NULL;
                ptr->next = newState;
            } else {
//                map< cell_value, ContainFirst* >::iterator fg = mappedIndexes[ptr->index].find(ptr->value);
//                int a = (fg == mappedIndexes[ptr->index].end()) ? 0 : 1;
//                bool t1 = hasIndex[ptr->index][ptr->value];
                ((mappedIndexes[ptr->index])[ptr->value])->next[ptr->index].push_back(newState);
            }
        } else {
            hasZeroRule = true;
            zeroClass = (*rule_it)[INPUT_SIZE];
        }
        
        rule_it++;
    }

//    for (int i = 0; i < INPUT_SIZE; i++)
//        cout << countIdx[i] << " ";
//
//    cout << endl << " average size of rules: " << sizes / (float) 2000000 << endl;
//    cout << endl << " number of empty rules: " << nulls << endl;

    map<cell_value, ContainFirst*>::iterator pit;

    for (int i = 0; i < INPUT_SIZE; i++) {
        for(pit = mappedIndexes[i].begin(); pit != mappedIndexes[i].end(); pit++) {
            for(int j=0; j<INPUT_SIZE; j++) {
                sort((*pit).second->next[j].begin(), (*pit).second->next[j].end(), compareObj);
                j++;
                sort((*pit).second->next[j].begin(), (*pit).second->next[j].end(), compareObj);
            }
        }
    }

    cout << "\nState machine build took: " << (((double) clock() - s) / CLOCKS_PER_SEC) << endl;
}