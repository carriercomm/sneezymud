#include "stdsneezy.h"

/* todo
light reduction
drifting
coughing etc
rain put out fire
 */


TSmoke::TSmoke() :
  TObj()
{
}

TSmoke::TSmoke(const TSmoke &a) :
  TObj(a)
{
}

TSmoke & TSmoke::operator=(const TSmoke &a)
{
  if (this == &a) return *this;
  TObj::operator=(a);
  return *this;
}

TSmoke::~TSmoke()
{
}

void TSmoke::setVolume(int n)
{
  TObj::setVolume(n);
  updateDesc();
  obj_flags.decay_time=getVolume();
}

void TSmoke::addToVolume(int n)
{
  TObj::addToVolume(n);
  updateDesc();
  obj_flags.decay_time=getVolume();
}


int TSmoke::getSizeIndex() const
{
  int volume=getVolume();

  if(volume<=0){
    return 0;
  } else if(volume<=5){
    return 1;
  } else if(volume<=10){
    return 2;
  } else if(volume<=25){
    return 3;
  } else if(volume<=50){ 
    return 4;
  } else if(volume<=100){
    return 5;
  } else if(volume<=250){
    return 6;
  } else if(volume<=1000){
    return 7;
  } else if(volume<=10000){
    return 8;
  } else if(volume<=25000){
    return 9;
  } else {
    return 10;
  }
}
  
void TSmoke::updateDesc()
{
  int sizeindex=getSizeIndex();
  char buf[256];
  
  const char *smokename [] =
  {
    "<k>a few wisps of smoke<1>", 
    "<k>a tiny cloud of smoke<1>", 
    "<k>a small cloud of smoke<1>", 
    "<k>a cloud of smoke<1>", 
    "<k>a fair sized cloud of smoke<1>", 
    "<k>a big cloud of smoke<1>", 
    "<k>a large cloud of smoke<1>", 
    "<k>a huge cloud of smoke<1>",
    "<k>a massive cloud of smoke<1>",
    "<k>a tremendously huge cloud of smoke<1>",
    "<k>a room full of smoke<1>"
  };
  
  const char *smokedesc [] =
  {
    "<k>A few wips of smoke are fading fast.<1>",
    "<k>A tiny cloud of smoke has gathered here.<1>",
    "<k>A small cloud of smoke is here.<1>",
    "<k>A cloud of smoke is here.<1>",
    "<k>A fair sized cloud of smoke is here.<1>",
    "<k>A big cloud of smoke is here.<1>",
    "<k>A large cloud of smoke is here.<1>",
    "<k>A huge cloud of smoke is here.<1>",
    "<k>A massive cloud of smoke is here.<1>",
    "<k>A tremendously huge cloud of smoke dominates the area.<1>",
    "<k>The whole area is filled with smoke.<1>"
  };

  if (isObjStat(ITEM_STRUNG)) {
    delete [] shortDescr;
    delete [] descr;

    extraDescription *exd;
    while ((exd = ex_description)) {
      ex_description = exd->next;
      delete exd;
    }
    ex_description = NULL;
    delete [] action_description;
    action_description = NULL;
  } else {
    addObjStat(ITEM_STRUNG);
    name = mud_str_dup(obj_index[getItemIndex()].name);
    ex_description = NULL;
    action_description = NULL;
  }

  sprintf(buf, smokename[sizeindex]);
  shortDescr = mud_str_dup(buf);

  sprintf(buf, smokedesc[sizeindex]);
  setDescr(mud_str_dup(buf));
}

bool TSmoke::isPluralItem() const
{
  // a few drops of blood
  if (getSizeIndex() == 0)
    return TRUE;

  // otherwise, make recursive
  return TObj::isPluralItem();
}

int TThing::dropSmoke(int amt)
{
  TSmoke *smoke=NULL;
  TThing *t;
  char buf[256];
  TObj *obj;

  if(amt==0 || !roomp)
    return FALSE;

  // look for preexisting smoke
  for(t=roomp->stuff;t;t=t->nextThing){
    if((smoke = dynamic_cast<TSmoke *>(t)))
      break;
  }

  if (!smoke) {
    // create new smoke
#if 1
// builder port uses stripped down database which was causing problems
// hence this setup instead.
    int robj = real_object(GENERIC_SMOKE);
    if (robj < 0 || robj >= (signed int) obj_index.size()) {
      vlogf(LOG_BUG, "dropSmoke(): No object (%d) in database!", GENERIC_SMOKE);
      return false;
    }

    if (!(obj = read_object(robj, REAL))) {
      vlogf(LOG_LOW, "Error, No GENERIC_SMOKE  (%d)", GENERIC_SMOKE);
      return FALSE;
    }
#else
    if (!(obj = read_object(GENERIC_SMOKE, VIRTUAL))) {
      vlogf(LOG_LOW, "Error, No GENERIC_SMOKE  (%d)", GENERIC_SMOKE);
      return FALSE;
    }
#endif
    if (!(smoke = dynamic_cast<TSmoke *>(obj))) {
      vlogf(LOG_BUG, "Error, unable to cast object to smoke: smoke.cc:TThing::dropSmoke");
      return FALSE;
    }
    smoke->swapToStrung();
    smoke->remObjStat(ITEM_TAKE);
    smoke->canBeSeen = 1;
    smoke->setMaterial(MAT_GHOSTLY);

    sprintf(buf, "smoke cloud");
    delete [] smoke->name;
    smoke->setName(mud_str_dup(buf));

    *roomp += *smoke;
  }

  smoke->addToVolume(amt);

  return TRUE;
}

void TSmoke::decayMe()
{
  int volume = getVolume();

  if (!roomp) {
    vlogf(LOG_BUG, "TSmoke::decayMe() called while TSmoke not in room!");
    setVolume(0);
    return;
  }

  if (volume <= 0)
    setVolume(0);
  else if (volume < 25)
    addToVolume((roomp->isIndoorSector() ? -1 : -3));
  else // large smokes evaporate faster
    addToVolume((roomp->isIndoorSector() ? -(volume/25) : -(volume/15))); 

  if (getVolume() < 0)
    setVolume(0);
}

string TSmoke::statObjInfo() const
{
  return "";
}

void TSmoke::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1 = 0;
  *x2 = 0;
  *x3 = 0;
  *x4 = 0;
}

void TSmoke::assignFourValues(int, int, int, int)
{
}
