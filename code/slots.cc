//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: slots.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


/*************************************************************************

      SneezyMUD 3.0 - All rights reserved, SneezyMUD Coding Team
      "slots.c" - All functions and routines related to the slot machines 
      
      Slot Machines coded by Russ Russell, January 1993. Last revision
      April 6th, 1994.

*************************************************************************/

#include "stdsneezy.h"
#include "games.h"

void spin_slot(TBeing *ch);

static const char *ChooseFirstFruit()
{
  int num;
  static const char *fruits[8] =
  {
    "cherry",
    "lime",
    "orange",
    "apple",
    "banana",
    "bell",
    "bally",
    "Seven",
  };

  num = number(1, 15);

  if (num == 1)
    return (fruits[7]);
  else if (num <= 3)
    return (fruits[6]);
  else if (num <= 12)
    return (fruits[number(1, 5)]);
  else
    return (fruits[0]);
}

static const char *ChooseSecondFruit()
{
  int num;
  static const char *fruits[8] =
  {
    "cherry",
    "lime",
    "orange",
    "apple",
    "banana",
    "bell",
    "bally",
    "Seven",
  };
  num = number(1, 15);
  if (num == 1)
    return (fruits[7]);
  else if (num <= 3)
    return (fruits[6]);
  else if (num <= 12)
    return (fruits[number(1, 5)]);
  else
    return (fruits[0]);
}

static const char *ChooseThirdFruit()
{
  int num;
  static const char *fruits[8] =
  {
    "cherry",
    "lime",
    "orange",
    "apple",
    "banana",
    "bell",
    "bally",
    "Seven",
  };
  num = number(1, 15);
  if (num == 1)
    return (fruits[7]);
  else if (num <= 3)
    return (fruits[6]);
  else if (num <= 12)
    return (fruits[number(1, 5)]);
  else
    return (fruits[0]);
}

bool TBeing::checkSlots() const
{
  if ((in_room < 8414) && (in_room > 8403))
    return TRUE;
  else
    return FALSE;
}

bool TBeing::checkSlotPlayer() const
{
  const TBeing *better = NULL;
  TThing *t;

  for (t = roomp->stuff; t; t = t->nextThing) {
    better = dynamic_cast<const TBeing *>(t);
    if (!better)
      continue;
    if (better == this)
      continue;
    if (better->getPosition() == POSITION_SITTING)
      return TRUE;
  }
  return FALSE;
}

void TBeing::doPlay(const char *arg)
{
  char game[256], options[256];
  Descriptor *d;

  if (!(d = desc))
    return;

  if (gGin.check(this)) {
    gGin.play(this, arg);
    return;
  }
  if (checkHearts()) {
    gHearts.play(this, arg);
    return;
  }
  if (checkCrazyEights()) {
    gEights.play(this, arg);
    return;
  }
  if (checkDrawPoker()) {
    gPoker.discard(this, arg);
    return;
  }
  half_chop(arg, game, options);

  if (!*game) {
    sendTo("Syntax: play <game>\n\r");
    sendTo("Games include : Craps, Roulette, Slots, and 21\n\r");
    return;
  } else if (is_abbrev(game, "slots")) {
    if (!checkSlots()) {
      sendTo("Do you have a gambling problem?  There's no slot machine here!\n\r");
      return;
    }
    if (!getPosition() == POSITION_SITTING) {
      sendTo("You must sit at the slot machine to play it.\n\r");
      return;
    }
    if (!*options) {
      sendTo("Slot machine options :\n\r");
      sendTo("1) Play the cheap slots, and bet 1 coins (100 talens).\n\r");
      sendTo("2) Play the cheap slots, and bet 2 coins (200 talens).\n\r");
      sendTo("3) Play the cheap slots, and bet 3 coins (300 talens).\n\r");
      sendTo("4) Play the expensive slots and bet 1 coin (1000 talens).\n\r");
      sendTo("5) Play the expensive slots and bet 2 coins (2000 talens).\n\r");
      sendTo("6) Play the expensive slots and bet 3 coins (3000 talens).\n\r");
      sendTo("You MUST play the 3000 talens machine to have a chance at the jackpot.\n\r");
    } else {
      if (!strcmp(options, "1")) {
	if (getMoney() >= 100) {
	  d->bet.slot = 100;
	  addToMoney(-100, GOLD_GAMBLE);
 
	  spin_slot(this);
	} else {
	  sendTo("You search your pockets for talens but come up empty-handed.\n\r");
	  return;
	}
      } else if (!strcmp(options, "2")) {
	if (getMoney() >= 200) {
	  d->bet.slot = 200;
	  addToMoney(-200, GOLD_GAMBLE);
	  spin_slot(this);
	} else {
	  sendTo("You search your pockets for talens but come up empty-handed.\n\r");
	  return;
	}
      } else if (!strcmp(options, "3")) {
	if (getMoney() >= 300) {
	  d->bet.slot = 300;
	  addToMoney(-300, GOLD_GAMBLE);
	  spin_slot(this);
	} else {
	  sendTo("You search your pockets for talens but come up empty-handed.\n\r");
	  return;
	}
      } else if (!strcmp(options, "4")) {
	if (getMoney() >= 1000) {
	  d->bet.slot = 1000;
	  addToMoney(-1000, GOLD_GAMBLE);
	  spin_slot(this);
	} else {
	  sendTo("You search your pockets for talens but come up empty-handed.\n\r");
	  return;
	}
      } else if (!strcmp(options, "5")) {
	if (getMoney() >= 2000) {
	  d->bet.slot = 2000;
	  addToMoney(-2000, GOLD_GAMBLE);
	  spin_slot(this);
	} else {
	  sendTo("You search your pockets for talens but come up empty-handed.\n\r");
	  return;
	}
      } else if (!strcmp(options, "6")) {
	if (getMoney() >= 3000) {
	  d->bet.slot = 3000;
	  addToMoney(-3000, GOLD_GAMBLE);
	  spin_slot(this);
	} else {
	  sendTo("You search your pockets for talens but come up empty-handed.\n\r");
	  return;
	}
      } else {
	sendTo("Incorrect Option.\n\r");
	return;
      }
    }
  } else if (is_abbrev(game, "roulette")) {
    if (!*options) {
      sendTo("\n\r                  (()()()()()()())\n\r");
      sendTo("                  |    |    |    |\n\r");
      sendTo("__________________|____|____|____|\n\r");
      sendTo("| 1 - 18 |        | 01 | 02 | 03 |\n\r");
      sendTo("|________|01-->12 | 04 | 05 | 06 |\n\r");
      sendTo("|  Even  |        | 07 | 08 | 09 |\n\r");
      sendTo("|________|________| 10 | 11 | 12 |\n\r");
      sendTo("|        |        | 13 | 14 | 15 |\n\r");
      sendTo("|________|13-->24 | 16 | 17 | 18 |\n\r");
      sendTo("|        |        | 19 | 20 | 21 |\n\r");
      sendTo("|________|________| 22 | 23 | 24 |\n\r");
      sendTo("|  Odd   |        | 25 | 26 | 27 |\n\r");
      sendTo("|________|25-->36 | 28 | 29 | 30 |\n\r");
      sendTo("| 19 -36 |        | 31 | 32 | 33 |\n\r");
      sendTo("|________|________| 34 | 35 | 36 |\n\r");
      sendTo("                  |====|====|====|\n\r");
      sendTo("                  |    |    |    |\n\r");
      sendTo("                  |    |    |    |\n\r");
      sendTo("                  (()()()()()()())\n\r");
      return;
    }
  } else if (is_abbrev(game, "craps")) {
    if (!*options) {
      sendTo("\n\rCraps table options : \n\r");
      sendTo("1)    Bet on the come out roll.\n\r");
      sendTo("2)    Bet against the come out roll.\n\r");
      sendTo("In order to place a bet, you must first use the bet command to determine\n\r");
      sendTo("the amount of the bets you will be making.Unfortunately, all your bets must\n\r");
      sendTo("remain constant throughout the dice roll. You can always bet on new things,\n\r");
      sendTo("but your bet must be the same. You can change the amount you are betting \n\r");
      sendTo("only after a point roll has been changed.\n\r\n\r");
      sendTo("Type help craps for more help on this. It is most probably confusing at\n\r");
      sendTo("first, but it works just like a real craps table.\n\r");

      return;
    } else {
      if (isdigit(*options)) {
	// int option = atoi(options);
#if 0
	m_craps->rollDice();
#endif

	return;
      } else {
	sendTo("Options need to be numbers between 1-20\n\r");
	return;
      }
    }
  } else if (is_abbrev(game, "21") || is_abbrev(game, "blackjack")) {
    sendTo("See HELP BLACKJACK for details and procedures.\n\r");
    return;
  }
}

void spin_slot(TBeing *ch)
{
  char buf[256];
  const char *fruit1;
  const char *fruit2;
  const char *fruit3;
  TObj *slot;
  TMoney *coins;
  unsigned int bits;
  TBeing *tmp_char;

  ch->sendTo("You stick your talens in the machine.\n\r");
  ch->sendTo("You pull the arm of the slot machine.\n\r");

  fruit1 = ChooseFirstFruit();

  if (strcmp(fruit1, "Seven")) {
    if (!number(0, 15)) {
      fruit2 = fruit1;
      fruit3 = fruit1;
    } else {
      fruit2 = ChooseSecondFruit();
      fruit3 = ChooseThirdFruit();
    }
  } else {
    fruit2 = ChooseSecondFruit();
    fruit3 = ChooseThirdFruit();
  }
  ch->sendTo("%-10s %-10s %-10s\n\r", fruit1, fruit2, fruit3);
  sprintf(buf, "$n spins a [%-10s %-10s %-10s]", fruit1, fruit2, fruit3);
  act(buf, FALSE, ch, 0, 0, TO_ROOM);

  bits = generic_find("slot", FIND_OBJ_ROOM, ch, &tmp_char, &slot);

  if (!strcmp(fruit1, "cherry")) {
    if (strcmp(fruit1, fruit2)) {
      ch->sendTo("You win!\n\r");
      coins = create_money(ch->desc->bet.slot);
      if (coins && (bits == FIND_OBJ_ROOM)) {
	*slot += *coins;
	ch->desc->bet.slot = 0;
	return;
      } else {
	vlogf(10, "No slot machine in room %d", ch->in_room);
	return;
      }
    } else {
      ch->sendTo("You win!\n\r");
      ch->sendTo("Some talens fall into the slot.\n\r");
      coins = create_money(2 * ch->desc->bet.slot);
      if (coins && (bits == FIND_OBJ_ROOM)) {
	*slot += *coins;
	ch->desc->bet.slot = 0;
	return;
      } else {
	vlogf(10, "No slot machine in room %d", ch->in_room);
	return;
      }
    }
  } else if (!strcmp(fruit1, fruit2) && !strcmp(fruit2, fruit3)) {
    ch->sendTo("You win!\n\r");
    ch->sendTo("Some talens fall into the slot.\n\r");
    coins = create_money(6 * ch->desc->bet.slot);
    if (coins && (bits == FIND_OBJ_ROOM)) {
      *slot += *coins;
      ch->desc->bet.slot = 0;
      return;
    } else {
      vlogf(10, "No slot machine in room %d", ch->in_room);
      return;
    }
  } else if (!strcmp(fruit3, "bally") && !strcmp(fruit2, fruit1)) {
    ch->sendTo("You win!\n\r");
    ch->sendTo("Some talens fall into the slot.\n\r");
    coins = create_money(20 * (ch->desc->bet.slot));
    if (coins && (bits == FIND_OBJ_ROOM)) {
      *slot += *coins;
      ch->desc->bet.slot = 0;
      return;
    } else {
      vlogf(10, "No slot machine in room %d", ch->in_room);
      return;
    }
  } else {
    ch->sendTo("You lose!\n\r");
    ch->desc->bet.slot = 0;
    return;
  }
}
