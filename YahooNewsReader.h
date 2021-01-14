#ifndef _YAHOO_NEWS_READER_H_
#define _YAHOO_NEWS_READER_H_

#include <vector>

namespace YahooNewsReader {
struct NewsEntry_t {
  const char* pTitle;
  const char* pDesc;
  const char* pLink;
};

typedef std::vector<NewsEntry_t*> NewsEntries;

class NewsReader {
  public:
    NewsReader(void);
    virtual ~NewsReader(void);
    void read(const char* pTargetPage);
    int getEntryNum(void) const;
    bool getEntry(int index, const char** ppTitle, const char** ppDesc, const char** ppLink) const;

  private:
    class RssParser* m_pParser;
};
}
#endif //_YAHOO_NEWS_READER_H_
