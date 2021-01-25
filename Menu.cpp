#include "Menu.h"
#include "Utility.h"
#include "Home.h"

#include <expat.h>
#include <vector>
#include <wstring.h>
#include <FS.h>
#include <SD.h>

#define TAG_SOURCE "Source"
#define TAG_CATEGORYNUM "CategoryNum"
#define TAG_CATEGORY "Category"
#define TAG_NAME "Name"
#define TAG_LINK "Link"
#define CHK_ELEM(tag) (strcmp(pElem, (tag)) == 0)

#define TFT_LIGHT_YELLOW COLOR565(250, 240, 139)
#define TFT_BLACK COLOR565(0, 0, 0)
#define TFT_WHITE COLOR565(255, 255, 255)

class Source {
  public:
    Source(void) {
      m_eStage = RS_Unknown;
      m_pCategory = NULL;
    }

    virtual ~Source(void) {
      int num = m_Categories.size();
      for (int i = 0; i < num; i++) {
        Category_t* pCategory = m_Categories[i];
        SAFE_DELARY(pCategory->pName);
        SAFE_DELARY(pCategory->pLink);
        SAFE_DELETE(pCategory);
      }
      m_Categories.clear();
    }

    bool load(const char* pFileName) {
      bool isLoaded = false;
      File file = SD.open(pFileName);
      if (file) {
        m_eStage = RS_None;
        XML_Parser parser = XML_ParserCreate(NULL);
        if (parser != NULL) {
          XML_SetElementHandler(parser, _startElem, _endElem);
          XML_SetCharacterDataHandler(parser, _recvData);
          XML_SetUserData(parser, this);
          while (file.available()) {
            String line = file.readString();
            XML_Parse(parser, line.c_str(), line.length(), 0);
          }
          XML_Parse(parser, "", 1, 1);
          XML_ParserFree(parser);
        }
        file.close();
        isLoaded = true;
      }
      return isLoaded;
    }

    int getNum(void) const {
      return m_Categories.size();
    }

    bool getSource(uint32_t index, const char** ppName, const char** ppLink) const {
      bool isGot = false;
      int num = getNum();
      if (index < num) {
        const char* pName = NULL;
        const char* pLink = NULL;
        const Category_t* pCategory = m_Categories[index];
        if (ppName != NULL) {
          *ppName = pCategory->pName;
        }
        if (ppLink != NULL) {
          *ppLink = pCategory->pLink;
        }
        isGot = true;
      }
      return isGot;
    }

  private:
    enum ReadStage {
      RS_Unknown = -1,
      RS_None = 0,
      RS_Source,
      RS_CategoryNum,
      RS_Category,
      RS_Name,
      RS_Link,
    };

    struct Category_t {
      const char* pName;
      const char* pLink;
    };

  private:
    ReadStage m_eStage;
    String m_Str;
    std::vector<Category_t*> m_Categories;
    Category_t* m_pCategory;

  private:
    static void _startElem(void* pUsrData, const XML_Char* pElem, const XML_Char* ppAttr[]) {
      Source* pSrc = (Source*)pUsrData;
      if (pSrc != NULL) {
        pSrc->startElem(pElem, ppAttr);
      }
    }

    static void _endElem(void* pUsrData, const XML_Char* pElem) {
      Source* pSrc = (Source*)pUsrData;
      if (pSrc != NULL) {
        pSrc->endElem(pElem);
      }
    }

    static void _recvData(void* pUsrData, const XML_Char* pData, int dataSz) {
      Source* pSrc = (Source*)pUsrData;
      if (pSrc != NULL) {
        pSrc->recvData(pData, dataSz);
      }
    }

    void startElem(const XML_Char* pElem, const XML_Char* ppAttr[]) {
      ReadStage eStage = RS_Unknown;
      switch (m_eStage) {
        case RS_None:
          if (CHK_ELEM(TAG_SOURCE)) {
            eStage = RS_Source;
          }
          break;
        case RS_Source:
          if (CHK_ELEM(TAG_CATEGORYNUM)) {
            eStage = RS_CategoryNum;
          }
          else if (CHK_ELEM(TAG_CATEGORY)) {
            eStage = RS_Category;
            SAFE_DELETE(m_pCategory);
            m_pCategory = new Category_t();
            m_pCategory->pName = NULL;
            m_pCategory->pLink = NULL;
          }
          break;
        case RS_Category:
          if (CHK_ELEM(TAG_NAME)) {
            eStage = RS_Name;
            m_Str = "";
          }
          else if (CHK_ELEM(TAG_LINK)) {
            eStage = RS_Link;
            m_Str = "";
          }
          break;
        default:
          //NOP
          break;
      }
      if (eStage != RS_Unknown) {
        m_eStage = eStage;
      }
    }

    void endElem(const XML_Char* pElem) {
      ReadStage eStage = RS_Unknown;
      switch (m_eStage) {
        case RS_None:
          break;
        case RS_Source:
          if (CHK_ELEM(TAG_SOURCE)) {
            eStage = RS_None;
          }
          break;
        case RS_CategoryNum:
          if (CHK_ELEM(TAG_CATEGORYNUM)) {
            eStage = RS_Source;
            m_Str = "";
          }
          break;
        case RS_Category:
          if (CHK_ELEM(TAG_CATEGORY)) {
            eStage = RS_Source;
            m_Categories.push_back(m_pCategory);
            m_pCategory = NULL;
          }
          break;
        case RS_Name:
          if (CHK_ELEM(TAG_NAME)) {
            eStage = RS_Category;
            if (m_pCategory != NULL) {
              m_pCategory->pName = duplicateString(m_Str);
            }
            m_Str = "";
          }
          break;
        case RS_Link:
          if (CHK_ELEM(TAG_LINK)) {
            eStage = RS_Category;
            if (m_pCategory != NULL) {
              m_pCategory->pLink = duplicateString(m_Str);
            }
            m_Str = "";
          }
          break;
        default:
          //NOP
          break;
      }
      if (eStage != RS_Unknown) {
        m_eStage = eStage;
      }
    }

    void recvData(const XML_Char* pData, int dataSz) {
      switch (m_eStage) {
        case RS_CategoryNum:
        case RS_Name:
        case RS_Link:
          appendStr(m_Str, pData, dataSz);
          break;
        default:
          break;
      }
    }

    void appendStr(String& str, const XML_Char* pData, int dataSz) const {
      for (int i = 0; i < dataSz; i++) {
        str.concat(pData[i]);
      }
    }
};

Menu::Menu(void)
  : IState ("Menu") {
  m_Index = 0;
  m_HeadIdx = 0;
  m_MenuNum = 0;
  m_pSource = NULL;
}

void Menu::onInitialized(void) {
  fillRect(0, 0, 320, 240, TFT_BLACK);
  setTextColor(TFT_WHITE, TFT_BLACK);

  m_Index = 0;
  m_pSource = new Source();
  m_pSource->load("/Source.xml");
  m_MenuNum = (m_pSource->getNum() + 1);

  updateMenu();
}

void Menu::onTerminated(void) {
  delete m_pSource;
}

IState::StateID Menu::onLoop(void) {
  return SID_Current;
}

IState::StateID Menu::onPressBtnA(bool isLong) {
  m_Index--;
  if (m_Index < 0) {
    m_Index = (m_MenuNum - 1);
  }
  updateMenu();
  return SID_Current;
}

IState::StateID Menu::onPressBtnB(bool isLongoid) {
  if (m_Index < (m_MenuNum - 1)) {
    const char* pLink = NULL;
    if (m_pSource->getSource(m_Index, NULL, &pLink)) {
      Home::setCategory(pLink);
    }
  }
  return SID_Home;
}

IState::StateID Menu::onPressBtnC(bool isLong) {
  m_Index++;
  if (m_Index >= m_MenuNum) {
    m_Index = 0;
  }
  updateMenu();
  return SID_Current;
}

bool Menu::getItemPos(int index, int* pX, int* pY) const {
  bool isGot = false;
  if (index < m_MenuNum) {
    if (pX != NULL) {
      *pX = 20;
    }
    if (pY != NULL) {
      *pY =  20 + ((index - m_HeadIdx) * 16);
    }
    isGot = true;
  }
  return isGot;
}

void Menu::updateMenu(void) {
  const char* pName = NULL;
  int x = 0, y = 0;
  setTextColor(TFT_WHITE, TFT_BLACK);
  drawStrJp(20, 2, "[カテゴリ選択]");
  for (int i = 0; i < m_MenuNum; i++) {
    if (m_pSource->getSource(i, &pName, NULL) == false) {
      pName = "Exit";
    }
    getItemPos(i, &x, &y);
    if (i == m_Index) {
      setTextColor(TFT_BLACK, TFT_LIGHT_YELLOW);
    }
    else {
      setTextColor(TFT_WHITE, TFT_BLACK);
    }
    drawStrJp(x, y, pName);
  }
}
