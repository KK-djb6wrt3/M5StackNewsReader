#include "IState.h"

#include <queue>

class DrawCmdQ {
  public:
    void push(ICommand* pCmd) {
      m_Queue.push(pCmd);
    }

    bool pop(ICommand** ppCmd) {
      bool isPoke = peek(ppCmd);
      if (isPoke) {
        m_Queue.pop();
      }
      return isPoke;
    }

    bool peek(ICommand** ppCmd) {
      bool isPoke = false;
      if (m_Queue.size() > 0) {
        if (ppCmd != NULL) {
          *ppCmd = m_Queue.front();
        }
        isPoke = true;
      }
      return isPoke;
    }

  private:
    std::queue<ICommand*> m_Queue;
};

///////////////////////////////////////////////////////////////////////////////
IState::IState(const char* pName)
  : m_pName(pName) {
  m_pDrawCmdQ = new DrawCmdQ();
  m_lcdWidth = 0;
  m_lcdHeight = 0;
}

IState::~IState(void) {
  delete m_pDrawCmdQ;
}

void IState::onInitialized(void) {
}

void IState::onTerminated(void) {
}

IState::StateID IState::onPressBtnA(bool isLong) {
  return SID_Current;
}

IState::StateID IState::onPressBtnB(bool isLongoid) {
  return SID_Current;
}

IState::StateID IState::onPressBtnC(bool isLong) {
  return SID_Current;
}

IState::StateID IState::onBatteryChanged(bool isCharging, int8_t level) {
  return SID_Current;
}

bool IState::popDrawCmd(ICommand** ppCmd) {
  return m_pDrawCmdQ->pop(ppCmd);
}

void IState::pushDrawCmd(ICommand* pCmd) {
  m_pDrawCmdQ->push(pCmd);
}

const char* IState::getName(void) const {
  return m_pName;
}

void IState::setLcdSize(int width, int height) {
  m_lcdWidth = width;
  m_lcdHeight = height;
}

void IState::getLcdSize(int* pWidth, int* pHeight) const {
  if (pWidth != NULL) {
    *pWidth = m_lcdWidth;
  }
  if (pHeight != NULL) {
    *pHeight = m_lcdHeight;
  }
}

void IState::powerOff(void) {
  pushDrawCmd(new PowerOff());
}

void IState::clear(void) {
  pushDrawCmd(new Clear());
}

void IState::fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
  pushDrawCmd(new FillRect(x, y, width, height, color));
}

void IState::fillRoundRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t corner, uint16_t color) {
  pushDrawCmd(new FillRoundRect(x, y, width, height, corner, color));
}

void IState::drawStr(uint16_t x, uint16_t y, const char* pStr, uint16_t fontSz) {
  pushDrawCmd(new DrawStr(x, y, pStr, fontSz));
}

void IState::drawStr(uint16_t x, uint16_t y, const __FlashStringHelper* pHelper, uint16_t fontSz) {
  pushDrawCmd(new DrawStr(x, y, pHelper, fontSz));
}

void IState::drawStrJp(uint16_t x, uint16_t y, const char* pTxt, uint16_t fontSz) {
  pushDrawCmd(new DrawStrJp(x, y, pTxt, fontSz));
}

void IState::drawStrJp(uint16_t x, uint16_t y, const __FlashStringHelper* pHelper, uint16_t fontSz) {
  pushDrawCmd(new DrawStrJp(x, y, pHelper, fontSz));
}

void IState::setTextColor(uint16_t fore, uint16_t back) {
  pushDrawCmd(new SetTextColor(fore, back));
}

void IState::drawBmp(uint16_t x, uint16_t y, IconID eID, uint16_t transparent) {
  pushDrawCmd(new DrawBmp(x, y, eID, transparent));
}

void IState::drawJpgFile(DrawJpgFile::STORAGE eStrg, const char* pPath, uint16_t x, uint16_t y) {
  pushDrawCmd(new DrawJpgFile(eStrg, pPath, x, y));
}

void IState::tickerCreate(TickerBase::TickerID eTkrID, uint16_t width, uint16_t height) {
  pushDrawCmd(new TickerCreate(eTkrID, width, height));
}

void IState::tickerDelete(TickerBase::TickerID eTkrID) {
  pushDrawCmd(new TickerDelete(eTkrID));
}

void IState::tickerSetText(TickerBase::TickerID eTkrID, const char* pStr) {
  pushDrawCmd(new TickerSetText(eTkrID, pStr));
}

void IState::tickerSetParam(TickerBase::TickerID eTkrID, uint8_t scale, uint16_t fore, uint16_t back) {
  pushDrawCmd(new TickerSetParam(eTkrID, scale, fore, back));
}

void IState::tickerRender(TickerBase::TickerID eTkrID, uint16_t x, uint16_t y, TickerRender::RenderCBR pCBR, void* pUsrParam) {
  pushDrawCmd(new TickerRender(eTkrID, x, y, pCBR, pUsrParam));
}

void IState::tickerResetPos(TickerBase::TickerID eTkrID) {
  pushDrawCmd(new TickerResetPos(eTkrID));
}

void IState::drawQrCode(const char* pStr) {
  pushDrawCmd(new DrawQrCode(pStr));
}
