#ifndef _HOME_H_
#define _HOME_H_

#include "IState.h"

#include "efontTicker.h"
#include "Preference.h"
#include "YahooNewsReader.h"

class Home : public IState {
  public:
    Home(void);
    virtual ~Home(void);

    virtual void onInitialized(void) override;

    virtual StateID onLoop(void) override;
    virtual StateID onPressBtnA(bool isLong) override;

  private:
    struct Rect_t {
      int x, y, width, height;
    };

  private:
    YahooNewsReader::NewsReader m_NewsReader;
    efontTicker m_etTitle;
    efontTicker m_etDesc;
    int m_NewsIdx;
    int m_LoopCt;
    int m_WholeLoopCt;
    int m_ImgIdx;
    int8_t m_PrevBatLv;
    bool m_isPrevCharge;
    Preference m_Pref;
    Rect_t m_Rect;
    const char* m_pSSID;
    const char* m_pPassword;

  private:
    void getStatusArea(Rect_t* pRect);
    void nextNews(void);
    void drawBattery(void);
    void loadImage(void);
    bool getNews(void);
};
#endif //_HOME_H_
