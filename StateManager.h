#ifndef _STATE_MANAGER_H_
#define _STATE_MANAGER_H_

#include "IState.h"

class StateManager {
  public:
    StateManager(void);
    virtual ~StateManager(void);

    void begin(void);
    void end(void);

    void onLoop(void);
    void onPressBtnA(bool isLong);
    void onPressBtnB(bool isLong);
    void onPressBtnC(bool isLong);

  private:
    IState* m_pCurState;
    void changeState(IState::StateID eID);
};

#endif //_STATE_MANAGER_H_