//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: disc_physical.cc,v $
// Revision 5.1.1.3  2000/11/11 10:15:08  jesus
// added powermove skill
//
// Revision 5.1.1.2  2000/10/26 05:39:02  jesus
// dual wield for warriors
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


#include "stdsneezy.h"
#include "disc_physical.h"

CDPhysical::CDPhysical() :
      CDiscipline(),
      skDoorbash(),
      skDualWieldWarrior(),
      skPowerMove(),
      skDeathstroke()
{
}

CDPhysical::CDPhysical(const CDPhysical &a) :
      CDiscipline(a),
      skDoorbash(a.skDoorbash),
      skDualWieldWarrior(a.skDualWieldWarrior),
      skPowerMove(a.skPowerMove),
      skDeathstroke(a.skDeathstroke)
{
}

CDPhysical & CDPhysical::operator=(const CDPhysical &a)
{
  if (this == &a) return *this;
  CDiscipline::operator=(a);
  skDoorbash = a.skDoorbash;
  skDualWieldWarrior = a.skDualWieldWarrior;
  skPowerMove = a.skPowerMove;
  skDeathstroke = a.skDeathstroke;
  return *this;
}

CDPhysical::~CDPhysical()
{
}




