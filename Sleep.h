#ifndef _SLEEP_H_
#define _SLEEP_H_

#include "IState.h"

class Sleep : public IState {
  public:
    Sleep() : IState("Sleep") {}
    virtual ~Sleep() {}

    virtual void onInitialized(void) override {
    	powerOff();
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
};
#endif //_SLEEP_H_
