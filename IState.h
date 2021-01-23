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

  protected:
	IState(const char* pName);
	void pushDrawCmd(ICommand* pCmd);

  private:
	const char* m_pName;
	class DrawCmdQ* m_pDrawCmdQ;
};

#endif //_ISTATE_H_
