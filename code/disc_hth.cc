//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//
//////////////////////////////////////////////////////////////////////////


#include "stdsneezy.h"
#include "range.h"
#include "disease.h"
#include "combat.h"
#include "disc_hth.h"

int TBeing::doShove(const char *argument, TBeing *vict)
{
  int rc;
  char name_buf[256], obje[100];
  TBeing * victim;

  spellNumT skill = getSkillNum(SKILL_SHOVE);

  half_chop(argument, name_buf, obje);
  if (!(victim = vict)) {
    if (!(victim = get_char_room_vis(this, name_buf))) {
      if (!(victim = fight())) {
        sendTo("Shove whom?\n\r");
        return FALSE;
      }
    }
  }
  if (victim == this) {
    sendTo("Aren't we funny today...\n\r");
    return FALSE;
  }
  if (checkPeaceful("You can't shove from this room!\n\r"))
    return FALSE;

  if (victim->isImmortal()) {
    sendTo("Oh no you don't!\n\r");
    return FALSE;
  }
  if (noHarmCheck(victim))
    return FALSE;

  if (riding) {
    act("You can't shove very well while on $p.",
            FALSE, this, riding, 0, TO_CHAR);
    return FALSE;
  }
  if (victim->riding) {
    //compare pusher's str to rider's dex
    if ((!victim->rideCheck(victim->plotStat(STAT_CURRENT, STAT_AGI, 3, 18, 12) - plotStat(STAT_CURRENT, STAT_STR, 3, 18, 10))) &&
         !isImmortal()) {
      act("You leap at $N, attempting to topple $M from $S $o, but fail.", 
                TRUE, this, victim->riding, victim, TO_CHAR);
      act("$n leaps at you, attempting to topple you from your $o, but fails.", 
                TRUE, this, victim->riding, victim, TO_VICT);
      act("$n leaps at $N, attempting to topple $M off $S $o, but fails.", 
                TRUE, this, victim->riding, victim, TO_NOTVICT);

      rc = crashLanding(POSITION_RESTING);
      if (IS_SET_DELETE(rc, DELETE_THIS))
        return DELETE_THIS;

      act("You land face-down on the $g.", FALSE, this, 0, 0, TO_CHAR, ANSI_RED);
      addSkillLag(skill, rc);
      reconcileDamage(victim, 0, skill);

      if (!victim->isPc()) {
        TMonster *tmons = dynamic_cast<TMonster *>(victim);
        rc = tmons->takeFirstHit(*this);
        if (IS_SET_DELETE(rc, DELETE_THIS)) {
          if (vict) 
            return rc;
          delete tmons;
          tmons = NULL;
        }
        if (IS_SET_DELETE(rc, DELETE_VICT))
          return DELETE_THIS;
      }
      return TRUE;
    } else {
      act("You leap at $N, toppling $M from $S $o.", 
          TRUE, this, victim->riding, victim, TO_CHAR);
      act("$n leaps at you, toppling you from your $o.", 
          TRUE, this, victim->riding, victim, TO_VICT);
      act("$n leaps at $N, toppling $M off $S $o.",
          TRUE, this, victim->riding, victim, TO_NOTVICT);
      rc = victim->fallOffMount(victim->riding, POSITION_SITTING);
      if (IS_SET_DELETE(rc, DELETE_THIS)) {
        delete victim;
        victim = NULL;
        return TRUE;
      }
      addSkillLag(skill, rc);
      reconcileDamage(victim, 0, skill);
      return TRUE;
    }
  }
  if (!doesKnowSkill(skill)) {
    sendTo("You can't go pushing people around like that.\n\r");
    return FALSE;
  }
  if (!sameRoom(*victim)) {
    sendTo("That person isn't around.\n\r");
    return FALSE;
  }
  rc = shove(this, victim, obje, skill);
  if (IS_SET_DELETE(rc, DELETE_THIS))
    return DELETE_THIS;
  if (rc)
    addSkillLag(skill, rc);
  return rc;
}
 
int shove(TBeing *caster, TBeing * victim, char * direction, spellNumT skill)
{
  int percent, level;
  dirTypeT dr;
  int shove_move = ::number(5,10);
 
  if (caster->getCombatMode() == ATTACK_BERSERK) {
    caster->sendTo("You are berserking! You can't focus enough to shove anyone!\n\r");
    return FALSE;
  }
  if (caster->fight()) {
    caster->sendTo("Not while fighting.\n\r");
    return FALSE;
  }
  if (victim->fight()) {
    act("You can't shove $N while $E is fighting.", FALSE, caster, 0, victim, TO_CHAR);
    return FALSE;
  }
  if (caster->riding) {
    caster->sendTo("You'd need to dismount before doing that.\n\r");
    return FALSE;
  }
  if (victim->rider) {
    caster->sendTo("Uhhh, I don't think you can shove the both of them...\n\r");
    return FALSE;
  }
  if (caster->getMove() < shove_move) {
    act("You lack the vitality to shove $N.", FALSE, caster, 0, victim, TO_CHAR);
    return FALSE;
  }
  caster->addToMove(-shove_move);

  level = caster->getSkillLevel(skill);
  percent = 0;
  percent += caster->getDexReaction() * 5;
  percent -= victim->getDexReaction() * 5;
  percent += (level - victim->GetMaxLevel());

  int bKnown = caster->getSkillValue(skill);
  dr = getDirFromChar(direction);
  if (dr == DIR_NONE) {
    caster->sendTo("You need to give a direction to shove.\n\r");
    return FALSE;
  }
  if (bSuccess(caster, bKnown+ percent, skill)) {
    if (victim->hasClass(CLASS_MONK)) {
      if (min(2*victim->GetMaxLevel(),100) > percent) {
        act("$N deftly resists your shove attempt.", 
            FALSE, caster, 0, victim, TO_CHAR);
        act("$N deftly resists $n's shove attempt.", 
            FALSE, caster, 0, victim, TO_NOTVICT);
        act("You deftly resist $n's attempt to shove you.", 
            FALSE, caster, 0, victim, TO_VICT);
        if (!victim->isPc())
          dynamic_cast<TMonster *>(victim)->aiShoveReact(caster, FALSE, dr); 
      } else 
        caster->throwChar(victim, dr, FALSE, SILENT_NO, false);

      if (!victim->isPc())
        dynamic_cast<TMonster *>(victim)->aiShoveReact(caster, 
                (caster->exitDir(dr) ? TRUE : FALSE), dr);

    } else {
      caster->throwChar(victim, dr, FALSE, SILENT_NO, false);
      if (!victim->isPc())
        dynamic_cast<TMonster *>(victim)->aiShoveReact(caster, 
      				      (caster->exitDir(dr) ? TRUE : FALSE), dr);
    }
  } else {
    act("You try to shove $N to no avail!", 
        TRUE, caster, 0, victim, TO_CHAR);
    act("$n tries to shove $N but has no luck.", 
        TRUE, caster, 0, victim, TO_NOTVICT);
    caster->setCharFighting(victim);
    caster->setVictFighting(victim);
    caster->reconcileHurt(victim, 0.01);
    if (!victim->isPc())
      dynamic_cast<TMonster *>(victim)->aiShoveReact(caster, FALSE, dr);
  }
  return TRUE;
}

int TBeing::parryWarrior(TBeing *v, TThing *weapon, int *dam, int w_type, wearSlotT part_hit)
{
  char buf[256], type[16];

  // presumes warrior is in appropriate position for parry already

  if (!v->doesKnowSkill(SKILL_PARRY_WARRIOR) && hasClass(CLASS_WARRIOR))
    return FALSE;

  w_type -= TYPE_HIT;

  // base amount, modified for difficulty
  // the higher amt is, the more things get blocked
  int amt = (int) (45 * 100 / getSkillDiffModifier(SKILL_PARRY_WARRIOR));

  if (::number(0, 999) >= amt)
    return FALSE;

  // check bSuccess after above check, so that we limit how often we
  // call the learnFrom stuff
  if (bSuccess(v, v->getSkillValue(SKILL_PARRY_WARRIOR), SKILL_PARRY_WARRIOR)) {
    *dam = 0;

    strcpy(type, "parry");

    sprintf(buf, "You %s $n's %s at your %s.", type,
	    attack_hit_text[w_type].singular,
	    v->describeBodySlot(part_hit).c_str());
    act(buf, FALSE, this, 0, v, TO_VICT, ANSI_CYAN);

    sprintf(buf, "$N %ss your %s at $S %s.", type,
	    attack_hit_text[w_type].singular,
	    v->describeBodySlot(part_hit).c_str());                                                          
    act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_CYAN);

    sprintf(buf, "$N %ss $n's %s at $S %s.", type,
	    attack_hit_text[w_type].singular,
	    v->describeBodySlot(part_hit).c_str());
    act(buf, TRUE, this, 0, v, TO_NOTVICT);

    return TRUE;
  }
  return FALSE;
}                  
int TBeing::doParry()
{
  sendTo("Parry is not yet supported in this fashion.\n\r");
  return 0;
} 




