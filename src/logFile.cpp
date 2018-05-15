//Custom log file system written to monitor the behaviour of
//compact blocks, getblocktxn, and blocktxn messages on a bitcoin node
//Soft forks created by two blocks coming in are also recorded

#include "logFile.h"

bool debug = false;
Env env;
const static std::string nodeID = env.getUserName();
const static std::string directory = "/home/" + env.getUserName() + "/.bitcoin/expLogFiles/";

void initLogger()
{
    
    boost::filesystem::path dir(directory);
    if(!(boost::filesystem::exists(dir)))
    {
        if(debug)
            std::cout << "Directory <" << directory << "> doesn't exist; creating directory" << std::endl;
        if(boost::filesystem::create_directory(directory))
            if(debug)
                std::cout << "Directory created successfully" << std::endl;
    }
}

std::string createTimeStamp()
{
    time_t currTime;
    struct tm* timeStamp;
    std::string timeString;

    time(&currTime); //returns seconds since epoch
    timeStamp = localtime(&currTime); //converts seconds to tm struct
    timeString = asctime(timeStamp); //converts tm struct to readable timestamp string
    timeString.back() = ' '; //replaces newline with space character
    return timeString + ": ";
}

int logFile(CBlockHeaderAndShortTxIDs &Cblock, std::string fileName)
{
    static int inc = 0; //file increment
    std::string timeString = createTimeStamp();
    if(fileName == "") fileName = directory + "logNode_" + nodeID + ".txt";
    else fileName = directory + fileName;
    std::string compactBlock = directory + std::to_string(inc) + "_cmpctblock.txt";
    std::vector<uint64_t> txid;
    std::ofstream fnOut;
    std::ofstream fnCmpct;

    fnOut.open(fileName, std::ofstream::app);
    fnCmpct.open(compactBlock, std::ofstream::out);

    fnOut << timeString << "CMPCTRECIVED - compact block recived" << std::endl;
    txid = Cblock.getTXID();

    for(unsigned int i = 0; i < txid.size(); i++)
    {
        fnCmpct << txid[i] << std::endl;
    }

    fnOut << createTimeStamp() << "CMPCTSAVED - " << compactBlock << " file created" << std::endl;

    if(debug){
        std::cout << "inc: " << inc << std::endl;
        std::cout << "fileName: " << fileName << " --- cmpctblock file: " << compactBlock << std::endl;
        std::cout << timeString << "CMPCTRECIVED - compact block recived" << std::endl;
    }

    inc++;
    fnCmpct.close();
    fnOut.close();

    return inc - 1;
}

void logFile(BlockTransactionsRequest &req, int inc,std::string fileName)
{
    std::string timeString = createTimeStamp();
    if(fileName == "") fileName = directory + "logNode_" + nodeID + ".txt";
    else fileName = directory + fileName;
    std::string reqFile = directory + std::to_string(inc) + "_getblocktxn.txt";
    std::vector<uint64_t> txid;
    std::ofstream fnOut;
    std::ofstream fnReq;

    fnOut.open(fileName, std::ofstream::app);
    fnReq.open(reqFile, std::ofstream::out);

    fnOut << timeString << "FAILCMPCT - getblocktxn message sent for cmpctblock #" << inc << std::endl;
    fnOut << timeString << "REQSENT - cmpctblock #" << inc << " is missing " << req.indexes.size() << " tx"<< std::endl;

    fnReq << timeString << "indexes requested for missing tx from cmpctblock #" << inc << std::endl;
    for(unsigned int i = 0; i < req.indexes.size(); i++)
    {
        fnReq << req.indexes[i] << std::endl;
    }

    fnOut << timeString << "REQSAVED -  " << reqFile << " file created" << std::endl;

    if(debug){
        std::cout << "logFile for block req called" << std::endl;
        std::cout << "filename: " << fileName << " --- reqFile: " << reqFile << std::endl;
        std::cout << timeString << "REQSAVED -  " << reqFile << " file created" << std::endl;
    }


    fnOut.close();
    fnReq.close();
}

void logFile(std::string info, std::string fileName)
{
	if(info == "mempool")
	{
		dumpMemPool(fileName);
		return;
	}
    std::string timeString = createTimeStamp();
    if(fileName == "") fileName = directory + "logNode_" + nodeID + ".txt";
    else fileName = directory + fileName;
    std::ofstream fnOut;
    fnOut.open(fileName,std::ofstream::app);

    fnOut << timeString << info << std::endl; //Thu Aug 10 11:31:32 2017\n is printed

    if(debug){
        if(!fnOut.is_open()) std::cout << "fnOut failed" << std::endl;
        std::cout << "logfile for std::string " << std::endl;
        std::cout << "fileName:" << fileName << std::endl;
        std::cout << timeString << info << std::endl;
    }

    fnOut.close();
}

void logFile(std::vector<CInv> vInv, std::string fileName)
{
    static int count = 0;
    std::string timeString = createTimeStamp();
    if(fileName == "") fileName = directory + "logNode_" + nodeID + ".txt";
    else fileName = directory + fileName;
    std::string vecFile = directory + std::to_string(count) + "_vecFile.txt";
    std::ofstream fnOut;
    std::ofstream fnVec;
    fnOut.open(fileName,std::ofstream::app);
    fnVec.open(vecFile, std::ofstream::out);


    fnOut << timeString << "VECGEN --- generated std::vector of tx to sync" << std::endl;
    fnVec << timeString << std::to_string(count) << std::endl;

    for(unsigned int  ii = 0; ii < vInv.size(); ii++)
    {
        fnVec << vInv[ii].ToString() << std::endl; //protocol.* file contains CInc class
    }

    fnOut << timeString << "VECSAVED --- saved file of tx std::vector: " << vecFile << std::endl;

    fnOut.close();
    fnVec.close();

    count++;
}

void dumpMemPool(std::string fileName)
{
	static int count = 0;
	std::string timeString = createTimeStamp();
	if(fileName == "") fileName = directory + "logNode_" + nodeID + ".txt";
	else fileName = directory + fileName;
	std::string mempoolFile = directory + std::to_string(count) + "_mempoolFile.txt";
	std::string sysCmd;
	std::ofstream fnOut;
	fnOut.open(fileName,std::ofstream::app);

	fnOut << timeString << "DMPMEMPOOL --- Dumping mempool to file: " << mempoolFile << std::endl;
	sysCmd = "bitcoin-cli getmempoolinfo > " + mempoolFile;
	(void)system(sysCmd.c_str());
	sysCmd = "bitcoin-cli getrawmempool >> " + mempoolFile;
	(void)system(sysCmd.c_str());

	fnOut.close();
	count++;
}
