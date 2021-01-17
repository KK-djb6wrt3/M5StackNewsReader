#include <M5Stack.h>

#include "StateManager.h"

static StateManager s_StateMgr;

void setup() {
  M5.begin();
  M5.Power.begin();
  M5.Lcd.clear();
  s_StateMgr.begin();
}

void loop() {
  s_StateMgr.onLoop();

  if(M5.BtnA.wasReleasefor(500)){
    s_StateMgr.onPressBtnA(true);
  }
  else if (M5.BtnA.wasReleased()) {
    s_StateMgr.onPressBtnA(false);
  }

  if(M5.BtnB.wasReleasefor(500)){
    s_StateMgr.onPressBtnB(true);
  }
  else if (M5.BtnB.wasReleased()) {
    s_StateMgr.onPressBtnB(false);
  }

  if(M5.BtnC.wasReleasefor(500)){
    s_StateMgr.onPressBtnC(true);
  }
  else if (M5.BtnC.wasReleased()) {
    s_StateMgr.onPressBtnC(false);
  }

  M5.update();
  delay(33);
}
