//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: cmd_rescue.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#include "stdsneezy.h"

static int rescue(TBeing * caster, TBeing * victim, spellNumT skill)
{
  TBeing *tmp_ch = NULL;
  int percent = 0;

  if (caster->getCombatMode() == ATTACK_BERSERK) {
    caster->sendTo("You are berserking! You can't focus enough to rescue anyone!\n\r");
    return FALSE;
  }
 
  if (caster->checkPeaceful("No one should need rescuing here.\n\r"))
    return FALSE;

  if (victim == caster) {
    caster->sendTo("What about fleeing instead?\n\r");
    return FALSE;
  }
  if (dynamic_cast<TBeing *>(victim->riding)) {
    caster->sendTo("You can't rescue a mounted person!\n\r");
    return FALSE;
  }
  if (victim->riding) {
    caster->sendTo(COLOR_MOBS, "You can't rescue %s off of %s!\n\r", 
         victim->getName(), victim->riding->getName());
    return FALSE;
  }
  if (!victim->isPc() && !victim->isPet()) {
    caster->sendTo("You can only rescue PCs or pets.\n\r");
    return FALSE;
  }

  if (caster->fight() == victim) {
    caster->sendTo("How can you rescue someone you are trying to kill?\n\r");
    return FALSE;
  }
  if (victim->attackers > 3) {
    caster->sendTo("You can't get close enough to them to rescue!\n\r");
    return FALSE;
  }
#if 0
  if (!caster->inGroup(victim)) {
    act("You need to be a part of $S group to rescue $N.", 
             FALSE, caster, 0, victim, TO_CHAR);
    return FALSE;
  }
#endif
  
  TThing *t;
  for (t = victim->roomp->stuff; t; t = t->nextThing, tmp_ch = NULL) {
    tmp_ch = dynamic_cast<TBeing *>(t);
    if (!tmp_ch)
      continue;
    if (tmp_ch->fight() == victim)
      break;
  }

  if (!tmp_ch) {
    act("But nobody is fighting $M?", FALSE, caster, 0, victim, TO_CHAR);
    return FALSE;
  }

  // int level = caster->getSkillLevel(skill);
  int bKnown = caster->getSkillValue(skill);

  if (bSuccess(caster, bKnown + percent, skill)) {
    caster->sendTo("Banzai! To the rescue...\n\r");
    act("You are rescued by $N, you are confused!",
         FALSE, victim, 0, caster, TO_CHAR);
    act("$n heroically rescues $N.", FALSE, caster, 0, victim, TO_NOTVICT);

    if (victim->fight() == tmp_ch)
      victim->stopFighting();
    if (tmp_ch->fight())
      tmp_ch->stopFighting();
    if (caster->fight())
      caster->stopFighting();

    caster->setCharFighting(tmp_ch);
    caster->setVictFighting(tmp_ch);

    victim->addToWait(combatRound(1));

    if (tmp_ch->GetMaxLevel() >= victim->GetMaxLevel()) {
      if (caster->getFactionTarget() != victim) {
        caster->reconcileHelp(victim, discArray[skill]->alignMod);
        caster->setFactionTarget(victim);
      } else {
        // rescuing over and over, or rescuing back and forth
        // let it help some but not as much
        caster->reconcileHelp(victim, discArray[skill]->alignMod/4.0);
      }
    }
    return TRUE;
  } else {
    caster->sendTo("You fail the rescue.\n\r");
    act("$n attempts to rescue you, but fails miserably.",
       FALSE, caster, 0, victim, TO_VICT);
    act("$n attempts to rescue $N, but fails miserably.",
       FALSE, caster, 0, victim, TO_NOTVICT);

    // start them fighting at least
    if (!caster->fight())
      caster->setCharFighting(tmp_ch);
  }
  return TRUE;
}

int TBeing::doRescue(const char *argument)
{
  int rc;
  TBeing *victim;
  char name_buf[240];

  spellNumT skill = getSkillNum(SKILL_RESCUE);

  if (checkBusy(NULL))
    return FALSE;

  if (!doesKnowSkill(skill)) {
    sendTo("You know nothing about rescuing.\n\r");
    return FALSE;
  }
  only_argument(argument, name_buf);

  if (!(victim = get_char_room_vis(this, name_buf))) {
    sendTo("Whom do you want to rescue?\n\r");
    return FALSE;
  }
  if (!sameRoom(victim)) {
    sendTo("That person isn't around.\n\r");
    return FALSE;
  }
  rc = rescue(this, victim, skill);
  if (rc)
    addSkillLag(skill);
  if (IS_SET_DELETE(rc, DELETE_THIS))
    return DELETE_THIS;

  return rc;
}

