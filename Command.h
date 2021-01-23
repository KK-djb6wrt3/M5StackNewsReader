#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <stddef.h>
#include <wstring.h>

#include "Resource.h"

///////////////////////////////////////////////////////////////////////////////
class ICommand {
  public:
    enum CmdID {
      CmdID_PowerOff,
      CmdID_Clear,
      CmdID_FillRect,
      CmdID_FillRoundRect,
      CmdID_DrawStr,
      CmdID_SetTextColor,
      CmdID_DrawBmp,
      CmdID_DrawJpgFile,
      CmdID_TickerCreate,
      CmdID_TickerDelete,
      CmdID_TickerSetText,
      CmdID_TickerSetParam,
      CmdID_TickerRender,
      CmdID_TickerResetPos,
      CmdID_DrawQrCode,
    };

  public:
    CmdID getCmdID(void) const;

  protected:
    ICommand(CmdID eID);

  private:
    const CmdID m_eCmdID;
};

///////////////////////////////////////////////////////////////////////////////
class PowerOff : public ICommand {
  public:
    PowerOff(void);
};

///////////////////////////////////////////////////////////////////////////////
class Clear : public ICommand {
  public:
    Clear(void);
};

///////////////////////////////////////////////////////////////////////////////
class FillRect : public ICommand {
  public:
    FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    void getParam(uint16_t* pX, uint16_t* pY, uint16_t* pWidth, uint16_t* pHeight, uint16_t* pColor) const;

  private:
    const uint16_t m_x, m_y;
    const uint16_t m_width, m_height;
    const uint16_t m_color;
};

///////////////////////////////////////////////////////////////////////////////
class FillRoundRect : public ICommand {
  public:
    FillRoundRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t corner, uint16_t color);
    void getParam(uint16_t* pX, uint16_t* pY, uint16_t* pWidth, uint16_t* pHeight, uint16_t* pCorner, uint16_t* pColor) const;

  private:
    const uint16_t m_x, m_y;
    const uint16_t m_width, m_height;
    const uint16_t m_corner;
    const uint16_t m_color;
};

///////////////////////////////////////////////////////////////////////////////
class DrawStr : public ICommand {
  public:
    DrawStr(uint16_t x, uint16_t y, const char* pStr, uint16_t fontSz = 1U);
    DrawStr(uint16_t x, uint16_t y, const __FlashStringHelper* pHelper, uint16_t fontSz = 1U);
    bool getParam(void** ppPtr, uint16_t* pX, uint16_t* pY, uint16_t* pFontSz) const;

  private:
    const uint16_t m_x, m_y;
    const uint16_t m_fontSz;
    void* const m_pPtr;
    const bool m_isProgram;
};

///////////////////////////////////////////////////////////////////////////////
class SetTextColor : public ICommand {
  public:
    SetTextColor(uint16_t fore, uint16_t back);
    void getParam(uint16_t* pFore, uint16_t* pBack) const;

  private:
    const uint16_t m_fore;
    const uint16_t m_back;
};

///////////////////////////////////////////////////////////////////////////////
class DrawBmp : public ICommand {
  public:
    DrawBmp(uint16_t x, uint16_t y, IconID eID, uint16_t transparent = 0);
    void getParam(IconID* peID, uint16_t* pX, uint16_t* pY, uint16_t* pTransParent) const;

  private:
    const uint16_t m_x, m_y;
    const IconID m_eIconID;
    const uint16_t m_transparent;
};

///////////////////////////////////////////////////////////////////////////////
class DrawJpgFile : public ICommand {
  public:
    enum STORAGE {
      SD,
    };

  public:
    DrawJpgFile(STORAGE eStrg, const char* pPath, uint16_t x = 0, uint16_t y = 0);
    void getParam(STORAGE* peStrg, const char** ppPath, uint16_t* pX, uint16_t* pY) const;

  private:
    STORAGE m_eStrg;
    const char* const m_pPath;
    const uint16_t m_x, m_y;
};

///////////////////////////////////////////////////////////////////////////////
class TickerBase : public ICommand {
  public:
    enum TickerID {
      TID_0,
      TID_1,
      TID_2,
      TID_3,
      TID_4,
      TID_MAX,
    };
  public:
    TickerID getTkrID(void) const;

  protected:
    TickerBase(CmdID eCmdID, TickerID eTkrID);

  private:
    const TickerID m_eTkrID;
};

///////////////////////////////////////////////////////////////////////////////
class TickerCreate : public TickerBase {
  public:
    TickerCreate(TickerID eTkrID, uint16_t width, uint16_t height);
    TickerID getParam(uint16_t* pWidth, uint16_t* pHeight) const;

  private:
    const uint16_t m_width, m_height;
};

///////////////////////////////////////////////////////////////////////////////
class TickerDelete : public TickerBase {
  public:
    TickerDelete(TickerID eTkrID);
};

///////////////////////////////////////////////////////////////////////////////
class TickerSetText : public TickerBase {
  public:
    TickerSetText(TickerID eTkrID, const char* pStr);
    TickerID getParam(const char** ppStr);

  private:
    const char* const m_pStr;
};

///////////////////////////////////////////////////////////////////////////////
class TickerSetParam : public TickerBase {
  public:
    TickerSetParam(TickerID eTkrID, uint8_t scale, uint16_t fore, uint16_t back);
    TickerID getParam(uint8_t* pScale, uint16_t* pFore, uint16_t* pBack) const;

  private:
    const uint8_t m_scale;
    const uint16_t m_fore, m_back;
};

///////////////////////////////////////////////////////////////////////////////
class TickerRender : public TickerBase {
  public:
    typedef void (*RenderCBR)(bool isWrap, void* pUsrParam);

  public:
    TickerRender(TickerID eTkrID, uint16_t x, uint16_t y, RenderCBR pCBR = NULL, void* pUsrParam = NULL);
    TickerID getParam(uint16_t* pX, uint16_t* pY) const;
    void callback(bool isWrap) const;

  private:
    const uint16_t m_x, m_y;
    RenderCBR const m_pCBR;
    void* const m_pUsrParam;
};

///////////////////////////////////////////////////////////////////////////////
class TickerResetPos : public TickerBase {
  public:
    TickerResetPos(TickerID eTkrID);
};

///////////////////////////////////////////////////////////////////////////////
class DrawQrCode : public ICommand {
  public:
    DrawQrCode(const char* pStr);
    void getParam(const char** ppStr) const;

  private:
    const char* const m_pStr;
};

#endif //_COMMAND_H_
