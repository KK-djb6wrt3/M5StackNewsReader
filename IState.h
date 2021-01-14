#ifndef _ISTATE_H_
#define _ISTATE_H_

#include <stddef.h>

class IState {
  public:
    enum StateID {
    	SID_Current,
      SID_Home,
      SID_QRCode,
      SID_Menu,
    };

  public:
    virtual void onInitialized(void) {}
    virtual void onTerminated(void) {}

    virtual StateID onLoop(void) = 0;
    virtual StateID onPressBtnA(bool isLong) {
      return SID_Current;
    }
    virtual StateID onPressBtnB(bool isLongoid) {
      return SID_Current;
    }
    virtual StateID onPressBtnC(bool isLong) {
      return SID_Current;
    }

    const char* getName(void) const {
      return m_pName;
    }

  protected:
    IState(const char* pName) : m_pName(pName) {}
  private:
    const char* m_pName;
};

#endif //_ISTATE_H_
