/*******************************************************************
 *                                                                 *
 *                                                                 *
 *   disc_shaman_spider.cc             Shaman Discipline Disc      *
 *                                                                 *
 *   SneezyMUD Development - All Rights Reserved                   *
 *                                                                 *
 *******************************************************************/
 
#include "stdsneezy.h"
#include "disease.h"
#include "combat.h"
#include "spelltask.h"
#include "disc_shaman_spider.h"

// STICKS TO SNAKES

int sticksToSnakes(TBeing * caster, TBeing * victim, int level, byte bKnown)
{
  TMonster *snake;
  affectedData aff; 
  int rc, mobile;
  followData *k, *n;

  if (level < 26) {
     mobile = MOB_SNAKES25;
  } else if (level < 31) {
     mobile = MOB_SNAKES30;
  } else if (level < 36) {
     mobile = MOB_SNAKES35;
  } else if (level < 41) {
     mobile = MOB_SNAKES40;
  } else {
     mobile = MOB_SNAKES50;
  }


  if(!(snake = read_mobile(mobile, VIRTUAL))) {
    vlogf(LOG_BUG, "Spell STICKS_TO_SNAKES unable to load mob...");
    caster->sendTo("Unable to create the snake, please report this.\n\r");
    act("Nothing seems to happen.", FALSE, caster, NULL, NULL, TO_ROOM);
    return SPELL_FAIL;
  }

  if ((caster->followers) && (caster->GetMaxLevel() < GOD_LEVEL1))  {
      for (k = caster->followers; k; k = n) {
        n = k->next;
        if (!strcmp(k->follower->getName(), snake->getName())) {
          act("Nothing seems to happen.", FALSE, caster, NULL, NULL, TO_ROOM);
          caster->sendTo("You would have to be immortal to summon another snake!\n\r");
          delete snake;
          snake=NULL;
          return SPELL_FAIL;
        }
      }
   }

  float lvl = max(1.0, (float) level/2.0);
  snake->setDamPrecision(20);

  snake->setExp(0);
  snake->setMoney(0);
  snake->setHitroll(0);

  *caster->roomp += *snake;
  act("A strange yellow mist appears and suddenly changes into $n!", TRUE, snake, NULL, caster, TO_ROOM);
  act("A strange yellow mist appears and suddenly changes into $n!", TRUE, snake, NULL, caster, TO_CHAR);

  if (bSuccess(caster, bKnown, caster->getPerc(), SPELL_STICKS_TO_SNAKES)) {
    switch(critSuccess(caster, SPELL_STICKS_TO_SNAKES)) {
      case CRIT_S_KILL:
      case CRIT_S_TRIPLE:
      case CRIT_S_DOUBLE:
        CS(SPELL_STICKS_TO_SNAKES);
        act("The snake appears to be particularly strong and particularly angry!", TRUE, caster, 0, snake, TO_ROOM);
        act("The snake appears to be particularly strong and particularly angry!", TRUE, caster, 0, snake, TO_CHAR);
        lvl *= 2;
        break;
      default:
        if (victim->isLucky(caster->spellLuckModifier(SPELL_STICKS_TO_SNAKES))) {
          SV(SPELL_STICKS_TO_SNAKES);
          lvl /=2;
        }
        break;
    }

    if (!caster->fight()) {
      if (caster->reconcileDamage(victim, 0, SPELL_STICKS_TO_SNAKES) == -1)
        return SPELL_SUCCESS + VICTIM_DEAD;
    }

    snake->setLevel(WARRIOR_LEVEL_IND, lvl);
    snake->setHPLevel(lvl);
    snake->setHPFromHPLevel();
    snake->setACLevel(lvl);
    snake->setACFromACLevel();
    snake->setDamLevel(lvl);

    if (!IS_SET(snake->specials.act, ACT_SENTINEL))
      SET_BIT(snake->specials.act, ACT_SENTINEL);

    if (!snake->master)
      caster->addFollower(snake);

    act("After following $n, the coiled snake bares its fangs and springs at $N!", TRUE, caster, 0, victim, TO_NOTVICT);
    act("After following you, the coiled snake bares its fangs and springs at $N!", TRUE, caster, 0, victim, TO_CHAR);
    act("After following $n, the snake bares its fangs and springs to attack you!", TRUE, caster, 0, victim, TO_VICT);


    aff.type = SPELL_STICKS_TO_SNAKES;
    aff.level = level;
    aff.duration = 32000;
    aff.location = APPLY_NONE;
    aff.modifier = 0;
    aff.bitvector = 0;
    snake->affectTo(&aff);

    if (snake->reconcileDamage(victim, 0, SPELL_STICKS_TO_SNAKES) == -1)
       return SPELL_SUCCESS + VICTIM_DEAD;

    return SPELL_SUCCESS;

  } else {
    snake->setLevel(WARRIOR_LEVEL_IND, lvl);
    snake->setHPLevel(lvl);
    snake->setHPFromHPLevel();
    snake->setACLevel(lvl);
    snake->setACFromACLevel();
    snake->setDamLevel(lvl);

    switch (critFail(caster, SPELL_STICKS_TO_SNAKES)) {
      case CRIT_F_HITOTHER:
      case CRIT_F_HITSELF:
        CF(SPELL_STICKS_TO_SNAKES);
        act("$n loses control of the magic $e has unleashed!", TRUE, caster, 0, snake, TO_ROOM);
        act("You lose control of the magic you have unleashed!", TRUE, caster, 0, snake, TO_CHAR);

        rc = snake->hit(caster);
        if (IS_SET_DELETE(rc, DELETE_VICT)) {
          return SPELL_CRIT_FAIL + CASTER_DEAD;
        }
        if (rc == DELETE_THIS) {
          delete snake;
          snake = NULL;
        }
        return SPELL_CRIT_FAIL;
      default:
        break;
    }
    caster->sendTo("You don't seem to have control of the snake.\n\r");
    act("The snake seems to have no interest in $n!", TRUE, caster, 0, snake, TO_ROOM);
    caster->sendTo("The snake reverts back into sticks.\n\r");
    act("$N reverts back into the sticks from which it came.", TRUE, caster, 0, snake, TO_ROOM);
    *snake->roomp += *read_object(OBJ_INERT_STICK, VIRTUAL);
    delete snake;
    snake = NULL;
    return SPELL_FAIL;
  }
}


int sticksToSnakes(TBeing * caster, TBeing * victim, TMagicItem * obj)
{
  int ret = 0;
  int rc=0;

  ret=sticksToSnakes(caster,victim,obj->getMagicLevel(),obj->getMagicLearnedness());
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

int sticksToSnakes(TBeing * caster, TBeing * victim)
{
  int level,ret;
  int rc = 0;

  if (!bPassMageChecks(caster, SPELL_STICKS_TO_SNAKES, victim)) 
    return FALSE;

  level = caster->getSkillLevel(SPELL_STICKS_TO_SNAKES);
  int bKnown = caster->getSkillValue(SPELL_STICKS_TO_SNAKES);

  ret=sticksToSnakes(caster, victim, level, bKnown);
  if (IS_SET(ret, CASTER_DEAD))
    ADD_DELETE(rc, DELETE_THIS);
  return rc;

}

// END STICKS TO SNAKES
// CONTROL UNDEAD

int controlUndead(TBeing *caster,TBeing *victim,int level,byte bKnown)
{
  affectedData aff;
  char buf[256];

  if (!victim->isUndead()) {
    caster->sendTo("Umm...that's not an undead creature. You can't charm it.\n\r");
    act("Nothing seems to happen.", FALSE, caster, NULL, NULL, TO_ROOM);
    return SPELL_FAIL;
  } 

  if (caster->isAffected(AFF_CHARM)) {
    sprintf(buf, "You can't charm $N -- you're busy taking orders yourself!");
    act("Nothing seems to happen.", FALSE, caster, NULL, NULL, TO_ROOM);
    act(buf, FALSE, caster, NULL, victim, TO_CHAR);
    return SPELL_FAIL;
  }

#if 0
  // all undead are immune charm, bypass this
  if (victim->isAffected(AFF_CHARM)) {
    again = (victim->master == caster);
    sprintf(buf, "You can't charm $N%s -- $E's busy following %s!", (again ? " again" : ""), (again ? "you already" : "somebody else"));
    act("Nothing seems to happen.", FALSE, caster, NULL, NULL, TO_ROOM);
    act(buf, FALSE, caster, NULL, victim, TO_CHAR);
    return SPELL_FAIL;
  }
#endif

  if (caster->tooManyFollowers(victim, FOL_ZOMBIE)) {
    act("$N refuses to enter a group the size of yours!", TRUE, caster, NULL, victim, TO_CHAR);
    act("$N refuses to enter a group the size of $n's!", TRUE, caster, NULL, victim, TO_ROOM);
    return SPELL_FAIL;
  }

  if (victim->circleFollow(caster)) {
    caster->sendTo("Umm, you probably don't want to follow each other around in circles.\n\r");
    act("Nothing seems to happen.", FALSE, caster, NULL, NULL, TO_ROOM);
    return SPELL_FAIL;
  }

  // note: no charm immune check : undead always immune
  if ((!victim->isPc() && dynamic_cast<TMonster *>(victim)->Hates(caster, NULL)) ||
       caster->isNotPowerful(victim, level, SPELL_CONTROL_UNDEAD, SILENT_YES)) {

      act("Something went wrong!  All you did was piss $N off!", 
                FALSE, caster, NULL, victim, TO_CHAR);
      act("Nothing seems to happen.", FALSE, caster, NULL, victim, TO_NOTVICT);
      act("$n just tried to charm you!", FALSE, caster, NULL, victim, TO_VICT);
      victim->failCharm(caster);
      return SPELL_FAIL;
  }

  if (bSuccess(caster, bKnown, caster->getPerc(), SPELL_CONTROL_UNDEAD)) {
    if (victim->master)
      victim->stopFollower(TRUE);
    caster->addFollower(victim);

    aff.type = SPELL_CONTROL_UNDEAD;
    aff.level = level;
    aff.modifier = 0;
    aff.location = APPLY_NONE;
    aff.bitvector = AFF_CHARM;
    aff.duration = caster->followTime(); 
    aff.duration = (int) (caster->percModifier() * aff.duration);

    switch (critSuccess(caster, SPELL_CONTROL_UNDEAD)) {
      case CRIT_S_DOUBLE:
        CS(SPELL_CONTROL_UNDEAD);
        aff.duration *= 2;
        break;
      case CRIT_S_TRIPLE:
        CS(SPELL_CONTROL_UNDEAD);
        aff.duration *= 3;
        break;
      default:
        break;
    } 
    victim->affectTo(&aff);

    aff.type = AFFECT_THRALL;
    aff.be = static_cast<TThing *>((void *) mud_str_dup(caster->getName()));
    victim->affectTo(&aff);

    if (!victim->isPc())
      dynamic_cast<TMonster *>(victim)->genericCharmFix();

    act("You feel an overwhelming urge to follow $n!",
            FALSE, caster, NULL, victim, TO_VICT);
    act("You decide to do whatever $e says!",
            FALSE, caster, NULL, victim, TO_VICT);
    act("$N has become charmed by $n!", FALSE, caster, NULL, victim, TO_NOTVICT);
    act("$N is now controlled by you.", 0, caster, 0, victim, TO_CHAR);
    return SPELL_SUCCESS;
  } else {
    act("Something went wrong!", FALSE, caster, NULL, victim, TO_CHAR);
    act("All you did was piss $N off!", FALSE, caster, NULL, victim, TO_CHAR);
    act("Nothing seems to happen.", FALSE, caster, NULL, NULL, TO_ROOM);
    act("$n just tried to charm you!", FALSE, caster, NULL, victim, TO_VICT);
    victim->failCharm(caster);
    return SPELL_FAIL;
  }
}

void controlUndead(TBeing * caster, TBeing * victim, TMagicItem *obj)
{
  int ret;

  if (caster != victim) {
    act("$p attempts to bend $N to your will.",
          FALSE, caster, obj, victim, TO_CHAR);
    act("$p attempts to bend you to $n's will.",
          FALSE, caster, obj, victim, TO_VICT);
    act("$p attempts to bend $N to $n's will.",
          FALSE, caster, obj, victim, TO_NOTVICT);
  } else {
    act("$p tries to get you to control yourself.",
          FALSE, caster, obj, 0, TO_CHAR);
    act("$p tries to get $n to control $mself.",
          FALSE, caster, obj, 0, TO_ROOM);
  }
 
  ret=controlUndead(caster,victim,obj->getMagicLevel(),obj->getMagicLearnedness());
 
  return;
}

void controlUndead(TBeing * caster, TBeing * victim)
{
  int ret,level;

  if (!bPassClericChecks(caster,SPELL_CONTROL_UNDEAD))
    return;

  level = caster->getSkillLevel(SPELL_CONTROL_UNDEAD);
  int bKnown = caster->getSkillValue(SPELL_CONTROL_UNDEAD);

  ret=controlUndead(caster,victim,level,bKnown);
}

// END CONTROL UNDEAD
// START CLARITY

int clarity(TBeing *caster, TBeing *victim, int level, byte bKnown)
{
  affectedData aff;

  caster->reconcileHelp(victim, discArray[SPELL_CLARITY]->alignMod);

  if (bSuccess(caster, bKnown, SPELL_CLARITY)) {
    aff.type = SPELL_CLARITY;
    aff.duration = 6+level / 3 * UPDATES_PER_MUDHOUR;
    aff.modifier = 0;
    aff.location = APPLY_NONE;
    aff.bitvector = AFF_TRUE_SIGHT;

    switch (critSuccess(caster, SPELL_CLARITY)) {
      case CRIT_S_DOUBLE:
      case CRIT_S_TRIPLE:
      case CRIT_S_KILL:
        CS(SPELL_CLARITY);
        aff.duration *= 2;
        break;
      case CRIT_S_NONE:
        break;
    }

    if (!victim->affectJoin(caster, &aff, AVG_DUR_NO, AVG_EFF_YES)) {
      caster->nothingHappens();
      return SPELL_FALSE;
    }


    victim->sendTo("Your eyes flash.\n\r");
    act("$n's eyes glow <G>green<1>.", FALSE, victim, 0, 0, TO_ROOM);

    return SPELL_SUCCESS;
  } else {
    caster->nothingHappens();
    return SPELL_FAIL;
  }
}

void clarity(TBeing *caster, TBeing *victim, TMagicItem * obj)
{
  clarity(caster,victim,obj->getMagicLevel(),obj->getMagicLearnedness());
}

int clarity(TBeing *caster, TBeing *victim)
{
  taskDiffT diff;

    if (!bPassShamanChecks(caster, SPELL_CLARITY, victim))
       return FALSE;

     lag_t rounds = discArray[SPELL_CLARITY]->lag;
     diff = discArray[SPELL_CLARITY]->task;

     start_cast(caster, victim, NULL, caster->roomp, SPELL_CLARITY, diff, 1, "", 
rounds, caster->in_room, 0, 0,TRUE, 0);
      return TRUE;
}

int castClarity(TBeing *caster, TBeing *victim)
{
  int ret,level;

  level = caster->getSkillLevel(SPELL_CLARITY);
  int bKnown = caster->getSkillValue(SPELL_CLARITY);

  ret=clarity(caster,victim,level,bKnown);
    return TRUE;
}
// END CLARITY
