//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: parse.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//      SneezyMUD++ - All rights reserved, SneezyMUD Coding Team
//      "parse.cc" - All functions and routines related to command parsing
//      
//
///////////////////////////////////////////////////////////////////////////

extern "C" {
#include <unistd.h>
#include <arpa/telnet.h>
}

#include "stdsneezy.h"
#include "mail.h"

string lockmess;
commandInfo *commandArray[MAX_CMD_LIST];
bool WizLock;

static const char *one_arg(const char *s, char *a)
{
  for (; isspace(*s); s++);
  for (; !isspace(*a = *s) && *s; s++, a++);
  *a = '\0';

  return s;
}

int search_block(const char *arg, const char * const *list, bool exact)
{
  register int i, l;

  l = strlen(arg);

  if (exact) {
    for (i = 0; **(list + i) != '\n'; i++)
      if (!strcasecmp(arg, *(list + i)))
        return (i);
  } else {
    if (!l)
      l = 1;

    for (i = 0; **(list + i) != '\n'; i++)
      if (!strncasecmp(arg, *(list + i), l))
        return (i);
  }
  return (-1);
}


int old_search_block(char *argument, int bgin, int length, const char * const * list, bool mode)
{
  int guess, search;
  bool found;

  // If the word contain 0 letters, then a match is already found 
  found = (length < 1);

  guess = 0;

  if (mode)
    while (!found && *(list[guess]) != '\n') {
      found = (length == (int) strlen(list[guess]));
      for (search = 0; (search < length && found); search++)
        found = (*(argument + bgin + search) == *(list[guess] + search));
      guess++;
  } else {
    while (!found && *(list[guess]) != '\n') {
      found = 1;
      for (search = 0; (search < length && found); search++)
        found = (*(argument + bgin + search) == *(list[guess] + search));
      guess++;
    }
  }
  return (found ? guess : -1);
}

cmdTypeT searchForCommandNum(const char *argument)
{
  char buf[256];
  cmdTypeT i;

  sprintf(buf, "%s", argument);
  for (i = MIN_CMD; i < MAX_CMD_LIST; i++) {
    if (!commandArray[i])
      continue;
    if (is_abbrev(buf, commandArray[i]->name))
      return i;
  }

  return MAX_CMD_LIST;
}

void TBeing::incorrectCommand() const
{
  sendTo("%sIncorrect%s command. Please see help files if you need assistance!\n\r", red(), norm());
}

extern int handleMobileResponse(TBeing *, cmdTypeT, const char *);

// returns DELETE_THIS if this should be nuked
// returns DELETE_VICT if vict should be nuked
// otherwise returns FALSE
int TBeing::doCommand(cmdTypeT cmd, const char *argument, TThing *vict, bool typedIn)
{
  int rc = 0;
  TBeing *ch;
  bool isPoly = FALSE;
  char newarg[1024];

  for (;isspace(*argument);argument++);

  if (!strcasecmp(argument, "self") || !strcasecmp(argument, "me"))
    strcpy(newarg, getName());
  else
    strcpy(newarg, argument);

  if (typedIn && desc && dynamic_cast<TMonster *>(this)) 
    isPoly = TRUE;
  
  if (GetMaxLevel() < commandArray[cmd]->minLevel &&
       ((cmd != CMD_WIZNET) || !desc || !desc->original ||
          desc->original->GetMaxLevel() < commandArray[cmd]->minLevel)) {
    incorrectCommand();
    return FALSE;
  }
  if (isAffected(AFF_PARALYSIS) &&
      commandArray[cmd]->minPosition > POSITION_STUNNED) {
    sendTo("You are paralyzed, you can't do much of anything!\n\r");
    return FALSE;
  }
  if (isAffected(AFF_STUNNED) &&
      commandArray[cmd]->minPosition > POSITION_STUNNED) {
    sendTo("You are stunned, you can't do much of anything!\n\r");
    return FALSE;
  }
  if (1) {
    if ((commandArray[cmd]->minPosition >= POSITION_CRAWLING) && fight()){
      sendTo("You can't concentrate enough while fighting!\n\r");
      return FALSE;
    } else if (getPosition() < commandArray[cmd]->minPosition) {
      switch (getPosition()) {
        case POSITION_DEAD:
          sendTo("You cannot do that while dead!\n\r");
          break;
        case POSITION_INCAP:
        case POSITION_MORTALLYW:
          sendTo("You cannot do that while mortally wounded!\n\r");
          break;
        case POSITION_STUNNED:
          sendTo("You cannot do that while stunned!!\n\r");
          break;
        case POSITION_SLEEPING:
          sendTo("You cannot do that while sleeping!\n\r");
          break;
        case POSITION_RESTING:
          sendTo("You cannot do that while resting!\n\r");
          break;
        case POSITION_SITTING:
          sendTo("You cannot do that while sitting!?\n\r");
          break;
        case POSITION_CRAWLING:
          sendTo("You cannot do that while crawling!\n\r");
          break;
        case POSITION_ENGAGED:
          sendTo("You cannot do that while engaged!\n\r");
          break;
        case POSITION_FIGHTING:
          sendTo("You cannot do that while fighting!\n\r");
          break;
        case POSITION_STANDING:
          sendTo("You cannot do that while standing!\n\r");
          break;
        case POSITION_MOUNTED:
          sendTo("You cannot do that while mounted!\n\r");
          break;
        case POSITION_FLYING:
          sendTo("You cannot do that while flying!\n\r");
          break;
      }
    } else {
      ch = ((desc && desc->original) ? desc->original : this);
      if ((ch->GetMaxLevel() > MAX_MORT && ch->isPc()) &&
        (strcmp(ch->name, "Batopr") &&
         strcmp(ch->name, "Cosmo") &&
         strcmp(ch->name, "Dolgan") &&
         strcmp(ch->name, "Kriebly") &&
         strcmp(ch->name, "Mithros") &&
         strcmp(ch->name, "Marsh") &&
         strcmp(ch->name, "Spawn") &&
         strcmp(ch->name, "Lapsos") &&
         strcmp(ch->name, "Brutius") &&
	 strcmp(ch->name, "Peel"))) { 

        if (ch == this)
          vlogf(-1, "%s:%s %s", name, commandArray[cmd]->name, newarg);
        else
          vlogf(-1, "%s (%s):%s %s", name, desc->original->name, 
                commandArray[cmd]->name, newarg);
      } else if (ch->isPc() && ch->isPlayerAction(PLR_LOGGED))
        vlogf(-1, "%s %s%s", name, commandArray[cmd]->name, newarg);
      else if (numberLogHosts && desc) {
        for (int a = 0; a < numberLogHosts; a++) {
          if (strcasestr(desc->host, hostLogList[a]))
            vlogf(-1, "%s %s%s", name, commandArray[cmd]->name, newarg);
        }
      }
      rc = triggerSpecial(NULL, cmd, newarg);
      if (IS_SET_DELETE(rc, DELETE_THIS)) 
        return DELETE_THIS;
      else if (rc) 
        return FALSE;

      // slowly switching to strings over char *
      const string tmpstring = newarg;

      switch(cmd) {
        case CMD_UNSADDLE:
          doUnsaddle(newarg);
          break;
        case CMD_SPRINGLEAP:
          doSpringleap(newarg, true, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_SADDLE:
          doSaddle(newarg);
          break;
        case CMD_CONCEAL:
          doConceal(newarg);
          break;
        case CMD_RESTRING:
          doRestring(newarg);
          break;
        case CMD_RESET:
          doReset(newarg);
          break;
        case CMD_RELEASE:
          doRelease(tmpstring);
          break;
        case CMD_CRIT:
          rc = doCrit(newarg);
          break;
        case CMD_CLIENTS:
          doClients();
          break;
        case CMD_CAPTURE:
          doCapture(tmpstring);
          break;
        case CMD_HEAVEN:
          doHeaven(newarg);
          break;
        case CMD_REFUEL:
          doRefuel(newarg);
          break;
        case CMD_REPLY:
          doReply(newarg);
          break;
        case CMD_USE:
          rc = doUse(newarg);
          break;
        case CMD_DRAG:
          doDrag(newarg);
          break;
        case CMD_DISSECT:
          rc = doDissect(newarg);
          break;
        case CMD_DISARM:
          rc = doDisarm(newarg, vict);
          break;
        case CMD_EXEC:
          rc = doExec();
          break;
        case CMD_QUAFF:
          rc = doQuaff(newarg);
          break;
        case CMD_GUARD:
        case CMD_PROTECT:
          doGuard(newarg);
          break;
        case CMD_ORDER:
          rc = doOrder(newarg);
          break;
        case CMD_HISTORY:
          doHistory();
          break;
        case CMD_PEEK:
          doPeek();
          break;
        case CMD_BUG:
          doBug(newarg);
          break;
        case CMD_IDEA:
          doIdea(newarg);
          break;
        case CMD_TYPO:
          doTypo(newarg);
          break;
        case CMD_NORTH:
        case CMD_SOUTH:
        case CMD_WEST:
        case CMD_EAST:
        case CMD_UP:
        case CMD_DOWN:
        case CMD_NE:
        case CMD_SW:
        case CMD_SE:
        case CMD_NW:
          rc = doMove(cmd);
          break;
        case CMD_TRACEROUTE:
          doSysTraceroute(newarg);
          break;
        case CMD_MID:
          doSysMid();
          break;
        case CMD_VIEWOUTPUT:
          doSysViewoutput();
          break;
        case CMD_TASKS:
          doSysTasks(newarg);
          break;
        case CMD_SAY:
        case CMD_SAY2:
          rc = doSay(argument);
          break;
        case CMD_LOOK:
          doLook(newarg, cmd);
          break;
        case CMD_ADJUST:
          doAdjust(newarg);
          break;
        case CMD_FACTIONS:
          doFactions(newarg);
          break;
        case CMD_LIST:
          if ((rc = handleMobileResponse(this, cmd, newarg)))
            break;

          doList(newarg);
          break;
        case CMD_RENT:
          rc = doRent(newarg);
          break;
        case CMD_BUY:
          if ((rc = handleMobileResponse(this, cmd, newarg)))
            break;
        case CMD_TWIST:
        case CMD_PRESS:
        case CMD_PUSH:
        case CMD_SELL:
        case CMD_VALUE:
        case CMD_OFFER:
        case CMD_BALANCE:
        case CMD_WITHDRAW:
        case CMD_DEPOSIT:
        case CMD_CHECK:
        case CMD_RECEIVE:
        case CMD_MAIL:
        case CMD_PULL:
        case CMD_POST:
        case CMD_BREAK:
        case CMD_GAIN:
        case CMD_CLIMB:
        case CMD_DESCEND:
        case CMD_CHIP:
        case CMD_DIG:
        case CMD_COVER:
        case CMD_OPERATE:
        case CMD_ABORT:
          doNotHere();
          break;
        case CMD_MEND_LIMB:
          rc = doMendLimb(newarg);
          break;
        case CMD_LONGDESCR:
          doLongDescr(newarg);
          break;
        case CMD_TITHE:
          rc = doTithe();
          break;
        case CMD_ACCOUNT:
          doAccount(newarg);
          break;
        case CMD_FILL:
          doFill(newarg);
          break;
        case CMD_BOUNCE:
        case CMD_DANCE:
        case CMD_SMILE:
        case CMD_CACKLE:
        case CMD_LAUGH:
        case CMD_GIGGLE:
        case CMD_SHAKE:
        case CMD_PUKE:
        case CMD_GROWL:
        case CMD_SCREAM:
        case CMD_COMFORT:
        case CMD_NOD:
        case CMD_SIGH:
        case CMD_SULK:
        case CMD_HUG:
        case CMD_SNUGGLE:
        case CMD_CUDDLE:
        case CMD_NUZZLE:
        case CMD_CRY:
        case CMD_POKE:
        case CMD_ACCUSE:
        case CMD_GRIN:
        case CMD_BOW:
        case CMD_APPLAUD:
        case CMD_BLUSH:
        case CMD_BURP:
        case CMD_CHUCKLE:
        case CMD_CLAP:
        case CMD_COUGH:
        case CMD_CURTSEY:
        case CMD_FART:
        case CMD_FLIP:
        case CMD_FONDLE:
        case CMD_FROWN:
        case CMD_GASP:
        case CMD_GLARE:
        case CMD_GROAN:
        case CMD_GROPE:
        case CMD_HICCUP:
        case CMD_LICK:
        case CMD_LOVE:
        case CMD_MOAN:
        case CMD_NIBBLE:
        case CMD_POUT:
        case CMD_PURR:
        case CMD_RUFFLE:
        case CMD_SHIVER:
        case CMD_SHRUG:
        case CMD_SING:
        case CMD_SLAP:
        case CMD_SMIRK:
        case CMD_SNAP:
        case CMD_SNEEZE:
        case CMD_SNICKER:
        case CMD_SNIFF:
        case CMD_SNORE:
        case CMD_SPIT:
        case CMD_SQUEEZE:
        case CMD_STARE:
        case CMD_STRUT:
        case CMD_THANK:
        case CMD_TWIDDLE:
        case CMD_WAVE:
        case CMD_WHISTLE:
        case CMD_WIGGLE:
        case CMD_WINK:
        case CMD_YAWN:
        case CMD_SNOWBALL:
        case CMD_FRENCH:
        case CMD_COMB:
        case CMD_MASSAGE:
        case CMD_TICKLE:
        case CMD_PAT:
        case CMD_CURSE:
        case CMD_BEG:
        case CMD_BLEED:
        case CMD_CRINGE:
        case CMD_DAYDREAM:
        case CMD_FUME:
        case CMD_GROVEL:
        case CMD_HOP:
        case CMD_NUDGE:
        case CMD_PEER:
        case CMD_PONDER:
        case CMD_PUNCH:
        case CMD_SNARL:
        case CMD_SPANK:
        case CMD_STEAM:
        case CMD_TACKLE:
        case CMD_TAUNT:
        case CMD_WHINE:
        case CMD_WORSHIP:
        case CMD_YODEL:
        case CMD_THINK:
        case CMD_WHAP:
        case CMD_BEAM:
        case CMD_CHORTLE:
        case CMD_BONK:
        case CMD_SCOLD:
        case CMD_DROOL:
        case CMD_RIP:
        case CMD_STRETCH:
        case CMD_PIMP:
        case CMD_BELITTLE:
        case CMD_TAP:
        case CMD_PILEDRIVE:
        case CMD_FLIPOFF:
        case CMD_MOON:
        case CMD_PINCH:
        case CMD_BITE:
        case CMD_KISS:
        case CMD_CHEER:
        case CMD_WOO:
        case CMD_GRUMBLE:
        case CMD_APOLOGIZE:
        case CMD_AGREE:
        case CMD_DISAGREE:
        case CMD_SPAM:
        case CMD_ARCH:
        case CMD_ROLL:
        case CMD_BLINK:
        case CMD_FAINT:
        case CMD_GREET:
        case CMD_TIP:
        case CMD_BOP:
        case CMD_JUMP:
        case CMD_WHIMPER:
        case CMD_SNEER:
        case CMD_MOO:
        case CMD_BOGGLE:
        case CMD_SNORT:
        case CMD_TANGO:
        case CMD_ROAR:
        case CMD_FLEX:
        case CMD_TUG:
        case CMD_CROSS:
        case CMD_HOWL:
        case CMD_GRUNT:
        case CMD_WEDGIE:
        case CMD_SCUFF:
        case CMD_NOOGIE:
        case CMD_BRANDISH:
        case CMD_TRIP:
        case CMD_DUCK:
        case CMD_BECKON:
        case CMD_WINCE:
        case CMD_HUM:
        case CMD_RAZZ:
        case CMD_GAG:
        case CMD_AVERT:
        case CMD_SALUTE:
        case CMD_PET:
        case CMD_GRIMACE:
          rc = doAction(tmpstring, cmd);
          break;
        case CMD_POINT:
          doPoint(newarg);
          break;
        case CMD_AS:
          rc = doAs(newarg);
          break;
        case CMD_AT:
          rc = doAt(newarg, false);
          break;
        case CMD_GIVE:
          rc = doGive(newarg);
          break;
        case CMD_TAKE:
        case CMD_GET:
          rc = doGet(newarg);
          break;
        case CMD_DROP:
          rc = doDrop(newarg, vict);
          break;
        case CMD_SAVE:
          doSave(SILENT_NO, newarg);
          break;
        case CMD_DEATHCHECK:
          doDeathcheck(newarg);
          break;
        case CMD_CHECKLOG:
          doSysChecklog(newarg);
          break;
        case CMD_RECITE:
          rc = doRecite(newarg);
          break;
        case CMD_RESTORE:
          doRestore(newarg);
          break;
        case CMD_EMOTE:
        case CMD_EMOTE2:
        case CMD_EMOTE3:
          rc = doEmote(newarg);
          break;
        case CMD_ECHO: 
          doEcho(newarg);
          break;
        case CMD_BAMFIN: 
          doBamfin(newarg);
          break;
        case CMD_BAMFOUT: 
          doBamfout(newarg);
          break;
        case CMD_SHOW:
          doShow(newarg);
          break;
        case CMD_HIGHFIVE:
          doHighfive(newarg);
          break;
        case CMD_TOGGLE:
          doToggle(newarg);
          break;
        case CMD_WIZLOCK:
          doWizlock(newarg);
          break;
        case CMD_FLAG:
          doFlag(newarg);
          break;
        case CMD_SYSTEM:
          doSystem(newarg);
          break;
        case CMD_TRANSFER:
          doTrans(newarg);
          break;
        case CMD_SWITCH:
          doSwitch(newarg);
          break;
        case CMD_CUTLINK:
          doCutlink(newarg);
          break;
        case CMD_WIZNEWS:
          doWiznews();
          break;
        case CMD_AUTO:
          doAuto(newarg);
          break;
        case CMD_NOSHOUT:
          doNoshout(argument);
          break;
        case CMD_STEAL:
          rc = doSteal(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_STEALTH:
          doStealth(newarg);
          break;
        case CMD_INVISIBLE:
          doInvis(newarg);
          break;
        case CMD_VISIBLE:
          doVisible(newarg, false);
          break;
        case CMD_LOGLIST:
          doSysLoglist();
          break;
        case CMD_LEAVE:
          rc = doLeave(newarg);
          break;
        case CMD_EXITS:
          rc = doExits(newarg, cmd);
          break;
        case CMD_WIPE:
          doWipe(newarg);
          break;
        case CMD_ACCESS:
          doAccess(newarg);
          break;
        case CMD_REPLACE:
          doReplace(newarg);
          break;
        case CMD_SETSEV:
          doSetsev(newarg);
          break;
        case CMD_INFO:
          doInfo(newarg);
          break;
        case CMD_TIMESHIFT:
          doTimeshift(newarg);
          break;
        case CMD_LOG:
          doLog(newarg);
          break;
        case CMD_HOSTLOG:
          doHostlog(newarg);
          break;
        case CMD_ASSIST:
          rc = doAssist(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_WIMPY:
          doWimpy(newarg);
          break;
        case CMD_WHO:
          doWho(newarg);
          break;
        case CMD_BRUTTEST:
          doBruttest(newarg);
          break;
        case CMD_PEELPK:
	  doPeelPk(newarg);
	  break;
        case CMD_SHUTDOW:
          doShutdow();
          break;
        case CMD_SHUTDOWN:
          doShutdown(newarg);
          break;
        case CMD_LOAD:
          doLoad(newarg);
          break;
        case CMD_GOTO:
          rc = doGoto(tmpstring);
          break;
        case CMD_SHOUT:
          doShout(argument);
          break;
        case CMD_CLIENTMESSAGE:
          doClientMessage(argument);
          break;
        case CMD_GT:
          doGrouptell(argument);
          break;
        case CMD_SIGN:
          doSign(newarg);
          break;
        case CMD_TELL:
          rc = doTell(argument);
          break;
        case CMD_WHISPER:
          rc = doWhisper(newarg);
          break;
        case CMD_ASK:
          rc = doAsk(newarg);
          break;
        case CMD_WRITE:
          doWrite(newarg);
          break;
        case CMD_WHOZONE:
          doWhozone();
          break;
        case CMD_EXAMINE:
          doExamine(newarg);
          break;
        case CMD_SCORE:
          doScore();
          break;
        case CMD_WIZHELP:
          doWizhelp();
          break;
        case CMD_WIZLIST:
          doWizlist();
          break;
        case CMD_INVENTORY:
          doInventory(newarg);
          break;
        case CMD_STAND:
          doStand();
          break;
        case CMD_SIT:
          doSit(tmpstring);
          break;
        case CMD_RAISE:
        case CMD_LIFT:
          rc = doRaise(newarg, cmd);
          break;
        case CMD_OPEN:
          rc = doOpen(newarg);
          break;
        case CMD_REST:
          doRest(tmpstring);
          break;
        case CMD_LOWER:
          rc = doLower(newarg);
          break;
        case CMD_CLOSE:
          doClose(newarg);
          break;
        case CMD_LOCK:
          doLock(newarg);
          break;
        case CMD_UNLOCK:
          doUnlock(newarg);
          break;
        case CMD_SLEEP:
          doSleep(tmpstring);
          break;
        case CMD_WAKE:
          doWake(newarg);
          break;
        case CMD_TIME:
          doTime(newarg);
          break;
        case CMD_WEATHER:
          doWeather(newarg);
          break;
        case CMD_USERS:
          doUsers(newarg);
          break;
        case CMD_EQUIPMENT:
          doEquipment(newarg);
          break;
        case CMD_QUIT:
          doQuit();
          break;
        case CMD_QUIT2:
          rc = doQuit2();
          break;
        case CMD_CREDITS:
          doCredits();
          break;
        case CMD_NEWS:
          doNews();
          break;
        case CMD_WHERE:
          doWhere(newarg);
          break;
        case CMD_LEVELS:
          doLevels(newarg);
          break;
        case CMD_CONSIDER:
          doConsider(newarg);
          break;
        case CMD_WORLD:
          doWorld();
          break;
        case CMD_ATTRIBUTE:
          doAttribute(newarg);
          break;
        case CMD_CLEAR:
          doClear(newarg);
          break;
        case CMD_ALIAS:
          doAlias(newarg);
          break;
        case CMD_GLANCE:
          doGlance(newarg);
          break;
        case CMD_MOTD:
          doMotd(newarg);
          break;
        case CMD_LIMBS:
          doLimbs(tmpstring);
          break;
        case CMD_BREATH:
          doBreath(newarg);
          break;
        case CMD_NOHASSLE:
          doNohassle(newarg);
          break;
        case CMD_CHANGE:
          doChange(newarg);
          break;
        case CMD_PROMPT:
          doPrompt(newarg);
          break;
        case CMD_REMOVE:
          rc = doRemove(newarg, vict);
          break;
        case CMD_WEAR:
          doWear(newarg);
          break;
        case CMD_STAT:
          doStat(newarg);
          break;
        case CMD_PURGE:
          doPurge(newarg);
          break;
        case CMD_SET:
          doSet(newarg);
          break;
        case CMD_COMMAND:
          doCommand(newarg);
          break;
        case CMD_WIZNET:
          doCommune(newarg);
          break;
        case CMD_WIELD:
          doWield(newarg);
          break;
        case CMD_GRAB:
        case CMD_HOLD:
          doGrab(newarg);
          break;
        case CMD_PUT:
          rc = doPut(newarg);
          break;
        case CMD_KILL:
        case CMD_SLAY:
          rc = doKill(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_HIT:
          rc = doHit(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_ENGAGE:
          rc = doEngage(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_DISENGAGE:
          rc = doDisengage();
          break;
        case CMD_QUEST:
          doQuest(newarg);
          break;
        case CMD_TESTCODE:
          doTestCode(newarg);
          break;
        case CMD_FOLLOW:
          doFollow(newarg);
          break;
        case CMD_RETURN:
          doReturn(newarg, WEAR_NOWHERE, true);
          break;
        case CMD_REPORT:
          doReport(newarg);
          break;
        case CMD_PRAY:
          rc = doPray(newarg);
          break;
        case CMD_CAST:
          rc = doCast(newarg);
          break;
        case CMD_CONTINUE:
          doContinue(newarg);
          break;
        case CMD_READ:
          doRead(newarg);
          break;
        case CMD_EAT:
          doEat(newarg);
          break;
        case CMD_DRINK:
          rc = doDrink(newarg);
          break;
        case CMD_POUR:
          rc = doPour(newarg);
          break;
        case CMD_SIP:
          doSip(newarg);
          break;
        case CMD_TASTE:
          doTaste(newarg);
          break;
        case CMD_BERSERK:
          rc = doBerserk();
          break;
        case CMD_SHOVE:
          rc = doShove(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_GRAPPLE:
          rc = doGrapple(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_RESCUE:
          rc = doRescue(newarg);
          break;
        case CMD_DEATHSTROKE:
          rc = doDeathstroke(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_BODYSLAM:
          rc = doBodyslam(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_STOMP:
          rc = doStomp(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_EMAIL:
          doEmail(newarg);
          break;
        case CMD_HEADBUTT:
          rc = doHeadbutt(newarg, dynamic_cast<TBeing *>(vict));
          break;
 	case CMD_KNEESTRIKE:
	  rc = doKneestrike(newarg, dynamic_cast<TBeing *>(vict));
 	  break;
        case CMD_DOORBASH:
          rc = doDoorbash(tmpstring);
          break;
        case CMD_ATTUNE:
          doAttune(newarg);
          break;
        case CMD_AFK:
          doAfk();
          break;
        case CMD_SHARPEN:
          doSharpen(newarg);
          break;
        case CMD_DULL:
          doDull(newarg);
          break;
        case CMD_REPAIR:
          doRepair(newarg);
          break;
        case CMD_BANDAGE:
          doBandage(newarg);
          break;
        case CMD_SET_TRAP:
          rc = doSetTraps(newarg);
          break;
        case CMD_PICK:
          rc = doPick(newarg);
          break;
        case CMD_SEARCH:
          rc = doSearch(newarg);
          break;
        case CMD_SPY:
          rc = doSpy();
          break;
        case CMD_DODGE:
          rc = doDodge();
          break;
        case CMD_INSULT:
          doInsult(newarg);
          break;
        case CMD_SCRATCH:
          doScratch(newarg);
          break;
        case CMD_PEE:
          doPee(newarg);
          break;
        case CMD_EDIT:
          doEdit(newarg);
          break;
        case CMD_RLOAD:
          doRload(newarg);
          break;
        case CMD_RSAVE:
          doRsave(newarg);
          break;
        case CMD_REDIT:
          doRedit(newarg);
          break;
        case CMD_GROUP:
          doGroup(newarg);
          break;
        case CMD_FLEE:
          rc = doFlee(newarg);
          break;
        case CMD_BREW:
          doBrew(newarg);
          break;
        case CMD_SCRIBE:
          doScribe(newarg);
          break;
        case CMD_TURN:
          doTurn(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_YOGINSA:
          doYoginsa();
          break;
        case CMD_MEDITATE:
          doMeditate();
          break;
        case CMD_PENANCE:
          doPenance();
          break;
        case CMD_PRACTICE:
          doPractice(newarg);
          break;
        case CMD_BLOAD:
          doBload(newarg);
          break;
        case CMD_THROW:
          doThrow(newarg);
          break;
        case CMD_SHOOT:
          rc = doShoot(newarg);
          break;
        case CMD_TRACK:
          doTrack(newarg);
          break;
        case CMD_SEEKWATER:
          doSeekwater();
          break;
        case CMD_MEDIT:
          doMedit(newarg);
          break;
        case CMD_SEDIT:
          doSEdit(newarg);
          break;
        case CMD_LAYHANDS:
          rc = doLayHands(newarg);
          break;
        case CMD_STAY:
          doStay();
          break;
        case CMD_PASS:
          doPass(newarg);
          break;
        case CMD_DEAL:
          doDeal(newarg);
          break;
        case CMD_ATTACK:
          doAttack(newarg);
          break;
        case CMD_BET:
          doBet(newarg);
          break;
        case CMD_OEDIT:
          doOEdit(newarg);
          break;
        case CMD_MAKELEADER:
          doMakeLeader(newarg);
          break;
        case CMD_NEWMEMBER:
          doNewMember(newarg);
          break;
        case CMD_SEND:
          doSend(newarg);
          break;
        case CMD_RMEMBER:
          doRMember(newarg);
          break;
        case CMD_HELP:
          doHelp(newarg);
          break;
        case CMD_PLAY:
          doPlay(newarg);
          break;
        case CMD_SORT:
          doSort(newarg);
          break;
        case CMD_GAMESTATS:
          doGamestats(newarg);
          break;
        case CMD_SCAN:
          doScan(newarg);
          break;
        case CMD_FEIGNDEATH:
          rc = doFeignDeath();
          break;
        case CMD_JUNK:
          rc = doJunk(newarg, dynamic_cast<TObj *>(vict));
          break;
        case CMD_KICK:
          rc = doKick(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_SHOULDER_THROW:
          rc = doShoulderThrow(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_CHOP:
          rc = doChop(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_HURL:
	  rc = doHurl(newarg, dynamic_cast<TBeing *>(vict));
	  break;
        case CMD_CHI:
	  rc = doChi(newarg, vict);
	  break;
        case CMD_EVALUATE:
          doEvaluate(newarg);
          break;
        case CMD_TITLE:
          doTitle(newarg);
          break;
        case CMD_SNOOP:
          doSnoop(newarg);
          break;
        case CMD_BRIEF:
          doBrief();
          break;
        case CMD_COMPACT:
          doCompact();
          break;
        case CMD_QUIVPALM:
          rc = doQuiveringPalm(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_IMMORTAL:
          doImmortal();
          break;
        case CMD_RIDE:
        case CMD_MOUNT:
        case CMD_DISMOUNT:
          rc = doMount(newarg, cmd, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_FORCE:
          doForce(newarg);
          break;
        case CMD_TERMINAL:
          doTerminal(newarg);
          break;
        case CMD_COLOR:
          doColor(newarg);
          break;
        case CMD_CLS:
          doCls(true);
          break;
        case CMD_LIGHT:
          doLight(tmpstring);
          break;
        case CMD_LOW:
          doLow(newarg);
          break;
        case CMD_ENTER:
          rc = doEnter(newarg, NULL);
          break;
        case CMD_RESIZE:
          doResize(newarg);
          break;
        case CMD_DISBAND:
          doDisband();
          break;
        case CMD_EXTINGUISH:
          doExtinguish(tmpstring);
          break;
        case CMD_BASH:
          rc = doBash(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_BACKSTAB:
          rc = doBackstab(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_HIDE:
          rc = doHide();
          break;
        case CMD_SNEAK:
          rc = doSneak(newarg);
          break;
        case CMD_CRAWL:
          doCrawl();
          break;
        case CMD_SUBTERFUGE:
          rc = doSubterfuge(newarg);
          break;
        case CMD_RENAME:
          doNameChange(newarg);
          break;
        case CMD_MARGINS:
          doResetMargins();
          break;
        case CMD_DISGUISE:
          rc = doDisguise(newarg);
          break;
        case CMD_DESCRIPTION:
          doDescription();
          break;
        case CMD_POISON_WEAPON:
          rc = doPoisonWeapon(newarg);
          break;
        case CMD_GARROTTE:
          rc = doGarrotte(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_STAB:
          rc = doStab(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_CUDGEL:
          rc = doCudgel(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_CHARGE:
          rc = doCharge(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_SPLIT:
          doSplit(newarg, true);
          break;
        case CMD_SMITE:
          rc = doSmite(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_FORAGE:
          doForage();
          break;
        case CMD_APPLY_HERBS:
          rc = doApplyHerbs(newarg);
          break;
        case CMD_BUTCHER:
          doButcher();
          break;
        case CMD_SKIN:
          doSkin(newarg);
          break;
        case CMD_TAN:
          doTan();
          break;
        case CMD_WHITTLE:
          doWhittle(newarg);
          break;
        case CMD_MESSAGE:
          doMessage(newarg);
          break;
        case CMD_FINDEMAIL:
          doFindEmail(newarg);
          break;
        case CMD_COMMENT:
          doComment(newarg);
          break;
        case CMD_CAMP:
          rc = doEncamp();
          break;
        case CMD_SOOTH:
          rc = doSoothBeast(newarg);
          break;
        case CMD_SUMMON:
          rc = doSummonBeast(newarg);
          break;
        case CMD_CHARM:
          rc = doCharmBeast(newarg);
          break;
        case CMD_RETRAIN:
          rc = doRetrainPet(newarg, dynamic_cast<TBeing *>(vict));
          break;
        case CMD_BEFRIEND:
          rc = doBefriendBeast(newarg);
          break;
        case CMD_TRANSFIX:
          rc = doTransfix(newarg);
          break;
        case CMD_BARKSKIN:
          rc = doBarkskin(newarg);
          break;
        case CMD_FLY:
          doFly();
          break;
        case CMD_LAND:
          doLand();
          break;
        case CMD_DIVINE:
          doDivine(newarg);
          break;
        case CMD_OUTFIT:
          doOutfit(newarg);
          break;
        case CMD_TRANSFORM:
          rc = doTransform(newarg);
          break;
        case CMD_EGOTRIP:
          doEgoTrip(newarg);
          break;
        case CMD_SPELLS:
	  doSpells(newarg);
	  break;
        case CMD_COMPARE:
	  doCompare(newarg);
	  break;
        case CMD_TEST_FIGHT:
	  doTestFight(newarg);
	  break;
        case CMD_DONATE:
          doDonate(newarg);
          break;
        case CMD_ZONES:
          doZones(newarg);
          break;
        case CMD_CREATE:
          rc = doCreate(newarg);
          break;
        case CMD_POWERS:
          doPowers(newarg);
          break;
	case CMD_SMOKE:
	  doSmoke(newarg);
	  break;
	case CMD_STOP:
	  doStop();
	  break;
        case MAX_CMD_LIST:
        case CMD_RESP_TOGGLE:
        case CMD_RESP_UNTOGGLE:
        case CMD_RESP_CHECKTOG:
        case CMD_RESP_PERSONALIZE:
        case CMD_RESP_ROOM_ENTER:
        case CMD_RESP_UNFLAG:
        case CMD_RESP_TOROOM:
        case CMD_RESP_TOVICT:
        case CMD_RESP_TONOTVICT:
        case CMD_RESP_CHECKUNTOG:
        case CMD_RESP_CHECKMAX:
        case CMD_RESP_LINK:
        case CMD_RESP_CODE_SEGMENT:
        case CMD_RESP_RESIZE:
        case CMD_RESP_CHECKLOAD:
        case CMD_RESP_LOADMOB:
        case CMD_RESP_PACKAGE:
        case CMD_RESP_PULSE:
        case CMD_RESP_CHECKROOM:
        case CMD_RESP_CHECKZONE:
        case CMD_GENERIC_PULSE:
        case CMD_GENERIC_CREATED:
        case CMD_GENERIC_RESET:
        case CMD_GENERIC_INIT:
        case CMD_GENERIC_DESTROYED:
        case CMD_ROOM_ENTERED:
        case CMD_ROOM_ATTEMPTED_EXIT:
        case CMD_OBJ_HITTING:
        case CMD_OBJ_HIT:
        case CMD_OBJ_MISS:
        case CMD_OBJ_BEEN_HIT:
        case CMD_OBJ_THROWN:
        case CMD_OBJ_PUT_INSIDE_SOMETHING:
        case CMD_OBJ_HAVING_SOMETHING_PUT_INTO:
        case CMD_OBJ_STUCK_IN:
        case CMD_OBJ_PULLED_OUT:
        case CMD_OBJ_USED:
        case CMD_OBJ_TOLD_TO_PLAYER:
        case CMD_OBJ_GOTTEN:
        case CMD_OBJ_WEATHER_TIME:
        case CMD_OBJ_WAGON_INIT:
        case CMD_OBJ_WAGON_UNINIT:
        case CMD_OBJ_MOVEMENT:
        case CMD_OBJ_SATON:
        case CMD_OBJ_EXPELLED:
        case CMD_OBJ_START_TO_FALL:
        case CMD_MOB_GIVEN_ITEM:
        case CMD_MOB_GIVEN_COINS:
        case CMD_MOB_ALIGN_PULSE:
        case CMD_MOB_KILLED_NEARBY:
        case CMD_MOB_MOVED_INTO_ROOM:
        case CMD_MOB_VIOLENCE_PEACEFUL:
        case CMD_MOB_COMBAT:
        case CMD_TASK_FIGHTING:
        case CMD_TASK_CONTINUE:
          incorrectCommand();
          return FALSE;
      }
    }
    if (IS_SET_DELETE(rc, DELETE_ITEM)) {
      // switch it to vict
      ADD_DELETE(rc, DELETE_VICT);
      REM_DELETE(rc, DELETE_ITEM);
    }
    if (IS_SET_DELETE(rc, DELETE_THIS)) {
      if (isPoly && !desc) {
        delete this;
      }
      return rc;
    }
    if (IS_SET_DELETE(rc, DELETE_VICT)) {
      return rc;
    }
    return FALSE;
  }
}


// call this if command should be executed right now (no lag)
// otherwise use addToCommandQue()
// return DELETE_THIS if tbeing has been killed
int TBeing::parseCommand(const char *orig_arg, bool typedIn)
{
  int i;
  char aliasbuf[256], arg1[512], arg2[1024];
  char argument[256];

  half_chop(orig_arg, arg1, arg2);

  if (!*arg1)
    return FALSE;

  if (riding) {
    if (!sameRoom(riding))
      dismount(POSITION_STANDING);
  }
  if (IS_SET(specials.affectedBy, AFF_HIDE))
    REMOVE_BIT(specials.affectedBy,AFF_HIDE);

  strcpy(argument, orig_arg);

  if (desc) {
    i = -1;
    do {
      i++;
    }
    while ((i < 16) && (strcmp(arg1, desc->alias[i].word)));

    if (i < 16) {
      if ((arg2) && (*arg2))
        sprintf(aliasbuf, "%s %s", desc->alias[i].command, arg2);
      else
        strcpy(aliasbuf, desc->alias[i].command);

      strcpy(argument, aliasbuf);
      half_chop(argument, arg1, arg2);
    }
  }
  // Let people use say and emote shortcuts with no spaces - Russ
  if ((*argument == '\'') || (*argument == ':') || (*argument == ',')) {
    arg1[0] = *argument;
    arg1[1] = '\0';
    strcpy(argument, &argument[1]);
  } else if (!strncmp(argument, "low", 3) && !isImmortal()) {
    // KLUDGE - for low and lower command
    // l and lo == look, so we need not check for them
    strcpy(arg1, "lower");
    strcpy(argument, one_arg(argument, arg2));
  } else if (!strncmp(argument, "repl", 4) && !isImmortal()) {
    // KLUDGE - for reply and replace command
    // rep == report, so we need not check for shorter
    strcpy(arg1, "reply");
    strcpy(argument, one_arg(argument, arg2));
  } else if (!strncmp(argument, "med", 3) && !isImmortal()) {
    // KLUDGE - for meditate and medit command
    // me == mend limb, so we need not check for shorter
    strcpy(arg1, "meditate");
    strcpy(argument, one_arg(argument, arg2));
  } else if (!strncasecmp(argument, "southe", 6)) {
    strcpy(arg1, "se");
    strcpy(argument, one_arg(argument, arg2));
  } else if (!strncasecmp(argument, "northw", 6)) {
    strcpy(arg1, "nw");
    strcpy(argument, one_arg(argument, arg2));
  } else if (!strncasecmp(argument, "southw", 6)) {
    strcpy(arg1, "sw");
    strcpy(argument, one_arg(argument, arg2));
  } else if (!strncasecmp(argument, "northe", 6)) {
    strcpy(arg1, "ne");
    strcpy(argument, one_arg(argument, arg2));
  } else
    strcpy(argument, one_arg(argument, arg2));

  cmdTypeT cmd = searchForCommandNum(arg1);
  if (cmd >= MAX_CMD_LIST) {
    incorrectCommand();
    return FALSE;
  }
  if (getCaptiveOf()) {
    switch (cmd) {
      case CMD_NORTH:
      case CMD_EAST:
      case CMD_SOUTH:
      case CMD_WEST:
      case CMD_UP:
      case CMD_DOWN:
      case CMD_NE:
      case CMD_NW:
      case CMD_SE:
      case CMD_SW:
      case CMD_FLEE:
        sendTo("You've been captured.  You aren't going anywhere until you get away.\n\r");
        return FALSE;
      case CMD_SAY:
      case CMD_LOOK:
      case CMD_LAUGH:
      case CMD_WHO:
      case CMD_HELP:
      case CMD_BUG:
      case CMD_IDEA:
      case CMD_TYPO:
      case CMD_NOD:
      case CMD_SAY2:
      case CMD_SIGH:
      case CMD_CRY:
      case CMD_GIVE:
      case CMD_REMOVE:
      case CMD_DROP:
      case CMD_RETURN:
        break;
      default:
        sendTo("You are unable to do that while a captive.\n\r");
        return FALSE;
    }
  }
#if 0
  if (typedIn) {
    return (doCommand(cmd, argument, NULL, TRUE));
  }
  return (doCommand(cmd, argument, NULL, FALSE));
#else
  return (doCommand(cmd, argument, NULL, typedIn));
#endif
}

// I tried it this way, but had a memory leak reported by insure from it
// bat 2/19/99
// static bool fill_word(string & argument)
static bool fill_word(const char * argument)
{
  const char *filler_word[] =
  {
    "in",
    "from",
    "with",
    "the",
    "on",
    "at",
    "to",
    "\n"
  };

  return (search_block(argument, filler_word, TRUE) >= 0);
}

void argument_interpreter(const char *argument, char *first_arg, char *second_arg)
{
  try {
    string tf1, tf2;
    argument_interpreter(argument, tf1, tf2);
    strcpy(first_arg, tf1.c_str());
    strcpy(second_arg, tf2.c_str());
  } catch (...) {
    mud_assert(0, "Failure in argument_interpreter");
  }
}

void argument_interpreter(string argument, string &first_arg, string &second_arg)
{
  string st = one_argument(argument, first_arg);
  one_argument(st, second_arg);
}

bool is_number(char *str)
{
  int look_at;

  if (!*str)
    return (0);

  for (look_at = 0; *(str + look_at) != '\0'; look_at++) {
    if ((*(str + look_at) < '0') || (*(str + look_at) > '9'))
      return (0);
  }
  return (1);
}

const char *one_argument(const char *argument, char *first_arg)
{
  try {
    string tmp_fa;
    string s = one_argument(argument, tmp_fa);
    strcpy(first_arg, tmp_fa.c_str());
  
    // we should return a pointer into argument equivalent to s.c_str
    if (s.empty())
      return &argument[strlen(argument)];  // return pointer to the NULL
    else {
#if 0
      // has problems with " 50 5"
      return strstr(argument, s.c_str());
#else
      // start looking at the spot denoted by "first_arg", for "s"
      return strstr(strstr(argument, first_arg), s.c_str());
#endif
    }
  } catch (...) {
    mud_assert(0, "Bat's expirimental code don't work - exception caught");
    return NULL;
  }
}

string one_argument(string argument, string & first_arg)
{
  size_t bgin, look_at;

  bgin = 0;

  do {
    string whitespace = " \n\r";
    bgin = argument.find_first_not_of(whitespace);
    look_at = argument.find_first_of(whitespace, bgin);

    if (look_at != string::npos) {
      // normal, return the part between
      first_arg = argument.substr(bgin, look_at - bgin);
      string a2 = argument.substr(look_at);
      argument = a2;
    } else if (bgin != string::npos) {
      // string had no terminator
      first_arg = argument.substr(bgin);
      argument = "";
    } else {
      // whole string was whitespace
      first_arg = "";
      argument = "";
    }

  } while (fill_word(first_arg.c_str()));
  return argument;
}

void only_argument(const char *argument, char *dest)
{
  while (*argument && isspace(*argument))
    argument++;
  strcpy(dest, argument);
}

bool is_abbrev(const char *arg1, const char *arg2, multipleTypeT multiple, exactTypeT exact)
{
  const string str1 = arg1;
  const string str2 = arg2;
  return is_abbrev(str1, str2, multiple, exact);
}

// determine if a given string is an abbreviation of another 
// multiple word functionality FALSE by c++ default - Russ
// Must be explicitly passed TRUE otherwise defaults to FALSE

bool is_abbrev(const string &arg1, const string &arg2, multipleTypeT multiple, exactTypeT exact)
{
  int spaces1 = 0;
  int spaces2 = 0;
  const string whitespace = " \n\r";

  // This functionality was added 01/03/98 by me - Russ
  if (multiple) {
    // Do we wanna check for multi word stuff?
    string carg1 = arg1;
    trimString(carg1);
    string::size_type pos = carg1.find_last_not_of(whitespace);
    if (pos != string::npos)
      carg1.erase(pos+1);

    pos = 0;
    do {
      pos++;
      pos = carg1.find_first_of(whitespace, pos);
      if (pos != string::npos)
        spaces1++;
    } while (pos != string::npos);

    // Now we have number of spaces in arg1, lets see if arg2
    // 1) Has that many words
    // 2) passes is_abbrev for all words
    string carg2 = arg2;
    pos = 0;
    do {
      ++pos;
      pos = carg2.find_first_of(whitespace, pos);
      if (pos != string::npos)
        spaces2++;
    } while (pos != string::npos);

    if (exact) {
      if (spaces1 != spaces2)
        return false;
    } else {
      if (spaces1 > spaces2)
        return false;
    }

    char *splitbuf1[32];
    char *splitbuf2[32];
    char tmpbuf1[256],
         tmpbuf2[256];
    // Tmp bufs are incase strtok screws up constant strings like man page says it does
    strcpy(tmpbuf1, carg1.c_str());
    strcpy(tmpbuf2, carg2.c_str());

    int num = split_string(tmpbuf1, " ", splitbuf1);
              split_string(tmpbuf2, " ", splitbuf2);
    for (int i = 0; i < num; i++) {
      for (; *splitbuf1[i]; splitbuf1[i]++, splitbuf2[i]++)
        if (LOWER(*splitbuf1[i]) != LOWER(*splitbuf2[i]))
          return false;
    }
    return true;
  }
  // Even if multiple, if we got here, just try to look for
  // something that matches

  if (arg1.length() > arg2.length() || arg1.empty())
    return false;

  // do case insenitive matching
  // we create carg2 "short" so that the compare will work properly
  string carg1 = lower(arg1);
  string carg2 = lower(arg2);

  // check for just garbage whitespace
  trimString(carg1);
  if (carg1.empty())
    return false;

  if (!stringncmp(carg1, carg2, carg1.length()))
    return true;
  return false;
}

// return first 'word' plus trailing substring of input string 
void half_chop(const char *string, char *arg1, char *arg2)
{
  for (; isspace(*string); string++);
  for (; *string && !isspace(*arg1 = *string); string++, arg1++);
  *arg1 = '\0';
  for (; isspace(*string); string++);
  for (; (*arg2 = *string); string++, arg2++);
}

const char *two_arg(const char *str, char *arg1, char *arg2)
{
  string t1 = str, t2, t3, t4;
  t4 = two_arg(t1, t2, t3);
  strcpy(arg1, t2.c_str());
  strcpy(arg2, t3.c_str());

  // return a pointer into the original argument
  if (t4.empty())
    return &str[strlen(str)];
  else {
    char *tmp = strstr(str, arg1);
    tmp = strstr(tmp, arg2);
    return strstr(tmp, t4.c_str());
  }
}

const string two_arg(const string &str, string &arg1, string &arg2)
{
  string whitespace = " \n\r";

  string::size_type n1 = str.find_first_not_of(whitespace);
  string::size_type n2 = str.find_first_of(whitespace, n1);
  if (n1 != string::npos && n2 != string::npos) {
    arg1 = str.substr(n1, n2 - n1);

    string::size_type n3 = str.find_first_not_of(whitespace, n2);
    string::size_type n4 = str.find_first_of(whitespace, n3);

    if (n3 != string::npos && n4 != string::npos) {
      // two args found, with extra stuff after
      arg2 = str.substr(n3, n4-n3);
      return string(str, n4);
    } else {
      if (n3 != string::npos) // two args, no extra
        arg2 = string(str, n3);
      else  // one arg, with trailing whitespace
        arg2 = "";
      return "";
    }
  } else {
    if (n1 != string::npos) // one arg, no trailing whitespace
      arg1 = string(str, n1);
    else // noargs
      arg1 = "";
    arg2 = "";
    return "";
  }
}

const char *three_arg(const char *string, char *a, char *b, char *c)
{
  for (; isspace(*string); string++);
  for (; !isspace(*a = *string) && *string; string++, a++);
  *a = '\0';
  for (; isspace(*string); string++);
  for (; !isspace(*b = *string) && *string; string++, b++);
  *b = '\0';
  for (; isspace(*string); string++);
  for (; !isspace(*c = *string) && *string; string++, c++);
  *c = '\0';

  return string;
}

char *add_bars(char *string)
{
  for (; *string; string++) {
    if (isspace(*string))
      *string = '-';
  }
  return string;
}

// returns DELETE_THIS, DELETE_VICT, TRUE or FALSE
int TBeing::triggerSpecialOnPerson(TThing *ch, cmdTypeT cmd, const char *arg)
{
  wearSlotT j;
  int rc;
  TThing *t2, *t;

  // special in equipment list?
  for (j = MIN_WEAR; j < MAX_WEAR; j++) {
    if ((t = equipment[j])) {
      rc = t->checkSpec(this, cmd, arg, ch);
      if (IS_SET_DELETE(rc, DELETE_THIS)) {
        delete t;
        t = NULL;
      }
      if (IS_SET_DELETE(rc, DELETE_VICT))
        return DELETE_THIS;
      if (IS_SET_DELETE(rc, DELETE_ITEM))
        return DELETE_VICT;
      else if (rc)
        return TRUE;
    }
    // special on imbedded item
    if ((t = getStuckIn(j))) {
      rc = t->checkSpec(this, cmd, arg, ch);
      if (IS_SET_DELETE(rc, DELETE_THIS)) {
        delete t;
        t = NULL;
      }
      if (IS_SET_DELETE(rc, DELETE_VICT))
        return DELETE_THIS;
      if (IS_SET_DELETE(rc, DELETE_ITEM))
        return DELETE_VICT;
      else if (rc)
        return TRUE;
    }
  }
  // special in inventory?
  for (t = stuff; t; t = t2) {
    t2 = t->nextThing;
    if (t->spec) {
      rc = t->checkSpec(this, cmd, arg, ch);
      if (IS_SET_DELETE(rc, DELETE_THIS)) {
        delete t;
        t = NULL;
      }
      if (IS_SET_ONLY(rc, DELETE_VICT))
        return DELETE_THIS;
      if (IS_SET_ONLY(rc, DELETE_ITEM))
        return DELETE_VICT;
      else if (rc)
        return TRUE;
    }
  }
  return FALSE;
}

// this function is a generic trigger for any/all special procs that
// "this" might trigger by doing cmd.  ch is here for future implementation
// if we want to add the capability to trigger with 2ndary parameters.  
// ch is not used now
// return DELETE_THIS will cause this to be destroyed
// return DELETE_VICT will cause ch to be destroyed
int TBeing::triggerSpecial(TThing *ch, cmdTypeT cmd, const char *arg)
{
  int rc;
  TThing *t, *t2;

  // is the player busy doing something else? 
  if (task && ((*(tasks[task->task].taskf)) 
            (this, cmd, arg, 0, roomp, task->obj)))
    return TRUE;

  // is the player busy doing something else?
  if (spelltask && cast_spell(this, cmd, 0))
    return TRUE;

  // special in room? 
  if (roomp) {
    rc = roomp->checkSpec(this, cmd, arg, NULL);
    if (IS_SET_DELETE(rc, DELETE_THIS)) {
      // delete room?
      vlogf(5, "checkSpec indicated delete room (%d)", in_room);
    }
    if (IS_SET_ONLY(rc, DELETE_VICT))
      return DELETE_THIS;
    if (rc)
      return TRUE;
  }

  rc = triggerSpecialOnPerson(ch, cmd, arg);
  if (IS_SET_ONLY(rc, DELETE_THIS))
    return DELETE_THIS;
  if (IS_SET_ONLY(rc, DELETE_VICT))
    return DELETE_VICT;
  else if (rc)
    return TRUE;

  if (roomp) {
    // special in mobile/object present? 
    for (t = roomp->stuff; t; t = t2) {
      t2 = t->nextThing;

      // note this is virtual function call
      rc = t->checkSpec(this, cmd, arg, ch);
      if (IS_SET_DELETE(rc, DELETE_THIS)) {
        delete t;
        t = NULL;
      }
      if (IS_SET_ONLY(rc, DELETE_VICT))
        return DELETE_THIS;
      if (IS_SET_ONLY(rc, DELETE_ITEM))
        return DELETE_VICT;
      if (rc)
        return TRUE;
    } 
  } 
  return FALSE;
}

void buildCommandArray(void)
{
  commandArray[CMD_NORTH] = new commandInfo("north", POSITION_CRAWLING, 0);
  commandArray[CMD_EAST] = new commandInfo("east", POSITION_CRAWLING, 0);
  commandArray[CMD_SOUTH] = new commandInfo("south", POSITION_CRAWLING, 0);
  commandArray[CMD_WEST] = new commandInfo("west", POSITION_CRAWLING, 0);
  commandArray[CMD_UP] = new commandInfo("up", POSITION_CRAWLING, 0);
  commandArray[CMD_DOWN] = new commandInfo("down", POSITION_CRAWLING, 0);
  commandArray[CMD_NE] = new commandInfo("ne", POSITION_CRAWLING, 0);
  commandArray[CMD_NW] = new commandInfo("nw", POSITION_CRAWLING, 0);
  commandArray[CMD_SE] = new commandInfo("se", POSITION_CRAWLING, 0);
  commandArray[CMD_SW] = new commandInfo("sw", POSITION_CRAWLING, 0);
  commandArray[CMD_DRINK] = new commandInfo("drink", POSITION_RESTING, 0);
  commandArray[CMD_EAT] = new commandInfo("eat", POSITION_RESTING, 0);
  commandArray[CMD_WEAR] = new commandInfo("wear", POSITION_RESTING, 0);
  commandArray[CMD_WIELD] = new commandInfo("wield", POSITION_RESTING, 0);
  commandArray[CMD_LOOK] = new commandInfo("look", POSITION_RESTING, 0);
  commandArray[CMD_SCORE] = new commandInfo("score", POSITION_DEAD, 0);
  commandArray[CMD_CACKLE] = new commandInfo("cackle", POSITION_RESTING, 0);
  commandArray[CMD_SHOUT] = new commandInfo("shout", POSITION_RESTING, 0);
  commandArray[CMD_TELL] = new commandInfo("tell",POSITION_RESTING, 0);
  commandArray[CMD_INVENTORY]=new commandInfo("inventory", POSITION_RESTING, 0);
  commandArray[CMD_GET] = new commandInfo("get", POSITION_RESTING, 0);
  commandArray[CMD_SAY] = new commandInfo("say", POSITION_RESTING, 0);
  commandArray[CMD_SMILE] = new commandInfo("smile", POSITION_RESTING, 0);
  commandArray[CMD_DANCE] = new commandInfo("dance", POSITION_STANDING, 0);
  commandArray[CMD_KILL] = new commandInfo("kill", POSITION_FIGHTING, 0);
  commandArray[CMD_CRAWL] = new commandInfo("crawl", POSITION_RESTING, 0);
  commandArray[CMD_LAUGH] = new commandInfo("laugh", POSITION_RESTING, 0);
  commandArray[CMD_GROUP] = new commandInfo("group", POSITION_RESTING, 0);
  commandArray[CMD_SHAKE] = new commandInfo("shake", POSITION_RESTING, 0);
  commandArray[CMD_PUKE] = new commandInfo("puke", POSITION_RESTING, 0);
  commandArray[CMD_GROWL] = new commandInfo("growl", POSITION_RESTING, 0);
  commandArray[CMD_SCREAM] = new commandInfo("scream", POSITION_RESTING, 0);
  commandArray[CMD_INSULT] = new commandInfo("insult", POSITION_RESTING, 0);
  commandArray[CMD_COMFORT] = new commandInfo("comfort", POSITION_RESTING, 0);
  commandArray[CMD_NOD] = new commandInfo("nod", POSITION_RESTING, 0);
  commandArray[CMD_SIGH] = new commandInfo("sigh", POSITION_RESTING, 0);
  commandArray[CMD_SULK] = new commandInfo("sulk", POSITION_RESTING, 0);
  commandArray[CMD_HELP] = new commandInfo("help", POSITION_DEAD, 0);
  commandArray[CMD_WHO] = new commandInfo("who", POSITION_DEAD, 0);
  commandArray[CMD_EMOTE] = new commandInfo("emote", POSITION_RESTING, 0);
  commandArray[CMD_ECHO]=new commandInfo("echo", POSITION_SLEEPING, GOD_LEVEL1);
  commandArray[CMD_STAND] = new commandInfo("stand", POSITION_RESTING, 0);
  commandArray[CMD_SIT] = new commandInfo("sit", POSITION_RESTING, 0);
  commandArray[CMD_REST] = new commandInfo("rest", POSITION_RESTING, 0);
  commandArray[CMD_SLEEP] = new commandInfo("sleep", POSITION_SLEEPING, 0);
  commandArray[CMD_WAKE] = new commandInfo("wake", POSITION_SLEEPING, 0);
  commandArray[CMD_FORCE]=new commandInfo("force",POSITION_SLEEPING,GOD_LEVEL1);
  commandArray[CMD_TRANSFER]=new commandInfo("transfer",POSITION_SLEEPING,GOD_LEVEL1);
  commandArray[CMD_HUG] = new commandInfo("hug", POSITION_RESTING, 0);
  commandArray[CMD_SNUGGLE] = new commandInfo("snuggle", POSITION_RESTING, 0);
  commandArray[CMD_CUDDLE] = new commandInfo("cuddle", POSITION_RESTING, 0);
  commandArray[CMD_NUZZLE] = new commandInfo("nuzzle", POSITION_RESTING, 0);
  commandArray[CMD_CRY] = new commandInfo("cry", POSITION_RESTING, 0);
  commandArray[CMD_NEWS] = new commandInfo("news", POSITION_DEAD, 0);
  commandArray[CMD_EQUIPMENT]=new commandInfo("equipment", POSITION_RESTING, 0);
  commandArray[CMD_BUY] = new commandInfo("buy", POSITION_SITTING, 0);
  commandArray[CMD_SELL] = new commandInfo("sell", POSITION_SITTING, 0);
  commandArray[CMD_VALUE] = new commandInfo("value", POSITION_SITTING, 0);
  commandArray[CMD_LIST] = new commandInfo("list", POSITION_SLEEPING, 0);
  commandArray[CMD_DROP] = new commandInfo("drop", POSITION_RESTING, 0);
  commandArray[CMD_GOTO] = new commandInfo("goto", POSITION_SLEEPING, 0);
  commandArray[CMD_WEATHER] = new commandInfo("weather", POSITION_RESTING, 0);
  commandArray[CMD_READ] = new commandInfo("read", POSITION_RESTING, 0);
  commandArray[CMD_POUR] = new commandInfo("pour", POSITION_SITTING, 0);
  commandArray[CMD_GRAB] = new commandInfo("grab", POSITION_RESTING, 0);
  commandArray[CMD_REMOVE] = new commandInfo("remove", POSITION_RESTING, 0);
  commandArray[CMD_PUT] = new commandInfo("put", POSITION_RESTING, 0);
  commandArray[CMD_SHUTDOW]=new commandInfo("shutdow",POSITION_DEAD,GOD_LEVEL1);
  commandArray[CMD_SAVE] = new commandInfo("save", POSITION_SLEEPING, 0);
  commandArray[CMD_HIT] = new commandInfo("hit", POSITION_SITTING, 0);
  commandArray[CMD_EXITS] = new commandInfo("exits", POSITION_RESTING, 0);
  commandArray[CMD_GIVE] = new commandInfo("give", POSITION_RESTING, 0);
  commandArray[CMD_QUIT] = new commandInfo("quit", POSITION_DEAD, 0);
  commandArray[CMD_STAT] = new commandInfo("stat", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_GUARD] = new commandInfo("guard", POSITION_STANDING, 0);
  commandArray[CMD_TIME] = new commandInfo("time", POSITION_DEAD, 0);
  commandArray[CMD_LOAD] = new commandInfo("load", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_PURGE] = new commandInfo("purge", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_SHUTDOWN] = new commandInfo("shutdown", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_IDEA] = new commandInfo("ideas", POSITION_DEAD, 0);
  commandArray[CMD_TYPO] = new commandInfo("typos", POSITION_DEAD, 0);
  commandArray[CMD_BUG] = new commandInfo("bugs", POSITION_DEAD, 0);
  commandArray[CMD_WHISPER] = new commandInfo("whisper", POSITION_RESTING, 0);
  commandArray[CMD_CAST] = new commandInfo("cast", POSITION_SITTING, 0);
  commandArray[CMD_AT] = new commandInfo("at", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_AS] = new commandInfo("as", POSITION_DEAD, 0);  // needs to be 0 so any mob can do it
  commandArray[CMD_ORDER] = new commandInfo("order", POSITION_RESTING, 0);
  commandArray[CMD_SIP] = new commandInfo("sip", POSITION_RESTING, 0);
  commandArray[CMD_TASTE] = new commandInfo("taste", POSITION_RESTING, 0);
  commandArray[CMD_SNOOP] = new commandInfo("snoop", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_FOLLOW] = new commandInfo("follow", POSITION_RESTING, 0);
  commandArray[CMD_RENT] = new commandInfo("rent", POSITION_CRAWLING, 0);
  commandArray[CMD_OFFER] = new commandInfo("offer", POSITION_RESTING, 0);
  commandArray[CMD_POKE] = new commandInfo("poke", POSITION_RESTING, 0);
  commandArray[CMD_ACCUSE] = new commandInfo("accuse", POSITION_SITTING, 0);
  commandArray[CMD_GRIN] = new commandInfo("grin", POSITION_RESTING, 0);
  commandArray[CMD_BOW] = new commandInfo("bow", POSITION_STANDING, 0);
  commandArray[CMD_OPEN] = new commandInfo("open", POSITION_RESTING, 0);
  commandArray[CMD_CLOSE] = new commandInfo("close", POSITION_RESTING, 0);
  commandArray[CMD_LOCK] = new commandInfo("lock", POSITION_CRAWLING, 0);
  commandArray[CMD_UNLOCK] = new commandInfo("unlock", POSITION_FIGHTING, 0);
  commandArray[CMD_LEAVE] = new commandInfo("leave", POSITION_CRAWLING, 0);
  commandArray[CMD_APPLAUD] = new commandInfo("applaud", POSITION_RESTING, 0);
  commandArray[CMD_BLUSH] = new commandInfo("blush", POSITION_RESTING, 0);
  commandArray[CMD_BURP] = new commandInfo("burp", POSITION_RESTING, 0);
  commandArray[CMD_CHUCKLE] = new commandInfo("chuckle", POSITION_RESTING, 0);
  commandArray[CMD_CLAP] = new commandInfo("clap", POSITION_RESTING, 0);
  commandArray[CMD_COUGH] = new commandInfo("cough", POSITION_RESTING, 0);
  commandArray[CMD_CURTSEY] = new commandInfo("curtsey", POSITION_STANDING, 0);
  commandArray[CMD_FART] = new commandInfo("fart", POSITION_RESTING, 0);
  commandArray[CMD_FLEE] = new commandInfo("flee", POSITION_RESTING, 0);
  commandArray[CMD_FONDLE] = new commandInfo("fondle", POSITION_RESTING, 0);
  commandArray[CMD_FROWN] = new commandInfo("frown", POSITION_RESTING, 0);
  commandArray[CMD_GASP] = new commandInfo("gasp", POSITION_RESTING, 0);
  commandArray[CMD_GLARE] = new commandInfo("glare", POSITION_RESTING, 0);
  commandArray[CMD_GROAN] = new commandInfo("groan", POSITION_RESTING, 0);
  commandArray[CMD_GROPE] = new commandInfo("grope", POSITION_RESTING, 0);
  commandArray[CMD_HICCUP] = new commandInfo("hiccup", POSITION_RESTING, 0);
  commandArray[CMD_LICK] = new commandInfo("lick", POSITION_RESTING, 0);
  commandArray[CMD_LOVE] = new commandInfo("love", POSITION_RESTING, 0);
  commandArray[CMD_MOAN] = new commandInfo("moan", POSITION_RESTING, 0);
  commandArray[CMD_NIBBLE] = new commandInfo("nibble", POSITION_RESTING, 0);
  commandArray[CMD_POUT] = new commandInfo("pout", POSITION_RESTING, 0);
  commandArray[CMD_PURR] = new commandInfo("purr", POSITION_RESTING, 0);
  commandArray[CMD_RUFFLE] = new commandInfo("ruffle", POSITION_CRAWLING, 0);
  commandArray[CMD_SHIVER] = new commandInfo("shiver", POSITION_RESTING, 0);
  commandArray[CMD_SHRUG] = new commandInfo("shrug", POSITION_RESTING, 0);
  commandArray[CMD_SING] = new commandInfo("sing", POSITION_RESTING, 0);
  commandArray[CMD_SLAP] = new commandInfo("slap", POSITION_RESTING, 0);
  commandArray[CMD_SMIRK] = new commandInfo("smirk", POSITION_RESTING, 0);
  commandArray[CMD_SNAP] = new commandInfo("snap", POSITION_RESTING, 0);
  commandArray[CMD_SNEEZE] = new commandInfo("sneeze", POSITION_RESTING, 0);
  commandArray[CMD_SNICKER] = new commandInfo("snicker", POSITION_RESTING, 0);
  commandArray[CMD_SNIFF] = new commandInfo("sniff", POSITION_RESTING, 0);
  commandArray[CMD_SNORE] = new commandInfo("snore", POSITION_SLEEPING, 0);
  commandArray[CMD_SPIT] = new commandInfo("spit", POSITION_RESTING, 0);
  commandArray[CMD_SQUEEZE] = new commandInfo("squeeze", POSITION_RESTING, 0);
  commandArray[CMD_STARE] = new commandInfo("stare", POSITION_RESTING, 0);
  commandArray[CMD_STRUT] = new commandInfo("strut", POSITION_STANDING, 0);
  commandArray[CMD_THANK] = new commandInfo("thank", POSITION_RESTING, 0);
  commandArray[CMD_TWIDDLE] = new commandInfo("twiddle", POSITION_RESTING, 0);
  commandArray[CMD_WAVE] = new commandInfo("wave", POSITION_RESTING, 0);
  commandArray[CMD_WHISTLE] = new commandInfo("whistle", POSITION_RESTING, 0);
  commandArray[CMD_WIGGLE] = new commandInfo("wiggle", POSITION_CRAWLING, 0);
  commandArray[CMD_WINK] = new commandInfo("wink", POSITION_RESTING, 0);
  commandArray[CMD_YAWN] = new commandInfo("yawn", POSITION_RESTING, 0);
  commandArray[CMD_SNOWBALL] = new commandInfo("snowball", POSITION_CRAWLING, GOD_LEVEL1);
  commandArray[CMD_WRITE] = new commandInfo("write", POSITION_RESTING, 0);
  commandArray[CMD_HOLD] = new commandInfo("hold", POSITION_RESTING, 0);
  commandArray[CMD_FLIP] = new commandInfo("flip", POSITION_STANDING, 0);
  commandArray[CMD_SNEAK] = new commandInfo("sneak", POSITION_CRAWLING, 0);
  commandArray[CMD_HIDE] = new commandInfo("hide", POSITION_STANDING, 0);
  commandArray[CMD_BACKSTAB]=new commandInfo("backstab", POSITION_STANDING, 0);
  commandArray[CMD_PICK] = new commandInfo("pick", POSITION_SITTING, 0);
  commandArray[CMD_STEAL] = new commandInfo("steal", POSITION_STANDING, 0);
  commandArray[CMD_BASH] = new commandInfo("bash", POSITION_FIGHTING, 0);
  commandArray[CMD_RESCUE] = new commandInfo("rescue", POSITION_FIGHTING, 0);
  commandArray[CMD_KICK] = new commandInfo("kick", POSITION_FIGHTING, 0);
  commandArray[CMD_FRENCH] = new commandInfo("french", POSITION_RESTING, 0);
  commandArray[CMD_COMB] = new commandInfo("comb", POSITION_RESTING, 0);
  commandArray[CMD_MASSAGE] = new commandInfo("massage", POSITION_RESTING, 0);
  commandArray[CMD_TICKLE] = new commandInfo("tickle", POSITION_RESTING, 0);
  commandArray[CMD_PRAY] = new commandInfo("pray", POSITION_DEAD, 0);
  commandArray[CMD_PAT] = new commandInfo("pat", POSITION_RESTING, 0);
  commandArray[CMD_QUIT2] = new commandInfo("quit!", POSITION_DEAD, 0);
  commandArray[CMD_TAKE] = new commandInfo("take", POSITION_RESTING, 0);
  commandArray[CMD_INFO]=new commandInfo("info", POSITION_SLEEPING, GOD_LEVEL1);
  commandArray[CMD_SAY2] = new commandInfo("'", POSITION_RESTING, 0);
  commandArray[CMD_QUEST]=new commandInfo("quest",POSITION_SLEEPING, 0);
  commandArray[CMD_CURSE] = new commandInfo("curse", POSITION_RESTING, 0);
  commandArray[CMD_USE] = new commandInfo("use", POSITION_SITTING, 0);
  commandArray[CMD_WHERE] = new commandInfo("where", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_LEVELS] = new commandInfo("levels", POSITION_DEAD, 0);
  commandArray[CMD_AUTO] = new commandInfo("auto", POSITION_SLEEPING, 0);
  commandArray[CMD_PEE] = new commandInfo("pee", POSITION_STANDING, GOD_LEVEL1);
  commandArray[CMD_EMOTE3] = new commandInfo(",", POSITION_RESTING, 0);
  commandArray[CMD_BEG] = new commandInfo("beg", POSITION_RESTING, 0);
  commandArray[CMD_BLEED] = new commandInfo("bleed", POSITION_RESTING, 0);
  commandArray[CMD_CRINGE] = new commandInfo("cringe", POSITION_RESTING, 0);
  commandArray[CMD_DAYDREAM]= new commandInfo("daydream", POSITION_SLEEPING, 0);
  commandArray[CMD_FUME] = new commandInfo("fume", POSITION_RESTING, 0);
  commandArray[CMD_GROVEL] = new commandInfo("grovel", POSITION_RESTING, 0);
  commandArray[CMD_HOP] = new commandInfo("hop", POSITION_STANDING, 0);
  commandArray[CMD_NUDGE] = new commandInfo("nudge", POSITION_RESTING, 0);
  commandArray[CMD_PEER] = new commandInfo("peer", POSITION_RESTING, 0);
  commandArray[CMD_POINT] = new commandInfo("point", POSITION_RESTING, 0);
  commandArray[CMD_PONDER] = new commandInfo("ponder", POSITION_RESTING, 0);
  commandArray[CMD_PUNCH] = new commandInfo("punch", POSITION_RESTING, 0);
  commandArray[CMD_SNARL] = new commandInfo("snarl", POSITION_RESTING, 0);
  commandArray[CMD_SPANK] = new commandInfo("spank", POSITION_RESTING, 0);
  commandArray[CMD_STEAM] = new commandInfo("steam", POSITION_RESTING, 0);
  commandArray[CMD_TACKLE] = new commandInfo("tackle", POSITION_RESTING, 0);
  commandArray[CMD_TAUNT] = new commandInfo("taunt", POSITION_RESTING, 0);
  commandArray[CMD_WIZNET] = new commandInfo("wiznet", POSITION_RESTING, GOD_LEVEL1);
  commandArray[CMD_WHINE] = new commandInfo("whine", POSITION_RESTING, 0);
  commandArray[CMD_WORSHIP] = new commandInfo("worship", POSITION_RESTING, 0);
  commandArray[CMD_YODEL] = new commandInfo("yodel", POSITION_RESTING, 0);
  commandArray[CMD_BRIEF] = new commandInfo("brief", POSITION_DEAD, 0);
  commandArray[CMD_WIZLIST] = new commandInfo("wizlist", POSITION_DEAD, 0);
  commandArray[CMD_CONSIDER] = new commandInfo("consider", POSITION_RESTING, 0);
  commandArray[CMD_GIGGLE] = new commandInfo("giggle", POSITION_RESTING, 0);
  commandArray[CMD_RESTORE]=new commandInfo("restore",POSITION_DEAD,GOD_LEVEL1);
  commandArray[CMD_RETURN] = new commandInfo("return", POSITION_DEAD, 0);
  commandArray[CMD_SWITCH]=new commandInfo("switch", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_QUAFF] = new commandInfo("quaff", POSITION_RESTING, 0);
  commandArray[CMD_RECITE] = new commandInfo("recite", POSITION_FIGHTING, 0);
  commandArray[CMD_USERS] = new commandInfo("users", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_PROTECT] = new commandInfo("protect", POSITION_CRAWLING, 0);
  commandArray[CMD_NOSHOUT] = new commandInfo("noshout", POSITION_SLEEPING, 2);
  commandArray[CMD_WIZHELP] = new commandInfo("wizhelp", POSITION_SLEEPING, GOD_LEVEL1);
  commandArray[CMD_CREDITS] = new commandInfo("credits", POSITION_DEAD, 0);
  commandArray[CMD_COMPACT] = new commandInfo("compact", POSITION_DEAD, 0);
  commandArray[CMD_EMOTE2] = new commandInfo(":", POSITION_RESTING, 0);
  commandArray[CMD_EXTINGUISH]=new commandInfo("extinguish",POSITION_RESTING,0);
  commandArray[CMD_SLAY] = new commandInfo("slay", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_WIMPY] = new commandInfo("wimpy", POSITION_DEAD, 0);
  commandArray[CMD_JUNK] = new commandInfo("junk", POSITION_RESTING, 0);
  commandArray[CMD_DEPOSIT] = new commandInfo("deposit", POSITION_RESTING, 0);
  commandArray[CMD_WITHDRAW] = new commandInfo("withdraw", POSITION_RESTING, 0);
  commandArray[CMD_BALANCE] = new commandInfo("balance", POSITION_RESTING, 0);
  commandArray[CMD_NOHASSLE] = new commandInfo("nohassle", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_SYSTEM]=new commandInfo("system", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_PULL] = new commandInfo("pull", POSITION_CRAWLING, 0);
  commandArray[CMD_STEALTH]=new commandInfo("stealth",POSITION_DEAD,GOD_LEVEL1);
  commandArray[CMD_EDIT] = new commandInfo("edit", POSITION_DEAD, 0);
  commandArray[CMD_SET] = new commandInfo("@set", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_RSAVE] = new commandInfo("rsave", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_RLOAD] = new commandInfo("rload", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_TRACK] = new commandInfo("track", POSITION_CRAWLING, 0);
  commandArray[CMD_WIZLOCK]=new commandInfo("wizlock",POSITION_DEAD,GOD_LEVEL1);
  commandArray[CMD_HIGHFIVE] = new commandInfo("highfive", POSITION_RESTING, 0);
  commandArray[CMD_TITLE] = new commandInfo("title", POSITION_DEAD, 0);
  commandArray[CMD_WHOZONE]=new commandInfo("whozone",POSITION_DEAD,GOD_LEVEL1);
  commandArray[CMD_ASSIST] = new commandInfo("assist", POSITION_FIGHTING, 0);
  commandArray[CMD_ATTRIBUTE] = new commandInfo("attribute", POSITION_DEAD, 0);
  commandArray[CMD_WORLD] = new commandInfo("world", POSITION_DEAD, 0);
  commandArray[CMD_BREAK] = new commandInfo("break", POSITION_CRAWLING, 0);
  commandArray[CMD_REFUEL] = new commandInfo("refuel", POSITION_RESTING, 0);
  commandArray[CMD_SHOW] = new commandInfo("show", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_BODYSLAM] =new commandInfo("bodyslam", POSITION_FIGHTING, 0);
  commandArray[CMD_INVISIBLE] = new commandInfo("invisible", POSITION_DEAD, 0);
  commandArray[CMD_GAIN] = new commandInfo("gain", POSITION_CRAWLING, 0);
  commandArray[CMD_TIMESHIFT] = new commandInfo("timeshift", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_DISARM] = new commandInfo("disarm", POSITION_FIGHTING, 0);
  commandArray[CMD_THINK] = new commandInfo("think", POSITION_RESTING, 0);
  commandArray[CMD_ENTER] = new commandInfo("enter", POSITION_SITTING, 0);
  commandArray[CMD_FILL] = new commandInfo("fill", POSITION_RESTING, 0);
  commandArray[CMD_SHOVE] = new commandInfo("shove", POSITION_STANDING, 0);
  commandArray[CMD_SCAN] = new commandInfo("scan", POSITION_CRAWLING, 0);
  commandArray[CMD_TOGGLE] = new commandInfo("toggle", POSITION_SLEEPING, GOD_LEVEL1);
  commandArray[CMD_BREATH] = new commandInfo("breathe", POSITION_RESTING, GOD_LEVEL1);
  commandArray[CMD_GT] = new commandInfo("gtell", POSITION_RESTING, 0);
  commandArray[CMD_WHAP] = new commandInfo("whap", POSITION_RESTING, 0);
  commandArray[CMD_LOG] = new commandInfo("log", POSITION_RESTING, GOD_LEVEL1);
  commandArray[CMD_BEAM] = new commandInfo("beam", POSITION_SLEEPING, 0);
  commandArray[CMD_CHORTLE] = new commandInfo("chortle", POSITION_RESTING, 0);
  commandArray[CMD_REPORT] = new commandInfo("report", POSITION_RESTING, 0);
  commandArray[CMD_WIPE] = new commandInfo("wipe", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_STOP] = new commandInfo("stop", POSITION_DEAD, 0);
  commandArray[CMD_BONK] = new commandInfo("bonk", POSITION_RESTING, 0);
  commandArray[CMD_SCOLD] = new commandInfo("scold", POSITION_RESTING, 0);
  commandArray[CMD_DROOL] = new commandInfo("drool", POSITION_SLEEPING, 0);
  commandArray[CMD_RIP] = new commandInfo("rip", POSITION_RESTING, 0);
  commandArray[CMD_STRETCH] = new commandInfo("stretch", POSITION_RESTING, 0);
  commandArray[CMD_SPLIT] = new commandInfo("split", POSITION_RESTING, 0);
  commandArray[CMD_COMMAND] = new commandInfo("commands", POSITION_SLEEPING, 0);
  commandArray[CMD_DEATHSTROKE] = new commandInfo("deathstroke", POSITION_FIGHTING, 0);
  commandArray[CMD_PIMP] = new commandInfo("pimp", POSITION_STANDING, 0);
  commandArray[CMD_LIGHT] = new commandInfo("light", POSITION_RESTING, 0);
  commandArray[CMD_BELITTLE] = new commandInfo("belittle", POSITION_RESTING, 0);
  commandArray[CMD_PILEDRIVE]=new commandInfo("piledrive",POSITION_STANDING, 0);
  commandArray[CMD_TAP] = new commandInfo("tap", POSITION_CRAWLING, 0);
  commandArray[CMD_BET] = new commandInfo("bet", POSITION_RESTING, 0);
  commandArray[CMD_STAY] = new commandInfo("stay", POSITION_RESTING, 0);
  commandArray[CMD_PEEK] = new commandInfo("peek", POSITION_RESTING, 0);
  commandArray[CMD_COLOR] = new commandInfo("color", POSITION_SLEEPING, 0);
  commandArray[CMD_HEADBUTT]=new commandInfo("headbutt", POSITION_FIGHTING, 0);
  commandArray[CMD_KNEESTRIKE]=new commandInfo("kneestrike",POSITION_FIGHTING, 0);
  commandArray[CMD_SUBTERFUGE] = new commandInfo("subterfuge", POSITION_CRAWLING, 0);
  commandArray[CMD_THROW] = new commandInfo("throw", POSITION_FIGHTING, 0);
  commandArray[CMD_EXAMINE] = new commandInfo("examine", POSITION_RESTING, 0);
  commandArray[CMD_SCRIBE]=new commandInfo("scribe", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_BREW] = new commandInfo("brew", POSITION_STANDING, 0);
  commandArray[CMD_GRAPPLE] = new commandInfo("grapple", POSITION_FIGHTING, 0);
  commandArray[CMD_FLIPOFF] = new commandInfo("flipoff", POSITION_SITTING, 0);
  commandArray[CMD_MOO] = new commandInfo("moo", POSITION_RESTING, 0);
  commandArray[CMD_PINCH] = new commandInfo("pinch", POSITION_SITTING, 0);
  commandArray[CMD_BITE] = new commandInfo("bite", POSITION_CRAWLING, 0);
  commandArray[CMD_SEARCH] = new commandInfo("search", POSITION_CRAWLING, 0);
  commandArray[CMD_SPY] = new commandInfo("spy", POSITION_CRAWLING, 0);
  commandArray[CMD_DOORBASH]= new commandInfo("doorbash", POSITION_STANDING, 0);
  commandArray[CMD_PLAY] = new commandInfo("play", POSITION_DEAD, 0);
  commandArray[CMD_FLAG] = new commandInfo("flag", POSITION_SLEEPING, 0);
  commandArray[CMD_QUIVPALM] = new commandInfo("quivering palm", POSITION_FIGHTING, 0);
  commandArray[CMD_FEIGNDEATH] = new commandInfo("feign death", POSITION_RESTING, 0);
  commandArray[CMD_SPRINGLEAP] = new commandInfo("springleap", POSITION_RESTING, 0);
  commandArray[CMD_MEND_LIMB]=new commandInfo("mend limb", POSITION_RESTING, 0);
  commandArray[CMD_ABORT] = new commandInfo("abort", POSITION_DEAD, 0);
  commandArray[CMD_SIGN] = new commandInfo("sign", POSITION_RESTING, 0);
  commandArray[CMD_CUTLINK]=new commandInfo("cutlink",POSITION_DEAD,GOD_LEVEL1);
  commandArray[CMD_LAYHANDS]=new commandInfo("lay-hands", POSITION_RESTING, 0);
  commandArray[CMD_WIZNEWS]=new commandInfo("wiznews",POSITION_DEAD, 0);
  commandArray[CMD_MAIL] = new commandInfo("mail", POSITION_CRAWLING, 0);
  commandArray[CMD_CHECK] = new commandInfo("check", POSITION_CRAWLING, 0);
  commandArray[CMD_RECEIVE] = new commandInfo("receive", POSITION_CRAWLING, 0);
  commandArray[CMD_CLS] = new commandInfo("cls", POSITION_DEAD, 0);
  commandArray[CMD_REPAIR] = new commandInfo("repair", POSITION_CRAWLING, GOD_LEVEL1);
  commandArray[CMD_TERMINAL] = new commandInfo("terminal", POSITION_DEAD, 0);
  commandArray[CMD_PROMPT] = new commandInfo("prompt", POSITION_DEAD, 0);
  commandArray[CMD_BAMFIN]=new commandInfo("bamfin", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_BAMFOUT]=new commandInfo("bamfout",POSITION_DEAD,GOD_LEVEL1);
  commandArray[CMD_GLANCE] = new commandInfo("glance", POSITION_RESTING, 0);
  commandArray[CMD_CHECKLOG] = new commandInfo("checklog", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_LOGLIST]=new commandInfo("loglist",POSITION_DEAD,GOD_LEVEL1);
  commandArray[CMD_DEATHCHECK] = new commandInfo("deathcheck", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_SET_TRAP] = new commandInfo("trap", POSITION_RESTING, 0);
  commandArray[CMD_CHANGE] = new commandInfo("change", POSITION_RESTING, 0);
  commandArray[CMD_REDIT]=new commandInfo("redit", POSITION_DEAD, GOD_LEVEL1); 
  commandArray[CMD_OEDIT] = new commandInfo("oedit", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_MEDIT] = new commandInfo("medit", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_DODGE] = new commandInfo("dodge", POSITION_FIGHTING, 0);
  commandArray[CMD_ALIAS] = new commandInfo("alias", POSITION_DEAD, 0);
  commandArray[CMD_CLEAR] = new commandInfo("clear", POSITION_DEAD, 0);
  commandArray[CMD_SHOOT] = new commandInfo("shoot", POSITION_CRAWLING, 0);
  commandArray[CMD_BLOAD] = new commandInfo("bload", POSITION_CRAWLING, 0);
  commandArray[CMD_MOUNT] = new commandInfo("mount", POSITION_ENGAGED, 0);
  commandArray[CMD_DISMOUNT]= new commandInfo("dismount", POSITION_ENGAGED, 0);
  commandArray[CMD_RIDE] = new commandInfo("ride", POSITION_ENGAGED, 0);
  commandArray[CMD_POST] = new commandInfo("post", POSITION_CRAWLING, 0);
  commandArray[CMD_ASK] = new commandInfo("ask", POSITION_RESTING, 0);
  commandArray[CMD_ATTACK] = new commandInfo("attack", POSITION_SLEEPING, 0);
  commandArray[CMD_SHARPEN] = new commandInfo("sharpen", POSITION_SITTING, 0);
  commandArray[CMD_KISS] = new commandInfo("kiss", POSITION_RESTING, 0);
  commandArray[CMD_ACCESS] = new commandInfo("access", POSITION_RESTING, GOD_LEVEL1);
  commandArray[CMD_MOTD] = new commandInfo("motd", POSITION_DEAD, 0);
  commandArray[CMD_REPLACE] = new commandInfo("replace", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_LIMBS] = new commandInfo("limbs", POSITION_SLEEPING, 0);
  commandArray[CMD_PRACTICE] = new commandInfo("practice", POSITION_DEAD, 0);
  commandArray[CMD_GAMESTATS] = new commandInfo("gamestats", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_BANDAGE] = new commandInfo("bandage", POSITION_SITTING, 0);
  commandArray[CMD_SETSEV]=new commandInfo("setsev", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_TURN]=new commandInfo("turn", POSITION_CRAWLING, 0);
  commandArray[CMD_DEAL] = new commandInfo("deal", POSITION_SITTING, 0);
  commandArray[CMD_PASS] = new commandInfo("pass", POSITION_SITTING, 0);
  commandArray[CMD_MAKELEADER]=new commandInfo("makeleader", POSITION_DEAD, 0);
  commandArray[CMD_NEWMEMBER]=new commandInfo("newmember",POSITION_RESTING,  0);
  commandArray[CMD_RMEMBER] = new commandInfo("rmember", POSITION_DEAD, 0);
  commandArray[CMD_HISTORY] = new commandInfo("history", POSITION_SLEEPING, 0);
  commandArray[CMD_DRAG] = new commandInfo("drag", POSITION_STANDING, 0);
  commandArray[CMD_MEDITATE] = new commandInfo("meditate", POSITION_RESTING, 0);
  commandArray[CMD_SCRATCH] = new commandInfo("scratch", POSITION_RESTING, 0);
  commandArray[CMD_CHEER] = new commandInfo("cheer", POSITION_RESTING, 0);
  commandArray[CMD_WOO] = new commandInfo("woo", POSITION_RESTING, 0);
  commandArray[CMD_GRUMBLE] = new commandInfo("grumble", POSITION_RESTING, 0);
  commandArray[CMD_APOLOGIZE]=new commandInfo("apologize", POSITION_RESTING, 0);
  commandArray[CMD_SEND] = new commandInfo("send", POSITION_RESTING, 0);
  commandArray[CMD_AGREE] = new commandInfo("agree", POSITION_RESTING, 0);
  commandArray[CMD_DISAGREE] = new commandInfo("disagree", POSITION_RESTING, 0);
  commandArray[CMD_BERSERK] = new commandInfo("berserk", POSITION_FIGHTING, 0);
  commandArray[CMD_TESTCODE] = new commandInfo("testcode", POSITION_SLEEPING, GOD_LEVEL1);
  commandArray[CMD_SPAM] = new commandInfo("spam", POSITION_RESTING,0);
  commandArray[CMD_RAISE] = new commandInfo("raise", POSITION_RESTING,0);
  commandArray[CMD_ROLL] = new commandInfo("roll", POSITION_RESTING,0);
  commandArray[CMD_BLINK] = new commandInfo("blink", POSITION_RESTING,0);
  commandArray[CMD_BRUTTEST]=new commandInfo("bruttest",POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_HOSTLOG]=new commandInfo("hostlog",POSITION_DEAD,GOD_LEVEL1);
  commandArray[CMD_PRESS] = new commandInfo("press",POSITION_SITTING,0);
  commandArray[CMD_TWIST] = new commandInfo("twist",POSITION_SITTING,0);
  commandArray[CMD_MID] = new commandInfo("mid",POSITION_DEAD,GOD_LEVEL1);
  commandArray[CMD_TRACEROUTE] = new commandInfo("traceroute", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_TASKS] = new commandInfo("tasks", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_VIEWOUTPUT] = new commandInfo("viewoutput", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_EVALUATE] = new commandInfo("evaluate", POSITION_RESTING, 0);
  commandArray[CMD_IMMORTAL] = new commandInfo("immortal", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_EXEC] = new commandInfo("exec", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_LOW] = new commandInfo("low", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_PUSH] = new commandInfo("push",POSITION_RESTING,0);
  commandArray[CMD_RESIZE] = new commandInfo("resize",POSITION_RESTING,GOD_LEVEL1);
  commandArray[CMD_DISBAND] = new commandInfo("disband",POSITION_DEAD,0);
  commandArray[CMD_LIFT] = new commandInfo("lift",POSITION_CRAWLING,0);
  commandArray[CMD_ARCH] = new commandInfo("arch",POSITION_RESTING,0);
  commandArray[CMD_BOUNCE] = new commandInfo("bounce",POSITION_STANDING,0);
  commandArray[CMD_DISGUISE] = new commandInfo("disguise", POSITION_STANDING, 0); 
  commandArray[CMD_RENAME] = new commandInfo("rename", POSITION_DEAD, 0);
  commandArray[CMD_MARGINS] = new commandInfo("margins", POSITION_DEAD,0);
  commandArray[CMD_DESCRIPTION]=new commandInfo("description",POSITION_DEAD,0);
  commandArray[CMD_POISON_WEAPON] = new commandInfo("poison-weapon", POSITION_STANDING, 0); 
  commandArray[CMD_GARROTTE]=new commandInfo("garrotte", POSITION_STANDING, 0); 
  commandArray[CMD_STAB] = new commandInfo("stab", POSITION_FIGHTING, 0); 
  commandArray[CMD_CUDGEL] = new commandInfo("cudgel", POSITION_STANDING, 0); 
  commandArray[CMD_PENANCE] = new commandInfo("penance", POSITION_RESTING, 0);
  commandArray[CMD_SMITE] = new commandInfo("smite", POSITION_SITTING, 0);
  commandArray[CMD_CHARGE] = new commandInfo("charge", POSITION_SITTING, 0);
  commandArray[CMD_LOWER] = new commandInfo("lower", POSITION_STANDING, 0);
  commandArray[CMD_REPLY] = new commandInfo("reply", POSITION_RESTING, 0);
  commandArray[CMD_HEAVEN]=new commandInfo("heaven",POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_CAPTURE] = new commandInfo("capture", POSITION_CRAWLING, GOD_LEVEL1);
  commandArray[CMD_ACCOUNT]=new commandInfo("account",POSITION_DEAD,GOD_LEVEL1);
  commandArray[CMD_RELEASE] = new commandInfo("release", POSITION_CRAWLING, GOD_LEVEL1);
  commandArray[CMD_FAINT] = new commandInfo("faint", POSITION_RESTING, 0);
  commandArray[CMD_GREET] = new commandInfo("greet", POSITION_RESTING, 0);
  commandArray[CMD_TIP] = new commandInfo("tip", POSITION_RESTING, 0);
  commandArray[CMD_BOP] = new commandInfo("bop", POSITION_RESTING, 0);
  commandArray[CMD_JUMP] = new commandInfo("jump", POSITION_STANDING, 0);
  commandArray[CMD_WHIMPER] = new commandInfo("whimper", POSITION_RESTING, 0);
  commandArray[CMD_SNEER] = new commandInfo("sneer", POSITION_RESTING, 0);
  commandArray[CMD_MOON] = new commandInfo("moon", POSITION_CRAWLING, 0);
  commandArray[CMD_BOGGLE] = new commandInfo("boggle", POSITION_RESTING, 0);
  commandArray[CMD_SNORT] = new commandInfo("snort", POSITION_RESTING, 0);
  commandArray[CMD_TANGO] = new commandInfo("tango", POSITION_STANDING, 0);
  commandArray[CMD_ROAR] = new commandInfo("roar", POSITION_SITTING, 0);
  commandArray[CMD_FLEX] = new commandInfo("flex", POSITION_STANDING, 0);
  commandArray[CMD_TUG] = new commandInfo("tug", POSITION_SITTING, 0);
  commandArray[CMD_CROSS] = new commandInfo("cross", POSITION_RESTING, 0);
  commandArray[CMD_HOWL] = new commandInfo("howl", POSITION_SITTING, 0);
  commandArray[CMD_GRUNT] = new commandInfo("grunt", POSITION_RESTING, 0);
  commandArray[CMD_WEDGIE] = new commandInfo("wedgie", POSITION_STANDING, 0);
  commandArray[CMD_SCUFF] = new commandInfo("scuff", POSITION_STANDING, 0);
  commandArray[CMD_NOOGIE] = new commandInfo("noogie", POSITION_STANDING, 0);
  commandArray[CMD_BRANDISH] = new commandInfo("brandish",POSITION_STANDING, 0);
  commandArray[CMD_TRIP] = new commandInfo("trip", POSITION_STANDING, 0);
  commandArray[CMD_DUCK] = new commandInfo("duck", POSITION_RESTING, 0);
  commandArray[CMD_BECKON] = new commandInfo("beckon", POSITION_RESTING, 0);
  commandArray[CMD_WINCE] = new commandInfo("wince", POSITION_RESTING, 0);
  commandArray[CMD_HUM] = new commandInfo("hum", POSITION_RESTING, 0);
  commandArray[CMD_RAZZ] = new commandInfo("razz", POSITION_RESTING, 0);
  commandArray[CMD_GAG] = new commandInfo("gag", POSITION_RESTING, 0);
  commandArray[CMD_AVERT] = new commandInfo("avert", POSITION_RESTING, 0);
  commandArray[CMD_SALUTE] = new commandInfo("salute", POSITION_STANDING, 0);
  commandArray[CMD_PET] = new commandInfo("pet", POSITION_RESTING, 0);
  commandArray[CMD_GRIMACE] = new commandInfo("grimace", POSITION_RESTING, 0);
  commandArray[CMD_SEEKWATER]=new commandInfo("seekwater",POSITION_CRAWLING, 0);
  commandArray[CMD_CRIT] = new commandInfo("crit", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_FORAGE] = new commandInfo("forage", POSITION_CRAWLING, 0);
  commandArray[CMD_APPLY_HERBS] = new commandInfo("apply-herbs", POSITION_CRAWLING, 0);
  commandArray[CMD_RESET] = new commandInfo("reset", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_STOMP] = new commandInfo("stomp", POSITION_FIGHTING, 0);
  commandArray[CMD_EMAIL] = new commandInfo("email", POSITION_DEAD, 0);
  commandArray[CMD_CLIMB] = new commandInfo("climb", POSITION_STANDING, 0);
  commandArray[CMD_DESCEND] = new commandInfo("descend", POSITION_STANDING, 0);
  commandArray[CMD_SORT] = new commandInfo("sort", POSITION_SITTING, 0);
  commandArray[CMD_SADDLE] = new commandInfo("saddle", POSITION_STANDING, 0);
  commandArray[CMD_UNSADDLE]=new commandInfo("unsaddle", POSITION_STANDING, 0);
  commandArray[CMD_SHOULDER_THROW] = new commandInfo("shoulder throw", POSITION_FIGHTING, 0);
  commandArray[CMD_CHOP] = new commandInfo("chop", POSITION_FIGHTING, 0);
  commandArray[CMD_HURL] = new commandInfo("hurl", POSITION_FIGHTING, 0);
  commandArray[CMD_CHI] = new commandInfo("chi", POSITION_FIGHTING, 0);
  commandArray[CMD_DIVINE] = new commandInfo("divine", POSITION_STANDING, 0);
  commandArray[CMD_OUTFIT] = new commandInfo("outfit", POSITION_STANDING, 0);
  commandArray[CMD_CLIENTS] = new commandInfo("clients", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_DULL] = new commandInfo("smooth", POSITION_SITTING, 0);
  commandArray[CMD_ADJUST] = new commandInfo("adjust", POSITION_SLEEPING, 0);
  commandArray[CMD_BUTCHER] = new commandInfo("butcher", POSITION_STANDING, 0);
  commandArray[CMD_SKIN] = new commandInfo("skin", POSITION_STANDING, 0);
  commandArray[CMD_TAN] = new commandInfo("tan", POSITION_STANDING, 0);
  commandArray[CMD_TITHE] = new commandInfo("tithe", POSITION_STANDING, 0);
  commandArray[CMD_DISSECT] = new commandInfo("dissection",POSITION_SITTING, 0);
  commandArray[CMD_FINDEMAIL] = new commandInfo("findemail", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_ENGAGE] = new commandInfo("engage", POSITION_FIGHTING, 0);
  commandArray[CMD_DISENGAGE]=new commandInfo("disengage",POSITION_FIGHTING, 0);
  commandArray[CMD_RESTRING]= new commandInfo("restring", POSITION_FIGHTING, 0);
  commandArray[CMD_CONCEAL] = new commandInfo("conceal", POSITION_STANDING, 0);
  commandArray[CMD_COMMENT] = new commandInfo("comment", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_CAMP] = new commandInfo("camp", POSITION_CRAWLING, 0);
  commandArray[CMD_YOGINSA] = new commandInfo("yoginsa", POSITION_RESTING, 0);
  commandArray[CMD_FLY] = new commandInfo("fly", POSITION_STANDING, 0);
  commandArray[CMD_LAND] = new commandInfo("land", POSITION_FLYING, 0);
  commandArray[CMD_ATTUNE] = new commandInfo("attune", POSITION_RESTING, 0);
  commandArray[CMD_AFK] = new commandInfo("afk", POSITION_DEAD, 0);
  commandArray[CMD_CONTINUE] = new commandInfo("continue", POSITION_DEAD, 0);
  commandArray[CMD_PEELPK] = new commandInfo("peelpk", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_SOOTH] = new commandInfo("sooth", POSITION_STANDING, 0);
  commandArray[CMD_SUMMON] = new commandInfo("summon", POSITION_STANDING, 0);
  commandArray[CMD_CHARM] = new commandInfo("charm", POSITION_STANDING, 0);
  commandArray[CMD_BEFRIEND] = new commandInfo("befriend", POSITION_STANDING, 0);
  commandArray[CMD_TRANSFIX] = new commandInfo("transfix", POSITION_STANDING, 0);
  commandArray[CMD_BARKSKIN] = new commandInfo("barkskin", POSITION_STANDING, 0);
  commandArray[CMD_TRANSFORM] = new commandInfo("transform", POSITION_STANDING, 0);
  commandArray[CMD_EGOTRIP] = new commandInfo("egotrip", POSITION_STANDING, GOD_LEVEL1);
  commandArray[CMD_CHIP] = new commandInfo("chip", POSITION_STANDING, 0);
  commandArray[CMD_DIG] = new commandInfo("dig", POSITION_STANDING, 0);
  commandArray[CMD_COVER] = new commandInfo("cover", POSITION_STANDING, 0);
  commandArray[CMD_OPERATE] = new commandInfo("operate", POSITION_STANDING, 0);
  commandArray[CMD_LONGDESCR] = new commandInfo("longdescr", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_SPELLS] = new commandInfo("spells", POSITION_DEAD, 0);
  commandArray[CMD_COMPARE] = new commandInfo("compare", POSITION_DEAD, 0);
  commandArray[CMD_TEST_FIGHT] = new commandInfo("testfight", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_DONATE] = new commandInfo("donate", POSITION_RESTING, 0);
  commandArray[CMD_ZONES] = new commandInfo("zones", POSITION_SLEEPING, 0);
  commandArray[CMD_FACTIONS] = new commandInfo("factions", POSITION_SLEEPING, 0);
  commandArray[CMD_CREATE] = new commandInfo("create", POSITION_STANDING, GOD_LEVEL1);
  commandArray[CMD_POWERS] = new commandInfo("powers", POSITION_STANDING, GOD_LEVEL1);
  commandArray[CMD_WHITTLE] = new commandInfo("whittle", POSITION_STANDING, 0);
  commandArray[CMD_MESSAGE] = new commandInfo("message", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_SMOKE] = new commandInfo("smoke", POSITION_RESTING, 0);
  commandArray[CMD_CLIENTMESSAGE] = new commandInfo("clientmessage", POSITION_SLEEPING, 0);
  commandArray[CMD_SEDIT] = new commandInfo("sedit", POSITION_DEAD, GOD_LEVEL1);
  commandArray[CMD_RETRAIN] = new commandInfo("retrain", POSITION_STANDING, 0);
  commandArray[CMD_VISIBLE] = new commandInfo("visible", POSITION_STANDING, 0);
}

bool _parse_name(const char *arg, char *name)
{
  char buf[80];
  unsigned int i;

  for (; isspace(*arg); arg++);

  if (strlen(arg) < 3)
    return TRUE;

  for (i = 0; *illegalnames[i] != '\n'; i++) {
    if (*illegalnames[i] == '*') {
      if (strstr(lower(arg).c_str(), illegalnames[i] + 1))
        return TRUE;
    } else {
      if (!strcasecmp(illegalnames[i], arg))
        return TRUE;
    }
  }
  if (!AllowPcMobs) {
    for (i= 0; i < mob_index.size(); i++) {
      sprintf(buf, fname(mob_index[i].name).c_str());
      if (!strcasecmp(buf, arg))
        return TRUE;
    }
  }
#if 0
  for (i= 0; i < obj_index.size(); i++) {
    sprintf(buf, fname(obj_index[i].name));
    if (!strcasecmp(buf, arg))
      return TRUE;
  }
#endif    
  for (i = 0; (*name = *arg); arg++, i++, name++)
    if ((*arg < 0) || !isalpha(*arg) || i > 15)
      return TRUE;

  if (!i)
    return TRUE;

  return FALSE;
}

#if 0
int min_stat(race_t race, statTypeT iStat)
{
  // 1=str 2=dex 3=int 4=wis 5=con 6=chr 
  if (iStat == 1) {                                                              
    if (race == RACE_DWARF) 
      return (8);            
    else if (race == RACE_GNOME)                                                 
      return (6);                                                              
    else if (race == RACE_OGRE)                                                 
      return (7);                                                              
    else                                                                        
      return (3);                                                              
  } else if (iStat == 2) {                                                       
    if (race == RACE_HOBBIT)
      return (8);                                                              
    else if (race == RACE_OGRE)
      return (5); 
    else if (race == RACE_ELVEN)
      return (6);
    else
      return (3);
  } else if (iStat == 3) {                                                       
    if ((race == RACE_GNOME) || (race == RACE_ELVEN))                           
      return (7);                                                               
    else                                                                        
      return (3);
  } else if (iStat == 4) {                                                       
    if ((race == RACE_GNOME) || (race == RACE_ELVEN)) 
      return (7);                                                              
    else                                                                        
      return (3);                                                              
  } else if (iStat == 5) {                                                       
    if ((race == RACE_DWARF) || (race == RACE_HOBBIT))
      return (8); 
    else if (race == RACE_OGRE)                                                
      return (7);                                                              
    else                                                                        
      return (3);                                                              
  } else if (iStat == 6) {                                                       
    if (race == RACE_ELVEN)                                                     
      return (7);                                                              
    else if (race == RACE_HOBBIT)
      return (6);
    else                                                                        
      return (3);                                                              
  }                                                                             
  return (3);                                                                  
} 


int max_stat(race_t race, statTypeT iStat)
{
  // 1=str 2=dex 3=int 4=wis 5=con 6=chr 

  if (iStat == 1) {
    if ((race == RACE_HOBBIT))
      return (14);
    else if (race == RACE_ELVEN)
      return (14);
    else if (race == RACE_GNOME) 
      return (14);
    else if (race == RACE_OGRE)
      return (19);
    else
      return (18);
  } else if (iStat == 2) {
    if (race == RACE_DWARF)
      return (14);
    else if (race == RACE_HOBBIT)
      return (19);
    else if (race == RACE_OGRE)
      return (14);
    else if (race == RACE_GNOME)
      return 16;
    else
      return (18);
  } else if (iStat == 3) {
    if (race == RACE_OGRE)
      return (15);
    else if (race == RACE_ELVEN)
      return (19);
    else if (race == RACE_HOBBIT)
      return (14);
    else if (race == RACE_DWARF)
       return 14;
    else
      return (18);
  } else if (iStat == 4) {
    if (race == RACE_GNOME)
      return (19);
    else if (race == RACE_OGRE)
      return (15);
    else if (race == RACE_HOBBIT)
      return (14);
    else if (race == RACE_DWARF)
      return 14;
    else
      return (18);
  } else if (iStat == 5) {
    if (race == RACE_DWARF)
      return (19);
    else if (race == RACE_GNOME)
      return (14);
    else if (race == RACE_ELVEN)
      return (12);
    else
      return (18);
  } else if (iStat == 6) {
    if (race == RACE_DWARF)
      return (15);
    else if ((race == RACE_OGRE))
      return (12);
    else if (race == RACE_GNOME)
      return 14;
    else
      return (18);
  }
  return (18);
}
#endif

// This will put a command into player's command que.
// for mobs, it causes instanteous execution (no que)
// will return DELETE_THIS if this should be deleted
int TBeing::addCommandToQue(const char *msg)
{
  int rc;

  if (isPc() && desc){
    if (!isPlayerAction(PLR_MAILING) && 
        desc->connected != CON_WRITING) 
    (&desc->input)->putInQ(msg);
  } else {
    rc = parseCommand(msg, TRUE);
    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_THIS;
  }
  return FALSE;
}

void sprintbit(unsigned long vektor, const char * const names[], char *result)
{
  long nr;

  *result = '\0';

  for (nr = 0; vektor; vektor >>= 1) {
    if (IS_SET(vektor, (unsigned long) 1L))
      if (*names[nr] == '\n') {
        //strcat(result, "UNDEFINED");
        //strcat(result, " ");
      } else if (*names[nr]) {
        strcat(result, names[nr]);
        strcat(result, " ");
      }
    if (*names[nr] != '\n')
      nr++;
  }

  if (!*result)
    strcat(result, "NOBITS");
}

void sprinttype(int type, const char * const names[], char *result)
{
  int nr;

  for (nr = 0; (*names[nr] != '\n'); nr++);
  if (type < nr)
    strcpy(result, names[type]);
  else
    strcpy(result, "UNDEFINED");
}

string lower(string s)
{
  string::size_type iter;

  do {
    iter = s.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (iter != string::npos)
      s.replace(iter, 1, tolower(s[iter]));
  } while (iter != string::npos);

  return s;
}

void makeLower(char *s)
{
  char *p;

  for (p = s; *p; p++)
    if (isupper(*p))
      *p = tolower(*p);
}

#if (!defined SUN && !defined LINUX && !defined(SOLARIS))
char *strstr(const char *s1, const char *s2)
{
  if (!*s2)
    return s1;  // conformance with strstr

  int j = strlen(s1) - strlen(s2);
  if (j < 0)
    return NULL;  // conformance with strstr
  int i, k = strlen(s2);
  for (i = 0; i <= j && strncmp(&s1[i], s2, k) != 0; i++);

  return (i > j) ? NULL : &s1[i];
}
#endif

const char *strcasestr(const char *s1, const char *s2)
{
  if (!*s2)
    return s1;  // conformance with strstr

  int j = strlen(s1) - strlen(s2);
  if (j < 0)
    return NULL;  // conformance with strstr
  int i, k = strlen(s2);
  for (i = 0; i <= j && strncasecmp(&s1[i], s2, k) != 0; i++);

  return (i > j) ? NULL : &s1[i];
}

// I redid this function to make it more flexible. It has a new argument  
// for the array, making it useful with any array.  - Russ                
void bisect_arg(const char *arg, int *field, char *string, const char * const array[])
{
  char buf[MAX_INPUT_LENGTH];

  arg = one_argument(arg, buf);
  if (!(*field = old_search_block(buf, 0, strlen(buf), array, 0)))
    return;

  for (; isspace(*arg); arg++);
  for (; (*string = *arg); arg++, string++);

  return;
}

char *cap(char *s)
{
  char *letter;
  int counter = 0;
  int i;

  if (!s || !*s) {
    return s;
  }

  letter = s; 

  if ((*letter != '<') && (*letter != '\0')) {
    if (s && islower(*s)) {
      *s = toupper(*s);
    }
    return s;
  } else {
// Accounting for Items with color strings and % as first character
    for (i = 0; *letter; letter++, i++) {
        if (*letter == '<')
          counter = 0;
        else 
          counter++;
      if (counter == 3) {
        if (letter && islower(*letter))
          *letter = toupper(*letter);
        
        s[i] = *letter;
        return s;
      } else 
        s[i] = *letter;       
    }
    return s;
  }
}

string good_cap(const string cp)
{
  char buf[1024];
  strcpy(buf, cp.c_str());
  return cap(buf);
}

char *uncap(char *s)
{
  char *letter;
  int counter = 0;
  int i;

  if (!s) {
    return s;
  }

  letter = s;

  if (*letter != '<') {
    if (s && isupper(*s)) {
      *s = tolower(*s);
    }
    return s;
  } else {
// Accounting for Items with color strings and % as first character
    for (i = 0; *letter; letter++, i++) {
        if (*letter == '<')
          counter = 0;
        else
          counter++;
      if (counter == 3) {
        if (letter && isupper(*letter)) {
          *letter = tolower(*letter);
        }
        s[i] = *letter;
        return s;
      } else {
        s[i] = *letter;
      }
    }
    return s;
  }
}

string good_uncap(const string cp)
{
  char buf[1024];
  strcpy(buf, cp.c_str());
  return uncap(buf);
}

char *fold(char *line)
{
  const unsigned int FOLDAT = 78;

  int i, j = 0, folded;

  if (strlen(line) > FOLDAT)
    for (i = FOLDAT; i < (int) strlen(line); i += FOLDAT) {
      folded = FALSE;
      for (j = i; !folded; j--)
        if (line[j] == ' ') {
          line[j] = '\n';
          folded = TRUE;
          i = j;
        }
    }
  return line;
}

int ctoi(char c)
{
  char buf[5];

  sprintf(buf, "%c", c);
  return atoi(buf);
}

// essentially, strips out multiple ' ' truncating to a single space
void cleanCharBuf(char *buf)
{
  char *from, *to;
  char p =' ';

  from = to = buf;

  while(*from) {
    if (*from != ' ' || (*from == ' ' && p != ' ')) 
      *to++ = *from;
    p = *from++;
  }

  if (p == ' ' && to != buf) 
    *(to-1) = '\000';
  else 
    *to = '\000';
}

int startsVowel(const char *string)
{
  for (;*string && isspace(*string); string++);

  switch (*string) {
    case 'A':
    case 'E':
    case 'I':
    case 'O':
    case 'U':
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
      return TRUE;
    default:
      return FALSE;
  }
} 

void str_shiftleft(char *str, int n)
{
  char *cp;

  for (cp = str + n; (*(cp - n) = *cp); cp++);
}

string nextToken(char delim, unsigned int maxSize, char *str)
{
  char retbuf[256];
  char *cp;

  for (cp = str; *cp && (*cp != delim); cp++);
  if ((cp - str) > (int) maxSize) {
    strncpy(retbuf, str, maxSize);
    retbuf[maxSize] = '\0';
  } else {
    strncpy(retbuf, str, (int) (cp - str));
    retbuf[(int) (cp - str)] = '\0';
  }
  if (!*cp)
    *str = '\0';
  else
    str_shiftleft(str, (int) (cp - str + 1));
 
  return retbuf;
}

// we use to restrict this to MAX_STRIN_LENGTH in size, we no longer do...
char * mud_str_dup(const char *buf)
{
  char *tmp = NULL;
  if (!buf)
    return NULL;

  try {
    tmp = new char[strlen(buf) + 1];
  } catch (...) {
    mud_assert(0, "exception caught in mud_str_dup");
  }
  strcpy(tmp, buf);
  return tmp;
}

void trimString(string &arg)
{
  if (arg.empty())
    return;
  size_t iter = arg.find_first_not_of(" \n\r");
  if (iter == string::npos)
    return;
  arg.erase(0, iter);  // erase the leading whitespace
}

int stringncmp(const string str1, const string str2, unsigned int len)
{
  // ANSI doesn't provide a strncmp for strings
  // I've hacked this together cuz I think it's needed

  // trunc down to length and compare
  return string(str1, 0, len).compare(string(str2, 0, len));
}

int atoi(const string &num)
{
  return atoi(num.c_str());
}

commandInfo::~commandInfo()
{
}

// take what is in the output buffer, cat into single string, and page it
void TBeing::makeOutputPaged()
{
  if (!desc)
    return;

  string str;
  char buf[MAX_STRING_LENGTH];

  memset(buf, '\0', sizeof(buf));
  while (desc->output.takeFromQ(buf)) {
    str += buf;
    memset(buf, '\0', sizeof(buf));
  }

  desc->page_string(str.c_str(), 0);
}
