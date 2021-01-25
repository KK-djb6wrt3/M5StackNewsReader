#include "efontTicker.h"

#include <M5Stack.h>

#include <efontEnableJaMini.h>
#include <efont.h>

void drawEfont(int x, int y, const char* pStr) {
  int posX = M5.Lcd.getCursorX();
  int posY = M5.Lcd.getCursorY();
  uint8_t textsize = M5.Lcd.textsize;
  uint32_t textcolor = M5.Lcd.textcolor;
  uint32_t textbgcolor = M5.Lcd.textbgcolor;

  byte font[32];

  while ( *pStr != 0x00 ) {
    // 改行処理
    if ( *pStr == '\n' ) {
      // 改行
      posY += 16 * textsize;
      posX = M5.Lcd.getCursorX();
      pStr++;
      continue;
    }

    // フォント取得
    uint16_t strUTF16;
    pStr = efontUFT8toUTF16( &strUTF16, (char*)pStr );
    getefontData( font, strUTF16 );

    // 文字横幅
    int width = 16 * textsize;
    if ( strUTF16 < 0x0100 ) {
      // 半角
      width = 8 * textsize;
    }

    // 背景塗りつぶし
    M5.Lcd.fillRect(posX, posY, width, 16 * textsize, textbgcolor);

    // 取得フォントの確認
    for (uint8_t row = 0; row < 16; row++) {
      word fontdata = font[row * 2] * 256 + font[row * 2 + 1];
      for (uint8_t col = 0; col < 16; col++) {
        if ( (0x8000 >> col) & fontdata ) {
          int drawX = posX + col * textsize;
          int drawY = posY + row * textsize;
          if ( textsize == 1 ) {
            M5.Lcd.drawPixel(drawX, drawY, textcolor);
          } else {
            M5.Lcd.fillRect(drawX, drawY, textsize, textsize, textcolor);
          }
        }
      }
    }

    // 描画カーソルを進める
    posX += width;

    // 折返し処理
    if ( M5.Lcd.width() <= posX ) {
      posX = 0;
      posY += 16 * textsize;
    }
  }

  // カーソルを更新
  M5.Lcd.setCursor(posX, posY);
}

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
