//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


// symbol.cc

#include "stdsneezy.h"
#include "shop.h"

TSymbol::TSymbol() :
  TObj(),
  strength(0),
  max_strength(0),
  faction(FACT_UNDEFINED)
{
}

TSymbol::TSymbol(const TSymbol &a) :
  TObj(a),
  strength(a.strength),
  max_strength(a.max_strength),
  faction(a.faction)
{
}

TSymbol & TSymbol::operator=(const TSymbol &a)
{
  if (this == &a) return *this;
  TObj::operator=(a);
  strength = a.strength;
  max_strength = a.max_strength;
  faction = a.faction;
  return *this;
}

TSymbol::~TSymbol()
{
}

void TSymbol::assignFourValues(int x1, int x2, int x3, int x4)
{
  setSymbolCurStrength(x1);
  setSymbolMaxStrength(x2);

  // allow undefined, rather than MIN_FACTION since undefined = unattnued
  if (x3 < FACT_UNDEFINED || x3 >= MAX_FACTIONS) {
     vlogf(LOG_LOW,"symbol with bad faction (%s).", getName());
    x3 = FACT_UNDEFINED;
  }
  mud_assert(x3 >= FACT_UNDEFINED && x3 < MAX_FACTIONS, "bad val");
  setSymbolFaction(factionTypeT(x3));
}

void TSymbol::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1 = getSymbolCurStrength();
  *x2 = getSymbolMaxStrength();
  *x3 = getSymbolFaction();
  *x4 = 0;
}

string TSymbol::statObjInfo() const
{
  char buf[256];
  string a;

  sprintf(buf, "Symbol Strength: Current: %d, Maximum: %d\n\r",
    getSymbolCurStrength(), 
    getSymbolMaxStrength());
  a += buf;
  sprintf(buf, "Symbol Faction: %d (%s)\n\r",
    getSymbolFaction(), 
    getSymbolFaction() == -1 ? "not attuned" : FactionInfo[getSymbolFaction()].faction_name);
  a += buf;

  return a;
}

int TSymbol::objectSell(TBeing *ch, TMonster *keeper)
{
  char buf[256];
  int attuneCode = 1;

  if ((getSymbolCurStrength() != getSymbolMaxStrength())) {
    sprintf(buf, "%s I'm sorry, I don't buy back used symbols.", ch->getName());
    keeper->doTell(buf);
    return TRUE;
  }

  if ((getSymbolFaction() != FACT_UNDEFINED) && attuneCode) {
    sprintf(buf, "%s I'm sorry, I don't buy back attuned symbols.", ch->getName());
    keeper->doTell(buf);
    return TRUE;
  }

  return FALSE;
}

int TSymbol::getSymbolCurStrength() const
{
  return strength;
}

void TSymbol::setSymbolCurStrength(int r)
{
  strength = r;
}

void TSymbol::addToSymbolCurStrength(int r)
{
  strength += r;
}

int TSymbol::getSymbolMaxStrength() const
{
  return max_strength;
}

void TSymbol::setSymbolMaxStrength(int r)
{
  max_strength = r;
}

void TSymbol::addToSymbolMaxStrength(int r)
{
  max_strength += r;
}

factionTypeT TSymbol::getSymbolFaction() const
{
  return faction;
}

void TSymbol::setSymbolFaction(factionTypeT r)
{
  faction = r;
}


bool TSymbol::sellMeCheck(const TBeing *ch, TMonster *keeper) const
{
  int total = 0;
  TThing *t;
  char buf[256];

  for (t = keeper->stuff; t; t = t->nextThing) {
    if ((t->number == number) &&
        (t->getName() && getName() &&
         !strcmp(t->getName(), getName()))) {
      total += 1;
      if (total > 9) {
        sprintf(buf, "%s I already have plenty of those.", ch->name);
        keeper->doTell(buf);
        return TRUE;
      }
    }
  }
  return FALSE;
}

void TSymbol::lowCheck()
{
  int i;

  if (getSymbolMaxStrength() < getSymbolCurStrength())
    vlogf(LOG_LOW, "symbol (%s) has lower max strength then current.",
             getName());
  for (i=0; i<MAX_OBJ_AFFECT;i++) {
    if (affected[i].location == APPLY_ARMOR) {
      vlogf(LOG_LOW, "symbol (%s) had armor, bad!",
         getName());
    }
  }
  int ap = suggestedPrice();
  if (ap != obj_flags.cost && obj_flags.cost >= 0 && ap) {
    // ignore newbie symbol with cost = -1
    vlogf(LOG_LOW, "symbol (%s:%d) has a bad price (%d).  should be (%d)",
         getName(), objVnum(), obj_flags.cost, ap);
    obj_flags.cost = ap;
  }

  TObj::lowCheck();
}

bool TSymbol::objectRepair(TBeing *ch, TMonster *repair, silentTypeT silent)
{
  if (!silent) {
    char buf[256];

    sprintf(buf, "%s Hey, I don't get involved with religion!", fname(ch->name).c_str());

    repair->doTell(buf);
  }
  return TRUE;
}

bool TSymbol::lowCheckSlots(silentTypeT silent)
{
  // symbols should be (take hold) or (take neck) or (take neck hold)
  // no other combos allowed, although permit the throw flag too

  unsigned int value = obj_flags.wear_flags;
  REMOVE_BIT(value, ITEM_THROW);
  REMOVE_BIT(value, ITEM_TAKE);
  REMOVE_BIT(value, ITEM_HOLD);
  REMOVE_BIT(value, ITEM_WEAR_NECK);

  if (value != 0) {
    if (!silent)
      vlogf(LOG_LOW, "symbol (%s) with bad wear slots: %d",
                 getName(), value);
    return true;
  }
  return false;
}

int TSymbol::suggestedPrice() const
{
  // c.f. balance notes for this
  int num = (int) (15 * (float) getSymbolMaxStrength() / 1000 + 0.5);
  if (canWear(ITEM_WEAR_NECK))
    num = (int) (num * 1.1);
  return num;
}

void TSymbol::objMenu(const TBeing *ch) const
{
  ch->sendTo(VT_CURSPOS, 3, 1);
  ch->sendTo("%sSuggested price:%s %d%s",
             ch->purple(), ch->norm(), suggestedPrice(),
             (suggestedPrice() != obj_flags.cost ? " *" : ""));
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("%sSymbol Level:%s %.1f",
       ch->purple(), ch->norm(),
       getSymbolLevel());
}

double TSymbol::getSymbolLevel() const
{
  return sqrt((double) getSymbolMaxStrength() / 500.0);
}

string TSymbol::showModifier(showModeT mode, const TBeing *ch) const
{
  string a;
  if (mode == SHOW_MODE_SHORT_PLUS ||
       mode == SHOW_MODE_SHORT_PLUS_INV ||
       mode == SHOW_MODE_SHORT) {
    if ((ch->isImmortal() && ch->hasWizPower(POWER_IMM_EVAL)) ||
        TestCode2) {
      char buf[256];
      sprintf(buf, " (L%d)", (int) (getSymbolLevel() + 0.5));
      a += buf;
    }
  }
  return a;
}

void TSymbol::evaluateMe(TBeing *ch) const
{
  int learn;

  learn = ch->getSkillValue(SKILL_EVALUATE);
  if (learn <= 0) {
    ch->sendTo("You are not sufficiently knowledgeable about evaluation.\n\r");
    return;
  }

  ch->sendTo(COLOR_OBJECTS, "You evaluate the spiritual powers of %s...\n\r\n\r",
             getName());

  ch->describeObject(this);

  if (learn > 10)
    ch->describeSymbolOunces(this, learn);
}

double TSymbol::objLevel() const
{
  return getSymbolLevel();
}

void TSymbol::purchaseMe(TBeing *ch, TMonster *keeper, int cost, int shop_nr)
{
  ch->addToMoney(-cost, GOLD_SHOP_SYMBOL);
  if (!IS_SET(shop_index[shop_nr].flags, SHOP_FLAG_INFINITE_MONEY)) {
    keeper->addToMoney(cost, GOLD_SHOP_SYMBOL);
  }

  shoplog(shop_nr, ch, keeper, getName(), cost, "buying");
}

void TSymbol::sellMeMoney(TBeing *ch, TMonster *keeper, int cost, int shop_nr)
{
  ch->addToMoney(cost, GOLD_SHOP_SYMBOL);
  if (!IS_SET(shop_index[shop_nr].flags, SHOP_FLAG_INFINITE_MONEY))
    keeper->addToMoney(-cost, GOLD_SHOP_SYMBOL);

  shoplog(shop_nr, ch, keeper, getName(), cost, "selling");
}

string TSymbol::getNameForShow(bool useColor, bool useName, const TBeing *ch) const
{
  char buf2[256];
  sprintf(buf2, "%s (L%d)",
       useName ? name : (useColor ? getName() : getNameNOC(ch).c_str()),
       (int) (getSymbolLevel() + 0.5));
  return buf2;
}

int TSymbol::chiMe(TBeing *tLunatic)
{
  int tMana  = ::number(10, 30),
      bKnown = tLunatic->getSkillLevel(SKILL_CHI);

  if (tLunatic->getMana() < tMana) {
    tLunatic->sendTo("You lack the chi to do this!\n\r");
    return RET_STOP_PARSING;
  } else
    tLunatic->reconcileMana(TYPE_UNDEFINED, 0, tMana);

  if (!bSuccess(tLunatic, bKnown, SKILL_CHI) ||
      (getSymbolCurStrength() >= getSymbolMaxStrength())) {
    act("You focus upon $p, but faulter and gently harm it!",
        FALSE, tLunatic, this, NULL, TO_CHAR);
    act("$n focuses on $p, but it cracks gently in response!",
        TRUE, tLunatic, this, NULL, TO_ROOM);

    addToSymbolCurStrength(-::number(1, 4));

    if (getSymbolCurStrength() <= 0) {
      act("$p reacts violently and shatters!",
          FALSE, tLunatic, this, NULL, TO_ROOM);
      return DELETE_VICT;
    }
  } else {
    act("You focus upon $p causing it to mend ever so slightly!",
        FALSE, tLunatic, this, NULL, TO_CHAR);
    act("$n concentrates upon $p, causing it to mend ever so slightly!",
        TRUE, tLunatic, this, NULL, TO_ROOM);

    setSymbolCurStrength(min(getSymbolMaxStrength(), (getSymbolCurStrength() + ::number(1, 4))));
  }

  return true;
}
