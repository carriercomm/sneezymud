// base_clothing.cc

#include <cmath>

#include "stdsneezy.h"
#include "shop.h"

TBaseClothing::TBaseClothing() :
  TObj()
{
}

TBaseClothing::TBaseClothing(const TBaseClothing &a) :
  TObj(a)
{
}

TBaseClothing & TBaseClothing::operator=(const TBaseClothing &a)
{
  if (this == &a) return *this;
  TObj::operator=(a);
  return *this;
}

TBaseClothing::~TBaseClothing()
{
}

void TBaseClothing::lowCheck()
{
  int ap = suggestedPrice();
#if 0
  // apparently, turning compiler optimization off (or not -O3) causes
  // some slight fluxuation.  Possibly from float truncation.
  int delta = ap - obj_flags.cost;
  if (((delta > 5) || (delta < -5)) && !isSaddle()) {
#elif 1
  // very strict
  if (ap != obj_flags.cost && !isSaddle()) {
#else
  // only correct prices that are too low
  // warn about prices that are WAY too high though too
  if ((ap > obj_flags.cost || ap < obj_flags.cost-200) && !isSaddle()) {
#endif
    vlogf(LOG_LOW, "base_clothing (%s:%d) has a bad price (%d).  should be (%d)",
         getName(), objVnum(), obj_flags.cost, ap);
    obj_flags.cost = ap;
  }

  if (isShield()) {
    float amt = -(float) itemAC();

    if ((20*getWeight()) < amt)
      vlogf(LOG_LOW, "shield %s has a bad weight.  should be (%.1f)",
           getName(), amt/20.0+.1);
  } else {
    if (canWear(ITEM_HOLD)) {
      int amt = -itemAC();
      if (amt)
        vlogf(LOG_LOW, "Holdable item (%s:%d) with AC that was not a shield.",
            getName(), objVnum());
    }
  }

  // insure proper wearability
  unsigned int ui = obj_flags.wear_flags;
  REMOVE_BIT(ui, ITEM_TAKE);
  REMOVE_BIT(ui, ITEM_THROW);

  if (ui != ITEM_HOLD) {
    int num = CountBits(ui) - 1;
    if (num < 0) {
      vlogf(LOG_LOW, "Base Clothing (%s:%d) with insufficient wearability.",
            getName(), objVnum());
    }
  }
  TObj::lowCheck();
}

bool TBaseClothing::isSaddle() const
{
  return isname("saddle", name);
}

bool TBaseClothing::isShield() const
{
  return isname("shield", name);
}

bool TBaseClothing::isBarding() const
{
  return isname("barding", name);
}

void TBaseClothing::objMenu(const TBeing *ch) const
{
  ch->sendTo(VT_CURSPOS, 3, 1);
  ch->sendTo("%sSuggested price:%s %d%s",
             ch->purple(), ch->norm(), suggestedPrice(),
             (suggestedPrice() != obj_flags.cost ? " *" : ""));
  ch->sendTo(VT_CURSPOS, 3, 25);
  ch->sendTo("%sReal Level:%s %.2f",
             ch->purple(), ch->norm(), armorLevel(ARMOR_LEV_REAL));
  ch->sendTo(VT_CURSPOS, 3, 45);
  ch->sendTo("%sAC Lev:%s %.2f",
             ch->purple(), ch->norm(), armorLevel(ARMOR_LEV_AC));
  ch->sendTo(VT_CURSPOS, 3, 60);
  ch->sendTo("%sStr Lev:%s %.2f",
             ch->purple(), ch->norm(), armorLevel(ARMOR_LEV_STR));
}

void TBaseClothing::evaluateMe(TBeing *ch) const
{
  int learn;

  learn = ch->getSkillValue(SKILL_EVALUATE);
  if (learn <= 0) {
    ch->sendTo("You are not sufficiently knowledgeable about evaluation.\n\r");
    return;
  }

  ch->learnFromDoingUnusual(LEARN_UNUSUAL_NORM_LEARN, SKILL_EVALUATE, 10);

  ch->sendTo(COLOR_OBJECTS, "You evaluate %s for its battle-worthiness...\n\r\n\r",
         getName());

  ch->describeObject(this);
  ch->describeNoise(this, learn);

  if (learn > 20)
    ch->describeMaxStructure(this, learn);
  if (learn > 50)
    ch->describeArmor(this, learn);

  if (ch->isImmortal()) {
    ch->sendTo(COLOR_OBJECTS, "IMMORTAL EVAL: %s overall is rated as %.2f gear.\n\r", getName(), armorLevel(ARMOR_LEV_REAL));
    ch->sendTo(COLOR_OBJECTS, "IMMORTAL EVAL: %s has structure of %.2f gear.\n\r", getName(), armorLevel(ARMOR_LEV_STR));
    ch->sendTo(COLOR_OBJECTS, "IMMORTAL EVAL: %s has armor of %.2f gear.\n\r", getName(), armorLevel(ARMOR_LEV_AC));
  }
}

void TBaseClothing::descMaxStruct(const TBeing *ch, int learn) const
{
  char capbuf[80];

  if (!ch->hasClass(CLASS_RANGER) && !ch->hasClass(CLASS_WARRIOR) &&
      !ch->hasClass(CLASS_DEIKHAN))
    learn /= 3;

  int maxstruct = GetApprox(getMaxStructPoints(), learn);

  strcpy(capbuf, ch->objs(this));
  ch->sendTo(COLOR_OBJECTS,"%s seems to %s.\n\r",
           cap(capbuf),
          ((maxstruct >= 99) ? "be virtually indestructible" :
           ((maxstruct >= 95) ? "be very durable" :
           ((maxstruct >= 91) ? "be durable" :
           ((maxstruct >= 87) ? "be fairly durable" :
           ((maxstruct >= 83) ? "be incredibly sturdy" :
           ((maxstruct >= 79) ? "be very sturdy" :
           ((maxstruct >= 75) ? "be sturdy" :
           ((maxstruct >= 71) ? "be somewhat sturdy" :
           ((maxstruct >= 67) ? "be fairly sturdy" :
           ((maxstruct >= 63) ? "be very substantial" :
           ((maxstruct >= 59) ? "be substantial" :
           ((maxstruct >= 55) ? "be somewhat substantial" :
           ((maxstruct >= 51) ? "be very well-constructed" :
           ((maxstruct >= 47) ? "be well-constructed" :
           ((maxstruct >= 43) ? "be fairly well-constructed" :
           ((maxstruct >= 39) ? "be incredibly rugged" :
           ((maxstruct >= 35) ? "be rugged" :
           ((maxstruct >= 31) ? "be somewhat rugged" :
           ((maxstruct >= 27) ? "be very strong" :
           ((maxstruct >= 23) ? "be strong" :
           ((maxstruct >= 19) ? "be somewhat strong" :
           ((maxstruct >= 15) ? "be fairly flimsy" :
           ((maxstruct >= 11) ? "be somewhat flimsy" :
           ((maxstruct >= 7) ? "be flimsy" :
           ((maxstruct >= 3) ? "be very flimsy" :
           ((maxstruct >= 0) ? "be incredibly flimsy" :
                          "be indestructible")))))))))))))))))))))))))));
}

void TBaseClothing::armorPercs(double *ac_perc, double *str_perc) const
{
  if (canWear(ITEM_WEAR_HEAD)) {
    *ac_perc  = 0.07;
    *str_perc = 0.07;
  } else if (canWear(ITEM_WEAR_NECK)) {
    *ac_perc  = 0.04;
    *str_perc = 0.04;
  } else if (canWear(ITEM_WEAR_BODY)) {
    *ac_perc  = 0.15;
    *str_perc = 0.26;
  } else if (canWear(ITEM_WEAR_BACK)) {
    *ac_perc  = 0.07;
    *str_perc = 0.10;
  } else if (canWear(ITEM_WEAR_WAISTE)) {
    *ac_perc  = 0.08;
    *str_perc = 0.05;
  } else if (canWear(ITEM_WEAR_ARMS)) {
    *ac_perc  = 0.04;
    *str_perc = 0.05;
  } else if (canWear(ITEM_WEAR_WRIST)) {
    *ac_perc  = 0.02;
    *str_perc = 0.03;
  } else if (canWear(ITEM_WEAR_HANDS)) {
    *ac_perc  = 0.03;
    *str_perc = 0.03;
  } else if (canWear(ITEM_WEAR_FINGER)) {
    *ac_perc  = 0.01;
    *str_perc = 0.01;
  } else if (canWear(ITEM_WEAR_LEGS)) {
    *ac_perc  = 0.05;
    *str_perc = 0.03;
  } else if (canWear(ITEM_WEAR_FEET)) {
    *ac_perc  = 0.02;
    *str_perc = 0.02;
  } else if (canWear(ITEM_HOLD)) {
    *ac_perc  = 0.25;
    *str_perc = 0.07;
  } else {
    vlogf(LOG_LOW, "Item %s needs a definition of where worn", getName());
    *ac_perc = 0.01;
    *str_perc = 0.01;
  }
}

// this is simplistic
void TBaseClothing::setDefArmorLevel(float lev)
{
  if (isSaddle())
    return;

  double ac_perc,
         str_perc,
         new_acVal,
         new_strVal;

  const double NEWBIE_AC  = 500.0,
               NEWBIE_STR = 30.0,
               BODY_STR   = 0.26;

  armorPercs(&ac_perc, &str_perc);

  new_acVal = (((lev * 25) * ac_perc) + ((NEWBIE_AC * ac_perc) +
               (isPaired() ? 1.0 : 0.5)));

  new_strVal = (((lev * 1.1) / sqrt(BODY_STR / str_perc)) +
                ((NEWBIE_STR * sqrt(str_perc / BODY_STR)) +
                (isPaired() ? 1.0 : 0.5)));

  new_acVal  = min(1000.0, max(-1000.0, new_acVal ));
  new_strVal = min( 100.0, max(    0.0, new_strVal));

  setMaxStructPoints((int)new_strVal);
  setStructPoints((int)new_strVal);

  for (int applyIndex = 0; applyIndex < MAX_OBJ_AFFECT; applyIndex++)
    if (affected[applyIndex].location == APPLY_ARMOR)
      affected[applyIndex].modifier = -(int)new_acVal;
}

// takes stats of eq, and returns a "level" for it
// armor is 50 points + 2.5 per level
// spread armor over all pieces based on armorPercs
// struct is 30 + 1 per level for jacket
// scale struct for non-jacket based on armorPercs
double TBaseClothing::armorLevel(armorLevT type) const
{
  int armor = 0;

  if (isSaddle())
    return 0;

  double ac_perc;    // these are arbitrary numbers I chose
  double str_perc;   // these should be identical to whats stored in
                     // constants.cc
  armorPercs(&ac_perc, &str_perc);

  armor = -itemAC();
  int str = getMaxStructPoints();
  if (isPaired()) {
    ac_perc *= 2.0;
    str_perc *= 2.0;
  }

  // this is the total AC of a newbie suit
  const double NEWBIE_AC = 500.0;

  // This calculates the AC of each body part of the newbie suit
  int ac_min = (int) ((NEWBIE_AC * ac_perc) +
         (isPaired() ? 1.0 : 0.5));

  // this is the struct of the BODY slot on newbie suit
  const double NEWBIE_STR = 30.0;

  // should be identical to struct % for body slot
  const double BODY_STR = 0.26;

  // this calculates the struct of each body part on the newbie suit
  // I am not positive exactly why the sqrt is in there, but I stuck
  // it in the original equation, so I probably had a reason...
  int str_min = (int) ((NEWBIE_STR *
           sqrt((double) str_perc / BODY_STR)) +
            (isPaired() ? 1.0 : 0.5));

  // newbie suit has 50 AC and 30 struct
  // high level gear (L60?) should have 200 AC and 90 struct
  // so we linearilize this and say 2.5 AC/level and 1.1 struct/level
  // but that is per SUIT, not per piece
  double quality_ac = ((double) armor - (double) ac_min);
  quality_ac = max(quality_ac, 0.0);

  double quality_struct = (double) (((double) str - (double) str_min));
  quality_struct = max(quality_struct, 0.0);

  // adjust for the piece
  quality_ac /= ac_perc;
  double factor = sqrt(BODY_STR / str_perc);
  quality_struct *= factor;

  double ac_lev = quality_ac / 25;
  double str_lev = quality_struct /= 1.1;

  if (type == ARMOR_LEV_REAL) {
    return (3.0 * ac_lev + str_lev) / 4.0;
  } else if (type == ARMOR_LEV_AC)
    return ac_lev;
  else if (type == ARMOR_LEV_STR)
    return str_lev;
  return 0.0;
}

// evaluates a "price" for the EQ based on AC and struct
int TBaseClothing::armorPriceStruct(armorLevT type, double *lev) const
{
  double ac_lev = armorLevel(ARMOR_LEV_AC);
  double str_lev = armorLevel(ARMOR_LEV_STR);

  double ac_perc;    // these are arbitrary numbers I chose
  double str_perc;   // these should be identical to whats stored in
                     // constants.cc
  armorPercs(&ac_perc, &str_perc);

  // make the price structure mirror the rent credit formula
  // that is, rent credit covers the AC/struct of their armor
  // but they have to pay for weapon, sundries, magic on eq, etc...
  double ac_price = (ac_lev * max(ac_lev, 20.0) * 75);
  double str_price = (str_lev * max(str_lev, 20.0) * 75);

  // now adjust above number so that it splits it over all the pieces...
  int ac_comp = (int) (ac_price * ac_perc);
  int str_comp = (int) (str_price * str_perc);

  int price = 0;
  *lev = 0.0;
  if (type == ARMOR_LEV_REAL) {
    // true price should be 1 part struct, 3 parts AC
    price = (int) ((3.0 * ac_comp + 1.0 * str_comp) / 4.0);
    *lev = (3.0 * ac_lev + str_lev)/4.0;
  } else if (type == ARMOR_LEV_AC) {
    price = ac_comp;
    *lev = ac_lev;
  } else if (type == ARMOR_LEV_STR) {
    price = str_comp;
    *lev = str_lev;
  }

  // OK, since "price" up to now is based on rent credit
  // and we want to return the cost of the object
  // double it since cost = 2 * rent
  price *= 2;

// vlogf(LOG_MISC, "ac_lev: %.2f, str_lev: %.2f, price: %d", ac_lev, str_lev, price);

  return price;
}

// this function adjusts price based on other qualities
// price from armor/struct calculated elsewhere is used.
int TBaseClothing::suggestedPrice() const
{
  int i;
  if (isSaddle())
    return 0;

  double lev;
  int price = armorPriceStruct(ARMOR_LEV_REAL, &lev);

#if 0
  // adjustment for wearability
  // from balance notes, we want various classes to have various AC from
  // their equipment.
  // full AC assumes hitrate of 60%, and 1 "level" is a 3% difference
  // so define lev_modifier as (hitrate-60)/3
  // phase the lev_mod in so that soft-leather is wearable (and appropriate)
  // for all classes
  // multiply price by the ratio of rentCredit(lev + lev_mod)/rentCredit(lev)
  double lev_mod = 0;

  // mage = hit rate of 90%
  if (!isObjStat(ITEM_ANTI_MAGE))
    lev_mod = max(lev_mod, 10.0);

  // cleric = hitrate of 81%
  if (!isObjStat(ITEM_ANTI_CLERIC))
    lev_mod = max(lev_mod, 7.0);

  // warrior = hitrate of 60%

  // thief = hitrate of 75%
  if (!isObjStat(ITEM_ANTI_THIEF))
    lev_mod = max(lev_mod, 5.0);

  // deikhan = hitrate of 69%
  if (!isObjStat(ITEM_ANTI_DEIKHAN))
    lev_mod = max(lev_mod, 3.0);

  // monk = hitrate of 90%
  if (!isObjStat(ITEM_ANTI_MONK) && !monkRestrictedItem(NULL))
    lev_mod = max(lev_mod, 10.0);

  // shaman = hitrate of 90%
  if (!isObjStat(ITEM_ANTI_SHAMAN) && !shamanRestrictedItem(NULL))
    lev_mod = max(lev_mod, 10.0);

  // ranger = hitrate of 69%
  if (!isObjStat(ITEM_ANTI_RANGER) && !rangerRestrictedItem(NULL))
    lev_mod = max(lev_mod, 3.0);

  // phase in with level
  lev_mod = min(lev_mod, lev/3.0);

  // adjust price based on rent credit formula
  double modif = (lev + lev_mod) * max(20.0, (lev + lev_mod)) /
                max(1.0, (lev * max(20.0, lev)));
  modif = min(1.25, modif);
// vlogf(LOG_MISC, "%s had a wearability modifier of %.3f (%.3f)", getName(), modif, lev_mod);
  price = (int) (price * modif);
#endif
  
  int adjustments = 0;
  for (i = 0;i < MAX_OBJ_AFFECT;i++) {
    int num = affected[i].modifier;
    num = max(0,num);
    switch (affected[i].location) {
      case APPLY_NONE:
        break;
      case APPLY_STR:
      case APPLY_BRA:
      case APPLY_DEX:
      case APPLY_AGI:
      case APPLY_CON:
      case APPLY_INT:
      case APPLY_WIS:
      case APPLY_FOC:
      case APPLY_PER:
      case APPLY_CHA:
      case APPLY_SPE:
      case APPLY_KAR:
        adjustments += max(100, (175 * (num / 3))); // cosmo changed to convert
        break;
      case APPLY_AGE:
        adjustments += 2000 * num;
        break;
      case APPLY_CHAR_HEIGHT:
      case APPLY_CHAR_WEIGHT:
        adjustments += 50 * num;
        break;
      case APPLY_ARMOR:
        break;
      case APPLY_HIT:
      case APPLY_MANA:
        adjustments += 800 * num;
        break;
      case APPLY_MOVE:
        adjustments += 300 * num;
        break;
      case APPLY_LIGHT:
        adjustments += 100 * num * num;
        break;
      case APPLY_NOISE:   // negative = good
        adjustments -= 250 * affected[i].modifier;
        break;
      case APPLY_CAN_BE_SEEN:
        adjustments += 1000 * num;
        break;
      case APPLY_VISION:
        adjustments += 3775 * num;
        break;
      case APPLY_SPELL_EFFECT:
        if (affected[i].modifier & AFF_FLYING)
          adjustments += 7500;
        if (affected[i].modifier & AFF_DETECT_INVISIBLE)
          adjustments += 2000;
        if (affected[i].modifier & AFF_DETECT_MAGIC)
          adjustments += 600;
        break;
#if 0
      case APPLY_IMMUNITY:
        adjustments += 334 * affected[i].modifier2;
        break;
      case APPLY_SPELL:
        adjustments += 427 * affected[i].modifier2;
        break;
#endif
      default:
        break;
    }
    if (affected[i].location == APPLY_HITROLL ||
        affected[i].location == APPLY_HITNDAM) {
      // add directly to price since we don't want to be scaled 
      // this formula is in balance notes
      int amt  = (int) (lev * max(lev, 20.0) * 450/4);
          amt -= (int) ((lev-num) * max(lev-num, 20.0)* 450/4);
      price += amt;
    }
    if (canWear(ITEM_WEAR_FINGER)) {
      if ((affected[i].location == APPLY_DAMROLL) ||
            (affected[i].location == APPLY_HITNDAM))
        adjustments += 6700 * num;
    } else if ((canWear(ITEM_WEAR_HANDS)) || (canWear(ITEM_WEAR_WRIST))) {
      if ((affected[i].location == APPLY_DAMROLL) ||
            (affected[i].location == APPLY_HITNDAM))
        adjustments += 10800 * num;
    } else if (canWear(ITEM_WEAR_ARMS)) {
      if ((affected[i].location == APPLY_DAMROLL) ||
            (affected[i].location == APPLY_HITNDAM))
        adjustments += 14500 * num;
    } else if (canWear(ITEM_WEAR_BACK)) {
      if ((affected[i].location == APPLY_DAMROLL) ||
            (affected[i].location == APPLY_HITNDAM))
        adjustments += 12400 * num;
    } else {
      if ((affected[i].location == APPLY_DAMROLL) ||
            (affected[i].location == APPLY_HITNDAM))
        adjustments += 40000 * num;
    }
  }
  // now scale the ajustments based on the gear
  lev = max(10.0,lev);
  lev = min(50.0,lev);
  adjustments = (int) (adjustments * lev / 40.0);
  price += adjustments;

  return price;
}

int TBaseClothing::scavengeMe(TBeing *ch, TObj **best_o)
{
  wearSlotT sl;

  sl = slot_from_bit(obj_flags.wear_flags);
  TObj * tobj = dynamic_cast<TObj *>(ch->equipment[sl]);
  if (itemAC() < (tobj ? tobj->itemAC() : 0)) {
    *best_o = this;
  }
  return FALSE;
}

bool TBaseClothing::sellMeCheck(const TBeing *ch, TMonster *keeper) const
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

int TBaseClothing::getShopPrice(int *discount) const
{
  return shop_price(discount);
}

void TBaseClothing::recalcShopData(int bought, int cost)
{
  genericCalc(bought, cost);
}

void TBaseClothing::describeObjectSpecifics(const TBeing *ch) const
{
  unsigned int ui = obj_flags.wear_flags;
  REMOVE_BIT(ui, ITEM_TAKE);
  REMOVE_BIT(ui, ITEM_THROW);

  if (ui == ITEM_HOLD) {
    string buf = "$p looks like it is meant to be held in your hands.";
    act(buf.c_str(), FALSE, ch, this, 0, TO_CHAR);
  } else {
    int num = CountBits(ui) - 1;
    if (num >= 0) {
      string buf = "$p looks like it is meant to be worn on your ";
      buf += good_uncap(wear_bits[num]);
      buf += ".";
      act(buf.c_str(), FALSE, ch, this, 0, TO_CHAR);
    } else {
      vlogf(LOG_LOW, "Base Clothing (%s:%d) with insufficient wearability.",
            getName(), objVnum());
    }
  }
}

int TBaseClothing::rentCost() const
{
  // silly, but I suspect we want to modify this in the future so I left it in place
  return TObj::rentCost();
}

string TBaseClothing::showModifier(showModeT mode, const TBeing *ch) const
{
  string a;
  if (mode == SHOW_MODE_SHORT_PLUS ||
       mode == SHOW_MODE_SHORT_PLUS_INV ||
       mode == SHOW_MODE_SHORT) {
    a = " (";
    a += equip_condition(-1);
    a += ")";
    if (ch->hasWizPower(POWER_IMM_EVAL) || TestCode2) {
      char buf[256];
      sprintf(buf, " (L%d)", (int) (objLevel() + 0.5));
      a += buf;
    }
  }
  return a;
}

double TBaseClothing::objLevel() const
{
  return armorLevel(ARMOR_LEV_REAL);
}

string TBaseClothing::getNameForShow(bool useColor, bool useName, const TBeing *ch) const
{
  char buf2[256];
  sprintf(buf2, "%s (L%d)",
       useName ? name : (useColor ? getName() : getNameNOC(ch).c_str()),
       (int) (objLevel() + 0.5));
  return buf2;
}

bool TBaseClothing::isPluralItem() const
{
  return (isPaired() && canWear(ITEM_WEAR_LEGS));
}

void TBaseClothing::purchaseMe(TBeing *ch, TMonster *keeper, int cost, int shop_nr)
{
  ch->addToMoney(-cost, GOLD_SHOP_ARMOR);
  if (!IS_SET(shop_index[shop_nr].flags, SHOP_FLAG_INFINITE_MONEY)) {
    keeper->addToMoney(cost, GOLD_SHOP_ARMOR);
  }

  shoplog(shop_nr, ch, keeper, getName(), cost, "buying");
}

void TBaseClothing::sellMeMoney(TBeing *ch, TMonster *keeper, int cost, int shop_nr)
{
  ch->addToMoney(cost, GOLD_SHOP_ARMOR);
  if (!IS_SET(shop_index[shop_nr].flags, SHOP_FLAG_INFINITE_MONEY))
    keeper->addToMoney(-cost, GOLD_SHOP_ARMOR);

  shoplog(shop_nr, ch, keeper, getName(), cost, "selling");
}

