//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: discipline.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


// discipline.cc

#include <cmath>

#include "stdsneezy.h"
#include "disease.h"
#include "components.h"
#include "combat.h"
#include "disc_mage.h"
#include "disc_cleric.h"
#include "disc_physical.h"
#include "disc_smythe.h"
#include "disc_deikhan_fight.h"
#include "disc_deikhan_aegis.h"
#include "disc_deikhan_cures.h"
#include "disc_deikhan_wrath.h"
#include "disc_mounted.h"
#include "disc_monk.h"
#include "disc_meditation.h"
#include "disc_leverage.h"
#include "disc_mindbody.h"
#include "disc_fattacks.h"
#include "disc_plants.h"
#include "disc_ranger_fight.h"
#include "disc_shaman_alchemy.h"
#include "disc_shaman_fight.h"
#include "disc_draining.h"
#include "disc_undead.h"
#include "disc_shaman_healing.h"
#include "disc_totem.h"
#include "disc_thief.h"
#include "disc_thief_fight.h"
#include "disc_poisons.h"
#include "disc_traps.h"
#include "disc_stealth.h"
#include "disc_air.h"
#include "disc_alchemy.h"
#include "disc_earth.h"
#include "disc_fire.h"
#include "disc_sorcery.h"
#include "disc_spirit.h"
#include "disc_water.h"
#include "disc_wrath.h"
#include "disc_shaman.h"
#include "disc_aegis.h"
#include "disc_afflictions.h"
#include "disc_cures.h"
#include "disc_hand_of_god.h"
#include "disc_ranger.h"
#include "disc_deikhan.h"
#include "disc_looting.h"
#include "disc_murder.h"
#include "disc_hth.h"
#include "disc_warrior.h"
#include "disc_brawling.h"
#include "disc_adventuring.h"
#include "disc_combat.h"
#include "disc_wizardry.h"
#include "disc_lore.h"
#include "disc_theology.h"
#include "disc_faith.h"
#include "disc_slash.h"
#include "disc_blunt.h"
#include "disc_pierce.h"
#include "disc_ranged.h"
#include "disc_barehand.h"
#include "disc_survival.h"
#include "disc_animal.h"
#include "disc_nature.h"
#include "spelltask.h"

#define DISC_DEBUG  0

static bool enforceVerbal(TBeing *ch, spellNumT spell)
{
  if (!IS_SET(discArray[spell]->comp_types, COMP_VERBAL))
    return TRUE;

  if (ch->isPc()) {

    if (!canDoVerbal(ch) ) {
      if (ch->getWizardryLevel() >= WIZ_LEV_NO_MANTRA) {
        act("Your skill at wizardry allows you to merely think the incantation.",TRUE,ch,0,0,TO_CHAR);
        return TRUE;
      }

      act("$n opens $s mouth as if to say something.", TRUE, ch, 0, 0, TO_ROOM);
      ch->sendTo("You are unable to chant the incantation!\n\r");
      return FALSE;
    }
    if (ch->getWizardryLevel() >= WIZ_LEV_NO_MANTRA) {
      act("$n begins to chant a mysterious and melodic incantation.", TRUE, ch, 0, 0, TO_ROOM, ANSI_CYAN);
      act("Although you no longer need to, you begin an incantation to facilitate your spell.", TRUE, ch, 0, 0, TO_CHAR, ANSI_CYAN);
    } else {
      act("$n begins to chant a mysterious and melodic incantation.", TRUE, ch, 0, 0, TO_ROOM, ANSI_CYAN);
      act("You begin to chant a mysterious and melodic incantation.", TRUE, ch, 0, 0, TO_CHAR, ANSI_CYAN);
    }
    return TRUE;
  } else 
    return TRUE;
}

// returns TRUE if they can use their hands or don't need to
static bool enforceGestural(TBeing *ch, spellNumT spell)
{
  TThing *sec_obj, *prim_obj;
  int sec_okay, prim_okay, sec_usable, prim_usable;
  char buf[256], buf2[40], msg[256];
  int num = ::number(1,100);

  if (ch->isImmortal())
    return TRUE;

  if (!IS_SET(discArray[spell]->comp_types, COMP_GESTURAL))
    return TRUE;

  if (ch->getWizardryLevel() < WIZ_LEV_NO_GESTURES) {
    // wizardry level still requries gestures, make generic checks
    if (ch->isPc() && (!ch->hasHands() || ch->eitherArmHurt())) {
      act("You cannot perform the ritual's gestures without arms and hands!", 
                FALSE, ch, NULL, NULL, TO_CHAR);
      act("Nothing seems to happen.", FALSE, ch, NULL, NULL, TO_ROOM);
      return FALSE;
    }
    // min position in discArray defines absolute minimum level
    // however, check some positions to see if restricted movement 
    // affects ability to make gestures
    // higher nums are more likely to cause restriction
    num = ::number(1,100);

    // sit, crawl use normal num.  raise it further for resting
    if (ch->getPosition() == POSITION_RESTING)
      num += 4 * num / 10;

    if ((ch->getPosition() == POSITION_RESTING ||
         ch->getPosition() == POSITION_SITTING ||
         ch->getPosition() == POSITION_CRAWLING) &&
           (num > ch->getSkillValue(SKILL_WIZARDRY))) {
      // we know that wizradry is < 60 from getWizardryLevel check
      ch->sendTo("Restricted movement while %s causes you to mess up the ritual's gestures.\n\r", good_uncap(position_types[ch->getPosition()]).c_str());
      act("Nothing seems to happen.", FALSE, ch, NULL, NULL, TO_ROOM);
      return FALSE;
    }
  }

  if (!ch->isPc()) {
    if (ch->getWizardryLevel() >= WIZ_LEV_NO_GESTURES) {
      act("You concentrate intently upon the magical task at hand...", 
            FALSE, ch, NULL, NULL, TO_CHAR, ANSI_CYAN);
      act("$n stares off into space, concentrating on something...", 
            FALSE, ch, NULL, NULL, TO_ROOM, ANSI_CYAN);
      return TRUE;
    }
    if (ch->hasHands()) {
      sprintf(msg, "$n traces a magical rune in the air with $s hands.");
      act(msg, FALSE, ch, NULL, NULL, TO_ROOM, ANSI_PURPLE);
      sprintf(msg, "You trace a magical rune in the air with your hands.");
      act(msg, FALSE, ch, NULL, NULL, TO_CHAR, ANSI_PURPLE);
    } else {
      act("You hop and wiggle about while creating the magical runes in the air...",
            FALSE, ch, NULL, NULL, TO_CHAR, ANSI_CYAN);
      act("$n hops and wiggles about while creating the magical runes in the air...",
            FALSE, ch, NULL, NULL, TO_ROOM, ANSI_CYAN);
    }
    return TRUE;
  } else {
    // PCs

    sec_obj = ch->heldInSecHand();
    sec_usable = ch->canUseArm(FALSE);
    sec_okay = ((!sec_obj || sec_obj->allowsCast()) && sec_usable);

    prim_obj = ch->heldInPrimHand();
    prim_usable = ch->canUseArm(TRUE);
    prim_okay = ((!prim_obj || prim_obj->allowsCast()) && prim_usable);

    if (ch->getWizardryLevel() >= WIZ_LEV_COMP_EITHER) {
      if (sec_okay || prim_okay) {
        if (sec_okay)
          sprintf(buf, "%s", (ch->isRightHanded() ? "left" : "right"));
        else
        sprintf(buf, "%s", (ch->isRightHanded() ? "right" : "left"));

        sprintf(msg, "$n traces a magical rune in the air with $s %s hand.", buf);
        act(msg, FALSE, ch, NULL, NULL, TO_ROOM, ANSI_PURPLE);


        if (ch->getWizardryLevel() >= WIZ_LEV_NO_GESTURES) {
          sprintf(msg, "While not absolutely neccessary, you trace a rune with your %s hand to facilitate your spell in forming.", buf);
          act(msg, FALSE, ch, NULL, NULL, TO_CHAR, ANSI_PURPLE);
        } else {
          sprintf(msg, "You trace a magical rune in the air with your %s hand.", buf);
          act(msg, FALSE, ch, NULL, NULL, TO_CHAR, ANSI_PURPLE);
        }
        return TRUE;
      } else {
        if (ch->getWizardryLevel() >= WIZ_LEV_NO_GESTURES) {
          act("You concentrate intently upon the magical task at hand...",
              FALSE, ch, NULL, NULL, TO_CHAR, ANSI_CYAN);
          act("$n stares off into space, concentrating on something...",
              FALSE, ch, NULL, NULL, TO_ROOM, ANSI_CYAN);
          return TRUE;
        }
        act("Nothing seems to happen.", FALSE, ch, NULL, NULL, TO_ROOM);
        act("You must have one hand free and usable to perform the ritual's gestures!", FALSE, ch, NULL, NULL, TO_CHAR);
        return FALSE;
      }
    } else {
      if (prim_okay && sec_okay) {
        sprintf(msg, "$n traces a magical rune in the air with $s hands.");
        act(msg, FALSE, ch, NULL, NULL, TO_ROOM, ANSI_PURPLE);
        sprintf(msg, "You trace a magical rune in the air with your hands.");
        act(msg, FALSE, ch, NULL, NULL, TO_CHAR, ANSI_PURPLE);
        return TRUE;
      } else {
        sprintf(buf, "%s",  (ch->isRightHanded() ? "right" : "left"));
        sprintf(buf2, "%s", (ch->isRightHanded() ? "left" : "right"));
        
        act("Nothing seems to happen.", FALSE, ch, NULL, NULL, TO_ROOM);
        if (ch->getWizardryLevel() < WIZ_LEV_COMP_EITHER_OTHER_FREE) {
          if (!(IS_SET(discArray[spell]->comp_types, COMP_MATERIAL))) {
            sprintf(msg, "Both of your hands must be free and usable to perform the ritual's gestures!");
          } else {
            if (!prim_okay && !sec_okay) {
              sprintf(msg, "Your component must be in your %s hand and your %s must be free and usable to perform the ritual's gestures!", buf, buf2);
            } else if (!prim_okay) {
              sprintf(msg, "Your component must be in your %s hand for you to be able to perform the ritual's gestures!", buf);
            } else {
              sprintf(msg, "Your %s hand must be free and usable to perform the ritual's gestures!", buf2);
            }
          }
        } else {
          if (!prim_okay && !sec_okay) {
              sprintf(msg, "Your %s hand must hold the component and your %s must be free to perform the ritual!", buf2, buf);
          } else if (!prim_okay) {
            sprintf(msg, "Your %s hand must be holding the component for you to properly perform this spell!", buf);
          } else {
            sprintf(msg, "Your %s hand must be free and usable to perform the ritual's gestures!", buf2);
          }
        }
        act(msg, FALSE, ch, NULL, NULL, TO_CHAR);
        return FALSE;
      }
    }
    return TRUE;
  }
}

bool bPassMageChecks(TBeing * caster, spellNumT spell, TThing *target)
{
  if (!caster->getSkillLevel(spell)) {
    // probably an immort with improper class
    caster->sendTo("You need to have the appropriate level AND class for this to work.\n\r");
    return FALSE;
  }

  if (caster->isImmortal() && caster->isPlayerAction(PLR_NOHASSLE))
    return TRUE;

  if (!enforceVerbal(caster, spell))
    return FALSE;

  if (!enforceGestural(caster, spell))
    return FALSE;

  // if spell uses component, check for it
  if (IS_SET(discArray[spell]->comp_types, COMP_MATERIAL)) {
    TBeing * vict = dynamic_cast<TBeing *>(target);
    TObj * obj = dynamic_cast<TObj *>(target);
    if (vict && !caster->useComponent(caster->findComponent(spell), vict, CHECK_ONLY_YES))
      return FALSE;
    if (obj && !caster->useComponentObj(caster->findComponent(spell), obj, CHECK_ONLY_YES))
      return FALSE;
  }

  return TRUE;
}

static bool enforcePrayer(TBeing *ch, spellNumT spell)
{
  if (!ch)
    return FALSE;

  if (ch->isImmortal())
    return TRUE;

  if (!IS_SET(discArray[spell]->comp_types, COMP_VERBAL))
    return TRUE;

  if (ch->isPc()) {

    if (!canDoVerbal(ch) ) {
      if (ch->getDevotionLevel() >= DEV_LEV_NO_MANTRA) {
        ch->sendTo("Your devotion allows you to merely think the prayer.\n\r");
        act("$n's face turns solemn as $e concentrates on something.",TRUE,ch,0,0,TO_ROOM);
        return TRUE;
      }
      act("$n opens $s mouth as if to say something.", TRUE, ch, 0, 0, TO_ROOM);
      ch->sendTo("You are unable to recite the sacred words!\n\r");
      return FALSE;
    }
    act("$n chants mysterious and sacred words.", TRUE, ch, 0, NULL, TO_ROOM, ANSI_CYAN);
    act("You beseech $d for aid.", TRUE, ch, NULL, NULL, TO_CHAR, ANSI_CYAN);
    return TRUE;
  } else 
    return TRUE;
}

static bool requireHolySym(const TBeing *ch)
{
  if (ch->isImmortal() || dynamic_cast<const TMonster *>(ch))
    return FALSE;

  if (ch->hasClass(CLASS_CLERIC) || ch->hasClass(CLASS_DEIKHAN))
    return TRUE;

  return FALSE;
}

//static
bool enforceHolySym(const TBeing *ch, spellNumT spell, bool checkDamage)
{
  // get the rough level of the spell being cast
  int level = ch->GetMaxLevel();
  int minl, maxl;
  getSkillLevelRange(spell, minl, maxl, 0);
  level = min(max(level, minl), maxl);

  int sym_stress = level * level;

  char buf[128];
  bool tasking = FALSE;
  int rounds = 0;

  if (IS_SET(discArray[spell]->comp_types, SPELL_TASKED_EVERY)) {
    tasking = TRUE;
    rounds = discArray[spell]->lag;
  }

  if (requireHolySym(ch)) {
    TSymbol * holy = ch->findHolySym(SILENT_NO);
    if (holy) {
      int curr_strength = holy->getSymbolCurStrength();
      int orig_strength = holy->getSymbolMaxStrength();

      // adjust the damage if overpowering the symbol
      float sym_level = holy->getSymbolLevel();
      string sb;  // damage string
      if (level >= (sym_level+15)) {
        sym_stress *= 8;
        sb = "$p suffers TREMENDOUS structural damage from the stress!";
      } else if (level >= (sym_level+10)) {
        sym_stress *= 4;
        sb = "$p suffers massive structural damage from the stress!";
      } else if (level >= (sym_level+5)) {
        sym_stress *= 2;
        sb = "$p suffers intense structural damage from the stress!";
      } else if (level < (sym_level-10)) {
        sb = "$p suffers some light damage from the stress!";
      } else {
        sb = "$p suffers structural damage from the stress!";
      }
    
      if (sym_stress > orig_strength) {
        act("$n's $o starts to glow with power but the glow fades away!", 
            FALSE, ch, holy, NULL, TO_ROOM);
        act("Your $o starts to glow with power but the glow fades away!", 
            FALSE, ch, holy, NULL, TO_CHAR);
        act("You sense that $p is not strong enough for that prayer.",
            FALSE, ch, holy, NULL, TO_CHAR);
        return FALSE;
      }

      if (curr_strength == orig_strength) {
          holy->setSymbolCurStrength((orig_strength - 1));
          act("The deities favor $p and accept it as a symbol of prayer.",
              FALSE, ch, holy, NULL, TO_CHAR);
      }
      if (!checkDamage) {
        act("$n's $o glows with power!", FALSE, ch, holy, NULL, TO_ROOM);
        act("Your $o glows with power!", FALSE, ch, holy, NULL, TO_CHAR);
        return TRUE;   // has holy and was a strong enough symbol
      } else {
        if (sym_stress >= curr_strength) {
          sprintf(buf, "$p %sshatters%s from the stress of the prayer.", ch->red(), ch->norm());
          act(buf, FALSE, ch, holy, NULL, TO_ROOM);
          act("$p shatters from the stress of the prayer!", 0, ch, holy, 0, TO_CHAR, ANSI_RED);
          delete holy;
          holy = NULL;
          return FALSE;
        }
      
        holy->addToSymbolCurStrength(-sym_stress);
        act(sb.c_str(), false, ch, holy, NULL, TO_CHAR);
        return TRUE;
      }
    } else {
      return FALSE;    // requires symbol, user didn't have one
    }
  } 
  return TRUE;         // doesn't require holy symbol
}

bool bPassClericChecks(TBeing * caster, spellNumT spell)
{
  if (!caster->getSkillLevel(spell)) {
    // probably an immort with improper class
    caster->sendTo("You need to have the appropriate level AND class for this to work.\n\r");
    return FALSE;
  }

  if (caster->isImmortal() && caster->isPlayerAction(PLR_NOHASSLE))
    return TRUE;

  if (!enforcePrayer(caster, spell))
    return FALSE;

  if (!enforceHolySym(caster,spell, false))
    return FALSE;

  return TRUE;
}

int CDiscipline::getDoLearnedness() const
{
    return(uDoLearnedness);
}

int CDiscipline::getNatLearnedness() const
{
    return(uNatLearnedness);
}

int CDiscipline::getLearnedness() const
{
    return(uLearnedness);
}

spellNumT TBeing::getSkillNum(spellNumT spell_num) const
{
  bool has_class = FALSE; 
  int num = 0;
  int num2 = 0;

  if ((spell_num < MIN_SPELL) || (spell_num >= MAX_SKILL)) {
    vlogf(5, "Something is passing a bad skill number (%d) to getSkillNum for %s", spell_num, getName());
    return TYPE_UNDEFINED;
  }

  switch (spell_num) {
    case SKILL_KICK: 
      if (hasClass(CLASS_WARRIOR)) {
        if (!has_class) {
          spell_num = SKILL_KICK;
        }
        if ((num2 = getSkillValue(SKILL_KICK)) > num) {
          spell_num = SKILL_KICK;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SKILL_KICK_DEIKHAN;
        }
        if ((num2 = getSkillValue(SKILL_KICK_DEIKHAN)) > num) {
          spell_num = SKILL_KICK_DEIKHAN;    
          num = num2;  
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_RANGER)) {
        if (!has_class) {
          spell_num = SKILL_KICK_RANGER;
        }
        if ((num2 = getSkillValue(SKILL_KICK_RANGER)) > num) {
          spell_num = SKILL_KICK_RANGER;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_THIEF)) {
        if (!has_class) {
          spell_num = SKILL_KICK_THIEF;
        }
        if ((num2 = getSkillValue(SKILL_KICK_THIEF)) > num) {
          spell_num = SKILL_KICK_THIEF;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_MONK)) {
        if (!has_class) {
          spell_num = SKILL_KICK_MONK;
        }
        if ((num2 = getSkillValue(SKILL_KICK_MONK)) > num) {
          spell_num = SKILL_KICK_MONK;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num; 
    case SKILL_BASH:
      if (hasClass(CLASS_WARRIOR)) {
        if (!has_class) {
          spell_num = SKILL_BASH;
        }
        if ((num2 = getSkillValue(SKILL_BASH)) > num) {
          spell_num = SKILL_BASH;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SKILL_BASH_DEIKHAN;
        }
        if ((num2 = getSkillValue(SKILL_BASH_DEIKHAN)) > num) {
          spell_num = SKILL_BASH_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_RANGER)) {
        if (!has_class) {
          spell_num = SKILL_BASH_RANGER;
        }
        if ((num2 = getSkillValue(SKILL_BASH_RANGER)) > num) {
          spell_num = SKILL_BASH_RANGER;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SKILL_RETREAT:
      if (hasClass(CLASS_THIEF)) {
        if (!has_class) {
          spell_num = SKILL_RETREAT_THIEF;
        }
        if ((num2 = getSkillValue(SKILL_RETREAT_THIEF)) > num) {
          spell_num = SKILL_RETREAT_THIEF;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_RANGER)) {
        if (!has_class) {
          spell_num = SKILL_RETREAT_RANGER;
        }
        if ((num2 = getSkillValue(SKILL_RETREAT_RANGER)) > num) {
          spell_num = SKILL_RETREAT_RANGER;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_MONK)) {
        if (!has_class) {
          spell_num = SKILL_RETREAT_MONK;
        }
        if ((num2 = getSkillValue(SKILL_RETREAT_MONK)) > num) {
          spell_num = SKILL_RETREAT_MONK;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_WARRIOR)) {
        if (!has_class) {
          spell_num = SKILL_RETREAT;
        }
        if ((num2 = getSkillValue(SKILL_RETREAT)) > num) {
          spell_num = SKILL_RETREAT;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SKILL_RETREAT_DEIKHAN;
        }
        if ((num2 = getSkillValue(SKILL_RETREAT_DEIKHAN)) > num) {
          spell_num = SKILL_RETREAT_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SKILL_SWITCH_OPP:
      if (hasClass(CLASS_MONK)) {
        if (!has_class) {
          spell_num = SKILL_SWITCH_MONK;
        }
        if ((num2 = getSkillValue(SKILL_SWITCH_MONK)) > num) {
          spell_num = SKILL_SWITCH_MONK;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_WARRIOR)) {
        if (!has_class) {
          spell_num = SKILL_SWITCH_OPP;
        }
        if ((num2 = getSkillValue(SKILL_SWITCH_OPP)) > num) {
          spell_num = SKILL_SWITCH_OPP;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_THIEF)) {
        if (!has_class) {
          spell_num = SKILL_SWITCH_THIEF;
        }
        if ((num2 = getSkillValue(SKILL_SWITCH_THIEF)) > num) {
          spell_num = SKILL_SWITCH_THIEF;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_RANGER)) {
        if (!has_class) {
          spell_num = SKILL_SWITCH_RANGER;
        }
        if ((num2 = getSkillValue(SKILL_SWITCH_RANGER)) > num) {
          spell_num = SKILL_SWITCH_RANGER;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SKILL_SWITCH_DEIKHAN;
        }
        if ((num2 = getSkillValue(SKILL_SWITCH_DEIKHAN)) > num) {
          spell_num = SKILL_SWITCH_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SKILL_RESCUE:
      if (hasClass(CLASS_WARRIOR)) {
        if (!has_class) {
          spell_num = SKILL_RESCUE;
        }
        if ((num2 = getSkillValue(SKILL_RESCUE)) > num) {
          spell_num = SKILL_RESCUE;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SKILL_RESCUE_DEIKHAN;
        }
        if ((num2 = getSkillValue(SKILL_RESCUE_DEIKHAN)) > num) {
          spell_num = SKILL_RESCUE_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_RANGER)) {
        if (!has_class) {
          spell_num = SKILL_RESCUE_RANGER;
        }
        if ((num2 = getSkillValue(SKILL_RESCUE_RANGER)) > num) {
          spell_num = SKILL_RESCUE_RANGER;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SKILL_DISARM:
      if (hasClass(CLASS_MONK)) {
        if (!has_class) {
          spell_num = SKILL_DISARM_MONK;
        }
        if ((num2 = getSkillValue(SKILL_DISARM_MONK)) > num) {
          spell_num = SKILL_DISARM_MONK;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_THIEF)) {
        if (!has_class) {
          spell_num = SKILL_DISARM_THIEF;
        }
        if ((num2 = getSkillValue(SKILL_DISARM_THIEF)) > num) {
          spell_num = SKILL_DISARM_THIEF;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_WARRIOR)) {
        if (!has_class) {
          spell_num = SKILL_DISARM;
        }
        if ((num2 = getSkillValue(SKILL_DISARM)) > num) {
          spell_num = SKILL_DISARM;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SKILL_DISARM_DEIKHAN;
        }
        if ((num2 = getSkillValue(SKILL_DISARM_DEIKHAN)) > num) {
          spell_num = SKILL_DISARM_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SKILL_SHOVE:
      if (hasClass(CLASS_WARRIOR)) {
        if (!has_class) {
          spell_num = SKILL_SHOVE;
        }
        if ((num2 = getSkillValue(SKILL_SHOVE)) > num) {
          spell_num = SKILL_SHOVE;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SKILL_SHOVE_DEIKHAN;
        }
        if ((num2 = getSkillValue(SKILL_SHOVE_DEIKHAN)) > num) {
          spell_num = SKILL_SHOVE_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SKILL_GRAPPLE:
      if (hasClass(CLASS_WARRIOR)) {
        if (!has_class) {
          spell_num = SKILL_GRAPPLE;
        }
        if ((num2 = getSkillValue(SKILL_GRAPPLE)) > num) {
          spell_num = SKILL_GRAPPLE;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SKILL_SNEAK:
      if (hasClass(CLASS_THIEF)) {
        if (!has_class) {
          spell_num = SKILL_SNEAK;
        }
        if ((num2 = getSkillValue(SKILL_SNEAK)) > num) {
          spell_num = SKILL_SNEAK;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SKILL_HIDE:
      if (hasClass(CLASS_THIEF)) {
        if (!has_class) {
          spell_num = SKILL_HIDE;
        }
        if ((num2 = getSkillValue(SKILL_HIDE)) > num) {
          spell_num = SKILL_HIDE;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_SALVE:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_SALVE;
        }
        if ((num2 = getSkillValue(SPELL_SALVE)) > num) {
          spell_num = SPELL_SALVE;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_SALVE_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_SALVE_DEIKHAN)) > num) {
          spell_num = SPELL_SALVE_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_HEAL_LIGHT:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_HEAL_LIGHT;
        }
        if ((num2 = getSkillValue(SPELL_HEAL_LIGHT)) > num) {
          spell_num = SPELL_HEAL_LIGHT;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_HEAL_LIGHT_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_HEAL_LIGHT_DEIKHAN)) > num) {
          spell_num = SPELL_HEAL_LIGHT_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_HEAL_SERIOUS:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_HEAL_SERIOUS;
        }
        if ((num2 = getSkillValue(SPELL_HEAL_SERIOUS)) > num) {
          spell_num = SPELL_HEAL_SERIOUS;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_HEAL_SERIOUS_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_HEAL_SERIOUS_DEIKHAN)) > num) {
          spell_num = SPELL_HEAL_SERIOUS_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_HEAL_CRITICAL:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_HEAL_CRITICAL;
        }
        if ((num2 = getSkillValue(SPELL_HEAL_CRITICAL)) > num) {
          spell_num = SPELL_HEAL_CRITICAL;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_HEAL_CRITICAL_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_HEAL_CRITICAL_DEIKHAN)) > num) {
          spell_num = SPELL_HEAL_CRITICAL_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_NUMB:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_NUMB;
        }
        if ((num2 = getSkillValue(SPELL_NUMB)) > num) {
          spell_num = SPELL_NUMB;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_NUMB_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_NUMB_DEIKHAN)) > num) {
          spell_num = SPELL_NUMB_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;

    case SPELL_HARM_LIGHT:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_HARM_LIGHT;
        }
        if ((num2 = getSkillValue(SPELL_HARM_LIGHT)) > num) {
          spell_num = SPELL_HARM_LIGHT;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_HARM_LIGHT_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_HARM_LIGHT_DEIKHAN)) > num) {
          spell_num = SPELL_HARM_LIGHT_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_HARM_SERIOUS:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_HARM_SERIOUS;
        }
        if ((num2 = getSkillValue(SPELL_HARM_SERIOUS)) > num) {
          spell_num = SPELL_HARM_SERIOUS;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_HARM_SERIOUS_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_HARM_SERIOUS_DEIKHAN)) > num) {
          spell_num = SPELL_HARM_SERIOUS_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_HARM_CRITICAL:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_HARM_CRITICAL;
        }
        if ((num2 = getSkillValue(SPELL_HARM_CRITICAL)) > num) {
          spell_num = SPELL_HARM_CRITICAL;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_HARM_CRITICAL_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_HARM_CRITICAL_DEIKHAN)) > num) {
          spell_num = SPELL_HARM_CRITICAL_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_HARM:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_HARM;
        }
        if ((num2 = getSkillValue(SPELL_HARM)) > num) {
          spell_num = SPELL_HARM;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_HARM_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_HARM_DEIKHAN)) > num) {
          spell_num = SPELL_HARM_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_CURE_POISON:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_CURE_POISON;
        }
        if ((num2 = getSkillValue(SPELL_CURE_POISON)) > num) {
          spell_num = SPELL_CURE_POISON;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_CURE_POISON_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_CURE_POISON_DEIKHAN)) > num) {
          spell_num = SPELL_CURE_POISON_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_POISON:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_POISON;
        }
        if ((num2 = getSkillValue(SPELL_POISON)) > num) {
          spell_num = SPELL_POISON;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_POISON_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_POISON_DEIKHAN)) > num) {
          spell_num = SPELL_POISON_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_REFRESH:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_REFRESH;
        }
        if ((num2 = getSkillValue(SPELL_REFRESH)) > num) {
          spell_num = SPELL_REFRESH;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_REFRESH_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_REFRESH_DEIKHAN)) > num) {
          spell_num = SPELL_REFRESH_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_HEROES_FEAST:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_HEROES_FEAST;
        }
        if ((num2 = getSkillValue(SPELL_HEROES_FEAST)) > num) {
          spell_num = SPELL_HEROES_FEAST;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_HEROES_FEAST_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_HEROES_FEAST_DEIKHAN)) > num) {
          spell_num = SPELL_HEROES_FEAST_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_CREATE_FOOD:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_CREATE_FOOD;
        }
        if ((num2 = getSkillValue(SPELL_CREATE_FOOD)) > num) {
          spell_num = SPELL_CREATE_FOOD;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_CREATE_FOOD_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_CREATE_FOOD_DEIKHAN)) > num) {
          spell_num = SPELL_CREATE_FOOD_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_CREATE_WATER:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) 
          spell_num = SPELL_CREATE_WATER;
        
        if ((num2 = getSkillValue(SPELL_CREATE_WATER)) > num) {
          spell_num = SPELL_CREATE_WATER;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) 
          spell_num = SPELL_CREATE_WATER_DEIKHAN;
        
        if ((num2 = getSkillValue(SPELL_CREATE_WATER_DEIKHAN)) > num) {
          spell_num = SPELL_CREATE_WATER_DEIKHAN;
          num = num2; 
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_ARMOR:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) 
          spell_num = SPELL_ARMOR;
        
        if ((num2 = getSkillValue(SPELL_ARMOR)) > num) { 
          spell_num = SPELL_ARMOR;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) 
          spell_num = SPELL_ARMOR_DEIKHAN;
        
        if ((num2 = getSkillValue(SPELL_ARMOR_DEIKHAN)) > num) {
          spell_num = SPELL_ARMOR_DEIKHAN;
          num = num2; 
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_BLESS:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_BLESS;
        }
        if ((num2 = getSkillValue(SPELL_BLESS)) > num) { 
          spell_num = SPELL_BLESS;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_BLESS_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_BLESS_DEIKHAN)) > num) {
          spell_num = SPELL_BLESS_DEIKHAN;
          num = num2; 
          has_class = TRUE;
        }
      }
      return spell_num;
#if 0
    case SPELL_DETECT_POISON:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_DETECT_POISON;
        }
        if ((num2 = getSkillValue(SPELL_DETECT_POISON)) > num) {
          spell_num = SPELL_DETECT_POISON;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_DETECT_POISON_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_DETECT_POISON_DEIKHAN)) > num) {
          spell_num = SPELL_DETECT_POISON_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
#endif
    case SPELL_EXPEL:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_EXPEL;
        }
        if ((num2 = getSkillValue(SPELL_EXPEL)) > num) {
          spell_num = SPELL_EXPEL;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_EXPEL_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_EXPEL_DEIKHAN)) > num) {
          spell_num = SPELL_EXPEL_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_CLOT:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_CLOT;
        }
        if ((num2 = getSkillValue(SPELL_CLOT)) > num) {
          spell_num = SPELL_CLOT;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_CLOT_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_CLOT_DEIKHAN)) > num) {
          spell_num = SPELL_CLOT_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_RAIN_BRIMSTONE:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_RAIN_BRIMSTONE;
        }
        if ((num2 = getSkillValue(SPELL_RAIN_BRIMSTONE)) > num) {
          spell_num = SPELL_RAIN_BRIMSTONE;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_RAIN_BRIMSTONE_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_RAIN_BRIMSTONE_DEIKHAN)) > num) {
          spell_num = SPELL_RAIN_BRIMSTONE_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_STERILIZE:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_STERILIZE;
        }
        if ((num2 = getSkillValue(SPELL_STERILIZE)) > num) {
          spell_num = SPELL_STERILIZE;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_STERILIZE_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_STERILIZE_DEIKHAN)) > num) {
          spell_num = SPELL_STERILIZE_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_REMOVE_CURSE:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_REMOVE_CURSE;
        }
        if ((num2 = getSkillValue(SPELL_REMOVE_CURSE)) > num) {
          spell_num = SPELL_REMOVE_CURSE;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_REMOVE_CURSE_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_REMOVE_CURSE_DEIKHAN)) > num) {
          spell_num = SPELL_REMOVE_CURSE_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;

    case SPELL_CURSE:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_CURSE;
        }
        if ((num2 = getSkillValue(SPELL_CURSE)) > num) {
          spell_num = SPELL_CURSE;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_CURSE_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_CURSE_DEIKHAN)) > num) {
          spell_num = SPELL_CURSE_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_INFECT:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_INFECT;
        }
        if ((num2 = getSkillValue(SPELL_INFECT)) > num) {
          spell_num = SPELL_INFECT;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_INFECT_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_INFECT_DEIKHAN)) > num) {
          spell_num = SPELL_INFECT_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_CURE_DISEASE:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_CURE_DISEASE;
        }
        if ((num2 = getSkillValue(SPELL_CURE_DISEASE)) > num) {
          spell_num = SPELL_CURE_DISEASE;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_CURE_DISEASE_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_CURE_DISEASE_DEIKHAN)) > num) {
          spell_num = SPELL_CURE_DISEASE_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    case SPELL_EARTHQUAKE:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_EARTHQUAKE;
        }
        if ((num2 = getSkillValue(SPELL_EARTHQUAKE)) > num) {
          spell_num = SPELL_EARTHQUAKE;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_EARTHQUAKE_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_EARTHQUAKE_DEIKHAN)) > num) {
          spell_num = SPELL_EARTHQUAKE_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
   case SPELL_CALL_LIGHTNING:
      if (hasClass(CLASS_CLERIC)) {
        if (!has_class) {
          spell_num = SPELL_CALL_LIGHTNING;
        }
        if ((num2 = getSkillValue(SPELL_CALL_LIGHTNING)) > num) {
          spell_num = SPELL_CALL_LIGHTNING;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_DEIKHAN)) {
        if (!has_class) {
          spell_num = SPELL_CALL_LIGHTNING_DEIKHAN;
        }
        if ((num2 = getSkillValue(SPELL_CALL_LIGHTNING_DEIKHAN)) > num) {
          spell_num = SPELL_CALL_LIGHTNING_DEIKHAN;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
   case SKILL_DUAL_WIELD:
      if (hasClass(CLASS_RANGER)) {
        if (!has_class) {
          spell_num = SKILL_DUAL_WIELD;
        }
        if ((num2 = getSkillValue(SKILL_DUAL_WIELD)) > num) {
          spell_num = SKILL_DUAL_WIELD;
          num = num2;
          has_class = TRUE;
        }
      }
      if (hasClass(CLASS_THIEF)) {
        if (!has_class) {
          spell_num = SKILL_DUAL_WIELD_THIEF;
        }
        if ((num2 = getSkillValue(SKILL_DUAL_WIELD_THIEF)) > num) {
          spell_num = SKILL_DUAL_WIELD_THIEF;
          num = num2;
          has_class = TRUE;
        }
      }
      return spell_num;
    default:
      return spell_num;
  }
  return spell_num;
}

bool TBeing::isValidDiscClass(discNumT discNum, int classNum, int indNum) 
{
  if (discNum >= MAX_DISCS)
    return FALSE;
  if (indNum) {
  }

  if ((discNames[discNum].class_num == 0)) {
    return TRUE;
  }
  if ((discNames[discNum].class_num == classNum)) {
    return TRUE;
  }

  return FALSE;
}

discNumT getDisciplineNumber(spellNumT spell_num, int class_num)
{
  mud_assert(spell_num >= MIN_SPELL && spell_num < MAX_SKILL, 
          "Bad skill in getDisciplineNumber: %d", spell_num);

  if (!class_num) {
    if(discArray[spell_num]) {
      return discArray[spell_num]->disc;
    } else {
      return DISC_NONE;
    }
  } else {
    return DISC_NONE;
  }
  return DISC_NONE;
}

void CDiscipline::setDoLearnedness(int uNewValue)
{
    uDoLearnedness = uNewValue;
}

void CDiscipline::setNatLearnedness(int uNewValue)
{
    if (uNewValue > MAX_DISC_LEARNEDNESS)
      uNatLearnedness = MAX_DISC_LEARNEDNESS;
    else
      uNatLearnedness = uNewValue;
}

void CDiscipline::setLearnedness(int uNewValue)
{
    if (uNewValue > MAX_DISC_LEARNEDNESS)
      uLearnedness = MAX_DISC_LEARNEDNESS;
    else 
      uLearnedness = uNewValue;
}

enum logSkillAttemptT {
  ATTEMPT_ADD_NORM,
  ATTEMPT_ADD_ENGAGE,
  ATTEMPT_REM_NORM,
  ATTEMPT_REM_ENGAGE
};

static void logSkillAttempts(const TBeing *caster, spellNumT spell, logSkillAttemptT type)
{
  int value;

  if (caster->desc) {
    if (discArray[spell]->minMana) {
      if (type == ATTEMPT_ADD_NORM) {
        caster->desc->session.spell_success_attempts++;
        caster->desc->career.spell_success_attempts++;
      } else if (type == ATTEMPT_REM_NORM) {
        caster->desc->session.spell_success_attempts--;
        caster->desc->career.spell_success_attempts--;
      }
    } else if (discArray[spell]->minPiety) {
      if (type == ATTEMPT_ADD_NORM) {
        caster->desc->session.prayer_success_attempts++;
        caster->desc->career.prayer_success_attempts++;
      } else if (type == ATTEMPT_REM_NORM) {
        caster->desc->session.prayer_success_attempts--;
        caster->desc->career.prayer_success_attempts--;
      }
    } else {
      if (type == ATTEMPT_ADD_NORM) {
        caster->desc->session.skill_success_attempts++;
        caster->desc->career.skill_success_attempts++;
      } else if (type == ATTEMPT_REM_NORM) {
        caster->desc->session.skill_success_attempts--;
        caster->desc->career.skill_success_attempts--;
      }
    }
  }

  value = caster->getSkillValue(spell);

  if (value < 0)
    return;

#if 0
// don't log if maxed?
  if (value >= MAX_SKILL_LEARNEDNESS)
    return; 
#endif

  // count the number of immortal uses
  if (caster->desc && caster->GetMaxLevel() > MAX_MORT) {
    switch (type) {
      case ATTEMPT_ADD_NORM:
        discArray[spell]->immUses++;
        discArray[spell]->immLevels += caster->getSkillLevel(spell);
        discArray[spell]->immLearned += (long) max(0, (int) caster->getSkillValue(spell));
        break;
      case ATTEMPT_REM_NORM:
        discArray[spell]->immUses--;
        discArray[spell]->immLevels -= caster->getSkillLevel(spell);
        discArray[spell]->immLearned -= (long) max(0, (int) caster->getSkillValue(spell));
        break;
      default:
        break;
    }
    return;
  } 

  // count mob uses
  if (!caster->desc || !caster->isPc()) {
    switch (type) {
      case ATTEMPT_ADD_NORM:
        discArray[spell]->mobUses++;
        discArray[spell]->mobLevels += caster->getSkillLevel(spell);
        discArray[spell]->mobLearned += (long) max(0, (int) caster->getSkillValue(spell));
        break;
      case ATTEMPT_REM_NORM:
        discArray[spell]->mobUses--;
        discArray[spell]->mobLevels -= caster->getSkillLevel(spell);
        discArray[spell]->mobLearned -= (long) max(0, (int) caster->getSkillValue(spell));
        break;
      default:
        break;
    }
    return;
  }

  // mortal PCs only get here

  switch (type) {
    case ATTEMPT_ADD_NORM:
      discArray[spell]->uses++;
      discArray[spell]->levels += caster->GetMaxLevel();
      discArray[spell]->learned += value;
      discArray[spell]->focusValue += caster->getStat(STAT_CURRENT, STAT_FOC);

      // log quality
      if (value <= 20) {
        discArray[spell]->newAttempts++;
      } else if (value <= 40) {
        discArray[spell]->lowAttempts++;
      } else if (value <= 60) {
        discArray[spell]->midAttempts++;
      } else if (value <= 80) {
        discArray[spell]->goodAttempts++;
      } else {
        discArray[spell]->highAttempts++;
      }

      break;
    case ATTEMPT_ADD_ENGAGE:
      discArray[spell]->engAttempts++;
      break;
    case ATTEMPT_REM_NORM:
      discArray[spell]->uses--;
      discArray[spell]->levels -= caster->GetMaxLevel();
      discArray[spell]->learned -= value;
      discArray[spell]->focusValue -= caster->getStat(STAT_CURRENT, STAT_FOC);

      if (value <= 20) {
        discArray[spell]->newAttempts--;
      } else if (value <= 40) {
        discArray[spell]->lowAttempts--;
      } else if (value <= 60) {
        discArray[spell]->midAttempts--;
      } else if (value <= 80) {
        discArray[spell]->goodAttempts--;
      } else {
        discArray[spell]->highAttempts--;
      }
      break;
    case ATTEMPT_REM_ENGAGE:
      discArray[spell]->engAttempts--;
      break;
  }
  return;
}

enum skillSuccessT {
  SKILL_SUCCESS_NORMAL,
  SKILL_SUCCESS_POTION
};

static void logSkillSuccess(const TBeing *caster, spellNumT spell, skillSuccessT type)
{
  // this is used to log skill success 
  // there is usually no need to call this directly as it sits inside bSuccess
  // however, it can be called directly if appropriate

  if (!caster) {
    vlogf(5,"Something went into logSkillSuccess with no caster");
    return;
  }

  if (type == SKILL_SUCCESS_POTION) {
    if (caster->GetMaxLevel() > MAX_MORT && caster->desc)
      discArray[spell]->potSuccessImm++;
    else if (!caster->desc || !caster->isPc())
      discArray[spell]->potSuccessMob++;
    else
      discArray[spell]->potSuccess++;
    return;
  }
  
  if ((caster->GetMaxLevel() > MAX_MORT) && caster->desc) {
    discArray[spell]->immSuccess++;
    return;
  } else if (!caster->desc || !caster->isPc()) {
    discArray[spell]->mobSuccess++;
    return;
  }

  // mortal PCs only get here
  discArray[spell]->success++;
  
  if (discArray[spell]->minMana) {
    caster->desc->session.spell_success_pass++;
    caster->desc->career.spell_success_pass++;
  } else if (discArray[spell]->minPiety) {
    caster->desc->session.prayer_success_pass++;
    caster->desc->career.prayer_success_pass++;
  } else {
    caster->desc->session.skill_success_pass++;
    caster->desc->career.skill_success_pass++;
  }
  return;
}

enum logSkillFailT {
  FAIL_GENERAL,
  FAIL_FOCUS,
  FAIL_ENGAGE
};

static void logSkillFail(const TBeing *caster, spellNumT spell, logSkillFailT type)
{
  // this is used to log skill fails 
  // there is usually no need to call this directly as it sits inside bSuccess
  // however, it can be called directly if appropriate

  if (!caster) {
    vlogf(5,"Something went into logSkillFail with no caster");
    return;
  }

#if DISC_DEBUG
  vlogf(2, "%s Fail Spell %s (%d) ubComp < 0", caster->getName(), discArray[spell]->name, spell);
#endif

  if ((caster->GetMaxLevel() > MAX_MORT) && caster->desc) {
    discArray[spell]->immFail++;
    return;
  } else if (!caster->desc || !caster->isPc()) {
    discArray[spell]->mobFail++;
    return;
  }

  // mortal PCs only get here
  switch (type) {
    case FAIL_GENERAL:    // genFail
      discArray[spell]->genFail++;
      break;
    case FAIL_FOCUS:   // Focus Fail
      discArray[spell]->focFail++;
      break;
    case FAIL_ENGAGE:   // EngageFail
      discArray[spell]->engFail++;
      break;
  }
  discArray[spell]->fail++;
  return;
}

static bool bSucCounter(TBeing *caster, skillUseClassT skillType, spellNumT spell, int roll, int ubCompetence)
{
  // success counter
  if (caster->desc) {
    if (skillType == SPELL_MAGE ||
        skillType == SPELL_CLERIC || 
        skillType == SPELL_DEIKHAN ||
        skillType == SPELL_SHAMAN || 
        skillType == SPELL_RANGER) {
      // Is a spell and not a skill
      if (((caster->fight() && !caster->isAffected(AFF_ENGAGER)) ||
         (!caster->fight() && IS_SET(discArray[spell]->targets, TAR_VIOLENT) && !IS_SET(caster->desc->autobits, AUTO_ENGAGE | AUTO_ENGAGE_ALWAYS)))) {
        // caster is hitting not engaging at the time
        logSkillAttempts(caster, spell, ATTEMPT_ADD_ENGAGE);
        int boost = caster->plotStat(STAT_CURRENT, STAT_FOC, 60, 10, 40, 1);
        int num = ::number(1,100);;

        // if they are not fighting, don't let fighting distract...
        // if my victim is uncon, skip distract too
        if (!caster->fight() ||
            (caster->attackers == 1 && !caster->fight()->awake()))
          boost = 0;

        if (num <= boost) {
          //fail them some % of the time if fighting-adjust if nessessarry
          logSkillFail(caster, spell, FAIL_ENGAGE);
#if DISC_DEBUG
          if (caster->desc && caster->isPc()) {
            vlogf(2, "%s Fail Spell %s (%d) EngFail: boost (%d) num (%d) , roll (%d) ubComp (%d)", caster->getName(), discArray[spell]->name, spell, boost, num, roll, ubCompetence);
          }
#endif
          switch (getSpellType(discArray[spell]->typ)) {
            case SPELL_CASTER:
              caster->sendTo(COLOR_SPELLS, "<c>Your fighting distracts you and you feel your casting skills failing you.<1>\n\r");
              break;
            case SPELL_PRAYER:
              caster->sendTo(COLOR_SPELLS, "<c>Your fighting distracts you from your prayer.<1>\n\r");
              break;
            default:
              vlogf(9, "bad spot in distract (1).");
              break;
          }
          return FALSE;
        } else {
          // success since made the fightin roll for spells
          if ((caster->inPraying || caster->spelltask) && (skillType == SPELL_CLERIC || skillType == SPELL_DEIKHAN)) {
            if (!enforceHolySym(caster, spell, TRUE)) {
              logSkillAttempts(caster, spell, ATTEMPT_REM_NORM);
              logSkillAttempts(caster, spell, ATTEMPT_REM_ENGAGE);
              return FALSE;
            }
          }
          logSkillSuccess(caster, spell, SKILL_SUCCESS_NORMAL);
          return TRUE;
        }
      } else {
        // Is gonna be an engager--no penalty
        if ((caster->inPraying || caster->spelltask) && (skillType == SPELL_CLERIC || skillType == SPELL_DEIKHAN)) {
          if (!enforceHolySym(caster, spell, TRUE)) {
            logSkillAttempts(caster, spell, ATTEMPT_REM_NORM);
            return FALSE;
          }
        }
        logSkillSuccess(caster, spell, SKILL_SUCCESS_NORMAL);
        return TRUE;
      }
    } else {
      if ((caster->inPraying || caster->spelltask) && (skillType == SPELL_CLERIC || skillType == SPELL_DEIKHAN)) {
        if (!enforceHolySym(caster, spell, TRUE)) {
          logSkillAttempts(caster, spell, ATTEMPT_REM_NORM);
          return FALSE;
        }
      }
      logSkillSuccess(caster, spell, SKILL_SUCCESS_NORMAL);
      return TRUE;
    }
  } else {
    if ((caster->inPraying || caster->spelltask) && (skillType == SPELL_CLERIC || skillType == SPELL_DEIKHAN)) {
      if (!enforceHolySym(caster, spell, TRUE)) {
        logSkillAttempts(caster, spell, ATTEMPT_REM_NORM);
        return FALSE;
      }
    }
    logSkillSuccess(caster, spell, SKILL_SUCCESS_NORMAL);
    return TRUE;
  }
}

bool bSuccess(TBeing * caster, int ubCompetence, double dPiety, spellNumT spell)
{
// Is same as other formulas, with this correction being made
// since factions' aren't in use, I'm simplifying and just making it
// call the other function
  return bSuccess(caster, ubCompetence, spell);

#if FACTIONS_IN_USE
  // slight penalty based on low getPerc()
  if (caster->desc) {
    int pietyNum;
    if ((skillType == SPELL_CLERIC) || (skillType == SPELL_DEIKHAN) ||
        (skillType == SKILL_CLERIC) || (skillType == SKILL_DEIKHAN)) {
      pietyNum = min(95, (3 * caster->GetMaxLevel())); 
    } else { 
      pietyNum = min(70, (2 * caster->GetMaxLevel()));
    }
    pietyNum = min(0, (((int) dPiety) - pietyNum));
    pietyNum = max(-64, pietyNum);
    pietyNum /= -4;
    roll += pietyNum;
  }
#endif
}

#if 0
static void logLearnFail(TBeing *caster, spellNumT spell, int type)
{
  // this is used to log learn fail
  // there is usually no need to call this directly as it sits inside i
  // learnFromDoing and learnFromDoingUnusual

  if (!caster) {
    vlogf(5,"Something went into logLearnFail with no caster (%d)", spell);
    return;
  }

  if (caster->GetMaxLevel() > MAX_MORT) {
    return;
  } 

  if (!caster->desc) {
    vlogf(5,"Something went into logLearnFail with no desc (%d)", spell);
    return;
  }

#if DISC_DEBUG
  vlogf(2, "%s Fail Spell %s (%d) ubComp < 0", caster->getName(), discArray[spell]->name, spell); 
#endif

  if (type) {
    ;
  }
  discArray[spell]->learnFail++;
  return;
}
#endif

bool bSuccess(TBeing * caster, int ubCompetence, spellNumT spell)
{
  // number of uses
  logSkillAttempts(caster, spell, ATTEMPT_ADD_NORM);

  if (caster->getQuaffUse()) {
    logSkillSuccess(caster, spell, SKILL_SUCCESS_POTION);
    return true;
  }

  if (caster->isImmortal() && caster->desc &&
      IS_SET(caster->desc->autobits, AUTO_SUCCESS)) {
    if (caster->isPlayerAction(PLR_NOHASSLE))
      return TRUE;
    else
      return FALSE;
  }

  if (caster->desc) {
    // Do learning
    if (caster->getRawSkillValue(spell) >= 0) {
      if (caster->learnFromDoing(spell, SILENT_NO, 0)) {
        ubCompetence++;
      }
    }
  }

   // not learned at all
  if (ubCompetence <= 0) {
    logSkillFail(caster, spell, FAIL_GENERAL);
#if DISC_DEBUG
    if (caster->desc && caster->isPc()) {
      vlogf(2, "%s Fail Spell %s (%d) ubComp < 0", caster->getName(), discArray[spell]->name, spell);
    }
#endif
    return FALSE;
  }

// force into range
  ubCompetence = min(max(ubCompetence, 0), (int) MAX_SKILL_LEARNEDNESS);

#if 1
  // Here's the basis of this stuff:
  // At max learning, we desire the following results:
  // trivial    = 100%
  // easy       = 90%
  // normal     = 80%
  // difficult  = 70%
  // dangerous  = 60%
  // for less than maxed learning, scale it up linearly
  // Have focus factor in: high focus = 1.25 * above rates
  // low focus = 0.80 * above rates

  float limit = getSkillDiffModifier(spell);

  // scale linearly based on learning
  limit *= ubCompetence;
  limit /= MAX_SKILL_LEARNEDNESS;

  // factor in focus
  limit *= caster->plotStat(STAT_CURRENT, STAT_FOC, 0.80, 1.25, 1.00);

  // make other adjustments here
  // possibly have some for things like position, etc

  int iLimit = (int) limit;
  int roll = ::number(0,99);
  skillUseClassT skillType = discArray[spell]->typ;

  if (roll < iLimit) {
    // success
    return bSucCounter(caster, skillType, spell, roll, ubCompetence);

  } else {
    // fail
    logSkillFail(caster, spell, FAIL_GENERAL);
    return false;
  }
#else
// old formula
  int boost = 0, num = 0; 
  skillUseClassTyp skillType;
  int focus;

// a LOW roll means success
// DO NOT DELETE-- COSMO
// A low roll is better
  roll = ::number(1,100);
  skillType = discArray[spell]->typ;

// if (roll > 98) {
  if (roll > 99) {
    // fail counter
    logSkillFail(caster, spell, FAIL_GENERAL);
#if DISC_DEBUG
    if (caster->desc && caster->isPc()) {
      vlogf(2, "%s Fail Spell %s (%d) roll > 98.", caster->getName(), discArray[spell]->name, spell);
    }
#endif
    return FALSE;
  } else if (roll <= 5) {
    // success counter
    if ((caster->inPraying || caster->spelltask) && (skillType == SPELL_CLERIC || skillType == SPELL_DEIKHAN)) {
      if (!enforceHolySym(caster, spell, TRUE)) {
        logSkillAttempts(caster, spell, ATTEMPT_REM_NORM);
        return FALSE;
      }
    }
    logSkillSuccess(caster, spell, SKILL_SUCCESS_NORMAL);
    return TRUE;
  }


//if (discArray[spell]->task >= 0) { // skills set this to -1, ignore difficul ty
//    roll += (discArray[spell]->task* 5);

    // increase difficulty if limited position
//  if (!(spell == SPELL_PENANCE) && !(spell == SKILL_ATTUNE) &&
//      !(spell == SKILL_MEDITATE) &&
//      !(IS_SET(discArray[spell]->comp_types, SPELL_IGNORE_POSITION))) {
  if (!(IS_SET(discArray[spell]->comp_types, SPELL_IGNORE_POSITION))) {
    boost = caster->plotStat(STAT_CURRENT, STAT_FOC, 1, 45, 25);
    if (caster->getPosition() == POSITION_RESTING) {
      roll += max(0, (50 - boost));
    } else if (caster->getPosition() == POSITION_SITTING) {
      roll += max(0,( 35 - boost));
    } else if (caster->getPosition() == POSITION_CRAWLING) {
       roll += max(0, (20 - boost));
    }
  }
#if FACTIONS_IN_USE
  // a penalty based on low getPerc()
  if (caster->desc) {
    int pietyNum;
    if ((skillType == SPELL_CLERIC) || (skillType == SPELL_DEIKHAN) ||
        (skillType == SKILL_CLERIC) || (skillType == SKILL_DEIKHAN)) {
      pietyNum = min(95, (3 * caster->GetMaxLevel()));
    } else {
      pietyNum = min(75, (2 * caster->GetMaxLevel()));
    }
    pietyNum = min(0, (((int) caster->getPerc()) - pietyNum));
    pietyNum = max(-64, pietyNum);
    pietyNum /= -4;
    roll += pietyNum;
  }
#endif

  if (roll > ubCompetence) {
    logSkillFail(caster, spell, FAIL_GENERAL);
#if DISC_DEBUG
    if (caster->desc && caster->isPc()) {
      vlogf(2, "%s Fail Spell %s (%d) GenFail: roll (%d) ubComp (%d)", caster->getName(), discArray[spell]->name, spell, roll, ubCompetence);
    }
#endif
    return FALSE;
  } else {
// FIRST FOCUS

   if (discArray[spell]->task == TASK_IGNORE_DIFF) {
      num = ::number(1, 100);
      boost = 100;
    } else if (discArray[spell]->task >= 0) { // skills set this to -1, ignore difficu
      num = ::number(1, (85 + ((1 + discArray[spell]->task) * 4)));
      boost = caster->plotStat(STAT_CURRENT, STAT_FOC, 60, 95, 85, 1);
    } else {
      num = ::number(1, 90);
      boost = caster->plotStat(STAT_CURRENT, STAT_FOC, 80, 95, 88, 1);
    }

    if (caster->desc && dynamic_cast<const TPerson *> (caster)) {
      focus = caster->plotStat(STAT_CHOSEN, STAT_FOC, -25, 25, 0, 1);
//      focus = caster->chosenStats.values[STAT_FOC] / 2;
      boost = min(99, (boost + focus));
    }
    if ((num > boost)) {
      logSkillFail(caster, spell, FAIL_FOCUS);
#if DISC_DEBUG
      if (caster->desc && caster->isPc()) {
        vlogf(2, "%s Fail Spell %s (%d) focFail: boost (%d) num (%d) , roll (%d)ubComp (%d)", caster->getName(), discArray[spell]->name, spell, boost, num, roll, ubCompetence);
      }
#endif
      return FALSE;
    }

    // success counter
    return bSucCounter(caster, skillType, spell, roll, ubCompetence);
  }
#endif
}

byte defaultProficiency(byte uLearned, byte uStart, byte uLearn)
{
  return((uLearned-uStart)*uLearn);
}

critSuccT critSuccess(TBeing *caster, spellNumT spell)
{
  // arbitrary to control overall rate of these
  if (::number(0,9))
    return CRIT_S_NONE;

  int roll, adjusted, task = 100; 
  discNumT das = getDisciplineNumber(spell, FALSE);
  CDiscipline *cd;

  if (das == DISC_NONE) {
    vlogf(5, "bad disc for skill %d", spell);
    return CRIT_S_NONE;
  }
  if (!(cd = caster->getDiscipline(das)))
    return CRIT_S_NONE;

  if (discArray[spell]->task >= 0) {
    task += (discArray[spell]->task* 5);

    // increase difficulty if limited position
    if (caster->getPosition() == POSITION_RESTING)
      task += 200;
    else if (caster->getPosition() == POSITION_SITTING)
      task += 100;
    else if (caster->getPosition() == POSITION_CRAWLING)
      task += 300;
   }

  adjusted = cd->getLearnedness() - discArray[spell]->start + 1;

  adjusted *= 2;
  adjusted += 100;
  adjusted=max(100, adjusted);

  task *= 100;
  task /= adjusted;
  task = max(40, task);
  task = min(400,task);
  roll = ::number(1, task);

// pick it up below in the cases

  switch (roll) {
    case 1:
      CS(caster, spell);
      return(CRIT_S_KILL);
      break;
    case 2:
    case 3:
      CS(caster, spell);
      return(CRIT_S_TRIPLE);
      break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
      CS(caster, spell);
      return(CRIT_S_DOUBLE);
      break;
    default:
      return(CRIT_S_NONE);
  }
}

critFailT critFail(TBeing *caster, spellNumT spell)
{
  // arbitrary to control overall rate of these
  if (::number(0,9))
    return CRIT_F_NONE;

  int roll, adjusted, task = 100;

  if (discArray[spell]->task >= 0) {
    task -= (discArray[spell]->task* 10);

    // adjust for position
    if (caster->getPosition() == POSITION_RESTING)
      task -= 15;
    else if (caster->getPosition() == POSITION_SITTING)
      task -= 10;
    else if (caster->getPosition() == POSITION_CRAWLING)
      task -= 20;
  }
  discNumT das = getDisciplineNumber(spell, FALSE);
  if (das == DISC_NONE) {
    vlogf(5, "bad disc for spell %d", spell);
    return CRIT_F_NONE; 
  } 
// adjust for learnedness of caster
  adjusted = caster->getDiscipline(das)->getLearnedness() - discArray[spell]->start + 1;

  adjusted *= 2;  
  adjusted += 100;
  adjusted=max(100, adjusted);

  task *= adjusted;  
  task /= 100;

  // Higher Focus gets better results
  task += ::number(0, caster->plotStat(STAT_CHOSEN, STAT_FOC, -5, 5, 0, 1));
  task = max(30, task);
  task = min(220,task);
  roll = ::number(1, task);

// Pick it up again below

  switch(roll) {
    case 1:
      CF(caster, spell);
      return(CRIT_F_HITOTHER);
      break;
    case 2:
    case 3:
    case 4:
    case 5:
      CF(caster, spell);
      return(CRIT_F_HITSELF);
      break;
    default:
      return(CRIT_F_NONE);
  }
}

int CDiscipline::useMana(byte ubCompetence, byte ubDifficulty)
{
  return(max((int) ubDifficulty,(100-((int) ubCompetence))/2));
}

double CDiscipline::usePerc(byte ubCompetence, double fDifficulty)
{
  return(fDifficulty+((fDifficulty*((double)(100-ubCompetence)))/100));
}

int checkMana(TBeing * caster, int mana)
{
  if (caster->noMana(mana)) {
    act("You can't seem to summon the magic energies...", FALSE, caster, 0, 0, TO_CHAR, ANSI_CYAN);
    act("$n begins to glow for a brief second, but it quickly fades.", FALSE, caster, NULL, NULL, TO_ROOM, ANSI_CYAN);
    return TRUE;
  } else   
    return FALSE;
}

#if FACTIONS_IN_USE
bool checkPerc(const TBeing * caster, double align)
{
  if (caster->percLess(align)) {
    act("$d scoffs in your general direction!", FALSE, caster, NULL, NULL, TO_CHAR);
    act("$n blushes as $d falls down laughing at $s request!", FALSE, caster, NULL, NULL, TO_ROOM);
    return TRUE;
  } else  
    return FALSE;
}
#endif

void TSymbol::findSym(TSymbol **best)
{
  *best = this;
}

TSymbol * TBeing::findHolySym(silentTypeT silent) const
{
  TThing *primary, *secondary, *neck;
  TSymbol *best = NULL;
  devotionLevelT dev_lev = getDevotionLevel();
  int bad_faction = 0, attuned = 0;
  int attuneCode = 0;
  factionTypeT sym_faction = FACT_UNDEFINED;

  primary = heldInPrimHand();
  secondary = heldInSecHand();
  neck = equipment[WEAR_NECK];

  if (isPc()) {
    if (primary) {
      best = NULL;
      primary->findSym(&best);
      if (best) {
        sym_faction = best->getSymbolFaction();
        attuned = 1;
        if ((sym_faction != FACT_UNDEFINED) || attuneCode) {
          if ((getFaction() == sym_faction) || attuneCode) { 
            // symbol in primary
            if (secondary && dev_lev <= DEV_LEV_SYMB_EITHER_OTHER_FREE) {
              // 0 = prim + sec free
              // 1 = sec + other free
              // must have sec free
              if (!silent) {
                act("$n looks confused as $e tentatively raises $p to the sky!",
                    FALSE, this, primary, NULL, TO_ROOM);
                act("$p feels inert as $N disrupts the conduit to $d!",
                    FALSE, this, primary, secondary, TO_CHAR);
              }
              return NULL;
            }
            return best;
          } else {
            bad_faction++;
          }
        }
      }
    }
    if (secondary) {
      best = NULL;
      secondary->findSym(&best);
      if (best) {
        sym_faction = best->getSymbolFaction();
        if ((sym_faction != FACT_UNDEFINED) || attuneCode) {
          attuned++;
          if ((getFaction() == sym_faction) || attuneCode) {
            // symbol in secondary
            if (dev_lev <= DEV_LEV_SYMB_PRIM_OTHER_FREE) {
             // 0 = prim + sec free
             // can only have it in primary
              if (!silent) {
                act("$n looks confused as $e tentatively raises $p to the sky!",
                    FALSE, this, secondary, NULL, TO_ROOM);
                act("$p feels inert and unbalanced as you raise it to the sky with the wrong hand!",
                    FALSE, this, secondary, NULL, TO_CHAR);
              }
              return NULL;
            } else if (primary && dev_lev < DEV_LEV_SYMB_EITHER_OTHER_EQUIP) {
              // 1 = sec + other free
              // 2 = prim + sec no restrict
              // must have primary free
              if (!silent) {
                act("$n looks confused as $e tentatively raises $p to the sky!",
                    FALSE, this, secondary, NULL, TO_ROOM);
                act("$p feels inert as $N disrupts the conduit to $d!",
                    FALSE, this, secondary, primary, TO_CHAR);
              }
              return NULL;
            }
            return best;
          } else {
            bad_faction++;
          }
        }
      }
    }
    if (neck) {
      best = NULL;
      neck->findSym(&best);
      if (best) {
        sym_faction = best->getSymbolFaction();
        if ((sym_faction != FACT_UNDEFINED) || attuneCode) {
          attuned++;
          if ((getFaction() == sym_faction) || attuneCode) {
            // symbol on neck
            if (dev_lev < DEV_LEV_SYMB_NECK) {
              // can't use neck
              if (!silent) {
                act("$n looks confused.",
                    FALSE, this, NULL, NULL, TO_ROOM);
                act("$d can't contact you without your symbol in your hands.",
                    FALSE, this, NULL, NULL, TO_CHAR);
              }
              return NULL;
            }
            return best;
          } else {
            bad_faction++;
          }
        }
      }
    }
  }
  if (!silent) {
    act("$n frantically gropes for something that $e can't seem to find!", 
             FALSE, this, NULL, NULL, TO_ROOM);
    if (bad_faction > 1) {
      act("Your deities do not recognize your choice of holy symbols!", 
             FALSE, this, NULL, NULL, TO_CHAR);
    } else if (bad_faction == 1) {
      act("Your deities do not recognize your choice of holy symbols!", FALSE, this, NULL, NULL, TO_CHAR);
    } else if (!bad_faction && (attuned > 1)) {
      act("Your holy symbols have not yet been attuned!", FALSE, this, NULL, NULL, TO_CHAR);
    } else if (!bad_faction && (attuned == 1)) {
     act("Your holy symbol has not yet been attuned!", FALSE, this, NULL, NULL, TO_CHAR);

    } else {
      act("You frantically grope for your holy symbol but can't find it! DAMN!", FALSE, this, NULL, NULL, TO_CHAR);
    }
  }
  return NULL;
}

bool canDoVerbal(TBeing *ch)
{
  // glub glub glub
  if (ch->roomp && ch->roomp->isUnderwaterSector())
    return FALSE;

  return (ch->canSpeak());
}

bool checkRoom(const TBeing * ch)
{
  if (ch->roomp->isRoomFlag(ROOM_NO_MAGIC)) {
    act("$n is surrounded by a dim glow that quickly fades.", TRUE, ch, 0, NULL, TO_ROOM);
    ch->sendTo("Some mysterious force prevents you from doing that here.\n\r");
    return FALSE;
  }
  return TRUE;
}

void checkFactionHurt(TBeing * caster, TBeing * victim) 
{
  int dec_amt;

  if (caster->isSameFaction(victim)) {
    dec_amt = (int) (caster->getMove() / 4);
    caster->addToMove(-dec_amt);
    caster->sendTo("%s frown upon the harming of a creature of the same faction.\n\r", good_cap(caster->yourDeity(your_deity_val, FIRST_PERSON)).c_str());
    caster->sendTo("You are exhausted from the effort of doing so.\n\r");
    act("$n's chest heaves from exhaustion.", FALSE, caster, 0, 0, TO_ROOM);
    caster->updatePos();
  }
}

void checkFactionHelp(TBeing *caster, TBeing *victim) 
{
  int dec_amt;

  if (caster->isOppositeFaction(victim)) {
    dec_amt = (int) (caster->getMove() / 4);
    caster->addToMove(-dec_amt);
    caster->sendTo("%s frown upon the minions of the enemy.\n\r", good_cap(caster->yourDeity(your_deity_val, FIRST_PERSON)).c_str());
    caster->sendTo("You are exhausted from the effort of doing so.\n\r");
    act("$n's chest heaves from exhaustion.", FALSE, caster, 0, 0, TO_ROOM);
    caster->updatePos();
  }
}

void TBeing::assignDisciplinesClass()
{
  // verify that disciplines haven't already been initted
  mud_assert(discs == NULL, "assignDisc(): assignment when already initted (1)");
  try {
    discs = new CMasterDiscipline();
  } catch (...) {
    forceCrash("assignDisc(): assertion trapped");
  }
  mud_assert(discs != NULL, "assignDisc(): discs was null after new");

  if (isPc()) {
    discs->disc[DISC_MAGE] = new CDMage();
    discs->disc[DISC_AIR] = new CDAir();
    discs->disc[DISC_ALCHEMY] = new CDAlchemy();
    discs->disc[DISC_EARTH] = new CDEarth();
    discs->disc[DISC_FIRE] = new CDFire();
    discs->disc[DISC_SORCERY] = new CDSorcery();
    discs->disc[DISC_SPIRIT] = new CDSpirit();
    discs->disc[DISC_WATER] = new CDWater();

    discs->disc[DISC_CLERIC] = new CDCleric();
    discs->disc[DISC_WRATH] = new CDWrath();
    discs->disc[DISC_AEGIS] = new CDAegis();
    discs->disc[DISC_AFFLICTIONS] = new CDAfflict();
    discs->disc[DISC_CURES] = new CDCures();
    discs->disc[DISC_HAND_OF_GOD] = new CDHand();

    discs->disc[DISC_WARRIOR] = new CDWarrior();
    discs->disc[DISC_HTH] = new CDHTH();
    discs->disc[DISC_BRAWLING] = new CDBrawling();
    discs->disc[DISC_PHYSICAL] = new CDPhysical();
    discs->disc[DISC_SMYTHE] = new CDSmythe();

    discs->disc[DISC_RANGER] = new CDRanger();
    discs->disc[DISC_RANGER_FIGHT] = new CDRangerFight();
    discs->disc[DISC_ANIMAL] = new CDAnimal();
    discs->disc[DISC_PLANTS] = new CDPlants();
    discs->disc[DISC_SURVIVAL] = new CDSurvival();

    discs->disc[DISC_DEIKHAN] = new CDDeikhan();
    discs->disc[DISC_DEIKHAN_FIGHT] = new CDDeikhanFight();
    discs->disc[DISC_MOUNTED] = new CDMounted();
    discs->disc[DISC_DEIKHAN_AEGIS] = new CDDeikhanAegis();
    discs->disc[DISC_DEIKHAN_CURES] = new CDDeikhanCures();
    discs->disc[DISC_DEIKHAN_WRATH] = new CDDeikhanWrath();

    discs->disc[DISC_MONK] = new CDMonk();
    discs->disc[DISC_MEDITATION_MONK] = new CDMeditationMonk();
    discs->disc[DISC_LEVERAGE] = new CDLeverage();
    discs->disc[DISC_MINDBODY] = new CDMindBody();
    //    discs->disc[DISC_JUDOKI] = new CDJudoki();
    //    discs->disc[DISC_KUKSOKI] = new CDKuksoki();
    discs->disc[DISC_FOCUSED_ATTACKS] = new CDFAttacks();
    //    discs->disc[DISC_NINJOKI] = new CDNinjoki();
    //    discs->disc[DISC_SHOGOKI] = new CDShogoki();

    discs->disc[DISC_THIEF] = new CDThief();
    discs->disc[DISC_THIEF_FIGHT] = new CDThiefFight();
    discs->disc[DISC_MURDER] = new CDMurder();
    discs->disc[DISC_LOOTING] = new CDLooting();
    discs->disc[DISC_POISONS] = new CDPoisons();
    discs->disc[DISC_STEALTH] = new CDStealth();
    discs->disc[DISC_TRAPS] = new CDTraps();

    discs->disc[DISC_SHAMAN] = new CDShaman();
    discs->disc[DISC_SHAMAN_FIGHT] = new CDShamanFight();
    discs->disc[DISC_SHAMAN_ALCHEMY] = new CDShamanAlchemy();
    discs->disc[DISC_SHAMAN_HEALING] = new CDShamanHealing();
    discs->disc[DISC_NATURE] = new CDNature();
    discs->disc[DISC_UNDEAD] = new CDUndead();
    discs->disc[DISC_DRAINING] = new CDDraining();
    discs->disc[DISC_TOTEM] = new CDTotem();

    discs->disc[DISC_WIZARDRY] = new CDWizardry();
    discs->disc[DISC_FAITH] = new CDFaith();
    discs->disc[DISC_THEOLOGY] = new CDTheology();
    discs->disc[DISC_LORE] = new CDLore();

    discs->disc[DISC_SLASH] = new CDSlash();
    discs->disc[DISC_BLUNT] = new CDBash();
    discs->disc[DISC_PIERCE] = new CDPierce();
    discs->disc[DISC_RANGED] = new CDRanged();
    discs->disc[DISC_BAREHAND] = new CDBarehand();

    discs->disc[DISC_COMBAT] = new CDCombat();
    discs->disc[DISC_ADVENTURING] = new CDAdventuring();

  }
  // assign these to every class
  if (!isPc()) {
    discs->disc[DISC_ADVENTURING] = new CDAdventuring();
    discs->disc[DISC_COMBAT] = new CDCombat();
  }

  getDiscipline(DISC_ADVENTURING)->ok_for_class = (1<<MAX_CLASSES) - 1;
  getDiscipline(DISC_COMBAT)->ok_for_class = (1<<MAX_CLASSES) - 1;


  if (!player.Class) {
    vlogf(10,"call to assignDisciplinesClass without a valid Class (%s)", getName());
    return;
  }

  if (hasClass(CLASS_MAGIC_USER)) {
    if (!isPc()) {
      discs->disc[DISC_MAGE] = new CDMage();
      discs->disc[DISC_AIR] = new CDAir();
      discs->disc[DISC_ALCHEMY] = new CDAlchemy();
      discs->disc[DISC_EARTH] = new CDEarth();
      discs->disc[DISC_FIRE] = new CDFire();
      discs->disc[DISC_SORCERY] = new CDSorcery();
      discs->disc[DISC_SPIRIT] = new CDSpirit();
      discs->disc[DISC_WATER] = new CDWater();
      discs->disc[DISC_WIZARDRY] = new CDWizardry();
      discs->disc[DISC_LORE] = new CDLore();
      discs->disc[DISC_PIERCE] = new CDPierce();

    }
    getDiscipline(DISC_MAGE)->ok_for_class  |= CLASS_MAGIC_USER;
    getDiscipline(DISC_AIR)->ok_for_class  |= CLASS_MAGIC_USER;
    getDiscipline(DISC_ALCHEMY)->ok_for_class |= CLASS_MAGIC_USER;
    getDiscipline(DISC_EARTH)->ok_for_class |= CLASS_MAGIC_USER;
    getDiscipline(DISC_FIRE)->ok_for_class  |= CLASS_MAGIC_USER;
    getDiscipline(DISC_SORCERY)->ok_for_class |= CLASS_MAGIC_USER;
    getDiscipline(DISC_SPIRIT)->ok_for_class |= CLASS_MAGIC_USER;
    getDiscipline(DISC_WATER)->ok_for_class |= CLASS_MAGIC_USER;
    getDiscipline(DISC_WIZARDRY)->ok_for_class |= CLASS_MAGIC_USER;
    getDiscipline(DISC_LORE)->ok_for_class |= CLASS_MAGIC_USER;
    getDiscipline(DISC_PIERCE)->ok_for_class |= CLASS_MAGIC_USER;
  }

  if (hasClass(CLASS_CLERIC)) {
    if (!isPc()) {
      discs->disc[DISC_CLERIC] = new CDCleric();
      discs->disc[DISC_WRATH] = new CDWrath();
      discs->disc[DISC_AEGIS] = new CDAegis();
      discs->disc[DISC_AFFLICTIONS] = new CDAfflict();
      discs->disc[DISC_CURES] = new CDCures();
      discs->disc[DISC_HAND_OF_GOD] = new CDHand();
      discs->disc[DISC_BLUNT] = new CDBash();
      discs->disc[DISC_FAITH] = new CDFaith();
      discs->disc[DISC_THEOLOGY] = new CDTheology();
    }
    getDiscipline(DISC_CLERIC)->ok_for_class |= CLASS_CLERIC;
    getDiscipline(DISC_WRATH)->ok_for_class |= CLASS_CLERIC;
    getDiscipline(DISC_AEGIS)->ok_for_class |= CLASS_CLERIC;
    getDiscipline(DISC_AFFLICTIONS)->ok_for_class |= CLASS_CLERIC;
    getDiscipline(DISC_CURES)->ok_for_class |= CLASS_CLERIC;
    getDiscipline(DISC_HAND_OF_GOD)->ok_for_class |= CLASS_CLERIC;
    getDiscipline(DISC_BLUNT)->ok_for_class |= CLASS_CLERIC;
    getDiscipline(DISC_FAITH)->ok_for_class |= CLASS_CLERIC;
    getDiscipline(DISC_THEOLOGY)->ok_for_class |= CLASS_CLERIC;
  }

  if (hasClass(CLASS_WARRIOR)) {
    if (!isPc()) {
      discs->disc[DISC_WARRIOR] = new CDWarrior();
      discs->disc[DISC_HTH] = new CDHTH();
      discs->disc[DISC_BRAWLING] = new CDBrawling();
      discs->disc[DISC_PHYSICAL] = new CDPhysical();
      discs->disc[DISC_SMYTHE] = new CDSmythe();
      discs->disc[DISC_BLUNT] = new CDBash();
      discs->disc[DISC_SLASH] = new CDSlash();
      discs->disc[DISC_PIERCE] = new CDPierce();
    }
    getDiscipline(DISC_WARRIOR)->ok_for_class |= CLASS_WARRIOR;
    getDiscipline(DISC_HTH)->ok_for_class |= CLASS_WARRIOR;
    getDiscipline(DISC_BRAWLING)->ok_for_class |= CLASS_WARRIOR;
    getDiscipline(DISC_PHYSICAL)->ok_for_class |= CLASS_WARRIOR;
    getDiscipline(DISC_SMYTHE)->ok_for_class |= CLASS_WARRIOR;
    getDiscipline(DISC_BLUNT)->ok_for_class |= CLASS_WARRIOR;
    getDiscipline(DISC_SLASH)->ok_for_class |= CLASS_WARRIOR;
    getDiscipline(DISC_PIERCE)->ok_for_class |= CLASS_WARRIOR;
  }

  if (hasClass(CLASS_RANGER)) {
    if (!isPc()) {
      discs->disc[DISC_RANGER] = new CDRanger();
      discs->disc[DISC_RANGER_FIGHT] = new CDRangerFight();
      discs->disc[DISC_ANIMAL] = new CDAnimal();
      discs->disc[DISC_PLANTS] = new CDPlants();
      discs->disc[DISC_SURVIVAL] = new CDSurvival();
      discs->disc[DISC_SLASH] = new CDSlash();
      discs->disc[DISC_RANGED] = new CDRanged();
      discs->disc[DISC_PIERCE] = new CDPierce();
    }
    getDiscipline(DISC_RANGER)->ok_for_class |= CLASS_RANGER;
    getDiscipline(DISC_RANGER_FIGHT)->ok_for_class |= CLASS_RANGER;
    getDiscipline(DISC_ANIMAL)->ok_for_class |= CLASS_RANGER;
    getDiscipline(DISC_PLANTS)->ok_for_class |= CLASS_RANGER;
    getDiscipline(DISC_SURVIVAL)->ok_for_class |= CLASS_RANGER;
    getDiscipline(DISC_SLASH)->ok_for_class |= CLASS_RANGER;
    getDiscipline(DISC_RANGED)->ok_for_class |= CLASS_RANGER;
    getDiscipline(DISC_PIERCE)->ok_for_class |= CLASS_RANGER;
  }

  if (hasClass(CLASS_DEIKHAN)) {
    if (!isPc()) {
      discs->disc[DISC_DEIKHAN] = new CDDeikhan();
      discs->disc[DISC_DEIKHAN_FIGHT] = new CDDeikhanFight();
      discs->disc[DISC_MOUNTED] = new CDMounted();
      discs->disc[DISC_DEIKHAN_AEGIS] = new CDDeikhanAegis();
      discs->disc[DISC_DEIKHAN_CURES] = new CDDeikhanCures();
      discs->disc[DISC_DEIKHAN_WRATH] = new CDDeikhanWrath();
      discs->disc[DISC_FAITH] = new CDFaith();
      discs->disc[DISC_THEOLOGY] = new CDTheology();
      discs->disc[DISC_BLUNT] = new CDBash();
      discs->disc[DISC_SLASH] = new CDSlash();
    }
    getDiscipline(DISC_DEIKHAN)->ok_for_class |= CLASS_DEIKHAN;
    getDiscipline(DISC_DEIKHAN_FIGHT)->ok_for_class |= CLASS_DEIKHAN;
    getDiscipline(DISC_MOUNTED)->ok_for_class |= CLASS_DEIKHAN;
    getDiscipline(DISC_DEIKHAN_AEGIS)->ok_for_class |= CLASS_DEIKHAN;
    getDiscipline(DISC_DEIKHAN_CURES)->ok_for_class |= CLASS_DEIKHAN;
    getDiscipline(DISC_DEIKHAN_WRATH)->ok_for_class |= CLASS_DEIKHAN;
    getDiscipline(DISC_FAITH)->ok_for_class |= CLASS_DEIKHAN;
    getDiscipline(DISC_THEOLOGY)->ok_for_class |= CLASS_DEIKHAN;
    getDiscipline(DISC_BLUNT)->ok_for_class |= CLASS_DEIKHAN;
    getDiscipline(DISC_SLASH)->ok_for_class |= CLASS_DEIKHAN;
  }

  if (hasClass(CLASS_MONK)) {
    if (!isPc()) {
      discs->disc[DISC_MONK] = new CDMonk();
      discs->disc[DISC_MEDITATION_MONK] = new CDMeditationMonk();
      discs->disc[DISC_LEVERAGE] = new CDLeverage();
      discs->disc[DISC_MINDBODY] = new CDMindBody();
      discs->disc[DISC_FOCUSED_ATTACKS] = new CDFAttacks();
      discs->disc[DISC_BAREHAND] = new CDBarehand();
    }
    getDiscipline(DISC_MONK)->ok_for_class |= CLASS_MONK;
    getDiscipline(DISC_MEDITATION_MONK)->ok_for_class |= CLASS_MONK;
    getDiscipline(DISC_LEVERAGE)->ok_for_class |= CLASS_MONK;
    getDiscipline(DISC_MINDBODY)->ok_for_class |= CLASS_MONK;
    getDiscipline(DISC_FOCUSED_ATTACKS)->ok_for_class |= CLASS_MONK;
    getDiscipline(DISC_BAREHAND)->ok_for_class |= CLASS_MONK;
  }

  if (hasClass(CLASS_THIEF)) {
    if (!isPc()) {

      discs->disc[DISC_THIEF] = new CDThief();
      discs->disc[DISC_THIEF_FIGHT] = new CDThiefFight();
      discs->disc[DISC_MURDER] = new CDMurder();
      discs->disc[DISC_LOOTING] = new CDLooting();
      discs->disc[DISC_POISONS] = new CDPoisons();
      discs->disc[DISC_STEALTH] = new CDStealth();
      discs->disc[DISC_TRAPS] = new CDTraps();
      discs->disc[DISC_PIERCE] = new CDPierce();
      discs->disc[DISC_SLASH] = new CDSlash();
    }
    getDiscipline(DISC_THIEF)->ok_for_class |= CLASS_THIEF;
    getDiscipline(DISC_THIEF_FIGHT)->ok_for_class |= CLASS_THIEF;
    getDiscipline(DISC_MURDER)->ok_for_class |= CLASS_THIEF;
    getDiscipline(DISC_LOOTING)->ok_for_class |= CLASS_THIEF;
    getDiscipline(DISC_POISONS)->ok_for_class |= CLASS_THIEF;
    getDiscipline(DISC_STEALTH)->ok_for_class |= CLASS_THIEF;
    getDiscipline(DISC_TRAPS)->ok_for_class |= CLASS_THIEF;
    getDiscipline(DISC_PIERCE)->ok_for_class |= CLASS_THIEF;
    getDiscipline(DISC_SLASH)->ok_for_class |= CLASS_THIEF;
  }

  if (hasClass(CLASS_SHAMAN)) {
    if (!isPc()) {
      discs->disc[DISC_SHAMAN] = new CDShaman();
      discs->disc[DISC_SHAMAN_FIGHT] = new CDShamanFight();
      discs->disc[DISC_SHAMAN_ALCHEMY] = new CDShamanAlchemy();
      discs->disc[DISC_SHAMAN_HEALING] = new CDShamanHealing();
      discs->disc[DISC_UNDEAD] = new CDUndead();
      discs->disc[DISC_NATURE] = new CDNature();
      discs->disc[DISC_DRAINING] = new CDDraining();
      discs->disc[DISC_TOTEM] = new CDTotem();
      discs->disc[DISC_BLUNT] = new CDBash();
      discs->disc[DISC_PIERCE] = new CDPierce();
      discs->disc[DISC_WIZARDRY] = new CDWizardry();
      discs->disc[DISC_LORE] = new CDLore();
    }
    getDiscipline(DISC_SHAMAN)->ok_for_class |= CLASS_SHAMAN;
    getDiscipline(DISC_SHAMAN_FIGHT)->ok_for_class |= CLASS_SHAMAN;
    getDiscipline(DISC_SHAMAN_ALCHEMY)->ok_for_class |= CLASS_SHAMAN;
    getDiscipline(DISC_SHAMAN_HEALING)->ok_for_class |= CLASS_SHAMAN;
    getDiscipline(DISC_UNDEAD)->ok_for_class |= CLASS_SHAMAN;
    getDiscipline(DISC_NATURE)->ok_for_class |= CLASS_SHAMAN;
    getDiscipline(DISC_DRAINING)->ok_for_class |= CLASS_SHAMAN;
    getDiscipline(DISC_TOTEM)->ok_for_class |= CLASS_SHAMAN;
    getDiscipline(DISC_BLUNT)->ok_for_class |= CLASS_SHAMAN;
    getDiscipline(DISC_PIERCE)->ok_for_class |= CLASS_SHAMAN;
    getDiscipline(DISC_WIZARDRY)->ok_for_class |= CLASS_SHAMAN;
    getDiscipline(DISC_LORE)->ok_for_class |= CLASS_SHAMAN;
  }
  // fix the mobs learnednesses
  if (isPc())
    return;

  // assign mobs skills
  assignSkillsClass();

  mud_assert(discs != NULL, "assignDisc(): discs was null after new (2)");
}

void TBeing::assignSkillsClass()
{
  int i, found = FALSE;
  sh_int pracs;
  classIndT Class;
  int value, num;
  CDiscipline *cd;

  mud_assert(discs != NULL, "Somehow got to assignSkillsClass without a discs %s", getName());

  // first, lets assign some "free" discs
  // keep track of how much free stuff we gave out though
  unsigned int freebies = 0;
  if ((cd = getDiscipline(DISC_ADVENTURING))) {
    value = min((3*(GetMaxLevel())), 100);
    cd->setNatLearnedness(value);
    cd->setLearnedness(value);
    freebies += value/3;
  }
  if (hasClass(CLASS_MAGE) || hasClass(CLASS_SHAMAN)) {
    if ((cd = getDiscipline(DISC_WIZARDRY))) {
      value = min((3*(GetMaxLevel())), 100);
      cd->setNatLearnedness(value);
      cd->setLearnedness(value);
      freebies += value/3;
    }
    if ((cd = getDiscipline(DISC_LORE))) {
      value = min((3*(GetMaxLevel())), 100);
      cd->setNatLearnedness(value);
      cd->setLearnedness(value);
      freebies += value/3;
    }
  } else if (hasClass(CLASS_CLERIC) || hasClass(CLASS_DEIKHAN)) {
    if ((cd = getDiscipline(DISC_THEOLOGY))) {
      value = min((3*(GetMaxLevel())), 100);
      cd->setNatLearnedness(value);
      cd->setLearnedness(value);
      freebies += value/3;
    }
  }
  for (Class = MIN_CLASS_IND; Class < MAX_CLASSES; Class++) {
    pracs = 0;
    if (!hasClass(1<<Class))
      continue;
    // a new char is started at level 0 and advanced 1 level automatically
    for (i = 1; i <= getLevel(Class); i++) {
      // adjust for basic/ vs spelcialization practices
      if (pracs >= 250) 
        pracs += calcNewPracs(Class, true);
      else 
        pracs += calcNewPracs(Class, false);
    }
    // but discount for the freebies given above
    // PCs get these for free at gain time, may as well not charge mobs
    //    pracs -= freebies;

    setPracs(max((int) pracs, 0), Class);
  }
   
// 1. Give them some Adventuring and Wizardry

// MAGE CLASS
  if (hasClass(CLASS_MAGE)) {
// 2.  Now use practices

    while (getPracs(MAGE_LEVEL_IND) > 0) {
      addPracs(-1, MAGE_LEVEL_IND);
      found = FALSE;

// 3. Get combat (tactics) up to some minimum
      if ((cd = getDiscipline(DISC_COMBAT)) && 
          cd->getLearnedness() < 15) {
        raiseDiscOnce(DISC_COMBAT);
        continue;
// 4.  Check to see if Basic Discipline is Maxxed
      } else if ((cd = getDiscipline(DISC_MAGE)) &&
              cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {

// 5.  If combat has been maxxed, then put everything into basic


        if ((cd = getDiscipline(DISC_COMBAT)) &&
            cd->getLearnedness() >= MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_MAGE);
          continue;
// 6.  If combat is not maxxed, then divide it evenly
        } else {
          if (::number(0,1)) {
            raiseDiscOnce(DISC_MAGE);
            continue;
          } else {
            raiseDiscOnce(DISC_COMBAT);
            continue;
          }
        }
// 7.  Check to make sure that combat is maxxed before moving to spec
      } else if ((cd = getDiscipline(DISC_COMBAT)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
        raiseDiscOnce(DISC_COMBAT);
        continue;
// 8.  SPECIALIZATIONS NEXT.

// 9.   IS there a favored Discipline

//    COSMO MARK-- TO BE CODED (CHANGES TO MOB FILE REQUIRED)

// 10.   Divide the learning into the specialites
      } else {

// 11.   First Favored Disciplines
//    Note that if a disc is maxxed it will drop to the next one
        switch ((::number(1,6))) {
          case 1:
            if ((cd = getDiscipline(DISC_SORCERY)) &&
                cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_SORCERY);
              found = TRUE;
              break;
            }
          case 2:
            if ((cd = getDiscipline(DISC_FIRE)) &&
                cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_FIRE);
              found = TRUE;
              break;
            }
          case 3:
            if ((cd = getDiscipline(DISC_WATER)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_WATER);
              found = TRUE;
              break;
            }
          case 4:
            if ((cd = getDiscipline(DISC_EARTH)) &&
                cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_EARTH);
              found = TRUE;
              break;
            }
          case 5:
            if ((cd = getDiscipline(DISC_AIR)) &&
                cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_AIR);
              found = TRUE;
              break;
            }
          default:
            break;
        }
// 12.  Then remaining disciplines in order.
        num = ::number(0,4);
        if (found) {
          continue;
        } else if ((cd = getDiscipline(DISC_PIERCE)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num <= 0)) {
          raiseDiscOnce(DISC_PIERCE);
        } else if ((cd = getDiscipline(DISC_SPIRIT)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num <= 1)) {
          raiseDiscOnce(DISC_SPIRIT);
        } else if ((cd = getDiscipline(DISC_ALCHEMY)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num <= 2)) {
          raiseDiscOnce(DISC_ALCHEMY);
        } else if ((cd = getDiscipline(DISC_SORCERY)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num <= 3)) {
          raiseDiscOnce(DISC_SORCERY);
        } else if ((cd = getDiscipline(DISC_EARTH)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_EARTH);
        } else if ((cd = getDiscipline(DISC_FIRE)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_FIRE);
        } else if ((cd = getDiscipline(DISC_WATER)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_WATER);
        } else if ((cd = getDiscipline(DISC_AIR)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_AIR);
        } else if ((cd = getDiscipline(DISC_SPIRIT)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_SPIRIT);
        } else if ((cd = getDiscipline(DISC_PIERCE)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_PIERCE);
        } else if ((cd = getDiscipline(DISC_ALCHEMY)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_ALCHEMY);
        } else {
        // what disc is left?
// this logs a lot for high level mobs
//          vlogf(5, "Mob (%s) has maxxed all mage disciplines (assignDisc", getName());
          break;
        }
      }
    }
  }  // class mage

// CLERIC CLASS
  if (hasClass(CLASS_CLERIC)) {

// 2.  Now use practices

    while (getPracs(CLERIC_LEVEL_IND) > 0) {
      addPracs(-1, CLERIC_LEVEL_IND);
      found = FALSE;

// 3. Get combat (tactics) up to some minimum
      if ((cd = getDiscipline(DISC_COMBAT)) &&
           cd->getLearnedness() < 15) {
        raiseDiscOnce(DISC_COMBAT);
        continue;
// 4.  Check to see if Basic Discipline is Maxxed
      } else if ((cd = getDiscipline(DISC_CLERIC)) &&
                  cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {

// 5.  If combat has been maxxed, then put everything into basic
        if ((cd = getDiscipline(DISC_COMBAT)) &&
             cd->getLearnedness() >= MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_CLERIC);
          continue;
// 6.  If combat is not maxxed, then divide it evenly
        } else {
          if (::number(0,1)) {
            raiseDiscOnce(DISC_CLERIC);
            continue;
          } else {
            raiseDiscOnce(DISC_COMBAT);
            continue;
          }
        }
// 7.  Check to make sure that combat is maxxed before moving to spec
      } else if ((cd = getDiscipline(DISC_COMBAT)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
        raiseDiscOnce(DISC_COMBAT);
        continue;
// 8.  SPECIALIZATIONS NEXT.
// 9.   IS there a favored Discipline

//    COSMO MARK-- TO BE CODED (CHANGES TO MOB FILE REQUIRED)

// 10.   Divide the learning into the specialites
      } else {

// 11.   First Favored Disciplines
//    Note that if a disc is maxxed it will drop to the next one
        switch ((::number(1, 5))) {
          case 1:
            if ((cd = getDiscipline(DISC_CURES)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_CURES);
              found = TRUE;
              break;
            }
          case 2:
            if ((cd = getDiscipline(DISC_WRATH)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_WRATH);
              found = TRUE;
              break;
            }
          case 3:
            if ((cd = getDiscipline(DISC_AFFLICTIONS)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_AFFLICTIONS);
              found = TRUE;
              break;
            }

          default:
            break;
        }

// 12.  Then remaining disciplines in order.
        num = ::number(0,5);
        if (found) {
          continue;
        } else if ((cd = getDiscipline(DISC_BLUNT)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num <= 0)) {
          raiseDiscOnce(DISC_BLUNT);
        } else if ((cd = getDiscipline(DISC_AEGIS)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num <= 2)) {
          raiseDiscOnce(DISC_AEGIS);
        } else if ((cd = getDiscipline(DISC_HAND_OF_GOD)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num <= 4)) {
          raiseDiscOnce(DISC_HAND_OF_GOD);

        } else if ((cd = getDiscipline(DISC_WRATH)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_WRATH);
        } else if ((cd = getDiscipline(DISC_AFFLICTIONS)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_AFFLICTIONS);
        } else if ((cd = getDiscipline(DISC_CURES)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_CURES);
        } else if ((cd = getDiscipline(DISC_BLUNT)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_BLUNT);

        } else if ((cd = getDiscipline(DISC_HAND_OF_GOD)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_HAND_OF_GOD);
        } else if ((cd = getDiscipline(DISC_AEGIS)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_AEGIS);
        } else {
        // what disc is left?
// this logs a lot for high level mobs
//          vlogf(5, "Mob (%s) has maxxed all cleric disciplines (assignDisc", getName());
          break;
        }
      }
    }
  }  // class cleric 

// WARRIOR CLASS

  if (hasClass(CLASS_WARRIOR)) {

// 2.  Now use practices

    while (getPracs(WARRIOR_LEVEL_IND) > 0) {
      addPracs(-1, WARRIOR_LEVEL_IND);
      found = FALSE;
 
// 3. Get combat (tactics) up to some minimum
      if ((cd = getDiscipline(DISC_COMBAT)) &&
           cd->getLearnedness() < 15) {

        raiseDiscOnce(DISC_COMBAT);
        continue;
// 4.  Check to see if Basic Discipline is Maxxed    
      } else if ((cd = getDiscipline(DISC_WARRIOR)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {

// 5.  If combat has been maxxed, then put everything into basic

        if ((cd = getDiscipline(DISC_COMBAT)) &&
             cd->getLearnedness() >= MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_WARRIOR);
          continue; 
// 6.  If combat is not maxxed, then divide it evenly 
        } else {
          if (::number(0,1)) {
            raiseDiscOnce(DISC_WARRIOR);
            continue;
          } else {
            raiseDiscOnce(DISC_COMBAT);
            continue;
          }
        }  
// 7.  Check to make sure that combat is maxxed before moving to spec 
      } else if ((cd = getDiscipline(DISC_COMBAT)) &&
                  cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
        raiseDiscOnce(DISC_COMBAT);
        continue;
// 8.  SPECIALIZATIONS NEXT.

// 9.   IS there a favored Discipline 

//    COSMO MARK-- TO BE CODED (CHANGES TO MOB FILE REQUIRED) 

// 10.   Divide the learning into the specialites 
      } else {

// 11.   First Favored Disciplines
//    Note that if a disc is maxxed it will drop to the next one
        switch ((::number(1,4))) {
          case 1:
            if ((cd = getDiscipline(DISC_BRAWLING)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {   
              raiseDiscOnce(DISC_BRAWLING);
              found = TRUE;
              break;          
            }
          case 2:
            if ((cd = getDiscipline(DISC_HTH)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_HTH);
              found = TRUE;
              break;
            }
          case 3:
            if ((cd = getDiscipline(DISC_PHYSICAL)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_PHYSICAL);
              found = TRUE;
              break;
            }
          default:
            break;
        } 

// 12.  Then remaining disciplines in order.
        num = ::number(0,6);
        if (found) {
          continue;  
        } else if ((cd = getDiscipline(DISC_SLASH)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num <= 0)) {
          raiseDiscOnce(DISC_SLASH);
        } else if ((cd = getDiscipline(DISC_BLUNT)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num <= 1)) {
          raiseDiscOnce(DISC_BLUNT);
        } else if ((cd = getDiscipline(DISC_PIERCE)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num <= 2)) {
          raiseDiscOnce(DISC_PIERCE);
        } else if ((cd = getDiscipline(DISC_SMYTHE)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num <=3)) {
          raiseDiscOnce(DISC_SMYTHE);

        } else if ((cd = getDiscipline(DISC_BRAWLING)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_BRAWLING);
        } else if ((cd = getDiscipline(DISC_HTH)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_HTH);
        } else if ((cd = getDiscipline(DISC_PHYSICAL)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_PHYSICAL);
        } else if ((cd = getDiscipline(DISC_SLASH)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_SLASH);
        } else if ((cd = getDiscipline(DISC_BLUNT)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_BLUNT);
        } else if ((cd = getDiscipline(DISC_PIERCE)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_PIERCE);
        } else if ((cd = getDiscipline(DISC_SMYTHE)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_SMYTHE);
        } else {
        // what disc is left?
// this logs a lot for high level mobs
//          vlogf(5, "Mob (%s) has maxxed all warrior disciplines (assignDisc", getName()); 
          break;
        }
      }
    }
  }  // class warrior

// THIEF CLASS

  if (hasClass(CLASS_THIEF)) {

// 2.  Now use practices

    while (getPracs(THIEF_LEVEL_IND) > 0) {
      addPracs(-1, THIEF_LEVEL_IND);
      found = FALSE;

// 3. Get combat (tactics) up to some minimum
      if ((cd = getDiscipline(DISC_COMBAT)) &&
          cd->getLearnedness() < 15) {

        raiseDiscOnce(DISC_COMBAT);
        continue;
// 4.  Check to see if Basic Discipline is Maxxed
      } else if ((cd = getDiscipline(DISC_THIEF)) &&
                  cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {

// 5.  If combat has been maxxed, then put everything into basic


        if ((cd = getDiscipline(DISC_COMBAT)) &&
             cd->getLearnedness() == MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_THIEF);
          continue;
// 6.  If combat is not maxxed, then divide it evenly
        } else {
          if (::number(0,1)) {
            raiseDiscOnce(DISC_THIEF);
            continue;
          } else {
            raiseDiscOnce(DISC_COMBAT);
            continue;
          }
        }
// 7.  Check to make sure that combat is maxxed before moving to spec
      } else if ((cd = getDiscipline(DISC_COMBAT)) &&
                  cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
        raiseDiscOnce(DISC_COMBAT);
        continue;
// 8.  SPECIALIZATIONS NEXT.

// 9.   IS there a favored Discipline

//    COSMO MARK-- TO BE CODED (CHANGES TO MOB FILE REQUIRED)

// 10.   Divide the learning into the specialites
      } else {

// 11.   First Favored Disciplines
//    Note that if a disc is maxxed it will drop to the next one
        switch ((::number(1,4))) {
          case 1:
            if ((cd = getDiscipline(DISC_MURDER)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_MURDER);
              found = TRUE;
              break;
            }
          case 2:
            if ((cd = getDiscipline(DISC_THIEF_FIGHT)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_THIEF_FIGHT);
              found = TRUE;
              break;
            }
          default:
            break;
        }

// 12.  Then remaining disciplines in order.
        num = ::number(0,3);
        if (found) {
          continue;
        } else if ((cd = getDiscipline(DISC_PIERCE)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num == 1)) {
          raiseDiscOnce(DISC_PIERCE);
        } else if ((cd = getDiscipline(DISC_SLASH)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num == 3)) {
          raiseDiscOnce(DISC_SLASH);
        } else if ((cd = getDiscipline(DISC_MURDER)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_MURDER);
        } else if ((cd = getDiscipline(DISC_THIEF_FIGHT)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_THIEF_FIGHT);
        } else if ((cd = getDiscipline(DISC_PIERCE)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_PIERCE);
        } else if ((cd = getDiscipline(DISC_SLASH)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_SLASH);
        } else if ((cd = getDiscipline(DISC_LOOTING)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_LOOTING);
        } else if ((cd = getDiscipline(DISC_TRAPS)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_TRAPS);
        } else if ((cd = getDiscipline(DISC_POISONS)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_POISONS);
        } else {
        // what disc is left?
// this logs a lot for high level mobs
//          vlogf(5, "Mob (%s) has maxxed all thief disciplines (assignDisc", getName());
          break;
        }
      }
    }
  }  // class thief 


//  CLASS DEIKHAN

  if (hasClass(CLASS_DEIKHAN)) {

// 2.  Now use practices

    while (getPracs(DEIKHAN_LEVEL_IND) > 0) {
      addPracs(-1, DEIKHAN_LEVEL_IND);
      found = FALSE;

// 3. Get combat (tactics) up to some minimum
      if ((cd = getDiscipline(DISC_COMBAT)) &&
           cd->getLearnedness() < 15) {

        raiseDiscOnce(DISC_COMBAT);
        continue;
// 4.  Check to see if Basic Discipline is Maxxed
      } else if ((cd = getDiscipline(DISC_DEIKHAN)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {

// 5.  If combat has been maxxed, then put everything into basic


        if ((cd = getDiscipline(DISC_COMBAT)) &&
             cd->getLearnedness() == MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_DEIKHAN);
          continue;
// 6.  If combat is not maxxed, then divide it evenly
        } else {
          if (::number(0,1)) {
            raiseDiscOnce(DISC_DEIKHAN);
            continue;
          } else {
            raiseDiscOnce(DISC_COMBAT);
            continue;
          }
        }
// 7.  Check to make sure that combat is maxxed before moving to spec
      } else if ((cd = getDiscipline(DISC_COMBAT)) &&
                  cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
        raiseDiscOnce(DISC_COMBAT);
        continue;
// 8.  SPECIALIZATIONS NEXT.

// 9.   IS there a favored Discipline
//    COSMO MARK-- TO BE CODED (CHANGES TO MOB FILE REQUIRED)

// 10.   Divide the learning into the specialites
      } else {

// 11.   First Favored Disciplines
//    Note that if a disc is maxxed it will drop to the next one
        switch ((::number(1,6))) {
          case 1:
            if ((cd = getDiscipline(DISC_DEIKHAN_FIGHT)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_DEIKHAN_FIGHT);
              found = TRUE;
              break;
            }
          case 2:
            if ((cd = getDiscipline(DISC_MOUNTED)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_MOUNTED);
              found = TRUE;
              break;
            }
          case 3:
            if ((cd = getDiscipline(DISC_DEIKHAN_WRATH)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_DEIKHAN_WRATH);
              found = TRUE;
              break;
            }
          default:
            break;
        }

// 12.  Then remaining disciplines in order.
        num = ::number(0,5);
        if (found) {
          continue;
        } else if ((cd = getDiscipline(DISC_SLASH)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num == 1)) {
          raiseDiscOnce(DISC_SLASH);
        } else if ((cd = getDiscipline(DISC_DEIKHAN_CURES)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num == 2)) {
          raiseDiscOnce(DISC_DEIKHAN_CURES);
        } else if ((cd = getDiscipline(DISC_DEIKHAN_AEGIS)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num == 3)) {
          raiseDiscOnce(DISC_DEIKHAN_AEGIS);
        } else if ((cd = getDiscipline(DISC_DEIKHAN_FIGHT)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_DEIKHAN_FIGHT);
        } else if ((cd = getDiscipline(DISC_SLASH)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_SLASH);
        } else if ((cd = getDiscipline(DISC_DEIKHAN_WRATH)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_DEIKHAN_WRATH);
        } else if ((cd = getDiscipline(DISC_MOUNTED)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_MOUNTED);
        } else if ((cd = getDiscipline(DISC_DEIKHAN_CURES)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_DEIKHAN_CURES);
        } else if ((cd = getDiscipline(DISC_DEIKHAN_CURES)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_DEIKHAN_CURES);
        } else if ((cd = getDiscipline(DISC_BLUNT)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_BLUNT);
        } else {
        // what disc is left?
// this logs a lot for high level mobs
//          vlogf(5, "Mob (%s) has maxxed all deikhan disciplines (assignDisc", getName());
          break;
        }
      }
    }
  }  // class deikhan 


// MONK CLASS

  if (hasClass(CLASS_MONK)) {

// 2.  Now use practices

    while (getPracs(MONK_LEVEL_IND) > 0) {
      addPracs(-1, MONK_LEVEL_IND);
      found = FALSE;

// 3. Get combat (tactics) up to some minimum
      if ((cd = getDiscipline(DISC_COMBAT)) &&
           cd->getLearnedness() < 15) {

        raiseDiscOnce(DISC_COMBAT);
        continue;
// 4.  Check to see if Basic Discipline is Maxxed
      } else if ((cd = getDiscipline(DISC_MONK)) &&
                  cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {

// 5.  If combat has been maxxed, then put everything into basic


        if ((cd = getDiscipline(DISC_COMBAT)) &&
             cd->getLearnedness() == MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_MONK);
          continue;
// 6.  If combat is not maxxed, then divide it evenly
        } else {
          if (::number(0,1)) {
            raiseDiscOnce(DISC_MONK);
            continue;
          } else {
            raiseDiscOnce(DISC_COMBAT);
            continue;
          }
        }
// 7.  Check to make sure that combat is maxxed before moving to spec
      } else if ((cd = getDiscipline(DISC_COMBAT)) &&
                  cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
        raiseDiscOnce(DISC_COMBAT);
        continue;
// 8.  SPECIALIZATIONS NEXT.

// 9.   IS there a favored Discipline

//    COSMO MARK-- TO BE CODED (CHANGES TO MOB FILE REQUIRED)

// 10.   Divide the learning into the specialites
      } else {

// 11.   First Favored Disciplines
//    Note that if a disc is maxxed it will drop to the next one
        switch ((::number(1,6))) {
          case 1:
            if ((cd = getDiscipline(DISC_LEVERAGE)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_LEVERAGE);
              found = TRUE;
              break;
            }
          case 2:
            if ((cd = getDiscipline(DISC_MEDITATION_MONK)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_MEDITATION_MONK);
              found = TRUE;
              break;
            }
          case 3:
            if ((cd = getDiscipline(DISC_MINDBODY)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_MINDBODY);
              found = TRUE;
              break;
            }
  	  case 4:
	    if ((cd = getDiscipline(DISC_FOCUSED_ATTACKS)) &&
		cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
	      raiseDiscOnce(DISC_FOCUSED_ATTACKS);
	      found = TRUE;
	      break;
	    }
          default:
            break;
        }

// 12.  Then remaining disciplines in order.
        num = ::number(0,6);
        if (found) {
          continue;
        } else if ((cd = getDiscipline(DISC_BAREHAND)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num == 1)) {
          raiseDiscOnce(DISC_BAREHAND);
        } else if ((cd = getDiscipline(DISC_BAREHAND)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_BAREHAND);
        } else {
        // what disc is left?
// this logs a lot for high level mobs
//          vlogf(5, "Mob (%s) has maxxed all monk disciplines (assignDisc", getName());
          break;
        }
      }
    }
  }  // class monk 

// RANGER CLASS

  if (hasClass(CLASS_RANGER)) {

// 2.  Now use practices

    while (getPracs(RANGER_LEVEL_IND) > 0) {
      addPracs(-1, RANGER_LEVEL_IND);
      found = FALSE;

// 3. Get combat (tactics) up to some minimum
      if ((cd = getDiscipline(DISC_COMBAT)) &&
          cd->getLearnedness() < 15) {

        raiseDiscOnce(DISC_COMBAT);
        continue;
// 4.  Check to see if Basic Discipline is Maxxed
      } else if ((cd = getDiscipline(DISC_RANGER)) &&
                  cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {

// 5.  If combat has been maxxed, then put everything into basic


        if ((cd = getDiscipline(DISC_COMBAT)) &&
             cd->getLearnedness() == MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_RANGER);
          continue;
// 6.  If combat is not maxxed, then divide it evenly
        } else {
          if (::number(0,1)) {
            raiseDiscOnce(DISC_RANGER);
            continue;
          } else {
            raiseDiscOnce(DISC_COMBAT);
            continue;
          }
        }
// 7.  Check to make sure that combat is maxxed before moving to spec
      } else if ((cd = getDiscipline(DISC_COMBAT)) &&
                  cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
        raiseDiscOnce(DISC_COMBAT);
        continue;
// 8.  SPECIALIZATIONS NEXT.

// 9.   IS there a favored Discipline

//    COSMO MARK-- TO BE CODED (CHANGES TO MOB FILE REQUIRED)

// 10.   Divide the learning into the specialites
      } else {

// 11.   First Favored Disciplines
//    Note that if a disc is maxxed it will drop to the next one
        switch ((::number(1,3))) {
          case 1:
            if ((cd = getDiscipline(DISC_RANGER_FIGHT)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_RANGER_FIGHT);
              found = TRUE;
              break;
            }
            break;
          default:
            break;
        }

// 12.  Then remaining disciplines in order.
        num = ::number(0,6);
        if (found) {
          continue;
        } else if ((cd = getDiscipline(DISC_SLASH)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num <= 3)) {
          raiseDiscOnce(DISC_SLASH);
        } else if ((cd = getDiscipline(DISC_RANGED)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num == 2)) {
          raiseDiscOnce(DISC_RANGED);
        } else if ((cd = getDiscipline(DISC_ANIMAL)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num == 4)) {
          raiseDiscOnce(DISC_ANIMAL);
        } else if ((cd = getDiscipline(DISC_PLANTS)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num == 5)) {
          raiseDiscOnce(DISC_PLANTS);
        } else if ((cd = getDiscipline(DISC_RANGER_FIGHT)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_RANGER_FIGHT);
        } else if ((cd = getDiscipline(DISC_SLASH)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_SLASH);
        } else if ((cd = getDiscipline(DISC_RANGED)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_RANGED);

        } else if ((cd = getDiscipline(DISC_ANIMAL)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_ANIMAL);
        } else if ((cd = getDiscipline(DISC_PLANTS)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_PLANTS);
        } else if ((cd = getDiscipline(DISC_PIERCE)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_PIERCE);
        } else if ((cd = getDiscipline(DISC_SURVIVAL)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_SURVIVAL);
        } else {
        // what disc is left?
// this logs a lot for high level mobs
//          vlogf(5, "Mob (%s) has maxxed all ranger disciplines (assignDisc", getName());
          break;
        }
      }
    }
  }  // class ranger 

// SHAMAN CLASS

  if (hasClass(CLASS_SHAMAN)) {

// 2.  Now use practices

    while (getPracs(SHAMAN_LEVEL_IND) > 0) {
      addPracs(-1, SHAMAN_LEVEL_IND);
      found = FALSE;

// 3. Get combat (tactics) up to some minimum
      if ((cd = getDiscipline(DISC_COMBAT)) &&
           cd->getLearnedness() < 15) {

        raiseDiscOnce(DISC_COMBAT);
        continue;
// 4.  Check to see if Basic Discipline is Maxxed
      } else if ((cd = getDiscipline(DISC_SHAMAN)) &&
                  cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {

// 5.  If combat has been maxxed, then put everything into basic


        if ((cd = getDiscipline(DISC_COMBAT)) &&
             cd->getLearnedness() == MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_SHAMAN);
          continue;
// 6.  If combat is not maxxed, then divide it evenly
        } else {
          if (::number(0,1)) {
            raiseDiscOnce(DISC_SHAMAN);
            continue;
          } else {
            raiseDiscOnce(DISC_COMBAT);
            continue;
          }
        }
// 7.  Check to make sure that combat is maxxed before moving to spec
      } else if ((cd = getDiscipline(DISC_COMBAT)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
        raiseDiscOnce(DISC_COMBAT);
        continue;
// 8.  SPECIALIZATIONS NEXT.

// 9.   IS there a favored Discipline

//    COSMO MARK-- TO BE CODED (CHANGES TO MOB FILE REQUIRED)

// 10.   Divide the learning into the specialites
      } else {

// 11.   First Favored Disciplines
//    Note that if a disc is maxxed it will drop to the next one
        switch ((::number(1,6))) {
          case 1:
            if ((cd = getDiscipline(DISC_DRAINING)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_DRAINING);
              found = TRUE;
              break;
            }
          case 2:
            if ((cd = getDiscipline(DISC_NATURE)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_NATURE);
              found = TRUE;
              break;
            }
          case 3:
            if ((cd = getDiscipline(DISC_SHAMAN_FIGHT)) &&
                 cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_SHAMAN_FIGHT);
              found = TRUE;
              break;
            }
         case 4:
           if ((cd = getDiscipline(DISC_UNDEAD)) &&
                cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
              raiseDiscOnce(DISC_UNDEAD);
              found = TRUE;
              break;
            }
          default:
            break;
        }

// 12.  Then remaining disciplines in order.
        num = ::number(0,4);
        if (found) {
          continue;
        } else if ((cd = getDiscipline(DISC_BLUNT)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num == 1)) {
          raiseDiscOnce(DISC_BLUNT);
        } else if ((cd = getDiscipline(DISC_TOTEM)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num == 2)) {
          raiseDiscOnce(DISC_TOTEM);
        } else if ((cd = getDiscipline(DISC_UNDEAD)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS && (num == 3)) {
          raiseDiscOnce(DISC_UNDEAD);

        } else if ((cd = getDiscipline(DISC_DRAINING)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_DRAINING);
        } else if ((cd = getDiscipline(DISC_SHAMAN_FIGHT)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_SHAMAN_FIGHT);
        } else if ((cd = getDiscipline(DISC_NATURE)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_NATURE);
        } else if ((cd = getDiscipline(DISC_BLUNT)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_BLUNT);
        } else if ((cd = getDiscipline(DISC_TOTEM)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_TOTEM);
        } else if ((cd = getDiscipline(DISC_UNDEAD)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_UNDEAD);
        } else if ((cd = getDiscipline(DISC_PIERCE)) &&
                   cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_PIERCE);
        } else if ((cd = getDiscipline(DISC_SHAMAN_HEALING)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_SHAMAN_HEALING);
        } else if ((cd = getDiscipline(DISC_SHAMAN_ALCHEMY)) &&
                    cd->getLearnedness() < MAX_DISC_LEARNEDNESS) {
          raiseDiscOnce(DISC_SHAMAN_ALCHEMY);
        } else {
        // what disc is left?
// this logs a lot for high level mobs
//          vlogf(5, "Mob (%s) has maxxed all shaman disciplines (assignDisc", getName());
          break;
        }
      }
    }
  }  // class shaman
  affectTotal();
  discNumT disc_num;
  for (disc_num = MIN_DISC; disc_num < MAX_DISCS; disc_num++) {
    if (!(cd = getDiscipline(disc_num))) 
      continue;   
    if (cd->getNatLearnedness() < 0) {
      continue;
    } 
    initSkillsBasedOnDiscLearning(disc_num);
  }
}

void TBeing::initSkillsBasedOnDiscLearning(discNumT disc_num) 
{
  int disc_learn = 0, boost = 0, max_amt = 0, value = 0;
  CDiscipline *cd;

  mud_assert(discs != NULL, "Somehow there was a call to initSkillsBasedOnDiscLearning without a discs %s", getName());

  // mob skills are always maxed for their disc-training
  if (!(cd = getDiscipline(disc_num))) {
  }
  disc_learn = cd->getLearnedness();

  spellNumT i;
  for (i = MIN_SPELL; i < MAX_SKILL; i++) {
    if (hideThisSpell(i))
      continue;
    if (discArray[i]->disc == disc_num) {
      if (!getSkill(i))
        continue;

      if (disc_learn < discArray[i]->start) 
        continue;

      if (isPc()) {
        setNatSkillValue(i, getMaxSkillValue(i));
        setSkillValue(i, getMaxSkillValue(i));
      } else {
        max_amt = (disc_learn - discArray[i]->start + 1) *  discArray[i]->learn;
        max_amt = max(max_amt, 1);
        max_amt = min(max_amt, (int) MAX_SKILL_LEARNEDNESS);
        if (discArray[i]->startLearnDo >= 0) {
          boost = min(max_amt, (int) discArray[i]->startLearnDo);
          if ((5 * discArray[i]->learn) >= MAX_SKILL_LEARNEDNESS) {
             value = 50 + ((disc_learn - discArray[i]->start) * 2);
             min(value, 100);
             value = value * max_amt;
             value /= 100;
             value = min((int) MAX_SKILL_LEARNEDNESS, value);
             value = max(value, 10);
             value = max(boost, value);
          } else {
            if (disc_learn <= MAX_DISC_LEARNEDNESS) {
              value = 75 + (disc_learn - discArray[i]->start);
              min(value, 100);
              value = value * max_amt;
              value /= 100;
              value=min((int) MAX_SKILL_LEARNEDNESS, value);
              value = max(value, 10);
              value = max(boost, value);
            } else {
              value = max_amt;
            }
          }
        } else {
          value = max_amt;
        }
        setNatSkillValue(i, value);
        setSkillValue(i,value);
        if (i ==  SKILL_TACTICS)
          setNatSkillValue(SKILL_TACTICS,min(100, (GetMaxLevel() * 12)));
          setSkillValue(SKILL_TACTICS,min(100, (GetMaxLevel() * 12)));
        if (i == SKILL_RIDE)
          setNatSkillValue(SKILL_RIDE,min(100,5 + GetMaxLevel() * 2));
          setSkillValue(SKILL_RIDE,min(100,5 + GetMaxLevel() * 2)); 
      }
    }
  }
}

const char *skill_diff(byte num)
{
  if (num <= 1)
    return "next practice";
  else if (num <= 3)
    return "very soon";
  else if (num < 8)
    return "soon";
  else if (num < 12)
    return "fairly soon";
  else if (num < 16)
    return "little more training";
  else if (num < 20)
    return "not too long now";
  else if (num < 30)
    return "lot more training";
  else if (num < 40)
    return "in awhile";
  else if (num < 50)
    return "some day";
  else if (num < 60)
    return "in the future";
  else
    return "way in the future";
}

// this should be more exotic taking into effect things like
// wizardry skill, etc

// returns TRUE if this is lower lev then vict
// returns 2 if vict is immortal
// else is FALSE
int TBeing::isNotPowerful(TBeing *vict, int lev, spellNumT skill, silentTypeT silent)
{
  // force success for immorts
  if (isImmortal() && !vict->isImmortal())
    return FALSE;
  if (vict->isImmortal() && !isImmortal()) {
    if (!silent) {
      act("You can't do that to $N.", FALSE, this, 0, vict, TO_CHAR);
      act("$E's a god!", FALSE, this, 0, vict, TO_CHAR);
      act("Nothing seems to happen.", TRUE, this, 0, 0, TO_ROOM);
    }
    return 2;
  }

  int advLearning = 0;
  // adjust lev for stuff here
  // this allows for casting spells over level
// COSMO DISC MARKER - change to add in deikhan and shaman, ranger for casting
  skill = getSkillNum(skill);
  int level = GetMaxLevel();
  CDiscipline *cd;

  lev = level;
  switch (getDisciplineNumber(skill, FALSE)) {
    case DISC_MAGE:
    case DISC_PLANTS:
    case DISC_AIR:
    case DISC_EARTH:
    case DISC_FIRE:
    case DISC_WATER:
    case DISC_SPIRIT:
    case DISC_SORCERY:
    case DISC_ALCHEMY:
    case DISC_NATURE:
    case DISC_ANIMAL:
    case DISC_SURVIVAL:
    case DISC_SHAMAN:
    case DISC_TOTEM:
    case DISC_DRAINING:
    case DISC_UNDEAD:
    case DISC_SHAMAN_HEALING:
      cd = getDiscipline(DISC_WIZARDRY);
      if (cd && cd->getLearnedness() > 0)
        lev += 2 + (cd->getLearnedness() / 20);
      break;
    case DISC_CURES:
    case DISC_AEGIS:
    case DISC_WRATH:
    case DISC_HAND_OF_GOD:
    case DISC_CLERIC:
    case DISC_DEIKHAN:
    case DISC_DEIKHAN_WRATH:
    case DISC_DEIKHAN_CURES:
    case DISC_DEIKHAN_AEGIS:
      cd = getDiscipline(DISC_FAITH);
      if (cd && cd->getLearnedness() > 0)
        lev += 2 + (cd->getLearnedness() / 20);
      break;
    case DISC_AFFLICTIONS:
      // a very special case so big mobs are especially vulnerble
      // basically means clerics rock vs mobs <= 100th level
      cd = getDiscipline(DISC_FAITH);
      if (cd && cd->getLearnedness() > 0)
        lev += 2 + (cd->getLearnedness() / 20);
      break;
    default:
      break;
  }

  if (discArray[skill]->disc != discArray[skill]->assDisc) {
    CDiscipline * assDisc = getDiscipline(discArray[skill]->assDisc);
    if (assDisc) {
      advLearning = getAdvLearning(skill);
      lev += (level * advLearning) / 200;
      advLearning = getAdvDoLearning(skill);
      lev += (level * advLearning) / 200;
    }                      
  }                      

  if (lev < vict->GetMaxLevel()) {
    if (!silent) {
      act("$N is too powerful to be affected.", FALSE, this, 0, vict, TO_CHAR);
      act("Nothing seems to happen.", TRUE, this, 0, 0, TO_ROOM);
    }
    return TRUE;
  }
  return FALSE;
}

int TBeing::getSkillLevel(spellNumT skill) const
{
  discNumT disc_num = getDisciplineNumber(skill, FALSE);
  int lev = 0;

  if (isImmortal()) {
    return GetMaxLevel();
  }

  switch(disc_num) {
    case DISC_CLERIC:
    case DISC_AEGIS:
    case DISC_WRATH:
    case DISC_AFFLICTIONS:
    case DISC_CURES:
    case DISC_HAND_OF_GOD:
      lev = getClassLevel(CLASS_CLERIC);
      break;
    case DISC_MAGE:
    case DISC_SPIRIT:
    case DISC_ALCHEMY:
    case DISC_AIR:
    case DISC_WATER:
    case DISC_FIRE:
    case DISC_EARTH:
    case DISC_SORCERY:
      lev = getClassLevel(CLASS_MAGIC_USER);
      break;
    case DISC_RANGER: 
    case DISC_RANGER_FIGHT:
    case DISC_PLANTS:
    case DISC_ANIMAL:
    case DISC_SURVIVAL:
      lev = getClassLevel(CLASS_RANGER);
      break;
    case DISC_WARRIOR:
    case DISC_HTH:
    case DISC_BRAWLING:
    case DISC_PHYSICAL:
    case DISC_SMYTHE:
      lev = getClassLevel(CLASS_WARRIOR);
      break;
    case DISC_DEIKHAN:
    case DISC_DEIKHAN_FIGHT:
    case DISC_MOUNTED:
    case DISC_DEIKHAN_AEGIS:
    case DISC_DEIKHAN_CURES:
    case DISC_DEIKHAN_WRATH:
      lev = getClassLevel(CLASS_DEIKHAN);
      break;
    case DISC_THIEF:
    case DISC_THIEF_FIGHT:
    case DISC_LOOTING:
    case DISC_MURDER:
    case DISC_POISONS:
    case DISC_STEALTH:
    case DISC_TRAPS:
      lev = getClassLevel(CLASS_THIEF);
      break;
    case DISC_MONK:
    case DISC_MEDITATION_MONK:
    case DISC_LEVERAGE:
    case DISC_MINDBODY:
    case DISC_FOCUSED_ATTACKS:
      lev = getClassLevel(CLASS_MONK);
      break;
    case DISC_SHAMAN:
    case DISC_SHAMAN_FIGHT:
    case DISC_SHAMAN_ALCHEMY:
    case DISC_NATURE:
    case DISC_SHAMAN_HEALING:
    case DISC_UNDEAD:
    case DISC_DRAINING:
    case DISC_TOTEM:
      lev = getClassLevel(CLASS_SHAMAN);
      break;
    case DISC_WIZARDRY:
    case DISC_LORE:
// if multiclass allowed this will work
      if (hasClass(CLASS_MAGIC_USER)) {
        lev = getClassLevel(CLASS_MAGIC_USER);
        break;
      } else {
        lev = getClassLevel(CLASS_SHAMAN);
      }
      break;
    case DISC_THEOLOGY:
    case DISC_FAITH:
      if (hasClass(CLASS_CLERIC)) {
        lev = getClassLevel(CLASS_CLERIC);
        break;
      } else {
        lev = getClassLevel(CLASS_DEIKHAN);
      }
        break;
    case DISC_ADVENTURING:
    case DISC_COMBAT:
    case DISC_SLASH:
    case DISC_BLUNT:
    case DISC_PIERCE:
    case DISC_RANGED:
    case DISC_BAREHAND:
      lev = GetMaxLevel();
      break;
    case MAX_DISCS:
    case DISC_NONE:
    case MAX_SAVED_DISCS:
      vlogf(5, "bad disc (%d, %d) in getSkillLevel (%s).",
               disc_num, skill, getName());
      lev = 0;
      break;
  }
  return lev;
}

byte TBeing::getMaxSkillValue(spellNumT skill) const
{
  int tmp2;
  discNumT dn = getDisciplineNumber(skill, FALSE);
  if (dn == DISC_NONE) {
    vlogf(5, "bad disc for skill %d", skill);
    return SKILL_MIN;
  }
  CDiscipline * cdisc = getDiscipline(dn);

  if (cdisc && discArray[skill] && *discArray[skill]->name) {
    tmp2 = max(0, cdisc->getLearnedness() - discArray[skill]->start + 1);

    if (((!desc || isImmortal()) || (!discArray[skill]->toggle || hasQuestBit(discArray[skill]->toggle))) && tmp2 > 0) {
      if (!desc && discArray[skill]->toggle && (master && master->desc && !master->isImmortal())) {
        return SKILL_MIN;
      } else {
        return min((discArray[skill]->learn * tmp2), (int) MAX_SKILL_LEARNEDNESS);
      }
    }
  }
  return SKILL_MIN;
}

CDiscipline * TBeing::getDiscipline(discNumT n) const
{
  return discs->disc[n];
}

void CS(const TBeing *caster, spellNumT spell)
{
  if ((caster->GetMaxLevel() > MAX_MORT) && caster->desc) {
    discArray[spell]->immCrits++;
  } else if (caster->desc && caster->isPc()) {
    discArray[spell]->crits++;
  } else {
    discArray[spell]->mobCrits++;
  }

}

static void learnAttemptLog(const TBeing *caster, spellNumT spell)
{
  if (caster->desc && caster->isPc()) {
    discArray[spell]->learnAttempts++;
  } 
}

static void learnLearnednessLog(const TBeing *caster, spellNumT spell, int amt)
{
  if (caster->desc && caster->isPc()) {
      discArray[spell]->learnLearn += amt;
  }
}

static void learnSuccessLog(const TBeing *caster, spellNumT spell, int boost)
{
  if (caster->desc && caster->isPc()) {
    discArray[spell]->learnSuccess++;
    discArray[spell]->learnBoost += boost;
  }
}

void CS(spellNumT spell_num)
{
  return;
  // crit success counter
  discArray[spell_num]->crits++;
}

void CF(const TBeing *caster, spellNumT spell)
{
  if ((caster->GetMaxLevel() > MAX_MORT) && caster->desc) {
    discArray[spell]->immCritf++;
  } else if (caster->desc && caster->isPc()) {
    discArray[spell]->critf++;
  } else {
    discArray[spell]->mobCritf++;
  }
}

void CF(spellNumT spell_num)
{
  return;
  // crit fail counter
  discArray[spell_num]->critf++;
}

void SV(const TBeing *caster, spellNumT spell)
{
  if ((caster->GetMaxLevel() > MAX_MORT) && caster->desc) {
    discArray[spell]->immSaves++;
  } else if (caster->desc && caster->isPc()) {
    discArray[spell]->saves++;
  } else {
    discArray[spell]->mobSaves++;
  }
}

void SV(spellNumT spell_num)
{
  // saves counter
  discArray[spell_num]->saves++;
}

void LogDam(const TBeing *caster, spellNumT spell_num, int dam)
{
  // this is used to log damage and number of victims
  // thiere is no need to call this directly as it sits inside reconcileDamage()
  // however, non damagin spells that want to log "damage" (eg, heal) do call it

  // sometimes we call reconcileDamage with dam = 0 just to start a fight
  // ignore such events
  // i'm not sure why we should ignore dam < 0, but it seems right to do.

  if (dam <= 0)
    return;
  if ((caster->GetMaxLevel() > MAX_MORT) && caster->desc) {
    discArray[spell_num]->immDamage += dam;
    discArray[spell_num]->immVictims++;
  } else if (caster->desc && caster->isPc()) {
    discArray[spell_num]->damage += dam;
    discArray[spell_num]->victims++;
  } else {
    discArray[spell_num]->mobDamage += dam;
    discArray[spell_num]->mobVictims++;
  }
}

#if 0
enum logLearnAttemptT {
  LEARN_ATT_ADD,
  LEARN_ATT_REM
};

static void logLearnAttempts(TBeing *caster, spellNumT spell, logLearnAttemptT type, int)
{
  if ((caster->GetMaxLevel() > MAX_MORT) || !caster->isPc() || !caster->desc)
    return;

  switch (type) {
    case LEARN_ATT_ADD:
      discArray[spell]->learnAttempts++;
      discArray[spell]->learnLearn += caster->getSkillValue(spell);
      discArray[spell]->learnLevel += caster->GetMaxLevel(); 
      break;
    case LEARN_ATT_REM:
      discArray[spell]->learnAttempts -= 1;
      discArray[spell]->learnLearn -= caster->getSkillValue(spell);
      discArray[spell]->learnLevel += caster->GetMaxLevel();
      break;
  }

 return;
}

enum logLearnSuccessT {
  LEARN_SUC_NORM,
  LEARN_SUC_DISC,
  LEARN_SUC_ADV
};

static void logLearnSuccess(TBeing *caster, spellNumT spell, logLearnSuccessT type, int boost)
{
  // this is used to log learn success 
  // there is usually no need to call this directly as it sits inside i
  // learnFromDoing and learnFromDoingUnusual

  if (!caster) {
    vlogf(5,"Something went into logLearnSuccess with no caster (%d)", spell);
    return;
  }

  if (caster->GetMaxLevel() > MAX_MORT) {
    return;
  }

  if (!caster->desc) {
    vlogf(5,"Something went into logLearnSuccess with no desc (%d)", spell);
    return;
  }

  switch (type) {
    case LEARN_SUC_NORM:
      discArray[spell]->learnSuccess++;
      discArray[spell]->learnBoost += boost;
      break;
    case LEARN_SUC_DISC:
      discArray[spell]->learnDiscSuccess++;
      break;
    case LEARN_SUC_ADV:
      discArray[spell]->learnAdvDiscSuccess++;
      break;
  }
  return;
}
#endif

int TMonster::learnFromDoingUnusual(learnUnusualTypeT, spellNumT, int)
{
  return FALSE;
}

int TPerson::learnFromDoingUnusual(learnUnusualTypeT type, spellNumT spell, int amt)
{
  spellNumT w_type = spell;
  spellNumT spell2 = TYPE_UNDEFINED;
  int value = 0;

  if (isImmortal() || !desc)
    return FALSE;

// for combat skills/armor wearing adventuring stuff that doesnt use bsuccess
  switch (type) {
    case LEARN_UNUSUAL_PROFICIENCY:   // proficiencies
      if (w_type == TYPE_HIT) {
          spell = SKILL_BAREHAND_PROF; 
	  spell2 = SKILL_BAREHAND_SPEC;
          amt *= 2;
      } else if (slashType(w_type)) {
        spell = SKILL_SLASH_PROF;
        spell2 = SKILL_SLASH_SPEC;
      } else if (bluntType(w_type)) {
        spell = SKILL_BLUNT_PROF;
        spell2 = SKILL_BLUNT_SPEC;
      } else if (pierceType(w_type)) {
        spell = SKILL_PIERCE_PROF;
        spell2 = SKILL_PIERCE_SPEC;
      } else {
        vlogf(5, "Wierd case in learnFromDoingUnusual %s, %d", getName(), w_type);
        return FALSE;
      }
      if (amt && ::number(0,amt)) {
        // arbitrary dont let the skill increase
        if (doesKnowSkill(spell) && 
            (value = getRawNatSkillValue(spell)) < MAX_SKILL_LEARNEDNESS) {
          learnAttemptLog(this, spell);
          learnLearnednessLog(this, spell, value);
          return FALSE;
        }
        if ((spell2 >= 0) && ((value = getRawNatSkillValue(spell2)) < MAX_SKILL_LEARNEDNESS)) {
          learnAttemptLog(this, spell2);
          learnLearnednessLog(this, spell2, value);
        }
        return FALSE;
      } else if (0 && spell == SKILL_BAREHAND_PROF) {
        if (doesKnowSkill(spell))
          return learnFromDoing(spell, SILENT_NO, 0);
        else
          return FALSE;
      } else {
        if (doesKnowSkill(spell) && !(value = learnFromDoing(spell, SILENT_NO, 0))) {
          if ((spell2 >= 0) &&
              (getDiscipline(DISC_COMBAT)->getLearnedness() >= MAX_DISC_LEARNEDNESS) &&
              doesKnowSkill(spell2)) {
            return learnFromDoing(spell2, SILENT_NO, 0);
          } else 
            return FALSE;
        } 
      } 
      break;
    case LEARN_UNUSUAL_NORM_LEARN:
      if (amt && ::number(0, amt)) {
        // arbitrary dont let the skill increase
        if (doesKnowSkill(spell) && (value = getRawNatSkillValue(spell)) < MAX_SKILL_LEARNEDNESS) {
          learnAttemptLog(this, spell);
          learnLearnednessLog(this, spell, value);
          return FALSE;
        }
        return FALSE;
      } else {
        if (doesKnowSkill(spell))
          return learnFromDoing(spell, SILENT_NO, 0);
        else
          return FALSE;
      }
      break;
    case LEARN_UNUSUAL_FORCED_LEARN:
      if (amt && ::number(0, amt)) {
      // arbitrary dont let the skill increase
        if (doesKnowSkill(spell) && (value = getRawNatSkillValue(spell)) < MAX_SKILL_LEARNEDNESS) {
          learnAttemptLog(this, spell);
          learnLearnednessLog(this, spell, value);
           return FALSE;
        }
        return FALSE;
      } else {
        if (doesKnowSkill(spell))
          return learnFromDoing(spell, SILENT_NO, 1);
        else
           return FALSE;
      }
      break;
    case LEARN_UNUSUAL_NONE:
      vlogf(5, "Wierd case in learnFromDoingUnusual %s, type %d spell %d", getName(), type, spell);
      return FALSE;
  }
  return FALSE;
}

// flags 1 = linear/no discipline
int TMonster::learnFromDoing(spellNumT sknum, silentTypeT silent, unsigned int)
{
  return FALSE;
}

// flags |= 1 == forced learn
int TPerson::learnFromDoing(spellNumT sknum, silentTypeT silent, unsigned int flags)
{
  CSkill *sk;
  CDiscipline *assDiscipline, *discipline;
  int discLearn, chance, chanceDisc, chanceAss;

  discLearn = chance = chanceDisc = chanceAss = 0;
  if (isImmortal() || !desc) {
    return FALSE;
  }


  if (!discArray[sknum] || 
      !*discArray[sknum]->name || 
      discArray[sknum]->startLearnDo == -1) {
    return FALSE;
  }
  if (!doesKnowSkill(sknum)) {
    return FALSE;
  }

  sk = getSkill(sknum);
  if (!sk) {
    return FALSE;
  }

  int actual = getRawNatSkillValue(sknum);
  if (actual < getMaxSkillValue(sknum)) {
    learnAttemptLog(this, sknum);
    learnLearnednessLog(this, sknum, actual);
  }
  int boost = 1;

  // this prevents them from gaining further without a minimum wait between
  // increases.
  // since the chance (below) drops off as they approach max, we don't want
  // them to wait too long or they will never gain some skills
  //
  // allow them to boost the skill up to a "usable" amount without problems.
  //

  if (actual <= 15) {
    if ((time(0) - sk->lastUsed) < (1 * SECS_PER_REAL_MIN))  {
      return FALSE;
    }
  } else if ((time(0) - sk->lastUsed) < (5 * SECS_PER_REAL_MIN))  {
    return FALSE;
  }

// DISCIPLINE LEARN BY DOING FIRST- PLAYERS DO *NOT* SEE THIS
// first set learning rates
// discipline and assDiscipline are used later to hold discipline number
// here they are just used to hold if the learning should take place

  if (discArray[sknum]->disc == DISC_COMBAT ||
        discArray[sknum]->disc == DISC_SLASH ||
        discArray[sknum]->disc == DISC_BLUNT ||
        discArray[sknum]->disc == DISC_PIERCE ||
        discArray[sknum]->disc == DISC_RANGED ||
        discArray[sknum]->disc == DISC_BAREHAND) {
    chanceDisc = ::number(0,200);
    chanceAss = ::number(0, 400);  
  } else if (discArray[sknum]->assDisc == discArray[sknum]->disc) {
    // advanced discipline here
    chanceDisc = ::number(0,150);
    chanceAss = 1;  // no chance of learn 
  } else {
    chanceDisc = ::number(0, 200);
    chanceAss = ::number(0,400);
  }

  if ((actual >= MAX_SKILL_LEARNEDNESS) || 
      (actual >= getMaxSkillValue(sknum))) {
    chanceDisc = (max(0, chanceDisc - 25));
    chanceAss = (max(0, chanceAss - 25));
  }


  if (!chanceDisc) {
  //   do skill's disc learning here COSMO MARKER
    if (!(discipline = getDiscipline(discArray[sknum]->disc))) {
#if DISC_DEBUG
      vlogf(-1, "(%s) has a skill (%d) but doesnt have the discipline", getName(), sknum);
#endif
      return FALSE;
    } 
    discLearn = discipline->getDoLearnedness();
    discLearn = max(1, discLearn);
    discLearn = min(100, discLearn + 1);
    if (discLearn < 100) {
      discipline->setDoLearnedness(discLearn);
#if DISC_DEBUG
      vlogf(-1, "%s just learned something in %s, Learn = %d.", getName(), disc_names[(discArray[sknum]->assDisc)], discLearn); 
#endif
    }
  }
  if (!chanceAss) {
    if (!(assDiscipline = getDiscipline(discArray[sknum]->assDisc))) {
#if DISC_DEBUG
      vlogf(-1, "(%s) has a skill (%d) but doesnt have the assDisc", getName(), sknum);
#endif
      return FALSE;
    }
    discLearn = assDiscipline->getDoLearnedness();
    discLearn = max(1, discLearn);
    discLearn = min(100, discLearn + 1);
    if (discLearn < 100) {
      assDiscipline->setDoLearnedness(discLearn);
#if DISC_DEBUG
      vlogf(-1, "%s just learned something in %s, Learn = %d.", getName(), disc_names[(discArray[sknum]->assDisc)], discLearn); 
#endif
    }
  }

// SKILL LEARNING NOW

  if (actual >= getMaxSkillValue(sknum)) {
    return FALSE;
  }

  if (IS_SET(flags, 1U)) {
// learn
  } else {
    const int max_amt = MAX_SKILL_LEARNEDNESS;
    float amount = ((float) max_amt - (float) actual) / ((float) max_amt);
#if DISC_DEBUG
    vlogf(-1, "learnFromDoing (%s) amt(%f) max(%d) actual(%d)", discArray[sknum]->name, amount, max_amt, actual);
#endif

  // some basic background on how this was formulated.
  // let y = f(amount) = the percentage chance we raise the skill 1%
  // given that amount is calculated as above, it goes from 0.0 to 1.0
  // ie. amount = 0 (skill at current max), amount = 1.0 (skill at 0).
  // we desire f(1.0) = 100% and f(0.0) = 0%
  // additionally, to have f(amount) to have the form amount ^ power
  // seems to make sense.
  // thus
  // y = A x ^ B + C     : amount = x
  // C = 0 from f(0.0) = 0
  // A = 100% from f(1.0)
  // y = 100% * amount ^ (B)
  // we desire B to be in range 1.0 (high wis) to 3.5 (low wis) 
  // solving for a linear formula, gave slope of (-1/60) and intersect of 4
    float power;
    power = 4.0 - ( plotStat(STAT_NATURAL, STAT_WIS, 0.5, 3.0, 1.75, 1.0));
    chance = (int) (1000.0 * (pow(amount, power)));

    // make a minimum chance of increase.
    if (amount > 0.0)
      chance = max(10, chance);


    if (::number(0, 999) >= chance)
      return FALSE;
  }

#if 1
  if (!silent) {
    char tString[256];

    if ((discArray[sknum]->comp_types & COMP_MATERIAL))
      strcpy(tString, "feel you have more control over the powers of");
    else if (discArray[sknum]->holyStrength) {
      string tStDeity("");

      tStDeity = yourDeity(sknum, FIRST_PERSON);
      sprintf(tString, "feel %s favoring you more in respects to",
              tStDeity.c_str());
    } else
      strcpy(tString, "feel your skills honing in regards to");

    sendTo(COLOR_BASIC, "<c>You %s %s.<z>\n\r", tString, discArray[sknum]->name);
  }
#else
  if (!silent)
    sendTo(COLOR_BASIC, "<c>You increase your mastery of %s.<z>\n\r", discArray[sknum]->name);
#endif

  // boost at this point is 1, now make it more it if appropriate
  if (discArray[sknum]->amtLearnDo > 1) {
    boost = discArray[sknum]->amtLearnDo;
    if (actual >= 90) 
      boost = 1;
    else if ((actual + boost) >= 90) 
      boost = 90 - actual;
  }
#if DISC_DEBUG
  vlogf(-1, "learnFromDoing (%s)(%d): actual (%d), boost (%d)", discArray[sknum]->name, sknum, actual, boost);
#endif
  setSkillValue(sknum, getSkillValue(sknum) + boost);
  setNatSkillValue(sknum, actual + boost);
  
  sk->lastUsed = time(0);
  learnSuccessLog(this, sknum, boost);

  if(hasClass(CLASS_MONK) && sknum == SKILL_KICK_MONK &&
     getSkillValue(sknum) == 100){
    setQuestBit(TOG_ELIGIBLE_ADVANCED_KICKING);

    act("<c>You feel that you have <g>mastered<c> the skill of <p>kicking<c>.<1>\n<c>Perhaps your guildmaster could help you with <p>advanced kicking<c>.<1>",
	FALSE, this, NULL, NULL, TO_CHAR);
  }

  return TRUE;
}

void TBeing::addSkillLag(spellNumT skill)
{
  lag_t lag_num = discArray[skill]->lag;
  float f_lag = lagAdjust(lag_num);
  f_lag *= combatRound(1);
  int i_lag = static_cast<int>(f_lag);

  // we wind up truncating, might want to randomize this based on remainder
  addToWait(i_lag);
}

CMasterDiscipline::CMasterDiscipline()
{
  for (discNumT i = MIN_DISC; i < MAX_DISCS; i++)
    disc[i] = NULL;
}

CMasterDiscipline::CMasterDiscipline(const CMasterDiscipline &a)
{
  for (discNumT i = MIN_DISC; i < MAX_DISCS; i++) {
    if (a.disc[i])
      disc[i] = a.disc[i]->cloneMe();
    else
      disc[i] = NULL;
  }
}

CMasterDiscipline & CMasterDiscipline::operator=(const CMasterDiscipline &a)
{
  if (this == &a) return *this;
  for (discNumT i = MIN_DISC; i < MAX_DISCS; i++) {
    delete disc[i];
    if (a.disc[i])
      disc[i] = a.disc[i]->cloneMe();
    else
      disc[i] = NULL;
  }
  return *this;
}

CMasterDiscipline::~CMasterDiscipline()
{
  for (discNumT i = MIN_DISC; i < MAX_DISCS; i++) {
    delete disc[i];
    disc[i] = NULL;
  }
}

CDiscipline::CDiscipline() :
  uNatLearnedness(0),
  uLearnedness(0),
  uDoLearnedness(0),
  ok_for_class(0)
{
}

CDiscipline::CDiscipline(const CDiscipline &a) :
  uNatLearnedness(a.uNatLearnedness), 
  uLearnedness(a.uLearnedness),
  uDoLearnedness(a.uDoLearnedness),
  ok_for_class(a.ok_for_class)
{
}

CDiscipline & CDiscipline::operator= (const CDiscipline &a)
{
  if (this == &a) return *this;
  uNatLearnedness = a.uNatLearnedness;
  uLearnedness = a.uLearnedness;
  uDoLearnedness = a.uDoLearnedness;
  ok_for_class = a.ok_for_class;
  return *this;
}

CDiscipline::~CDiscipline()
{
}
