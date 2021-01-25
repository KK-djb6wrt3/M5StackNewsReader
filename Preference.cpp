#include "Preference.h"
#include "Utility.h"

#include <expat.h>
#include <vector>
#include <wstring.h>
#include <FS.h>
#include <SD.h>

#define TAG_PREFERENCE		"Preference"
#define TAG_ACCESSPOINTS	"AccessPoints"
#define TAG_AP						"AP"
#define TAG_SSID					"SSID"
#define TAG_PASSWORD			"Password"
#define TAG_BACKGROUND		"Background"
#define TAG_IMAGENUM			"ImageNum"
#define TAG_IMAGE					"Image"
#define TAG_FILE					"File"
#define TAG_BALLOON				"Balloon"
#define CHK_ELEM(tag) (strcmp(pElem, (tag)) == 0)

class PrefImpl {
  public:
    PrefImpl(void) {
      m_pImage = NULL;
      m_pAP = NULL;
      m_isLoaded = false;
      m_eStage = RS_None;
    }

    virtual ~PrefImpl(void) {
      int num = getImageNum();
      for (int i = 0; i < num; i++) {
        Image_t* pImg = m_Images[i];
        SAFE_DELARY(pImg->pFile);
        SAFE_DELETE(pImg);
      }
      m_Images.clear();

      if (m_pAP != NULL ) {
        SAFE_DELARY(m_pAP->pSSID);
        SAFE_DELARY(m_pAP->pPassword);
        SAFE_DELETE(m_pAP);
      }
    }

    bool load(const char* pFileName) {
      File file = SD.open(pFileName);
      if (file) {
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
        m_isLoaded = true;
        return true;
      }
      m_isLoaded = false;
      return false;
    }

    bool isLoaded(void) const {
      return m_isLoaded;
    }

    int getApNum(void) const {
      return m_APs.size();
    }

    bool getAP(int index, const char** ppSSID, const char** ppPassword) const {
      bool result = false;
      if (index < getApNum()) {
        const AP_t* pAP = m_APs[index];
        if (ppSSID != NULL) {
          *ppSSID = pAP->pSSID;
        }
        if (ppPassword != NULL) {
          *ppPassword = pAP->pPassword;
        }
        return true;
      }
      return false;
    }

    int getImageNum(void) const {
      return m_Images.size();
    }

    bool getImage(int index, const char** ppFile, Preference::BalloonPos* pePos) const {
      bool result = false;
      if (index < getImageNum()) {
        const Image_t* pImg = m_Images[index];
        if (ppFile != NULL) {
          *ppFile = pImg->pFile;
        }
        if (pePos != NULL) {
          *pePos = pImg->ePos;
        }
        result = true;
      }
      return result;
    }
  private:
    struct Image_t {
      const char* pFile;
      Preference::BalloonPos ePos;
    };

    struct AP_t {
      const char* pSSID;
      const char* pPassword;
    };

    enum ReadStage {
      RS_Unknown = -1,
      RS_None = 0,
      RS_Preference,
      RS_AccessPoints,
      RS_AP,
      RS_SSID,
      RS_Password,
      RS_BackGround,
      RS_Image,
      RS_ImageNum,
      RS_File,
      RS_Balloon,
    };

  private:
    ReadStage m_eStage;
    String m_Str;
    std::vector<Image_t*> m_Images;
    Image_t* m_pImage;
    std::vector<AP_t*> m_APs;
    AP_t* m_pAP;
    bool m_isLoaded;

  private:
    static void _startElem(void* pUsrData, const XML_Char* pElem, const XML_Char* ppAttr[]) {
      PrefImpl* pPref = (PrefImpl*)pUsrData;
      if (pPref != NULL) {
        pPref->startElem(pElem, ppAttr);
      }
    }

    static void _endElem(void* pUsrData, const XML_Char* pElem) {
      PrefImpl* pPref = (PrefImpl*)pUsrData;
      if (pPref != NULL) {
        pPref->endElem(pElem);
      }
    }

    static void _recvData(void* pUsrData, const XML_Char* pData, int dataSz) {
      PrefImpl* pPref = (PrefImpl*)pUsrData;
      if (pPref != NULL) {
        pPref->recvData(pData, dataSz);
      }
    }

    void startElem(const XML_Char* pElem, const XML_Char* ppAttr[]) {
      ReadStage eStage = RS_Unknown;
      switch (m_eStage) {
        case RS_None:
          if (CHK_ELEM(TAG_PREFERENCE)) {
            eStage = RS_Preference;
          }
          break;
        case RS_Preference:
          if (CHK_ELEM(TAG_ACCESSPOINTS)) {
            eStage = RS_AccessPoints;
          }
          else if (CHK_ELEM(TAG_BACKGROUND)) {
            eStage = RS_BackGround;
          }
          break;
        case RS_AccessPoints:
          if (CHK_ELEM(TAG_AP)) {
            eStage = RS_AP;
          }
          break;
        case RS_AP:
          if (CHK_ELEM(TAG_SSID)) {
            eStage = RS_SSID;
            m_Str = "";
            m_pAP = new AP_t();
            m_pAP->pSSID = NULL;
            m_pAP->pPassword = NULL;
          }
          else if (CHK_ELEM(TAG_PASSWORD)) {
            eStage = RS_Password;
            m_Str = "";
          }
          break;
        case RS_BackGround:
          if (CHK_ELEM(TAG_IMAGENUM)) {
            eStage = RS_ImageNum;
            m_Str = "";
          }
          else if (CHK_ELEM(TAG_IMAGE)) {
            eStage = RS_Image;
            SAFE_DELETE(m_pImage);
            m_pImage = new Image_t();
            m_pImage->pFile = NULL;
            m_pImage->ePos = Preference::BP_LeftTop;
          }
          break;
        case RS_Image:
          if (CHK_ELEM(TAG_FILE)) {
            eStage = RS_File;
            m_Str = "";
          }
          else if (CHK_ELEM(TAG_BALLOON)) {
            eStage = RS_Balloon;
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
        case RS_Preference:
          if (CHK_ELEM(TAG_PREFERENCE)) {
            eStage = RS_None;
          }
          break;
        case RS_AccessPoints:
          if (CHK_ELEM(TAG_ACCESSPOINTS)) {
            eStage = RS_Preference;
          }
          break;
        case RS_AP:
          if (CHK_ELEM(TAG_AP)) {
            eStage = RS_AccessPoints;
            if (m_pAP != NULL) {
              m_APs.push_back(m_pAP);
              m_pAP = NULL;
            }
          }
          break;
        case RS_BackGround:
          if (CHK_ELEM(TAG_BACKGROUND)) {
            eStage = RS_Preference;
          }
          break;
        case RS_Image:
          if (CHK_ELEM(TAG_IMAGE)) {
            eStage = RS_BackGround;
            if (m_pImage != NULL) {
              m_Images.push_back(m_pImage);
              m_pImage = NULL;
            }
          }
          break;
        case RS_SSID:
          eStage = RS_AP;
          if (m_pAP != NULL) {
            m_pAP->pSSID = duplicateString(m_Str);
          }
          m_Str = "";
          break;
        case RS_Password:
          eStage = RS_AP;
          if (m_pAP != NULL) {
            m_pAP->pPassword = duplicateString(m_Str);
          }
          m_Str = "";
          break;
        case RS_ImageNum:
          eStage = RS_BackGround;
          m_Str = "";
          break;
        case RS_File:
          eStage = RS_Image;
          if (m_pImage != NULL) {
            m_pImage->pFile = duplicateString(m_Str);
          }
          m_Str = "";
          break;
        case RS_Balloon:
          eStage = RS_Image;
          if (m_pImage != NULL) {
            if (m_Str == "LT") {
              m_pImage->ePos = Preference::BP_LeftTop;
            }
            else if (m_Str == "LB") {
              m_pImage->ePos = Preference::BP_LeftBottom;
            }
            else if (m_Str == "RT") {
              m_pImage->ePos = Preference::BP_RightTop;
            }
            else if (m_Str == "RB") {
              m_pImage->ePos = Preference::BP_RightBottom;
            }
          }
          m_Str = "";
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
        case RS_SSID:
        case RS_Password:
        case RS_ImageNum:
        case RS_File:
        case RS_Balloon:
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

Preference::Preference(void) {
  m_pImpl = new PrefImpl();
}

Preference::~Preference(void) {
  SAFE_DELETE(m_pImpl);
}

bool Preference::load(const char* pFileName) {
  return m_pImpl->load(pFileName);
}

bool Preference::isLoaded(void) const {
  return m_pImpl->isLoaded();
}

int Preference::getApNum(void) const {
  return m_pImpl->getApNum();
}

bool Preference::getAP(int index, const char** ppSSID, const char** ppPassword) const {
  return m_pImpl->getAP(index, ppSSID, ppPassword);
}

int Preference::getImageNum(void) const {
  return m_pImpl->getImageNum();
}

bool Preference::getImage(int index, const char** ppFile, BalloonPos* pePos) const {
  return m_pImpl->getImage(index, ppFile, pePos);
}
