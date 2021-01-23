#include "Home.h"

#include <WiFi.h>

#include <vector>

#include "Resource.h"
#include "Utility.h"
#include "QRCode.h"

#define COLOR565(r, g, b) (((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)
#define TFT_LIGHT_YELLOW COLOR565(250, 240, 139)
#define TFT_BLACK COLOR565(0, 0, 0)

#define LOOP_MAX (3)
#define WHOLE_LOOP_MAX (5)

Home::Home(void)
  : IState("Home") {
  clean();
}

Home::~Home(void) {
}

void Home::onInitialized(void) {
  bool isGetNews = false;
  if (m_Pref.isLoaded() == false) {
    isGetNews = true;
    if (m_Pref.load("/Preference.xml") == false) {
      pushDrawCmd(new DrawStr(0, 0, F("Can't open Preference.xml")));
      for (;;) {
        delay(portMAX_DELAY);
      }
    }
    if (findAccessPoint() == false) {
      pushDrawCmd(new DrawStr(0, 0, F("Can't find any access points to connect.")));
      for (;;) {
        delay(portMAX_DELAY);
      }
    }
  }

  pushDrawCmd(new TickerCreate(TickerBase::TID_0, 144, 16));
  pushDrawCmd(new TickerSetParam(TickerBase::TID_0, 1, TFT_BLACK, TFT_LIGHT_YELLOW));

  pushDrawCmd(new TickerCreate(TickerBase::TID_1, 144, 32));
  pushDrawCmd(new TickerSetParam(TickerBase::TID_1, 2, TFT_BLACK, TFT_LIGHT_YELLOW));

  pushDrawCmd(new SetTextColor(TFT_BLACK, TFT_LIGHT_YELLOW));
  loadImage();

  if (isGetNews) {
    getNews();
  }
}
void Home::onTerminated(void) {
  pushDrawCmd(new TickerDelete(TickerBase::TID_0));
  pushDrawCmd(new TickerDelete(TickerBase::TID_1));
}

void Home::clean(void) {
  m_NewsIdx = 0;
  m_LoopCt = 0;
  m_WholeLoopCt = 0;
  m_ImgIdx = 0;
  m_BatteryLv = -1;
  m_isCharging = false;
  m_Rect.x = 0;
  m_Rect.y = 0;
  m_Rect.width = 144;
  m_Rect.height = 50;
  m_pSSID = NULL;
  m_pPassword = NULL;
}

void Home::forward(void) {
  m_LoopCt = 0;
  if (m_WholeLoopCt >= WHOLE_LOOP_MAX) {
    m_WholeLoopCt = 0;
    getNews();
  }
  loadImage();
  nextNews();
}

void Home::_renderCBR(bool isWrap, void* pUsrParam) {
  Home* pHome = (Home*)pUsrParam;
  if (pHome != NULL) {
    pHome->renderCBR(isWrap);
  }
}

void Home::renderCBR(bool isWrap) {
  if (isWrap) {
    if (++m_LoopCt >= LOOP_MAX) {
      forward();
    }
  }
}

IState::StateID Home::onLoop(void) {
  pushDrawCmd(new TickerRender(TickerBase::TID_0, m_Rect.x, m_Rect.y));
  pushDrawCmd(new TickerRender(TickerBase::TID_1, m_Rect.x, (m_Rect.y + 20), _renderCBR, (void*)this));

  drawBattery();
  return SID_Current;
}

IState::StateID Home::onPressBtnA(bool isLong) {
  m_LoopCt = 0;
  pushDrawCmd(new TickerResetPos(TickerBase::TID_0));
  pushDrawCmd(new TickerResetPos(TickerBase::TID_1));
  return isLong ? SID_Sleep : SID_Current;
}

IState::StateID Home::onPressBtnC(bool isLong) {
  if (isLong == false) {
    forward();
    return SID_Current;
  }
  else {
    return SID_QRCode;
  }
}
IState::StateID Home::onBatteryChanged(bool isCharging, int8_t level) {
  if ((level != m_BatteryLv) || (isCharging != m_isCharging)) {
    m_BatteryLv = level;
    m_isCharging = isCharging;
    drawBattery();
  }
  return SID_Current;
}

bool Home::findAccessPoint(void) {
  bool isFound = false;
  std::vector<const char*> accesspoints;
  int numAP = WiFi.scanNetworks();
  for (int i = 0; i < numAP; i++) {
    accesspoints.push_back(duplicateString(WiFi.SSID(i)));
  }
  int num = m_Pref.getApNum();
  const char* pSSID = NULL;
  const char* pPassword = NULL;
  for (int i = 0; i < numAP; i++) {
    const char* pAP = accesspoints[i];
    if (isFound == false) {
      for (int j = 0; j < num; j++) {
        if (m_Pref.getAP(j, &pSSID, &pPassword)) {
          if (strcmp(pAP, pSSID) == 0) {
            m_pSSID = pSSID;
            m_pPassword = pPassword;
            isFound = true;
          }
        }
      }
    }
    SAFE_DELETE(pAP);
  }
  accesspoints.clear();
  return isFound;
}

void Home::getStatusArea(Rect_t* pRect) {
  if (pRect != NULL) {
    pRect->x = (m_Rect.x + 10);
    pRect->y = (m_Rect.y + m_Rect.height + 5);
    pRect->width = (m_Rect.width - 20);
    pRect->height = 8;
  }
}

void Home::nextNews(void) {
  int num = m_NewsReader.getEntryNum();
  if (num > 0) {
    const char* pTitle = NULL;
    const char* pDesc = NULL;
    const char* pLink = NULL;
    if (m_NewsReader.getEntry(m_NewsIdx, &pTitle, &pDesc, &pLink)) {
      pushDrawCmd(new TickerSetText(TickerBase::TID_0, pTitle));
      pushDrawCmd(new TickerSetText(TickerBase::TID_1, pDesc));
    }
    QRCode::setLink(pLink);

    static char spStr[6];
    Rect_t rcStat = {};
    getStatusArea(&rcStat);
    sprintf(spStr, "%u/%u", (m_NewsIdx + 1), num);
    pushDrawCmd(new DrawStr(rcStat.x, rcStat.y, spStr));

    if (++m_NewsIdx >= num) {
      m_NewsIdx = 0;
      m_WholeLoopCt++;
    }
  }
}

void Home::drawBattery(void) {
  IconID eID = ICN_UNKNOWN;
  switch (m_BatteryLv) {
    case 100:
      eID = (m_isCharging ? ICN_CHRG_FULL : ICN_BATT_FULL);
      break;
    case 75:
      eID = (m_isCharging ? ICN_CHRG_75 : ICN_BATT_75);
      break;
    case 50:
      eID = (m_isCharging ? ICN_CHRG_50 : ICN_BATT_50);
      break;
    case 25:
      eID = (m_isCharging ? ICN_CHRG_25 : ICN_BATT_25);
      break;
    default:
      break;
  }
  if (eID != ICN_UNKNOWN) {
    Rect_t rcStat = {};
    getStatusArea(&rcStat);
    const int x = (rcStat.x + rcStat.width - 16);
    const int y = rcStat.y;
    const int w = 16;
    const int h = 8;

    pushDrawCmd(new FillRect(x, y, w, h, TFT_LIGHT_YELLOW));
    pushDrawCmd(new DrawBmp(x, y, eID, 0xF81F));
  }
}

void Home::loadImage(void) {
  int imgNum = m_Pref.getImageNum();
  if (imgNum > 0) {
    const char* pFile = NULL;
    Preference::BalloonPos ePos = Preference::BP_LeftTop;
    if (m_Pref.getImage(m_ImgIdx, &pFile, &ePos)) {
      pushDrawCmd(new DrawJpgFile(DrawJpgFile::SD, pFile));
      switch (ePos) {
        case Preference::BP_LeftBottom:
          m_Rect.x = 10;
          m_Rect.y = 160;
          break;
        case Preference::BP_RightTop:
          m_Rect.x = 166;
          m_Rect.y = 20;
          break;
        case Preference::BP_RightBottom:
          m_Rect.x = 166;
          m_Rect.y = 160;
          break;
        default:
          m_Rect.x = 10;
          m_Rect.y = 20;
          break;
      }
      pushDrawCmd(new FillRoundRect(m_Rect.x, (m_Rect.y - 10), m_Rect.width, (m_Rect.height + 25), 10, TFT_LIGHT_YELLOW));
      drawBattery();
    }
    if (++m_ImgIdx >= imgNum) {
      m_ImgIdx = 0;
    }
  }
}

bool Home::getNews(void) {
  bool result = false;
  if (m_pSSID != NULL && m_pPassword != NULL) {
    WiFi.begin(m_pSSID, m_pPassword);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    m_NewsReader.read("/rss/topics/top-picks.xml");
    WiFi.disconnect(true);
    m_NewsIdx = 0;
    nextNews();
    result = true;
  }
  return result;
}
