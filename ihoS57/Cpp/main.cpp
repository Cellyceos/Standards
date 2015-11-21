//
//  main.cpp
//  ihoS57
//
//  Created by Kirill Bravichev on 25/05/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

#include <ratio>
#include <chrono>
#include <fstream>
#include <iostream>

#include "ihoS57.h"

using namespace Standards;
using namespace std::chrono;

#define FILE_INPUT_NAME "SAMPLE.000"
//#define FILE_OUTPUT_NAME "out.txt"

int main(int argc, const char * argv[]) {
    auto &&start = high_resolution_clock::now();

    ENCChart chart(FILE_INPUT_NAME);
    
#ifdef FILE_OUTPUT_NAME
    ofstream file(FILE_OUTPUT_NAME, ios::out);
    auto &&buff = cout.rdbuf();
    cout.rdbuf(file.rdbuf());
#endif
    
    if (chart.Open()) {
#ifdef FILE_OUTPUT_NAME
        chart.View();
#else
        chart.Ingest();
#endif
    }
    
    auto &&stop = high_resolution_clock::now();
    auto &&timeSpan = duration_cast<duration<double>>(stop - start);

#ifdef FILE_OUTPUT_NAME
    cout.rdbuf(buff);
    file.close();
#endif
    
    cout << "Duration time: " << timeSpan.count() <<  " sec." << endl;
    
    return 0;
}
