#include "Command.h"

///////////////////////////////////////////////////////////////////////////////
PowerOff::PowerOff(void) : ICommand(CmdID_PowerOff) {}

///////////////////////////////////////////////////////////////////////////////
Clear::Clear(void) : ICommand(CmdID_Clear) {}

///////////////////////////////////////////////////////////////////////////////
FillRect::FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
  : ICommand(CmdID_FillRect)
  , m_x(x)
  , m_y(y)
  , m_width(width)
  , m_height(height)
  , m_color(color) {
}

void FillRect::getParam(uint16_t* pX, uint16_t* pY, uint16_t* pWidth, uint16_t* pHeight, uint16_t* pColor) const {
  if (pX != NULL) {
	*pX = m_x;
  }
  if (pY != NULL) {
	*pY = m_y;
  }
  if (pWidth != NULL) {
	*pWidth = m_width;
  }
  if (pHeight != NULL) {
	*pHeight = m_height;
  }
  if (pColor != NULL) {
	*pColor = m_color;
  }
}

FillRoundRect::FillRoundRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t corner, uint16_t color)
  : ICommand(CmdID_FillRoundRect)
  , m_x(x)
  , m_y(y)
  , m_width(width)
  , m_height(height)
  , m_corner(corner)
  , m_color(color) {
}

void FillRoundRect::getParam(uint16_t* pX, uint16_t* pY, uint16_t* pWidth, uint16_t* pHeight, uint16_t* pCorner, uint16_t* pColor) const {
  if (pX != NULL) {
	*pX = m_x;
  }
  if (pY != NULL) {
	*pY = m_y;
  }
  if (pWidth != NULL) {
	*pWidth = m_width;
  }
  if (pHeight != NULL) {
	*pHeight = m_height;
  }
  if (pCorner != NULL) {
	*pCorner = m_corner;
  }
  if (pColor != NULL) {
	*pColor = m_color;
  }
}

///////////////////////////////////////////////////////////////////////////////
DrawStr::DrawStr(uint16_t x, uint16_t y, const char* pStr, uint16_t fontSz)
  : ICommand(CmdID_DrawStr)
  , m_isProgram(false)
  , m_x(x)
  , m_y(y)
  , m_pPtr((void*)pStr)
  , m_fontSz(fontSz) {
}

DrawStr::DrawStr(uint16_t x, uint16_t y, const __FlashStringHelper* pHelper, uint16_t fontSz)
  : ICommand(CmdID_DrawStr)
  , m_isProgram(false)
  , m_x(x)
  , m_y(y)
  , m_pPtr((void*)pHelper)
  , m_fontSz(fontSz) {
}

bool DrawStr::getParam(void** ppPtr, uint16_t* pX, uint16_t* pY, uint16_t* pFontSz) const {
  if (ppPtr != NULL) {
	*ppPtr = m_pPtr;
  }
  if (pX != NULL) {
	*pX = m_x;
  }
  if (pY != NULL) {
	*pY = m_y;
  }
  if (pFontSz != NULL) {
	*pFontSz = m_fontSz;
  }
  return m_isProgram;
}

///////////////////////////////////////////////////////////////////////////////
SetTextColor::SetTextColor(uint16_t fore, uint16_t back)
  : ICommand(CmdID_SetTextColor)
  , m_fore(fore)
  , m_back(back) {
}

void SetTextColor::getParam(uint16_t* pFore, uint16_t* pBack) const {
  if (pFore != NULL) {
	*pFore = m_fore;
  }
  if (pBack != NULL) {
	*pBack = m_back;
  }
}

///////////////////////////////////////////////////////////////////////////////
DrawBmp::DrawBmp(uint16_t x, uint16_t y, IconID eID, uint16_t transparent)
  : ICommand(CmdID_DrawBmp)
  , m_x(x)
  , m_y(y)
  , m_eIconID(eID)
  , m_transparent(transparent) {
}

void DrawBmp::getParam(IconID* peID, uint16_t* pX, uint16_t* pY, uint16_t* pTransParent) const {
  if (peID != NULL) {
	*peID = m_eIconID;
  }
  if (pX != NULL) {
	*pX = m_x;
  }
  if (pY != NULL) {
	*pY = m_y;
  }
  if (pTransParent != NULL) {
	*pTransParent = m_transparent;
  }
}

///////////////////////////////////////////////////////////////////////////////
DrawJpgFile::DrawJpgFile(STORAGE eStrg, const char* pPath, uint16_t x, uint16_t y)
  : ICommand(CmdID_DrawJpgFile)
  , m_eStrg(eStrg)
  , m_pPath(pPath)
  , m_x(x)
  , m_y(y) {
}

void DrawJpgFile::getParam(STORAGE* peStrg, const char** ppPath, uint16_t* pX, uint16_t* pY) const {
  if (peStrg != NULL) {
	*peStrg = m_eStrg;
  }
  if (ppPath != NULL) {
	*ppPath = m_pPath;
  }
  if (pX != NULL) {
	*pX = m_x;
  }
  if (pY != NULL) {
	*pY = m_y;
  }
}

///////////////////////////////////////////////////////////////////////////////
TickerBase::TickerBase(CmdID eCmdID, TickerID eTkrID)
  : ICommand(eCmdID)
  , m_eTkrID(eTkrID) {
}

TickerBase::TickerID TickerBase::getTkrID(void) const {
  return m_eTkrID;
}

///////////////////////////////////////////////////////////////////////////////
TickerCreate::TickerCreate(TickerID eTkrID, uint16_t width, uint16_t height)
  : TickerBase(CmdID_TickerCreate, eTkrID)
  , m_width(width)
  , m_height(height) {
}

TickerBase::TickerID TickerCreate::getParam(uint16_t* pWidth, uint16_t* pHeight) const {
  if (pWidth != NULL) {
	*pWidth = m_width;
  }
  if (pHeight != NULL) {
	*pHeight = m_height;
  }
  return getTkrID();
}

///////////////////////////////////////////////////////////////////////////////
TickerDelete::TickerDelete(TickerID eTkrID)
  : TickerBase (CmdID_TickerDelete, eTkrID) {
}

///////////////////////////////////////////////////////////////////////////////
TickerSetText::TickerSetText(TickerID eTkrID, const char* pStr)
  : TickerBase(CmdID_TickerSetText, eTkrID)
  , m_pStr(pStr) {
}

TickerBase::TickerID TickerSetText::getParam(const char** ppStr) {
  if (ppStr != NULL) {
	*ppStr = m_pStr;
  }
  return getTkrID();
}

///////////////////////////////////////////////////////////////////////////////
TickerSetParam::TickerSetParam(TickerID eTkrID, uint8_t scale, uint16_t fore, uint16_t back)
  : TickerBase(CmdID_TickerSetParam, eTkrID)
  , m_scale(scale)
  , m_fore(fore)
  , m_back(back) {

}

TickerBase::TickerID TickerSetParam::getParam(uint8_t* pScale, uint16_t* pFore, uint16_t* pBack) const {
  if (pScale != NULL) {
	*pScale = m_scale;
  }
  if (pFore != NULL) {
	*pFore = m_fore;
  }
  if (pBack != NULL) {
	*pBack = m_back;
  }
  return getTkrID();
}


///////////////////////////////////////////////////////////////////////////////
TickerRender::TickerRender(TickerID eTkrID, uint16_t x, uint16_t y, RenderCBR pCBR, void* pUsrParam)
  : TickerBase(CmdID_TickerRender, eTkrID)
  , m_x(x)
  , m_y(y)
  , m_pCBR(pCBR)
  , m_pUsrParam(pUsrParam) {
}

TickerBase::TickerID TickerRender::getParam(uint16_t* pX, uint16_t* pY) const {
  if (pX != NULL) {
	*pX = m_x;
  }
  if (pY != NULL) {
	*pY = m_y;
  }
  return getTkrID();
}

void TickerRender::callback(bool isWrap) const {
  if (m_pCBR != NULL) {
	(*m_pCBR)(isWrap, m_pUsrParam);
  }
}

///////////////////////////////////////////////////////////////////////////////
TickerResetPos::TickerResetPos(TickerID eTkrID)
  : TickerBase(CmdID_TickerResetPos, eTkrID) {
}

///////////////////////////////////////////////////////////////////////////////
DrawQrCode::DrawQrCode(const char* pStr)
  : ICommand(CmdID_DrawQrCode)
  , m_pStr(pStr) {
}

void DrawQrCode::getParam(const char** ppStr) const {
	if(ppStr != NULL) {
		*ppStr = m_pStr;
	}
}
