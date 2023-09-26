#ifndef __BRANCH_PREDICTOR_HPP__
#define __BRANCH_PREDICTOR_HPP__
#include <cstring>
#include <vector>
#include <bitset>
using namespace std;
#include <iostream>
#include <cmath>
#include <fstream>
#include <cassert>

struct BranchPredictor {
    virtual bool predict(uint32_t pc) = 0;
    virtual void update(uint32_t pc, bool taken) = 0;
};


struct SaturatingBranchPredictor : public BranchPredictor {
    vector<bitset<2>> table;
    SaturatingBranchPredictor(int value) : table(1 << 14, value) {}

    bool predict(uint32_t pc) {
        // Get the index into the table based on the PC
        int index=(pc & 16383);

        // If the history for this branch is "strongly taken" or "taken", predict taken
        if (table[index] == bitset<2>("11") || table[index] == bitset<2>("10")) {
            return true;
        }
        // If the history for this branch is "strongly not taken" or "not taken", predict not taken
        else if (table[index] ==bitset<2>("00") || table[index] ==bitset<2>("01")) {
            return false;
        }
        // Otherwise, predict not taken (default behavior)
    }

    void update(uint32_t pc, bool taken) {
        // Get the index into the table based on the PC
        int index=(pc & 16383);
        // Update the history for this branch based on whether it was taken or not
        if (taken) {
            // If the history is "strongly not taken" or "not taken", change to "not taken"
            if (table[index] == bitset<2>("00"))  {
                table[index] =bitset<2>("01");
            }
            else if (table[index] == bitset<2>("01")){
                table[index] = bitset<2>("10");
            }
            // If the history is "taken" or "weakly taken", change to "weakly taken"
            else if (table[index] == bitset<2>("10") ) {
                table[index] = bitset<2>("11");
            }
            else if  (table[index] == bitset<2>("11")) {
                table[index] =bitset<2>("11");
            }
           
        }
        else {
             if (table[index] ==bitset<2>("00"))  {
                table[index] =bitset<2>("00");
            }
            else if (table[index] == bitset<2>("01")){
                table[index] = bitset<2>("00");
            }
            // If the history is "taken" or "weakly taken", change to "weakly taken"
            else if (table[index] ==bitset<2>("10") ) {
                table[index] = bitset<2>("01");
            }
            else if  (table[index] == bitset<2>("11")) {
                table[index] = bitset<2>("10");
            }
            
        }
    }
};


struct BHRBranchPredictor : public BranchPredictor {
    vector<bitset<2>> bhrTable;
    bitset<2> bhr;
    BHRBranchPredictor(int value) : bhrTable(1 << 2, value), bhr(value) {}
// bhrTable[bhr]
    bool predict(uint32_t pc) {
        // If the history for this branch is "strongly taken" or "taken", predict taken
        if (bhrTable[bhr.to_ulong()] == bitset<2>("11") || bhrTable[bhr.to_ulong()] == bitset<2>("10")) {
            return true;
        }
        // If the history for this branch is "strongly not taken" or "not taken", predict not taken
        else if (bhrTable[bhr.to_ulong()]==bitset<2>("00") || bhrTable[bhr.to_ulong()]==bitset<2>("01")) {
            return false;
        }
    }

    void update(uint32_t pc,bool taken) {
         if (taken) {
            // If the history is "strongly not taken" or "not taken", change to "not taken"
            if (bhrTable[bhr.to_ulong()] == bitset<2>("00"))  {
               bhrTable[bhr.to_ulong()]=bitset<2>("01");
            }
            else if (bhrTable[bhr.to_ulong()]== bitset<2>("01")){
                bhrTable[bhr.to_ulong()] = bitset<2>("10");
            }
            // If the history is "taken" or "weakly taken", change to "weakly taken"
            else if (bhrTable[bhr.to_ulong()]== bitset<2>("10") ) {
                bhrTable[bhr.to_ulong()]= bitset<2>("11");
            }
            else if  (bhrTable[bhr.to_ulong()]== bitset<2>("11")) {
                bhrTable[bhr.to_ulong()]=bitset<2>("11");
            }
           
        }
        else {
             if (bhrTable[bhr.to_ulong()]==bitset<2>("00"))  {
              bhrTable[bhr.to_ulong()]=bitset<2>("00");
            }
            else if (bhrTable[bhr.to_ulong()] == bitset<2>("01")){
                bhrTable[bhr.to_ulong()]= bitset<2>("00");
            }
            // If the history is "taken" or "weakly taken", change to "weakly taken"
            else if (bhrTable[bhr.to_ulong()]==bitset<2>("10") ) {
                bhrTable[bhr.to_ulong()]= bitset<2>("01");
            }
            else if  (bhrTable[bhr.to_ulong()]== bitset<2>("11")) {
              bhrTable[bhr.to_ulong()]= bitset<2>("10");
            }
            
        }
        if (taken){
                if( bhr.to_ulong()==0 || bhr.to_ulong()==2){
                            bhr= bitset<2>("01");
                }
                else{
                    bhr=bitset<2>("11");
                }
           }
           else{
                if (bhr.to_ulong()==0 ||bhr.to_ulong()==2){
                            bhr= bitset<2>("00");
                }
                else{
                    bhr=bitset<2>("10");
                }
           } 
           }
    };
    


#endif
struct SaturatingBHRBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> bhrTable;
    std::bitset<2> bhr;
    std::vector<std::bitset<2>> table;
    std::vector<std::bitset<2>> combination;
    SaturatingBHRBranchPredictor(int value, int size) : bhrTable(1 << 2, value), bhr(value), table(1 << 14, value), combination(size, value) {
        assert(size <= (1 << 16));
    }
bool predict(uint32_t pc) {
    int pc_index=(pc & 16383);
    int table_index = ((bhr.to_ulong() << 14)^pc_index) % combination.size();
    bitset<2>  counter = combination[table_index];
    if (counter == bitset<2>("11") ||counter == bitset<2>("10")) {
            return true;
        }
        // If the history for this branch is "strongly not taken" or "not taken", predict not taken
        else if (counter==bitset<2>("00") || counter ==bitset<2>("01")) {
            return false;
    }
}

void update(uint32_t pc, bool taken) {
    
    int pc_index=(pc & 16383);
    int table_index = ((bhr.to_ulong() << 14) | pc_index) % combination.size();
    bitset<2>  counter = combination[table_index];
    if (taken) {
            // If the history is "strongly not taken" or "not taken", change to "not taken"
            if (counter == bitset<2>("00"))  {
                counter=bitset<2>("01");
            }
            else if (counter == bitset<2>("01")){
                counter = bitset<2>("10");
            }
            // If the history is "taken" or "weakly taken", change to "weakly taken"
            else if (counter== bitset<2>("10") ) {
                counter = bitset<2>("11");
            }
            else if  (counter == bitset<2>("11")) {
               counter =bitset<2>("11");
            }
           
        }
        else {
             // If the history is "strongly not taken" or "not taken", change to "not taken"
            if (counter == bitset<2>("00"))  {
                counter=bitset<2>("00");
            }
            else if (counter == bitset<2>("01")){
                counter = bitset<2>("00");
            }
            // If the history is "taken" or "weakly taken", change to "weakly taken"
            else if (counter== bitset<2>("10") ) {
                counter = bitset<2>("01");
            }
            else if  (counter == bitset<2>("11")) {
               counter =bitset<2>("10");
            }
            
        }
    combination[table_index]=counter;       
   
    bitset<2>  bhr_counter = bhrTable[bhr.to_ulong()];
     if (taken) {
            // If the history is "strongly not taken" or "not taken", change to "not taken"
            if (bhr_counter == bitset<2>("00"))  {
                bhr_counter=bitset<2>("01");
            }
            else if (bhr_counter == bitset<2>("01")){
                bhr_counter= bitset<2>("10");
            }
            // If the history is "taken" or "weakly taken", change to "weakly taken"
            else if (bhr_counter== bitset<2>("10") ) {
                bhr_counter= bitset<2>("11");
            }
            else if  (bhr_counter == bitset<2>("11")) {
               bhr_counter =bitset<2>("11");
            }
           
        }
    else {
             // If the history is "strongly not taken" or "not taken", change to "not taken"
            if (bhr_counter == bitset<2>("00"))  {
                bhr_counter=bitset<2>("00");
            }
            else if (bhr_counter == bitset<2>("01")){
                bhr_counter = bitset<2>("00");
            }
            // If the history is "taken" or "weakly taken", change to "weakly taken"
            else if (bhr_counter== bitset<2>("10") ) {
                bhr_counter= bitset<2>("01");
            }
            else if  (bhr_counter== bitset<2>("11")) {
               bhr_counter =bitset<2>("10");
            }
            
        }
    bhrTable[bhr.to_ulong()]=bhr_counter;
    if (taken){
                if( bhr.to_ulong()==0 || bhr.to_ulong()==2){
                            bhr= bitset<2>("01");
                }
                else{
                    bhr=bitset<2>("11");
               }
    }
    else{
                if (bhr.to_ulong()==0 ||bhr.to_ulong()==2){
                            bhr= bitset<2>("00");
                }
                else{
                    bhr=bitset<2>("10");
                }
    } 
}};