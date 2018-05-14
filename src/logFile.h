//Created by Nabeel Younis <nyounis@bu.edu> on 08/10/2017
//log file header function used to log certain activity of Bitcoin Core
#ifndef LOGFILE_H
#define LOGFILE_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "blockencodings.h" //Comacpt block, getblocktxn, blocktxn, normal block
#include "protocol.h" //CInv
#include "validation.h" //for cs_main

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

// function prototypes for different logging functions
std::string createTimeStamp();
void logFile(std::string info, std::string fileName = ""); //logging a simple statement with timestamp
int  logFile(CBlockHeaderAndShortTxIDs &Cblock, std::string fileName = "");//info from cmpctBlock
void logFile(BlockTransactionsRequest &req, int inc, std::string fileName = ""); //info from getblocktxn
void logFile(std::vector <CInv> vInv, std::string fileName = "" );
void dumpMemPool(std::string fileName = "");

#endif
