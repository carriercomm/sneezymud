///////////////////////////////////////////////////////////////////////////
//
//      SneezyMUD++ - All rights reserved, SneezyMUD Coding Team
//      "drug.cc" - Methods for TDrug class
//
///////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"

TDrug::TDrug() :
  TObj(),
  curFuel(0),
  maxFuel(0),
  drugType(DRUG_NONE)
{
}

TDrug::TDrug(const TDrug &a) :
  TObj(a),
  curFuel(a.curFuel),
  maxFuel(a.maxFuel),
  drugType(a.drugType)
{
}

TDrug & TDrug::operator=(const TDrug &a)
{
  if (this == &a) return *this;

  TObj::operator=(a);
  curFuel = a.curFuel;
  maxFuel = a.maxFuel;
  drugType = a.drugType;
  return *this;
}

TDrug::~TDrug()
{
}

void TDrug::addToMaxFuel(int n)
{
  maxFuel += n;
}

void TDrug::setMaxFuel(int n)
{
  maxFuel = n;
}

int TDrug::getMaxFuel() const
{
  return maxFuel;
}

void TDrug::addToCurFuel(int n)
{
  curFuel += n;
}

void TDrug::setCurFuel(int n)
{
  curFuel = n;
}

int TDrug::getCurFuel() const
{
  return curFuel;
}

void TDrug::setDrugType(drugTypeT n)
{
  drugType = n;
}

drugTypeT TDrug::getDrugType() const
{
  return drugType;
}

void TDrug::describeObjectSpecifics(const TBeing *ch) const
{
  double diff;

  if (getMaxFuel()) {
    diff = (double) ((double) getCurFuel() / (double) getMaxFuel());
    ch->sendTo(COLOR_OBJECTS,
          "You can tell that %s has %s of its %s left.\n\r",
          good_uncap(getName()).c_str(),
          ((diff < .20) ? "very little" : ((diff < .50) ? "some" :
          ((diff < .75) ? "a good bit of" : "almost all of its"))),
	       drugTypes[getDrugType()].name);
  }
}

void TDrug::refuelMeDrug(TBeing *ch, TDrugContainer *lamp)
{
  int use;
  char buf[256];

  if (lamp->getMaxBurn() < 0) {
    act("$p can't be refueled.", FALSE, ch, lamp, 0, TO_CHAR);
    return;
  }
  if (lamp->getCurBurn() == lamp->getMaxBurn()) {
    act("$p is already full of fuel.", FALSE, ch, lamp, 0, TO_CHAR);
    return;
  }
  if (((lamp->getDrugType() != getDrugType()) && lamp->getCurBurn()>0) ||
      getDrugType()==DRUG_NONE) {
    ch->sendTo("Mixing drugs would not be wise.\n\r");
    return;
  }

  use = lamp->getMaxBurn() - lamp->getCurBurn();
  use = min(use, getCurFuel());

  sprintf(buf, "$n packs some %s into $s $o", drugTypes[getDrugType()].name);
  act(buf, TRUE, ch, lamp, 0, TO_ROOM);
  ch->sendTo("You pack some %s into the %s.\n\r", drugTypes[getDrugType()].name,
	     fname(lamp->name).c_str());

  addToCurFuel(-use);
  lamp->addToCurBurn(use);
  lamp->setDrugType(getDrugType());

  if (getCurFuel() <= 0) {
    ch->sendTo("Your %s is all used up, and you discard it.\n\r",
	       drugTypes[getDrugType()].name);
    if (equippedBy) {
      dynamic_cast<TBeing *>(equippedBy)->unequip(eq_pos);
    } else
      --(*this);

    delete this;
  }
}

void TDrug::assignFourValues(int x1, int x2, int x3, int)
{
  setCurFuel(x1);
  setMaxFuel(x2);

  drugTypeT dtt = mapFileToDrug(x3);
  setDrugType(dtt);
}

void TDrug::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1 = getCurFuel();
  *x2 = getMaxFuel();
  *x3 = mapDrugToFile(getDrugType());
  *x4 = 0;
}

void TDrug::lowCheck()
{
  if (getCurFuel() > getMaxFuel())
    vlogf(LOG_LOW,"fuel %s had more current fuel than max.", getName());

  TObj::lowCheck();
}

int TDrug::objectSell(TBeing *ch, TMonster *keeper)
{
  char buf[256];

  sprintf(buf, "%s I'm sorry, I don't buy back drugs.", ch->getName());
  keeper->doTell(buf);
  return TRUE;
}

string TDrug::statObjInfo() const
{
  char buf[256];

  sprintf(buf, "Refuel capability : current : %d, max : %d",
         getCurFuel(), getMaxFuel());

  string a(buf);
  return a;
}

int TDrug::getVolume() const
{
  int amt = TObj::getVolume();

  amt *= getCurFuel();
  if(getMaxFuel())
    amt /= getMaxFuel();

  return amt;
}

float TDrug::getTotalWeight(bool pweight) const
{
  float amt = TObj::getTotalWeight(pweight);

  amt *= getCurFuel();
  if(getMaxFuel())
    amt /= getMaxFuel();

  return amt;
}

drugData::drugData() :
  first_use(),
  last_use(),
  total_consumed(0),
  current_consumed(0)
{
}

drugData::~drugData()
{
}

drugData::drugData(const drugData &t) :
  first_use(t.first_use),
  last_use(t.last_use),
  total_consumed(t.total_consumed),
  current_consumed(t.current_consumed)
{
}

drugData & drugData::operator =(const drugData &t)
{
  if (this == &t) return *this;

  first_use=t.first_use;
  last_use=t.last_use;
  total_consumed=t.total_consumed;
  current_consumed=t.current_consumed;

  return *this;
}

affectedData *findDrugAffect(TBeing *ch, int drug, int type){
  affectedData *hjp;
  for (hjp = ch->affected; hjp; hjp = hjp->next) {
    if (hjp->type==AFFECT_DRUG && hjp->modifier2==drug && hjp->location==type)
      return hjp;
  }
  return NULL;
}

void reapplyDrugAffect(TBeing *ch, affectedData *affptr, 
		       int modifier, int duration){
  int origamt = ch->specials.affectedBy;

  ch->affectModify(affptr->location, affptr->modifier, affptr->modifier2, affptr->bitvector, FALSE, SILENT_NO);
  
  affptr->modifier=modifier;
  affptr->duration=duration;
  
  ch->affectModify(affptr->location, affptr->modifier, affptr->modifier2, affptr->bitvector, TRUE, SILENT_NO);
  ch->affectTotal();
  ch->affectChange(origamt, SILENT_NO);
}

void applyDrugAffects(TBeing *ch, drugTypeT drug, bool istick){
  unsigned int consumed, potency;
  int duration1hit, rc;
  affectedData aff, *affptr;

  // Create/increase affect(s)
  aff.type = AFFECT_DRUG;
  //  aff.level = level;
  aff.bitvector = 0;
  aff.modifier2 = drug;

  // all affects should be based on amount consumed
  consumed=ch->desc->drugs[drug].current_consumed;
  potency=drugTypes[drug].potency;
  if(consumed>potency) consumed=potency;

  duration1hit=drugTypes[drug].duration * UPDATES_PER_MUDHOUR / 2;

  switch(drug){
    case DRUG_PIPEWEED:
      if(!istick){
	if(ch->desc->drugs[drug].total_consumed==1){
	  // first smoke :)
	  act("Ugh, you're not used to smoking this stuff, it makes you nauseous.", TRUE,ch,0,0,TO_CHAR);
	  ch->doAction("", CMD_PUKE);
	  ch->dropPool(10, LIQ_VOMIT);
	}

	if(ch->desc->drugs[drug].current_consumed>(potency*3)){
	  act("You overdose on pipeweed and pass out.",
	      TRUE,ch,0,0,TO_CHAR);
	  if (ch->riding) {
	    act("$n sways then crumples as $e passes out.",TRUE,ch,0,0,TO_ROOM);
	    rc = ch->fallOffMount(ch->riding, POSITION_RESTING);
	  } else
	    act("$n stumbles then crumples as $e passes out.",TRUE,ch,0,0,TO_ROOM);
	  ch->setPosition(POSITION_SLEEPING);
	  ch->setMove(0);
	  break;
	}

	switch(consumed){
	  case 0:
	    act("You feel a little light-headed.",TRUE,ch,0,0,TO_CHAR);
	    break;
	  case 3:
	    act("You can feel the pipeweed taking affect.",
		TRUE,ch,0,0,TO_CHAR);
	    break;
	  case 6:
	    act("You feel pretty buzzed.",TRUE,ch,0,0,TO_CHAR);
	    break;
	  default:
	    act("You're really buzzed now.  Smoking any more might be a bad idea.",
		TRUE,ch,0,0,TO_CHAR);
	    break;
	  case 1: case 2: case 4: case 5: case 7: case 8:
	    break;
	}
      }

      aff.modifier = consumed;
      aff.duration = duration1hit * consumed;

      if(!(affptr=findDrugAffect(ch, DRUG_PIPEWEED, APPLY_SPE))){
	aff.location = APPLY_SPE;
	ch->affectTo(&aff, -1);
      } else {
	reapplyDrugAffect(ch, affptr, aff.modifier, 
	 istick?affptr->duration:min(affptr->duration+duration1hit, aff.duration));
      }
      if(!(affptr=findDrugAffect(ch, DRUG_PIPEWEED, APPLY_KAR))){
	aff.location = APPLY_KAR;
	ch->affectTo(&aff, -1);
      } else {
	reapplyDrugAffect(ch, affptr, aff.modifier, 
	 istick?affptr->duration:min(affptr->duration+duration1hit, aff.duration));
      }

      aff.modifier = -(consumed);
      aff.duration = duration1hit * consumed;

      if(!(affptr=findDrugAffect(ch, DRUG_PIPEWEED, APPLY_CHA))){
	aff.location = APPLY_CHA;
	ch->affectTo(&aff, -1);
      } else {
	reapplyDrugAffect(ch, affptr, aff.modifier, 
	 istick?affptr->duration:min(affptr->duration+duration1hit, aff.duration));
      }
      if(!(affptr=findDrugAffect(ch, DRUG_PIPEWEED, APPLY_FOC))){
	aff.location = APPLY_FOC;
	ch->affectTo(&aff, -1);
      } else {
	reapplyDrugAffect(ch, affptr, aff.modifier, 
	 istick?affptr->duration:min(affptr->duration+duration1hit, aff.duration));
      }

      break;
    case DRUG_OPIUM:
    case DRUG_NONE:
    case MAX_DRUG:
      break;
  }  

}

// severity is overall average use of the drug per hour * number of hours
// since last use
void applyAddictionAffects(TBeing *ch, drugTypeT drug, int severity){
  affectedData aff, *affptr;

  switch(drug){
    case DRUG_PIPEWEED:
      // severity is average amount of drug in body since first use
      // roughly, we say a cigarette is 5 drug units, so a pack a day
      // is 20*5=100 units, divided by 24 hours = about 4

      if(severity<4){
	ch->sendTo("You could use some %s right now.\n\r", drugTypes[drug].name);
      } else if(severity<8){
	ch->sendTo("You feel queasy and your hands are trembling, you really need some %s.\n\r", drugTypes[drug].name);

	aff.type = AFFECT_DRUG;
	aff.bitvector = 0;
	aff.modifier2 = drug;
	aff.modifier = -5;
	aff.duration = PULSE_MUDHOUR;
	aff.location = APPLY_FOC;

	if(!(affptr=findDrugAffect(ch, DRUG_PIPEWEED, APPLY_FOC))){
	  ch->affectTo(&aff, -1);
	} else {
	  reapplyDrugAffect(ch, affptr, aff.modifier, aff.duration);
	}
      } else {
	ch->sendTo("You need to smoke some %s to feed your addiction.\n\r", drugTypes[drug].name);
	ch->sendTo("You've got a splitting headache and you feel very very tired.\n\r");
	ch->setMove(max((ch->getMove() - 50), 0));
	
	aff.type = AFFECT_DRUG;
	aff.bitvector = 0;
	aff.modifier2 = drug;
	aff.modifier = -10;
	aff.duration = PULSE_MUDHOUR;
	aff.location = APPLY_FOC;

	if(!(affptr=findDrugAffect(ch, DRUG_PIPEWEED, APPLY_FOC))){
	  ch->affectTo(&aff, -1);
	} else {
	  reapplyDrugAffect(ch, affptr, aff.modifier, aff.duration);
	}
      }
      break;
    case DRUG_OPIUM:
    case DRUG_NONE:
    case MAX_DRUG:
      break;
  }

}


int TBeing::doSmoke(const char *argument)
{
  TDrugContainer *tdc;
  TThing *t;
  char arg[256], buf[256];
  affectedData aff;

  //  sendTo("Smoking is bad for your health.\n\r");
  //  return FALSE;
  

  only_argument(argument, arg);

  if(!*arg || !(t=get_thing_char_using(this, arg, 0, FALSE, FALSE))){
    sendTo("Smoke what?\n\r");
    return FALSE;
  }
  if(!(tdc=dynamic_cast<TDrugContainer *>(t))){
    sendTo("You can't smoke that.\n\r");
    return FALSE;
  }
  if(!tdc->getLit()){
    sendTo("How could you smoke it, when it isn't even lit!\n\r");
    return FALSE;
  }
  if(tdc->getCurBurn()<1){
    sendTo("You can't smoke that, it's empty.\n\r");
    return FALSE;
  }

  sprintf(buf, "Ok, you smoke the %s from %s.", 
	 drugTypes[tdc->getDrugType()].name, tdc->getName());
  act(buf,TRUE,this,0,0,TO_CHAR);
  sprintf(buf, "$n smokes %s from %s.", 
	  drugTypes[tdc->getDrugType()].name, tdc->getName());
  act(buf,TRUE,this,0,0,TO_ROOM);

  dropSmoke(::number(1,5));

  // Update drug stats
  tdc->addToCurBurn(-1);
  if(!desc->drugs[tdc->getDrugType()].total_consumed)
    desc->drugs[tdc->getDrugType()].first_use=time_info;
  desc->drugs[tdc->getDrugType()].last_use=time_info;
  desc->drugs[tdc->getDrugType()].total_consumed++;
  desc->drugs[tdc->getDrugType()].current_consumed++;

  saveDrugStats();

  applyDrugAffects(this, tdc->getDrugType(), false);

  return TRUE;
}




TDrugInfo::TDrugInfo(const char *n, int p, int d) :
  name(n),
  potency(p),
  duration(d)
{
}

TDrugInfo::TDrugInfo() :
  name(NULL),
  potency(0),
  duration(0)
{
}

TDrugInfo::TDrugInfo(const TDrugInfo &a) :
  name(a.name),
  potency(a.potency),
  duration(a.duration)
{
}

TDrugInfo & TDrugInfo::operator=(const TDrugInfo &a)
{
  if (this == &a) return *this;

  name = a.name;
  potency = a.potency;
  duration = a.duration;

  return *this;
}

TDrugInfo::~TDrugInfo()
{
}

vector<TDrugInfo>drugTypes(0);

void assign_drug_info(void)
{
  drugTypes.push_back(TDrugInfo("none", 0, 0));
  drugTypes.push_back(TDrugInfo("pipeweed", 10, 1));
  drugTypes.push_back(TDrugInfo("opium", 0, 0));
}
