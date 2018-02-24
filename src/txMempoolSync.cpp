//Written by Nabeel Younis in the NISLab at Boston Universty on 17/11/2017
//PI: Professor Trachtenberg of BU ENG ECE

#include "txMempoolSync.h"
//#include "logFile.h"
#include <vector>
#include "validation.h"


//using namepsace std;

indexed_transaction_set syncTxIndex;
const int mintxsync = 1000;


std::vector<CInv> generateVInv(){

    //logFile("Inside function");
    LOCK(cs_main); //would need to create the cs_main object first, calling this in the net_processing.cpp code
    std::vector<CInv> vInv;
    MPiter it = mempool.mapTx.get<ancestor_score>().begin();
    int txInMemPool = mempool.mapTx.get<ancestor_score>().size();
    int txToSync = txInMemPool * 0.1;

    if(txInMemPool < mintxsync) txToSync = txInMemPool;
    else if(mintxsync > txToSync) txToSync = mintxsync;

    logFile("TXCOUNT --- tx in mempool: " + to_string(txInMemPool) + " --- tx sync count: " + to_string(txToSync));
    for(int ii = 0; ii < txToSync; ii++)
    {
    	//logFile("..." + to_string(ii));
    	if (it == syncTxIndex.get<ancestor_score>().end())
    	{
    		//logFile("...end reached");
    		break;
    	}
    	//logFile("...trying lock");
    	//LOCK(cs_main);
        CInv inv(MSG_TX, it->GetTx().GetHash());
        //logFile("......hash received");
        vInv.push_back(inv);
        //logFile("......hash pushed");
        ++it;
        //std::advance(it, 1);
        //logFile("......iterator incremented");
    }
    //logFile("Exiting loop");
    logFile("mempool");
    logFile(vInv);

    return vInv;
}
