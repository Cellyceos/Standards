//
//  main.cpp
//  ihoS57
//
//  Created by Kirill Bravichev on 25/05/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

#include "ihoS57.h"

#include <iostream>

using namespace Standards;

int main(int argc, const char * argv[]) {
    ENCChart chart("SAMPLE.000");
    
    if (chart.Open()) {
        chart.View();
    }
    
    cout << "Press any key to exit...";
    cin.get();
    
    return 0;
}
