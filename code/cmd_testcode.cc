//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: cmd_testcode.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.5  1999/10/08 19:27:55  batopr
// *** empty log message ***
//
// Revision 1.4  1999/10/08 19:26:54  batopr
// Added stuff for calculating money equilibrium
//
// Revision 1.3  1999/09/14 02:24:58  batopr
// *** empty log message ***
//
// Revision 1.2  1999/09/13 14:27:25  batopr
// *** empty log message ***
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#include "stdsneezy.h"
#include "statistics.h"

void TBeing::doTestCode(const char *arg)
{
  if (!hasWizPower(POWER_WIZARD)) {
    sendTo("Prototype command.  You need to be a developer to use this.\n\r");
    return;
  }

  act("test code", FALSE, this, 0, 0, TO_CHAR, "\033[1;30m");

#if 1
  unsigned int shopPos = getPosGoldShops();
  int shopNet = getNetGoldShops();
  int shopDrain = shopPos - shopNet;

  float old_shop_mod = gold_modifier[GOLD_SHOP];

  // target shop is to drain 1.05 * what it provides
  float new_shop_mod = old_shop_mod * shopDrain / shopPos / 1.05;

  sendTo("Theoretical shop equilibrium modifier:   %.2f\n\r", new_shop_mod);

  // calculate the "budget" portion of the shop values
  unsigned int shopPosBudg = shopPos - getPosGold(GOLD_SHOP_PET);
  int shopNetBudg = shopNet - getNetGold(GOLD_SHOP_PET);
  int shopDrainBudg = shopPosBudg - shopNetBudg;
  
  // tweak the budget values for a "corrected" shop system
  // the drain would be the same, but the pos would only be:
  int shopPosNew = (int) (shopDrainBudg / 1.05);
  int shopDrainNew = shopDrainBudg;
  int shopNetNew = shopPosNew - shopDrainNew;

  unsigned int budgPosTrue = getPosGoldBudget();
  int budgNetTrue = getNetGoldBudget();
  int budgDrainTrue = budgPosTrue - budgNetTrue;

  int budgPosNew = budgPosTrue - shopPosBudg + shopPosNew;
  int budgDrainNew = budgDrainTrue - shopDrainBudg + shopDrainNew;
  int budgNetNew = budgPosNew - budgDrainNew;

  // repair
  int drainRepAct = getPosGold(GOLD_REPAIR) - getNetGold(GOLD_REPAIR);
  int drainNoRep = budgDrainNew - drainRepAct;
  int drainDes = (int) (budgPosNew * 0.90);
  int drainRepDes = drainDes - drainNoRep;
  float old_repair_factor = gold_modifier[GOLD_REPAIR];
  float new_repair_factor = drainRepDes / drainRepAct * old_repair_factor;
  sendTo("Theoretical repair equilibrium modifier:   %.2f\n\r", new_repair_factor);

  // income
  int posIncAct = getPosGold(GOLD_INCOME);
  int posNoInc = budgPosNew - posIncAct;
  int posDes = (int) (budgDrainNew * 1.02);
  int posIncDes = posDes - posNoInc;
  float old_inc_factor = gold_modifier[GOLD_INCOME];
  float new_inc_factor = posIncDes / posIncAct * old_inc_factor;
  sendTo("Theoretical income quilibrium modifier: %.2f\n\r", new_inc_factor);
#endif
#if 0
  char arg1[256];
  arg = one_argument(arg, arg1);
  int spell = atoi(arg1);

  if (spell < 0 || !discArray[spell]) {
    sendTo("Bad spell\n\r");
    return;
  }
  // don't confuse requireHolySym
  if (isImmortal()) {
    sendTo("go mortal\n\r");
    return;
  }
  if (!hasClass(CLASS_CLERIC)) {
    sendTo("Be a cleric\n\r");
    return;
  }

  TSymbol *sym = dynamic_cast<TSymbol *>(heldInPrimHand());
  if (!sym) {
    sendTo("need symbol\n\r");
    return;
  }
  int sym_num = sym->number;

  unsigned int breaks = 0;
  unsigned int iters = 0;
  char dummy[MAX_STRING_LENGTH];
  do {
    sym->setSymbolFaction(getFaction());

    do {
       enforceHolySym(this, spell, true);
       iters++;
       // flush
       while (desc->output.takeFromQ(dummy));
    } while (heldInPrimHand() && iters < 10000000); 
    breaks++;

    sym = dynamic_cast<TSymbol *>(read_object(sym_num, REAL));
    equipChar(sym, getPrimaryHold());
  } while (breaks < 1000);

  sendTo("Spell (%s:%d) cast successfully %d total times (avg:%.1f).\n\r",
     discArray[spell]->name, spell, iters, (float) iters / breaks);
  delete sym;
#endif
#if 0
  extern void nukeSomePfiles();
  nukeSomePfiles();
#endif
#if 0
  sendTo("Checking array boundaries\n\r");
  for (int i = 5;i>=-1;i--) {
    if (discArray[i])
      sendTo("%s\n\r", discArray[i]->name); 
  }
  sendTo("Checking new/delete\n\r");
  char *s = mud_str_dup("test string");
  delete s;
  sendTo("Checking unused.\n\r");
  int x = 5;
  int j;
  sendTo("done\n\r");
#endif
#if 0
  string str1, str2, strret;
  string strin = "this is input";
  strret = two_arg(strin, str1, str2);
  sendTo("in: '%s'\n\r", strin.c_str());
  sendTo("1: '%s'\n\r", str1.c_str());
  sendTo("2: '%s'\n\r", str2.c_str());
  sendTo("ret: '%s'\n\r", strret.c_str());
#endif
#if 0
  extern void rewrite_obj_file();
  rewrite_obj_file();
  return;
#endif
#if 0
  TBeing *ch;
  int i;
  TRoom * rp = real_roomp(ROOM_VOID);
  for (i = 0; i < mob_index.size(); i++) {
    ch = read_mobile(i, REAL);
    *rp += *ch;
    if (ch->canFly() && !ch->isFlying())
      sendTo("Mob '%s' : %d not flying.\n\r", ch->getName(), ch->mobVnum());
    delete ch;
  }
  return;
#endif
#if 0
  cmdTypeT i;
  TThing *note = NULL;
  note = searchLinkedListVis(this, "exec script",  stuff);

  for (i = MIN_CMD; i < MAX_CMD_LIST;i++) {
    if ((i == CMD_QUIT2) || (i == CMD_TESTCODE) || (i == CMD_SHUTDOWN) ||
        (i == CMD_BUG) || (i == CMD_IDEA) || (i == CMD_TYPO) ||
        (i == CMD_COMMENT) || (i == CMD_DESCRIPTION) || (i == CMD_REDIT) ||
        (i == CMD_VIEWOUTPUT) || (i == CMD_PRACTICE) || (i == CMD_COMMAND) ||
        (i == CMD_NEWS) || (i == CMD_WIZNEWS) || (i == CMD_CREDITS) ||
        (i == CMD_SIT) || (i == CMD_REST) || (i == CMD_SLEEP) || 
        (i == CMD_CRAWL) || (i == CMD_WIZLIST) || (i == CMD_MEDITATE) ||
        (i == CMD_PENANCE) || (i == CMD_IMMORTAL) || (i == CMD_TRACEROUTE) ||
        (i == CMD_MID) || (i == CMD_LOGLIST) || (i == CMD_BRUTTEST))
      continue;
    vlogf(-1, "%s : con %d", commandArray[i]->name, desc->connected);
    doCommand(i, "", NULL, FALSE);
    if (note) {
      char *tmp = note->action_description;
      if (tmp) {
        note->action_description = new char[strlen(tmp) + strlen(commandArray[i]->name) + 3];
        strcpy(note->action_description, tmp);
        strcat(note->action_description, commandArray[i]->name);
        strcat(note->action_description, "\n");
        delete [] tmp;
      } else {
        note->action_description = new char[strlen(commandArray[i]->name) + 2];
        strcpy(note->action_description, commandArray[i]->name);
        strcat(note->action_description, "\n");
      }
    }
  }
  return;
#endif
#if 0  
  TThing *obj = NULL;
  char tmpbuf[256];

  sendTo("You are in zone %d.\n\r", roomp->getZone());
  return;

  one_argument(arg, tmpbuf);
  if (!(obj = searchLinkedListVis(this, tmpbuf, stuff)))
    return;
  TBaseClothing *tbc = dynamic_cast<TBaseClothing *>(obj);
  if (!tbc) return;

  sendTo("item: %s, level: %d,  base: %d\n\r",
        tbc->getName(), tbc->armorLevel(), tbc->armorPriceStruct(0));
#endif
#if 0
  one_argument(arg, tmpbuf);
  i = atoi(tmpbuf);
  if (i < 0 || i > 7)
    i = 0;
  calcNewPracs(i, FALSE);
#endif
}

void TBeing::doBruttest(const char *)
{
  if (!hasWizPower(POWER_WIZARD)) {
    sendTo("Prototype command.  You need to be a developer to use this.\n\r");
    return;
  }

#if 0
  char test[] = {IAC, WILL, '\x03', '\0'};
  char test2[] = {IAC, DO, '\x18', '\0'};

  desc->EchoOff();
  write(desc->socket->sock, test, 4);
  write(desc->socket->sock, test2, 4);

  doCls(false);
  sendTo(VT_CURSPOS, 1, 1);
  desc->connected = CON_EDITTING;
  desc->edit.x = desc->edit.y = 1;
#endif
}

