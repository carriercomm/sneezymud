//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


#ifndef __ACCOUNT_H
#define __ACCOUNT_H

extern unsigned int account_number;
extern unsigned int player_count;
extern unsigned int active_player;

class TAccount
{
  public:
    byte status;
    char email[80];
    char passwd[11];
    char name[10];
    long birth;
    long login;
    termTypeT term;
    Descriptor *desc;
    byte time_adjust;
    unsigned int flags;

    TAccount();
    TAccount(const TAccount &a);
    TAccount & operator=(const TAccount &a);
    ~TAccount();
};

// list of account flags
const unsigned int ACCOUNT_BOSS     = (1<<0);
const unsigned int ACCOUNT_IMMORTAL = (1<<1);
const unsigned int ACCOUNT_BANISHED = (1<<2);
const unsigned int ACCOUNT_EMAIL    = (1<<3);
const unsigned int ACCOUNT_MSP      = (1<<4);
const unsigned int ACCOUNT_ALLOW_DOUBLECLASS      = (1<<5);
const unsigned int ACCOUNT_ALLOW_TRIPLECLASS      = (1<<6);


// This file is saved as "account".  Do not alter structure size
class accountFile {
  public:
    char email[80];
    char passwd[11];
    char name[10];
    long birth;
    byte term;
    byte time_adjust;
    unsigned int flags;
};
#endif
