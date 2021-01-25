#ifndef _MENU_H_
#define _MENU_H_

#include "IState.h"

class Menu : public IState {
  public:
    Menu(void);
    virtual void onInitialized(void);
    virtual void onTerminated(void);
    virtual StateID onLoop(void);
    virtual StateID onPressBtnA(bool isLong);
    virtual StateID onPressBtnB(bool isLongoid);
    virtual StateID onPressBtnC(bool isLong);

  private:
    class Source* m_pSource;
    int m_Index;
    int m_HeadIdx;
    int m_MenuNum;

  private:
  	bool getItemPos(int index, int* pX, int* pY) const;
  	void updateMenu(void);

};

#endif //_MENU_H_
