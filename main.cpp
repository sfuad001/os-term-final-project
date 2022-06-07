#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include "trace.h"
#include "predictor.h"

using namespace std;

int main(int argc, char ** argv){
    int opt;
    unsigned int m = 6; //Global branch history (up to 12)
    unsigned int n = 1; //n-bit predictor ( 1 or 2)
    unsigned int addressBits = 8; //how many bits of address is used (up to 12)
    bool debug = false;
    string fileName = "";
    while ((opt = getopt(argc,argv,"i:m:n:a:d")) != EOF){
        switch(opt){
            case 'i': fileName.assign(optarg); break;
            case 'm': m = atoi(optarg); break;  // Global history size
            case 'n': n = atoi(optarg); break; // n-bit predictor
            case 'a': addressBits = atoi(optarg); break; // how many LSB of address is used for indexing
            case 'd': debug = true; break;
	    case '?': printf("usage is \n -i : fileName \n -m : history length\n -n : n-bit\n -a : address length\n -d : debug\n");
            default: exit(1);
        }
    }

    /*
    if(fileName.empty()){
            printf("usage is \n -i : fileName \n -m : history length\n -n : n-bit\n -a : address length\n -d: debug\n");
	    exit(1);
    }
   */
    // Initialize Trace File
    //TraceFile trace(fileName);

    // Initialize Predictor
    Predictor pred(m,n, addressBits, debug);
    string pc = "7f6fd2b38e1f";
    string opName = "";
    string memAddress = "7f6fd2b64e68";
    string expectedLV, predictedLV;
    expectedLV = "000000000002de68";
    predictedLV = pred.makePrediction(pc, opName, memAddress, expectedLV);
    printf("PC address: 0x%s, predicted: 0x%s, expected: 0x%s\n", pc.c_str(), predictedLV.c_str(), expectedLV.c_str());

    // Execute trace through Predictor

    /*
    while(trace.getNextBranch(pc, memAddress, expectedLV)){
        predicted = pred.makePrediction(pc, memAddress, expectedLV);
    	if(debug){
		printf("PC address: 0x%s, predicted: %lu, expected: %lu\n", pc.c_str(), predictedLV, expectedLV);
    	}
    }*/

    // Print results
    pred.printStats();
    return 0;
}
