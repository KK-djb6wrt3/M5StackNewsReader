#ifndef _EFONT_TICKER_H_
#define _EFONT_TICKER_H_

#include <M5Stack.h>

void drawEfont(int x, int y, const char* pStr);

class efontTicker {
  public:
    efontTicker(TFT_eSPI& lcd, int width, int height);
    virtual ~efontTicker(void);

    void setTextColor(uint32_t fgColor, uint32_t bgColor);
    void setScale(int scale);
    void setKeeyCount(int count);
    void setText(const char* pText);
    void resetPos(void);
    bool render(int x, int y);

  private:
    TFT_eSprite m_Sprite;
    int m_TotalWidth;
    int m_KeepCt;
    int m_KeepCtRemain;
    const char* m_pText;
    const char* m_pHead;
    int m_X;

  private:
    int getCharWidth(const uint16_t& chUTF16) const;
    int getCharSize(const char* pText) const;
};

#endif //_EFONT_TICKER_H_
