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

#define OUTPUT(file, input, r_class, t_id) \
(file)->output[(t_id)].push_back(pair<cell_array, cell_value>((input), (r_class)))

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

    fileHandler.start();
    
//    inputSet = fileHandler.readInputFile("dataset/THE_PROBLEM/trans_day_1.csv");
    //        inputSet = fileHandler.readInputFile("dataset/sm_input.csv");
    //    inputSet = fileHandler.readInputFile("dataset/xs_input.csv");

    buildStateMachine(ruleSet);

    //    map<cell_value, ContainFirst*>::iterator pit;
    //    map< level, vector<StateNode*>* >::iterator map_it;
    //    vector<StateNode*>::iterator itt;
    //        for(int i=0; i<INPUT_SIZE; i++) {
    //            for(pit = mappedIndexes[i].begin(); pit != mappedIndexes[i].end(); pit++) {
    //                
    //                cout << (*pit).first << endl;
    //                
    //                for(map_it=pit->second->next.begin(); map_it!=pit->second->next.end(); map_it++) {
    //                    itt = map_it->second->begin();
    //                    while(itt != map_it->second->begin()) {
    //                        printf("(%d)", map_it->first);
    //                        printSM(*itt, i+1);
    //                        itt++;
    //                    }
    //                }
    //                
    //            }
    //            cout << endl;
    //        }

    clock_t s = clock();

#ifdef SERIAL
    cell_vector::iterator input_it;

    map< cell_value, ContainFirst* >::iterator mit;
    map< level, vector<StateNode*>* >::iterator l_it;

    vector<StateNode*>::iterator it;

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

    int fileId=0;
    LoadedFile* cwf = fileHandler.getNextWorkFile(0);
    
    do {
        inputSet = cwf->workVector;
    for (input_it = inputSet->begin(); input_it < inputSet->end(); input_it++) {

        for (int i = 0; i < INPUT_SIZE; i++) {
            cmp.value = (*input_it)[i];
            //            ncy++;
            mit = mappedIndexes[i].find(cmp.value);

            //            nbc++;
            if (mit != mappedIndexes[i].end()) {
                fs = (*mit).second;

                l_it = fs->next.begin();

                if (l_it->first == i) {
                    for (it = l_it->second->begin(); it < l_it->second->end(); it++) {
                        OUTPUT(cwf, *input_it, (*it)->value, 0);
                    }

                    l_it++;
                }

                while (l_it != fs->next.end()) {

                    cmp.value = (*input_it)[l_it->first];
                    //                    ncy+=2;

                    it = lower_bound(l_it->second->begin(), l_it->second->end(), &cmp, compareObj);
                    //ret = equal_range(l_it->second->begin(), l_it->second->end(), &cmp, compareObj);

                    //for (it = ret.first; it != ret.second; ++it) {
                    for (; it != l_it->second->end() && (*it)->value == cmp.value; it++) {

                        state = (*it)->next;
                        //                        nbc++;
                        while (true) {
                            //                            nwcy++;
                            if (state->next != NULL) {
                                if (state->value != (*input_it)[state->index]) {
                                    //nb++;
                                    break;
                                } else {
                                    state = state->next;
                                }
                            } else {
                                //                                nb++;
                                OUTPUT(cwf, *input_it, state->value, 0);
                                break;
                            }
                        }
                    }

                    l_it++;
                }
            }
        }

        if (hasZeroRule)
            OUTPUT(cwf, *input_it, zeroClass, 0);
    }
        fileId++;
    
    cwf->finished();
    
    cwf = fileHandler.getNextWorkFile(fileId);
    } while(cwf != NULL);
    //printf("boolean Comp: %d\nContinues: %d\nBreaks: %d\nBinary searches: %d\nWhile cycles: %d\n", nbc, nc, nb, ncy, nwcy);
#endif

#ifdef OPENMP
    omp_lock_t mutex;
    omp_init_lock(&mutex);

#pragma omp parallel default(shared) num_threads(NUM_THREADS)
    {
        int k = 0, my_id = omp_get_thread_num();
        cell_vector::iterator input_it = inputSet->begin() + my_id;
        printf("openMP process %d has begun\n", my_id);

        map< cell_value, ContainFirst* >::iterator mit;
        map< level, vector<StateNode*>* >::iterator l_it;

        vector<StateNode*>::iterator it;

        StateNode cmp;
        StateNode *state;
        ContainFirst *fs;

        while (input_it < inputSet->end()) {

            for (int i = 0; i < INPUT_SIZE; i++) {
                cmp.value = (*input_it)[i];

                mit = mappedIndexes[i].find(cmp.value);

                if (mit != mappedIndexes[i].end()) {
                    fs = (*mit).second;

                    l_it = fs->next.begin();

                    if (l_it->first == i) {
                        for (it = l_it->second->begin(); it < l_it->second->end(); it++) {
                            fileHandler.addOutput(*input_it, (*it)->value, my_id);
                        }

                        l_it++;
                    }

                    while (l_it != fs->next.end()) {

                        cmp.value = (*input_it)[l_it->first];

                        it = lower_bound(l_it->second->begin(), l_it->second->end(), &cmp, compareObj);

                        for (; it != l_it->second->end() && (*it)->value == cmp.value; it++) {

                            state = (*it)->next;

                            while (true) {

                                if (state->next != NULL) {
                                    if (state->value != (*input_it)[state->index]) {

                                        break;
                                    } else {
                                        state = state->next;
                                    }
                                } else {

                                    fileHandler.addOutput(*input_it, state->value, my_id);
                                    break;
                                }
                            }
                        }

                        l_it++;
                    }
                }
            }

            if (hasZeroRule)
                fileHandler.addOutput(*input_it, zeroClass, my_id);

            k++;
            input_it += NUM_THREADS;
        }

        //    omp_set_lock(&mutex);
        //    cout << "my Id: " << my_id << ": " << omp_get_wtime() - opmpStart << endl;
        //    omp_unset_lock(&mutex);
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

//    cout << "\nFile Handler memory used: " << fileHandler.getMemoryUsed() / (float) (1048576) << " MB\n";

//    fileHandler.manageOutputOf(inputSet);


    pthread_exit(NULL);
}

void SMSolution::thread_work() {
#ifdef PTHREADS
    int my_id;
    int num_worked, startIndex, fileId = 0;

    LOCK(mutex_ID);
    my_id = work_ID++;
    cout << "Thread " << my_id << " reporting in" << endl;
    UNLOCK(mutex_ID);

    // Get a work item - Current Work File (CWF)
    LoadedFile* cwf = fileHandler.getNextWorkFile(fileId);
    cell_vector::iterator input_it;
    
    do {
    
    while (true) {
        
        LOCK(cwf->mutex);
        startIndex = cwf->availableWork - 1;
        cwf->availableWork -= WORK_RANGE;
        UNLOCK(cwf->mutex);

        if (startIndex < 0) {
            break;
        }
        
        num_worked = 0;
        input_it = cwf->workVector->begin() + startIndex;

        map< cell_value, ContainFirst* >::iterator mit;
        map< level, vector<StateNode*>* >::iterator l_it;

        vector<StateNode*>::iterator it;

        StateNode cmp;
        StateNode *state;
        ContainFirst *fs;

        while (num_worked < WORK_RANGE) {
            
            for (int i = 0; i < INPUT_SIZE; i++) {
                cmp.value = (*input_it)[i];

                mit = mappedIndexes[i].find(cmp.value);

                if (mit != mappedIndexes[i].end()) {
                    fs = (*mit).second;

                    l_it = fs->next.begin();

                    if (l_it->first == i) {
                        for (it = l_it->second->begin(); it < l_it->second->end(); it++) {
                            OUTPUT(cwf, *input_it, (*it)->value, my_id);
                        }

                        l_it++;
                    }

                    while (l_it != fs->next.end()) {

                        cmp.value = (*input_it)[l_it->first];

                        it = lower_bound(l_it->second->begin(), l_it->second->end(), &cmp, compareObj);

                        for (; it != l_it->second->end() && (*it)->value == cmp.value; it++) {

                            state = (*it)->next;

                            while (true) {

                                if (state->next != NULL) {
                                    if (state->value != (*input_it)[state->index]) {

                                        break;
                                    } else {
                                        state = state->next;
                                    }
                                } else {
                                    OUTPUT(cwf, *input_it, state->value, my_id);
                                    break;
                                }
                            }
                        }

                        l_it++;
                    }
                }
            }

            if (hasZeroRule)
                OUTPUT(cwf, *input_it, zeroClass, my_id);

            num_worked++;
            input_it--;
        }
    }
    
    fileId++;
    
    cwf->finished();
    
    cwf = fileHandler.getNextWorkFile(fileId);
    
    } while(cwf != NULL);
    
    LOCK(mutex_ID);
    cout << "Thread " << my_id << " handled " << num_worked << " exited\n";
    UNLOCK(mutex_ID);

    pthread_exit((void*) 0);
#endif
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
    map< cell_value, ContainFirst* >::iterator idx_it;
    //    pair< map< cell_value, ContainFirst* >::iterator, bool> idx_it;
    map< level, vector<StateNode*>* >::iterator map_it;

    short depth;

    ContainFirst *last;

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

                if (ptr != NULL) {
                    if (depth == 2) {
                        last = idx_it->second;

                        map_it = last->next.lower_bound(i);

                        if (map_it != last->next.end() && map_it->first == i) {
                            //                            printf("added in %d value %d\n", i, (*rule_it)[i]);
                            map_it->second->push_back(newState);

                        } else {
                            //                            printf("created new level %d with %d\n", i, (*rule_it)[i]);
                            map_it = last->next.insert(map_it, pair<level, vector<StateNode*>*>(i, new vector<StateNode*>));
                            map_it->second->push_back(newState);
                        }


                        delete ptr;

                    } else {

                        ptr->next = newState;
                    }
                } else {
                    //                    printf("INDEX %d added value %d\n", i, (*rule_it)[i]);
                    idx_it = mappedIndexes[i].lower_bound(newState->value);
                    if (idx_it->first != newState->value) {
                        idx_it = mappedIndexes[i].insert(idx_it, pair< cell_value, ContainFirst* >(newState->value, new ContainFirst));
                    }

                }

                ptr = newState;
            }
        }

        if (ptr != NULL) {

            StateNode *newState = new StateNode;
            newState->index = -1;
            newState->value = (*rule_it)[INPUT_SIZE];

            if (depth > 1) {
                newState->next = NULL;
                ptr->next = newState;
            } else {
                last = ((mappedIndexes[ptr->index])[ptr->value]);

                map_it = last->next.lower_bound(ptr->index);

                if (map_it->first == ptr->value) {
                    //                    printf("++added in %d value %d\n", ptr->index, (*rule_it)[INPUT_SIZE]);
                    map_it->second->push_back(newState);

                } else {
                    //                    printf("++created new level %d with %d\n", ptr->index, (*rule_it)[INPUT_SIZE]);
                    map_it = last->next.insert(map_it, pair<level, vector<StateNode*>*>(ptr->index, new vector<StateNode*>));
                    map_it->second->push_back(newState);
                }
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
        for (pit = mappedIndexes[i].begin(); pit != mappedIndexes[i].end(); pit++) {
            for (map_it = pit->second->next.begin(); map_it != pit->second->next.end(); map_it++) {
                sort(map_it->second->begin(), map_it->second->end(), compareObj);
            }
        }
    }

    // Release space reserved by file handler for rules
    fileHandler.freeRuleSpace();

    cout << "\nState machine build took: " << (((double) clock() - s) / CLOCKS_PER_SEC) << endl;
}