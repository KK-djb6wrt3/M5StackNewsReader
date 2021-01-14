#include "efontTicker.h"

#include <efontEnableJaMini.h>
#include <efont.h>

efontTicker::efontTicker(TFT_eSPI& lcd, int width, int height) : m_Sprite(&lcd) {
  m_Sprite.createSprite(width, height);
  m_TotalWidth = 0;
  m_KeepCt = 66;
  m_KeepCtRemain = m_KeepCt;
  m_pText = NULL;
  m_pHead = NULL;
  m_X = 0;
}

efontTicker::~efontTicker(void) {
  m_Sprite.deleteSprite();
}

void efontTicker::setTextColor(uint32_t fgColor, uint32_t bgColor) {
  m_Sprite.setTextColor(fgColor, bgColor);
}

void efontTicker::setScale(int scale) {
  m_Sprite.setTextSize(scale);
}

void efontTicker::setKeeyCount(int count) {
  m_KeepCt = count;
  m_KeepCtRemain = m_KeepCt;
}

void efontTicker::setText(const char* pText) {
  m_pText = pText;
  m_pHead = m_pText;
  m_TotalWidth = 0;
  m_KeepCtRemain = m_KeepCt;
  m_X = 0;

  if (pText != NULL) {
    const char* pHead = pText;
    uint16_t chUTF16 = 0;
    while (pHead[0] != '\0') {
      pHead = efontUFT8toUTF16(&chUTF16, (char*)pHead);
      m_TotalWidth += getCharWidth(chUTF16);
    }
  }
}

void efontTicker::resetPos(void) {
  m_KeepCtRemain = m_KeepCt;
  m_pHead = m_pText;
  m_X = 0;
}

bool efontTicker::render(int x, int y) {
  bool isRestart = false;
  if (m_pHead != NULL) {
    m_Sprite.pushSprite(x, y);

    byte pFont[32];

    const int spriteW = m_Sprite.width();
    const uint8_t scale = m_Sprite.textsize;
    const uint32_t fgColor = m_Sprite.textcolor;
    const uint32_t bgColor = m_Sprite.textbgcolor;
    const char* pHead = m_pHead;
    const int chH = (16 * scale);
    uint16_t chUTF16 = 0;
    int drawX, drawY;
    int posX = -m_X;
    int headChW = -1;
    while (pHead[0] != '\0') {
      pHead = efontUFT8toUTF16(&chUTF16, (char*)pHead);
      getefontData(pFont, chUTF16);
      const int chW = (getCharWidth(chUTF16) * scale);
      if (headChW < 0) {
        headChW = chW;
      }
      m_Sprite.fillRect(posX, 0, (chW + 1), chH, bgColor);
      for (int y = 0; y < 16; y++) {
        const word& fontData = ((pFont[y * 2] * 256) + pFont[(y * 2) + 1]);
        for (int x = 0; x < 16; x++) {
          if ((0x8000 >> x) & fontData) {
            drawX = posX + (x * scale);
            drawY = (y * scale);
            if (drawX < 0) {
              continue;
            }
            else if (scale == 1) {
              m_Sprite.drawPixel(drawX, drawY, fgColor);
            }
            else {
              m_Sprite.fillRect(drawX, drawY, scale, scale, fgColor);
            }
          }
        }
      }
      posX += chW;
      if (posX >= spriteW) {
        break;
      }
    }

    if (m_KeepCtRemain > 0) {
      m_KeepCtRemain--;
    }
    else {
      m_X += scale;
      if (m_X >= headChW) {
        m_X = 0;
        m_pHead += getCharSize(m_pHead);
        if (m_pHead[0] == '\0') {
          isRestart = true;
          m_pHead = m_pText;
          m_KeepCtRemain = m_KeepCt;
        }
      }
    }
  }
  else {
    isRestart = true;
  }
  return isRestart;
}

int efontTicker::getCharWidth(const uint16_t& chUTF16) const {
  return ((chUTF16 < 0x0100) ? 8 : 16);
}

int efontTicker::getCharSize(const char* pText) const {
  int chSz = 0;
  if (pText != NULL) {
    const char ch = pText[0];
    if (ch < 0x80) {
      chSz = 1;
    }
    else if (ch < 0xE0)  {
      chSz = 2;
    }
    else if (ch < 0xF0) {
      chSz = 3;
    }
    else {
      chSz = 4;
    }
  }
  return chSz;
}
