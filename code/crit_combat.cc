//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: crit_combat.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
//      SneezyMUD++ - All rights reserved, SneezyMUD Coding Team
//
//      "crit_combat.cc" - All functions and routines related to combat
//
//////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"
#include "disease.h"
#include "combat.h"
#include "statistics.h"

// returns DELETE_THIS
// return SENT_MESS if oneHit should abort and no further oneHits should occur
int TBeing::critFailureChance(TBeing *v, TThing *weap, spellNumT w_type)
{
  char buf[256];
  TThing *target;
  int rc;
  int num, num2;
  TBeing *tbt = NULL;

  if (isAffected(AFF_ENGAGER))
    return FALSE;

  TObj *weapon = dynamic_cast<TObj *>(weap);
    
  stats.combat_crit_fail++;
  if (::number(1, 300) <= 
      (plotStat(STAT_CURRENT, STAT_KAR, 50, 3, 10) + 10*drunkMinus())) {
    if (isLucky(levelLuckModifier(v->GetMaxLevel())))
      return FALSE;

    stats.combat_crit_fail_pass++;
    if (desc)
      desc->career.crit_misses++;

    // OK, kind of silly, but if you trace the code, you'll realize
    // that a critfail blocks the actual hit, thus if you crit fail
    // on the first hit, no fight starts...
    // do this, to force the fight.  Bat 11/9/98
    reconcileDamage(v, 0, DAMAGE_NORMAL);

    num = dice(1, 20);

    // keep chance of sword in foot low
    while ((num == 11 || num == 12) && ::number(0, 2))
      num = dice(1,20);

    // don't want them "stumbling"
    if ((getPosition() != POSITION_STANDING) || isFlying())
      num = 4;

    switch (num) {
      case 1:
        // Slip, dex check or fall and be stunned for a few rounds.
        if (::number(0, plotStat(STAT_NATURAL, STAT_AGI, -30, 60, 0)) <
            (30 + getCond(DRUNK))) {
          act("In your attempt to swing at $N, you trip and fall!", FALSE, this, NULL, v, TO_CHAR);
          act("In an attempt to swing at $N, $n trips and falls!", TRUE, this, NULL, v, TO_NOTVICT);
          act("In an attempt to swing at you, $n trips and falls!", TRUE, this, NULL, v, TO_VICT);
          addToWait(combatRound(1));
          cantHit += loseRound(::number(1,2));
          setPosition(POSITION_SITTING);
          if (riding)
            dismount(POSITION_SITTING);
        } else {
          act("You start to stumble, but catch yourself before you fall!", FALSE, this, NULL, NULL, TO_CHAR);
          act("$n starts to stumble but catches $mself before $e falls!", TRUE, this, NULL, NULL, TO_ROOM);
        }
        return (SENT_MESS);
      case 2:
        // Slip, dex check or fall and be stunned for some rounds. 
	// Speef changed to Agility but probably use Speed for reaction mod.
        if (::number(0, plotStat(STAT_NATURAL, STAT_AGI, -30, 60, 0)) <
            (30 + getCond(DRUNK))) {
          act("In your attempt to swing at $N, you trip and fall!", FALSE, this, NULL, v, TO_CHAR);
          act("In an attempt to swing at $N, $n trips and falls!", TRUE, this, NULL, v, TO_NOTVICT);
          act("In an attempt to swing at you, $n trips and falls!", TRUE, this, NULL, v, TO_VICT);
          addToWait(combatRound(1));
          cantHit += loseRound(::number(1,3));
          setPosition(POSITION_SITTING);
          if (riding)
            dismount(POSITION_SITTING);
        } else {
          act("You start to stumble, but catch yourself before you fall!", FALSE, this, NULL, NULL, TO_CHAR);
          act("$n starts to stumble but catches $mself before $e falls!", TRUE, this, NULL, NULL, TO_ROOM);
        }
        return (SENT_MESS);
      case 3:
        // Trip and fall. No dex check. Stunned for many rounds.   
        act("In your attempt to swing at $N, you trip and fall!", TRUE, this, NULL, v, TO_CHAR);
        act("In an attempt to swing at $N, $n trips and falls!", TRUE, this, NULL, v, TO_NOTVICT);
        act("In an attempt to swing at you, $n trips and falls!", TRUE, this, NULL, v, TO_VICT);
        addToWait(combatRound(1));
        cantHit += loseRound(::number(1,4));
        setPosition(POSITION_SITTING);
        if (riding)
          dismount(POSITION_SITTING);

        return (SENT_MESS);
      case 4:
        // Lose grip. Dex check or drop weapon!
        if (weapon) {
          if ((::number(0, plotStat(STAT_NATURAL, STAT_AGI, -30, 60, 0)) <
              (30 + getCond(DRUNK))) &&
               weapon->canDrop()) {
            sprintf(buf, "You %slose%s your grip on $p and it %sfalls out of your grasp%s!",
                         red(), norm(), red(), norm());
            act(buf, FALSE, this, weapon, NULL, TO_CHAR);
            act("$n seems to lose $s grip on $p and it falls out of $s grasp!", TRUE, this, weapon, NULL, TO_ROOM);
            *roomp += *unequip(weapon->eq_pos);
          } else {
            act("You start to fumble $p, but miraculously retrieve it before you drop it.", FALSE, this, weapon, NULL, TO_CHAR);
            act("$n starts to fumble $p, but miraculously retrieves it before $e drops it.", TRUE, this, weapon, NULL, TO_ROOM);
          }
          return (SENT_MESS);
        }
      case 5:
        // Lose grip and drop weapon with no dex check. 
        if (weapon && weapon->canDrop()) {
          sprintf(buf, "You %slose%s your grip on $p and it %sfalls out of your grasp%s!",
                       red(), norm(), red(), norm());
          act(buf, FALSE, this, weapon, NULL, TO_CHAR);
          act("$n seems to lose $s grip on $p and it falls out of $s grasp!", TRUE, this, weapon, NULL, TO_ROOM);
          *roomp += *unequip(weapon->eq_pos);
          return (SENT_MESS);
        }
      case 6:
      case 7:
        // Hit self (half damage) 
        if (weapon) {
          act("$n gets clumsy, and nails $mself with $p.", TRUE, this, weapon, v, TO_ROOM);
          act("You become careless, and nail yourself with $p.", FALSE, this, weapon, v, TO_CHAR, ANSI_ORANGE);
          int dam = getWeaponDam(this, weapon,(weapon == heldInPrimHand()));
          dam = getActualDamage(this, weapon, dam, w_type);
          dam /= 4;
          rc = applyDamage(this, dam,w_type);
          if (IS_SET_DELETE(rc, DELETE_VICT))
            return DELETE_THIS;
        } else {
          act("With stunning grace, $n has just struck $mself.", TRUE, this, NULL, v, TO_ROOM);
          act("You strike yourself in your carelessness.", FALSE, this, NULL, v, TO_CHAR, ANSI_ORANGE);
          int dam = getWeaponDam(this, NULL,0);
          dam = getActualDamage(this, NULL, dam, w_type);
          dam /= 4;
          rc = applyDamage(this, dam,w_type);
          if (IS_SET_DELETE(rc, DELETE_VICT))
            return DELETE_THIS;
        }
        return (SENT_MESS);
      case 8:
        // Hit self (full damage) 
        if (weapon) {
          act("$n slips, and smacks $mself with $p.", TRUE, this, weapon, v, TO_ROOM);
          act("You become careless, and smack yourself with your $o.", FALSE, this, weapon, v, TO_CHAR, ANSI_ORANGE);
          int dam = getWeaponDam(this, weapon,(weapon == heldInPrimHand()));
          dam = getActualDamage(this, weapon, dam, w_type);
          dam /= 2;
          rc = applyDamage(this, dam,w_type);
          if (IS_SET_DELETE(rc, DELETE_VICT))
            return DELETE_THIS;
        } else {
          act("With stunning grace, $n has just struck $mself.", TRUE, this, NULL, v, TO_ROOM);
          act("You strike yourself in your carelessness.", FALSE, this, NULL, v, TO_CHAR, ANSI_ORANGE);
          int dam = getWeaponDam(this, NULL,0);
          dam = getActualDamage(this, NULL, dam, w_type);
          dam /= 2;
          rc = applyDamage(this, dam,w_type);
          if (IS_SET_DELETE(rc, DELETE_VICT))
             return DELETE_THIS;
        }
        return (SENT_MESS);
      case 9:
        // Hit friend (half damage) (if no friend hit self) 
        for (target = roomp->stuff; target; target = target->nextThing, tbt = NULL) {
          tbt = dynamic_cast<TBeing *>(target);
          if (!tbt)
            continue;

          if (!tbt->isAffected(AFF_CHARM) && 
               inGroup(tbt) && (tbt != this))
            break;
        }
        if (!tbt) 
          tbt = this;
        
        if (weapon) {
          if (tbt != this) {
            act("$n slips, and smacks $N with $p.", TRUE, this, weapon, tbt, TO_NOTVICT);
            act("You become careless, and smack $N with your $o.", FALSE, this, weapon, tbt, TO_CHAR, ANSI_ORANGE);
            act("$n carelessly smacks you with $p.",TRUE,this,weapon,tbt,TO_VICT, ANSI_ORANGE);
          } else {
            act("$n slips, and smacks $mself with $p.", TRUE, this, weapon, this, TO_ROOM);
            act("You become careless, and smack yourself with your $o.", FALSE, this, weapon, this, TO_CHAR, ANSI_ORANGE);
          }
          int dam = getWeaponDam(tbt, weapon,(weapon == heldInPrimHand()));
          dam = getActualDamage(tbt, weapon, dam, w_type);
          dam /=4;
          rc = applyDamage(tbt, dam,w_type);
          if (tbt != this) {
            // this check added since otherwise a fight between "friends"
            // could break out.
            act("In the confusion, you and $N stop fighting.",
                  FALSE, this, 0, tbt, TO_CHAR);
            act("In the confusion, $n and you stop fighting.",
                  FALSE, this, 0, tbt, TO_VICT);
            act("In the confusion, $n and $N stop fighting.",
                  FALSE, this, 0, tbt, TO_NOTVICT);
            stopFighting();
            tbt->stopFighting();
          }
          if (IS_SET_DELETE(rc, DELETE_VICT)) {
            if (tbt != this) {
              delete tbt;
              tbt = NULL;
            } else
              return DELETE_THIS;
          }
        } else {
          if (tbt != this) {
            act("$n slips, and smacks $N.", TRUE, this, NULL, tbt,TO_NOTVICT);
            act("You become careless, and smack $N.", FALSE, this,NULL, tbt, TO_CHAR, ANSI_ORANGE);
            act("$n carelessly smacks you.",TRUE,this,NULL,tbt,TO_VICT, ANSI_ORANGE);
          } else {
            act("$n slips, and smacks $mself.", TRUE, this, NULL, this, TO_ROOM);
            act("You become careless, and smack yourself.", FALSE,this, NULL, this, TO_CHAR, ANSI_ORANGE);
          }
          int dam = getWeaponDam(tbt, NULL, false);
          dam = getActualDamage(tbt, NULL, dam, w_type);
          dam /=4;
          rc = applyDamage(tbt, dam,w_type);
          if (tbt != this) {
            act("In the confusion, you and $N stop fighting.",
                  FALSE, this, 0, tbt, TO_CHAR);
            act("In the confusion, $n and you stop fighting.",
                  FALSE, this, 0, tbt, TO_VICT);
            act("In the confusion, $n and $N stop fighting.",
                  FALSE, this, 0, tbt, TO_NOTVICT);
            stopFighting();
            tbt->stopFighting();
          }
          if (IS_SET_DELETE(rc, DELETE_VICT)) {
            if (tbt != this) {
              delete tbt;
              tbt = NULL;
            } else
              return DELETE_THIS;
          }
        }
        return SENT_MESS;
      case 10:
        // Hit friend (full damage) (if no friend hit self) 
        for (target = roomp->stuff;target;target = target->nextThing, tbt = NULL) {
          tbt = dynamic_cast<TBeing *>(target);
          if (!tbt)
            continue;
          if (!tbt->isAffected(AFF_CHARM) &&
               inGroup(tbt) && (tbt != this))
            break;
        }
        if (!tbt)
          tbt = this;
        if (weapon) {
          if (tbt != this) {
            act("$n slips, and smacks $N with $p.", TRUE, this, weapon, tbt,TO_NOTVICT);
            act("You become careless, and smack $N with your $o.", FALSE, this,weapon, tbt, TO_CHAR, ANSI_ORANGE);
            act("$n carelessly smacks you with $p.",TRUE,this,weapon,tbt,TO_VICT, ANSI_ORANGE);
          } else {
            act("$n slips, and smacks $mself with $p.", TRUE, this, weapon, this, TO_ROOM);
            act("You become careless, and smack yourself with your $o.", FALSE,this, weapon, this, TO_CHAR, ANSI_ORANGE);
          }
          int dam = getWeaponDam(tbt, weapon,(weapon == heldInPrimHand()));
          dam = getActualDamage(tbt, weapon, dam, w_type);
          dam /= 2;
          rc = applyDamage(tbt, dam,w_type);
          if (tbt != this) {
            act("In the confusion, you and $N stop fighting.",
                  FALSE, this, 0, tbt, TO_CHAR);
            act("In the confusion, $n and you stop fighting.",
                  FALSE, this, 0, tbt, TO_VICT);
            act("In the confusion, $n and $N stop fighting.",
                  FALSE, this, 0, tbt, TO_NOTVICT);
            stopFighting();
            tbt->stopFighting();
          }
          if (IS_SET_DELETE(rc, DELETE_VICT)) {
            if (tbt != this) {
              delete tbt;
              tbt = NULL;
            } else
              return DELETE_THIS;
          }
        } else {
          if (tbt != this) {
            act("$n slips, and smacks $N.", TRUE, this, NULL, tbt,TO_NOTVICT);
            act("You become careless, and smack $N.", FALSE, this,NULL, tbt,TO_CHAR, ANSI_ORANGE);
            act("$n carelessly smacks you.",TRUE,this,NULL,tbt,TO_VICT, ANSI_ORANGE);
          } else {
            act("$n slips, and smacks $mself.", TRUE, this, NULL, this, TO_ROOM);
            act("You become careless, and smack yourself.", FALSE,this, NULL, this, TO_CHAR, ANSI_ORANGE);
          }
          int dam = getWeaponDam(tbt, NULL,0);
          dam = getActualDamage(tbt, NULL, dam, w_type);
          dam /= 2;
          rc = applyDamage(tbt, dam,w_type);
          if (tbt != this) {
            act("In the confusion, you and $N stop fighting.",
                  FALSE, this, 0, tbt, TO_CHAR);
            act("In the confusion, $n and you stop fighting.",
                  FALSE, this, 0, tbt, TO_VICT);
            act("In the confusion, $n and $N stop fighting.",
                  FALSE, this, 0, tbt, TO_NOTVICT);
            stopFighting();
            tbt->stopFighting();
          }
          if (IS_SET_DELETE(rc, DELETE_VICT)) {
            if (tbt != this) {
              delete tbt;
              tbt = NULL;
            } else
              return DELETE_THIS;
          }
        }
        return SENT_MESS;
      case 11:
        // Miss miserably and stick weapon in foot (with dex check). 
        if (::number(0, plotStat(STAT_NATURAL, STAT_AGI, -30, 60, 0)) >=
            (30 + getCond(DRUNK)))
          return 0;
      case 12:
        // Miss miserably and stick weapon in foot (no dex check). 
        if (weapon && hasPart(WEAR_FOOT_R) && !weapon->isBluntWeapon() &&
            !getStuckIn(WEAR_FOOT_R)) {
          sprintf(buf, "You miserably miss $N, %sand stick $p in your foot%s!",
                  red(),norm());
          act(buf, FALSE, this, weapon, v, TO_CHAR);
          sprintf(buf, "$n miserably misses $N, and sticks $p in $s foot!");
          act(buf, TRUE, this, weapon, v, TO_NOTVICT);
          sprintf(buf, "$n miserably misses you, %sand sticks $p in $s foot%s!",
                  v->cyan(),v->norm());
          act(buf, TRUE, this, weapon, v, TO_VICT);
          num2 = getWeaponDam(this,weapon,(weapon == heldInPrimHand()));
          num2 /= 2;
          rc = stickIn(unequip(weapon->eq_pos), WEAR_FOOT_R);
          if (desc)
            desc->career.stuck_in_foot++;

          if (IS_SET_DELETE(rc, DELETE_THIS))
            return DELETE_THIS;

          rc = damageLimb(this,WEAR_FOOT_R,weapon,&num);
          if (IS_SET_DELETE(rc, DELETE_VICT))
            return DELETE_THIS;

          return (SENT_MESS);
        }
        // no weapon, blunt weapn, etc = fall through into next case

      case 13:
       if (!isHumanoid())
         return 0;

        // Fall and twist ankle, affect with new affect that halfs max moves 
        act("You stumble and twist your ankle as you swing at $N!", 
               FALSE, this, 0, v, TO_CHAR);
        act("$n stumbles and twists $s ankle as $e tries to swing at $N!", 
               TRUE, this, 0, v, TO_NOTVICT);
        act("$n stumbles and twists $s ankle as $e tries to swing at you!", 
               TRUE, this, 0, v, TO_VICT);
        setMove(getMove()/2);

        return (SENT_MESS);
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
        break;
      default:
        act("$n stumbles as $e tries pathetically to attack $N.", TRUE, this, 0, v, TO_NOTVICT);
        act("You stumble pathetically as you try to attack $N.", FALSE, this, 0, v, TO_CHAR);
        act("$n stumbles as $e tries pathetically to attack you.", TRUE, this, 0, v, TO_VICT);
        return (SENT_MESS);
    }
  }
  return FALSE;
}

// This will just be a big ass case statement based on random diceroll 
// returns DELETE_VICT if v dead
// mod is -1 from generic combat, mod == crit desired from immortal command.
int TBeing::critSuccessChance(TBeing *v, TThing *weapon, wearSlotT *part_hit, spellNumT wtype, int *dam, int mod)
{
  int num, dicenum, dexstat, new_wtype, i;
  char buf[256], buf2[256];
  wearSlotT new_slot;
  affectedData af;
  TThing *obj = NULL;
  int rc;
  string limbStr;

  if (isAffected(AFF_ENGAGER))
    return FALSE;

  if (!isImmortal() && 
      (v->isImmortal() || IS_SET(v->specials.act, ACT_IMMORTAL)))
    return FALSE;

  if ((mod == -1) && v->isImmune(getTypeImmunity(wtype), 0))
    return FALSE;

  // get wtype back so it fits in array  
  new_wtype = wtype - TYPE_HIT;

  if (mod == -1) {
    stats.combat_crit_suc++;

    num = ::number(1, 300);
    num += getSkillValue(SKILL_TACTICS) - MAX_SKILL_LEARNEDNESS;  // this is negative unless fully learned
    if (num <=0)
      return FALSE;
  } else 
    num = mod;

  if(doesKnowSkill(SKILL_CRIT_HIT) && isPc()){
    dicenum = dice(1, (int)(100000-(getSkillValue(SKILL_CRIT_HIT)*900)));
  } else 
    dicenum = dice(1, 100000);    // This was 10k under 3.x - Bat

  dexstat = plotStat(STAT_CURRENT, STAT_DEX, 10, 100, 63) - 2*getCond(DRUNK);

  if (isImmortal())
    dicenum /= 10;

  if ((mod != -1) || 
       dicenum <= (dexstat * (50 + GetMaxLevel() - v->GetMaxLevel()))) {
    if (mod == -1) {
      stats.combat_crit_suc_pass++;
      if (desc)
        desc->career.crit_hits++;
      if (v->desc)
        v->desc->career.crit_hits_suff++;
    }

    // play the crit-hit sound
    // boost the priority so that this sound will trump normal combat sounds
    soundNumT snd = pickRandSound(SOUND_CRIT_01, SOUND_CRIT_43);
    playsound(snd, SOUND_TYPE_COMBAT, 100, 45, 1);

    if (pierceType(wtype)) {
      // Do pierce crit 
      if (num <= 33) {
        // double damage 
        *dam <<= 1;
        sprintf(buf, 
   "You strike $N's %s exceptionally well, sinking your %s deep into $S flesh!",
             v->describeBodySlot(*part_hit).c_str(),
             (weapon ? "$o" : getMyRace()->getBodyLimbPierce().c_str()));
        act(buf, FALSE, this, weapon, v, TO_CHAR, ANSI_ORANGE);
        sprintf(buf,
   "$n strikes your %s exceptionally well, sinking $s %s deep into your flesh!",
             v->describeBodySlot(*part_hit).c_str(),
             (weapon ? "$o" : getMyRace()->getBodyLimbPierce().c_str()));
        act(buf, FALSE, this, weapon, v, TO_VICT, ANSI_RED);
        sprintf(buf, 
   "$n strikes $N's %s exceptionally well, sinking $s %s deep into $N's flesh!",
             v->describeBodySlot(*part_hit).c_str(),
             (weapon ? "$o" : getMyRace()->getBodyLimbPierce().c_str()));
        act(buf, FALSE, this, weapon, v, TO_NOTVICT, ANSI_BLUE);
        return (SENT_MESS);
      } else if (num <= 66) {
        // triple damage 
        sprintf(buf, 
    "You critically strike $N's %s, sinking your %s deep into $S flesh!",
             v->describeBodySlot(*part_hit).c_str(),
             (weapon ? "$o" : getMyRace()->getBodyLimbPierce().c_str()));
        act(buf, FALSE, this, weapon, v, TO_CHAR, ANSI_ORANGE);
        sprintf(buf, 
    "$n critically strikes your %s, sinking $s %s deep into your flesh!",
             v->describeBodySlot(*part_hit).c_str(),
             (weapon ? "$o" : getMyRace()->getBodyLimbPierce().c_str()));
        act(buf, FALSE, this, weapon, v, TO_VICT, ANSI_RED);
        sprintf(buf, "$n critically strikes $N's %s, sinking $s %s deep into $N's flesh!",
             v->describeBodySlot(*part_hit).c_str(),
             (weapon ? "$o" : getMyRace()->getBodyLimbPierce().c_str()));
        act(buf, FALSE, this, weapon, v, TO_NOTVICT, ANSI_BLUE);
        *dam *= 3;
        return (SENT_MESS);
      } else {
        // better stuff 
        limbStr =  (weapon ? fname(weapon->name) : getMyRace()->getBodyLimbPierce());
        switch (num) {
          case 67:
            if (!v->hasPart(WEAR_NECK))
              return 0;
            *part_hit = WEAR_NECK;
            if ((obj = v->equipment[WEAR_NECK])) {
              v->sendTo(COLOR_OBJECTS, "Your %s saves you from a punctured larynx!\n\r",
                fname(obj->name).c_str());
              for (i=1;i<5;i++)
                if (v->equipment[WEAR_NECK])
                  v->damageItem(this,WEAR_NECK,wtype,weapon,*dam);
              return SENT_MESS;
            }
            // intentional drop through
          case 68:
            // Punctured Larnyx, can't speak 
            if (!v->hasPart(WEAR_NECK))
              return 0;
            if (v->hasDisease(DISEASE_VOICEBOX))
              return 0;
            sprintf(buf, 
    "You pop your %s into $N's throat, puncturing $S voice box!", 
                     limbStr.c_str());
            act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
            sprintf(buf, 
    "$n pops $s %s into your throat, puncturing your voice box!",
                     limbStr.c_str());
            act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
            sprintf(buf, "$n pops $s %s into $N's throat, puncturing $S voice box!",
                     limbStr.c_str());
            act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
            for (i=1;i<5;i++)
              if (v->equipment[WEAR_NECK])
                v->damageItem(this,WEAR_NECK,wtype,weapon,*dam);
            af.type = AFFECT_DISEASE;
            af.level = 0;   // has to be 0 for doctor to treat
            af.duration = PERMANENT_DURATION;
            af.modifier = DISEASE_VOICEBOX;
            af.location = APPLY_NONE;
            af.bitvector = AFF_SILENT;
            v->affectTo(&af);
            *part_hit = WEAR_NECK;
            if (desc)
              desc->career.crit_voice++;
            if (v->desc)
              v->desc->career.crit_voice_suff++;
            return SENT_MESS;
          case 69:
          case 70:
            // Struct in eye, blinded with new blind type 
            if (v->hasDisease(DISEASE_EYEBALL))
              return 0;
            if (!v->hasPart(WEAR_HEAD))
              return 0;
            sprintf(buf, 
    "You pop your %s into $N's eyes, gouging them out and blinding $M!",
                 limbStr.c_str());
            act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
            sprintf(buf, 
    "$n pops $s %s into your eyes and The World goes DARK!",
                 limbStr.c_str());
            act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
            sprintf(buf, "$n pops $s %s into $N's eyes, gouging them out and blinding $M!",
                 limbStr.c_str());
            act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
            act("$n's eyeballs fall from $s sockets!",TRUE,v,0,0,TO_ROOM);
            v->makeOtherPart(NULL,"eyeballs");
            af.type = AFFECT_DISEASE;
            af.level = 0;   // has to be 0 for doctor to treat
            af.duration = PERMANENT_DURATION;
            af.modifier = DISEASE_EYEBALL;
            af.location = APPLY_NONE;
            af.bitvector = AFF_BLIND;
            v->affectTo(&af);
            v->rawBlind(GetMaxLevel(), af.duration, SAVE_NO);
            *part_hit = WEAR_HEAD;
            if (desc)
              desc->career.crit_eye_pop++;
            if (v->desc)
              v->desc->career.crit_eye_pop_suff++;
            return SENT_MESS;
          case 71:
            if (!v->hasPart(WEAR_LEGS_R))
              return 0;
            if (!v->isHumanoid())
              return 0;
              if ((obj = v->equipment[WEAR_LEGS_R])) {
                v->sendTo(COLOR_OBJECTS, "Your %s saves you from losing a tendon!\n\r",
                  fname(obj->name).c_str());
                for (i=1;i<5;i++)
                  if (v->equipment[WEAR_LEGS_R])
                    v->damageItem(this,WEAR_LEGS_R,wtype,weapon,*dam);
                *part_hit = WEAR_LEGS_R;
                return SENT_MESS;
              }
              // an intentional drop through
            case 72:
              // strike lower leg, rip tendons, vict at -25% move. 
              if (!v->hasPart(WEAR_LEGS_R))
                return 0;
              if (!v->isHumanoid())
                return 0;
              sprintf(buf, 
    "Your %s rips through $N's tendon on $S lower leg!",
                 limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s rips through the tendon in your lower leg.",
                 limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s rips into $N, tearing the tendon in $S lower leg.", 
                 limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->setMove(v->getMove()/4);
              for (i=1;i<5;i++)
                if (v->equipment[WEAR_LEGS_R])
                  v->damageItem(this,WEAR_LEGS_R,wtype,weapon,*dam);
              *part_hit = WEAR_LEGS_R;
              rc = damageLimb(v,WEAR_LEGS_R,weapon,dam);
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              return SENT_MESS;
            case 73:
              if (!v->hasPart(WEAR_BACK))
                return 0;
              if ((obj = v->equipment[WEAR_BACK])) {
                v->sendTo(COLOR_OBJECTS, "Your %s saves you from a gory wound!\n\r",
                  fname(obj->name).c_str());
                for (i=1;i<5;i++)
                  if (v->equipment[WEAR_BACK])
                    v->damageItem(this,WEAR_BACK,wtype,weapon,*dam);
                *part_hit = WEAR_BACK;
                return SENT_MESS;
              }
            case 74:
              // Side wound, vict stunned 6 rounds. 
              if (!v->hasPart(WEAR_BACK))
                return 0;
              strcpy(buf2,v->hssh());
              sprintf(buf, 
    "You plunge your %s deep into $N's side, stunning $M!",
                 limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n plunges $s %s deep into your side.  The agony makes you forget about the fight.",
                 limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n plunges $s %s deep into $N's side, stunning $M.",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              for (i=1;i<5;i++)
                if (v->equipment[WEAR_BACK])
                  v->damageItem(this,WEAR_BACK,wtype,weapon,*dam); 
              v->cantHit += v->loseRound(6);
              rc = dislodgeWeapon(v,weapon,WEAR_BACK);
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              *part_hit = WEAR_BACK;
              return SENT_MESS;
            case 75:
            case 76:
              // Strike in back of head. If no helm, vict dies. 
              if (!v->hasPart(WEAR_HEAD))
                return 0;
              if ((obj = v->equipment[WEAR_HEAD])) {
                sprintf(buf, 
    "You try to thrust your %s into the back of $N's head.",
                    limbStr.c_str());
                act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
                sprintf(buf, "Unfortunately, $p saves $M from a hideous death!");
                act(buf, FALSE, this, obj, v, TO_CHAR);
                sprintf(buf, 
    "$n tries to thrust $s %s into the back of your head.",
                    limbStr.c_str());
                act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
                sprintf(buf,
                   "But $p saves you from a hideous death!");
                act(buf, FALSE, this, obj, v, TO_VICT, ANSI_RED);
                sprintf(buf, "$n tries plunging $s %s into the back of $N's head, but $p saves $M.",
                    limbStr.c_str());
                act(buf, FALSE, this, obj, v, TO_NOTVICT, ANSI_BLUE);
                for (i=1;i<5;i++)
                  if (v->equipment[WEAR_HEAD])
                    v->damageItem(this,WEAR_HEAD,wtype,weapon,*dam);
                rc = dislodgeWeapon(v,weapon,WEAR_HEAD);
                if (IS_SET_DELETE(rc, DELETE_VICT))
                  return DELETE_VICT;
                *part_hit = WEAR_HEAD;
                rc = damageLimb(v,WEAR_HEAD,weapon,dam);
                if (IS_SET_DELETE(rc, DELETE_VICT))
                  return DELETE_VICT;
                return SENT_MESS;
              } else {
                sprintf(buf, 
    "You thrust your %s into the back of $N's head causing an immediate death.",
                    limbStr.c_str());
                act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
                sprintf(buf, 
    "$n's %s tears into the back of your unprotected head.",
                    limbStr.c_str());
                act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
                sprintf(buf,"The world goes black and dark...");
                act(buf, FALSE, this, 0, v, TO_VICT, ANSI_BLACK);
                sprintf(buf, "$n thrusts $s %s deep into the back of $N's unprotected head, causing an immediate death.", 
                    limbStr.c_str());
                act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
                rc = dislodgeWeapon(v,weapon,WEAR_HEAD);
                if (IS_SET_DELETE(rc, DELETE_VICT))
                  return DELETE_VICT;
                applyDamage(v, (20 * v->hitLimit()),wtype);
                *part_hit = WEAR_HEAD;
                if (desc)
                  desc->career.crit_cranial_pierce++;
                if (v->desc)
                  v->desc->career.crit_cranial_pierce_suff++;
                return DELETE_VICT;
              }
              return FALSE;   // not possible, but just in case
            case 77:
            case 78:
              // Strike shatters elbow in weapon arm. Arm broken 
              new_slot = v->getSecondaryArm();
              if (!v->hasPart(new_slot))
                return 0;
              if (!v->isHumanoid())
                return FALSE;
              sprintf(buf, 
    "$N blocks your %s with $S arm.  However the force shatters $S elbow!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s is blocked by your arm.  Unfortunately your elbow is shattered!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s shatters $N's elbow!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->damageArm(FALSE,PART_BROKEN);
              if (desc)
                desc->career.crit_broken_bones++;
              if (v->desc)
                v->desc->career.crit_broken_bones_suff++;
              *part_hit = new_slot;
              rc = damageLimb(v,new_slot,weapon,dam);
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              return SENT_MESS;
            case 79:
              new_slot = v->getPrimaryHand();
              if (!v->hasPart(new_slot))
                return 0;
              if (!v->isHumanoid())
                return 0;
              if ((obj = v->equipment[v->getPrimaryWrist()])) {
                act("Your $o just saved you from losing your hand!",TRUE,v,obj,0,TO_CHAR, ANSI_PURPLE);
                act("You nearly sever $N's hand, but $S $o saved $M!",TRUE,this,obj,v,TO_CHAR);
                *part_hit = v->getPrimaryWrist();
                return SENT_MESS;
              }
            case 80:
              // Sever weapon arm at hand 
              if (!v->hasPart(v->getPrimaryHand()))
                return 0;
              if (!v->isHumanoid())
                return 0;
              sprintf(buf, 
    "Your %s severs $N's hand at $S wrist!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s severs your arm below the wrist!", 
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s severs $N's hand at the wrist!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->makePartMissing(v->getPrimaryHand(), FALSE);
              v->rawBleed(v->getPrimaryWrist(), PERMANENT_DURATION, SILENT_NO, CHECK_IMMUNITY_YES);
              v->woundedHand(TRUE);
              *part_hit = v->getPrimaryHand();
              if (desc)
                desc->career.crit_sev_limbs++;
              if (v->desc)
                v->desc->career.crit_sev_limbs_suff++;
              return SENT_MESS;
            case 81:
              if (!v->hasPart(WEAR_BODY))
                return 0;
              if ((obj = v->equipment[WEAR_BODY])) {
                v->sendTo(COLOR_OBJECTS, "Your %s saves you from a punctured lung!\n\r",
                  fname(obj->name).c_str());
                for (i=1;i<9;i++)
                  if (v->equipment[WEAR_BODY])
                    v->damageItem(this,WEAR_BODY,wtype,weapon,*dam);
                *part_hit = WEAR_BODY;
                return SENT_MESS;
              }
            case 82:
              // Punctured lungs. Can't breathe. Dies if not healed quickly 
              if (v->hasDisease(DISEASE_LUNG))
                return 0;
              sprintf(buf, "Your %s plunges into $N's chest puncturing a lung!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
              "$n's %s plunges into your chest and punctures a lung!!!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s plunges into $N's chest.\n\rA hiss of air escapes $S punctured lung!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              af.type = AFFECT_DISEASE;
              af.level = 0;   // has to be 0 for doctor to treat
              af.duration = PERMANENT_DURATION;
              af.modifier = DISEASE_LUNG;
              af.location = APPLY_NONE;
              af.bitvector = AFF_SILENT;
              v->affectTo(&af);
              rc = dislodgeWeapon(v,weapon,WEAR_BODY);
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              v->sendTo("You won't be able to speak or breathe until you get that punctured lung fixed!!!\n\r");
              *part_hit = WEAR_BODY;
              if (desc)
                desc->career.crit_lung_punct++;
              if (v->desc)
                v->desc->career.crit_lung_punct_suff++;
              return SENT_MESS;
            case 83:
            case 84:
              if (!v->hasPart(WEAR_BODY))
                return 0;
              if ((obj = v->equipment[WEAR_BODY])) {
                v->sendTo(COLOR_OBJECTS, "Your %s saves you from a kidney wound!\n\r",
                  fname(obj->name).c_str());
                for (i=1;i<7;i++)
                  if (v->equipment[WEAR_BODY])
                    v->damageItem(this,WEAR_BODY,wtype,weapon,*dam);
                *part_hit = WEAR_BODY;
                return SENT_MESS;
              }
            case 85:
             // punctured kidney causes infection
              if (!v->hasPart(WEAR_BODY))
                return 0;
              sprintf(buf, 
    "You puncture $N's kidney with your %s and cause an infection!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s tears into your kidney; the pain is AGONIZING and an infection has started!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s punctures $N's kidney!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);

              if (desc)
                desc->career.crit_kidney++;
              if (v->desc)
                v->desc->career.crit_kidney_suff++;

              rc = dislodgeWeapon(v,weapon,WEAR_BODY);
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              v->rawInfect(WEAR_BODY, PERMANENT_DURATION, SILENT_NO, CHECK_IMMUNITY_YES);
              *part_hit = WEAR_BODY;
              return SENT_MESS;
            case 86:
            case 87:
             // stomach wound.  causes death 5 mins later if not healed.
              if (!v->hasPart(WEAR_BODY))
                return 0;
              if (v->hasDisease(DISEASE_STOMACH))
                return 0;
              sprintf(buf, 
    "You plunge your %s into $N's stomach, opening up $S gullet!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s tears into your stomach and exposes your intestines!!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s tears into $N's stomach exposing intestines!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->rawInfect(WEAR_BODY, PERMANENT_DURATION, SILENT_NO, CHECK_IMMUNITY_YES);
              if (v->hasPart(WEAR_WAISTE))
                v->rawInfect(WEAR_WAISTE, PERMANENT_DURATION, SILENT_NO, CHECK_IMMUNITY_YES);
              af.type = AFFECT_DISEASE;
              af.level = 0;   // for doctor to heal
              af.duration = PERMANENT_DURATION;
              af.modifier = DISEASE_STOMACH;
              v->affectTo(&af);
              *part_hit = WEAR_WAISTE;
              if (desc)
                desc->career.crit_eviscerate++;
              if (v->desc)
                v->desc->career.crit_eviscerate_suff++;

              return SENT_MESS;
            case 88:
            case 89:
              // abdominal wound
              // You plunge your %s into $N's abdoman and tear out causing a shower of blood

            case 90:
            case 91:
            case 92:
            case 93:
            case 94:
            case 95:
            case 96:
            case 97:
            case 98:
            case 99:
              return FALSE;
              break;
            case 100:
              sprintf(buf, 
    "You sink your %s between $N's eyes, causing an immediate death!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n sinks $s %s right between your eyes, causing an immediate death!",
                    limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n sinks $s %s smack between $N's eyes, causing an immediate death!",
                             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_ROOM);
              applyDamage(v, (20 * v->hitLimit()),wtype);
              *part_hit = WEAR_HEAD;
              if (desc)
                desc->career.crit_cranial_pierce++;
              if (v->desc)
                v->desc->career.crit_cranial_pierce_suff++;
              return DELETE_VICT;
            default:
              break;
          }
        }
    } else if (slashType(wtype)) {
        // Do slash crit
        string limbStr = (weapon ? fname(weapon->name) : getMyRace()->getBodyLimbSlash());
        if (num <= 33) {
             // double damage 
          *dam <<= 1;
          sprintf(buf, 
    "You strike $N's %s exceptionally well, sinking your %s deep into $S flesh!",
             v->describeBodySlot(*part_hit).c_str(),
             limbStr.c_str());
          act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
          sprintf(buf, 
    "$n strikes your %s exceptionally well, sinking $s %s deep into your flesh!",
             v->describeBodySlot(*part_hit).c_str(),
             limbStr.c_str());
          act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
          sprintf(buf, "$n strikes $N's %s exceptionally well, sinking $s %s deep into $N's flesh!",
             v->describeBodySlot(*part_hit).c_str(),
             limbStr.c_str());
          act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
          return (SENT_MESS);
        } else if (num <= 66) {
          // triple damage
          sprintf(buf, 
    "You critically strike $N's %s, sinking your %s deep into $S flesh!",
             v->describeBodySlot(*part_hit).c_str(),
             limbStr.c_str());
          act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
          sprintf(buf, 
    "$n critically strikes your %s, sinking $s %s deep into your flesh!",
             v->describeBodySlot(*part_hit).c_str(),
             limbStr.c_str());
          act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
          sprintf(buf, "$n critically strikes $N's %s, sinking $s %s deep into $N's flesh!",
             v->describeBodySlot(*part_hit).c_str(),
             limbStr.c_str());
          act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
          *dam *= 3;
          return (SENT_MESS);
        } else {
          // better stuff 
          if ((num == 83 || num == 84) && wtype == TYPE_CLEAVE)
            num = 85;  // axes don't impale
          switch (num) {
            case 67:
             // sever finger-r
              if (!v->hasPart(WEAR_FINGER_R))
                return 0;
              sprintf(buf, 
    "Your %s severs $N's %s and sends it flying!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_FINGER_R).c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s severs your %s and sends it flying!!  OH THE PAIN!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_FINGER_R).c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s severs $N's %s and sends it flying!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_FINGER_R).c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->makePartMissing(WEAR_FINGER_R, FALSE);
              v->rawBleed(WEAR_HAND_R, PERMANENT_DURATION, SILENT_NO, CHECK_IMMUNITY_YES);
              *part_hit = WEAR_FINGER_R;
              if (desc)
                desc->career.crit_sev_limbs++;
              if (v->desc)
                v->desc->career.crit_sev_limbs_suff++;
              return SENT_MESS;
            case 68:
             // sever finger-l
              if (!v->hasPart(WEAR_FINGER_L))
                return 0;
              sprintf(buf, 
    "Your %s severs $N's %s and sends it flying!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_FINGER_L).c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s severs your %s and sends it flying!!  OH THE PAIN!",
             limbStr.c_str(),
                   v->describeBodySlot(WEAR_FINGER_L).c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s severs $N's %s and sends it flying!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_FINGER_L).c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->makePartMissing(WEAR_FINGER_L, FALSE);
              v->rawBleed(WEAR_HAND_L, PERMANENT_DURATION, SILENT_NO, CHECK_IMMUNITY_YES);
              *part_hit = WEAR_FINGER_L;
              if (desc)
                desc->career.crit_sev_limbs++;
              if (v->desc)
                v->desc->career.crit_sev_limbs_suff++;
              return SENT_MESS;
            case 69:
            case 70:
              if (!v->hasPart(WEAR_HAND_R))
                return 0;
              if (!v->isHumanoid())
                return 0;
              if ((obj = v->equipment[WEAR_WRIST_R])) {
                v->sendTo(COLOR_OBJECTS, "Your %s saves you from losing your %s!\n\r",
                  fname(obj->name).c_str(), v->describeBodySlot(WEAR_HAND_R).c_str());
                for (i=1;i<5;i++)
                  if (v->equipment[WEAR_WRIST_R])
                    v->damageItem(this,WEAR_WRIST_R,wtype,weapon,*dam);
                *part_hit = WEAR_HAND_R;
                return SENT_MESS;
              }
            case 71:
             // sever hand-r at wrist
              if (!v->hasPart(WEAR_HAND_R))
                return 0;
              if (!v->isHumanoid())
                return 0;
              sprintf(buf, 
    "Your %s severs $N's %s and sends it flying!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_HAND_R).c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s severs your %s and sends it flying!!  OH THE PAIN!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_HAND_R).c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s severs $N's %s and sends it flying!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_HAND_R).c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->makePartMissing(WEAR_HAND_R, FALSE);
              v->rawBleed(WEAR_WRIST_R, PERMANENT_DURATION, SILENT_NO, CHECK_IMMUNITY_YES);
              v->woundedHand(v->isRightHanded());
              *part_hit = WEAR_WRIST_R;
              if (desc)
                desc->career.crit_sev_limbs++;
              if (v->desc)
                v->desc->career.crit_sev_limbs_suff++;
              return SENT_MESS;
            case 72:
            case 73:
              if (!v->hasPart(WEAR_WRIST_L))
                return 0;
              if (!v->isHumanoid())
                return 0;
              if ((obj = v->equipment[WEAR_WRIST_L])) {
                v->sendTo(COLOR_OBJECTS, "Your %s saves you from losing a hand!\n\r",
                  fname(obj->name).c_str());
                for (i=1;i<5;i++)
                  if (v->equipment[WEAR_WRIST_L])
                    v->damageItem(this,WEAR_WRIST_L,wtype,weapon,*dam);
                *part_hit = WEAR_WRIST_L;
                return SENT_MESS;
              }
            case 74:
             // sever hand-l at wrist
              if (!v->hasPart(WEAR_HAND_L))
                return 0;
              if (!v->isHumanoid())
                return 0;
              sprintf(buf, 
    "Your %s severs $N's %s and sends it flying!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_HAND_L).c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s severs your %s and sends it flying!!  OH THE PAIN!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_HAND_L).c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s severs $N's %s and sends it flying!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_HAND_L).c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->makePartMissing(WEAR_HAND_L, FALSE);
              v->rawBleed(WEAR_WRIST_L, PERMANENT_DURATION, SILENT_NO, CHECK_IMMUNITY_YES);
              v->woundedHand(!v->isRightHanded());
              *part_hit = WEAR_WRIST_L;
              if (desc)
                desc->career.crit_sev_limbs++;
              if (v->desc)
                v->desc->career.crit_sev_limbs_suff++;
              return SENT_MESS;
            case 75:
            case 76:
             // cleave arm at shoulder
              if (!v->hasPart(WEAR_ARM_R))
                return 0;
              if (!v->isHumanoid())
                return 0;
              sprintf(buf, 
    "Your %s severs $N's %s and sends it flying!",
             limbStr.c_str(),
                   v->describeBodySlot(WEAR_ARM_R).c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s severs your %s and sends it flying!!  OH THE PAIN!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_ARM_R).c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s severs $N's %s and sends it flying!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_ARM_R).c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->makePartMissing(WEAR_ARM_R, FALSE);
              v->rawBleed(WEAR_BODY, PERMANENT_DURATION, SILENT_NO, CHECK_IMMUNITY_YES);
              v->woundedHand(v->isRightHanded());
              *part_hit = WEAR_ARM_R;
              if (desc)
                desc->career.crit_sev_limbs++;
              if (v->desc)
                v->desc->career.crit_sev_limbs_suff++;
              return SENT_MESS;
            case 77:
            case 78:
             // cleave l-arm at shoulder
              if (!v->hasPart(WEAR_ARM_L))
                return 0;
              if (!v->isHumanoid())
                return 0;
              sprintf(buf, "Your %s severs $N's %s and sends it flying!",
             limbStr.c_str(),
                   v->describeBodySlot(WEAR_ARM_L).c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s severs your %s and sends it flying!!  OH THE PAIN!",
             limbStr.c_str(),
                   v->describeBodySlot(WEAR_ARM_L).c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s severs $N's %s and sends it flying!",
             limbStr.c_str(),
                    v->describeBodySlot(WEAR_ARM_L).c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->makePartMissing(WEAR_ARM_L, FALSE);
              v->rawBleed(WEAR_BODY, PERMANENT_DURATION, SILENT_NO, CHECK_IMMUNITY_YES);
              v->woundedHand(!v->isRightHanded());
              *part_hit = WEAR_ARM_L;
              if (desc)
                desc->career.crit_sev_limbs++;
              if (v->desc)
                v->desc->career.crit_sev_limbs_suff++;
              return SENT_MESS;
            case 79:
            case 80:
             // sever leg: foot missing, leg useless
              if (v->roomp && !v->roomp->isRoomFlag(ROOM_ARENA)) {
                if (!v->hasPart(WEAR_FOOT_R))
                  return 0;
                sprintf(buf, 
      "Your %s severs $N's %s and sends it flying!",
               limbStr.c_str(),
                      v->describeBodySlot(WEAR_FOOT_R).c_str());
                act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
                sprintf(buf, 
      "$n's %s severs your %s!!  OH THE PAIN!",
               limbStr.c_str(),
                      v->describeBodySlot(WEAR_FOOT_R).c_str());
                act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
                sprintf(buf, "$n's %s severs $N's %s and sends it flying!",
                 limbStr.c_str(),
                      v->describeBodySlot(WEAR_FOOT_R).c_str());
                act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
                v->makePartMissing(WEAR_FOOT_R, FALSE);
                v->addToLimbFlags(WEAR_LEGS_R, PART_USELESS);
                if ((obj = v->equipment[WEAR_LEGS_R])) {
                  obj->makeScraps();
                  delete obj;
                  obj = NULL;
                }
                v->rawBleed(WEAR_LEGS_R, PERMANENT_DURATION, SILENT_NO, CHECK_IMMUNITY_YES);
                *part_hit = WEAR_LEGS_R;
                if (desc)
                  desc->career.crit_sev_limbs++;
                if (v->desc)
                  v->desc->career.crit_sev_limbs_suff++;
                return SENT_MESS;
              }
            case 81:
            case 82:
             // sever other leg: foot missing, leg useless
              if (v->roomp && !v->roomp->isRoomFlag(ROOM_ARENA) &&
                  v->hasPart(WEAR_FOOT_L)) {
                sprintf(buf, 
      "Your %s severs $N's %s and sends it flying!",
               limbStr.c_str(),
                      v->describeBodySlot(WEAR_FOOT_L).c_str());
                act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
                sprintf(buf, 
      "$n's %s severs your %s!!  OH THE PAIN!",
               limbStr.c_str(),
                      v->describeBodySlot(WEAR_FOOT_L).c_str());
                act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
                sprintf(buf, "$n's %s severs $N's %s and sends it flying!",
               limbStr.c_str(),
                    v->describeBodySlot(WEAR_FOOT_L).c_str());
                act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
                v->makePartMissing(WEAR_FOOT_L, FALSE);
                v->addToLimbFlags(WEAR_LEGS_L, PART_USELESS);
                if ((obj = v->equipment[WEAR_LEGS_L])) {
                  obj->makeScraps();
                  delete obj;
                  obj = NULL;
                }
                v->rawBleed(WEAR_LEGS_L, PERMANENT_DURATION, SILENT_NO, CHECK_IMMUNITY_YES);
                *part_hit = WEAR_LEGS_L;
                if (desc)
                  desc->career.crit_sev_limbs++;
                if (v->desc)
                  v->desc->career.crit_sev_limbs_suff++;
                return SENT_MESS;
              }
            case 83:
            case 84:
             // impale with weapon
              if (!v->hasPart(WEAR_BODY))
                return 0;
              sprintf(buf, 
    "You stick your %s through $N's body, impaling $M!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s is thrust through your torso, impaling you!!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf,
    "$n thrusts $s %s deep into $N's torso, impaling $M!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              if (desc)
                desc->career.crit_impale++;
              if (v->desc)
                v->desc->career.crit_impale_suff++;
              rc = dislodgeWeapon(v, weapon, WEAR_BODY);
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              rc = applyDamage(v, v->hitLimit()/2,DAMAGE_IMPALE);
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              *part_hit = WEAR_BODY;
              return SENT_MESS;
            case 85:
            case 86:
              if (v->roomp && !v->roomp->isRoomFlag(ROOM_ARENA) &&
                  v->hasPart(WEAR_WAISTE) &&
                  (obj = v->equipment[WEAR_WAISTE])) {
                sprintf(buf, 
    "You attempt to cleave $N in two with your %s, but $p saves $M from a hideous fate.",
             limbStr.c_str());
                act(buf, FALSE, this, obj, v, TO_CHAR, ANSI_ORANGE);
                sprintf(buf, 
    "$n tries to cleave you in two with $s %s, but $p saves you thankfully!",
             limbStr.c_str());
                act(buf, FALSE, this, obj, v, TO_VICT, ANSI_RED);
                sprintf(buf, "$n attempts to cleave $N in two with $s %s! Thankfully $p saves $M!",
             limbStr.c_str());
                act(buf, FALSE, this, obj, v, TO_NOTVICT, ANSI_BLUE);
                obj->makeScraps();
                delete obj;
                obj = NULL;
                *part_hit = WEAR_WAISTE;
                rc = damageLimb(v,WEAR_WAISTE,weapon,dam);
                if (IS_SET_DELETE(rc, DELETE_VICT))
                  return DELETE_VICT;
                return SENT_MESS;
              }
              // if no girth, its going into next critSuccess...
            case 87:
            case 88:
              // cleave in two
              if (v->roomp && !v->roomp->isRoomFlag(ROOM_ARENA)) {
                if (!weapon && ((getHeight()*3) < v->getHeight()) &&
                    !isDiabolic() && !isLycanthrope()) {
                  sprintf(buf,
        "With a mighy warcry, you almost cleave $N in two with your %s.",
                  limbStr.c_str());
                  act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
                  sprintf(buf,
        "$n unleashes a mighty warcry and slashes you HARD down the center with $s %s!",
                  limbStr.c_str());
                  act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
                  sprintf(buf,
        "$n gives a mighty warcry and slashes $N down the center with $s %s!",
                  limbStr.c_str());
                  act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
                  if ((obj = v->equipment[WEAR_WAISTE])) {
                    obj->makeScraps();
                    delete obj;
                    obj = NULL;
                  }
                  *part_hit = WEAR_WAISTE;
                  if (desc)
                    desc->career.crit_cleave_two++;
                  if (v->desc)
                    v->desc->career.crit_cleave_two_suff++;
                  return applyDamage(v, GetMaxLevel()*3, DAMAGE_HACKED);
                } else {
                  sprintf(buf, 
        "With a mighty warcry, you cleave $N in two with your %s.",
                  limbStr.c_str());
                  act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
                  sprintf(buf, 
        "$n unleashes a mighty warcry before cleaving you in two with $s %s!",
                  limbStr.c_str());
                  act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
                  sprintf(buf, "$n gives a mighty warcry and cleaves $N in two with $s %s!",
                  limbStr.c_str());
                  act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
                  if ((obj = v->equipment[WEAR_WAISTE])) {
                    obj->makeScraps();
                    delete obj;
                    obj = NULL;
                  }
                  applyDamage(v, 20 * v->hitLimit(),DAMAGE_HACKED);
                  *part_hit = WEAR_WAISTE;
                  if (desc)
                    desc->career.crit_cleave_two++;
                  if (v->desc)
                    v->desc->career.crit_cleave_two_suff++;
                  return DELETE_VICT;
                }
              }
            case 89:
            case 90:
              // slice torso from gullet to groin
              if (v->roomp && !v->roomp->isRoomFlag(ROOM_ARENA)) {
                sprintf(buf, 
      "With your %s, you slice $N from gullet to groin disembowling $M!",
               limbStr.c_str());
                act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
                sprintf(buf, 
      "$n's %s slices you from gullet to groin, disembowling you!",
               limbStr.c_str());
                act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
                sprintf(buf, "$n's %s slices into $N from gullet to groin, disembowling $M!",
               limbStr.c_str());
                act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
                if ((obj = v->equipment[WEAR_BODY])) {
                  obj->makeScraps();
                  delete obj;
                  obj = NULL;
                }
                applyDamage(v, 20 * v->hitLimit(),DAMAGE_DISEMBOWLED);
                *part_hit = WEAR_BODY;
                if (desc)
                  desc->career.crit_disembowel++;
                if (v->desc)
                  v->desc->career.crit_disembowel_suff++;
                return DELETE_VICT;
              }
            case 91:
            case 92:
            case 93:
            case 94:
            case 95:
            case 96:
            case 97:
              return FALSE;
              break;
            case 98:
            case 99:
              // decapitate if no neck armor
              if (v->roomp && !v->roomp->isRoomFlag(ROOM_ARENA) &&
                  (obj = v->equipment[WEAR_NECK])) {
                sprintf(buf, 
    "You attempt to decapitate $N with your %s, but $p saves $M from a hideous fate.",
             limbStr.c_str());
                act(buf, FALSE, this, obj, v, TO_CHAR, ANSI_ORANGE);
                sprintf(buf, 
    "$n tries to decapitate you with $s %s, but $p saves you!",
             limbStr.c_str());
                act(buf, FALSE, this, obj, v, TO_VICT, ANSI_RED);
                sprintf(buf, "$n attempts to decapitate $N with $s %s!  Luckily, $p saves $M!",
             limbStr.c_str());
                act(buf, FALSE, this, obj, v, TO_NOTVICT, ANSI_BLUE);
                obj->makeScraps();
                delete obj;
                obj = NULL;
                *part_hit = WEAR_NECK;
                rc = damageLimb(v,*part_hit,weapon,dam);
                if (IS_SET_DELETE(rc, DELETE_VICT))
                  return DELETE_VICT;
                return SENT_MESS;
              }
              // if no collar, its going into next critSuccess...
            case 100:
             // POW! He was deCAPITATED! 
              act("$n strikes a fatal blow and cuts off $N's head!", FALSE, this, 0, v, TO_NOTVICT, ANSI_CYAN);
              act("You strike a fatal blow and completely behead $N!", FALSE, this, 0, v, TO_CHAR, ANSI_RED);
              act("$n strikes a fatal blow and completely beheads you!", FALSE, this, 0, v, TO_VICT, ANSI_RED);
              if (v->roomp && !v->roomp->isRoomFlag(ROOM_ARENA) &&
                  (obj = v->equipment[WEAR_NECK])) {
                obj->makeScraps();
                delete obj;
                obj = NULL;
              }
              v->makeBodyPart(WEAR_HEAD);
              applyDamage(v, (20 * v->hitLimit()),DAMAGE_BEHEADED);
              *part_hit = WEAR_NECK;
              if (desc)
                desc->career.crit_beheads++;

              if (v->desc)
                v->desc->career.crit_beheads_suff++;

              return DELETE_VICT;
            default:
              return FALSE;
          }
        }
    } else if (bluntType(wtype)) {
        // Do crush crit 
      string limbStr = (weapon ? fname(weapon->name) : getMyRace()->getBodyLimbBlunt());
    
        if (num <= 33) {
          // double damage 
          *dam <<= 1;
          sprintf(buf, 
    "You strike $N exceptionally well, %s $S %s with your %s!",
                  attack_hit_text[new_wtype].hitting,
                  v->describeBodySlot(*part_hit).c_str(),
              limbStr.c_str());
          act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
          sprintf(buf, "$n strikes you exceptionally well, %s your %s with $s %s.",
                  attack_hit_text[new_wtype].hitting,
                  v->describeBodySlot(*part_hit).c_str(),
              limbStr.c_str());
          act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
          sprintf(buf, "$n strikes $N exceptionally well, %s $S %s with $s %s.",
                  attack_hit_text[new_wtype].hitting,
                  v->describeBodySlot(*part_hit).c_str(), 
              limbStr.c_str());
          act(buf, TRUE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
          return (SENT_MESS);
        } else if (num <= 66) {
          // triple damage 
          *dam *= 3;
          sprintf(buf, "You critically strike $N, %s $S %s with your %s!",
                  attack_hit_text[new_wtype].hitting,
                  v->describeBodySlot(*part_hit).c_str(),
              limbStr.c_str());
          act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
          sprintf(buf, "$n critically strikes you, %s your %s with $s %s.",
                  attack_hit_text[new_wtype].hitting,
                  v->describeBodySlot(*part_hit).c_str(),
              limbStr.c_str());
          act(buf, TRUE, this, 0, v, TO_VICT, ANSI_RED);
          sprintf(buf, "$n critically strikes $N, %s $S %s with $s %s.",
                  attack_hit_text[new_wtype].hitting,
                  v->describeBodySlot(*part_hit).c_str(), 
              limbStr.c_str());
          act(buf, TRUE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
          return (SENT_MESS);
        } else {
          // better stuff 
          switch (num) {
            case 67:
            case 68:
             // crush finger
              if (!v->hasPart(WEAR_FINGER_R))
                return 0;
              if (v->race->hasNoBones())
                return 0;
              if (v->isImmune(IMMUNE_BONE_COND, *dam * 6))
                return 0;
              sprintf(buf, 
    "With your %s, you crush $N's %s!",
             limbStr.c_str(),
             v->describeBodySlot(WEAR_FINGER_R).c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s crushes your %s!",
             limbStr.c_str(),
             v->describeBodySlot(WEAR_FINGER_R).c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s crushes $N's %s!",
                limbStr.c_str(),
             v->describeBodySlot(WEAR_FINGER_R).c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->addToLimbFlags(WEAR_FINGER_R, PART_BROKEN);
              if (v->roomp && !v->roomp->isRoomFlag(ROOM_ARENA) &&
                  (obj = v->equipment[WEAR_FINGER_R])) {
                obj->makeScraps();
                delete obj;
                obj = NULL;
              }
              *part_hit = WEAR_FINGER_R;
              if (desc)
                desc->career.crit_broken_bones++;
              if (v->desc)
                v->desc->career.crit_broken_bones_suff++;
              rc = damageLimb(v,*part_hit,weapon,dam);
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              return SENT_MESS;
            case 69:
            case 70:
             //shatter bones in 1 hand
              if (!v->hasPart(WEAR_HAND_R))
                return 0;
              if (v->isImmune(IMMUNE_BONE_COND, *dam * 6))
                return 0;
              if (v->race->hasNoBones())
                return 0;
              sprintf(buf, 
    "With your %s, you shatter the bones in $N's %s!",
             limbStr.c_str(),
               v->describeBodySlot(WEAR_HAND_R).c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s shatters the bones in your %s!",
             limbStr.c_str(),
               v->describeBodySlot(WEAR_HAND_R).c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s shatters the bones in $N's %s!",
             limbStr.c_str(),
               v->describeBodySlot(WEAR_HAND_R).c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->addToLimbFlags(WEAR_HAND_R, PART_BROKEN);
              for (i=1;i<5;i++)
                if (v->equipment[WEAR_HAND_R])
                  v->damageItem(this,WEAR_HAND_R,wtype,weapon,*dam);
              v->woundedHand(v->isRightHanded());
              *part_hit = WEAR_HAND_R;
              if (desc)
                desc->career.crit_broken_bones++;
              if (v->desc)
                v->desc->career.crit_broken_bones_suff++;
              rc = damageLimb(v,*part_hit,weapon,dam);
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              return SENT_MESS;
            case 71:
            case 72:
             // shatter bones other hand
              if (!v->hasPart(WEAR_HAND_L))
                return 0;
              if (v->isImmune(IMMUNE_BONE_COND, *dam * 6))
                return 0;
              if (v->race->hasNoBones())
                return 0;
              *part_hit = WEAR_HAND_L;
              sprintf(buf, 
    "With your %s, you shatter the bones in $N's %s!",
             limbStr.c_str(),
               v->describeBodySlot(WEAR_HAND_L).c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s shatters the bones in your %s!",
             limbStr.c_str(),
               v->describeBodySlot(WEAR_HAND_L).c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s shatters the bones in $N's %s!",
                limbStr.c_str(),
               v->describeBodySlot(WEAR_HAND_L).c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->addToLimbFlags(WEAR_HAND_L, PART_BROKEN);
              v->woundedHand(!v->isRightHanded());
              for (i=1;i<5;i++)
                if (v->equipment[WEAR_HAND_L])
                  v->damageItem(this,WEAR_HAND_L,wtype,weapon,*dam);
              *part_hit = WEAR_HAND_L;
              if (desc)
                desc->career.crit_broken_bones++;
              if (v->desc)
                v->desc->career.crit_broken_bones_suff++;
              rc = damageLimb(v,*part_hit,weapon,dam);
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              return SENT_MESS;
            case 73:
            case 74:
             // break bones arm - broken
              if (!v->hasPart(v->getPrimaryArm()))
                return 0;
              if (v->isImmune(IMMUNE_BONE_COND, *dam * 6))
                return 0;
              if (v->race->hasNoBones())
                return 0;
              *part_hit = v->getPrimaryArm();
              sprintf(buf, 
    "You shatter the bones in $N's forearm with your %s!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s shatters the bones in your forearm!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s shatters the bones in $N's forearm!",
                limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->addToLimbFlags(v->getPrimaryArm(), PART_BROKEN);
              for (i=1;i<5;i++)
                if (v->equipment[v->getPrimaryArm()])
                  v->damageItem(this,v->getPrimaryArm(),wtype,weapon,*dam);
              *part_hit = v->getPrimaryArm();
              if (desc)
                desc->career.crit_broken_bones++;
              if (v->desc)
                v->desc->career.crit_broken_bones_suff++;
              rc = damageLimb(v,*part_hit,weapon,dam);
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              return SENT_MESS;
            case 75:
            case 76:
             // crush nerves arm - useless
              new_slot = v->getPrimaryArm();
              if (!v->slotChance(new_slot))
                return 0;
              sprintf(buf, 
    "With your %s, you crush the nerves in $N's shoulder!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s crushes the nerves in your shoulder!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s crushes the nerves in $N's shoulder!",
                limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->damageArm(TRUE,PART_USELESS);
              *part_hit = new_slot;
              if (desc)
                desc->career.crit_crushed_nerve++;
              if (v->desc)
                v->desc->career.crit_crushed_nerve_suff++;
              return SENT_MESS;
            case 77:
            case 78:
             // break bones leg
              if (!v->hasPart(WEAR_LEGS_L))
                return 0;
              if (v->isImmune(IMMUNE_BONE_COND, *dam * 6))
                return 0;
              if (v->race->hasNoBones())
                return 0;
              *part_hit = WEAR_LEGS_L;
              sprintf(buf, 
    "You shatter $N's femur with your %s!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s shatters your femur!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s shatters $N's femur!",
                limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              *part_hit = WEAR_LEGS_L;
              v->addToLimbFlags(WEAR_LEGS_L, PART_BROKEN);
              for (i=1;i<5;i++)
                if (v->equipment[WEAR_LEGS_L])
                  v->damageItem(this,WEAR_LEGS_L,wtype,weapon,*dam);
              rc = damageLimb(v,*part_hit,weapon,dam);
              if (desc)
                desc->career.crit_broken_bones++;
              if (v->desc)
                v->desc->career.crit_broken_bones_suff++;
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              return SENT_MESS;
            case 79:
            case 80:
             // crush muscles in leg
              if (!v->hasPart(WEAR_LEGS_L))
                return 0;
              *part_hit = WEAR_LEGS_L;
              sprintf(buf, 
    "With your %s, you crush the muscles in $N's leg!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s crushes the muscles in your leg!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s crushes the muscles in $N's leg!",
                limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              *part_hit = WEAR_LEGS_L;
              v->addToLimbFlags(WEAR_LEGS_L, PART_USELESS);
              v->addToLimbFlags(WEAR_FOOT_L, PART_USELESS);
              for (i=1;i<5;i++)
                if (v->equipment[WEAR_LEGS_L])
                  v->damageItem(this,WEAR_LEGS_L,wtype,weapon,*dam);
              rc = damageLimb(v,*part_hit,weapon,dam);

              if (desc)
                desc->career.crit_crushed_nerve++;
              if (v->desc)
                v->desc->career.crit_crushed_nerve_suff++;

              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              return SENT_MESS;
            case 81:
            case 82:
             // head blow - stuns 10 rounds
             // this is half, goes into next case for other half
              if (!v->hasPart(WEAR_HEAD))
                return 0;
              *part_hit = WEAR_HEAD;
              v->cantHit += v->loseRound(10);
            case 83:
            case 84:
             // head blow - stuns 5 rounds
              if (!v->hasPart(WEAR_HEAD))
                return 0;
              *part_hit = WEAR_HEAD;
              sprintf(buf, 
    "You slam $N's head massively with your %s!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s slams into your head and .. What?  who?  Where am I????",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s slams into $N's head, stunning $M completely!",
                limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->cantHit += v->loseRound(5);
              *part_hit = WEAR_HEAD;
              for (i = 1;i < 5; i++) {
                if (v->equipment[WEAR_HEAD]) {
                  v->damageItem(this,WEAR_HEAD,wtype,weapon,*dam);
                  if (v->cantHit > 0)
                    v->cantHit -= v->loseRound(2);
                }
              }
              rc = damageLimb(v,*part_hit,weapon,dam);
              if (IS_SET_DELETE(rc, DELETE_VICT))
                return DELETE_VICT;
              return SENT_MESS;
            case 85:
            case 86:
             //  shatter rib
              if (v->isImmune(IMMUNE_BONE_COND, *dam * 6))
                return 0;
              if (v->race->hasNoBones())
                return 0;
              *part_hit = WEAR_BODY;
              sprintf(buf, 
    "With your %s, you slam $N's chest, breaking a rib!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s crushes your chest and shatters a rib!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s shatters one of $N's ribs!",
                limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->addToLimbFlags(WEAR_BODY, PART_BROKEN);
              if (desc)
                desc->career.crit_broken_bones++;
              if (v->desc)
                v->desc->career.crit_broken_bones_suff++;
              for (i=1;i<7;i++)
                if (v->equipment[WEAR_BODY])
                  v->damageItem(this,WEAR_BODY,wtype,weapon,*dam);
              return SENT_MESS;
            case 87:
            case 88:
             //  shatter rib - internal damage, death if not healed
              if (v->isImmune(IMMUNE_BONE_COND, *dam * 6))
                return 0;
              if (v->race->hasNoBones())
                return 0;
              if (v->hasDisease(DISEASE_HEMORRAGE))
                return 0;
              *part_hit = WEAR_BODY;
              sprintf(buf, 
    "With your %s, you slam $N's chest, breaking a rib and causing internal damage!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s crushes your chest, shatters a rib and causes internal bleeding!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s shatters one of $N's ribs!",
                limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              v->addToLimbFlags(WEAR_BODY, PART_BROKEN);
              for (i=1;i<9;i++)
                if (v->equipment[WEAR_BODY])
                  v->damageItem(this,WEAR_BODY,wtype,weapon,*dam);
              af.type = AFFECT_DISEASE;
              af.level = 0;   // has to be 0 for doctor to treat
              af.duration = PERMANENT_DURATION;
              af.modifier = DISEASE_HEMORRAGE;
              af.location = APPLY_NONE;
              af.bitvector = 0;
              v->affectTo(&af);
              if (desc)
                desc->career.crit_broken_bones++;
              if (v->desc)
                v->desc->career.crit_broken_bones_suff++;
              return SENT_MESS;
            case 89:
            case 90:
            case 91:
            case 92:
            case 93:
            case 94:
            case 95:
            case 96:
            case 97:
              return FALSE;
            case 98:
            case 99:
              // crush skull unless helmet
              if (!v->hasPart(WEAR_HEAD))
                return 0;
              if ((obj = v->equipment[WEAR_HEAD])) {
                sprintf(buf, 
    "With a mighty blow, you crush $N's head with your %s. Unfortunately, $S $o saves $M.",
             limbStr.c_str());
                act(buf, FALSE, this, obj, v, TO_CHAR, ANSI_ORANGE);
                sprintf(buf, 
    "$n's %s strikes a mighty blow to your head crushing your $o!",
             limbStr.c_str());
                act(buf, FALSE, this, obj, v, TO_VICT, ANSI_RED);
                sprintf(buf, "$n's %s strikes a mighty blow to $N's head, crushing $S $o!",
                  limbStr.c_str());
                act(buf, FALSE, this, obj, v, TO_NOTVICT, ANSI_BLUE);
                if (v->roomp && !v->roomp->isRoomFlag(ROOM_ARENA)) {
                  obj->makeScraps();
                  delete obj;
                  obj = NULL;
                }
                *part_hit = WEAR_HEAD;
                rc = damageLimb(v,*part_hit,weapon,dam);
                if (IS_SET_DELETE(rc, DELETE_VICT))
                  return DELETE_VICT;
                return SENT_MESS;
              }
              // no helm goes into next case
            case 100:
              // crush skull
              if (!v->hasPart(WEAR_HEAD))
                return 0;
              sprintf(buf, 
    "With your %s, you crush $N's skull, and $S brains ooze out!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_CHAR, ANSI_ORANGE);
              sprintf(buf, 
    "$n's %s crushes your skull and The World goes dark!",
             limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_VICT, ANSI_RED);
              sprintf(buf, "$n's %s crushes $N's skull.  Brains ooze out as $E crumples!",
                limbStr.c_str());
              act(buf, FALSE, this, 0, v, TO_NOTVICT, ANSI_BLUE);
              if (v->roomp && !v->roomp->isRoomFlag(ROOM_ARENA) &&
                  (obj = v->equipment[WEAR_HEAD])) {
                obj->makeScraps();
                delete obj;
                obj = NULL;
              }
              if (desc)
                desc->career.crit_crushed_skull++;
              if (v->desc)
                v->desc->career.crit_crushed_skull_suff++;
              applyDamage(v, (20 * v->hitLimit()),DAMAGE_CAVED_SKULL);
              return DELETE_VICT;
            default:
              return FALSE;
              break;
          }
        }
    }
  }
  return FALSE;
}
