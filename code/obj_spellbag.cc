//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


// spellbag.cc
//

#include "stdsneezy.h"

TSpellBag::TSpellBag() :
  TExpandableContainer()
{
}

TSpellBag::TSpellBag(const TSpellBag &a) :
  TExpandableContainer(a)
{
}

TSpellBag & TSpellBag::operator=(const TSpellBag &a)
{
  if (this == &a) return *this;
  TExpandableContainer::operator=(a);
  return *this;
}

TSpellBag::~TSpellBag()
{
}

void TSpellBag::assignFourValues(int x1, int x2, int x3, int x4)
{
  TExpandableContainer::assignFourValues(x1, x2, x3, x4);
}

void TSpellBag::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  TExpandableContainer::getFourValues(x1, x2, x3, x4);
}

string TSpellBag::statObjInfo() const
{
  return TExpandableContainer::statObjInfo();
}

bool TSpellBag::objectRepair(TBeing *, TMonster *, silentTypeT)
{
  // with worn spellbags, this is needed
  return false;
}

bool TSpellBag::lowCheckSlots(silentTypeT silent)
{
  // spellbags should be (take hold) or (take waist) or (take hold waiste)
  // no other combos allowed, although permit the throw flag too
  // neck for juju

  unsigned int value = obj_flags.wear_flags;
  REMOVE_BIT(value, ITEM_THROW);
  REMOVE_BIT(value, ITEM_TAKE);
  REMOVE_BIT(value, ITEM_HOLD);
  REMOVE_BIT(value, ITEM_WEAR_WAISTE);
  REMOVE_BIT(value, ITEM_WEAR_NECK);

  if (value != 0) {
    if (!silent)
      vlogf(LOG_LOW, "spellbag (%s) with bad wear slots: %d",
                 getName(), value);
    return true;
  }
  return false;
}

TThing & TSpellBag::operator+= (TThing &t)
{
  TExpandableContainer::operator+=(t);

  // way down in tthing::operator, we expand volume and increase weight of bag
  // undo this if obj being put in is a component
  // counterintuitive: we are UNDOing what will happen in TThing::
  // we want things to be 25% of true mass/vol
  if (dynamic_cast<TComponent *>(&t)) {
    addToCarriedWeight((-t.getTotalWeight(TRUE) * 0.75));
    addToCarriedVolume(-t.getReducedVolume(this) * 0.75);
  }

  return *this;
}

void TSpellBag::findSomeComponent(TComponent **comp_gen, TComponent **comp_spell, TComponent **comp_brew, spellNumT which, int type)
{
  TThing *t;

  for (t = stuff; t; t = t->nextThing)
    t->findSomeComponent(comp_gen, comp_spell, comp_brew, which, type);
}

void TSpellBag::getObjFromMeText(TBeing *tBeing, TThing *tThing, getTypeT tType, bool tFirst)
{
  --(*tThing);
  *tBeing += *tThing;

#if 1
  act("You take $p from $P.",
      FALSE, tBeing, tThing, this, TO_CHAR);
  act("$n takes $p from $P.",
      TRUE, tBeing, tThing, this, TO_ROOM);
#else
  if (tType == GETNULL || tType == GETALL || tType == GETOBJ || tType == GETOBJOBJ) {
    act("You take $p from $P.",
        FALSE, tBeing, tThing, this, TO_CHAR);
    act("$n takes $p from $P.",
        TRUE, tBeing, tThing, this, TO_ROOM);
  } else if (tFirst) {
    act("You begin to take from $p.",
        FALSE, tBeing, this, NULL, TO_CHAR);
    act("$n begins to take things from $p.",
        FALSE, tBeing, this, NULL, TO_CHAR);
  }
#endif
}
