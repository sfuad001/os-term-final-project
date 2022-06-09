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
        int fileLength;
        bool debug;
        unsigned long int stride;
        unsigned long int strideOld;
        double a;
        double c;
        int choice;
        int tournament;
        //unsigned long int strideOld;
        unsigned long int** LCT;
        unsigned long int** LVPT;
        unsigned long int** LRUCount;

        unsigned long int LCTrows;
        unsigned long int LCTcolumns;
        int n;
        //double a;
       // double c;
	unsigned long int hexToInt(string);
        unsigned long int truncateAddress (unsigned long int);
        void updateGlobalHistory(bool); //update the history

    public:
        Predictor(unsigned int, unsigned int, unsigned int, bool, unsigned long int, int );
        string makePrediction(string, string, string);
        void lastValue(unsigned long int , unsigned long int );
        void lastValueWithLCT(unsigned long int , unsigned long int );

        void pcWithMemAddress(string, string, unsigned long int);
        void fcmWithMaxOccurence(unsigned long int , unsigned long int );
        void updateLCT(unsigned long int , bool );

        void strideConstantNoLCT(unsigned long int , unsigned long int );
        void strideConstantWithLCT(unsigned long int , unsigned long int );
        void strideLearnNoLCT(unsigned long int , unsigned long int );
        void strideLearnWithLCT(unsigned long int , unsigned long int );
        void tournamentPredictor(unsigned long int , unsigned long int );
        void tournamentWithLCTPredictor(unsigned long int, unsigned long int);
        void printStats();
};

#endif
