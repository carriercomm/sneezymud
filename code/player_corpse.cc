//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: player_corpse.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#include "stdsneezy.h"

int TPCorpse::checkOnLists()
{
  return on_lists;
}

void TPCorpse::togOnCorpseListsOn()
{
  on_lists = TRUE;
}

void TPCorpse::togOnCorpseListsOff()
{
  on_lists = 0;
}

void TPCorpse::setRoomNum(int n)
{
  corpse_in_room = n;
}

int TPCorpse::getRoomNum() const
{
  return corpse_in_room;
}

void TPCorpse::setNumInRoom(int n)
{
  num_corpses_in_room = n;
}

int TPCorpse::getNumInRoom() const
{
  return num_corpses_in_room;
}

void TPCorpse::addToNumInRoom(int n)
{
  num_corpses_in_room += n;
}

void TPCorpse::setOwner(const string Name)
{
  fileName = Name;
}

const string & TPCorpse::getOwner() const
{
  return fileName;
}

void TPCorpse::clearOwner()
{
  fileName.erase();
}

void TPCorpse::setNext(TPCorpse *n)
{
  nextCorpse = n;
}

void TPCorpse::removeNext()
{
  nextCorpse = NULL;
}

TPCorpse * TPCorpse::getNext() const
{
  return nextCorpse;
}

void TPCorpse::setPrevious(TPCorpse *n)
{
  previousCorpse = n;
}

void TPCorpse::removePrevious()
{
  previousCorpse = NULL;
}

TPCorpse * TPCorpse::getPrevious() const
{
  return previousCorpse;
}

void TPCorpse::setNextGlobal(TPCorpse *n)
{
  nextGlobalCorpse = n;
}

void TPCorpse::removeGlobalNext()
{
  nextGlobalCorpse = NULL;
}

TPCorpse * TPCorpse::getGlobalNext() const
{
  return nextGlobalCorpse;
}

TPCorpse::TPCorpse() :
  TBaseCorpse(),
  on_lists(0),
  corpse_in_room(0),
  num_corpses_in_room(0),
  fileName(""),
  nextGlobalCorpse(NULL),
  nextCorpse(NULL),
  previousCorpse(NULL)
{
}

TPCorpse::TPCorpse(const TPCorpse &a) :
  TBaseCorpse(a),
  on_lists(a.on_lists),
  corpse_in_room(a.corpse_in_room),
  num_corpses_in_room(a.num_corpses_in_room),
  fileName(a.fileName),
  nextGlobalCorpse(a.nextGlobalCorpse),
  nextCorpse(a.nextCorpse),
  previousCorpse(a.previousCorpse)
{
}

TPCorpse & TPCorpse::operator=(const TPCorpse &a)
{
  if (this == &a) return *this;
  TBaseCorpse::operator=(a);
  on_lists = a.on_lists;
  corpse_in_room = a.corpse_in_room;
  num_corpses_in_room = a.num_corpses_in_room;
  fileName = a.fileName;
  nextGlobalCorpse = a.nextGlobalCorpse;
  nextCorpse = a.nextCorpse;
  previousCorpse = a.previousCorpse;
  return *this;
}

TPCorpse::~TPCorpse()
{
   if (getGlobalNext() || getRoomNum() || getPrevious() || getNext())
     removeCorpseFromList();
}

void TPCorpse::assignFourValues(int x1, int x2, int x3, int x4)
{
  TBaseCorpse::assignFourValues(x1, x2, x3,x4);

  setRoomNum(x4);
}

void TPCorpse::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  TBaseCorpse::getFourValues(x1, x2, x3,x4);

  *x4 = getRoomNum();
}

string TPCorpse::statObjInfo() const
{
  char buf[256];

  sprintf(buf, "Corpse Flags: %d, Corpse race: %d\n\r",
       getCorpseFlags(), getCorpseRace());
  sprintf(buf + strlen(buf), "Corpse Level: %d, Corpse Vnum: %d\n\r",
       getCorpseLevel(), getCorpseVnum());

  string a(buf);
  return a;
}

int TPCorpse::getMe(TBeing *ch, TThing *sub)
{
  // do baseclass stuff for recusivity
  int rc = TObj::getMe(ch, sub);
  if (rc)
    return rc;

  if (ch) {
    removeCorpseFromList();
    return TRUE;
  }
  if (sub) {
    vlogf(5,"Problem with a sub in corpse.getMe %s %s", (ch ? ch->getName() : ""), getName());
    removeCorpseFromList();
    return DELETE_THIS;
  }
  return TRUE;
}

