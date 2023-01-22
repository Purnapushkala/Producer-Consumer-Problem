//
//  time.cpp
//  prodcon
//
//  Created by Purna Hari on 2022-10-31.
//

#include <stdio.h>
#include <chrono>
extern double totalTime;
using namespace std;
template <typename T>
double ret_time(T begintime){
    //https://stackoverflow.com/questions/71915860/how-initialize-time-points-and-set-them-to-a-falsy-value-afterward-in-c
    auto endtime = chrono::high_resolution_clock::now();
    std::chrono::duration<double> current_Time = endtime - begintime;
    totalTime = (double)(current_Time.count());
    return (double)(current_Time.count());
}
