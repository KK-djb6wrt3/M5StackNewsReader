#include "YahooNewsReader.h"
#include "Utility.h"

#include <WiFiClientSecure.h>
#include <expat.h>

#define TAG_TITLE "title"
#define TAG_DESC "description"
#define TAG_LINK "link"

namespace YahooNewsReader {
class RssParser {
  public:
    RssParser(void) {
      m_Parser = NULL;
      m_pItem = NULL;
      m_pStr = NULL;
    }

    virtual ~RssParser(void) {
      clean();
    }

    void begin(void) {
      clean();
      if (m_Parser == NULL) {
        m_Parser = XML_ParserCreate(NULL);
        if (m_Parser != NULL) {
          XML_SetElementHandler(m_Parser, _startElem, _endElem);
          XML_SetCharacterDataHandler(m_Parser, _recvData);
          XML_SetUserData(m_Parser, this);
        }
      }
    }

    void put(const char* pStr, int32_t sz) {
      if (m_Parser != NULL) {
        XML_Parse(m_Parser, pStr, sz, 0);
      }
    }

    void end(void) {
      if (m_Parser != NULL) {
        XML_Parse(m_Parser, "", 1, 1);
        XML_ParserFree(m_Parser);
        m_Parser = NULL;
      }
      SAFE_DELETE(m_pItem);
      SAFE_DELETE(m_pStr);
    }

    const NewsEntries* getEntries(void) {
      return &m_Items;
    }

    void clean(void) {
      uint16_t sz = m_Items.size();
      for (int i = 0; i < sz; i++) {
        SAFE_DELETE(m_Items[i]->pTitle);
        SAFE_DELETE(m_Items[i]->pDesc);
        SAFE_DELETE(m_Items[i]->pLink);
        SAFE_DELETE(m_Items[i]);
      }
      m_Items.clear();
    }

  private:
    XML_Parser m_Parser;
    NewsEntries m_Items;
    NewsEntry_t* m_pItem;
    String* m_pStr;

  private:
    static void _startElem(void* pUsrData, const XML_Char* pElem, const XML_Char* ppAttr[]) {
      RssParser* pParser = (RssParser*)pUsrData;
      if (pParser != NULL) {
        pParser->startElem(pElem, ppAttr);
      }
    }

    static void _endElem(void* pUsrData, const XML_Char* pElem) {
      RssParser* pParser = (RssParser*)pUsrData;
      if (pParser != NULL) {
        pParser->endElem(pElem);
      }
    }

    static void _recvData(void* pUsrData, const XML_Char* pData, int dataSz) {
      RssParser* pParser = (RssParser*)pUsrData;
      if (pParser != NULL) {
        pParser->recvData(pData, dataSz);
      }
    }

    void dupStr(const String* pSrc, const char** ppDst) {
      if ((pSrc != NULL) && (ppDst != NULL)) {
        unsigned int len = (pSrc->length() + 1);
        char* pDst = new char[len];
        pSrc->toCharArray(pDst, len);
        *ppDst = pDst;
        //Serial.println(pDst);
      }
    }

    void startElem(const XML_Char* pElem, const XML_Char* ppAttr[]) {
      if (strcmp(pElem, "item") == 0) {
        SAFE_DELETE(m_pItem);
        m_pItem = new NewsEntry_t();
      }
      else {
        if ((strcmp(pElem, TAG_TITLE) == 0) ||
            (strcmp(pElem, TAG_DESC) == 0) ||
            (strcmp(pElem, TAG_LINK) == 0)) {
          SAFE_DELETE(m_pStr);
          m_pStr = new String();
        }
      }
    }

    void endElem(const XML_Char * pElem) {
      if (strcmp(pElem, "item") == 0) {
        m_Items.push_back(m_pItem);
        m_pItem = NULL;
      }
      else if ((m_pItem != NULL) && (m_pStr != NULL)) {
        if (strcmp(pElem, TAG_TITLE) == 0) {
          dupStr(m_pStr, &m_pItem->pTitle);
          SAFE_DELETE(m_pStr);
        }
        else if (strcmp(pElem, TAG_DESC) == 0) {
          dupStr(m_pStr, &m_pItem->pDesc);
          SAFE_DELETE(m_pStr);
        }
        else if (strcmp(pElem, TAG_LINK) == 0) {
          dupStr(m_pStr, &m_pItem->pLink);
          SAFE_DELETE(m_pStr);
        }
      }
    }

    void recvData(const XML_Char* pData, int dataSz) {
      if (dataSz > 0) {
        if (m_pStr != NULL) {
          char* p = new char[dataSz + 1];
          memcpy(p, pData, dataSz);
          p[dataSz] = '\0';
          *m_pStr += p;
         SAFE_DELETE(p);
        }
      }
    }
};

NewsReader::NewsReader(void) {
  m_pParser = NULL;
}

void NewsReader::read(const char* pTargetPage) {
  static const char* pRootCA =
    "-----BEGIN CERTIFICATE-----\n"                                      \
    "MIIDdzCCAl+gAwIBAgIBADANBgkqhkiG9w0BAQsFADBdMQswCQYDVQQGEwJKUDEl\n" \
    "MCMGA1UEChMcU0VDT00gVHJ1c3QgU3lzdGVtcyBDTy4sTFRELjEnMCUGA1UECxMe\n" \
    "U2VjdXJpdHkgQ29tbXVuaWNhdGlvbiBSb290Q0EyMB4XDTA5MDUyOTA1MDAzOVoX\n" \
    "DTI5MDUyOTA1MDAzOVowXTELMAkGA1UEBhMCSlAxJTAjBgNVBAoTHFNFQ09NIFRy\n" \
    "dXN0IFN5c3RlbXMgQ08uLExURC4xJzAlBgNVBAsTHlNlY3VyaXR5IENvbW11bmlj\n" \
    "YXRpb24gUm9vdENBMjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANAV\n" \
    "OVKxUrO6xVmCxF1SrjpDZYBLx/KWvNs2l9amZIyoXvDjChz335c9S672XewhtUGr\n" \
    "zbl+dp+++T42NKA7wfYxEUV0kz1XgMX5iZnK5atq1LXaQZAQwdbWQonCv/Q4EpVM\n" \
    "VAX3NuRFg3sUZdbcDE3R3n4MqzvEFb46VqZab3ZpUql6ucjrappdUtAtCms1FgkQ\n" \
    "hNBqyjoGADdH5H5XTz+L62e4iKrFvlNVspHEfbmwhRkGeC7bYRr6hfVKkaHnFtWO\n" \
    "ojnflLhwHyg/i/xAXmODPIMqGplrz95Zajv8bxbXH/1KEOtOghY6rCcMU/Gt1SSw\n" \
    "awNQwS08Ft1ENCcadfsCAwEAAaNCMEAwHQYDVR0OBBYEFAqFqXdlBZh8QIH4D5cs\n" \
    "OPEK7DzPMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3\n" \
    "DQEBCwUAA4IBAQBMOqNErLlFsceTfsgLCkLfZOoc7llsCLqJX2rKSpWeeo8HxdpF\n" \
    "coJxDjrSzG+ntKEju/Ykn8sX/oymzsLS28yN/HH8AynBbF0zX2S2ZTuJbxh2ePXc\n" \
    "okgfGT+Ok+vx+hfuzU7jBBJV1uXk3fs+BXziHV7Gp7yXT2g69ekuCkO2r1dcYmh8\n" \
    "t/2jioSgrGK+KwmHNPBqAbubKVY8/gA3zyNs8U6qtnRGEmyR7jTV7JqR50S+kDFy\n" \
    "1UkC9gLl9B/rfNmWVan/7Ir5mUf/NVoCqgTLiluHcSmRvaS0eg29mvVXIwAHIRc/\n" \
    "SjnRBUkLp7Y3gaVdjKozXoEofKd9J+sAro03\n"                             \
    "-----END CERTIFICATE-----\n";

  WiFiClientSecure client;
  client.setCACert(pRootCA);

  const char* pHost = "news.yahoo.co.jp";
  if (client.connect(pHost, 443)) {
    String strReq = "GET https://";
    strReq += pHost;
    strReq += pTargetPage;
    strReq += " HTTP/1.1\r\n";
    strReq += "Host: " + String(pHost) + "\r\n";
    strReq += "User-Agent: BuildFailureDetectorESP32\r\n";
    strReq += "Connection: close\r\n\r\n";
    strReq += "\0";

    client.print(strReq);
    client.flush();
  }

  if (client) {
    int readSz = 0;
    bool isStart = false;
    if(m_pParser != NULL) {
      m_pParser->clean();
      SAFE_DELETE(m_pParser);
    }
    m_pParser = new RssParser();
    m_pParser->begin();
    while (client.connected()) {
      while (client.available()) {
        String line = client.readStringUntil('\n');
        if (line.startsWith("<?xml")) {
          isStart = true;
        }
        if (isStart) {
          line.replace("&amp;", "&");
          line.replace("&#039;", "\'");
          line.replace("&#39;", "\'");
          line.replace("&apos;", "\'");
          line.replace("&quot;", "\"");
          m_pParser->put(line.c_str(), line.length());
        }
      }
    }
    m_pParser->end();
    client.stop();
  }
}

NewsReader::~NewsReader(void) {
  if(m_pParser != NULL) {
    m_pParser->clean();
    SAFE_DELETE(m_pParser);
  }
}

int NewsReader::getEntryNum(void) const {
  int num = 0;
  if(m_pParser != NULL) {
    const NewsEntries* pEntries = m_pParser->getEntries();
    if(pEntries != NULL) {
      num = pEntries->size();
    }
  }
  return num;
}

bool NewsReader::getEntry(int index, const char** ppTitle, const char** ppDesc, const char** ppLink) const {
  bool result = false;
  if(m_pParser != NULL) {
    const NewsEntries* pEntries = m_pParser->getEntries();
    if(pEntries != NULL) {
      const NewsEntries& entries = *pEntries;
      if(index < entries.size()) {
        if(ppTitle != NULL) {
          *ppTitle = entries[index]->pTitle;
        }
        if(ppDesc != NULL) {
          *ppDesc = entries[index]->pDesc;
        }
        if(ppLink != NULL) {
          *ppLink = entries[index]->pLink;
        }
        result = true;
      }
    }
  }
  return result;
}
}
