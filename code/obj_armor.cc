//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: obj_armor.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


// armor.cc
//

#include "stdsneezy.h"

TArmor::TArmor() :
  TBaseClothing()
{
}

TArmor::TArmor(const TArmor &a) :
  TBaseClothing(a)
{
}

TArmor & TArmor::operator=(const TArmor &a)
{
  if (this == &a) return *this;
  TBaseClothing::operator=(a);
  return *this;
}

TArmor::~TArmor()
{
}

void TArmor::assignFourValues(int , int , int , int )
{
}

void TArmor::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1 = 0;
  *x2 = 0;
  *x3 = 0;
  *x4 = 0;
}

string TArmor::statObjInfo() const
{
  string a("");
  return a;
}

void TArmor::lowCheck()
{
  TBaseClothing::lowCheck();
}

int TArmor::galvanizeMe(TBeing *local_caster, byte bKnown)
{
  if (getMaxStructPoints() < 0) {
    act("$p is as solid as it is possible.",
         FALSE, local_caster, this, 0, TO_CHAR);
    act("Nothing seems to happen.", FALSE, local_caster, 0, 0, TO_ROOM);
    return SPELL_FAIL;
  }

  if (bSuccess(local_caster, bKnown, SPELL_GALVANIZE)) {
    addToMaxStructPoints(1);
    addToStructPoints(1);
    return SPELL_SUCCESS;
  } else {
    if (critFail(local_caster, SPELL_GALVANIZE)) {
      CF(SPELL_GALVANIZE);
      return SPELL_CRIT_FAIL_2;  // item destroyed
    }
    addToMaxStructPoints(-2);
    addToStructPoints(-2);
    return SPELL_CRIT_FAIL;  // just to distinguish from "nothing happens"
  }
}

