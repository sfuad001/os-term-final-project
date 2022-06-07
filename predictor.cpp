#include "predictor.h"
#include <sstream>
#include <iomanip>
#include <cmath>
Predictor::Predictor(unsigned int m, unsigned int n, unsigned int addrLength, bool debug, unsigned long int s){
    this->historyBits = m; // Size of global history
    this->globalHistory = 0; 
    this->addrBits = addrLength;
    this->debug = debug;
    this->correct = 0;
    this->total = 0;
    this->n=n;
    this->stride=s;
    LCTrows=pow(2,addrLength);
    LCTcolumns=pow(2,m);
    this->LCT = new unsigned long int*[LCTrows];
    this->LVPT = new unsigned long int*[LCTrows];
    for(unsigned long int i=0;i<LCTrows;i++){
        LCT[i]=new unsigned long int[LCTcolumns];
        LVPT[i]=new unsigned long int[LCTcolumns];
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
    pc.erase(0,2);
    unsigned long int pcAddress = truncateAddress(hexToInt(pc));
    unsigned long int memAddress = truncateAddress(hexToInt(mem));
    unsigned long int LoadValue = hexToInt(expectedLV);

    //printf("LoadValue: %u\n",LoadValue);
    //printf("PC string: %s, PC address: %lu, LoadValue: %lu\n", pc.c_str(), pcAddress, LoadValue);


    if(n==0){      
        lastValue(pcAddress, LoadValue);
    }
    else if(n==1){
        lastValueWithLCT(pcAddress, LoadValue);
    }
    else if(n==3){
        strideConstantNoLCT(pcAddress, LoadValue);
    }
    else if(n==4){
        strideConstantWithLCT(pcAddress, LoadValue);
    }



   // Currently, this simple branch predictor simulator simply takes 
    // the previous observed branch direction as the next prediction.
    // Predict branch based on last observed branch
    //bool predicted = globalHistory & 1; 
    return expectedLV;
}

void Predictor::lastValue(unsigned long int pcAddress, unsigned long int LoadValue)
{

    if(LVPT[pcAddress][historyBits]!=LoadValue){
        LVPT[pcAddress][historyBits]=LoadValue;

        LCT[pcAddress][historyBits]=0;
    }
    else{
        this->correct+=1;
        LCT[pcAddress][historyBits]=1;
    }
    this->total+=1;
}

void Predictor::lastValueWithLCT(unsigned long int pcAddress, unsigned long int LoadValue)
{
    if(LCT[pcAddress][historyBits] == 1)
        this->total+=1;


    if(LVPT[pcAddress][historyBits]!=LoadValue){
        LVPT[pcAddress][historyBits]=LoadValue;
        LCT[pcAddress][historyBits] = 0;
    }
    else{
        if(LCT[pcAddress][historyBits] == 1)
            this->correct+=1;

        LCT[pcAddress][historyBits]=1;
    }

    
}

void Predictor::strideConstantNoLCT(unsigned long int pcAddress, unsigned long int LoadValue)
{
    unsigned long int predicted = LVPT[pcAddress][historyBits] + this->stride;
    if(predicted!=LoadValue){
        this->stride = LoadValue - LVPT[pcAddress][historyBits];
    }
    else{
        this->correct+=1;
    }
    LVPT[pcAddress][historyBits]=LoadValue;
    this->total+=1;
}


void Predictor::strideConstantWithLCT(unsigned long int pcAddress, unsigned long int LoadValue)
{
    if(LCT[pcAddress][historyBits] == 1)
        this->total+=1;

    unsigned long int predicted = LVPT[pcAddress][historyBits] + this->stride;
    if(predicted!=LoadValue){
        this->stride = LoadValue - LVPT[pcAddress][historyBits];
    }
    else{
        if(LCT[pcAddress][historyBits] == 1)
            this->correct+=1;
        LCT[pcAddress][historyBits]=1;
    }

    LVPT[pcAddress][historyBits]=LoadValue;   
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
unsigned long int Predictor::hexToInt(string input){
    istringstream converter(input);
    unsigned long int result;
    converter >> std::hex >> result; 
    return result;
}

/*
 * Truncate Address to specified number of address bits
 */
unsigned long int Predictor::truncateAddress(unsigned long int input){
    unsigned long int mask = (1 << this->addrBits) - 1;
    unsigned long int result = input & mask;

    return result;
}

