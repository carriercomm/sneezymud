//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: thing.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.2  1999/09/16 22:14:37  peel
// Disable immortal check for lighting things with flint and steel
// ./
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


// thing.h

#include "stdsneezy.h"

const char * TThing::objs(const TThing *t) const
{
  return (canSee(t) ? t->getName() : "something");
}

const string TThing::objn(const TThing *t) const
{
  return (canSee(t) ? fname(t->name) : "something");
}

const char * TThing::ana() const
{
  return (strchr("aeiouyAEIOUY", *name) ? "An" : "A");
}

const char * TThing::sana() const
{
  return (strchr("aeiouyAEIOUY", *name) ? "an" : "a");
}

const char * TThing::pers(const TThing *t) const
{
  return (canSee(t) ? t->getName() : "someone");
}

const string TThing::persfname(const TThing *t) const
{
  return (canSee(t) ? fname(t->name) : "someone");
}

bool TThing::sameRoom(const TThing *ch) const
{
  return (inRoom() == ch->inRoom());
}

bool TThing::inImperia() const
{
  return (inRoom() >= 0 && inRoom() < 100);
}

bool TThing::inGrimhaven() const
{
  return ((inRoom() >= 100 && inRoom() < 950) ||
          (inRoom() >= 25400 && inRoom() <= 25499));
}

bool TThing::inLethargica() const
{
  return (inRoom() >= 23400 && inRoom() <= 23599);
}

bool TThing::isSpiked() const
{
  return (isname("spiked", name));
}

int TThing::swungObjectDamage(const TBeing *, const TBeing *) const
{
  int dam = 0;

  dam = (int) getWeight() / 5;
  dam = min(15, dam);
  return dam;
}

int TThing::useMe(TBeing *ch, const char *)
{
  ch->sendTo("Use is normally only for wands and magic staves.\n\r");
  return FALSE;
}

float TThing::getCarriedWeight() const
{
  return carried_weight;
}

int TThing::getCarriedVolume() const
{
  return carried_volume;
}

void TThing::setCarriedWeight(float num)
{
  carried_weight = num;
}

void TThing::setCarriedVolume(int num)
{
  carried_volume = num;
}

float TThing::getWeight() const
{
  return weight;
}

void TThing::setWeight(const float w)
{
  weight = w;
}

void TThing::addToWeight(const float w)
{
  weight += w;
}

void TThing::findComp(TComponent **, spellNumT)
{
}

void TThing::nukeFood()
{
  if (getMaterial() == MAT_FOODSTUFF)
    delete this;
}

int TThing::inRoom() const
{
  return in_room;
}

int TThing::getLight() const
{
  return light;
}

void TThing::setLight(int num)
{
  light = num;
}

void TThing::addToLight(int num)
{
  light += num;
}

void TThing::setRoom(int room)
{
  in_room = room;
}

void TThing::setMaterial(ubyte num)
{
  material_type = num;
}

ubyte TThing::getMaterial() const
{
  return material_type;
}

int TThing::getReducedVolume(const TThing *o) const
{
  // we use o if we want to do a check BEFORE the volume changes.
  // otherwise, (o == NULL) we fall into the parent case (normally done in += )
  // note: item might be in a bag and also have o == NULL
  // also: if item is in a bag, reduce volume by 5%

  int num = getTotalVolume();

  if ((o && dynamic_cast<const TContainer *>(o)) ||
      (parent && dynamic_cast<const TContainer *>(parent))) {
    // do material type reduction
    num /= material_nums[getMaterial()].vol_mult;

    // we know we are in a bag here, reduce by 5%
    num *= 95;
    num /= 100;
  }

  return num;
}

int TThing::getTotalVolume() const
{
  return getVolume();
}

float TThing::getTotalWeight(bool pweight) const
{
  float calc = getCarriedWeight();

  if (pweight)
    calc += getWeight();

  return calc;
}

void TThing::peeOnMe(const TBeing *ch)
{
  ch->sendTo("Piss on what?!\n\r");
}

void TThing::lightMe(TBeing *ch, silentTypeT)
{
  if(!material_nums[getMaterial()].flammability){
    act("You can't light $p, it's not flammable!", FALSE, ch, this, 0, TO_CHAR);
    return;
  } else {
    TThing *t;
    TTool *flintsteel;

    if (!(t = get_thing_char_using(ch, "flintsteel", 0, FALSE, FALSE)) ||
        !(flintsteel=dynamic_cast<TTool *>(t))){
      ch->sendTo("You need to own some flint and steel to light that.\n\r");
      return;
    }

    ch->sendTo("You begin to start a fire.\n\r");
    start_task(ch, this, NULL, TASK_LIGHTFIRE, "", 2, ch->inRoom(), 0, 0, 5);
  }
}

