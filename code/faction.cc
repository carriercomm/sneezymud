//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: faction.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.2  1999/10/07 19:27:25  batopr
// Leader's name show first 20 chars now
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


/************************************************************************

      SneezyMUD - All rights reserved, SneezyMUD Coding Team
      faction.cc : Functions and routines related to factions
      modified from ideas:
      c. July 1994, Russ Russell and Wes Neal

      March 1995, Jeffrey Bennett

*************************************************************************/

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
}
#include <cmath>

#include "stdsneezy.h"

TFactionInfo FactionInfo[MAX_FACTIONS];

double avg_faction_power = 0.0;
spellNumT your_deity_val = TYPE_UNDEFINED;

int load_factions()
{
  FILE *fp;
  char buf[256];
  int num, j;
  int inum1, inum2, inum3, inum4;
  unsigned int uinum;
  long ln;
  float num1, num2;

  if (!(fp = fopen(FACTION_FILE, "r"))) {
    vlogf(10, "Couldn't open factionlist file in function load_factions()!");
    return FALSE;
  }
  for (factionTypeT i = MIN_FACTION;i < MAX_FACTIONS;i++) {
    if (fgets(buf,256,fp) == NULL) {
      vlogf(5,"ERROR: bogus line in FACTION_FILE");
      fclose(fp);
      return FALSE;
    }
    if (!strcmp(buf,"$"))   // EOF
      break;
    if (sscanf(buf,"#%d\n\r",&num) == 1) {   //   new faction
      if (fgets(buf,256,fp) == NULL) {
        vlogf(5,"ERROR: bogus line in FACTION_FILE: faction %d",num);
        fclose(fp);
        return FALSE;
      }
      if (!strcmp(buf, "")) {
        vlogf(5, "ERROR: Null faction name.");
        fclose(fp);
        return FALSE;
      }
      // strip off the trailing newline
      buf[strlen(buf) - 1] = '\0';
      FactionInfo[i].faction_name = new char[strlen(buf) + 1];
      strcpy(FactionInfo[i].faction_name,buf);

      for (j = 0; j < FACT_LEADER_SLOTS;j++) {
        if (fgets(buf,256,fp) == NULL) {
          vlogf(5,"ERROR: bogus line in FACTION_FILE: faction %d",num);
          fclose(fp);
          return FALSE;
        }
        // strip off the trailing newline
        buf[strlen(buf) - 1] = '\0';
        FactionInfo[i].leader[j] = new char[strlen(buf) + 1];
        strcpy(FactionInfo[i].leader[j],buf);
      }
      if (fgets(buf,256,fp) == NULL) {
        vlogf(5,"ERROR: bogus line in FACTION_FILE: faction %d",num);
        fclose(fp);
        return FALSE;
      }
      // strip off the trailing newline
      buf[strlen(buf) - 1] = '\0';
      FactionInfo[i].faction_password = new char[strlen(buf) + 1];
      strcpy(FactionInfo[i].faction_password,buf);

      factionTypeT ij;
      for (ij = MIN_FACTION; ij < MAX_FACTIONS; ij++) {
        if (fscanf(fp, "%f %f\n", &num1, &num2) != 2) {
          vlogf(5, "ERROR: bogus faction array faction (%d) (j=%2)",i,ij);
          fclose(fp);
          return FALSE;
        }
        FactionInfo[i].faction_array[ij][0] = (double) num1;
        FactionInfo[i].faction_array[ij][1] = (double) num2;
      }
      if (fscanf(fp, "%f\n", &num1) != 1) {
        vlogf(5, "ERROR: bogus setting of faction power for faction(%d)",i);
        fclose(fp);
        return FALSE;
      }
      FactionInfo[i].faction_power = (double) num1;
      if (fscanf(fp, "%ld %f\n", &ln, &num1) != 2) {
        vlogf(5, "ERROR: bogus setting of faction wealth/tithe for faction(%d)",i);
        fclose(fp);
        return FALSE;
      }
      FactionInfo[i].faction_wealth = ln;
      FactionInfo[i].faction_tithe = (double) num1;

      if (fscanf(fp, "%d %d %d %d\n", &inum1, &inum2, &inum3, &inum4) != 4) {
        vlogf(5, "ERROR: bogus setting of faction caravan info 1 for faction(%d)",i);
        fclose(fp);
        return FALSE;
      }
      FactionInfo[i].caravan_interval = inum1;
      FactionInfo[i].caravan_counter = inum2;
      FactionInfo[i].caravan_value = inum3;
      FactionInfo[i].caravan_defense = inum4;

      if (fscanf(fp, "%d %d %u\n", &inum1, &inum2, &uinum) != 3) {
        vlogf(5, "ERROR: bogus setting of faction caravan info 2 for faction(%d)",i);
        fclose(fp);
        return FALSE;
      }
      FactionInfo[i].caravan_attempts = inum1;
      FactionInfo[i].caravan_successes = inum2;
      FactionInfo[i].caravan_flags = uinum;
    }
  }
  fclose(fp);

  // data read was all ok, save this in case of corruption
  sprintf(buf, "cp %s %s", FACTION_FILE, FACTION_BAK);
  vsystem(buf);

  return TRUE;
}

void save_factions()
{
  FILE *fp;
  int j;

  if (!(fp = fopen(FACTION_FILE, "w+"))) {
    vlogf(10, "Couldn't open factionlist file in function load_factions()!");
    return;
  }
  for (factionTypeT i = MIN_FACTION;i < MAX_FACTIONS;i++) {
    fprintf(fp,"#%d\n",i);
    fprintf(fp,"%s\n",FactionInfo[i].faction_name);
    for (j = 0; j < FACT_LEADER_SLOTS;j++) {
      fprintf(fp,"%s\n",FactionInfo[i].leader[j]);
    }
    fprintf(fp,"%s\n",FactionInfo[i].faction_password);
    for (j = 0;j < MAX_FACTIONS;j++) {
      fprintf(fp, "%.1f %.1f\n",
            (float) FactionInfo[i].faction_array[j][0],
            (float) FactionInfo[i].faction_array[j][1]);
    }
    fprintf(fp, "%.4f\n", (float) FactionInfo[i].faction_power);
    fprintf(fp, "%ld %.4f\n", FactionInfo[i].faction_wealth, FactionInfo[i].faction_tithe);
    fprintf(fp, "%d %d %d %d\n", FactionInfo[i].caravan_interval, 
             FactionInfo[i].caravan_counter,
             FactionInfo[i].caravan_value,
             FactionInfo[i].caravan_defense);
    fprintf(fp, "%d %d %u\n", FactionInfo[i].caravan_attempts, 
             FactionInfo[i].caravan_successes,
             FactionInfo[i].caravan_flags);
  }
  fprintf(fp,"$\n");
  fclose(fp);
}


factionTypeT factionNumber(const char *name)
{
  if (is_abbrev(name,"brotherhood") || is_abbrev(name,"galek"))
    return FACT_BROTHERHOOD;
  else if (is_abbrev(name,"cult") || is_abbrev(name,"chaos") ||
            is_abbrev(name,"logrus"))
    return FACT_CULT;
  else if (is_abbrev(name,"unaffiliated") || is_abbrev(name,"none"))
    return FACT_NONE;
  else if (is_abbrev(name,"order") || is_abbrev(name,"serpents") ||
            is_abbrev(name,"snakes"))
    return FACT_SNAKE;
  else
    return FACT_UNDEFINED;
}

static const string factionLeaderTitle(factionTypeT faction, int slot)
{
  // display length for this is typically 30 chars
  char buf[64];

  mud_assert(faction >= MIN_FACTION && faction < MAX_FACTIONS,
    "factionLeaderTitle(): faction outside range %d", faction);
  mud_assert(slot >= 0 && slot < FACT_LEADER_SLOTS,
    "factionLeaderTitle(): slot outside range %d", slot);

  if (faction == FACT_CULT) {
    if (slot == 0)
      strcpy(buf, "Grand Master of the Way");
    else if (slot == 1)
      strcpy(buf, "Tharchion (Lesser Master)");
    else if (slot == 2)
      strcpy(buf, "Head of the Templar Guard (Lesser Master)");
    else if (slot == 3)
      strcpy(buf, "Keeper of the Way (Lesser Master)");
  } else if (faction == FACT_BROTHERHOOD) {
    if (slot == 0)
      strcpy(buf, "Chief Councillor of Three");
    else if (slot == 1 || slot == 2)
      strcpy(buf, "Councillor of Three");
    else if (slot == 3)
      strcpy(buf, "Master Guardian of the Steps");
  } else if (faction == FACT_SNAKE) {
    if (slot == 0)
      strcpy(buf, "Overlord of the Serpents");
    else if (slot == 1)
      strcpy(buf, "Emir of the Serpents");
    else if (slot == 2 || slot == 3)
      strcpy(buf, "Commander of the Serpents");
  } else {
    if (slot == 0)
      strcpy(buf, "Faction Leader");
    else if (slot == 1)
      strcpy(buf, "Sub-Leader 1");
    else if (slot == 2)
      strcpy(buf, "Sub-Leader 2");
    else if (slot == 3)
      strcpy(buf, "Sub-Leader 3");
  }

  return buf;
}

// for determining leadership position within the faction
// -1 if not a leader
// FALSE if a leader but lacking "power"
// TRUE if leader with "power"
int TBeing::getFactionAuthority(factionTypeT fnum, int power)
{
  if (isImmortal() && hasWizPower(POWER_WIZARD)) {
    return TRUE;
  }
  for (int i = 0;i < FACT_LEADER_SLOTS;i++) {
    if (!strcmp(getName(),FactionInfo[fnum].leader[i]))
      return (power >= i);
  }
  return -1;
}
 
void TBeing::doMakeLeader(const char *arg)
{
  char namebuf[100];
  factionTypeT fnum = getFaction();
  int which;
  charFile st;
  TBeing *vict;
  bool doNoone = FALSE; 

  arg = one_argument(arg, namebuf);
  if (!*namebuf) {
    sendTo("Whom do you wish to make a leader??\n\r");
    sendTo("Syntax: makeleader <name> <leader slot>\n\r");
    return;
  }
  if (!*arg) {
    sendTo("You need to define a leader_slot.\n\r");   
    sendTo("Syntax: makeleader <name> <leader slot>\n\r");
    return;
  }
  which = atoi(arg);
  /* Capitalize name and faction */
  cap(namebuf);

  if (!strcmp("Noone", namebuf) || !strcmp("noone", namebuf))
    doNoone = TRUE;

  if (!doNoone && !load_char(namebuf, &st)) {
    sendTo("No such person exists.\n\r");
    return;
  }

  if (getFactionAuthority(fnum,which) <= 0) {
    sendTo("You lack the authority to change that.\n\r");
    return;
  }
  if (!doNoone && st.f_type != fnum) {
    sendTo("You can't make them a leader, they aren't even a member.\n\r");
    return;
  }
  if (fnum == FACT_NONE) {
    sendTo("%s don't have or want leaders.\n\r",
         good_cap(FactionInfo[FACT_NONE].faction_name).c_str());
    return;
  }
  if (which == 0 && doNoone) {
    sendTo("You must appoint a new head leader.\n\r");
    return;
  }
  if ((which < 0) || (which >= FACT_LEADER_SLOTS)) {
    sendTo("Syntax: makeleader <name> <leader slot>\n\r");
    return;
  } else {
    vlogf(6,"Leader slot %d for faction %s changed.",which,
           FactionInfo[fnum].faction_name);
    vlogf(6,"Changed from %s to %s.",FactionInfo[fnum].leader[which],
           namebuf);
    sendTo("You have set %s's leader %d to %s.\n\r", 
           FactionInfo[fnum].faction_name, which, namebuf);

    if (strcmp(namebuf, "Noone")) {
      strcpy(FactionInfo[fnum].leader[which],namebuf);
      if ((vict = get_char(namebuf, EXACT_YES)) ||
          (vict = get_char(namebuf, EXACT_NO))) {
        vict->sendTo(COLOR_MOBS, "%s has made you leader %d of %s.\n\r",
            getName(), which, FactionInfo[fnum].faction_name);
      }
    } else
      strcpy(FactionInfo[fnum].leader[which],"");

    save_factions();
    return;
  }
}

void TBeing::doNewMember(const char *arg)
{
  TBeing *vict;
  char namebuf[100];
  factionTypeT fnum = getFaction();

  if (isUnaff()) {
    sendTo("You can't apppoint members to %s.\n\r",
               FactionInfo[FACT_NONE].faction_name);
    return;
  }
  if (getFactionAuthority(fnum,FACT_LEADER_SLOTS - 1) <= 0) {
    sendTo("You lack the authority to add members.\n\r");
    return;
  }

  arg = one_argument(arg, namebuf);
  if (!*namebuf) {
    sendTo("Whom do you wish to make a member of your faction??\n\r");
    sendTo("Syntax: newmember <name>\n\r");
    return;
  }
  if (!(vict = get_pc_world(this, namebuf, EXACT_YES))) {
    if (!(vict = get_pc_world(this, namebuf, EXACT_NO))) {
      sendTo("They don't want to be a member of a faction, they aren't here.\n\r");
      return;
    }
  }
  if (!vict->isPc()) {
    sendTo("Only PCs can be added.\n\r");
    return;
  } else if (vict->desc && !IS_SET(vict->desc->autobits, AUTO_JOIN)) {
    sendTo("That person doesn't want to join factions.\n\r");
    return;
  } else if (vict->getFaction() == fnum) {
    sendTo("That person is already a member of your faction.\n\r");
    return;
  } else if (!vict->isUnaff()) {
    sendTo("You can't add them, they are already a member somewhere else.\n\r");
    return;
  }
  vict->setFaction(fnum);

#if FACTIONS_IN_USE
  vict->setPerc(0.00);
  factionTypeT i;
  for (i = MIN_FACTION; i < MAX_FACTIONS; i++)
    vict->setPercX(0.0, i);
#endif

  vict->sendTo("You have been made a member of the %s.\n\r",
        FactionInfo[fnum].faction_name);
  sendTo(COLOR_MOBS, "You have added %s to the %s.\n\r", vict->getName(),
        FactionInfo[fnum].faction_name);
  vlogf(0, "Newmember: %s adding %s to %s.",getName(),vict->getName(),
        FactionInfo[fnum].faction_name); 
}

void TBeing::doRMember(const char *arg)
{
  TBeing *vict = NULL;
  char namebuf[128];
  factionTypeT fnum = getFaction();
  int j;
  
  arg = one_argument(arg, namebuf);
  if (!*namebuf) {
    sendTo("Whom do you wish to remove as a member?\n\r");
    sendTo("Syntax: RMember <name>\n\r");
    return;
  }
  if (getFactionAuthority(fnum,FACT_LEADER_SLOTS - 1) <= 0) {
    sendTo("You lack the authority to remove members.\n\r");
    return;
  }
  if (fnum == FACT_NONE) {
    sendTo("You can't remove someone from %s!\n\r",
           FactionInfo[FACT_NONE].faction_name);
    return;
  }
  if (!(vict = get_pc_world(this, namebuf, EXACT_YES))) {
    if (!(vict = get_pc_world(this, namebuf, EXACT_NO))) {
      sendTo("You can't remove that person, they aren't here.\n\r");
      return;
    }
  }
  if (vict == this) {
    sendTo("Use the disband command to remove yourself from a faction.\n\r");
    return;
  }
  if (vict->getFaction() != fnum) {
    sendTo("That person is not a member of that faction.\n\r");
    return;
  }
  for (j = 0;j < FACT_LEADER_SLOTS;j++) {
    if (!strcmp(FactionInfo[fnum].leader[j],vict->getName())) {
      sendTo("Sorry, that person is a leader of that faction and can't be removed.\n\r");
      sendTo("Appoint a new leader to slot %d and then you may remove them.\n\r",j);
      return;
    }
  }
  vict->setFaction(FACT_NONE);
#if FACTIONS_IN_USE
  vict->setPerc(0.00);
  factionTypeT i;
  for (i = MIN_FACTION; i < MAX_FACTIONS; i++)
    vict->setPercX(0.0, i);
#endif

  vict->sendTo("You have been kicked out of the %s.\n\r",
        FactionInfo[fnum].faction_name);
  sendTo(COLOR_MOBS, "You have removed %s from the %s.\n\r", vict->getName(),
        FactionInfo[fnum].faction_name);
  vlogf(0, "RMember: %s removing %s from %s.",getName(),vict->getName(),
        FactionInfo[fnum].faction_name);
}
  
void TBeing::doDisband()
{
  factionTypeT fnum = getFaction();

  if (fnum == FACT_NONE) {
    sendTo("You don't seem to be affiliated to any faction.\n\r");
    return;
  }

  sendTo("No disbanding is allowed at the present time.\n\r");
  return;

  int ij;
  for (ij = 0;ij < FACT_LEADER_SLOTS;ij++) {
    if (!strcmp(FactionInfo[fnum].leader[ij],getName())) {
      sendTo("What?!?  And leave them leaderless?\n\r");
      sendTo("Appoint a new leader for slot %d and then you may disband.\n\r",ij);
      return;
    }
  }
  sendTo("You have disbanded from %s.\n\r",FactionInfo[fnum].faction_name);
  vlogf(0,"Disband: %s left %s.",getName(),FactionInfo[fnum].faction_name);
  setFaction(FACT_NONE);
#if FACTIONS_IN_USE
  setPerc(0.0);
  factionTypeT i;
  for (i = MIN_FACTION; i < MAX_FACTIONS; i++)
    setPercX(0.0, i);
#endif
}

void sendToFaction(factionTypeT fnum, const char *who, const char *arg)
{
  Descriptor *d, *d_next;
  TBeing *tmpch;
  char buf[2048];

  //  sprintf(buf, "%s Faction Announcement from %s:\n\r%s\n\r",
  sprintf(buf, "<g>%s <c>%s<1>: %s\n\r",
    FactionInfo[fnum].faction_name, who, arg);

  for (d = descriptor_list; d; d = d_next) {
    d_next = d->next;
    if (d->connected)
      continue;

    tmpch = (d->original ? d->original : d->character);

    if ((tmpch->getFaction() != fnum) &&
        !tmpch->hasWizPower(POWER_SEE_FACTION_SENDS))
      continue;

    d->character->sendTo(COLOR_SHOUTS, buf);
  }
}

void TBeing::doSend(const char *arg)
{ 
  factionTypeT fnum = getFaction();
  char buf2[MAX_INPUT_LENGTH];

  // allow immortals to send to any faction via "send snake mess"
  bool wizSent = false;
  if (isImmortal()) {
    char buf[MAX_INPUT_LENGTH];
    const char *b3;
    b3 = one_argument(arg, buf);
    factionTypeT fnum2 = factionNumber(buf);
    if (fnum2 != FACT_UNDEFINED) {
      fnum = fnum2;
      wizSent = true;
      for (;b3 && *b3 && isspace(*b3); b3++);
      strcpy(buf2, b3);
    }
  }
  if (!wizSent) {
    for (;arg && *arg && isspace(*arg); arg++);
    strcpy(buf2, arg);
  }

#if 0
  if ((getFactionAuthority(fnum,FACT_LEADER_SLOTS - 1) <= 0) &&
      !isImmortal()) {
    sendTo("You lack the authority to send faction announcements.\n\r");
    return;
  }
#endif
  if (fnum == FACT_NONE) {
    sendTo("You are not a member of a faction.\n\r");
    return;
  }

  if (desc)
    desc->talkCount = time(0);

  if (applySoundproof())
    return;

  if (isDumbAnimal()) {
    sendTo("You are a dumb animal; you can't talk!\n\r");
    return;
  }
  if (isAffected(AFF_SILENT)) {
    sendTo("You can't make a sound!\n\r");
    act("$n waves $s hands and points silently toward $s mouth.", TRUE, this, 0, 0, TO_ROOM);
    return;
  }

  if (isPc() && ((desc && IS_SET(desc->autobits, AUTO_NOSHOUT)) || isPlayerAction(PLR_GODNOSHOUT))) {
    sendTo("You can't send a faction message!!\n\r");
    return;
  }
  if (isAffected(AFF_CHARM) && master) {
    if (!isPc()) {
      sendTo("What a dumb master you have, charmed mobiles can't shout.\n\r");
      master->sendTo("Stop ordering your charms to shout.  *scold*  \n\r");
    } else {
      sendTo("You're charmed, you can't shout.\n\r");
    }
    return;
  }
  if (!dynamic_cast<TMonster *>(this) && (Silence == 1) && !isImmortal()) {
    sendTo("Faction messages has been banned.\n\r");
    return;
  }
  if (master && isAffected(AFF_CHARM)) {
    master->sendTo("I don't think so :-)\n\r");
    return;
  }
  if (rider) {
    rider->sendTo("I don't think so :-)\n\r");
    return;
  }
  if ((getMove() < 15) && isPc()) {
    sendTo("You don't have the energy to send a faction message!\n\r");
    return;
  }

  if (!*buf2) {
    sendTo("What message do you wish to send?\n\r");
    sendTo("Syntax: send <message>\n\r");
    return;
  }
  if (isPc())
    addToMove(-15);

  addToWait(combatRound(0.5));


  sendToFaction(fnum, getName(), buf2);
}

void TBeing::doRelease(const string & arg)
{
  TBeing *targ;
  string buf;

  if (!hasWizPower(POWER_WIZARD)) {
    sendTo("Prototype command.  You need to be a developer to use this.\n\r");
    return;
  }

  one_argument(arg, buf);

  if (is_abbrev(buf, "all")) {
    for (targ = getCaptive(); targ; targ = getCaptive()) 
      doRelease(fname(targ->name));
    
    return;
  }
  if (!(targ = get_char_room_vis(this, buf.c_str()))) {
    sendTo("Release whom?\n\r");
    return;
  }
  if (targ->getCaptiveOf() != this) {
    sendTo("They don't seem to be a captive of yours.\n\r");
    return;
  }

  sendTo(COLOR_MOBS, "You release %s.\n\r", targ->getName());
  act("$n releases you.", TRUE, this, 0, targ, TO_VICT);
  act("$n releases $N.", TRUE, this, 0, targ, TO_NOTVICT);
  remCaptive(targ);  
  targ->stopFollower(FALSE);
  return;
}

void TBeing::doCapture(const string & arg)
{
  TBeing *targ;
  string buf;

  if (!hasWizPower(POWER_WIZARD)) {
    sendTo("Prototype command.  You need to be a developer to use this.\n\r");
    return;
  }

  one_argument(arg, buf);

  if (buf.empty()) {
    sendTo("Your captives: ");
    if (!getCaptive())
      sendTo("No one.\n\r");
    else {
      for(targ = getCaptive(); targ; targ = targ->getNextCaptive()) {
        sendTo(COLOR_MOBS, " %s",targ->getName());
        if (targ->getNextCaptive())
          sendTo(",");
      }
      sendTo("\n\r");
    }
    return;
  }
  if (!(targ = get_char_room_vis(this, buf.c_str()))) {
    sendTo("Capture whom?\n\r");
    return;
  }
  if (targ->getCaptiveOf()) {
    sendTo("They are someone else's captive already.\n\r");
    return;
  }
  if (targ->isImmortal()) {
    sendTo("I don't _THINK_ so...\n\r");
    return;
  }
  if (rider) {
    sendTo("You can't capture them while they are being ridden.\n\r");
    return;
  }
  if (targ->master) {
    targ->stopFollower(TRUE);
  }
  sendTo(COLOR_MOBS, "You capture %s.\n\r", targ->getName());
  targ->sendTo(COLOR_MOBS, "You have been captured by %s!\n\r", getName());
  act("$n captures $N!", TRUE, this, 0, targ, TO_NOTVICT);

  // Don't let captives have captives...
  while (targ->getCaptive())
    targ->doRelease(fname(targ->getCaptive()->name));

  addCaptive(targ);  

  if (getPosition() < POSITION_CRAWLING)
    targ->doStand();

  if (master)
    targ->stopFollower(TRUE);

  addFollower(targ);
  return;
}

void TBeing::doFactions(const char *arg)
{
  factionTypeT which;
  char buf[16000];
  struct stat timestat;
  char fileBuf[256], timebuf[256];

  if (!desc)
    return;

  if (!isImmortal() || !arg || !*arg)
    which = getFaction();
  else { 
    only_argument(arg, buf);
    which = factionNumber(buf);
    if (which == -1) {
      sendTo("No such faction.\n\r");
      sendTo("Syntax: list faction <faction>\n\r");
      return;
    }
  }

  sprintf(buf, "You are allied to: %s\n\r", FactionInfo[which].faction_name);  
  if (which != FACT_NONE) {
    sprintf(buf + strlen(buf), "Your faction has a potency of: %.2f, which is %s.\n\r",
      FactionInfo[which].faction_power,
      ((FactionInfo[which].faction_power == avg_faction_power) ? "average" :
       ((FactionInfo[which].faction_power > avg_faction_power) ?
          "above average" :"below average")));
    sprintf(buf + strlen(buf), "Your faction has %ld talens of wealth, and a tithe percentage of %.2f%%.\n\r",
                    FactionInfo[which].faction_wealth,
                    FactionInfo[which].faction_tithe);
  }
  if (which != FACT_NONE || isImmortal()) {
    sprintf(buf + strlen(buf),
            "\n\rOne of your faction's caravans departed %d hour%s ago bound for %s.\n\r",
            FactionInfo[which].caravan_counter / 2,
            (FactionInfo[which].caravan_counter / 2 == 1 ? "" : "s"),
            CaravanDestination(-which-1));
    sprintf(buf + strlen(buf), 
            "It was carrying %d talens of goods, and paid %d talens for defenders.\n\r",
            FactionInfo[which].caravan_value,
            FactionInfo[which].caravan_defense);
    if (FactionInfo[which].caravan_interval != -1) {
      sprintf(buf + strlen(buf), 
            "The next caravan is scheduled to leave in %d hour%s.\n\r",
            (FactionInfo[which].caravan_interval -
            FactionInfo[which].caravan_counter)/2,
            ((FactionInfo[which].caravan_interval -
            FactionInfo[which].caravan_counter)/2 == 1 ? "" : "s"));
      sprintf(buf + strlen(buf), 
            "Currently, caravan destinations are: ");
      if (IS_SET(FactionInfo[which].caravan_flags, CARAVAN_DEST_GH))
        sprintf(buf + strlen(buf), " %s", CaravanDestination(CARAVAN_DEST_GH));
      if (IS_SET(FactionInfo[which].caravan_flags, CARAVAN_DEST_BM))
        sprintf(buf + strlen(buf), " %s", CaravanDestination(CARAVAN_DEST_BM));
      if (IS_SET(FactionInfo[which].caravan_flags, CARAVAN_DEST_LOG))
        sprintf(buf + strlen(buf), " %s", CaravanDestination(CARAVAN_DEST_LOG));
      if (IS_SET(FactionInfo[which].caravan_flags, CARAVAN_DEST_AMBER))
        sprintf(buf + strlen(buf), " %s", CaravanDestination(CARAVAN_DEST_AMBER));
      if (!IS_SET(FactionInfo[which].caravan_flags, 
                CARAVAN_DEST_BM | CARAVAN_DEST_GH |
                CARAVAN_DEST_LOG | CARAVAN_DEST_AMBER))
        sprintf(buf + strlen(buf), " None");
      sprintf(buf + strlen(buf), "\n\r");
    } else {
      sprintf(buf + strlen(buf), 
            "At this time, no caravans are scheduled.\n\r");
    }
    sprintf(buf + strlen(buf), 
            "Your faction has sent %d caravan%s, and %d of them have arrived successfully.\n\r",
            FactionInfo[which].caravan_attempts,
            (FactionInfo[which].caravan_attempts == 1 ? "" : "s"),
            FactionInfo[which].caravan_successes);
  }

  sprintf(buf + strlen(buf), "\n\rContribution ratios:\n\r");
  factionTypeT i;
  for (i = MIN_FACTION;i < MAX_FACTIONS; i++) {
    sprintf(buf + strlen(buf), "     %-25.25s : helping: %4.1f     harming: %4.1f\n\r",
            FactionInfo[i].faction_name,
            FactionInfo[which].faction_array[i][OFF_HELP],
            FactionInfo[which].faction_array[i][OFF_HURT]);
  }
  if (which != FACT_NONE) {
    sprintf(buf + strlen(buf), "%-50.50s:     %-20.20s\n\r",
          factionLeaderTitle(which, 0).c_str(), FactionInfo[which].leader[0]);
    sprintf(buf + strlen(buf), "%-50.50s:        %-20.20s\n\r",
          factionLeaderTitle(which, 1).c_str(), FactionInfo[which].leader[1]);
    sprintf(buf + strlen(buf), "%-50.50s:        %-20.20s\n\r",
          factionLeaderTitle(which, 2).c_str(), FactionInfo[which].leader[2]);
    sprintf(buf + strlen(buf), "%-50.50s:        %-20.20s\n\r",
          factionLeaderTitle(which, 3).c_str(), FactionInfo[which].leader[3]);

    if (which == FACT_BROTHERHOOD)
      strcpy(fileBuf, FACT_LIST_BROTHER);
    else if (which == FACT_CULT)
      strcpy(fileBuf, FACT_LIST_CULT);
    else if (which == FACT_SNAKE)
      strcpy(fileBuf, FACT_LIST_SNAKE);

    // reveal membership only to leaders, otherwise someone can cross reference
    if ((getFactionAuthority(which,FACT_LEADER_SLOTS - 1) > 0) ||
        isImmortal()) {
      if (stat(fileBuf, &timestat)) {
        vlogf(0,"bad call to list faction function %s", fileBuf);
        return;
      }
      strcpy(timebuf, ctime(&(timestat.st_mtime)));
      timebuf[strlen(timebuf) - 1] = '\0';

      sprintf(buf + strlen(buf), "\n\rMembership as of last rollcall (%s):\n\r", timebuf);
      string str;
      file_to_string(fileBuf, str);
      strcat(buf, str.c_str());
    }
  }
  desc->page_string(buf, 0);

  return;
}

void TBeing::doAdjust(const char *arg)
{
  char tmpbuf[128];
  char tmpbuf2[128];
  factionTypeT fnum = getFaction();
  int verses = -1, hval;
  float value;
  float old_val;
  int amount;
  unsigned int uamount;

  enum adjTypeT {
       ADJ_HELP,
       ADJ_HURT,
       ADJ_TITHE,
       ADJ_NAME,
       ADJ_CARAVAN
  };

  arg = one_argument(arg, tmpbuf);
  if (is_abbrev(tmpbuf, "help")) {
    hval = ADJ_HELP;
  } else if (is_abbrev(tmpbuf, "harm")) {
    hval = ADJ_HURT;
  } else if (is_abbrev(tmpbuf, "tithe")) {
    hval = ADJ_TITHE;
  } else if (is_abbrev(tmpbuf, "name")) {
    hval = ADJ_NAME;
  } else if (is_abbrev(tmpbuf, "caravan")) {
    hval = ADJ_CARAVAN;
  } else {
    sendTo("Specify either help, harm or tithe.\n\r");
    sendTo("Syntax: adjust <\"help\" | \"harm\"> <verses faction> <value>\n\r");
    sendTo("Syntax: adjust tithe <value>\n\r");
    sendTo("Syntax: adjust name <new name>\n\r");
    sendTo("Syntax: adjust caravan ...\n\r");
    return;
  }
  if (hval == ADJ_NAME) {
    if (strcmp(getName(), "Batopr")) {
      // this works, but it screws up factionNumber() functionality.
      sendTo("Contact Batopr to change the name of the faction.\n\r");
      return; 
    }
    if ((getFactionAuthority(fnum,0) <= 0) && !isImmortal()) {
      // must be primary leader
      sendTo("You lack the authority to change the faction's name.\n\r");
      return;
    }
    char oldname[256];
    sprintf(oldname, FactionInfo[fnum].faction_name);
    for (; arg && *arg && isspace(*arg);arg++);

    if (!arg || !*arg) {
      sendTo("Please specify a new faction name.\n\r");
      return;
    }
    vlogf(5, "Faction name changed from %s to %s.\n\r",
          oldname, arg);
    sendTo("You change the name of the faction from %s to %s.\n\r",
          oldname, arg);
    sendTo("Remember to update factionNumber() in faction.cc with the new abbreviations.\n\r");
    sprintf(FactionInfo[fnum].faction_name, arg);
    save_factions();
    return;
  }
  if (hval == ADJ_CARAVAN) {
if (!isImmortal()) {
sendTo("Caravans are currently disabled.\n\r");
return;
}

    if ((getFactionAuthority(fnum,FACT_LEADER_SLOTS-1) <= 0) && !isImmortal()) {
      sendTo("You lack the authority to change caravan parameters.\n\r");
      return;
    }
    arg = one_argument(arg, tmpbuf);
    arg = one_argument(arg, tmpbuf2);
    if (is_abbrev(tmpbuf, "interval")) {
      if (!*tmpbuf2) {
        sendTo("Specify an interval.\n\r");
        sendTo("Syntax: adjust caravan interval <interval>\n\r");
        return;
      }
      amount = atoi(tmpbuf2);
      if (amount < MIN_CARAVAN_INTERVAL  && amount != -1) {
        sendTo("You can't specify an interval less than %d.\n\r", MIN_CARAVAN_INTERVAL);
        return;
      }
      FactionInfo[fnum].caravan_interval = amount;
      sendTo("You set %s's caravan interval to %d ticks.\n\r",
          FactionInfo[fnum].faction_name,
          FactionInfo[fnum].caravan_interval);
      return;
    } else if (is_abbrev(tmpbuf, "value")) {
      if (!*tmpbuf2) {
        sendTo("Specify the value the caravans should transport.\n\r");
        sendTo("Syntax: adjust caravan value <value>\n\r");
        return;
      }
      amount = atoi(tmpbuf2);
      if (amount <= 0) {
        sendTo("Caravans must transport something.\n\r");
        return;
      }
      if (amount % CARAVAN_TRADE) {
        sendTo("Caravans may only transport goods in multiples of %d.\n\r", CARAVAN_TRADE);
        return;
      }
      FactionInfo[fnum].caravan_value = amount;
      sendTo("You set %s's caravan value to %d talens.\n\r",
          FactionInfo[fnum].faction_name,
          FactionInfo[fnum].caravan_value);
       return;
    } else if (is_abbrev(tmpbuf, "defense")) {
      if (!*tmpbuf2) {
        sendTo("Specify the amount per caravan to spend on defenses.\n\r");
        sendTo("Syntax: adjust caravan defense <amount>\n\r");
        return;
      }
      amount = atoi(tmpbuf2);
      if (amount < 0) {
        sendTo("You can't spend a negative amount on defense.\n\r");
        return;
      }
      FactionInfo[fnum].caravan_defense = amount;
      sendTo("You set %s's caravan defense value to %d talens.\n\r",
          FactionInfo[fnum].faction_name,
          FactionInfo[fnum].caravan_defense);
       return;
    } else if (is_abbrev(tmpbuf, "destination")) {
      if (!*tmpbuf2) {
        sendTo("Specify the caravan destination city.\n\r");
        sendTo("Syntax: adjust caravan destination <city>\n\r");
        sendTo("Valid <cities>:\n\r");
        sendTo("Grimhaven, Brightmoon, Logrus, Amber\n\r");
        return;
      }
      strcpy(tmpbuf2, lower(tmpbuf2).c_str());
      if (is_abbrev(tmpbuf2, "grimhaven")) {
        uamount = CARAVAN_DEST_GH;
      } else if (is_abbrev(tmpbuf2, "brightmoon")) {
        uamount = CARAVAN_DEST_BM;
      } else if (is_abbrev(tmpbuf2, "logrus")) {
        uamount = CARAVAN_DEST_LOG;
      } else if (is_abbrev(tmpbuf2, "amber")) {
        uamount = CARAVAN_DEST_AMBER;
      } else {
        sendTo("Undefined city.\n\r");
        sendTo("Valid <cities>:\n\r");
        sendTo("Grimhaven, Brightmoon, Logrus, Amber\n\r");
        return;
      }
      if (fnum == FACT_NONE) {
        if (uamount == CARAVAN_DEST_GH) {
          sendTo("You are based in that city, and can't just trade with yourself.\n\r");
          sendTo("Valid <city number>:\n\r");
          sendTo("Grimhaven, Brightmoon, Logrus, Amber\n\r");
          return;
        }
      } else if (fnum == FACT_BROTHERHOOD) {
        if (uamount == CARAVAN_DEST_BM) {
          sendTo("You are based in that city, and can't just trade with yourself.\n\r");
          sendTo("Valid <cities>:\n\r");
          sendTo("Grimhaven, Brightmoon, Logrus, Amber\n\r");
          return;
        }
        if (uamount == CARAVAN_DEST_LOG) {
          sendTo("Direct trade with an enemy city is forbidden.\n\r");
          sendTo("Valid <cities>:\n\r");
          sendTo("Grimhaven, Brightmoon, Logrus, Amber\n\r");
          return;
        }
      } else if (fnum == FACT_CULT) {
        if (uamount == CARAVAN_DEST_LOG) {
          sendTo("You are based in that city, and can't just trade with yourself.\n\r");
          sendTo("Valid <cities>:\n\r");
          sendTo("Grimhaven, Brightmoon, Logrus, Amber\n\r");
          return;
        }
        if (uamount == CARAVAN_DEST_BM) {
          sendTo("Direct trade with an enemy city is forbidden.\n\r");
          sendTo("Valid <cities>:\n\r");
          sendTo("Grimhaven, Brightmoon, Logrus, Amber\n\r");
          return;
        }
      } else if (fnum == FACT_SNAKE) {
        if (uamount == CARAVAN_DEST_AMBER) {
          sendTo("You are based in that city, and can't just trade with yourself.\n\r");
          sendTo("Valid <cities>:\n\r");
          sendTo("Grimhaven, Brightmoon, Logrus, Amber\n\r");
          return;
        }
      }
      if (!IS_SET(FactionInfo[fnum].caravan_flags, uamount)) {
        SET_BIT(FactionInfo[fnum].caravan_flags, uamount);
        sendTo("You set %s's caravan destination to %s.\n\r",
            FactionInfo[fnum].faction_name,
            CaravanDestination(uamount));
      } else {
        REMOVE_BIT(FactionInfo[fnum].caravan_flags, uamount);
        sendTo("Future caravans of %s will no longer go to %s.\n\r",
            FactionInfo[fnum].faction_name,
            CaravanDestination(uamount));
      }
      return;
    } else {
      sendTo("Unknown caravan option.\n\r");
      sendTo("Syntax: adjust caravan interval <interval>\n\r");
      sendTo("Syntax: adjust caravan value <value>\n\r");
      sendTo("Syntax: adjust caravan defense <defense>\n\r");
      sendTo("Syntax: adjust caravan destination <city>\n\r");
    }
    return;
  }
  if (hval == ADJ_HELP || hval == ADJ_HURT) {
    arg = one_argument(arg, tmpbuf);
    if ((verses = factionNumber(tmpbuf)) == -1) {
      sendTo("That faction doesn't exist.\n\r");
      sendTo("Syntax: adjust <help | harm> <verses faction> <value>\n\r");
      return;
    }
  }
  if (!arg || !*arg || (sscanf(arg, " %f ", &value) != 1)) {
    sendTo("Specify a legitimate value.\n\r");
    sendTo("Syntax: adjust <\"help\" | \"harm\"> <verses faction> <value>\n\r");
    sendTo("Syntax: adjust tithe <value>\n\r");
    sendTo("Syntax: adjust name <new name>\n\r");
    return;
  }
  if (getFactionAuthority(fnum,FACT_LEADER_SLOTS - 1) <= 0 && !isImmortal()) {
    sendTo("You lack the authority to adjust values.\n\r");
    return;
  }

  if (hval == ADJ_HELP || hval == ADJ_HURT) {
#if !FACTIONS_IN_USE
    sendTo("You are not permitted to alter the help/harm values because faction percent is not in use.\n\r");
    return;
#else
    old_val = FactionInfo[fnum].faction_array[verses][(hval == ADJ_HURT ? OFF_HURT : OFF_HELP)];

    if (value < -4.0 || value > 4.0) {
      sendTo("Absolute value of <value> may not exceed 4.0\n\r");
      return;
    }
    if (((FactionInfo[fnum].faction_array[verses][(hval == ADJ_HURT ? OFF_HELP : OFF_HURT)] > 0.0) &&
        value > 0.0) ||
        ((FactionInfo[fnum].faction_array[verses][(hval == ADJ_HURT ? OFF_HELP : OFF_HURT)] < 0.0) &&
        value < 0.0)) {
      sendTo("Helping and harming can't BOTH be in same direction.\n\r");
      if (isImmortal())
        sendTo("Ignoring this error due to immortality.  Please fix this though.\n\r");
      else
        return;
    }
    if ((hval == ADJ_HURT && fabs(FactionInfo[fnum].faction_array[verses][OFF_HELP]) < fabs(value)) ||
        (hval == ADJ_HELP && fabs(FactionInfo[fnum].faction_array[verses][OFF_HURT]) > fabs(value))) {
      sendTo("The absolute value of \"helping\" must exceed \"harming\".\n\r");
      return;
    }
    if (fnum == verses) {
      if (hval == ADJ_HURT && value > -1.0) {
        sendTo("Sorry, harming your own faction must cause some detriments.\n\r");
        return;
      }
      if (hval == ADJ_HELP && value < 1.0) {
        sendTo("Sorry, helping your own faction must cause some benefit.\n\r");
        return;
      }
      if ((hval == ADJ_HURT && value > 0.0) ||
          (hval == ADJ_HELP && value < 0.0)) {
        sendTo("That would be a little self-destructive.\n\r");
        return;
      }
    }
    if (verses == FACT_NONE) {
      if (hval == ADJ_HURT && value > -1.0) {
        sendTo("You have no quarrel with %s, and can not benefit from their destruction.\n\r", FactionInfo[FACT_NONE].faction_name);
        return;
      }
    }
    if (fnum == FACT_BROTHERHOOD) {
      if (verses == FACT_CULT) {
        if (hval == ADJ_HURT && value < 2.0) {
          sendTo("You may not reduce this value below 2.0\n\r");
          return;
        }
        if (hval == ADJ_HELP && value > -2.0) {
          sendTo("You may not raise this value above -2.0\n\r");
          return;
        }
      }
    } else if (fnum == FACT_CULT) {
      if (verses == FACT_BROTHERHOOD) {
        if (hval == ADJ_HURT && value < 2.0) {
          sendTo("You may not reduce this value below 2.0\n\r");
          return;
        }
        if (hval == ADJ_HELP && value > -2.0) {
          sendTo("You may not raise this value above -2.0\n\r");
          return;
        }
      }
    } else if (fnum == FACT_SNAKE) {
      if (verses == FACT_BROTHERHOOD) {
        if ((hval == ADJ_HELP) &&
            (value + FactionInfo[fnum].faction_array[FACT_CULT][OFF_HELP] > 1.0)) {
          sendTo("At present, that value is limited to %5.2f.\n\r",
              1.0 - FactionInfo[fnum].faction_array[FACT_CULT][OFF_HELP]);
          return;
        }
        if ((hval == ADJ_HURT) &&
            (value + FactionInfo[fnum].faction_array[FACT_CULT][OFF_HURT] > 1.0)) {
          sendTo("At present, that value is limited to %5.2f.\n\r",
              1.0 - FactionInfo[fnum].faction_array[FACT_CULT][OFF_HURT]);
          return;
        }
      } else if (verses == FACT_CULT) {
        if ((hval == ADJ_HELP) &&
            (value + FactionInfo[fnum].faction_array[FACT_BROTHERHOOD][OFF_HELP] > 1.0)) {
          sendTo("At present, that value is limited to %5.2f.\n\r",
              1.0 - FactionInfo[fnum].faction_array[FACT_BROTHERHOOD][OFF_HELP]);
          return;
        }
        if ((hval == ADJ_HURT) &&
            (value + FactionInfo[fnum].faction_array[FACT_BROTHERHOOD][OFF_HURT] > 1.0)) {
          sendTo("At present, that value is limited to %5.2f.\n\r",
              1.0 - FactionInfo[fnum].faction_array[FACT_BROTHERHOOD][OFF_HURT]);
          return;
        }
      }
    }

    sendTo("You change %s's %s rating verses %s from %5.2f to %5.2f.\n\r",
          FactionInfo[fnum].faction_name,
          (hval == ADJ_HURT ? "harm" : "help"),
          FactionInfo[verses].faction_name,
          old_val, value);
    FactionInfo[fnum].faction_array[verses][(hval == ADJ_HURT ? OFF_HURT : OFF_HELP)] = value;
#endif
  } else if (hval == ADJ_TITHE) {
    old_val = FactionInfo[fnum].faction_tithe;

    if (value < 0.0 || value > 15.0) {
      sendTo("Tithe <value> must be in range 0.0 to 15.0\n\r");
      return;
    }
    sendTo("You change %s's tithe rate from %5.2f to %5.2f.\n\r",
          FactionInfo[fnum].faction_name, old_val, value);
    FactionInfo[fnum].faction_tithe = value;
  }
  
  save_factions();
  return;
}

bool TBeing::isOppositeFaction(const TBeing *v) const
{
  if ((isCult() && v->isBrother()) ||
      (isBrother() && v->isCult()))
    return TRUE;

  if ((FactionInfo[getFaction()].faction_array[v->getFaction()][OFF_HURT] > 2.0) &&
     (FactionInfo[getFaction()].faction_array[v->getFaction()][OFF_HELP] < -2.0))
    return TRUE;

  return FALSE;
}

int TBeing::doTithe()
{
  sendTo("You can only tithe at a bank.\n\r");
  return FALSE;
}

string TBeing::yourDeity(spellNumT skill, personTypeT self, const TBeing *who) const
{
  char buf[256];
  deityTypeT deity = DEITY_NONE;
  factionTypeT fnum = getFaction();

  // switch for specifc skill
  if (deity == DEITY_NONE) {
    switch (skill) {
      case SPELL_REFRESH_DEIKHAN:
      case SPELL_CURSE_DEIKHAN:
      case SPELL_REMOVE_CURSE_DEIKHAN:
      case SPELL_BLESS_DEIKHAN:
      case SPELL_CREATE_FOOD_DEIKHAN:
      case SPELL_CREATE_WATER_DEIKHAN:
      case SPELL_HEROES_FEAST_DEIKHAN:
      case SPELL_BLESS:
      case SPELL_REFRESH:
      case SPELL_SECOND_WIND:
      case SPELL_CREATE_FOOD:
      case SPELL_CREATE_WATER:
      case SPELL_HEROES_FEAST:
      case SPELL_CURSE:
      case SPELL_REMOVE_CURSE:
        if (fnum == FACT_SNAKE)
          deity = DEITY_LUNA;
        break;
      case SPELL_PORTAL:
      case SPELL_ASTRAL_WALK:
      case SPELL_SUMMON:
      case SPELL_WORD_OF_RECALL:
      case SPELL_EXPEL_DEIKHAN:
      case SPELL_EXPEL:
        if (fnum == FACT_SNAKE)
          deity = DEITY_ICON;
        break;
      case SPELL_CURE_PARALYSIS:
        if (fnum == FACT_SNAKE)
          deity = DEITY_SINSUKEY;
        break;
      case SPELL_CURE_BLINDNESS:
      case SPELL_PARALYZE:
      case SPELL_PARALYZE_LIMB:
      case SPELL_NUMB_DEIKHAN:
      case SPELL_NUMB:
      case SPELL_CURE_POISON_DEIKHAN:
      case SPELL_CURE_POISON:
        if (fnum == FACT_SNAKE)
          deity = DEITY_SASUKEY;
        break;
      default:
        break;
    }
  }

  // switch for discipline
  if (deity == DEITY_NONE) {
    switch (getDisciplineNumber(skill, FALSE)) {
      case DISC_AEGIS:
        if (fnum == FACT_NONE)
          deity = DEITY_SASUKEY;
        else if (fnum == FACT_SNAKE)
          deity = DEITY_SASUKEY;
        else if (fnum == FACT_BROTHERHOOD)
          deity = DEITY_MENANON;
        else if (fnum == FACT_CULT)
          deity = DEITY_SALUREL;
        break;
      case DISC_CURES:
        if (fnum == FACT_NONE)
          deity = DEITY_LUNA;
        else if (fnum == FACT_SNAKE)
          deity = DEITY_LUNA;
        else if (fnum == FACT_BROTHERHOOD)
          deity = DEITY_AMANA;
        else if (fnum == FACT_CULT)
          deity = DEITY_LESPRIT;
        break;
      case DISC_AFFLICTIONS:
        if (fnum == FACT_NONE)
          deity = DEITY_SINSUKEY;
        else if (fnum == FACT_SNAKE)
          deity = DEITY_SINSUKEY;
        else if (fnum == FACT_BROTHERHOOD)
          deity = DEITY_ELYON;
        else if (fnum == FACT_CULT)
          deity = DEITY_SHROUD;
        break;
      case DISC_DEIKHAN:
      case DISC_WRATH:
        if (fnum == FACT_NONE)
          deity = DEITY_ELYON;
        else if (fnum == FACT_SNAKE)
          deity = DEITY_SINSUKEY;
        else if (fnum == FACT_BROTHERHOOD)
          deity = DEITY_ELYON;
        else if (fnum == FACT_CULT)
          deity = DEITY_MALSHYRA;
        break;
      case DISC_HAND_OF_GOD:
        if (fnum == FACT_NONE)
          deity = DEITY_JEVON;
        else if (fnum == FACT_SNAKE)
          deity = DEITY_SASUKEY;
        else if (fnum == FACT_BROTHERHOOD)
          deity = DEITY_JEVON;
        else if (fnum == FACT_CULT)
          deity = DEITY_SALUREL;
        break;
      case DISC_SHAMAN:
        if (fnum == FACT_NONE)
          deity = DEITY_SHROUD;
        else if (fnum == FACT_SNAKE)
          deity = DEITY_ICON;
        else if (fnum == FACT_BROTHERHOOD)
          deity = DEITY_OMNON;
        else if (fnum == FACT_CULT)
          deity = DEITY_SHROUD;
        break;
      case DISC_THEOLOGY:
        if (fnum == FACT_NONE)
          deity = DEITY_MEZAN;
        break;
      case DISC_CLERIC:
        if (fnum == FACT_NONE)
          deity = DEITY_MEZAN;
        break;
      default:
        break;
    }
  }

  if (self == FIRST_PERSON) {
    if ((deity <= DEITY_NONE) || (deity >= MAX_DEITIES))
      sprintf(buf, "your deity");
    else
      sprintf(buf, "%s", deities[deity]);
  } else if (self == SECOND_PERSON) 
    sprintf(buf, "%s deity", hshr());
  else if (self == THIRD_PERSON) {
    if (who) 
      sprintf(buf, "%s's deity", who->pers(this));
    else
      sprintf(buf, "%s's deity", getName());
  } 
  return buf;
}

int bestFactionPower()
{
  factionTypeT i,cur_best;
  double cur_score = 0.0;

  // skip over power of unaffiliated
  for (i = cur_best = MIN_FACTION;i < MAX_FACTIONS; i++) {
    if (FactionInfo[i].faction_power > cur_score) {
      cur_best = i;
      cur_score = FactionInfo[i].faction_power;
    }
  }
  return cur_best;
}

void recalcFactionPower()
{
  factionTypeT i;

  avg_faction_power = 0;

  // skip over power of unaffiliated
  for (i = factionTypeT(FACT_NONE+1);i < MAX_FACTIONS; i++) {
    avg_faction_power += FactionInfo[i].faction_power;
  }
 
  // subtract 1 due to skipping FACT_NONE
  avg_faction_power /= (double) ( MAX_FACTIONS - 1.0);
 
  if (!avg_faction_power)
    avg_faction_power = 1.0;
}

void TPerson::reconcileHelp(TBeing *victim, double amp)
{
#if FACTIONS_IN_USE

#undef ALIGN_STUFF
  double abso;
  double perc, mob_perc;
  double value;
  double tmp;

  if (this == victim)
    return;

  if (victim && !victim->isPc() && (victim->master == this))
    return;

  perc = getPerc();
  if (victim)
    mob_perc = victim->getPerc();
  else
    mob_perc = 100.0;

//  double old_perc = getPerc();

  // KLUDGE TO get things going faster
  amp *= 17.00;

  if (amp > 0.0) {
    // helping
    if (victim)
      value = mob_perc *
        FactionInfo[getFaction()].faction_array[victim->getFaction()][OFF_HELP] / 100.0;
    else
      value = mob_perc *
        FactionInfo[getFaction()].faction_array[getFaction()][OFF_HELP] / 100.0;
  } else {
    // harming
    if (victim)
      value = -mob_perc *
        FactionInfo[getFaction()].faction_array[victim->getFaction()][OFF_HURT] / 100.0;
    else
      value = -mob_perc *
        FactionInfo[getFaction()].faction_array[getFaction()][OFF_HURT] / 100.0;
  }

  abso = value * amp;

#ifdef ALIGN_STUFF 
  vlogf(3, "align thing: value %2.6f, amp %2.6f abso  %2.6f, %10.10s (%d) vs %10.10s (%d)",
       value, amp, abso, getName(), getFaction(), victim->getName(),
       victim->getFaction());
#endif

  if (victim) {
    addToPercX(abso, victim->getFaction());
  } else
    addToPercX(abso, getFaction());

  double avg = (getPercX(FACT_NONE) + getPercX(FACT_BROTHERHOOD) + getPercX(FACT_CULT) + getPercX(FACT_SNAKE)) / 4.0;
  double sigma;

  sigma =  (getPercX(FACT_NONE) - avg) * (getPercX(FACT_NONE) - avg);
  sigma += (getPercX(FACT_BROTHERHOOD) - avg) * (getPercX(FACT_BROTHERHOOD) - avg);
  sigma += (getPercX(FACT_CULT) - avg) * (getPercX(FACT_CULT) - avg);
  sigma += (getPercX(FACT_SNAKE) - avg) * (getPercX(FACT_SNAKE) - avg);
  sigma /= 4.0;
  sigma = sqrt(sigma);

  setPerc(avg - (0.33*sigma));
#ifdef ALIGN_STUFF
  vlogf(3, "avg %2.6f    sigma %2.6f", avg, sigma);
  vlogf(3,"real %2.6f    0: %2.6f    1: %2.6f    2: %2.6f    3: %2.6f",
     getPerc(), getPercX(FACT_NONE), getPercX(FACT_BROTHERHOOD), getPercX(FACT_CULT), getPercX(FACT_SNAKE)); 
#endif

  // adjust faction pool
  // redefine abso as amount to change faction-power by
  // this effectively changes faction-power even if faction % doesn't
  abso /= 4.25;

  if (FactionInfo[getFaction()].faction_power + abso < 0.0)
    abso = -FactionInfo[getFaction()].faction_power;

  if (!isUnaff()) {
    FactionInfo[getFaction()].faction_power += abso;
    if (desc)
      desc->session.perc += abso;
    if (victim && !victim->isUnaff()) {
      // amp > 0 if helpful act, < 0 if harmful
      // helping victims faction, raise victims
      // harming victims faction, lower victims
      tmp = mob_perc * amp / 100.0;
      tmp /= 6.0;  // semi-arbitrary (2.0 causes drop)

      if (FactionInfo[victim->getFaction()].faction_power + tmp < 0.0)
        tmp = -FactionInfo[victim->getFaction()].faction_power;

      FactionInfo[victim->getFaction()].faction_power += tmp;
    }
  } else {
    // unaffiliated don't have their own power, they simply drain/add to
    // other factions.

    if (victim && !victim->isUnaff()) {
      // amp > 0 if helpful act, < 0 if harmful
      // helping victims faction, raise victims
      // harming victims faction, lower victims

      tmp = mob_perc * amp / 100.0;
//      tmp /= 12.0;  // semi-arbitrary (2.0 causes drop)

      if (FactionInfo[victim->getFaction()].faction_power + tmp < 0.0)
        tmp = -FactionInfo[victim->getFaction()].faction_power;

      FactionInfo[victim->getFaction()].faction_power += tmp;
    }
  }
#endif  // factions_in_use
}

void TPerson::reconcileHurt(TBeing *victim, double amp)
{
  reconcileHelp(victim, -amp);
}

const char * CaravanDestination(int which)
{
  int i;

  if (which < 0)
    i = FactionInfo[-which - 1].caravan_flags & 
           (CARAVAN_CUR_DEST_GH | CARAVAN_CUR_DEST_BM | 
           CARAVAN_CUR_DEST_LOG | CARAVAN_CUR_DEST_AMBER);
  else
    i = which;

  switch (i) {
    case 0:
      return "Nowhere";
    case CARAVAN_DEST_GH:
    case CARAVAN_CUR_DEST_GH:
      return "Grimhaven";
    case CARAVAN_DEST_BM:
    case CARAVAN_CUR_DEST_BM:
      return "Brightmoon";
    case CARAVAN_DEST_LOG:
    case CARAVAN_CUR_DEST_LOG:
      return "Logrus";
    case CARAVAN_DEST_AMBER:
    case CARAVAN_CUR_DEST_AMBER:
      return "Amber";
    default:
      vlogf(9, "CaravanDestination had an i of %d (which %d)", i, which);
      forceCrash("bad carvan");
      return "unknown";
  }
}

void launch_caravans()
{
  factionTypeT i;
  TMonster *mob;

  // don't launch caravans if in shutdown mode
  if (timeTill)
    return;

  for (i = MIN_FACTION; i < MAX_FACTIONS;i++) {
    if (!FactionInfo[i].caravan_value) // no gold on board, why go?
      continue;
    FactionInfo[i].caravan_counter++;
    if ((FactionInfo[i].caravan_interval == -1) ||
        (!IS_SET(FactionInfo[i].caravan_flags, 
                CARAVAN_DEST_BM | CARAVAN_DEST_GH |
                CARAVAN_DEST_LOG | CARAVAN_DEST_AMBER))) {
      // no caravans will be launched
      // don't let caravan interval grow out of bounds
      FactionInfo[i].caravan_counter = min(FactionInfo[i].caravan_counter, 
                                             MIN_CARAVAN_INTERVAL);
      continue;
    }
    if (FactionInfo[i].caravan_counter < FactionInfo[i].caravan_interval) {
      // not time yet
      continue;
    }
    // launch a caravan
    FactionInfo[i].caravan_counter = 0;
    if (!(mob = read_mobile(MOB_CARAVAN_MASTER, VIRTUAL))) {
      vlogf(10, "No caravan master read in load_caravan");
      return;
    }
    mob->setFaction(i);
    switch (i) {
      case FACT_NONE:
        thing_to_room(mob, CAR_GH_HOME);
        break;
      case FACT_BROTHERHOOD:
        thing_to_room(mob, CAR_BM_HOME);
        break;
      case FACT_CULT:
        thing_to_room(mob, CAR_LOG_HOME);
        break;
      case FACT_SNAKE:
        thing_to_room(mob, CAR_AMBER_HOME);
        break;
      case MAX_FACTIONS:
      case FACT_UNDEFINED:
        forceCrash("Bad faction (%d) in LaunchCaravans", i);
        break;
    }
  }
  return;
}

void TBeing::deityIgnore(silentTypeT silent_caster) const
{
  int num = ::number(0,2);
  switch (num) {
    case 0:
    default:
      if (!silent_caster)
        act("$d ignores you.", FALSE, this, NULL, NULL, TO_CHAR, ANSI_RED);
      act("$n's request is ignored by $d.", TRUE, this, NULL, NULL, TO_ROOM);
      break;
    case 1:
      if (!silent_caster)
        act("$d fails to come to your aid.", FALSE, this, NULL, NULL, TO_CHAR, ANSI_RED);

      act("$n is not aided by $d.", TRUE, this, NULL, NULL, TO_ROOM);
      break;
    case 2:
      if (!silent_caster)
        act("$d does not answer your prayer.", FALSE, this, NULL, NULL, TO_CHAR, ANSI_RED);
      act("$n's prayer is not answered by $d.", TRUE, this, NULL, NULL, TO_ROOM);
      break;
  }
}

// a generic modifier for prayer effects
float TBeing::percModifier() const
{
#if FACTIONS_IN_USE
  return (getPerc() + 100.0) / 200.0;
#else
  return 0.75;
#endif
}
