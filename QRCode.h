#ifndef _QR_CODE_H_
#define _QR_CODE_H_

#include "IState.h"

class QRCode : public IState {
  public:
    QRCode(void) : IState("QRCode") {}
    virtual ~QRCode(void) {}

    virtual void onInitialized(void) override {
    	pushDrawCmd(new Clear());
      if (m_pLink != NULL) {
      	pushDrawCmd(new DrawQrCode(m_pLink));
      }
    }

    static void setLink(const char* pLink) {
      m_pLink = pLink;
    }

    virtual StateID onLoop(void) override {
      return SID_Current;
    }

    virtual StateID onPressBtnA(bool isLong) override {
      return SID_Home;
    }

    virtual StateID onPressBtnB(bool isLongoid) override {
      return SID_Home;
    }

    virtual StateID onPressBtnC(bool isLong) override {
      return SID_Home;
    }
  private:
    static const char* m_pLink;
};

#endif //_QR_CODE_H_
