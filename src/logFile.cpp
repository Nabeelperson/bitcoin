//Custom log file system written to monitor the behaviour of
//compact blocks, getblocktxn, and blocktxn messages on a bitcoin node
//Soft forks created by two blocks coming in are also recorded

#include "logFile.h"
#include <chrono>

#define UNIX_TIMESTAMP \
    std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count())

bool debug = false;
Env env;
const static std::string nodeID = env.getUserName();
static std::string strDataDir;
static std::string directory;
static std::string invRXdir;
static std::string addrLoggerdir;
static int64_t addrLoggerTimeoutSecs = 1;

void dumpMemPool(std::string fileName = "", INVTYPE type = FALAFEL_SENT, INVEVENT event = BEFORE, int counter = 0);

bool initLogger()
{
    strDataDir = GetDataDir().string();
    directory  = strDataDir + "/expLogFiles/";
    invRXdir = directory + "/received/";
    
    boost::filesystem::path dir(directory);
    if(!(boost::filesystem::exists(dir)))
    {
        if(fPrintToConsole)
            std::cout << "Directory <" << dir << "> doesn't exist; creating directory" << std::endl;
        if(boost::filesystem::create_directory(dir))
        {
            if(fPrintToConsole)
                std::cout << "Directory <" << dir << "> created successfully" << std::endl;
        }
        else
        {
            if(fPrintToConsole)
                std::cout << "Directory <" << dir << "> not created" << std::endl;
            return false;
        }
    }

    boost::filesystem::path RX(invRXdir);
    if(!(boost::filesystem::exists(RX)))
    {
        if(fPrintToConsole)
            std::cout << "Directory <" << RX << "> doesn't exist; creating directory" << std::endl;
        if(boost::filesystem::create_directory(RX))
        {
            if(fPrintToConsole)
                std::cout << "Directory <" << RX << "> created successfully" << std::endl;
        }
        else
        {
            if(fPrintToConsole)
                std::cout << "Directory <" << RX << "> not created" << std::endl;
            return false;
        }
    }
    return true;
}

void AddrLoggerThread(CConnman* connman)
{
    while(true)
    {
        boost::this_thread::sleep_for(boost::chrono::seconds{addrLoggerTimeoutSecs});
        std::vector<CNodeStats> vstats;
        connman->GetNodeStats(vstats);
        std::string fileName = addrLoggerdir + UNIX_TIMESTAMP + ".txt";
        std::ofstream fnOut(fileName, std::ofstream::out);
        for (auto stat : vstats)
            fnOut << stat.addr.ToStringIP() << std::endl;
        fnOut.close();
    }
}

bool initAddrLogger()
{
    addrLoggerdir = directory + "/addrs/";

    boost::filesystem::path addr(addrLoggerdir);
    if(!(boost::filesystem::exists(addr)))
    {
        if(fPrintToConsole)
            std::cout << "Directory <" << addr << "> doesn't exist; creating directory" << std::endl;
        if(boost::filesystem::create_directory(addr))
        {
            if(fPrintToConsole)
                std::cout << "Directory <" << addr << "> created successfully" << std::endl;
        }
        else
        {
            if(fPrintToConsole)
                std::cout << "Directory <" << addr << "> not created" << std::endl;
            return false;
        }
    }
    return true;
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
    return timeString + UNIX_TIMESTAMP + " : ";
}

int logFile(CBlockHeaderAndShortTxIDs &Cblock, std::string from, std::string fileName)
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

    fnOut << timeString << "CMPCTRECIVED - compact block received from " << from << std::endl;
    fnOut << timeString << "CMPCTBLKHASH - " << Cblock.header.GetHash().ToString() << std::endl;
    txid = Cblock.getTXID();

    fnCmpct << Cblock.header.GetHash().ToString() << std::endl;

    for(unsigned int i = 0; i < txid.size(); i++)
    {
        fnCmpct << txid[i] << std::endl;
    }

    fnOut << createTimeStamp() << "CMPCTSAVED - " << compactBlock << " file created" << std::endl;

    if(debug){
        std::cout << "inc: " << inc << std::endl;
        std::cout << "fileName: " << fileName << " --- cmpctblock file: " << compactBlock << std::endl;
        std::cout << timeString << "CMPCTRECIVED - compact block received from " << from << std::endl;
    }

    inc++;
    fnCmpct.close();
    fnOut.close();

    dumpMemPool();

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

void logFile(std::string info, INVTYPE type, INVEVENT event, int counter, std::string fileName)
{
	if(info == "mempool")
	{
		dumpMemPool(fileName, type, event, counter);
		return;
	}
}

void logFile(std::string info, std::string fileName)
{
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

int logFile(std::vector<CInv> vInv, INVTYPE type, std::string fileName)
{
    static int count = 0;
    std::string timeString = createTimeStamp();
    if(fileName == "") fileName = directory + "logNode_" + nodeID + ".txt";
    else fileName = directory + fileName;
    std::ofstream fnOut;
    fnOut.open(fileName,std::ofstream::app);

    if(type == FALAFEL_SENT)
    {
        std::string vecFile = directory + std::to_string(count) + "_vecFile_invsent.txt";
        std::ofstream fnVec;
        fnVec.open(vecFile, std::ofstream::out);

        fnOut << timeString << "VECGEN --- generated std::vector of tx to sync" << std::endl;
        fnVec << timeString << std::to_string(vInv.size()) << std::endl;

        for(unsigned int  ii = 0; ii < vInv.size(); ii++)
        {
            fnVec << vInv[ii].ToString() << std::endl; //protocol.* file contains CInc class
        }

        fnOut << timeString << "VECSAVED --- saved file of tx std::vector: " << vecFile << std::endl;

        fnVec.close();
    }
    else if(type == FALAFEL_RECEIVED)
    {
        std::string vecFile = invRXdir + std::to_string(count) + "_vecFile_invreceived.txt";
        std::ofstream fnVec;
        fnVec.open(vecFile, std::ofstream::out);

        fnOut << timeString << "INVRX --- received inv" << std::endl;
        fnVec << timeString << std::to_string(vInv.size()) << std::endl;

        for(unsigned int  ii = 0; ii < vInv.size(); ii++)
        {
            fnVec << vInv[ii].ToString() << std::endl; //protocol.* file contains CInc class
        }

        fnOut << timeString << "INVSAVED --- received inv saved to: " << vecFile << std::endl;

        fnVec.close();
    }

    fnOut.close();

    return count++;
}

void dumpMemPool(std::string fileName, INVTYPE type, INVEVENT event, int counter)
{
    std::string timeString = createTimeStamp();
    if(fileName == "") fileName = directory + "logNode_" + nodeID + ".txt";
    else fileName = directory + fileName;
    std::ofstream fnOut;
    std::string sysCmd;
    std::string mempoolFile;
    fnOut.open(fileName,std::ofstream::app);
    if(type == FALAFEL_RECEIVED)
    {
        mempoolFile = invRXdir + std::to_string(counter) + ((event == BEFORE)? "_before" : "_after") + "_mempoolFile.txt";
        fnOut << timeString << "INV" << ((event == BEFORE) ? "B" : "A") << "DMPMEMPOOL --- Dumping mempool to file: " << mempoolFile << std::endl;
    }
    else
    {
	    static int count = 0;
	    mempoolFile = directory + std::to_string(count) + "_mempoolFile.txt";

	    fnOut << timeString << "DMPMEMPOOL --- Dumping mempool to file: " << mempoolFile << std::endl;
	    count++;
    }
    sysCmd = "bitcoin-cli getmempoolinfo > " + mempoolFile;
    (void)system(sysCmd.c_str());
    sysCmd = "bitcoin-cli getrawmempool >> " + mempoolFile;
    (void)system(sysCmd.c_str());

    fnOut.close();
}
