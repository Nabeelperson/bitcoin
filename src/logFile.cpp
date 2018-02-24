//Custom log file system written to monitor the behaviour of
//compact blocks, getblocktxn, and blocktxn messages on a bitcoin node
//Soft forks created by two blocks coming in are also recorded

#include "logFile.h"

// TODO: remove use of namespace in future
using namespace std;

bool debug = false;
const static string nodeID = "003";
const static string directory = "/home/an4s/.bitcoin/expLogFiles/";

string createTimeStamp()
{
    time_t currTime;
    struct tm* timeStamp;
    string timeString;

    time(&currTime); //returns seconds since epoch
    timeStamp = localtime(&currTime); //converts seconds to tm struct
    timeString = asctime(timeStamp); //converts tm struct to readable timestamp string
    timeString.back() = ' '; //replaces newline with space character
    return timeString + ": ";
}

int logFile(CBlockHeaderAndShortTxIDs &Cblock, string fileName)
{
    static int inc = 0; //file increment
    string timeString = createTimeStamp();
    if(fileName == "") fileName = directory + "logNode" + nodeID + ".txt";
    else fileName = directory + fileName;
    string compactBlock = directory + to_string(inc) + "_cmpctblock.txt";
    vector<uint64_t> txid;
    ofstream fnOut;
    ofstream fnCmpct;

    fnOut.open(fileName, ofstream::app);
    fnCmpct.open(compactBlock, ofstream::out);

    fnOut << timeString << "CMPCTRECIVED - compact block recived" << endl;
    txid = Cblock.getTXID();

    for(unsigned int i = 0; i < txid.size(); i++)
    {
        fnCmpct << txid[i] << endl;
    }

    fnOut << createTimeStamp() << "CMPCTSAVED - " << compactBlock << " file created" << endl;

    if(debug){
        cout << "inc: " << inc << endl;
        cout << "fileName: " << fileName << " --- cmpctblock file: " << compactBlock << endl;
        cout << timeString << "CMPCTRECIVED - compact block recived" << endl;
    }

    inc++;
    fnCmpct.close();
    fnOut.close();

    return inc - 1;
}

void logFile(BlockTransactionsRequest &req, int inc,string fileName)
{
    string timeString = createTimeStamp();
    if(fileName == "") fileName = directory + "logNode" + nodeID + ".txt";
    else fileName = directory + fileName;
    string reqFile = directory + to_string(inc) + "_getblocktxn.txt";
    vector<uint64_t> txid;
    ofstream fnOut;
    ofstream fnReq;

    fnOut.open(fileName, ofstream::app);
    fnReq.open(reqFile, ofstream::out);

    fnOut << timeString << "FAILCMPCT - getblocktxn message sent for cmpctblock #" << inc << endl;
    fnOut << timeString << "REQSENT - cmpctblock #" << inc << " is missing " << req.indexes.size() << " tx"<< endl;

    fnReq << timeString << "indexes requested for missing tx from cmpctblock #" << inc << endl;
    for(unsigned int i = 0; i < req.indexes.size(); i++)
    {
        fnReq << req.indexes[i] << endl;
    }

    fnOut << timeString << "REQSAVED -  " << reqFile << " file created" << endl;

    if(debug){
        cout << "logFile for block req called" << endl;
        cout << "filename: " << fileName << " --- reqFile: " << reqFile << endl;
        cout << timeString << "REQSAVED -  " << reqFile << " file created" << endl;
    }


    fnOut.close();
    fnReq.close();
}

void logFile(string info, string fileName)
{
	if(info == "mempool")
	{
		dumpMemPool(fileName);
		return;
	}
    string timeString = createTimeStamp();
    if(fileName == "") fileName = directory + "logNode" + nodeID + ".txt";
    else fileName = directory + fileName;
    ofstream fnOut;
    fnOut.open(fileName,ofstream::app);

    fnOut << timeString << info << endl; //Thu Aug 10 11:31:32 2017\n is printed

    if(debug){
        if(!fnOut.is_open()) cout << "fnOut failed" << endl;
        cout << "logfile for string " << endl;
        cout << "fileName:" << fileName << endl;
        cout << timeString << info << endl;
    }

    fnOut.close();
}

void logFile(vector<CInv> vInv, string fileName)
{
    static int count = 0;
    string timeString = createTimeStamp();
    if(fileName == "") fileName = directory + "logNode" + nodeID + ".txt";
    else fileName = directory + fileName;
    string vecFile = directory + to_string(count) + "_vecFile.txt";
    ofstream fnOut;
    ofstream fnVec;
    fnOut.open(fileName,ofstream::app);
    fnVec.open(vecFile, ofstream::out);


    fnOut << timeString << "VECGEN --- generated vector of tx to sync" << endl;
    fnVec << timeString << to_string(count) << endl;

    for(unsigned int  ii = 0; ii < vInv.size(); ii++)
    {
        fnVec << vInv[ii].ToString() << endl; //protocol.* file contains CInc class
    }

    fnOut << timeString << "VECSAVED --- saved file of tx vector: " << vecFile << endl;

    fnOut.close();
    fnVec.close();

    count++;
}

void dumpMemPool(string fileName)
{
	static int count = 0;
	string timeString = createTimeStamp();
	if(fileName == "") fileName = directory + "logNode" + nodeID + ".txt";
	else fileName = directory + fileName;
	string mempoolFile = directory + to_string(count) + "_mempoolFile.txt";
	string sysCmd;
	ofstream fnOut;
	fnOut.open(fileName,ofstream::app);

	fnOut << timeString << "DMPMEMPOOL --- Dumping mempool to file: " << mempoolFile << endl;
	sysCmd = "bitcoin-cli getmempoolinfo > " + mempoolFile;
	(void)system(sysCmd.c_str());
	sysCmd = "bitcoin-cli getrawmempool >> " + mempoolFile;
	(void)system(sysCmd.c_str());

	fnOut.close();
	count++;
}
