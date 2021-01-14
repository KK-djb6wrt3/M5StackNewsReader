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
  if (M5.BtnA.wasPressed()) {
    s_StateMgr.onPressBtnA(false);
  }
  if (M5.BtnB.wasPressed()) {
    s_StateMgr.onPressBtnB(false);
  }
  if (M5.BtnC.wasPressed()) {
    s_StateMgr.onPressBtnC(false);
  }
  M5.update();
  delay(33);
}
