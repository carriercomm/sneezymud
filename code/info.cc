//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//    "info.cc" - All informative functions and routines                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"

#include <algorithm>
#include <iostream.h>

#include "account.h"
#include "games.h"
#include "disease.h"
#include "combat.h"
#include "statistics.h"
#include "components.h"
#include "skillsort.h"

string describeDuration(const TBeing *ch, int dur)
{
  char buf[160];
  string ret;
  int weeks = 0, days = 0, hours = 0, mins = 0;
  int errnum = 0;

  if (dur == PERMANENT_DURATION) {
    ret = "permanent";
    return ret;
  }
  // random error
  if (!ch->isImmortal()) {
#if 0
    errnum = ch->plotStat(STAT_CURRENT, STAT_PER, 175, 15, 75); 
    errnum = ::number(-1 * errnum, 1 * errnum);
#else
    // bad to randomize it, just have them overestimate it
    errnum = ch->plotStat(STAT_CURRENT, STAT_PER, 400, 30, 175);
#endif
    dur *= 1000 + errnum;
    dur /= 1000;
    dur = max(1, dur);
  }

  // aff->dur decrements once per PULSE_COMBAT
  // total duration (in pulses) it lasts is dur*COMBAT

  hours = dur * PULSE_COMBAT / PULSE_MUDHOUR;
  mins = ((dur * PULSE_COMBAT) % PULSE_MUDHOUR) * 60 / PULSE_MUDHOUR;

  if (hours >= 24) {
    days = hours/24;
    hours = hours % 24;
  }
  if (days >= 7) {
    weeks = days / 7;
    days = days % 7;
  }
  *buf = '\0';
  if (weeks)
    sprintf(buf + strlen(buf), "%d week%s, ", weeks, (weeks == 1 ? "" : "s"));
  if (days)
    sprintf(buf + strlen(buf), "%d day%s, ", days, (days == 1 ? "" : "s"));
  if (hours)
    sprintf(buf + strlen(buf), "%d hour%s, ", hours, (hours == 1 ? "" : "s"));
  if (mins)
    sprintf(buf + strlen(buf), "%d minute%s, ", mins, (mins== 1 ? "" : "s"));
    
  if (strlen(buf) > 0) {
    while (buf[strlen(buf) - 1] == ' ' || buf[strlen(buf) - 1] == ',')
      buf[strlen(buf) - 1] = '\0';
  }
  ret = buf;
  return ret;
}

void argument_split_2(const char *argument, char *first_arg, char *second_arg)
{
  int look_at, begin;
  begin = 0;

  for (; *(argument + begin) == ' '; begin++);

  for (look_at = 0; *(argument + begin + look_at) > ' '; look_at++)
    *(first_arg + look_at) = LOWER(*(argument + begin + look_at));

  *(first_arg + look_at) = '\0';
  begin += look_at;

  for (; *(argument + begin) == ' '; begin++);

  for (look_at = 0; *(argument + begin + look_at) > ' '; look_at++)
    *(second_arg + look_at) = LOWER(*(argument + begin + look_at));

  *(second_arg + look_at) = '\0';
  begin += look_at;
}

static const string describe_part_wounds(const TBeing *ch, wearSlotT pos)
{
  int i, flags;
  int last = 0, count = 0;
  char buf[256];

  if (ch->isLimbFlags(pos, PART_MISSING))
    return ("missing.");

  *buf = '\0';

  for (i = 0; i < MAX_PARTS; i++) {
    flags = (1 << i);
    if (ch->isLimbFlags(pos, flags)) {
      last = i;
      count++;
    }
  }
  if (count == 1 && (ch->isLimbFlags(pos, PART_TRANSFORMED)))
    return "";
  for (i = 0; i < MAX_PARTS; i++) {
    flags = (1 << i);
    if (ch->isLimbFlags(pos, flags)) {
      if (count == 1)
        sprintf(buf + strlen(buf), "%s%s%s.", ch->red(), body_flags[i], ch->norm());
      else if (i != last)
        sprintf(buf + strlen(buf), "%s%s%s, ", ch->red(), body_flags[i], ch->norm());
      else
        sprintf(buf + strlen(buf), "and %s%s%s.", ch->red(), body_flags[i], ch->norm());
    }
  }
  if (*buf)
    return (buf);
  else
    return ("");
}

// rp is the room looking at
// can't use roomp since spying into other room possible
void TBeing::listExits(const TRoom *rp) const
{
  int num = 0, count = 0;
  dirTypeT door;
  roomDirData *exitdata;
  char buf[1024];
  bool tCS = (getStat(STAT_CURRENT, STAT_PER) > 150);

  const char *exDirs[] =
  {
    "N", "E", "S", "W", "U",
    "D", "NE", "NW", "SE", "SW"
  };

  *buf = '\0';

  if (desc && desc->m_bIsClient) 
    return;
  
  // Red if closed (imm only), Blue if an open exit has a type, purple if normal

  if (isPlayerAction(PLR_BRIEF)) {
    sendTo("[Exits:");
    for (door = MIN_DIR; door < MAX_DIR; door++) {
      exitdata = rp->exitDir(door);

      if (exitdata && (exitdata->to_room != ROOM_NOWHERE)) {
        if (isImmortal()) {
          if (IS_SET(exitdata->condition, EX_CLOSED)) 
            sendTo(" %s%s%s", red(), exDirs[door], norm());
          else if (exitdata->door_type != DOOR_NONE) 
            sendTo(" %s%s%s", blue(), exDirs[door], norm());
          else 
            sendTo(" %s%s%s", purple(), exDirs[door], norm());
        } else /*if (canSeeThruDoor(exitdata))*/ {
          TRoom *exitp = real_roomp(exitdata->to_room);

#if 0
          if (exitdata->door_type != DOOR_NONE && !IS_SET(exitdata->condition, EX_CLOSED)) 
            sendTo(" %s%s%s", blue(), exDirs[door], norm());
          else 
            sendTo(" %s%s%s", purple(), exDirs[door], norm());
#else

          if (exitp) {
#if 1
            if (exitdata->door_type != DOOR_NONE &&
                ((tCS && !IS_SET(exitdata->condition, EX_SECRET)) ||
                 !IS_SET(exitdata->condition, EX_CLOSED))) {
#else
            if (exitdata->door_type != DOOR_NONE && !IS_SET(exitdata->condition, EX_CLOSED)) {
#endif
              if (IS_SET(exitdata->condition, EX_CLOSED))
                sendTo(" %s*%s%s",
                       (exitp->getSectorType() == SECT_FIRE ? red() :
                        (exitp->isAirSector() ? cyan() :
                         (exitp->isWaterSector() ? blue() : purple()))),
                       exDirs[door], norm());
              else
                sendTo(" %s%s%s",
                       (exitp->getSectorType() == SECT_FIRE ? redBold() :
                        (exitp->isAirSector() ? cyanBold() :
                         (exitp->isWaterSector() ? blueBold() : purpleBold()))),
                       exDirs[door], norm());
            } else if (exitdata->door_type == DOOR_NONE)
	      sendTo(" %s%s%s",
                     (exitp->getSectorType() == SECT_FIRE ? red() :
                      (exitp->isAirSector() ? cyan() :
                       (exitp->isWaterSector() ? blue() : purple()))),
                     exDirs[door], norm());
          } else
            vlogf(LOG_LOW, "Problem with door in room %d", inRoom());
#endif
        }
      }
    }
    sendTo(" ]\n\r");
    return;
  }

  // The following for loop is to figure out which room is the last
  // legal exit, so the word "and" can be put in front of it to make
  // the sentence sent to the player grammatically correct.        

  for (door = MIN_DIR; door < MAX_DIR; door++) {
    exitdata = rp->exitDir(door);
    if (exitdata) {
      if (exitdata->to_room != ROOM_NOWHERE &&
          (canSeeThruDoor(exitdata) ||
           isImmortal())) {
        num = door;
        count++;
      }
      if (IS_SET(exitdata->condition, EX_DESTROYED)) {
        if (!exitdata->keyword) {
          vlogf(LOG_LOW,"Destroyed door with no name!  Room %d", in_room);
        } else if (door == 4) 
          sendTo("%sThe %s in the ceiling has been destroyed.%s\n\r",
              blue(), fname(exitdata->keyword).c_str(), norm());
        else if (door == 5)
          sendTo("%sThe %s in the %s has been destroyed.%s\n\r",
              blue(), fname(exitdata->keyword).c_str(), roomp->describeGround().c_str(), norm());
        else
          sendTo("%sThe %s %s has been destroyed.%s\n\r",
              blue(), fname(exitdata->keyword).c_str(), dirs_to_leading[door], norm());
      }
      if (IS_SET(exitdata->condition, EX_CAVED_IN)) {
        sendTo("%sA cave in blocks the way %s.%s\n\r",
            blue(), dirs[door], norm());
      }
      // chance to detect secret - bat
      // the || case is a chance at a false-positive   :)
      if ((IS_SET(exitdata->condition, EX_SECRET) &&
          IS_SET(exitdata->condition, EX_CLOSED)) ||
          (!::number(0,100) && !isPerceptive())) {
        int chance = max(0, (int) getSkillValue(SKILL_SEARCH));

        if (getRace() == RACE_ELVEN)
          chance += 25;
        if (getRace() == RACE_GNOME)
          chance += plotStat(STAT_CURRENT, STAT_PER, 3, 18, 13) +
                    GetMaxLevel()/2;
        if (getRace() == RACE_DWARF && rp->isIndoorSector())
          chance += GetMaxLevel()/2 + 10;

        if ((::number(1,1000) < chance) && !isImmortal())
          sendTo("%sYou suspect something out of the ordinary here.%s\n\r",
              blue(), norm());
      }
    }
  }

  for (door = MIN_DIR; door < MAX_DIR; door++) {
    exitdata = rp->exitDir(door);
    if (exitdata && (exitdata->to_room != ROOM_NOWHERE)) {
      if (isImmortal()) {
// Red if closed, Blue if an open exit has a type, purple if normal
        if (IS_SET(exitdata->condition, EX_CLOSED)) {
          if (count == 1)
            sprintf(buf + strlen(buf), "%s%s%s.\n\r", red(), dirs[door], norm());
          else if (door != num)
            sprintf(buf + strlen(buf), "%s%s%s, ", red(), dirs[door], norm());
          else
            sprintf(buf + strlen(buf), "and %s%s%s.\n\r", red(), dirs[door],norm());
        } else if (exitdata->door_type != DOOR_NONE) {
          if (count == 1)
            sprintf(buf + strlen(buf), "%s%s%s.\n\r", blue(), dirs[door], norm());
          else if (door != num)
            sprintf(buf + strlen(buf), "%s%s%s, ", blue(), dirs[door], norm());
          else
            sprintf(buf + strlen(buf), "and %s%s%s.\n\r", blue(), dirs[door], norm());
        } else {  
          if (count == 1)
            sprintf(buf + strlen(buf), "%s%s%s.\n\r", purple(), dirs[door], norm());
          else if (door != num)
            sprintf(buf + strlen(buf), "%s%s%s, ", purple(), dirs[door], norm());
          else
            sprintf(buf + strlen(buf), "and %s%s%s.\n\r", purple(), dirs[door], norm());
        }
      } else /*if (canSeeThruDoor(exitdata))*/ {
        TRoom *exitp = real_roomp(exitdata->to_room);

        if (exitp) {
#if 1
          if (exitdata->door_type != DOOR_NONE &&
              ((tCS && !IS_SET(exitdata->condition, EX_SECRET)) ||
               !IS_SET(exitdata->condition, EX_CLOSED))) {
#else
          if (exitdata->door_type != DOOR_NONE &&
              !IS_SET(exitdata->condition, EX_CLOSED)) {
#endif
            if (IS_SET(exitdata->condition, EX_CLOSED))
              sprintf(buf + strlen(buf), "%s%s*%s%s%s",
                ((count != 1 && door == num) ? "and " : ""),
                (exitp->getSectorType() == SECT_FIRE ? red() :
                (exitp->isAirSector() ? cyan() :
                (exitp->isWaterSector() ? blue() :
                purple()))),
                dirs[door],
                norm(),
                (count == 1 || door == num ? ".\n\r" : ", "));
            else
              sprintf(buf + strlen(buf), "%s%s%s%s%s",
                ((count != 1 && door == num) ? "and " : ""),
                (exitp->getSectorType() == SECT_FIRE ? redBold() :
                (exitp->isAirSector() ? cyanBold() :
                (exitp->isWaterSector() ? blueBold() :
                purpleBold()))),
                dirs[door],
                norm(),
                (count == 1 || door == num ? ".\n\r" : ", "));
          } else if (exitdata->door_type == DOOR_NONE) {
            sprintf(buf + strlen(buf), "%s%s%s%s%s",
              ((count != 1 && door == num) ? "and " : ""),
              (exitp->getSectorType() == SECT_FIRE ? red() :
              (exitp->isAirSector() ? cyan() :
              (exitp->isWaterSector() ? blue() :
              purple()))),
              dirs[door],
              norm(),
              (count == 1 || door == num ? ".\n\r" : ", "));
          }
        } else
          vlogf(LOG_LOW, "Problem with door in room %d", inRoom());
      }
    }
  }

  if (*buf) {
    if (count == 1) 
      sendTo("You see an exit %s", buf);
    else 
      sendTo("You can see exits to the %s", buf);
  } else
    sendTo("You see no obvious exits.\n\r");
}


void list_char_in_room(TThing *list, TBeing *ch)
{
  TThing *i, *cond_ptr[50];
  int k, cond_top;
  unsigned int cond_tot[50];
  bool found = FALSE;

  cond_top = 0;

  for (i = list; i; i = i->nextThing) {
    if (dynamic_cast<TBeing *>(i) && (ch != i) && (!i->rider) &&
        (ch->isAffected(AFF_SENSE_LIFE) || ch->isAffected(AFF_INFRAVISION) || (ch->canSee(i)))) {
      if ((cond_top < 50) && !i->riding) {
        found = FALSE;
        if (dynamic_cast<TMonster *>(i)) {
          for (k = 0; (k < cond_top && !found); k++) {
            if (cond_top > 0) {
              if (i->isSimilar(cond_ptr[k])) {
                cond_tot[k] += 1;
                found = TRUE;
              }
            }
          }
        }
        if (!found) {
          cond_ptr[cond_top] = i;
          cond_tot[cond_top] = 1;
          cond_top += 1;
        }
      } else
        ch->showTo(i, SHOW_MODE_DESC_PLUS);
    }
  }
  if (cond_top) {
    for (k = 0; k < cond_top; k++) {
      if (cond_tot[k] > 1)
        ch->showMultTo(cond_ptr[k], SHOW_MODE_DESC_PLUS, cond_tot[k]);
      else
        ch->showTo(cond_ptr[k], SHOW_MODE_DESC_PLUS);
    }
  }
}


void list_char_to_char(TBeing *list, TBeing *ch, int)
{
  TThing *i;

  for (i = list; i; i = i->nextThing) {
    if ((ch != i) && (ch->isAffected(AFF_SENSE_LIFE) || (ch->canSee(i))))
      ch->showTo(i, SHOW_MODE_DESC_PLUS);
  }
}

string TBeing::dynColorRoom(TRoom * rp, int title, bool) const
{
//  if (rp && title && full) {
//  }

  int len, letter;
  char argument[MAX_STRING_LENGTH];
  char buf2[10];
  char buf3[5];

  *buf2 = *buf3 = '\0';

  memset(argument, '\0', sizeof(argument));
  memset(buf2, '\0', sizeof(buf2));
  memset(buf3, '\0', sizeof(buf3));

  if (title == 1) {
    if (rp->getName()) {
      strcpy(argument, rp->getName());
      if (argument[0] == '<') {
          buf3[0] = argument[0];
          buf3[1] = argument[1];
          buf3[2] = argument[2];
          strcpy(buf2, buf3);
      } else {
        strcpy(buf2, addColorRoom(rp, 1).c_str());
      }
    } else {
      vlogf(LOG_BUG, "%s is in a room with no descr", getName());
      return "Bogus Name";
    }
  } else if (title == 2) {
    if (rp->getDescr()) {
      strcpy(argument, rp->getDescr());
      if (argument[0] == '<') {
#if 1
        buf2[0] = argument[0];
        buf2[1] = argument[1];
        buf2[2] = argument[2];
#else
        buf3[0] = argument[0];
        buf3[1] = argument[1];  
        buf3[2] = argument[2];  
        strcpy(buf2, buf3);
#endif
      } else {   
        strcpy(buf2, addColorRoom(rp, 2).c_str());
      }
    } else {
      vlogf(LOG_BUG, "%s is in a room with no descr", getName());
      return "Bogus Name";
    }
  } else {
    vlogf(LOG_BUG, "%s called a function with a bad dynColorRoom argument", getName());
    return "Something Bogus, tell a god";
  }
// Found had to initialize with this logic and too tired to figure out why

  string buf = "";
  if (buf2) {
    buf = buf2;
  }

  len = strlen(argument);
  for(letter=0; letter <= len; letter++) {
    if (letter < 2) {
      buf += argument[letter];
      continue;
    }
    if ((argument[letter] == '>') && (argument[letter - 2] == '<')) {
      switch (argument[(letter - 1)]) {
        case '1':
        case 'z':
        case 'Z':
          buf += argument[letter];
          if (buf2) {
            buf += buf2;
          }
          break;
        default:
          buf += argument[letter];
          break;
      }
    } else {
      buf += argument[letter];
    }
  }
  buf += "<1>";
  return buf;
}

// Peel
string TRoom::daynightColorRoom() const
{
  if(IS_SET(roomFlags, ROOM_INDOORS))
    return("<z>");

  switch (weather_info.sunlight) {
    case SUN_DAWN:
    case SUN_RISE:
      return("<w>");
      break;
    case SUN_LIGHT:
      return("<z>");
      break;
    case SUN_SET:
    case SUN_TWILIGHT:
      return("<w>");
      break;
    case SUN_DARK:
      return("<k>");
      break;
  }

  return("<z>");
}

const string TBeing::addColorRoom(TRoom * rp, int title) const
{
  char buf2[10];
  char buf3[10];

  *buf2 = *buf3 = '\0';

// Found had to initialize with this logic and too tired to figure out why
  strcpy(buf3, "<z>");

  sectorTypeT sector = rp->getSectorType();

  switch (sector) {
    case SECT_SUBARCTIC:
      strcpy(buf2, "<P>");
      strcpy(buf3, "<p>");
      break;
    case SECT_ARCTIC_WASTE:
      strcpy(buf2, "<w>");
      strcpy(buf3, "<W>");
      break;
    case SECT_ARCTIC_CITY:
      strcpy(buf2, "<C>");
      strcpy(buf3, rp->daynightColorRoom().c_str());
      break;
    case SECT_ARCTIC_ROAD:
      strcpy(buf2, "<W>");
      strcpy(buf3, rp->daynightColorRoom().c_str());
      break;
    case SECT_TUNDRA:
      strcpy(buf2, "<o>");
      strcpy(buf3, "<p>");
      break;
    case SECT_ARCTIC_MOUNTAINS:
      strcpy(buf2, "<o>");
      strcpy(buf3, "<W>");
      break;
    case SECT_ARCTIC_FOREST:
      strcpy(buf2, "<G>");
      strcpy(buf3, "<W>");
      break;
    case SECT_ARCTIC_MARSH:
      strcpy(buf2, "<B>");
      strcpy(buf3, "<p>");
      break;
    case SECT_ARCTIC_RIVER_SURFACE:
      strcpy(buf2, "<C>");
      strcpy(buf3, "<c>");
      break;
    case SECT_ICEFLOW:
      strcpy(buf2, "<C>");
      strcpy(buf3, "<W>");
      break;
    case SECT_COLD_BEACH:
      strcpy(buf2, "<p>");
      strcpy(buf3, "<P>");
      break;
    case SECT_SOLID_ICE:
      strcpy(buf2, "<c>");
      strcpy(buf3, "<C>");
      break;
    case SECT_ARCTIC_BUILDING:
      strcpy(buf2, "<p>");
      break;
    case SECT_ARCTIC_CAVE:
      strcpy(buf2, "<c>");
      strcpy(buf3, "<k>");
      break;
    case SECT_ARCTIC_ATMOSPHERE:
      strcpy(buf2, "<C>");
      strcpy(buf3, "<C>");
      break;
    case SECT_ARCTIC_CLIMBING:
    case SECT_ARCTIC_FOREST_ROAD:
      strcpy(buf2, "<p>");
      strcpy(buf3, rp->daynightColorRoom().c_str());
      break;
    case SECT_PLAINS:
      strcpy(buf2, "<G>");
      strcpy(buf3, "<g>");
      break;
    case SECT_TEMPERATE_CITY:
    case SECT_TEMPERATE_ROAD:
      strcpy(buf2, "<p>");
      strcpy(buf3, rp->daynightColorRoom().c_str());
      break;
    case SECT_GRASSLANDS:
      strcpy(buf2, "<G>");
      strcpy(buf3, "<g>");
      break;
    case SECT_TEMPERATE_HILLS:
      strcpy(buf2, "<o>");
      strcpy(buf3, "<g>");
      break;
    case SECT_TEMPERATE_MOUNTAINS:
      strcpy(buf2, "<G>");
      strcpy(buf3, "<o>");
      break;
    case SECT_TEMPERATE_FOREST:
      strcpy(buf2, "<G>");
      strcpy(buf3, "<g>");
      break;
    case SECT_TEMPERATE_SWAMP:
      strcpy(buf2, "<P>");
      strcpy(buf3, "<p>");
      break;
    case SECT_TEMPERATE_OCEAN:
      strcpy(buf2, "<C>");
      strcpy(buf3, "<c>");
      break;
    case SECT_TEMPERATE_RIVER_SURFACE:
      strcpy(buf2, "<B>");
      strcpy(buf3, "<b>");
      break;
    case SECT_TEMPERATE_UNDERWATER:
      strcpy(buf2, "<C>");
      strcpy(buf3, "<b>");
      break;
    case SECT_TEMPERATE_CAVE:
      strcpy(buf2, "<o>");
      strcpy(buf3, "<k>");
      break;
    case SECT_TEMPERATE_ATMOSPHERE:
      strcpy(buf2, "<G>");
      strcpy(buf3, rp->daynightColorRoom().c_str());
      break;
    case SECT_TEMPERATE_CLIMBING:
      strcpy(buf2, "<G>");
      strcpy(buf3, rp->daynightColorRoom().c_str());
      break;
    case SECT_TEMPERATE_FOREST_ROAD:
      strcpy(buf2, "<g>");
      strcpy(buf3, rp->daynightColorRoom().c_str());
      break;
    case SECT_DESERT:
    case SECT_SAVANNAH:
      strcpy(buf2, "<y>");
      strcpy(buf3, "<o>");
      break;
    case SECT_VELDT:
      strcpy(buf2, "<g>");
      strcpy(buf3, "<o>");
      break;
    case SECT_TROPICAL_CITY:
      strcpy(buf2, "<G>");
      strcpy(buf3, rp->daynightColorRoom().c_str());
      break;
    case SECT_TROPICAL_ROAD:
      strcpy(buf2, "<g>");
      strcpy(buf3, rp->daynightColorRoom().c_str());
      break;
    case SECT_JUNGLE:
      strcpy(buf2, "<P>");
      strcpy(buf3, "<g>");
      break;
    case SECT_RAINFOREST:
      strcpy(buf2, "<G>");
      strcpy(buf3, "<g>");
      break;
    case SECT_TROPICAL_HILLS:
      strcpy(buf2, "<R>");
      strcpy(buf3, "<g>");
      break;
    case SECT_TROPICAL_MOUNTAINS:
      strcpy(buf2, "<P>");
      strcpy(buf3, "<p>");
      break;
    case SECT_VOLCANO_LAVA:
      strcpy(buf2, "<y>");
      strcpy(buf3, "<R>");
      break;
    case SECT_TROPICAL_SWAMP:
      strcpy(buf2, "<G>");
      strcpy(buf3, "<g>");
      break;
    case SECT_TROPICAL_OCEAN:
      strcpy(buf2, "<b>");
      strcpy(buf3, "<c>");
      break;
    case SECT_TROPICAL_RIVER_SURFACE:
      strcpy(buf2, "<C>");
      strcpy(buf3, "<B>");
      break;
    case SECT_TROPICAL_UNDERWATER:
      strcpy(buf2, "<B>");
      strcpy(buf3, "<b>");
      break;
    case SECT_TROPICAL_BEACH:
      strcpy(buf2, "<P>");
      strcpy(buf3, "<y>");
      break;
    case SECT_TROPICAL_BUILDING:
      strcpy(buf2, "<p>");
      break;
    case SECT_TROPICAL_CAVE:
      strcpy(buf2, "<P>");
      strcpy(buf3, "<k>");
      break;
    case SECT_TROPICAL_ATMOSPHERE:
      strcpy(buf2, "<P>");
      strcpy(buf3, rp->daynightColorRoom().c_str());
      break;
    case SECT_TROPICAL_CLIMBING:
      strcpy(buf2, "<P>");
      strcpy(buf3, rp->daynightColorRoom().c_str());
      break;
    case SECT_RAINFOREST_ROAD:
      strcpy(buf2, "<P>");
      strcpy(buf3, rp->daynightColorRoom().c_str());
      break;
    case SECT_ASTRAL_ETHREAL:
      strcpy(buf2, "<C>");
      strcpy(buf3, "<c>");
      break;
    case SECT_SOLID_ROCK:
      strcpy(buf2, "<k>");
      strcpy(buf3, "<w>");
      break;
    case SECT_FIRE:
      strcpy(buf2, "<y>");
      strcpy(buf3, "<R>");
      break;
    case SECT_INSIDE_MOB:
      strcpy(buf2, "<R>");
      strcpy(buf3, "<r>");
      break;
    case SECT_FIRE_ATMOSPHERE:
      strcpy(buf2, "<y>");
      strcpy(buf3, "<R>");
      break;
    case SECT_TEMPERATE_BEACH:
    case SECT_TEMPERATE_BUILDING:
    case SECT_MAKE_FLY:
    case MAX_SECTOR_TYPES:
      strcpy(buf2, "<p>");
      break;
  }

  if (title == 1) {
    if (rp->getName()) {
      return buf2;
    } else {
      vlogf(LOG_BUG, "room without a name for dynamic coloring");
      return "";
    }
  } else if (title == 2) {
    if (rp->getDescr()) 
      return buf3;
    else {
      vlogf(LOG_BUG, "room without a descr for dynamic coloring, %s", roomp->getName());
      return "";
    }
  } else {
    vlogf(LOG_BUG, "addColorRoom without a correct title variable");
    return "";
  }
}

void TBeing::doRead(const char *argument)
{
  char buf[100];

  // This is just for now - To be changed later! 
  sprintf(buf, "at %s", argument);
  doLook(buf, CMD_READ);
}

void TBaseCup::examineObj(TBeing *ch) const
{
  int bits = FALSE;

  if (parent && (ch == parent)) {
    bits = FIND_OBJ_INV;
  } else if (equippedBy && (ch == equippedBy)) {
    bits = FIND_OBJ_EQUIP;
  } else if (parent && (ch->roomp == parent)) {
    bits = FIND_OBJ_ROOM;
  }

  ch->sendTo("When you look inside, you see:\n\r");
#if 1
  lookObj(ch, bits);
#else
  char buf[256];
  char buf2[256];
  sprintf(buf2, "%s", name);
  add_bars(buf2);
  sprintf(buf, "in %s", buf2);
  ch->doLook(buf, CMD_LOOK);
#endif
}

void TBeing::doExamine(const char *argument, TThing * specific)
{
  char caName[100], buf[100];
  int bits;
  TBeing *tmp = NULL;
  TObj *o = NULL;

  if (specific) {
    doLook("", CMD_LOOK, specific);
    return;
  }

  one_argument(argument, caName);

  if (!*caName) {
    sendTo("Examine what?\n\r");
    return;
  }
  bits = generic_find(caName, FIND_ROOM_EXTRA | FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP | FIND_CHAR_ROOM, this, &tmp, &o);

  if (bits) {
    sprintf(buf, "at %s", argument);
    doLook(buf, CMD_LOOK);
  }
  if (o) 
    o->examineObj(this);
  
  if (!bits && !o)
    sendTo("Examine what?\n\r");
}

// affect is on ch, this is person looking
string TBeing::describeAffects(TBeing *ch, showMeT showme) const
{
  affectedData *aff, *af2;
  char buf[256];
  string str;

  // limit what others can see.  Magic should reveal truth, but in general
  // keep some stuff concealed
  bool show = (ch==this) | showme;

  for (aff = ch->affected; aff; aff = af2) {
    af2 = aff->next;

    switch (aff->type) {
      case SKILL_TRACK:
      case SKILL_SEEKWATER:
        sprintf(buf,"Tracking: %s\n\r", (aff->type == SKILL_TRACK ?
               ch->specials.hunting->getName() : "seeking water"));
        str += buf;
        break;
      case SPELL_GUST:
      case SPELL_DUST_STORM:
      case SPELL_TORNADO:
      case SKILL_QUIV_PALM:
      case SKILL_SHOULDER_THROW:
      case SPELL_CALL_LIGHTNING_DEIKHAN:
      case SPELL_CALL_LIGHTNING:
      case SPELL_LIGHTNING_BREATH:
      case SPELL_GUSHER:
      case SPELL_AQUATIC_BLAST:
      case SPELL_ICY_GRIP:
      case SPELL_ARCTIC_BLAST:
      case SPELL_ICE_STORM:
      case SPELL_FROST_BREATH:
      case SPELL_WATERY_GRAVE:
      case SPELL_TSUNAMI:
      case SPELL_CHLORINE_BREATH:
      case SPELL_DUST_BREATH:
      case SPELL_POISON_DEIKHAN:
      case SPELL_POISON:
      case SPELL_ACID_BREATH:
      case SPELL_ACID_BLAST:
      case SKILL_BODYSLAM:
      case SKILL_SPIN:
      case SKILL_POWERMOVE:
      case SKILL_CHARGE:
      case SKILL_SMITE:
      case SPELL_METEOR_SWARM:
      case SPELL_EARTHQUAKE_DEIKHAN:
      case SPELL_EARTHQUAKE:
      case SPELL_PILLAR_SALT:
      case SPELL_FIREBALL:
      case SPELL_HANDS_OF_FLAME:
      case SPELL_INFERNO:
      case SPELL_HELLFIRE:
      case SPELL_RAIN_BRIMSTONE_DEIKHAN:
      case SPELL_RAIN_BRIMSTONE:
      case SPELL_FLAMESTRIKE:
      case SPELL_FIRE_BREATH:
      case SPELL_SPONTANEOUS_COMBUST:
      case SPELL_FLAMING_SWORD:
      case SPELL_FLARE:
      case SPELL_MYSTIC_DARTS:
      case SPELL_STUNNING_ARROW:
      case SPELL_COLOR_SPRAY:
      case SPELL_SAND_BLAST:
      case SPELL_PEBBLE_SPRAY:
      case SPELL_LAVA_STREAM:
      case SPELL_SLING_SHOT:
      case SPELL_GRANITE_FISTS:
      case SPELL_ENERGY_DRAIN:
      case SPELL_SYNOSTODWEOMER:
      case SPELL_HARM_DEIKHAN:
      case SPELL_HARM:
      case SPELL_HARM_LIGHT_DEIKHAN:
      case SPELL_HARM_SERIOUS_DEIKHAN:
      case SPELL_HARM_CRITICAL_DEIKHAN:
      case SPELL_HARM_LIGHT:
      case SPELL_HARM_SERIOUS:
      case SPELL_HARM_CRITICAL:
      case SPELL_WITHER_LIMB:
      case SPELL_BLEED:
      case SKILL_KICK_DEIKHAN:
      case SKILL_KICK_THIEF:
      case SKILL_KICK_MONK:
      case SKILL_KICK_RANGER:
      case SKILL_KICK:
      case SKILL_SPRINGLEAP:
      case SKILL_DEATHSTROKE:
      case SKILL_BASH_DEIKHAN:
      case SKILL_BASH_RANGER:
      case SKILL_BASH:
      case SPELL_BONE_BREAKER:
      case SPELL_PARALYZE:
      case SPELL_PARALYZE_LIMB:
      case SPELL_INFECT_DEIKHAN:
      case SPELL_INFECT:
      case SKILL_CHOP:
      case SPELL_DISEASE:
      case SPELL_SUFFOCATE:
      case SKILL_GARROTTE:
      case SKILL_STABBING:
      case SKILL_BACKSTAB:
      case SKILL_HEADBUTT:
      case SKILL_STOMP:
      case SPELL_BLAST_OF_FURY:
      case SKILL_CHI:
      case SPELL_FUMBLE:
      case SPELL_BLINDNESS:
      case SPELL_GARMULS_TAIL:
      case SPELL_SORCERERS_GLOBE:
      case SPELL_FAERIE_FIRE:
      case SPELL_ILLUMINATE:
      case SPELL_DETECT_MAGIC:
      case SPELL_MATERIALIZE:
      case SPELL_PROTECTION_FROM_EARTH:
      case SPELL_PROTECTION_FROM_AIR:
      case SPELL_PROTECTION_FROM_FIRE:
      case SPELL_PROTECTION_FROM_WATER:
      case SPELL_PROTECTION_FROM_ELEMENTS:
      case SPELL_INFRAVISION:
      case SPELL_IDENTIFY:
      case SPELL_POWERSTONE:
      case SPELL_FAERIE_FOG:
      case SPELL_TELEPORT:
      case SPELL_SENSE_LIFE:
      case SPELL_CALM:
      case SPELL_ACCELERATE:
      case SPELL_LEVITATE:
      case SPELL_FEATHERY_DESCENT:
      case SPELL_STEALTH:
      case SPELL_GILLS_OF_FLESH:
      case SPELL_AQUALUNG:
      case SPELL_TELEPATHY:
      case SPELL_FEAR:
      case SPELL_SLUMBER:
      case SPELL_CONJURE_EARTH:
      case SPELL_ENTHRALL_SPECTRE:
      case SPELL_ENTHRALL_GHAST:
      case SPELL_ENTHRALL_GHOUL:
      case SPELL_ENTHRALL_DEMON:
      case SPELL_CONJURE_AIR:
      case SPELL_CONJURE_FIRE:
      case SPELL_CONJURE_WATER:
      case SPELL_DISPEL_MAGIC:
      case SPELL_ENHANCE_WEAPON:
      case SPELL_GALVANIZE:
      case SPELL_DETECT_INVISIBLE:
      case SPELL_DISPEL_INVISIBLE:
      case SPELL_FARLOOK:
      case SPELL_FALCON_WINGS:
      case SPELL_INVISIBILITY:
      case SPELL_ENSORCER:
      case SPELL_EYES_OF_FERTUMAN:
      case SPELL_COPY:
      case SPELL_HASTE:
      case SPELL_IMMOBILIZE:
      case SPELL_FLY:
      case SPELL_ANTIGRAVITY:
      case SPELL_DIVINATION:
      case SPELL_SHATTER:
      case SKILL_SCRIBE:
      case SPELL_SPONTANEOUS_GENERATION:
      case SPELL_STONE_SKIN:
      case SPELL_TRAIL_SEEK:
      case SPELL_FLAMING_FLESH:
      case SPELL_ATOMIZE:
      case SPELL_ANIMATE:
      case SPELL_BIND:
      case SPELL_TRUE_SIGHT:
      case SPELL_CLOUD_OF_CONCEALMENT:
      case SPELL_POLYMORPH:
      case SPELL_SILENCE:
      case SPELL_BREATH_OF_SARAHAGE:
      case SPELL_PLASMA_MIRROR:
      case SPELL_THORNFLESH:
      case SPELL_ETHER_GATE:
      case SPELL_HEAL_LIGHT:
      case SPELL_CREATE_FOOD:
      case SPELL_CREATE_WATER:
      case SPELL_ARMOR:
      case SPELL_BLESS:
      case SPELL_CLOT:
      case SPELL_HEAL_SERIOUS:
      case SPELL_STERILIZE:
      case SPELL_EXPEL:
      case SPELL_CURE_DISEASE:
      case SPELL_CURSE:
      case SPELL_REMOVE_CURSE:
      case SPELL_CURE_POISON:
      case SPELL_HEAL_CRITICAL:
      case SPELL_SALVE:
      case SPELL_REFRESH:
      case SPELL_NUMB:
      case SPELL_PLAGUE_LOCUSTS:
      case SPELL_CURE_BLINDNESS:
      case SPELL_SUMMON:
      case SPELL_HEAL:
      case SPELL_WORD_OF_RECALL:
      case SPELL_SANCTUARY:
      case SPELL_CURE_PARALYSIS:
      case SPELL_SECOND_WIND:
      case SPELL_HEROES_FEAST:
      case SPELL_ASTRAL_WALK:
      case SPELL_PORTAL:
      case SPELL_HEAL_FULL:
      case SPELL_HEAL_CRITICAL_SPRAY:
      case SPELL_HEAL_SPRAY:
      case SPELL_HEAL_FULL_SPRAY:
      case SPELL_RESTORE_LIMB:
      case SPELL_KNIT_BONE:
      case SPELL_SHIELD_OF_MISTS:
      case SKILL_RESCUE:
      case SKILL_SMYTHE:
      case SKILL_SACRIFICE:
      case SKILL_DISARM:
      case SKILL_PARRY_WARRIOR:
      case SKILL_DUAL_WIELD_WARRIOR:
      case SKILL_BERSERK:
      case SKILL_SWITCH_OPP:
      case SKILL_KNEESTRIKE:
      case SKILL_SHOVE:
      case SKILL_RETREAT:
      case SKILL_GRAPPLE:
      case SKILL_DOORBASH:
      case SKILL_TRANCE_OF_BLADES:
      case SKILL_HIKING:
      case SKILL_FORAGE:
      case SKILL_TRANSFORM_LIMB:
      case SKILL_BEAST_SOOTHER:
      case SPELL_ROOT_CONTROL:
      case SKILL_RESCUE_RANGER:
      case SKILL_BEFRIEND_BEAST:
      case SKILL_TRANSFIX:
      case SKILL_SKIN:
      case SKILL_DUAL_WIELD:
      case SPELL_LIVING_VINES:
      case SKILL_BEAST_SUMMON:
      case SKILL_BARKSKIN:
      case SKILL_SWITCH_RANGER:
      case SKILL_RETREAT_RANGER:
      case SPELL_STICKS_TO_SNAKES:
      case SPELL_STORMY_SKIES:
      case SPELL_TREE_WALK:
      case SKILL_BEAST_CHARM:
      case SPELL_SHAPESHIFT:
      case SKILL_CONCEALMENT:
      case SKILL_APPLY_HERBS:
      case SKILL_DIVINATION:
      case SKILL_ENCAMP:
      case SPELL_HEAL_LIGHT_DEIKHAN:
      case SKILL_CHIVALRY:
      case SPELL_ARMOR_DEIKHAN:
      case SPELL_BLESS_DEIKHAN:
      case SPELL_EXPEL_DEIKHAN:
      case SPELL_CLOT_DEIKHAN:
      case SPELL_STERILIZE_DEIKHAN:
      case SPELL_REMOVE_CURSE_DEIKHAN:
      case SPELL_CURSE_DEIKHAN:
      case SKILL_RESCUE_DEIKHAN:
      case SPELL_CURE_DISEASE_DEIKHAN:
      case SPELL_CREATE_FOOD_DEIKHAN:
      case SPELL_HEAL_SERIOUS_DEIKHAN:
      case SPELL_CURE_POISON_DEIKHAN:
      case SKILL_DISARM_DEIKHAN:
      case SPELL_HEAL_CRITICAL_DEIKHAN:
      case SKILL_SWITCH_DEIKHAN:
      case SKILL_RETREAT_DEIKHAN:
      case SKILL_SHOVE_DEIKHAN:
      case SKILL_RIDE:
      case SKILL_CALM_MOUNT:
      case SKILL_TRAIN_MOUNT:
      case SKILL_ADVANCED_RIDING:
      case SKILL_RIDE_DOMESTIC:
      case SKILL_RIDE_NONDOMESTIC:
      case SKILL_RIDE_WINGED:
      case SPELL_CREATE_WATER_DEIKHAN:
      case SKILL_RIDE_EXOTIC:
      case SPELL_HEROES_FEAST_DEIKHAN:
      case SPELL_REFRESH_DEIKHAN:
      case SPELL_SALVE_DEIKHAN:
      case SKILL_LAY_HANDS:
      case SPELL_NUMB_DEIKHAN:
      case SKILL_YOGINSA:
      case SKILL_CINTAI:
      case SKILL_OOMLAT:
      case SKILL_ADVANCED_KICKING:
      case SKILL_DISARM_MONK:
      case SKILL_GROUNDFIGHTING:
      case SKILL_DUFALI:
      case SKILL_RETREAT_MONK:
      case SKILL_SNOFALTE:
      case SKILL_COUNTER_MOVE:
      case SKILL_SWITCH_MONK:
      case SKILL_JIRIN:
      case SKILL_KUBO:
      case SKILL_CATFALL:
      case SKILL_WOHLIN:
      case SKILL_VOPLAT:
      case SKILL_BLINDFIGHTING:
      case SKILL_CRIT_HIT:
      case SKILL_FEIGN_DEATH:
      case SKILL_BLUR:
      case SKILL_HURL:
      case SKILL_SWINDLE:
      case SKILL_SNEAK:
      case SKILL_RETREAT_THIEF:
      case SKILL_PICK_LOCK:
      case SKILL_SEARCH:
      case SKILL_SPY:
      case SKILL_SWITCH_THIEF:
      case SKILL_STEAL:
      case SKILL_DETECT_TRAP:
      case SKILL_SUBTERFUGE:
      case SKILL_DISARM_TRAP:
      case SKILL_CUDGEL:
      case SKILL_HIDE:
      case SKILL_POISON_WEAPON:
      case SKILL_DISGUISE:
      case SKILL_DODGE_THIEF:
      case SKILL_SET_TRAP_CONT:
      case SKILL_SET_TRAP_DOOR:
      case SKILL_SET_TRAP_MINE:
      case SKILL_SET_TRAP_GREN:
      case SKILL_DUAL_WIELD_THIEF:
      case SKILL_DISARM_THIEF:
      case SKILL_COUNTER_STEAL:
      case SPELL_CACAODEMON:
      case SPELL_CREATE_GOLEM:
      case SPELL_DANCING_BONES:
      case SPELL_CONTROL_UNDEAD:
      case SPELL_RESURRECTION:
      case SPELL_VOODOO:
      case SKILL_BREW:
      case SPELL_VAMPIRIC_TOUCH:
      case SPELL_LIFE_LEECH:
      case SKILL_TURN:
      case SKILL_SIGN:
      case SKILL_SWIM:
      case SKILL_CONS_UNDEAD:
      case SKILL_CONS_VEGGIE:
      case SKILL_CONS_DEMON:
      case SKILL_CONS_ANIMAL:
      case SKILL_CONS_REPTILE:
      case SKILL_CONS_PEOPLE:
      case SKILL_CONS_GIANT:
      case SKILL_CONS_OTHER:
      case SKILL_READ_MAGIC:
      case SKILL_BANDAGE:
      case SKILL_CLIMB:
      case SKILL_FAST_HEAL:
      case SKILL_EVALUATE:
      case SKILL_TACTICS:
      case SKILL_DISSECT:
      case SKILL_DEFENSE:
      case SKILL_OFFENSE:
      case SKILL_WHITTLE:
      case SKILL_WIZARDRY:
      case SKILL_MEDITATE:
      case SKILL_DEVOTION:
      case SKILL_PENANCE:
      case SKILL_SLASH_PROF:
      case SKILL_PIERCE_PROF:
      case SKILL_BLUNT_PROF:
      case SKILL_BAREHAND_PROF:
      case SKILL_SLASH_SPEC:
      case SKILL_BLUNT_SPEC:
      case SKILL_PIERCE_SPEC:
      case SKILL_BAREHAND_SPEC:
      case SKILL_RANGED_SPEC:
      case SKILL_RANGED_PROF:
      case SKILL_FAST_LOAD:
      case SKILL_SHARPEN:
      case SKILL_DULL:
      case SKILL_ATTUNE:
      case SKILL_STAVECHARGE:
#if 1
      case SPELL_EARTHMAW:
      case SPELL_CREEPING_DOOM:
      case SPELL_FERAL_WRATH:
      case SPELL_SKY_SPIRIT:
#endif
        // some spells have 2 effects, skip over one of them
        if (!aff->shouldGenerateText())
          continue;
        else if (discArray[aff->type]) {
          if (show && strcmp(discArray[aff->type]->name, "sneak")) {
            if (aff->renew < 0) {
              sprintf(buf,"Affected : '%s'\t: Approx. Duration : %s\n\r",
                   discArray[aff->type]->name,
                   describeDuration(this, aff->duration).c_str());
  
            } else {
              sprintf(buf,"Affected : '%s'\t: Time Left : %s %s\n\r",
                 discArray[aff->type]->name,
                 describeDuration(this, aff->duration).c_str(), 
                 (aff->canBeRenewed() ? "(Renewable)" : "(Not Yet Renewable)"));
            }
            str += buf;
          }
        } else {
          forceCrash("BOGUS AFFECT (%d) on %s.", aff->type, ch->getName());
          ch->affectRemove(aff);
        }
        break;
      case AFFECT_DISEASE:
        if (show) {
          sprintf(buf, "Disease: '%s'\n\r",
                  DiseaseInfo[affToDisease(*aff)].name);
          str += buf;
        } 
        break;
      case AFFECT_DUMMY:
        if (show) {
          sprintf(buf, "Affected : '%s'\t: Time Left : %s %s\n\r",
                 "DUMMY",
                 describeDuration(this, aff->duration).c_str(),
                 (aff->canBeRenewed() ? "(Renewable)" : "(Not Yet Renewable)"));
          str += buf;
        }
        break;
      case AFFECT_FREE_DEATHS:
        sprintf(buf, "Free deaths remaining: %ld\n\r",
               aff->modifier);
        str += buf;
        break;
      case AFFECT_HORSEOWNED:
        sprintf(buf, "Horseowned:\t Time Left : %s\n\r",
  	     describeDuration(this, aff->duration).c_str());
        str += buf;
        break;
      case AFFECT_PLAYERKILL:
        sprintf(buf, "Player Killer:\t Time Left : %s\n\r",
	     describeDuration(this, aff->duration).c_str());
        str += buf;
        break;
      case AFFECT_PLAYERLOOT:
        sprintf(buf, "Player Looter:\t Time Left : %s\n\r",
                describeDuration(this, aff->duration).c_str());
        str += buf;
        break;
      case AFFECT_TEST_FIGHT_MOB:
        sprintf(buf, "Test Fight Mob: %ld\n\r",
               aff->modifier);
        str += buf;
        break;
      case AFFECT_SKILL_ATTEMPT:
        if (isImmortal()) {
          sprintf(buf, "Skill Attempt:(%ld) '%s'\t: Time Left : %s\n\r", aff->modifier, (discArray[aff->modifier] ? discArray[aff->modifier]->name : "Unknown"), describeDuration(this, aff->duration).c_str());
          str += buf;
        } else if (aff->modifier != getSkillNum(SKILL_SNEAK)) {
          sprintf(buf, "Skill Attempt: '%s'\t: Time Left : %s\n\r", (discArray[aff->modifier] ? discArray[aff->modifier]->name : "Unknown"), describeDuration(this, aff->duration).c_str());
          str += buf;
        }
        break;
      case AFFECT_NEWBIE:
        if (show) {
          sprintf(buf, "Donation Recipient: \n\r");
          str += buf;
        }
        break;
      case AFFECT_DRUNK:
        if (show) {
          sprintf(buf, "Affected: Drunken Slumber: approx. duration : %s\n\r",
                 describeDuration(this, aff->duration).c_str());
          str += buf;
        } else {
          sprintf(buf, "Affected: Drunken Slumber: \n\r");
          str += buf;
        }
        break;
      case AFFECT_DRUG:
        if (!aff->shouldGenerateText())
          continue;
        if (show) {
          sprintf(buf, "Affected: %s: approx. duration : %s\n\r",
  	       drugTypes[aff->modifier2].name,
	       describeDuration(this, aff->duration).c_str());
          str += buf;
        } else {
          sprintf(buf, "Affected: %s: \n\r", drugTypes[aff->modifier2].name);
          str += buf;
        }
        break;
      case AFFECT_TRANSFORMED_ARMS:
        if (show) {
          sprintf(buf, "Affected: Transformed Limb: falcon wings: approx. duration : %s\n\r",
                 describeDuration(this, aff->duration).c_str());
        } else {
          sprintf(buf, "Affected: Transformed Limb: falcon wings: \n\r");
        }
        str += buf;
        break;
      case AFFECT_TRANSFORMED_HANDS:
        if (ch == this)
          sprintf(buf, "Affected: Transformed Limb: bear claws: approx. duration : %s\n\r",
                 describeDuration(this, aff->duration).c_str());
        else
          sprintf(buf, "Affected: Transformed Limb: bear claws \n\r");
        str += buf;
        break;
      case AFFECT_TRANSFORMED_LEGS:
        if (ch == this)
          sprintf(buf, "Affected: Transformed Limb: dolphin tail: approx. duration : %s\n\r",
                 describeDuration(this, aff->duration).c_str());
        else
          sprintf(buf, "Affected: Transformed Limb: dolphin tail: \n\r");
        str += buf;
        break;
      case AFFECT_TRANSFORMED_HEAD:
        if (ch == this)
          sprintf(buf, "Affected: Transformed Limb: eagle's head: approx. duration : %s\n\r",
                 describeDuration(this, aff->duration).c_str());
        else
          sprintf(buf, "Affected: Transformed Limb: eagle's head: \n\r");
        break;
      case AFFECT_TRANSFORMED_NECK:
        if (ch == this)
          sprintf(buf, "Affected: Transformed Limb: fish gills: approx. duration : %s\n\r",
                 describeDuration(this, aff->duration).c_str());
        else
          sprintf(buf, "Affected: Transformed Limb: fish gills: \n\r");
        str += buf;
        break;
      case AFFECT_GROWTH_POTION:
        if (ch == this)
          sprintf(buf, "Affected: Abnormal Growth: approx duration : %s\n\r",
                  describeDuration(this, aff->duration).c_str());
        else
          sprintf(buf, "Affected: Abnormal Growth\n\r");

        str += buf;
        break;

      case AFFECT_COMBAT:
        // no display
        break;
      case AFFECT_PET:
        if (show) {
          sprintf(buf, "Pet of: '%s'.  Approx. duration : %s\n\r",
                 (char *) aff->be,
                 describeDuration(this, aff->duration).c_str());
        } else {
          sprintf(buf, "Somebody's Pet.\n\r");
        }
        str += buf;
        break;
      case AFFECT_CHARM:
        if (show) {
          sprintf(buf, "Charm of: '%s'.  Approx. duration : %s\n\r",
                 (char *) aff->be,
                 describeDuration(this, aff->duration).c_str());
        } else {
          sprintf(buf, "Somebody's Charm.\n\r");
        }
        str += buf;
        break;
      case AFFECT_THRALL:
        if (show) {
          sprintf(buf, "Thrall of: '%s'.  Approx. duration : %s\n\r",
                 (char *) aff->be,
                 describeDuration(this, aff->duration).c_str());
        } else {
          sprintf(buf, "Somebody's Thrall.\n\r");
        }
        str += buf;
        break;
      case AFFECT_ORPHAN_PET:
        // no display
        break;

      // cases beyond here are considered BOGUS
      case LAST_ODDBALL_AFFECT:
      case LAST_TRANSFORMED_LIMB:
      case LAST_BREATH_WEAPON:
      case DAMAGE_GUST:
      case DAMAGE_TRAP_TNT:
      case DAMAGE_ELECTRIC:
      case DAMAGE_TRAP_FROST:
      case DAMAGE_FROST:
      case DAMAGE_DROWN:
      case DAMAGE_WHIRLPOOL:
      case DAMAGE_HEMORRAGE:
      case DAMAGE_IMPALE:
      case DAMAGE_TRAP_POISON:
      case DAMAGE_ACID:
      case DAMAGE_TRAP_ACID:
      case DAMAGE_COLLISION:
      case DAMAGE_FALL:
      case DAMAGE_TRAP_BLUNT:
      case DAMAGE_TRAP_FIRE:
      case DAMAGE_FIRE:
      case DAMAGE_DISRUPTION:
      case DAMAGE_DRAIN:
      case DAMAGE_TRAP_ENERGY:
      case DAMAGE_KICK_HEAD:
      case DAMAGE_KICK_SHIN:
      case DAMAGE_KICK_SIDE:
      case DAMAGE_KICK_SOLAR:
      case DAMAGE_TRAP_DISEASE:
      case DAMAGE_SUFFOCATION:
      case DAMAGE_TRAP_SLASH:
      case DAMAGE_ARROWS:
      case DAMAGE_TRAP_PIERCE:
      case DAMAGE_DISEMBOWLED_HR:
      case DAMAGE_DISEMBOWLED_VR:
      case DAMAGE_EATTEN:
      case DAMAGE_HACKED:
      case DAMAGE_KNEESTRIKE_FOOT:
      case DAMAGE_HEADBUTT_FOOT:
      case DAMAGE_KNEESTRIKE_SHIN:
      case DAMAGE_KNEESTRIKE_KNEE:
      case DAMAGE_KNEESTRIKE_THIGH:
      case DAMAGE_HEADBUTT_LEG:
      case DAMAGE_KNEESTRIKE_SOLAR:
      case DAMAGE_HEADBUTT_BODY:
      case DAMAGE_KNEESTRIKE_CROTCH:      
      case DAMAGE_HEADBUTT_CROTCH:
      case DAMAGE_HEADBUTT_THROAT:
      case DAMAGE_KNEESTRIKE_CHIN:
      case DAMAGE_HEADBUTT_JAW:
      case DAMAGE_KNEESTRIKE_FACE:
      case DAMAGE_CAVED_SKULL:
      case DAMAGE_HEADBUTT_SKULL:
      case DAMAGE_STARVATION:
      case DAMAGE_STOMACH_WOUND:
      case DAMAGE_RAMMED:
      case DAMAGE_BEHEADED:
      case DAMAGE_NORMAL:
      case DAMAGE_TRAP_SLEEP:
      case DAMAGE_TRAP_TELEPORT:
      case MAX_SKILL:
      case TYPE_WATER:
      case TYPE_AIR:
      case TYPE_EARTH:
      case TYPE_FIRE:
      case TYPE_KICK:
      case TYPE_CLAW:
      case TYPE_SLASH:
      case TYPE_CLEAVE:
      case TYPE_SLICE:
      case TYPE_BEAR_CLAW:
      case TYPE_MAUL:
      case TYPE_SMASH:
      case TYPE_WHIP:
      case TYPE_CRUSH:
      case TYPE_BLUDGEON:
      case TYPE_SMITE:
      case TYPE_HIT:
      case TYPE_FLAIL:
      case TYPE_PUMMEL:
      case TYPE_THRASH:
      case TYPE_THUMP:
      case TYPE_WALLOP:
      case TYPE_BATTER:
      case TYPE_BEAT:
      case TYPE_STRIKE:
      case TYPE_POUND:
      case TYPE_CLUB:
      case TYPE_PIERCE:
      case TYPE_STAB:
      case TYPE_STING:
      case TYPE_THRUST:
      case TYPE_SPEAR:
      case TYPE_BEAK:
      case TYPE_BITE:
      case TYPE_UNDEFINED:
      case TYPE_MAX_HIT:
      case SKILL_ALCOHOLISM:
      case SKILL_FISHING:
        forceCrash("BOGUS AFFECT (%d) on %s.", aff->type, ch->getName());
        ch->affectRemove(aff);
        break;
    }
  }
  return str;
}

void TBeing::describeLimbDamage(const TBeing *ch) const
{
  char buf[256], buf2[80];
  wearSlotT j;
  TThing *t;

  if (ch == this)
    strcpy(buf2,"your");
  else
    strcpy(buf2,ch->hshr());

  for (j = MIN_WEAR; j < MAX_WEAR; j++) {
    if (j == HOLD_RIGHT || j == HOLD_LEFT)
      continue;
    if (!ch->slotChance(j))
      continue;
    if (ch->isLimbFlags(j, ~PART_TRANSFORMED)) {
      const string str = describe_part_wounds(ch, j);
      if (!str.empty()) {
        sprintf(buf, "<y>%s %s %s %s<1>", cap(buf2), 
               ch->describeBodySlot(j).c_str(),
               ch->slotPlurality(j).c_str(), 
               str.c_str());
        act(buf, FALSE, this, NULL, NULL, TO_CHAR);
      }
    }
    if ((t = ch->getStuckIn(j))) {
      if (canSee(t)) {
        sprintf(buf, "<y>$p is sticking out of %s %s!<1>",
                uncap(buf2), ch->describeBodySlot(j).c_str());
        act(buf, FALSE, this, t, NULL, TO_CHAR);
      }
    }
  }
  if (ch->affected) {
    affectedData *aff;
    for (aff = ch->affected; aff; aff = aff->next) {
      if (aff->type == AFFECT_DISEASE) {
        if (!aff->level) {
          if (ch == this)
            sendTo(COLOR_BASIC, "<y>You have %s.<1>\n\r",
               DiseaseInfo[affToDisease(*aff)].name);
          else
            sendTo(COLOR_BASIC, "<y>It seems %s has %s.<1>\n\r",
                ch->hssh(), DiseaseInfo[affToDisease(*aff)].name);
        }
      }
    }
  }
}

void TBeing::doTime(const char *argument)
{
  char buf[100], arg[160];
  int weekday, day, tmp2;

  if (!desc) {
    sendTo("Silly mob, go home.\n\r");
    return;
  }

  one_argument(argument, arg);
  if (*arg) {
    if (!atoi(arg) && strcmp(arg, "0")) {
      sendTo("Present time differential is set to %d hours.\n\r", desc->account->time_adjust);
      sendTo("Syntax: time <difference>\n\r");
      return;
    }
    desc->account->time_adjust = atoi(arg);
    sendTo("Your new time difference between your site and %s's will be: %d hours.\n\r", MUD_NAME, desc->account->time_adjust);
    desc->saveAccount();
    return;
  }
  sprintf(buf, "It is %s, on ",
          hmtAsString(hourminTime()).c_str());

  weekday = ((28 * time_info.month) + time_info.day + 1) % 7;        // 28 days in a month 

  strcat(buf, weekdays[weekday]);
  strcat(buf, "\n\r");
  sendTo(buf);

  day = time_info.day + 1;        // day in [1..28] 

  sendTo("The %s day of %s, Year %d P.S.\n\r", 
           numberAsString(day).c_str(),
           month_name[time_info.month], time_info.year);

  tmp2 = sunTime(SUN_TIME_RISE);
  sprintf(buf, "The sun will rise today at:   %s.\n\r",
       hmtAsString(tmp2).c_str());
  sendTo(buf);

  tmp2 = sunTime(SUN_TIME_SET);
  sprintf(buf, "The sun will set today at:    %s.\n\r",
       hmtAsString(tmp2).c_str());
  sendTo(buf);

  tmp2 = moonTime(MOON_TIME_RISE);
  sprintf(buf, "The moon will rise today at:  %s    (%s).\n\r",
       hmtAsString(tmp2).c_str(), moonType());
  sendTo(buf);

  tmp2 = moonTime(MOON_TIME_SET);
  sprintf(buf, "The moon will set today at:   %s.\n\r",
       hmtAsString(tmp2).c_str());
  sendTo(buf);

  time_t ct;
  char *tmstr;
  if (desc->account)
    ct = time(0) + 3600 * desc->account->time_adjust;
  else
    ct = time(0);
  tmstr = asctime(localtime(&ct));
  *(tmstr + strlen(tmstr) - 1) = '\0';
  sendTo("%sIn the real world, the time is:                     %s%s\n\r", 
        blue(), tmstr, norm());

  if (timeTill) {
    sendTo("%sThe game will be shutdown in %s.\n\r%s",
           red(),
           secsToString(timeTill - time(0)).c_str(),
           norm());
  }
}

void TBeing::doWeather(const char *arg)
{
  char buf[80];
  char buffer[256];
  changeWeatherT change = WEATHER_CHANGE_NONE;
 
  arg = one_argument(arg, buffer);

  if (!*buffer || !isImmortal()) {
    if (roomp->getWeather() == WEATHER_SNOWY)
      strcpy(buf,"It is snowing");
    else if (roomp->getWeather() == WEATHER_LIGHTNING)
      strcpy(buf,"The sky is lit by flashes of lightning as a heavy rain pours
down");
    else if (roomp->getWeather() == WEATHER_RAINY)
      strcpy(buf,"It is raining");
    else if (roomp->getWeather() == WEATHER_CLOUDY)
      strcpy(buf,"The sky is cloudy");
    else if (roomp->getWeather() == WEATHER_CLOUDLESS) {
      if (is_nighttime())
        strcpy(buf,"A <K>dark sky<1> stretches before you");
      else
        strcpy(buf,"A <b>clear blue sky<1> stretches before you");
//  } else if (roomp->getWeather() == WEATHER_WINDY) {
//    strcpy(buf,"The winds begin to pick up");
    } else if (roomp->getWeather() == WEATHER_NONE) {
      sendTo("You have no feeling about the weather at all.\n\r");
      describeRoomLight();
      return;
    } else {
      vlogf(LOG_BUG,"Error in getWeather for %s.",getName());
      return;
    }
    if (isImmortal()) {
      sendTo("The current barometer is: %d.  Barometric change is: %d\n\r",
            weather_info.pressure, weather_info.change); 
    }
    sendTo(COLOR_BASIC, "%s and %s.\n\r", buf,
        (weather_info.change >= 0 ? "you feel a relatively warm wind from the south" :
         "your foot tells you bad weather is due"));

    if (moonIsUp()) {
      sendTo("A %s moon hangs in the sky.\n\r", moonType());
    }
    describeRoomLight();
    return;
  } else {
    if (is_abbrev(buffer, "worse")) {
      weather_info.change = -10;
      sendTo("The air-pressure drops and the weather worsens.\n\r");
      weather_info.pressure += weather_info.change;

      AlterWeather(&change);
      if (weather_info.pressure >= 1040) {
        sendTo("The weather can't get any better.\n\r");
        weather_info.pressure = 1040;
      } else if (weather_info.pressure <= 960) {
        sendTo("The weather can't get any worse.\n\r");
        weather_info.pressure = 960;
      }
      return;
    } else if (is_abbrev(buffer, "better")) {
      weather_info.change = +10;
      sendTo("The air-pressure climbs and the weather improves.\n\r");
      weather_info.pressure += weather_info.change;

      AlterWeather(&change);
      if (weather_info.pressure >= 1040) {
        sendTo("The weather can't get any better.\n\r");
        weather_info.pressure = 1040;
      } else if (weather_info.pressure <= 960) {
        sendTo("The weather can't get any worse.\n\r");
        weather_info.pressure = 960;
      }
    } else if (is_abbrev(buffer, "month")) {
      arg = one_argument(arg, buffer);
      if (!*buffer) {
        sendTo("Syntax: weather month <num>\n\r");
        return;
      }
      int num = atoi(buffer);
      if (num <= 0 || num > 12) {
        sendTo("Syntax: weather month <num>\n\r");
        sendTo("<num> must be in range 1-12.\n\r");
        return;
      }
      time_info.month = num - 1 ;
      sendTo("You set the month to: %s\n\r", month_name[time_info.month]);
      return;
    } else if (is_abbrev(buffer, "moon")) {
      arg = one_argument(arg, buffer);
      if (!*buffer) {
        sendTo("Syntax: weather moon <num>\n\r");
        return;
      }
      int num = atoi(buffer);
      if (num <= 0 || num > 32) {
        sendTo("Syntax: weather moon <num>\n\r");
        sendTo("<num> must be in range 1-32.\n\r");
        return;
      }
      moontype = num;
      sendTo("The moon is now in stage %d (%s).\n\r", moontype, moonType());
      return;
    } else {
      sendTo("Syntax: weather <\"worse\" | \"better\" | \"month\" | \"moon\">\n\r");
      return;
    }
  }
}

wizPowerT wizPowerFromCmd(cmdTypeT cmd)
{
  switch (cmd) {
    case CMD_CHANGE:
      return POWER_CHANGE;
      break;
    case CMD_ECHO:
      return POWER_ECHO;
      break;
    case CMD_FORCE:
      return POWER_FORCE;
      break;
    case CMD_TRANSFER:
      return POWER_TRANSFER;
      break;
    case CMD_STAT:
      return POWER_STAT;
      break;
    case CMD_LOAD:
      return POWER_LOAD;
      break;
    case CMD_PURGE:
      return POWER_PURGE;
      break;
    case CMD_AT:
      return POWER_AT;
      break;
    case CMD_SNOOP:
      return POWER_SNOOP;
      break;
    case CMD_AS:
    case CMD_SWITCH:
      return POWER_SWITCH;
      break;
    case CMD_SNOWBALL:
      return POWER_SNOWBALL;
      break;
    case CMD_INFO:
      return POWER_INFO;
      break;
    case CMD_WHERE:
      return POWER_WHERE;
      break;
    case CMD_PEE:
      return POWER_PEE;
      break;
    case CMD_WIZNET:
      return POWER_WIZNET;
      break;
    case CMD_RESTORE:
      return POWER_RESTORE;
      break;
    case CMD_USERS:
      return POWER_USERS;
      break;
    case CMD_SYSTEM:
      return POWER_SYSTEM;
      break;
    case CMD_STEALTH:
      return POWER_STEALTH;
      break;
    case CMD_SET:
      return POWER_SET;
      break;
    case CMD_RSAVE:
      return POWER_RSAVE;
      break;
    case CMD_RLOAD:
      return POWER_RLOAD;
      break;
    case CMD_WIZLOCK:
      return POWER_WIZLOCK;
      break;
    case CMD_SHOW:
      return POWER_SHOW;
      break;
    case CMD_TOGGLE:
      return POWER_TOGGLE;
      break;
    case CMD_BREATH:
      return POWER_BREATHE;
      break;
    case CMD_LOG:
      return POWER_LOG;
      break;
    case CMD_WIPE:
      return POWER_WIPE;
      break;
    case CMD_CUTLINK:
      return POWER_CUTLINK;
      break;
    case CMD_CHECKLOG:
      return POWER_CHECKLOG;
      break;
    case CMD_LOGLIST:
      return POWER_LOGLIST;
      break;
    case CMD_DEATHCHECK:
      return POWER_DEATHCHECK;
      break;
    case CMD_REDIT:
      return POWER_REDIT;
      break;
    case CMD_OEDIT:
      return POWER_OEDIT;
      break;
    case CMD_MEDIT:
      return POWER_MEDIT;
      break;
    case CMD_ACCESS:
      return POWER_ACCESS;
      break;
    case CMD_REPLACE:
      return POWER_REPLACE;
      break;
    case CMD_GAMESTATS:
      return POWER_GAMESTATS;
      break;
    case CMD_HOSTLOG:
      return POWER_HOSTLOG;
      break;
    case CMD_TRACEROUTE:
      return POWER_TRACEROUTE;
      break;
    case CMD_LOW:
      return POWER_LOW;
      break;
    case CMD_RESIZE:
      return POWER_RESIZE;
      break;
    case CMD_HEAVEN:
      return POWER_HEAVEN;
      break;
    case CMD_ACCOUNT:
      return POWER_ACCOUNT;
      break;
    case CMD_CLIENTS:
      return POWER_CLIENTS;
      break;
    case CMD_FINDEMAIL:
      return POWER_FINDEMAIL;
      break;
    case CMD_COMMENT:
      return POWER_COMMENT;
      break;
    case CMD_EGOTRIP:
      return POWER_EGOTRIP;
      break;
    case CMD_POWERS:
      return POWER_POWERS;
      break;
    case CMD_SEDIT:
      return POWER_SEDIT;
      break;
    case CMD_SHUTDOWN:
    case CMD_SHUTDOW:
      return POWER_SHUTDOWN;
      break;
    case CMD_RESET:
      return POWER_RESET;
      break;
    case CMD_SLAY:
      return POWER_SLAY;
      break;
    case CMD_TIMESHIFT:
      return POWER_TIMESHIFT;
      break;
    case CMD_CRIT:
      return POWER_CRIT;
      break;
    case CMD_RELEASE: // ???
    case CMD_CAPTURE: // ???
    case CMD_CREATE:  // Lapsos
    case CMD_TASKS:
    case CMD_TEST_FIGHT:
    case CMD_PEELPK:
    case CMD_TESTCODE:
    case CMD_BRUTTEST:
      return POWER_WIZARD;
      break;
    break;
    default:
      break;
  }

  return MAX_POWER_INDEX;
}

void TBeing::doWizhelp()
{
  char      buf[MAX_STRING_LENGTH],
            tString[MAX_STRING_LENGTH];
  int       no,
            tLength = 2;
  unsigned int i;
  wizPowerT tPower;

  if (!isImmortal())
    return;

  for (i = 0; i < MAX_CMD_LIST; i++) {
    if (!commandArray[i])
      continue;

    if ((GetMaxLevel() >= commandArray[i]->minLevel) &&
        (commandArray[i]->minLevel > MAX_MORT) &&
        ((tPower = wizPowerFromCmd(cmdTypeT(i))) == MAX_POWER_INDEX ||
         hasWizPower(tPower)))
      tLength = max(strlen(commandArray[i]->name), (unsigned) tLength);
  }

  sprintf(tString, "%%-%ds", (tLength + 1));
  tLength = (79 / tLength);

  sendTo("The following privileged commands are available:\n\r\n\r");

  *buf = '\0';

  if ((tPower = wizPowerFromCmd(CMD_AS)) == MAX_POWER_INDEX ||
      hasWizPower(tPower))
    sprintf(buf, tString, "as");

  for (no = 2, i = 0; i < MAX_CMD_LIST; i++) {
    if (!commandArray[i])
      continue;

    if ((GetMaxLevel() >= commandArray[i]->minLevel) &&
        (commandArray[i]->minLevel > MAX_MORT) &&
        ((tPower = wizPowerFromCmd(cmdTypeT(i))) == MAX_POWER_INDEX ||
         hasWizPower(tPower))) {

      sprintf(buf + strlen(buf), tString, commandArray[i]->name);

      if (!(no % (tLength - 1)))
        strcat(buf, "\n\r");

      no++;
    }
  }

  strcat(buf, "\n\r      Check out HELP GODS (or HELP BUILDERS) for an index of help files.\n\r");
  desc->page_string(buf);
}

void TBeing::doUsers(const char *)
{
  sendTo("Dumb monsters can't use the users command!\n\r");
}

void TPerson::doUsers(const char *argument)
{
  char line[200], buf2[100], buf3[100], buf4[10];
  Descriptor *d;
  int count = 0;
  string sb;
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
  TBeing *k = NULL;

  if (powerCheck(POWER_USERS))
    return;

  const char USERS_HEADER[] = "\n\rName              Hostname                           Connected  Account Name\n\r--------------------------------------------------------------------------------\n\r";

  argument = two_arg(argument, arg1, arg2);

  *line = '\0';

  if (!*arg1 || !arg1) {
    sb += USERS_HEADER;

    for (d = descriptor_list; d; d = d->next) {
      if (d->character && d->character->name) {
        if (!d->connected && !canSeeWho(d->character))
          continue;

        if (d->original)
          sprintf(line, "%s%-16.16s%s: ",purple(), d->original->name, norm());
        else
          sprintf(line, "%s%-16.16s%s: ",purple(), d->character->name, norm());
      } else
        strcpy(line, "UNDEFINED       : ");

      // don't let newbie gods blab who imm's mortals are
      if (d->account && IS_SET(d->account->flags, ACCOUNT_IMMORTAL) && 
            !hasWizPower(POWER_VIEW_IMM_ACCOUNTS)) {
        sprintf(line + strlen(line), "*** Information Concealed ***\n\r");
      } else {
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	dbquery(&res, "sneezy", "doUser", "select pingtime from pings where host='%s'", d->host);
	if((row=mysql_fetch_row(res))){
	  sprintf(buf2, "[%s](%s)", (d->host ? d->host : "????"), row[0]);
	} else {
	  sprintf(buf2, "[%s](???)", (d->host ? d->host : "????"));
	}

        sprintf(buf3, "[%s]", ((d->connected < MAX_CON_STATUS && d->connected >= 0) ? connected_types[d->connected] : "Editing"));
        sprintf(buf4, "[%s]", (d->account && d->account->name) ? d->account->name : "UNDEFINED");
        sprintf(line + strlen(line), "%s%-34.34s%s %s%-10.10s%s %s%s%s\n\r", red(), buf2, norm(), green(), buf3, norm(), cyan(), buf4, norm());
      }
      sb += line;
      count++;
    }
    sprintf(buf2, "\n\rTotal Descriptors : %d\n\r", count);
    sb += buf2;
    if (desc)
      desc->page_string(sb.c_str(), SHOWNOW_NO, ALLOWREP_YES);
    return;
  } else if (is_abbrev(arg1, "site")) {
    if (!*arg2 || !arg2) {
      sendTo("Syntax : users site <sitename>\n\r");
      return;
    } else {
      sendTo("\n\rPlayers online from %s:\n\r\n\r", arg2);
      sendTo(USERS_HEADER);
      for (d = descriptor_list; d; d = d->next) {
        if (d->host && strcasestr(d->host, arg2)) {
          if (d->character && d->character->name) {
            if (!d->connected && !canSeeWho(d->character))
              continue;

            if (d->original)
              sprintf(line, "%-16.16s: ", d->original->name);
            else
              sprintf(line, "%-16.16s: ", d->character->name);
          } else
            strcpy(line, "UNDEFINED       : ");

          // don't let newbie gods blab who imm's mortals are
          if (d->account && IS_SET(d->account->flags, ACCOUNT_IMMORTAL) && 
                !hasWizPower(POWER_VIEW_IMM_ACCOUNTS)) {
            sprintf(line + strlen(line), "*** Information Concealed ***\n\r");
          } else {
            sprintf(buf2, "[%s]", (d->host ? d->host : "????"));
            sprintf(buf3, "[%s]", ((d->connected < MAX_CON_STATUS && d->connected >= 0) ? connected_types[d->connected] : "Editing"));
            sprintf(buf4, "[%s]", (d->account && d->account->name) ? d->account->name : "UNDEFINED");
            sprintf(line + strlen(line), "%-34.34s %-10.10s %s\n\r", buf2, buf3, buf4);
          }
          sendTo(line);
          count++;
        }
      }
    }
    if (!count) {
      sendTo("No players online from that site.\n\r");
      return;
    }
  } else if ((k = get_pc_world(this, arg1, EXACT_YES)) ||
             (k = get_pc_world(this, arg1, EXACT_NO))) {
    if (k->desc) {
      // don't let newbie gods blab who imm's mortals are
      if (k->desc->account && IS_SET(k->desc->account->flags, ACCOUNT_IMMORTAL) && 
            !hasWizPower(POWER_VIEW_IMM_ACCOUNTS)) {
        sendTo(COLOR_MOBS, "\n\r%-16.16s : *******Information Concealed*******\n\r", k->getName());
      } else {
        sprintf(buf2, "[%s]", (k->desc->host ? k->desc->host : "????"));
        sprintf(buf3, "[%s]", ((k->desc->connected < MAX_CON_STATUS && k->desc->connected >= 0) ? connected_types[k->desc->connected] : "Editing"));
        sprintf(buf4, "[%s]", (k->desc->account->name));
        sendTo(COLOR_MOBS, "\n\r%-16.16s : %-34.34s %-15.15s %-10.10s\n\r", k->getName(), buf2, buf3, buf4);
      }
      return;
    } else {
      sendTo("That person is linkdead. Users can give you no info on them.\n\r");
      return;
    }
  } else {
    sendTo("Syntax : users (no argument list all users)\n\r");
    sendTo("         users <playername>\n\r");
    sendTo("         users site <sitename>\n\r");
    sendTo("The sitename can be abbreviated just like when you wizlock.\n\r");
    return;
  }
}

void TBeing::doInventory(const char *argument)
{
  TBeing *victim;
  char arg[80];

  one_argument(argument, arg);

  if (!*argument || !isImmortal()) {
    if (isAffected(AFF_BLIND)) {
      sendTo("It's pretty hard to take inventory when you can't see.\n\r");
      return;
    }
    sendTo("You are carrying:\n\r");
    list_in_heap(stuff, this, 0, 100);

    if (GetMaxLevel() > 10) {
      sendTo("\n\r%3.f%% volume, %3.f%% weight.\n\r",
             ((float) getCarriedVolume() / (float) carryVolumeLimit()) * 100.0,
             ((float) getCarriedWeight() / (float) carryWeightLimit()) * 100.0);
    }
  } else {
    victim = get_char_vis_world(this, arg, NULL, EXACT_YES);
    if (!victim)
      victim = get_char_vis_world(this, arg, NULL, EXACT_NO);

    if (victim) {
      act("$N is carrying:", FALSE, this, NULL, victim, TO_CHAR);
      list_in_heap(victim->stuff, this, 1, 100);
    }
  }
}

void TBeing::doEquipment(const char *argument)
{
  wearSlotT j;
  int found;
  char capbuf[80], buf[80], trans[80];
  TThing *t;

  one_argument(argument, buf);
  if (is_abbrev(buf, "damaged") || is_abbrev(buf, "all.damaged")) {
    sendTo("The following equipment is damaged:\n\r");
    found = FALSE;
    for (j = MIN_WEAR; j < MAX_WEAR; j++) {
      TThing *tt = equipment[j];
      TObj *tobj = dynamic_cast<TObj *>(tt);
      if (tobj && tobj->getMaxStructPoints() != tobj->getStructPoints()) {
        if (!tobj->shouldntBeShown(j)) {
          sprintf(buf, "<%s>", describeEquipmentSlot(j).c_str());
            sendTo("%s%-25s%s", cyan(), buf, norm());
            if (canSee(tobj)) {
            showTo(tobj, SHOW_MODE_SHORT_PLUS);
            found = TRUE;
          } else {
            sendTo("Something.\n\r");
            found = TRUE;
          }
        }
      }
    }
  } else if (!*argument || !isImmortal()) {
    sendTo("You are using:\n\r");
    found = FALSE;
    for (j = MIN_WEAR; j < MAX_WEAR; j++) {
      if (equipment[j]) {
        if (!equipment[j]->shouldntBeShown(j)) {
          sprintf(buf, "<%s>", describeEquipmentSlot(j).c_str());
          sendTo("%s%-25s%s", cyan(), buf, norm());
          if (canSee(equipment[j])) {
            showTo(equipment[j], SHOW_MODE_SHORT_PLUS);
            found = TRUE;
          } else {
            sendTo("Something.\n\r");
            found = TRUE;
          }
        }
      }
    }
  } else {
    // allow immortals to get eq of players
    TBeing *victim = get_char_vis_world(this, argument, NULL, EXACT_YES);
    if (!victim)
      victim = get_char_vis_world(this, argument, NULL, EXACT_NO);

    if (victim) {
      act("$N is using.", FALSE, this, 0, victim, TO_CHAR);
      found = FALSE;
      for (j = MIN_WEAR; j < MAX_WEAR; j++) {
        if (victim->equipment[j]) {
          if (!victim->equipment[j]->shouldntBeShown(j)) {
            sprintf(buf, "<%s>", victim->describeEquipmentSlot(j).c_str());
            sendTo("%s%-25s%s", cyan(), buf, norm());
            if (canSee(victim->equipment[j])) {
              showTo(victim->equipment[j], SHOW_MODE_SHORT_PLUS);
              found = TRUE;
            } else {
              sendTo("Something.\n\r");
              found = TRUE;
            }
          }
        }
      }
    } else 
      sendTo("No such character exists.\n\r");

    return;
  }
  if (!found)
    sendTo("Nothing.\n\r");

  for (j = MIN_WEAR; j < MAX_WEAR; j++) {
    if (j == HOLD_RIGHT || j == HOLD_LEFT)
      continue;
    if (!slotChance(j))
      continue;
    if (isLimbFlags(j, PART_TRANSFORMED)){
      switch (j) {
        case WEAR_FINGER_R:
        case WEAR_FINGER_L:
          break;
        case WEAR_NECK:
          sprintf(trans, "<%s>", describeTransLimb(j).c_str());
          sendTo("%s%s%s\n\r", cyan(), trans, norm());
          break;
        case WEAR_BODY:
          break;
        case WEAR_HEAD:
          sprintf(trans, "<%s>", describeTransLimb(j).c_str());
          sendTo("%s%s%s\n\r", cyan(), trans, norm());
          break;
        case WEAR_LEGS_L:
          break;
        case WEAR_LEGS_R:
          sprintf(trans, "<%s>", describeTransLimb(j).c_str());
          sendTo("%s%s%s\n\r", cyan(), trans, norm());
          break;
        case WEAR_FOOT_R:
        case WEAR_FOOT_L:
          break;
        case WEAR_HAND_R:
          if (isLimbFlags(WEAR_ARM_R, PART_TRANSFORMED))
            break;
          sprintf(trans, "<%s>", describeTransLimb(j).c_str());
          sendTo("%s%s%s\n\r", cyan(), trans, norm());
          break;
        case WEAR_HAND_L:
          if (isLimbFlags(WEAR_ARM_L, PART_TRANSFORMED))
            break;
          sprintf(trans, "<%s>", describeTransLimb(j).c_str());
          sendTo("%s%s%s\n\r", cyan(), trans, norm());
          break;
        case WEAR_ARM_R:
          sprintf(trans, "<%s>", describeTransLimb(j).c_str());
          sendTo("%s%s%s\n\r", cyan(), trans, norm());
          break;
        case WEAR_ARM_L:
          sprintf(trans, "<%s>", describeTransLimb(j).c_str());
          sendTo("%s%s%s\n\r", cyan(), trans, norm());
          break;
        case WEAR_BACK :
        case WEAR_WAISTE:
        case WEAR_WRIST_R:
        case WEAR_WRIST_L:
        case WEAR_EX_LEG_R:
        case WEAR_EX_LEG_L:
        case WEAR_EX_FOOT_R:
        case WEAR_EX_FOOT_L:
          break;
        default:
          break;
      }
    }
    if ((t = getStuckIn(j))) {
      if (canSee(t)) {
        strcpy(capbuf, t->getName());
        sendTo(COLOR_OBJECTS, "%s is sticking out of your %s!\n\r", cap(capbuf), describeBodySlot(j).c_str());
      }
    }
  }
}


void TBeing::doCredits()
{
  if (desc)
    desc->start_page_file(CREDITS_FILE, "Credits file being revised!\n\r");
}

void TBeing::doWizlist()
{
  if (desc) {
    FILE   *tFile;
    string  tStString("");

    if (!(tFile = fopen(WIZLIST_FILE, "r")))
      sendTo("Sorry, wizlist under construction!\n\r");
    else {
      wizlist_used_num++;

      file_to_string(WIZLIST_FILE, tStString);
      desc->page_string(tStString.c_str());
      fclose(tFile);
    }
  }
}

static int whichNumberMobile(const TThing *mob)
{
  TBeing *i;
  string name;
  int iNum;

  name = fname(mob->name);
  for (i = character_list, iNum = 0; i; i = i->next) {
    if (isname(name.c_str(), i->name) && i->in_room != ROOM_NOWHERE) {
      iNum++;
      if (i == mob)
        return iNum;
    }
  }
  return 0;
}

static const string numbered_person(const TBeing *ch, const TThing *person)
{
  char buf[256];

  if (dynamic_cast<const TMonster *>(person) && ch->isImmortal())
    sprintf(buf, "%d.%s", whichNumberMobile(person), fname(person->name).c_str());
  else
    strcpy(buf, ch->pers(person));

  return buf;
}

void do_where_thing(const TBeing *ch, const TThing *obj, bool recurse, string &sb)
{
  char buf[256];

  if (obj->in_room != ROOM_NOWHERE) {       // object in a room 
    sprintf(buf, "%-30s- ",
           obj->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "%-35s [%d]\n\r",
           obj->roomp->getNameNOC(ch).c_str(), obj->in_room);
// object carried by monster
 } else if (dynamic_cast<TBeing *>(obj->parent) && obj->parent->roomp) {
    sprintf(buf, "%-30s- carried by %s -", obj->getNameNOC(ch).c_str(), 
               numbered_person(ch, obj->parent).c_str());
    sprintf(buf + strlen(buf), " %-20s [%d]\n\r",
               (obj->parent->roomp->name ? obj->parent->roomp->getNameNOC(ch).c_str() : "Room Unknown"), 
               obj->parent->in_room);
  } else if (dynamic_cast<TBeing *>(obj->parent) && obj->parent->riding && obj->parent->riding->roomp) {
    sprintf(buf, "%-30s- carried by %s - ", 
               obj->getNameNOC(ch).c_str(), 
               numbered_person(ch, obj->parent).c_str());
    sprintf(buf + strlen(buf), "riding %s - ", 
               obj->parent->riding->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "%s [%d]\n\r", 
               (obj->parent->riding->roomp->name ? obj->parent->riding->roomp->getNameNOC(ch).c_str() : "Room Unknown"),
               obj->parent->riding->in_room);
  } else if (dynamic_cast<TBeing *>(obj->parent) && obj->parent->riding) {
    sprintf(buf, "%-30s- carried by %s - ",
               obj->getNameNOC(ch).c_str(), numbered_person(ch, obj->parent).c_str());
    sprintf(buf + strlen(buf), "riding %s - (Room Unknown)\n\r",
               obj->parent->riding->getNameNOC(ch).c_str());
  } else if (dynamic_cast<TBeing *>(obj->parent)) {  // object carried by monster 
    sprintf(buf, "%-30s- carried by %s (Room Unknown)\n\r", obj->getNameNOC(ch).c_str(), 
               numbered_person(ch, obj->parent).c_str());
// object equipped by monster
  } else if (obj->equippedBy && obj->equippedBy->roomp) {
    sprintf(buf, "%-30s- equipped by %s - ", obj->getNameNOC(ch).c_str(), 
               numbered_person(ch, obj->equippedBy).c_str());
    sprintf(buf + strlen(buf), "%s [%d]\n\r", 
               (obj->equippedBy->roomp->name ? obj->equippedBy->roomp->getNameNOC(ch).c_str() : "Room Unknown"), 
               obj->equippedBy->in_room);
  } else if (obj->equippedBy) {       // object equipped by monster 
    sprintf(buf, "%-30s- equipped by %s (Room Unknown)\n\r", obj->getNameNOC(ch).c_str(), 
               numbered_person(ch, obj->equippedBy).c_str());
  } else if (obj->stuckIn && obj->stuckIn->roomp) {
    sprintf(buf, "%-20s- stuck in %s - ",
               obj->getNameNOC(ch).c_str(), 
               numbered_person(ch, obj->stuckIn).c_str());
    sprintf(buf + strlen(buf), "%s [%d]\n\r",
               (obj->stuckIn->roomp->name ? obj->stuckIn->roomp->getNameNOC(ch).c_str() : "Room Unknown"), 
               obj->stuckIn->in_room);
  } else if (obj->stuckIn) {
    sprintf(buf, "%-20s- stuck in %s - (Room Unknown)\n\r", obj->getNameNOC(ch).c_str(), 
               numbered_person(ch, obj->stuckIn).c_str());
// object in object
  } else if (obj->parent && obj->parent->parent) {
    sprintf(buf, "%-30s- ",
               obj->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "in %s - ",
               obj->parent->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "in %s - ",
               obj->parent->parent->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "%s [%d]\n\r", 
            ((obj->parent->parent->roomp && obj->parent->parent->roomp->name) ?
             obj->parent->parent->roomp->getNameNOC(ch).c_str() : "Room Unknown"),
               obj->parent->parent->in_room);
  } else if (obj->parent && obj->parent->equippedBy) {
    sprintf(buf, "%-30s- ",
               obj->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "in %s - ",
               obj->parent->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "equipped by %s - ",
               obj->parent->equippedBy->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "%s [%d]\n\r", 
    ((obj->parent->equippedBy->roomp && obj->parent->equippedBy->roomp->name) ?
       obj->parent->equippedBy->roomp->getNameNOC(ch).c_str() : "Room Unknown"),
       obj->parent->equippedBy->in_room);
  } else if (obj->parent && obj->parent->stuckIn) {
    sprintf(buf, "%-30s- ",
               obj->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "in %s - ",
               obj->parent->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "stuck in %s - ",
               obj->parent->stuckIn->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "%s [%d]\n\r", 
       ((obj->parent->stuckIn->roomp && obj->parent->stuckIn->roomp->name) ?
        obj->parent->stuckIn->roomp->getNameNOC(ch).c_str() : "Room Unknown"),
        obj->parent->stuckIn->in_room);
// object in object 
  } else if (obj->parent) {
    sprintf(buf, "%-30s- in ",
               obj->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "%s - ",
               obj->parent->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "%s [%d]\n\r",
               obj->parent->roomp ? obj->parent->roomp->getNameNOC(ch).c_str() : "(Room Unkown)",
               obj->parent->in_room);
  } else if (obj->riding) {
    sprintf(buf, "%-30s- on ",
               obj->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "%s - ",
               obj->riding->getNameNOC(ch).c_str());
    sprintf(buf + strlen(buf), "%s [%d]\n\r",
               obj->riding->roomp ? obj->riding->roomp->getNameNOC(ch).c_str() : "(Room Unkown)",
               obj->riding->in_room);
  } else {
    sprintf(buf, "%-30s- god doesn't even know where...\n\r", obj->getNameNOC(ch).c_str());
  }
  if (*buf)
    sb += buf;

  if (recurse) {
    if (obj->in_room != ROOM_NOWHERE)
      return;
    else if (dynamic_cast<TBeing *>(obj->parent))
      do_where_thing(ch, obj->parent, TRUE, sb);
    else if (obj->equippedBy)
      do_where_thing(ch, obj->equippedBy, TRUE, sb);
    else if (obj->stuckIn)
      do_where_thing(ch, obj->stuckIn, TRUE, sb);
    else if (obj->parent)
      do_where_thing(ch, obj->parent, TRUE, sb);
    else if (obj->riding)
      do_where_thing(ch, obj->riding, TRUE, sb);
  }
}

void TBeing::doWhere(const char *argument)
{
  char namebuf[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
  char *nameonly;
  register TBeing *i, *ch;
  register TObj *k;
  Descriptor *d;
  int iNum, count;
  string sb;
  bool dash = FALSE;
  bool gods = FALSE;
  unsigned int tot_found = 0;
  string tStString(argument),
         tStName(""),
         tStArg("");

  if (powerCheck(POWER_WHERE))
    return;

  only_argument(argument, namebuf);
  two_arg(tStString, tStArg, tStName);

  if (hasWizPower(POWER_WIZARD) && (GetMaxLevel() > MAX_MORT) &&
      (is_abbrev(tStArg, "engraved") || is_abbrev(tStArg, "owners"))) {
    count = 0;

    for (k = object_list; k; k = k->next) {
      if (!k->name) {
        vlogf(LOG_BUG, "Item without a name in object_list (doWhere) looking for %s", namebuf);
        continue;
      }

      if (is_abbrev(tStArg, "owners")) {
        if (!k->owners)
          continue;

        const char * tTmpBuffer = k->owners;
	char tTmpString[256];

        while (tTmpBuffer && *tTmpBuffer) {
          tTmpBuffer = one_argument(tTmpBuffer, tTmpString);

          if (!tTmpString || !*tTmpString)
            continue;

          if (!is_abbrev(tStName, tTmpString))
            continue;

          break;
        }

        if (!tTmpString || !*tTmpString || !is_abbrev(tStName, tTmpString))
          continue;
      }

      if (is_abbrev(tStArg, "engraved"))
        if (!k->action_description)
          continue;
        else if ((sscanf(k->action_description, "This is the personalized object of %s.", buf)) != 1)
          continue;
        else if (!is_abbrev(tStName, buf))
          continue;

      sprintf(buf, "[%2d] ", ++count);
      sb += buf;

      if (++tot_found == 100) {
        sb += "Too many objects found.\n\r";
        break;
      }

      do_where_thing(this, k, 0, sb);
    }

    if (sb.empty())
      sendTo("Couldn't find any such object.\n\r");
    else if (desc)
      desc->page_string(sb.c_str(), SHOWNOW_NO, ALLOWREP_YES);

    return;
  }

  if (!*namebuf || (dash = (*namebuf == '-'))) {
    if (GetMaxLevel() <= MAX_MORT) {
      sendTo("What are you looking for?\n\r");
      return;
    } else {
      if (dash) {
        if (!strchr(namebuf, 'g')) {
          sendTo("Syntax : where -g\n\r");
          return;
        }
        gods = TRUE;
      }
      sb += "Players:\n\r--------\n\r";

      for (d = descriptor_list; d; d = d->next) {
        if ((ch = d->character) && canSeeWho(ch) &&
            (ch->in_room != ROOM_NOWHERE) &&
            (!gods || ch->isImmortal())) {
          if (d->original)
            sprintf(buf, "%-20s - %s [%d] In body of %s\n\r",
                    d->original->getNameNOC(ch).c_str(), ch->roomp->name,
                    ch->in_room, ch->getNameNOC(ch).c_str());
          else
            sprintf(buf, "%-20s - %s [%d]\n\r", ch->getNameNOC(ch).c_str(),
                    ch->roomp ? ch->roomp->name : "Bad room", ch->in_room);

          sb += buf;
        }
      }
      if (desc)
        desc->page_string(sb.c_str(), SHOWNOW_NO, ALLOWREP_YES);
      return;
    }
  }
  if (isdigit(*namebuf)) {
    nameonly = namebuf;
    count = iNum = get_number(&nameonly);
  } else
    count = iNum = 0;

  *buf = '\0';

  for (i = character_list; i; i = i->next) {
    if (!i->name) {
      vlogf(LOG_BUG, "Being without a name in character_list (doWhere) looking for %s", namebuf);
      continue;
    }
    if (isname(namebuf, i->name) && canSeeWho(i) && canSee(i)) {
      if ((i->in_room != ROOM_NOWHERE) && (isImmortal() || (i->roomp->getZone() == roomp->getZone()))) {
        if (!iNum || !(--count)) {
          if (!iNum) {
            sprintf(buf, "[%2d] ", ++count);
            sb += buf;
          }
          if (++tot_found > 500) {
            sb += "Too many creatures found.\n\r";
            break;
          }
          
          do_where_thing(this, i, TRUE, sb);
          *buf = 1;
          if (iNum != 0)
            break;
        }
        if (GetMaxLevel() <= MAX_MORT)
          break;
      }
    }
  }
  if (GetMaxLevel() > MAX_MORT) {
    for (k = object_list; k; k = k->next) {
      if (!k->name) {
        vlogf(LOG_BUG, "Item without a name in object_list (doWhere) looking for %s", namebuf);
        continue;
      }
      if (isname(namebuf, k->name) && canSee(k)) {
        if (!iNum || !(--count)) {
          if (!iNum) {
            sprintf(buf, "[%2d] ", ++count);
            sb += buf;
          }
          if (++tot_found > 500) {
            sb += "Too many objects found.\n\r";
            break;
          }
          do_where_thing(this, k, iNum != 0, sb);
          *buf = 1;
          if (iNum != 0)
            break;
        }
      }
    }
  }
  if (sb.empty())
    sendTo("Couldn't find any such thing.\n\r");
  else {
    if (desc)
      desc->page_string(sb.c_str(), SHOWNOW_NO, ALLOWREP_YES);
  }
}

extern void comify(char *);

void TBeing::doLevels(const char *argument)
{
#if 0
  int       tIndex,
            tValueA,
            tValueB,
            tValueC,
            tValueD,
            tDents[4];
  char      tString[256],
            tBuffer[256];
  string    tStString("");
  classIndT tClass     = MAGE_LEVEL_IND;
  bool      tRemaining = false;

  if (argument && *argument && is_abbrev(argument, "left"))
    tRemaining = true;

  for (tIndex = 1; tIndex <= (MAX_MORT / 4) + 1; tIndex++) {
    tValueA = (int) getExpClassLevel(tClass, (tDents[0] = (tIndex + 0 * (MAX_MORT / 4 + 1))));
    tValueB = (int) getExpClassLevel(tClass, (tDents[1] = (tIndex + 1 * (MAX_MORT / 4 + 1))));
    tValueC = (int) getExpClassLevel(tClass, (tDents[2] = (tIndex + 2 * (MAX_MORT / 4 + 1))));
    tValueD = (int) getExpClassLevel(tClass, (tDents[3] = (tIndex + 3 * (MAX_MORT / 4 + 1))));

    if (tRemaining) {
      tValueA = max(0, ((int) getExp() - tValueA));
      tValueB = max(0, ((int) getExp() - tValueB));
      tValueC = max(0, ((int) getExp() - tValueC));
      tValueD = max(0, ((int) getExp() - tValueD));
    }

    for (int tTemp = 0; tTemp < 4; tTemp++)
      if (tDents[tTemp] <= MAX_MORT) {
        sprintf(tBuffer, "%d", tValueA);
        comify(tBuffer);
        sprintf(tString, "%s[%2d]%s %s%13s%s",
                cyan(), tDents[tTemp], norm(),
                (tRemaining ? (tValueA ? orange() : green()) :
                 ((tDents[tTemp] > GetMaxLevel()) ? orange() : green())),
                tString, norm());
        tStString += tString;
      }
    tStString += "\n\r";
  }

  tStString += "\n\r";

  if (desc)
    desc->page_string(tStString.c_str(), SHOWNOW_NO, ALLOWREP_YES);
#else
  int i;
  classIndT Class;
// int RaceMax;
  string sb;
  char buf[256],
       tString[256];

  for (; isspace(*argument); argument++);

  if (!*argument) {
    if (isSingleClass()) {
      int num = CountBits(getClass()) - 1;
      if (num < MIN_CLASS_IND || num >= MAX_CLASSES) {
        return;
      }
      Class = classIndT(num);
    } else {
      sendTo("You must supply a class!\n\r");
      return;
    }
  } else if (is_abbrev(argument, "mage"))
    Class = MAGE_LEVEL_IND;
  else if (is_abbrev(argument, "monk"))
    Class = MONK_LEVEL_IND;
  else {
    switch (*argument) {
      case 'C':
      case 'c':
        Class = CLERIC_LEVEL_IND;
        break;
      case 'F':
      case 'f':
      case 'W':
      case 'w':
        Class = WARRIOR_LEVEL_IND;
        break;
      case 'T':
      case 't':
        Class = THIEF_LEVEL_IND;
        break;
      case 'R':
      case 'r':
        Class = RANGER_LEVEL_IND;
        break;
      case 'p':
      case 'P':
      case 'd':     // deikhan
      case 'D':
        Class = DEIKHAN_LEVEL_IND;
        break;
      case 's':     // shaman 
      case 'S':
        Class = SHAMAN_LEVEL_IND;
        break;

      default:
        sendTo("I don't recognize %s\n\r", argument);
        return;
        break;
    }
  }
  //RaceMax = RacialMax[race->getRace()][Class];

  sprintf(buf, "Experience needed for level in class %s:\n\r\n\r",
      classNames[Class].capName);
  sb += buf;

  ubyte cLvl = getLevel(Class);

  for (i = 1; i <= MAX_MORT/4 + 1; i++) {
    int j = i + 1*(MAX_MORT/4+1);
    int k = i + 2*(MAX_MORT/4+1);
    int m = i + 3*(MAX_MORT/4+1);

    if (i <= MAX_MORT) {
      sprintf(tString, "%d", (int) getExpClassLevel(Class, i));
      comify(tString);
      sprintf(buf, "%s[%2d]%s %s%13s%s%s", 
            cyan(), i, norm(),
            ((i > cLvl) ? orange() : green()), tString, norm(),
            " ");
      sb += buf;
    }
    if (j <= MAX_MORT) {
      sprintf(tString, "%d", (int) getExpClassLevel(Class, j));
      comify(tString);
      sprintf(buf, "%s[%2d]%s %s%13s%s%s",
            cyan(), j, norm(),
            ((j > cLvl) ? orange() : green()), tString, norm(),
              " ");
      sb += buf;
    }
    if (k <= MAX_MORT) {
      sprintf(tString, "%d", (int) getExpClassLevel(Class, k));
      comify(tString);
      sprintf(buf, "%s[%2d]%s %s%13s%s%s",
            cyan(), k, norm(),
            ((k > cLvl) ? orange() : green()), tString, norm(),
            " ");
      sb += buf;
    }
    if (m <= MAX_MORT) {
      sprintf(tString, "%d", (int) getExpClassLevel(Class, m));
      comify(tString);
      sprintf(buf, "%s[%2d]%s %s%13s%s%s",
            cyan(), m, norm(),
            ((m > cLvl) ? orange() : green()), tString, norm(),
            "\n\r");
      sb += buf;
    } else {
      sb += "\n\r";
    }
  }
  sb += "\n\r";
  if (desc)
    desc->page_string(sb.c_str(), SHOWNOW_NO, ALLOWREP_YES);
  return;
#endif
}

void TBeing::doWorld()
{
  time_t ct, ot, tt;
  char *tmstr, *otmstr;
  int i;
  string str;
  char buf[256];
  MYSQL_RES *res;
  MYSQL_ROW row;

  if (!desc)
    return;

  ot = Uptime;
  tt = ot + 3600 * desc->account->time_adjust;

  otmstr = asctime(localtime(&tt));
  *(otmstr + strlen(otmstr) - 1) = '\0';
  sprintf(buf, "%sStart time was:                      %s%s\n\r", 
        blue(),otmstr, norm());
  str += buf;

  ct = time(0) + 3600 * desc->account->time_adjust;

  tmstr = asctime(localtime(&ct));
  *(tmstr + strlen(tmstr) - 1) = '\0';
  sprintf(buf, "%sCurrent time is:                     %s%s\n\r", 
         blue(), tmstr, norm());
  str += buf;

  time_t upt = ct - tt;
  sprintf(buf, "%sUptime is:                           %s%s\n\r", 
         blue(), secsToString(upt).c_str(), norm());
  str += buf;

  long int total=0, count=0;
  for(i=0;i<10;++i){
    if(lag_info.lagcount[i]){
      total+=lag_info.lagtime[i];
      count++;
    }
  }
  sprintf(buf, "%sMachine Lag: Avg/Cur/High/Low        %ld/%ld/%ld/%ld%s\n\r",
	  blue(),
	  (count ? total/count : 0),
	  lag_info.current,
	  lag_info.high,
	  lag_info.low, norm());
  str += buf;

  dbquery(&res, "sneezy", "doWorld(1)", 
	  "select pingtime from pings where host='%s'", desc->host);
  if((row=mysql_fetch_row(res))){
    sprintf(buf, "%sNetwork Lag: Yours/Avg/High/Low      %s",
	    blue(), row[0]);
    str += buf;
    dbquery(&res, "sneezy", "doWorld(2)", 
	    "select avg(pingtime), max(pingtime), min(pingtime) from pings");
    if((row=mysql_fetch_row(res))){
      sprintf(buf, "/%s/%s/%s%s\n\r", row[0], row[1], row[2], norm());
      str += buf;
    } else {
      sprintf(buf, "/???/???/???%s\n\r", norm());
      str += buf;
    }
  }


  sprintf(buf, "Total number of rooms in world:               %ld\n\r", 
        roomCount);
  str += buf;
  sprintf(buf, "Total number of zones in world:               %d\n\r", 
        zone_table.size());
  str += buf;
  sprintf(buf, "Total number of distinct objects in world:%s    %d%s\n\r",
        green(), obj_index.size(), norm());
  str += buf;
  sprintf(buf, "Total number of objects in game:%s              %ld%s\n\r",
        green(), objCount, norm());
  str += buf;
  sprintf(buf, "Total number of registered accounts:%s          %d%s\n\r", 
         blue(), accStat.account_number, norm());
  str += buf;
  sprintf(buf, "Total number of registered players:%s           %d%s\n\r", 
         blue(), accStat.player_count, norm());
  str += buf;

  if (hasWizPower(POWER_WIZARD)) {
    sprintf(buf, "Total number of 7-day active accounts:%s        %d%s\n\r", 
           blue(), accStat.active_account7, norm());
    str += buf;
    sprintf(buf, "Total number of 7-day active players:%s         %d%s\n\r", 
           blue(), accStat.active_player7, norm());
    str += buf;
    sprintf(buf, "Total number of 30-day active accounts:%s       %d%s\n\r", 
           blue(), accStat.active_account30, norm());
    str += buf;
    sprintf(buf, "Total number of 30-day active players:%s        %d%s\n\r", 
           blue(), accStat.active_player30, norm());
    str += buf;
  }

  char timebuf[256];

  strcpy(timebuf, ctime(&stats.first_login));
  timebuf[strlen(timebuf) - 1] = '\0';
  strcat(timebuf, ":");
  sprintf(buf, "Logins since %-32.32s %s%ld  (%ld per day)%s\n\r",
     timebuf,  blue(), stats.logins, 
     (long) ((double) stats.logins * SECS_PER_REAL_DAY / (time(0) - stats.first_login)),
     norm());
  str += buf;

  sprintf(buf, "Total number of distinct mobiles in world:%s    %d%s\n\r",
        red(), mob_index.size(), norm());
  str += buf;

  if (GetMaxLevel() >= GOD_LEVEL1) {
    sprintf(buf, "%sDistinct Mobs by level:%s\n\r",
         blue(), norm());
    str += buf;
    sprintf(buf, "%sL1-5  [%s%3d%s]  L6-10 [%s%3d%s]  L11-15[%s%3d%s]  L16-20[%s%3d%s]  L21-25 [%s%3d%s] L26-30  [%s%3d%s]%s\n\r", norm(),
        purple(), stats.mobs_1_5, norm(),
        purple(), stats.mobs_6_10, norm(),
        purple(), stats.mobs_11_15, norm(),
        purple(), stats.mobs_16_20, norm(),
        purple(), stats.mobs_21_25, norm(),
        purple(), stats.mobs_26_30, norm(),
        norm());
    str += buf;
    sprintf(buf, "%sL31-40[%s%3d%s]  L41-50[%s%3d%s]  L51-60[%s%3d%s]  L61-70[%s%3d%s]  L71-100[%s%3d%s] L101-127[%s%3d%s]%s\n\r", norm(),
        purple(), stats.mobs_31_40, norm(),
        purple(), stats.mobs_41_50, norm(),
        purple(), stats.mobs_51_60, norm(),
        purple(), stats.mobs_61_70, norm(),
        purple(), stats.mobs_71_100, norm(),
        purple(), stats.mobs_101_127, norm(),
        norm());
    str += buf;
  }
  sprintf(buf, "Total number of monsters in game:%s             %ld%s\n\r",
        red(), mobCount, norm());
  str += buf;

  sprintf(buf, "%sActual Mobs by level:%s\n\r",
        purple(), norm());
  str += buf;

  sprintf(buf, "%sL  1-  5  [%s%4u%s]  L  6- 10  [%s%4u%s]  L 11- 15  [%s%4u%s]  L 16- 20  [%s%4u%s]\n\r", norm(),
         purple(), stats.act_1_5, norm(),
         purple(), stats.act_6_10, norm(),
         purple(), stats.act_11_15, norm(),
         purple(), stats.act_16_20, norm());
  str += buf;

  sprintf(buf, "%sL 21- 25  [%s%4u%s]  L 26- 30  [%s%4u%s]  L 31- 40  [%s%4u%s]  L 41- 50  [%s%4u%s]\n\r", norm(),
         purple(), stats.act_21_25, norm(),
         purple(), stats.act_26_30, norm(),
         purple(), stats.act_31_40, norm(),
         purple(), stats.act_41_50, norm());
  str += buf;

  sprintf(buf, "%sL 51- 60  [%s%4u%s]  L 61- 70  [%s%4u%s]  L 71-100  [%s%4u%s]  L101-127  [%s%4u%s]\n\r", norm(),
         purple(), stats.act_51_60, norm(),
         purple(), stats.act_61_70, norm(),
         purple(), stats.act_71_100, norm(),
         purple(), stats.act_101_127, norm());
  str += buf;

  desc->page_string(str.c_str());
}

const char *DescRatio(double f)
{                                // theirs / yours 
  if (f >= 4.0)
    return ("Way better than yours");
  else if (f > 3.0)
    return ("More than three times yours");
  else if (f > 2.0)
    return ("More than twice yours");
  else if (f > 1.5)
    return ("More than half again greater than yours");
  else if (f > 1.4)
    return ("At least a third greater than yours");
  else if (f > 1.0)
    return ("About the same as yours");
  else if (f > .9)
    return ("A little worse than yours");
  else if (f > .6)
    return ("Worse than yours");
  else if (f > .4)
    return ("About half as good as yours");
  else if (f > .2)
    return ("Much worse than yours");
  else if (f > .1)
    return ("Inferior");
  else
    return ("Extremely inferior");
}

const char *DescDamage(double dam)
{
  if (dam < 1.0)
    return ("Minimal Damage");
  else if (dam <= 2.0)
    return ("Slight damage");
  else if (dam <= 4.0)
    return ("A bit of damage");
  else if (dam <= 10.0)
    return ("A decent amount of damage");
  else if (dam <= 15.0)
    return ("A lot of damage");
  else if (dam <= 25.0)
    return ("A whole lot of damage");
  else if (dam <= 35.0)
    return ("A very large amount");
  else
    return ("A TON of damage");
}

const char *DescAttacks(double a)
{
  if (a < 1.0)
    return ("Not many");
  else if (a < 2.0)
    return ("About average");
  else if (a < 3.0)
    return ("A few");
  else if (a < 5.0)
    return ("A lot");
  else if (a < 9.0)
    return ("Many");
  else
    return ("A whole bunch");
}

const char *DescMoves(double a)
{
  if (a < .1)
    return ("very tired");
  else if (a < .3)
    return ("tired");
  else if (a < .5)
    return ("slightly tired");
  else if (a < .7)
    return ("decently rested");
  else if (a < 1.0)
    return ("well rested");
  else
    return ("totally rested");
}

const char *ac_for_score(int a)
{
  if (a > 750)
    return ("scantily clothed");
  else if (a > 500)
    return ("heavily clothed");
  else if (a > 250)
    return ("slightly armored");
  else if (a > 0)
    return ("moderately armored");
  else if (a > -250)
    return ("armored rather heavily");
  else if (a > -500)
    return ("armored very heavily");
  else if (a > -1000)
    return ("armored extremely heavily");
  else
    return ("totally armored");
}

void TBeing::doClear(const char *argument)
{
  int i;

  if (!desc)
    return;

  if (!*argument) {
    sendTo("Clear which alias?\n\r");
    return;
  } else
    i = atoi(argument) - 1;

  if ((i > -1) && (i < 16)) {
    desc->alias[i].command[0] = '\0';
    desc->alias[i].word[0] = '\0';
    sendTo("Ok. Alias %d now clear.\n\r", i + 1);
    return;
  } else {
    sendTo("Syntax :clear <alias number>\n\r");
    return;
  }
}

void TBeing::doAlias(const char *argument)
{
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  char spaces[20];
  int i;
  int remOption = FALSE;

  if (!desc)
    return;

  if (desc->m_bIsClient) {
    sendTo("Use the client aliases. See client help file for #alias command and
options menu\n\r");
    return;
  }

  for (i = 0; i < 19; i++)
    spaces[i] = ' ';

  spaces[19] = '\0';
  if (!*argument) {
    sendTo("Your list of aliases.....\n\r");
    for (i = 0; i < 16; i++) {
      sendTo("%2d) %s%s %s %s\n\r", i + 1, desc->alias[i].word,
            spaces + strlen(desc->alias[i].word),
            (ansi() ? ANSI_BLUE_BAR : "|"),
            desc->alias[i].command);
    }
    return;
  }
  half_chop(argument, arg1, arg2);
  if ((!arg2) || (!*arg2)) {
    sendTo("You need a second argument.\n\r");
    return;
  }
  if (!strcmp(arg2, "clear")) {
    sendTo("You could get in a loop like that!\n\r");
    return;
  }
  if (!strcmp(arg1, "clear")) 
    remOption = TRUE;
  
  if (!strcmp(arg1, arg2)) {
    sendTo("You could get in a loop like that!\n\r");
    return;
  }
  if (strlen(arg1) > 11) {
    sendTo("First argument is too long. It must be shorter than 12 characters.\n\r");
    return;
  }
  if (strlen(arg2) > 28) {
    sendTo("Second argument is too long. It must be less than 30 characters.\n\r");
    return;
  }
  i = 0;
  if (remOption) {
    while ((i < 16)) {
      if (*desc->alias[i].word && !strcmp(arg2, desc->alias[i].word)) {
        sendTo("Clearing alias %s\n\r", arg2);
        return;
      }
      i++;
    }
    sendTo("You have no alias for %s.\n\r", arg2);
    return;
  }

  i = -1;
  do {
    i++;
  }

  while ((i < 16) && *desc->alias[i].word && strcmp(arg1, desc->alias[i].word));
  if ((i == 16)) {
    sendTo("You have no more space for aliases. You will have to clear an alias before adding another one.\n\r");
    return;
  }
  strcpy(desc->alias[i].word, arg1);
  strcpy(desc->alias[i].command, arg2);
  sendTo("Setting alias %s to %s\n\r", arg1, arg2);
}

string TObj::equip_condition(int amt) const
{
  double p;

  if (!getMaxStructPoints()) {
    string a("<C>brand new<1>");
    return a;
  } else if (amt == -1)
    p = ((double) getStructPoints()) / ((double) getMaxStructPoints());
  else
    p = ((double) amt) / ((double) getMaxStructPoints());

  if (p == 1) {
    string a("<C>brand new<1>");
    return a;
  } else if (p > .9) {
    string a("<c>like new<1>");
    return a;
  } else if (p > .8) {
    string a("<B>excellent<1>");
    return a;
  } else if (p > .7) {
    string a("<b>very good<1>");
    return a;
  } else if (p > .6) {
    string a("<P>good<1>");
    return a;
  } else if (p > .5) {
    string a("<p>fine<1>");
    return a;
  } else if (p > .4) {
    string a("<G>fair<1>");
    return a;
  } else if (p > .3) {
    string a("<g>poor<1>");
    return a;
  } else if (p > .2) {
    string a("<y>very poor<1>");
    return a;
  } else if (p > .1) {
    string a("<o>bad<1>");
    return a;
  } else if (p > .001) {
    string a("<R>very bad<1>");
    return a;
  } else {
    string a("<r>destroyed<1>");
    return a;
  }
}

void TBeing::doMotd(const char *argument)
{
  for (; isspace(*argument); argument++);

  if (!desc)
    return;

  if (!*argument || !argument) {
    sendTo("Today's message of the day is :\n\r\n\r");
    desc->sendMotd(GetMaxLevel() > MAX_MORT);
    return;
#if 0
sendTo("Feature disabled, bug Batopr.\n\r");
  } else if (is_abbrev(argument, "message") && (GetMaxLevel() >= GOD_LEVEL4)) {
    TThing *t_note = searchLinkedListVis(this, "note", stuff);
    TObj *note = dynamic_cast<TObj *>(t_note);
    if (note) {
      if (!note->action_description) {
        sendTo("Your note has no message for the new motd!\n\r");
        return;
      } else {
        strcpy(motd, note->action_description);
        return;
      }
    } else {
      sendTo("You need a note with what you want the message to be in you inventory.\n\r");
      return;
    }
#endif
  }
}

const char *LimbHealth(double a)
{
  if (a < .1)
    return ("<R>in extremely bad shape<Z>");
  else if (a < .3)
    return ("<o>badly beaten<z>");
  else if (a < .5)
    return ("<O>moderately wounded<z>");
  else if (a < .7)
    return ("<g>mildly wounded<z>");
  else if (a < .99)
    return ("<g>slightly wounded<z>");
  else if (a >= 1.00)
    return ("<c>in perfect condition<Z>");
  else
    return ("in near perfect condition");
}

const string TBeing::slotPlurality(int limb) const
{
  char buf[10];

  if ((race->getBodyType() == BODY_BIRD) &&
      (limb == WEAR_WAISTE)) {
    // tail feathers
    sprintf(buf, "are");
  } else if ((race->getBodyType() == BODY_TREE) &&
      ((limb == WEAR_ARM_R) ||
       (limb == WEAR_ARM_L))) {
    // branches
    sprintf(buf, "are");
  } else
    sprintf(buf, "is");

  return buf;
}

void TBeing::doLimbs(const string & argument)
{
  wearSlotT i;
  char buf[512], who[5];
  affectedData *aff;
  TThing *t;
  TBeing *v=NULL;

  *buf = '\0';


  if(!argument.empty()) {
    only_argument(argument.c_str(), buf);
    if (!(v = get_char_room_vis(this, buf))) {
      if (!(v = fight())) {
	sendTo("Check whose limbs?\n\r");
	return;
      }
    }
    if (!sameRoom(*v)) {
      sendTo("That person doesn't seem to be around.\n\r");
      return;
    }
  }

  if(!v || v==this){
    v=this;
    strcpy(who, "Your");
    sendTo("You evaluate your limbs and their health.\n\r");
  } else {
    strncpy(who, v->hshr(), 5);
    cap(who);
    sendTo(COLOR_BASIC, "You evaluate %s's limbs and their health.\n\r", v->getName());
  }


  bool found = false;
  for (i = MIN_WEAR; i < MAX_WEAR; i++) {
    if (i == HOLD_RIGHT || i == HOLD_LEFT)
      continue;
    if (!v->slotChance(i))
      continue;
    double perc = (double) v->getCurLimbHealth(i) / (double) v->getMaxLimbHealth(i);

    if (v->isLimbFlags(i, PART_MISSING)) {
      sendTo(COLOR_BASIC, "<R>%s %s%s%s %s missing!<Z>\n\r", who, red(), v->describeBodySlot(i).c_str(), norm(), v->slotPlurality(i).c_str());
      found = TRUE;
      continue;
    } 
    if (perc < 1.00) {
      sendTo(COLOR_BASIC, "%s %s %s %s.\n\r", who, v->describeBodySlot(i).c_str(), v->slotPlurality(i).c_str(), LimbHealth(perc));
      found = TRUE;
    } 
    if (v->isLimbFlags(i, PART_USELESS)) {
      sendTo(COLOR_BASIC, "%s %s%s%s %s <O>useless<Z>!\n\r",who, red(),v->describeBodySlot(i).c_str(),norm(), v->slotPlurality(i).c_str());
      found = TRUE;
    }
    if (v->isLimbFlags(i, PART_PARALYZED)) {
      sendTo(COLOR_BASIC, "%s %s%s%s %s <O>paralyzed<Z>!\n\r",
         who,red(),v->describeBodySlot(i).c_str(),
         norm(), v->slotPlurality(i).c_str());
      found = TRUE;
    }
    if (v->isLimbFlags(i, PART_BLEEDING)) {
      sendTo(COLOR_BASIC, "%s %s%s%s %s <R>bleeding profusely<Z>!\n\r",
         who,red(),v->describeBodySlot(i).c_str(),
         norm(), v->slotPlurality(i).c_str());
      found = TRUE;
    }
    if (v->isLimbFlags(i, PART_INFECTED)) {
      sendTo(COLOR_BASIC, "%s %s%s%s %s infected with many germs!\n\r",
         who,red(),v->describeBodySlot(i).c_str(),
         norm(), v->slotPlurality(i).c_str());
      found = TRUE;
    }
    if (v->isLimbFlags(i, PART_BROKEN)) {
      sendTo(COLOR_BASIC, "%s %s%s%s %s broken!\n\r",
         who,red(),v->describeBodySlot(i).c_str(),
         norm(), v->slotPlurality(i).c_str());
      found = TRUE;
    }
    if (v->isLimbFlags(i, PART_LEPROSED)) {
      sendTo(COLOR_BASIC, "Leprosy has set into %s %s%s%s!\n\r",
          v->hshr(), red(),v->describeBodySlot(i).c_str(),norm());
      found = TRUE;
    }
    if (v->isLimbFlags(i, PART_TRANSFORMED)) {
      sendTo(COLOR_BASIC, "%s %s%s%s has been transformed!\n\r",
          who, red(),v->describeBodySlot2(i).c_str(),norm());
      found = TRUE;
    }
    if ((t = v->getStuckIn(i))) {
      if (canSee(t)) {
        strcpy(buf, t->getName());
        sendTo(COLOR_OBJECTS, "%s is sticking out of %s %s!\n\r",
        cap(buf), v->hshr(), v->describeBodySlot(i).c_str());
      }
    }
  }
   
  if(v==this)
    strcpy(who, "You");
  else {
    strncpy(who, v->hssh(), 5);
    cap(who);
  }
    
  if (v->affected) {
    for (aff = v->affected; aff; aff = aff->next) {
      if (aff->type == AFFECT_DISEASE) {
        if (!aff->level) {
          sendTo("%s %s %s.\n\r", who, 
	         (v==this)?"have":"has",
                 DiseaseInfo[affToDisease(*aff)].name);
          found = TRUE;
        }
      }
    }
  }
  if (!found)
    sendTo("All %s limbs are perfectly healthy!\n\r", (v==this)?"your":v->hshr());
}

void TBeing::genericEvaluateItem(const TThing *obj)
{
  obj->evaluateMe(this);
}

void TThing::evaluateMe(TBeing *ch) const
{
  int learn;

  learn = ch->getSkillValue(SKILL_EVALUATE);
  if (learn <= 0) {
    ch->sendTo("You are not sufficiently knowledgeable about evaluation.\n\r");
    return;
  }

  act("$p has little practical value in a fight.", false, ch, this, 0, TO_CHAR);
}

void TMagicItem::evaluateMe(TBeing *ch) const
{
  int learn;

  learn = ch->getSkillValue(SKILL_EVALUATE);
  if (learn <= 0) {
    ch->sendTo("You are not sufficiently knowledgeable about evaluation.\n\r");
    return;
  }

  ch->learnFromDoingUnusual(LEARN_UNUSUAL_NORM_LEARN, SKILL_EVALUATE, 10);

  // adjust for knowledge about magic stuff
  if (ch->hasClass(CLASS_RANGER)) {
    learn *= ch->getClassLevel(CLASS_RANGER);
    learn /= 200;
  } else {
    learn *= ch->getSkillValue(SPELL_IDENTIFY);
    learn /= 100;
  }
  if (learn <= 0) {
    ch->sendTo("You lack the knowledge to identify that item of magic.\n\r");
    return;
  }
  ch->sendTo(COLOR_OBJECTS, "You evaluate the magical powers of %s...\n\r\n\r",
         getName()); 

  ch->describeObject(this);

  if (learn > 10) 
    ch->describeMagicLevel(this, learn);
  
  if (learn > 15) {
    ch->describeMagicLearnedness(this, learn);
  }
  
  if (learn > 50) {
    ch->describeMagicSpell(this, learn);
  }
}

void TBeing::doEvaluate(const char *argument)
{
  char arg[160];
  TThing *obj;
  int count = 0,
      rNatureCount = 0;

  only_argument(argument, arg);
  if (!arg || !*arg) {
    sendTo("Evaluate what?\n\r");
    return;
  }
  if (is_abbrev(arg, "room")) {
    if (!roomp)
      return;

    if (isAffected(AFF_BLIND) && !isImmortal() && !isAffected(AFF_TRUE_SIGHT)) {
      sendTo("You can't see a damn thing -- you're blinded!\n\r");
      return;
    }

    describeRoomLight();

    if (!roomp) {
      sendTo("You have no idea where you are do you...\n\r");
      vlogf(LOG_BUG, "Player without room called evaluate room.  [%s]", getName());
    } else if (roomp->isCitySector())
      sendTo("You assume you are in a city by the way things look.\n\r");
    else if (roomp->isRoadSector())
      sendTo("It seems to be a well traveled road.\n\r");
    else if (roomp->isFlyingSector())
      sendTo("It seems as if you could just fly around here.\n\r");
    else if (roomp->isVertSector())
      sendTo("You seem to be scaling something.\n\r");
    else if (roomp->isUnderwaterSector())
      sendTo("\"Glub glub glub\" seem to best explain your current location.\n\r");
    else if (roomp->isSwampSector())
      sendTo("The swampy marsh gently rolls over you.\n\r");
    else if (roomp->isBeachSector())
      sendTo("A nice beach with a good view\n\r");
    else if (roomp->isHillSector())
      sendTo("A simple hill, nothing special.\n\r");
    else if (roomp->isMountainSector())
      sendTo("This appears to be a mountain.\n\r");
    else if (roomp->isForestSector())
      sendTo("Judging by the trees you bet you're in a forest.\n\r");
    else if (roomp->isAirSector())
      sendTo("You are up in the air, are you sure you should be here?\n\r");
    else if (roomp->isOceanSector())
      sendTo("You are in the ocean, going on a sea cruise?\n\r");
    else if (roomp->isRiverSector()) {
      if (roomp->getRiverSpeed() >= 30)
        sendTo("The river flows swifly towards the distance.\n\r");
      else if (roomp->getRiverSpeed() >= 15)
        sendTo("The river flows steadily towards the distance.\n\r");
      else if (roomp->getRiverSpeed() > 0)
        sendTo("The river gently flows towards the distance.\n\r");
      else
        sendTo("The river seems to be standing still.\n\r");
    } else if (roomp->isArcticSector())
      sendTo("The arctic winds around you seem to tell it all.\n\r");
    else if (roomp->isTropicalSector())
      sendTo("The tropical world around you makes you hot and sweaty.\n\r");
    else if (roomp->isWildernessSector() || roomp->isNatureSector())
      sendTo("You would appear to be in area draped with nature and the natural wilds.\n\r");
    else
      sendTo("You are somewhere...But damned if you can figure it out.\n\r");

    if (!hasClass(CLASS_RANGER))
      return;

    for (int Runner = MIN_DIR; Runner < MAX_DIR; Runner++)
      if (Runner != DIR_UP && Runner != DIR_DOWN && roomp->dir_option[Runner] &&
          real_roomp((count = roomp->dir_option[Runner]->to_room)))
        if (real_roomp(count)->notRangerLandSector())
          rNatureCount--;
        else
          rNatureCount++;

    if (roomp->notRangerLandSector()) {
      if (rNatureCount > 6)
        sendTo("You are getting real close to nature, you can feel it.\n\r");
      else if (rNatureCount > 3)
        sendTo("You are not that far, just a little more.\n\r");
      else if (rNatureCount > 0)
        sendTo("You feel really out of touch from nature here...\n\r");
      else
        sendTo("You don't feel very much in touch with nature here.\n\r");
    } else {
      if (rNatureCount > 6)
        sendTo("Now this is nature, you feel right at home.\n\r");
      else if (rNatureCount > 3)
        sendTo("The soothing sounds of nature rush over your body.\n\r");
      else if (rNatureCount > 0)
        sendTo("Altho not that dense, it's still home.\n\r");
      else
        sendTo("It's nature, but it sure doesn't feel like nature...\n\r");
    }
  } else {
    wearSlotT j;
    if (!(obj = get_thing_in_equip(this, arg, equipment, &j, TRUE, &count))) {
      if (!(obj = searchLinkedListVis(this, arg, stuff, &count))) {
        sendTo("You do not seem to have the '%s'.\n\r", arg);
        return;
      }
    }
    genericEvaluateItem(obj);
  }
}

void TTool::describeCondition(const TBeing *) const
{
  // intentionally blank
}

void TObj::describeCondition(const TBeing *ch) const
{
  ch->sendTo(COLOR_OBJECTS, "It is in %s condition.\n\r",equip_condition(-1).c_str());
}

void TThing::describeContains(const TBeing *ch) const
{
  if (stuff)
    ch->sendTo(COLOR_OBJECTS, "%s seems to have something in it...\n\r", good_cap(getName()).c_str());
}

void TBaseCup::describeContains(const TBeing *ch) const
{
  if (getDrinkUnits())
    ch->sendTo(COLOR_OBJECTS, "%s seems to have something in it...\n\r", good_cap(getName()).c_str());
}

void TFood::describeCondition(const TBeing *ch) const
{
  ch->sendTo(COLOR_OBJECTS, "It is in %s condition.\n\r",equip_condition(-1).c_str());
}

void TFood::describeObjectSpecifics(const TBeing *ch) const
{
  if (isFoodFlag(FOOD_SPOILED))
    act("$p looks a bit spoiled.", FALSE, ch, this, 0, TO_CHAR); 
}

void TCorpse::describeObjectSpecifics(const TBeing *ch) const
{
  if (isCorpseFlag(CORPSE_NO_SKIN))
    act("$p doesn't appear to have any skin left on it.",
        FALSE, ch, this, 0, TO_CHAR);
  else if (isCorpseFlag(CORPSE_HALF_SKIN))
    act("$p appears to have been half skinned.",
        FALSE, ch, this, 0, TO_CHAR);
  if (isCorpseFlag(CORPSE_NO_DISSECT))
    act("$p appears to have been dissected already.",
        FALSE, ch, this, 0, TO_CHAR);
  if (isCorpseFlag(CORPSE_NO_SACRIFICE))
    act("$p appears to have been sacrificed already.",
        FALSE, ch, this, 0, TO_CHAR);
}

void TSymbol::describeObjectSpecifics(const TBeing *ch) const
{
  double diff;
  int attuneCode = 1;
  factionTypeT sym_faction = getSymbolFaction();
  
if (attuneCode) {
  switch (sym_faction) {
    case FACT_NONE:
      ch->sendTo(COLOR_OBJECTS, "You can tell that %s has been sanctified but it bears no insignia.\n\r", good_cap(getName()).c_str());
      break;
    case FACT_BROTHERHOOD:
      ch->sendTo(COLOR_OBJECTS, "%s has the sign of the Brotherhood of Galek stamped upon it.\n\r", good_cap(getName()).c_str());
      break;
    case FACT_CULT:
      ch->sendTo(COLOR_OBJECTS, "%s has been sanctified and bears the insignia of the Cult of the Logrus.\n\r", good_cap(getName()).c_str());
      break;
    case FACT_SNAKE:
      ch->sendTo(COLOR_OBJECTS, "%s has been sanctified and branded with the image of a snake.\n\r", good_cap(getName()).c_str());
      break;
    case MAX_FACTIONS:
    case FACT_UNDEFINED:
      ch->sendTo(COLOR_OBJECTS, "%s is inert and has not been sanctified for use.\n\r", good_cap(getName()).c_str());
  }
}

  if (getSymbolMaxStrength()) {
    if (getSymbolCurStrength() == getSymbolMaxStrength()) {
      ch->sendTo(COLOR_OBJECTS, "%s has all of its original strength, it has not been used.\n\r", good_cap(getName()).c_str());

    } else {
      diff = (double) ((double) getSymbolCurStrength() /
                       (double) getSymbolMaxStrength());
      ch->sendTo(COLOR_OBJECTS, "You can tell that %s has %s strength left.\n\r", getName(),
          ((diff < .20) ? "very little" : ((diff < .50) ? "some" :
          ((diff < .75) ? "a good bit of" : "most of its"))));
    }
  } else {
    ch->sendTo(COLOR_OBJECTS, "%s has none of its strength left.\n\r", good_cap(getName()).c_str());
  }
}

void TTool::describeObjectSpecifics(const TBeing *ch) const
{
  double diff;

  if (getToolMaxUses()) 
    diff = ((double) getToolUses()) / ((double) getToolMaxUses());
  else
    diff = 1.00;

  ch->sendTo(COLOR_OBJECTS,"It appears %s is %s.\n\r", getName(),
       ((diff <= 0.0) ? "totally gone" :
        ((diff >= 1.0) ? "brand new" :
        ((diff >= 0.8) ? "almost new" :
        ((diff >= 0.6) ? "like new" :
        ((diff >= 0.4) ? "half gone" :
        ((diff >= 0.2) ? "more than half gone" :
                  "almost gone")))))));
}

void TObj::describeMe(const TBeing *ch) const
{
  char buf[80], buf2[256];
  char capbuf[256];
  char name_buf[80];
  TThing *t2;

  strcpy(buf, material_nums[getMaterial()].mat_name);
  strcpy(buf2, ch->objs(this));
  ch->sendTo(COLOR_OBJECTS,"%s is %s made of %s.\n\r", cap(buf2),
                 ItemInfo[itemType()]->common_name, uncap(buf));

  if (ch->isImmortal() || canWear(ITEM_TAKE)) {

    if (ch->desc && IS_SET(ch->desc->account->flags, ACCOUNT_IMMORTAL)){
#if 1
      if (obj_index[getItemIndex()].max_exist < 9999)
#else
      if (obj_index[getItemIndex()].max_exist <= MIN_EXIST_IMMORTAL)
#endif
        ch->sendTo("It is limited for immortals and there is %i out of %i in existence.\n\r", 
                   obj_index[getItemIndex()].number,
                   obj_index[getItemIndex()].max_exist);
      else
        ch->sendTo("It is not limited for immortals.\n\r");
    }
    if (10 >= max_exist) {
      ch->sendTo("This item is considered limited and will cost a rental fee.\n\r");
    }
#ifndef SNEEZY2000
    if (isRentable()) {
      int temp = max(0, rentCost());
  
      ch->sendTo("It has a rental cost of %d talen%s.\n\r",
          temp, (temp != 1 ? "s" : ""));
    } else 
      ch->sendTo("It can't be rented.\n\r");
#endif
    int volume = getVolume();
    volume = ((volume >= 100) ? volume/100 * 100 :
              ((volume >= 10) ? volume/10 * 10 : volume));

    // weight >= 1.0
    float wgt = getTotalWeight(TRUE);
    int volumeTmp, yards = 0;
    int feet = 0;
    int inches;
    char volumeBuf[256] = "\0";

    volumeTmp = volume;
    if (volumeTmp > CUBIC_INCHES_PER_YARD) {
      yards = volume/CUBIC_INCHES_PER_YARD;
      volumeTmp = volume % CUBIC_INCHES_PER_YARD;
      sprintf(volumeBuf, "%d cubic yard%s, ", yards, (yards == 1) ? "" : "s");
    }
    if (volumeTmp > CUBIC_INCHES_PER_FOOT) {
      feet = volumeTmp/CUBIC_INCHES_PER_FOOT;
      volumeTmp = volume % CUBIC_INCHES_PER_FOOT;
      sprintf(volumeBuf + strlen(volumeBuf), "%d cubic %s, ", feet, (yards == 1) ? "foot" : "feet");
    }
    if ((inches = volumeTmp))
      sprintf(volumeBuf + strlen(volumeBuf), "%d cubic inch%s", inches, (inches == 1) ? "" : "es");
    if (!volume) {
      // this only kicks in if no volume
      sprintf(volumeBuf + strlen(volumeBuf), "0 cubic inches");
    }

    if (compareWeights(wgt, 1.0) != 1) 
      ch->sendTo("It weighs about %d pound%s and occupies roughly %s.\n\r", 
               (int) wgt, ((((int) wgt) == 1) ? "" : "s"), volumeBuf);
    else 
      ch->sendTo("It weighs about %d drechel%s and occupies roughly %s.\n\r", 
               getDrechels(TRUE), ((getDrechels(TRUE) == 1) ? "" : "s"), volumeBuf);
  }
  describeCondition(ch);
  if (isObjStat(ITEM_GLOW))
    act("It is <o>glowing<1>.", FALSE, ch, 0, 0, TO_CHAR);
  if (isObjStat(ITEM_BURNING))
    act("It is <r>burning<1>.", FALSE, ch, 0, 0, TO_CHAR);
  if (isObjStat(ITEM_CHARRED))
    act("It is <k>charred<1>.", FALSE, ch, 0, 0, TO_CHAR);
  describeContains(ch);

  if (dynamic_cast<TBeing *>(rider)) {
    act("$p is being used by $N.", FALSE, ch, this, horseMaster(), TO_CHAR);
    for (t2 = rider->nextRider; t2; t2 = t2->nextRider) {
      if (t2 == horseMaster())
        continue;
      if (!dynamic_cast<TBeing *>(t2))
        continue;
      act("$p is also being used by $N.", FALSE, ch, this, t2, TO_CHAR);
    }
  }
  if (action_description) {
    strcpy(capbuf, action_description);
    if ((sscanf(capbuf, "This is the personalized object of %s.", name_buf)) == 1)
      sendTo("A monogram on it indicates it belongs to %s.\n\r", name_buf);
  }
  describeObjectSpecifics(ch);
}

void TBeing::describeObject(const TThing *t) const
{
  t->describeMe(this);
}

string TBeing::describeSharpness(const TThing *obj) const
{
  return obj->describeMySharp(this);
}

string TThing::describeMySharp(const TBeing *) const
{
  char buf[256];
  sprintf(buf, "%s is not a weapon", getName());
  return buf;
}

string TBeing::describePointiness(const TBaseWeapon *obj) const
{
  char buf[256];
  char sharpbuf[80];
  int maxsharp = obj->getMaxSharp();
  int sharp = obj->getCurSharp();
  double diff;

  if (!maxsharp)
    diff = (double) 0;
  else
    diff = (double) ((double) sharp / (double) maxsharp);
//  strcpy(capbuf, objs(obj));
  string capbuf = colorString(this,desc, objs(obj), NULL, COLOR_OBJECTS, TRUE);

  if (diff <= .02)
    strcpy(sharpbuf, "is totally blunt");
  else if (diff < .10)
    strcpy(sharpbuf, "is virtually blunt now");
  else if (diff < .30)
    strcpy(sharpbuf, "has very little of its point left");
  else if (diff < .50)
    strcpy(sharpbuf, "is starting to get blunt");
  else if (diff < .70)
    strcpy(sharpbuf, "has some pointedness");
  else if (diff < .90)
    strcpy(sharpbuf, "has a good point");
  else if (diff < 1.0)
    strcpy(sharpbuf, "is nice and pointy");
  else
    strcpy(sharpbuf, "is as pointed as it is going to get");

  sprintf(buf, "%s %s", good_uncap(capbuf).c_str(), sharpbuf);
  return buf;
}

string TBeing::describeBluntness(const TBaseWeapon *obj) const
{
  char buf[256];
  char sharpbuf[80];
  int maxsharp = obj->getMaxSharp();
  int sharp = obj->getCurSharp();
  double diff;
  if (!maxsharp)
    diff = (double) 0;
  else
    diff = (double) ((double) sharp / (double) maxsharp);
//  strcpy(capbuf, objs(obj));
  string capbuf = colorString(this,desc, objs(obj), NULL, COLOR_OBJECTS, TRUE);

  if (diff <= .02)
    strcpy(sharpbuf, "is totally jagged");
  else if (diff < .10)
    strcpy(sharpbuf, "is extremely jagged now");
  else if (diff < .30)
    strcpy(sharpbuf, "has become jagged");
  else if (diff < .50)
    strcpy(sharpbuf, "is fairly jagged");
  else if (diff < .70)
    strcpy(sharpbuf, "has a lot of chips and is starting to get jagged");
  else if (diff < .90)
    strcpy(sharpbuf, "has some chips");
  else if (diff < 1.0)
    strcpy(sharpbuf, "has a few chips");
  else
    strcpy(sharpbuf, "is completely blunt");

  sprintf(buf, "%s %s", good_uncap(capbuf).c_str(), sharpbuf);
  return buf;
}

void TBeing::describeMaxSharpness(const TBaseWeapon *obj, int learn) const
{
  if (obj->isBluntWeapon()) {
    describeMaxBluntness(obj, learn);
    return;
  } else if (obj->isPierceWeapon()) {
    describeMaxPointiness(obj, learn);
    return;
  }
  if (!hasClass(CLASS_THIEF) && !hasClass(CLASS_WARRIOR) && 
      !hasClass(CLASS_DEIKHAN) && !hasClass(CLASS_RANGER))
    learn /= 3;

  int maxsharp = GetApprox(obj->getMaxSharp(), learn);

  char capbuf[80], sharpbuf[80];
  strcpy(capbuf, objs(obj));

  if (maxsharp >= 99)
    strcpy(sharpbuf, "being unhumanly sharp");
  else if (maxsharp >= 90)
    strcpy(sharpbuf, "being razor sharp");
  else if (maxsharp >= 80)
    strcpy(sharpbuf, "approximating razor sharpness");
  else if (maxsharp >= 72)
    strcpy(sharpbuf, "being incredibly sharp");
  else if (maxsharp >= 64)
    strcpy(sharpbuf, "being very sharp");
  else if (maxsharp >= 56)
    strcpy(sharpbuf, "being sharp");
  else if (maxsharp >= 48)
    strcpy(sharpbuf, "being fairly sharp");
  else if (maxsharp >= 40)
    strcpy(sharpbuf, "having a sword-like edge");
  else if (maxsharp >= 30)
    strcpy(sharpbuf, "having a very sharp edge");
  else if (maxsharp >= 20)
    strcpy(sharpbuf, "having a knife-like edge");
  else if (maxsharp >= 10)
    strcpy(sharpbuf, "having a sharp edge");
  else
    strcpy(sharpbuf, "having a ragged edge");

  sendTo(COLOR_OBJECTS,"%s seems to be capable of %s.\n\r",
           cap(capbuf), sharpbuf);
}

void TBeing::describeMaxPointiness(const TBaseWeapon *obj, int learn) const
{
  char capbuf[80], sharpbuf[80];
  strcpy(capbuf, objs(obj));

  if (!hasClass(CLASS_THIEF) && !hasClass(CLASS_WARRIOR) && 
      !hasClass(CLASS_DEIKHAN) && !hasClass(CLASS_RANGER) &&
      !hasClass(CLASS_SHAMAN) && !hasClass(CLASS_MAGIC_USER))
    learn /= 3;

  int maxsharp = GetApprox(obj->getMaxSharp(), learn);

  if (maxsharp >= 99)
    strcpy(sharpbuf, "being unhumanly pointy");
  else if (maxsharp >= 90)
    strcpy(sharpbuf, "being awesomly pointy");
  else if (maxsharp >= 80)
    strcpy(sharpbuf, "having an amazing point");
  else if (maxsharp >= 72)
    strcpy(sharpbuf, "being incredibly pointy");
  else if (maxsharp >= 64)
    strcpy(sharpbuf, "being very pointy");
  else if (maxsharp >= 56)
    strcpy(sharpbuf, "being pointy");
  else if (maxsharp >= 48)
    strcpy(sharpbuf, "being fairly pointy");
  else if (maxsharp >= 40)
    strcpy(sharpbuf, "having a dagger-like point");
  else if (maxsharp >= 30)
    strcpy(sharpbuf, "having a nice point");
  else if (maxsharp >= 20)
    strcpy(sharpbuf, "having a spear-like point");
  else if (maxsharp >= 10)
    strcpy(sharpbuf, "having a point");
  else
    strcpy(sharpbuf, "having a dull point");

  sendTo(COLOR_OBJECTS,"%s seems to be capable of %s.\n\r",
           cap(capbuf), sharpbuf);
}

void TBeing::describeOtherFeatures(const TGenWeapon *obj, int learn) const
{
  char capbuf[80];
  strcpy(capbuf, objs(obj));

  if (!obj) {
    sendTo("Something went wrong, tell a god what you did.\n\r");
    return;
  }

  if (hasClass(CLASS_THIEF) || isImmortal()) {
    if (obj->canCudgel())
      sendTo(COLOR_OBJECTS, "%s seems small enough to be used for cudgeling.\n\r",
             cap(capbuf));
    if (obj->canStab())
      sendTo(COLOR_OBJECTS, "%s seems small enough to be used for stabbing.\n\r",
             cap(capbuf));
    if (obj->canBackstab())
      sendTo(COLOR_OBJECTS, "%s seems small enough to be used for backstabbing.\n\r",
             cap(capbuf));
  }
}

void TBeing::describeMaxBluntness(const TBaseWeapon *obj, int learn) const
{
  char capbuf[80], sharpbuf[80];
  strcpy(capbuf, objs(obj));

  if (!hasClass(CLASS_CLERIC) && !hasClass(CLASS_WARRIOR) && 
      !hasClass(CLASS_SHAMAN) && !hasClass(CLASS_DEIKHAN))
    learn /= 3;

  int maxsharp = GetApprox(obj->getMaxSharp(), learn);

  if (maxsharp >= 99)
    strcpy(sharpbuf, "being polished smooth");
  else if (maxsharp >= 90)
    strcpy(sharpbuf, "being awesomly blunt");
  else if (maxsharp >= 80)
    strcpy(sharpbuf, "being extremely blunt");
  else if (maxsharp >= 72)
    strcpy(sharpbuf, "being incredibly blunt");
  else if (maxsharp >= 64)
    strcpy(sharpbuf, "being very blunt");
  else if (maxsharp >= 56)
    strcpy(sharpbuf, "being blunt");
  else if (maxsharp >= 48)
    strcpy(sharpbuf, "being fairly blunt");
  else if (maxsharp >= 40)
    strcpy(sharpbuf, "having a hammer-like bluntness");
  else if (maxsharp >= 30)
    strcpy(sharpbuf, "being somewhat blunt");
  else if (maxsharp >= 20)
    strcpy(sharpbuf, "having a mace-like bluntness");
  else if (maxsharp >= 10)
    strcpy(sharpbuf, "having a ragged bluntness");
  else
    strcpy(sharpbuf, "having a sharp and ragged bluntness");

  sendTo(COLOR_OBJECTS,"%s seems to be capable of %s.\n\r",
           cap(capbuf), sharpbuf);
}

void TBeing::describeMaxStructure(const TObj *obj, int learn) const
{
  obj->descMaxStruct(this, learn);
}

void TBeing::describeWeaponDamage(const TBaseWeapon *obj, int learn) const
{
  if (!hasClass(CLASS_RANGER) &&
      !hasClass(CLASS_WARRIOR) && 
      !hasClass(CLASS_DEIKHAN) &&
      !hasWizPower(POWER_WIZARD)) {
    learn /= 3;
  }

#if 1
  double av_dam = GetApprox(obj->damageLevel(), learn);

  sendTo(COLOR_OBJECTS, "It is capable of doing %s of damage for your level\n\r", 
         describe_damage((int) av_dam, this));
#else
  double av_dam = obj->baseDamage();
  av_dam += (double) obj->itemDamroll();
  av_dam = GetApprox((int) av_dam, learn);

  sendTo(COLOR_OBJECTS,"It's capable of doing %s damage.\n\r",
          ((av_dam < 1) ? "exceptionally low" :
          ((av_dam < 2) ? "incredibly low" :
          ((av_dam < 3) ? "very low" :
          ((av_dam < 4) ? "low" :
          ((av_dam < 5) ? "fairly low" :
          ((av_dam < 6) ? "moderate" :
          ((av_dam < 7) ? "a good bit of" :
          ((av_dam < 8) ? "fairly good" :
          ((av_dam < 9) ? "good" :
          ((av_dam < 10) ? "very good" :
          ((av_dam < 11) ? "really good" :
          ((av_dam < 12) ? "exceptionally good" :
          ((av_dam < 13) ? "very nice" :
          ((av_dam < 14) ? "a sizable amount of" :
          ((av_dam < 15) ? "respectable" :
          ((av_dam < 16) ? "whopping" :
          ((av_dam < 17) ? "punishing" :
          ((av_dam < 18) ? "devestating" :
          ((av_dam < 19) ? "awesome" :
          ((av_dam < 20) ? "superb" :
                           "super-human")))))))))))))))))))));
#endif
}

void TBeing::describeArmor(const TBaseClothing *obj, int learn)
{
  if (!hasClass(CLASS_RANGER) && !hasClass(CLASS_WARRIOR) && 
      !hasClass(CLASS_DEIKHAN))
    learn /= 3;

#if 1
  double tACPref,
         tStrPref,
         tSHLvl = suggestArmor(),
         tIsLvl = obj->armorLevel(ARMOR_LEV_AC),
         tACMin,
         tACQua;

  obj->armorPercs(&tACPref, &tStrPref);

  // This tells us what they 'should have' on the slot:
  tSHLvl *= tACPref;

  // Convert tSHLvl to a level
  tACPref *= (obj->isPaired() ? 2.0 : 1.0);
  tACMin = ((500.0 * tACPref) + (obj->isPaired() ? 1.0 : 0.5));
  tACQua = (max(0.0, (tSHLvl - tACMin)) / tACPref);
  tSHLvl = tACQua / 25;

  int tDiff = GetApprox((int) (tIsLvl - tSHLvl), learn);
  string tStLevel("");

  if (tDiff < -20)
    tStLevel = "a horrid amount";
  else if (tDiff < -15)
    tStLevel = "a sad amount";
  else if (tDiff < -10)
    tStLevel = "a pathetic amount";
  else if (tDiff < -5)
    tStLevel = "a decent amount";
  else if (tDiff <= -1)
    tStLevel = "a near perfect amount";
  else if (tDiff == 0)
    tStLevel = "a Perfect amount";
  else if (tDiff <= 2)
    tStLevel = "a near perfect amount"; // This and -1 is where we confuse them
  else if (tDiff < 5)
    tStLevel = "a good amount";
  else if (tDiff < 10)
    tStLevel = "a really good amount";
  else if (tDiff < 15)
    tStLevel = "an extremely good amount";
  else
    tStLevel = "way too much of an amount";

  sendTo(COLOR_OBJECTS, "This supplies %s of protection for your class and level\n\r",
         tStLevel.c_str());
#else
  int armor = 0;    // works in reverse here.  armor > 0 is GOOD
  armor -= obj->itemAC();
  armor = GetApprox(armor, learn);

  sendTo(COLOR_OBJECTS,"In terms of armor quality, it ranks as %s.\n\r",
      ((armor < 0) ? "being more hurt then help" :
      ((armor < 2) ? "being virtually non-existant" :
      ((armor < 4) ? "being exceptionally low" :
      ((armor < 6) ? "being low" :
      ((armor < 8) ? "being fairly good" :
      ((armor < 10) ? "being somewhat good" :
      ((armor < 12) ? "being good" :
      ((armor < 14) ? "being really good" :
      ((armor < 16) ? "protecting you fairly well" :
      ((armor < 18) ? "protecting you well" :
      ((armor < 20) ? "protecting you very well" :
      ((armor < 22) ? "protecting you really well" :
      ((armor < 24) ? "protecting you exceptionally well" :
      ((armor < 26) ? "armoring you like a fortress" :
      ((armor < 28) ? "armoring you like a dragon" :
      ((armor < 30) ? "being virtually impenetrable" :
                       "being impenetrable")))))))))))))))));
#endif
}

string TBeing::describeImmunities(const TBeing *vict, int learn) const
{
  char buf[80];
  char buf2[256];
  string str;

  int x;
  for (immuneTypeT i = MIN_IMMUNE;i < MAX_IMMUNES; i++) {
    x = GetApprox(vict->getImmunity(i), learn);

    if (x == 0 || !*immunity_names[i])
      continue;
    if (x > 90 || x < -90)
      strcpy(buf, "extremely");
    else if (x > 70 || x < -70)
      strcpy(buf, "heavily");
    else if (x > 50 || x < -50)
      strcpy(buf, "majorly");
    else if (x > 30 || x < -30)
      strcpy(buf, "greatly");
    else if (x > 10 || x < -10)
      strcpy(buf, "somewhat");
    else
      strcpy(buf, "lightly");

    if (vict == this) 
      sprintf(buf2, "You are %s %s to %s.\n\r",
         buf, (x > 0 ? "resistant" : "susceptible"),
         immunity_names[i]);
    else
      sprintf(buf2, "%s is %s %s to %s.\n\r",
         good_cap(pers(vict)).c_str(),
         buf, (x > 0 ? "resistant" : "susceptible"),
         immunity_names[i]);
    str += buf2;
  }
  return str;
}

void TBeing::describeArrowDamage(const TArrow *obj, int learn)
{
  if (!hasClass(CLASS_RANGER))
    learn /= 3;

  double av_dam = obj->baseDamage();
  av_dam += (double) obj->itemDamroll();
  av_dam = GetApprox((int) av_dam, learn);

  sendTo(COLOR_OBJECTS, "It's capable of doing %s damage.\n\r",
          ((av_dam < 1) ? "exceptionally low" :
          ((av_dam < 2) ? "incredibly low" :
          ((av_dam < 3) ? "very low" :
          ((av_dam < 4) ? "low" :
          ((av_dam < 5) ? "fairly low" :
          ((av_dam < 6) ? "moderate" :
          ((av_dam < 7) ? "a good bit of" :
          ((av_dam < 8) ? "fairly good" :
          ((av_dam < 9) ? "good" :
          ((av_dam < 10) ? "very good" :
          ((av_dam < 11) ? "really good" :
          ((av_dam < 12) ? "exceptionally good" :
          ((av_dam < 13) ? "very nice" :
          ((av_dam < 14) ? "a sizable amount of" :
          ((av_dam < 15) ? "respectable" :
          ((av_dam < 16) ? "whopping" :
          ((av_dam < 17) ? "punishing" :
          ((av_dam < 18) ? "devestating" :
          ((av_dam < 19) ? "awesome" :
          ((av_dam < 20) ? "superb" :
                           "super-human")))))))))))))))))))));
}

void TBeing::describeArrowSharpness(const TArrow *obj, int learn)
{
  if (!hasClass(CLASS_RANGER))
    learn /= 3;

  int maxsharp = GetApprox(obj->getCurSharp(), learn);
 
  char capbuf[80], sharpbuf[80];
  strcpy(capbuf, objs(obj));
 
  if (maxsharp >= 99)
    strcpy(sharpbuf, "unhumanly sharp");
  else if (maxsharp >= 90)
    strcpy(sharpbuf, "razor sharp");
  else if (maxsharp >= 80)
    strcpy(sharpbuf, "almost razor sharp");
  else if (maxsharp >= 72)
    strcpy(sharpbuf, "incredibly sharp");
  else if (maxsharp >= 64)
    strcpy(sharpbuf, "very sharp");
  else if (maxsharp >= 56)
    strcpy(sharpbuf, "sharp");
  else if (maxsharp >= 48)
    strcpy(sharpbuf, "fairly sharp");
  else if (maxsharp >= 40)
    strcpy(sharpbuf, "kind of sharp");
  else if (maxsharp >= 30)
    strcpy(sharpbuf, "not really sharp");
  else if (maxsharp >= 20)
    strcpy(sharpbuf, "dull");
  else if (maxsharp >= 10)
    strcpy(sharpbuf, "very dull");
  else
    strcpy(sharpbuf, "extremely dull");
 
  sendTo(COLOR_OBJECTS, "%s has a tip that is %s.\n\r", cap(capbuf), sharpbuf);

}

void TBeing::describeNoise(const TObj *obj, int learn) const
{
  if (!dynamic_cast<const TBaseClothing *>(obj) &&
      !dynamic_cast<const TBaseWeapon *>(obj) && 
      !dynamic_cast<const TBow *>(obj))
    return;

  if (obj->canWear(ITEM_HOLD) || obj->canWear(ITEM_WEAR_FINGER))
    return;

  int iNoise = GetApprox(material_nums[obj->getMaterial()].noise, learn);

  char capbuf[160];
  strcpy(capbuf, objs(obj));

  sendTo(COLOR_OBJECTS, "%s is %s.\n\r", cap(capbuf),
          ((iNoise < -9) ? "beyond silent" :
          ((iNoise < -5) ? "extremely silent" :
          ((iNoise < -2) ? "very silent" :
          ((iNoise < 1) ? "silent" :
          ((iNoise < 3) ? "very quiet" :
          ((iNoise < 6) ? "quiet" :
          ((iNoise < 10) ? "pretty quiet" :
          ((iNoise < 14) ? "mostly quiet" :
          ((iNoise < 19) ? "slightly noisy" :
          ((iNoise < 25) ? "fairly noisy" :
          ((iNoise < 31) ? "noisy" :
          ((iNoise < 38) ? "very noisy" :
                           "loud")))))))))))));
}

void TBeing::describeRoomLight()
{
  int illum = roomp->getLight();

  sendTo(COLOR_BASIC, "This area is %s.\n\r", 
          ((illum < -4) ? "<k>super dark<1>" :
          ((illum < 0) ? "<k>pitch dark<1>" :
          ((illum < 1) ? "<k>dark<1>" :
          ((illum < 3) ? "<w>very dimly lit<1>" :
          ((illum < 5) ? "<w>dimly lit<1>" :
          ((illum < 9) ? "barely lit" :
          ((illum < 13) ? "lit" :
          ((illum < 19) ? "brightly lit" :
          ((illum < 25) ? "<Y>very brightly lit<1>" :
                           "<Y>bright as day<1>"))))))))));
}

void TBeing::describeBowRange(const TBow *obj, int learn)
{
  if (!hasClass(CLASS_RANGER))
    learn /= 3;

  int range = GetApprox((int) obj->getMaxRange(), learn);

  char capbuf[160];
  strcpy(capbuf, objs(obj));

  sendTo(COLOR_OBJECTS, "%s can %s.\n\r", cap(capbuf),
          ((range < 1) ? "not shoot out of the immediate area" :
          ((range < 3) ? "barely shoot beyond arm's length" :
          ((range < 5) ? "shoot a short distance" :
          ((range < 8) ? "fire a reasonable distance" :
          ((range < 11) ? "shoot quite a ways" :
          ((range < 15) ? "shoot a long distance" :
                           "fire incredibly far")))))));
}

void TBeing::describeMagicLevel(const TMagicItem *obj, int learn) const
{
  if (!hasClass(CLASS_MAGIC_USER) && !hasClass(CLASS_CLERIC) &&
      !hasClass(CLASS_RANGER)  && !hasClass(CLASS_DEIKHAN))
    return;

  int level = GetApprox(obj->getMagicLevel(), learn);
  level = max(level,0);

  char capbuf[160];
  strcpy(capbuf, objs(obj));

  sendTo(COLOR_OBJECTS, "Spells from %s seem to be cast at %s level.\n\r", 
          uncap(capbuf),
          numberAsString(level).c_str());

}

const string numberAsString(int num)
{
  char buf[50];

  if (in_range(num%100, 11, 13))
    sprintf(buf, "%dth", num);
  else if ((num%10) == 1)
    sprintf(buf, "%dst", num);
  else if ((num%10) == 2)
    sprintf(buf, "%dnd", num);
  else if ((num%10) == 3)
    sprintf(buf, "%drd", num);
  else
    sprintf(buf, "%dth", num);

  return buf;
}

void TBeing::describeMagicLearnedness(const TMagicItem *obj, int learn) const
{
  if (!hasClass(CLASS_MAGIC_USER) && !hasClass(CLASS_CLERIC) &&
      !hasClass(CLASS_RANGER)  && !hasClass(CLASS_DEIKHAN))
    return;

  int level = GetApprox(obj->getMagicLearnedness(), learn);

  char capbuf[160];
  strcpy(capbuf, objs(obj));

  sendTo(COLOR_OBJECTS, "The learnedness of the spells in %s is: %s.\n\r", uncap(capbuf),
                how_good(level));
}

void TBeing::describeMagicSpell(const TMagicItem *obj, int learn)
{
  char capbuf[160];
  strcpy(capbuf, objs(obj));

  if (!hasClass(CLASS_MAGIC_USER) && !hasClass(CLASS_CLERIC) &&
      !hasClass(CLASS_RANGER)  && !hasClass(CLASS_DEIKHAN))
    return;

  int level = GetApprox(getSkillLevel(SKILL_EVALUATE), learn);

  if (obj->getMagicLevel() > level) {
    sendTo(COLOR_OBJECTS, "You can tell nothing about the spells %s produces.\n\r", uncap(capbuf));
    return;
  }

  obj->descMagicSpells(this);
}

void TWand::descMagicSpells(TBeing *ch) const
{
  discNumT das = DISC_NONE;
  spellNumT iSpell;
  char capbuf[160];
  strcpy(capbuf, ch->objs(this));

  if ((iSpell = getSpell()) >= MIN_SPELL && discArray[iSpell] &&
      ((das = getDisciplineNumber(iSpell, FALSE)) != DISC_NONE)) {
    if (ch->doesKnowSkill(iSpell))
      ch->sendTo(COLOR_OBJECTS, "%s produces: %s.\n\r", cap(capbuf), 
            discArray[iSpell]->name);
    else
      ch->sendTo(COLOR_OBJECTS, "%s produces: Something from the %s discipline.\n\r", cap(capbuf),  disc_names[das]);
  }

  return;
}

void TStaff::descMagicSpells(TBeing *ch) const
{
  discNumT das = DISC_NONE;
  spellNumT iSpell;
  char capbuf[160];
  strcpy(capbuf, ch->objs(this));

  if ((iSpell = getSpell()) >= MIN_SPELL && discArray[iSpell] &&
      ((das = getDisciplineNumber(iSpell, FALSE)) != DISC_NONE)) {
    if (ch->doesKnowSkill(iSpell))
      ch->sendTo(COLOR_OBJECTS, "%s produces: %s.\n\r", cap(capbuf), 
            discArray[iSpell]->name);
    else
      ch->sendTo(COLOR_OBJECTS, "%s produces: Something from the %s discipline.\n\r", cap(capbuf),  disc_names[das]);
  }

  return;
}

void TScroll::descMagicSpells(TBeing *ch) const
{
  discNumT das = DISC_NONE;
  spellNumT spell;
  char capbuf[160];
  strcpy(capbuf, ch->objs(this));

  spell = getSpell(0);
  if (spell > TYPE_UNDEFINED && discArray[spell] &&
      ((das = getDisciplineNumber(spell, FALSE)) != DISC_NONE)) {
    if (ch->doesKnowSkill(spell))
      ch->sendTo(COLOR_OBJECTS, "%s produces: %s.\n\r", cap(capbuf), 
            discArray[spell]->name);
    else
      ch->sendTo(COLOR_OBJECTS, "%s produces: Something from the %s discipline.\n\r", cap(capbuf),  disc_names[das]);
  }

  spell = getSpell(1);
  if (spell > TYPE_UNDEFINED && discArray[spell] &&
      ((das = getDisciplineNumber(spell, FALSE)) != DISC_NONE)) {
    if (ch->doesKnowSkill(spell))
      ch->sendTo(COLOR_OBJECTS, "%s produces: %s.\n\r", cap(capbuf), 
            discArray[spell]->name);
    else
       ch->sendTo(COLOR_OBJECTS, "%s produces: Something from the %s discipline.\n\r", cap(capbuf), disc_names[das]);
  }

  spell = getSpell(2);
  if (spell > TYPE_UNDEFINED && discArray[spell] &&
      ((das = getDisciplineNumber(spell, FALSE)) != DISC_NONE)) {
    if (ch->doesKnowSkill(spell))
      ch->sendTo(COLOR_OBJECTS, "%s produces: %s.\n\r", cap(capbuf), 
            discArray[spell]->name);
    else
       ch->sendTo(COLOR_OBJECTS, "%s produces: Something from the %s discipline.\n\r", cap(capbuf), disc_names[das]);
  }

  return;
}

void TPotion::descMagicSpells(TBeing *ch) const
{
  discNumT das = DISC_NONE;
  spellNumT spell;
  char capbuf[160];
  strcpy(capbuf, ch->objs(this));

  spell = getSpell(0);
  if (spell >= 0 && discArray[spell] &&
      ((das = getDisciplineNumber(spell, FALSE)) != DISC_NONE)) {
    if (ch->doesKnowSkill(spell))
      ch->sendTo(COLOR_OBJECTS, "%s produces: %s.\n\r", cap(capbuf), 
            discArray[spell]->name);
    else
      ch->sendTo(COLOR_OBJECTS, "%s produces: Something from the %s discipline.\n\r", cap(capbuf),  disc_names[das]);
  }

  spell = getSpell(1);
  if (spell >= 0 && discArray[spell] &&
      ((das = getDisciplineNumber(spell, FALSE)) != DISC_NONE)) {
    if (ch->doesKnowSkill(spell))
      ch->sendTo(COLOR_OBJECTS, "%s produces: %s.\n\r", cap(capbuf), 
            discArray[spell]->name);
    else
       ch->sendTo(COLOR_OBJECTS, "%s produces: Something from the %s discipline.\n\r", cap(capbuf), disc_names[das]);
  }

  spell = getSpell(2);
  if (spell >= 0 && discArray[spell] &&
      ((das = getDisciplineNumber(spell, FALSE)) != DISC_NONE)) {
    if (ch->doesKnowSkill(spell))
      ch->sendTo(COLOR_OBJECTS, "%s produces: %s.\n\r", cap(capbuf), 
            discArray[spell]->name);
    else
       ch->sendTo(COLOR_OBJECTS, "%s produces: Something from the %s discipline.\n\r", cap(capbuf), disc_names[das]);
  }

  return;
}

void TBeing::describeSymbolOunces(const TSymbol *obj, int learn) const
{
  if (obj->getSymbolFaction() != FACT_UNDEFINED) {
    act("$p is already attuned.", false, this, obj, 0, TO_CHAR);
    return;
  }

  if (!hasClass(CLASS_CLERIC) && !hasClass(CLASS_DEIKHAN))
    learn /= 3;

  // number of ounces needed, see attuning
  int amt = obj->obj_flags.cost / 100;
  amt = GetApprox(amt, learn);

  char capbuf[160];
  strcpy(capbuf, objs(obj));

  sendTo(COLOR_OBJECTS, "%s requires about %d ounce%s of holy water to attune.\n\r", cap(capbuf), amt, amt == 1 ? "" : "s");

  return;
}

void TBeing::describeComponentUseage(const TComponent *obj, int) const
{
  char capbuf[160];
  strcpy(capbuf, objs(obj));

  if (IS_SET(obj->getComponentType(), COMP_SPELL))
    sendTo(COLOR_OBJECTS, "%s is a component used in creating magic.\n\r", cap(capbuf));
  else if (IS_SET(obj->getComponentType(), COMP_POTION))
    sendTo(COLOR_OBJECTS, "%s is a component used to brew potions.\n\r", cap(capbuf));
  else if (IS_SET(obj->getComponentType(), COMP_SCRIBE))
    sendTo(COLOR_OBJECTS, "%s is a component used during scribing.\n\r", cap(capbuf));

  return;
}

void TBeing::describeComponentDecay(const TComponent *obj, int learn) const
{
  if (!hasClass(CLASS_MAGIC_USER) && !hasClass(CLASS_CLERIC) &&
      !hasClass(CLASS_RANGER)  && !hasClass(CLASS_DEIKHAN))
    learn /= 3;

  int level = GetApprox(obj->obj_flags.decay_time, learn);

  char capbuf[160];
  strcpy(capbuf, objs(obj));

  sendTo(COLOR_OBJECTS, "%s will last ", cap(capbuf));

  if (!obj->isComponentType(COMP_DECAY)) {
    sendTo("well into the future.\n\r");
    return;
  }

  if ((level <= -1) || (level >= 800))
    sendTo("well into the future.\n\r");
  else if (level < 50)
    sendTo("about a day.\n\r");
  else if (level < 100)
    sendTo("a few days *tops*.\n\r");
  else if (level < 200)
    sendTo("about a week.\n\r");
  else if (level < 400)
    sendTo("only a couple of weeks.\n\r");
  else if (level < 800)
    sendTo("around a month.\n\r");

  return;
}

void TBeing::describeComponentSpell(const TComponent *obj, int learn) const
{
  if (!hasClass(CLASS_MAGIC_USER) && !hasClass(CLASS_CLERIC) &&
      !hasClass(CLASS_RANGER)  && !hasClass(CLASS_DEIKHAN))
    learn /= 3;

//  int level = GetApprox(getSkillLevel(SKILL_EVALUATE), learn);

  char capbuf[160];
  strcpy(capbuf, objs(obj));

#if 0
  if (obj->getMagicLevel() > level) {
    sendTo(COLOR_OBJECTS, "You can tell nothing about the spell %s is used for.\n\r", uncap(capbuf));
    return;
  }
#endif

  int which = obj->getComponentSpell();

  if (which >= 0 && discArray[which])
    sendTo(COLOR_OBJECTS, "%s is used for: %s.\n\r", cap(capbuf),
          discArray[which]->name);

  return;
}

string describeMaterial(const TThing *t)
{
  string str;
  char buf[256];

  int mat = t->getMaterial();
  char mat_name[40];

  strcpy(mat_name, good_uncap(material_nums[mat].mat_name).c_str());

  if (dynamic_cast<const TBeing *>(t))
    sprintf(buf, "%s has a skin type of %s.\n\r", good_cap(t->getName()).c_str(), mat_name);
  else
    sprintf(buf, "%s is made of %s.\n\r", good_cap(t->getName()).c_str(), mat_name);
  str += buf;

  str += describeMaterial(mat);

  return str;
}

string describeMaterial(int mat)
{
  string str;
  char buf[256];

  char mat_name[40];

  strcpy(mat_name, good_uncap(material_nums[mat].mat_name).c_str());

  sprintf(buf, "%s is %d%% susceptible to slash attacks.\n\r",
     good_cap(mat_name).c_str(),
     material_nums[mat].cut_susc);
  str += buf;

  sprintf(buf, "%s is %d%% susceptible to pierce attacks.\n\r",
     good_cap(mat_name).c_str(),
     material_nums[mat].pierced_susc);
  str += buf;

  sprintf(buf, "%s is %d%% susceptible to blunt attacks.\n\r",
     good_cap(mat_name).c_str(),
     material_nums[mat].smash_susc);
  str += buf;

  sprintf(buf, "%s is %d%% susceptible to flame attacks.\n\r",
     good_cap(mat_name).c_str(),
     material_nums[mat].burned_susc);
  str += buf;

  sprintf(buf, "%s is %d%% susceptible to acid attacks.\n\r",
     good_cap(mat_name).c_str(),
     material_nums[mat].acid_susc);
  str += buf;

  sprintf(buf, "%s is %d%% susceptible to water erosion, and suffers %d damage per erosion.\n\r",
     good_cap(mat_name).c_str(),
     material_nums[mat].water_susc%10 * 10,
     material_nums[mat].water_susc/10);
  str += buf;

  sprintf(buf, "%s is %d%% susceptible to fall shock, and suffers %d damage per shock.\n\r",
     good_cap(mat_name).c_str(),
     material_nums[mat].fall_susc%10 *10,
     material_nums[mat].fall_susc/10);
  str += buf;

  sprintf(buf, "%s has a hardness of %d units.\n\r",
     good_cap(mat_name).c_str(),
     material_nums[mat].hardness);
  str += buf;

  sprintf(buf, "%s has a compaction ratio of %d:1.\n\r",
     good_cap(mat_name).c_str(),
     material_nums[mat].vol_mult);
  str += buf;

  sprintf(buf, "%s is %sconsidered a conductive material.\n\r",
     good_cap(mat_name).c_str(),
     (material_nums[mat].conductivity ? "" : "not "));
  str += buf;

  return str;
}

void TBeing::sendRoomName(TRoom *rp) const
{
  unsigned int rFlags = rp->getRoomFlags();
  Descriptor *d = desc;
  char clientBuf[20];

  if (!d)
    return;

  sprintf(clientBuf, "\200%d|", CLIENT_ROOMNAME);

  if (IS_SET(desc->plr_color, PLR_COLOR_ROOM_NAME)) {
    if (hasColorStrings(this, rp->getName(), 2)) {
      sendTo(COLOR_ROOM_NAME,"%s%s%s%s%s%s\n\r", 
                d->m_bIsClient ? clientBuf : "",
                dynColorRoom(rp, 1, TRUE).c_str(),
                norm(), red(), 
                ((rFlags & ROOM_NO_HEAL) ? "     [NOHEAL]" :
                ((rFlags & ROOM_HOSPITAL) ? "     [HOSPITAL]" :
                ((rFlags & ROOM_ARENA) ? "     [ARENA]" :
                " "))), norm());
    } else {
      sendTo(COLOR_ROOM_NAME,"%s%s%s%s%s%s%s\n\r", 
                d->m_bIsClient ? clientBuf : "",  
                addColorRoom(rp, 1).c_str(),
                rp->getName(), norm(), red(),  
                ((rFlags & ROOM_NO_HEAL) ? "     [NOHEAL]" :
                ((rFlags & ROOM_HOSPITAL) ? "     [HOSPITAL]" :
                ((rFlags & ROOM_ARENA) ? "     [ARENA]" :
                " "))), norm());
    }
  } else {
    if (hasColorStrings(this, rp->getName(), 2)) {
      sendTo(COLOR_BASIC,"%s%s%s%s%s%s\n\r", 
              d->m_bIsClient ? clientBuf : "", purple(), 
              colorString(this, desc, rp->getName(), NULL, COLOR_NONE, FALSE).c_str(),
              red(),
              ((rFlags & ROOM_NO_HEAL) ? "     [NOHEAL]" :
              ((rFlags & ROOM_HOSPITAL) ? "     [HOSPITAL]" :
              ((rFlags & ROOM_HOSPITAL) ? "     [ARENA]" :
             " "))), norm());
    } else {
      sendTo(COLOR_BASIC,"%s%s%s%s%s%s\n\r", d->m_bIsClient ? clientBuf : "", 
             purple(),rp->getName(), red(),
             ((rFlags & ROOM_NO_HEAL) ? "     [NOHEAL]" :
             ((rFlags & ROOM_HOSPITAL) ? "     [HOSPITAL]" :
             ((rFlags & ROOM_HOSPITAL) ? "     [ARENA]" :
             " "))), norm());
    }
  }
  if (isImmortal() && (desc->prompt_d.type & PROMPT_BUILDER_ASSISTANT)) {
    sendTo("{ %s%s%s%s%s%s%s%s%s%s%s%s%s%s}\n\r",
           (rFlags == 0 ?
           "--none-- "       : ""),
           (!(rFlags & (ROOM_ALWAYS_LIT | ROOM_NO_MOB    | ROOM_INDOORS |
                        ROOM_PEACEFUL   | ROOM_NO_STEAL  | ROOM_NO_SUM  |
                        ROOM_NO_MAGIC   | ROOM_NO_PORTAL | ROOM_SILENCE |
                        ROOM_NO_ORDER   | ROOM_NO_FLEE   | ROOM_HAVE_TO_WALK)) &&
            (rFlags > 0)                 ? "--others-- "     : ""),
           ((rFlags & ROOM_ALWAYS_LIT)   ? "[Light] "        : ""),
           ((rFlags & ROOM_NO_MOB)       ? "[!Mob] "         : ""),
           ((rFlags & ROOM_INDOORS)      ? "[Indoors] "      : ""),
           ((rFlags & ROOM_PEACEFUL)     ? "[Peaceful] "     : ""),
           ((rFlags & ROOM_NO_STEAL)     ? "[!Steal] "       : ""),
           ((rFlags & ROOM_NO_SUM)       ? "[!Summon] "      : ""),
           ((rFlags & ROOM_NO_MAGIC)     ? "[!Magic] "       : ""),
           ((rFlags & ROOM_NO_PORTAL)    ? "[!Portal] "      : ""),
           ((rFlags & ROOM_SILENCE)      ? "[Silent] "       : ""),
           ((rFlags & ROOM_NO_ORDER)     ? "[!Order] "       : ""),
           ((rFlags & ROOM_NO_FLEE)      ? "[!Flee] "        : ""),
           ((rFlags & ROOM_HAVE_TO_WALK) ? "[Have-To-Walk] " : ""));
  }
}

void TBeing::sendRoomDesc(TRoom *rp) const
{
  if (hasColorStrings(this, rp->getDescr(), 2)) {
    sendTo(COLOR_ROOMS, "%s%s", dynColorRoom(rp, 2, TRUE).c_str(), norm());
  } else {
    sendTo(COLOR_ROOMS, "%s%s%s", addColorRoom(rp, 2).c_str(), rp->getDescr(), norm());
  }
}

void TBeing::describeTrapEffect(const TTrap *, int) const
{
  if (!hasClass(CLASS_THIEF))
    return;

  // this tells things like the triggers, why let them know these?
  return;
}

void TBeing::describeTrapLevel(const TTrap *obj, int learn) const
{
  if (!hasClass(CLASS_THIEF))
    return;

  int level = GetApprox(obj->getTrapLevel(), learn);
  level = max(level,0);

  sendTo(COLOR_OBJECTS, "%s seems to be a %s level trap.\n\r", 
       good_cap(objs(obj)).c_str(), numberAsString(level).c_str());
}

void TBeing::describeTrapCharges(const TTrap *obj, int learn) const
{
  if (!hasClass(CLASS_THIEF))
    return;

  int level = GetApprox(obj->getTrapCharges(), learn);
  level = max(level,0);

  sendTo(COLOR_OBJECTS, "%s seems to have %d charge%s left.\n\r", 
       good_cap(objs(obj)).c_str(), level, level == 1 ? "" : "s");
}

void TBeing::describeTrapDamType(const TTrap *obj, int) const
{
  if (!hasClass(CLASS_THIEF))
    return;

  sendTo(COLOR_OBJECTS, "You suspect %s is %s %s trap.\n\r", 
       good_uncap(objs(obj)).c_str(),
       startsVowel(trap_types[obj->getTrapDamType()]) ? "an" : "a",
       good_uncap(trap_types[obj->getTrapDamType()]).c_str());
}

void TBeing::doSpells(const char *argument)
{
  char buf[MAX_STRING_LENGTH * 2], buffer[MAX_STRING_LENGTH * 2];
  char learnbuf[64];
  spellNumT i;
  unsigned int j, l;
  Descriptor *d;
  CDiscipline *cd;
  char arg[200], arg2[200], arg3[200];
  int subtype=0, types[4], type=0, badtype=0, showall=0;
  discNumT das;
  TThing *primary=heldInPrimHand(), *secondary=heldInSecHand();
  TThing *belt=equipment[WEAR_WAISTE];
  TComponent *item=NULL;
  int totalcharges;
  wizardryLevelT wizlevel = getWizardryLevel();
  TThing *t1, *t2;
  TOpenContainer *tContainer;

  struct {
    TThing *where;
    wizardryLevelT wizlevel;
  } search[] = {
      {primary  , WIZ_LEV_COMP_PRIM_OTHER_FREE},
      {secondary, WIZ_LEV_COMP_EITHER         },
      {stuff    , WIZ_LEV_COMP_INV            },
      {belt     , WIZ_LEV_COMP_BELT           }
  };

  if (!(d = desc))
    return;

  *buffer = '\0';

  if (!*argument)
    memset(types, 1, sizeof(int) * 4);      
  else {
    memset(types, 0, sizeof(int) * 4);

    three_arg(argument, arg, arg2, arg3);    

    if (*arg3 && is_abbrev(arg3, "all"))
      showall = 1;

    if (*arg2) {
      if (is_abbrev(arg2, "all"))
        showall = 1;
      else if (is_abbrev(arg2, "targeted"))
        subtype = 1;
      else if (is_abbrev(arg2, "nontargeted"))
        subtype = 2;
      else
        badtype = 1;
    }
    
    if (is_abbrev(arg, "offensive")) {
      if(!subtype || subtype == 1)
        types[0] = 1;
      if(!subtype || subtype == 2)
        types[1] = 1;
    } else if (is_abbrev(arg, "utility")) {
      if(!subtype || subtype == 1)
        types[2] = 1;
      if(!subtype || subtype == 2)
        types[3] = 1;      
    } else
      badtype = 1;
    
    if (badtype) {
      sendTo("You must specify a valid spell type.\n\r");
      sendTo("Syntax: spells <offensive|utility> <targeted|nontargeted> <all>.\n\r");
      return;
    }
  }

  vector<skillSorter>skillSortVec(0);

  for (i = MIN_SPELL; i < MAX_SKILL; i++) {
    if (hideThisSpell(i) || !discArray[i]->minMana)
      continue;

    skillSortVec.push_back(skillSorter(this, i));
  }
  
  // sort it into proper order
  sort(skillSortVec.begin(), skillSortVec.end(), skillSorter());

  for (type = 0; type < 4;++type) {
    if (!types[type])
      continue;

    if (*buffer)
      strcat(buffer, "\n\r");

    switch (type) {
      case 0:
        strcat(buffer, "Targeted offensive spells:\n\r");
        break;
      case 1:
        strcat(buffer, "Non-targeted offensive spells:\n\r");
        break;
      case 2:
        strcat(buffer, "Targeted utility spells:\n\r");
        break;
      case 3:
        strcat(buffer, "Non-targeted utility spells:\n\r");
        break;
    }

    for (j = 0; j < skillSortVec.size(); j++) {
      i = skillSortVec[j].theSkill;
      das = getDisciplineNumber(i, FALSE);
      if (das == DISC_NONE) {
        vlogf(LOG_BUG, "Bad disc for skill %d in doSpells", i);
        continue;
      }
      cd = getDiscipline(das);
      
      // getLearnedness is -99 for an unlearned skill, make it seem like a 0
      int tmp_var = ((!cd || cd->getLearnedness() <= 0) ? 0 : cd->getLearnedness());
      tmp_var = min((int) MAX_DISC_LEARNEDNESS, tmp_var);

      switch (type) {
        case 0: // single target offensive
          if (!(discArray[i]->targets & TAR_VIOLENT) ||
              (discArray[i]->targets & TAR_AREA))
            continue;
          break;
        case 1: // area offensive
          if (!(discArray[i]->targets & TAR_VIOLENT) ||
              !(discArray[i]->targets & TAR_AREA))
            continue;
          break;
        case 2: // targeted utility
          if ((discArray[i]->targets & TAR_VIOLENT) ||
              !(discArray[i]->targets & TAR_CHAR_ROOM))
            continue;
          break;
        case 3: // non-targeted utility
          if ((discArray[i]->targets &  TAR_VIOLENT) ||
              (discArray[i]->targets & TAR_CHAR_ROOM))
            continue;
          break;
      }

      // can't we say if !cd, continue here?
      if (cd && !cd->ok_for_class && getSkillValue(i) <= 0) 
        continue;

      totalcharges = 0;
      item = NULL;
      
      for (l = 0; l < 4; l++) {
        if (search[l].where && wizlevel >= search[l].wizlevel) {
          for (t1 = search[l].where; t1; t1 = t1->nextThing) {
            if (!(item = dynamic_cast<TComponent *>(t1)) &&
                (!(tContainer = dynamic_cast<TOpenContainer *>(item)) ||
                 !tContainer->isClosed())) {
              for (t2 = t1->stuff; t2; t2 = t2->nextThing) {
                if ((item = dynamic_cast<TComponent *>(t2)) &&
                    item->getComponentSpell() == i && 
                    item->isComponentType(COMP_SPELL))
                  totalcharges += item->getComponentCharges();
              }
            } else if (item->getComponentSpell() == i && 
                       item->isComponentType(COMP_SPELL))
              totalcharges += item->getComponentCharges();
          }
        }
      }

      if ((getSkillValue(i) <= 0) &&
          (!tmp_var || (discArray[i]->start - tmp_var) > 0)) {
        if (!showall) 
          continue;

        sprintf(buf, "%s%-22.22s%s  (Learned: %s)", 
                cyan(), discArray[i]->name, norm(),
                skill_diff(discArray[i]->start - tmp_var));
      } else if (discArray[i]->toggle && 
                 !hasQuestBit(discArray[i]->toggle)) {
        if (!showall) 
          continue;

        sprintf(buf, "%s%-22.22s%s  (Learned: Quest)",
                cyan(), discArray[i]->name, norm());
      } else { 
        if (getMaxSkillValue(i) < MAX_SKILL_LEARNEDNESS) {
          if (discArray[i]->startLearnDo > 0) {
            sprintf(learnbuf, "%.9s/%.9s", how_good(getSkillValue(i)),
                    how_good(getMaxSkillValue(i))+1);
            sprintf(buf, "%s%-22.22s%s %-19.19s",
                    cyan(), discArray[i]->name, norm(), 
                    learnbuf);
          } else {
            sprintf(buf, "%s%-22.22s%s %-19.19s",
                    cyan(), discArray[i]->name, norm(), 
                    how_good(getSkillValue(i)));
          }
        } else {
          sprintf(buf, "%s%-22.22s%s %-19.19s",
                  cyan(), discArray[i]->name, norm(), 
                  how_good(getSkillValue(i)));
        }
        unsigned int comp;

        for (comp = 0; (comp < CompInfo.size()) &&
                       (i != CompInfo[comp].spell_num); comp++);

        if (comp != CompInfo.size() && CompInfo[comp].comp_num >= 0) {
          sprintf(buf + strlen(buf), "   [%3i] %s",  totalcharges, 
                  obj_index[real_object(CompInfo[comp].comp_num)].short_desc);
        }         
      }
        strcat(buf, "\n\r");
        
      if (strlen(buf) + strlen(buffer) > (MAX_STRING_LENGTH * 2) - 2)
        break;

      strcat(buffer, buf);
    } 
  }
  d->page_string(buffer);
  return;
}

void TBeing::doPrayers(const char *argument)
{
  char buf[MAX_STRING_LENGTH * 2] = "\0";
  char buffer[MAX_STRING_LENGTH * 2] = "\0";
  char learnbuf[64];
  spellNumT i;
  unsigned int j, l;
  Descriptor *d;
  CDiscipline *cd;
  char arg[200], arg2[200], arg3[200];
  int subtype=0, types[4], type=0, badtype=0, showall=0;
  discNumT das;
  TThing *primary = heldInPrimHand(), *secondary = heldInSecHand();
  TThing *belt = equipment[WEAR_WAISTE];
  TComponent *item = NULL;
  int totalcharges;
  wizardryLevelT wizlevel = getWizardryLevel();
  TThing *t1, *t2;
  TOpenContainer *tContainer;

  struct {
    TThing *where;
    wizardryLevelT wizlevel;
  } search[]={{primary, WIZ_LEV_COMP_PRIM_OTHER_FREE}, {secondary, WIZ_LEV_COMP_EITHER}, {stuff, WIZ_LEV_COMP_INV}, {belt, WIZ_LEV_COMP_BELT}};

  if (!(d = desc))
    return;

  if(!*argument)
    memset(types, 1, sizeof(int)*4);      
  else {
    memset(types, 0, sizeof(int)*4);

    three_arg(argument, arg, arg2, arg3);    

    if (*arg3 && is_abbrev(arg3, "all"))
      showall = 1;

    if (*arg2){
      if (is_abbrev(arg2, "all")) 
        showall=1;
        else if(is_abbrev(arg2, "targeted")) 
        subtype=1;
        else if(is_abbrev(arg2, "nontargeted")) 
        subtype=2;
        else badtype=1;
    }      
    if (is_abbrev(arg, "offensive")){
        if (!subtype || subtype==1) 
        types[0] = 1;

        if (!subtype || subtype==2) 
        types[1] = 1;
    } else if(is_abbrev(arg, "utility")) {
        if (!subtype || subtype==1) 
        types[2] = 1;
        
      if (!subtype || subtype==2) 
        types[3] = 1;      
    } else  
      badtype = 1;
      
    if (badtype) {
        sendTo("You must specify a valid spell type.\n\r");
        sendTo("Syntax: spells <offensive|utility> <targeted|nontargeted> <all>.\n\r");
        return;
    }
  }

  vector<skillSorter>skillSortVec(0);

  for (i = MIN_SPELL; i < MAX_SKILL; i++) {
    if (hideThisSpell(i) || !discArray[i]->minMana)
      continue;
    skillSortVec.push_back(skillSorter(this, i));
  }  
    
  sort(skillSortVec.begin(), skillSortVec.end(), skillSorter());

  for (type = 0;type < 4;++type) {
    if (!types[type])
      continue;

    if(*buffer)
        strcat(buffer, "\n\r");

    switch(type){
      case 0:
        strcat(buffer, "Targeted offensive spells:\n\r");
        break;
      case 1:
        strcat(buffer, "Non-targeted offensive spells:\n\r");
        break;
      case 2:
        strcat(buffer, "Targeted utility spells:\n\r");
        break;
      case 3:
        strcat(buffer, "Non-targeted utility spells:\n\r");
        break;
    }
    for (j = 0; j < skillSortVec.size(); j++) {
      i = skillSortVec[j].theSkill;
      das = getDisciplineNumber(i, FALSE);
      if (das == DISC_NONE) {
        vlogf(LOG_BUG, "Bad disc for skill %d in doPrayers", i);
          continue;
      }
      cd = getDiscipline(das);
        
      // getLearnedness is -99 for an unlearned skill, make it seem like a 0
      int tmp_var = ((!cd || cd->getLearnedness() <= 0) ? 0 : cd->getLearnedness());
      tmp_var = min((int) MAX_DISC_LEARNEDNESS, tmp_var);

      switch (type) {
        case 0: // single target offensive
          if(!(discArray[i]->targets & TAR_VIOLENT) ||
                (discArray[i]->targets & TAR_AREA))
                continue;
  
          break;
        case 1: // area offensive
          if(!(discArray[i]->targets & TAR_VIOLENT) ||
             !(discArray[i]->targets & TAR_AREA))
            continue;
  
          break;
         case 2: // targeted utility
          if((discArray[i]->targets & TAR_VIOLENT) ||
             !(discArray[i]->targets & TAR_CHAR_ROOM))
            continue;

          break;
        case 3: // non-targeted utility
          if((discArray[i]->targets &  TAR_VIOLENT) ||
             (discArray[i]->targets & TAR_CHAR_ROOM))
            continue;

          break;
      }
      // can't we say if !cd, continue here?
      if (cd && !cd->ok_for_class && getSkillValue(i) <= 0) 
        continue;

      totalcharges = 0;
      item = NULL;
        
      for (l = 0; l < 4; ++l){
        if (search[l].where && wizlevel >= search[l].wizlevel) {
          for (t1 = search[l].where; t1; t1 = t1->nextThing) {
            if(!(item=dynamic_cast<TComponent *>(t1)) &&
               (!(tContainer = dynamic_cast<TOpenContainer *>(item)) ||
                !tContainer->isClosed())) {
              for (t2 = t1->stuff; t2; t2 = t2->nextThing) {
                if ((item=dynamic_cast<TComponent *>(t2)) &&
                  item->getComponentSpell() == i && 
                  item->isComponentType(COMP_SPELL))
                  totalcharges += item->getComponentCharges();
                }
              } else if(item->getComponentSpell() == i && 
                item->isComponentType(COMP_SPELL))
                
              totalcharges += item->getComponentCharges();
            }
          }
        }
        if ((getSkillValue(i) <= 0) &&
          (!tmp_var || (discArray[i]->start - tmp_var) > 0)) {
          
        if (!showall) 
          continue;

        sprintf(buf, "%s%-22.22s%s  (Learned: %s)",  cyan(), discArray[i]->name, norm(), skill_diff(discArray[i]->start - tmp_var));
      } else if (discArray[i]->toggle && !hasQuestBit(discArray[i]->toggle)) {
          if (!showall) 
          continue;

          sprintf(buf, "%s%-22.22s%s  (Learned: Quest)", cyan(), discArray[i]->name, norm());
      } else { 
        if (getMaxSkillValue(i) < MAX_SKILL_LEARNEDNESS) {
          if (discArray[i]->startLearnDo > 0) {
            sprintf(learnbuf, "%.9s/%.9s", how_good(getSkillValue(i)), how_good(getMaxSkillValue(i))+1);
            sprintf(buf, "%s%-22.22s%s %-19.19s", cyan(), discArray[i]->name, norm(), learnbuf);
          } else 
            sprintf(buf, "%s%-22.22s%s %-19.19s", cyan(), discArray[i]->name, norm(), how_good(getSkillValue(i)));   
        } else 
          sprintf(buf, "%s%-22.22s%s %-19.19s", cyan(), discArray[i]->name, norm(), how_good(getSkillValue(i)));
            
        unsigned int comp;

        for (comp = 0; (comp < CompInfo.size()) && (i != CompInfo[comp].spell_num);comp++);

        if (comp != CompInfo.size() && CompInfo[comp].comp_num >= 0) 
          sprintf(buf + strlen(buf), "   [%2i] %s",  totalcharges, obj_index[real_object(CompInfo[comp].comp_num)].short_desc); 
      }
      strcat(buf, "\n\r");
          
      if (strlen(buf) + strlen(buffer) > (MAX_STRING_LENGTH * 2) - 2)
        break;

      strcat(buffer, buf);
    } 
  }
  d->page_string(buffer);
  return;
}
