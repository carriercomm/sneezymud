//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: pets.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#include "stdsneezy.h"

#if 0
  bool TMonster::reloadNPCAsNew()
{
  int rc, numx;

  rc = mobVnum();

  if (mobVnum() < 0) {
    vlogf(9, "Attempt to reload a prototype in ReloadNPCAsNew.  Trying to reload %s.",getName ());
    return FALSE;
  }


  if (rc <= 0 || ((numx = real_mobile(rc)) <= 0)) {
    vlogf(9, "Problem in ReloadNPCAsNew (ERR 1).  Trying to reload %s.",getName ());
    return FALSE;
  }

  if (numx < 0 || numx >= (signed int) mob_index.size()) {
    vlogf(9, "Problem in ReloadNPCAsNew (ERR 2).  Trying to reload %s.",getName ());
    return FALSE;
  }

  if (!(newMob = read_mobile(rc, REAL))) {
    vlogf(9, "Problem in ReloadNPCAsNew (ERR 3).  Trying to reload %s.",getName ());
    return FALSE;
  }
  if (mob_index[rc].spec == SPEC_SHOPKEEPER) {
    vlogf(9, "Problem in ReloadNPCAsNew (ERR 4).  Trying to reload shopkeepere-%s.",getName ());
    delete newMob;
    return FALSE;
  }
  if (mob_index[rc].spec == SPEC_NEWBIE_EQUIPPER) {
    vlogf(9, "Problem in ReloadNPCAsNew (ERR 5).  Trying to reload newbieHelper -%s.",getName ());
    delete newMob;
    return FALSE;
  }

  *roomp += *newMob;
  newMob->oldRoom = inRoom();
  newMob->createWealth();
  return TRUE;
}
#endif
int TBeing::getAffectedDataFromType(spellNumT whichAff, double whichField) 
{
  affectedData *an = NULL,
               *aff = NULL;
  int numAffs = 0;

  for (an = affected; an; an = an->next) {
    if (an->type == whichAff) {
      aff = an;
      numAffs++;
    }
  }

  if (!aff)
    return 0;

  if (numAffs > 1) {
    vlogf(5, "Somehow %s has 2 affectedDatas with same type (%d)",
          getName(), whichAff);
  }

  /* **Commented out so code could be compiled**
  if (IS_SET(whichField, AFFECT_FIELD_LEVEL)) {
  } else if (IS_SET(whichField, AFFECT_FIELD_DURATION)) {
  } else if (IS_SET(whichField, AFFECT_FIELD_RENEW)) {
  } else if (IS_SET(whichField, AFFECT_FIELD_MODIFIER)) {
  } else if (IS_SET(whichField, AFFECT_FIELD_MODIFIER2)) {
  } else if (IS_SET(whichField, AFFECT_FIELD_LOCATION)) {
  } else if (IS_SET(whichField, AFFECT_FIELD_BITVECTOR)) {
  } else if (IS_SET(whichField, AFFECT_FIELD_BE)) {
  } else if (IS_SET(whichField, AFFECT_FIELD_NEXT)) {
  }
  */

  // **Added so code could be compiled**
  return 0;
} 

int TBeing::getPetOrderLevel()
{
// Returns both true/false if not even a valid concept
// returns an updated reference as well
  /* **Commented out so code could be compiled**
  affectedData *an = NULL, *aff = NULL;
  */

  // **Added so code could be compiled**
  return 0;
}

int TBeing::getPetAge() 
{
// Returns both true/false if not even a valid concept
// returns an updated reference as well

  /* **Commented out so code could be compiled**
  affectedData *an = NULL, *aff = NULL;
  */

  // **Added so code could be compiled**
  return 0;
}
#if 1 
bool TMonster::isRetrainable() 
{
  affectedData *aff;

  for (aff = affected; aff; aff = aff->next) {
    if (aff->type != AFFECT_ORPHAN_PET)
      continue;
    if (aff->level == 0)
      return TRUE; 
  }
  return FALSE;
}
#endif
bool TBeing::doRetrainPet(const char *argument, TBeing *vict)
{
  TRoom *rp = NULL;
  TBeing *mob = NULL;
  TMonster *v = NULL;

// no room
  if (!(rp = roomp)) {
    vlogf(5, "%s was in doRetrainPet without a roomp", getName());
    return FALSE;
  }

// find target in room - visibility important

  if (!(mob = vict)) {
    if (!(mob = get_char_room_vis(this, argument))) {
      act("The one you want to retrain isnt here.",
          FALSE, this, NULL, NULL, TO_CHAR);
      return FALSE;
    }
  }
// various checks

  if (!(v = dynamic_cast<TMonster *>(mob))) {
    act("You cant retrain $N.",
        FALSE, this, NULL, v, TO_CHAR);
    return FALSE;
  }

  if (v->master) {
    act("$N already has a master.",
        FALSE, this, NULL, v, TO_CHAR);
    return FALSE;
  }

#if 1 
  if (!v->isRetrainable()) {
    act("$N is not trainable.  Perhaps you have made a mistake.",
        FALSE, this, NULL, v, TO_CHAR);
    return FALSE;
  }
#else
  if (!v->affectedBySpell(AFFECT_ORPHAN_PET)) {
    act("$N is not trainable.  Perhaps you have made a mistake.",
        FALSE, this, NULL, v, TO_CHAR);
    return FALSE;
  }

#endif
  if (v->fight() || !v->awake()) {
    act("$N is busy now. Perhaps you should wait till the fight is over.",
        FALSE, this, NULL, v, TO_CHAR);
    return FALSE;
  }

  if (v->isAffected(AFF_BLIND)) {
    act("$N is blind and not trainable.",
        FALSE, this, NULL, v, TO_CHAR);
    return FALSE;
  }

  if (hasClass(CLASS_RANGER) && v->isAnimal()) {
    if (GetMaxLevel() >= v->GetMaxLevel()) {
      if (v->restorePetToPc(this)) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      act("$N is too powerful for you to retrain for its old master.",
          FALSE, this, NULL, v, TO_CHAR);
      return FALSE;
    }
  }

// 20% chance of rejection

  if (!::number(0,4)) {
    act("$N rejects your retraining and remains wild.",
        FALSE, this, NULL, v, TO_CHAR);
    v->affectFrom(AFFECT_ORPHAN_PET);
    return FALSE;
  }
  if (v->restorePetToPc(this)) {
    return TRUE;

  } else {
// taking affect orphan done in retore Pet
    return FALSE;
  }
}

// **Added so code would compile**
bool TBeing::restorePetToPc(TBeing *ch)
{
  TMonster *tMonster = dynamic_cast<TMonster *>(this);

  return (tMonster ? tMonster->restorePetToPc(ch) : false);
}

bool TMonster::restorePetToPc(TBeing *ch)
{
  TRoom *rp = NULL;
  affectedData *aff = NULL, *an = NULL;
  char * affName = NULL;
  TThing *t = NULL;
  TBeing *pc = NULL;
  bool found = FALSE;

  if (fight() || !awake()) {
    return FALSE;
  }
  for (an = affected; an; an = an->next) {
    if (an->type == AFFECT_PET) {
      aff = an;
      break;
    }
  }

  // **aff.be changed to aff->be**
  if (!aff || !aff->be) {
    act("$N has never been a pet and can not be retrained.",
        FALSE, ch, NULL, this, TO_CHAR);
    return FALSE;
  }
  // **semicolon added to end of line**
  // **aff.be changed to aff->be**
  affName = (char *) aff->be;
  rp = roomp;

  for (t = roomp->stuff; t; t = t->nextThing) {
    if (!(pc = dynamic_cast<TBeing *>(t)))
      continue;
    if (is_exact_name(affName, pc->getName())) {
      found = TRUE;
      break;
    }
  }

  if (!pc) {
    act("$N's owner is not in this room.",
        FALSE, ch, NULL, this, TO_CHAR);
    return FALSE;
  }

  if (!found) {
    act("$N's owner is not in this room.",
        FALSE, ch, NULL, this, TO_CHAR);
    return FALSE;
  }

  if ((ch != pc) && !ch->hasClass(CLASS_RANGER) && !ch->isImmortal()) {
    act("Only rangers can retrain a pet for someone other than themselves.",
        FALSE, ch, NULL, this, TO_CHAR);
    return FALSE;
  }

  if (pc->tooManyFollowers(this, FOL_PET)) {
    if (ch == pc) {
      act("Your charmisma won't support the retraining of this $N.",
          FALSE, ch, NULL, this, TO_CHAR);
    } else {
      act("$p's charmisma won't support the retraining of this $N.",
          FALSE, ch, pc, this, TO_CHAR);
    }
    return FALSE;
  }


  SET_BIT(specials.affectedBy, AFF_CHARM);
  pc->addFollower(this);
  affectFrom(AFFECT_ORPHAN_PET); 
  if (pc == ch) {
    act("$N has been restored to your side.",
          FALSE, pc, NULL, this, TO_CHAR);
    act("$N has been restored to $s place at $n's side.",
          FALSE, pc, NULL, this, TO_ROOM);
  } else {
    act("$N has been restored to your side.",
         FALSE, pc, NULL, this, TO_CHAR);
    act("You have restored $N to $E owner.",
         FALSE, ch, NULL, this, TO_VICT);
    act("$N has been restored to $S place at $n's side.",
          FALSE, pc, NULL, this, TO_ROOM);
  }
  return TRUE;
}

