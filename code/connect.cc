//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: connect.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.3  1999/10/15 21:55:26  batopr
// typo fix
//
// Revision 1.2  1999/10/15 21:53:22  batopr
// Added code to protect against use of potentially freed memory following
// call to parseCommand.
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#include <csignal>
#include <cstdarg>
#include <iostream.h>
#include <fstream.h>

extern "C" {
#include <dirent.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <crypt.h>
#include <sys/syscall.h>
#include <sys/param.h>
#include <arpa/telnet.h>
#include <arpa/inet.h>

#if defined(LINUX) || defined(SOLARIS)
#include <sys/stat.h>
#endif
}

#include "stdsneezy.h"
#include "account.h"
#include "statistics.h"
#include "socket.h"
#include "mail.h"
#include "games.h"

const int DONT_SEND = -1;
const int FORCE_LOW_INVSTE = 1;

static const char * const WIZLOCK_PASSWORD           = "motelvi";
const char * const MUD_NAME      = "Grimhaven";
const char * const MUD_NAME_VERS = "Grimhaven v5.1";
static const char * const WELC_MESSG = "\n\rWelcome to Grimhaven 5.1! May your journeys be interesting!\n\r\n\r";

static const char * const TER_HUMAN_HELP = "help/territory help human";
static const char * const TER_ELF_HELP = "help/territory help elf";
static const char * const TER_DWARF_HELP = "help/territory help dwarf";
static const char * const TER_GNOME_HELP = "help/territory help gnome";
static const char * const TER_OGRE_HELP = "help/territory help ogre";
static const char * const TER_HOBBIT_HELP = "help/territory help hobbit";
static const char * const SIGN_MESS = "/mud/sign/currentMess";

Descriptor::Descriptor() :
  output(true),
  input(false)
{
  // this guy is private to prevent being called
  // just need to init member vars that are appropriate
}

Descriptor::Descriptor(TSocket *s) :
  socket(s),
  edit(),
  connected(CON_NME),
  wait(1),
  showstr_head(NULL),
  tot_pages(0),
  cur_page(0),
  str(NULL),
  max_str(0),
  prompt_mode(0),
  output(false),
  input(true),
  session(),
  career(),
  autobits(0),
  best_rent_credit(0),
  character(NULL),
  account(NULL),
  original(NULL),
  snoop(),
  next(descriptor_list),
  pagedfile(NULL),
  obj(NULL),
  mob(NULL),
  bet(),
  bet_opt(),
  screen_size(24),
  point_roll(0),
  talkCount(time(0)),
  client(FALSE),
  bad_login(0),
  severity(0),
  last(),
  poof(),
  deckSize(0),
  prompt_d(),
  plr_act(0),
  plr_color(0),
  plr_colorSub(COLOR_SUB_NONE),
  plr_colorOff(0)
{
  int i;

  *raw = '\0';
  *delname = '\0';

  for (i = 0; i < 10; i++)
    *history[i] = '\0';
  
  descriptor_list = this;
}

Descriptor::Descriptor(const Descriptor &a) :
  socket(a.socket),
  edit(a.edit),
  connected(a.connected),
  wait(a.wait),
  tot_pages(a.tot_pages),
  cur_page(a.cur_page),
  max_str(a.max_str),
  prompt_mode(a.prompt_mode),
  output(a.output),
  input(a.input),
  session(a.session),
  career(a.career),
  autobits(a.autobits),
  best_rent_credit(a.best_rent_credit),
  character(a.character),
  account(a.account),
  original(a.original),
  snoop(a.snoop),
  next(descriptor_list),
  obj(a.obj),
  mob(a.mob),
  bet(a.bet),
  bet_opt(a.bet_opt),
  screen_size(a.screen_size),
  point_roll(a.point_roll),
  talkCount(a.talkCount),
  client(a.client),
  bad_login(a.bad_login),
  severity(a.severity),
  last(a.last),
  poof(a.poof),
  deckSize(a.deckSize),
  prompt_d(a.prompt_d),
  plr_act(a.plr_act),
  plr_color(a.plr_color),
  plr_colorSub(a.plr_colorSub),
  plr_colorOff(a.plr_colorOff)
{
  int i;

  // not sure how this is being used, theoretically correct, but watch
  // for duplication stuff
  // str may also be prolematic
  vlogf(10, "Inform Batopr immediately that Descriptor copy constructor was called.");

  showstr_head = mud_str_dup(a.showstr_head);
//  str = mud_str_dup(a.str);
  str = NULL;
  pagedfile = mud_str_dup(a.pagedfile);

  strcpy(raw, a.raw);
  strcpy(delname, a.delname);

  for (i = 0; i < 10; i++)
    strcpy(history[i], a.history[i]);
  
  descriptor_list = this;
}

Descriptor & Descriptor::operator=(const Descriptor &a) 
{
  if (this == &a) return *this;

  // not sure how this is being used, theoretically correct, but watch
  // for duplication stuff
  // str is most likely also screwy
  vlogf(10, "Inform Batopr immediately that Descriptor operator= was called.");

  socket = a.socket;
  edit = a.edit;
  connected = a.connected;
  wait = a.wait;
  tot_pages = a.tot_pages;
  cur_page = a.cur_page;
  max_str = a.max_str;
  prompt_mode = a.prompt_mode;
  output = a.output;
  input = a.input;
  session = a.session;
  career = a.career;
  autobits = a.autobits;
  best_rent_credit = a.best_rent_credit;
  character = a.character;
  account = a.account;
  original = a.original;
  snoop = a.snoop;
  obj = a.obj;
  mob = a.mob;
  bet = a.bet;
  bet_opt = a.bet_opt;
  screen_size = a.screen_size;
  point_roll = a.point_roll;
  talkCount = a.talkCount;
  client = a.client;
  bad_login = a.bad_login;
  severity = a.severity;
  last = a.last;
  poof = a.poof;
  deckSize = a.deckSize;
  prompt_d = a.prompt_d;
  plr_act = a.plr_act;
  plr_color = a.plr_color;
  plr_colorSub = a.plr_colorSub;
  plr_colorOff = a.plr_colorOff;

  delete [] showstr_head;
  showstr_head = mud_str_dup(a.showstr_head);

//  delete [] str;
//  str = mud_str_dup(a.str);
  str = NULL;

  delete [] pagedfile;
  pagedfile = mud_str_dup(a.pagedfile);

  strcpy(raw, a.raw);
  strcpy(delname, a.delname);

  for (int i = 0; i < 10; i++)
    strcpy(history[i], a.history[i]);
  
  return *this;
}

// returns TRUE if multiplay is detected
bool Descriptor::checkForMultiplay()
{
#if CHECK_MULTIPLAY
  TBeing *ch;
  unsigned int total = 1;
  Descriptor *d;

  if (!character || !account || !character->name)
    return FALSE;

  if (gamePort == ALPHA_GAMEPORT)
    return FALSE;

  if (character->hasWizPower(POWER_MULTIPLAY))
    return FALSE;

  // determine player load
  unsigned int tot_descs = 0;
  for (d = descriptor_list; d; d = d->next)
    tot_descs++;

  // established maximums based on player load thresholds
  unsigned int max_multiplay_chars;
  if (tot_descs < 15)
    max_multiplay_chars = 1;
  else if (tot_descs < 30)
    max_multiplay_chars = 1;
  else if (tot_descs < 60)
    max_multiplay_chars = 1;
  else
    max_multiplay_chars = 1;

  // for first 30 mins after a reboot, limit to 1 multiplay
  // this prevents a "race to login" from occurring.
  time_t diff = time(0) - Uptime;
  if (diff < (30 * SECS_PER_REAL_MIN))
    max_multiplay_chars = 1;

  for (d = descriptor_list; d; d = d->next) {
    if (d == this)
      continue;
    if (!(ch = d->character) || !ch->name)
      continue;

    if (ch->hasWizPower(POWER_MULTIPLAY))
      continue;

    // reconnect while still connected triggers otherwise
    if (!strcmp(character->name, ch->name))
      continue;

    if (!strcmp(d->account->name, account->name)) {
      total += 1;
      if (total > max_multiplay_chars) {
        vlogf(8, "MULTIPLAY: %s and %s from same account[%s]",
              character->name, ch->name, account->name);
#if FORCE_MULTIPLAY_COMPLIANCE
        character->sendTo("Player Load: %d, Current MultiPlay Limit: %d\n\r",
             tot_descs, max_multiplay_chars);
        if (diff < (30 * SECS_PER_REAL_MIN))
          character->sendTo("No MultiPlay allowed during first 30 mins after reboot.  Please wait %d mins.\n\r", diff/SECS_PER_REAL_MIN + 1);

        character->sendTo("Adding this character would cause you to be in violation of multiplay rules.\n\r");
        character->sendTo("Access denied.  Please log one (or more) of your other characters off and then\n\r");
        character->sendTo("try again.\n\r");
        outputProcessing();  // gotta write this to them, before we sever  :)
#endif
        return TRUE;
      }
    }

#if 0
    // beyond here, we start checking for cheaters using multiple accounts
    // since this logic is imprecise, we should first slip around any known
    // accounts that tend to trigger this.
    FILE *fp;
    fp = fopen("allowed_multiplay", "r");
    if (fp) {
      char acc1[256], acc2[256];
      bool allowed = false;
      while (fscanf(fp, "%s %s", acc1, acc2) == 2) {
        if (!strcmp(account->name, acc1) && !strcmp(d->account->name, acc2))
          allowed = true;
        if (!strcmp(account->name, acc2) && !strcmp(d->account->name, acc1))
          allowed = true;
      }
      fclose(fp);
      if (allowed)
        continue;
    }

    if (max_multiplay_chars == 1) {
      // some diabolical logic to catch multiplay with separate accounts
      // check to see if they are grouped, but haven't spoken recently
      if (character->inGroup(ch)) {
        time_t now = time(0);
        const int trigger_minutes = 1;
        if (((now - talkCount) > ((trigger_minutes + character->getTimer()) * SECS_PER_REAL_MIN)) &&
            ((now - ch->desc->talkCount) > ((trigger_minutes + ch->getTimer()) * SECS_PER_REAL_MIN))) {
          vlogf(7, "MULTIPLAY: Players %s and %s are possibly multiplaying.", character->getName(), ch->getName());

          time_t ct = time(0);
          struct tm * lt = localtime(&ct);
          char *tmstr = asctime(lt);
          *(tmstr + strlen(tmstr) - 1) = '\0';

          string tmpstr;
          tmpstr = "***** Auto-Comment on ";
          tmpstr += tmstr;
          tmpstr += ":\nPlayer ";
          tmpstr += character->getName();
          tmpstr += " was potentially multiplaying with ";
          tmpstr += ch->getName();
          tmpstr += " from account '";
          tmpstr += d->account->name;
          tmpstr += "'.\n";

          string cmd_buf;
          cmd_buf = "account/";
          cmd_buf += LOWER(account->name[0]);
          cmd_buf += "/";
          cmd_buf += lower(account->name);
          cmd_buf += "/comment";

          FILE *fp;
          if (!(fp = fopen(cmd_buf.c_str(), "a+"))) {
            perror("doComment");
            vlogf(9, "Could not open the comment-file (%s).", cmd_buf.c_str());
          } else {
            fputs(tmpstr.c_str(), fp);
            fclose(fp);
          }
        }
      }
    } // CHAR_LIMIT = 1
#endif
  }

  if (character && account && account->name && !character->hasWizPower(POWER_MULTIPLAY)) {
    TBeing *tChar = NULL,
           *oChar = NULL;
    char tAccount[256];
    FILE *tFile = NULL;

    for (tChar = character_list; tChar;) {
      oChar = tChar->next;

      if (tChar != character && tChar->isLinkdead()) {
        sprintf(tAccount, "account/%c/%s/%s",
                LOWER(account->name[0]),
                lower(account->name).c_str(),
                lower(tChar->getNameNOC(character).c_str()).c_str());

        if (tChar->isPc() && (tFile = fopen(tAccount, "r"))) {
          if (tChar->hasWizPower(POWER_MULTIPLAY))
            return FALSE;

          fclose(tFile);
#if 1
          character->sendTo("You have at least one linkdead player online.\n\r");
          character->sendTo("Adding this character would cause you to be in violation of multiplay rules.\n\r");
          character->sendTo("Access denied.  Please log one (or more) of your other characters off and then\n\r");
          character->sendTo("try again.\n\r");
          outputProcessing();  // gotta write this to them, before we sever  :)
          return TRUE;
#else
          nukeLdead(tChar);
          delete tChar;
          tChar = NULL;
#endif
        }
      }

      if (!(tChar = oChar))
        return FALSE;
    }
  }
#endif

  return FALSE;
}

int Descriptor::outputProcessing()
{
  // seems silly, but we sometimes do descriptor_list->outputProcessing()
  // to send everyone their output.  We need to check for the no-one-connected
  // state just for sanity.
  if (!this)
    return 1;

  char i[MAX_STRING_LENGTH + MAX_STRING_LENGTH];
  int counter = 0;
  char buf[MAX_STRING_LENGTH + MAX_STRING_LENGTH];

  if (!prompt_mode && !connected && !client)
    if (socket->writeToSocket("\n\r") < 0)
      return -1;

  if (output.getEnd() && !output.getBegin()) {
    if (character && character->name)
      vlogf(5, "%s's output has end and no begin (client: %d).", character->getName(), client ? 1 : 0);
    else
      vlogf(5, "output has end and no begin.");
// kludge, seems like it may lead to memory leaks but better than
// leaving an end in here cos -4/2/98
    output.setEnd(NULL);
  }
  
  memset(i, '\0', sizeof(i));
  // Take everything from queued output
  while (output.takeFromQ(i)) {
    counter++;

    // I bumped this from 500 to 1000 - Batopr
    // It happens sporadically if a lagged person is dragged on a long
    // speed walk
    if (counter >= 5000) {
      char buf2[MAX_STRING_LENGTH + MAX_STRING_LENGTH];
      strcpy(buf2, i);
      vlogf(5, "Tell a coder, bad loop in outputProcessing, please investigate %s", character ? character->getName() : "'No char for desc'");
      vlogf(5, "i = %s, last i= %s", buf2, buf); 
      // Set everything to NULL, might lose memory but we dont wanna try
      // a delete cause it might crash/ - Russ
      output.setBegin(NULL);
      output.setEnd(NULL);
      break;
    } 
    // recall that in inputProcessing we have mangaled any '$' character into
    // '$$' to avoid confusion problems in act/sendTo/etc.
    // it's now time to undo this
    // the tb stuff is so if they type '$$', it becomes '$$$$' in input,
    // and we want to return this to '$$' for output
    char *tb = i;
    char *tc;
    while ((tc = strstr(tb, "$$"))) {
      // note we are shrinking i by 1 char
      // in essence, replace "$$" with a "$"
      char *tmp = mud_str_dup(i);
      int amt = tc - i;
      strcpy(tc, tmp+amt+1);
      delete [] tmp;
      tb = tc+1;
    }
    strcpy(buf, i);
    if (snoop.snoop_by && snoop.snoop_by->desc) {
      (&snoop.snoop_by->desc->output)->putInQ("% ");
      (&snoop.snoop_by->desc->output)->putInQ(i);
    }
    if (socket->writeToSocket(i))
      return -1;
    memset(i, '\0', sizeof(i));
  }
  return (1);
}

Descriptor::~Descriptor()
{
  Descriptor *tmp;
  int num = 0;
  TThing *th, *th2;
  TRoom *rp;

  if (close(socket->sock))
    vlogf(10, "Close() exited with errno (%d) return value in ~Descriptor", errno);
  
  // clear out input/output buffers
  flush();

  // This is a semi-kludge to fix some extra crap we had being sent
  // upon reconnect - Russ 6/15/96
  if (socket->sock == maxdesc) 
    --maxdesc;

  // clear up any editing strings
  cleanUpStr();

  // Forget snoopers
  if (snoop.snooping)
    snoop.snooping->desc->snoop.snoop_by = 0;

  if (snoop.snoop_by && snoop.snoop_by->desc) {
    snoop.snoop_by->sendTo("Your victim is no longer among us.\n\r");
    snoop.snoop_by->desc->snoop.snooping = 0;
  }
  if (character) {
    if (original)
      character->doReturn("", WEAR_NOWHERE, CMD_RETURN);
    if ((connected >= CON_REDITING) || !connected) {
      if ((connected == CON_OEDITING) && obj) {
        delete obj;
        obj = NULL;
      }
      if ((connected == CON_MEDITING) && mob) {
        extract_edit_char(mob);
        mob = NULL;
      }
      if ((connected == CON_SEDITING) && mob) {
        extract_edit_char(mob);
        mob = NULL;
      }
      if (connected == CON_REDITING) 
        character->roomp->removeRoomFlagBit(ROOM_BEING_EDITTED);
      if ((character->checkBlackjack()) &&
          (gBj.index(character) >= 0)) {
        gBj.exitGame(character);
      }

      act("$n has lost $s link.", TRUE, character, 0, 0, TO_ROOM);
      vlogf(0, "Closing link to: %s.", character->getName());

      // this is partly a penalty for losing link (lose followers)
      // the more practical reason is that the mob and items are saved
      // if the player linkdead and someone comes in and purges the
      // mob, the numbers on the mob's item get messed up
      // this isn't necessarily limited to purge.  The basic problem is
      // the item is in two places (in the mob's rent, and on the mob still
      // in game).
      // The solution seems to remove one of them, and this was just easier.
      // plus it's an incentive not to linkdrop.
      // in any event, if they reconnect, the mob is still there following
      // the first save will recreate the followers...
      character->removeFollowers();

      for (th = character->stuff; th; th = th->nextThing) {
        if (th) {
          for (th2 = th->stuff; th2; th2 = th2->nextThing)
            num++;
          num++;
        }        
      }

      for (int i = MIN_WEAR; i < MAX_WEAR; i++) {
        if ((th = character->equipment[i])) {
          for (th2 = th->stuff; th2; th2 = th2->nextThing)
            num++;

          num++;
        }
      }
      vlogf(0, "Link Lost for %s: [%d talens/%d bank/%.2f xps/%d items/%d age-mod/%d rent]",
            character->getName(), character->getMoney(), character->getBank(),
            character->getExp(), num, character->age_mod, 
            dynamic_cast<TPerson *>(character)->last_rent);
      character->desc = NULL;
      character->setInvisLevel(GOD_LEVEL1);

      if (character->riding) 
        character->dismount(POSITION_STANDING);
      
      // this is done out of nceness to keep people from walking linkdeads 
      // to someplace nasty
      if (!character->isAffected(AFF_CHARM)) {
        if (character->master)
          character->stopFollower(TRUE);
      }
      character->fixClientPlayerLists(TRUE);
    } else {
      if (connected == CON_PWDNRM)
        bad_login++;
      if (character->getName())
        vlogf(1, "Losing player: %s [%s].", character->getName(), host);

      // shove into list so delete works OK
      character->desc = NULL;
      character->next = character_list;
      character_list = character;

      if (character->inRoom() >= 0) {
        // loadFromSt will have inRoom() == last rent
        // roomp not set yet, so just clear this value
        character->setRoom(ROOM_VOID);
      }
      rp = real_roomp(ROOM_VOID);
      *rp += *character;
      delete character;
      character = NULL;
    }
  }
  // to avoid crashing the process loop
  while (next_to_process && next_to_process == this)
    next_to_process = next_to_process->next;

  if (this == descriptor_list)
    descriptor_list = descriptor_list->next;
  else {
    for (tmp = descriptor_list; tmp && (tmp->next != this); tmp = tmp->next);

    mud_assert(tmp != NULL, 
                 "~Descriptor : unable to find previous descriptor.");

    if (tmp)
      tmp->next = next;
  }
  delete [] showstr_head;
  showstr_head = NULL;

  tot_pages = cur_page = 0;

  delete [] pagedfile;
  pagedfile = NULL;

  delete account;
  account = NULL;

  delete socket;

  // Was I editing something???
  // These are not in char_list or obj_list anymore, so don't leak them
  // we already did some MEDIT/OEDIT checks above for obvious handling

  // this is mostly just for sanity
  // we can get here if lose-link while editing, and reconnect
#if 1
  // remove me if stable, i don't think mob would ever be in list
  if (mob) {
    TBeing *tch;
    bool found = false;
    for (tch = character_list; tch; tch = tch->next) {
      if (tch == mob) {
        forceCrash("Whoa bad!, in list but adding to list again");
        found = true;
      }
    }
    if (!found) {
      mob->next = character_list;
      character_list = mob;
    }
  }
#endif

  delete mob;
  delete obj;
}

void Descriptor::cleanUpStr()
{
  if (str) {
    if (character && 
          (character->isPlayerAction(PLR_MAILING) ||
           character->isPlayerAction(PLR_BUGGING))) {
      delete [] *str;
      *str = NULL;
      delete [] str;
      str = NULL;
    } else if (character &&
                  (connected == CON_WRITING ||
                   connected == CON_REDITING ||
                   connected == CON_OEDITING ||
                   connected == CON_MEDITING ||
                   connected == CON_SEDITING)) {
      // the str is attached to the mob/obj/room, so this is OK
    } else
      vlogf(6, "Descriptor::cleanUpStr(): Probable memory leak");
  }
}

void Descriptor::flushInput()
{
  char dummy[MAX_STRING_LENGTH];

  while (input.takeFromQ(dummy));
}

void Descriptor::flush()
{
  char dummy[MAX_STRING_LENGTH];

  while (output.takeFromQ(dummy));
  while (input.takeFromQ(dummy));
}

void Descriptor::add_to_history_list(const char *arg)
{
  int i;

  unsigned int hist_size = sizeof(history[0]) * sizeof(char);
  for (i = 9; i >= 1; i--) {
    strncpy(history[i], history[i - 1], hist_size - 1);
    history[i][hist_size-1] = '\0';
  }

  strncpy(history[0], arg, hist_size - 1);
  history[0][hist_size-1] = '\0';
}

void TPerson::autoDeath()
{
  char buf[1024];

  vlogf(6,"%s reconnected with negative hp, auto death occurring.", 
                        getName());
  sendTo("You reconnected with negative hit points, automatic death occurring.");
  sprintf(buf, "%s detected you reconnecting with %d hit points.\n\r", MUD_NAME, getHit());
  sprintf(buf + strlen(buf), "In order to discourage people from dropping link in order to avoid death,\n\r");
  sprintf(buf + strlen(buf), "it was decided that such an event would result in a partial death.\n\r");
  sprintf(buf + strlen(buf), "As such, you have been penalized %.2f experience.\n\r\n\r", deathExp());
  sprintf(buf + strlen(buf), "Please understand that this is a code generated\n\r");
  sprintf(buf + strlen(buf), "message and does not come from any particular god.\n\r");
  sprintf(buf + strlen(buf), "If you have problems or questions regarding this action, please feel free to\n\r");
  sprintf(buf + strlen(buf), "contact a god.  Type WHO -G to see if any gods are connected.\n\r");
            
  autoMail(this, NULL, buf);
  gain_exp(this, -deathExp());
  genericKillFix();
  return;
}

// if descriptor is to be deleted, DELETE_THIS
int Descriptor::nanny(const char *arg)
{
  char buf[256];
  int count = 0, local_stats = 0;
  charFile st;
  TBeing *tmp_ch;
  Descriptor *k, *k2;
  char tmp_name[20];
  int rc, which;
  bool found, go2next = FALSE;
  int free_stat;
  TRoom *rp;
  string str;

  switch (connected) {
    case CON_QRACE:
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      for (; isspace(*arg); arg++);

      if (!*arg)
        sendRaceList();
      else {
        switch (*arg) {
          case '1':
            character->setRace(RACE_HUMAN);
            character->cls();
            writeToQ("Ok, race set.\n\r\n\r");
            connected = CON_HOME_HUMAN;
            sendHomeList();
            break;
          case '2':
            character->setRace(RACE_GNOME);
            character->cls();
            writeToQ("Ok, race set.\n\r\n\r");
            connected = CON_HOME_GNOME;
            sendHomeList();
            break;
          case '3':
            character->setRace(RACE_ELVEN);
            character->cls();
            writeToQ("Ok, race set.\n\r\n\r");
            connected = CON_HOME_ELF;
            sendHomeList();
            break;
          case '4':
            character->setRace(RACE_OGRE);
            character->cls();
            writeToQ("Ok, race set.\n\r\n\r");
            connected = CON_HOME_OGRE;
            sendHomeList();
            break;
          case '5':
            character->setRace(RACE_DWARF);
            character->cls();
            writeToQ("Ok, race set.\n\r\n\r");
            connected = CON_HOME_DWARF;
            sendHomeList();
            break;
          case '6':
            character->setRace(RACE_HOBBIT);
            character->cls();
            writeToQ("Ok, race set.\n\r\n\r");
            connected = CON_HOME_HOBBIT;
            sendHomeList();
            break;
          case '?':
            character->cls();
            file_to_string(RACEHELP, str);
            character->fullscreen();

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

            page_string(str.c_str(), TRUE);
            connected = CON_QRACE;
            break;
	  case 'X':
	    if(IS_SET(account->flags, ACCOUNT_IMMORTAL)){
	      int racenum=atoi((arg+1));
	      character->setRace(race_t(racenum));
	      character->cls();
	      writeToQ("Ok, race set.\n\r\n\r");
	      connected = CON_HOME_HUMAN;
	      sendHomeList();
	    } else {
	      writeToQ("For help type '?'. \n\r");
	      writeToQ("To go back a menu type '/'.\n\r");
	      writeToQ("To disconnect type '~'.\n\r");
	      writeToQ("Race--> ");
	      connected = CON_QRACE;
	    }
	    break;
          case '/':
            go_back_menu(connected);
            break;
          case '~':
            return DELETE_THIS;
          default:
            writeToQ("For help type '?'. \n\r");
            writeToQ("To go back a menu type '/'.\n\r");
            writeToQ("To disconnect type '~'.\n\r");
            writeToQ("Race--> ");
            connected = CON_QRACE;
            break;
        }
      }
      break;
    case CON_NME:                // wait for input of name   
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      if (!*arg) 
        return DELETE_THIS;   // notify calling proc to delete me
       
      if (_parse_name(arg, tmp_name)) {
        writeToQ("Illegal name, please try another.\n\r");
        writeToQ("Name -> ");
        return FALSE;
      }
      if (checkForCharacter(tmp_name)) {
        writeToQ("Name -> ");
        return FALSE;
      }

      character->name = new char[strlen(tmp_name) + 1];
      strcpy(character->name, cap(tmp_name));
      character->cls();
      file_to_string("objdata/books/1458.28", str);
      str += "\n\r[Press Return to continue]\n\r";

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

      character->fullscreen();
      writeToQ(str.c_str());
      connected = CON_DISCLAIMER;
      break;
    case CON_DISCLAIMER:
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      character->cls();
      file_to_string("objdata/books/1458.29", str);
      str += "\n\r[Press Return to continue]\n\r";

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

      character->fullscreen();
      writeToQ(str.c_str());
      connected = CON_DISCLAIMER2;
      break;
    case CON_DISCLAIMER2:
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      character->cls();
      file_to_string("objdata/books/1458.30", str);
      str += "\n\r[Press Return to continue]\n\r";
      character->fullscreen();

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

      writeToQ(str.c_str());
      connected = CON_DISCLAIMER3;
      break;
    case CON_DISCLAIMER3:
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      writeToQ("Now you pick your gender.\n\r");
      writeToQ("What is your gender?\n\r1. Male\n\r2. Female?\n\r");
      writeToQ("--> ");
      connected = CON_QSEX;
      break;
    case CON_CONN:
      if (!*arg) {
        rp = real_roomp(ROOM_VOID);
        *rp += *character;
        delete character;
        character = NULL;
        if (account->term == TERM_ANSI) 
          SET_BIT(plr_act, PLR_COLOR);
        
        rc = doAccountMenu("");
        if (IS_SET_DELETE(rc, DELETE_THIS))
          return DELETE_THIS;

        break;
      }
      if (_parse_name(arg, tmp_name)) {
        writeToQ("Illegal name, please try another.\n\r");
        writeToQ("Name -> ");
        return FALSE;
      }

      if (IS_SET(account->flags, ACCOUNT_EMAIL)) {
        // too bad they can't do this from the menu, but they won't get this
        // far if this was set anyway
        writeToQ("The email account you entered for your account is thought to be bogus.\n\r");
        sprintf(buf, "You entered an email address of: %s\n\r", account->email);
        writeToQ(buf);
        sprintf(buf, "If this address is truly valid, please send a mail from it to: %s", MUDADMIN_EMAIL);
        writeToQ(buf);
        writeToQ("Otherwise, please change your account email address.\n\r");
        rp = real_roomp(ROOM_VOID);
        *rp += *character;
        delete character;
        character = NULL;
        if (account->term == TERM_ANSI) 
          SET_BIT(plr_act, PLR_COLOR);
        
        rc = doAccountMenu("");
        if (IS_SET_DELETE(rc, DELETE_THIS))
          return DELETE_THIS;

        break;
      }

      if (load_char(tmp_name, &st)) {
        dynamic_cast<TPerson *>(character)->loadFromSt(&st);
      } else {
        writeToQ("No such character, please enter another name.\n\r");
        writeToQ("Name -> ");
        return FALSE;
      }

      if (strcasecmp(account->name, st.aname)) {
        writeToQ("No such character, please enter another name.\n\r");
        writeToQ("Name -> ");
        // character existed, but wasn't in my account
        // loadFromSt has initted character (improperly)
        // delete it and recreate it so initialization will be proper

        // deletion at this point is semi-problematic
        // we need to remove desc so the doAccountMenu() in ~TBeing skips
        // but this means character_list assumes presence
        // temporarily shove them into the char_list, and delete will
        // remove them
        character->desc = NULL;
        character->next = character_list;
        character_list = character;

        character->setRoom(ROOM_NOWHERE);

        delete character;
        character = new TPerson(this);
        return FALSE;
      }
      for (k = descriptor_list; k; k = k->next) {
        if ((k->character != character) && k->character) {
          if (k->original) {
            if (k->original->getName() && !strcasecmp(k->original->getName(), character->getName())) {
              writeToQ("That character is already connected.\n\rReconnect? :");
              connected = CON_DISCON;
              return FALSE;
            }
          } else {
            if (k->character->getName() && !strcasecmp(k->character->getName(), character->getName())) {
              writeToQ("That character is already connected.\n\rReconnect? :");
              connected = CON_DISCON;
              return FALSE;
            }
          }
        }
      }
      for (tmp_ch = character_list; tmp_ch; tmp_ch = tmp_ch->next) {
        if ((!strcasecmp(character->getName(), tmp_ch->getName()) &&
            !tmp_ch->desc && dynamic_cast<TPerson *>(tmp_ch)) ||
            (dynamic_cast<TMonster *>(tmp_ch) && tmp_ch->orig &&
             !strcasecmp(character->getName(),
                      tmp_ch->orig->getName()))) {

          if (character->inRoom() >= 0) {
            // loadFromSt will have inRoom() == last rent
            // roomp not set yet, so just clear this value
            character->setRoom(ROOM_VOID);
          }
          thing_to_room(character, ROOM_VOID);
          delete character;
          tmp_ch->desc = this;
          character = tmp_ch;
          tmp_ch->setTimer(0);
          tmp_ch->setInvisLevel(0);

          if (tmp_ch->GetMaxLevel() > MAX_MORT &&
              FORCE_LOW_INVSTE &&
              !tmp_ch->hasWizPower(POWER_VISIBLE)) {
            if (!tmp_ch->isPlayerAction(PLR_STEALTH))
              tmp_ch->addPlayerAction(PLR_STEALTH);

            if (tmp_ch->getInvisLevel() <= MAX_MORT)
              tmp_ch->setInvisLevel(GOD_LEVEL1);
          }

          if (tmp_ch->orig) {
            tmp_ch->desc->original = tmp_ch->orig;
            tmp_ch->orig = 0;
          }
          connected = CON_PLYNG;
          EchoOn();
          // This is a semi-kludge to fix some extra crap we had being sent
          // upon reconnect - Russ 6/15/96
          flush();
          writeToQ("Reconnecting.\n\r");
          tmp_ch->initDescStuff(&st);
          if (tmp_ch->isPlayerAction(PLR_VT100 | PLR_ANSI))
            tmp_ch->doCls(false);

          rc = checkForMultiplay();
#if FORCE_MULTIPLAY_COMPLIANCE
          if (rc) {
            // disconnect, but don't cause character to be deleted
            // do this by disassociating character from descriptor
            character = NULL;

            return DELETE_THIS;
          }
#endif
          if (should_be_logged(character)) {
            objCost cost;

            if (IS_SET(account->flags, ACCOUNT_IMMORTAL)) {
              vlogf(0, "%s[*masked*] has reconnected  (account: *masked*).",
                     character->getName());
            } else {
              vlogf(0, "%s[%s] has reconnected  (account: %s).", 
                     character->getName(), host, account->name);
            }

            character->recepOffer(NULL, &cost);
            dynamic_cast<TPerson *>(character)->saveRent(&cost, FALSE, 1);
          }
          act("$n has reconnected.", TRUE, tmp_ch, 0, 0, TO_ROOM);
          tmp_ch->loadCareerStats();
	  tmp_ch->loadDrugStats();
          if (tmp_ch->getHit() < 0) 
            dynamic_cast<TPerson *>(tmp_ch)->autoDeath();
          
          tmp_ch->fixClientPlayerLists(FALSE);
          return FALSE;
        }
      }
      if (should_be_logged(character)) {
        if (IS_SET(account->flags, ACCOUNT_IMMORTAL)) {
          vlogf(0, "%s[*masked*] has connected  (account: *masked*).", character->getName());
        } else {
          vlogf(0, "%s[%s] has connected  (account: %s).", character->getName(), host, account->name);
        }
      }
      
      character->cls();
      sendMotd(character->GetMaxLevel() > MAX_MORT);

      writeToQ("\n\r\n\r*** PRESS RETURN: ");
      connected = CON_RMOTD;
      break;
    case CON_DISCON:
      for (; isspace(*arg); arg++);

      if (!*arg) {
        writeToQ("Please enter 'Y' or 'N'\n\rReconnect? :");
        break;
      }
      switch(*arg) {
        case 'Y':
        case 'y':
          for (k = descriptor_list; k; k = k2) {
            k2 = k->next;
            if ((k->character != character) && k->character) {
              if (k->original) {
                if (k->original->getName() && 
                    !strcasecmp(k->original->getName(), character->getName())) {
                  delete k;
                  k = NULL;
                }
              } else {
                if (k->character->getName() && 
                  !strcasecmp(k->character->getName(), character->getName())) {

                  if (k->character) {
                    // disassociate the char from old descriptor before
                    // we delete the old descriptor
                    k->character->desc = NULL;
                    k->character = NULL;
                  }
                  delete k;
                  k = NULL;
                }
              }
            }
          }
          for (tmp_ch = character_list; tmp_ch; tmp_ch = tmp_ch->next) {
            if ((!strcasecmp(character->getName(), tmp_ch->getName()) &&
                !tmp_ch->desc && dynamic_cast<TPerson *>(tmp_ch)) ||
                (dynamic_cast<TMonster *>(tmp_ch) && tmp_ch->orig &&
                 !strcasecmp(character->getName(),
                          tmp_ch->orig->getName()))) {
  
              if (character->inRoom() >= 0) {
                // loadFromSt will have inRoom() == last rent
                // roomp not set yet, so just clear this value
                character->setRoom(ROOM_NOWHERE);
              }
              rp = real_roomp(ROOM_VOID);
              *rp += *character;
              delete character;
              tmp_ch->desc = this;
              character = tmp_ch;
              tmp_ch->setTimer(0);
              tmp_ch->setInvisLevel(0);

              if (tmp_ch->GetMaxLevel() > MAX_MORT &&
                  FORCE_LOW_INVSTE &&
                  !tmp_ch->hasWizPower(POWER_VISIBLE)) {
                if (!tmp_ch->isPlayerAction(PLR_STEALTH))
                  tmp_ch->addPlayerAction(PLR_STEALTH);

                if (tmp_ch->getInvisLevel() <= MAX_MORT)
                  tmp_ch->setInvisLevel(GOD_LEVEL1);
              }

              if (tmp_ch->orig) {
                tmp_ch->desc->original = tmp_ch->orig;
                tmp_ch->orig = 0;
              }
              connected = CON_PLYNG;
              EchoOn();
              flush();
              writeToQ("Reconnecting.\n\r");
              tmp_ch->initDescStuff(&st);
  
              if (tmp_ch->isPlayerAction(PLR_VT100 | PLR_ANSI))
                tmp_ch->doCls(false);
  
              if (should_be_logged(character)) {
                objCost cost;

                if (IS_SET(account->flags, ACCOUNT_IMMORTAL)) 
                  vlogf(0, "%s[*masked*] has reconnected  (account: *masked*).", tmp_ch->getName());
                else 
                  vlogf(0, "%s[%s] has reconnected  (account: %s).", tmp_ch->getName(), host, account->name);
                
                tmp_ch->recepOffer(NULL, &cost);
                dynamic_cast<TPerson *>(tmp_ch)->saveRent(&cost, FALSE, 1);
              }
              act("$n has reconnected.", TRUE, tmp_ch, 0, 0, TO_ROOM);
              tmp_ch->loadCareerStats();
              tmp_ch->loadDrugStats();
              if (tmp_ch->getHit() < 0) 
                dynamic_cast<TPerson *>(tmp_ch)->autoDeath();
              
              tmp_ch->fixClientPlayerLists(FALSE);
              return FALSE;
            }
          }
          break;
        case 'N':
        case 'n':
          return DELETE_THIS;
        default:
          writeToQ("Please enter 'y' or 'n'\n\rReconnect? :");
          break;
      }
      break;
    case CON_QSEX:                // query sex of new user    
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      for (; isspace(*arg); arg++);
      switch (*arg) {
        case '1':
        case 'm':
        case 'M':
          character->setSex(SEX_MALE);
          break;
        case '2':
        case 'f':
        case 'F':
          character->setSex(SEX_FEMALE);
          break;
        case '~':
           return DELETE_THIS;
        default:
          character->cls();
          writeToQ("That's not a sex.\n\r");
          writeToQ("What IS your gender?\n\r1. Male\n\r2. Female\n\r");
          writeToQ("--> ");
          return FALSE;
      }
//    This is really used later in stats, this is just a good
//    place to initialize it
      character->chosenStats.zero();

      character->setFaction(FACT_NONE);
      character->cls();
      writeToQ("Now you get to pick your handedness. The hand you pick\n\r");
      writeToQ("as your primary hand will be the strongest, and be able to\n\r");
      writeToQ("do more things than your secondary hand.\n\r");
      writeToQ("To go back a menu type '/'.\n\r");
      writeToQ("To disconnect type '~'.\n\r");
      writeToQ("\n\rPick your primary hand : 1) Right  2) Left\n\r--> ");
      connected = CON_QHANDS;
      break;
    case CON_HOME_HUMAN:
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      for (; isspace(*arg); arg++);
      if (!*arg) {
        sendHomeList();
        return FALSE;
      } else if (*arg == '/') {
        go_back_menu(connected);
        return FALSE;
      } else if (*arg == '~') {
        return DELETE_THIS;
      }
      switch (*arg) {
        case '1':
          character->cls();
          writeToQ("OK, you were an urban youth.\n\r");
          character->player.hometerrain = HOME_TER_HUMAN_URBAN;
          break;
        case '2':
          character->cls();
          writeToQ("OK, you hail from a modest village.\n\r");
          character->player.hometerrain = HOME_TER_HUMAN_VILLAGER;
          break;
        case '3':
          character->cls();
          writeToQ("OK, you come from the rolling plains.\n\r");
          character->player.hometerrain = HOME_TER_HUMAN_PLAINS;
          break;
        case '4':
          character->cls();
          writeToQ("OK, you hail from reclusive background.\n\r");
          character->player.hometerrain = HOME_TER_HUMAN_RECLUSE;
          break;
        case '5':
          character->cls();
          writeToQ("OK, you hail from the rolling hills.\n\r");
          character->player.hometerrain = HOME_TER_HUMAN_HILL;
          break;
        case '6':
          character->cls();
          writeToQ("OK, you hail from the high mountains.\n\r");
          character->player.hometerrain = HOME_TER_HUMAN_MOUNTAIN;
          break;
        case '7':
          character->cls();
          writeToQ("OK, you hail from the green forests.\n\r");
          character->player.hometerrain = HOME_TER_HUMAN_FOREST;
          break;
        case '8':
          character->cls();
          writeToQ("OK, you come from a coastal village.\n\r");
          character->player.hometerrain = HOME_TER_HUMAN_MARINER;
          break;
        case 'h':
        case 'H':
        case '?':
          character->cls();
          file_to_string(TER_HUMAN_HELP, str);
          character->fullscreen();

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

          page_string(str.c_str(), TRUE);
          return FALSE;
        default:
          writeToQ("That's not a valid choice.\n\r");
          return FALSE;
      }

      connected = CON_QCLASS;
      sendClassList(1);

      break;
    case CON_HOME_ELF:
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      for (; isspace(*arg); arg++);
      if (!*arg) {
        sendHomeList();
        return FALSE;
      } else if (*arg == '/') {
        go_back_menu(connected);
        return FALSE;
      } else if (*arg == '~') {
        return DELETE_THIS;
      }
      switch (*arg) {
        case '1':
          character->cls();
          writeToQ("OK, you were an urban elfling.\n\r");
          character->player.hometerrain = HOME_TER_ELF_URBAN;
          break;
        case '2':
          character->cls();
          writeToQ("OK, you hail from a modest village tribe.\n\r");
          character->player.hometerrain = HOME_TER_ELF_TRIBE;
          break;
        case '3':
          character->cls();
          writeToQ("OK, you were a plains elfling.\n\r");
          character->player.hometerrain = HOME_TER_ELF_PLAINS;
          break;
        case '4':
          character->cls();
          writeToQ("OK, you were a snow elfling.\n\r");
          character->player.hometerrain = HOME_TER_ELF_SNOW;
          break;
        case '5':
          character->cls();
          writeToQ("OK, you were a civilized wood elfling.\n\r");
          character->player.hometerrain = HOME_TER_ELF_WOOD;
          break;
        case '6':
          character->cls();
          writeToQ("OK, you were a sea elfling.\n\r");
          character->player.hometerrain = HOME_TER_ELF_SEA;
          break;
        case '7':
          character->cls();
          writeToQ("OK, you were a reclusive elfling.\n\r");
          character->player.hometerrain = HOME_TER_ELF_RECLUSE;
          break;
        case 'h':
        case 'H':
        case '?':
          character->cls();
          file_to_string(TER_ELF_HELP, str);
          character->fullscreen();

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

          page_string(str.c_str(), TRUE);
          return FALSE;
        default:
          writeToQ("That's not a valid choice.\n\r");
          return FALSE;
      }

      connected = CON_QCLASS;
      sendClassList(1);

      break;
    case CON_HOME_DWARF:
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      for (; isspace(*arg); arg++);
      if (!*arg) {
        sendHomeList();
        return FALSE;
      } else if (*arg == '/') {
        go_back_menu(connected);
        return FALSE;
      } else if (*arg == '~') {
        return DELETE_THIS;
      }
      switch (*arg) {
        case '1':
          character->cls();
          writeToQ("OK, you were an urban dwarfling.\n\r");
          character->player.hometerrain = HOME_TER_DWARF_URBAN;
          break;
        case '2':
          character->cls();
          writeToQ("OK, you hail from a modest dwarf village.\n\r");
          character->player.hometerrain = HOME_TER_DWARF_VILLAGER;
          break;
        case '3':
          character->cls();
          writeToQ("OK, you were a hill dwarfling.\n\r");
          character->player.hometerrain = HOME_TER_DWARF_HILL;
          break;
        case '4':
          character->cls();
          writeToQ("OK, you were a mountain dwarfling.\n\r");
          character->player.hometerrain = HOME_TER_DWARF_MOUNTAIN;
          break;
        case '5':
          character->cls();
          writeToQ("OK, you were a reclusive dwarfling.\n\r");
          character->player.hometerrain = HOME_TER_DWARF_RECLUSE;
          break;
        case 'h':
        case 'H':
        case '?':
          character->cls();
          file_to_string(TER_DWARF_HELP, str);
          character->fullscreen();

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

          page_string(str.c_str(), TRUE);
          return FALSE;
        default:
          writeToQ("That's not a valid choice.\n\r");
          return FALSE;
      }

      connected = CON_QCLASS;
      sendClassList(1);

      break;
    case CON_HOME_GNOME:
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      for (; isspace(*arg); arg++);
      if (!*arg) {
        sendHomeList();
        return FALSE;
      } else if (*arg == '/') {
        go_back_menu(connected);
        return FALSE;
      } else if (*arg == '~') {
        return DELETE_THIS;
      }
      switch (*arg) {
        case '1':
          character->cls();
          writeToQ("OK, you were an urban gnomelet.\n\r");
          character->player.hometerrain = HOME_TER_GNOME_URBAN;
          break;
        case '2':
          character->cls();
          writeToQ("OK, you hail from a modest gnome village.\n\r");
          character->player.hometerrain = HOME_TER_GNOME_VILLAGER;
          break;
        case '3':
          character->cls();
          writeToQ("OK, you were a hill gnomelet.\n\r");
          character->player.hometerrain = HOME_TER_GNOME_HILL;
          break;
        case '4':
          character->cls();
          writeToQ("OK, you were a swamp gnomelet.\n\r");
          character->player.hometerrain = HOME_TER_GNOME_SWAMP;
          break;
        case 'h':
        case 'H':
        case '?':
          character->cls();
          file_to_string(TER_GNOME_HELP, str);
          character->fullscreen();

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

          page_string(str.c_str(), TRUE);
          return FALSE;
        default:
          writeToQ("That's not a valid choice.\n\r");
          return FALSE;
      }

      connected = CON_QCLASS;
      sendClassList(1);

      break;
    case CON_HOME_OGRE:
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      for (; isspace(*arg); arg++);
      if (!*arg) {
        sendHomeList();
        return FALSE;
      } else if (*arg == '/') {
        go_back_menu(connected);
        return FALSE;
      } else if (*arg == '~') {
        return DELETE_THIS;
      }
      switch (*arg) {
        case '1':
          character->cls();
          writeToQ("Ugh!  Me villager ogreling!\n\r");
          character->player.hometerrain = HOME_TER_OGRE_VILLAGER;
          break;
        case '2':
          character->cls();
          writeToQ("Ugh!  Me plains ogreling!\n\r");
          character->player.hometerrain = HOME_TER_OGRE_PLAINS;
          break;
        case '3':
          character->cls();
          writeToQ("Ugh!  Me hill ogreling!\n\r");
          character->player.hometerrain = HOME_TER_OGRE_HILL;
          break;
        case 'h':
        case 'H':
        case '?':
          character->cls();
          file_to_string(TER_OGRE_HELP, str);
          character->fullscreen();

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

          page_string(str.c_str(), TRUE);
          return FALSE;
        default:
          writeToQ("Ugh!  Choice BAD!\n\r");
          return FALSE;
      }

      connected = CON_QCLASS;
      sendClassList(1);

      break;
    case CON_HOME_HOBBIT:
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      for (; isspace(*arg); arg++);
      if (!*arg) {
        sendHomeList();
        return FALSE;
      } else if (*arg == '/') {
        go_back_menu(connected);
        return FALSE;
      } else if (*arg == '~') {
        return DELETE_THIS;
      }
      switch (*arg) {
        case '1':
          character->cls();
          writeToQ("OK, you were an urban hobbit youth.\n\r");
          character->player.hometerrain = HOME_TER_HOBBIT_URBAN;
          break;
        case '2':
          character->cls();
          writeToQ("OK, you hail from a modest shire.\n\r");
          character->player.hometerrain = HOME_TER_HOBBIT_SHIRE;
          break;
        case '3':
          character->cls();
          writeToQ("OK, you were a grasslands hobbit youth.\n\r");
          character->player.hometerrain = HOME_TER_HOBBIT_GRASSLANDS;
          break;
        case '4':
          character->cls();
          writeToQ("OK, you were a hill hobbit youth.\n\r");
          character->player.hometerrain = HOME_TER_HOBBIT_HILL;
          break;
        case '5':
          character->cls();
          writeToQ("OK, you were a woodland hobbit youth.\n\r");
          character->player.hometerrain = HOME_TER_HOBBIT_WOODLAND;
          break;
        case '6':
          character->cls();
          writeToQ("OK, you were a maritime hobbit youth.\n\r");
          character->player.hometerrain = HOME_TER_HOBBIT_MARITIME;
          break;
        case 'h':
        case 'H':
        case '?':
          character->cls();
          file_to_string(TER_HOBBIT_HELP, str);
          character->fullscreen();

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

          page_string(str.c_str(), TRUE);
          return FALSE;
        default:
          writeToQ("That's not a valid choice.\n\r");
          return FALSE;
      }

      connected = CON_QCLASS;
      sendClassList(1);

      break;
    case CON_ENTER_DONE:
    case CON_CREATE_DONE:
      for (; isspace(*arg); arg++);
      switch (*arg) {
        case 'e':
        case 'E':
          break;
        case '~':
          return DELETE_THIS;
        case '/':
          go_back_menu(connected);
          return FALSE;
        default:
          sendDoneScreen();
          return FALSE;
      }
      character->convertAbilities();
      character->affectTotal();
      vlogf(9, "%s [%s] new player.", character->getName(), host);

      character->saveChar(ROOM_AUTO_RENT);
      player_count++;
      character->cls();
      sendMotd(FALSE);
      writeToQ("\n\r\n*** PRESS RETURN: ");
      connected = CON_RMOTD;
      break;
    case CON_QCLASS:{
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      for (; isspace(*arg); arg++);
      character->setClass(0);
      count = 0;
      switch (*arg) {
        case '1':
          if (canChooseClass(CLASS_WARRIOR)) {
              character->setClass(CLASS_WARRIOR);
              go2next = TRUE;
          } else {
            writeToQ(badClassMessage(CLASS_WARRIOR).c_str());
            writeToQ("--> ");
            connected = CON_QCLASS;
          }
          break;
        case '2':
          if (canChooseClass(CLASS_CLERIC)) {
            character->setClass(CLASS_CLERIC);
              go2next = TRUE;
          } else {
            writeToQ(badClassMessage(CLASS_CLERIC).c_str());
            writeToQ("--> ");
            connected = CON_QCLASS;
          }
          break;
          case '3':
            if (canChooseClass(CLASS_MAGIC_USER)) {
              character->setClass(CLASS_MAGIC_USER);
              go2next = TRUE;
            } else {
              writeToQ(badClassMessage(CLASS_MAGIC_USER).c_str());
              writeToQ("--> ");
                     connected = CON_QCLASS;
            }
            break;
          case '4':
            if (canChooseClass(CLASS_THIEF)) {
              character->setClass(CLASS_THIEF);
              go2next = TRUE;
            } else {
                writeToQ(badClassMessage(CLASS_THIEF).c_str());
              writeToQ("--> ");
              connected = CON_QCLASS;
            }
            break;
          case '5':
            if (canChooseClass(CLASS_DEIKHAN)) {
              character->setClass(CLASS_DEIKHAN);
              go2next = TRUE;
            } else {
              writeToQ(badClassMessage(CLASS_DEIKHAN).c_str());
              writeToQ("--> ");
              connected = CON_QCLASS;
            }
            break;
          case '6':
            if (canChooseClass(CLASS_MONK)) {
              character->setClass(CLASS_MONK);
              go2next = TRUE;
            } else {
              writeToQ(badClassMessage(CLASS_MONK).c_str());
              writeToQ("--> ");
              connected = CON_QCLASS;
            }
            break;
          case '7':
            if (canChooseClass(CLASS_RANGER)) {
              character->setClass(CLASS_RANGER);
              go2next = TRUE;
            } else {
              writeToQ(badClassMessage(CLASS_RANGER).c_str());
              writeToQ("--> ");
              connected = CON_QCLASS;
            }
            break;
          case '8':
#if 0
            if (canChooseClass(CLASS_SHAMAN)) {
              character->setClass(CLASS_SHAMAN);
              go2next = TRUE;
            } else {
              writeToQ(badClassMessage(CLASS_SHAMAN).c_str());
              writeToQ("--> ");
              connected = CON_QCLASS;
            }
#endif
            writeToQ("Shaman are not yet a playable class, sorry.\n\r");
            writeToQ("--> ");
            connected = CON_QCLASS;
            break;
          case 'a':
          case 'A':
            if (canChooseClass(CLASS_THIEF | CLASS_WARRIOR, TRUE)) {
              if (IS_SET(account->flags, ACCOUNT_ALLOW_DOUBLECLASS)) {
                character->setClass(CLASS_WARRIOR + CLASS_THIEF);
                go2next = TRUE;
              } else {
                writeToQ("You must obtain a L50 single-class character first.\n\r");
                writeToQ("--> ");
                connected = CON_QCLASS;
              }
            } else {
              writeToQ(badClassMessage(CLASS_THIEF | CLASS_WARRIOR, TRUE).c_str());
              writeToQ("--> ");
              connected = CON_QCLASS;
            }
            break;
          case 'b':
          case 'B':
            if (canChooseClass(CLASS_CLERIC | CLASS_WARRIOR, TRUE)) {
              if (IS_SET(account->flags, ACCOUNT_ALLOW_DOUBLECLASS)) {
                character->setClass(CLASS_WARRIOR + CLASS_CLERIC);
                go2next = TRUE;
              } else {
                writeToQ("You must obtain a L50 single-class character first.\n\r");
                writeToQ("--> ");
                connected = CON_QCLASS;
              }
            } else {
              writeToQ(badClassMessage(CLASS_CLERIC | CLASS_WARRIOR, TRUE).c_str());
              writeToQ("--> ");
                   connected = CON_QCLASS;
            }
            break;
          case 'c':
          case 'C':
            if (canChooseClass(CLASS_THIEF | CLASS_MAGIC_USER, TRUE)) {
              if (IS_SET(account->flags, ACCOUNT_ALLOW_DOUBLECLASS)) {
                 character->setClass(CLASS_MAGIC_USER + CLASS_THIEF);
                 go2next = TRUE;
              } else {
                writeToQ("You must obtain a L50 single-class character first.\n\r");
                writeToQ("--> ");
                connected = CON_QCLASS;
              }
            } else {
              writeToQ(badClassMessage(CLASS_THIEF | CLASS_MAGIC_USER, TRUE).c_str());
              writeToQ("--> ");
              connected = CON_QCLASS;
            }
            break;
          case 'd':
          case 'D':
            if (canChooseClass(CLASS_MAGIC_USER | CLASS_WARRIOR, TRUE)) {
              if (IS_SET(account->flags, ACCOUNT_ALLOW_DOUBLECLASS)) {
                character->setClass(CLASS_MAGIC_USER + CLASS_WARRIOR);
                go2next = TRUE;
              } else {
                writeToQ("You must obtain a L50 single-class character first.\n\r");
                writeToQ("--> ");
                connected = CON_QCLASS;
              }
            } else {
              writeToQ(badClassMessage(CLASS_MAGIC_USER | CLASS_WARRIOR, TRUE).c_str());
              writeToQ("--> ");
              connected = CON_QCLASS;
            }
            break;
          case 'e':
          case 'E':
            if (canChooseClass(CLASS_THIEF | CLASS_CLERIC, TRUE)) {
              if (IS_SET(account->flags, ACCOUNT_ALLOW_DOUBLECLASS)) {
                character->setClass(CLASS_CLERIC + CLASS_THIEF);
                go2next = TRUE;
              } else {
                writeToQ("You must obtain a L50 single-class character first.\n\r");
                writeToQ("--> ");
                connected = CON_QCLASS;
              }
            } else {
              writeToQ(badClassMessage(CLASS_THIEF | CLASS_CLERIC, TRUE).c_str());
              writeToQ("--> ");
              connected = CON_QCLASS;
            }
            break;
          case 'f':
          case 'F':
            if (canChooseClass(CLASS_THIEF | CLASS_WARRIOR | CLASS_CLERIC, FALSE, TRUE)) {
              if (IS_SET(account->flags, ACCOUNT_ALLOW_TRIPLECLASS)) {
                character->setClass(CLASS_WARRIOR + CLASS_THIEF + CLASS_CLERIC);
                go2next = TRUE;
              } else {
                writeToQ("You must obtain a L50 double-class character first.\n\r");
                writeToQ("--> ");
                connected = CON_QCLASS;
              }
            } else {
              writeToQ(badClassMessage(CLASS_THIEF | CLASS_WARRIOR | CLASS_CLERIC, FALSE, TRUE).c_str());
              writeToQ("--> ");
              connected = CON_QCLASS;
            }
            break;
          case 'g':
          case 'G':
            if (canChooseClass(CLASS_THIEF | CLASS_WARRIOR | CLASS_MAGIC_USER, FALSE, TRUE)) {
              if (IS_SET(account->flags, ACCOUNT_ALLOW_TRIPLECLASS)) {
                character->setClass(CLASS_MAGIC_USER + CLASS_THIEF + CLASS_WARRIOR);
                go2next = TRUE;
              } else {
                writeToQ("You must obtain a L50 double-class character first.\n\r");
                writeToQ("--> ");
                connected = CON_QCLASS;
              }
            } else {
              writeToQ(badClassMessage(CLASS_THIEF | CLASS_WARRIOR | CLASS_MAGIC_USER, FALSE, TRUE).c_str());
              writeToQ("--> ");
              connected = CON_QCLASS;
            }
            break;
          case '~':
            return DELETE_THIS;
          case '/':
            go_back_menu(connected);
            break;
          case '?':
            character->cls();
            file_to_string(CLASSHELP, str);
            character->fullscreen();

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

            page_string(str.c_str(), TRUE);
            connected = CON_QCLASS;
            break;
          default:
            character->cls();
            sendClassList(FALSE);
            connected = CON_QCLASS;
            break;
      }
      if (go2next) {
        connected = CON_STATS_START;
        sendStartStatList();
      }
      break;
    }
    case CON_QHANDS:
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      switch (*arg) {
        case '1':
          character->cls();
          writeToQ("Ok, you are now right handed!\n\r\n\r");
          character->addPlayerAction(PLR_RT_HANDED);
          break;
        case '2':
          character->cls();
          writeToQ("Ok, you are now left handed!\n\r\n\r");
          // Do nothing - This is determine by !RIGHT_HANDED - Russ 
          break;
        case '/':
          go_back_menu(connected);
          return FALSE;
        case '~':
           return DELETE_THIS;
        default:
          writeToQ("Please pick either 1 or 2.\n\r");
          writeToQ("To go back a menu type '/'.\n\r");
          writeToQ("To disconnect type '~'.\n\r");
          return FALSE;
      }
      sendRaceList();
      connected = CON_QRACE;
      break;
    case CON_STATS_START:
      mud_assert(character != NULL, "Character NULL where it shouldn't be");
      switch (*arg) {
        case 'E':
        case 'e':
          // prevent players from setting stats, then backing out and 
	  // entering the game with unallowed stats - Peel
	  character->chosenStats.values[STAT_STR] =
	    character->chosenStats.values[STAT_BRA] =
	    character->chosenStats.values[STAT_CON] =
	    character->chosenStats.values[STAT_DEX] =
	    character->chosenStats.values[STAT_AGI] =
	    character->chosenStats.values[STAT_INT] =
	    character->chosenStats.values[STAT_WIS] =
	    character->chosenStats.values[STAT_FOC] =
	    character->chosenStats.values[STAT_PER] =
	    character->chosenStats.values[STAT_CHA] =
	    character->chosenStats.values[STAT_KAR] =
	    character->chosenStats.values[STAT_SPE] = 0;
	    
          connected = CON_ENTER_DONE;
          character->cls();
          sendDoneScreen();
          break;
        case 'C':
        case 'c':
          character->cls();
          connected = CON_STATS_RULES;
          sendStatRules(1);
          break;
        case '~':
          return DELETE_THIS;
        case '/':
          go_back_menu(connected);
          return FALSE;
        default:
          writeToQ("Please pick either (E)nter or (C)ustomize.\n\r");
          writeToQ("To go back a menu type '/'.\n\r");
          writeToQ("To disconnect type '~'.\n\r");
          writeToQ("-> ");
          return FALSE;
      }
      break;
    case CON_STATS_RULES:
      connected = CON_STATS_RULES2;
      character->cls();
      sendStatRules(2);
      break;
    case CON_STATS_RULES2:
      connected = CON_STAT_COMBAT;
      character->cls();
      sendStatList(1, TRUE);
      break;
    case CON_STAT_COMBAT:
      if (!*arg) {
        sendStatList(1, FALSE);
        break;
      }
      for (; isspace(*arg); arg++);
      local_stats = 0;

      arg = one_argument(arg, buf);

      if ((*buf == '-') || (*buf == '+')) {
        if (strchr(buf, 's') || strchr(buf, 'S')) {
          local_stats = character->chosenStats.get(STAT_STR),
          which = STAT_STR;
          found = TRUE;
        } else if (strchr(buf, 'd') || strchr(buf, 'D')) {
          local_stats = character->chosenStats.get(STAT_DEX),
          which = STAT_DEX;
          found = TRUE;
        } else if (strchr(buf, 'a') || strchr(buf, 'A')) {
          local_stats = character->chosenStats.get(STAT_AGI),
          which = STAT_AGI;
          found = TRUE;
        } else if (strchr(buf, 'c') || strchr(buf, 'C')) {
          local_stats = character->chosenStats.get(STAT_CON),
          which = STAT_CON;
          found = TRUE;
        } else if (strchr(buf, 'b') || strchr(buf, 'B')) {
          local_stats = character->chosenStats.get(STAT_BRA),
          which = STAT_BRA;
          found = TRUE;
        } else {
          writeToQ("You must specify a valid characteristic.\n\r");
          writeToQ("\n\rPress return....");
          break;
        }
      } else if (*buf == 'e' || *buf == 'E') {
        free_stat = (0 - ((character->chosenStats.values[STAT_STR]) +
                 (character->chosenStats.values[STAT_DEX]) +
                 (character->chosenStats.values[STAT_AGI]) +
                 (character->chosenStats.values[STAT_CON]) +
                 (character->chosenStats.values[STAT_BRA])));
        if (free_stat < 0) {
          character->cls();
          writeToQ("You may not continue with negative free points.\n\r");
          writeToQ("\n\rPress return....");
          break;
        } else {
          connected = CON_STAT_LEARN;
          sendStatList(2, TRUE);
          break;       
        }
      } else if (*buf == '/') {
        go_back_menu(connected);
        break;
      } else if (*buf == '~') {
        return DELETE_THIS;
      } else if (*buf == '?' || *buf == 'h' || *buf == 'H') {
        character->cls();
        file_to_string(STATHELP, str);
        character->fullscreen();

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

        page_string(str.c_str(), TRUE);
        break;
      } else {
//        writeToQ("You typed in an incorrect command at this point.\n\r");
        sendStatList(1, FALSE);
        break;
      }

      if (found) {
        int amt = 0;
        character->cls();

        // buf should have form -9s or whatever
        // fortunately, atoi will strip non signedness and numbers
        // we do need to check for +s and make sure this is +1, -s == -1
        if (!(amt = atoi(buf)))
          amt = (*buf == '+' ? +1 : -1);
        character->sendTo("amount was: %d\n\r", amt);
        // Need to initialize buf or they can cheat 
        memset(buf, '\0', sizeof(buf));
        if (local_stats + amt < -25) {
          writeToQ("You can't go below -25 on any characteristic.\n\r");
          writeToQ("\n\rPress return....");
          break;
        }
        if (local_stats + amt > 25) {
          writeToQ("You can't go above 25 on any characteristic.\n\r");
          writeToQ("\n\rPress return....");
          break;
        }
        character->chosenStats.values[which] += amt;
      }
      if (connected == CON_STAT_COMBAT) {
        sendStatList(1, FALSE);
        break;
      }
      break;
    case CON_STAT_LEARN:
      if (!*arg) {
        sendStatList(2, FALSE);
        break;
      }
      for (; isspace(*arg); arg++);
      local_stats = 0;

      arg = one_argument(arg, buf);
      if ((*buf == '-') || (*buf == '+')) {
        if (strchr(buf, 'i') || strchr(buf, 'I')) {
          local_stats = character->chosenStats.get(STAT_INT),
          which = STAT_INT;
          found = TRUE;
        } else if (strchr(buf, 'w') || strchr(buf, 'W')) {
          local_stats = character->chosenStats.get(STAT_WIS),
          which = STAT_WIS;
          found = TRUE;
        } else if (strchr(buf, 'f') || strchr(buf, 'F')) {
          local_stats = character->chosenStats.get(STAT_FOC),
          which = STAT_FOC;
          found = TRUE;
        } else {
          writeToQ("You must specify a valid characteristic.\n\r");
          writeToQ("\n\rPress return....");
          break;
        }
      } else if (*buf == 'e' || *buf == 'E') {
        free_stat = (0 - ((character->chosenStats.values[STAT_INT]) +
                 (character->chosenStats.values[STAT_FOC]) +
                 (character->chosenStats.values[STAT_WIS])));
        if (free_stat < 0) {
          character->cls();
          writeToQ("You may not continue with negative free points.\n\r");
          writeToQ("\n\rPress return....");
          break;
        } else {
          connected = CON_STAT_UTIL;
          sendStatList(3, TRUE);
          break;
        }
      } else if (*buf == '/') {
        go_back_menu(connected);
        break;
      } else if (*buf == '~') {
        return DELETE_THIS;
      } else if (*buf == '?' || *buf == 'h' || *buf == 'H') {
        character->cls();
        file_to_string(STATHELP, str);
        character->fullscreen();

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

        page_string(str.c_str(), TRUE);
        break;
      } else {
        sendStatList(2, FALSE);
//        writeToQ("You typed in an incorrect command at this point.\n\r");
        break;
      }

      if (found) {
        int amt = 0;
        character->cls();

        // buf should have form -9s or whatever
        // fortunately, atoi will strip non signedness and numbers
        // we do need to check for +s and make sure this is +1, -s == -1
        if (!(amt = atoi(buf)))
          amt = (*buf == '+' ? +1 : -1);
        character->sendTo("amount was: %d\n\r", amt);

        // Need to initialize buf or they can cheat
        memset(buf, '\0', sizeof(buf));

        if (local_stats + amt < -25) {
          writeToQ("You can't go below -25 on any characteristic.\n\r");
          writeToQ("\n\rPress return....");
          break;
        }
        if (local_stats + amt > 25) {
          writeToQ("You can't go above 25 on any characteristic.\n\r");
          writeToQ("\n\rPress return....");
          break;
        }
        character->chosenStats.values[which] += amt;
      }
      if (connected == CON_STAT_LEARN) {
        sendStatList(2, FALSE);
        break;
      }
      break;
    case CON_STAT_UTIL:
      if (!*arg) {
        sendStatList(3, FALSE);
        break;
      }
      for (; isspace(*arg); arg++);
      local_stats = 0;

      arg = one_argument(arg, buf);
      if ((*buf == '-') || (*buf == '+')) {
        if (strchr(buf, 'p') || strchr(buf, 'P')) {
          local_stats = character->chosenStats.get(STAT_PER),
          which = STAT_PER;
          found = TRUE;
        } else if (strchr(buf, 'k') || strchr(buf, 'K')) {
          local_stats = character->chosenStats.get(STAT_KAR),
          which = STAT_KAR;
          found = TRUE;
        } else if (strchr(buf, 'c') || strchr(buf, 'C')) {
          local_stats = character->chosenStats.get(STAT_CHA),
          which = STAT_CHA;
          found = TRUE;
        } else if (strchr(buf, 's') || strchr(buf, 'S')) {
          local_stats = character->chosenStats.get(STAT_SPE),
          which = STAT_SPE;
          found = TRUE;
        } else {
          writeToQ("You must specify a valid characteristic.\n\r");
          writeToQ("\n\rPress return....");
          break;
        }
      } else if (*buf == 'e' || *buf == 'E') {
        free_stat = (0 - ((character->chosenStats.values[STAT_PER]) +
                 (character->chosenStats.values[STAT_KAR]) +
                 (character->chosenStats.values[STAT_CHA]) +
                 (character->chosenStats.values[STAT_SPE])));
        if (free_stat < 0) {
          character->cls();
          writeToQ("You may not continue with negative free points.\n\r");
          writeToQ("\n\rPress return....");
          break;
        } else {
          sendDoneScreen();
          connected = CON_CREATE_DONE;
          break;
        }
      } else if (*buf == '/') {
        go_back_menu(connected);
        break;
      } else if (*buf == '~') {
        return DELETE_THIS;
      } else if (*buf == '?' || *buf == 'h' || *buf == 'H') {
        character->cls();
        file_to_string(STATHELP, str);
        character->fullscreen();

      // swap color strings
      str = colorString(character, this, str.c_str(), NULL, COLOR_BASIC,  false);

        page_string(str.c_str(), TRUE);
        break;
      } else {
        sendStatList(3, FALSE);
//        writeToQ("You typed in an incorrect command at this point.\n\r");
        break;
      }

      if (found) {
        int amt = 0;
        character->cls();

        // buf should have form -9s or whatever
        // fortunately, atoi will strip non signedness and numbers
        // we do need to check for +s and make sure this is +1, -s == -1
        if (!(amt = atoi(buf)))
          amt = (*buf == '+' ? +1 : -1);
        character->sendTo("amount was: %d\n\r", amt);

        // Need to initialize buf or they can cheat
        memset(buf, '\0', sizeof(buf));

        if (local_stats + amt < -25) {
          writeToQ("You can't go below -25 on any characteristic.\n\r");
          writeToQ("\n\rPress return....");
          break;
        }
        if (local_stats + amt > 25) {
          writeToQ("You can't go above 25 on any characteristic.\n\r");
          writeToQ("\n\rPress return....");
          break;
        }
        character->chosenStats.values[which] += amt;
      }
      if (connected == CON_STAT_UTIL) {
        sendStatList(3, FALSE);
        break;
      }
      break;
    case CON_RMOTD:        
      character->doCls(false);
      if (!character->GetMaxLevel())
        dynamic_cast<TPerson *>(character)->doStart();

      rc = dynamic_cast<TPerson *>(character)->genericLoadPC();
      if (IS_SET_DELETE(rc, DELETE_THIS)) {
        // we will wind up deleting desc, and in turn desc->character
        // since something bad happened in the load above, char may
        // have eq, which would cause problems, so...
        dynamic_cast<TPerson *>(character)->dropItemsToRoom(SAFE_YES, NUKE_ITEMS);
        return DELETE_THIS;
      }


      if (character->isPlayerAction(PLR_VT100 | PLR_ANSI))
        character->doCls(false);

      character->fixClientPlayerLists(FALSE);
   
      character->doLook("", CMD_LOOK);
      prompt_mode = 1;
      character->doSave(SILENT_YES);
      break;
    case CON_PLYNG:
    case CON_NMECNF:
    case CON_PWDNRM:
    case CON_PWDCNF:
    case CON_PWDNCNF:
    case CON_WIZLOCK:
    case CON_DELETE:
    case CON_NEWACT:
    case CON_ACTPWD:
    case CON_NEWLOG:
    case CON_NEWACTPWD:
    case CON_EMAIL:
    case CON_TERM:
    case CON_NEWPWD:
    case CON_OLDPWD:
    case CON_RETPWD:
    case CON_DELCHAR:
    case CON_ACTDELCNF:
    case CON_EDITTING:
    case CON_TIME:
    case CON_CHARDELCNF:
    case CON_WIZLOCKNEW:
    case CON_LISTSERV:
    case MAX_CON_STATUS:
    case CON_REDITING:
    case CON_OEDITING:
    case CON_MEDITING:
    case CON_HELP:
    case CON_WRITING:
    case CON_SEDITING:
      vlogf(0, "Nanny: illegal state of con'ness");
      abort();
      break;
  }
  return FALSE;
}

int TPerson::genericLoadPC()
{
  TRoom *rp;
  int rc;

  rc = desc->checkForMultiplay();
#if FORCE_MULTIPLAY_COMPLIANCE
  if (rc)
    return DELETE_THIS;
#endif

  if (should_be_logged(this))
    vlogf(0, "Loading %s's equipment", name);
  resetChar();
  BatoprsResetCharFlags(this);
  loadRent();
  if (player.time.last_logon && (player.time.last_logon < Uptime)) {
    if ((time(0) - player.time.last_logon) > 36 * SECS_PER_REAL_HOUR)
      wipeCorpseFile(lower(name).c_str());
    else
      assignCorpsesToRooms();
  }
  saveChar(ROOM_AUTO_RENT);
  sendTo(WELC_MESSG);
  next = character_list;
  character_list = this;

#if SPEEF_MAKE_BODY
  vlogf(5,"Loading a body for %s\n\r", name);
  body = new Body(race->getBodyType(), points.maxHit);
#endif
  if (in_room == ROOM_NOWHERE || in_room == ROOM_AUTO_RENT) {
    if (banished()) {
      rp = real_roomp(ROOM_HELL);
      *rp += *this;
      player.hometown = ROOM_HELL;
    } else if (GetMaxLevel() <= MAX_MORT) {
      if (player.hometown >= 0) {
        rp = real_roomp(player.hometown);
        if (!rp) {
          vlogf(LOW_ERROR, "Player (%s) had non-existant hometown (%d)", getName(), player.hometown);
          rp = real_roomp(ROOM_CS);
        }
        *rp += *this;
      } else {
        rp = real_roomp(ROOM_CS);
        *rp += *this;
        player.hometown = ROOM_CS;
      }
    } else {
      if (!strcmp(name, "Bump"))
        rp = real_roomp(3);
      else if (!strcmp(name, "Kriebly"))
        rp = real_roomp(4);
      else if (!strcmp(name, "Brutius"))
        rp = real_roomp(5);
      else if (!strcmp(name, "Stargazer"))
        rp = real_roomp(7);
      else if (!strcmp(name, "Spawn"))
       rp = real_roomp(11);
      else if (!strcmp(name, "Kyla"))
        rp = real_roomp(12);
      else if (!strcmp(name, "Batopr"))
        rp = real_roomp(13);
      else if (!strcmp(name, "Meerlar"))
        rp = real_roomp(14);
      else if (!strcmp(name, "Damescena"))
        rp = real_roomp(15);
      else if (!strcmp(name, "Aedion"))
        rp = real_roomp(16);
      else if (!strcmp(name, "Phenohol"))
        rp = real_roomp(17);
      else if (!strcmp(name, "Messiah"))
        rp = real_roomp(18);
      else if (!strcmp(name, "Lothar"))
        rp = real_roomp(19);
      else if (!strcmp(name, "Marsh"))
        rp = real_roomp(20);
      else if (!strcmp(name, "Dirk"))
        rp = real_roomp(24);
      else if (!strcmp(name, "Matel"))
        rp = real_roomp(25);
      else if (!strcmp(name, "Cosmo"))
        rp = real_roomp(26);
      else if (!strcmp(name, "Dolgan"))
        rp = real_roomp(27);
      else if (!strcmp(name, "Mithros"))
        rp = real_roomp(28);
      else if (!strcmp(name, "Armagedon"))
        rp = real_roomp(30);
      else if (!strcmp(name, "Onslaught"))
        rp = real_roomp(31);
      else if (!strcmp(name, "Custer"))
        rp = real_roomp(32);
      else if (!strcmp(name, "Gish"))
        rp = real_roomp(33);
      else if (!strcmp(name, "Smyrke"))
        rp = real_roomp(34);
      else if (!strcmp(name, "Shota"))
        rp = real_roomp(35);
      else if (!strcmp(name, "Jaxom"))
        rp = real_roomp(36);
      else if (!strcmp(name, "Finch"))
        rp = real_roomp(38);
      else if (!strcmp(name, "Omen"))
        rp = real_roomp(39);
      else if (!strcmp(name, "Drentar"))
        rp = real_roomp(40);
      else if (!strcmp(name, "Albria"))
        rp = real_roomp(41);
      else if (!strcmp(name, "Rixanne"))
        rp = real_roomp(42);
      else if (!strcmp(name, "Ghosks"))
        rp = real_roomp(43);
      else if (!strcmp(name, "Wrayth"))
        rp = real_roomp(44);
      else if (!strcmp(name, "Theodoric"))
        rp = real_roomp(45);
      else if (!strcmp(name, "Moath"))
        rp = real_roomp(46);
      else if (!strcmp(name, "Urvile"))
        rp = real_roomp(47);
      else if (!strcmp(name, "Lapsos"))
        rp = real_roomp(48);
      else if (!strcmp(name, "Alyria"))
        rp = real_roomp(50);
      else if (!strcmp(name, "Syl"))
        rp = real_roomp(51);
      else if (!strcmp(name, "Gringar"))
        rp = real_roomp(53);
      else if (!strcmp(name, "Speef"))
        rp = real_roomp(57);
      else if (!strcmp(name, "Sidartha"))
        rp = real_roomp(61);
      else if (!strcmp(name, "Peel"))
        rp = real_roomp(62);
      else if (!strcmp(name, "Coral"))
        rp = real_roomp(63);

      else
        rp = real_roomp(ROOM_IMPERIA);

      if (!rp) {
        vlogf(9, "Attempting to place %s in room that does not exist.\n\r", name);
        rp = real_roomp(ROOM_VOID);
      }
      in_room = ROOM_NOWHERE;  // change it so it doesn't error in +=
      *rp += *this;
      player.hometown = ROOM_IMPERIA;
      wizFileRead();        // Immort bamfins 
      if (!isImmortal())   // they turned it off
        doImmortal();

      if (FORCE_LOW_INVSTE && !hasWizPower(POWER_VISIBLE)) {
        if (!isPlayerAction(PLR_STEALTH))
          addPlayerAction(PLR_STEALTH);

        if (getInvisLevel() <= MAX_MORT)
          setInvisLevel(GOD_LEVEL1);
      }
    }
  } else {
    if (!banished()) {
      if (in_room >= 0) {
        rp = real_roomp(in_room);
        in_room = ROOM_NOWHERE;  // change it so it doesn't error in +=
        *rp += *this;
        player.hometown = in_room;
      } else {
        rp = real_roomp(ROOM_CS);
        in_room = ROOM_NOWHERE;  // change it so it doesn't error in +=
        *rp += *this;
        player.hometown = ROOM_CS;
      }
    } else {
      rp = real_roomp(ROOM_HELL);
      in_room = ROOM_NOWHERE;  // change it so it doesn't error in +=
      *rp += *this;
      player.hometown = ROOM_HELL;
    }
  }

  act("$n steps in from another world.", TRUE, this, 0, 0, TO_ROOM);
  desc->connected = CON_PLYNG;

  // must be after char is placed in valid room
  loadFollowers();
  loadCareerStats();
  loadDrugStats();

  stats.logins++;
  save_game_stats();

  return FALSE;
}

void Descriptor::go_back_menu(connectStateT con_state)
{
  switch (con_state) {
    case CON_DELCHAR:
// character is NULL, do this another way.
//      character->cls();
      writeToQ("Which do you want to do?\n\r");
      writeToQ("1) Delete your account\n\r");
      writeToQ("2) Delete a character in your account\n\r");
      writeToQ("3) Return to main account menu.\n\r-> ");
      connected = CON_DELETE;
      break;
    case CON_QCLASS:
      sendHomeList();
      switch (character->getRace()) {
        case RACE_HUMAN:
          connected = CON_HOME_HUMAN;
          break;
        case RACE_ELVEN:
          connected = CON_HOME_ELF;
          break;
        case RACE_DWARF:
          connected = CON_HOME_DWARF;
          break;
        case RACE_GNOME:
          connected = CON_HOME_GNOME;
          break;
        case RACE_OGRE:
          connected = CON_HOME_OGRE;
          break;
        case RACE_HOBBIT:
          connected = CON_HOME_HOBBIT;
          break;
        default:
          forceCrash("Unknown race");
          connected = CON_HOME_HUMAN;
          break;
      }
      break;
#if 0
      sendRaceList();
      connected = CON_QRACE;
      break;
#endif
    case CON_QRACE:
      character->cls();
      writeToQ("Now you get to pick your handedness. The hand you pick\n\r");
      writeToQ("as your primary hand will be the strongest, and be able to\n\r")
;
      writeToQ("do more things than your secondary hand.\n\r");
      writeToQ("To go back a menu type '/'.\n\r");
      writeToQ("To disconnect type '~'.\n\r");
      writeToQ("\n\rPick your primary hand : 1) Right  2) Left\n\r--> ");
      connected = CON_QHANDS;
      break;
    case CON_STAT_COMBAT:
      character->cls();
      connected = CON_STATS_START;
      sendStartStatList();
      break;
    case CON_HOME_HUMAN:
    case CON_HOME_ELF:
    case CON_HOME_DWARF:
    case CON_HOME_GNOME:
    case CON_HOME_OGRE:
    case CON_HOME_HOBBIT:
      sendRaceList();
      connected = CON_QRACE;
      break;
    case CON_STAT_LEARN:
      sendStatList(1, TRUE);
      connected = CON_STAT_COMBAT;
      break;
    case CON_STAT_UTIL:
      sendStatList(2, TRUE);
      connected = CON_STAT_LEARN;
      break;
    case CON_QHANDS:
      character->cls();
      writeToQ("Now you choose your gender.\n\r");
      writeToQ("What is your gender?\n\r1. Male\n\r2. Female\n\r-> ");
      connected = CON_QSEX;
      break;
    case CON_CREATE_DONE:
      sendStatList(3, TRUE);
      connected = CON_STAT_UTIL;
      break;
    case CON_ENTER_DONE:
      sendStartStatList();
      connected = CON_STATS_START;
      break;
    case CON_STATS_START:
      character->cls();
      connected = CON_QCLASS;
      sendClassList(FALSE);
      break;
    case CON_PLYNG:
    case CON_NME:
    case CON_NMECNF:
    case CON_PWDNRM:
    case CON_PWDCNF:
    case CON_QSEX:
    case CON_RMOTD:
    case CON_PWDNCNF:
    case CON_WIZLOCK:
    case CON_DELETE:
    case CON_DISCON:
    case CON_NEWACT:
    case CON_ACTPWD:
    case CON_NEWLOG:
    case CON_NEWACTPWD:
    case CON_EMAIL:
    case CON_TERM:
    case CON_CONN:
    case CON_NEWPWD:
    case CON_OLDPWD:
    case CON_RETPWD:
    case CON_ACTDELCNF:
    case CON_EDITTING:
    case CON_DISCLAIMER:
    case CON_TIME:
    case CON_CHARDELCNF:
    case CON_DISCLAIMER2:
    case CON_DISCLAIMER3:
    case CON_WIZLOCKNEW:
    case CON_STATS_RULES:
    case CON_STATS_RULES2:
    case CON_LISTSERV:
    case MAX_CON_STATUS:
    case CON_REDITING:
    case CON_OEDITING:
    case CON_MEDITING:
    case CON_SEDITING:
    case CON_HELP:
    case CON_WRITING:
      vlogf(10, "Bad connected state in go_back_menu() [%d], BUG BRUTIUS!!!!", con_state);
      break;
  }
}

void Descriptor::EchoOn()
{
  if (client)
    return;

  char echo_on[6] = {IAC, WONT, TELOPT_ECHO, '\n', '\r', '\0'};

  write(socket->sock, echo_on, 6);
}

void Descriptor::EchoOff()
{
  if (client)
    return;

  char echo_off[4] = {IAC, WILL, TELOPT_ECHO, '\0'};

  write(socket->sock, echo_off, 4);
}

void Descriptor::sendHomeList()
{
  character->cls();
  switch (character->getRace()) {
    case RACE_ELVEN:
      writeToQ("You are now an elf.\n\r\n\r");
      writeToQ("Elves are the oldest race on the face of The World and their tribes can\n\r");
      writeToQ("be found in almost every clime where nature can be found:\n\r");
      writeToQ("\n\r");
      writeToQ("Pick your territory:\n\r");
      writeToQ("\n\r");
      writeToQ("1.) Urban\n\r");
      writeToQ("2.) Tribes Elf\n\r");
      writeToQ("3.) Plains Elf\n\r");
      writeToQ("4.) Snow Elf\n\r");
      writeToQ("5.) Wood Elf\n\r");
      writeToQ("6.) Sea Elf\n\r");
      writeToQ("7.) Recluse\n\r");
      writeToQ("\n\r");
      writeToQ("?.) Help on choices\n\r");
      writeToQ("/.) Go back one screen\n\r");
      break;
    case RACE_DWARF:
      writeToQ("You are now a dwarf.\n\r\n\r");
      writeToQ("Dwarves are an ancient race and are one of the linchpins of the Civilized\n\r");
      writeToQ("Races.  They have adapted to a variety of territories, and favor those with\n\r");
      writeToQ("good mineral resources:\n\r");
      writeToQ("\n\r");
      writeToQ("Pick your territory:\n\r");
      writeToQ("\n\r");
      writeToQ("1.) Urban\n\r");
      writeToQ("2.) Villager\n\r");
      writeToQ("3.) Hill Dwarf\n\r");
      writeToQ("4.) Mountain Dwarf\n\r");
      writeToQ("5.) Recluse\n\r");
      writeToQ("\n\r");
      writeToQ("?.) Help on choices\n\r");
      writeToQ("/.) Go back one screen\n\r");
      break;
    case RACE_OGRE:
      writeToQ("UGH!  Me Ogre!\n\r\n\r");
      writeToQ("Ogre big mighty race!  Most powerful of ALL Civilized Races!\n\r");
      writeToQ("Most ogres still wild!  Some ogres got smarts and joined Civilized Races!\n\r");
      writeToQ("Not many ogres Civilized!  Ogres not Civilized that long!\n\r");
      writeToQ("Me want to be:\n\r");
      writeToQ("\n\r");
      writeToQ("1.) Villager\n\r");
      writeToQ("2.) Plains Ogre\n\r");
      writeToQ("3.) Hill Ogre\n\r");
      writeToQ("\n\r");
      writeToQ("?.) Help on choices\n\r");
      writeToQ("/.) Go back one screen\n\r");
      break;
    case RACE_GNOME:
      writeToQ("You are now a gnome.\n\r\n\r");
      writeToQ("Gnomes are a close knit society, but are more than happy to be a member\n\r");
      writeToQ("of the Civilized Races.  They tend to expand their large communal enclaves\n\r");
      writeToQ("rather than find new homes.  This has hampered their territorial expansion.\n\r");
      writeToQ("\n\r");
      writeToQ("Pick your territory:\n\r");
      writeToQ("\n\r");
      writeToQ("1.) Urban\n\r");
      writeToQ("2.) Villager\n\r");
      writeToQ("3.) Hill Gnome\n\r");
      writeToQ("4.) Swamp Gnome\n\r");
      writeToQ("\n\r");
      writeToQ("?.) Help on choices\n\r");
      writeToQ("/.) Go back one screen\n\r");
      break;
    case RACE_HOBBIT:
      writeToQ("You are now a hobbit.\n\r\n\r");
      writeToQ("Hobbits have long been favored by a need to travel, and frequently\n\r");
      writeToQ("will settle down afterwards in one of the places they found in their\n\r");
      writeToQ("wanderings.  This has meant there are a large number of territorial choices\n\r");
      writeToQ("for hobbits to choose from:\n\r");
      writeToQ("\n\r");
      writeToQ("Pick your territory:\n\r");
      writeToQ("\n\r");
      writeToQ("1.) Urban\n\r");
      writeToQ("2.) Shire Hobbit\n\r");
      writeToQ("3.) Grassland Hobbit\n\r");
      writeToQ("4.) Hill Hobbit\n\r");
      writeToQ("5.) Woodland Hobbit\n\r");
      writeToQ("6.) Maritime Hobbit\n\r");
      writeToQ("\n\r");
      writeToQ("?.) Help on choices\n\r");
      writeToQ("/.) Go back one screen\n\r");
      break;
    case RACE_HUMAN:
    default:
      writeToQ("You are now a human.\n\r\n\r");
      writeToQ("Humans form the largest contingent of all the Civilized Races.\n\r");
      writeToQ("As such, they have expanded into all corners or The World and\n\r");
      writeToQ("a wide variety of territorial choices exist:\n\r");
      writeToQ("\n\r");
      writeToQ("Pick your territory:\n\r");
      writeToQ("\n\r");
      writeToQ("1.) Urban\n\r");
      writeToQ("2.) Villager\n\r");
      writeToQ("3.) Plainsfolk\n\r");
      writeToQ("4.) Recluse\n\r");
      writeToQ("5.) Hillsperson\n\r");
      writeToQ("6.) Mountaineer\n\r");
      writeToQ("7.) Forester\n\r");
      writeToQ("8.) Mariner\n\r");
      writeToQ("\n\r");
      writeToQ("?.) Help on choices\n\r");
      writeToQ("/.) Go back one screen\n\r");
      break;
      //    default:
      //      forceCrash("crash");
  }
  writeToQ("\n\r--> ");
  return;
}

void Descriptor::sendStartStatList()
{
  char buf[256];

  character->cls();
  writeToQ("\n\rCongratulations, you have finished the basic character creation process.\n\r\n\rThe only remaining step is to determine your character's characteristics.\n\r");

  sprintf(buf, "%s uses a unique system of characteristics.  In essence though, based\n\r", MUD_NAME);
  writeToQ(buf);

  writeToQ("on your character's race, characteristics have already been assigned to\n\ryour character.  You are free to adjust these characteristics somewhat above\n\ror below the racial average to suit your particular likings, or, you may\n\rbypass this step, and proceed right into the game.\n\r\n\r");

  sprintf(buf, "%sThis is the only opportunity you will have to adjust these characteristics.%s\n\r\n\r", orange(), norm());
  writeToQ(buf);

  sprintf(buf, "Choose:\n\r(%sE%s)%snter the game%s accepting the characteristics assigned by race, or\n\r",
        cyan(), norm(), cyan(), norm());
  writeToQ(buf);

  sprintf(buf, "(%sC%s)%sustomize your characteristics%s.\n\r",
        cyan(), norm(), cyan(), norm());
  writeToQ(buf);

  sprintf(buf, "Or type '%s/%s' %sto go back to basic character creation%s.\n\r\n\r--> ",
        cyan(), norm(), cyan(), norm());
  writeToQ(buf);
}

void Descriptor::sendDoneScreen()
{
  char buf[256];

  character->cls();
  writeToQ("\n\rCongratulations, you have finished the character creation process.\n\r");
  sprintf(buf, "If you are a newcomer to %s, %stake a minute to read this screen%s.\n\r\n\r", MUD_NAME, orange(), norm());
  writeToQ(buf);

  writeToQ("Upon connecting, you will want to check your initial terminal options.\n\r");
  writeToQ("Know that the game will automatically set some of these options for you.\n\r");
  writeToQ("These include: prompts, automatic actions, terminal size, color.\n\r");
  writeToQ("Your initial settings are just defaults and you can change them easily.\n\r");
  sprintf(buf, "Good help files to read are <%sCOLOR%s>, <%sPROMPTS%s> and <%sAUTO%s>.\n\r\n\r", orange(), norm(), orange(), norm(), orange(), norm());
  writeToQ(buf);
  sprintf(buf, "You should also %sread the newbie guide%s and %swear your equipment%s.\n\r", orange(), norm(), orange(), norm());
  writeToQ(buf);

  writeToQ("For further orientation, use the help system, newbie helpers and immortal staff.\n\r");
  writeToQ("In contacting immortals, be aware that our immortal staff is not\n\r");
  writeToQ("allowed to help you discover The World.  However, they are allowed\n\r");
  writeToQ("and encouraged to help you with command problems and general orientation.\n\r\n\r");

  sprintf(buf, "The gods and implementors of %s hope that you enjoy your stay.\n\r", MUD_NAME);
  writeToQ(buf);
  writeToQ("You may also wish to check out our mud client, web site and listserver.\n\r\n\r");
  sprintf(buf, "When you are ready, (%sE%s)%snter%s the game or '%s/%s' %sto go back a menu%s.\n\r\n\r--> ",
        cyan(), norm(), cyan(), norm(), cyan(), norm(), cyan(), norm());
  writeToQ(buf);
}

const char *Descriptor::getStatDescription(int local_stat)
{
  if (local_stat <= -21) {
    return "very low";
  } else if (local_stat <= -16) {
    return "pretty low";
  } else if (local_stat <= -10) {
    return "below average";
  } else if (local_stat <= -4) {
    return "slightly below average";
  } else if (local_stat <= 3) {
    return "average";
  } else if (local_stat <= 9) {
    return "slightly above average";
  } else if (local_stat <= 15) {
    return "above average";
  } else if (local_stat <= 20) {
    return "pretty high";
  } else if (local_stat <= 25) {
    return "very high";
  } else {
    return "ERROR/REPORT";
  }
}
 
void Descriptor::sendStatRules(int num)
{
  char buf[1024];

  if (num == 1) {
    sprintf(buf, "Welcome to the %s characteristic's customization process.\n\r\n\r", MUD_NAME);
    writeToQ(buf);
    writeToQ("Please remember the following as you customize your characteristics.  First,\n\rall characteristics are merely modifiers on preselected racial norms.\n\rThat is, if you elected to be an ogre, you start off with an ogre's\n\rcharacteristics (strength, brawn, intellect, etc).  Likewise, those who\n\relected to be elves, have the elven norm as their starting point.\n\r\n\r");
    writeToQ("REALIZE THAT A ZERO STAT IS YOUR RACIAL NORM.\n\r\n\r");
  } else if (num == 2) {
    sprintf(buf, "Characteristics on %s are separated into three sections, physical,\n\rmental, and utility.  Each section has 3-5 characteristics.  You are free\n\rto move points between characteristics in the same section, but you may not\n\rmove points between characteristics in different sections.  Additionally, no\n\rcharacteristic may be raised or lowered more than 25 points above or below\n\rthe racial norm.\n\r\n\r", MUD_NAME);
    writeToQ(buf);
    writeToQ("The customization process begins on the following screens.\n\r\n\r");
    writeToQ("To change any characteristic, you may do ('+', '-'){amount}(characteristic).\n\rWhere {amount} represents the number of points of the (characteristic)\n\rto change, and (characteristic) represents the letter corresponding to the\n\rcharacteristic.\n\r\n\r");
    writeToQ("For example, +3s would raise your (s)trength by 3 points (in the physical\n\rcharacteristic section).  To simplify the customization process, you may\n\rhave negative free points, however you will not be able to proceed onward\n\rwith a negative number of free points.\n\r\n\r");
  }

  writeToQ("[Press Return to continue]\n\r");
  return;
}

void Descriptor::sendStatList(int group, int)
{
  char buf[1024];
  char buf1[80];
  char buf2[80];
  char pc_race[80];
  int stat1, stat2;
  int free_stat;

  // get the race description
  strcpy(pc_race, character->getMyRace()->getSingularName().c_str());

  switch (group) {
    case 1:
      character->cls();
      writeToQ("Your current physical characteristics are: \n\r\n\r");
      stat1 = character->chosenStats.get(STAT_STR);
      stat2 = character->chosenStats.get(STAT_BRA);
      strcpy(buf1, getStatDescription(stat1));
      strcpy(buf2, getStatDescription(stat2));
      if (account->term == TERM_ANSI) {
        sprintf(buf,"(%sS%s)%strength%s       [%3d] (%s%s %s%s)\n\r(%sB%s)%srawn%s          [%3d] (%s%s %s%s)\n\r", 
          cyan(), norm(), cyan(), norm(), stat1, 
          orange(), buf1, pc_race, norm(),
          cyan(), norm(), cyan(), norm(), stat2,
          orange(), buf2, pc_race, norm());
      } else {
        sprintf(buf,"(S)trength       [%3d] (%s %s)\n\r(B)rawn          [%3d] (%s %s)\n\r", stat1, buf1, pc_race, stat2, buf2, pc_race);
      }
      writeToQ(buf);
      stat1 = character->chosenStats.get(STAT_CON);
      strcpy(buf1, getStatDescription(stat1));
      if (account->term == TERM_ANSI) {
        sprintf(buf,"(%sC%s)%sonstitution%s   [%3d] (%s%s %s%s)\n\r",
          cyan(), norm(), cyan(), norm(), stat1, 
          orange(), buf1, pc_race, norm());
      } else {
        sprintf(buf,"(C)onstitution   [%3d] (%s %s)\n\r", stat1, buf1, pc_race);
      }
      writeToQ(buf);
      stat1 = character->chosenStats.get(STAT_DEX);
      stat2 = character->chosenStats.get(STAT_AGI);
      strcpy(buf1, getStatDescription(stat1));
      strcpy(buf2, getStatDescription(stat2));
      if (account->term == TERM_ANSI) {
        sprintf(buf,"(%sD%s)%sexterity%s      [%3d] (%s%s %s%s)\n\r(%sA%s)%sgility%s        [%3d] (%s%s %s%s)\n\r\n\r", 
          cyan(), norm(), cyan(), norm(), stat1, 
          orange(), buf1, pc_race, norm(),
          cyan(), norm(), cyan(), norm(), stat2,
          orange(), buf2, pc_race, norm());
      } else {
        sprintf(buf,"(D)exterity      [%3d] (%s %s)\n\r(A)gility        [%3d] (%s %s)\n\r\n\r", stat1, buf1, pc_race, stat2, buf2, pc_race);
      }
      writeToQ(buf);

      writeToQ("(S)trength affects your ability to manipulate weight and your combat damage.\n\r");
      writeToQ("(B)rawn affects your ability to wear armor and your hardiness.\n\r");
      writeToQ("(C)onstitution affects your endurance and your life force.\n\r");
      writeToQ("(D)exterity affects volume manipulation and offensive skill abilities.\n\r");
      writeToQ("(A)gility affects your defensive combat abilities.\n\r\n\r");

      free_stat = (0 - ((character->chosenStats.values[STAT_STR]) +
                 (character->chosenStats.values[STAT_DEX]) +
                 (character->chosenStats.values[STAT_AGI]) +
                 (character->chosenStats.values[STAT_CON]) +
                 (character->chosenStats.values[STAT_BRA])));
      sprintf(buf, "You have %d free physical stat points.\n\r\n\r", free_stat);
      writeToQ(buf);
      if (account->term == TERM_ANSI) {
        sprintf(buf, "To go back to the previous menu, %stype%s '%s/%s'.\n\r",
             cyan(), norm(), cyan(), norm());
        writeToQ(buf);
        sprintf(buf, "To disconnect, %stype%s '%s~%s'.\n\r",
             cyan(), norm(), cyan(), norm());
        writeToQ(buf);
        sprintf(buf, "For help, %stype%s '%s?%s' or (%sH%s)%selp%s.\n\r",
             cyan(), norm(), cyan(), norm(),
             cyan(), norm(), cyan(), norm());
        writeToQ(buf);
        sprintf(buf, "(%sE%s)%snd%s when you are done customizing your physical characteristics.\n\r\n\r",
             cyan(), norm(), cyan(), norm());
        writeToQ(buf);
      } else {
        writeToQ("To go back to the previous menu, type '/'.\n\r");
        writeToQ("To disconnect, type '~'.\n\r");
        writeToQ("For help, type '?' or (H)elp.\n\r");
        writeToQ("(E)nd when you are done customizing your physical characteristics.\n\r\n\r");
      }
      writeToQ("--> ");
      break;
    case 2:
      character->cls();
      writeToQ("Your current mental characteristics are: \n\r\n\r");
      stat1 = character->chosenStats.get(STAT_INT);
      strcpy(buf1, getStatDescription(stat1));
      if (account->term == TERM_ANSI) {
        sprintf(buf,"(%sI%s)%sntelligence%s   [%3d] (%s%s %s%s)\n\r",
          cyan(), norm(), cyan(), norm(), stat1, 
          orange(), buf1, pc_race, norm());
      } else {
        sprintf(buf,"(I)ntelligence   [%3d] (%s %s)\n\r", stat1, buf1,  pc_race);
      }
      writeToQ(buf);
      stat1 = character->chosenStats.get(STAT_WIS);
      strcpy(buf1, getStatDescription(stat1));
      if (account->term == TERM_ANSI) {
        sprintf(buf,"(%sW%s)%sisdom%s         [%3d] (%s%s %s%s)\n\r",
          cyan(), norm(), cyan(), norm(), stat1, 
          orange(), buf1, pc_race, norm());
      } else {
        sprintf(buf,"(W)isdom         [%3d] (%s %s)\n\r", stat1, buf1,  pc_race);
      }
      writeToQ(buf);

      stat1 = character->chosenStats.get(STAT_FOC);
      strcpy(buf1, getStatDescription(stat1));
      if (account->term == TERM_ANSI) {
        sprintf(buf,"(%sF%s)%socus%s          [%3d] (%s%s %s%s)\n\r\n\r",
          cyan(), norm(), cyan(), norm(), stat1, 
          orange(), buf1, pc_race, norm());
      } else {
        sprintf(buf,"(F)ocus          [%3d] (%s %s)\n\r\n\r", stat1, buf1,  pc_race);
      }
      writeToQ(buf);

      writeToQ("(I)ntelligence affects your maximum total learning.\n\r");

      writeToQ("(W)isdom affects how fast you will learn your skills.\n\r");
      writeToQ("(F)ocus affects the success rate of your skills.\n\r\n\r");

      free_stat = (0 - ((character->chosenStats.values[STAT_INT]) +
                 (character->chosenStats.values[STAT_FOC]) +
                 (character->chosenStats.values[STAT_WIS])));
      sprintf(buf, "You have %d free mental stat points.\n\r\n\r", free_stat);
      writeToQ(buf);
      if (account->term == TERM_ANSI) {
        sprintf(buf, "To go back to the previous menu, %stype%s '%s/%s'.\n\r",
             cyan(), norm(), cyan(), norm());
        writeToQ(buf);
        sprintf(buf, "To disconnect, %stype%s '%s~%s'.\n\r",
             cyan(), norm(), cyan(), norm());
        writeToQ(buf);
        sprintf(buf, "For help, %stype%s '%s?%s' or (%sH%s)%selp%s.\n\r",
             cyan(), norm(), cyan(), norm(),
             cyan(), norm(), cyan(), norm());
        writeToQ(buf);
        sprintf(buf, "(%sE%s)%snd%s when you are done customizing your mental characteristics.\n\r\n\r",
             cyan(), norm(), cyan(), norm());
        writeToQ(buf);
      } else {
        writeToQ("To go back to the previous menu, type '/'.\n\r");
        writeToQ("To disconnect, type '~'.\n\r");
        writeToQ("For help, type '?' or (H)elp.\n\r");
        writeToQ("(E)nd when you are done customizing your mental characteristics.\n\r\n\r");
      }
      writeToQ("--> ");
      break;
    case 3:
      character->cls();
      writeToQ("Your current utility characteristics are: \n\r\n\r");
      stat1 = character->chosenStats.get(STAT_PER);
      strcpy(buf1, getStatDescription(stat1));
      if (account->term == TERM_ANSI) {
        sprintf(buf,"(%sP%s)%serception%s     [%3d] (%s%s %s%s)\n\r", 
          cyan(), norm(), cyan(), norm(), stat1, 
          orange(), buf1, pc_race, norm());
      } else {
        sprintf(buf,"(P)erception     [%3d] (%s %s)\n\r", stat1, buf1,  pc_race);
      }
      writeToQ(buf);

      stat1 = character->chosenStats.get(STAT_CHA);
      strcpy(buf1, getStatDescription(stat1));
      if (account->term == TERM_ANSI) {
        sprintf(buf,"(%sC%s)%sharisma%s       [%3d] (%s%s %s%s)\n\r", 
          cyan(), norm(), cyan(), norm(), stat1, 
          orange(), buf1, pc_race, norm());
      } else {
        sprintf(buf,"(C)harisma       [%3d] (%s %s)\n\r", stat1, buf1,  pc_race);
      }
      writeToQ(buf);

      stat1 = character->chosenStats.get(STAT_KAR);
      strcpy(buf1, getStatDescription(stat1));
      if (account->term == TERM_ANSI) {
        sprintf(buf,"(%sK%s)%sarma%s          [%3d] (%s%s %s%s)\n\r", 
          cyan(), norm(), cyan(), norm(), stat1, 
          orange(), buf1, pc_race, norm());
      } else {
        sprintf(buf,"(K)arma          [%3d] (%s %s)\n\r", stat1, buf1,  pc_race);
      }
      writeToQ(buf);

      stat1 = character->chosenStats.get(STAT_SPE);
      strcpy(buf1, getStatDescription(stat1));
      if (account->term == TERM_ANSI) {
        sprintf(buf,"(%sS%s)%speed%s          [%3d] (%s%s %s%s)\n\r\n\r", 
          cyan(), norm(), cyan(), norm(), stat1, 
          orange(), buf1, pc_race, norm());
      } else {
        sprintf(buf,"(S)peed          [%3d] (%s %s)\n\r\n\r", stat1, buf1,  pc_race);
      }
      writeToQ(buf);

      writeToQ("(P)erception affects your abilities to see and evaluate.\n\r");
      writeToQ("(C)harisma affects your ability to lead others including control pets.\n\r");
      writeToQ("(K)arma affects your luck.\n\r");
      writeToQ("(S)peed affects how fast you are able to do things.\n\r\n\r");
      free_stat = (0 - ((character->chosenStats.values[STAT_PER]) +
                 (character->chosenStats.values[STAT_KAR]) +
                 (character->chosenStats.values[STAT_CHA]) +
                 (character->chosenStats.values[STAT_SPE])));
      sprintf(buf, "You have %d free utility stat points.\n\r\n\r", free_stat);
      writeToQ(buf);
      if (account->term == TERM_ANSI) {
        sprintf(buf, "To go back to the previous menu, %stype%s '%s/%s'.\n\r",
             cyan(), norm(), cyan(), norm());
        writeToQ(buf);
        sprintf(buf, "To disconnect, %stype%s '%s~%s'.\n\r",
             cyan(), norm(), cyan(), norm());
        writeToQ(buf);
        sprintf(buf, "For help, %stype%s '%s?%s' or (%sH%s)%selp%s.\n\r",
             cyan(), norm(), cyan(), norm(),
             cyan(), norm(), cyan(), norm());
        writeToQ(buf);
        sprintf(buf, "(%sE%s)%snd%s when you are done customizing your utility characteristics.\n\r\n\r",
             cyan(), norm(), cyan(), norm());
        writeToQ(buf);
      } else {
        writeToQ("To go back to the previous menu, type '/'.\n\r");
        writeToQ("To disconnect, type '~'.\n\r");
        writeToQ("For help, type '?' or (H)elp.\n\r");
        writeToQ("(E)nd when you are done customizing your utility characteristics.\n\r\n\r");
      }
      writeToQ("--> ");
      break;
    default:
      break;
  }
  return;
}

void Descriptor::sendRaceList()
{
  char buf[1024];

  *buf = '\0';

  character->cls();
  strcpy(buf, "Please pick one of the following races.\n\r\n\r");

  sprintf(buf + strlen(buf), "1. Human                     2. Gnome\n\r");
  sprintf(buf + strlen(buf), "3. Elf                       4. Ogre\n\r");
  sprintf(buf + strlen(buf), "5. Dwarf                     6. Hobbit\n\r\n\r");
  sprintf(buf + strlen(buf), "The choice of race is very important on %s.\n\r", MUD_NAME);
  strcat(buf, "There are advantages and disadvantages to each choice.\n\r");
  strcat(buf, "Among other factors, race will majorly affect your stats,\n\r");
  strcat(buf, "immunities, racial skills, and the supply of available equipment.\n\r");
  sprintf(buf + strlen(buf), "Type %s?%s to see a help file telling you these advantages and disadvantages.\n\r",
          red(), norm());
  sprintf(buf + strlen(buf), "Type %s/%s to go back a menu to redo things.\n\r",
          red(), norm());
  sprintf(buf + strlen(buf), "Type %s~%s to disconnect.\n\r\n\r--> ",
          red(), norm());

  writeToQ(buf);
}

static const char CCC(Descriptor *d, int Class, int multi = FALSE, int triple = FALSE)
{
  return (d->canChooseClass(Class, multi, triple) ? 'X' : ' ');
}

void Descriptor::sendClassList(int home)
{
  char buf[MAX_STRING_LENGTH];

  *buf = '\0';

  if (home) {
    writeToQ("Now you get to select your class.\n\r\n\r");
  }



  strcpy(buf, "Please pick one of the following combinations for your class.\n\r");
  strcat(buf, "An X in front of the selection means that you can pick this class.\n\r");
  strcat(buf, "If there is no X, for some reason you can't choose the class(es).\n\r");
  strcat(buf, "To see why you can't choose a selection, choose it and you will be\n\r");
  strcat(buf, "given an error message telling you why you cannot select the class(es).\n\r\n\r");
  sprintf(buf + strlen(buf), "[%c] 1. Warrior                  [%c] A. Warrior/Thief\n\r", CCC(this, CLASS_WARRIOR), CCC(this, CLASS_WARRIOR | CLASS_THIEF, TRUE));
  sprintf(buf + strlen(buf), "[%c] 2. Cleric                   [%c] B. Warrior/Cleric\n\r", CCC(this, CLASS_CLERIC), CCC(this, CLASS_WARRIOR | CLASS_CLERIC, TRUE));
  sprintf(buf + strlen(buf), "[%c] 3. Mage                     [%c] C. Thief/Mage\n\r", CCC(this, CLASS_MAGIC_USER), CCC(this, CLASS_THIEF | CLASS_MAGIC_USER, TRUE));
  sprintf(buf + strlen(buf), "[%c] 4. Thief                    [%c] D. Warrior/Mage\n\r", CCC(this, CLASS_THIEF), CCC(this, CLASS_WARRIOR | CLASS_MAGIC_USER, TRUE));
  sprintf(buf + strlen(buf), "[%c] 5. Deikhan                  [%c] E. Thief/Cleric\n\r", CCC(this, CLASS_DEIKHAN), CCC(this, CLASS_THIEF | CLASS_CLERIC, TRUE));
  sprintf(buf + strlen(buf), "[%c] 6. Monk                     [%c] F. Warrior/Thief/Cleric\n\r", CCC(this, CLASS_MONK), CCC(this, CLASS_WARRIOR | CLASS_THIEF | CLASS_CLERIC, FALSE, TRUE));
  sprintf(buf + strlen(buf), "[%c] 7. Ranger                   [%c] G. Warrior/Thief/Mage\n\r", CCC(this, CLASS_RANGER), CCC(this, CLASS_WARRIOR | CLASS_THIEF | CLASS_MAGIC_USER, FALSE, TRUE));
  sprintf(buf + strlen(buf), "[%c] 8. Shaman\n\r\n\r",
          CCC(this, CLASS_SHAMAN));
  strcat(buf, "There are advantages and disadvantages to each choice.\n\r");
  sprintf(buf + strlen(buf), "Type %s?%s to see a help file telling you these advantages and disadvantages.\n\r",
          red(), norm());
  sprintf(buf + strlen(buf), "Type %s/%s to go back a menu to redo things.\n\r",
          red(), norm());
  sprintf(buf + strlen(buf), "Type %s~%s to disconnect.\n\r\n\r--> ",
          red(), norm());

  writeToQ(buf);

  return;
}

static bool rangerTerrainDeny(TBeing *ch)
{
    if (ch->player.hometerrain == HOME_TER_HUMAN_URBAN ||
        ch->player.hometerrain == HOME_TER_HUMAN_VILLAGER ||
        ch->player.hometerrain == HOME_TER_HUMAN_MARINER ||
        ch->player.hometerrain == HOME_TER_DWARF_URBAN ||
        ch->player.hometerrain == HOME_TER_DWARF_VILLAGER ||
        ch->player.hometerrain == HOME_TER_GNOME_URBAN ||
        ch->player.hometerrain == HOME_TER_GNOME_VILLAGER ||
        ch->player.hometerrain == HOME_TER_HOBBIT_URBAN ||
        ch->player.hometerrain == HOME_TER_HOBBIT_SHIRE ||
        ch->player.hometerrain == HOME_TER_HOBBIT_MARITIME ||
        ch->player.hometerrain == HOME_TER_OGRE_VILLAGER ||
        ch->player.hometerrain == HOME_TER_ELF_TRIBE ||
        ch->player.hometerrain == HOME_TER_ELF_SEA ||
        ch->player.hometerrain == HOME_TER_ELF_URBAN)
    return TRUE;
  return FALSE;
}

bool Descriptor::canChooseClass(int Class, bool multi, bool triple)
{
  TBeing *ch;

  if (!(ch = character)) {
    vlogf(10, "Descriptor got to canChooseClass with no character!!! BUG BRUTIUS!");
    return FALSE;
  }

  if (multi) {
    if (!IS_SET(account->flags, ACCOUNT_ALLOW_DOUBLECLASS))
      return FALSE;
  }

  if (triple) {
    if (!IS_SET(account->flags, ACCOUNT_ALLOW_TRIPLECLASS))
      return FALSE;
  }

  if (Class & CLASS_MONK) {
    return TRUE;
  }

  if (Class & CLASS_WARRIOR) {
    return TRUE;
  }

  if (Class & CLASS_MAGIC_USER) {
    return TRUE;
  }

  if (Class & CLASS_CLERIC) {
    return TRUE;
  }

  if (Class & CLASS_SHAMAN) {
    return FALSE;
  }

  if (Class &CLASS_DEIKHAN) {
    return TRUE;
  }

  if (Class & CLASS_RANGER) {
    if (ch->getRace() == RACE_OGRE)
      return FALSE;
    if (rangerTerrainDeny(ch))
      return FALSE;
    return TRUE;
  }

  if (Class & CLASS_SHAMAN) {
    return TRUE;
  }

  if (Class & CLASS_THIEF) {
    return TRUE;
  }
  return FALSE;
}

bool Descriptor::start_page_file(const char *fpath, const char *errormsg)
{
  if (pagedfile) {
    delete [] pagedfile;
    pagedfile = NULL;
    cur_page = tot_pages = 0;
  }

  pagedfile = new char[strlen(fpath) + 1];
  strcpy(pagedfile, fpath);
  cur_page = 0;
  tot_pages = 0;

  if (!page_file("")) {      // couldn't open file, etc. 
    delete [] pagedfile;
    pagedfile = NULL;
    cur_page = 0;
    tot_pages = 0;
    writeToQ(errormsg);
    return FALSE;
  }
  return TRUE;
}

// page_file returns TRUE if something was paged, FALSE if nothing got sent 
// if (position) comes back < 0 then EOF was hit when outputing file.    
bool Descriptor::page_file(const char *the_input)
{
  FILE *fp;
  char buffer[256];
  int lines;
  int sent_something = FALSE;
  int lines_per_page;

  if (character)
    lines_per_page = ((character->ansi() || character->vt100()) ? (screen_size - 6) : (screen_size - 2));
  else
    lines_per_page = ((account->term == TERM_ANSI || account->term == TERM_VT100) ? (screen_size - 6) : (screen_size - 2));

  if (!pagedfile)
    return FALSE;

  if (!tot_pages) {
    lines = 0;
    if ((fp = fopen(pagedfile, "r"))) {
      while (fgets(buffer, 255, fp)) {
        lines++;
        continue;
      }
      if (lines_per_page <= 0) {
        writeToQ("Bad screensize set.\n\r");
        delete [] pagedfile;
        pagedfile = NULL;
        tot_pages = cur_page = 0;
        fclose(fp);
        return TRUE;
      }
      tot_pages = lines / lines_per_page;
      tot_pages += 1;
      cur_page = 0;
      fclose(fp);
    } else
      return FALSE;
  }
  one_argument(the_input, buffer);

  if (*buffer) {
    if (*buffer == 'r' || *buffer == 'R') {
      cur_page -= 1;
      cur_page = max((byte) 0, cur_page);
    } else if (*buffer == 'b' || *buffer == 'B') {
      cur_page -= 2;
      cur_page = max((byte) 0, cur_page);
    } else if (isdigit(*buffer)) {
      cur_page = max(min((int) tot_pages, atoi(buffer)), 1) - 1;
    } else {
      delete [] pagedfile;
      pagedfile = NULL;
      tot_pages = cur_page = 0;
      writeToQ("*** INTERRUPTED ***\n\r");
      return TRUE;
    }
  }

  cur_page += 1;
  if (!(fp = fopen(pagedfile, "r")))
    return FALSE;

  for (lines = 0; lines < (cur_page - 1) * lines_per_page; lines++) {
    if (!fgets(buffer, 255, fp)) {
      vlogf(5, "Error paging file: %s, %d", pagedfile, cur_page);
      delete [] pagedfile;
      pagedfile = NULL;
      cur_page = tot_pages = 0;
      fclose(fp);
      return FALSE;
    }
  }
  for (lines = 0; lines < lines_per_page; lines++) {
    if (fgets(buffer, 255, fp)) {
      writeToQ(buffer);
      writeToQ("\r");
      sent_something = TRUE;
    }
    if (feof(fp)) {
      fclose(fp);
      delete [] pagedfile;
      pagedfile = NULL;
      tot_pages = cur_page = 0;
      return sent_something;
    }
  }
  fclose(fp);
  return sent_something;
}

// show should be false, except in nanny() loop
// the prompt handler normmaly will show the "press return" crap
// allow will permit the string to parse for %n in the colorstring
// the chief problem with this is you can make a board message look like the
// reader's name is in it.  (it is default TRUE)
void Descriptor::page_string(const char *strs, int show, bool allow)
{
  delete [] showstr_head;
  showstr_head = mud_str_dup(strs);
  mud_assert(showstr_head != NULL, "Bad alloc in page_string");

  cur_page = 0;
  tot_pages = 0;

  show_string("", show, allow);
}

void Descriptor::show_string(const char *the_input, bool show, bool allow)
{
  // this will hold the text of the single page that we are on
  // theortically, it is no more than screen_length * 80, but color
  // codes extend it somewhat
  char buffer[MAX_STRING_LENGTH];

  char buf[MAX_INPUT_LENGTH];
  char *chk;
  int lines = 0, toggle = 1;
  int lines_per_page;
  int i;

  if (account)
    lines_per_page = ((account->term == TERM_ANSI || account->term == TERM_VT100) ? (screen_size - 6) : (screen_size - 2));
  else
    lines_per_page = 20;

  // runs only once to initialize values
  if (!tot_pages) {
    toggle = 1;
    lines = 0;
    for (chk = showstr_head; *chk; chk++) {
      if ((*chk == '\n' || *chk == '\r') && ((toggle = -toggle) < 0)) {
        lines++;
        toggle = 0;
        continue;
      }
      if (!toggle) 
        toggle = 1;
    }
    if (lines_per_page <= 0) {
      writeToQ("Bad screensize set.\n\r");
      delete [] showstr_head;
      showstr_head = NULL;
      tot_pages = cur_page = 0;
      return;
    }
    tot_pages = lines / lines_per_page;
    tot_pages += 1;
    cur_page = 0;
  }
  one_argument(the_input, buf);

  if (*buf) {
    if (*buf == 'r' || *buf == 'R') {
      cur_page -= 1;
      cur_page = max((byte) 0, cur_page);
    } else if (*buf == 'b' || *buf == 'B') {
      cur_page -= 2;
      cur_page = max((byte) 0, cur_page);
    } else if (isdigit(*buf)) 
      cur_page = max(min((int) tot_pages, atoi(buf)), 1) - 1;
    else {
      if (showstr_head) {
        delete [] showstr_head;
        showstr_head = NULL;
        tot_pages = cur_page = 0;
      }
      return;
    } 
  }
  toggle = 1;
  lines = 0;
  cur_page++;
  i = 0;
  *buffer = '\0';
  for (chk = showstr_head; *chk; chk++) {
    if ((*chk == '\n' || *chk == '\r') && ((toggle = -toggle) < 0)) {
      lines++;
      if ((lines/lines_per_page + 1) == cur_page)
        buffer[i++] = *chk;
      continue;
    }
    if ((lines/lines_per_page + 1) > cur_page)
      break;
    if ((lines/lines_per_page + 1) < cur_page)
      continue;
    if (!i && isspace(*chk))
      continue;
      
    // some redundant code from colorString()
    if ((*chk == '<') && (*(chk +2) == '>')) {
      if (chk != showstr_head && *(chk-1) == '<') {
        // encountered double <<, so skip 1 of them, 
        // we've already written the first
        continue;
      }
      switch (*(chk + 1)) {
        case 'n':
        case 'N':
          if (allow) {
            strcpy(buffer + i, character->getName());
            i += strlen(character->getName());
            chk += 2;
          } else {
            buffer[i++] = *chk;
          }
          break;
        case 'h':
          strcpy(buffer + i, MUD_NAME);
          i+= strlen(MUD_NAME);
          chk += 2;
          break;
        case 'H':
          strcpy(buffer + i, MUD_NAME_VERS);
          i+= strlen(MUD_NAME);
          chk += 2;
          break;
        case 'R':
          strcpy(buffer + i, redBold());
          i += strlen(redBold());
          chk += 2;
          break;
        case 'r':
          strcpy(buffer + i, red());
          i += strlen(red());
          chk += 2;
          break;
        case 'G':
          strcpy(buffer + i, greenBold());
          i += strlen(greenBold());
          chk += 2;
          break;
        case 'g':
          strcpy(buffer + i, green());
          i += strlen(green());
          chk += 2;
          break;
        case 'Y':
        case 'y':
          strcpy(buffer + i, orangeBold());
          i += strlen(orangeBold());
          chk += 2;
          break;
        case 'O':
        case 'o':
          strcpy(buffer + i, orange());
          i += strlen(orange());
          chk += 2;
          break;
        case 'B':
          strcpy(buffer + i, blueBold());
          i += strlen(blueBold());
          chk += 2;
          break;
        case 'b':
          strcpy(buffer + i, blue());
          i += strlen(blue());
          chk += 2;
          break;
        case 'P':
          strcpy(buffer + i, purpleBold());
          i += strlen(purpleBold());
          chk += 2;
          break;
        case 'p':
          strcpy(buffer + i, purple());
          i += strlen(purple());
          chk += 2;
          break;
        case 'C':
          strcpy(buffer + i, cyanBold());
          i += strlen(cyanBold());
          chk += 2;
          break;
        case 'c':
          strcpy(buffer + i, cyan());
          i += strlen(cyan());
          chk += 2;
          break;
        case 'W':
          strcpy(buffer + i, whiteBold());
          i += strlen(whiteBold());
          chk += 2;
          break;
        case 'w':
          strcpy(buffer + i, white());
          i += strlen(white());
          chk += 2;
          break;
        case 'K':
          strcpy(buffer + i, black());
          i += strlen(black());
          chk += 2;
          break;
        case 'k':
          strcpy(buffer + i, blackBold());
          i += strlen(blackBold());
          chk += 2;
          break;
        case 'A':
          strcpy(buffer + i, underBold());
          i += strlen(underBold());
          chk += 2;
          break;
        case 'a':
          strcpy(buffer + i, under());
          i += strlen(under());
          chk += 2;
          break;
        case 'D':
        case 'd':
          strcpy(buffer + i, bold());
          i += strlen(bold());
          chk += 2;
          break;
        case 'F':
        case 'f':
          strcpy(buffer + i, flash());
          i += strlen(flash());
          chk += 2;
          break;
        case 'I':
        case 'i':
          strcpy(buffer + i, invert());
          i += strlen(invert());
          chk += 2;
          break;
        case 'E':
        case 'e':
          strcpy(buffer + i, BlackOnWhite());
          i += strlen(BlackOnWhite());
          chk += 2;
          break;
        case 'j':
        case 'J':
          strcpy(buffer + i, BlackOnBlack());
          i += strlen(BlackOnBlack());
          chk += 2;
          break;
        case 'L':
        case 'l':
          strcpy(buffer + i, WhiteOnRed());
          i += strlen(WhiteOnRed());
          chk += 2;
          break;
        case 'Q':
        case 'q':
          strcpy(buffer + i, WhiteOnGreen());
          i += strlen(WhiteOnGreen());
          chk += 2;
          break;
        case 'T':
        case 't':
          strcpy(buffer + i, WhiteOnOrange());
          i += strlen(WhiteOnOrange());
          chk += 2;
          break;
        case 'U':
        case 'u':
          strcpy(buffer + i, WhiteOnBlue());
          i += strlen(WhiteOnBlue());
          chk += 2;
          break;
        case 'V':
        case 'v':
          strcpy(buffer + i, WhiteOnPurple());
          i += strlen(WhiteOnPurple());
          chk += 2;
          break;
        case 'X':
        case 'x':
          strcpy(buffer + i, WhiteOnCyan());
          i += strlen(WhiteOnCyan());
          chk += 2;
          break;
        case 'Z':
        case 'z':
        case '1':
          strcpy(buffer + i, norm());
          i += strlen(norm());
          chk += 2;
          break;
        default:
          buffer[i++] = *chk;
          break;
      }
    } else
      buffer[i++] = *chk;
  }
  if (!i || cur_page == tot_pages) {
    delete [] showstr_head;
    showstr_head = NULL;
    cur_page = tot_pages = 0;
  }
  buffer[i] = '\0';
  strcat(buffer, norm());

  if (show) {
    if (tot_pages) {
      sprintf(buffer + strlen(buffer),
         "\n\r[ %sReturn%s to continue, %s(r)%sefresh, %s(b)%sack, page %s(%d/%d)%s, or %sany other key%s to quit ]\n\r", 
            green(),  norm(),
            green(),  norm(),
            green(),  norm(),
            green(),  
            cur_page, tot_pages, norm(),
            green(),  norm());
    } else {
      sprintf(buffer + strlen(buffer),
          "\n\r[ %sReturn%s to continue ]\n\r",
            green(), norm());
    }
  }
  writeToQ(buffer);
}

void Descriptor::writeToQ(const char *arg) 
{
  output.putInQ(arg);
}

#if 0
char *Descriptor::badRaceMessage(int race)
{
  TBeing *ch;
  char buf[512];
  char buf2[20];
  int num;

  *buf = '\0';

  if (!(ch = character)) {
    vlogf(10, "Descriptor got to badClassMessage with no character!!! BUG BRUTIUS!");
    return NULL;
  }

  strcpy(buf2, Races[race]->getProperName().c_str());

  if (ch->getStat(STAT_CHOSEN, STAT_STR) > (num = max_stat(race, 1)))
    sprintf(buf, "%s may not have more than a %d strength.\n\r",
      cap(buf2), num);
  if (ch->getStat(STAT_CHOSEN, STAT_DEX) > (num = max_stat(race, 2)))
    sprintf(buf, "%s may not have more than a %d dexterity.\n\r",
      cap(buf2), num);
  if (ch->getStat(STAT_CHOSEN, STAT_INT)  > (num = max_stat(race, 3)))
    sprintf(buf, "%s may not have more than a %d intelligence.\n\r",
      cap(buf2), num);
  if (ch->getStat(STAT_CHOSEN, STAT_WIS) > (num = max_stat(race, 4)))
    sprintf(buf, "%s may not have more than a %d wisdom.\n\r",
      cap(buf2), num);
  if (ch->getStat(STAT_CHOSEN, STAT_CON) > (num = max_stat(race, 5)))
    sprintf(buf, "%s may not have more than a %d constitution.\n\r",
      cap(buf2), num);
  if (ch->getStat(STAT_CHOSEN, STAT_KAR) > (num = max_stat(race, 6)))
    sprintf(buf, "%s may not have more than a %d karma.\n\r",
      cap(buf2), num);

  if (ch->getStat(STAT_CHOSEN, STAT_STR) < (num = min_stat(race, 1)))
    sprintf(buf, "%s may not have less than a %d strength.\n\r",
      cap(buf2), num);
  if (ch->getStat(STAT_CHOSEN, STAT_DEX) < (num = min_stat(race, 2)))
    sprintf(buf, "%s may not have less than a %d dexterity.\n\r",
      cap(buf2), num);
  if (ch->getStat(STAT_CHOSEN, STAT_INT)  < (num = min_stat(race, 3)))
    sprintf(buf, "%s may not have less than a %d intelligence.\n\r",
      cap(buf2), num);
  if (ch->getStat(STAT_CHOSEN, STAT_WIS) < (num = min_stat(race, 4)))
    sprintf(buf, "%s may not have less than a %d wisdom.\n\r",
      cap(buf2), num);
  if (ch->getStat(STAT_CHOSEN, STAT_CON) < (num = min_stat(race, 5)))
    sprintf(buf, "%s may not have less than a %d constitution.\n\r",
      cap(buf2), num);
  if (ch->getStat(STAT_CHOSEN, STAT_KAR) < (num = min_stat(race, 6)))
    sprintf(buf, "%s may not have less than a %d karma.\n\r",
      cap(buf2), num);

  return (buf);
}
#endif

const string Descriptor::badClassMessage(int Class, bool multi, bool triple)
{
  TBeing *ch;
  char buf[512];

  *buf = '\0';

  if (!(ch = character)) {
    vlogf(10, "Descriptor got to badClassMessage with no character!!! BUG BRUTIUS!");
    return ("");
  }
  if (multi && !IS_SET(account->flags, ACCOUNT_ALLOW_DOUBLECLASS)) {
    sprintf(buf, "You must obtain a L%d single-class character first.\n\r", MAX_MORT);
    return (buf);
  }
  if (triple && !IS_SET(account->flags, ACCOUNT_ALLOW_TRIPLECLASS)) {
    sprintf(buf, "You must obtain a L%d double-class character first.\n\r", MAX_MORT);
    return (buf);
  }

  if (Class & CLASS_MONK) {
    strcat(buf, "Monks temporarily Disabled.\n\r");
  }
  if (Class & CLASS_WARRIOR) {
  }
  if (Class & CLASS_MAGIC_USER) {
  }
  if (Class & CLASS_CLERIC) {
  }
  if (Class & CLASS_DEIKHAN) {
  }
  if (Class & CLASS_RANGER) {
    if (rangerTerrainDeny(ch))
      strcat(buf, "Rangers must come from a wilderness background.\n\r");
    if (ch->getRace() == RACE_OGRE)
      strcat(buf, "Ogre's don't have the sensitivity to become Rangers.\n\r");
  }
  if (Class & CLASS_SHAMAN) {
    strcat(buf, "Shaman class is not currently supported.\n\r");

  }
  if (Class & CLASS_THIEF) {
  }
  return (buf);
}

void Descriptor::string_add(char *s)
{
  char *scan;
  int terminator = 0, t2 = 0;

  for (scan = s; *scan; scan++) {
    if ((terminator = (*scan == '~')) || (t2 = (*scan == '`'))) {
      *scan = '\0';
      break;
    }
  }
  if (!*str) {
    if (strlen(s) > (unsigned) max_str) {
      writeToQ("String too long - Truncated.\n\r");
      *(s + max_str) = '\0';
      terminator = 1;
    }
    unsigned int loop = 0;
    while (strlen(s) > 80) {
      int iter = -1;
      loop++;

      for (iter = 80; iter >= 0 && s[iter] != ' '; iter--);
      if (iter < 0)
        break;
      if (!*str) {
        *str = new char[iter + 3];
        mud_assert(*str != NULL, "string_add(): Bad string memory");
        strncpy(*str, s, iter);
        (*str)[iter] = '\0';
        strcat(*str, "\n\r");
      } else {
        char *t = *str;
        *str = new char[strlen(t) + iter + 3];
        mud_assert(*str != NULL, "string_add(): Bad string memory");
        strcpy(*str, t);
        strncat(*str, s, iter);
        (*str)[strlen(t) + iter] = '\0';
        strcat(*str, "\n\r");
        delete [] t;
      }

      // fix s
      char *t = s;
      for (;iter < (int) strlen(s) && isspace(s[iter]); iter++);
      s = mud_str_dup(&s[iter]);
      mud_assert(s != NULL, "string_add(): Bad string memory");
      if (loop > 1)
        delete [] t;
    }

    if (!*str) {
      *str = new char[strlen(s) + 3];
      strcpy(*str, s);
    } else {
      char *t = *str;
      *str = new char[strlen(t) + strlen(s) + 3];
      mud_assert(*str != NULL, "string_add(): Bad string memory");
      strcpy(*str, t);
      strcat(*str, s);
      delete [] t;
      if (loop >= 1)
        delete [] s;
    }
  } else {
    // preexisting *str
    if (strlen(s) + strlen(*str) > (unsigned) max_str) {
      writeToQ("String too long. Last line skipped.\n\r");
      terminator = 1;
    } else {
      if (character->isPlayerAction(PLR_BUGGING)) {
        if (!**str) {
          // we are on the subject line
          char *t = s;
          for (;*t && isspace(*t); t++);
    
          if (!*t) {
            writeToQ("Blank subject entered.  Ignoring!\n\r");
            *(name) = '\0';
    
            delete [] *str;
            *str = NULL;
  
            delete str;
            str = NULL;
    
            character->remPlayerAction(PLR_BUGGING);
  
            if (connected == CON_WRITING)
              connected = CON_PLYNG;

            if (client)
              clientf("%d|%d", CLIENT_ENABLEWINDOW, FALSE);

            return;
          }
          char buf[256];
          sprintf(buf, "Write your %s, use ~ when done, or ` to cancel.\n\r",
                good_uncap(name).c_str());
          writeToQ(buf);
          t = *str;
          if (strcmp(name, "Comment")) {
            // bugs, ideas, typos in here
            *str = new char[strlen(t) + strlen(s) + strlen("Subject: ") + 3 +
                    strlen(name) + 3];
            mud_assert(*str != NULL, "string_add(): Bad string memory");
            strcpy(*str, t);
            strcat(*str, "Subject: [");
            strcat(*str, name);
            strcat(*str, "] ");
            strcat(*str, s);
          } else {
            // comments in here
            *str = new char[strlen(t) + strlen(s) + 3];
            mud_assert(*str != NULL, "string_add(): Bad string memory");
            strcpy(*str, t);
            strcat(*str, s);
          }
          delete [] t;
        } else {
          // body of idea
          char *t = *str;
          *str = new char[strlen(t) + strlen(s) + 3];
          mud_assert(*str != NULL, "string_add(): Bad string memory");
          strcpy(*str, t);
          strcat(*str, s);
          if (!client)
            delete [] t;
        }
      } else {
        // not a bug/idea
#if 0
        char *t = *str;
        *str = new char[strlen(t) + strlen(s) + 3];
        mud_assert(*str != NULL, "string_add(): Bad string memory");
        strcpy(*str, t);
        strcat(*str, s);
        delete [] t;
#else
        // fix it up so that it all fits on one line
        unsigned int loop = 0;
        while (strlen(s) > 80) {
          int iter = -1;

          for (iter = 80; iter >= 0 && s[iter] != ' '; iter--);
          if (iter < 0)
            break;

          loop++;
          char *t = *str;
          *str = new char[strlen(t) + iter + 3];
          mud_assert(*str != NULL, "string_add(): Bad string memory");
          strcpy(*str, t);
          strncat(*str, s, iter);
          (*str)[strlen(t) + iter] = '\0';
          strcat(*str, "\n\r");
          delete [] t;

          // fix s
          t = s;
          for (;iter < (int) strlen(s) && isspace(s[iter]); iter++);
          if (iter < (int) strlen(s)) {
            s = mud_str_dup(&s[iter]);
            mud_assert(s != NULL, "string_add(): Bad string memory");
            if (loop > 1)
              delete [] t;
          } else {
            // don't bother doing anything, whitespace all the way to EOL
            if (loop > 1)
              delete [] t;
            break;
          }
        }

        char *t = *str;
        *str = new char[strlen(t) + strlen(s) + 3];
        mud_assert(*str != NULL, "string_add(): Bad string memory");
        strcpy(*str, t);
        strcat(*str, s);
        delete [] t;
        if (loop > 1)
          delete [] s;
#endif
      }
    }
  }
  if (terminator || t2) {
    if (character->isPlayerAction(PLR_MAILING)) {
      if (terminator)
        store_mail(name, character->getName(), *str);

      delete [] *str;
      *str = NULL;

      delete str;
      str = NULL;

      *(name) = '\0';
      if (terminator)
        writeToQ("Message sent!\n\r");
      else
        writeToQ("Message deleted!\n\r");

      character->remPlayerAction(PLR_MAILING);
    } else if (character->isPlayerAction(PLR_BUGGING)) {
      if (terminator) {
        char *t = *str;

        for (;*t && isspace(*t); t++);

        if (!*t) 
          writeToQ("Blank string entered.  Ignoring!\n\r");
        else {
          if (!strcmp(name, "Comment"))
            add_comment(delname, t);
          else
            send_bug(name, t);
          writeToQ(name);
          writeToQ(" sent!\n\r");
        }
      } else {
        writeToQ(name);
        writeToQ(" deleted!\n\r");
      }
      *(name) = '\0';

      delete [] *str;
      *str = NULL;

      delete str;
      str = NULL;

      character->remPlayerAction(PLR_BUGGING);
    } else {
      if (t2) {
        // Cancalation capability added by Russ 020997
        delete [] *str;
        *str = NULL;
      }
      str = NULL;

    }
    if (connected == CON_WRITING) {
      connected = CON_PLYNG;
      // do not delete the string, it has been aplied to the mob/obj/room
    }
    // set the string to NULL to insure we don't fall into string_add again
    str = NULL;

    if (client)
      clientf("%d|%d", CLIENT_ENABLEWINDOW, FALSE);
  } else {
    strcat(*str, "\n\r");
    //if (client)
      //prompt_mode = -1;
  }
}

void Descriptor::fdSocketClose(int desc)
{
  Descriptor *d, *d2;

  for (d = this; d; d = d2) {
    d2 = d->next;
    if (d->socket->sock == desc) {
      delete d;
      d = NULL;
    }
  }
  return;
}

void setPrompts(fd_set out)
{
  Descriptor *d, *nextd;
  TBeing *tank = NULL;
  TBeing *ch;
  TThing *obj;
  char promptbuf[256] = "\0\0\0",
       tString[256];
  unsigned int update;

  for (d = descriptor_list; d; d = nextd) {
    nextd = d->next;
    if ((FD_ISSET(d->socket->sock, &out) && d->output.getBegin()) || d->prompt_mode) {
      update = 0;
      if (!d->connected && (ch = d->character) && ch->isPc() &&
          !(ch->isPlayerAction(PLR_COMPACT)))
        (&d->output)->putInQ("\n\r");

      if ((ch = d->character) && ch->task) {
        if (ch->task->task == TASK_PENANCE) {
          sprintf(promptbuf, "\n\rPIETY : %5.2f > ", ch->getPiety());
          (&d->output)->putInQ(cap(promptbuf));
        }
        if (ch->task->task == TASK_MEDITATE) {
          sprintf(promptbuf, "\n\rMANA : %d > ", ch->getMana());
          (&d->output)->putInQ(cap(promptbuf));
        }
        if ((ch->task->task == TASK_SHARPEN || 
             ch->task->task == TASK_DULL) && 
            (obj = ch->heldInPrimHand())) {
          sprintf(promptbuf, "\n\r%s > ", ch->describeSharpness(obj).c_str());
          (&d->output)->putInQ(promptbuf);
        }
      }
      if (d->str && (d->prompt_mode != DONT_SEND)) {
        if (ch && ch->isPlayerAction(PLR_BUGGING) && !**d->str &&
            strcmp(d->name, "Comment")) {
          // ideas, bugs, typos
          (&d->output)->putInQ("Subject: ");
        } else {
          // comments
          (&d->output)->putInQ("-> ");
        }
      } else if (d->pagedfile && (d->prompt_mode != DONT_SEND)) {
        sprintf(promptbuf, "\n\r[ %sReturn%s to continue, %s(r)%sefresh, %s(b)%sack, page %s(%d/%d)%s, or %sany other key%s to quit ]\n\r", 
            d->green(),  d->norm(),
            d->green(),  d->norm(),
            d->green(),  d->norm(),
            d->green(),  
            d->cur_page, d->tot_pages, d->norm(),
            d->green(),  d->norm());
        (&d->output)->putInQ(promptbuf);
      } else if (!d->connected) {
        if (!ch) {
          vlogf(6, "Descriptor in connected mode with NULL desc->character.");
          continue;
        }
        if (d->showstr_head && (d->prompt_mode != DONT_SEND)) {
          sprintf(promptbuf, "\n\r[ %sReturn%s to continue, %s(r)%sefresh, %s(b)%sack, page %s(%d/%d)%s, or %sany other key%s to quit ]\n\r", 
            d->green(),  d->norm(),
            d->green(),  d->norm(),
            d->green(),  d->norm(),
            d->green(),  
            d->cur_page, d->tot_pages, d->norm(),
            d->green(),  d->norm());
          (&d->output)->putInQ(promptbuf);
        } else {
          if ((d->client || (ch->isPlayerAction(PLR_VT100 | PLR_ANSI) &&
                             IS_SET(d->prompt_d.type, PROMPT_VTANSI_BAR)))) {
            if (ch->getHit() != d->last.hit) {
              d->last.hit = ch->getHit();
              SET_BIT(update, CHANGED_HP);
            }
            if (ch->getMana() != d->last.mana) {
              d->last.mana = ch->getMana();
              SET_BIT(update, CHANGED_MANA);
            }
            if (ch->getPiety() != d->last.piety) {
              d->last.piety= ch->getPiety();
              SET_BIT(update, CHANGED_PIETY);
            }
            if (ch->getMove() != d->last.move) {
              d->last.move = ch->getMove();
              SET_BIT(update, CHANGED_MOVE);
            }
            if ((ch->getPosition() > POSITION_SLEEPING) && ch->getMoney() != d->last.money) {
              d->last.money = ch->getMoney();
              SET_BIT(update, CHANGED_GOLD);
            }
            if (ch->getCond(FULL) != d->last.full) {
              d->last.full = ch->getCond(FULL);
              SET_BIT(update, CHANGED_COND);
            }
            if (ch->getCond(THIRST) != d->last.thirst) {
              d->last.thirst = ch->getCond(THIRST);
              SET_BIT(update, CHANGED_COND);
            }
            if (ch->getPosition() != d->last.pos) {
              d->last.pos = ch->getPosition();
              SET_BIT(update, CHANGED_POS);
            }
            if (ch->getExp() != d->last.exp) {
              d->last.exp = ch->getExp();
              SET_BIT(update, CHANGED_EXP);
            }
            if (ch->in_room != d->last.room) {
              d->last.room = ch->in_room;
              SET_BIT(update, CHANGED_ROOM);
            }
#if FACTIONS_IN_USE
            if (ch->getPerc() != d->last.perc) {
              d->last.perc = ch->getPerc();
              SET_BIT(update, CHANGED_PERC);
            }
#endif
            if (time_info.hours != d->last.mudtime) {
              d->last.mudtime = time_info.hours;
              SET_BIT(update, CHANGED_MUD);
            }
            if (update || ch->fight()) {
              if (ch->vt100())
                d->updateScreenVt100(update);
              else if (ch->ansi())
                d->updateScreenAnsi(update);
              else if (d->client) {
                d->outputProcessing();
                d->send_client_prompt(TRUE, update); // Send client prompt
              }
            }
            /*
            if (d->prompt_mode != DONT_SEND && d->client) {
              strcpy(promptbuf, "> ");
              (&d->output)->putInQ(promptbuf);
            }
            */
          }

          if (d->prompt_mode != DONT_SEND) {
            bool hasColor = IS_SET(d->prompt_d.type, PROMPT_COLOR);
            *promptbuf = '\0';

            const char *StPrompts[] =
            {
              "[Z:%d Pr:%s L:%d H:%d C:%d S:%s]\n\r", // Builder Assistant
              "%sH:%d%s ",           // Hit Points
              "%sP:%.1f%s ",         // Piety
              "%sM:%d%s ",           // Mana
              "%sV:%d%s ",           // Moves
              "%sT:%d%s ",           // Talens
              "%sR:%d%s ",           // Room
              "%sE:%s%s ",           // Exp
              "%sN:%s%s ",           // Exp Tnl
              "%s%s<%s%s=%s>%s ",    // Opponent
              "%s%s<%s/tank=%s>%s ", // Tank / Tank-Other
              "%s<%.1f%s> "          // Lockout
            };

            if (ch->isImmortal() && IS_SET(d->prompt_d.type, PROMPT_BUILDER_ASSISTANT)) {
              sprintf(promptbuf + strlen(promptbuf),
                      StPrompts[0],
                      ch->roomp->getZone(),
                      (ch->roomp->funct ? "Y" : "N"),
                      ch->roomp->getLight(),
                      ch->roomp->getRoomHeight(),
                      ch->roomp->getMoblim(),
                      TerrainInfo[ch->roomp->getSectorType()]->name);
            }
            if (IS_SET(d->prompt_d.type, PROMPT_HIT))
              sprintf(promptbuf + strlen(promptbuf),
                      StPrompts[1],
                      (hasColor ? d->prompt_d.hpColor : ""),
                      ch->getHit(),
                      ch->norm());
            if (IS_SET(d->prompt_d.type, PROMPT_MANA))
              if (ch->hasClass(CLASS_CLERIC) || ch->hasClass(CLASS_DEIKHAN))
                sprintf(promptbuf + strlen(promptbuf),
                        StPrompts[2],
                        (hasColor ? d->prompt_d.manaColor : ""),
                        ch->getPiety(),
                        ch->norm());
              else
                sprintf(promptbuf + strlen(promptbuf),
                        StPrompts[3],
                        (hasColor ? d->prompt_d.manaColor : ""),
                        ch->getMana(),
                        ch->norm());
            if (IS_SET(d->prompt_d.type, PROMPT_MOVE))
              sprintf(promptbuf + strlen(promptbuf),
                      StPrompts[4],
                      (hasColor ? d->prompt_d.moveColor : ""),
                      ch->getMove(),
                      ch->norm());
            if (IS_SET(d->prompt_d.type, PROMPT_GOLD))
              sprintf(promptbuf + strlen(promptbuf),
                      StPrompts[5],
                      (hasColor ? d->prompt_d.moneyColor : ""),
                      ch->getMoney(),
                      ch->norm());
            if (IS_SET(d->prompt_d.type, PROMPT_ROOM))
              sprintf(promptbuf + strlen(promptbuf),
                      StPrompts[6],
                      (hasColor ? d->prompt_d.roomColor : ""),
                      ch->roomp->number,
                      ch->norm());
            if (IS_SET(d->prompt_d.type, PROMPT_EXP)) {
              strcpy(tString, ch->displayExp().c_str());
              comify(tString);
              sprintf(promptbuf + strlen(promptbuf),
                      StPrompts[7],
                      (hasColor ? d->prompt_d.expColor : ""),
                      tString,
                      ch->norm());
            }
            if (IS_SET(d->prompt_d.type, PROMPT_EXPTONEXT_LEVEL)) {
              classIndT i;

              for (i = MAGE_LEVEL_IND; i < MAX_CLASSES; i++)
                if (ch->getLevel(i) && ch->getLevel(i) < MAX_MORT) {
                  if (ch->getExp() > d->prompt_d.xptnl)
                    d->prompt_d.xptnl = getExpClassLevel(i, ch->getLevel(i) + 1);

                  double need = d->prompt_d.xptnl - ch->getExp();

                  sprintf(tString, "%.0f", need);
                  comify(tString);
                  sprintf(promptbuf + strlen(promptbuf),
                          StPrompts[8],
                          (hasColor ? ch->desc->prompt_d.expColor : ""),
                          tString,
                          ch->norm());
                  break;
                }
            }
            bool bracket_used = false;
            if (IS_SET(d->prompt_d.type, PROMPT_OPPONENT))
              if (ch->fight() && ch->fight()->sameRoom(ch)) {
                int ratio = min(10, max(0, ((ch->fight()->getHit() * 9) /
                                ch->fight()->hitLimit())));

                sprintf(promptbuf + strlen(promptbuf),
                        StPrompts[9],
//                        (bracket_used ? "" : ">\n\r"),
                        (bracket_used ? "" : " "),
                        (hasColor ? d->prompt_d.oppColor : ""),
                        (ch->isAffected(AFF_ENGAGER) ? "ENGAGED " : ""),
                        ch->persfname(ch->fight()).c_str(), prompt_mesg[ratio],
                        ch->norm());
                bracket_used = true;
              }
            bool isOther = false;
            if ((isOther = IS_SET(d->prompt_d.type, PROMPT_TANK_OTHER)) ||
                IS_SET(d->prompt_d.type, PROMPT_TANK))
              if (ch->fight() && ch->awake() && ch->fight()->sameRoom(ch)) {
                tank = ch->fight()->fight();

                if (tank && (!isOther || tank != ch)) {
                  if (ch->sameRoom(tank)) {
                    int ratio = min(10, max(0, ((tank->getHit() * 9) / tank->hitLimit())));

                    sprintf(promptbuf + strlen(promptbuf),
                            StPrompts[10],
//                        (bracket_used ? "" : ">\n\r"),
                        (bracket_used ? "" : " "),
                            (hasColor ? d->prompt_d.tankColor : ""),
                            ch->persfname(tank).c_str(),
                            prompt_mesg[ratio],
                            ch->norm());
                    bracket_used = true;
                  }
                }
              }
            if (d->wait > 1) {
              float waittime = (float) (d->wait - 1) / ONE_SECOND;

              sprintf(promptbuf + strlen(promptbuf),
                      StPrompts[11],
//                        (bracket_used ? "" : ">\n\r"),
                        (bracket_used ? "" : " "),
                      waittime,
                      (IS_SET(d->autobits, AUTO_NOSPAM) ? "" : " secs lockout"));
            }

            strcat(promptbuf, "> ");
            (&d->output)->putInQ(promptbuf);
          }
        }
      }
    }
  }
}

void afterPromptProcessing(fd_set out)
{
  Descriptor *d, *next_d;

  for (d = descriptor_list; d; d = next_d) {
    next_d = d->next;
    if (FD_ISSET(d->socket->sock, &out) && d->output.getBegin())
      if (d->outputProcessing() < 0) {
        delete d;
        d = NULL;
      } else
        d->prompt_mode = 0;
  }
}

void Descriptor::saveAll()
{
  Descriptor *d;

  for (d = descriptor_list; d; d = d->next) {
    if (d->character)
      d->character->doSave(SILENT_NO);
  }
}

void Descriptor::worldSend(const char *text, TBeing *ch)
{
  Descriptor *d;

  for (d = descriptor_list; d; d = d->next) {
    if (!d->connected)
      (&d->output)->putInQ(colorString(ch, d, text, NULL, COLOR_BASIC, TRUE).c_str());
  }
}

void Descriptor::sendShout(TBeing *ch, const char *arg)
{
  Descriptor *i;
  TBeing *b;
  char capbuf[256];
  char namebuf[100];

  for (i = descriptor_list; i; i = i->next) {
    if ((b = i->character) && (b != ch) && !i->connected &&
       b->awake() &&
       (dynamic_cast<TMonster *>(b) ||
        (!b->isImmortal() && ch->isImmortal()) ||
        (b->desc && !IS_SET(i->autobits, AUTO_NOSHOUT))) &&
       !b->checkSoundproof() && 
       !b->isPlayerAction(PLR_MAILING | PLR_BUGGING)) {
      strcpy(capbuf, b->pers(ch));
      string argbuf = colorString(b, i, arg, NULL, COLOR_NONE, FALSE);
      sprintf(namebuf, "<g>%s<z>", cap(capbuf));
      string nameStr = colorString(b, i, namebuf, NULL, COLOR_NONE, FALSE);
      if(hasColorStrings(NULL, capbuf, 2)) {
        if (IS_SET(b->desc->plr_color, PLR_COLOR_MOBS)) {
          string tmpbuf = colorString(b, i, cap(capbuf), NULL, COLOR_MOBS, FALSE);
          string tmpbuf2 = colorString(b, i, cap(capbuf), NULL, COLOR_NONE, FALSE);

          if (i->client) 
            i->clientf("%d|%s|%s", CLIENT_SHOUT, tmpbuf2.c_str(), argbuf.c_str());

          b->sendTo(COLOR_SHOUTS, "%s shouts, \"%s<1>\"\n\r",tmpbuf.c_str(), arg);
        } else { 
          if (i->client) 
            i->clientf("%d|%s|%s%s", CLIENT_SHOUT, nameStr.c_str(), argbuf.c_str());

          b->sendTo(COLOR_SHOUTS, "<g>%s<z> shouts, \"%s<1>\"\n\r", cap(capbuf), arg);
        }
      } else {
        if (i->client) 
          i->clientf("%d|%s|%s", CLIENT_SHOUT, nameStr.c_str(), argbuf.c_str());

        b->sendTo(COLOR_SHOUTS, "<g>%s<z> shouts, \"%s<1>\"\n\r", cap(capbuf), arg);
      }
    }
  }
}

void processAllInput()
{
  Descriptor *d;
  char comm[20000] = "\0\0\0";
  int rc;
  TRoom *rp;

  for (d = descriptor_list; d; d = next_to_process) {
    next_to_process = d->next;

    // this is where PC wait gets handled
    if ((--(d->wait) <= 0) && (&d->input)->takeFromQ(comm)){
      if (d->character && !d->connected && 
          d->character->specials.was_in_room != ROOM_NOWHERE) {
        --(*d->character);
        rp = real_roomp(d->character->specials.was_in_room);
        *rp += *d->character;
        d->character->specials.was_in_room = ROOM_NOWHERE;
        act("$n has returned.", TRUE, d->character, 0, 0, TO_ROOM);
      }
      d->wait = 1;
      if (d->character) {
        d->character->setTimer(0);
        if (d->character->isPlayerAction(PLR_AFK)) {
          d->character->sendTo("Your afk flag has been removed.\n\r");
          d->character->remPlayerAction(PLR_AFK);
        }
      }
      if (d->original) {
        d->original->setTimer(0);
        d->original->remPlayerAction(PLR_AFK);
      }
      if (d->prompt_mode != DONT_SEND)
        d->prompt_mode = 1;

      if (is_client_string(comm)) {
        rc = d->read_client(comm);
        if (IS_SET_DELETE(rc, DELETE_THIS)) {
          delete d;
          d = NULL;
          continue;
        } 
        if (IS_SET_DELETE(rc, DELETE_VICT)) {
          delete d->character;
          d->character = NULL;
          continue;
        }
      } else if (d->str) 
        d->string_add(comm);
      else if (d->pagedfile) 
        d->page_file(comm);
      else if (!d->account) {            // NO ACCOUNT
        if (d->client) {
          rc = d->client_nanny(comm);
          if (IS_SET_DELETE(rc, DELETE_THIS)) {
            delete d;
            d = NULL;
          } 
          continue;
        }
        rc = d->sendLogin(comm);
        if (IS_SET_DELETE(rc, DELETE_THIS)) {
          delete d;
          d = NULL;
          continue;
        }
      } else if (!d->account->status) {       //ACCOUNT STUFF 
        rc = d->doAccountStuff(comm);
        if (IS_SET_DELETE(rc, DELETE_THIS)) {
          delete d;
          d = NULL;
          continue;
        }
      } else if (!d->connected) {
        if (d->showstr_head) {
          d->show_string(comm, FALSE, TRUE);
        } else {
          rc = d->character->parseCommand(comm, TRUE);
          // the "if d" is here due to a core that showed d=0x0
          // after a purge ldead
          if (d && IS_SET_DELETE(rc, DELETE_THIS)) {
            // in another wierd core, d was no longer in the descriptor_list
            Descriptor *tempdesc;
            for (tempdesc = descriptor_list; tempdesc; tempdesc = tempdesc->next) {
              if (tempdesc == d || tempdesc == next_to_process)
                break;
            }
            if (tempdesc == d) {
              delete d->character;
              d->character = NULL;
            } else {
              // either descriptor_list hit end, or d is the next guy to process
              // in all likelihood, this descriptor has already been deleted and we point to free'd memory
              vlogf(9, "Descriptor not found in list after parseCommand called.  (%s).  VERY BAD!", comm);
            }
            continue;
          }
        }
      } else if (!d->character) {
        if (d->account->term && d->account->term == TERM_ANSI)
          SET_BIT(d->plr_act, PLR_ANSI);

        rc = d->doAccountMenu(comm);
        if (IS_SET_DELETE(rc, DELETE_THIS)) {
          delete d;
          d = NULL;
          continue;
        }
      } else if (d->connected == CON_REDITING) 
        room_edit(d->character, comm);
      else if (d->connected == CON_OEDITING) 
        obj_edit(d->character, comm);
      else if (d->connected == CON_MEDITING) 
        mob_edit(d->character, comm);
      else if (d->connected == CON_SEDITING)
        seditCore(d->character, comm);
      else if (d->showstr_head) {
        d->show_string(comm, TRUE, TRUE);
      } else {
        rc = d->nanny(comm);
        if (IS_SET_DELETE(rc, DELETE_THIS)) {
          delete d;
          d = NULL;
          continue;
        }
      }
    } else if (d->wait <= 0) 
      d->wait = 1;
  }
}

int bogusAccountName(const char *arg)
{
  char buf[256];
  int i = 0;

  strcpy(buf, arg);
  for (i=0; i < (int) strlen(buf) && i <20; i++) {
    if ((buf[i] < 'A') || ((buf[i] > 'Z') && (buf[i] < 'a')) ||
        (buf[i] > 'z'))
      return TRUE;
  }
  return FALSE;
}

// return DELETE_THIS
int Descriptor::sendLogin(const char *arg)
{
  char buf[160], buf2[4096] = "\0\0\0";
  accountFile afp;

  if (client)
    return FALSE;

  if (!*arg)
    return DELETE_THIS;
  else if (*arg == '?') {
    writeToQ("Accounts are used to store all characters belonging to a given person.\n\r");
    writeToQ("One account can hold multiple characters.  Creating more than one account\n\r");
    sprintf(buf, "for yourself is a violation of %s multiplay rules and will lead to\n\r", MUD_NAME);
    writeToQ(buf);
    writeToQ("strict sanctions.  Your account holds information that is applied to all\n\r");
    writeToQ("characters that you own (including: generic terminal type, time zone\n\r");
    writeToQ("you play from, etc.).  You are required to enter a valid and unique\n\r");
    writeToQ("E-mail address which will be kept secret and used by the game-maintainers\n\r");
    writeToQ("to inform you if a serious problem occurs with your account.\n\r\n\r");
    writeToQ("Note that the only password protection is at the account level.  Do not\n\r");
    writeToQ("reveal your password to others or they have access to ALL of your characters.\n\r\n\r");
    writeToQ("Type NEW to generate a new account.\n\r");
    writeToQ("Login:");
    return FALSE;
  } else if (*arg == '1') {
    FILE * fp = fopen("txt/version", "r");
    if (!fp) {
      vlogf(10, "No version file found");
    } else {
      fgets(buf, 79, fp);
      // strip off the terminating newline char
      buf[strlen(buf) - 1] = '\0';

      sprintf(buf2 + strlen(buf2), "\n\r\n\rWelcome to %s :\n\r%s :\n\r", MUD_NAME_VERS, buf);
      fclose(fp);
    }
    sprintf(buf2 + strlen(buf2), "Celebrating seven years providing quality MUDding.\n\r\n\r");
    sprintf(buf2 + strlen(buf2), "Enter NEW for a new account, or ? for help.\n\r");
    sprintf(buf2 + strlen(buf2), "\n\rLogin: ");
    output.putInQ(buf2);
    return FALSE;
  } else if (!strcasecmp(arg, "new")) {
    if (WizLock) {
      writeToQ("The game is currently wiz-locked.\n\r");
      if (!lockmess.empty()) {
        page_string(lockmess.c_str(), TRUE);
      } else {
        FILE *signFile;

        if ((signFile = fopen(SIGN_MESS, "r"))) {
          fclose(signFile);
          string iostring;
          file_to_string(SIGN_MESS, iostring);
          page_string(iostring.c_str(), TRUE);
        }
      }
      writeToQ("Wiz-Lock password: ");

      account = new TAccount();
      connected = CON_WIZLOCKNEW;
    } else {
      account = new TAccount();
      output.putInQ("Enter a login name for your account -> ");
      connected = CON_NEWLOG;
    }
    return FALSE;
  } else {
    account = new TAccount();
    if (*arg == '#')   // NCSA telnet put # when first logging in.
       arg++;

    if (bogusAccountName(arg)) {
      output.putInQ("Illegal account name.\n\r");
      delete account;
      account = NULL;
      return (sendLogin("1"));
    }
    strcpy(account->name, arg);
    sprintf(buf, "account/%c/%s/account", LOWER(arg[0]), lower(arg).c_str());
    // If account exists, open and copy password, otherwise set pwd to \0
    FILE * fp = fopen(buf, "r");
    if (fp) {
      fread(&afp, sizeof(afp), 1, fp);
      strcpy(account->name, afp.name);
      strcpy(account->passwd, afp.passwd);
      strcpy(account->email, afp.email);
      account->term = termTypeT(afp.term);
#if 1
// COSMO TEST
      if (account->term == TERM_ANSI) 
        plr_act = PLR_COLOR;
#endif

      account->birth = afp.birth;
      account->login = time(0);
      account->status = FALSE;
      account->flags = afp.flags;
      account->time_adjust = afp.time_adjust;
      account->desc = this;
      strcpy(pwd, afp.passwd); 
      fclose(fp);
    } else 
      *pwd = '\0';
 
    output.putInQ("Password: ");
    EchoOff();
    connected = CON_ACTPWD;
  }
  return FALSE;
}

bool Descriptor::checkForAccount(char *arg)
{
  char buf[256];
  struct stat timestat;

  if (bogusAccountName(arg)) {
    writeToQ("Sorry, that is an illegal name for an account.\n\r");
    return TRUE;
  }
  sprintf(buf, "account/%c/%s/account", LOWER(arg[0]), lower(arg).c_str());
  
  if (!stat(buf, &timestat)) {
    writeToQ("Account already exists, enter another name.\n\r");
    return TRUE;
  }
  return FALSE;
}

bool Descriptor::checkForCharacter(char *arg)
{
  char buf[256];
  struct stat timestat;

  sprintf(buf, "player/%c/%s", LOWER(arg[0]), lower(arg).c_str());
 
  if (!stat(buf, &timestat)) {
    if (!client)
      writeToQ("Character already exists, enter another name.\n\r--> ");
    return TRUE;
  }
  return FALSE;
}



// return DELETE_THIS
int Descriptor::doAccountStuff(char *arg)
{
  char tmp_name[256];
  char buf[256];
  int count = 0;
  string lStr;
  struct stat timestat;
  int rc;
  int tss = screen_size;

  // apparently, crypt() has a mem leak in the lib function
  // By making this static, we limit the number of leaks to one
  // rather than one per call.
  static char *crypted;

  for (;isspace(*arg);arg++);

  switch (connected) {
    case CON_NEWLOG:
      // kick um out so they aren't stuck
      if (!*arg) 
        return DELETE_THIS;
      
      if (checkForAccount(arg)) {
        output.putInQ("Please enter a login name -> ");
        return FALSE;
      } 
      if (strlen(arg) >= 10) {
        output.putInQ("Account names must be 9 characters or less.\n\r");
        output.putInQ("Please enter a login name -> ");
        return FALSE;
      }
      strcpy(account->name, arg);
      output.putInQ("Now enter a password for your new account\n\r-> ");
      EchoOff();

      connected = CON_NEWACTPWD;
      break;
    case CON_NEWACTPWD:
      if (strlen(arg) < 5) {
        writeToQ("Your password must contain at least 5 characters.\n\r");
        writeToQ("Password -> ");
        return FALSE;
      } else if (strlen(arg) > 10) {
        writeToQ("Your password can only contain 10 or fewer characters.\n\r");
        writeToQ("Password -> ");
        return FALSE;
      } 
      if (!hasDigit(arg)) {
        writeToQ("Your password must contain at least one number.\n\r");
        writeToQ("Password -> ");
        return FALSE;
      }
      crypted =(char *) crypt(arg, account->name);
      strncpy(pwd, crypted, 10);
      *(pwd + 10) = '\0';
      writeToQ("Retype your password for verification -> ");
      connected = CON_PWDCNF;
      break;
    case CON_PWDCNF:
      crypted = (char *) crypt(arg, pwd);
      if (strncmp(crypted, pwd, 10)) {
        writeToQ("Mismatched Passwords. Try again.\n\r");
        writeToQ("Retype password -> ");
        connected = CON_NEWACTPWD;
        return FALSE;
      } else {
        strcpy(account->passwd, pwd);
        EchoOn();
        writeToQ("Enter your email address.\n\r");
        writeToQ("E-mail addresses are used strictly for administrative purposes, or for\n\r");
        writeToQ("contacting you in the event of a problem.  The information is never used for\n\r");
        writeToQ("advertisements, nor will it be provided to any other person or organization.\n\r");
        writeToQ("Bogus addressed accounts will be deleted.\n\rAddress -> ");
        connected = CON_EMAIL;
      }
      break;
    case CON_EMAIL:
      if (!*arg) 
        return DELETE_THIS;
      
      if (illegalEmail(arg, this, SILENT_NO)) {
        writeToQ("Please enter an email address. Accounts with bogus addresses will be deleted.\n\r");
        writeToQ("Address -> ");
        break;
      }
      strcpy(account->email, arg);

      sprintf(buf, "\n\r\n\r%s presently has email-listservers for discussion of %s\n\r", MUD_NAME, MUD_NAME);
      writeToQ(buf);
      writeToQ("information. Would you like to subscribe to these lists? Please\n\r");
      writeToQ("do not attempt to subscribe if you entered a bogus email address.\n\r");
      writeToQ("1. Yes I want to subscribe!\n\r2. No, but thanks anyway.\n\r-> ");
      connected = CON_LISTSERV;
      break;
    case CON_LISTSERV:
      switch(*arg) {
        case '1':
          writeToQ("OK, you will be subscribed to the listserver!\n\r\n\r");
          sprintf(buf,  "/usr/lib/sendmail -f%s russrussell@icqmail.com < listserver.temp", account->email);
          vsystem(buf);
          break;
        case '2':
          writeToQ("OK, you will not be subscribed to the listserver.\n\r\n\r");
          break;
        default:
          writeToQ("Invalid Response!\n\r1. Yes I want to subscribe!\n\r2. No, but thanks anyway.\n\r-> ");
          return FALSE;
      }
      sprintf(buf, "%s is presently based in California (Pacific Time)\n\r", MUD_NAME);
      writeToQ(buf);
      writeToQ("For purposes of keeping track of time, please enter the difference\n\r");
      writeToQ("between your home site and Pacific Time.  For instance, players on\n\r");
      writeToQ("the East Coast should enter 3, whereas Hawaii would enter -2.\n\r");
      writeToQ("You may alter this later via the TIME command. (see HELP TIME once online).\n\r\n\r");
      writeToQ("Time Difference -> ");
      connected = CON_TIME;
      break;
    case CON_TIME:
      if (!*arg || (atoi(arg) > 23) || (atoi(arg) < -23)) {
        writeToQ("Please enter a number from -23 to 23\n\r");
        writeToQ("Time difference -> ");
        break;
      }
      account->time_adjust = atoi(arg);

      writeToQ("What is your terminal type? (A)nsi, [V]t100 (default), (N)one. -> ");
      connected = CON_TERM;
      break;
    case CON_TERM:
      if (*arg == 'a' || *arg == 'A') 
        account->term = TERM_ANSI;
      else if (!*arg || *arg == 'V' || *arg == 'v')
        account->term = TERM_VT100;
      else if ((*arg == 'n') || *arg == 'N') 
        account->term = TERM_NONE;
      else {
        writeToQ("What is your terminal type? (A)nsi, [V]t100 (default), (N)one -> ");
        return FALSE;
      }
      saveAccount();
      account_number++;
      account->status = TRUE;
      rc = doAccountMenu("");
      if (IS_SET_DELETE(rc, DELETE_THIS))
        return DELETE_THIS;
      break;
    case CON_WIZLOCKNEW:
      if (!*arg || strcasecmp(arg, WIZLOCK_PASSWORD)) 
        return DELETE_THIS;

      vlogf(9, "Person making new character after entering wizlock password.");

      output.putInQ("Enter a login name for your account -> ");
      connected = CON_NEWLOG;
      break;
    case CON_WIZLOCK:
      if (!*arg || strcasecmp(arg, WIZLOCK_PASSWORD)) 
        return DELETE_THIS;
      
      vlogf(9, "Person entering game by entering wizlock password.");

      account->status = TRUE;
      if (!IS_SET(account->flags, ACCOUNT_BOSS)) {
        if (account->term != TERM_ANSI) {
          screen_size = 40;  // adjust for the size of the menu bar temporarily
          start_page_file(NORM_OPEN, "");
          screen_size = tss;
        } else {
          screen_size = 40;  // adjust for the size of the menu bar temporarily
          start_page_file(ANSI_OPEN, "");
          screen_size = tss;
          writeToQ(ANSI_NORMAL);
        }
      }
      writeToQ("Type 'C' to connect with an existing character, or <enter> to see account menu.\n\r-> ");
      break;
    case CON_ACTPWD:
      EchoOn();
      if (!*pwd) {
        writeToQ("Incorrect login.\n\r");
        delete account;
        account = NULL;
        return (sendLogin("1"));
      }
      crypted = (char *) crypt(arg, pwd);
      if (strncmp(crypted, pwd, 10)) {
        writeToQ("Incorrect login.\n\r");
        delete account;
        account = NULL;
        return (sendLogin("1"));
      }
      if (IS_SET(account->flags, ACCOUNT_BANISHED)) {
        writeToQ("Your account has been flagged banished.\n\r");
        sprintf(buf, "If you do not know the reason for this, contact %s\n\r",
              MUDADMIN_EMAIL);
        writeToQ(buf);
        outputProcessing();
        return DELETE_THIS;
      }
      if (IS_SET(account->flags, ACCOUNT_EMAIL)) {
        writeToQ("The email account you entered for your account is thought to be bogus.\n\r");
        sprintf(buf, "You entered an email address of: %s\n\r", account->email);
        writeToQ(buf);
        sprintf(buf,"To regain access to your account, please send an email\n\rto: %s\n\r",
              MUDADMIN_EMAIL);
        writeToQ(buf);
        writeToQ("Indicate the name of your account, and the reason for the wrong email address.\n\r");
        outputProcessing();
        return DELETE_THIS;
      }
      // let's yank the password out of their history list
      strcpy(history[0], "");

      if (WizLock && !IS_SET(account->flags, ACCOUNT_IMMORTAL)) {
        writeToQ("The game is currently wiz-locked.\n\r");
        if (!lockmess.empty()) {
          page_string(lockmess.c_str(), TRUE);
        } else {
#if 0
          ifstream op(SIGN_MESS, ios::in | ios::nocreate);
          if (op) {
            op.close();
            string iostring;
            file_to_string(SIGN_MESS, iostring);
            page_string(iostring.c_str(), TRUE);
          }
#else
          FILE *signFile;

          if ((signFile = fopen(SIGN_MESS, "r"))) {
            fclose(signFile);
            string iostring;
            file_to_string(SIGN_MESS, iostring);
            page_string(iostring.c_str(), TRUE);
          }
#endif
        }
        writeToQ("Wiz-Lock password: ");
        connected = CON_WIZLOCK;
        break;
      } else if (WizLock) {
        // wizlock is on, but I am an IMM, just notify me
        writeToQ("The game is currently wiz-locked.\n\r");
        if (!lockmess.empty()) {
          page_string(lockmess.c_str(), TRUE);
        } else {
#if 0
          ifstream opp(SIGN_MESS, ios::in | ios::nocreate);
          if (opp) {
            opp.close();
            string iosstring;
            file_to_string(SIGN_MESS, iosstring);
            page_string(iosstring.c_str(), TRUE);
          }
#else
          FILE *signFile;

          if ((signFile = fopen(SIGN_MESS, "r"))) {
            fclose(signFile);
            string iostring;
            file_to_string(SIGN_MESS, iostring);
            page_string(iostring.c_str(), TRUE);
          }
#endif
        }
      }
      account->status = TRUE;
      if (!IS_SET(account->flags, ACCOUNT_BOSS)) {
        if (account->term != TERM_ANSI) {
          screen_size = 40;  // adjust for the size of the menu bar temporarily
          start_page_file(NORM_OPEN, "");
          screen_size = tss;
        } else {
          screen_size = 40;  // adjust for the size of the menu bar temporarily
          start_page_file(ANSI_OPEN, "");
          screen_size = tss;
          writeToQ(ANSI_NORMAL);
        }
      }
      count = listAccount(account->name, lStr);
      if (count) 
        writeToQ("Type 'C' to connect with an existing character, or <enter> to see account menu.\n\r-> ");
      else
        writeToQ("Type 'A' to add a new character, or <enter> to see account menu.\n\r-> ");
      break;
    case CON_DELCHAR:
      if (*arg == '/') {
        go_back_menu(connected);
        break;
      }

      if (_parse_name(arg, tmp_name)) {
        writeToQ("Illegal name, please try another.\n\r");
        writeToQ("Name -> ");
        return FALSE;
      }
      // lower() returns static buf, so add one at a time
      sprintf(buf, "account/%c/%s", LOWER(account->name[0]), lower(account->name).c_str());
      sprintf(buf + strlen(buf), "/%s", lower(arg).c_str());
      // sprintf(buf, "account/%c/%s/%s", LOWER(account->name[0]), 
      //                                  lower(account->name), lower(arg));
      if (stat(buf, &timestat)) {
        writeToQ("No such character.\n\r");
        account->status = TRUE;
        rc = doAccountMenu("");
        if (IS_SET_DELETE(rc, DELETE_THIS))
          return DELETE_THIS;
        break;
      }
      writeToQ("Deleting a character will result in total deletion and\n\r");
      writeToQ("equipment loss. Enter your password to verify or hit enter\n\r"); 
      writeToQ("to return to the account menu system\n\r-> ");
      EchoOff();

      strcpy(delname, lower(arg).c_str());
      connected = CON_CHARDELCNF;
      break;
    case CON_CHARDELCNF:
      EchoOn();
      if (!*pwd) {
        writeToQ("Incorrect password.\n\r");
        writeToQ("Which do you want to do?\n\r");
        writeToQ("1) Delete your account\n\r");
        writeToQ("2) Delete a character in your account\n\r");
        writeToQ("3) Return to main account menu.\n\r-> ");
        connected = CON_DELETE;
        break;
      }
      crypted = (char *) crypt(arg, pwd);
      if (strncmp(crypted, pwd, 10)) {
        writeToQ("Incorrect password.\n\r");
        writeToQ("Which do you want to do?\n\r");
        writeToQ("1) Delete your account\n\r");
        writeToQ("2) Delete a character in your account\n\r");
        writeToQ("3) Return to main account menu.\n\r-> ");
        connected = CON_DELETE;
        break;
      }
      writeToQ("Character deleted.\n\r");
      vlogf(1, "Character %s self-deleted. (%s account)", delname, account->name);
      DeleteHatreds(NULL, delname);
      autobits = 0;

      wipePlayerFile(delname);  // handles corpses too
      wipeRentFile(delname);
      wipeFollowersFile(delname);

      while (has_mail(delname)) {
        vlogf(1, "Deleting mail for character %s.", delname);
        char *tmpp = read_delete(delname, delname);
        delete [] tmpp;
      }

      sprintf(buf, "account/%c/%s/%s", LOWER(account->name[0]), 
           lower(account->name).c_str(), delname);
      if (unlink(buf) != 0)
        vlogf(9, "error in unlink (3) (%s) %d", buf, errno);
      account->status = TRUE;
      rc = doAccountMenu("");
      if (IS_SET_DELETE(rc, DELETE_THIS))
        return DELETE_THIS;
      break; 
    case CON_ACTDELCNF:
      EchoOn();
      if (!*pwd) {
        writeToQ("Incorrect password.\n\r");
        writeToQ("Which do you want to do?\n\r");
        writeToQ("1) Delete your account\n\r");
        writeToQ("2) Delete a character in your account\n\r");
        writeToQ("3) Return to main account menu.\n\r-> ");
        connected = CON_DELETE;
        break;
      }
      crypted = (char *) crypt(arg, pwd);
      if (strncmp(crypted, pwd, 10)) {
        writeToQ("Incorrect password.\n\r");
        writeToQ("Which do you want to do?\n\r");
        writeToQ("1) Delete your account\n\r");
        writeToQ("2) Delete a character in your account\n\r");
        writeToQ("3) Return to main account menu.\n\r-> ");
        connected = CON_DELETE;
        break;
      }
      deleteAccount();
      writeToQ("Your account has been deleted, including all characters and equipment.\n\r");
      sprintf(buf, "Thank you for playing %s, and hopefully you will remake your character(s)!\n\r", MUD_NAME);
      writeToQ(buf);
      outputProcessing();
      return DELETE_THIS;
    case CON_DELETE:
      switch (*arg) {
        case '1':
          writeToQ("Are you sure you want to completely delete your account?\n\r");
          writeToQ("Doing so will delete all characters and their equipment.\n\r");
          writeToQ("If you are sure, enter your password for verification.\n\r");
          writeToQ("Otherwise hit enter.\n\r-> ");
          EchoOff();
          connected = CON_ACTDELCNF;
          break;
        case '2':
          writeToQ("To abort the deletion type '/', otherwise ...\n\r");
          writeToQ("Enter the name of the character you wish to delete.\n\r-> ");
          connected = CON_DELCHAR;
          break; 
        case '3':
          account->status = TRUE;
          rc = doAccountMenu("");
          if (IS_SET_DELETE(rc, DELETE_THIS))
            return DELETE_THIS;
          break;
        default:
          writeToQ("Which do you want to do?\n\r");
          writeToQ("1) Delete your account\n\r");
          writeToQ("2) Delete a character in your account\n\r");
          writeToQ("3) Return to main account menu.\n\r-> ");
          break;
      }
      break;
    case CON_OLDPWD:
      EchoOn();
      if (!*pwd) {
        writeToQ("Incorrect password.\n\r");
        writeToQ("[Press return to continue]\n\r");
        account->status = TRUE;
        break;
      }
      crypted = (char *) crypt(arg, pwd);
      if (strncmp(crypted, pwd, 10)) {
        writeToQ("Incorrect password.\n\r");
        writeToQ("[Press return to continue]\n\r");
        account->status = TRUE;
        break;
      }
      writeToQ("Enter new password -> ");
      EchoOff();
      connected = CON_NEWPWD;
      break;
    case CON_NEWPWD:
      EchoOn();
      if (strlen(arg) < 5) {
        writeToQ("Your password must contain at least 5 characters.\n\r");
        writeToQ("Password -> ");
        EchoOff();
        return FALSE;
      } else if (strlen(arg) > 10) {
        writeToQ("Your password can only contain 10 or fewer characters.\n\r");
        writeToQ("Password -> ");
        EchoOff();
        return FALSE;
      }
      if (!hasDigit(arg)) {
        writeToQ("Your password must contain at least one number.\n\r");
        writeToQ("Password -> ");
        EchoOff();
        return FALSE;
      }
      crypted = (char *) crypt(arg, account->name);
      strncpy(pwd, crypted, 10);
      *(pwd + 10) = '\0';
      writeToQ("Retype your password for verification -> ");
      EchoOff();
      connected = CON_RETPWD;
      break;
    case CON_RETPWD:
      crypted = (char *) crypt(arg, pwd);
      EchoOn();
      if (strncmp(crypted, pwd, 10)) {
        writeToQ("Mismatched Passwords. Try again.\n\r");
        writeToQ("Retype password -> ");
        connected = CON_NEWPWD;
        EchoOff();
      } else {
        strcpy(account->passwd, pwd);
        account->status = TRUE;
        saveAccount();
        writeToQ("Password changed successfully.\n\r");
        writeToQ("[Press return to continue]\n\r");
      }
      break;
    case CON_PLYNG:
    case CON_NME:
    case CON_NMECNF:
    case CON_PWDNRM:
    case CON_QSEX:
    case CON_RMOTD:
    case CON_QCLASS:
    case CON_PWDNCNF:
    case CON_QRACE:
    case CON_STAT_COMBAT:
    case CON_QHANDS:
    case CON_DISCON:
    case CON_NEWACT:
    case CON_CONN:
    case CON_EDITTING:
    case CON_DISCLAIMER:
    case CON_DISCLAIMER2:
    case CON_DISCLAIMER3:
    case CON_STAT_LEARN:
    case CON_STAT_UTIL:
    case CON_CREATE_DONE:
    case CON_STATS_START:
    case CON_ENTER_DONE:
    case CON_STATS_RULES:
    case CON_STATS_RULES2:
    case CON_HOME_HUMAN:
    case CON_HOME_ELF:
    case CON_HOME_DWARF:
    case CON_HOME_GNOME:
    case CON_HOME_OGRE:
    case CON_HOME_HOBBIT:
    case MAX_CON_STATUS:
    case CON_REDITING:
    case CON_OEDITING:
    case CON_MEDITING:
    case CON_SEDITING:
    case CON_HELP:
    case CON_WRITING:
      vlogf(10, "Bad connectivity in doAccountStuff() (%d, %s, %s)", 
          connected, (character ? character->getName() : "false"), "0");
      vlogf(10, "Trying to delete it.");
      return DELETE_THIS;
  }
  return FALSE;
}

// returns DELETE_THIS
int Descriptor::doAccountMenu(const char *arg)
{
  string lStr;
  int count = 1;
  int tss = screen_size;

  if (client) {
    clientf("%d", CLIENT_MENU);
    return DELETE_THIS;
  }
  if (!connected) {
    vlogf(10, "DEBUG: doAM with !connected");
    return DELETE_THIS;
  }
#if 1
// COSMO TEST-fake color setting
  if (account->term == TERM_ANSI) {
    SET_BIT(plr_act, PLR_COLOR);
  }
#endif
  switch (*arg) {
    case 'E':
    case 'e':
      return DELETE_THIS;
    case 'A':
    case 'a':
      writeToQ("Enter the name for your character -> ");
      character = new TPerson(this);
      mud_assert(character != NULL, "Mem alloc problem");
      connected = CON_NME;
      break;
    case 'C':
    case 'c':
      writeToQ("Enter name of character -> ");
      character = new TPerson(this);
      connected = CON_CONN;
      break;
    case 'M':
    case 'm':
      sendMotd(FALSE);
      writeToQ("[Press return to continue]\n\r");
      break;
    case 'N':
    case 'n':
      start_page_file(NEWS_FILE, "No news today\n\r");
      break;
    case 'L':
    case 'l':
      count = listAccount(account->name, lStr);
      if (count == 0)
        writeToQ("No characters in account.\n\r");
      else
        writeToQ(lStr.c_str());

      writeToQ("\n\r");
      writeToQ("[Press return to continue]\n\r");
      break;
    case 'P':
    case 'p':
      writeToQ("Enter old password -> ");
      EchoOff();
      account->status = FALSE;
      connected = CON_OLDPWD;
      break; 
    case 'W':
    case 'w':
      menuWho();
      break;
    case 'D':
    case 'd':
      writeToQ("Which do you want to do?\n\r");
      writeToQ("1) Delete your account\n\r");
      writeToQ("2) Delete a character in your account\n\r");
      writeToQ("3) Return to main account menu.\n\r-> ");
      account->status = FALSE; //set status to FALSE to throw back into loop.
      connected = CON_DELETE;
      break;
    case 'H':
    case 'h':
      start_page_file("txt/accounthelp", "No help for account menu currently.");
      writeToQ("\n\r[Press return to continue]\n\r");
      break;
    case 'F':
    case 'f':
      writeToQ("Fingering other accounts is not yet enabled.\n\r");
      writeToQ("\n\r[Press return to continue]\n\r");
      break;
    default:
      count = ::number(1,3);
      if (!IS_SET(account->flags, ACCOUNT_BOSS)) {
        if (account->term == TERM_ANSI) {
          screen_size = 40;  // adjust for the size of the menu bar temporarily
         
          if (count == 1)
            start_page_file(ANSI_MENU_1, "");
          else if (count == 2)
            start_page_file(ANSI_MENU_2, "");
          else {
//            start_page_file(ANSI_MENU_3, "");
            string fileBuf;
            if (file_to_string(ANSI_MENU_3, fileBuf)) {
               fileBuf += "\n\r";
               page_string(fileBuf.c_str(), 0);
            }
          }
          screen_size = tss;
          writeToQ(ANSI_NORMAL);
        } else {
          screen_size = 40;  // adjust for the size of the menu bar temporarily
          if (count == 1)
            start_page_file(NORM_MENU_1, "");
          else if (count == 2)
            start_page_file(NORM_MENU_2, "");
          else 
            start_page_file(NORM_MENU_3, "");
          screen_size = tss;
        }
      } else {
        char buf[256];

        writeToQ("<C>onnect an existing character       <A>dd a new character\n\r");
        writeToQ("<D>elete account or character         <M>essage of the day\n\r");
        sprintf(buf, "<N>ews of %-25.25s   <F>inger an account\n\r", MUD_NAME);
        writeToQ(buf);
        writeToQ("<W>ho is in the game                  <P>assword change\n\r");
        writeToQ("<L>ist characters in account          <H>elp\n\r");
        sprintf(buf, "<E>xit %s\n\r", MUD_NAME);
        writeToQ(buf);
        writeToQ("\n\r-> ");
      }
      break;
  }
  return FALSE;
}

void Descriptor::saveAccount()
{
  FILE *fp;
  char buf[256], buf2[256];
  accountFile afp;

  if (!account || !account->name) {
    vlogf(10, "Bad descriptor in saveAccount");
    return;
  }
  sprintf(buf, "account/%c/%s/account", LOWER(account->name[0]), lower(account->name).c_str());
  if (!(fp = fopen(buf, "w"))) {
    sprintf(buf2, "account/%c/%s", LOWER(account->name[0]), lower(account->name).c_str());
    if (mkdir(buf2, 0770)) {
      vlogf(10, "Can't make directory for saveAccount (%s)", lower(account->name).c_str());
      return;
    }
    if (!(fp = fopen(buf, "w"))) {
      vlogf(10, "Big problems in saveAccount (s)", lower(account->name).c_str());
      return;
    }
  }
  // If we get here, fp should be valid
  memset(&afp, '\0', sizeof(afp));

  strcpy(afp.email, account->email);
  strcpy(afp.passwd, account->passwd);
  strcpy(afp.name, account->name);

  afp.birth = account->birth;
  afp.term = account->term;
  afp.time_adjust = account->time_adjust;
  afp.flags = account->flags;

  fwrite(&afp, sizeof(accountFile), 1, fp);
  fclose(fp);
}

void Descriptor::deleteAccount()
{
  DIR *dfd;
  struct dirent *dp;
  char buf[256];

  sprintf(buf, "account/%c/%s", LOWER(account->name[0]), lower(account->name).c_str());
  if (!(dfd = opendir(buf))) {
    vlogf(10, "Unable to walk directory for delete account (%s account)", account->name);
    return;
  }
  while ((dp = readdir(dfd))) {
    if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
      continue;

    sprintf(buf, "account/%c/%s/%s", LOWER(account->name[0]), lower(account->name).c_str(), dp->d_name);
    if (unlink(buf) != 0)
      vlogf(9, "error in unlink (4) (%s) %d", buf, errno);

    // these are in the dir, but are not "players"
    if (!strcmp(dp->d_name, "comment") ||
        !strcmp(dp->d_name, "account"))
      continue;

    wipePlayerFile(dp->d_name);
    wipeRentFile(dp->d_name);
    wipeFollowersFile(dp->d_name);
  }
  sprintf(buf, "account/%c/%s", LOWER(account->name[0]), lower(account->name).c_str());
  rmdir(buf);
  account_number--;
  closedir(dfd);
}


int Descriptor::inputProcessing()
{
  int sofar, thisround, bgin, squelch, i, k, flag;
  char tmp[20000], buffer[20000];
  int which = -1, total = 0, count = 0;
  char *s, *s2;

  sofar = 0;
  flag = 0;
  bgin = strlen(raw);

  do {
    if ((thisround = read(socket->sock, raw + bgin + sofar,
                          4096 - (bgin + sofar) - 1)) > 0) {
      sofar += thisround;
    } else {
      if (thisround < 0) {
        if (errno != EWOULDBLOCK) {
          perror("Read1 - ERROR");
          return (-1);
        } else 
          break;
        
      } else {
        vlogf(1, "EOF encountered on socket read.");
        return (-1);
      }
    }
  } while (!ISNEWL(*(raw + bgin + sofar - 1)));

  *(raw + bgin + sofar) = 0;

  for (i = bgin; !ISNEWL(*(raw + i)); i++)
    if (!*(raw + i))
      return (0);

  for (i = 0, k = 0; *(raw + i);) {
    if (!ISNEWL(*(raw + i)) && !(flag = (k >= (!client ? (MAX_INPUT_LENGTH - 2) : 4096)))) {
      if (*(raw + i) == '\b') {      // backspace 
        if (k) {                // more than one char ? 
          if (*(tmp + --k) == '$')
            k--;
          i++;
        } else
          i++;
      } else {
        if ((*(raw + i) == '\200') ||
            (isascii(*(raw + i)) && isprint(*(raw + i)))) {
          // trans char, double for '$' (printf)   
          if ((*(tmp + k) = *(raw + i)) == '$')
            *(tmp + ++k) = '$';
          k++;
          i++;
        } else
          i++;
      }
    } else {
      *(tmp + k) = 0;

      // New history related c-shell type commands - Russ 
      if (*tmp == '!') {
        if (!tmp[1] || (tmp[1] == '!'))
          strcpy(tmp, history[0]);
        else if ((tmp[1] >= '0') && (tmp[1] <= '9'))
          strcpy(tmp, history[ctoi(tmp[1])]);
        else {
          for (s = tmp + 1, k = 0; k <= 9; k++) {
            s2 = history[k];
            while (*s && *s2 && (*s == *s2)) {
              s++;
              s2++;
              count++;
            }
            if (count > total)
              which = k;

            count = 0;
          }
          if (which >= 0)
            strcpy(tmp, history[which]);
        }
      } else if (*tmp == '^') {
        if (history[9]) {
        }
      } else {
        // by default, put everything in history
        add_to_history_list(tmp);
      }

      input.putInQ(tmp);

      if (snoop.snoop_by && snoop.snoop_by->desc) {
        snoop.snoop_by->desc->output.putInQ("% ");
        snoop.snoop_by->desc->output.putInQ(tmp);
        snoop.snoop_by->desc->output.putInQ("\n\r");
      }
      if (flag) {
        sprintf(buffer, "Line too long. Truncated to:\n\r%s\n\r", tmp);
        if (socket && socket->writeToSocket(buffer) < 0) {
          return (-1);
        }

        // skip the rest of the line 
        for (; !ISNEWL(*(raw + i)); i++);
      }
      // find end of entry 
      for (; ISNEWL(*(raw + i)); i++);

      // squelch the entry from the buffer 
      for (squelch = 0;; squelch++) {
        if ((*(raw + squelch) = *(raw + i + squelch)) == '\0')
          break;
      }
      k = 0;
      i = 0;
    }
  }
  return TRUE;
}

void Descriptor::sendMotd(int wiz)
{
  char wizmotd[MAX_STRING_LENGTH] = "\0\0\0";
  char motd[MAX_STRING_LENGTH] = "\0\0\0";
  string version;
  struct stat timestat;

  file_to_string("txt/version", version);
  // file_to_str adds \n\r, strip both off
  size_t iter = version.find_last_not_of(" \n\r");
  if (iter != string::npos)
    version.erase(iter+1);

  strcpy(motd, "/****************************************************************************/\n\r");
  sprintf(motd + strlen(motd), "     Welcome to %s\n\r     %s\n\r\n\r", MUD_NAME_VERS, version.c_str());
  sprintf(motd + strlen(motd), "     Running on a dual processor Linux machine at Stanford University.\n\r\n\r");

  file_to_string(MOTD_FILE, version);
  // swap color strings
  version = colorString(character, this, version.c_str(), NULL, COLOR_BASIC,  false);
  strcat(motd, version.c_str());

  if (stat(NEWS_FILE, &timestat)) {
    vlogf(0,"bad call to news file");
    return;
  }

  sprintf(motd + strlen(motd), "\n\rREAD the NEWS LAST UPDATED       : %s\n\r",
                                ctime(&(timestat.st_mtime)));
  if (wiz) {
    file_to_string(WIZMOTD_FILE, version);
    // swap color strings
    version = colorString(character, this, version.c_str(), NULL, COLOR_BASIC,  false);
    strcat(motd, version.c_str());
    if (stat(WIZNEWS_FILE, &timestat)) {
      vlogf(0,"bad call to wiznews file");
      return;
    }
    sprintf(wizmotd + strlen(wizmotd), 
                              "\n\rREAD the WIZNEWS LAST UPDATED    : %s\n\r",
                               ctime(&(timestat.st_mtime)));
  }

  if (!client) {
    writeToQ(motd);
    if (wiz)
      writeToQ(wizmotd);
   
    return;
  } else {
    string sb;
    if (!wiz) {
      sb = motd;
    } else {
      sb = wizmotd;
    }
    processStringForClient(sb);
    clientf("%d|%s", CLIENT_MOTD, sb.c_str());
  }
}

bool textQ::takeFromQ(char *dest)
{
  commText *tmp = NULL;

  // Are we empty?
  if (!begin)
    return (0);
 
  if (begin->getText())
    strcpy(dest, begin->getText());
  else {
    vlogf(5, "There was a begin with no text but a next");
    return (0);
  }
  // store it off for later
  tmp = begin;

  // update linked list-- added the if 12/02/97 cos
  if ((begin = begin->getNext())) {
    if (begin->getNext() == begin) {
      begin->setNext(NULL);
      begin = NULL;
      vlogf(5, "Tell a coder, begin->next = begin");
    }
  } else {
    begin = NULL;
    end = NULL;
  }

  // trash the old text q : causes the old begin->text to be deleted
  delete tmp;
  tmp = NULL;

  return (1);
}

void textQ::putInQ(const char *txt)
{
  commText *n;
 
  n = new commText();
  if (!n) {
    vlogf(10, "Failed mem alloc in putInQ()");
    return;
  }
  char *tx = mud_str_dup(txt);
  n->setText(tx);
  n->setNext(NULL);
 
  if (!begin) {
    begin = end = n;
  } else {
    end->setNext(n);
    end = n;
  }
}

int TBeing::applyAutorentPenalties(int secs)
{
#if PENALIZE_FOR_AUTO_RENTING
  vlogf(0, "%s was autorented for %d secs", getName() ? getName() : "Unknown name", secs);

#endif
  return FALSE;
}

int TBeing::applyRentBenefits(int secs)
{
  int local_tics, rc = 0;
  affectedData *af = NULL, *next_af_dude = NULL;
  int amt, transFound = FALSE;

  // award healing for every 3 mins gone
  local_tics = secs * ONE_SECOND / PULSE_TICKS;
  local_tics /= 3;  // arbitrary

  vlogf(0, "%s was rented for %d secs, counting as %d tics out-of-game",
        getName(), secs, local_tics);

  setHit(min((int) hitLimit(), getHit() + (local_tics * hitGain())));
  setMana(min((int) manaLimit(), getMana() + (local_tics * manaGain())));
  setMove(min((int) moveLimit(), getMove() + (local_tics * moveGain())));
  setPiety(min(pietyLimit(), getPiety() + (local_tics * pietyGain(0.0))));
 
  wearSlotT ij;
  for (ij=MIN_WEAR;ij < MAX_WEAR; ij++) {
    amt = min(local_tics, getMaxLimbHealth(ij) - getCurLimbHealth(ij));  
    addCurLimbHealth(ij, amt);
  }

  // a stripped down updateAffects()
  for (af = affected; af; af = next_af_dude) {
    next_af_dude = af->next;

    if (af->duration == PERMANENT_DURATION)
      continue;
    if ((af->duration - (local_tics * UPDATES_PER_TICK)) <= 0) {
      if (((af->type >= MIN_SPELL) && (af->type < MAX_SKILL)) ||
          ((af->type >= FIRST_TRANSFORMED_LIMB) && (af->type < LAST_TRANSFORMED_LIMB)) ||
          ((af->type >= FIRST_BREATH_WEAPON) && (af->type < LAST_BREATH_WEAPON)) ||
          ((af->type >= FIRST_ODDBALL_AFFECT) && (af->type < LAST_ODDBALL_AFFECT))) {
        if (af->shouldGenerateText() ||
            (af->next->duration > 0)) {
          if (af->type == AFFECT_DISEASE)
            diseaseStop(af);
          else {
            rc = spellWearOff(af->type);
            if (IS_SET_DELETE(rc, DELETE_THIS))
              return DELETE_THIS;
          }
        }
        if ((af->type == AFFECT_TRANSFORMED_ARMS) || 
            (af->type == AFFECT_TRANSFORMED_HANDS) ||
            (af->type == AFFECT_TRANSFORMED_LEGS) ||
            (af->type == AFFECT_TRANSFORMED_HEAD) ||
            (af->type == AFFECT_TRANSFORMED_NECK)) {
           transFound = TRUE;
        }
        affectRemove(af);
      }
    } else 
      af->duration -= local_tics * UPDATES_PER_TICK;
  }
  if (transFound)
    transformLimbsBack("", MAX_WEAR, FALSE);

  if (secs >= 6 * SECS_PER_REAL_HOUR) {
    sendTo("A full night's rest has restored your body!\n\r");
    if (!isImmortal()) {
      setCond(FULL, 24);
      setCond(THIRST, 24);
      setCond(DRUNK, 0);
    }
  }
  return FALSE;
}

bool illegalEmail(char *buf, Descriptor *desc, silentTypeT silent)
{
  char arg[81];
  char username[20], host[80];
  char *s;

  strcpy(arg, buf);

  if (!*arg) {
    if (desc && !silent)
      desc->writeToQ("You must enter an email address.\n\r");
    return TRUE;
  }
  if (strlen(arg) > 60) {
    if (desc && !silent)
      desc->writeToQ("Email addresses may not exceed 60 characters.\n\r");
    return TRUE;
  }
  if (!(s = strchr(arg, '@'))) {
    if (desc && !silent)
      desc->writeToQ("Email addresses must be of the form <user>@<host>.\n\r");
    return TRUE;
  }
  s = strtok(arg, "@");
  if (!s) {
    if (desc && !silent)
      desc->writeToQ("You must enter a username.\n\r");
    return TRUE;
  }
  strcpy(username, s);
 
  s = strtok(NULL, "@");
  if (!s) {
    if (desc && !silent)
      desc->writeToQ("You must enter a host.\n\r");
    return TRUE;
  }
  strcpy(host, s);

  if ((s = strchr(username, ' '))) {
    if (desc && !silent) {
      desc->writeToQ("User names can not have spaces in them.\n\r");
    }
    return TRUE;
  }

  if ((s = strchr(host, ' '))) {
    if (desc && !silent)
      desc->writeToQ("Host names can not have spaces in them.\n\r");

    return TRUE;
  }

  strcpy(host, lower(host).c_str());

  if (strstr(host, "localhost") || strstr(host, "127.0.0.1")) {
    if (desc && !silent)
      desc->writeToQ("Apologies, but due to abuse, that host is disallowed.\n\r");

    return TRUE;
  }

#if 0
// This works, but was deemed a bad idea...
  char tempBuf[256]; 
  struct hostent *hst;

  hst = gethostbyname(host);
  if (!hst) {
    if (desc && !silent) {
      sprintf(tempBuf, "Host '%s' does not seem to be valid.\n\r", host); 
      desc->writeToQ(tempBuf);
      sprintf(tempBuf, "If this is a valid host, please send e-mail to %s\n\r\n\r\n\r", MUDADMIN_EMAIL);
      desc->writeToQ(tempBuf);
    }
    return TRUE;
  }
#endif

  return FALSE;
}

commText::commText()
      : text(NULL), next(NULL)
{
}

commText::commText(const commText &a)
{
  text = mud_str_dup(a.text);
  if (a.next)
    next = new commText(*a.next);
  else
    next = NULL;
}

commText & commText::operator=(const commText &a)
{
  if (this == &a) return *this;

  delete [] text;
  text = mud_str_dup(a.text);

  commText *ct;
  while ((ct = next)) {
    next = ct->next;
    delete ct;
  }

  if (a.next)
    next = new commText(*a.next);
  else
    next = NULL;
  return *this;
}

commText::~commText()
{
  if (text) {
    delete [] text;
    text = NULL;
  }
}

textQ::textQ(bool n) :
  begin(NULL),
  end(NULL)
{
}

textQ::textQ(const textQ &a) :
  begin(NULL),
  end(NULL)
{
  if (a.begin)
    begin = new commText(*a.begin);
  else
    begin = NULL;

  if (a.begin) {
    commText *ct = NULL;
    for (ct = begin; ct->getNext(); ct = ct->getNext());
    end = ct;
  } else
    end = NULL;
}

textQ & textQ::operator=(const textQ &a)
{
  if (this == &a) return *this;
  commText *ct, *ct2;
  for (ct = begin; ct; ct = ct2) {
    ct2 = ct->getNext();
    delete ct;
  }
  if (a.begin)
    begin = new commText(*a.begin);
  else
    begin = NULL;

  if (a.begin) {
    for (ct = begin; ct->getNext(); ct = ct->getNext());
    end = ct;
  } else
    end = NULL;

  return *this;
}

textQ::~textQ()
{
  commText *ct, *ct2;
  for (ct = begin; ct; ct = ct2) {
    ct2 = ct->getNext();
    delete ct;
  }
}

editStuff::editStuff()
  : x(1), y(1),
    bottom(0), end(0), lines(NULL)
{
}

editStuff::editStuff(const editStuff &a)
  : x(a.x), y(a.y),
    bottom(a.bottom), end(a.end)
{
  lines = a.lines;  // not positive this is correct
}

editStuff::~editStuff()
{
}

careerData::careerData()
{
  setToZero();
}

careerData::~careerData()
{
}

sessionData::sessionData()
{
  setToZero();
}

sessionData::~sessionData()
{
}

promptData::promptData() :
  type(0),
  prompt(NULL),
  xptnl(0.0)
{
  *hpColor = *moneyColor = *manaColor = *moveColor = *expColor = '\0';
  *roomColor = *oppColor = *tankColor = '\0';

//  for (classIndT i = MIN_CLASS_IND; i < MAX_CLASSES; i++)
//    xptnl[i] = 0.0;
}

promptData::~promptData()
{
}
