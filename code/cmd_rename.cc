/*************************************************************************

      SneezyMUD++ - All rights reserved, SneezyMUD Coding Team
      "cmd_rename.cc" - the rename command and related things

*************************************************************************/

#include <unistd.h>

#include "stdsneezy.h"

// this function gets a bit recursive, be careful
static void renamePersonalizeFix(TThing *t, const char * orig_name, const char * new_name)
{
  if (!t)
    return;
  TObj *obj = dynamic_cast<TObj *>(t);
  if (!obj)
    return;

  if (obj->isPersonalized() && obj->action_description) {
    char buf[256], persbuf[256];
    strcpy(buf, obj->action_description);
    if ((sscanf(buf, "This is the personalized object of %s.", persbuf)) == 1) {
      if (!strcmp(persbuf, orig_name)) {
        // we are personalized already, so not bothering to swap to strung again
        vlogf(LOG_MISC, "Personalized object (%s) on %s, being restrung.",
           obj->getName(), new_name);
        sprintf(buf, "This is the personalized object of %s", new_name);
        delete [] obj->action_description;
        obj->action_description = mud_str_dup(buf);
      }
    }
  }

  renamePersonalizeFix(t->stuff, orig_name, new_name);
  renamePersonalizeFix(t->nextThing, orig_name, new_name);
}

void TBeing::doNameChange(const char *argument)
{
  char orig_name[80], new_name[80], tmpbuf[160], tmp_name[80];
  char tmpbuf2[160], arg[80];
  TBeing *vict;
  FILE *fp;

  argument = one_argument(argument, orig_name);
  argument = one_argument(argument, new_name);

  if (!orig_name || !*orig_name || !new_name || !*new_name) {
    sendTo("Syntax: rename <person> <new_name>\n\r");
    return;
  }
  if (_parse_name(new_name, tmp_name)) {
    sendTo("Illegal name for new_name.\n\r");
    return;
  }
  if (!(vict = get_char_room_vis(this, orig_name))) {
    sendTo("You don't see them here.\n\r");
    return;
  }
  // in case they abbreviated the person's name, grab the true name
  strcpy(orig_name, vict->getName());

  TMonster *mons = dynamic_cast<TMonster *>(vict);
  if (!isImmortal() || !hasWizPower(POWER_RENAME)) {
    // mortal rename for pets
    // have both checks above so Imm can go mort to rename a pet

    if (!mons) {
      sendTo("You can't rename that.\n\r");
      return;
    }

    if (mons->mobVnum() <= 0) {
      sendTo("You can't rename that.\n\r");
      return;
    }
    if (mons->isPc()) {
      sendTo("You can't rename players.\n\r");
      return;
    }
    if (IS_SET(mons->specials.act, ACT_STRINGS_CHANGED)) {
      act("$N already has a name.", FALSE, this, 0, mons, TO_CHAR);
      return;
    }
    if (!isImmortal()) {
      if (!mons->isPet(PETTYPE_PET | PETTYPE_THRALL) || mons->master != this) {
        act("$N is not your pet.", FALSE, this, 0, mons, TO_CHAR);
        return;
      }
    }
    charFile st;
    if (load_char(new_name, &st)) {
      act("You can't name $N that.", FALSE, this, 0, mons, TO_CHAR);
      return;
    }
    char tmpname[20];
    if (_parse_name(new_name, tmpname)) {
      act("You can't name $N that.", FALSE, this, 0, mons, TO_CHAR);
      return;
    }
    
    mons->swapToStrung();

    //  Remake the pet's name.  
    sprintf(tmpbuf, "%s %s", mons->name, new_name);
    delete [] mons->name;
    mons->name = mud_str_dup(tmpbuf);

    // remake the short desc
    sprintf(tmpbuf2, mons->getName());
    one_argument(tmpbuf2, arg);
    if (!strcmp(arg, "a") || !strcmp(arg, "an"))
      sprintf(tmpbuf, "\"%s\", the%s", good_cap(new_name).c_str(), 
             one_argument(tmpbuf2, arg));
    else
      sprintf(tmpbuf, "\"%s\", %s", good_cap(new_name).c_str(), mons->getName());

    delete [] mons->shortDescr;
    mons->shortDescr = mud_str_dup(tmpbuf);

    // remake the long desc
    strcat(tmpbuf, " is here.\n\r");
    delete [] mons->player.longDescr;
    mons->player.longDescr = mud_str_dup(tmpbuf);

    act("You grant $N a new name.",
        FALSE, this, 0, mons, TO_CHAR);
    act("$n grants $N a new name.",
        FALSE, this, 0, mons, TO_NOTVICT);
    act("$n grants you a new name.",
        FALSE, this, 0, mons, TO_VICT);
    return;
  }

  if (vict == this) {
    sendTo("You can't change your own name, sorry.\n\r");
    return;
  }
  if (!vict->desc) {
    sendTo("That creature doesn't seem to be connected.\n\r");
    return;
  }
  if (!vict->desc->account) {
    sendTo("That creature lacks an account.\n\r");
    return;
  }
  if (desc->checkForCharacter(tmp_name)) {
    sendTo("That player already exists.\n\r");
    return;
  }

  // check for corspse file
  sprintf(tmpbuf, "corpses/%s", lower(orig_name).c_str());
  if ((fp = fopen(tmpbuf, "r"))) {
    fclose(fp);
    sendTo("That player has a corpse file.\n\r");
    return;
  }

  strcpy(tmp_name, cap(tmp_name));
  delete [] vict->name;
  vict->name = mud_str_dup(cap(tmp_name));

  wipePlayerFile(orig_name);
  wipeRentFile(orig_name);

  sprintf(tmpbuf, "account/%c/%s", LOWER(vict->desc->account->name[0]),
                  lower(vict->desc->account->name).c_str());
  sprintf(tmpbuf + strlen(tmpbuf), "/%s", lower(orig_name).c_str());
  if (unlink(tmpbuf) != 0)
    vlogf(LOG_FILE, "error in unlink (11) (%s) %d", tmpbuf, errno);
  
  if (vict->GetMaxLevel() > MAX_MORT) {
    sprintf(tmpbuf, "mv immortals/%s/ immortals/%s/",
            orig_name, cap(tmp_name));
    vsystem(tmpbuf);
  }

  vict->doSave(SILENT_NO);
  sprintf(tmpbuf, "The World shall now know %s as %s.",cap(orig_name),
       vict->getName());
  doSystem(tmpbuf);
  vict->fixClientPlayerLists(FALSE);

  // OK, now for some fun
  // what if they had personalized items, gotta change the damn names...
  wearSlotT slot;
  for (slot = MIN_WEAR; slot < MAX_WEAR; slot++) {
    renamePersonalizeFix(vict->equipment[slot], orig_name, tmp_name);
  }
  // and again for inventory
  renamePersonalizeFix(vict->stuff, orig_name, tmp_name);

  return;
}

void TBeing::doDescription()
{
  if (!desc)
    return ;

  if (fight())  {
    sendTo("You cannot perform that action while fighting!\n\r");
    return;
  }
  if (!isPc()) {
    sendTo("Ugly monsters like you can't change a description!\n\r");
    return;
  }
#if 0 
  if (desc->client) {
    // it winds up sending their desc as a bug report...

    sendTo("Sorry, descriptions can not be editted through the client.\n\r");
    return;
  }
#endif
  sendTo("Enter your description. Maximum length is 500 characters\n\r");
  sendTo("End the description with a '~'.  Use '`' to cancel.\n\r");
  if (getDescr()) {
    delete [] getDescr();
    setDescr(NULL);
  }
  desc->connected = CON_WRITING;
  desc->str = &descr;
  desc->max_str = 500;
#if 0
  if (desc->client)
    desc->clientf("%d|%d", CLIENT_STARTEDIT, 500);
#endif
}
