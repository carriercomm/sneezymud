//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: cmd_attribute.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//                                                                      //
//    SneezyMUD++ - All rights reserved, SneezyMUD Coding Team      //
//                                                                      //
//    "attribute.cc" - the attribute command
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"

static void showStatsTo(const Descriptor *d, const TBeing *ch, bool hidden_stuff)
{
  struct time_info_data playing_time;
  char time_buf[160];
  char buffer[256];
  char buf3[12], buf4[12];
  string str;

  realTimePassed((time(0) - d->session.connect),0, &playing_time);
  if (playing_time.day)
    playing_time.hours += playing_time.day * 24;
  sprintf(time_buf, "%d hour%s, %d minute%s and %d second%s", 
        playing_time.hours, 
        ((playing_time.hours == 1) ? "" : "s"),
        playing_time.minutes,
        ((playing_time.minutes== 1) ? "" : "s"),
        playing_time.seconds,
        ((playing_time.seconds== 1) ? "" : "s"));

  TBeing *victim = d->character;

  sprintf(buffer, "In this session:\n\r");
  str += buffer;

  if (d == ch->desc)
    sprintf(buffer, "You have");
  else
    sprintf(buffer, "%s (L%d:%d) has", victim->getName(), victim->GetMaxLevel(), victim->GetMaxLevel() * 50 / 3);
  str += buffer;

  sprintf(buffer, " been playing %s%s%s.\n\r", ch->green(), time_buf, ch->norm());
  str += buffer;

  sprintf(buffer, "Contribution to deities' potency: %s%5.2f%s\n\r", ch->cyan(), d->session.perc, ch->norm());
  str += buffer;
  sprintf(buffer, "Creatures killed : Alone  %s%d%s, Group %s%d%s. Exp Gained : %s%.2f%s\n\r",
         ch->cyan(), d->session.kills, ch->norm(),
         ch->cyan(), d->session.groupKills, ch->norm(),
         ch->cyan(), d->session.xp, ch->norm());
  str += buffer;

  int i;
  if (hidden_stuff) {
    sprintf(buffer, "Offense: %d      Defense: %d    mode: %s\n\r",
        victim->attackRound(NULL), victim->defendRound(NULL),
          attack_modes[victim->getCombatMode()]);
    str += buffer;
    sprintf(buffer, "Combat   :            hits swings   rnds   hit%%  ComDam SklDam  Mod  Pot\n\r");
    str += buffer;
  
    for (i = 0; i < MAX_ATTACK_MODE_TYPE; i++) {
      // prevent them from using this to see specific damage of a weapon
      sprintf(buffer, "%s%-9.9s%s: inflict: %s%6u%s %s%6u%s %s%6u%s (%s%4.1f%%%s) %s%6d%s %s%6d%s %s%4d%s %s%4.1f%s\n\r",
           ch->cyan(), attack_modes[i], ch->norm(),
           ch->cyan(), d->session.hits[i], ch->norm(),
           ch->cyan(), d->session.swings[i], ch->norm(),
           ch->cyan(), d->session.rounds[i], ch->norm(),
           ch->cyan(), (d->session.swings[i] ? 100.0 * d->session.hits[i] / d->session.swings[i] : 0.0), ch->norm(),
           ch->cyan(), d->session.combat_dam_done[i], ch->norm(),
           ch->cyan(), d->session.skill_dam_done[i], ch->norm(),
           ch->cyan(), (d->session.swings[i] ? d->session.mod_done[i] / (int) d->session.swings[i] :0), ch->norm(),
           ch->cyan(), (d->session.hits[i] ? (float) d->session.potential_dam_done[i] / d->session.hits[i] :0), ch->norm());
      str += buffer;

      sprintf(buffer, "  %sL%6.2f%s: receive: %s%6u%s %s%6u%s %s%6u%s (%s%4.1f%%%s) %s%6d%s %s%6d%s %s%4d%s %s%4.1f%s\n\r",
           ch->cyan(), (d->session.swings_received[i] ? (float) d->session.level_attacked[i] / d->session.swings_received[i] : 0.0), ch->norm(),
           ch->cyan(), d->session.hits_received[i], ch->norm(),
           ch->cyan(), d->session.swings_received[i], ch->norm(),
           ch->cyan(), d->session.rounds_received[i], ch->norm(),
           ch->cyan(), (d->session.swings_received[i] ? 100.0 * d->session.hits_received[i] / d->session.swings_received[i] : 0.0), ch->norm(),
           ch->cyan(), d->session.combat_dam_received[i], ch->norm(),
           ch->cyan(), d->session.skill_dam_received[i], ch->norm(),
           ch->cyan(), (d->session.swings_received[i] ? d->session.mod_received[i] / (int) d->session.swings_received[i] :0), ch->norm(),
           ch->cyan(), (d->session.hits_received[i] ? (float) d->session.potential_dam_received[i] / d->session.hits_received[i] :0), ch->norm());
      str += buffer;
    }
    sprintf(buffer, "Skill Success  : %5.3f%%   pass: %d, att: %d\n\r",
           100.0 * ((float) d->session.skill_success_pass/
            (float) max((unsigned int) 1, d->session.skill_success_attempts)),
           d->session.skill_success_pass,
           d->session.skill_success_attempts);
    str += buffer;
    sprintf(buffer, "Spell Success  : %5.3f%%   pass: %d, att: %d\n\r",
           100.0 * ((float) d->session.spell_success_pass/
            (float) max((unsigned int) 1, d->session.spell_success_attempts)),
           d->session.spell_success_pass,
           d->session.spell_success_attempts);
    str += buffer;
    sprintf(buffer, "Prayer Success : %5.3f%%   pass: %d, att: %d\n\r",
           100.0 * ((float) d->session.prayer_success_pass/
            (float) max((unsigned int) 1, d->session.prayer_success_attempts)),
           d->session.prayer_success_pass,
           d->session.prayer_success_attempts);
    str += buffer;
  } else {
    // !hiddenstuff
    sprintf(buffer, "Combat   : hit%%      damage ratio (inflicted:received)\n\r");
    str += buffer;
    for (i = 0; i < MAX_ATTACK_MODE_TYPE; i++) {
      if (d->session.combat_dam_received[i])
        sprintf(buf3, "%5.3f:1.0", (float) (((float) d->session.combat_dam_done[i])/((float) d->session.combat_dam_received[i])));
      else
        sprintf(buf3, "0.0:1.0");
      sprintf(buffer, "%s%-9.9s%s: (%s%5.2f%%%s)     %s%-10s%s\n\r",
           ch->cyan(), attack_modes[i], ch->norm(),
           ch->cyan(), (d->session.swings[i] ? 100.0 * d->session.hits[i] / d->session.swings[i] : 0.0), ch->norm(),
           ch->cyan(), buf3, ch->norm());
      str += buffer;
    }
    spellNumT skill = victim->getSkillNum(SKILL_SNEAK);
    if (!ch->isImmortal() && 
         (victim->affectedBySpell(skill) || victim->checkForSkillAttempt(skill))) {
      sprintf(buffer, "Skill Success  : info concealed at present time.\n\r");
    } else {
      sprintf(buffer, "Skill Success  : %5.3f%%\n\r",
             100.0 * ((float) d->session.skill_success_pass/
              (float) max((unsigned int) 1, d->session.skill_success_attempts)));
    }
    str += buffer;
    sprintf(buffer, "Spell Success  : %5.3f%%\n\r",
           100.0 * ((float) d->session.spell_success_pass/
            (float) max((unsigned int) 1, d->session.spell_success_attempts)));
    str += buffer;
    sprintf(buffer, "Prayer Success : %5.3f%%\n\r",
           100.0 * ((float) d->session.prayer_success_pass/
            (float) max((unsigned int) 1, d->session.prayer_success_attempts)));
    str += buffer;
  }

  if (ch->desc == d)
    sprintf(buffer, "\n\rIn your career:\n\rYou have");
  else
    sprintf(buffer, "\n\rIn %s's career:\n\r%s has", victim->getName(), victim->getName());
  str += buffer;
  realTimePassed((time(0) - victim->player.time.logon) +
                                victim->player.time.played, 0, &playing_time);

  sprintf(buffer, " been playing for %s%d%s days and %s%d%s hours.\n\r",
      ch->purple(), playing_time.day, ch->norm(),
      ch->purple(), playing_time.hours, ch->norm());
  str += buffer;

  sprintf(buffer, "Creatures killed : Alone  %s%u%s, Group %s%u%s. Exp Gained : %s%.2f%s\n\r",
         ch->cyan(), d->career.kills, ch->norm(),
         ch->cyan(), d->career.group_kills, ch->norm(),
         ch->cyan(), d->career.exp, ch->norm());
  str += buffer;

  if (hidden_stuff) {
    sprintf(buffer, "Combat   : hits     swings           dam done dam received\n\r");
    for (i = 0; i < MAX_ATTACK_MODE_TYPE; i++) {
      // prevent them from using this to see specific damage of a weapon
      sprintf(buf3, "%8d", d->career.dam_done[i]);
      sprintf(buf4, "%8d", d->career.dam_received[i]);
      sprintf(buffer, "%s%-9.9s%s: %s%8u%s %s%8u%s (%s%4.1f%%%s) %s%8s%s %s%8s%s\n\r",
           ch->cyan(), attack_modes[i], ch->norm(),
           ch->cyan(), d->career.hits[i], ch->norm(),
           ch->cyan(), d->career.swings[i], ch->norm(),
           ch->cyan(), (d->career.swings[i] ? 100.0 * d->career.hits[i] / d->career.swings[i] : 0.0), ch->norm(),
           ch->cyan(), buf3, ch->norm(),
           ch->cyan(), buf4, ch->norm());
      str += buffer;
    }
  } else {
    // !hidden_stuff
    sprintf(buffer, "Combat   : hit%%      damage ratio (inflicted:received)\n\r");
    str += buffer;
    for (i = 0; i < MAX_ATTACK_MODE_TYPE; i++) {
      if (d->career.dam_received[i])
        sprintf(buf3, "%5.3f:1.0", (float) (((float) d->career.dam_done[i])/((float) d->career.dam_received[i])));
      else
        sprintf(buf3, "0.0:1.0");
      sprintf(buffer, "%s%-9.9s%s: (%s%5.2f%%%s)     %s%-10s%s\n\r",
           ch->cyan(), attack_modes[i], ch->norm(),
           ch->cyan(), (d->career.swings[i] ? 100.0 * d->career.hits[i] / d->career.swings[i] : 0.0), ch->norm(),
           ch->cyan(), buf3, ch->norm());
      str += buffer;
    }
  }
  sprintf(buffer, "Total deaths: %s%u%s, Arena deaths: %s%u%s, Arena victories: %s%u%s\n\r",
         ch->cyan(), d->career.deaths, ch->norm(),
         ch->cyan(), d->career.arena_loss, ch->norm(),
         ch->cyan(), d->career.arena_victs, ch->norm());
  str += buffer;

  if (hidden_stuff) {
    sprintf(buffer, "Skill Success  : %5.3f%%   pass: %d, att: %d\n\r",
           100.0 * ((float) d->career.skill_success_pass/
            (float) max((unsigned int) 1, d->career.skill_success_attempts)),
           d->career.skill_success_pass,
           d->career.skill_success_attempts);
    str += buffer;
    sprintf(buffer, "Spell Success  : %5.3f%%   pass: %d, att: %d\n\r",
           100.0 * ((float) d->career.spell_success_pass/
            (float) max((unsigned int) 1, d->career.spell_success_attempts)),
           d->career.spell_success_pass,
           d->career.spell_success_attempts);
    str += buffer;
    sprintf(buffer, "Prayer Success : %5.3f%%   pass: %d, att: %d\n\r",
           100.0 * ((float) d->career.prayer_success_pass/
            (float) max((unsigned int) 1, d->career.prayer_success_attempts)),
           d->career.prayer_success_pass,
           d->career.prayer_success_attempts);
    str += buffer;
  } else {
    // !hidden_stuff
    spellNumT skill = victim->getSkillNum(SKILL_SNEAK);
    if (!ch->isImmortal() && 
         (victim->affectedBySpell(skill) || victim->checkForSkillAttempt(skill))) {
      sprintf(buffer, "Skill Success  : info concealed at present time.\n\r");
    } else {
      sprintf(buffer, "Skill Success  : %5.3f%%\n\r",
             100.0 * ((float) d->career.skill_success_pass/
             (float) max((unsigned int) 1, d->career.skill_success_attempts)));
    }
    str += buffer;
    sprintf(buffer, "Spell Success  : %5.3f%%\n\r",
           100.0 * ((float) d->career.spell_success_pass/
            (float) max((unsigned int) 1, d->career.spell_success_attempts)));
    str += buffer;
    sprintf(buffer, "Prayer Success : %5.3f%%\n\r",
           100.0 * ((float) d->career.prayer_success_pass/
            (float) max((unsigned int) 1, d->career.prayer_success_attempts)));
    str += buffer;
  }
  sprintf(buffer, "Pets owned: %d of an average level of %5.3f\n\r",
         d->career.pets_bought,
         ((float) d->career.pet_levels_bought/
            (float) max((unsigned int) 1, d->career.pets_bought)));
  str += buffer;

  if (hidden_stuff) {
    char * local_time;
    if (d->career.hit_level40) {
      local_time = ctime(&d->career.hit_level40);
      local_time[strlen(local_time) -1] = '\0';
      sprintf(buffer, "Hit level 40 on: %s\n\r", local_time);
      str += buffer;
    }
    if (d->career.hit_level50) {
      local_time = ctime(&d->career.hit_level50);
      local_time[strlen(local_time) -1] = '\0';
      sprintf(buffer, "Hit level 50 on: %s\n\r", local_time);
      str += buffer;
    }
  }

  sprintf(buffer, "Combat inflicted : crit-hits %s%u%s, crit-misses %s%u%s, crit-kills %s%u%s\n\r",
         ch->cyan(), d->career.crit_hits, ch->norm(),
         ch->cyan(), d->career.crit_misses, ch->norm(),
         ch->cyan(), d->career.crit_kills, ch->norm());
  str += buffer;
  sprintf(buffer, "Combat suffered  : crit-hits %s%u%s, crit-kills %s%u%s\n\r",
         ch->cyan(), d->career.crit_hits_suff, ch->norm(),
         ch->cyan(), d->career.crit_kills_suff, ch->norm());
  str += buffer;
  if (d->career.ounces_of_blood) {
    sprintf(buffer, "Ounces of blood lost                    : %s%u%s\n\r",
           ch->cyan(), d->career.ounces_of_blood, ch->norm());
    str += buffer;
  } 
  if (d->career.stuck_in_foot) {
    sprintf(buffer, "COMBAT-CRIT: Weapons stuck in foot      : %s%u%s\n\r",
           ch->cyan(), d->career.stuck_in_foot, ch->norm());
    str += buffer;
  } 
  if (d->career.crit_beheads) {
    sprintf(buffer, "COMBAT-CRIT: Beheadings inflicted       : %s%u%s\n\r",
           ch->cyan(), d->career.crit_beheads, ch->norm());
    str += buffer;
  }
  if (d->career.crit_beheads_suff) {
    sprintf(buffer, "COMBAT-CRIT: Beheadings suffered        : %s%u%s\n\r",
           ch->cyan(), d->career.crit_beheads_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_sev_limbs) {
    sprintf(buffer, "COMBAT-CRIT: Severed limbs inflicted    : %s%u%s\n\r",
           ch->cyan(), d->career.crit_sev_limbs, ch->norm());
    str += buffer;
  }
  if (d->career.crit_sev_limbs_suff) {
    sprintf(buffer, "COMBAT-CRIT: Severed limbs suffered     : %s%u%s\n\r",
           ch->cyan(), d->career.crit_sev_limbs_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_cranial_pierce) {
    sprintf(buffer, "COMBAT-CRIT: Cranial piercings inflicted: %s%u%s\n\r",
           ch->cyan(), d->career.crit_cranial_pierce, ch->norm());
    str += buffer;
  }
  if (d->career.crit_cranial_pierce_suff) {
    sprintf(buffer, "COMBAT-CRIT: Cranial piercings suffered : %s%u%s\n\r",
           ch->cyan(), d->career.crit_cranial_pierce_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_broken_bones) {
    sprintf(buffer, "COMBAT-CRIT: Broken bones inflicted     : %s%u%s\n\r",
           ch->cyan(), d->career.crit_broken_bones, ch->norm());
    str += buffer;
  }
  if (d->career.crit_broken_bones_suff) {
    sprintf(buffer, "COMBAT-CRIT: Broken bones suffered      : %s%u%s\n\r",
           ch->cyan(), d->career.crit_broken_bones_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_crushed_skull) {
    sprintf(buffer, "COMBAT-CRIT: Crushed skulls inflicted   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_crushed_skull, ch->norm());
    str += buffer;
  }
  if (d->career.crit_crushed_skull_suff) {
    sprintf(buffer, "COMBAT-CRIT: Crushed skulls suffered    : %s%u%s\n\r",
           ch->cyan(), d->career.crit_crushed_skull_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_cleave_two) {
    sprintf(buffer, "COMBAT-CRIT: Cleaved in two inflicted   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_cleave_two, ch->norm());
    str += buffer;
  }
  if (d->career.crit_cleave_two_suff) {
    sprintf(buffer, "COMBAT-CRIT: Cleaved in two suffered    : %s%u%s\n\r",
           ch->cyan(), d->career.crit_cleave_two_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_disembowel) {
    sprintf(buffer, "COMBAT-CRIT: Disembowels inflicted      : %s%u%s\n\r",
           ch->cyan(), d->career.crit_disembowel, ch->norm());
    str += buffer;
  }
  if (d->career.crit_disembowel_suff) {
    sprintf(buffer, "COMBAT-CRIT: Disembowels suffered       : %s%u%s\n\r",
           ch->cyan(), d->career.crit_disembowel_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_crushed_nerve) {
    sprintf(buffer, "COMBAT-CRIT: Crushed Nerves/Muscles inflicted   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_crushed_nerve, ch->norm());
    str += buffer;
  }
  if (d->career.crit_crushed_nerve_suff) {
    sprintf(buffer, "COMBAT-CRIT: Crushed Nerves/Muscles suffered   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_crushed_nerve_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_voice) {
    sprintf(buffer, "COMBAT-CRIT: Punctured Voice Boxes inflicted   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_voice, ch->norm());
    str += buffer;
  }
  if (d->career.crit_voice_suff) {
    sprintf(buffer, "COMBAT-CRIT: Punctured Voice Boxes suffered   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_voice_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_eye_pop) {
    sprintf(buffer, "COMBAT-CRIT: Gouged Out Eyeballs inflicted   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_eye_pop, ch->norm());
    str += buffer;
  }
  if (d->career.crit_eye_pop_suff) {
    sprintf(buffer, "COMBAT-CRIT: Gouged Out Eyeballs suffered   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_eye_pop_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_lung_punct) {
    sprintf(buffer, "COMBAT-CRIT: Punctured Lungs inflicted   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_lung_punct, ch->norm());
    str += buffer;
  }
  if (d->career.crit_lung_punct_suff) {
    sprintf(buffer, "COMBAT-CRIT: Punctured Lungs suffered   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_lung_punct_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_impale) {
    sprintf(buffer, "COMBAT-CRIT: Weapon Impalings inflicted   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_impale, ch->norm());
    str += buffer;
  }
  if (d->career.crit_impale_suff) {
    sprintf(buffer, "COMBAT-CRIT: Weapon Impalings suffered   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_impale_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_eviscerate) {
    sprintf(buffer, "COMBAT-CRIT: Eviscerations inflicted   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_eviscerate, ch->norm());
    str += buffer;
  }
  if (d->career.crit_eviscerate_suff) {
    sprintf(buffer, "COMBAT-CRIT: Eviscerations suffered   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_eviscerate_suff, ch->norm());
    str += buffer;
  }
  if (d->career.crit_kidney) {
    sprintf(buffer, "COMBAT-CRIT: Kidney Wounds inflicted   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_kidney, ch->norm());
    str += buffer;
  }
  if (d->career.crit_kidney_suff) {
    sprintf(buffer, "COMBAT-CRIT: Kidney Wounds suffered   : %s%u%s\n\r",
           ch->cyan(), d->career.crit_kidney_suff, ch->norm());
    str += buffer;
  }
  ch->desc->page_string(str.c_str(), 0);
  return;
}

void TBeing::doAttribute(const char *arg)
{
  char buf[512], buf2[512], cmdbuf[256];
  struct time_info_data playing_time;
  struct time_info_data birth_data;
  int day;
  Descriptor *d;

  if (!desc)
    return;

  one_argument(arg, cmdbuf);
  if (!*cmdbuf) {
    sendTo("Syntax: attribute <personal | condition | statistics>\n\r");
    return;
  }
  // I put immortal check before the regular since there is a PC called "per"
  if (isImmortal()) {
    TBeing *victim;
    victim = get_pc_world(this, cmdbuf, EXACT_YES);
    if (!victim)
      victim = get_pc_world(this, cmdbuf, EXACT_NO);

    if (victim) {
      if (!(d = victim->desc)) {
        sendTo(COLOR_MOBS, "%s doesn't seem to have a connection right now.\n\r", victim->getName());
        return;
      }
      showStatsTo(d, this, true);
      return;
    }
  }
  if (is_abbrev(cmdbuf, "personal")) {
    mudTimePassed(player.time.birth, BEGINNING_OF_TIME, &birth_data);
    birth_data.year += YEAR_ADJUST;
    birth_data.year -= getBaseAge();

    day = birth_data.day + 1;        // day in [1..35] 

    sprintf(buf, "You were born on the %s of %s, in the year %d P.S.\n\r",
          numberAsString(day).c_str(),
          month_name[birth_data.month], birth_data.year);
    sendTo(buf);

    sprintf(buf, "You grew up as %s %s and began adventuring at the age of %d.\n\r",
      (startsVowel(home_terrains[player.hometerrain]) ? "an" : "a"),
      home_terrains[player.hometerrain], getBaseAge());
    sendTo(buf);

    sendTo("You are %d years and %d months old, %d inches tall, and you weigh %d lbs.\n\r",
        age()->year, age()->month, getHeight(), (int) getWeight());
    if (!age()->month && !age()->day)
      sendTo(" It's your birthday today.\n\r");

    strcpy(buf, "Your social flags are: ");
    sprintbit(desc->plr_act, attr_player_bits, buf2);
    strcat(buf, buf2);
    strcat(buf, "\n\r");
    sendTo(buf);

#if FACTIONS_IN_USE
    sendTo("You are allied to %s, and have a %.4f%% rating.\n\r",
          FactionInfo[getFaction()].faction_name, getPerc());
#else
    sendTo("You are allied to %s.\n\r",
          FactionInfo[getFaction()].faction_name);
#endif

#if SPEEF_MAKE_BODY
    vlogf(5,"Attribute argument: %s",cmdbuf);
    if(body)
      body->showBody(this);
    else
      sendTo("You have no Body!\n\r");
    vlogf(5,"I tried to show a body.");
#endif
 
    return;
  } else if (is_abbrev(cmdbuf, "condition")) {
    if (GetMaxLevel() > 10) {
      if (!isImmortal()) {
        sendTo("You are carrying %3.f%% of your maximum weight capacity.\n\r",
          (getCarriedWeight()/carryWeightLimit()) * 100.0);
        sendTo("You are carrying %3.f%% of your maximum volume capacity.\n\r",
          (float) ((float) getCarriedVolume()/(float) carryVolumeLimit()) * 100);
      } else {
        sendTo("You have %.1f lbs of equipment, and can carry up to %.1f lbs - %.1f spare lbs.\n\r",
            getCarriedWeight(), carryWeightLimit(), 
            carryWeightLimit() - getCarriedWeight());
        sendTo("You have %d volume in inventory.\n\rYou can carry up to %d volume - You have %d spare volume capacity.\n\r",
            getCarriedVolume(), carryVolumeLimit(), 
            carryVolumeLimit() - getCarriedVolume());
      }
    }
    sendTo(COLOR_MOBS, "<c>Stats  :<z>");
    sendTo(COLOR_MOBS, chosenStats.printStatHeader().c_str());

    Stats mortalStats;

    if (GetMaxLevel() >= 1){
      sendTo(COLOR_MOBS,"<c>Chosen:<z>");
      sendTo(COLOR_MOBS, chosenStats.printRawStats(this).c_str());
    }

    if (GetMaxLevel() >= 20) {
      // use to be immortal only, best to show it as avoids confusion about
      // new whacky stat system
      sendTo(COLOR_MOBS,"<c>Natural:<z>");
      statTypeT ij;
      for(ij = MIN_STAT; ij<MAX_STATS_USED; ij++) {
        sendTo(" %3d ", getStat(STAT_NATURAL, ij));
      }
      sendTo("\n\r");

      sendTo(COLOR_MOBS,"<c>Current:<z>");
      sendTo(COLOR_MOBS, curStats.printRawStats(this).c_str());
    } else if (GetMaxLevel() >= 1){
      sendTo(COLOR_MOBS,"<c>Natural:<z>");
      statTypeT ij;
      for(ij = MIN_STAT; ij<MAX_STATS_USED; ij++) {
        sendTo(" %3d ", getStat(STAT_NATURAL, ij));
      }
      sendTo("\n\r");

    }

    if (GetMaxLevel() >= 5)
      describeImmunities(this, 100);

    if (!getCond(THIRST))
      sendTo(COLOR_BASIC, "<R>You are totally parched.<1>\n\r");
    else if ((getCond(THIRST) > 0) && (getCond(THIRST) <= 5))
      sendTo("Your throat is very dry.\n\r");
    else if ((getCond(THIRST) > 5) && (getCond(THIRST) <= 10))
      sendTo("You could use a little drink.\n\r");
    else if ((getCond(THIRST) > 10) && (getCond(THIRST) <= 20))
      sendTo("You are slightly thirsty.\n\r");
    else if (getCond(THIRST) > 20)
      sendTo("Your thirst is the least of your worries.\n\r");

    if (!getCond(FULL))
      sendTo(COLOR_BASIC, "<R>You are totally famished.<1>\n\r");
    else if ((getCond(FULL) > 0) && (getCond(FULL) <= 5))
      sendTo("Your stomach is growling loudly.\n\r");
    else if ((getCond(FULL) > 5) && (getCond(FULL) <= 10))
      sendTo("You could use a little bite to eat.\n\r");
    else if ((getCond(FULL) > 10) && (getCond(FULL) <= 20))
      sendTo("You are slightly hungry.\n\r");
    else if (getCond(FULL) > 20)
      sendTo("Your hunger is the least of your worries.\n\r");

    if (getCond(DRUNK) >= 20)
      sendTo("You are VERY drunk.\n\r");
    else if (getCond(DRUNK) >= 15)
      sendTo("You are very drunk.\n\r");
    else if (getCond(DRUNK) >= 10)
      sendTo("You are drunk.\n\r");
    else if (getCond(DRUNK) >= 4)
      sendTo("You are intoxicated.\n\r");
    else if (getCond(DRUNK) > 0)
      sendTo("You are feeling tipsy.\n\r");

    if (fight())
      act("You are fighting $N.", FALSE, this, NULL, fight(), TO_CHAR);
    else if (task) {
      sprintf(buf, "You are %s.\n\r", tasks[task->task].name);
      sendTo(buf);
    } else {
      TBeing *tbr;
      switch (getPosition()) {
        case POSITION_DEAD:
          sendTo("You are DEAD!\n\r");
          break;
        case POSITION_MORTALLYW:
          sendTo("You are mortally wounded!  You should seek help!\n\r");
          break;
        case POSITION_INCAP:
          sendTo("You are incapacitated, slowly fading away.\n\r");
          break;
        case POSITION_STUNNED:
          sendTo("You are stunned! You can't move.\n\r");
          break;
        case POSITION_SLEEPING:
          if (riding) {
            sprintf(buf, "You are sleeping on ");
            if (riding->getName())
              strcat(buf,objs(riding));
            else
              strcat(buf, "A bad object");
  
            strcat(buf,".\n\r");
          } else
            sprintf(buf, "You are sleeping.\n\r");
          sendTo(buf);
          break;
        case POSITION_RESTING:
          if (riding) {
            sprintf(buf, "You are resting on ");
            if (riding->getName())
              strcat(buf,objs(riding));
            else
              strcat(buf, "A horse with a bad short description, BUG THIS!");
            strcat(buf,".\n\r");
          } else
            sprintf(buf, "You are resting.\n\r");
          sendTo(buf);
            break;
        case POSITION_CRAWLING:
          sendTo(buf, "You are crawling.\n\r");
          break;
        case POSITION_SITTING:
          if (riding) {
            strcpy(buf, "You are sitting on ");
            if (riding->getName())
              strcat(buf,objs(riding));
            else
              strcat(buf, "A bad object!");
            strcat(buf,".\n\r");
          } else
            strcpy(buf, "You are sitting.\n\r");
          sendTo(buf);
          break;
        case POSITION_FLYING:
           if (roomp && roomp->isUnderwaterSector()) 
            sendTo("You are swimming about.");
          else
            sendTo("You are flying about.\n\r");

          break;
        case POSITION_STANDING:
          sendTo("You are standing.\n\r");
          break;
        case POSITION_MOUNTED:
          tbr = dynamic_cast<TBeing *>(riding);
          if (tbr && tbr->horseMaster() == this) {
            strcpy(buf, "You are here, riding ");
            strcat(buf, pers(tbr));
            strcat(buf, ".\n\r");
            sendTo(COLOR_MOBS, buf);
          } else if (tbr) {
            sprintf(buf, "You are here, also riding on %s's %s%s.\n\r",
                pers(tbr->horseMaster()),
                persfname(tbr).c_str(),
                tbr->isAffected(AFF_INVISIBLE) ? " (invisible)" : "");
            sendTo(COLOR_MOBS, buf);
          } else {
            sendTo("You are standing.\n\r");
          }
          break;
        default:
          sendTo("You are floating.\n\r");
          break;
      }
    }
    if (spelltask) {
      int which = spelltask->spell;
      if (discArray[which]->minPiety)
        sendTo("You are praying.\n\r");
      if (discArray[which]->minMana)
        sendTo("You are casting a spell.\n\r");
    }
    sendTo("You are in %s%s%s attack mode.\n\r",
          cyan(), attack_modes[getCombatMode()], norm());

    if (wimpy)
      sendTo("You are in wimpy mode, and will flee at %d hit points.\n\r", wimpy);
    if (IS_SET(desc->account->flags, ACCOUNT_BOSS))
      sendTo("Your account is in boss-mode.\n\r");
    if (IS_SET(desc->account->flags, ACCOUNT_MSP))
      sendTo("Your account has MUD Sound Protocol enabled.\n\r");
  
    describeLimbDamage(this);
    describeAffects(this);
  
    return;
  } else if (is_abbrev(cmdbuf, "statistics")) {
    showStatsTo(desc, this, false);

    return;
  } else if (is_abbrev(cmdbuf, "drugs")) {
    int i;
    for(i=MIN_DRUG;i<MAX_DRUG;++i){
      if(desc->drugs[i].total_consumed>0){
	sendTo("%s (%i):\n\r", drugTypes[i].name, i);
	sendTo("You first used %s on the %s day of %s, Year %d P.S.\n\r", 
	       drugTypes[i].name,
	       numberAsString(desc->drugs[i].first_use.day+1).c_str(),
	       month_name[desc->drugs[i].first_use.month], 
	       desc->drugs[i].first_use.year);
	sendTo("You last used %s on the %s day of %s, Year %d P.S.\n\r", 
	       drugTypes[i].name,
	       numberAsString(desc->drugs[i].last_use.day+1).c_str(),
	       month_name[desc->drugs[i].last_use.month], 
	       desc->drugs[i].last_use.year);
	sendTo("You have consumed %i units of %s.\n\r", 
	       desc->drugs[i].total_consumed, drugTypes[i].name);
	sendTo("You currently have %i units of %s in your body.\n\r", 
	       desc->drugs[i].current_consumed, drugTypes[i].name);
	
      }
    }
  } else {
    sendTo("Syntax: attribute <statistics | personal | condition>\n\r");
    return;
  }
}
