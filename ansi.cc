//
//      SneezyMUD - All rights reserved, SneezyMUD Coding Team
//      "ansi.cc" - Various color functions.
//
//      Last major revision : April 1997
//
//////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"

void TBeing::setColor(setColorFieldT num, setColorKolorT col)
{
  if (!desc)
    return;
    
  if (col == SET_COL_KOL_OFF) 
    sendTo("Unsetting color....\n\r");
  else {
    sendTo("Setting color....\n\r");
    desc->prompt_d.type |= PROMPT_COLOR;
  }

  sstring buf;
  switch (col) {
    case SET_COL_KOL_OFF:
      buf = "";
      break;
    case SET_COL_KOL_BLUE:
      buf = ANSI_BLUE;
      break;
    case SET_COL_KOL_RED:
      buf = ANSI_RED;
      break;
    case SET_COL_KOL_GREEN:
      buf = ANSI_GREEN;
      break;
    case SET_COL_KOL_WHITE:
      buf = VT_BOLDTEX;
      break;
    case SET_COL_KOL_PURPLE:
      buf = ANSI_PURPLE;
      break;
    case SET_COL_KOL_CYAN:
      buf = ANSI_CYAN;
      break;
    case SET_COL_KOL_ORANGE:
      buf = ANSI_ORANGE;
      break;
    case SET_COL_KOL_YELLOW:
      buf = ANSI_YELLOW_BOLD;
      break;
    case SET_COL_KOL_CHARCOAL:
      buf = ANSI_GRAY;
      break;
    case SET_COL_KOL_WH_ON_BL:
      buf = ANSI_WH_ON_BL;
      break;
    case SET_COL_KOL_INVERT:
      buf = VT_INVERTT;
      break;
    case SET_COL_KOL_WH_ON_CY:
      buf = ANSI_WH_ON_CY;
      break;
    case SET_COL_KOL_WH_ON_RD:
      buf = ANSI_WH_ON_RD;
      break;
    case SET_COL_KOL_WH_ON_PR:
      buf = ANSI_WH_ON_PR;
      break;
    case SET_COL_KOL_WH_ON_GR:
      buf = ANSI_WH_ON_GR;
      break;
    case SET_COL_KOL_WH_ON_OR:
      buf = ANSI_WH_ON_OR;
      break;
    case SET_COL_KOL_BLINK:
      sendTo("Color not found.\n\r");
      buf = "";
      break;
    case SET_COL_KOL_BOLDRED:
      buf = ANSI_RED_BOLD;
      break;
    case SET_COL_KOL_BOLDGREEN:
      buf = ANSI_GREEN_BOLD;
      break;
    case SET_COL_KOL_BOLDBLUE:
      buf = ANSI_BLUE_BOLD;
      break;
    case SET_COL_KOL_BOLDPURPLE:
      buf = ANSI_PURPLE_BOLD;
      break;
    case SET_COL_KOL_BOLDCYAN:
      buf = ANSI_CYAN_BOLD;
      break;
  }

  switch (num) {
    case SET_COL_FIELD_HIT:
      desc->prompt_d.hpColor = buf;
      break;
    case SET_COL_FIELD_MANA:
      desc->prompt_d.manaColor = buf;
      break;
    case SET_COL_FIELD_MOVE:
      desc->prompt_d.moveColor = buf;
      break;
    case SET_COL_FIELD_TALEN:
      desc->prompt_d.moneyColor = buf;
      break;
    case SET_COL_FIELD_XP:
      desc->prompt_d.expColor = buf;
      break;
    case SET_COL_FIELD_OPP:
      desc->prompt_d.oppColor = buf;
      break;
    case SET_COL_FIELD_ROOM:
      desc->prompt_d.roomColor = buf;
      break;
    case SET_COL_FIELD_TANK:
      desc->prompt_d.tankColor = buf;
      break;
    case SET_COL_FIELD_TANK_OTHER:
      desc->prompt_d.tankColor = buf;
      break;
    case SET_COL_FIELD_PIETY:
      desc->prompt_d.pietyColor = buf;
      break;
    case SET_COL_FIELD_LIFEFORCE:
      desc->prompt_d.lifeforceColor = buf;
      break;
  }
}

bool TBeing::hasColor() const
{
  return (color() || ansi());
}

bool TBeing::hasColorVt() const
{
  return (color() || ansi() || vt100());
}

const sstring TBeing::ansi_color_bold(const sstring &s) const
{
  sstring buf;

  if (hasColor()) {
    buf = bold();
    buf += s;
  } else if (hasColorVt()) {
    buf = "";
  } else {
    buf = "";
  }
  return buf;
}

const sstring TBeing::doColorSub() const
{
  sstring buf = "";

  if (hasColor()) {
    switch (desc->plr_colorSub) {
      case COLOR_SUB_NONE:
        buf = "";
        break;
      case COLOR_SUB_BLACK:
        buf = black();
        break;
      case COLOR_SUB_RED:
        buf = red();
        break;
      case COLOR_SUB_GREEN:
        buf = green();
        break;
      case COLOR_SUB_ORANGE:
        buf = orange();
        break;
      case COLOR_SUB_BLUE:
        buf = blue();
        break;
      case COLOR_SUB_PURPLE:
        buf = purple();
        break;
      case COLOR_SUB_CYAN:
        buf = cyan();
        break;
      case COLOR_SUB_WHITE:
        buf = white();
        break;
      case COLOR_SUB_YELLOW:
        buf = orangeBold();
        break;
      case COLOR_SUB_GRAY:
        buf = blackBold();
        break;
      case COLOR_SUB_BOLD_RED:
        buf = redBold();
        break;
      case COLOR_SUB_BOLD_GREEN:
        buf = greenBold();
        break;
      case COLOR_SUB_BOLD_BLUE:
        buf = blueBold();
        break;
      case COLOR_SUB_BOLD_PURPLE:
        buf = purpleBold();
        break;
      case COLOR_SUB_BOLD_CYAN:
        buf = cyanBold();
        break;
      case COLOR_SUB_BOLD_WHITE:
        buf = whiteBold();
        break;
      case COLOR_SUB_BOLD:
        buf = bold();
        break;
      default:
        buf = "";
        vlogf(LOG_BUG, fmt("Problem in color substituting (%s)") % getName());
        break;
    }
  } else if (hasColorVt()) {
    buf = "";
  } else {
    buf = "";
  }
  return buf;
}

const sstring TBeing::ansi_color_bold(const sstring &s, unsigned int ans_color) const
{
  sstring buf = "";
  int repFound = FALSE;

  if (desc) {
    if (IS_SET(desc->plr_colorOff, ans_color)) {
      repFound = TRUE;
    }
  }

  if (repFound) {
    if (desc->plr_colorSub) {
      buf = doColorSub();
    }
  } else {
    if (hasColor()) {
      buf = bold();
      buf += s;
    } else if (hasColorVt()) {
      if (ans_color == COLOR_BOLD) {
        buf = VT_BOLDTEX;
      }
    }
  }
  return buf;
}

const sstring TBeing::ansi_color(const sstring &s) const
{
  sstring buf = "";

  if (hasColor()) {
    buf = s;
  } else if (hasColorVt()) {
    buf = "";
  } else {
    buf = "";
  }
  return buf;
}

const sstring TBeing::ansi_color(const sstring &s, unsigned int ans_color) const
{
  sstring buf = "";
  int repFound = FALSE;

  if (desc) {
    if (IS_SET(desc->plr_colorOff, ans_color)) {
      repFound = TRUE;
    }
  }

  if (repFound) {
    if (desc->plr_colorSub) {
      buf = doColorSub();
    }
  } else {
    if (hasColor()) {
      buf = s;
    } else if (hasColorVt()) {
      if (ans_color == COLOR_BOLD) {
        buf = VT_BOLDTEX;
      } else if (ans_color == COLOR_NONE) {
        buf = ANSI_NORMAL;
      }
    }
  }
  return buf;
}

const sstring TBeing::highlight(sstring &s) const
{
  return (hasColorVt() ? s : "");
}

const sstring TBeing::whiteBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_WHITE, COLOR_BOLD_WHITE);
  return buf;
}

const sstring TBeing::white() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WHITE, COLOR_WHITE);
  return buf;
}

const sstring TBeing::blackBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_BLACK, COLOR_GRAY);
  return buf;
}

const sstring TBeing::black() const
{
  static sstring buf;
  buf = ansi_color(ANSI_BLACK, COLOR_BLACK);
  return buf;
}

const sstring TBeing::redBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_RED, COLOR_BOLD_RED);
  return buf;
}

const sstring TBeing::red() const
{
  static sstring buf;
  buf = ansi_color(ANSI_RED, COLOR_RED);
  return buf;
}

const sstring TBeing::underBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_UNDER, COLOR_UNDER);
  return buf;
}

const sstring TBeing::under() const
{
  static sstring buf;
  buf = ansi_color(ANSI_UNDER, COLOR_UNDER);
  return buf;
}

const sstring TBeing::bold() const
{
  static sstring buf;
  buf = ansi_color(VT_BOLDTEX, COLOR_BOLD);
  return buf;
}

const sstring TBeing::norm() const
{
  static sstring buf;
  buf = ansi_color(ANSI_NORMAL, COLOR_NONE);
  return buf;
}

const sstring TBeing::blueBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_BLUE, COLOR_BOLD_BLUE);
  return buf;
}

const sstring TBeing::blue() const
{
  static sstring buf;
  buf = ansi_color(ANSI_BLUE, COLOR_BLUE);
  return buf;
}

const sstring TBeing::cyanBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_CYAN, COLOR_BOLD_CYAN);
  return buf;
}

const sstring TBeing::cyan() const
{
  static sstring buf;
  buf = ansi_color(ANSI_CYAN, COLOR_CYAN);
  return buf;
}

const sstring TBeing::greenBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_GREEN, COLOR_BOLD_GREEN);
  return buf;
}

const sstring TBeing::green() const
{
  static sstring buf;
  buf = ansi_color(ANSI_GREEN, COLOR_GREEN);
  return buf;
}

const sstring TBeing::orangeBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_ORANGE, COLOR_YELLOW);
  return buf;
}

const sstring TBeing::orange() const
{
  static sstring buf;
  buf = ansi_color(ANSI_ORANGE, COLOR_ORANGE);
  return buf;
}

const sstring TBeing::purpleBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_PURPLE, COLOR_BOLD_PURPLE);
  return buf;
}

const sstring TBeing::purple() const
{
  static sstring buf;
  buf = ansi_color(ANSI_PURPLE, COLOR_PURPLE);
  return buf;
}

const sstring TBeing::invert() const
{
  static sstring buf;
  buf = ansi_color(VT_INVERTT);
  return buf;
}

const sstring TBeing::flash() const
{
  static sstring buf;
  buf = ansi_color(ANSI_FLASH);
  return buf;
}

const sstring TBeing::BlackOnBlack() const
{
  static sstring buf;
  buf = ansi_color(ANSI_BK_ON_BK);
  return buf;
}

const sstring TBeing::BlackOnWhite() const
{
  static sstring buf;
  buf = ansi_color(ANSI_BK_ON_WH);
  return buf;
}

const sstring TBeing::WhiteOnBlue() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WH_ON_BL);
  return buf;
}

const sstring TBeing::WhiteOnCyan() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WH_ON_CY);
  return buf;
}

const sstring TBeing::WhiteOnGreen() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WH_ON_GR);
  return buf;
}

const sstring TBeing::WhiteOnOrange() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WH_ON_OR);
  return buf;
}

const sstring TBeing::WhiteOnPurple() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WH_ON_PR);
  return buf;
}

const sstring TBeing::WhiteOnRed() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WH_ON_RD);
  return buf;
}

bool Descriptor::hasColor() const
{
  return ((plr_act & PLR_COLOR) || (plr_act & PLR_ANSI));
}

bool Descriptor::hasColorVt() const
{
  return ((plr_act & PLR_COLOR) || (plr_act & PLR_ANSI) || (plr_act & PLR_VT100));
}

const sstring Descriptor::doColorSub() const
{
  sstring buf = "";

  if (hasColor()) {
    switch (plr_colorSub) {
      case COLOR_SUB_NONE:
        buf = "";
        break;
      case COLOR_SUB_BLACK:
        buf = black();
        break;
      case COLOR_SUB_RED:
        buf = red();
        break;
      case COLOR_SUB_GREEN:
        buf = green();
        break;
      case COLOR_SUB_ORANGE:
        buf = orange();
        break;
      case COLOR_SUB_BLUE:
        buf = blue();
        break;
      case COLOR_SUB_PURPLE:
        buf = purple();
        break;
      case COLOR_SUB_CYAN:
        buf = cyan();
        break;
      case COLOR_SUB_WHITE:
        buf = white();
        break;
      case COLOR_SUB_YELLOW:
        buf = orangeBold();
        break;
      case COLOR_SUB_GRAY:
        buf = blackBold();
        break;
      case COLOR_SUB_BOLD_RED:
        buf = redBold();
        break;
      case COLOR_SUB_BOLD_GREEN:
        buf = greenBold();
        break;
      case COLOR_SUB_BOLD_BLUE:
        buf = blueBold();
        break;
      case COLOR_SUB_BOLD_PURPLE:
        buf = purpleBold();
        break;
      case COLOR_SUB_BOLD_CYAN:
        buf = cyanBold();
        break;
      case COLOR_SUB_BOLD_WHITE:
        buf = whiteBold();
        break;
      case COLOR_SUB_BOLD:
        buf = bold();
        break;
      default:
        buf = "";
        vlogf(LOG_BUG, "Problem in color substituting/desc");
        break;
    }
  } else if (hasColorVt()) {
    buf = "";
  } else {
    buf = "";
  }
  return buf;
}

const sstring Descriptor::ansi_color_bold(const sstring &s) const
{
  sstring buf;

  if (hasColor()) {
    buf = bold();
    buf += s;
  } else if (hasColorVt()) {
    buf = "";
  } else {
    buf = "";
  }
  return buf;
}

const sstring Descriptor::ansi_color_bold(const sstring &s, unsigned int color) const
{
  sstring buf = "";
  int repFound = FALSE;

  if (IS_SET(plr_colorOff, color)) {
    repFound = TRUE;
  }

  if (repFound) {
    if (plr_colorSub) {
      buf = doColorSub();
    }
  } else {
    if (hasColor()) {
      buf = bold();
      buf += s;
    } else if (hasColorVt()) {
      if (color == COLOR_BOLD) {
        buf = VT_BOLDTEX;
      }
    }
  }
  return buf;
}

const sstring Descriptor::ansi_color(const sstring &s) const
{
  sstring buf;

  if (hasColor()) {
    buf = s;
  } else if (hasColorVt()) {
    buf = "";
  } else {
    buf = "";
  }
  return buf;
}

const sstring Descriptor::ansi_color(const sstring &s, unsigned int color) const
{
  sstring buf = "";
  int repFound = FALSE;

  if (IS_SET(plr_colorOff, color)) {
    repFound = TRUE;
  }

  if (repFound) {
    if (plr_colorSub) {
      buf = doColorSub();
    }
  } else {
    if (hasColor()) {
      buf = s;
    } else if (hasColorVt()) {
      if (color == COLOR_BOLD) {
        buf = VT_BOLDTEX;
      } else if (color == COLOR_NONE) {
        buf = ANSI_NORMAL;
      }
    }
  }
  return buf;
}


const sstring Descriptor::highlight(sstring &s) const
{
  return (hasColorVt() ? s : "");
}

const sstring Descriptor::whiteBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_WHITE, COLOR_BOLD_WHITE);
  return buf;
}

const sstring Descriptor::white() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WHITE, COLOR_WHITE);
  return buf;
}

const sstring Descriptor::blackBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_BLACK, COLOR_GRAY);
  return buf;
}

const sstring Descriptor::black() const
{
  static sstring buf;
  buf = ansi_color(ANSI_BLACK, COLOR_BLACK);
  return buf;
}

const sstring Descriptor::redBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_RED, COLOR_BOLD_RED);
  return buf;
}

const sstring Descriptor::red() const
{
  static sstring buf;
  buf = ansi_color(ANSI_RED, COLOR_RED);
  return buf;
}

const sstring Descriptor::underBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_UNDER);
  return buf;
}

const sstring Descriptor::under() const
{
  static sstring buf;
  buf = ansi_color(ANSI_UNDER);
  return buf;
}

const sstring Descriptor::bold() const
{
  static sstring buf;
  buf = ansi_color(VT_BOLDTEX, COLOR_BOLD);
  return buf;
}

const sstring Descriptor::norm() const
{
  static sstring buf;
  buf = ansi_color(ANSI_NORMAL, COLOR_NONE);
  return buf;
}

const sstring Descriptor::blueBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_BLUE, COLOR_BOLD_BLUE);
  return buf;
}

const sstring Descriptor::blue() const
{
  static sstring buf;
  buf = ansi_color(ANSI_BLUE, COLOR_BLUE);
  return buf;
}

const sstring Descriptor::cyanBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_CYAN, COLOR_BOLD_CYAN);
  return buf;
}

const sstring Descriptor::cyan() const
{
  static sstring buf;
  buf = ansi_color(ANSI_CYAN, COLOR_CYAN);
  return buf;
}

const sstring Descriptor::greenBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_GREEN, COLOR_BOLD_GREEN);
  return buf;
}

const sstring Descriptor::green() const
{
  static sstring buf;
  buf = ansi_color(ANSI_GREEN, COLOR_GREEN);
  return buf;
}

const sstring Descriptor::orangeBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_ORANGE, COLOR_YELLOW);
  return buf;
}

const sstring Descriptor::orange() const
{
  static sstring buf;
  buf = ansi_color(ANSI_ORANGE, COLOR_ORANGE);
  return buf;
}

const sstring Descriptor::purpleBold() const
{
  static sstring buf;
  buf = ansi_color_bold(ANSI_PURPLE, COLOR_BOLD_PURPLE);
  return buf;
}

const sstring Descriptor::purple() const
{
  static sstring buf;
  buf = ansi_color(ANSI_PURPLE, COLOR_PURPLE);
  return buf;
}

const sstring Descriptor::invert() const
{
  static sstring buf;
  buf = ansi_color(VT_INVERTT);
  return buf;
}

const sstring Descriptor::flash() const
{
  static sstring buf;
  buf = ansi_color(ANSI_FLASH);
  return buf;
}

const sstring Descriptor::BlackOnBlack() const
{
  static sstring buf;
  buf = ansi_color(ANSI_BK_ON_BK);
  return buf;
}

const sstring Descriptor::BlackOnWhite() const
{
  static sstring buf;
  buf = ansi_color(ANSI_BK_ON_WH);
  return buf;
}

const sstring Descriptor::WhiteOnBlue() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WH_ON_BL);
  return buf;
}

const sstring Descriptor::WhiteOnCyan() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WH_ON_CY);
  return buf;
}

const sstring Descriptor::WhiteOnGreen() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WH_ON_GR);
  return buf;
}

const sstring Descriptor::WhiteOnOrange() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WH_ON_OR);
  return buf;
}

const sstring Descriptor::WhiteOnPurple() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WH_ON_PR);
  return buf;
}

const sstring Descriptor::WhiteOnRed() const
{
  static sstring buf;
  buf = ansi_color(ANSI_WH_ON_RD);
  return buf;
}

bool TBeing::color() const
{
  return isPlayerAction(PLR_COLOR);
}

bool TBeing::ansi() const
{
  return isPlayerAction(PLR_ANSI);
}

bool TBeing::vt100() const
{
  return isPlayerAction(PLR_VT100);
}

void TBeing::cls() const
{
  if ((ansi() || vt100()))
    sendTo(VT_HOMECLR);
}

void TBeing::fullscreen() const
{
  if ((ansi() || vt100()))
    sendTo(fmt(VT_MARGSET) % 1 % getScreen());
}

int TBeing::getScreen() const
{
   if (!desc)
     return 0;

   return (desc->screen_size ? desc->screen_size : 24);
}

