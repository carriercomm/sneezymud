//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//   create_mobs.cc : Online mobile creation/saving/loading for builders.
//
//////////////////////////////////////////////////////////////////////////

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
}

#include "stdsneezy.h"
#include "create.h"
#include "mail.h"
#include "combat.h"
#include "statistics.h"

static void stripSpellAffects(TBeing * mob)
{
  // strip off any magic effects, don't want spells screwing up stats
  affectedData *aff, *aff2;
  for (aff = mob->affected; aff; aff = aff2) {
    aff2 = aff->next;
    mob->affectRemove(aff);
  }
}

static bool isBadForActFlags(int update)
{
  switch (update) {
    case ACT_STRINGS_CHANGED:
    case ACT_HATEFUL:
    case ACT_AFRAID:
    case ACT_HUNTING:
    case ACT_DEADLY:
    case ACT_POLYSELF:
    case ACT_GUARDIAN:
      return TRUE;
    default:
      return FALSE;
  }
}

static bool isBadForAffectFlags(int update)
{
  switch (1<<update) {
    case AFF_GROUP:
    case AFF_CURSE:
    case AFF_POISON:
    case AFF_PARALYSIS:
    case AFF_SLEEP:
    case AFF_STUNNED:
    case AFF_SHOCKED:
    case AFF_UNDEF3:    
    case AFF_UNDEF4:
    case AFF_ENGAGER:
    case AFF_SCRYING:
    case AFF_WEB:
    case AFF_CHARM:
      return TRUE;
    default:
      return FALSE;
  }
}

static void send_mob_menu(const TBeing *ch, const TMonster *tMon)
{
  const char *mob_edit_menu_basic =
 " %s1)%s Name                        %s 2)%s Short Description\n\r"
 " %s3)%s Long Description            %s 4)%s Description\n\r"
 " %s5)%s Action flags                %s 6)%s Affect flags\n\r"
 " %s7)%s Faction                     %s 8)%s Number of attacks\n\r"
 " %s9)%s Level                       %s10)%s Hitroll\n\r"
 "%s11)%s Armor Level                 %s12)%s HP Level\n\r"
 "%s13)%s Damage Level                %s14)%s Money constant\n\r"
 "%s15)%s unused                      %s16)%s Race\n\r"
 "%s17)%s Sex                         %s18)%s Max Exist\n\r"
 "%s19)%s Default Position            %s20)%s Mobile Strings\n\r"
 "%s21)%s Immunities                  %s22)%s Skin type\n\r"
 "%s23)%s Class                       %s24)%s Characteristics\n\r"
 "%s25)%s Height                      %s26)%s Weight\n\r"
 "%s27)%s Special Procedure           %s28)%s Vision Bonus\n\r"
 "%s29)%s Can be seen                 %s30)%s Mobile sounds\n\r"
 "\n\r";
  const char *mob_edit_menu_advanced =
 " %s1)%s %-20s  %s2)%s %s\n\r"
 " %s3)%s %-20s  %s4)%s %sDescription%s\n\r"
 " %s5)%s %sAction Flags%s          %s6)%s %sAffect Flags%s\n\r"
 " %s7)%s %sFaction%s: %-11s  %s8)%s %sAttacks%s: %s\n\r"
 " %s9)%s %sLvl%s: %-15s %s10)%s %sHitRoll%s: %s\n\r"
 "%s11)%s %sAC-Lvl%s: %-12s %s12)%s %sHP-Lvl%s: %s\n\r"
 "%s13)%s %sDam-Lvl%s: %-11s %s14)%s %sMoney%s:%s\n\r"
 "%s15)%s %sUnused%s               %s16)%s %s\n\r"
 "%s17)%s %-20s %s18)%s %sMax%s: %s\n\r"
 "%s19)%s %-20s %s20)%s %sMobile Strings%s\n\r"
 "%s21)%s %sImmunities%s           %s22)%s %s\n\r"
 "%s23)%s %-20s %s24)%s %sCharacteristics%s\n\r"
 "%s25)%s %sHeight%s: %-12s %s26)%s %sWeight%s: %s\n\r"
 "%s27)%s %-20s %s28)%s %sVB%s: %s\n\r"
 "%s29)%s %sCBS%s: %-15s %s30)%s %sMobile Sounds%s\n\r"
  "\n\r";

  if (IS_SET(ch->desc->autobits, AUTO_TIPS)) {
    char tStringOut[22][256];

    strcpy(tStringOut[0], (tMon->name ? tMon->name : "Unknown"));
    strcpy(tStringOut[1], (tMon->shortDescr ? tMon->shortDescr : "Unknown"));
    strcpy(tStringOut[2], (tMon->getLongDesc() ? tMon->getLongDesc() : "Unknown"));
    sprintf(tStringOut[3], "%d %3.2f", tMon->getFaction(), tMon->getPerc());
    sprintf(tStringOut[4], "%.1f", tMon->getMult());
    sprintf(tStringOut[5], "%d", tMon->GetMaxLevel());
    sprintf(tStringOut[6], "%d", tMon->getHitroll());
    sprintf(tStringOut[7], "%.1f", tMon->getACLevel());
    sprintf(tStringOut[8], "%.1f", tMon->getHPLevel());
    sprintf(tStringOut[9], "%.1f+%d", tMon->getDamLevel(), tMon->getDamPrecision());
    sprintf(tStringOut[10], "%d", tMon->moneyConst);
    strcpy(tStringOut[11], tMon->getMyRace()->getSingularName().c_str());
    sprintf(tStringOut[12], "%s", (!tMon->getSex() ? "Other" : (tMon->getSex() == 1 ? "Male" : "Female")));
    sprintf(tStringOut[13], "%d", tMon->max_exist);
    strcpy(tStringOut[14], position_types[tMon->default_pos]);
    strcpy(tStringOut[15], material_nums[tMon->getMaterial()].mat_name);
    strcpy(tStringOut[16], tMon->getProfName());
    int tHeight = (int)(tMon->getHeight() / 12);
    sprintf(tStringOut[17], "%d\'%d\" (%d)", tHeight, (tMon->getHeight() - (tHeight * 12)), tMon->getHeight());
    sprintf(tStringOut[18], "%.0f (lbs)", tMon->getWeight());
    strcpy(tStringOut[19], ((tMon->spec < NUM_MOB_SPECIALS) ? (tMon->spec <= 0 ? "Proc: none" : mob_specials[GET_MOB_SPE_INDEX(tMon->spec)].name) : "Confused..."));
    sprintf(tStringOut[20], "%d", tMon->visionBonus);
    sprintf(tStringOut[21], "%d", tMon->canBeSeen);

    for (int tMsgIndex = 0; tMsgIndex < 22; tMsgIndex++)
      if (strlen(tStringOut[tMsgIndex]) > 20) {
        tStringOut[tMsgIndex][16] = '\0';
        strcat(tStringOut[tMsgIndex], "...");
      }

    ch->sendTo(COLOR_MOBS, mob_edit_menu_advanced,
          ch->cyan()  , ch->norm(),                           tStringOut[0],
          ch->purple(), ch->norm(),                           tStringOut[1],
          ch->cyan()  , ch->norm(),                           tStringOut[2],
          ch->purple(), ch->norm(), ch->cyan()  , ch->norm(),
          ch->cyan()  , ch->norm(), ch->purple(), ch->norm(),
          ch->purple(), ch->norm(), ch->cyan()  , ch->norm(),
          ch->cyan()  , ch->norm(), ch->purple(), ch->norm(), tStringOut[3],
          ch->purple(), ch->norm(), ch->cyan()  , ch->norm(), tStringOut[4],
          ch->cyan()  , ch->norm(), ch->purple(), ch->norm(), tStringOut[5],
          ch->purple(), ch->norm(), ch->cyan()  , ch->norm(), tStringOut[6],
          ch->cyan()  , ch->norm(), ch->purple(), ch->norm(), tStringOut[7],
          ch->purple(), ch->norm(), ch->cyan()  , ch->norm(), tStringOut[8],
          ch->cyan()  , ch->norm(), ch->purple(), ch->norm(), tStringOut[9],
          ch->purple(), ch->norm(), ch->cyan()  , ch->norm(), tStringOut[10],
          ch->cyan()  , ch->norm(), ch->purple(), ch->norm(),
          ch->purple(), ch->norm(),                           tStringOut[11],
          ch->cyan()  , ch->norm(),                           tStringOut[12],
          ch->purple(), ch->norm(), ch->cyan()  , ch->norm(), tStringOut[13],
          ch->cyan()  , ch->norm(),                           tStringOut[14],
          ch->purple(), ch->norm(), ch->cyan()  , ch->norm(),
          ch->cyan()  , ch->norm(), ch->purple(), ch->norm(),
          ch->purple(), ch->norm(),                           tStringOut[15],
          ch->cyan()  , ch->norm(),                           tStringOut[16],
          ch->purple(), ch->norm(), ch->cyan()  , ch->norm(),
          ch->cyan()  , ch->norm(), ch->purple(), ch->norm(), tStringOut[17],
          ch->purple(), ch->norm(), ch->cyan()  , ch->norm(), tStringOut[18],
          ch->cyan()  , ch->norm(),                           tStringOut[19],
          ch->purple(), ch->norm(), ch->cyan()  , ch->norm(), tStringOut[20],
          ch->cyan()  , ch->norm(), ch->purple(), ch->norm(), tStringOut[21],
          ch->purple(), ch->norm(), ch->cyan()  , ch->norm());
  } else
    ch->sendTo(COLOR_MOBS, mob_edit_menu_basic,
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm(),
          ch->cyan(), ch->norm(), ch->purple(), ch->norm());
}

static void TBeingLoad(TBeing *ch, int vnum)
{
  TMonster *mob;
  FILE *mob_f;
  char buf[256];
  int rc, num;

  sprintf(buf, "immortals/%s/mobs/%d", ch->name, vnum);
  if (!(mob_f = fopen(buf, "r"))) {
    ch->sendTo("Couldn't open that file.\n\r");
    return;
  }
  mob = new TMonster();

  // This stuff was ripped out of db.c - Russ
  
  fscanf(mob_f, "#%d\n", &num);
  ch->sendTo("Loading mob number %d.\n\r", num);

  rc = mob->readMobFromFile(mob_f, TRUE);
  if (IS_SET_DELETE(rc, DELETE_THIS)) {
    ch->sendTo("Mob deleted by initializer.\n\r");
    delete mob;
    return;
  }

  act(ch->msgVariables(MSG_MEDIT, mob).c_str(), TRUE, ch, 0, 0, TO_ROOM);

  if ((mob->getRace() == RACE_HORSE) || (mob->getRace() == RACE_DRAGON)) {
    mob->setMaxMove(mob->getMaxMove() + 150);
    mob->setMove(mob->moveLimit());
  }

  mob->convertAbilities();
  mob->snum = num;

  wearSlotT ij;
  for (ij = MIN_WEAR; ij < MAX_WEAR; ij++) {        // Initializing 
    mob->equipment[ij] = NULL;
    mob->setLimbFlags(ij, 0);
    mob->setCurLimbHealth(ij, mob->getMaxLimbHealth(ij));
    mob->setStuckIn(ij, NULL);
  }

  mob->desc = 0;
  mob->riding = NULL;

  // insert in list 
  mob->next = character_list;
  character_list = mob;

  mobCount++;

  *ch->roomp += *mob;

  fclose(mob_f);

  mob->checkMobStats(TINYFILE_NO);
}

static void TBeingSave(TBeing *ch, TMonster *mob, int vnum)
{
  FILE *fp;
  char buf[255], temp[4096];
  int j, k;

  if (!mob->name || !mob->getDescr() || 
      !mob->shortDescr || !mob->getLongDesc()) {
    ch->sendTo("Your mob is missing one or more strings.\n\r");
    ch->sendTo("Please update the follwing before saving:%s%s%s%s\n\r",
               (mob->name                 ? "" : " Name"),
               (mob->getDescr()           ? "" : " Description"),
               (mob->shortDescr           ? "" : " Short-Description"),
               (mob->getLongDesc() ? "" : " Long-Description"));
    return;
  }

  // make sure they have a mob directory 
  sprintf(buf, "immortals/%s/mobs", ch->getName());
  if (!(fp = fopen(buf, "r"))) {
    if (mkdir(buf, 0770)) {
      ch->sendTo("Unable to create a mobs directory for you.  Bug a coder.\n\r");
      return;
    } else
      ch->sendTo("Mobile directory created...\n\r");
  }
  if (fp)
    fclose(fp);

  sprintf(buf, "immortals/%s/mobs/%d", ch->name, vnum);
  if (!(fp = fopen(buf, "w"))) {
    ch->sendTo("Problem writing to disk. Maybe try again later.\n\r");
    return;
  }
  ch->sendTo("Saving.\n\r");
  for (j = 0, k = 0; k <= (int) strlen(mob->name); k++) {
    if (mob->name[k] != 13)
      temp[j++] = mob->name[k];
  }
  temp[j] = '\0';
  fprintf(fp, "#%d\n%s~\n", vnum, temp);
  for (j = 0, k = 0; k <= (int) strlen(mob->shortDescr); k++) {
    if (mob->shortDescr[k] != 13)
      temp[j++] = mob->shortDescr[k];
  }
  temp[j] = '\0';
  fprintf(fp, "%s~\n", temp);
  for (j = 0, k = 0; k <= (int) strlen(mob->getLongDesc()); k++) {
    if (mob->getLongDesc()[k] != 13)
      temp[j++] = mob->getLongDesc()[k];
  }
  temp[j] = '\0';
  fprintf(fp, "%s~\n", temp);
  for (j = 0, k = 0; k <= (int) strlen(mob->getDescr()); k++) {
    if (mob->getDescr()[k] != 13)
      temp[j++] = mob->getDescr()[k];
  }
  temp[j] = '\0';
  fprintf(fp, "%s~\n", temp);
  fprintf(fp, "%ld %ld %d %.1f %s %.1f\n",
        mob->specials.act,
        mob->specials.affectedBy,
        mob->getFaction(), (double) mob->getPerc(),
          (mob->sounds ? "L" : "A"),
        mob->getMult());
  fprintf(fp, "%d %d %d %.1f %.1f %.1f+%d\n",
        mob->getClass(),
        mob->GetMaxLevel(),
        mob->getHitroll(),
        mob->getACLevel(),
        mob->getHPLevel(),
        mob->getDamLevel(), 
        mob->getDamPrecision());
  fprintf(fp, "%d %d %d %d\n",
        mob->moneyConst, mob->getRace(), (int) mob->getWeight(), mob->getHeight());

  statTypeT local_stat;
  for(local_stat=MIN_STAT;local_stat<MAX_STATS_USED;local_stat++)
    fprintf(fp, "%d ", mob->getStat(STAT_CHOSEN, local_stat));
  fprintf(fp, "\n");

  fprintf(fp, "%d %d %d %d\n",
        mapPosToFile(mob->getPosition()),
        mapPosToFile(mob->default_pos),
      mob->getSex(), mob->spec);

  immuneTypeT ij;
  for (ij=MIN_IMMUNE;ij < 14;ij++)
    fprintf(fp, "%d ",mob->getImmunity(ij));
  fprintf(fp,"\n");
  for (;ij < MAX_IMMUNES;ij++)
    fprintf(fp, "%d ",mob->getImmunity(ij));
  fprintf(fp,"\n");

  fprintf(fp, "%d %d %d %d\n",
        mob->getMaterial(), mob->canBeSeen, mob->visionBonus, mob->max_exist);

  if (mob->sounds || mob->distantSnds)
    fprintf(fp, "%s~\n%s~\n", mob->sounds ? mob->sounds : "", mob->distantSnds ? mob->distantSnds : "");

  extraDescription *tExDescr;

  for (tExDescr = mob->ex_description; tExDescr; tExDescr = tExDescr->next) {
    int tMarker = 0;

    if (tExDescr->description) {
      for (unsigned int tPos = 0; tPos <= strlen(tExDescr->description); tPos++)
        if (tExDescr->description[tPos] != 13)
          temp[tMarker++] = tExDescr->description[tPos];

      temp[tMarker] = '\0';

      fprintf(fp, "E\n%s~\n%s~\n", tExDescr->keyword, tExDescr->description);
    }
    // No else.  if we didn't have text then something went wrong.  We Do Not save this one.
  }

  fclose(fp);
}

static void msave(TBeing *ch, char *argument)
{
  char i, buf[80];
  int vnum;

  TBeing *mob;

  for (i = 0; *(argument + i) == ' '; i++);
  if (!*(argument + i)) {
    ch->sendTo("Syntax :msave <mobile name> <vnum>\n\r");
    return;
  }
  if (sscanf(argument, "%s %d", buf, &vnum) != 2) {
    ch->sendTo("Syntax :msave <mobile name> <vnum>\n\r");
    return;
  }
  mob = get_char_vis(ch, buf, NULL);

  if (mob && vnum) {
    stripSpellAffects(mob);

    if (!mob->isPc())
      TBeingSave(ch, dynamic_cast<TMonster *>(mob), vnum);
    else {
      ch->sendTo("You can't save a player!\n\r");
      return;
    }
  } else {
    ch->sendTo("Syntax :msave <mobile name> <vnum>\n\r");
    return;
  }
}

static void update_mob_menu(TBeing *ch, TMonster *mob)
{
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("%sMobile Name:%s %s", ch->cyan(), ch->norm(), mob->name);
  ch->sendTo(VT_CURSPOS, 2, 1);
  ch->sendTo("%sCurrent level:%s %d", ch->purple(), ch->norm(), mob->GetMaxLevel());
  ch->sendTo(VT_CURSPOS, 3, 1);
  ch->sendTo("%sSuggested level:%s %d", ch->purple(), ch->norm(), 
    (int) ((double) 0.5 + (double) mob->getRealLevel()));
  ch->sendTo(VT_CURSPOS, 5, 1);
  ch->sendTo("Editing Menu:\n\r");
  send_mob_menu(ch, mob);
  ch->sendTo("--> ");
}

static void medit(TBeing *ch, char *arg)
{
  TBeing *mob, *k, *next_char;
  Descriptor *d;
  TRoom *rp;

  if (!ch->isPc())
    return;

  if (!ch->desc)
    return;

  if (!(mob = get_char_vis(ch, arg, NULL))) {
    ch->sendTo("No such mobile.\n\r");
    return;
  }
  if (mob->isPc()) {
    ch->sendTo("Edit a PC? NEVER!!!\n\r");
    return;
  }
  TMonster * mons = dynamic_cast<TMonster *>(mob);
  ch->specials.edit = MAIN_MENU;
  ch->desc->connected = CON_MEDITING;
  act("$n is sucked into a swirling vortex.", FALSE, mons, 0, 0, TO_ROOM);

  mons->swapToStrung();
  stripSpellAffects(mob);

  // return people poly'd into this mob 
  if (dynamic_cast<TPerson *>(mob) && !mob->desc) {
    for (d = descriptor_list; d; d = d->next)
      if (d->original == mob)
        d->character->doReturn("", WEAR_NOWHERE, 0);
  }
  if (mob->in_room == ROOM_NOWHERE) {
    rp = real_roomp(ROOM_VOID);
    *rp += *mob;
  }

  while (mob->rider)
    mob->rider->dismount(POSITION_STANDING);

  if (mob->followers || mob->master)
    mob->dieFollower();

  if (mob->fight())
    mob->stopFighting();

  for (k = gCombatList; k; k = next_char) {
    next_char = k->next_fighting;
    if (k->fight() == mob)
      k->stopFighting();
  }
  --(*mob);

  if (mob == character_list)
    character_list = mob->next;
  else {
    for (k = character_list; (k) && (k->next != mob); k = k->next);
    if (k)
      k->next = mob->next;
    else {
      vlogf(LOG_EDIT, "Trying to remove ?? from character_list.");
      abort();
    }
  }
  // Spit out all anyone who is switched 
  if (mob->desc) {
    if (mob->desc->original)
      mob->doReturn("", WEAR_NOWHERE, 0);

    mob->saveChar(ROOM_NOWHERE);
  }
  ch->desc->mob = dynamic_cast<TMonster *>(mob);
  act("$n just went into mobile edit mode.", FALSE, ch, 0, 0, TO_ROOM);
  update_mob_menu(ch, ch->desc->mob);
}

static void mlist(TPerson *ch)
{
  char buf[256];
  FILE *fp = NULL;

  // remove old temporary file, if any 
  sprintf(buf, "tmp/%s.tempfile", ch->getName());
  unlink(buf);

  // make sure they have an mob directory 
  sprintf(buf, "immortals/%s/mobs", ch->getName());
  if (!(fp = fopen(buf, "r"))) {
    if (mkdir(buf, 0770)) {
      ch->sendTo("Unable to create a mobs directory for you.  Bug Brutius.\n\r");
      return;
    } else
      ch->sendTo("Mobile directory created...\n\r");
  }
  if (fp)
    fclose(fp);

  if (!safe_to_be_in_system(ch->getName()))
    return;

  // create the listing 
#if 0
  sprintf(buf, "(cd immortals/%s/mobs;ls -C * > ../../../tmp/%s.tempfile)", ch->getName(), ch->getName());
  vsystem(buf);

  sprintf(buf, "tmp/%s.tempfile", ch->getName());
  ch->desc->start_page_file(buf, "No mobs found!\n\r");
#else
  generic_dirlist(buf, ch);
#endif
}


static void mremove(TBeing *ch, int vnum)
{
  char buf[256];

  sprintf(buf, "immortals/%s/mobs/%d", ch->getName(), vnum);
  if (unlink(buf))
    ch->sendTo("Unable to remove that mobile.  Sure you got the # right?\n\r");
}

static void change_mob_name(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  if (type != ENTER_CHECK)
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  if (type != ENTER_CHECK) {
    delete [] mob->name;
    mob->name = NULL;
    mob->name = mud_str_dup(arg);
    ch->specials.edit = MAIN_MENU;
    update_mob_menu(ch, mob);
    return;
  }
  ch->sendTo(VT_HOMECLR);

  ch->sendTo("Current Mobile Name: %s", mob->name);
  ch->sendTo("\n\r\n\rNew Mobile Name: ");

  return;
}

static void change_mob_short_desc(TBeing *ch, TMonster *mob, editorEnterTypeT type)
{
  if (type != ENTER_CHECK) {
    ch->specials.edit = MAIN_MENU;
    update_mob_menu(ch, mob);
    return;
  }
  ch->sendTo(VT_HOMECLR);

  ch->sendTo("Current short description:\n\r");
  ch->sendTo("%s", mob->shortDescr);
  ch->sendTo("\n\r\n\rNew short description:\n\r");
  ch->sendTo("ALWAYS start the short description with a lowercase letter.\n\r");
  ch->sendTo("Unless it is a formal name, like Brutius, or Kalas.\n\r");
  ch->sendTo("This helps us keep the things sent to character formatted nicely.\n\r");
  ch->sendTo("Terminate with a ~ ON THE SAME LINE. Press <ENTER> again to continue.\n\r");
  delete [] mob->shortDescr;
  mob->shortDescr = NULL;
  ch->desc->str = &mob->shortDescr;
  ch->desc->max_str = MAX_NAME_LENGTH-1;
  return;
}

static void change_mob_long_desc(TBeing *ch, TMonster *mob, editorEnterTypeT type)
{
  if (type != ENTER_CHECK) {
    ch->specials.edit = MAIN_MENU;
    update_mob_menu(ch, mob);
    return;
  }
  ch->sendTo(VT_HOMECLR);

  ch->sendTo("Current long description:\n\r");
  ch->sendTo("%s", mob->getLongDesc());
  ch->sendTo("\n\r\n\rNew mob long description:\n\r");
  ch->sendTo("Terminate with a ~ ON A SEPERATE LINE. Press <ENTER> again to continue.\n\r");
  delete [] mob->player.longDescr;
  mob->player.longDescr = NULL;
  ch->desc->str = &mob->player.longDescr;
  ch->desc->max_str = MAX_STRING_LENGTH;
  return;
}

static void change_mob_desc(TBeing *ch, TMonster *mob, editorEnterTypeT type)
{
  if (type != ENTER_CHECK) {
    ch->specials.edit = MAIN_MENU;
    update_mob_menu(ch, mob);
    return;
  }
  ch->sendTo(VT_HOMECLR);

  ch->sendTo("Current description:\n\r");
  ch->sendTo("%s", mob->getDescr());
  ch->sendTo("\n\r\n\rNew mob description:\n\r");
  ch->sendTo("Terminate with a ~. Press <ENTER> again to continue.\n\r");
  delete [] mob->getDescr();
  mob->setDescr(NULL);
  ch->desc->str = &mob->descr;
  ch->desc->max_str = MAX_STRING_LENGTH;
  return;
}

static void change_mob_act_flags(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int row, update, j;
  char buf[256];
  unsigned long i;

  if (type != ENTER_CHECK)
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  update = atoi(arg);
  update--;

  if (type != ENTER_CHECK) {
    if (update < 0 || update >= MAX_MOB_ACTS)
      return;
    i = 1 << update;
    if (isBadForActFlags(i)) {
      ch->sendTo("That flag cannot be changed.\n\r");
      return;
    }


    if (IS_SET(mob->specials.act, ACT_IMMORTAL)) {
      if ((i == ACT_PROTECTEE) || (i == ACT_PROTECTOR)) {
        ch->sendTo("Protections can't be set on immortal mobs. \n\r");
        return;
      }
    }


    if (IS_SET(mob->specials.act, ACT_PROTECTOR) || 
        IS_SET(mob->specials.act, ACT_PROTECTEE)) {
      if ((i == ACT_IMMORTAL)) {
        ch->sendTo("Protections can't be set on immortal mobs. \n\r");
        return;
      }
    }


    if (IS_SET(mob->specials.act, ACT_GHOST))
      if ((i == ACT_SKELETON) || (i == ACT_ZOMBIE)) {
        ch->sendTo("Ghostly can't be set in conjunction with other undead flags.\n\r");
        return;
      }
    if (IS_SET(mob->specials.act, ACT_ZOMBIE))
      if ((i == ACT_SKELETON) || (i == ACT_GHOST)) {
        ch->sendTo("Zombie can't be set in conjunction with other undead flags.\n\r");
        return;
      }
    if (IS_SET(mob->specials.act, ACT_SKELETON))
      if ((i == ACT_GHOST) || (i == ACT_ZOMBIE)) {
        ch->sendTo("Skeleton can't be set in conjunction with other undead flags.\n\r");
        return;
      }
    if (IS_SET(mob->specials.act, ACT_DIURNAL))
      if (i == ACT_NOCTURNAL) {
        ch->sendTo("If you want a full-time mob, don't set either nocturnal or diurnal.\n\r");
        return;
      }

    if (IS_SET(mob->specials.act, ACT_NOCTURNAL))
      if (i == ACT_DIURNAL) {
        ch->sendTo("If you want a full-time mob, don't set either nocturnal or diurnal.\n\r");
        return;
      }

    if (IS_SET(mob->specials.act, i))
      REMOVE_BIT(mob->specials.act, i);
    else
      SET_BIT(mob->specials.act, i);

    // this changes exp and level
    mob->setExp(mob->determineExp());
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Mobile act flags :");

  row = 0;
  for (j = 0; j < MAX_MOB_ACTS; j++) {
    sprintf(buf, VT_CURSPOS, row + 4, ((j & 1) ? 45 : 5));
    if (j & 1)
      row++;
    ch->sendTo(buf);
    ch->sendTo("%2d [%s] %s", j + 1, ((mob->specials.act & (1 << j)) ? "X" : " "), action_bits[j]);
  }
  ch->sendTo(VT_CURSPOS, 21, 1);
  ch->sendTo("Select number to toggle, <ENTER> to return to the main menu.\n\r--> ");
}

static void change_mob_affect_flags(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int row, update;
  char buf[256];
  unsigned long i;

  if (type != ENTER_CHECK)
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  update = atoi(arg);
  update--;

  if (type != ENTER_CHECK) {
    if (update < 0 || update > 28)
      return;
    if (isBadForAffectFlags(update)) {
      ch->sendTo("That flag cannot be changed.\n\r");
      return;
    }
    i = 1 << update;

    if (IS_SET(mob->specials.affectedBy, i))
      REMOVE_BIT(mob->specials.affectedBy, i);
    else
      SET_BIT(mob->specials.affectedBy, i);
    // this changes exp and level
    mob->setExp(mob->determineExp());
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Mobile affected by flags :");

  row = 0;
  for (i = 0; i < 28; i++) {
    sprintf(buf, VT_CURSPOS, row + 4, ((i & 1) ? 45 : 5));
    if (i & 1)
      row++;
    ch->sendTo(buf);
    ch->sendTo("%2d [%s] %s", i + 1, 
        ((mob->specials.affectedBy & (1 << i)) ? "X" : " ") ,affected_bits[i]);
  }
  ch->sendTo(VT_CURSPOS, 21, 1);
  ch->sendTo("Select number to toggle, <ENTER> to return to the main menu.\n\r--> ");
}

static void change_mob_faction_percent(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int new_faction;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_faction = atoi(arg);

    if (new_faction < 0 || new_faction > 100) {
      ch->sendTo("Please enter a number from 0 to 100.\n\r");
      return;
    } else {
      mob->setPerc((double) new_faction);
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob faction percent: %3.2f", mob->getPerc());
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("Select a new faction percent.\n\r--> ");
}

static void change_mob_faction_type(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int new_faction;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_faction = atoi(arg);

    if (new_faction < MIN_FACTION || new_faction >= MAX_FACTIONS ) {
      ch->sendTo("Please enter a number from %d to %d.\n\r",
                 MIN_FACTION, MAX_FACTIONS - 1);
      return;
    } else {
      mob->setFaction(factionTypeT(new_faction));
      ch->specials.edit = CHANGE_MOB_FACTION2;
      change_mob_faction_percent(ch, ch->desc->mob, "", ENTER_CHECK);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob faction: %d : %s", mob->getFaction(),
           FactionInfo[mob->getFaction()].faction_name);
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("Select a new faction.\n\r0: Unaffiliated    1: Brotherhood     2: Cult      3: Serpent Order\n\r--> ");
}

static void change_mob_mult_att(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  float new_att;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    if (sscanf(arg, "%f", &new_att) == 1) {
      mob->setMult((double) new_att);

      // this changes exp and level
      mob->setExp(mob->determineExp());
 
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    } else {
      ch->sendTo("Please enter a decimal number like 1.2 or 4.0. \n\r");
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob number of attacks: %.1f", mob->getMult());
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("Select a new attack number.\n\r--> ");
}

static void change_mob_level(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int new_level;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_level = atoi(arg);

    if (new_level < 1 || new_level > 100) {
      ch->sendTo("Please enter a number from 1 to 100.\n\r");
      return;
    } else {
      mob->fixLevels(new_level);
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  if (!UtilProcs(mob->spec) && !GuildProcs(mob->spec)) {
    ch->specials.edit = MAIN_MENU;
    update_mob_menu(ch, mob);
    ch->sendTo(VT_CURSPOS, 23, 1);
    ch->sendTo("Level is automatically set for this mob based on things like HP, AC and damage.\n\r");
    return;
  }

  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob level: %d", mob->GetMaxLevel());
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("Select a new level.\n\r--> ");
}

static void change_mob_thaco(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int new_thaco;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_thaco = atoi(arg);

    if (new_thaco < -50 || new_thaco > 50) {
      ch->sendTo("Please enter a number from -50 to 50.\n\r");
      return;
    } else {
      mob->setHitroll(new_thaco);

      // this changes exp and level
      mob->setExp(mob->determineExp());
 
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob hitroll: %d", mob->getHitroll());
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("A hitroll of 0 = normal for level.  A hitroll of +10 gives hitrate of level+1\n\r--> ");
  ch->sendTo(VT_CURSPOS, 5, 1);
  ch->sendTo("Select a new hitroll.\n\r--> ");
}

static void change_mob_class(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int row, update;
  char buf[256];

  if (type != ENTER_CHECK)
    if (!*arg || (*arg == '\n')) {
      if (!mob->isSingleClass() || !mob->getClass()) {
        ch->sendTo("Mobs must have one and only one class.\n\r");
        return;
      }
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  update = atoi(arg);
  update--;

  if (type != ENTER_CHECK) {
    if (update < MIN_CLASS_IND || update >= MAX_CLASSES)
      return;
    unsigned short i;
    i = 1 << update;

    if (IS_SET(mob->getClass(), i)) {
      REMOVE_BIT(mob->player.Class, i);
    } else {
      SET_BIT(mob->player.Class, i);
    }
    mob->fixLevels(mob->GetMaxLevel());

    // this changes exp and level
    mob->setExp(mob->determineExp());
 
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Mobile class :");

  row = 0;
  classIndT ij;
  for (ij = MIN_CLASS_IND; ij < MAX_CLASSES; ij++) {
    sprintf(buf, VT_CURSPOS, row + 3, ((ij % 2) ? 45 : 5));
    if (ij % 2 == 1)
      row++;
    ch->sendTo(buf);
    ch->sendTo("%2d [%s] %s", ij + 1, ((mob->getClass() & (1 << ij)) ? "X" : " "), classNames[ij].capName);
  }
  ch->sendTo(VT_CURSPOS, 21, 1);
  ch->sendTo("Select number to toggle, <ENTER> to return to the main menu.\n\r--> ");
}

static void change_mob_height(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int new_height;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_height = atoi(arg);

    if (new_height < 1 || new_height > 10000) {
      ch->sendTo("Please enter a number from 1 to 10000.\n\r");
      return;
    } else {
      mob->setHeight(new_height);
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Height affects whether they bump their heads on the ceiling.\n\r");
  ch->sendTo("So, MAKE SURE you are giving their height and not their length.\n\r");
  ch->sendTo("Current mob height: %d (inches)", mob->getHeight());
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("Select a new height (in inches).\n\r--> ");
}

static void change_mob_weight(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int new_weight;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_weight = atoi(arg);

    if (new_weight < 1 || new_weight > 100000) {
      ch->sendTo("Please enter a number from 1 to 100000.\n\r");
      return;
    } else {
      mob->setWeight(new_weight);
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob weight: %d (lbs.)", (int) mob->getWeight());
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("Select a new weight (in lbs.).\n\r--> ");
}

static void change_mob_vision(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int new_vision;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_vision = atoi(arg);

    if (new_vision < -100 || new_vision > 100) {
      ch->sendTo("Please enter a number from -100 to 100.\n\r");
      return;
    } else {
      mob->visionBonus = new_vision;
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob vision bonus: %d\n\r", mob->visionBonus);
  ch->sendTo("TBeing vision bonus will help a mob see in low light.\n\r");
  ch->sendTo("Values for light range from 0 at pitch black dark,\n\r");
  ch->sendTo("to 25 at noon time sunlight. To see more about the\n\r");
  ch->sendTo("lighting system and how it works see help lighting.\n\r");
  ch->sendTo("A monsters vision bonus should only be set for mobs\n\r");
  ch->sendTo("that intuitively should be able to see in the dark\n\r");
  ch->sendTo("better than others.\n\r");
  ch->sendTo(VT_CURSPOS, 12, 1);
  ch->sendTo("Select a new vision bonus.\n\r--> ");
}

static void change_mob_cbs(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int new_cbs;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_cbs = atoi(arg);

    if (new_cbs < 0 || new_cbs > 10000) {
      ch->sendTo("Please enter a number from 1 to 10000.\n\r");
      return;
    } else {
      mob->canBeSeen = new_cbs;
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob can be seen: %d\n\r", mob->canBeSeen);
  ch->sendTo("If you don't understand how the lighting\n\r");
  ch->sendTo("system works, please look at help light before going\n\r");
  ch->sendTo("further.\n\r");
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("Select a new can be seen.\n\r--> ");
}

static void change_mob_armor(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  float new_armor;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_armor = atof(arg);

    if (new_armor < 0 || new_armor > 70.0) {
      ch->sendTo("Please enter a number from 0.0 to 70.0.\n\r");
      return;
    } else {
      mob->setACLevel(new_armor);
      mob->setACFromACLevel();

      // this changes exp and level
      mob->setExp(mob->determineExp());
 
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob AC Level: %.1f", mob->getACLevel());
  ch->sendTo(VT_CURSPOS, 5, 1);
  ch->sendTo("AC Level should be from 0.0 to 70.0");
  ch->sendTo(VT_CURSPOS, 6, 1);
  ch->sendTo("AC Level will give the mob the appropriate armor for that level.");
  ch->sendTo(VT_CURSPOS, 8, 1);
  ch->sendTo("Select a new AC Level.\n\r--> ");
}

static void change_mob_hit_bonus(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  float new_bonus;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_bonus = atof(arg);

    if (new_bonus < 0 || new_bonus > 70.0) {
      ch->sendTo("Please enter a number from 0.0 to 70.0.\n\r");
      return;
    } else {
      mob->setHPLevel(new_bonus);
      mob->setHPFromHPLevel();

      // this changes exp and level
      mob->setExp(mob->determineExp());
 
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob HP Level: %.1f", mob->getHPLevel());
  ch->sendTo(VT_CURSPOS, 5, 1);
  ch->sendTo("Select a new HP Level.\n\r--> ");
}

static void change_mob_damage(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  float new_level;
  int new_precision;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    if (sscanf(arg, "%f+%d", &new_level, &new_precision) == 2) {
      if ((new_level < 0) || (new_level > 70.0)) {
        ch->sendTo("Damage Level must be between 0.0 and 70.0.\n\r");
        return;
      }
      if ((new_precision < 0) || (new_precision > 100)) {
        ch->sendTo("Damage Precision must be between 0 and 100%.\n\r");
        return;
      }
      mob->setDamLevel(new_level);
      mob->setDamPrecision((ubyte) new_precision);

      // this changes exp and level
      mob->setExp(mob->determineExp());
 
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    } else {
      ch->sendTo("Please enter Damage Level and Precision in the form X+Y. \n\r");
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob damage: %.1f+%d",
        mob->getDamLevel(),
        mob->getDamPrecision());
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("Please enter Damage Level and Precision in the form X+Y.");
  ch->sendTo(VT_CURSPOS, 5, 1);
  ch->sendTo("X = the Damage Level (0.0 to 70.0).");
  ch->sendTo(VT_CURSPOS, 6, 1);
      ch->sendTo("    Damage Level sets damage appropriate for that level of mob.");
  ch->sendTo(VT_CURSPOS, 7, 1);
      ch->sendTo("Y = the Damage Precision (0%% to 100%%).");
  ch->sendTo(VT_CURSPOS, 8, 1);
      ch->sendTo("    Damage Precision randomizes the damage.");
  ch->sendTo(VT_CURSPOS, 9, 1);
      ch->sendTo("    25%% Precision is +/-25%% the dam from the level.");
  ch->sendTo(VT_CURSPOS, 11, 1);
  ch->sendTo("Select a new damage.\n\r--> ");
}

static void change_mob_gold(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int new_gold;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_gold = atoi(arg);

    if (new_gold < 0 || new_gold > 10) {
      ch->sendTo("Please enter a number from 0 to 10.\n\r");
      return;
    } else {
      mob->moneyConst = new_gold;
      mob->calculateGoldFromConstant();
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob money constant: %d", mob->moneyConst);
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("Please use a value of 0 for animals.\n\r");
  ch->sendTo("Average people should have values from 1-5.\n\r");
  ch->sendTo("The VERY wealthy should get values from 5-8.\n\r");
  ch->sendTo("Only the VERY wealthy, living in very dangerous areas, far from town get 8+!!!!\n\r\n\r\n\r");
  ch->sendTo("Select a new amount.\n\r--> ");
}

static void change_mob_race(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int i, row, update;
  char buf[256];

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  update = atoi(arg);

  if (type != ENTER_CHECK) {
    if (update <= 0 || update >= MAX_RACIAL_TYPES)
      return;
    else {
      mob->setRace(race_t(update));
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("TBeing Race: %s", mob->getMyRace()->getSingularName().c_str());

  row = 0;
  for (i = 0; i < MAX_RACIAL_TYPES; i++) {
    sprintf(buf, VT_CURSPOS, row + 3, (((i % 3) * 25) + 5));
    if (!((i + 1) % 3))
      row++;
    ch->sendTo(buf);
    sprintf(buf, "%2d %s", i, Races[i]->getPluralName().c_str());
    ch->sendTo(buf);
    if (row > 18)
      break;    // display is overly long
  }
  ch->sendTo(VT_CURSPOS, 21, 1);
  ch->sendTo("Read HELP RACES for a complete list of races.\n\r");
  ch->sendTo("Select the number to set to, <ENTER> to return to main menu.\n\r--> ");
}

static void change_mob_def_pos(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int new_pos;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_pos = atoi(arg);

    switch (new_pos) {
      case 1:
        mob->default_pos = POSITION_STANDING;
        break;
      case 2:
        mob->default_pos = POSITION_SITTING;
        break;
      case 3:
        mob->default_pos = POSITION_SLEEPING;
        break;
      case 4:
        mob->default_pos = POSITION_RESTING;
        break;
      case 5:
        mob->default_pos = POSITION_CRAWLING;
        break;
      case 6:
        mob->default_pos = POSITION_FLYING;
        break;
      default:
        ch->sendTo("Please enter a number from 1 to 6.\n\r");
        return;
    }
    ch->specials.edit = MAIN_MENU;
    update_mob_menu(ch, mob);
    return;
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob default position: %s", position_types[mob->default_pos]);
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("1) Standing\n\r2) Sitting\n\r3) Sleeping\n\r4) Resting\n\r");
  ch->sendTo("5) Crawling\n\r6) Flying\n\r");
  ch->sendTo("\n\r\n\r");
  ch->sendTo("Select a new default position.\n\r--> ");
}

static void change_mob_sex(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int new_sex;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_sex = atoi(arg);

    if (new_sex < 0 || new_sex > 2) {
      ch->sendTo("Please enter 0, 1, or 2.\n\r");
      return;
    } else {
      mob->setSex(sexTypeT(new_sex));
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob sex: %d", mob->getSex());
  ch->sendTo(VT_CURSPOS, 4, 1);
  ch->sendTo("0 = other, 1 = male, 2 = female.\n\r\n\r");
  ch->sendTo("Select a new sex.\n\r--> ");
}

static void change_mob_max_exist(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int max_exist;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    max_exist = atoi(arg);

    if (max_exist < 0 || max_exist > 9999) {
      ch->sendTo("Please enter a number from 0 to 9999.\n\r");
      return;
    } else {
      mob->max_exist = max_exist;
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob max exist: %d\n\r\n\r", mob->max_exist);
  
  ch->sendTo("Mob max exist will limit the absolute number of mobs that can exist\n\ranywhere in the mud (rent, or loaded).\n\r");
  ch->sendTo("You can limit the number of mobs that load in a given room through the zone\n\rfile.\n\r");

  ch->sendTo(VT_CURSPOS,22, 1);
  ch->sendTo("Select a new max exist.\n\r--> ");
}

static void change_mob_immune(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  char buf[256];
  int i, j, row, immune, numx;

  if (type != ENTER_CHECK)
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  if (type != ENTER_CHECK) {
    if ((sscanf(arg, "%d %d", &immune, &numx) != 2)) {
      ch->sendTo("Syntax : <immune number> <modifier>\n\r");
      return;
    }
    immune--;
    if (immune < 0 || immune >= MAX_IMMUNES)
      return;
    
    if ((numx > 100) || (numx < -100)) {
      ch->sendTo("Please enter a number between -100 and 100.\n\r");
      return;
    }

    if (immune == IMMUNE_UNUSED2 || !*immunity_names[immune]) {
      ch->sendTo("Please don't set these until they are defined.\n\r");
      return;
    }
    mob->setImmunity(immuneTypeT(immune), numx);
  }

  ch->sendTo(VT_HOMECLR);
  row = 0;
  for (i = 0, j = 0; i < MAX_IMMUNES; i++) {
    if (!*immunity_names[i])
      continue;
    sprintf(buf, VT_CURSPOS, row + 2, (((j % 4) * 20) + 1));
    if (!((j + 1) % 4))
      row++;
    ch->sendTo(buf);
    ch->sendTo("%2d %s", i + 1, immunity_names[i]);
    j++;
  }

  ch->sendTo(VT_CURSPOS, 11, 1);
  immuneTypeT ij;
  for (ij = MIN_IMMUNE;ij < MAX_IMMUNES; ij++) {
    if (mob->getImmunity(ij) == 0)
      continue;
    ch->sendTo("%d%% %s to %s.\n\r",mob->getImmunity(ij), 
       "resistant", immunity_names[ij]);
  }
  ch->sendTo(VT_CURSPOS, 19, 1);
  ch->sendTo("Enter the number of the immunity and the amount seperated by a space.\n\r");
  ch->sendTo("To delete an apply, type the number and 0 seperated by a space.\n\r");
  ch->sendTo("For example, enter 17 33 to give a 33%% immunity to electricity.\n\r");
  ch->sendTo("Remember, certain races get automatic immunities so please don't set them here.\n\r");
  ch->sendTo("Press <ENTER> to return to main menu.\n\r--> ");
}

static void change_mob_skin(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int i, row, update;
  char buf[1024];

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    update = atoi(arg);
    update--;

    switch (ch->specials.edit) {
      case CHANGE_MOB_SKIN:
        if (update < 0 || update > 3)
          return;
        else {
          switch (update) {
            case 0:
              ch->sendTo(VT_HOMECLR);
              ch->specials.edit = CHANGE_MOB_SKIN1;
              for (i = row = 0; i < MAX_MAT_GENERAL; i++) {
                sprintf(buf, VT_CURSPOS, row + 3, ((i & 1) ? 45 : 5));
                if (i & 1)
                  row++;
                ch->sendTo(buf);
                ch->sendTo("%2d) %s", i + 1, material_nums[i].mat_name);
              }
              break;
            case 1:
              ch->sendTo(VT_HOMECLR);
              ch->specials.edit = CHANGE_MOB_SKIN2;
              for (i = row = 0; i < MAX_MAT_NATURE; i++) {
                sprintf(buf, VT_CURSPOS, row + 3, ((i & 1) ? 45 : 5));
                if (i & 1)
                  row++;
                ch->sendTo(buf);
                ch->sendTo("%2d) %s", i + 1, material_nums[i+50].mat_name);
              }
              break;
            case 2:
              ch->sendTo(VT_HOMECLR);
              ch->specials.edit = CHANGE_MOB_SKIN3;
              for (i = row = 0; i < MAX_MAT_MINERAL; i++) {
                sprintf(buf, VT_CURSPOS, row + 3, ((i & 1) ? 45 : 5));
                if (i & 1)
                  row++;
                ch->sendTo(buf);
                ch->sendTo("%2d) %s", i + 1, material_nums[i+100].mat_name);
              }
              break;
            case 3:
              ch->sendTo(VT_HOMECLR);
              ch->specials.edit = CHANGE_MOB_SKIN4;
              for (i = row = 0; i < MAX_MAT_METAL; i++) {
                sprintf(buf, VT_CURSPOS, row + 3, ((i & 1) ? 45 : 5));
                if (i & 1)
                  row++;
                ch->sendTo(buf);
                ch->sendTo("%2d) %s", i + 1, material_nums[i].mat_name);
              }
              break;
          }
          ch->sendTo(VT_CURSPOS, 20, 1);
          ch->sendTo("Enter a new material type.\n\r--> ");
          return;
        }
      case CHANGE_MOB_SKIN1:
        if (update < 0 || update >= MAX_MAT_GENERAL)
          return;
        else {
          mob->setMaterial(update);
          ch->specials.edit = MAIN_MENU;
          update_mob_menu(ch, mob);
          return;
        }
      case CHANGE_MOB_SKIN2:
        if (update < 0 || update >= MAX_MAT_NATURE )
          return;
        else {
          mob->setMaterial(update + 50);
          ch->specials.edit = MAIN_MENU;
          update_mob_menu(ch, mob);
          return;
        }
      case CHANGE_MOB_SKIN3:
        if (update < 0 || update >= MAX_MAT_MINERAL )
          return;
        else {
          mob->setMaterial(update + 100);
          ch->specials.edit = MAIN_MENU;
          update_mob_menu(ch, mob);
          return;
        }
      case CHANGE_MOB_SKIN4:
        if (update < 0 || update >= MAX_MAT_METAL)
          return;
        else {
          mob->setMaterial(update + 150);
          ch->specials.edit = MAIN_MENU;
          update_mob_menu(ch, mob);
          return;
        }
      default:
        return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current skin type : %s\n\r\n\r", material_nums[mob->getMaterial()].mat_name);
  for (i = 0; i <= 3; i++)
    ch->sendTo("%d) %s\n\r", (i + 1), material_groups[i]);

  ch->sendTo(VT_CURSPOS, 10, 1);
  ch->sendTo("Which general group do you want?\n\r--> ");
}

static void change_mob_stats(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int update;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    update = atoi(arg);

    switch (ch->specials.edit) {
      case CHANGE_MOB_STATS:
        if (update < 1 || update > 12)
          return;
        else {
          ch->sendTo(VT_HOMECLR);
          switch (update) {
            case 1:
              ch->specials.edit = CHANGE_MOB_STR;
              ch->sendTo(VT_HOMECLR);
              ch->sendTo("Current str : %d\n\rNew strength :", 
                         mob->getStat(STAT_CHOSEN, STAT_STR));
              return;
            case 2:
              ch->specials.edit = CHANGE_MOB_DEX;
              ch->sendTo(VT_HOMECLR);
              ch->sendTo("Current dex : %d\n\rNew dex :", 
                         mob->getStat(STAT_CHOSEN, STAT_DEX));
              return;
            case 3:
              ch->specials.edit = CHANGE_MOB_CON;
              ch->sendTo(VT_HOMECLR);
              ch->sendTo("Current con : %d\n\rNew con :", 
                         mob->getStat(STAT_CHOSEN, STAT_CON));
              return;
            case 4:
              ch->specials.edit = CHANGE_MOB_BRA;
              ch->sendTo(VT_HOMECLR);
              ch->sendTo("Current bra : %d\n\rNew bra :", 
                         mob->getStat(STAT_CHOSEN, STAT_BRA));
              return;
            case 5:
              ch->specials.edit = CHANGE_MOB_AGI;
              ch->sendTo(VT_HOMECLR);
              ch->sendTo("Current agi : %d\n\rNew agi :", 
                         mob->getStat(STAT_CHOSEN, STAT_AGI));
              return;
            case 6:
              ch->specials.edit = CHANGE_MOB_INT;
              ch->sendTo(VT_HOMECLR);
              ch->sendTo("Current int : %d\n\rNew intelligence :", 
                         mob->getStat(STAT_CHOSEN, STAT_INT));
              return;
            case 7:
              ch->specials.edit = CHANGE_MOB_FOC;
              ch->sendTo(VT_HOMECLR);
              ch->sendTo("Current foc : %d\n\rNew focus :", 
                         mob->getStat(STAT_CHOSEN, STAT_FOC));
              return;
            case 8:
              ch->specials.edit = CHANGE_MOB_WIS;
              ch->sendTo(VT_HOMECLR);
              ch->sendTo("Current wis : %d\n\rNew wis :", 
                         mob->getStat(STAT_CHOSEN, STAT_WIS));
              return;
            case 9:
              ch->specials.edit = CHANGE_MOB_SPE;
              ch->sendTo(VT_HOMECLR);
              ch->sendTo("Current spe : %d\n\rNew speed:", 
                         mob->getStat(STAT_CHOSEN, STAT_SPE));
              return;
            case 10:
              ch->specials.edit = CHANGE_MOB_PER;
              ch->sendTo(VT_HOMECLR);
              ch->sendTo("Current per : %d\n\rNew per:", 
                         mob->getStat(STAT_CHOSEN, STAT_PER));
              return;
            case 11:
              ch->specials.edit = CHANGE_MOB_KAR;
              ch->sendTo(VT_HOMECLR);
              ch->sendTo("Current kar : %d\n\rNew karma:", 
                         mob->getStat(STAT_CHOSEN, STAT_KAR));
              return;
            case 12:
              ch->specials.edit = CHANGE_MOB_CHA;
              ch->sendTo(VT_HOMECLR);
              ch->sendTo("Current cha : %d\n\rNew cha:", 
                         mob->getStat(STAT_CHOSEN, STAT_CHA));
              return;
          }
        }
      case CHANGE_MOB_STR:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_STR, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      case CHANGE_MOB_DEX:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_DEX, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      case CHANGE_MOB_CON:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_CON, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      case CHANGE_MOB_BRA:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_BRA, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      case CHANGE_MOB_AGI:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_AGI, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      case CHANGE_MOB_INT:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_INT, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      case CHANGE_MOB_FOC:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_FOC, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      case CHANGE_MOB_WIS:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_WIS, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      case CHANGE_MOB_SPE:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_SPE, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      case CHANGE_MOB_KAR:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_KAR, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      case CHANGE_MOB_PER:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_PER, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      case CHANGE_MOB_CHA:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_CHA, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      case CHANGE_MOB_LUC:
        if ((update < -25) || (update > 25)) {
          ch->sendTo("Please no stats over 25 or under -25.\n\r");
          return;
        } else {
          mob->setStat(STAT_CHOSEN, STAT_LUC, update);
          ch->specials.edit = CHANGE_MOB_STATS;
          change_mob_stats(ch, mob, "", ENTER_CHECK);
          return;
        }
      default:
        return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current stats :\n\r1) str : %d\n\r2) dex : %d\n\r3) con : %d\n\r4) bra : %d\n\r5) agi : %d\n\r6) int : %d\n\r7) foc : %d\n\r8) wis : %d\n\r9) spe : %d\n\r10) per : %d\n\r11) kar : %d\n\r12) chr : %d\n\r",
        mob->getStat(STAT_CHOSEN, STAT_STR),
        mob->getStat(STAT_CHOSEN, STAT_DEX),
        mob->getStat(STAT_CHOSEN, STAT_CON),
        mob->getStat(STAT_CHOSEN, STAT_BRA),
        mob->getStat(STAT_CHOSEN, STAT_AGI),
        mob->getStat(STAT_CHOSEN, STAT_INT),
        mob->getStat(STAT_CHOSEN, STAT_FOC),
        mob->getStat(STAT_CHOSEN, STAT_WIS),
        mob->getStat(STAT_CHOSEN, STAT_SPE),
        mob->getStat(STAT_CHOSEN, STAT_PER),
        mob->getStat(STAT_CHOSEN, STAT_KAR),
        mob->getStat(STAT_CHOSEN, STAT_CHA),
        mob->getStat(STAT_CHOSEN, STAT_LUC));
  ch->sendTo("\n\r");
  ch->sendTo("Physical sum (must be 0 or less) : %d\n\r",
        mob->getStat(STAT_CHOSEN, STAT_STR) +
        mob->getStat(STAT_CHOSEN, STAT_BRA) +
        mob->getStat(STAT_CHOSEN, STAT_DEX) +
        mob->getStat(STAT_CHOSEN, STAT_AGI) +
        mob->getStat(STAT_CHOSEN, STAT_CON));
  ch->sendTo("Mental sum (must be 0 or less) : %d\n\r",
        mob->getStat(STAT_CHOSEN, STAT_INT) +
        mob->getStat(STAT_CHOSEN, STAT_WIS) +
        mob->getStat(STAT_CHOSEN, STAT_FOC));
  ch->sendTo("Utility sum (must be 0 or less) : %d\n\r",
        mob->getStat(STAT_CHOSEN, STAT_SPE) +
        mob->getStat(STAT_CHOSEN, STAT_CHA) +
        mob->getStat(STAT_CHOSEN, STAT_KAR) +
        mob->getStat(STAT_CHOSEN, STAT_PER));
  ch->sendTo(VT_CURSPOS, 20, 1);
  ch->sendTo("Which stat to change?\n\r--> ");
}

static void change_mob_spec(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  char buf[256];
  int row, j, i, new_spec;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    new_spec = atoi(arg);

    if (new_spec < 0 || new_spec > NUM_MOB_SPECIALS) {
      ch->sendTo("Please enter a number from 0 to %d.\n\r", NUM_MOB_SPECIALS);
      return;
    } else if (!mob_specials[new_spec].assignable && 
	       !ch->hasWizPower(POWER_MEDIT_IMP_POWER)) {
      ch->sendTo("That spec_proc has been deemed unassignable by builders sorry.\n\r");
      return;
    } else {
      mob->spec = new_spec;
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Current mob spec: %s", (mob->spec) ? mob_specials[GET_MOB_SPE_INDEX(mob->spec)].name : "none");
  row = 0;
  for (i = 1, j=1; i <= NUM_MOB_SPECIALS; i++) {
    if (!mob_specials[i].assignable)
      continue;
    sprintf(buf, VT_CURSPOS, row + 3, ((((j - 1) % 3) * 25) + 5));
    if (!(j % 3))
      row++;
    ch->sendTo(buf);
    ch->sendTo("%2d %s", i, mob_specials[i].name);
    j++;
  }
  ch->sendTo(VT_CURSPOS, 22, 1);

  ch->sendTo("Select a new special procedure (0 = no procedure).\n\r--> ");
}

static void change_mob_sounds(TBeing *ch, TMonster *mob, const char *arg, editorEnterTypeT type)
{
  int update;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, mob);
      return;
    }
    update = atoi(arg);

    switch (update) {
      case 1:
        ch->sendTo(VT_HOMECLR);
        ch->sendTo("Current mobile room sound:\n\r");
        ch->sendTo("%s", mob->sounds);
        ch->sendTo("\n\r\n\rNew room sound:\n\r");
        ch->sendTo("Terminate with a ~ ON A NEW LINE. Press <ENTER> again to continue.\n\r");
        delete [] mob->sounds;
        mob->sounds = NULL;
        ch->desc->str = &mob->sounds;
        ch->desc->max_str = MAX_STRING_LENGTH;
        return;
      case 2:
        ch->sendTo(VT_HOMECLR);
        ch->sendTo("Current mobile distant sound:\n\r");
        ch->sendTo("%s", mob->distantSnds);
        ch->sendTo("\n\r\n\rNew distant sound:\n\r");
        ch->sendTo("Terminate with a ~ ON A NEW LINE. Press <ENTER> again to continue.\n\r");
        delete [] mob->distantSnds;
        mob->distantSnds = NULL;
        ch->desc->str = &mob->distantSnds;
        ch->desc->max_str = MAX_STRING_LENGTH;
        return;
    }
  }
  ch->sendTo(VT_HOMECLR);
  ch->sendTo("1) Sounds sent to room of mobile.\n\r");
  ch->sendTo("2) Sounds sent to adjacent rooms.\n\r");
  ch->sendTo(VT_CURSPOS, 10, 1);
  ch->sendTo("Enter your choice to modify.\n\r--> ");
}

// function for mapping what the tinyfiles save vs the actual positions
positionTypeT mapFileToPos(int pos)
{
  switch (pos) {
    case 0:
      return POSITION_DEAD;
    case 1:
      return POSITION_MORTALLYW;
    case 2:
      return POSITION_INCAP;
    case 3:
      return POSITION_STUNNED;
    case 4:
      return POSITION_SLEEPING;
    case 5:
      return POSITION_RESTING;
    case 6:
      return POSITION_SITTING;
    case 7:
      return POSITION_FIGHTING;
    case 8:
      return POSITION_CRAWLING;
    case 9:
      return POSITION_STANDING;
    case 10:
      return POSITION_MOUNTED;
    case 11:
      return POSITION_ENGAGED;
    case 12:
      return POSITION_FLYING;
    default:
      vlogf(LOG_LOW, "Undefined position (%d) in mapPosition(load)", pos);
      return POSITION_STANDING;
  }
}

int mapPosToFile(positionTypeT pos)
{
  switch (pos) {
    case POSITION_DEAD:
      return 0;
    case POSITION_MORTALLYW:
      return 1;
    case POSITION_INCAP:
      return 2;
    case POSITION_STUNNED:
      return 3;
    case POSITION_SLEEPING:
      return 4;
    case POSITION_RESTING:
      return 5;
    case POSITION_SITTING:
      return 6;
    case POSITION_FIGHTING:
      return 7;
    case POSITION_CRAWLING:
      return 8;
    case POSITION_STANDING:
      return 9;
    case POSITION_MOUNTED:
      return 10;
    case POSITION_ENGAGED:
      return 11;
    case POSITION_FLYING:
      return 12;
  }
  forceCrash("Bad pos");
  return -1;
}

int TMonster::readMobFromFile(FILE *fp, bool should_alloc)
{
  long tmp, tmp2;
  int tmp3;
  float att;
  int rc;
  char letter;
  char buf[256];

  if (should_alloc) {
    number = -1;

    name = fread_string(fp);
    shortDescr = fread_string(fp);
    player.longDescr = fread_string(fp);
    setDescr(fread_string(fp));
  } else {
    name = mob_index[number].name;
    shortDescr = mob_index[number].short_desc;
    player.longDescr = mob_index[number].long_desc;
    setDescr(mob_index[number].description);

    // We allocated the names and strings in the indeces, don't do it again!
    readStringNoAlloc(fp);
    readStringNoAlloc(fp);
    readStringNoAlloc(fp);
    readStringNoAlloc(fp);
  }

  setMult(1.0);

  fscanf(fp, "%ld ", &tmp);
  specials.act = tmp;
  if (should_alloc)
    SET_BIT(specials.act, ACT_STRINGS_CHANGED);

  fscanf(fp, " %ld ", &tmp);
  specials.affectedBy = tmp;

  if (isAffected(AFF_SANCTUARY))
    setProtection(50);

  fscanf(fp, " %ld ", &tmp);
  mud_assert(tmp >= MIN_FACTION && tmp < MAX_FACTIONS, "Bad faction value");
  setFaction(factionTypeT(tmp));

  fscanf(fp, " %f ", &att);
  setPerc((double) att);

  fscanf(fp, " %c ", &letter);

  if ((letter == 'A') || (letter == 'L')) {
    fscanf(fp, " %f ", &att);
    setMult((double) att);

    fscanf(fp, "\n");

    fscanf(fp, " %ld ", &tmp);
    setClass(tmp);
    fscanf(fp, " %ld ", &tmp);
    fixLevels(tmp);
    int lvl = tmp;

    fscanf(fp, " %ld ", &tmp);
    setHitroll(tmp);

    fscanf(fp, " %f ", &att);
    setACLevel(att);
    setACFromACLevel();

    fscanf(fp, " %f ", &att);
    setHPLevel(att);
    setHPFromHPLevel();

    bool oldStyle = false;
    rc = fscanf(fp, " %f+%d \n", &att, &tmp3);
    if (rc == 2) {
      // correct, new style
      setDamLevel(att);
      setDamPrecision(tmp3);
    } else {
      vlogf(LOG_EDIT, "Old style mob (%s).  Please fix AC/Dam/HP.", getName());
      oldStyle = true;
      setDamLevel(lvl);
      setDamPrecision(20);
      setACLevel(lvl);
      setACFromACLevel();
      setHPLevel(lvl);
      setHPFromHPLevel();

      // lets try to put filepos on correct path
      if (rc == 1) {
        // old format was %dd%d+%d, we read the first %d
        rc = fscanf(fp, "d%ld+%ld \n", &tmp, &tmp2);
        if (rc != 2)
          vlogf(LOG_EDIT, "Unable to self-correct old style mob (rc=%d)", rc);
      }
    }

    setMana(10);
    setMaxMana(10);

    setMaxMove(50 + 10*GetMaxLevel());
    setMove(moveLimit());

    fscanf(fp, " %ld ", &tmp);
    if (tmp > 10) {
      vlogf(LOG_EDIT, "Old style mob (%s) for money constant.  Please reset money.", getName());
      tmp = tmp * 10 / 4 / GetMaxLevel() / GetMaxLevel();
      tmp = min(max(1, (int) tmp), 10);
    }
    moneyConst = (ubyte) tmp;

    if (oldStyle) {
      fscanf(fp, " %ld ", &tmp);
      // use to be a expBonus, ignored now
    }
    setExp(0);

    fscanf(fp, " %ld ", &tmp);
    setRace(race_t(tmp));
    fscanf(fp, " %ld ", &tmp);
    setWeight(tmp);
    fscanf(fp, " %ld ", &tmp);
    setHeight(tmp);

    long pos = ftell(fp);
    statTypeT local_stat;
    fgets(buf, 255, fp);
    if (sscanf(buf, "%ld/%ld", &tmp, &tmp2) == 2) {
      vlogf(LOG_EDIT, "Old style mob loaded (%s).  converting characteristics",
              getName());
      for(local_stat=MIN_STAT;local_stat<MAX_STATS_USED;local_stat++) {
        setStat(STAT_CHOSEN, local_stat, 0);
      }
      max_exist = 9999;
    } else {
      fseek(fp, pos, SEEK_SET);
      for(local_stat=MIN_STAT;local_stat<MAX_STATS_USED;local_stat++) {
        fscanf(fp, " %ld ", &tmp);
        setStat(STAT_CHOSEN, local_stat, tmp);
      }
    }

    fscanf(fp, " %ld ", &tmp);
    setPosition(mapFileToPos(tmp));

    if (getPosition() == POSITION_DEAD) {
      // can happen.  no legs and trying to set resting, etc
      vlogf(LOG_LOW, "Mob (%s) put in dead position during creation.",
          getName());
    }

    fscanf(fp, " %ld ", &tmp);
    default_pos = mapFileToPos(tmp);

    fscanf(fp, " %ld ", &tmp);
    setSexUnsafe(tmp);

    fscanf(fp, " %ld ", &tmp);
    spec = tmp;

    // don't set the xp until here, since a lot of things factor in
    // gold isn't calculated until here either...
    addToExp(determineExp());

    immuneTypeT ij;
    for (ij = MIN_IMMUNE; ij< MAX_IMMUNES; ij++) {
      fscanf(fp, " %ld ", &tmp);
      setImmunity(ij, tmp);
    }

    if (fscanf(fp, " %ld ", &tmp) == 1)
      setMaterial(tmp);
    else
      setMaterial(MAT_UNDEFINED);

    if (fscanf(fp, " %ld ", &tmp) == 1)
      canBeSeen = tmp;
    else
      canBeSeen = 0;

    if (fscanf(fp, " %ld ", &tmp) == 1)
      visionBonus = tmp;
    else
      canBeSeen = 0;

    if (fscanf(fp, " %ld ", &tmp) == 1)
      max_exist = tmp;
    else
      max_exist = 0;

    if (!should_alloc) {
      rc = checkSpec(this, CMD_GENERIC_INIT, "", NULL);
      if (IS_SET_DELETE(rc, DELETE_THIS) ||
        IS_SET_DELETE(rc, DELETE_VICT)) {
        return DELETE_THIS;
      }
    }

    if (letter == 'L') {
      sounds = fread_string(fp);
      if (sounds && *sounds)
        distantSnds = fread_string(fp);
    } else {
      sounds = NULL;
      distantSnds = NULL;
    }

    extraDescription *tExDescr;
    long tCurOffSet = ftell(fp);

    // First condition should Always be true (!0).  We only do it so it's done every turn.
    while (!fscanf(fp, "E\n") && !feof(fp) && tCurOffSet != ftell(fp)) {
      tExDescr              = new extraDescription();
      tExDescr->keyword     = fread_string(fp);
      tExDescr->description = fread_string(fp);
      tExDescr->next        = ex_description;
      ex_description        = tExDescr;

      tCurOffSet = ftell(fp);
    }

    player.time.birth = time(0);
    player.time.played = 0;
    player.time.logon = time(0);

    condTypeT ic;
    for (ic = MIN_COND; ic < MAX_COND_TYPE; ++ic)
      setCond(ic, -1);

    aiMobCreation();

    // have read chosen, must set current stats now
    // curr stats are needed to properly assign discs (next step)
    // have to do this AFTER chosen read and AFTER race assigned
    statTypeT ik;
    for (ik = MIN_STAT; ik < MAX_STATS_USED; ik++)
      setStat(STAT_CURRENT, ik, getStat(STAT_NATURAL, ik));

    // assign disc before anything else
    // skills are needed by almost everything
    assignDisciplinesClass();
  } else
    forceCrash("Bogus letter on mob");

  return TRUE;
}

void TBeing::doMedit(const char *)
{
  sendTo("Mobs may not edit.\n\r");
}

// This is the main function that controls all the mobile stuff - Russ 
void TPerson::doMedit(const char *argument)
{
  const char *tString = NULL;
  int vnum, field,/* zGot,*/ oValue, Diff = 0;
  float oFValue;
  TMonster *cMob = NULL;
  string tStr,
         tStString(""),
         tStBuffer(""),
         tStArg("");
  char string[256],
       mobile[80],
       Buf[256],
       tTextLns[3][256] = {"\0", "\0", "\0"};

  if (!hasWizPower(POWER_MEDIT)) {
    sendTo("You have not been granted the power to edit mobs.\n\r");
    return;
  }

  // Sanity check.
  if (!desc)
    return;

  bisect_arg(argument, &field, string, editor_types_medit);

  switch (field) {
    case 30:
      if (!*string)
        sendTo("Syntax: med resave <mobile>\n\r");
      else if (!(cMob = dynamic_cast<TMonster *>(searchLinkedListVis(this, string, roomp->stuff))))
        sendTo("Unable to find %s...Sorry...\n\r", string);
      else if (cMob->getSnum() == cMob->mobVnum() && !hasWizPower(POWER_MEDIT_IMP_POWER))
        sendTo("Unknown value on this mobile.  resave only usable on med loaded mobiles...\n\r");
      else {
        sprintf(string, "%s %d", string, cMob->getSnum());
        msave(this, string);
      }
      return;
      break;
    case 1:        // save 
#if 1
      tStArg = string;
      tStArg = two_arg(tStArg, tStString, tStBuffer);

      if (tStString.empty() || tStBuffer.empty())
        sendTo("Syntax: med save <mobile> <vnum>\n\r");
      else {
        if (is_abbrev(tStBuffer, "resave")) {
          if (!hasWizPower(POWER_MEDIT_IMP_POWER))
            sendTo("Syntax: med save <mobile> <vnum>\n\r");
          else if (!(cMob = dynamic_cast<TMonster *>(searchLinkedListVis(this, tStString.c_str(), roomp->stuff))))
            sendTo("Unable to find %s...Sorry...\n\r", tStString.c_str());
          else if (cMob->getSnum() <= 0)
            sendTo("That mobile has a bad snum.  Sorry.  Can not resave.\n\r");
          else {
            sprintf(string, "%s %d", tStString.c_str(), cMob->getSnum());

            msave(this, string);
            doPurge(tStString.c_str());
          }
        } else
          msave(this, string);
      }

#else
      // zGot, cMob, tString are additions for Mithros for:
      //   load mob 100
      //   **modify mob_100**
      //   med save mob_100 resave
      //   for: med save mob_100 100  followed by a  purge mob_100
      // Basically for doing large db changes online.
      if ((zGot = sscanf(string, "%s %d", mobile, &vnum)) != 2) {
        if (zGot == 1) {
          cMob = dynamic_cast<TMonster *>(searchLinkedListVis(this, mobile, roomp->stuff));
          tString = one_argument(string, mobile);
          if (*tString) tString++;
        }
        if (!hasWizPower(POWER_MEDIT_IMP_POWER) || !cMob || (cMob->mobVnum() < 0) ||
            !*tString || !is_abbrev(tString, "resave")) {
          sendTo("Syntax : med save <mobile> <vnum>\n\r");
          return;
        } else
          sprintf(string, "%s %d", mobile, cMob->mobVnum());
      }
      msave(this, string);
      if (zGot == 1)
        doPurge(mobile);
      doSave(SILENT_YES);
#endif
      return;
      break;
    case 2:        // load 
      if (sscanf(string, "%d", &vnum) != 1) {
        sendTo("Syntax : med load <vnum>\n\r");
        return;
      }
      if (!hasWizPower(POWER_MEDIT_LOAD_ANYWHERE)) {
        if ((in_room == 9) || (in_room == 2) || (in_room == desc->office))
          TBeingLoad(this, vnum);
        else
          sendTo("You must be in the lab(room 9), lounge(room 2) or office(room %d) to load mobs.\n\r", desc->office);
      } else
        TBeingLoad(this, vnum);

      return;
      break;
    case 3:        // modify 
      if (sscanf(string, "%s", mobile) != 1) {
        sendTo("Syntax : med modify <mobile name>\n\r");
        return;
      }
      medit(this, string);
      return;
      break;
    case 4:        // list 
      mlist(this);
      return;
      break;
    case 5:        // remove 
      if (sscanf(string, "%d", &vnum) != 1) {
        sendTo("Syntax : med remove <vnum>\n\r");
        return;
      }
      mremove(this, vnum);
      return;
      break;
  }

  // Make sure we are a real person, bad things could happen otherwise.
  if (!desc)
    return;

  tString = string;
  half_chop(tString, mobile, string);

  if (!(cMob = dynamic_cast<TMonster *>(searchLinkedList(mobile, roomp->stuff)))) {
    sendTo("Try a mobile next time, it works better.\n\r");
    return;
  }
  // Don't want to edit a mob that has someone in it, just isn't proper.
  if (cMob->desc || cMob->isPc()) {
    sendTo("Someone is using that mob or they're a PC, Very Very bad to edit it.\n\r");
    return;
  }

  switch (field) {
    case 6: // Name
      if (!*string) {
        sendTo("You need to give me a name.\n\r");
        sendTo("Current name is: %s\n\r", cMob->name);
        return;
      }
      cMob->swapToStrung();
      if (cMob->name) {
        delete [] cMob->name;
        cMob->name = NULL;
      }
      cMob->name = mud_str_dup(string);
      return;
      break;
    case 7: // Short Description
      if (!*string) {
        sendTo("You need to give me a short description.\n\r");
        sendTo("Current Short is: %s\n\r", cMob->shortDescr);
        return;
      }
      cMob->swapToStrung();
      if (cMob->shortDescr)
        delete [] cMob->shortDescr;
      cMob->shortDescr = mud_str_dup(string);
      return;
      break;
    case 8: // Long Description
      if (!*string) {
        sendTo("You need to give me a long description.\n\r");
        sendTo("Current Long is:\n\r%s\n\r", cMob->getLongDesc());
        return;
      }
      cMob->swapToStrung();
      if (cMob->getLongDesc())
        delete [] cMob->player.longDescr;
      strcat(string, "\n\r");
      cMob->player.longDescr = mud_str_dup(string);
      return;
      break;
    case 9: // Description
      cMob->swapToStrung();
      if (cMob->descr) {
        sendTo("Current Description:\n\r%s\n\r\n\r", cMob->descr);
        delete [] cMob->descr;
        cMob->descr = NULL;
      }
      sendTo("Enter new description, terminate with '~' on a new line.\n\r");
      desc->str = &cMob->descr;
      desc->max_str = MAX_STRING_LENGTH;
      return;
      break;
    case 10: // Level
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < 1 || oValue > 100) {
        sendTo("Incorrect Level, Must be between 1 and 100.\n\r");
        return;
      }
      if (!UtilProcs(cMob->spec) && !GuildProcs(cMob->spec)) {
        sendTo("Level is automatically set for this mob.\n\r");
        return;
      }
      cMob->fixLevels(oValue);
      return;
      break;
    case 11: // Number of Attacks
      if (sscanf(string, "%f", &oFValue) != 1) {
        sendTo("Incorrect Number of attacks.\n\r");
        return;
      }
      cMob->setMult((double) oFValue);
      cMob->setExp(cMob->determineExp());
      return;
      break;
    case 12: // Hitroll
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < -30 || oValue > 200) {
        sendTo("Incorrect Hitroll, Must be between -30 and 200.\n\r");
        return;
      }
      cMob->setHitroll(oValue);
      cMob->setExp(cMob->determineExp());
      return;
      break;
    case 13: // Armor Class
      sendTo("Defunct feature.  Bug Batopr.\n\r");
#if 0
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < -1000 || oValue > 1000) {
        sendTo("Incorrect Armor Class, Must be between -1000 and 1000.\n\r");
        return;
      }
      cMob->setArmor(oValue);
      cMob->setExp(cMob->determineExp());
#endif
      return;
    case 14: // HP Bonus
      sendTo("Defunct feature.  Bug Batopr.\n\r");
#if 0
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < 0 || oValue > 5000) {
        sendTo("Incorrect HP Bonus, Must be between 0 and 5000.\n\r");
        return;
      }
      Diff = oValue - cMob->hpBonus;
      cMob->hpBonus = oValue;
      cMob->addToHit(Diff);
      cMob->setMaxHit(cMob->getHit());
      if (cMob->getHit() <= 0) {
        sendTo("ERROR!  You dropped the hit points below 1.  Setting to 1.\n\r");
        cMob->hpBonus = 0;
        cMob->setMaxHit(1);
        cMob->setHit(1);
      }
      cMob->setExp(cMob->determineExp());
#endif
      return;
    case 15: // Faction & Percent
      if (sscanf(string, "%d %d", &oValue, &Diff) != 2 ||
          oValue < MIN_FACTION || oValue >= MAX_FACTIONS ||
          Diff < 0 || Diff > 100) {
        sendTo("Incorrect Faction or Percent.\n\r");
        sendTo("Faction must be between %d and %d.\n\r", MIN_FACTION, MAX_FACTIONS-1);
        sendTo("Pecent must be between 0 and 100.\n\r");
        return;
      }
      cMob->setFaction(factionTypeT(oValue));
      cMob->setPerc((double) Diff);
      return;
      break;
    case 16: // Bare Hand Damage
      sendTo("Defunct feature.  Bug Batopr.\n\r");
#if 0
      if (sscanf(string, "%dd%d+%d", &oValue, &Diff, &zGot) != 3 ||
          oValue < 1 || Diff < 1 ||
          zGot < 0 || zGot > 50) {
        sendTo("Incorrect Bare Hand Damage.\n\r");
        sendTo("Size and Dice count must be greater than 0.\n\r");
        sendTo("Damage roll must be between 0 and 50.\n\r");
        return;
      }
      cMob->damnodice = oValue;
      cMob->damsizedice = Diff;
      cMob->setDamroll(zGot);
      cMob->setExp(cMob->determineExp());
#endif
      return;
    case 17: // Money Constant
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < 0 || oValue > 10) {
        sendTo("Incorrect Money Constant, Must be between 0 and 10.\n\r");
        sendTo("Dumb animals should be 0.\n\r");
        sendTo("Normal NPCs should be 1 - 5.\n\r");
        sendTo("Very Wealthy NPCs should be 5 - 8.\n\r");
        sendTo("Only those Incredibly wealthy, in very dangerous areas, FAR from Grimhaven.\n\r");
        sendTo("Should be 8+\n\r");
        return;
      }
      cMob->moneyConst = oValue;
      cMob->calculateGoldFromConstant();
      return;
      break;
    case 18: // Race
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < 1 || oValue >= MAX_RACIAL_TYPES) {
        sendTo("Incorrect Race, Must be between 1 and %d.\n\r", MAX_RACIAL_TYPES-1);
        sendTo("See HELP RACES for race list.\n\r");
        return;
      }
      cMob->setRace(race_t(oValue));
      sendTo("Setting Race To: %s\n\r", cMob->getMyRace()->getSingularName().c_str());
      return;
      break;
    case 19: // Sex
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < 0 || oValue > 2) {
        sendTo("Incorrect Sex, Must be between 0(other) and 2(female).\n\r");
        return;
      }
      cMob->setSex(sexTypeT(oValue));
      return;
      break;
    case 20: // Max Exist
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < 0 || oValue > 9999) {
        sendTo("Incorrect Max Exist, Must be between 0 and 9999.\n\r");
        return;
      }
      cMob->max_exist = oValue;
      return;
      break;
    case 21: // Default Position
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < 1 || oValue > 6) {
        sendTo("Incorrect Default Position, Must be between 1 and 6.\n\r");
        return;
      }
      switch (oValue) {
        case 1:
          cMob->default_pos = POSITION_STANDING;
          break;
        case 2:
          cMob->default_pos = POSITION_SITTING;
          break;
        case 3:
          cMob->default_pos = POSITION_SLEEPING;
          break;
        case 4:
          cMob->default_pos = POSITION_RESTING;
          break;
        case 5:
          cMob->default_pos = POSITION_CRAWLING;
          break;
        case 6:
          cMob->default_pos = POSITION_FLYING;
          break;
      }
      return;
      break;
    case 22: // Height
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < 1 || oValue > 10000) {
        sendTo("Incorrect Height, Must be between 1 and 10000.\n\r");
        return;
      }
      cMob->setHeight(oValue);
      return;
      break;
    case 23: // Weight
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < 1 || oValue > 10000) {
        sendTo("Incorrect Weight, Must be between 1 and 10000.\n\r");
        return;
      }
      cMob->setWeight(oValue);
      return;
      break;
    case 24: // Class
      sendTo("This part is still being worked on.\n\r");
      return;



      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < MIN_CLASS_IND || oValue >= MAX_CLASSES) {
        sendTo("Incorrect Class, Must be between %d and %d.\n\r",
               MIN_CLASS_IND, MAX_CLASSES-1);
        return;
      }
      Diff = cMob->GetMaxLevel();
      cMob->fixLevels(0);
      cMob->player.Class = (1 << oValue);
      cMob->fixLevels(Diff);
      cMob->setExp(cMob->determineExp());
      return;
      break;
    case 25: // Vision Bonus
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < -100 || oValue > 100) {
        sendTo("Incorrect Vision Bonus, Must be between -100 and 100.\n\r");
        return;
      }
      cMob->visionBonus = oValue;
      return;
      break;
    case 26: // Can Be Seen
      if (sscanf(string, "%d", &oValue) != 1 ||
          oValue < 0 || oValue > 10000) {
        sendTo("Incorrect Can Be Seen, Must be between 0 and 10000.\n\r");
        return;
      }
      cMob->canBeSeen = oValue;
      return;
      break;
    case 27: // Room Sound
      if (cMob->sounds) {
        sendTo("Current Room Sound:\n\r%s\n\r", cMob->sounds);
        delete [] cMob->sounds;
      }
      sendTo("Enter Room Sound, terminate with a '~' on a NEW line.\n\r");
      desc->str = &cMob->sounds;
      desc->max_str = MAX_STRING_LENGTH;
      return;
      break;
    case 28: // Other Room Sound
      if (cMob->distantSnds) {
        sendTo("Current Distant Sound:\n\r%s\n\r", cMob->distantSnds);
        delete [] cMob->distantSnds;
      }
      sendTo("Enter Distant Room Sound, terminate with a '~' on a NEW line.\n\r");
      desc->str = &cMob->distantSnds;
      desc->max_str = MAX_STRING_LENGTH;
      return;
      break;
    case 29: // medit replace <long/desc> <"text"> <"text">
      /*
      if (strcmp("Lapsos", getName())) {
        sendTo("Please don't use this option yet, it is still being tested.\n\r");
        return;
      }
      */

      strcpy(tTextLns[0], "[]A-Za-z0-9~`!@#$%&*()_+-={}[;\':,./<>? ]");
      sprintf(Buf, "%%s \"%%%s\" \"%%%s\"", tTextLns[0], tTextLns[0]);
      tTextLns[0][0] = '\0';

      vnum = sscanf(string, Buf, tTextLns[0], tTextLns[1], tTextLns[2]);

      if ((!is_abbrev(tTextLns[0], "long") &&
           !is_abbrev(tTextLns[0], "desc")) ||
          vnum < 2 || !tTextLns[1][0]) {
        sendTo("Syntax: medit replace <long/desc> <\"text\"> <\"text\">\n\r");
        return;
      }

      cMob->swapToStrung();

      if (is_abbrev(tTextLns[0], "long")) {
        if (!cMob->getLongDesc()) {
          sendTo("Mobile doesn't have a long description, cannot use replace.\n\r");
          return;
        }

        tStr = cMob->getLongDesc();

        if (tStr.find(tTextLns[1]) == string::npos) {
          sendTo("Couldn't find pattern in long description.\n\r");
          return;
        }

        tStr.replace(tStr.find(tTextLns[1]), strlen(tTextLns[1]), tTextLns[2]);

        delete [] cMob->player.longDescr;
        cMob->player.longDescr = mud_str_dup(tStr.c_str());
      } else {
        if (!cMob->descr) {
          sendTo("Mobile doesn't have a description, cannot use replace.\n\r");
          return;
        }

        tStr = cMob->descr;

        if (tStr.find(tTextLns[1]) == string::npos) {
          sendTo("Couldn't find pattern in description.\n\r");
          return;
        }

        tStr.replace(tStr.find(tTextLns[1]), strlen(tTextLns[1]), tTextLns[2]);

        delete [] cMob->descr;
        cMob->descr = mud_str_dup(tStr.c_str());
      }
      return;
      break;
    case 31:
      if (!*string)
        sendTo("Syntax: med average <mobile> <level> <class(optional)>\n\r");
      else
        cMob->editAverageMe(this, string);
      return;
      break;
    default:
      sendTo("Syntax : med <type> <flags>\n\r");
      break;
  }
}

const char * const tMobStringShorts[] =
  {"bamfin", "bamfout", "deathcry", "repop", "movein", "moveout"};

static void change_mob_string_values(TBeing *ch, TMonster *tMob, const char *tString, editorEnterTypeT tEnterT)
{
  int tUpdate;
  bool tHas = (tMob->ex_description ? true : false);

  if (tEnterT != ENTER_CHECK) {
    if (!*tString || (*tString == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_mob_menu(ch, tMob);
      return;
    }

    tUpdate = atoi(tString);
    tUpdate--;

    if (tUpdate < 0 || tUpdate > 5)
      return;

    ch->sendTo("Enter Message: %s\n\rEnter to complete entry\n\r` to delete entry\n\r--> ", tMobStringShorts[tUpdate]);
    ch->specials.edit = editorChangeTypeT(CHANGE_MOB_STRINGS__BAMFIN + tUpdate);

    return;
  }

  ch->sendTo(VT_HOMECLR);
  ch->sendTo("Mobile affected by flags :\n\r\n\r");

  unsigned int iter;
  for (iter = 0; iter < 6; iter++) {
    const char * tMobStringValues[] = {
      "%s1%s) bamfin [diurnal/nocturnal enter world message\n\r%s<z>\n\r\n\r",
      "%s2%s) bamfout [diurnal/nocturnal leave world message\n\r%s<z>\n\r\n\r",
      "%s3%s) death cry [blood freezes message when mob dies]\n\r%s<z>\n\r\n\r",
      "%s4%s) repop [when mob is added to the world initially]\n\r%s<z>\n\r\n\r",
      "%s5%s) movein [message when mobile enters a room]\n\r%s<z>\n\r\n\r",
      "%s6%s) moveout [message when mobile leaves a room]\n\r%s<z>\n\r\n\r"
    };

    const char *exd = NULL;
    if (tHas && tMob->ex_description) {
      exd = tMob->ex_description->findExtraDesc(tMobStringShorts[iter]);
    }
    ch->sendTo(COLOR_MOBS, tMobStringValues[iter],
             ch->cyan(), ch->norm(),
             exd ? exd : "Empty");
  }

  ch->sendTo(VT_CURSPOS, 21, 1);
  ch->sendTo("Select message type, <ENTER> to return to the main menu.\n\r--> ");
}

static void change_mob_string_enter(TBeing *ch, TMonster *tMob, const char *tString, int tType)
{
  if (tType < 0 || tType > 5 || !*tString || *tString == '\n')
    return;

  extraDescription *tExDesc,
                   *tExLast = NULL;

  string tStString(tString);

  while (tStString.find("$$") != string::npos)
    tStString.replace(tStString.find("$$"), 2, "$");

  for (tExDesc = tMob->ex_description; ; tExDesc = tExDesc->next) {
    if (!tExDesc && *tString != '`') {
      tExDesc = new extraDescription();
      tExDesc->next = tMob->ex_description;
      tMob->ex_description = tExDesc;
      tExDesc->keyword = mud_str_dup(tMobStringShorts[tType]);
      tExDesc->description = mud_str_dup(tStString.c_str());
      break;
    } else if (tExDesc && !strcmp(tExDesc->keyword, tMobStringShorts[tType]))
      if (*tString == '`') {
        if (tExLast)
          tExLast->next = tExDesc->next;
        else
          tMob->ex_description = tExDesc->next;

        delete tExDesc;
        tExDesc = NULL;
        break;
      } else {
        delete [] tExDesc->description;
        tExDesc->description = mud_str_dup(tStString.c_str());
        break;
      }

    if ((tExLast = tExDesc)) {
      vlogf(LOG_EDIT, "Fell off end of mobile string entry.");
      break;
    }
  }

  ch->specials.edit = CHANGE_MOB_STRINGS;
  change_mob_string_values(ch, tMob, "", ENTER_CHECK);
}

void mob_edit(TBeing *ch, const char *arg)
{
  if (ch->specials.edit == MAIN_MENU) {
    if (!*arg || *arg == '\n') {
      ch->desc->connected = CON_PLYNG;
      act("$n has returned from mobile editing.", TRUE, ch, 0, 0, TO_ROOM);

      if (ch->desc->mob) {
        ch->desc->mob->next = character_list;
        character_list = ch->desc->mob;
        *ch->roomp += *ch->desc->mob;
        ch->desc->mob = NULL;
      }

      // reset the terminal bars
      if (ch->vt100() || ch->ansi())
        ch->doCls(false);

      return;
    }

    switch (atoi(arg)) {
      case 0:
        update_mob_menu(ch, ch->desc->mob);
        return;
      case 1:
        ch->specials.edit = CHANGE_NAME;
        change_mob_name(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 2:
        ch->specials.edit = CHANGE_SHORT_DESC;
        change_mob_short_desc(ch, ch->desc->mob, ENTER_CHECK);
        return;
      case 3:
        ch->specials.edit = CHANGE_LONG_DESC;
        change_mob_long_desc(ch, ch->desc->mob, ENTER_CHECK);
        return;
      case 4:
        ch->specials.edit = CHANGE_DESC;
        change_mob_desc(ch, ch->desc->mob, ENTER_CHECK);
        return;
      case 5:
        ch->specials.edit = CHANGE_MOB_ACT_FLAGS;
        change_mob_act_flags(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 6:
        ch->specials.edit = CHANGE_MOB_AFF_FLAGS;
        change_mob_affect_flags(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 7:
        ch->specials.edit = CHANGE_MOB_FACTION;
        change_mob_faction_type(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 8:
        ch->specials.edit = CHANGE_MOB_MULT_ATT;
        change_mob_mult_att(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 9:
        ch->specials.edit = CHANGE_MOB_LEVEL;
        change_mob_level(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 10:
        ch->specials.edit = CHANGE_MOB_THACO;
        change_mob_thaco(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 11:
        ch->specials.edit = CHANGE_MOB_ARMOR;
        change_mob_armor(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 12:
        ch->specials.edit = CHANGE_MOB_HIT_BONUS;
        change_mob_hit_bonus(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 13:
        ch->specials.edit = CHANGE_MOB_DAMAGE;
        change_mob_damage(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 14:
        ch->specials.edit = CHANGE_MOB_GOLD;
        change_mob_gold(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 15:
        return;
      case 16:
        ch->specials.edit = CHANGE_MOB_RACE;
        change_mob_race(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 17:
        ch->specials.edit = CHANGE_MOB_SEX;
        change_mob_sex(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 18:
        ch->specials.edit = CHANGE_MOB_MAX_EXIST;
        change_mob_max_exist(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 19:
        ch->specials.edit = CHANGE_MOB_DEF_POS;
        change_mob_def_pos(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 20:
        ch->specials.edit = CHANGE_MOB_STRINGS;
        change_mob_string_values(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 21:
        ch->specials.edit = CHANGE_MOB_IMMUNE;
        change_mob_immune(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 22:
        ch->specials.edit = CHANGE_MOB_SKIN;
        change_mob_skin(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 23:
        ch->specials.edit = CHANGE_MOB_CLASS;
        change_mob_class(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 24:
        ch->specials.edit = CHANGE_MOB_STATS;
        change_mob_stats(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 25:
        ch->specials.edit = CHANGE_MOB_HEIGHT;
        change_mob_height(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 26:
        ch->specials.edit = CHANGE_MOB_WEIGHT;
        change_mob_weight(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 27:
        ch->specials.edit = CHANGE_SPEC_PROC;
        change_mob_spec(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 28:
        ch->specials.edit = CHANGE_MOB_VISION;
        change_mob_vision(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 29:
        ch->specials.edit = CHANGE_MOB_CBS;
        change_mob_cbs(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      case 30:
        ch->specials.edit = CHANGE_MOB_SOUND;
        change_mob_sounds(ch, ch->desc->mob, "", ENTER_CHECK);
        return;
      default:
        ch->specials.edit = MAIN_MENU;
        update_mob_menu(ch, ch->desc->mob);
    }
  }
  switch (ch->specials.edit) {
    case CHANGE_NAME:
      change_mob_name(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_SHORT_DESC:
      change_mob_short_desc(ch, ch->desc->mob, ENTER_REENTRANT);
      return;
    case CHANGE_LONG_DESC:
      change_mob_long_desc(ch, ch->desc->mob, ENTER_REENTRANT);
      return;
    case CHANGE_DESC:
      change_mob_desc(ch, ch->desc->mob, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_ACT_FLAGS:
      change_mob_act_flags(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_AFF_FLAGS:
      change_mob_affect_flags(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_FACTION:
      change_mob_faction_type(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_FACTION2:
      change_mob_faction_percent(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_MULT_ATT:
      change_mob_mult_att(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_LEVEL:
      change_mob_level(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_THACO:
      change_mob_thaco(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_ARMOR:
      change_mob_armor(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_HIT_BONUS:
      change_mob_hit_bonus(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_DAMAGE:
      change_mob_damage(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_GOLD:
      change_mob_gold(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_RACE:
      change_mob_race(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_SEX:
      change_mob_sex(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_DEF_POS:
      change_mob_def_pos(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_IMMUNE:
      change_mob_immune(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_CLASS:
      change_mob_class(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_SKIN:
    case CHANGE_MOB_SKIN1:
    case CHANGE_MOB_SKIN2:
    case CHANGE_MOB_SKIN3:
    case CHANGE_MOB_SKIN4:
      change_mob_skin(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_STATS:
    case CHANGE_MOB_STR:
    case CHANGE_MOB_DEX:
    case CHANGE_MOB_CON:
    case CHANGE_MOB_BRA:
    case CHANGE_MOB_AGI:
    case CHANGE_MOB_INT:
    case CHANGE_MOB_FOC:
    case CHANGE_MOB_WIS:
    case CHANGE_MOB_SPE:
    case CHANGE_MOB_PER:
    case CHANGE_MOB_KAR:
    case CHANGE_MOB_CHA:
    case CHANGE_MOB_LUC:
      change_mob_stats(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_HEIGHT:
      change_mob_height(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_WEIGHT:
      change_mob_weight(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_MAX_EXIST:
      change_mob_max_exist(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_SPEC_PROC:
      change_mob_spec(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_VISION:
      change_mob_vision(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_CBS:
      change_mob_cbs(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_SOUND:
    case CHANGE_SOUND_ROOM:
    case CHANGE_SOUND_DIST:
      change_mob_sounds(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_STRINGS:
      change_mob_string_values(ch, ch->desc->mob, arg, ENTER_REENTRANT);
      return;
    case CHANGE_MOB_STRINGS__BAMFIN:
      change_mob_string_enter(ch, ch->desc->mob, arg, 0);
      return;
    case CHANGE_MOB_STRINGS__BAMFOUT:
      change_mob_string_enter(ch, ch->desc->mob, arg, 1);
      return;
    case CHANGE_MOB_STRINGS__DEATHCRY:
      change_mob_string_enter(ch, ch->desc->mob, arg, 2);
      return;
    case CHANGE_MOB_STRINGS__REPOP:
      change_mob_string_enter(ch, ch->desc->mob, arg, 3);
      return;
    case CHANGE_MOB_STRINGS__MOVEIN:
      change_mob_string_enter(ch, ch->desc->mob, arg, 4);
      return;
    case CHANGE_MOB_STRINGS__MOVEOUT:
      change_mob_string_enter(ch, ch->desc->mob, arg, 5);
      return;
    default:
      vlogf(LOG_EDIT, "Got to a bad spot in mob_edit");
      return;
  }
}
