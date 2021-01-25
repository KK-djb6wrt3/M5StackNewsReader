#ifndef _ISTATE_H_
#define _ISTATE_H_

#include <stddef.h>

#include "Command.h"

class IState {
  public:
    enum StateID {
      SID_Current,
      SID_Home,
      SID_QRCode,
      SID_Menu,
      SID_Sleep,
    };

  public:
    virtual ~IState(void);

    virtual void onInitialized(void);
    virtual void onTerminated(void);
    virtual StateID onLoop(void) = 0;
    virtual StateID onPressBtnA(bool isLong);
    virtual StateID onPressBtnB(bool isLongoid);
    virtual StateID onPressBtnC(bool isLong);
    virtual StateID onBatteryChanged(bool isCharging, int8_t level);

    bool popDrawCmd(ICommand** ppCmd);
    const char* getName(void) const;
    void setLcdSize(int width, int height);
    void getLcdSize(int* pWidth, int* pHeight) const;

  protected:
    IState(const char* pName);
    void powerOff(void);
    void clear(void);
    void fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    void fillRoundRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t corner, uint16_t color);
    void drawStr(uint16_t x, uint16_t y, const char* pStr, uint16_t fontSz = 1U);
    void drawStr(uint16_t x, uint16_t y, const __FlashStringHelper* pHelper, uint16_t fontSz = 1U);
    void drawStrJp(uint16_t x, uint16_t y, const char* pTxt, uint16_t fontSz = 1U);
    void drawStrJp(uint16_t x, uint16_t y, const __FlashStringHelper* pHelper, uint16_t fontSz = 1U);
    void setTextColor(uint16_t fore, uint16_t back);
    void drawBmp(uint16_t x, uint16_t y, IconID eID, uint16_t transparent = 0);
    void drawJpgFile(DrawJpgFile::STORAGE eStrg, const char* pPath, uint16_t x = 0, uint16_t y = 0);
    void tickerCreate(TickerBase::TickerID eTkrID, uint16_t width, uint16_t height);
    void tickerDelete(TickerBase::TickerID eTkrID);
    void tickerSetText(TickerBase::TickerID eTkrID, const char* pStr);
    void tickerSetParam(TickerBase::TickerID eTkrID, uint8_t scale, uint16_t fore, uint16_t back);
    void tickerRender(TickerBase::TickerID eTkrID, uint16_t x, uint16_t y, TickerRender::RenderCBR pCBR = NULL, void* pUsrParam = NULL);
    void tickerResetPos(TickerBase::TickerID eTkrID);
    void drawQrCode(const char* pStr);

  private:
    const char* m_pName;
    class DrawCmdQ* m_pDrawCmdQ;
    int m_lcdWidth, m_lcdHeight;

  private:
    void pushDrawCmd(ICommand* pCmd);
};

#endif //_ISTATE_H_
