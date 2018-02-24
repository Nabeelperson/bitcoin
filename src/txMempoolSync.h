//Written by Nabeel Younis in the NISLab at Boston Universty on 17/11/2017
//PI: Professor Trachtenberg of BU ENG ECE

#ifndef BITCOIN_TXMEMPOOLSYNC_H
#define BITCOIN_TXMEMPOOLSYNC_H

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/ordered_index.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include <boost/multi_index/sequenced_index.hpp>
#include "logFile.h"
#include "netmessagemaker.h"
#include "txmempool.h"
#include "net.h"
//Guessing what I will need for this protocol
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>

//Copied over from txmempool.h line 451
//Included all the indexes for testing later to see which ones is the best to use
typedef boost::multi_index_container<
        CTxMemPoolEntry,
        boost::multi_index::indexed_by<
                // sorted by txid
                boost::multi_index::hashed_unique<mempoolentry_txid, SaltedTxidHasher>,
                // sorted by fee rate
                boost::multi_index::ordered_non_unique<
                        boost::multi_index::tag<descendant_score>,
                        boost::multi_index::identity<CTxMemPoolEntry>,
                        CompareTxMemPoolEntryByDescendantScore
                >,
                // sorted by entry time
                boost::multi_index::ordered_non_unique<
                        boost::multi_index::tag<entry_time>,
                        boost::multi_index::identity<CTxMemPoolEntry>,
                        CompareTxMemPoolEntryByEntryTime
                >,
                // sorted by fee rate with ancestors
                boost::multi_index::ordered_non_unique<
                        boost::multi_index::tag<ancestor_score>,
                        boost::multi_index::identity<CTxMemPoolEntry>,
                        CompareTxMemPoolEntryByAncestorFee
                >
        >
> indexed_transaction_set;

typedef indexed_transaction_set::index<ancestor_score>::type::iterator MPiter; //indexer for ancestor_score
std::vector<CInv> generateVInv();

#endif //BITCOIN_TXMEMPOOLSYNC_H
