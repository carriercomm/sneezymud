//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: obj_seethru.cc,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


// see_thru.cc
//

#include "stdsneezy.h"

TSeeThru::TSeeThru() :
  TObj(),
  target_room(0)
{
}

TSeeThru::TSeeThru(const TSeeThru &a) :
  TObj(a),
  target_room(a.target_room)
{
}

TSeeThru & TSeeThru::operator=(const TSeeThru &a)
{
  if (this == &a) return *this;
  TObj::operator=(a);
  target_room = a.target_room;
  return *this;
}

TSeeThru::~TSeeThru()
{
  if (roomp && givesOutsideLight())
    roomp->decrementWindow();
}

void TSeeThru::assignFourValues(int x1, int, int, int)
{
  if (x1 < 0 && dynamic_cast<TWindow *>(this))
    x1 = (-(x1) | (1 << 23));

  setTarget(GET_BITS(x1, 23, 24));
}

void TSeeThru::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  int r = 0;
  SET_BITS(r, 23, 24, target_room);
  *x1 = r;

  *x2 = 0;
  *x3 = 0;
  *x4 = 0;
}

int TSeeThru::getTarget(int *isRandom = NULL) const
{
  int tValue = target_room;

  if (tValue & (1 << 23)) {
    tValue &= ~(1 << 23);
    tValue = -(tValue);
  }

  if (tValue < 0) {
    int         tExitObject = -(tValue),
                tExitRoom   = ROOM_NOWHERE,
                tMatches    = 0;
    const TObj *tObj;
    TRoom      *tRoom;

    for (tObj = object_list; tObj; tObj = tObj->next)
      if (tObj->objVnum() == tExitObject && tObj != this &&
          (tRoom = real_roomp(tObj->in_room)) &&
          !tRoom->isRoomFlag(ROOM_NO_PORTAL)) {
        tMatches++;
        tExitRoom = tObj->in_room;
      }

    if (tMatches <= 1) {
      if (isRandom)
        *isRandom = tExitRoom;

      return tExitRoom;
    }

    if (isRandom)
      *isRandom = -1;

    tExitRoom = ::number(1, tMatches);

    for (tObj = object_list; tObj; tObj = tObj->next)
      if (tObj->objVnum() == tExitObject &&
          (--tExitRoom <= 0) &&
          tObj != this && (tRoom = real_roomp(tObj->in_room)) &&
          !tRoom->isRoomFlag(ROOM_NO_PORTAL))
        return tObj->in_room;

    return tExitRoom;
  }

  if (isRandom)
    *isRandom = tValue;

  return tValue;
}

void TSeeThru::setTarget(int t)
{
  if (t < 0)
    target_room = (-(t) | (1 << 23));
  else
    target_room = t;
}

bool TSeeThru::givesOutsideLight() const
{
  TRoom *rp;

  return (roomp && roomp->isRoomFlag(ROOM_INDOORS) &&
          (rp = real_roomp(getTarget())) &&
          !(rp->isRoomFlag(ROOM_INDOORS)));
}

int TSeeThru::getLightFromOutside() const
{
  TRoom *rp;
  int val;

  if (!givesOutsideLight() || (target_room & (1 << 23)))
    return 0;

  rp = real_roomp(getTarget());

  if (!rp) {
    vlogf(7,"Bad room value on %s for light determination.",getName());
    return 0;
  }
  val = rp->outdoorLightWindow();

  return val;
}

void TSeeThru::purgeMe(TBeing *)
{
  // intentionally blank so they don't get purged
}

int TSeeThru::riverFlow(int)
{
  // don't have these objects float around
  return FALSE;
}
