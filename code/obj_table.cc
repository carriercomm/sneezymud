//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: obj_table.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


// table.cc
//

#include "stdsneezy.h"

TTable::TTable() :
  TObj()
{
}

TTable::TTable(const TTable &a) :
  TObj(a)
{
}

TTable & TTable::operator=(const TTable &a)
{
  if (this == &a) return *this;
  TObj::operator=(a);
  return *this;
}

TTable::~TTable()
{
  TThing *t;

  for (; rider; ) {
  // assumption that tables are always in room (never on people)
  // for anyone who sees this, this was a bad assumption :)
    t = rider;
    positionTypeT new_pos = POSITION_DEAD;
    TBeing *tbt = dynamic_cast<TBeing *>(t);
    if (tbt)
      new_pos = tbt->getPosition();
    t->dismount(new_pos);
    if (parent) {
      if (t->parent)
        --*t;
      *parent += *t;
    }
    else if (roomp)
      *roomp += *t;
    else {
      vlogf(5,"Table Delete %s: Not in room not in parent", getName());
      delete t;
      t = NULL;
      continue;
    }
    // table it was on got toasted, item should take some damage too
    TObj *tobj = dynamic_cast<TObj *>(t);
    if (tobj && tobj->getStructPoints() >= 0) {
      tobj->addToStructPoints(-tobj->getStructPoints()/2);
      if (tobj->getStructPoints() <= 0) {
        delete tobj;
        tobj = NULL;
      }
    }
  }
}

void TTable::assignFourValues(int x1, int x2, int x3, int x4)
{
}

void TTable::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1 = 0;
  *x2 = 0;
  *x3 = 0;
  *x4 = 0;
}

void TTable::lowCheck()
{
  if (canWear(ITEM_TAKE)) {
    vlogf(LOW_ERROR, "Table (%s) set to be takeable.", getName());
  }

// we are explicitely NOT doing the TObj part of lowCheck since we
// don't want to pick up the applyLight warning for tables.
//  TObj::lowCheck();
}

string TTable::statObjInfo() const
{
  string a("");
  return a;
}

void TTable::writeAffects(int i, FILE *fp) const
{
  if (affected[i].location != APPLY_NONE) {
    fprintf(fp, "A\n%d %ld %ld\n", 
            mapApplyToFile(affected[i].location),
            applyTypeShouldBeSpellnum(affected[i].location) ? mapSpellnumToFile(spellNumT(affected[i].modifier)) : affected[i].modifier,
            affected[i].modifier2);
  }
}

void TTable::lookObj(TBeing *ch, int bits) const
{
  if (bits)
    ch->sendTo(fname(name).c_str());
  switch (bits) {
    case FIND_OBJ_INV:
      ch->sendTo(" (carried) : \n\r");
      break;
    case FIND_OBJ_ROOM:
      ch->sendTo(" (here) : \n\r");
      break;
    case FIND_OBJ_EQUIP:
      ch->sendTo(" (used) : \n\r");
      break;
  }
  list_thing_on_heap(rider, ch, 0);
}

void TTable::examineObj(TBeing *ch) const
{
  ch->sendTo("On top of it, you see:\n\r");
  lookObj(ch, 0);
}

bool TTable::canGetMeDeny(const TBeing *ch, silentTypeT silent) const
{
  if (!ch->isImmortal()) {
    if (!silent)
      ch->sendTo("Sorry, you aren't meant to be a furniture mover.\n\r");
    return true;
  }
  return false;
}

int TTable::getAllFrom(TBeing *ch, const char *argument)
{
  int rc;

  act("You start getting items off $p.", TRUE, ch, this, NULL, TO_CHAR);
  act("$n starts getting items off $p.", TRUE, ch, this, NULL, TO_ROOM);
  start_task(ch, ch->roomp->stuff, ch->roomp, TASK_GET_ALL, argument, 350, ch->in_room, 0,0,0);
  // this is a kludge, task_get still has a tiny delay on it
  // this dumps around it and goes right to the guts
  rc = (*(tasks[TASK_GET_ALL].taskf))
      (ch, CMD_TASK_CONTINUE, "", 0, ch->roomp, 0);
  if (IS_SET_DELETE(rc, DELETE_THIS)) {
    return DELETE_VICT;
  }
  return FALSE;
}

bool TTable::isSimilar(const TThing *t) const
{
  // table's can have things on them
  // things on them should make them dissimilar
  if (rider)
    return FALSE;

  const TTable *ttab = dynamic_cast<const TTable *>(t);
  if (ttab && ttab->rider) {
    // i have no objs, but other table does
    return FALSE;
  }
  return TObj::isSimilar(t);
}
