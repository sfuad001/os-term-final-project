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
        unsigned long int** LCT;
        unsigned long int** LVPT;
        unsigned long int LCTrows;
        unsigned long int LCTcolumns;
        int n;
	unsigned long int hexToInt(string);
        unsigned long int truncateAddress (unsigned long int);
        void updateGlobalHistory(bool); //update the history

    public:
        Predictor(unsigned int, unsigned int, unsigned int, bool);
        string makePrediction(string, string, string);
        void lastValue(unsigned long int , unsigned long int );
        void lastValueWithLCT(unsigned long int , unsigned long int );
        void pcWithMemAddress(string, string, unsigned long int);

        void printStats();
};

#endif
