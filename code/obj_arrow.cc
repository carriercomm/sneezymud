//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: obj_arrow.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#include "stdsneezy.h"
#include "create.h"

TArrow::TArrow() :
  TBaseWeapon(),
  arrowType(0),
  arrowHead(0),
  arrowHeadMat(0),
  arrowFlags(0)
{
}

TArrow::TArrow(const TArrow &a) :
  TBaseWeapon(a),
  arrowType(a.arrowType),
  arrowHead(a.arrowHead),
  arrowHeadMat(a.arrowHeadMat),
  arrowFlags(a.arrowFlags)
{
}

TArrow & TArrow::operator=(const TArrow &a)
{
  if (this == &a) return *this;
  TBaseWeapon::operator=(a);
  arrowType    = a.arrowType;
  arrowHead    = a.arrowHead;
  arrowHeadMat = a.arrowHeadMat;
  arrowFlags   = a.arrowFlags;
  return *this;
}

TArrow::~TArrow()
{
}

unsigned char TArrow::getArrowType() const
{
  return arrowType;
}

unsigned char TArrow::getArrowHead() const
{
  return arrowHead;
}

unsigned short TArrow::getArrowFlags() const
{
  return arrowFlags;
}

unsigned char TArrow::getArrowHeadMat() const
{
  return arrowHeadMat;
}

void TArrow::remArrowFlags(unsigned short n)
{
  arrowFlags &= ~n;
}

bool TArrow::isArrowFlag(unsigned short n)
{
  return arrowFlags & n;
}

void TArrow::addArrowFlags(unsigned short n)
{
  arrowFlags |= n;
}

void TArrow::setArrowType(unsigned int newArrowType)
{
  arrowType = newArrowType;
}

void TArrow::setArrowHead(unsigned char newArrowHead)
{
  arrowHead = newArrowHead;
}

void TArrow::setArrowFlags(unsigned short newArrowFlags)
{
  arrowFlags = newArrowFlags;
}

void TArrow::setArrowHeadMat(unsigned char newArrowHeadMat)
{
  arrowHeadMat = newArrowHeadMat;
}

bool TArrow::sellMeCheck(const TBeing *ch, TMonster *keeper) const
{
  int total = 0;
  TThing *t;
  char buf[256];

  for (t = keeper->stuff; t; t = t->nextThing) {
    if ((t->number == number) &&
        (t->getName() && getName() &&
         !strcmp(t->getName(), getName()))) {
      total += 1;
      if (total > 50) {
        sprintf(buf, "%s I already have plenty of those.", ch->name);
        keeper->doTell(buf);
        return TRUE;
      }
    }
  }
  return FALSE;
}

void TArrow::assignFourValues(int x1, int x2, int x3, int x4)
{
  TBaseWeapon::assignFourValues(x1, x2, x3, x4);

  setArrowType   (GET_BITS(x4,  3,  4));
  setArrowHead   (GET_BITS(x4,  7,  4));
  setArrowHeadMat(GET_BITS(x4, 16,  9));
  setArrowFlags  (GET_BITS(x4, 31, 15));
}

void TArrow::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  TBaseWeapon::getFourValues(x1, x2, x3, x4);

  int tValue = *x4;
  SET_BITS(tValue,  3,  4, getArrowType());
  SET_BITS(tValue,  7,  4, getArrowHead());
  SET_BITS(tValue, 16,  9, getArrowHeadMat());
  SET_BITS(tValue, 31, 15, getArrowFlags());
  *x4 = tValue;
}

string TArrow::displayFourValues()
{
  char tString[256];
  int  x1,
       x2,
       x3,
       x4;

  getFourValues(&x1, &x2, &x3, &x4);
  sprintf(tString, "Current values : %d %d %d %d\n\r", x1, x2, x3, x4);
  sprintf(tString + strlen(tString),
          "Current values : %d %d %d Size: %d Arrow-Head: %d Head-Material: %d Flags: %d",
          x1, x2, x3, getArrowType(), getArrowHead(), getArrowHeadMat(), getArrowFlags());

  return tString;
}

string TArrow::statObjInfo() const
{
  char buf[256];
  string a;

  sprintf(buf, "Damage Level: %.4f, Damage Deviation: %d\n\r",
           getWeapDamLvl() / 4.0,
           getWeapDamDev());
  a += buf;
  
  sprintf(buf, "Damage inflicted: %d\n\r",
           (int) damageLevel());
  a += buf;
  
  sprintf(buf, "Sharpness   : %d\n\r",
          getCurSharp());
  a += buf;

  sprintf(buf, "Arrow Type  : %d\n\r",
          getArrowType());
  a += buf;

  sprintf(buf, "Arrow Head  : %d",
          getArrowHead());
  a += buf;

  sprintf(buf, "Head Mat   : %d",
          getArrowHeadMat());
  a += buf;

  sprintf(buf, "Arrow Flags: %d",
          getArrowFlags());
  a += buf;

  return a;
}

int TArrow::putMeInto(TBeing *, TRealContainer *)
{
  return FALSE;
}

bool TArrow::engraveMe(TBeing *ch, TMonster *me, bool give)
{
  char buf[256];

  sprintf(buf, "%s Engraving this would destroy its aerodynamics.", ch->getName());
  me->doTell(buf);

  if (give) {
    strcpy(buf, name);
    add_bars(buf);
    sprintf(buf + strlen(buf), " %s", fname(ch->name).c_str());
    me->doGive(buf);
  }

  return TRUE;
}

int TArrow::throwMe(TBeing *ch, dirTypeT, const char *)
{
  act("$p isn't designed to be thrown.", FALSE, ch, this, 0, TO_CHAR);
  return FALSE;
}

spellNumT TArrow::getWtype() const
{
  return TYPE_PIERCE;
}

void TArrow::evaluateMe(TBeing *ch) const
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

  if (learn > 5)
    ch->describeMaxSharpness(this, learn);

  if (learn > 15) {
    ch->describeArrowSharpness(this, learn);
  }
  if (learn > 20)
    ch->describeMaxStructure(this, learn);

  if (learn > 30)
    ch->describeArrowDamage(this, learn);

  if (learn > 35)
    ch->describeWeaponDamage(this, learn);
}

void TArrow::bloadBowArrow(TBeing *ch, TThing *bow)
{
  bow->bloadArrowBow(ch, this);
}

int TArrow::suggestedPrice() const
{
  int amt = TBaseWeapon::suggestedPrice();
  return amt / 10;
}

void TArrow::changeObjValue4(TBeing *ch)
{
  ch->specials.edit = CHANGE_ARROW_VALUE4;
  change_arrow_value4(ch, this, "", ENTER_CHECK);
}
