#include <M5Stack.h>

#include "StateManager.h"
#include "efontTicker.h"

static StateManager s_StateMgr;
static efontTicker* s_ppTickers[TickerBase::TID_MAX];

static void fillRect(FillRect* pCmd) {
  uint16_t x = 0U, y = 0U, width = 0U, height = 0U, color = 0U;
  pCmd->getParam(&x, &y, &width, &height, &color);
  M5.Lcd.fillRect(x, y, width, height, color);

}
static void fillRoundRect(FillRoundRect*pCmd) {
  uint16_t x = 0U, y = 0U, width = 0U, height = 0U, corner = 0U, color = 0U;
  pCmd->getParam(&x, &y, &width, &height, &corner, &color);
  M5.Lcd.fillRoundRect(x, y, width, height, corner, color);
}

static void drawStr(DrawStr* pCmd) {
  void* pPtr = NULL;
  uint16_t x = 0U, y = 0U, fontSz = 0U;
  bool isProgMem = pCmd->getParam(&pPtr, &x, &y, &fontSz);
  M5.Lcd.setCursor(x, y);
  M5.Lcd.setTextSize(fontSz);
  if (isProgMem) {
    M5.Lcd.print((__FlashStringHelper*)pPtr);
    Serial.print("FSH:");
    Serial.println((__FlashStringHelper*)pPtr);
  }
  else {
    M5.Lcd.print((char*)pPtr);
    Serial.print("char:");
    Serial.println((char*)pPtr);
  }
}

static void setTextColor(SetTextColor* pCmd) {
  uint16_t fore = 0U, back = 0U;
  pCmd->getParam(&fore, &back);
  M5.Lcd.setTextColor(fore, back);
}

static void drawBmp(DrawBmp* pCmd) {
  IconID eID = ICN_UNKNOWN;
  uint16_t x = 0U, y = 0U, transparent = 0U;
  pCmd->getParam(&eID, &x, &y, &transparent);
  uint16_t width = 0U, height = 0U;
  const uint16_t* pIcon = getIcon(eID, &width, &height);
  if (pIcon != NULL) {
    M5.Lcd.drawBitmap(x, y, width, height, pIcon, transparent);
  }
}
static void drawJpgFile(DrawJpgFile* pCmd) {
  DrawJpgFile::STORAGE eStrg;
  const char* pPath = NULL;
  uint16_t x = 0U, y = 0U;
  pCmd->getParam(&eStrg, &pPath, &x, &y);
  switch (eStrg) {
    case DrawJpgFile::SD:
      M5.Lcd.drawJpgFile(SD, pPath, x, y);
      break;
    default:
      break;
  }
}

static void tickerCreate(TickerCreate* pCmd) {
  uint16_t width = 0U, height = 0U;
  TickerBase::TickerID eID = pCmd->getParam(&width, &height);
  efontTicker*& pTkr = s_ppTickers[eID];
  if (pTkr == NULL) {
    pTkr = new efontTicker(M5.Lcd, width, height);
  }
}

static void tickerDelete(TickerDelete* pCmd) {
  TickerBase::TickerID eID = pCmd->getTkrID();
  efontTicker*& pTkr = s_ppTickers[eID];
  if (pTkr != NULL) {
    delete pTkr;
    pTkr = NULL;
  }
}

static void tickerSetText(TickerSetText* pCmd) {
  const char* pStr = NULL;
  TickerBase::TickerID eID = pCmd->getParam(&pStr);
  efontTicker*& pTkr = s_ppTickers[eID];
  if (pTkr != NULL) {
    pTkr->setText(pStr);
  }
}

static void tickerSetParam(TickerSetParam* pCmd) {
  uint8_t scale = 0U;
  uint16_t fore = 0U, back = 0U;
  TickerBase::TickerID eID = pCmd->getParam(&scale, &fore, &back);
  efontTicker*& pTkr = s_ppTickers[eID];
  if (pTkr != NULL) {
    pTkr->setTextColor(fore, back);
    pTkr->setScale(scale);
  }
}

static void tickerRender(TickerRender* pCmd) {
  uint16_t x = 0U, y = 0U;
  TickerBase::TickerID eID = pCmd->getParam(&x, &y);
  efontTicker*& pTkr = s_ppTickers[eID];
  if (pTkr != NULL) {
    bool isWrap = pTkr->render(x, y);
    pCmd->callback(isWrap);
  }
}

static void tickerResetPos(TickerResetPos* pCmd) {
  TickerBase::TickerID eID = pCmd->getTkrID();
  efontTicker*& pTkr = s_ppTickers[eID];
  if (pTkr != NULL) {
    pTkr->resetPos();
  }
}

static void drawQrCode(DrawQrCode* pCmd) {
  const char* pStr = NULL;
  pCmd->getParam(&pStr);
  if (pStr != NULL) {
    M5.Lcd.qrcode(pStr);
  }
}

static void checkBattery(void) {
  static bool sIsCharging = false;
  static int8_t sBatteryLv = -1;
  if (M5.Power.canControl()) {
    bool isCharging = M5.Power.isCharging();
    int8_t batteryLv = M5.Power.getBatteryLevel();
    if ((sIsCharging != isCharging) || (sBatteryLv != batteryLv)) {
      s_StateMgr.onBatteryChanged(isCharging, batteryLv);
    }
    sIsCharging = isCharging;
    batteryLv = sBatteryLv;
  }
}

void setup() {
  M5.begin();
  M5.Power.begin();
  M5.Lcd.clear();
  M5.Lcd.println(F("Now loading..."));
  
  s_StateMgr.begin();
}

void loop() {
  s_StateMgr.onLoop();

  if (M5.BtnA.wasReleasefor(500)) {
    s_StateMgr.onPressBtnA(true);
  }
  else if (M5.BtnA.wasReleased()) {
    s_StateMgr.onPressBtnA(false);
  }

  if (M5.BtnB.wasReleasefor(500)) {
    s_StateMgr.onPressBtnB(true);
  }
  else if (M5.BtnB.wasReleased()) {
    s_StateMgr.onPressBtnB(false);
  }

  if (M5.BtnC.wasReleasefor(500)) {
    s_StateMgr.onPressBtnC(true);
  }
  else if (M5.BtnC.wasReleased()) {
    s_StateMgr.onPressBtnC(false);
  }

  checkBattery();

  ICommand* pCmd = NULL;
  while (s_StateMgr.popDrawCommand(&pCmd)) {
    ICommand::CmdID eCmdID = pCmd->getCmdID();
    switch (eCmdID) {
      case ICommand::CmdID_PowerOff:
        M5.Power.powerOFF();
        break;
      case ICommand::CmdID_Clear:
        M5.Lcd.clear();
        break;
      case ICommand::CmdID_FillRect:
        fillRect((FillRect*)pCmd);
        break;
      case ICommand::CmdID_FillRoundRect:
        fillRoundRect((FillRoundRect*)pCmd);
        break;
      case ICommand::CmdID_DrawStr:
        drawStr((DrawStr*)pCmd);
        break;
      case ICommand::CmdID_SetTextColor:
        setTextColor((SetTextColor*)pCmd);
        break;
      case ICommand::CmdID_DrawBmp:
        drawBmp((DrawBmp*)pCmd);
        break;
      case ICommand::CmdID_DrawJpgFile:
        drawJpgFile((DrawJpgFile*)pCmd);
        break;
      case ICommand::CmdID_TickerCreate:
        tickerCreate((TickerCreate*)pCmd);
        break;
      case ICommand::CmdID_TickerDelete:
        tickerDelete((TickerDelete*)pCmd);
        break;
      case ICommand::CmdID_TickerSetText:
        tickerSetText((TickerSetText*)pCmd);
        break;
      case ICommand::CmdID_TickerSetParam:
        tickerSetParam((TickerSetParam*)pCmd);
        break;
      case ICommand::CmdID_TickerRender:
        tickerRender((TickerRender*)pCmd);
        break;
      case ICommand::CmdID_TickerResetPos:
        tickerResetPos((TickerResetPos*)pCmd);
        break;
      case ICommand::CmdID_DrawQrCode:
        drawQrCode((DrawQrCode*)pCmd);
        break;
      default:
        break;
    }
    delete pCmd;
  }

  M5.update();
  delay(33);
}
