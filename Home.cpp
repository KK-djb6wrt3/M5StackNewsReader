#include "Home.h"

#include <M5Stack.h>
#include <WiFi.h>

#include <vector>

#include "Icons.h"
#include "Utility.h"

#define COLOR565(r, g, b) (((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)
#define TFT_LIGHT_YELLOW COLOR565(250, 240, 139)

#define LOOP_MAX (3)
#define WHOLE_LOOP_MAX (5)

Home::Home(void)
  : IState("Home")
  , m_etTitle(M5.Lcd, 144, 16)
  , m_etDesc(M5.Lcd, 144, 32)
{
  m_NewsIdx = 0;
  m_LoopCt = 0;
  m_WholeLoopCt = 0;
  m_ImgIdx = 0;
  m_PrevBatLv = -1;
  m_isPrevCharge = false;
  m_Rect.x = 0;
  m_Rect.y = 0;
  m_Rect.width = 144;
  m_Rect.height = 50;
  m_pSSID = NULL;
  m_pPassword = NULL;
}

Home::~Home(void) {
}

void Home::onInitialized(void) {
  std::vector<const char*> accesspoints;
  int numAP = WiFi.scanNetworks();
  for (int i = 0; i < numAP; i++) {
    accesspoints.push_back(duplicateString(WiFi.SSID(i)));
  }

  if (m_Pref.load("/Preference.xml") == false) {
    M5.Lcd.println("Can't open Preference.xml");
    for (;;) {
      delay(portMAX_DELAY);
    }
  }
  bool isFound = false;
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

  if (m_pSSID == NULL) {
    M5.Lcd.print(F("Can't find any access points to connect."));
  }

  M5.Lcd.setTextColor(TFT_BLACK, TFT_LIGHT_YELLOW);
  loadImage();

  m_etTitle.setTextColor(TFT_BLACK, TFT_LIGHT_YELLOW);
  m_etDesc.setTextColor(TFT_BLACK, TFT_LIGHT_YELLOW);
  m_etDesc.setScale(2);

  getNews();
}

IState::StateID Home::onLoop(void) {
  m_etTitle.render(m_Rect.x, m_Rect.y);
  if (m_etDesc.render(m_Rect.x, (m_Rect.y + 20))) {
    if (++m_LoopCt >= LOOP_MAX) {
      m_LoopCt = 0;
      if (m_WholeLoopCt >= WHOLE_LOOP_MAX) {
        m_WholeLoopCt = 0;
        getNews();
      }
      loadImage();
      nextNews();
    }
  }

  drawBattery();

  return SID_Home;
}

IState::StateID Home::onPressBtnA(bool isLong) {
  m_LoopCt = 0;
  m_etTitle.resetPos();
  m_etDesc.resetPos();
  return SID_Home;
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
    if (m_NewsReader.getEntry(m_NewsIdx, &pTitle, &pDesc, NULL)) {
      m_etTitle.setText(pTitle);
      m_etDesc.setText(pDesc);
    }

    char pStr[6];
    Rect_t rcStat = {};
    getStatusArea(&rcStat);
    sprintf(pStr, "%u/%u", (m_NewsIdx + 1), num);
    M5.Lcd.drawString(pStr, rcStat.x, rcStat.y, 1);

    if (++m_NewsIdx >= num) {
      m_NewsIdx = 0;
      m_WholeLoopCt++;
    }
  }
}

void Home::drawBattery(void) {
  if (M5.Power.canControl()) {
    int8_t batLv = M5.Power.getBatteryLevel();
    bool isCharge = M5.Power.isCharging();
    if ((batLv != m_PrevBatLv) || (isCharge != m_isPrevCharge)) {
      m_PrevBatLv = batLv;
      m_isPrevCharge = isCharge;
      const uint16_t* pBatLv = NULL;
      switch (batLv) {
        case 100:
          pBatLv = (isCharge ? s_pChrgLv100 : s_pBatLv100);
          break;
        case 75:
          pBatLv = (isCharge ? s_pChrgLv75 : s_pBatLv75);
          break;
        case 50:
          pBatLv = (isCharge ? s_pChrgLv50 : s_pBatLv50);
          break;
        case 25:
          pBatLv = (isCharge ? s_pChrgLv25 : s_pBatLv25);
          break;
        default:
          pBatLv = NULL;
          break;
      }
      if (pBatLv != NULL) {
        Rect_t rcStat = {};
        getStatusArea(&rcStat);
        int x = (rcStat.x + rcStat.width - 16);
        int y = rcStat.y;
        M5.Lcd.fillRect(x, y, 16, 8, TFT_LIGHT_YELLOW);
        M5.Lcd.drawBitmap(x, y, 16, 8, pBatLv, 0xF81F);
      }
    }
  }
  else {
    Serial.println(F("can't ctrl"));
  }
}

void Home::loadImage(void) {
  int imgNum = m_Pref.getImageNum();
  if (imgNum > 0) {
    const char* pFile = NULL;
    Preference::BalloonPos ePos = Preference::BP_LeftTop;
    if (m_Pref.getImage(m_ImgIdx, &pFile, &ePos)) {
      M5.Lcd.drawJpgFile(SD, pFile);
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
      M5.Lcd.fillRoundRect(m_Rect.x, (m_Rect.y - 10), m_Rect.width, (m_Rect.height + 25), 10, TFT_LIGHT_YELLOW);

      m_PrevBatLv = -1;
      m_isPrevCharge = false;
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
