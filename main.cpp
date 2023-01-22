//
//  main.cpp
//  prodcon
//
//  Created by Purna Hari on 2022-10-19.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <pthread.h>
#include <stdio.h>
#include <string>
#include "variable.h"
#include "time.h"
void Trans( int n );
void Sleep( int n );
using namespace std;
bool working = true;
int  ask=0, consCount=0;// Current consumer no.;
bool producerExist = false;
double totalTime = 0.00;
vector<string> vecOfStrs,prod; //
vector<int> completedbycon; //Tells the tasks completed by each consumer
int consumerRunning = 0;
int tid =1,numOfT = 0, buffersize, buffercount, bufferItems;     // Number of T<n> commands
int totalCommands = 0;         // Number of total commands - T<n> and S<n>
int commandsRemaining = 0;// Commands remaining
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; // For mutual exclusion
pthread_cond_t condfull = PTHREAD_COND_INITIALIZER, condincr = PTHREAD_COND_INITIALIZER;
template <typename T>
double ret_time(T begintime){
    //https://stackoverflow.com/questions/71915860/how-initialize-time-points-and-set-them-to-a-falsy-value-afterward-in-c
    auto endtime = chrono::high_resolution_clock::now();
    std::chrono::duration<double> current_Time = endtime - begintime;
    totalTime = (double)(current_Time.count());
    return (double)(current_Time.count());
}
void inputFile()
//https://thispointer.com/c-how-to-read-a-file-line-by-line-into-a-vector/
{

    string str;
    while (getline(cin, str))
        {
            // Line contains string of length > 0 then save it in vector
            if(str.size() > 0)
                vecOfStrs.push_back(str);
            str.clear();
        }
    totalCommands = int(vecOfStrs.size());
    for( int i = 0; i < vecOfStrs.size(); i++ )
    {
        prod.push_back( vecOfStrs[i] );
        if( vecOfStrs[i].at(0) == 'T' )                       // Counting how many T commands
        {
            numOfT++;
            commandsRemaining++;
        }
    }

}

//https://cplusplus.com/reference/chrono/high_resolution_clock/now/
void * consumer(void * args){ // Thread func for consumer
    auto start = chrono::high_resolution_clock::now();
    int completed = 0;
    string * buffer = (string *) args;
    consumerRunning++;
    int conID = tid;
    tid ++;
    
    while (working)
    {
        if( commandsRemaining <= 0 )
        {
            consumerRunning--;
            completedbycon[ conID ] = completed;
            cout<<std::fixed<<std::setprecision(3)<< ret_time(start)<<" "<< "ID= " << conID<< "     "<< "Ask" << endl;
            ask++;
            pthread_exit(0);
        }
        pthread_mutex_lock(&mutex1);
        cout<<std::fixed<<std::setprecision(3)<< ret_time(start)<<" "<< "ID= " << conID<< "      "<< "Ask" << endl;
        ask++;
        while (bufferItems <= 0 &&commandsRemaining > 0) {
            pthread_cond_wait(&condincr, &mutex1);
        }
        int inbuffer = 0;// Consumers waiting in the buffer
        string current = buffer[consCount++];
        if(bufferItems > 0)
            
        
        cout<<std::fixed<<std::setprecision(3)<< ret_time(start)<<" "<< "ID= " << conID<< " " << "Q= " << inbuffer << " " << "Receive  " << current.substr( 1, current.size() - 1 ) << endl;
        inbuffer ++;
        consCount = consCount % buffersize;
        bufferItems--;
        commandsRemaining--;
        pthread_cond_signal(&condfull);
        pthread_mutex_unlock(&mutex1);
        Trans(stoi(current.substr( 1, current.size() - 1 )));
        cout<<std::fixed<<std::setprecision(3)<< ret_time(start)<<" "<< "ID= " << conID<< "    "<< "Complete   " << current.substr( 1, current.size() - 1 ) << endl;
        completed++;
        
    }
    pthread_exit(0);
}

//https://www.youtube.com/watch?v=rKGq9AlfPR4
void * producer(void * args){
    auto start = chrono::high_resolution_clock::now();
    string * buffer = (string *) args;
    for (int i = 0; i< totalCommands; i++) {
        if (sizeof(prod)==0)
            break;
        pthread_mutex_lock(&mutex1);
        while (bufferItems >= buffersize) {
            pthread_cond_wait(&condfull, &mutex1);
        }
        if(prod[0].at(0) == 'T') { // Working by adding T
            buffer[buffercount] = prod[0];
            buffercount++;
            bufferItems++;
            buffercount = buffercount % buffersize;
                
            cout<<std::fixed<<std::setprecision(3)<< ret_time(start)<<" "<< "ID= " << "0"<< " " << "Q= " << bufferItems << " " << "Work     " << prod[0].substr( 1, prod.size() - 1 ) << endl;
        }
        else if (prod[0].at(0) == 'S'){
            cout<<std::fixed<<std::setprecision(3)<< ret_time(start)<<" "<< "ID= " << "0"<< "    " << "Sleep      " << prod[0].substr( 1, prod.size() - 1 ) << endl;
            Sleep(stoi(prod[0].substr( 1, prod.size() - 1 )));
        }
            prod.erase(prod.begin());
            pthread_cond_signal(&condincr);
            pthread_mutex_unlock(&mutex1);
        }
    while(consumerRunning > 0 or producerExist == false)
       {
       }
       pthread_exit(0);
}

void printSummary(int nthreads){
    cout << "Summary: " << endl;
    cout << "  Work  " << numOfT<< endl;
    cout << "  Ask  " << ask << endl;
    cout << "  Recieve  " << numOfT<< endl;//  all tasks were completed
    cout << "  Complete  " << numOfT<< endl;
    cout << "  Sleep  " << totalCommands - numOfT << endl;

    for( int i = 1; i <= nthreads; i++ )
    {
        cout << " Thread  " << i << "    " << completedbycon[i] << endl;;
    }

    cout << "Transactions per second: " << std::setprecision( 2 ) << std::fixed << numOfT / totalTime << endl;
}

int main(int argc, const char * argv[]) {
    pthread_mutex_lock( &mutex1 );
    inputFile();
    pthread_mutex_unlock( &mutex1 );
    int nthreads = stoi(argv[1]);;
    pthread_t c[nthreads];
    string id = "0";

    if (argc< 2 or argc > 3){
        cout<<"Error in number of arguments";
        return 0;
    }
    else if (argc == 3){
        id = argv[2];
    }
    string filename = "prodcon." + id +".log";
    buffersize = 2*nthreads;
    
    string idbuf [buffersize];
    ofstream out( filename ); //https://stackoverflow.com/questions/10150468/how-to-redirect-cin-and-cout-to-files
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(out.rdbuf());
    completedbycon.resize( nthreads );
    // Synchronization by having both parent and child as thread
    pthread_create(&c[0], NULL, producer, idbuf);
    for (int i = 0; i < nthreads; i++){
        pthread_create(&c[i], NULL, consumer, idbuf);
        }
    for ( int i = 0; i < nthreads; i++){
        pthread_join(c[i], NULL);
    }

    printSummary(nthreads);
    producerExist = true;
    std::cout.rdbuf(coutbuf); //reset to standard output again
    return 0;
}
