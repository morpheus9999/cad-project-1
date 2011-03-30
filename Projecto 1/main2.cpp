/* 
 * File:   main2.cpp
 * Author: jojo
 *
 * Created on 26 de Março de 2011, 20:52
 */

#include <cstdlib>
#include <iostream>
#include <list>
#include <vector>
using namespace std;
/*
 * 
 */

//#include "def.h"
#include "FileHandler.h"
#include "SMSolution.h"

#include <cstdlib>
#include <iostream>
#include <time.h>

#define NUM_THREADS 10

int main(int argc, char** argv) {
    //pthread_t threads[NUM_THREADS];


    vector <list <int> > m;

    vector< vector<list<int> > > myvector (10,vector< list<int> >(1000000,0));

    FileHandler fh;

    vector<int*>* ruleSet;
    ruleSet = fh.readRuleFile("dataset/sm_rules.csv");

    vector<int*>* input;
    input = fh.readInputFile("dataset/THE_PROBLEM/trans_day_1.csv");

    vector<int*>::iterator input_it = input->begin();
    vector<int*>::iterator rule_it;

    rule_it = ruleSet->begin();
    //input[0][0];

    
//    for (int i = 0; i < ruleSet->size(); i++) {
//        for(int k=0;k<10;k++)
//            cout << (*ruleSet)[i][k] << " ";
//
//        cout << endl;
//
//    }


    clock_t s = clock();


    
        for (int i = 0; i < input->size(); i++) {

            for (int k=0;k<10;k++){
           // myvector[k][input_it[k][i]].push_back(i);
            //cout<< input_it[i][k] << " ";
            myvector[k][input_it[i][k]].push_back(i);
            }
            //cout << endl;
        }
    cout << "\n1a took: " << (((double)clock() - s) / CLOCKS_PER_SEC) << endl;
    cout << myvector[0][12].size()<< endl;
    
    cout << (*ruleSet)[2][2]<< endl;
    cout << "merda"<< (*ruleSet)[10][9] << endl;
    cout << myvector [9][1017].size() << endl;
    list<int>::iterator itt;
    int u=10000;
    s = clock();
    for (int i = 0; i < ruleSet->size(); i++) {

        if(u==i){
            cout << i << endl;
            u+=u;
            }
        //rule_it = ruleSet->begin();
        int minimo=1000;
        int min=0;
        //cout << "entra1 " << i<<endl;

        
        int test0=0;
        for(int k=0;k<10; k++){
            //cout <<"R:" << (*ruleSet)[i][k] <<endl;
            if((*ruleSet)[i][k]!=0){
                test0=1;
                
//                cout << "mylist contains:";
//                for (itt=myvector[i][((*ruleSet)[i][k])].begin(); itt!=myvector[i][((*ruleSet)[i][k])].end(); itt++)
//                cout << " " << *itt;
//                cout << endl;
                if(minimo>myvector[k][((*ruleSet)[i][k])].size()){
                    minimo=myvector[k][((*ruleSet)[i][k])].size();
                    min=k;

                }
            }

        }
        //cout << "entra2" << endl;
        //cout << "min: " << min << endl;
        list <int> temp=myvector[min][(*ruleSet)[i][min]];
        rule_it = ruleSet->begin();
        list<int>::iterator it;
        list<int>::iterator it2;
        for(int k=0;k<10;k++){
            //cout << "k "<< k<< endl;;

            if(rule_it[i][k]!=0 && k!=min){
                //cout << "k entrou :"<< k << endl;
                for (it=temp.begin(); it!=temp.end(); it++){
                    bool apaga=false;
                    rule_it = ruleSet->begin();

                    for (it2=myvector[k][rule_it[i][k]].begin(); it2!=myvector[k][rule_it[i][k]].end(); it2++){
                        if((*it2<*it))
                            break;

                        if(*it == *it2 ){
                           // cout << "compara->" << *it << " " << *it2 << endl;
                            apaga=true;
                            break;
                           // cout << "vai apagar" << endl;
                            }
                    }
                    if(apaga==false)
                        temp.remove(*it);
                }
            }
        }
        
        if(test0!=0){

//            if(temp.size()>0){
//                cout << "RESULTADOS  : "<<i<<" : " ;
//                for ( it=temp.begin();it!=temp.end();it++){
//                    cout << *it << " ";
//
//                }
//                cout <<endl;
//            }

        }else{
//            if(test0==0){
//            cout << "RESULTADOS  : "<<i<<" : " ;
//            for (int l=0;l<input->size() ; l++){
//                cout <<l << " ";
//
//            }
//            cout <<endl;
//                }

        }
    }

//    list<int>::iterator it;
//                cout << "mylist contains:";
//                for (it=myvector[0][1].begin(); it!=myvector[0][1].end(); it++)
//                cout << " " << *it;
//                cout << endl;
    
//    for(int i=0;i<rule_it;i++){
//        rule_it=ruleSet->begin();
//        for(int k=0;k<10;k++){
//
//            if(rule_it[i][k]!=0){
//
//                list<int>::iterator it;
//                cout << "mylist contains:";
//                for (it=myvector[i][rule_it[i][k]].begin(); it!=myvector[i][rule_it[i][k]].end(); it++)
//                cout << " " << *it;
//                cout << endl;
//
//            }
//
//
//        }
//
//
//
//    }


//    while(input_it < input->end()) {
//        rule_it = ruleSet->begin();
//
//        while (rule_it < ruleSet->end()) {
//            if (passesRule(*input_it, *rule_it)) {
//
//                for (int i = 0; i < 10; i++) {
//                    cout << (*input_it)[i] << ",";
//                }
//
//                cout << (*rule_it)[RULE_SIZE - 1] << endl;
//            }
//            rule_it++;
//        }
//
//        input_it++;
//    }




    cout << "\nSearch took: " << (((double)clock() - s) / CLOCKS_PER_SEC) << endl;



    return 0;


    
    
    
//    myvector[0][0].push_back(1);
//    myvector[0][0].push_back(3);
//    myvector[0][2].push_back(2);
//    myvector[0][3].push_back(3);
//
//    myvector[0][70].push_back(70);
//    int x=0;
//    int size;
//    int valor;
//    for (int i = 0; i < size; i++) {
//        valor=0;
//        myvector[x][valor].push_back(i);
//
//
//
//    }
//
//
//
//
//    list<int>::iterator it;
//    cout << "mylist contains:";
//    for (it=myvector[0][10].begin(); it!=myvector[0][10].end(); it++)
//    cout << " " << *it;
//    cout << endl;

   //cout << myvector[0][0].at(0)<<endl;
    //m.insert(0,)
    //myvector.at(0);



    //list<int> mylist;





}

