#include "StateManager.h"

#include "Home.h"
#include "Sleep.h"
#include "QRCode.h"

static Home s_Home;
static Sleep s_Sleep;
static QRCode s_QRCode;

const char* QRCode::m_pLink = NULL;

StateManager::StateManager(void) {
  m_pCurState = NULL;
}

void StateManager::begin(void) {
  if (m_pCurState == NULL) {
    m_pCurState = &s_Home;
    m_pCurState->onInitialized();
  }
}

StateManager::~StateManager(void) {
  end();
}

void StateManager::end(void) {
  if (m_pCurState != NULL) {
    m_pCurState->onTerminated();
    m_pCurState = NULL;
  }
}

void StateManager::onLoop(void) {
  if (m_pCurState != NULL) {
    changeState(m_pCurState->onLoop());
  }
}
void StateManager::onPressBtnA(bool isLong) {
  if (m_pCurState != NULL) {
    changeState(m_pCurState->onPressBtnA(isLong));
  }
}

void StateManager::onPressBtnB(bool isLong) {
  if (m_pCurState != NULL) {
    changeState(m_pCurState->onPressBtnB(isLong));
  }
}

void StateManager::onPressBtnC(bool isLong) {
  if (m_pCurState != NULL) {
    changeState(m_pCurState->onPressBtnC(isLong));
  }
}

void StateManager::onBatteryChanged(bool isCharging, int8_t level){
  if (m_pCurState != NULL) {
    changeState(m_pCurState->onBatteryChanged(isCharging, level));
  }
}

bool StateManager::popDrawCommand(ICommand** ppCmd) {
  bool isGot = false;
  if (m_pCurState != NULL) {
    isGot = m_pCurState->popDrawCmd(ppCmd);
  }
  return isGot;
}

void StateManager::changeState(IState::StateID eID) {
  IState* pNewState = NULL;
  switch (eID) {
    case IState::SID_Current:
      pNewState = m_pCurState;
      break;
    case IState::SID_Home:
      pNewState = &s_Home;
      break;
    case IState::SID_Sleep:
      pNewState = &s_Sleep;
      break;
    case IState::SID_QRCode:
      pNewState = &s_QRCode;
    default:
      break;
  }

  if (m_pCurState != pNewState) {
    const char* pCur = "null";
    const char* pNew = "null";
    if (m_pCurState != NULL) {
      m_pCurState->onTerminated();
      pCur = m_pCurState->getName();
    }
    if (pNewState != NULL) {
      pNewState->onInitialized();
      pNew = pNewState->getName();
    }
    m_pCurState = pNewState;
  }
}
