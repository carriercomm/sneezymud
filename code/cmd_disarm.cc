//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: cmd_disarm.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#include "stdsneezy.h"
#include "combat.h"

bool TBeing::canDisarm(TBeing *victim, silentTypeT silent)
{
  switch (race->getBodyType()) {
    case BODY_PIERCER:
    case BODY_MOSS:
    case BODY_KUOTOA:
    case BODY_MANTICORE:
    case BODY_GRIFFON:
    case BODY_SHEDU:
    case BODY_SPHINX:
    case BODY_LAMMASU:
    case BODY_WYVERN:
    case BODY_DRAGONNE:
    case BODY_HIPPOGRIFF:
    case BODY_CHIMERA:
    case BODY_SNAKE:
    case BODY_NAGA:
    case BODY_ORB:
    case BODY_VEGGIE:
    case BODY_LION:
    case BODY_FELINE:
    case BODY_REPTILE:
    case BODY_DINOSAUR:
    case BODY_FOUR_LEG:
    case BODY_PIG:
    case BODY_FOUR_HOOF:
    case BODY_ELEPHANT:
    case BODY_BAANTA:
    case BODY_AMPHIBEAN:
    case BODY_FROG:
    case BODY_MIMIC:
      if (!silent)
        sendTo("You have the wrong bodyform for grappling.\n\r");
      return FALSE;
    default:
      break;
  }
  if (checkPeaceful("You feel too peaceful to contemplate violence.\n\r"))
    return FALSE;

  if (getCombatMode() == ATTACK_BERSERK) {
    if (!silent)
      sendTo("You are berserking! You can't focus enough to disarm anyone!\n\r ");
    return FALSE;
  }

  if (victim == this) {
    if (!silent)
      sendTo("Aren't we funny today...\n\r");
    return FALSE;
  }

  if (riding) {
    if (!silent)
      sendTo("Yeah... right... while mounted.\n\r");
    return FALSE;
  }
  if (victim->isFlying() && (victim->fight() != this)) {
    if (!silent)
      sendTo("You can only disarm fliers that are fighting you.\n\r");
    return FALSE;
  }
  if (!victim->heldInPrimHand() && !victim->heldInSecHand()) {
    if (!silent)
      act("$N is not wielding anything.",FALSE, this, 0, victim, TO_CHAR);
    return FALSE;
  }
  if (isHumanoid()) {
    if (bothArmsHurt()) {
      if (!silent)
        act("You need working arms to disarm!", FALSE, this, 0, 0, TO_CHAR);
      return FALSE;
    }
  }

  if (victim->attackers > 3) {
    if (!silent)
      act("There is no room around $N to disarm $M.", FALSE, this, 0, victim, TO_CHAR);
    return FALSE;
  }
  if (attackers > 3) {
    if (!silent)
      sendTo("There is no room to disarm!\n\r");
    return FALSE;
  }
#if 0
  if (!equipment[getPrimaryHold()]) {
    sendTo("Your primary hand must be FREE in order to attempt a disarm!\n\r");
    return FALSE;
  }
#endif

  return TRUE;
}

static int disarm(TBeing * caster, TBeing * victim, spellNumT skill) 
{
  int percent;
  int level, i = 0;

  if (!caster->canDisarm(victim, SILENT_NO))
    return FALSE;

  const int disarm_move = 20;
  if (caster->getMove() < disarm_move) {
    caster->sendTo("You are too tired to attempt a disarm maneuver!\n\r");
    return FALSE;
  }
  caster->addToMove(-disarm_move);

  level = caster->getSkillLevel(skill);
  int bKnown = caster->getSkillValue(skill);

  if (caster->isNotPowerful(victim, level, skill, SILENT_YES)) {
    act("You try to disarm $N, but fail miserably.",
           TRUE, caster, 0, victim, TO_CHAR);
    if (caster->isHumanoid())
      act("$n does a nifty fighting move, but then falls on $s butt.",
           TRUE, caster, 0, 0, TO_ROOM);
    else {
      act("$n lunges at you, but fails to accomplish anything.", 
              TRUE, caster, 0, victim, TO_VICT);
      act("$n lunges at $N, but fails to accomplish anything.",
              TRUE, caster, 0, victim, TO_NOTVICT);
    }
    caster->setPosition(POSITION_SITTING);
    if (dynamic_cast<TMonster *>(victim) && victim->awake() && !victim->fight()) 
      caster->reconcileDamage(victim, 0, skill);;

    return TRUE;
  }

  percent = 0;
  percent += caster->getDexReaction() * 5;
  percent -= victim->getDexReaction() * 5;

  // if my hands are empty, make it easy  
  if (!caster->heldInPrimHand() &&
//      !caster->hasClass(CLASS_MONK) &&
      caster->isHumanoid())
    percent += 10;

  // if i am an equipped monk, make it tough
  if (caster->heldInPrimHand() && caster->hasClass(CLASS_MONK))
    percent -= 10;

  i = caster->specialAttack(victim,skill);
  if (i && bKnown >= 0 && i != GUARANTEED_FAILURE &&
      bSuccess(caster, bKnown + percent, skill)) {
    TObj * obj = NULL;
    if (victim->heldInPrimHand()) {
      TThing *tt = victim->unequip(victim->getPrimaryHold());
      obj = dynamic_cast<TObj *>(tt);;
    } 
    if (!obj) {
      TThing *tt = victim->unequip(victim->getSecondaryHold());
      obj = dynamic_cast<TObj *>(tt);;
    }
    if (obj) {
      act("You attempt to disarm $N.", TRUE, caster, 0, victim, TO_CHAR);
      if (caster->isHumanoid())
        act("$n makes an impressive fighting move.", TRUE, caster, 0, 0, TO_ROOM);
      else {
        act("$n lunges at $N!",
             TRUE, caster, 0, victim, TO_NOTVICT);
        act("$n lunges at you!",
             TRUE, caster, 0, victim, TO_VICT);
      }
      act("You send $p flying from $N's grasp.", FALSE, caster, obj, victim, TO_CHAR);
      act("$p flies from your grasp.", FALSE, caster, obj, victim, TO_VICT, ANSI_RED);
      act("$p flies from $N's grasp.", FALSE, caster, obj, victim, TO_NOTVICT);
      *victim->roomp += *obj;
      victim->logItem(obj, CMD_DISARM);
    } else {
      act("You try to disarm $N, but $E doesn't have a weapon.", TRUE, caster, 0, victim, TO_CHAR);
      act("$n makes an impressive fighting move, but does little more.", TRUE, caster, 0, 0, TO_ROOM);
    }
    caster->reconcileDamage(victim, 0, skill);;

    victim->addToWait(combatRound(1));
    caster->reconcileHurt(victim, 0.01);
  } else {
    act("You try to disarm $N, but fail miserably, falling down in the process.", TRUE, caster, 0, victim, TO_CHAR, ANSI_YELLOW);
    act("$n does a nifty fighting move, but then falls on $s butt.", TRUE, caster, 0, 0, TO_ROOM);
    caster->setPosition(POSITION_SITTING);
    caster->reconcileDamage(victim, 0, skill);;
  }
  return TRUE;
}

int TBeing::doDisarm(const char *argument, TThing *v) 
{
  char v_name[MAX_INPUT_LENGTH];
  TBeing * victim = NULL;
  int rc;

  spellNumT skill = getSkillNum(SKILL_DISARM);
  

  if (checkBusy(NULL)) {
    return FALSE;
  }
  only_argument(argument, v_name);
  if (!v) {
    if (!(victim = get_char_room_vis(this, v_name))) {
      if (!(victim = fight())) {
        if (!*argument) {
          sendTo("Syntax: disarm <person | item>\n\r");
          return FALSE;
        } else {
          rc = disarmTrap(argument, NULL);
          if (IS_SET_DELETE(rc, DELETE_THIS))
            return DELETE_THIS;
          return FALSE;
        }
      }
    }
  } else {
    // v is either a being or an obj, unknown at this point
    victim = dynamic_cast<TBeing *>(v);

    if (!victim) {
      TObj *to = dynamic_cast<TObj *>(v);
      if (to) {
        rc = disarmTrap(argument, to);
        if (IS_SET_DELETE(rc, DELETE_THIS))
          return DELETE_THIS;
        return FALSE;
      }      
    }      
  }
  if (!doesKnowSkill(skill)) {
    sendTo("You know nothing about how to disarm someone.\n\r");
    return FALSE;
  }
  if (!sameRoom(victim)) {
    sendTo("That person isn't around.\n\r");
    return FALSE;
  }
  rc = disarm(this, victim, skill);
  if (IS_SET_DELETE(rc, DELETE_THIS))
    return DELETE_THIS;
  if (rc)
    addSkillLag(skill);

  return TRUE;
}

