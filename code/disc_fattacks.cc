//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: disc_fattacks.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#include "stdsneezy.h"
#include "disc_fattacks.h"

CDFAttacks::CDFAttacks() :
  CDiscipline(),
  skQuiveringPalm(),
  skCriticalHitting()
{
}

CDFAttacks::CDFAttacks(const CDFAttacks &a) :
  CDiscipline(a),
  skQuiveringPalm(a.skQuiveringPalm),
  skCriticalHitting(a.skCriticalHitting)
{
}

CDFAttacks & CDFAttacks::operator=(const CDFAttacks &a)
{
  if (this == &a) return *this;
  CDiscipline::operator=(a);
  skQuiveringPalm = a.skQuiveringPalm;
  skCriticalHitting = a.skCriticalHitting;
  return *this;
}

CDFAttacks::~CDFAttacks()
{
}

CDFAttacks * CDFAttacks::cloneMe()
{
  return new CDFAttacks(*this);
}
