//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: key.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


// key.cc

#include "stdsneezy.h"

TKey::TKey() :
  TObj()
{
}

TKey::TKey(const TKey &a)
   : TObj(a)
{
}

TKey & TKey::operator=(const TKey &a)
{
  if (this == &a) return *this;
  TObj::operator=(a);
  return *this;
}

TKey::~TKey()
{
}

void TKey::assignFourValues(int, int, int, int)
{
}

void TKey::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1 = 0;
  *x2 = 0;
  *x3 = 0;
  *x4 = 0;
}

string TKey::statObjInfo() const
{
  char buf[256];
  sprintf(buf, "It is a key to %s", what_does_it_open(this));

  string a(buf);
  return a;
}

void TKey::lowCheck()
{
  if ((obj_flags.cost >= 0) && isRentable() &&
           (obj_flags.decay_time <= 0))
    vlogf(LOW_ERROR, "rentable key (%s)!", getName());

  TObj::lowCheck();
}

bool TKey::objectRepair(TBeing *ch, TMonster *repair, silentTypeT silent)
{
  if (!silent) {
    char buf[256];

    sprintf(buf, "%s Does this look like a locksmithery to you?", fname(ch->name).c_str());

    repair->doTell(buf);
  }
  return TRUE;
}

int TKey::stealModifier()
{
  return 77;   // make keys tough to steal
}

int TKey::putMeInto(TBeing *ch, TRealContainer *container)
{
  TObj *o;
  TThing *t;
  char buf[256];
  
  for(t=container->stuff; t; t=t->nextThing){
    o = dynamic_cast<TObj *>(t);

    if (!o)
      continue;

    if (dynamic_cast<TKeyring *>(container) &&
	obj_index[getItemIndex()].virt == obj_index[o->getItemIndex()].virt) {
      sprintf(buf, "You already have one of those keys in your %s.\n\r",
	      fname(container->name).c_str());
      ch->sendTo(buf);
      return TRUE;
    }
  }
  return FALSE;
}
