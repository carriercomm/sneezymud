#include "stdsneezy.h"
#include "disease.h"
#include "combat.h"
#include "disc_shaman.h"
#include "spelltask.h"

// returns VICTIM_DEAD if corpse should be fried
int voodoo(TBeing *caster, TObj *obj, int level, byte bKnown)
{
  TMonster *mob;
  TThing *t, *n;
  TBaseCorpse *corpse = dynamic_cast<TBaseCorpse *>(obj);
  char buf[256], capbuf[256];
  wearSlotT i;

  /* some sort of check for corpse hood */
  if (!(corpse = dynamic_cast<TBaseCorpse *>(obj))) {
    caster->sendTo("You're invoking that on something that isn't a corpse!?\n\r");
    act("Nothing seems to happen.", FALSE, caster, 0, 0, TO_ROOM);
    return SPELL_FAIL;
  }
 
  if (corpse->isCorpseFlag(CORPSE_NO_REGEN)) {
    // a corpse that can't be res'd  (body-part or something)
    caster->sendTo("You can't do that.  Nothing happens.\n\r");
    act("Nothing seems to happen.", FALSE, caster, 0, 0, TO_ROOM);
    return SPELL_FAIL;
  }
  if (corpse->getCorpseLevel() > (unsigned int) caster->GetMaxLevel() - 3) {
    caster->sendTo("Your invokation lacks the power.  Nothing happens.\n\r");
    act("Nothing seems to happen.", FALSE, caster, 0, 0, TO_ROOM);
    return SPELL_FAIL;
  }
  if (corpse->getCorpseVnum() <= 0) {
    // med mobs == -1, pcs == -2
    caster->sendTo("A strange power prevents anything from occurring.\n\r");
    act("Nothing seems to happen.", FALSE, caster, 0, 0, TO_ROOM);
    return SPELL_FAIL;
  }
  if (!(mob = read_mobile(corpse->getCorpseVnum(), VIRTUAL))) {
    vlogf(LOG_BUG, "FAILED Load!!  No mob (%d)", corpse->getCorpseVnum());
    caster->sendTo("Something screwed up.  Tell a god.\n\r");
    act("Nothing seems to happen.", FALSE, caster, 0, 0, TO_ROOM);
    return SPELL_FAIL;
  }
  
  mob->genericCharmFix();

  // make it a zombie
  SET_BIT(mob->specials.act, ACT_ZOMBIE);
  
  *caster->roomp += *mob;

  act("You channel some of the cosmic energy into $p!", FALSE, caster, corpse, NULL, TO_CHAR);
  act("$n channels some of the cosmic energy into $p!", TRUE, caster, corpse, NULL, TO_ROOM);

  caster->roomp->playsound(SOUND_SPELL_ANIMATE_DEAD, SOUND_TYPE_MAGIC);

  // adjust stats : somewhat weaker
  mob->setMaxHit(max(1, mob->hitLimit() * 2 / 7));
  mob->setHit((int) (mob->hitLimit() >> 1));
  mob->setSex(SEX_NEUTER);

  // take all from corpse, and give to zombie 
  for (t = corpse->stuff; t; t = n) {
    n = t->nextThing;
    --(*t);
    *mob += *t;
  }

  for (i=MIN_WEAR;i < MAX_WEAR;i++) {
    if (mob->hasPart(i)) 
      mob->setCurLimbHealth(i, mob->getMaxLimbHealth(i));
  }

  // set up descriptions and such 
  mob->swapToStrung();
  sprintf(buf, "zombie %s", mob_index[mob->getMobIndex()].name);
  delete [] mob->name;
  mob->name = mud_str_dup(buf);
  sprintf(buf, "a zombie of %s", mob_index[mob->getMobIndex()].short_desc);
  delete [] mob->shortDescr;
  mob->shortDescr = mud_str_dup(buf);

  strcpy(capbuf, mob->getName());
  sprintf(buf, "%s is here, obediently following its master.\n\r", cap(capbuf));
  delete [] mob->player.longDescr;
  mob->player.longDescr = mud_str_dup(buf);

  if (caster->tooManyFollowers(mob, FOL_ZOMBIE)) {
    act("$N refuses to enter a group the size of yours!", 
          TRUE, caster, NULL, mob, TO_CHAR);
    act("$N refuses to enter a group the size of $n's!", 
          TRUE, caster, NULL, mob, TO_ROOM);
    return SPELL_FAIL + VICTIM_DEAD;
  }

  REMOVE_BIT(mob->specials.affectedBy, AFF_CHARM );

  if (bSuccess(caster, bKnown, caster->getPerc(), SPELL_VOODOO)) {
    affectedData aff;

    SET_BIT(mob->specials.affectedBy, AFF_CHARM );
    mob->setPosition(POSITION_STUNNED);    // make it take a little to wake up
    caster->addFollower(mob);
    act("$N slowly begins to move...it's slowly standing up!",
             FALSE, caster, NULL, mob, TO_CHAR);
    act("$N slowly begins to move...it's slowly standing up!", 
             FALSE, caster, NULL, mob, TO_ROOM);

    aff.type = SPELL_VOODOO;
    aff.level = level;
    aff.location = APPLY_NONE;
    aff.modifier = 0;
    aff.bitvector = AFF_CHARM;
    aff.duration = caster->followTime();
    aff.duration = (int) (caster->percModifier() * aff.duration);

    aff.duration *= 2;   // zombie adjustment

    mob->affectTo(&aff);

    aff.type = AFFECT_THRALL;
    aff.be = static_cast<TThing *>((void *) mud_str_dup(caster->getName()));
    mob->affectTo(&aff);
    delete corpse;
    return SPELL_SUCCESS + VICTIM_DEAD;
  } else {
    act("You've created a monster; $N hates you!",
            FALSE, caster, NULL, mob, TO_CHAR);
    caster->setCharFighting(mob);
    caster->setVictFighting(mob);
    delete corpse;
    return SPELL_FAIL + VICTIM_DEAD;
  }
}

void voodoo(TBeing *caster, TObj *corpse, TMagicItem *obj)
{
  int ret, level;

  level = caster->getSkillLevel(SPELL_VOODOO);
  int bKnown = caster->getSkillValue(SPELL_VOODOO);
  act("You direct a strange beam of energy at $p.",
          FALSE, caster, obj, corpse, TO_CHAR);
  act("$n directs a strange beam of energy at $p.",
          FALSE, caster, obj, corpse, TO_ROOM);
  ret=voodoo(caster,corpse,level,bKnown);
}

int castVoodoo(TBeing * caster, TObj * corpse)
{
  int ret, level;

  level = caster->getSkillLevel(SPELL_VOODOO);
  int bKnown = caster->getSkillValue(SPELL_VOODOO);
  act("You direct a strange beam of energy at $p.",
          FALSE, caster, corpse, 0, TO_CHAR);
  act("$n directs a strange beam of energy at $p.",
          FALSE, caster, corpse, 0, TO_ROOM);
  if ((ret=voodoo(caster,corpse,level,bKnown)) == SPELL_SUCCESS) {
  }
  if (IS_SET(ret, VICTIM_DEAD))
    return DELETE_ITEM;
  return TRUE;
}

int voodoo(TBeing * caster, TObj * corpse)
{
  taskDiffT diff;

  if (!bPassShamanChecks(caster, SPELL_VOODOO, corpse))
    return FALSE;

  lag_t rounds = discArray[SPELL_VOODOO]->lag;
  diff = discArray[SPELL_VOODOO]->task;

  start_cast(caster, NULL, corpse, caster->roomp, SPELL_VOODOO, diff, 1, "", rounds, caster->in_room, 0, 0,TRUE, 0);
  return TRUE;
}

// returns VICTIM_DEAD if corpse should be fried
int dancingBones(TBeing * caster, TObj * obj, int level, byte bKnown)
{
  TMonster *mob;
  TThing *t, *n;
  TBaseCorpse *corpse;
  char buf[256], capbuf[256];
  wearSlotT i;

  /* some sort of check for corpse hood */
  if (!(corpse = dynamic_cast<TBaseCorpse *>(obj))) {
    caster->sendTo("You're invoking that on something that isn't a corpse!?\n\r");
    act("Nothing seems to happen.", FALSE, caster, 0, 0, TO_ROOM);
    return SPELL_FAIL;
  }
 
  if (corpse->isCorpseFlag(CORPSE_NO_REGEN)) {
    // a corpse that can't be res'd  (body-part or something)
    caster->sendTo("You can't do that.  Nothing happens.\n\r");
    act("Nothing seems to happen.", FALSE, caster, 0, 0, TO_ROOM);
    return SPELL_FAIL;
  }
  if (corpse->getCorpseLevel() > (unsigned int) (3 * level / 5)) {
    caster->sendTo("Your invokation lacks the power.  Nothing happens.\n\r");
    act("Nothing seems to happen.", FALSE, caster, 0, 0, TO_ROOM);
    return SPELL_FAIL;
  }
  if (corpse->getCorpseVnum() <= 0) {
    // med mobs == -1, pcs == -2
    caster->sendTo("A strange power prevents anything from occurring.\n\r");
    act("Nothing seems to happen.", FALSE, caster, 0, 0, TO_ROOM);
    return SPELL_FAIL;
  }
  if (!(mob = read_mobile(corpse->getCorpseVnum(), VIRTUAL))) {
    vlogf(LOG_BUG, "FAILED Load!!  No mob (%d)", corpse->getCorpseVnum());
    caster->sendTo("Something screwed up.  Tell a god.\n\r");
    act("Nothing seems to happen.", FALSE, caster, 0, 0, TO_ROOM);
    return SPELL_FAIL;
  }
  
  // make it a skeleton
  SET_BIT(mob->specials.act, ACT_SKELETON);
  
  mob->genericCharmFix();
  *caster->roomp += *mob;

  act("You channel some of the cosmic energy into $p!", FALSE, caster, corpse, NULL, TO_CHAR);
  act("$n channels some of the cosmic energy into $p!", TRUE, caster, corpse, NULL, TO_ROOM);

  // adjust stats : somewhat weaker
  mob->setMaxHit(max(1, mob->hitLimit() * 4 / 7));
  mob->setHit((int) (mob->hitLimit() >> 1));
  mob->setSex(SEX_NEUTER);
  mob->setCarriedWeight(0.0);
  mob->setCarriedVolume(0);

  // take all from corpse, and give to mob
  for (t = corpse->stuff; t; t = n) {
    n = t->nextThing;
    --(*t);
    *mob += *t;
  }

  for (i=MIN_WEAR;i < MAX_WEAR;i++) {
    if (mob->hasPart(i)) {
      mob->setCurLimbHealth(i, mob->getMaxLimbHealth(i));
    }
  }

 /* set up descriptions and such */
  mob->swapToStrung();
  sprintf(buf, "skeleton %s", mob_index[mob->getMobIndex()].name);
  delete [] mob->name;
  mob->name = mud_str_dup(buf);
  sprintf(buf, "a skeleton of %s", mob_index[mob->getMobIndex()].short_desc);
  delete [] mob->shortDescr;
  mob->shortDescr = mud_str_dup(buf);
  strcpy(capbuf, mob->getName());
  sprintf(buf, "%s is here, enthralled by it's master.\n\r", cap(capbuf));
  delete [] mob->player.longDescr;
  mob->player.longDescr = mud_str_dup(buf);

  if (caster->tooManyFollowers(mob, FOL_ZOMBIE)) {
    act("$N refuses to enter a group the size of yours!", 
          TRUE, caster, NULL, mob, TO_CHAR);
    act("$N refuses to enter a group the size of $n's!", 
          TRUE, caster, NULL, mob, TO_ROOM);
    return SPELL_FAIL + VICTIM_DEAD;
  }

  REMOVE_BIT(mob->specials.affectedBy, AFF_CHARM );

  if (bSuccess(caster, bKnown, caster->getPerc(), SPELL_DANCING_BONES)) {
    affectedData aff;

    SET_BIT(mob->specials.affectedBy, AFF_CHARM );
    mob->setPosition(POSITION_STUNNED);    // make it take a little to wake up
    caster->addFollower(mob);
    act("$N slowly begins to move...it's slowly standing up!",
             FALSE, caster, NULL, mob, TO_CHAR);
    act("$N slowly begins to move...it's slowly standing up!", 
             FALSE, caster, NULL, mob, TO_ROOM);

    aff.type = SPELL_DANCING_BONES;
    aff.level = level;
    aff.location = APPLY_NONE;
    aff.modifier = 0;
    aff.bitvector = AFF_CHARM;
    aff.duration = caster->followTime();
    aff.duration = (int) (caster->percModifier() * aff.duration);

    aff.duration *= 3;   // skeleton adjustment

    mob->affectTo(&aff);

    aff.type = AFFECT_THRALL;
    aff.be = static_cast<TThing *>((void *) mud_str_dup(caster->getName()));
    mob->affectTo(&aff);
    delete corpse;
    return SPELL_SUCCESS + VICTIM_DEAD;
  } else {
    act("You've created a monster; $N hates you!",
            FALSE, caster, NULL, mob, TO_CHAR);
    caster->setCharFighting(mob);
    caster->setVictFighting(mob);
    delete corpse;
    return SPELL_FAIL + VICTIM_DEAD;
  }
}

void dancingBones(TBeing *caster, TObj *corpse, TMagicItem *obj)
{
  int ret, level;

  level = caster->getSkillLevel(SPELL_DANCING_BONES);
  int bKnown = caster->getSkillValue(SPELL_DANCING_BONES);
  act("You direct a strange beam of energy at $p.",
          FALSE, caster, obj, corpse, TO_CHAR);
  act("$n directs a strange beam of energy at $p.",
          FALSE, caster, obj, corpse, TO_ROOM);
  ret=dancingBones(caster,corpse,level,bKnown);
}

int castDancingBones(TBeing * caster, TObj * corpse)
{
  int ret, level;

  level = caster->getSkillLevel(SPELL_DANCING_BONES);
  int bKnown = caster->getSkillValue(SPELL_DANCING_BONES);
  act("You direct a strange beam of energy at $p.",
          FALSE, caster, corpse, 0, TO_CHAR);
  act("$n directs a strange beam of energy at $p.",
          FALSE, caster, corpse, 0, TO_ROOM);
  if ((ret=dancingBones(caster,corpse,level,bKnown)) == SPELL_SUCCESS) {
  }
  if (IS_SET(ret, VICTIM_DEAD))
    return DELETE_ITEM;
  return TRUE;
}

int dancingBones(TBeing * caster, TObj * corpse)
{
  taskDiffT diff;

  if (!bPassShamanChecks(caster, SPELL_DANCING_BONES, corpse))
    return FALSE;

  lag_t rounds = discArray[SPELL_DANCING_BONES]->lag;
  diff = discArray[SPELL_DANCING_BONES]->task;

  start_cast(caster, NULL, corpse, caster->roomp, SPELL_DANCING_BONES, diff, 1, "", rounds, caster->in_room, 0, 0,TRUE, 0);
  return TRUE;
}

int shieldOfMists(TBeing *caster, TBeing *victim, int level, byte bKnown)
{
  affectedData aff;

  aff.type = SPELL_SHIELD_OF_MISTS;
  aff.level = level;
  aff.duration = (3 + (aff.level / 2)) * UPDATES_PER_MUDHOUR;
  aff.location = APPLY_ARMOR;
  aff.modifier = -80;
  aff.bitvector = 0;

  if (bSuccess(caster,bKnown,SPELL_SHIELD_OF_MISTS)) {
    switch (critSuccess(caster, SPELL_SHIELD_OF_MISTS)) {
      case CRIT_S_KILL:
      case CRIT_S_TRIPLE:
      case CRIT_S_DOUBLE:
        CS(SPELL_SHIELD_OF_MISTS);
        aff.duration = (12 + (level / 2)) * UPDATES_PER_MUDHOUR;
        if (caster != victim)
          aff.modifier *= 2;
        break;
      case CRIT_S_NONE:
        break;
    }
    if (caster != victim) 
      aff.modifier /= 5;

    //Second argument FALSE causes it to add new duration to old
    //Third argument TRUE causes it to average the old and newmodifier

    if (!victim->affectJoin(caster, &aff, AVG_DUR_NO, AVG_EFF_YES)) {
      caster->nothingHappens();
      return FALSE;
    }


    act("<G>$n <G>is enveloped by a thick green mist!<z>", FALSE, victim, NULL,
NULL, TO_ROOM);
    act("<G>You are enveloped by a thick green mist!<z>", FALSE, victim,
NULL, NULL, TO_CHAR);

    caster->reconcileHelp(victim, discArray[SPELL_SHIELD_OF_MISTS]->alignMod);
    return SPELL_SUCCESS;
  } else {
    caster->nothingHappens();
    return SPELL_FAIL;
  }
}

void shieldOfMists(TBeing *caster, TBeing *victim, TMagicItem * obj)
{
int ret;

ret=shieldOfMists(caster,victim,obj->getMagicLevel(),obj->getMagicLearnedness());
}

int shieldOfMists(TBeing *caster, TBeing *victim)
{
  taskDiffT diff;

    if (!bPassMageChecks(caster, SPELL_SHIELD_OF_MISTS, victim))
      return FALSE;

    lag_t rounds = discArray[SPELL_SHIELD_OF_MISTS]->lag;
    diff = discArray[SPELL_SHIELD_OF_MISTS]->task;

    start_cast(caster, victim, NULL, caster->roomp, SPELL_SHIELD_OF_MISTS,
diff, 1, "", rounds, caster->in_room, 0, 0,TRUE, 0);
      return TRUE;
}

int castShieldOfMists(TBeing *caster, TBeing *victim)
{
int ret,level;

  level = caster->getSkillLevel(SPELL_SHIELD_OF_MISTS);
  int bKnown = caster->getSkillValue(SPELL_SHIELD_OF_MISTS);

  if ((ret=shieldOfMists(caster,victim,level,bKnown)) == SPELL_SUCCESS) {
  }
  return TRUE;
}

int enthrallSpectre(TBeing * caster, int level, byte bKnown)
{
  affectedData aff;
  TMonster * victim;

  if (!(victim = read_mobile(THRALL_SPECTRE, VIRTUAL))) {
    caster->sendTo("You cannot summon a being of that type.\n\r");
    return SPELL_FAIL;
  }

  victim->elementalFix(caster, SPELL_ENTHRALL_SPECTRE, 0);

  if (bSuccess(caster, bKnown, SPELL_ENTHRALL_SPECTRE)) {
     act("You call upon the powers of your ancestors!",
            TRUE, caster, NULL, NULL, TO_CHAR);
     act("$n summons the powers of $s ancestors!",
            TRUE, caster, NULL, NULL, TO_ROOM);

    // charm them for a while
    if (victim->master)
      victim->stopFollower(TRUE);

    aff.type = SPELL_ENTHRALL_SPECTRE;
    aff.level = level;
    aff.duration  = caster->followTime();
    aff.modifier = 0;
    aff.location = APPLY_NONE;
    aff.bitvector = AFF_CHARM;
    victim->affectTo(&aff);

    aff.type = AFFECT_THRALL;
    aff.be = static_cast<TThing *>((void *) mud_str_dup(caster->getName()));
    victim->affectTo(&aff);

    victim->setMaxHit(victim->hitLimit() + number(1, level));
    victim->setHit(victim->hitLimit());

    *caster->roomp += *victim;

    switch (critSuccess(caster, SPELL_ENTHRALL_SPECTRE)) {
      case CRIT_S_DOUBLE:
      case CRIT_S_TRIPLE:
      case CRIT_S_KILL:
        CS(SPELL_ENTHRALL_SPECTRE);
        act("$N flexes $S overly strong muscles.", TRUE, caster, 0, victim, TO_ROOM);
        caster->sendTo("The gods have blessed your wishes greatly!\n\r");
        victim->setMaxHit((int) (victim->hitLimit() * 1.5));
        victim->setHit((int) (victim->hitLimit() * 1.5));
        break;
      case CRIT_S_NONE:
        break;
    }
    if (caster->tooManyFollowers(victim, FOL_CHARM)) {
      act("$N refuses to enter a group the size of yours!",
             TRUE, caster, NULL, victim, TO_CHAR);
      act("$N refuses to enter a group the size of $n's!",
             TRUE, caster, NULL, victim, TO_ROOM);
      act("Your loa is displeased! $N hates you!",
             FALSE, caster, NULL, victim, TO_CHAR);
      victim->affectFrom(SPELL_ENTHRALL_SPECTRE);
      victim->affectFrom(AFFECT_THRALL);
    } else
      caster->addFollower(victim);

    if (!caster->isPc()) {
      SET_BIT(caster->specials.affectedBy, AFF_GROUP);
      SET_BIT(victim->specials.affectedBy, AFF_GROUP);
    }
    return SPELL_SUCCESS;
  } else {
    *caster->roomp += *victim;
    act("<R>In a flash of red light you see $N standing before you!<1>", TRUE, caster, NULL, victim, TO_NOTVICT);
    act("The gods are not pleased! $N hates you!", FALSE, caster, NULL, victim, TO_CHAR);
    victim->developHatred(caster);
    caster->setCharFighting(victim);
    caster->setVictFighting(victim);
    return SPELL_FAIL;
  }
}

int enthrallSpectre(TBeing * caster)
{
  if (caster->roomp && caster->roomp->isUnderwaterSector()) {
    caster->sendTo("You cannot dance the ritual under these wet conditions!\n\r");
    return FALSE;
  }

  if (real_mobile(THRALL_SPECTRE) < 0) {
    caster->sendTo("You cannot call upon a being of that type.\n\r");
    return FALSE;
  }

  if (!bPassMageChecks(caster, SPELL_ENTHRALL_SPECTRE, NULL))
    return FALSE;

  lag_t rounds = discArray[SPELL_ENTHRALL_SPECTRE]->lag;
  taskDiffT diff = discArray[SPELL_ENTHRALL_SPECTRE]->task;

  start_cast(caster, NULL, NULL, caster->roomp, SPELL_ENTHRALL_SPECTRE, diff, 1, "", rounds, caster->in_room, 0, 0,TRUE, 0);
  return TRUE;
}

int castEnthrallSpectre(TBeing * caster)
{
   int ret,level;


   if (!caster)
     return TRUE;

   level = caster->getSkillLevel(SPELL_ENTHRALL_SPECTRE);

   if
((ret=enthrallSpectre(caster,level,caster->getSkillValue(SPELL_ENTHRALL_SPECTRE))) == SPELL_SUCCESS) {
   } else {
     act("You feel the ancestors are not pleased.", FALSE, caster, NULL, NULL, TO_CHAR);
  }
  return TRUE;
}

int enthrallGhast(TBeing * caster, int level, byte bKnown)
{
  affectedData aff;
  TMonster * victim;

  if (!(victim = read_mobile(THRALL_GHAST, VIRTUAL))) {
    caster->sendTo("You cannot summon a being of that type.\n\r");
    return SPELL_FAIL;
  }

  victim->elementalFix(caster, SPELL_ENTHRALL_GHAST, 0);

  if (bSuccess(caster, bKnown, SPELL_ENTHRALL_GHAST)) {
     act("You call upon the powers of your ancestors!",
            TRUE, caster, NULL, NULL, TO_CHAR);
     act("$n summons the powers of $s ancestors!",
            TRUE, caster, NULL, NULL, TO_ROOM);

    // charm them for a while
    if (victim->master)
      victim->stopFollower(TRUE);

    aff.type = SPELL_ENTHRALL_GHAST;
    aff.level = level;
    aff.duration  = caster->followTime();
    aff.modifier = 0;
    aff.location = APPLY_NONE;
    aff.bitvector = AFF_CHARM;
    victim->affectTo(&aff);

    aff.type = AFFECT_THRALL;
    aff.be = static_cast<TThing *>((void *) mud_str_dup(caster->getName()));
    victim->affectTo(&aff);

    victim->setMaxHit(victim->hitLimit() + number(1, level));
    victim->setHit(victim->hitLimit());

    *caster->roomp += *victim;

    switch (critSuccess(caster, SPELL_ENTHRALL_GHAST)) {
      case CRIT_S_DOUBLE:
      case CRIT_S_TRIPLE:
      case CRIT_S_KILL:
        CS(SPELL_ENTHRALL_GHAST);
        act("$N flexes $S overly strong muscles.", TRUE, caster, 0, victim, TO_ROOM);
        caster->sendTo("The gods have blessed your wishes greatly!\n\r");
        victim->setMaxHit((int) (victim->hitLimit() * 1.5));
        victim->setHit((int) (victim->hitLimit() * 1.5));
        break;
      case CRIT_S_NONE:
        break;
    }
    if (caster->tooManyFollowers(victim, FOL_CHARM)) {
      act("$N refuses to enter a group the size of yours!",
             TRUE, caster, NULL, victim, TO_CHAR);
      act("$N refuses to enter a group the size of $n's!",
             TRUE, caster, NULL, victim, TO_ROOM);
      act("Your loa is displeased! $N hates you!",
             FALSE, caster, NULL, victim, TO_CHAR);
      victim->affectFrom(SPELL_ENTHRALL_GHAST);
      victim->affectFrom(AFFECT_THRALL);
    } else
      caster->addFollower(victim);

    if (!caster->isPc()) {
      SET_BIT(caster->specials.affectedBy, AFF_GROUP);
      SET_BIT(victim->specials.affectedBy, AFF_GROUP);
    }

    return SPELL_SUCCESS;
  } else {
    *caster->roomp += *victim;
    act("<R>In a flash of red light you see $N standing before you!<1>", TRUE,
caster, NULL, victim, TO_NOTVICT);
    act("The gods are not pleased! $N hates you!", FALSE, caster, NULL,
victim, TO_CHAR);
    victim->developHatred(caster);
    caster->setCharFighting(victim);
    caster->setVictFighting(victim);
    return SPELL_FAIL;
  }
}

int enthrallGhast(TBeing * caster)
{
  if (caster->roomp && caster->roomp->isUnderwaterSector()) {
    caster->sendTo("You cannot dance the ritual under these wet
conditions!\n\r");
    return FALSE;
  }

  if (real_mobile(THRALL_GHAST) < 0) {
    caster->sendTo("You cannot call upon a being of that type.\n\r");
    return FALSE;
  }

  if (!bPassMageChecks(caster, SPELL_ENTHRALL_GHAST, NULL))
    return FALSE;

  lag_t rounds = discArray[SPELL_ENTHRALL_GHAST]->lag;
  taskDiffT diff = discArray[SPELL_ENTHRALL_GHAST]->task;

  start_cast(caster, NULL, NULL, caster->roomp, SPELL_ENTHRALL_GHAST, diff, 1,
"", rounds, caster->in_room, 0, 0,TRUE, 0);
  return TRUE;
}

int castEnthrallGhast(TBeing * caster)
{
   int ret,level;


   if (!caster)
     return TRUE;

   level = caster->getSkillLevel(SPELL_ENTHRALL_GHAST);

   if ((ret=enthrallGhast(caster,level,caster->getSkillValue(SPELL_ENTHRALL_GHAST))) == SPELL_SUCCESS) {
   } else {
     act("You feel the ancestors are not pleased.", FALSE, caster, NULL, NULL, TO_CHAR);
  }
  return TRUE;
}

void TThing::sacrificeMe(TBeing *ch, const char *arg)
{
  TBaseCorpse *corpse;
  TObj *obj;
  TBeing *dummy;

  if (ch->getPosition() != POSITION_STANDING) {
    ch->sendTo(COLOR_OBJECTS, "You must stand to sacrifice %s.\n\r", obj->getName());
    return;
  }

  // Check to see if argument passed exists in room
  if (!generic_find(arg, FIND_OBJ_ROOM, ch, &dummy, &obj)) {
    ch->sendTo("You do not see a %s here.\n\r", arg);
    return;
  }
  // Check to see if corpse is a corpse
  
  if (!(corpse = dynamic_cast<TBaseCorpse *>(obj))) {
    ch->sendTo(COLOR_OBJECTS, "You cannot sacrifice %s.\n\r", obj->getName());
    return;
  }
  if (corpse->isCorpseFlag(CORPSE_SACRIFICE)) {
    act("$p is no longer worthy of the ritual.",
          FALSE, ch, corpse, 0, TO_CHAR);    
    return;
  }
  if (corpse->isCorpseFlag(CORPSE_NO_REGEN)) {
    // a body part or something
    act("You aren't able to sacrifice that $p.",
          FALSE, ch, corpse, 0, TO_CHAR);    
    return;
  }
  if (!corpse->isCorpseFlag(CORPSE_SACRIFICE)) {
    corpse->addCorpseFlag(CORPSE_SACRIFICE);
  }
  ch->sendTo("You start sacrificing a corpse.\n\r");
  act("$n begins to chant over a corpse.", FALSE, ch, NULL, 0, TO_ROOM);
}

void TTool::sacrificeMe(TBeing *ch, const char *arg)
{
  TObj *obj;
  TBaseCorpse *corpse;
  TBeing *dummy;

  if (getToolType() != TOOL_TOTEM) {
    ch->sendTo("You must be holding a totem in your primary hand to perform this ritual.\n\r");
    return;
  }
  if (ch->getPosition() != POSITION_STANDING) {
    ch->sendTo(COLOR_OBJECTS, "You must stand to sacrifice %s.\n\r", obj->getName());
    return;
  }

  // Check to see if argument passed exists in room
  if (!generic_find(arg, FIND_OBJ_ROOM, ch, &dummy, &obj)) {
    ch->sendTo("You do not see a %s here.\n\r", arg);
    return;
  }
  // Check to see if corpse is a corpse
  
  if (!(corpse = dynamic_cast<TBaseCorpse *>(obj))) {
    ch->sendTo(COLOR_OBJECTS, "You cannot sacrifice %s.\n\r", obj->getName());
    return;
  }
  if (corpse->isCorpseFlag(CORPSE_SACRIFICE)) {
    act("Someone must be sacrificing $p currently.",
          FALSE, ch, corpse, 0, TO_CHAR);    
    return;
  }

  if (corpse->isCorpseFlag(CORPSE_NO_REGEN)) {
    act("You aren't able to sacrifice $p.",
          FALSE, ch, corpse, 0, TO_CHAR);    return;
  }
  if (!corpse->isCorpseFlag(CORPSE_SACRIFICE)) {
    corpse->addCorpseFlag(CORPSE_SACRIFICE);
  }
  ch->sendTo("You start the sacrificial ritual.\n\r");
  act("$n begins to chant over a corpse.", FALSE, ch, NULL, 0, TO_ROOM);
  start_task(ch, corpse, 0, TASK_SACRIFICE, "", 2, ch->inRoom(), 0, 0, 5);
}

void TBeing::doSacrifice(const char *arg)
{
  TThing *tobj;

  for (; isspace(*arg); arg++);

  if (getPosition() != POSITION_STANDING) {
    sendTo("Have some respect! Stand to perform the sacrifice!\n\r");
    return;
  }
  if (!doesKnowSkill(SKILL_SACRIFICE)) {
    sendTo("You don't have a clue about sacrificing anything.\n\r");
    return;
  }

  tobj = heldInPrimHand();
  if (!(tobj = heldInPrimHand())) {
    sendTo("You must be holding a totem in your primary hand to perform the ritual.\n\r");
    return;
  }
  if (!tobj) {
    sendTo("You must be holding a totem in your primary hand to perform the ritual.\n\r");
    return;
  }
  tobj->sacrificeMe(this, arg);
}

int vampiricTouch(TBeing *caster, TBeing *victim, int level, byte bKnown, int adv_learn)
{
  if (victim->isImmortal()) {
    act("You can't touch a god in that manner!",
             FALSE, caster, NULL, victim, TO_CHAR);
    caster->nothingHappens(SILENT_YES);
    return SPELL_FAIL;
  }
  if (1 > victim->getHit()) {
    act("That creature has nothing left for you!",
             FALSE, caster, NULL, victim, TO_CHAR);
    caster->nothingHappens(SILENT_YES);
    return SPELL_FAIL;
  }
  int num = ::number(1,((caster->getSkillValue(SPELL_VAMPIRIC_TOUCH) / 3) *2));
  int num2 = ::number(1,((caster->getSkillValue(SPELL_VAMPIRIC_TOUCH) / 6) *5));
  int num3 = ::number(10,100);

  if (bSuccess(caster, bKnown,SPELL_VAMPIRIC_TOUCH)) {
    act("$N groans in pain as life is drawn from $S body!", FALSE, caster, NULL, victim, TO_NOTVICT);
    act("$N groans in pain as life is drawn from $S body!", FALSE, caster, NULL, victim, TO_CHAR);
    act("You groan in pain as life is drawn from your body!", FALSE, caster, NULL, victim, TO_VICT);
    if (victim->getHit() > num) {
      victim->addToHit(-num);
    } else {
      victim->setHit(0);
      victim->updatePos();
    }
    caster->addToHit(num2);
    caster->addToLifeforce(num3);
    vlogf(LOG_JESUS, "Vampiric Touch success: %s got %d HP and %d LF.", 
caster->getName(), num2, num3);
    vlogf(LOG_JESUS, "Vampiric Touch success: %s lost %d HP.", victim->getName(), num);
    return SPELL_SUCCESS;
  } else {
    switch (critFail(caster, SPELL_VAMPIRIC_TOUCH)) {
      case CRIT_F_HITSELF:
      case CRIT_F_HITOTHER:
        CF(SPELL_VAMPIRIC_TOUCH);
        act("$n's body glows a dark, evil-looking red!", 
               FALSE, caster, NULL, NULL, TO_ROOM);
        act("You sang the invokation incorrectly! The ancestors are EXTREMELY pissed!", 
               FALSE, caster, NULL, NULL, TO_CHAR);
        victim->addToHit(num);
	if (caster->getHit() > num) {
	  caster->addToHit(-num);
	} else {
	  caster->setHit(0);
	  caster->updatePos();
	}
        return SPELL_CRIT_FAIL;
      case CRIT_F_NONE:
        break;
    }
    caster->nothingHappens();
    return SPELL_FAIL;
  }
  caster->updatePos();
}

int vampiricTouch(TBeing *caster, TBeing *victim)
{
  if (!bPassShamanChecks(caster, SPELL_VAMPIRIC_TOUCH, victim))
    return FALSE;

  lag_t rounds = discArray[SPELL_VAMPIRIC_TOUCH]->lag;
  taskDiffT diff = discArray[SPELL_VAMPIRIC_TOUCH]->task;

  start_cast(caster, victim, NULL, caster->roomp, SPELL_VAMPIRIC_TOUCH, diff, 1, "", rounds, caster->in_room, 0, 
0,TRUE, 
0);

  return TRUE; 
}

int castVampiricTouch(TBeing *caster, TBeing *victim)
{
  int ret,level;
  int rc = 0;

  level = caster->getSkillLevel(SPELL_VAMPIRIC_TOUCH);
  int bKnown = caster->getSkillValue(SPELL_VAMPIRIC_TOUCH);

  ret=vampiricTouch(caster,victim,level,bKnown, caster->getAdvLearning(SPELL_VAMPIRIC_TOUCH));
  if (ret == SPELL_SUCCESS) {
  } else {
    if (ret==SPELL_CRIT_FAIL) {
    } else {
    }
  }
  if (IS_SET(ret, VICTIM_DEAD))
    ADD_DELETE(rc, DELETE_VICT);
  if (IS_SET(ret, CASTER_DEAD))
    ADD_DELETE(rc, DELETE_THIS);
  return rc;
}

int vampiricTouch(TBeing *tMaster, TBeing *tSucker, TMagicItem *tMagItem)
{
  int tRc = FALSE,
      tReturn;

  tReturn = vampiricTouch(tMaster, tSucker, tMagItem->getMagicLevel(), tMagItem->getMagicLearnedness(), 0);

  if (IS_SET(tReturn, VICTIM_DEAD))
    ADD_DELETE(tRc, DELETE_VICT);

  if (IS_SET(tReturn, CASTER_DEAD))
    ADD_DELETE(tRc, DELETE_THIS);

  return tRc;
}

int lifeLeech(TBeing *caster, TBeing *victim, int level, byte bKnown, int adv_learn)
{
  if (victim->isImmortal()) {
    act("You can't touch a god in that manner!",
             FALSE, caster, NULL, victim, TO_CHAR);
    caster->nothingHappens(SILENT_YES);
    return SPELL_FAIL;
  }
  if (1 > victim->getHit()) {
    act("That creature has nothing left for you!",
             FALSE, caster, NULL, victim, TO_CHAR);
    caster->nothingHappens(SILENT_YES);
    return SPELL_FAIL;
  }
  int num = ::number(1,((caster->getSkillValue(SPELL_LIFE_LEECH) / 5) *2));
  int num2 = ::number(1,((caster->getSkillValue(SPELL_LIFE_LEECH) / 9) *5));
  int num3 = ::number(5,60);

  if (bSuccess(caster, bKnown,SPELL_LIFE_LEECH)) {
    act("$N buckles in pain as life is drawn from $S body!", FALSE, caster, NULL, victim, TO_NOTVICT);
    act("$N buckles in pain as life is drawn from $S body!", FALSE, caster, NULL, victim, TO_CHAR);
    act("You buckle in pain as life is drawn from your body!", FALSE, caster, NULL, victim, TO_VICT);
    if (victim->getHit() > num) {
      victim->addToHit(-num);
    } else {
      victim->setHit(0);
      victim->updatePos();
    }
    caster->addToHit(num2);
    caster->addToLifeforce(num3);
    vlogf(LOG_JESUS, "Life Leech success: %s got %d HP and %d LF.", caster->getName(), num2, 
num3);
    vlogf(LOG_JESUS, "Life Leech success: %s lost %d HP.", victim->getName(), num);
    return SPELL_SUCCESS;
  } else {
    switch (critFail(caster, SPELL_LIFE_LEECH)) {
      case CRIT_F_HITSELF:
      case CRIT_F_HITOTHER:
        CF(SPELL_LIFE_LEECH);
        act("<r>$n's body glows a dark, evil-looking red!<z>", 
               FALSE, caster, NULL, NULL, TO_ROOM);
        act("<r>You sang the invokation incorrectly! The ancestors are<z> <R>EXTREMELY<z> <r>pissed!<z>", 
               FALSE, caster, NULL, NULL, TO_CHAR);
	if (caster->getHit() > num) {
	  caster->addToHit(-num);
	} else {
	  caster->setHit(0);
	  caster->updatePos();
	}
        victim->addToHit(num);
        return SPELL_CRIT_FAIL;
      case CRIT_F_NONE:
        break;
    }
    caster->nothingHappens();
    return SPELL_FAIL;
  }
  caster->updatePos();
}

int lifeLeech(TBeing *caster, TBeing *victim)
{
  if (!bPassShamanChecks(caster, SPELL_LIFE_LEECH, victim))
    return FALSE;

  lag_t rounds = discArray[SPELL_LIFE_LEECH]->lag;
  taskDiffT diff = discArray[SPELL_LIFE_LEECH]->task;

  start_cast(caster, victim, NULL, caster->roomp, SPELL_LIFE_LEECH, diff, 1, "", rounds, caster->in_room, 0, 
0,TRUE, 
0);

  return TRUE; 
}

int castLifeLeech(TBeing *caster, TBeing *victim)
{
  int ret,level;
  int rc = 0;

  level = caster->getSkillLevel(SPELL_LIFE_LEECH);
  int bKnown = caster->getSkillValue(SPELL_LIFE_LEECH);

  ret=lifeLeech(caster,victim,level,bKnown, caster->getAdvLearning(SPELL_LIFE_LEECH));
  if (ret == SPELL_SUCCESS) {
  } else {
    if (ret==SPELL_CRIT_FAIL) {
    } else {
    }
  }
  if (IS_SET(ret, VICTIM_DEAD))
    ADD_DELETE(rc, DELETE_VICT);
  if (IS_SET(ret, CASTER_DEAD))
    ADD_DELETE(rc, DELETE_THIS);
  return rc;
}

int lifeLeech(TBeing *tMaster, TBeing *tSucker, TMagicItem *tMagItem)
{
  int tRc = FALSE,
      tReturn;

  tReturn = lifeLeech(tMaster, tSucker, tMagItem->getMagicLevel(), tMagItem->getMagicLearnedness(), 0);

  if (IS_SET(tReturn, VICTIM_DEAD))
    ADD_DELETE(tRc, DELETE_VICT);

  if (IS_SET(tReturn, CASTER_DEAD))
    ADD_DELETE(tRc, DELETE_THIS);

  return tRc;
}

int cheval(TBeing *caster, TBeing *victim, int level, byte bKnown)
{
  affectedData aff;

  caster->reconcileHelp(victim, discArray[SPELL_CHEVAL]->alignMod);

  if (bSuccess(caster, bKnown, SPELL_CHEVAL)) {
    aff.type = SPELL_CHEVAL;
    aff.level = level;
    aff.duration = (aff.level / 3) * UPDATES_PER_MUDHOUR;
    aff.modifier = -50;
    aff.location = APPLY_ARMOR;
    aff.bitvector = 0;
    switch (critSuccess(caster, SPELL_CHEVAL)) {
      case CRIT_S_DOUBLE:
      case CRIT_S_TRIPLE:
      case CRIT_S_KILL:
        CS(SPELL_CHEVAL);
        aff.duration *= 2;
        break;
      case CRIT_S_NONE:
        break;
    }

    if (!victim->affectJoin(caster, &aff, AVG_DUR_NO, AVG_EFF_YES)) {
      caster->nothingHappens();
      return SPELL_FALSE;
    }

    act("$N seems as if $E is possessed!",
        FALSE, caster, NULL, victim, TO_NOTVICT, ANSI_GREEN);
    act("The power of the loa takes dominion inside of you!", 
        FALSE, victim, NULL, NULL, TO_CHAR, ANSI_GREEN);
    return SPELL_SUCCESS;
  } else {
    act("The loa ignore your unfaithful request!",
        FALSE, caster, NULL, victim, TO_CHAR, ANSI_GREEN);
    caster->addToLifeforce(-10);
    caster->nothingHappens(SILENT_YES);
    return SPELL_FAIL;
  }
}

int cheval(TBeing *caster, TBeing *victim)
{
  taskDiffT diff;

    if (!bPassShamanChecks(caster, SPELL_CHEVAL, victim))
       return FALSE;

     lag_t rounds = discArray[SPELL_CHEVAL]->lag;
     diff = discArray[SPELL_CHEVAL]->task;

     start_cast(caster, victim, NULL, caster->roomp, SPELL_CHEVAL, diff, 1, "", 
rounds, caster->in_room, 0, 0,TRUE, 0);
       return TRUE;
}

void cheval(TBeing *caster, TBeing *victim, TMagicItem * obj)
{
  cheval(caster,victim,obj->getMagicLevel(),obj->getMagicLearnedness());
}

int castCheval(TBeing *caster, TBeing *victim)
{
int ret,level;

  level = caster->getSkillLevel(SPELL_CHEVAL);
  int bKnown = caster->getSkillValue(SPELL_CHEVAL);

  if ((ret=cheval(caster,victim,level,bKnown)) == SPELL_SUCCESS) {
  } else {
  }
  return TRUE;
}

bool shaman_create_deny(int numberx)
{
  objIndexData oid = obj_index[numberx];

  if (!isname("[chrism_object]", oid.name))
    return true;

  return false;
}
        
int chrism(TBeing *caster, TObj **obj, int, const char * name, byte bKnown)
{
  unsigned int numberx;

  caster->sendTo("You drop some talens and they sink into the ground.\n\r");
  act("$n drops some money.",
      TRUE, caster, 0, 0, TO_ROOM, ANSI_WHITE);
  caster->addToMoney(-CHRISM_PRICE, GOLD_HOSPITAL);

  for (numberx = 0; numberx < obj_index.size(); numberx++) {
    if (!isname(name, obj_index[numberx].name)) 
      continue;
    if (obj_index[numberx].value > CHRISM_PRICE) 
      continue;
    if (shaman_create_deny(numberx))
      continue;
    break;
  }
  if (numberx >= obj_index.size()) {
    caster->sendTo("The loa know what your needs are, so don't try and confuse things!\n\r");
    caster->nothingHappens(SILENT_YES);
    return SPELL_FAIL;
  }

  if (bSuccess(caster, bKnown, SPELL_CHRISM)) {

    int i, num;
    if (obj_index[numberx].value)
      num = CHRISM_PRICE / obj_index[numberx].value;
    else
      num = 250;
    num = 1;
    // only one at a time

    bool grabbed = false;
    for (i = 0; i < num; i++) {
      *obj = read_object(numberx, REAL);

      (*obj)->remObjStat(ITEM_NEWBIE);
      (*obj)->setEmpty();

      if (!caster->heldInPrimHand()) {
        caster->equipChar(*obj, caster->getPrimaryHold(), SILENT_YES);
        grabbed = true;
      } else {
        *caster->roomp += **obj;
      }
    }

    act("Out of the sky $p gently falls into $n's hands.", TRUE, caster, *obj, NULL, 
TO_ROOM);
    act("The loa have blessed you with $p to aid you.", TRUE, caster, *obj, NULL, TO_CHAR);

    if (grabbed)
      act("You catch $p as it was gently falling onto the ground.",
          TRUE, caster, *obj, NULL, TO_CHAR);

    return SPELL_SUCCESS;
  } else {
    caster->nothingHappens();
    return SPELL_FAIL;
  }
}

int chrism(TBeing *caster, const char * name)
{
  if (!bPassShamanChecks(caster, SPELL_CHRISM, NULL))
    return FALSE;

  lag_t rounds = discArray[SPELL_CHRISM]->lag;
  taskDiffT diff = discArray[SPELL_CHRISM]->task;

  start_cast(caster, NULL, NULL, caster->roomp, SPELL_CHRISM, diff, 2, name, rounds, 
caster->in_room, 0, 0,TRUE, 0);
  return TRUE;
}

int castChrism(TBeing *caster, const char * name)
{
  if (caster->getMoney() < CHRISM_PRICE) {
    caster->sendTo("You don't have the money for that!\n\r");
    return FALSE;
  }

  if (!name || !*name) {
    caster->sendTo("You need to specify an item. (insert item name)\n\r");
    return FALSE;
  }
  if (strlen(name) < 3) {
    caster->sendTo("You must specify something more specific.\n\r");
    return FALSE;
  }

  int level = caster->getSkillLevel(SPELL_CHRISM);
  TObj *obj = NULL;
  
  act("$n places $s hands on $s head and howls at the sky.", TRUE, caster, NULL, NULL, 
TO_ROOM);
  act("You place your hands on the sides of your head and call unto the loa to hear your 
plea.", TRUE, caster, NULL, NULL, TO_CHAR);

  chrism(caster, &obj, level, name, caster->getSkillValue(SPELL_CHRISM));
  return TRUE;
}

int rombler(TBeing *caster, int, byte bKnown)
{
  Descriptor *i;
  const char *msg = caster->spelltask->orig_arg;

  for (; isspace(*msg); msg++);

  if (caster->isPc() && 
     ((caster->desc && 
      IS_SET(caster->desc->autobits, AUTO_NOSHOUT)) || caster->isPlayerAction(PLR_GODNOSHOUT))) {
    caster->sendTo("You aren't allowed to invoke the ritual drumming at this time!!\n\r");
    return SPELL_FAIL;
  }


  if (bSuccess(caster, bKnown, SPELL_ROMBLER)) {
    if (!*msg) {
      caster->sendTo("Drumming without spirits to send is moot.\n\r");
      caster->nothingHappens(SILENT_YES);
    } else {
      caster->sendTo(COLOR_SPELLS, "<g>You romble to the world, \"<z>%s<g>\"<z>\n\r", msg);
      for (i = descriptor_list; i; i = i->next) {
        if (i->character && (i->character != caster) &&
            !i->connected && !i->character->checkSoundproof() &&
            (dynamic_cast<TMonster *>(i->character) ||
              (!IS_SET(i->autobits, AUTO_NOSHOUT)) ||
              !i->character->isPlayerAction(PLR_GODNOSHOUT))) {
	  if (i->character->hasClass(CLASS_SHAMAN) || i->character->isImmortal()) {
            i->character->sendTo(COLOR_SPELLS, "<Y>%s<z> rombles, \"<o>%s%s\"\n\r", caster->getName(),  msg, i->character->norm());
          } else {
	    int num = ::number(0,3);
	    if (num == 0) {
            i->character->sendTo(COLOR_SPELLS, "<p>In the faint distance you hear savage drumming.<z>\n\r"); 
	    }
	    if (num == 1) {
            i->character->sendTo(COLOR_SPELLS, "<o>Savage drumming can be heard in the distance.<z>\n\r"); 
	    }
	    if (num == 2) {
	    }
	    if (num == 3) {
	    }
          } 
        }
      }
      caster->addToMove(-5);
    }
    return SPELL_SUCCESS;
  } else {
    caster->nothingHappens();
    return SPELL_FAIL;
  }
}

int rombler(TBeing *caster, const char * msg)
{
  taskDiffT diff;

    if (!bPassShamanChecks(caster, SPELL_ROMBLER, NULL))
       return FALSE;

     lag_t rounds = discArray[SPELL_ROMBLER]->lag;
     diff = discArray[SPELL_ROMBLER]->task;

     start_cast(caster, NULL, NULL, caster->roomp, SPELL_ROMBLER, diff, 1, msg, rounds, caster->in_room, 0, 0,TRUE, 0);
      return TRUE;
}

int castRombler(TBeing *caster)
{
int ret,level;

  level = caster->getSkillLevel(SPELL_ROMBLER);
  int bKnown = caster->getSkillValue(SPELL_ROMBLER);

  if ((ret=rombler(caster,level,bKnown)) == SPELL_SUCCESS) {
  }
  return TRUE;
}

int intimidate(TBeing *caster, TBeing *victim, int level, byte bKnown)
{
  int rc;

  if (caster->isNotPowerful(victim, level, SPELL_INTIMIDATE, SILENT_NO)) {
    return SPELL_FAIL;
  }

  caster->reconcileHurt(victim, discArray[SPELL_INTIMIDATE]->alignMod);

  if (bSuccess(caster, bKnown, SPELL_INTIMIDATE)) {
    if (victim->isLucky(caster->spellLuckModifier(SPELL_INTIMIDATE)) || victim->isImmune(IMMUNE_FEAR, level)) {
      SV(SPELL_INTIMIDATE);
      act("Nothing seems to happen.", FALSE, caster, NULL, NULL, TO_CHAR);
      act("You feel intimidatedbriefly.", FALSE, caster, NULL, victim, TO_VICT, ANSI_YELLOW_BOLD);
    } else {
      act("$N is totally intimidated of $n!", FALSE, caster, NULL, victim, TO_NOTVICT, ANSI_YELLOW_BOLD);
      act("$N is totally intimidated by you.", FALSE, caster, NULL, victim, TO_CHAR, ANSI_YELLOW_BOLD);
      act("$n intimidates you! RUN!!!", FALSE, caster, NULL, victim, TO_VICT, ANSI_YELLOW_BOLD);

      rc = victim->doFlee("");
      if (IS_SET_DELETE(rc, DELETE_THIS)) {
        return SPELL_SUCCESS + VICTIM_DEAD;
      }

      // this will keep them running away
      if (!victim->isPc() && level >= (int) (victim->GetMaxLevel() * 1.1)) {
        dynamic_cast<TMonster *>(victim)->addFeared(caster);
      }

      // but since once at full HP (and fearing) they will grow a hatred
      // we need a way to make the fear last a little while....
      affectedData aff;
      aff.type = SPELL_INTIMIDATE;
      aff.duration = level * UPDATES_PER_MUDHOUR / 2;
      aff.renew = aff.duration;  // renewable immediately

    // we've made raw immunity check, but allow it to reduce effects too
    aff.duration *= (100 - victim->getImmunity(IMMUNE_FEAR));
    aff.duration /= 100;

      victim->affectJoin(caster, &aff, AVG_DUR_NO, AVG_EFF_YES);
    }
    return SPELL_SUCCESS;
  } else {
    switch (critFail(caster, SPELL_INTIMIDATE)) {
      case CRIT_F_HITSELF:
      case CRIT_F_HITOTHER:
        CF(SPELL_INTIMIDATE);
        act("$n attempts to flee...from $mself?!? Strange...",
              FALSE, caster, NULL, NULL, TO_ROOM, ANSI_YELLOW_BOLD);
        act("Damn! The loa have intimidated you by yourself. Run for your life!",
              FALSE, caster, NULL, victim, TO_CHAR, ANSI_YELLOW_BOLD);
        act("$n was trying to invoke ritual on you!",
              FALSE, caster, NULL, victim, TO_VICT, ANSI_YELLOW_BOLD);
        rc = caster->doFlee("");
        if (IS_SET_DELETE(rc, DELETE_THIS)) {
          return SPELL_CRIT_FAIL + CASTER_DEAD;
        }
        return SPELL_CRIT_FAIL;
        break;
      case CRIT_F_NONE:
        break;
    }
    caster->nothingHappens();
    return SPELL_FAIL;
  }
}

int intimidate(TBeing *caster, TBeing *victim)
{
  if (!bPassShamanChecks(caster, SPELL_INTIMIDATE, victim))
    return FALSE;

  lag_t rounds = discArray[SPELL_INTIMIDATE]->lag;
  taskDiffT diff = discArray[SPELL_INTIMIDATE]->task;

  start_cast(caster, victim, NULL, caster->roomp, SPELL_INTIMIDATE, diff, 1, "", rounds, caster->in_room, 0, 0,TRUE, 0);
  return TRUE;
}

int castIntimidate(TBeing *caster, TBeing *victim)
{
  int rc = 0;

  int level = caster->getSkillLevel(SPELL_INTIMIDATE);
  int bKnown = caster->getSkillValue(SPELL_INTIMIDATE);

  int ret=intimidate(caster,victim,level,bKnown);
  if (IS_SET(ret, SPELL_SUCCESS)) {
  } else if (IS_SET(ret,SPELL_CRIT_FAIL)) {
  } else { 
  }
  
  if (IS_SET(ret, VICTIM_DEAD))
    ADD_DELETE(rc, DELETE_VICT);
  if (IS_SET(ret, CASTER_DEAD))
    ADD_DELETE(rc, DELETE_THIS);
  return rc;
}

int senseLifeShaman(TBeing *caster, TBeing *victim, int level, byte bKnown)
{
  affectedData aff;

  caster->reconcileHelp(victim, discArray[SPELL_SENSE_LIFE_SHAMAN]->alignMod);

  if (bSuccess(caster, bKnown, SPELL_SENSE_LIFE_SHAMAN)) {
    aff.type = SPELL_SENSE_LIFE_SHAMAN;
    aff.duration = (((level*2)/3) * UPDATES_PER_MUDHOUR);
    aff.modifier = 0;
    aff.location = APPLY_NONE;
    aff.bitvector = AFF_SENSE_LIFE;

    switch (critSuccess(caster, SPELL_SENSE_LIFE_SHAMAN)) {
      case CRIT_S_DOUBLE:
      case CRIT_S_TRIPLE:
      case CRIT_S_KILL:
        CS(SPELL_SENSE_LIFE_SHAMAN);
        aff.duration *= 2;
        break;
      case CRIT_S_NONE:
        break;
    }
    if (!victim->affectJoin(caster, &aff, AVG_DUR_NO, AVG_EFF_YES)) {
      caster->nothingHappens();
      return SPELL_FALSE;
    }
    return SPELL_SUCCESS;
  } else {
    return SPELL_FAIL;
  }
}

void senseLifeShaman(TBeing *caster, TBeing *victim, TMagicItem * obj)
{
  int ret;

  ret = senseLifeShaman(caster,victim,obj->getMagicLevel(),obj->getMagicLearnedness());
  if (ret == SPELL_SUCCESS) {
    victim->sendTo("You feel more aware of the world about you.\n\r");
    act("$n's eyes flicker a faint pale blue.", FALSE, victim, NULL, NULL, TO_ROOM, 
ANSI_CYAN);
  } else { 
    caster->nothingHappens();
  }
}

int senseLifeShaman(TBeing *caster, TBeing *victim)
{
  taskDiffT diff;

    if (!bPassShamanChecks(caster, SPELL_SENSE_LIFE_SHAMAN, victim))
       return FALSE;

     lag_t rounds = discArray[SPELL_SENSE_LIFE_SHAMAN]->lag;
     diff = discArray[SPELL_SENSE_LIFE_SHAMAN]->task;

     start_cast(caster, victim, NULL, caster->roomp, SPELL_SENSE_LIFE_SHAMAN, diff, 1, "", 
rounds, caster->in_room, 0, 0,TRUE, 0);
       return TRUE;
}

int castSenseLifeShaman(TBeing *caster, TBeing *victim)
{
  int ret,level;

  level = caster->getSkillLevel(SPELL_SENSE_LIFE_SHAMAN);
  int bKnown = caster->getSkillValue(SPELL_SENSE_LIFE_SHAMAN);

  ret = senseLifeShaman(caster,victim,level,bKnown);
  if (ret == SPELL_SUCCESS) {
    victim->sendTo("You feel more aware of the world about you.\n\r");
    act("$n's eyes flicker a faint pale blue.", FALSE, victim, NULL, NULL, TO_ROOM, 
ANSI_CYAN);
  } else 
    caster->nothingHappens();

  return TRUE;
}

int detectShadow(TBeing *caster, TBeing *victim, int level, byte bKnown)
{
  affectedData aff;

  caster->reconcileHelp(victim, discArray[SPELL_DETECT_SHADOW]->alignMod);

  if (bSuccess(caster, bKnown, SPELL_DETECT_SHADOW)) {
    aff.type = SPELL_DETECT_SHADOW;
    aff.duration = (((level * 3)/2) * UPDATES_PER_MUDHOUR);
    aff.modifier = 0;
    aff.location = APPLY_NONE;
    aff.bitvector = AFF_DETECT_INVISIBLE;
 
    switch (critSuccess(caster, SPELL_DETECT_SHADOW)) {
      case CRIT_S_DOUBLE:
      case CRIT_S_TRIPLE:
      case CRIT_S_KILL:
        CS(SPELL_DETECT_SHADOW);
        aff.duration *= 2;
        break;
      case CRIT_S_NONE:
        break;
    }
    if (!victim->affectJoin(caster, &aff, AVG_DUR_NO, AVG_EFF_YES)) {
      caster->nothingHappens();
      return SPELL_FALSE;
    }

    act("$n's eyes briefly glow red.", FALSE, victim, 0, 0, TO_ROOM, ANSI_RED_BOLD);
    act("Your eyes sting.", FALSE, victim, 0, 0, TO_CHAR, ANSI_RED_BOLD);
    return SPELL_SUCCESS;
  } else {
    caster->nothingHappens();
    return SPELL_FAIL;
  }
}

void detectShadow(TBeing *caster, TBeing *victim, TMagicItem * obj)
{
  int ret;

  ret=detectShadow(caster,victim,obj->getMagicLevel(),obj->getMagicLearnedness());
}

int detectShadow(TBeing *caster, TBeing *victim)
{
  taskDiffT diff;

  if (!bPassShamanChecks(caster, SPELL_DETECT_SHADOW, victim))
    return FALSE;

  lag_t rounds = discArray[SPELL_DETECT_SHADOW]->lag;
  diff = discArray[SPELL_DETECT_SHADOW]->task;

  start_cast(caster, victim, NULL, caster->roomp, SPELL_DETECT_SHADOW, diff, 1, "", rounds, 
caster->in_room, 0, 0,TRUE, 0);
    return TRUE;
}

int castDetectShadow(TBeing *caster, TBeing *victim)
{
  int ret,level;

  level = caster->getSkillLevel(SPELL_DETECT_SHADOW);
  int bKnown = caster->getSkillValue(SPELL_DETECT_SHADOW);

  ret=detectShadow(caster,victim,level,bKnown);

  if (ret == SPELL_SUCCESS) {
  }
  return TRUE;
}

int djallasProtection(TBeing *caster, TBeing *victim, int level, byte bKnown)
{
  affectedData aff, aff2, aff3, aff4;

  aff.type = SPELL_DJALLA;
  aff.level = level;
  aff.duration = (3 + (level / 2)) * UPDATES_PER_MUDHOUR;
  aff.location = APPLY_IMMUNITY;
  aff.modifier = IMMUNE_SUMMON;
  aff2.location = APPLY_IMMUNITY;
  aff2.modifier = IMMUNE_POISON;
  aff3.location = APPLY_IMMUNITY;
  aff3.modifier = IMMUNE_DRAIN;
  aff4.location = APPLY_IMMUNITY;
  aff4.modifier = IMMUNE_ENERGY;
  aff.modifier2 = ((level * 2) / 3);
  aff.bitvector = 0;
 
  if (bSuccess(caster,bKnown,SPELL_DJALLA)) {
    act("$n becomes one with the spirits.", FALSE, victim, NULL, NULL, TO_ROOM, ANSI_GREEN);
    act("You have been granted the protection of Djalla!", FALSE, victim, NULL, NULL, 
TO_CHAR, ANSI_GREEN);
    switch (critSuccess(caster, SPELL_DJALLA)) {
      case CRIT_S_DOUBLE:
      case CRIT_S_TRIPLE:
      case CRIT_S_KILL:
        CS(SPELL_DJALLA);
        aff.duration = (10 + (level / 2)) * UPDATES_PER_MUDHOUR;
        aff.modifier2 = (level * 2);
        break;
      case CRIT_S_NONE:
        break;
    }
 
    if (caster != victim) 
      aff.modifier2 /= 2;
 
    victim->affectJoin(caster, &aff, AVG_DUR_NO, AVG_EFF_YES);
    caster->reconcileHelp(victim, discArray[SPELL_DJALLA]->alignMod);
    return SPELL_SUCCESS;
  } else {
    caster->nothingHappens();
    return SPELL_FAIL;
  }
}
void djallasProtection(TBeing *caster, TBeing *victim, TMagicItem * obj)
{
  djallasProtection(caster,victim,obj->getMagicLevel(),obj->getMagicLearnedness());
}

int djallasProtection(TBeing *caster, TBeing *victim)
{
  if (!bPassShamanChecks(caster, SPELL_DJALLA, victim))
    return FALSE;

  lag_t rounds = discArray[SPELL_DJALLA]->lag;
  taskDiffT diff = discArray[SPELL_DJALLA]->task;

  start_cast(caster, victim, NULL, caster->roomp, SPELL_DJALLA, diff, 1, "", rounds, 
caster->in_room, 0, 0,TRUE, 0);
  return TRUE;
}

int castDjallasProtection(TBeing *caster, TBeing *victim)
{
  int level = caster->getSkillLevel(SPELL_DJALLA);
  int bKnown = caster->getSkillValue(SPELL_DJALLA);
 
  int ret=djallasProtection(caster,victim,level,bKnown);
  if (ret == SPELL_SUCCESS) {
  } else {
  }
  return TRUE;
}

int legbasGuidance(TBeing *caster, TBeing *victim, int level, byte bKnown)
{
  affectedData aff, aff2, aff3, aff4;

  aff.type = SPELL_LEGBA;
  aff.level = level;
  aff.duration = (3 + (level / 2)) * UPDATES_PER_MUDHOUR;
  aff.location = APPLY_IMMUNITY;
  aff.modifier = IMMUNE_BLEED;
  aff2.location = APPLY_IMMUNITY;
  aff2.modifier = IMMUNE_EARTH;
  aff3.location = APPLY_IMMUNITY;
  aff3.modifier = IMMUNE_CHARM;
  aff4.location = APPLY_IMMUNITY;
  aff4.modifier = IMMUNE_SLEEP;
  aff.modifier2 = ((level * 2) / 3);
  aff.bitvector = 0;
 
  if (bSuccess(caster,bKnown,SPELL_LEGBA)) {
    act("$n becomes one with the spirits.", FALSE, victim, NULL, NULL, TO_ROOM, ANSI_GREEN);
    act("You have been granted the protection of Legba!", FALSE, victim, NULL, NULL, 
TO_CHAR, ANSI_GREEN);
    switch (critSuccess(caster, SPELL_LEGBA)) {
      case CRIT_S_DOUBLE:
      case CRIT_S_TRIPLE:
      case CRIT_S_KILL:
        CS(SPELL_LEGBA);
        aff.duration = (10 + (level / 2)) * UPDATES_PER_MUDHOUR;
        aff.modifier2 = (level * 2);
        break;
      case CRIT_S_NONE:
        break;
    }
 
    if (caster != victim) 
      aff.modifier2 /= 2;
 
    victim->affectJoin(caster, &aff, AVG_DUR_NO, AVG_EFF_YES);
    caster->reconcileHelp(victim, discArray[SPELL_LEGBA]->alignMod);
    return SPELL_SUCCESS;
  } else {
    caster->nothingHappens();
    return SPELL_FAIL;
  }
}
void legbasGuidance(TBeing *caster, TBeing *victim, TMagicItem * obj)
{
  legbasGuidance(caster,victim,obj->getMagicLevel(),obj->getMagicLearnedness());
}

int legbasGuidance(TBeing *caster, TBeing *victim)
{
  if (!bPassShamanChecks(caster, SPELL_LEGBA, victim))
    return FALSE;

  lag_t rounds = discArray[SPELL_LEGBA]->lag;
  taskDiffT diff = discArray[SPELL_LEGBA]->task;

  start_cast(caster, victim, NULL, caster->roomp, SPELL_LEGBA, diff, 1, "", rounds, 
caster->in_room, 0, 0,TRUE, 0);
  return TRUE;
}

int castLegbasGuidance(TBeing *caster, TBeing *victim)
{
  int level = caster->getSkillLevel(SPELL_LEGBA);
  int bKnown = caster->getSkillValue(SPELL_LEGBA);
 
  int ret=legbasGuidance(caster,victim,level,bKnown);
  if (ret == SPELL_SUCCESS) {
  } else {
  }
  return TRUE;
}

int squish(TBeing * caster, TBeing * victim, int level, byte bKnown, int adv_learn)
{
  level = min(level, 25);

  int dam = caster->getSkillDam(victim, SPELL_SQUISH, level, adv_learn);

  caster->reconcileHurt(victim,discArray[SPELL_SQUISH]->alignMod);

  if (bSuccess(caster, bKnown, SPELL_SQUISH)) {
    switch (critSuccess(caster, SPELL_SQUISH)) {
      case CRIT_S_KILL:
      case CRIT_S_TRIPLE:
      case CRIT_S_DOUBLE:
        dam *= 2;
        act("Your hands become ENORMOUS and you squish $N's body HARD!",
               FALSE, caster, NULL, victim, TO_CHAR);
        act("$n's hands somehow become ENORMOUS and $e squishes $N's body REALLY HARD!",
               FALSE, caster, NULL, victim, TO_NOTVICT);
        act("$n's hands somehow become ENORMOUS and $e squishes your body REALLY HARD!",
               FALSE, caster, NULL, victim, TO_VICT);
        break;
      case CRIT_S_NONE:
        act("Your hands become enlargened and you reach out and squish $N!",
               FALSE, caster, NULL, victim, TO_CHAR);
        act("$n's hands become enlargened and $e reaches out and squishes $N!",
               FALSE, caster, NULL, victim, TO_NOTVICT);
        act("$n's hands become enlargened and $e reaches out and squishes your body!",
               FALSE, caster, NULL, victim, TO_VICT);
        if (victim->isLucky(caster->spellLuckModifier(SPELL_SQUISH))) {
          SV(SPELL_SQUISH);
          dam /= 2;
        }
        break;
    }
    vlogf(LOG_JESUS, "Squish damage: %d victim: %s caster: %s.", dam, victim->getName(), caster->getName());
    if (caster->reconcileDamage(victim, dam, SPELL_SQUISH) == -1)
      return SPELL_SUCCESS + VICTIM_DEAD;
    return SPELL_SUCCESS;
  } else {
    caster->setCharFighting(victim);
    caster->setVictFighting(victim);
    act("$n just tried to attack you!", FALSE, caster, NULL, victim, TO_VICT);
    if (critFail(caster, SPELL_SQUISH) == CRIT_F_HITSELF) {
      CF(SPELL_SQUISH);
      act("Your hands have a mind of their own and you grab the wrong thing! OWWW!",
          FALSE, caster, NULL, victim, TO_CHAR);
      act("$n fondles $mself fondly, and a bit rough...oww!",
          FALSE, caster, NULL, victim, TO_ROOM);
      if (caster->reconcileDamage(caster, dam, SPELL_SQUISH) == -1)
        return SPELL_CRIT_FAIL + CASTER_DEAD;
      return SPELL_CRIT_FAIL;
    }
    caster->nothingHappens();
    return SPELL_FAIL;
  }
}


int squish(TBeing * caster, TBeing * victim)
{
  taskDiffT diff;

     if (!bPassShamanChecks(caster, SPELL_SQUISH, victim))
        return FALSE; 

     lag_t rounds = discArray[SPELL_SQUISH]->lag;
     diff = discArray[SPELL_SQUISH]->task;

     start_cast(caster, victim, NULL, caster->roomp, SPELL_SQUISH, diff, 1, "", rounds, 
caster->in_room, 0, 0,TRUE, 0);

       return TRUE;
}

int castSquish(TBeing * caster, TBeing * victim)
{
  int ret,level;
  int rc = 0;

  level = caster->getSkillLevel(SPELL_SQUISH);

  ret=squish(caster,victim,level,caster->getSkillValue(SPELL_SQUISH), caster->getAdvLearning(SPELL_SQUISH));
  if (IS_SET(ret, SPELL_SUCCESS)) {
  } else {
    if (ret == SPELL_CRIT_FAIL) {
    } else {
    }
  }
  if (IS_SET(ret, VICTIM_DEAD))
    ADD_DELETE(rc, DELETE_VICT);
  if (IS_SET(ret, CASTER_DEAD))
    ADD_DELETE(rc, DELETE_THIS);
  return rc;
}

int distort(TBeing *caster, TBeing *victim, int level, byte bKnown, int adv_learn)
{
  char buf[256];
  string bBuf;

  if (caster->isNotPowerful(victim, level, SPELL_DISTORT, SILENT_NO))
    return SPELL_FAIL;

  level = min(level, 10);

  int dam = caster->getSkillDam(victim, SPELL_DISTORT, level, adv_learn);
  int beams = (dam / 3) + ::number(0, (caster->GetMaxLevel() / 10));
  beams = max(beams, 1);

  caster->reconcileHurt(victim, discArray[SPELL_DISTORT]->alignMod);

  if (bSuccess(caster, bKnown,SPELL_DISTORT)) {
    switch (critSuccess(caster, SPELL_DISTORT)) {
      case CRIT_S_DOUBLE:
        CS(SPELL_DISTORT);
        dam *= 2;
        beams *= 2;
        sprintf(buf, "%d", beams);
        bBuf = buf;
        bBuf += " intense energy beam";
        if (beams != 1)
          bBuf += "s expel";
        else
          bBuf += " expels";

        sprintf(buf, "%s from $n's hands and course into $N's body!", bBuf.c_str());
        act(buf, FALSE, caster, NULL, victim, TO_NOTVICT);
        sprintf(buf, "%s from your hands and course into $N's body!", bBuf.c_str());
        act(buf, FALSE, caster, NULL, victim, TO_CHAR);
        sprintf(buf, "%s from $n's hands and course into your body distorting your soul!", bBuf.c_str());
        act(buf, FALSE, caster, NULL, victim, TO_VICT);
        break;
      case CRIT_S_TRIPLE:
      case CRIT_S_KILL:
        CS(SPELL_DISTORT);
        dam *= 3;
        beams *=3;

        sprintf(buf, "%d", beams);
        bBuf = buf;
        bBuf += " BRILLIANT energy beam";
        if (beams != 1)
          bBuf += "s stream";
        else
          bBuf += " streams";

        sprintf(buf, "%s from $n's hands and course into $N's body!", bBuf.c_str());
        act(buf, FALSE, caster, NULL, victim, TO_NOTVICT);
        sprintf(buf, "%s from your hands and course into $N's body!", bBuf.c_str());
        act(buf, FALSE, caster, NULL, victim, TO_CHAR);
        sprintf(buf, "%s from $n's hands and course into your body distorting your soul!", bBuf.c_str());
        act(buf, FALSE, caster, NULL, victim, TO_VICT);
        break;
      case CRIT_S_NONE:
        sprintf(buf, "%d", beams);
        bBuf = buf;
        bBuf += " energy beam";
        if (beams != 1)
          bBuf += "s stream";
        else
          bBuf += " streams";

        sprintf(buf, "%s from $n's hands and course into $N's body!", bBuf.c_str());
        act(buf, FALSE, caster, NULL, victim, TO_NOTVICT);
        sprintf(buf, "%s from your hands and course into $N's body!", bBuf.c_str());
        act(buf, FALSE, caster, NULL, victim, TO_CHAR);
        sprintf(buf, "%s from $n's hands and course into your body distorting your soul!", bBuf.c_str());
        act(buf, FALSE, caster, NULL, victim, TO_VICT);
        if (victim->isLucky(caster->spellLuckModifier(SPELL_DISTORT))) {
          SV(SPELL_DISTORT);
          dam /= 2;
        }
    }
    vlogf(LOG_JESUS, "Distort damage: %d victim: %s caster: %s", dam, victim->getName(), caster->getName());
    if (caster->reconcileDamage(victim, dam, SPELL_DISTORT) == -1)
      return SPELL_SUCCESS + VICTIM_DEAD;
    return SPELL_SUCCESS;
  } else {
    switch (critFail(caster, SPELL_DISTORT)) {
      case CRIT_F_HITSELF:
      case CRIT_F_HITOTHER:
        CF(SPELL_DISTORT);
        sprintf(buf, "%d", beams);
        bBuf = buf;
        bBuf += " energy beam";
        if (beams != 1)
          bBuf += "s stream";
        else
          bBuf += " streams";

        sprintf(buf, "%s from $n's hands and blow up in $n's face!", bBuf.c_str());
        act(buf, FALSE, caster, NULL, victim, TO_NOTVICT);
        sprintf(buf, "%s from your hands and blow up in your face!", bBuf.c_str());
        act(buf, FALSE, caster, NULL, victim, TO_CHAR);
        sprintf(buf, "%s from $n's hands and blow up in $n's face!", bBuf.c_str());
        act(buf, FALSE, caster, NULL, victim, TO_VICT);
        if (caster->reconcileDamage(caster, dam, SPELL_DISTORT) == -1)
          return SPELL_CRIT_FAIL + CASTER_DEAD;

        return SPELL_CRIT_FAIL;
        break;
      case CRIT_F_NONE:
        break;
    } 
    caster->nothingHappens();
    return SPELL_FAIL;
  }
}

int distort(TBeing *caster, TBeing *victim, TMagicItem * obj)
{
  int rc = 0;
  int ret = 0;

  ret = distort(caster,victim,obj->getMagicLevel(),obj->getMagicLearnedness(), 0);
  if (IS_SET(ret, VICTIM_DEAD)) 
    ADD_DELETE(rc, DELETE_VICT);
  
  if (IS_SET(ret, CASTER_DEAD)) 
    ADD_DELETE(rc, DELETE_THIS);

  return rc;
}

int distort(TBeing *caster, TBeing *victim)
{
  taskDiffT diff;

  if (!bPassShamanChecks(caster, SPELL_DISTORT, victim))
    return FALSE;

  lag_t rounds = discArray[SPELL_DISTORT]->lag;
  diff = discArray[SPELL_DISTORT]->task;

  start_cast(caster, victim, NULL, caster->roomp, SPELL_DISTORT, diff, 1, "", rounds, caster->in_room, 0, 0,TRUE, 0);

  return TRUE;
}

int castDistort(TBeing *caster, TBeing *victim)
{
  int level;
  int rc = 0;
  int ret = 0;

  level = caster->getSkillLevel(SPELL_DISTORT);
  int bKnown = caster->getSkillValue(SPELL_DISTORT);

  ret=distort(caster,victim,level,bKnown, caster->getAdvLearning(SPELL_DISTORT));

  if (IS_SET(ret, VICTIM_DEAD))
    ADD_DELETE(rc, DELETE_VICT);
  if (IS_SET(ret, CASTER_DEAD))
    ADD_DELETE(rc, DELETE_THIS);

  return rc;
}

int soulTwist(TBeing *caster, TBeing *victim, int level, byte bKnown, int adv_learn)
{
  if (victim->isImmortal()) {
    act("You can't twist a gods soul!", FALSE, caster, NULL, victim, TO_CHAR);
    caster->nothingHappens(SILENT_YES);
    return SPELL_FAIL;
  }

  level = min(level, 25);

  int dam = caster->getSkillDam(victim, SPELL_SOUL_TWIST, level, adv_learn);
  caster->reconcileHurt(victim, discArray[SPELL_SOUL_TWIST]->alignMod);

  if (bSuccess(caster, bKnown, SPELL_SOUL_TWIST)) {
    switch (critSuccess(caster, SPELL_SOUL_TWIST)) {
      case CRIT_S_KILL:
      case CRIT_S_TRIPLE:
      case CRIT_S_DOUBLE:
        act("$N screams in EXTREME pain!.", 
               FALSE,caster,0,victim, TO_CHAR);
        act("You scream in EXTREME pain!", 
               FALSE, caster,0,victim, TO_VICT);
        act("$N screams in EXTREME pain!", 
               FALSE,caster,0,victim, TO_NOTVICT);
        CS(SPELL_SOUL_TWIST);
        dam <<= 1;
        break;
      case CRIT_S_NONE:
        act("$N screams in pain!.", 
               FALSE,caster,0,victim, TO_CHAR);
        act("You scream in pain!", 
               FALSE, caster,0,victim, TO_VICT);
        act("$N screams in pain!", 
               FALSE,caster,0,victim, TO_NOTVICT);
        if (victim->isLucky(caster->spellLuckModifier(SPELL_SOUL_TWIST))) {
          SV(SPELL_SOUL_TWIST);
          dam /= 2;
        }
    }

    vlogf(LOG_JESUS, "Soul Twister damage: %d victim: %s caster: %s", dam, victim->getName(), caster->getName());
    if (caster->reconcileDamage(victim, dam, SPELL_SOUL_TWIST) == -1)
      return SPELL_SUCCESS + VICTIM_DEAD;
    return SPELL_SUCCESS;
  } else {
    switch(critFail(caster, SPELL_SOUL_TWIST)) {
      case CRIT_F_HITSELF:
      case CRIT_F_HITOTHER:
        CF(SPELL_SOUL_TWIST);
        act("You scream in EXTREME pain!.", 
               FALSE,caster,0,victim, TO_CHAR);
        act("$n screams in EXTREME pain!", 
               FALSE,caster,0,victim, TO_ROOM);
        if (caster->reconcileDamage(caster, dam, SPELL_SOUL_TWIST) == -1)
          return SPELL_CRIT_FAIL + CASTER_DEAD;
        return SPELL_CRIT_FAIL;
      case CRIT_F_NONE:
        break;
    }
    caster->nothingHappens();
    return SPELL_FAIL;
  }
}

int soulTwist(TBeing *caster, TBeing *victim, TMagicItem * obj)
{
  int ret = 0;
  int rc = 0;

  ret = soulTwist(caster,victim,obj->getMagicLevel(),obj->getMagicLearnedness(), 0);
  if (IS_SET(ret, VICTIM_DEAD))
    ADD_DELETE(rc, DELETE_VICT);
  if (IS_SET(ret, CASTER_DEAD))
    ADD_DELETE(rc, DELETE_THIS);

  return rc;
}

int soulTwist(TBeing *caster, TBeing *victim)
{
  taskDiffT diff;

  if (!bPassShamanChecks(caster, SPELL_SOUL_TWIST, victim))
    return FALSE;

  lag_t rounds = discArray[SPELL_SOUL_TWIST]->lag;
  diff = discArray[SPELL_SOUL_TWIST]->task;

  start_cast(caster, victim, NULL, caster->roomp, SPELL_SOUL_TWIST, diff, 1, "", rounds, caster->in_room, 0, 0,TRUE, 0);

  return TRUE;
}

int castSoulTwist(TBeing *caster, TBeing *victim)
{
  int ret,level;
  int rc = 0;

  level = caster->getSkillLevel(SPELL_SOUL_TWIST);
  int bKnown = caster->getSkillValue(SPELL_SOUL_TWIST);

  ret=soulTwist(caster,victim,level,bKnown, caster->getAdvLearning(SPELL_SOUL_TWIST));
  if (IS_SET(ret, SPELL_SUCCESS)) {
  } else {
    if (ret==SPELL_CRIT_FAIL) {
    } else {
    }
  }
  if (IS_SET(ret, VICTIM_DEAD))
    ADD_DELETE(rc, DELETE_VICT);
  if (IS_SET(ret, CASTER_DEAD))
    ADD_DELETE(rc, DELETE_THIS);

  return rc;
}
// ENTHRALL GHOUL

int enthrallGhoul(TBeing * caster, int level, byte bKnown)
{
  affectedData aff;
  TMonster * victim;

  if (!(victim = read_mobile(THRALL_GHOUL, VIRTUAL))) {
    caster->sendTo("You cannot summon a being of that type.\n\r");
    return SPELL_FAIL;
  }

  victim->elementalFix(caster, SPELL_ENTHRALL_GHOUL, 0);

  if (bSuccess(caster, bKnown, SPELL_ENTHRALL_GHOUL)) {
     act("You call upon the powers of your ancestors!",
            TRUE, caster, NULL, NULL, TO_CHAR);
     act("$n summons the powers of $s ancestors!",
            TRUE, caster, NULL, NULL, TO_ROOM);

    // charm them for a while
    if (victim->master)
      victim->stopFollower(TRUE);

    aff.type = SPELL_ENTHRALL_GHOUL;
    aff.level = level;
    aff.duration  = caster->followTime();
    aff.modifier = 0;
    aff.location = APPLY_NONE;
    aff.bitvector = AFF_CHARM;
    victim->affectTo(&aff);

    aff.type = AFFECT_THRALL;
    aff.be = static_cast<TThing *>((void *) mud_str_dup(caster->getName()));
    victim->affectTo(&aff);

    victim->setMaxHit(victim->hitLimit() + number(1, level));
    victim->setHit(victim->hitLimit());

    *caster->roomp += *victim;

    switch (critSuccess(caster, SPELL_ENTHRALL_GHOUL)) {
      case CRIT_S_DOUBLE:
      case CRIT_S_TRIPLE:
      case CRIT_S_KILL:
        CS(SPELL_ENTHRALL_GHOUL);
        act("$N flexes $S overly strong muscles.", TRUE, caster, 0, victim, TO_ROOM);
        caster->sendTo("The gods have blessed your wishes greatly!\n\r");
        victim->setMaxHit((int) (victim->hitLimit() * 1.5));
        victim->setHit((int) (victim->hitLimit() * 1.5));
        break;
      case CRIT_S_NONE:
        break;
    }
    if (caster->tooManyFollowers(victim, FOL_CHARM)) {
      act("$N refuses to enter a group the size of yours!",
             TRUE, caster, NULL, victim, TO_CHAR);
      act("$N refuses to enter a group the size of $n's!",
             TRUE, caster, NULL, victim, TO_ROOM);
      act("Your loa is displeased! $N hates you!",
             FALSE, caster, NULL, victim, TO_CHAR);
      victim->affectFrom(SPELL_ENTHRALL_GHOUL);
      victim->affectFrom(AFFECT_THRALL);
    } else
      caster->addFollower(victim);

    if (!caster->isPc()) {
      SET_BIT(caster->specials.affectedBy, AFF_GROUP);
      SET_BIT(victim->specials.affectedBy, AFF_GROUP);
    }

    return SPELL_SUCCESS;
  } else {
    *caster->roomp += *victim;
    act("<R>In a flash of red light you see $N standing before you!<1>", TRUE,
caster, NULL, victim, TO_NOTVICT);
    act("The gods are not pleased! $N hates you!", FALSE, caster, NULL,
victim, TO_CHAR);
    victim->developHatred(caster);
    caster->setCharFighting(victim);
    caster->setVictFighting(victim);
    return SPELL_FAIL;
  }
}

int enthrallGhoul(TBeing * caster)
{
  if (caster->roomp && caster->roomp->isUnderwaterSector()) {
    caster->sendTo("You cannot dance the ritual under these wet conditions!\n\r");
    return FALSE;
  }

  if (real_mobile(THRALL_GHOUL) < 0) {
    caster->sendTo("You cannot call upon a being of that type.\n\r");
    return FALSE;
  }

  if (!bPassShamanChecks(caster, SPELL_ENTHRALL_GHOUL, NULL))
    return FALSE;

  lag_t rounds = discArray[SPELL_ENTHRALL_GHOUL]->lag;
  taskDiffT diff = discArray[SPELL_ENTHRALL_GHOUL]->task;

  start_cast(caster, NULL, NULL, caster->roomp, SPELL_ENTHRALL_GHOUL, diff, 1,
"", rounds, caster->in_room, 0, 0,TRUE, 0);
  return TRUE;
}

int castEnthrallGhoul(TBeing * caster)
{
   int ret,level;


   if (!caster)
     return TRUE;

   level = caster->getSkillLevel(SPELL_ENTHRALL_GHOUL);

   if
((ret=enthrallGhoul(caster,level,caster->getSkillValue(SPELL_ENTHRALL_GHOUL)))
== SPELL_SUCCESS) {
   } else {
     act("You feel the ancestors are not pleased.", FALSE, caster, NULL, NULL,
TO_CHAR);
  }
  return TRUE;
}

// END ENTHRALL GHOUL

int stupidity(TBeing *caster, TBeing *victim, int level, byte bKnown)
{
  affectedData aff;
  int ret = 0;

  if (victim->affectedBySpell(SPELL_STUPIDITY)) {
    act("You sense that $N is already stupid!",
        FALSE, caster, NULL, victim, TO_CHAR);
    act("$n just tried to invoke something on you!", 
        0, caster, NULL, victim, TO_VICT);
    caster->nothingHappens(SILENT_YES);
    return SPELL_FALSE;
  }

  caster->reconcileHurt(victim,discArray[SPELL_STUPIDITY]->alignMod);

  aff.type = SPELL_STUPIDITY;
  aff.level = level;
  aff.location = APPLY_INT;
  aff.bitvector = 0;

  // we'd like it to last about 10 minutes
  aff.duration = 10 * UPDATES_PER_MUDHOUR / 2;

  // let the affect be level dependant
  aff.modifier = -(aff.level/4);

  if (bSuccess(caster, bKnown, SPELL_STUPIDITY)) {
    ret = SPELL_SUCCESS;
    switch (critSuccess(caster, SPELL_STUPIDITY)) {
      case CRIT_S_DOUBLE:
      case CRIT_S_TRIPLE:
      case CRIT_S_KILL:
        CS(SPELL_STUPIDITY);
        aff.duration *=2;
        aff.modifier *=2;
        ret += SPELL_CRIT_SUCCESS + SPELL_CSUC_DOUBLE;
        break;
      case CRIT_S_NONE:
        if (victim->isLucky(caster->spellLuckModifier(SPELL_STUPIDITY))) {
          SV(SPELL_STUPIDITY);
          aff.duration /= 2;
          aff.modifier /= 2;
          ret += SPELL_SAVE;
        }
        break;
    }
    victim->affectJoin(caster, &aff, AVG_DUR_NO, AVG_EFF_YES);

    // this spell is non-violent, cause it to piss off mobs though
    if (!victim->isPc()) {
      TMonster *tmons = dynamic_cast<TMonster *>(victim);
      tmons->UM(4);
      tmons->US(5);
      tmons->UA(7);
      tmons->aiTarget(caster);
    }
    return ret;
  } else {
    ret += SPELL_FAIL;
    switch (critFail(caster, SPELL_STUPIDITY)) {
      case CRIT_F_HITSELF:
      case CRIT_F_HITOTHER:
        CF(SPELL_STUPIDITY);
        caster->affectJoin(caster, &aff, AVG_DUR_NO, AVG_EFF_YES);
        ret += SPELL_CFAIL_DEFAULT + SPELL_CRIT_FAIL;
        break;
      case CRIT_F_NONE:
        break;
    }
    return ret;
  }
}

void stupidity(TBeing *caster, TBeing *victim, TMagicItem * obj)
{
  int ret;

  if (victim->affectedBySpell(SPELL_STUPIDITY)) {
    act("You sense that $N is already stupid!",
        FALSE, caster, NULL, victim, TO_CHAR);
    act("$n just tried to invoke something on you!",
        0, caster, NULL, victim, TO_VICT);
    caster->nothingHappens(SILENT_YES);
    return;
  }

  ret = stupidity(caster,victim,obj->getMagicLevel(),obj->getMagicLearnedness());


  if (IS_SET(ret, SPELL_SUCCESS)) {
    act("A drab olive green aura engulfs $N!",
        TRUE, caster, NULL, victim, TO_NOTVICT);
    act("A drab olive green aura engulfs $N!",
        TRUE, caster, NULL, victim, TO_CHAR);
    act("A drab olive green aura engulfs your head!",
        FALSE, caster, NULL, victim, TO_VICT);
    }
  if (IS_SET(ret, SPELL_SAVE)) {
    act("The fog of stupidity weakens.",
        FALSE, caster, NULL, victim, TO_CHAR);
    act("The fog of stupidity weakens.",
        FALSE, caster, NULL, victim, TO_ROOM);
  } else {
  }

  if (IS_SET(ret, SPELL_CRIT_SUCCESS)) {
    if (IS_SET(ret, SPELL_CSUC_DEFAULT)) {
    }
    if (IS_SET(ret, SPELL_CSUC_DOUBLE)) {
      act("Yep...really stupid....",
          FALSE, caster, NULL, victim, TO_CHAR);
      act("Yep...really stupid....",
          FALSE, caster, NULL, victim, TO_ROOM);
    }
    if (IS_SET(ret, SPELL_CSUC_TRIPLE)) {
    }
  }

  if (IS_SET(ret, SPELL_FAIL) && !IS_SET(ret, SPELL_CRIT_FAIL)) {
    caster->nothingHappens();
  }
  if (IS_SET(ret, SPELL_CRIT_FAIL) || IS_SET(ret, SPELL_CFAIL_DEFAULT)) {
    act("A drab olive green aura engulfs $n!",
        TRUE, caster, NULL, victim, TO_ROOM);
    act("A drab olive green aura engulfs your head!",
        TRUE, caster, NULL, victim, TO_CHAR);
  } else {
    if (IS_SET(ret, SPELL_CFAIL_SELF)) {
    }
    if (IS_SET(ret, SPELL_CFAIL_OTHER)) {
    }
  }

  if (IS_SET(ret, SPELL_FAIL_SAVE)) {
  }
  if (IS_SET(ret, SPELL_ACTION)) {
  }
  if (IS_SET(ret, SPELL_FALSE)) {
  }
}

void stupidity(TBeing *caster, TBeing *victim)
{
  taskDiffT diff;
// 1. First check for unusual fails
  if (victim->affectedBySpell(SPELL_STUPIDITY)) {
    act("You sense that $N is already stupid!",
        FALSE, caster, NULL, victim, TO_CHAR);
    act("$n just tried to invoke something on you!",
        0, caster, NULL, victim, TO_VICT);
    caster->nothingHappens(SILENT_YES);
    return;
  }

// 2. Second send to general mage check function
  if (!bPassShamanChecks(caster, SPELL_STUPIDITY, victim))
    return;

// 4.   Get lag and difficulty for adding to casting object
  lag_t rounds = discArray[SPELL_STUPIDITY]->lag;
  diff = discArray[SPELL_STUPIDITY]->task;

// 5.   Initialize the casting object with data
  start_cast(caster, victim, NULL, caster->roomp, SPELL_STUPIDITY, diff, 1, "", rounds, 
caster->in_room, 0, 0,TRUE, 0);

// 6.   Start them fighting with 0 damage
// this spell is non-violent, it piss off mobs if it hits 

}

int castStupidity(TBeing *caster, TBeing *victim)
{
int ret,level;

  level = caster->getSkillLevel(SPELL_STUPIDITY);

  ret=stupidity(caster,victim,level,caster->getSkillValue(SPELL_STUPIDITY));

  if (!IS_SET(ret, SPELL_FALSE)) {
    act("You point at $N.", TRUE, caster, NULL, victim, TO_CHAR);
    act("$n points at $N.", TRUE, caster, NULL, victim, TO_NOTVICT);
    act("$n points at you.", TRUE, caster, NULL, victim, TO_VICT);
  }

  if (IS_SET(ret, SPELL_SUCCESS)) {
    act("A drab olive green aura engulfs $N!", 
        TRUE, caster, NULL, victim, TO_NOTVICT);
    act("A drab olive green aura engulfs $N!", 
        TRUE, caster, NULL, victim, TO_CHAR);
    act("A drab olive green aura engulfs your head!", 
        FALSE, caster, NULL, victim, TO_VICT);
  }
  if (IS_SET(ret, SPELL_SAVE)) {
    act("The fog of stupidity weakens.",
        FALSE, caster, NULL, victim, TO_CHAR);
    act("The fog of stupidity weakens.",
        FALSE, caster, NULL, victim, TO_ROOM);
  } else {
  }
  if (IS_SET(ret, SPELL_CRIT_SUCCESS)) {
    if (IS_SET(ret, SPELL_CSUC_DEFAULT)) {
    }
    if (IS_SET(ret, SPELL_CSUC_DOUBLE)) {
    act("Yep....really stupid....",
        FALSE, caster, NULL, victim, TO_CHAR);
    act("Yep....really stupid....",
        FALSE, caster, NULL, victim, TO_ROOM);
    }
    if (IS_SET(ret, SPELL_CSUC_TRIPLE)) {
    }
  }

//  if (IS_SET(ret, SPELL_SAVE)) {
//  }

  if (IS_SET(ret, SPELL_FAIL) && !IS_SET(ret, SPELL_CRIT_FAIL)) {
    caster->nothingHappens();
  }
  if (IS_SET(ret, SPELL_CRIT_FAIL) || IS_SET(ret, SPELL_CFAIL_DEFAULT)) {
    act("A drab olive green aura engulfs $n!", 
        TRUE, caster, NULL, victim, TO_ROOM);
    act("A drab olive green aura engulfs your head!", 
        TRUE, caster, NULL, victim, TO_CHAR);
  } else {

    if (IS_SET(ret, SPELL_CFAIL_SELF)) {
    }
    if (IS_SET(ret, SPELL_CFAIL_OTHER)) {
    }
  }
  if (IS_SET(ret, SPELL_FAIL_SAVE)) {
  }
  if (IS_SET(ret, SPELL_ACTION)) {
  }
  if (IS_SET(ret, SPELL_FALSE)) {
  }
// spell doesnt cause damage so return false
  return FALSE;

}

int flatulence(TBeing * caster, int level, byte bKnown, int adv_learn)
{
  TThing *t, *t2;
  TBeing *vict = NULL;

  level = min(level, 20);

  int dam = caster->getSkillDam(NULL, SPELL_FLATULENCE, level, adv_learn);

  if (bSuccess(caster, bKnown, SPELL_FLATULENCE)) {
    act("<o>You turn around quickly and pass gas!<1>", FALSE, caster, NULL, NULL, TO_CHAR);
    act("<o>$n turns around quickly and passes gas!<1>", FALSE, caster, NULL, NULL, TO_ROOM);
    for (t = caster->roomp->stuff; t; t = t2) {
      t2 = t->nextThing;
      vict = dynamic_cast<TBeing *>(t);
      if (!vict)
        continue;

      if (!caster->inGroup(*vict) && !vict->isImmortal()) {
        caster->reconcileHurt(vict, discArray[SPELL_FLATULENCE]->alignMod);
        act("$n is choked by the natural gasses!", FALSE, vict, NULL, NULL, TO_ROOM);
        act("You are choked by the natural gasses!", FALSE, vict, NULL, NULL, TO_CHAR);
        if (caster->reconcileDamage(vict, dam, SPELL_FLATULENCE) == -1) {
          delete vict;
          vict = NULL;
        }
      } else {
        act("$n takes a deep breath and holds it until the noxious fumes disperse.", TRUE, vict, NULL, 0 , TO_ROOM);
        act("You take a deep breath and hold it until the noxious fumes disperse.", TRUE, vict, NULL, NULL, TO_CHAR);
      }
    }
    vlogf(LOG_JESUS, "Flatulance damage %d", dam);
    return SPELL_SUCCESS;
  } else {
    if (critFail(caster, SPELL_FLATULENCE)) {
      CF(SPELL_FLATULENCE);
      act("Oh no!!! That one stuck with you!", 
            FALSE, caster, NULL, NULL, TO_CHAR);
      act("$n chokes on $s own fumes!!", 
            FALSE, caster, NULL, NULL, TO_ROOM);
      if (caster->reconcileDamage(caster, dam, SPELL_FLATULENCE) == -1)
        return SPELL_CRIT_FAIL + CASTER_DEAD;
      return SPELL_CRIT_FAIL;
    }
    caster->nothingHappens();
    return SPELL_FAIL;
  }
}

int flatulence(TBeing * caster)
{
  if (!bPassShamanChecks(caster, SPELL_FLATULENCE, NULL))
    return FALSE; 

  lag_t rounds = discArray[SPELL_FLATULENCE]->lag;
  taskDiffT diff = discArray[SPELL_FLATULENCE]->task;

  start_cast(caster, NULL, NULL, caster->roomp, SPELL_FLATULENCE, diff, 1, "", rounds, 
caster->in_room, 0, 0,TRUE, 0);
  return TRUE;
}

int castFlatulence(TBeing * caster)
{
  int ret,level;
  int rc = 0;

  level = caster->getSkillLevel(SPELL_FLATULENCE);

  ret=flatulence(caster,level,caster->getSkillValue(SPELL_FLATULENCE), caster->getAdvLearning(SPELL_FLATULENCE));
  if (IS_SET(ret, SPELL_SUCCESS)) {
  } else {
    if (ret == SPELL_CRIT_FAIL) {
    } else {
    }
  }
  if (IS_SET(ret, CASTER_DEAD))
    ADD_DELETE(rc, DELETE_THIS);
  return rc;
}
