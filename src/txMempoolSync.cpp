//Written by Nabeel Younis in the NISLab at Boston Universty on 17/11/2017
//PI: Professor Trachtenberg of BU ENG ECE

#include "txMempoolSync.h"
#include <vector>
#include "validation.h"

// variable to hold prioritized indexed set of transactions in mempool
indexed_transaction_set syncTxIndex;
// threshold of minimum number of transactions to sync
const int mintxsync = 1000;

uint256 genMagicHash()
{
    const char MAGIC_HASH_BYTES[4] = {'\xe1', '\xaf', '\xa1', '\x0f'};
    std::vector<unsigned char> MAGIC_HASH_VEC;
    for(int i = 0; i < 32; i++)
        MAGIC_HASH_VEC.push_back(MAGIC_HASH_BYTES[i % 4]);
    uint256 MAGIC_HASH = uint256(MAGIC_HASH_VEC);
    return MAGIC_HASH;
}

std::vector<CInv> generateVInv(){
    LOCK(cs_main);
    std::vector<CInv> vInv;
    MPiter it = mempool.mapTx.get<ancestor_score>().begin();
    int txInMemPool = mempool.mapTx.get<ancestor_score>().size();

    // decide how many transactions to sync
    // case 1: sync 10 percent of transactions in mempool
    int txToSync = txInMemPool * 0.1;
    // case 2: if number of transactions in mempool is too low, sync all
    //         transactions in mempool
    if(txInMemPool < mintxsync) txToSync = txInMemPool;
    // case 3: if 10 percent of transactions in mempool lower than threshold,
    // sync threshold number of transactions
    else if(mintxsync > txToSync) txToSync = mintxsync;

    // log to file information regarding count of transactions to sync
    logFile("TXCOUNT --- tx in mempool: " + std::to_string(txInMemPool) + 
                                " --- tx sync count: " + std::to_string(txToSync));

    CInv magichash(MSG_TX, genMagicHash());
    vInv.push_back(magichash);

    for(int ii = 0; ii < txToSync; ii++)
    {
    	// reached end of number of transactions to sync
    	if (it == syncTxIndex.get<ancestor_score>().end())
    	{
    		break;
    	}
        // create an inventory of hash of current transaction
        CInv inv(MSG_TX, it->GetTx().GetHash());
        // add transaction to inventory vector
        vInv.push_back(inv);
        ++it;
    }

    // write current state of mempool to a file
    // logFile("mempool");
    // write trasactions to sync to a file
    logFile(vInv, FALAFEL_SENT);

    // return vector containing transactions to sync
    return vInv;
}
