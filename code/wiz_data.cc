//////////////////////////////////////////////////////////////////////////
//
//    SneezyMUD - All rights reserved, SneezyMUD Coding Team
//    wiz_data.cc : load/save immortal information
//
//////////////////////////////////////////////////////////////////////////

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
}

#include "stdsneezy.h"

class wizSaveData {
  public:
    int setsev,
        office,
        blockastart,
        blockaend,
        blockbstart,
        blockbend;

    wizSaveData();
};

wizSaveData::wizSaveData() :
  setsev(0),
  office(0),
  blockastart(0),
  blockaend(0),
  blockbstart(0),
  blockbend(0)
{}

void TBeing::wizFileRead()
{
  FILE *fp;
  char buf[256], buf2[256];
  Descriptor *d = NULL;
  wizSaveData saveData;

  // don't use isImmortal, save always
  if (!(GetMaxLevel() > MAX_MORT) || !(d = desc))
    return;

  sprintf(buf, "immortals/%s/wizdata", getName());
  fp = fopen(buf, "r");
  if (!fp) {
    sprintf(buf2, "immortals/%s", getName());
    fp = fopen(buf2, "r");
    if (!fp) {	// no immort directory 
      if (mkdir(buf2, 0770)) {
	sendTo("Unable to create a wizard directory for you.\n\r");
	vlogf(LOG_FILE, "Unable to create a wizard directory for %s.", getName());
      } else
	sendTo("Wizard directory created...\n\r");
    } else
      fclose(fp);	// player has no wizard data, but has immort directory 

    return;
  }
  if (fread(&saveData, sizeof(saveData), 1, fp) != 1) {
    vlogf(LOG_BUG, "Corrupt wiz save file for %s", getName());
    fclose(fp);
    return;
  } 
  fclose(fp);

  d->severity    = saveData.setsev;
  d->office      = saveData.office;
  d->blockastart = saveData.blockastart;
  d->blockaend   = saveData.blockaend;
  d->blockbstart = saveData.blockbstart;
  d->blockbend   = saveData.blockbend;

  if (should_be_logged(this))
    vlogf(LOG_IIO, "Loaded %s's wizard file.", getName());

  TPerson * tPerson = dynamic_cast<TPerson *>(this);

  if (tPerson && !tPerson->tLogFile && should_be_logged(tPerson)) {
    char tString[256];

    sprintf(tString, "immortals/%s/logfile", name);

    if (!(tPerson->tLogFile = fopen(tString, "a")))
      vlogf(LOG_FILE, "Unable to open Log File for %s", name);
    else
      tPerson->logf("Logging in...");
  }
}

void TMonster::wizFileSave()
{
  return;
}

void TPerson::wizFileSave()
{
  FILE *fp;
  char buf[128];
  Descriptor *d = NULL;
  wizSaveData saveData;

  if (!(d = desc))
    return;
  if (d->connected != CON_PLYNG)  // semi arbitrary, but here for sanity
    return;

  sprintf(buf, "immortals/%s/wizdata", getName());
  unlink(buf);
  if (!(fp = fopen(buf, "wa+"))) {
    sprintf(buf, "immortals/%s", getName());
    if (!(fp = fopen(buf, "r"))) {	// no immort directory 
      if (mkdir(buf, 0770))
	sendTo("Unable to create a wizard directory for you.  Tell Brutius or Batopr.\n\r");
      else
	sendTo("Wizard directory created... type 'save' again.\n\r");
    } else
      fclose(fp);	// player has no wizard data, but has immort directory 

    return;
  }

  saveData.setsev      = d->severity;
  saveData.office      = d->office;
  saveData.blockastart = d->blockastart;
  saveData.blockaend   = d->blockaend;
  saveData.blockbstart = d->blockbstart;
  saveData.blockbend   = d->blockbend;

  fwrite(&saveData, sizeof(wizSaveData), 1, fp);   
  fclose(fp);
}
