//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: obj_gemstone.cc,v $
// Revision 5.3  2003/03/13 22:40:53  peel
// added sstring class, same as string but takes NULL as an empty string
// replaced all uses of string to sstring
//
// Revision 5.2  2002/01/10 00:45:49  peel
// more splitting up of obj2.h
//
// Revision 5.1  2001/07/13 05:32:20  peel
// renamed a bunch of source files
//
// Revision 5.1.1.1  1999/10/16 04:32:20  batopr
// new branch
//
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


// gemstone.cc

#include "stdsneezy.h"
#include "obj_gemstone.h"

TGemstone::TGemstone() :
  TObj()
{
}

TGemstone::TGemstone(const TGemstone &a) :
  TObj(a)
{
}

TGemstone & TGemstone::operator=(const TGemstone &a)
{
  if (this == &a) return *this;
  TObj::operator=(a);
  return *this;
}

TGemstone::~TGemstone()
{
}

void TGemstone::assignFourValues(int, int, int, int)
{
}

void TGemstone::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1 = 0;
  *x2 = 0;
  *x3 = 0;
  *x4 = 0;
}

sstring TGemstone::statObjInfo() const
{
  sstring a("");
  return a;
}

