#ifndef _HOME_H_
#define _HOME_H_

#include "IState.h"

#include "Preference.h"
#include "YahooNewsReader.h"

class Home : public IState {
  public:
    Home(void);
    virtual ~Home(void);

    virtual void onInitialized(void) override;
    virtual void onTerminated(void) override;

    virtual StateID onLoop(void) override;
    virtual StateID onPressBtnA(bool isLong) override;
    virtual StateID onPressBtnB(bool isLong) override;
    virtual StateID onPressBtnC(bool isLong) override;
    virtual StateID onBatteryChanged(bool isCharging, int8_t level) override;

    static void setCategory(const char* pLink);

  private:
    struct Rect_t {
      int x, y, width, height;
    };

  private:
    YahooNewsReader::NewsReader m_NewsReader;
    int m_NewsIdx;
    int m_LoopCt;
    int m_WholeLoopCt;
    int m_ImgIdx;
    int8_t m_BatteryLv;
    bool m_isCharging;
    bool m_isGetNews;
    Preference m_Pref;
    Rect_t m_Rect;
    const char* m_pSSID;
    const char* m_pPassword;
    const char* m_pCategory;

  private:
    void getStatusArea(Rect_t* pRect);
    void nextNews(void);
    void drawBattery(void);
    void loadImage(void);
    bool getNews(void);
    void forward(void);
    void clean(void);
    bool findAccessPoint(void);
    static void _renderCBR(bool isWrap, void* pUsrParam);
    void renderCBR(bool isWrap);
};
#endif //_HOME_H_
