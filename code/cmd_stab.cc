#include "stdsneezy.h"
#include "combat.h"

spellNumT doStabMsg(TBeing *tThief, TBeing *tSucker, TGenWeapon *tWeapon, wearSlotT tLimb, int & tDamage, int tSever)
{
  const float tDamageValues[MAX_WEAR] =
  { 0.00, // Nowhere
    1.50, // Head
    1.10, // Neck
    1.05, // Body
    1.10, // Back
    0.90, // Arm-R
    0.90, // Arm-L
    0.80, // Wrist-R
    0.80, // Wrist-L
    0.40, // Hand-R
    0.40, // Hand-L
    0.20, // Finger-R
    0.20, // Finger-L
    1.20, // Waiste
    1.10, // Leg-R
    1.10, // Leg-L
    0.30, // Foot-R
    0.30, // Foot-L
    0.00, // Hold-R
    0.00, // Hold-L
    1.10, // Ex-Leg-R
    1.10, // Ex-Leg-L
    0.30, // Ex-Foot-R
    0.30, // Ex-Foot-L
  };

  spellNumT tDamageType = DAMAGE_NORMAL;

  switch (tLimb) {
    case WEAR_HEAD:
      tDamageType = DAMAGE_CAVED_SKULL;
      break;
    case WEAR_NECK:
      tDamageType = DAMAGE_BEHEADED;
      break;
    case WEAR_BODY:
      tDamageType = DAMAGE_IMPALE;
      break;
    case WEAR_WAISTE:
      tDamageType = DAMAGE_DISEMBOWLED;
      break;
    case WEAR_BACK:
      tDamageType = SKILL_STABBING;
      break;
    default:
      tDamageType = TYPE_STAB;
      break;
  }

  // Basically damage is varied based on the limb here.
  tDamage = (int) ((float) tDamage * tDamageValues[tLimb]);

  bool tKill = tThief->willKill(tSucker, tDamage, tDamageType, FALSE);

  string tStLimb(tSucker->describeBodySlot(tLimb));

  char tStringChar[256],
       tStringVict[256],
       tStringOthr[256];

  switch (::number(0, 3)) {
    case 0:
      sprintf(tStringChar, "You thrust your $o into $N's %s!", tStLimb.c_str());
      sprintf(tStringVict, "$n thrusts $s $o into your %s!", tStLimb.c_str());
      sprintf(tStringOthr, "$n thrusts $s $o into $N's %s!", tStLimb.c_str());
      break;

    case 1:
      sprintf(tStringChar, "You stab $N in $S %s!", tStLimb.c_str());
      sprintf(tStringVict, "$n stabs you in your %s!", tStLimb.c_str());
      sprintf(tStringOthr, "$n stabs $N in $S %s!", tStLimb.c_str());
      break;

    default:
      sprintf(tStringChar, "You puncture $N's %s with your $o!", tStLimb.c_str());
      sprintf(tStringVict, "$n punctures your %s with $S $o!", tStLimb.c_str());
      sprintf(tStringOthr, "$n punctures $N's %s with $S $o!", tStLimb.c_str());
      break;
  }

  act(tStringChar, FALSE, tThief, tWeapon, tSucker, TO_CHAR);
  act(tStringVict, FALSE, tThief, tWeapon, tSucker, TO_VICT);
  act(tStringOthr, FALSE, tThief, tWeapon, tSucker, TO_NOTVICT);

  if (tKill) {
    switch (tLimb) {
      case WEAR_HEAD:
        sprintf(tStringChar, "You cause $N's skull to cave in!");
        sprintf(tStringVict, "$n caused your skull to cave in!");
        sprintf(tStringOthr, "$n caused $N's skull to cave in!");
        break;

      case WEAR_NECK:
        sprintf(tStringChar, "You sever $N at the neck!");
        sprintf(tStringVict, "$n severs you at the neck!");
        sprintf(tStringOthr, "$n severs $N at the neck!");
        break;

      case WEAR_BODY:
        sprintf(tStringChar, "You thrust $p DEEP into $N's gut!");
        sprintf(tStringVict, "$n thusts $p DEEP into your gut, good thing you are dead now!");
        sprintf(tStringOthr, "$n thrusts $p DEEP into $N's gut!");
        break;

      case WEAR_WAISTE:
        sprintf(tStringChar, "You slice $N from love handle to love handle!");
        sprintf(tStringVict, "$n slices you from love handle to love handle!");
	sprintf(tStringOthr, "$n slices $N from love handle to love handle!");
        break;

      case WEAR_BACK:
        sprintf(tStringChar, "You carve a rather nice hole in $N's back!");
        sprintf(tStringVict, "$n carves you a good sized hole in your back!");
        sprintf(tStringOthr, "$n carves $N a good sized hole in the back!");
        break;

      default:
        sprintf(tStringChar, "Your stab to $N's %s ceasing their existance!", tStLimb.c_str());
        sprintf(tStringVict, "$n stabs you in your %s, ceasing your existance!", tStLimb.c_str());
        sprintf(tStringOthr, "$n stabs $N in $S %s, ceasing their existance!", tStLimb.c_str());
        break;
    }

    act(tStringChar, FALSE, tThief, tWeapon, tSucker, TO_CHAR);
    act(tStringVict, FALSE, tThief, tWeapon, tSucker, TO_VICT);
    act(tStringOthr, FALSE, tThief, tWeapon, tSucker, TO_NOTVICT);
  }

  return tDamageType;
}

static int stab(TBeing *thief, TBeing * victim)
{
  const int STAB_MOVE = 2;
  int rc;
  int level;

  if (thief == victim) {
    thief->sendTo("Hey now, let's not be stupid.\n\r");
    return FALSE;
  }
  if (thief->checkPeaceful("Naughty, naughty.  None of that here.\n\r"))
    return FALSE;

  TGenWeapon * obj = dynamic_cast<TGenWeapon *>(thief->heldInPrimHand());
  if (!obj) {
    thief->sendTo("You need to wield a weapon, to make it a success.\n\r");
    return FALSE;
  }

  if (thief->riding) {
    thief->sendTo("Not while mounted!\n\r");
    return FALSE;
  }

  if (dynamic_cast<TBeing *> (victim->riding)) {
    thief->sendTo("Not while that person is mounted!\n\r");
    return FALSE;
  }

  if (thief->noHarmCheck(victim))
    return FALSE;

  if (!obj->canStab()) {
    act("You can't use $o to stab.",  false, thief, obj, NULL, TO_CHAR);
    return FALSE;
  }

  if (victim->attackers > 3) {
    thief->sendTo("There's not enough room for you to stab!\n\r");
    return FALSE;
  }

  if (thief->getMove() < STAB_MOVE) {
    thief->sendTo("You are too tired to stab.\n\r");
    return FALSE;
  }
  if (IS_SET(victim->specials.act, ACT_GHOST)) {
    // mostly because kill is "you slit the throat", etc.
    thief->sendTo("Ghosts can not be stabbed!\n\r");
    return FALSE;
  }

  thief->addToMove(-STAB_MOVE);

  thief->reconcileHurt(victim,0.06);

  level = thief->getSkillLevel(SKILL_STABBING);
  int bKnown = thief->getSkillValue(SKILL_STABBING);

  int dam = thief->getSkillDam(victim, SKILL_STABBING, level, thief->getAdvLearning(SKILL_STABBING));
  dam = thief->getActualDamage(victim, obj, dam, SKILL_STABBING);

  int i = thief->specialAttack(victim,SKILL_STABBING);


  // Start of test stab-limb code.


  if (gamePort != PROD_GAMEPORT) {
    wearSlotT tLimb  = victim->getPartHit(thief, FALSE);
    int       tSever = 0;

    if (!victim->hasPart(tLimb))
      tLimb = victim->getCritPartHit();

    if (!victim->awake() ||
        (i && (i != GUARANTEED_FAILURE) && bSuccess(thief, bKnown, SKILL_STABBING))) {
      switch (critSuccess(thief, SKILL_STABBING)) {
        case CRIT_S_KILL:
          CS(SKILL_STABBING);
          dam   *=  4;
          tSever = 30;
          break;
        case CRIT_S_TRIPLE:
          CS(SKILL_STABBING);
          dam   *=  3;
          tSever = 15;
          break;
        case CRIT_S_DOUBLE:
          CS(SKILL_STABBING);
          dam   *= 2;
          tSever = 5;
          break;
        case CRIT_S_NONE:
        default:
          tSever = 0;
          break;
      }

      spellNumT tDamageType = doStabMsg(thief, victim, obj, tLimb, dam, tSever);

      if (thief->reconcileDamage(victim, dam, tDamageType) == -1)
        return DELETE_VICT;
    } else {
      switch (critFail(thief, SKILL_STABBING)) {
        case CRIT_F_HITSELF:
        case CRIT_F_HITOTHER:
          act("You over thrust and fall flat on your face!",
              FALSE, thief, obj, victim, TO_CHAR);
          act("$n misses $s thrust into $N and falls flat on their face!",
              FALSE, thief, obj, victim, TO_NOTVICT);
          act("$n misses $s thrust into you and falls flat on their face!",
              FALSE, thief, obj, victim, TO_VICT);
          rc = thief->crashLanding(POSITION_SITTING);

          if (IS_SET_DELETE(rc, DELETE_THIS))
            return DELETE_THIS;

          break;
        case CRIT_F_NONE:
        default:
          act("You miss your thrust into $N.",
              FALSE, thief, obj, victim, TO_CHAR);
          act("$n misses $s thrust into $N.",
              FALSE, thief, obj, victim, TO_NOTVICT);
          act("$n misses $s thrust into you.",
              FALSE, thief, obj, victim, TO_NOTVICT);
          break;
      }

      thief->reconcileDamage(victim, 0, SKILL_STABBING);
    }

    return FALSE;
  }


  // End of test stab-limb code.


  if (!victim->awake() || 
      (i &&
       (i != GUARANTEED_FAILURE) &&
       bSuccess(thief, bKnown, SKILL_STABBING))) {

    switch (critSuccess(thief, SKILL_STABBING)) {
      case CRIT_S_KILL:
        if (!victim->getStuckIn(WEAR_BODY)) {
          CS(SKILL_STABBING);
          act("You thrust $p ***REALLY DEEP*** into $N and twist.", 
               FALSE, thief, obj, victim, TO_CHAR);
          act("$n thrusts $p ***REALLY DEEP*** into $N and twists.", 
               FALSE, thief, obj, victim, TO_NOTVICT);
          act("$n thrusts $p ***REALLY DEEP*** into you and twists it.", 
               FALSE, thief, obj, victim, TO_VICT);

          dam *= 4;
          act("You hit exceptionally well but lost your grasp on $p.", 
                 FALSE, thief, obj, victim, TO_CHAR, ANSI_RED);
          act("$n left $s $o stuck in you.", 
                 FALSE, thief, obj, victim, TO_VICT, ANSI_ORANGE);
          act("$n loses $s grasp on $p.", 
                 TRUE, thief, obj, victim, TO_NOTVICT);

          rc = victim->stickIn(thief->unequip(thief->getPrimaryHold()) ,WEAR_BODY);
          if (IS_SET_DELETE(rc, DELETE_THIS))
            return DELETE_VICT;
          victim->rawBleed(WEAR_BODY, 150, SILENT_NO, CHECK_IMMUNITY_YES);
          break;
        }  // if already stuckIn, drop through to next
      case CRIT_S_TRIPLE:
        CS(SKILL_STABBING);
        act("You thrust $p REALLY DEEP into $N and twist.", 
               FALSE, thief, obj, victim, TO_CHAR);
        act("$n thrusts $p REALLY DEEP into $N and twists.", 
               FALSE, thief, obj, victim, TO_NOTVICT);
        act("$n thrusts $p REALLY DEEP into you and twists it.", 
               FALSE, thief, obj, victim, TO_VICT);

        dam *= 3;
        break;
      case CRIT_S_DOUBLE:
        CS(SKILL_STABBING);
        dam *= 2;
        act("You thrust $p DEEP into $N and twist.", 
               FALSE, thief, obj, victim, TO_CHAR);
        act("$n thrusts $p DEEP into $N and twists.", 
               FALSE, thief, obj, victim, TO_NOTVICT);
        act("$n thrusts $p DEEP into you and twists it.", 
               FALSE, thief, obj, victim, TO_VICT);

        break;
      case CRIT_S_NONE:
        act("You thrust $p into $N and twist.", 
               FALSE, thief, obj, victim, TO_CHAR);
        act("$n thrusts $p into $N and twists.", 
               FALSE, thief, obj, victim, TO_NOTVICT);
        act("$n thrusts $p into you and twists it.", 
               FALSE, thief, obj, victim, TO_VICT);

        break;
    }
    if (thief->willKill(victim, dam, SKILL_STABBING, FALSE)) {
      act("Your hand is coated in ichor as you slit $N's guts!",
               FALSE, thief, obj, victim, TO_CHAR, ANSI_RED);
      act("Ichor spews from the gaping stab wound $n leaves in $N's lifeless body!",
               TRUE, thief, obj, victim, TO_NOTVICT);
    }

    if (thief->reconcileDamage(victim, dam, SKILL_STABBING) == -1)
      return DELETE_VICT;
  } else {
    switch (critFail(thief, SKILL_STABBING)) {
      case CRIT_F_HITSELF:
      case CRIT_F_HITOTHER:
        CF(SKILL_STABBING);
        act("You miss your thrust into $N and stab yourself.", FALSE, thief, obj, victim, TO_CHAR);
        act("$n misses $s thrust into $N and stabs $mself.", FALSE, thief, obj, victim, TO_NOTVICT);
        act("$n misses $s thrust into you and stabs $mself.", FALSE, thief, obj, victim, TO_VICT);
        if (thief->reconcileDamage(thief, dam/3, SKILL_STABBING) == -1)
          return DELETE_THIS;
        break;
      case CRIT_F_NONE:
        act("You miss your thrust into $N.", FALSE, thief, obj, victim, TO_CHAR);
        act("$n misses $s thrust into $N.",  FALSE, thief, obj, victim, TO_NOTVICT);
        act("$n misses $s thrust into you.", FALSE, thief, obj, victim, TO_VICT);
        thief->reconcileDamage(victim, 0, SKILL_STABBING);
    }
  }
  return TRUE;
}

int TBeing::doStab(const char * argument, TBeing *vict)
{
  TBeing *victim;
  char namebuf[256];
  int rc;

  if (!doesKnowSkill(SKILL_STABBING)) {
    sendTo("You haven't learned how to stab yet.\n\r");
    return FALSE;
  }
  if (checkBusy(NULL)) {
    return FALSE;
  }
  only_argument(argument, namebuf);

  if (!(victim = vict)) {
    if (!(victim = get_char_room_vis(this, namebuf))) {
      if (!(victim = fight())) {
        sendTo("Stab whom?\n\r");
        return FALSE;
      }
    }
  }
  if (!sameRoom(victim)) {
    sendTo("That person isn't around.\n\r");
    return FALSE;
  }
  rc = stab(this, victim);
  if (rc)
    addSkillLag(SKILL_STABBING, rc);
  if (IS_SET_DELETE(rc, DELETE_VICT)) {
    if (vict)
      return rc;
    delete victim;
    victim = NULL;
    REM_DELETE(rc, DELETE_VICT);
  }
  return rc;
}
