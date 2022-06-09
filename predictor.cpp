#include "predictor.h"
#include <sstream>
#include <iomanip>
#include <cmath>
Predictor::Predictor(unsigned int m, unsigned int n, unsigned int addrLength, bool debug, unsigned long int s, int p){
    this->historyBits = m; // Size of global history
    this->globalHistory = 0; 
    this->addrBits = addrLength;
    this->debug = debug;
    this->correct = 0;
    this->total = 0;
    this->fileLength = 0;
    this->n=n;
    this->stride=s;
    this->strideOld=0;
    this->a = 1;
    this->c=0;
    this-> choice = p;
    this->tournament = 3;
    LCTrows=pow(2,addrLength);
    LCTcolumns= m;
    this->LCT = new unsigned long int*[LCTrows];
    this->LVPT = new unsigned long int*[LCTrows];
    this->LRUCount = new unsigned long int*[LCTrows];

    for(unsigned long int i=0;i<LCTrows;i++){
        LCT[i]=new unsigned long int[LCTcolumns];
        LVPT[i]=new unsigned long int[LCTcolumns];
        LRUCount[i] = new unsigned long int[LCTcolumns];

        for (unsigned long int j = 0; j < LCTcolumns; j++) {
            LCT[i][j] = 0;
            LVPT[i][j] = 0;
            LRUCount[i][j] = 0;
        }
    }

    printf("Address length: %u\n", this->addrBits);
    printf("Saturating counter: %u bit(s)\n", this->n);
    printf("Stride: %u\n", this->stride);
    printf("Local history instances: %u\n", this->historyBits);
    printf("Debug mode: %u\n", this->debug);
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

    this->fileLength++;
    if(choice==0){      
        lastValue(pcAddress, LoadValue);
    }
    else if(choice==1){
        lastValueWithLCT(pcAddress, LoadValue);
    } 
    else if(choice==2){
        strideConstantNoLCT(pcAddress, LoadValue);
    }
    else if(choice==3){
        strideConstantWithLCT(pcAddress, LoadValue);
    }
    else if(choice==4){
        strideLearnNoLCT(pcAddress, LoadValue);
    }
    else if(choice==5){
        strideLearnWithLCT(pcAddress, LoadValue);
    }
    else if (choice == 6) { // pc+mem address
        pcWithMemAddress(pc, mem, LoadValue);
    }
    else if (choice == 7) { // pc+mem address
        fcmWithMaxOccurence(pcAddress, LoadValue);
    }
    else if(choice == 8){
        tournamentPredictor(pcAddress, LoadValue);
    }
    else if(choice == 9){
        tournamentWithLCTPredictor(pcAddress, LoadValue);
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
    
    bool expected = false;

    if(LVPT[pcAddress][0] != LoadValue){
        LVPT[pcAddress][0] = LoadValue;
    }
    else{
        expected = true;
    }

    if(this->n==2){
        if(LCT[pcAddress][0] == 2 || LCT[pcAddress][0] == 3) {
            this->total++;
            if(expected){
                this->correct++;
            }
        }   
    }
    else{
        if(LCT[pcAddress][0] == 1){
            this->total++;
            if(expected){
                this->correct++;
            }
        }
    }
    updateLCT(pcAddress, expected);
    
}


void Predictor::fcmWithMaxOccurence(unsigned long int pcAddress, unsigned long int LoadValue){
    

    //predict max occur
    unsigned int m = historyBits;

    unsigned long int max = LRUCount[pcAddress][0];
    unsigned long int maxIndex = 0;
    unsigned long int min = LRUCount[pcAddress][0];
    unsigned long int minIndex = 0;
    for(unsigned long int j=1;j< m;j++){
        if(LRUCount[pcAddress][j] > max){
            max = LRUCount[pcAddress][j];
            maxIndex = j;
        }
        if(LRUCount[pcAddress][j] < min){
            min = LRUCount[pcAddress][j];
            minIndex = j;
        }
    }



    long unsigned int predictedValue = LVPT[pcAddress][maxIndex];
    bool expected = false;
    if(predictedValue == LoadValue) expected = true;

    if(this->n==2){
        if(LCT[pcAddress][0] == 2 || LCT[pcAddress][0] == 3) {
            this->total++;
            if(expected){
                this->correct++;
            }
        }   
    }
    else{
        if(LCT[pcAddress][0] == 1){
            this->total++;
            if(expected){
                this->correct++;
            }
        }
    }
    

    updateLCT(pcAddress, expected);
    bool found = false;
    for(unsigned long int j=1;j< m;j++){
        if(LVPT[pcAddress][j] == LoadValue){
            LRUCount[pcAddress][j]++;
            found = true;
            break;
        }
    }

    if(!found){
        LVPT[pcAddress][minIndex] = LoadValue;
        LRUCount[pcAddress][minIndex] = 1;
    }

}

void Predictor::updateLCT(unsigned long int address, bool expected){
    if(this->n == 2){
        if((this->LCT[address][0]==0) && (expected==true)) this->LCT[address][0]=1;
        else if((this->LCT[address][0]==3) && (expected==false)) this->LCT[address][0]=2;
        else if((this->LCT[address][0]==1) || (this->LCT[address][0]==2)){
            if(expected==false) this->LCT[address][0]=0;
            else this->LCT[address][0]=3;
        }       
    }
    else{
        if(expected) this->LCT[address][0] = 1;
        else this->LCT[address][0] = 0;
    }
    
}


void Predictor::pcWithMemAddress(string pc, string mem, unsigned long int LoadValue) {
    pc.erase(0, pc.length() - (this->addrBits/2));
    mem.erase(0, mem.length() - (this->addrBits/2));
    string index = pc + mem;
    unsigned long int indexAddress = truncateAddress(hexToInt(index));

    if (this->n == 2) {
        if (LCT[indexAddress][0] == 2 || LCT[indexAddress][0] == 3) {
            this->total++;
        }
        if (LVPT[indexAddress][0] != LoadValue) {
            LVPT[indexAddress][0] = LoadValue;
            // update historybits
            if (LCT[indexAddress][0] == 1) {
                LCT[indexAddress][0] = 0;
            } else if (LCT[indexAddress][0] == 2) {
                LCT[indexAddress][0] = 0;
            } else if (LCT[indexAddress][0] == 3) { // case for 3 
                LCT[indexAddress][0] = 2;
            } else {
                LCT[indexAddress][0] = 0;
            }
        } else {
            if (LCT[indexAddress][0] == 2 || LCT[indexAddress][0] == 3) {
                this->correct++;
            }
            // update historybits
            if (LCT[indexAddress][0] == 0) {
                LCT[indexAddress][0] = 1;
            } else if (LCT[indexAddress][0] == 1) {
                LCT[indexAddress][0] = 3;
            } else if (LCT[indexAddress][0] == 2) {
                LCT[indexAddress][0] = 3;
            } else { // case for 3 or others
                LCT[indexAddress][0] = 3;
            }
        }
    } else { // default case
        if (LCT[indexAddress][0] == 1) {
            this->total++;
        }

        if (LVPT[indexAddress][0]!=LoadValue){
            LVPT[indexAddress][0] = LoadValue;
            LCT[indexAddress][0] = 0;
        } else {
            if (LCT[indexAddress][0] == 1) {
                this->correct++;
            }
            LCT[indexAddress][0]=1;
        }
    }
}

void Predictor::strideConstantNoLCT(unsigned long int pcAddress, unsigned long int LoadValue)
{
    unsigned long int predicted = LVPT[pcAddress][0] + this->stride;
    if(predicted!=LoadValue){
        this->stride = LoadValue - LVPT[pcAddress][0];
    }
    else{
        this->correct+=1;
    }
    LVPT[pcAddress][0]=LoadValue;
    this->total+=1;
}


void Predictor::strideConstantWithLCT(unsigned long int pcAddress, unsigned long int LoadValue)
{
    bool expected = false;
    
    unsigned long int predicted = LVPT[pcAddress][0] + this->stride;

    if(predicted!=LoadValue){
        this->stride = LoadValue - LVPT[pcAddress][0];
    }
    else{
        expected = true;
    }


    LVPT[pcAddress][0]=LoadValue;  

    if(this->n==2){
        if(LCT[pcAddress][0] == 2 || LCT[pcAddress][0] == 3) {
            this->total++;
            if(expected){
                this->correct++;
            }
        }   
    }
    else{
        if(LCT[pcAddress][0] == 1){
            this->total++;
            if(expected){
                this->correct++;
            }
        }
    }
    updateLCT(pcAddress, expected); 
}


void Predictor::strideLearnNoLCT(unsigned long int pcAddress, unsigned long int LoadValue)
{
    unsigned long int strideNew = (unsigned long int) (this->a * this->stride + this->c);
    unsigned long int predicted = LVPT[pcAddress][historyBits] + strideNew;
    if(predicted!=LoadValue){
        strideNew = LoadValue - LVPT[pcAddress][historyBits];
        if(this->stride != this->strideOld){
            this->a = (strideNew - this->stride) / (this->stride - this->strideOld);
            this->c = strideNew - a * this->stride;
        }
        else{
            this->a =1;
            this->c =0;
        }
        this->strideOld = this->stride;
        this->stride = strideNew;     
    }
    else{
        this->correct+=1;
        
    }
    LVPT[pcAddress][historyBits]=LoadValue;
    this->total+=1;
}

void Predictor::strideLearnWithLCT(unsigned long int pcAddress, unsigned long int LoadValue)
{
    bool expected = false; 

    unsigned long int strideNew = (unsigned long int) (this->a * this->stride + this->c);
    unsigned long int predicted = LVPT[pcAddress][0] + strideNew;
    if(predicted!=LoadValue){
        strideNew = LoadValue - LVPT[pcAddress][0];
        if(this->stride != this->strideOld){
            this->a = (strideNew - this->stride) / (this->stride - this->strideOld);
            this->c = strideNew - a * this->stride;
        }
        else{
            this->a =1;
            this->c =0;
        }
        this->strideOld = this->stride;
        this->stride = strideNew;     
    }
    else{
        expected = true;
    }
    LVPT[pcAddress][0]=LoadValue;

    if(this->n==2){
        if(LCT[pcAddress][0] == 2 || LCT[pcAddress][0] == 3) {
            this->total++;
            if(expected){
                this->correct++;
            }
        }   
    }
    else{
        if(LCT[pcAddress][0] == 1){
            this->total++;
            if(expected){
                this->correct++;
            }
        }
    }
    updateLCT(pcAddress, expected); 
}

void Predictor::tournamentPredictor(unsigned long int pcAddress, unsigned long int LoadValue){
    //predictor 1: LastInstanceWithoutLCT, predictor 2: LinearLCTwithStride
    //first get predicted value from predictor-1

    unsigned long int predicted2 = LVPT[pcAddress][0];

    unsigned long int strideNew = (unsigned long int) (this->a * this->stride + this->c);
    unsigned long int predicted1 = LVPT[pcAddress][0] + strideNew;

    if(tournament == 0){
        strideLearnNoLCT(pcAddress, LoadValue);
        if(predicted1!=LoadValue)   tournament=1;
    }
    else if(tournament == 3){
        lastValue(pcAddress, LoadValue);

        if(predicted1!=LoadValue){
            strideNew = LoadValue - LVPT[pcAddress][0];
            if(this->stride != this->strideOld){
                this->a = (strideNew - this->stride) / (this->stride - this->strideOld);
                this->c = strideNew - a * this->stride;
            }
            else{
                this->a =1;
                this->c =0;
            }
            this->strideOld = this->stride;
            this->stride = strideNew;     

        }
        
        if(predicted2!=LoadValue)   tournament=2;
    }
    else if(tournament == 1){
        strideLearnNoLCT(pcAddress, LoadValue);

        if(predicted1!=LoadValue)   tournament=2;
        else    tournament = 0;
    }
    else{
        lastValue(pcAddress, LoadValue);

        if(predicted1!=LoadValue){
            strideNew = LoadValue - LVPT[pcAddress][0];
            if(this->stride != this->strideOld){
                this->a = (strideNew - this->stride) / (this->stride - this->strideOld);
                this->c = strideNew - a * this->stride;
            }
            else{
                this->a =1;
                this->c =0;
            }
            this->strideOld = this->stride;
            this->stride = strideNew;     

        }

        if(predicted2!=LoadValue)   tournament=1;
        else    tournament=3;
    }


}

void Predictor::tournamentWithLCTPredictor(unsigned long int pcAddress, unsigned long int LoadValue){
    //predictor 1: LastInstanceWithoutLCT, predictor 2: LinearLCTwithStride
    //first get predicted value from predictor-1

    unsigned long int predicted2 = LVPT[pcAddress][0];

    unsigned long int strideNew = (unsigned long int) (this->a * this->stride + this->c);
    unsigned long int predicted1 = LVPT[pcAddress][0] + strideNew;
    bool predict = true;

    if(predict){
        if(tournament == 0){
            strideLearnWithLCT(pcAddress, LoadValue);
            if(predicted1!=LoadValue)   tournament=1;
        }
        else if(tournament == 3){
            lastValueWithLCT(pcAddress, LoadValue);

            if(predicted1!=LoadValue){
                strideNew = LoadValue - LVPT[pcAddress][0];
                if(this->stride != this->strideOld){
                    this->a = (strideNew - this->stride) / (this->stride - this->strideOld);
                    this->c = strideNew - a * this->stride;
                }
                else{
                    this->a =1;
                    this->c =0;
                }
                this->strideOld = this->stride;
                this->stride = strideNew;     

            }
            
            if(predicted2!=LoadValue)   tournament=2;
        }
        else if(tournament == 1){
            strideLearnWithLCT(pcAddress, LoadValue);

            if(predicted1!=LoadValue)   tournament=2;
            else    tournament = 0;
        }
        else{
            lastValueWithLCT(pcAddress, LoadValue);

            if(predicted1!=LoadValue){
                strideNew = LoadValue - LVPT[pcAddress][0];
                if(this->stride != this->strideOld){
                    this->a = (strideNew - this->stride) / (this->stride - this->strideOld);
                    this->c = strideNew - a * this->stride;
                }
                else{
                    this->a =1;
                    this->c =0;
                }
                this->strideOld = this->stride;
                this->stride = strideNew;     

            }

            if(predicted2!=LoadValue)   tournament=1;
            else    tournament=3;
        } 
    }


}
/*
 * Print out branch predictor statistics
 */
void Predictor::printStats(){ 
    printf("------------------------------------------\n"); 
    if(total == 0){
        printf("Error, Cannot get rate \n");
    }
    else{
        //printf("Misclassification rate: %.2f%%\n", 100.00 * (1 - (double)(this->correct)/this->total));
        printf("correct: %d\n", this->correct);
        printf("Predicted: %d\n",this->total);
        
        printf("Correct percentage: %.2f%%\n", 100.00 * ((double) this->correct/this->total));
        printf("Predicted percentage: %.2f%%\n", 100.00 * ( (double) this->total/this->fileLength));
    } 
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
