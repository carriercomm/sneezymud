//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: organic.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


// organic.cc

#include "stdsneezy.h"
#include "shop.h"

TOrganic::TOrganic() :
  TObj(),
  OCType(ORGANIC_NONE),
  TUnits(0),
  OLevel(0),
  TAEffect(0)
{
}

TOrganic::TOrganic(const TOrganic &a) :
  TObj(a),
  OCType(a.OCType),
  TUnits(a.TUnits),
  OLevel(a.OLevel),
  TAEffect(a.TAEffect)
{
}

TOrganic & TOrganic::operator=(const TOrganic &a)
{
  if (this == &a) return *this;
  TObj::operator=(a);
  OCType   = a.OCType;
  TUnits   = a.TUnits;
  OLevel   = a.OLevel;
  TAEffect = a.TAEffect;
  return *this;
}

TOrganic::~TOrganic()
{
}

void TOrganic::assignFourValues(int x1, int x2, int x3, int x4)
{
  setOType(organicTypeT(x1));
  setUnits(x2);
  setOLevel(x3);
  setAEffect(x4);
}

void TOrganic::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1 = getOType();
  *x2 = getUnits();
  *x3 = getOLevel();
  *x4 = getAEffect();
}

string TOrganic::statObjInfo() const
{
  int  MType = 9;
  char Buf[256];

  const char *type[] = {
    "Undefined",
    "Raw Skin (Hide)",
    "Refined Skin (True Hide)",
    "Wood",
    "Bone",
    "Coral",
    "Silk",
    "Wool",
    "Herbal Ingrediant",
    "General Purpose"};

  sprintf(Buf, "Type: %s (%d)  Units: %d  Level: %d  AEffect: %d",
          (getOType() > MType ? "Unknown" : type[getOType()]), getOType(),
          getUnits(),
          getOLevel(),
          getAEffect());

  string a(Buf);
  return a;
}

// Class specific functions:

// Does what it says.
void TOrganic::lightMe(TBeing *ch, silentTypeT iSilent)
{
  TThing *toObj = this;

  // This is where TFFlame comes in.  If you try to light a OType 3
  // Then we want to goto igniteObject, otherwise the standard lightMe.
  if (getOType() != ORGANIC_WOOD)
    TObj::lightMe(ch, iSilent);
  else
    ch->igniteObject("", toObj);
}

// Determine Sell[PC selling] value
int TOrganic::sellPrice(int shop_nr, int, int *)
{
#if 1
  int price;

  // price the shopkeeper will Pay
  price = max(1, (int) (obj_flags.cost * shop_index[shop_nr].profit_sell));

  return price;
#else
  int cost_per;
  int price;

  cost_per = pricePerUnit();
  price = (int) (numUnits() * cost_per * shop_index[shop_nr].profit_sell);

  if (obj_flags.cost <= 1) {
    price = max(0, price);
  } else {
    price = max(1, price);
  }

  return price;
#endif
}

// Determine Buy[Shop selling] value
int TOrganic::shopPrice(int num, int shop_nr, int, int *) const
{
#if 1
  int price;

  // price the shopkeeper will Want
  price = max(1, (int) (obj_flags.cost * shop_index[shop_nr].profit_buy));

  return price;
#else
  int cost_per;
  int price;

  cost_per = pricePerUnit();
  price = (int) (num * cost_per * shop_index[shop_nr].profit_buy);
  price = max(1, price);

  return price;
#endif
}

// This function deals with the buying of these things.
void TOrganic::buyMe(TBeing *ch, TMonster *keeper, int num, int shop_nr)
{
  char Buf[2][256];
  int price,
      vnum,
      discount = 0,
      nCost,
      nVolume,
      nWeight;
  TObj *obj2 = NULL;
  TOrganic *nOrg = NULL;
  string nocName("");
  nVolume = (int) getVolume();
  nWeight = (int) getWeight();
  nCost   = (int) obj_flags.cost;

  if (parent != keeper) {
    vlogf(8, "Error: buyMe():organic.cc  obj not on keeper");
    return;
  }
  // Make sure it's an item we buy.
  if (!trade_with(this, shop_nr)) {
    sprintf(Buf[0], shop_index[shop_nr].do_not_buy, ch->getName());
    keeper->doTell(Buf[0]);
    return;
  }
  nocName = getNameNOC(ch);
  // If it's a 'unit' item, then treat it as such.
  if (getUnits() > 0) {
    if (num > getUnits() && !shop_producing(this, shop_nr)) {
      num = getUnits();
      sprintf(Buf[0], "%s I don't have that much of %s.  Here's the %d that I do have.",
	      ch->getName(), nocName.c_str(), num);
      keeper->doTell(Buf[0]);
    }
  }
  // cost_per = pricePerUnit();
  // price = shopPrice(num, shop_nr, -1, &discount);
  price = shopPrice(num, shop_nr, 0, &discount);
  vnum = objVnum();
  if (ch->getMoney() < price) {
    sprintf(Buf[0], shop_index[shop_nr].missing_cash2, ch->name);
    keeper->doTell(Buf[0]);

    switch (shop_index[shop_nr].temper1) {
      case 0:
        keeper->doAction(ch->name, CMD_SMILE);
        return;
      case 1:
        act("$n grins happily.", 0, keeper, 0, 0, TO_ROOM);
        return;
      default:
        return;
    }
  }
  strcpy(Buf[1], shortDescr);

  if (!shop_producing(this, shop_nr))
    --(*this);

  // more 'unit' special code.
  if (getUnits() > 0 && !shop_producing(this, shop_nr)) {
    int num2 = getUnits() - num;
    if (num2) {
      setVolume(max(1, (int) (getVolume()/getUnits()*num2)));
      setWeight(max(1, (int) (getWeight()/getUnits()*num2)));
      obj_flags.cost = max(2, (int) (obj_flags.cost/getUnits()*num2));
      setUnits(num2);
      nVolume = max(1, (int) (nVolume-getVolume()));
      nWeight = max(1, (int) (nWeight-getWeight()));
      nCost   = max(1, (int) (nCost-obj_flags.cost));
      *keeper += *this;
    } else
      delete this;
  }
  // and more 'unit' special code, but this is also the main block for getting cash,
  // giving item.
  if (num || getUnits() <= 0) {
    if (getUnits() > 0) {
      obj2 = read_object(vnum, VIRTUAL);
      nOrg = dynamic_cast<TOrganic *>(obj2);
      nOrg->setUnits(num);
      nOrg->setOLevel(getOLevel());
      obj2->setVolume(nVolume);
      obj2->setWeight(nWeight);
      obj2->obj_flags.cost = nCost;
      *ch += *obj2;
      sprintf(Buf[0], "%s Here ya go.  That's %d unit%s of %s for %d talen%s.",
	      ch->getName(), num, (num > 1 ? "s" : ""), nocName.c_str(),
              price, (price > 1 ? "s" : ""));
      keeper->doTell(Buf[0]);
      act("$n buys $p.", TRUE, ch, obj2, keeper, TO_NOTVICT);
    } else {
      // Must not have been a unit item, just give them the item in question.
      if (!shop_producing(this, shop_nr))
        *ch += *this;
      else {
        obj2 = read_object(this->objVnum(), VIRTUAL);
        *ch += *obj2;
      }
      sprintf(Buf[0], "%s Here ya go.  Thanks for shopping with us.", ch->getName());
      keeper->doTell(Buf[0]);
      act("$n buys $p.", TRUE, ch, this, keeper, TO_NOTVICT);
    }

    ch->addToMoney(-price, GOLD_COMM);

    if (!IS_SET(shop_index[shop_nr].flags, SHOP_FLAG_INFINITE_MONEY))
      keeper->addToMoney(price, GOLD_COMM);
  } else {
    // this happens with sub zero weight components
    vlogf(5, "Bogus num %d in buyMe component at %d.  wgt=%.2f", num, ch->in_room, getWeight());
  }

  sprintf(Buf[0], "%s/%d", SHOPFILE_PATH, shop_nr);
  keeper->saveItems(Buf[0]);
  ch->doSave(SILENT_YES);
  return;
}

// used by sell all.hide and sell all.skin
int TOrganic::sellHidenSkin(TBeing *ch, TMonster *keeper, int shop_nr, TThing *obj)
{
  int rc;

  if (equippedBy)
    *ch += *ch->unequip(eq_pos);

  if (obj) {
    rc = get(ch, this, obj);
    if (IS_SET_DELETE(rc, DELETE_ITEM))
      return DELETE_THIS;

    if (IS_SET_DELETE(rc, DELETE_VICT))
      return DELETE_ITEM;

    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_VICT;

    // This is to make sure that volume/weight didn't prevent the upper get.
    if (!parent || dynamic_cast<TBeing *>(parent))
      generic_sell(ch, keeper, this, shop_nr);
  } else
    generic_sell(ch, keeper, this, shop_nr);

  return FALSE;
}

static void sellReducePrice(const TBeing *ch, TBeing *keeper, const TOrganic *obj2, int &price)
{
  char Buf[256];

  if (obj2 && obj2->getUnits() >= 100) {
    int bnCost = max(1, (int) (price/4));
    sprintf(Buf, "%s Well it would seem I have a surplus of that hide...", ch->getName());
    keeper->doTell(Buf);
    if (bnCost != price) {
      sprintf(Buf, "%s Afraid I can not pay you full price for it.", ch->getName());
      keeper->doTell(Buf);
      price = bnCost;
    }
  }
}

// This function deals with the selling of TOrganic stuff.
void TOrganic::sellMe(TBeing *ch, TMonster *keeper, int shop_nr)
{
  TThing   *t;
  TOrganic *obj2 = NULL;
  int  num = 1,
       price,
       found = 0,
       discount;
  char Buf[256];

  if (getUnits() > 0)
    price = min(shopPrice(max(1, getUnits()), shop_nr, 0, &discount),
                sellPrice(shop_nr, 0, &discount));
  else
    price = sellPrice(shop_nr, 0, &discount);

  if (isObjStat(ITEM_NODROP)) {
    ch->sendTo("You can't let go of it, it must be CURSED!\n\r");
    return;
  }
  if (isObjStat(ITEM_PROTOTYPE)) {
    ch->sendTo("That's a prototype, no selling that!\n\r");
    return;
  }
  if (objVnum() <= 0) {
    ch->sendTo("You shouldn't try selling that, bad bad builder.\n\r");
    return;
  }
  if (will_not_buy(ch, keeper, this, shop_nr))
    return;
  if (!trade_with(this, shop_nr)) {
    sprintf(Buf, shop_index[shop_nr].do_not_buy, ch->getName());
    keeper->doTell(Buf);
    return;
  }
  if (keeper->getMoney() < price) {
    sprintf(Buf, shop_index[shop_nr].missing_cash1, ch->getName());
    keeper->doTell(Buf);
    return;
  }
  // See if the shop keeper already has one of these items.
  // This is mainly for 'unit' code.
  for (t = keeper->stuff; t; t = t->nextThing) {
    TOrganic *obj3 = dynamic_cast<TOrganic *>(t);
    if (!obj3)
      continue;

    if (obj3->objVnum() == objVnum()) {
      found++;
      if (found == 1)
        obj2 = dynamic_cast<TOrganic *>(t);
    }
  }
  if (found >= 20) {
    sprintf(Buf, "%s I'm afraid I already have too many of those, sorry.", ch->getName());
    keeper->doTell(Buf);
    return;
  }
  if (getUnits() > 0) {
    if (!obj2) {
      TObj *to = read_object(objVnum(), VIRTUAL);
      obj2 = dynamic_cast<TOrganic *>(to);
      obj2->setWeight(0.0);
      obj2->setVolume(0.0);
      obj2->obj_flags.cost = 0;
    } else
      --(*obj2);
  } else
    obj2 = this;

  num = obj2->getUnits() + getUnits();
  if (getUnits() > 0) {
    obj2->setOLevel(max(1, (int) ((obj2->getOLevel()+getOLevel())/2)));
    obj2->obj_flags.cost = max(2, obj2->obj_flags.cost+obj_flags.cost);
    // We need to preserve the weight&volume for when we sell it later on.
    // Doing it this way is less time/resource consuming than loading up
    // a fake, recalculating for 1 unit then multiplying it by how many
    // we sell.  This way we simply divide by and assign.
    obj2->setWeight(obj2->getWeight() + getWeight());
    obj2->setVolume(obj2->getVolume() + getVolume());
    if (num > 100) {
      int per_Unit = (int) (obj2->obj_flags.cost/num);
      double left_Over = 100.0 / (double) (num);
      obj2->obj_flags.cost = per_Unit*100;
      // We want the NEW weight/volume, not what it Would have been if we had
      // more than 100 units here.
      obj2->setWeight((int) (obj2->getWeight() * left_Over));
      obj2->setVolume((int) (obj2->getVolume() * left_Over));
      num = 100;
    } else if (num < 0) {
      num = 0;
      obj2->obj_flags.cost = 0;
    }
    obj2->setUnits(num);
  }
  if (num || getUnits() <= 0) {
    --(*this);

    sellReducePrice(ch, keeper, obj2, price);

    if (!IS_SET(shop_index[shop_nr].flags, SHOP_FLAG_INFINITE_MONEY))
      keeper->addToMoney(-price, GOLD_COMM);
    ch->addToMoney(price, GOLD_COMM);
    sprintf(Buf, "%s Thanks, here's your %d talen%s.",
            ch->getName(), price, (price > 1 ? "s" : ""));
    keeper->doTell(Buf);
    act("$n sells $p.", TRUE, ch, this, 0, TO_ROOM);
    if (ch->isAffected(AFF_GROUP) && ch->desc &&
        IS_SET(ch->desc->autobits, AUTO_SPLIT) &&
        (ch->master || ch->followers)) {
      sprintf(Buf, "%d", price);
      ch->doSplit(Buf, false);
    }
    if (getUnits() > 0) {
      *keeper += *obj2;
      delete this;
    } else if (!shop_producing(this, shop_nr))
      *keeper += *this;
  }

  ch->doSave(SILENT_YES);
  sprintf(Buf, "%s/%d", SHOPFILE_PATH, shop_nr);
  keeper->saveItems(Buf);
  return;
}

// Used by the value command
void TOrganic::valueMe(TBeing *ch, TMonster *keeper, int shop_nr)
{
  int  price,
       discount = 0;
  char Buf[256];
  TThing   *t;
  TOrganic *obj2 = NULL;

  if (getUnits() > 0)
    price = min(shopPrice(max(1, getUnits()), shop_nr, 0, &discount),
                sellPrice(shop_nr, 0, &discount));
  else
    price = sellPrice(shop_nr, 0, &discount);

  if (!trade_with(this, shop_nr)) {
    sprintf(Buf, shop_index[shop_nr].do_not_buy, ch->getName());
    keeper->doTell(Buf);
    return;
  }

  for (t = keeper->stuff; t; t = t->nextThing) {
    obj2 = dynamic_cast<TOrganic *>(t);
    if (!obj2)
      continue;

    if (obj2->objVnum() == objVnum())
      break;
  }

  sellReducePrice(ch, keeper, obj2, price);

  sprintf(Buf, "%s Hmm, I'd give you %d talen%s for that.",
          ch->getName(), price, (price > 1 ? "s" : ""));
  keeper->doTell(Buf);
  return;
}

// Used by the list command
const string TOrganic::shopList(const TBeing *ch, const char *arg,
                                int min_amt, int max_amt, int num,
                                int shop_nr, int k, unsigned long int) const
{
  char Buf[2][256];
  char tString[256];
  bool usePlural = false;
  int cost = shopPrice(num, shop_nr, 0, &num);

  sprintf(Buf[1], "%s", shortDescr);

  if (strlen(Buf[1]) > 31) {
    Buf[1][28] = '\0';
    strcat(Buf[1], "...");
  }

  if (getUnits() > 0) {
    if (shop_producing(this, shop_nr)) {
      strcpy(tString, "unlim");
      usePlural = true;
    } else {
      sprintf(tString, "%5d", getUnits());
      usePlural = (getUnits() > 1 ? true : false);
    }
  } else {
    if (shop_producing(this, shop_nr))
      strcpy(tString, "unlimited");
    else
      sprintf(tString, "%d", num);

    usePlural = (cost > 1 ? true : false);
  }

  if (getUnits() > 0)
    sprintf(Buf[0], "[%2d] %-31s  <Z>: %s unit%c %5d talen%c (per unit)\n\r",
            k + 1, Buf[1], tString, (usePlural ? 's' : ' '),
            cost, (cost > 1 ? 's' : ' '));
  else
    sprintf(Buf[0], "[%2d] %-31s  <Z>:             %5d talen%c [%s]\n\r",
            k + 1, Buf[1], cost, (usePlural ? 's' : ' '),tString);

  if (!*arg && min_amt == 999999)     // everything
    // specific item
    return Buf[0];
  else if (isname(arg, name) && min_amt == 999999)
    return Buf[0];
  // specific item and specific cost
  else if (isname(arg, name) && cost >= min_amt && cost <= max_amt)
    return Buf[0];
  else if (!*arg && cost >= min_amt && cost <= max_amt)   // specific cost
    return Buf[0];

  return "";
}

// This is our general check to make sure it's Sellable.
int TOrganic::objectSell(TBeing *ch, TMonster *keeper)
{/*
  char Buf[256];

  // We just don't do this, wood items should be hunted down when there
  // needed or held for later use, no ranger worth his salt is going to
  // really Trade in them.
  if (getOType() == ORGANIC_WOOD) {
    if ((getAEffect() > 0 && ch->isImmortal()) || ch->isImmortal()) {
      // Eventually I will allow mortals to sell logs with 'special' affects on them.
      sprintf(Buf, "%s I don't normally buy wood items, but I'll make an exception...",
              ch->getName());
      keeper->doTell(Buf);
      return FALSE;
    } else {
      sprintf(Buf, "%s I'm afraid I don't buy wood items...", ch->getName());
      keeper->doTell(Buf);
      return TRUE;
    }
  }
  // This is here until I get the code for non-unit organics in.
  if (getUnits() <= 0 && !ch->isImmortal()) {
    sprintf(Buf, "%s I'm afraid I don't deal in these items at the moment.", ch->getName());
    keeper->doTell(Buf);
    return TRUE;
  }
 */
  return FALSE; // If we hit here, we Must be doing a unitary item, so allow it.
}

void TOrganic::describeObjectSpecifics(const TBeing *ch) const
{
}

bool TOrganic::splitMe(TBeing *ch, const char *argument)
{
  char Buf[256];
  int  num = 2,
       per_Cost,
       per_Weight,
       nUnits = 0,
       per_Volume,
       uWeiVol[2];
  bool canHold = true;

  if (getUnits() <= 0)
    return false;

  argument = one_argument(argument, Buf); // Ditch the item name.
  argument = one_argument(argument, Buf); // Get the split count.

  if (is_number(Buf))
    num = atoi(Buf);

  double Szmod = ((double) ch->GetMaxLevel()/2.0/100);
  nUnits     = (int) ((getUnits()*(.70+Szmod))/num);
  per_Cost   = (int) (obj_flags.cost/getUnits() * nUnits);
  per_Weight = (int) (getWeight()   /getUnits() * nUnits);
  per_Volume = (int) (getVolume()   /getUnits() * nUnits);
  if (num > 10) {
    ch->sendTo("That's alot of work for just one split, why not do a few small ones?\n\r");
    return true;
  }
  if (!nUnits || !per_Weight || !per_Volume || !per_Cost) {
    ch->sendTo("That hide is just not big enough to be split so much.\n\r");
    return true;
  }
  ch->sendTo(COLOR_OBJECTS, "You split %s into %d pieces.\n\r",
             shortDescr, num);
  sprintf(Buf, "$n splits %s into %d pieces.",
          shortDescr, num);
  act(Buf, TRUE, ch, 0, 0, TO_ROOM);

  // Lets get the users Weight&Volume so we can decide if the new hide goes on the
  // floor or in there inventory.
  uWeiVol[0] = (int) (ch->carryWeightLimit() - ch->getCarriedWeight());
  uWeiVol[1] = ch->carryVolumeLimit() - ch->getCarriedVolume();

  for (int run = 1; run < num; run++) {
    TOrganic *obj2 = NULL;

    if (objVnum() == -1 || !(obj2 = dynamic_cast<TOrganic *>(read_object(objVnum(), VIRTUAL)))) {
      ch->sendTo("Tell a god what you did, it was bad.\n\r");
      return true;
    }
    obj2->setWeight(per_Weight);
    obj2->setVolume(per_Volume);
    obj2->setUnits(nUnits);
    obj2->obj_flags.cost = per_Cost;
    obj2->setOLevel(getOLevel());
    obj2->setAEffect(getAEffect());

    if (canHold && (obj2->getTotalVolume()-obj2->getReducedVolume(NULL)) > uWeiVol[1]) {
      ch->sendTo("Unfortunatly the sheer amount of hide gets too much for you.\n\r");
      act("You leave the rest of the hide on the $g.",
          TRUE, ch, 0, 0, TO_CHAR);
      canHold = false;
    }
    if (canHold && !compareWeights(obj2->getTotalWeight(TRUE), uWeiVol[0])) {
      ch->sendTo("Unfortunatly the sheer weight of hide gets too much for you.\n\r");
      act("You leave the rest of the hide on the $g.",
           TRUE, ch, 0, 0, TO_CHAR);
      canHold = false;
    }
    if (canHold)
      *ch += *obj2;
    else
      *ch->roomp += *obj2;
  }
  setWeight(per_Weight);
  setVolume(per_Volume);
  setUnits(nUnits);
  obj_flags.cost = per_Cost;

  return true;
}

void TOrganic::setOType(organicTypeT x1)
{
  OCType = x1;
}

organicTypeT TOrganic::getOType() const
{
  return OCType;
}

void TOrganic::setUnits(int x2)
{
  TUnits = x2;
}

int TOrganic::getUnits() const
{
  return TUnits;
}

void TOrganic::setOLevel(int x3)
{
  OLevel = x3;
}

int TOrganic::getOLevel() const
{
  return OLevel;
}

void TOrganic::setAEffect(int x4)
{
  TAEffect = x4;
}

int TOrganic::getAEffect() const
{
  return TAEffect;
}
