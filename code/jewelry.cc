//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: jewelry.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


// jewelry.cc
//

#include "stdsneezy.h"

TJewelry::TJewelry() :
  TBaseClothing()
{
}

TJewelry::TJewelry(const TJewelry &a) :
  TBaseClothing(a)
{
}

TJewelry & TJewelry::operator=(const TJewelry &a)
{
  if (this == &a) return *this;
  TBaseClothing::operator=(a);
  return *this;
}

TJewelry::~TJewelry()
{
}

void TJewelry::assignFourValues(int , int , int , int )
{
}

void TJewelry::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1 = 0;
  *x2 = 0;
  *x3 = 0;
  *x4 = 0;
}

string TJewelry::statObjInfo() const
{
  string a("");
  return a;
}

void TJewelry::lowCheck()
{
  int i;
  for (i=0; i<MAX_OBJ_AFFECT;i++) {
    if ((affected[i].location == APPLY_ARMOR) &&
        (affected[i].modifier > 1))
      vlogf(LOW_ERROR, "jewelry (%s : %d) had armor, bad!",
         getName(), objVnum());
  }

  TBaseClothing::lowCheck();
}
