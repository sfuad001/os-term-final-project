#ifndef __BRANCH_H_
#define __BRANCH_H_

#include <iostream>
using namespace std;

class Predictor{
    private:

        unsigned int addrBits; //number of bits of address used to index  
        unsigned int historyBits; //number of bits used for global history
        unsigned int globalHistory; //record of global histroy
        int correct;
        int total;
        bool debug;
        unsigned int** LCT;
        unsigned int** LVPT;
        unsigned int LCTrows;
        unsigned int LCTcolumns;
        int n;
	unsigned int hexToInt(string);
        unsigned int truncateAddress (unsigned int);
        void updateGlobalHistory(bool); //update the history

    public:
        Predictor(unsigned int, unsigned int, unsigned int, bool);
        string makePrediction(string, string, string);
        void printStats();
};

#endif
