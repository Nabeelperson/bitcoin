//Created by Nabeel Younis <nyounis@bu.edu> on 08/10/2017
//log file header function used to log certain activity of Bitcoin Core
#ifndef LOGFILE_H
#define LOGFILE_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "blockencodings.h" // Compact block, getblocktxn, blocktxn, normal block
#include "protocol.h" //CInv
#include "validation.h" //for cs_main
#include "net.h" // CConnman

// get current user name for logfile
// https://stackoverflow.com/a/8953445
#include <pwd.h>
#include <unistd.h>

class Env
{
    public:
    static std::string getUserName()
    {
        register struct passwd *pw;
        register uid_t uid;

        uid = geteuid ();
        pw = getpwuid (uid);
        if (pw)
        {
                return std::string(pw->pw_name);
        }
        return std::string("");
    }
};

enum INVTYPE
{
    FALAFEL_SENT,
    FALAFEL_RECEIVED,
};

enum INVEVENT
{
    BEFORE,
    AFTER,
};

#define LOG_NEIGHBOR_ADDRESSES  0
#define ENABLE_FALAFEL_SYNC     0
#define FALAFEL_SENDER          0
#define FALAFEL_RECEIVER        0

#if !ENABLE_FALAFEL_SYNC && (FALAFEL_SENDER || FALAFEL_RECEIVER)
    #error "FalafelSync must be enabled"
#endif

#if ENABLE_FALAFEL_SYNC && !(FALAFEL_SENDER ^ FALAFEL_RECEIVER)
    #error "Must be only Falafel sender or receiver"
#endif

// function prototypes for different logging functions
bool initLogger();
bool initAddrLogger();
void AddrLoggerThread(CConnman* connman);
std::string createTimeStamp();
void logFile(std::string info, std::string fileName = ""); //logging a simple statement with timestamp
int  logFile(CBlockHeaderAndShortTxIDs &Cblock, std::string from, std::string fileName = "");//info from cmpctBlock
void logFile(BlockTransactionsRequest &req, int inc, std::string fileName = ""); //info from getblocktxn
int  logFile(std::vector <CInv> vInv, INVTYPE type = FALAFEL_SENT, std::string fileName = "");
void logFile(std::string info, INVTYPE type, INVEVENT = BEFORE, int counter = 0, std::string fileName = "");

#endif
