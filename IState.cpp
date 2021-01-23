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

ICommand::ICommand(CmdID eID)
  : m_eCmdID(eID) {
}

ICommand::CmdID ICommand::getCmdID(void) const {
  return m_eCmdID;
}
