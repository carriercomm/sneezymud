//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: disc_brawling.cc,v $
// Revision 5.1.1.2  2000/10/23 05:56:14  jesus
// added spin skill
//
// Revision 5.1.1.1  1999/10/16 04:32:20  batopr
// new branch
//
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#include "stdsneezy.h"
#include "disc_brawling.h"

CDBrawling::CDBrawling() :
  CDiscipline(),
  skGrapple(),
  skStomp(),
  skBodyslam(),
  skSpin()
{
}

CDBrawling::CDBrawling(const CDBrawling &a) :
  CDiscipline(a),
  skGrapple(a.skGrapple),
  skStomp(a.skStomp),
  skBodyslam(a.skBodyslam),
  skSpin(a.skSpin)
{
}

CDBrawling & CDBrawling::operator=(const CDBrawling &a)
{
  if (this == &a) return *this;
  CDiscipline::operator=(a);
  skGrapple = a.skGrapple;
  skStomp = a.skStomp;
  skBodyslam = a.skBodyslam;
  skSpin = a.skSpin;
  return *this;
}

CDBrawling::~CDBrawling()
{
}
















