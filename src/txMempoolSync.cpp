//Written by Nabeel Younis in the NISLab at Boston Universty on 17/11/2017
//PI: Professor Trachtenberg of BU ENG ECE

#include "txMempoolSync.h"
//#include "logFile.h"
#include <vector>


//using namepsace std;

indexed_transaction_set syncTxIndex;
int txToSync = 4000;

std::vector<CInv> generateVInv(){

    //LOCK(cs_main); //would need to create the cs_main object first, calling this in the net_processing.cpp code
    std::vector<CInv> vInv;
    MPiter it = syncTxIndex.get<ancestor_score>().begin();

    for(int ii = 0; ii < txToSync; ii++)
    {
        CInv inv(MSG_TX, it->GetTx().GetHash());
        vInv.push_back(inv);
        it++;
    }

    //logFile("VECGEN - naive sync vector generated");

    return vInv;
}