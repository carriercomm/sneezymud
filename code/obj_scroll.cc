//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: obj_scroll.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


// scroll.cc

#include "stdsneezy.h"

TScroll::TScroll() :
  TMagicItem()
{
  int i;
  for (i= 0; i < 3; i++) {
    spells[i] = TYPE_UNDEFINED;
  }
}

TScroll::TScroll(const TScroll &a) :
  TMagicItem(a)
{
  int i;
  for (i= 0; i < 3; i++) {
    spells[i] = a.spells[i];
  }
}

TScroll & TScroll::operator=(const TScroll &a)
{
  if (this == &a) return *this;
  TMagicItem::operator=(a);
  int i;
  for (i= 0; i < 3; i++) {
    spells[i] = a.spells[i];
  }
  return *this;
}

TScroll::~TScroll()
{
}

int TScroll::changeItemVal2Check(TBeing *ch, int the_update)
{
  if (the_update != -1 &&
      (!discArray[the_update] ||
      (!discArray[the_update]->minMana && !discArray[the_update]->minPiety))) {
    ch->sendTo("Invalid value or value is not a spell.\n\r");
    return TRUE;
  }
  return FALSE;
}

int TScroll::changeItemVal3Check(TBeing *ch, int the_update)
{
  return changeItemVal2Check(ch, the_update);
}

int TScroll::changeItemVal4Check(TBeing *ch, int the_update)
{
  return changeItemVal2Check(ch, the_update);
}

void TScroll::divinateMe(TBeing *caster) const
{
  caster->describeMagicLevel(this, 101);
  caster->describeMagicLearnedness(this, 101);
  caster->describeMagicSpell(this, 101);
}

void TScroll::assignFourValues(int x1, int x2, int x3, int x4)
{
  TMagicItem::assignFourValues(x1,x2,x3,x4);

  setSpell(0, mapFileToSpellnum(x2));
  setSpell(1, mapFileToSpellnum(x3));
  setSpell(2, mapFileToSpellnum(x4));
}

void TScroll::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  TMagicItem::getFourValues(x1,x2,x3,x4);

  *x2 = mapSpellnumToFile(getSpell(0));
  *x3 = mapSpellnumToFile(getSpell(1));
  *x4 = mapSpellnumToFile(getSpell(2));
}

string TScroll::statObjInfo() const
{
  char buf[256];
  sprintf(buf, "Level:   %d        Learnedness:   %d\n\rSpells : %s, %s, %s",
          getMagicLevel(), getMagicLearnedness(),
          (getSpell(0) == TYPE_UNDEFINED ? "No Spell" :
            (discArray[getSpell(0)] ?
                discArray[getSpell(0)]->name :
                "BOGUS SPELL.  Bug this")),
          (getSpell(1) == TYPE_UNDEFINED ? "No Spell" :
            (discArray[getSpell(1)] ?
                discArray[getSpell(1)]->name :
                "BOGUS SPELL.  Bug this")),
          (getSpell(2) == TYPE_UNDEFINED ? "No Spell" :
            (discArray[getSpell(2)] ?
                discArray[getSpell(2)]->name :
                "BOGUS SPELL.  Bug this")));

  string a(buf);
  return a;
}

void TScroll::lowCheck()
{
  int i;

  for (i= 0; i < 2; i++) {
    spellNumT curspell = getSpell(i);
    if ((curspell < TYPE_UNDEFINED) || (curspell >= MAX_SKILL) ||
        ((curspell > TYPE_UNDEFINED) &&
         ((!discArray[curspell] ||
          ((discArray[curspell]->typ != SPELL_RANGER) &&
           !discArray[curspell]->minMana &&
           !discArray[curspell]->minPiety)) ||
        (getDisciplineNumber(curspell, FALSE) == DISC_NONE)))) {
      vlogf(LOW_ERROR, "scroll (%s:%d) has messed up spell (slot %d: %d)",
           getName(), objVnum(), i+1, curspell);
      if ((curspell < TYPE_UNDEFINED) || (curspell >= MAX_SKILL))
        vlogf(LOW_ERROR, "bogus range");
      else if (!discArray[curspell])
        vlogf(LOW_ERROR, "bogus spell, %d", curspell);
      else if ((!discArray[curspell]->minMana &&
        !discArray[curspell]->minPiety))
        vlogf(LOW_ERROR, "non-spell");
      continue;
    }
    if (curspell > TYPE_UNDEFINED &&
        discArray[curspell]->targets & TAR_CHAR_WORLD) {
      // spells that use this setting are not a good choice for obj spells
      vlogf(LOW_ERROR, "Obj (%s : %d) had spell that shouldn't be on objs (%s : %d)" ,
          getName(), objVnum(), discArray[curspell]->name, curspell);
    }
  }

  TMagicItem::lowCheck();
}

bool TScroll::objectRepair(TBeing *ch, TMonster *repair, silentTypeT silent)
{
  if (!silent) {
    char buf[256];

    sprintf(buf, "%s You might wanna take that to the magic shop!", fname(ch->name).c_str());

    repair->doTell(buf);
  }
  return TRUE;
}

spellNumT TScroll::getSpell(int num) const
{
  mud_assert(num >= 0 && num < 3, "Bad num");
  return spells[num];
}

void TScroll::setSpell(int num, spellNumT xx)
{
  mud_assert(num >= 0 && num < 3, "Bad num");
  spells[num] = xx;
}

int TScroll::suggestedPrice() const
{
  int tot = 0;
  int i;
  for (i = 0; i < 3; i++) {
    spellNumT curspell = getSpell(i);
    int value = 0;
    if (curspell >= 0) {
      value = getSpellCost(curspell, getMagicLevel(), getMagicLearnedness());
      // since it's cast instantaneously, adjust for this
      value *= getSpellCasttime(curspell);

      // since it's from an obj, arbitrarily double it
      value *= 2;
    }


    tot += value;
  }
  return tot;
}

