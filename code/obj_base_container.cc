//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


// container.cc
//

#include "stdsneezy.h"

TContainer::TContainer() :
  TObj()
{
}

TContainer::TContainer(const TContainer &a) :
  TObj(a)
{
}

TContainer & TContainer::operator=(const TContainer &a)
{
  if (this == &a) return *this;
  TObj::operator=(a);
  return *this;
}

TContainer::~TContainer()
{
}

bool TContainer::engraveMe(TBeing *ch, TMonster *me, bool give)
{
  char buf[256];

  // engraved bags would protect too many things
  sprintf(buf, "%s The powers that be say I can't do that anymore.", ch->getName
());
  me->doTell(buf);

  if (give) {
    strcpy(buf, name);
    add_bars(buf);
    sprintf(buf + strlen(buf), " %s", fname(ch->name).c_str());
    me->doGive(buf);
  }
  return TRUE;
}

int TContainer::stealModifier()
{
  return 50;   // make bags tough to steal
}

int TContainer::getReducedVolume(const TThing *) const
{
  return getTotalVolume();
}

string TContainer::showModifier(showModeT tMode, const TBeing *tBeing) const
{
  string tString("");

  // Take 1 higher than the current used and minus 1 from it to get All of the
  // bits set.  From there remove the hold/thrown/take items as we only care
  // about those worn containers.  Ex: Mage Belt
  int    tCanWear = canWear((1 << MAX_ITEM_WEARS) - 1 - ITEM_HOLD - ITEM_THROW - ITEM_TAKE);

  if ((tMode == SHOW_MODE_SHORT_PLUS ||
       tMode == SHOW_MODE_SHORT_PLUS_INV ||
       tMode == SHOW_MODE_SHORT) && tCanWear) {
    tString += " (";
    tString += equip_condition(-1);
    tString += ")";
  }

  return tString;
}

void TContainer::purchaseMe(TBeing *ch, TMonster *tKeeper, int tCost, int tShop)
{
  TObj::purchaseMe(ch, tKeeper, tCost, tShop);
}
