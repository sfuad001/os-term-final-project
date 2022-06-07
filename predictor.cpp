#include "predictor.h"
#include <sstream>
#include <iomanip>
#include <cmath>
Predictor::Predictor(unsigned int m, unsigned int n, unsigned int addrLength, bool debug){
    this->historyBits = m; // Size of global history
    this->globalHistory = 0; 
    this->addrBits = addrLength;
    this->debug = debug;
    this->correct = 0;
    this->total = 0;
    this->n=n;
    LCTrows=pow(2,addrLength);
    LCTcolumns=pow(2,m);
    this->LCT = new unsigned int*[LCTrows];
    this->LVPT = new unsigned int*[LCTrows];
    for(unsigned int i=0;i<LCTrows;i++){
        LCT[i]=new unsigned int[LCTcolumns];
        LVPT[i]=new unsigned int[LCTcolumns];
    }

    printf("BHT: %u-bit\n", n);
    printf("Address length: %u\n", this->addrBits);
    printf("History length: %u\n", this->historyBits);
    printf("Debug mode: %u\n", this->debug);
    //printf("BHT rows: %u\n", BHTrows);
    //printf("BHT columns: %u\n", BHTcolumns);
};


void Predictor::updateGlobalHistory(bool expected){
    globalHistory = globalHistory << 1;
    globalHistory = globalHistory | expected;
    unsigned int mask = (1 << this->historyBits) - 1;
    globalHistory = globalHistory & mask; 
}

string Predictor::makePrediction(string pc, string mem, string expectedLV){
    // Convert Hex address to integer address
    unsigned int pcAddress = truncateAddress(hexToInt(pc));
    unsigned int memAddress = truncateAddress(hexToInt(mem));
    unsigned int LoadValue = hexToInt(expectedLV);

    printf("LoadValue: %u\n",LoadValue);

    if(n==1){

    }
   // Currently, this simple branch predictor simulator simply takes 
    // the previous observed branch direction as the next prediction.
    // Predict branch based on last observed branch
    //bool predicted = globalHistory & 1; 
    return expectedLV;
}

/*
bool Predictor::makePrediction(string input, bool expected){
    // Convert Hex address to integer address
    unsigned int address = truncateAddress(hexToInt(input));

    // Currently, this simple branch predictor simulator simply takes 
    // the previous observed branch direction as the next prediction.
    // Predict branch based on last observed branch
    //bool predicted = globalHistory & 1; 
    int BHT_entry= BHT[address][globalHistory];
    bool predicted;
    BHTEntryCount[address][globalHistory]+=1;
    if(n==1){
        predicted=BHT_entry;
    }
    else{
        switch(BHT_entry){
            case 0:
            case 1:
                predicted=0;
                break;
            case 2:
            default:
                predicted=1;
                break;
        }
    }
    //update BHT
    if(n==1){
        BHT[address][globalHistory]=int(expected);
    }
    if(n==2){
        if(expected){
            switch(BHT_entry)
            {
                case 0:
                   BHT[address][globalHistory]=1;
                   break; 
                case 1:
                   BHT[address][globalHistory]=3;
                   break;   
                case 2:
                     BHT[address][globalHistory]=3;
                     break; 
            }
        }
        else{
          switch(BHT_entry)
            {
                case 1:
                   BHT[address][globalHistory]=0;
                   break;   
                case 2:
                     BHT[address][globalHistory]=0;
                     break; 
                case 3:
                    BHT[address][globalHistory]=2;
                    break; 
            }  
        }
    }
    // Update global history 
    if(historyBits!=0){
        updateGlobalHistory(expected);
    }
    

    // Update statistics
    if(predicted == expected){
        this->correct++;
    }
    this->total++;

    return predicted;
}
*/
/*
 * Print out branch predictor statistics
 */
void Predictor::printStats(){ 
    printf("------------------------------------------\n"); 
    if(total == 0){
        printf("Error, Cannot get rate \n");
    }
    else{
        printf("Misclassification rate: %.2f%%\n", 100.00 * (1 - (double)(this->correct)/this->total));
    } 
    
    printf("correct: %d\n", this->correct);
    printf("total: %d\n",this->total);

}

/*
 * DO NOT MODIFY BELOW THIS
 */


/*
 * Convert Hex string from trace file to integer address
 */
unsigned int Predictor::hexToInt(string input){
    istringstream converter(input);
    unsigned int result;
    converter >> std::hex >> result; 
    return result;
}

/*
 * Truncate Address to specified number of address bits
 */
unsigned int Predictor::truncateAddress(unsigned int input){
    unsigned int mask = (1 << this->addrBits) - 1;
    unsigned int result = input & mask;

    return result;
}

