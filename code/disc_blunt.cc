//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: disc_blunt.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#include "stdsneezy.h"
#include "disc_blunt.h"

CDBash::CDBash() :
  CDiscipline(),
  skBluntSpec()
{
}

CDBash::CDBash(const CDBash &a) :
  CDiscipline(a),
  skBluntSpec(a.skBluntSpec)
{
}

CDBash & CDBash::operator=(const CDBash &a)
{
  if (this == &a) return *this;
  CDiscipline::operator=(a);
  skBluntSpec = a.skBluntSpec;
  return *this;
}

CDBash::~CDBash()
{
}
