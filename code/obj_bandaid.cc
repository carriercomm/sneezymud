//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: obj_bandaid.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


// bandaid.cc

#include "stdsneezy.h"

TBandaid::TBandaid() :
  TObj()
{
}

TBandaid::TBandaid(const TBandaid &a) : 
 TObj(a)
{
}

TBandaid & TBandaid::operator=(const TBandaid &a)
{
  if (this == &a) return *this;
  TObj::operator=(a);
  return *this;
}

TBandaid::~TBandaid()
{
}

void TBandaid::assignFourValues(int, int, int, int)
{
}

void TBandaid::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1 = 0;
  *x2 = 0;
  *x3 = 0;
  *x4 = 0;
}

string TBandaid::statObjInfo() const
{
  string a("");
  return a;
}

void TBandaid::scrapMe(TBeing *ch)
{
  ch->remLimbFlags(eq_pos, PART_BANDAGED);
}

void TBandaid::findBandage(int *count)
{
  (*count)++;
}

void TBandaid::destroyBandage(int *count)
{
  (*count)++;
  delete this;
}

