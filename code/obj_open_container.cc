//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


// real_container.cc
//

#include "stdsneezy.h"
#include "create.h"

TOpenContainer::TOpenContainer() :
  TBaseContainer(),
  max_weight(0.0),
  container_flags(0),
  trap_type(DOOR_TRAP_NONE),
  trap_dam(0),
  key_num(-1),
  max_volume(0)
{
}

TOpenContainer::TOpenContainer(const TOpenContainer &a) :
  TBaseContainer(a),
  max_weight(a.max_weight),
  container_flags(a.container_flags),
  trap_type(a.trap_type),
  trap_dam(a.trap_dam),
  key_num(a.key_num),
  max_volume(a.max_volume)
{
}

TOpenContainer & TOpenContainer::operator=(const TOpenContainer &a)
{
  if (this == &a) return *this;
  TBaseContainer::operator=(a);
  max_weight = a.max_weight;
  container_flags = a.container_flags;
  trap_type = a.trap_type;
  trap_dam = a.trap_dam;
  key_num = a.key_num;
  max_volume = a.max_volume;
  return *this;
}

TOpenContainer::~TOpenContainer()
{
}

void TOpenContainer::assignFourValues(int x1, int x2, int x3, int x4)
{
  setCarryWeightLimit((float) x1);
  setContainerFlags(GET_BITS(x2, 7, 8));
  setContainerTrapType(mapFileToDoorTrap(GET_BITS(x2, 15, 8)));
  setContainerTrapDam(GET_BITS(x2, 23, 8));
  setKeyNum(x3);
  setCarryVolumeLimit(x4);
}

void TOpenContainer::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1 = (int) carryWeightLimit();

  int r = 0;
  SET_BITS(r, 7, 8, getContainerFlags());
  SET_BITS(r, 15, 8, mapDoorTrapToFile(getContainerTrapType()));
  SET_BITS(r, 23, 8, getContainerTrapDam());
  *x2 = r;
  *x3 = getKeyNum();
  *x4 = carryVolumeLimit();
}

string TOpenContainer::statObjInfo() const
{
  char buf[256];

  sprintf(buf, "Max Weight :%.3f, Max Volume : %d\n\rTrap type :%s (%d), Trap damage :%d\n\r",
          carryWeightLimit(),
          carryVolumeLimit(),
          trap_types[getContainerTrapType()],
          getContainerTrapType(),
          getContainerTrapDam());
  sprintf(buf + strlen(buf), "Vnum of key that opens: %d",
           getKeyNum());

  string a(buf);
  return a;
}

bool TOpenContainer::isCloseable() const
{
  return (isContainerFlag(CONT_CLOSEABLE));
}

bool TOpenContainer::isClosed() const
{
  return (isContainerFlag(CONT_CLOSED));
}

void TOpenContainer::setCarryWeightLimit(float r)
{
  max_weight = r;
}

float TOpenContainer::carryWeightLimit() const
{
  return max_weight;
}

unsigned char TOpenContainer::getContainerFlags() const
{
  return container_flags;
}

void TOpenContainer::setContainerFlags(unsigned char r)
{
  container_flags = r;
}

void TOpenContainer::addContainerFlag(unsigned char r)
{
  container_flags |= r;
}

void TOpenContainer::remContainerFlag(unsigned char r)
{
  container_flags &= ~r;
}

bool TOpenContainer::isContainerFlag(unsigned char r) const
{
  return ((container_flags & r) != 0);
}

doorTrapT TOpenContainer::getContainerTrapType() const
{
  return trap_type;
}

void TOpenContainer::setContainerTrapType(doorTrapT r)
{
  trap_type = r;
}

char TOpenContainer::getContainerTrapDam() const
{
  return trap_dam;
}

void TOpenContainer::setContainerTrapDam(char r)
{
  trap_dam = r;
}

void TOpenContainer::setKeyNum(int r)
{
  key_num = r;
}

int TOpenContainer::getKeyNum() const
{
  return key_num;
}

int TOpenContainer::carryVolumeLimit() const
{
  return max_volume;
}

void TOpenContainer::setCarryVolumeLimit(int r)
{
  max_volume = r;
}

void TOpenContainer::changeObjValue2(TBeing *ch)
{
  ch->specials.edit = CHANGE_CHEST_VALUE2;
  change_chest_value2(ch, this, "", ENTER_CHECK);
  return;
}

void TOpenContainer::describeContains(const TBeing *ch) const
{
  if (stuff && !isClosed())
    ch->sendTo(COLOR_OBJECTS, "%s seems to have something in it...\n\r", good_cap(getName()).c_str());
}

void TOpenContainer::lowCheck()
{
  if (carryWeightLimit() <= 0.0) {
    vlogf(LOG_LOW, "Container (%s) with bad weight limit (%5.2f).",
            getName(), carryWeightLimit());
  }
  if (carryVolumeLimit() <= 0) {
    vlogf(LOG_LOW, "Container (%s) with bad volume limit (%d).",
            getName(), carryVolumeLimit());
  }

  if (isContainerFlag(CONT_TRAPPED)) {
    if (getContainerTrapType() == DOOR_TRAP_NONE) {
      vlogf(LOG_LOW, "Container (%s:%d) trapped with no trap type.  Removing.",
           getName(), objVnum());
      remContainerFlag(CONT_TRAPPED);
    }
  }
  TBaseContainer::lowCheck();
}

void TOpenContainer::purchaseMe(TBeing *ch, TMonster *tKeeper, int tCost, int tShop)
{
  TObj::purchaseMe(ch, tKeeper, tCost, tShop);

  // This prevents 'just bought from store' bags from having
  // Ghost Traps unless they are already ghost trapped or have
  // a real trap on them.
  if (!isContainerFlag(CONT_TRAPPED) && !isContainerFlag(CONT_GHOSTTRAP))
    addContainerFlag(CONT_EMPTYTRAP);
}

string TOpenContainer::showModifier(showModeT tMode, const TBeing *tBeing) const
{
  // recurse if necessary
  string tString = TBaseContainer::showModifier(tMode, tBeing);

  if (isCloseable()) {
    tString += " (";                                          
    tString += isClosed() ? "closed" : "opened";
    tString += ")";                                           
  }                                                           
                                                              
  return tString;                                             
}                                                             

