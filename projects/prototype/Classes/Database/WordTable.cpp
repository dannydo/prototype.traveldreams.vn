#include "WordTable.h"
#include "InitDatabase.h"
#include "VersionTable.h"
#include "ChapterTable.h"

USING_NS_CC; 
USING_NS_CC_EXT;

WordTable* WordTable::m_WordTable = NULL;

WordTable::WordTable()
{

}

void WordTable::releaseInstance()
{
	if (m_WordTable == NULL)
	{
		delete m_WordTable;
	}
	m_WordTable = NULL;	
}

WordTable* WordTable::getInstance()
{
	if (m_WordTable == NULL) {
		m_WordTable = new WordTable();
		m_WordTable->init();
	}

	return m_WordTable;
}

bool WordTable::init()
{
	m_sCurrentChapterId = "";
	m_iTotalFlashCardCollected = getNumberWordCollected();
	return true;
}

int WordTable::increaseTotalWordCollected(const int& iIncrease)
{
	m_iTotalFlashCardCollected += iIncrease;
	return m_iTotalFlashCardCollected;
}

unsigned long WordTable::getMinTimeNextPlayMiniGame()
{
	char **re;
	int nRow, nColumn;

	String sql = "select * from Words where IsCollected=0 and CountCollected > 1 and TimeBeginPlayMiniGame > 0 order by TimeBeginPlayMiniGame asc limit 1";
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	if (nRow > 0)
		return long(strtod(re[nColumn+4], 0));

	return 0;
}

int WordTable::getNumberWordNew()
{
	char **re;
	int nRow, nColumn;

	String sql = "select * from Words where IsCollected=0 and CountCollected > 1";
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	return nRow;
}

int WordTable::getNumberWordCollected()
{
	char **re;
	int nRow, nColumn;

	String sql = "select * from Words where IsCollected=1";
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	return nRow;
}

int WordTable::getNumberWordPlayMiniGame(const unsigned long& iTimeCurrent)
{
	char **re;
	int nRow, nColumn;

	unsigned long udeltaTime = iTimeCurrent - _SECONDS_NEXT_PLAY_MINI_GAME_; 

	String sql = "select * from Words where IsCollected=0 and CountCollected > 1 and (TimeBeginPlayMiniGame=0 or TimeBeginPlayMiniGame<=";
	sql.appendWithFormat("%u)", udeltaTime);
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	return nRow;
}

int WordTable::getTotalWordInfoCollectedForChapter(const std::string& sChapterId)
{
	char **re;
	int nRow, nColumn;

	String sql = "select * from Words inner join MapChapterWords on MapChapterWords.WordId = Words.WordId where IsCollected=1 and MapChapterWords.ChapterId=";
	sql.appendWithFormat("'%s'", sChapterId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	return nRow;
}

std::vector<WordInfo> WordTable::getWordInfoCollectedForChapter(const std::string& sChapterId)
{
	char **re;
	int nRow, nColumn;

	String sql = "select * from Words inner join MapChapterWords on MapChapterWords.WordId = Words.WordId where IsCollected=1 and MapChapterWords.ChapterId=";
	sql.appendWithFormat("'%s'", sChapterId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	std::vector<WordInfo> wordsCollected;
	for (int iRow=1; iRow<=nRow; iRow++)
	{
		WordInfo wordInfo;
		wordInfo.sWordId = re[iRow*nColumn+0];
		wordInfo.iCountCollected = int(strtod(re[iRow*nColumn+1], 0));
		wordInfo.iVersion = int(strtod(re[iRow*nColumn+2], 0));
		wordInfo.bIsCollected = int(strtod(re[iRow*nColumn+3], 0));
		wordInfo.uTimeBeginPlayMiniGame = long(strtod(re[iRow*nColumn+4], 0));
		wordInfo.iMapChapterWordId = int(strtod(re[iRow*nColumn+5], 0));
		wordInfo.sChapterId = re[iRow*nColumn+6];

		wordsCollected.push_back(wordInfo);
	}

	sqlite3_free_table(re);

	return wordsCollected;
}

std::vector<WordInfo> WordTable::getAllWordNew(const unsigned long& iTimeCurrent)
{
	char **re;
	int nRow, nColumn;

	unsigned long udeltaTime = iTimeCurrent - _SECONDS_NEXT_PLAY_MINI_GAME_; 

	String sql = "select * from Words inner join MapChapterWords on MapChapterWords.WordId = Words.WordId where IsCollected=0 and CountCollected > 1 and (TimeBeginPlayMiniGame=0 or TimeBeginPlayMiniGame<=";
	sql.appendWithFormat("%u)", udeltaTime);
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	std::vector<WordInfo> wordsNew;
	for (int iRow=1; iRow<=nRow; iRow++)
	{
		WordInfo wordInfo;
		wordInfo.sWordId = re[iRow*nColumn+0];
		wordInfo.iCountCollected = int(strtod(re[iRow*nColumn+1], 0));
		wordInfo.iVersion = int(strtod(re[iRow*nColumn+2], 0));
		wordInfo.bIsCollected = int(strtod(re[iRow*nColumn+3], 0));
		wordInfo.uTimeBeginPlayMiniGame = long(strtod(re[iRow*nColumn+4], 0));
		wordInfo.iMapChapterWordId = int(strtod(re[iRow*nColumn+5], 0));
		wordInfo.sChapterId = re[iRow*nColumn+6];

		wordsNew.push_back(wordInfo);
	}

	sqlite3_free_table(re);

	return wordsNew;
}

void WordTable::fetchWordsForChapter(const std::string& sChapterId)
{
	while(!m_ChapterWords.empty())
	{
		m_ChapterWords.pop_back();
	}

	char **re;
	int nRow, nColumn;

	String sql = "select * from Words inner join MapChapterWords on MapChapterWords.WordId = Words.WordId where MapChapterWords.ChapterId=";
	sql.appendWithFormat("'%s'", sChapterId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		WordInfo wordInfo;
		wordInfo.sWordId = re[iRow*nColumn+0];
		wordInfo.iCountCollected = int(strtod(re[iRow*nColumn+1], 0));
		wordInfo.iVersion = int(strtod(re[iRow*nColumn+2], 0));
		wordInfo.bIsCollected = int(strtod(re[iRow*nColumn+3], 0));
		wordInfo.uTimeBeginPlayMiniGame = long(strtod(re[iRow*nColumn+4], 0));
		wordInfo.iMapChapterWordId = int(strtod(re[iRow*nColumn+5], 0));
		wordInfo.sChapterId = re[iRow*nColumn+6];

		m_ChapterWords.push_back(wordInfo);
	}

	sqlite3_free_table(re);
}

std::vector<WordInfo>& WordTable::getAllWordsForChapter(const std::string& sChapterId)
{
	if(m_sCurrentChapterId != sChapterId)
	{
		this->fetchWordsForChapter(sChapterId);
	}

	return m_ChapterWords;
}

WordInfo WordTable::getWordInfoOnChapter(const std::string& sChapterId, const std::string& sWordId)
{	
	if(m_sCurrentChapterId != sChapterId)
	{
		this->fetchWordsForChapter(sChapterId);
		m_sCurrentChapterId = sChapterId;
	}

	for(int iIndex=0; iIndex<m_ChapterWords.size(); iIndex++)
	{
		if(m_ChapterWords[iIndex].sWordId == sWordId)
		{
			return m_ChapterWords[iIndex];
		}
	}

	return WordInfo();
}

void WordTable::refreshWordsForChapter(const std::string& sChapterId)
{
	this->fetchWordsForChapter(sChapterId);
}


bool WordTable::updateWord(const WordInfo& wordInfo)
{
	String sql = "update Words Set";
	sql.appendWithFormat(" CountCollected=%d,", wordInfo.iCountCollected);
	sql.appendWithFormat(" Version=%d,", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);
	sql.appendWithFormat(" IsCollected=%d,", wordInfo.bIsCollected);
	sql.appendWithFormat(" TimeBeginPlayMiniGame=%u", wordInfo.uTimeBeginPlayMiniGame);
	sql.appendWithFormat(" where WordId='%s';", wordInfo.sWordId.c_str());

	/*
	sql.append("update MapChapterWords Set");
	sql.appendWithFormat(" IsNew=%d,", wordInfo.bIsNew);
	sql.appendWithFormat(" OrderUnlock=%d,", wordInfo.iOrderUnlock);
	sql.appendWithFormat(" Version=%d", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);
	sql.appendWithFormat(" where MapChapterWordId=%d;", wordInfo.iMapChapterWordId);
	*/

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	for(int iIndex=0; iIndex<m_ChapterWords.size(); iIndex++)
	{
		if (m_ChapterWords[iIndex].sWordId == wordInfo.sWordId && m_ChapterWords[iIndex].sChapterId == wordInfo.sChapterId)
		{
			m_ChapterWords[iIndex] = wordInfo;
			break;
		}
	}

	return true;
}

bool WordTable::insertWord(const WordInfo& wordInfo)
{
	char **re;
	int nRow, nColumn;

	String sql = "select * from Words inner join MapChapterWords on MapChapterWords.WordId  = Words.WordId where MapChapterWords.ChapterId=";
	sql.appendWithFormat("'%s'", wordInfo.sChapterId.c_str());
	sql.appendWithFormat(" and MapChapterWords.WordId='%s'", wordInfo.sWordId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	if (nRow > 0)
		return true;

	// Insert Words
	String sqlRun="";
	sqlRun.append("insert into Words values(");
	sqlRun.appendWithFormat("'%s',", wordInfo.sWordId.c_str());
	sqlRun.appendWithFormat("%d,", wordInfo.iCountCollected);
	sqlRun.appendWithFormat("%d,", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);
	sqlRun.appendWithFormat("%d,", wordInfo.bIsCollected);
	sqlRun.appendWithFormat("%u);", wordInfo.uTimeBeginPlayMiniGame);

	// Insert Words
	sqlRun.append("insert into MapChapterWords (ChapterId,WordId,Version) values(");
	sqlRun.appendWithFormat("'%s',", wordInfo.sChapterId.c_str());
	sqlRun.appendWithFormat("'%s',", wordInfo.sWordId.c_str());
	sqlRun.appendWithFormat("%d);", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);

	int iResult = sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sqlRun.getCString(), &re, &nRow, &nColumn,NULL);
	if(iResult != SQLITE_OK)
		return false;

	ChapterInfo chapterInfo = ChapterTable::getInstance()->getChapterInfo(wordInfo.sChapterId.c_str());
	chapterInfo.iTotalFlashCard++;
	ChapterTable::getInstance()->updateChapter(chapterInfo);

	return true;
}

std::string	WordTable::syncGetWords()
{
	char **re;
	int nRow, nColumn;
		
	String sql = "select * from Words where Version>";
	sql.appendWithFormat("%d ", VersionTable::getInstance()->getVersionInfo().iVersionSync);
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	String sJsonData = "\"Words\":[";
	for (int iRow=1; iRow<=nRow; iRow++)
	{
		sJsonData.append("{");
		sJsonData.appendWithFormat("\"WordId\": \"%s\",", re[iRow*nColumn+0]);
		sJsonData.appendWithFormat("\"CountCollected\": %s,", re[iRow*nColumn+1]);
		sJsonData.appendWithFormat("\"Version\": %s,", re[iRow*nColumn+2]);
		sJsonData.appendWithFormat("\"IsCollected\": %s", re[iRow*nColumn+3]);
		//sJsonData.appendWithFormat("\"TimeBeginPlayMiniGame\": %s", re[iRow*nColumn+4]);

		if (iRow == nRow)
			sJsonData.append("}");
		else
			sJsonData.append("},");
	}
	sJsonData.append("]");
	sqlite3_free_table(re);

	return sJsonData.getCString();
}

std::string	WordTable::syncGetMapChapterWords()
{
	char **re;
	int nRow, nColumn;
		
	String sql = "select * from MapChapterWords where Version>";
	sql.appendWithFormat("%d ", VersionTable::getInstance()->getVersionInfo().iVersionSync);
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	String sJsonData = "\"MapChapterWords\":[";
	for (int iRow=1; iRow<=nRow; iRow++)
	{
		sJsonData.append("{");
		sJsonData.appendWithFormat("\"ChapterId\": \"%s\",", re[iRow*nColumn+1]);
		sJsonData.appendWithFormat("\"WordId\": \"%s\",", re[iRow*nColumn+2]);
		sJsonData.appendWithFormat("\"Version\": %s", re[iRow*nColumn+3]);

		if (iRow == nRow)
			sJsonData.append("}");
		else
			sJsonData.append("},");
	}
	sJsonData.append("]");
	sqlite3_free_table(re);

	return sJsonData.getCString();
}

bool WordTable::updateDataSyncWords(cs::JsonDictionary* pJsonSync, const int& iVersion)
{
	String sqlRun = "";
	std::vector<WordInfo> words;

	if (pJsonSync->getArrayItemCount("Words") > 0)
	{
		char **re;
		int nRow, nColumn;
		String sql = "select * from Words";
		sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

		for(int iRow=1; iRow<=nRow; iRow++)
		{
			WordInfo wordInfo;
			wordInfo.sWordId = re[iRow*nColumn+0];

			words.push_back(wordInfo);
		}

		sqlite3_free_table(re);
	}

	for(int iIndex=0; iIndex<pJsonSync->getArrayItemCount("Words"); iIndex++)
	{
		cs::JsonDictionary* pJsonWord = pJsonSync->getSubItemFromArray("Words", iIndex);
		bool isInsert = true;

		std::string sWordId = pJsonWord->getItemStringValue("WordId");
		for(int iIndexWord=0; iIndexWord<words.size(); iIndexWord++)
		{
			if (sWordId == words[iIndexWord].sWordId)
			{
				isInsert = false;
				break;
			}
		}

		if (isInsert)
		{
			// Insert Words
			sqlRun.append("insert into Words values(");
			sqlRun.appendWithFormat("'%s',", sWordId.c_str());
			sqlRun.appendWithFormat("%d,", pJsonWord->getItemIntValue("CountCollected", 0));
			sqlRun.appendWithFormat("%d,", iVersion);
			sqlRun.appendWithFormat("%d,", pJsonWord->getItemIntValue("IsCollected", 0));
			sqlRun.appendWithFormat("%d);", 0);
		}
		else
		{
			// Update Words
			sqlRun.append("update Words Set");
			sqlRun.appendWithFormat(" CountCollected=%d,", pJsonWord->getItemIntValue("CountCollected", 0));
			sqlRun.appendWithFormat(" Version=%d,", iVersion);
			sqlRun.appendWithFormat(" IsCollected=%d", pJsonWord->getItemIntValue("IsCollected", 0));
			sqlRun.appendWithFormat(" where WordId='%s';", sWordId.c_str());
		}
	}

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sqlRun.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	CCLOG("sync Word true");

	if (m_sCurrentChapterId != "")
		this->fetchWordsForChapter(m_sCurrentChapterId);
	return true;
}

bool WordTable::updateDataSyncMapChapterWords(cs::JsonDictionary* pJsonSync, const int& iVersion)
{
	String sqlRun = "";
	std::vector<WordInfo> mapChapterWords;

	if (pJsonSync->getArrayItemCount("MapChapterWords") > 0)
	{
		char **re;
		int nRow, nColumn;
		String sql = "select * from MapChapterWords";
		sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

		for(int iRow=1; iRow<=nRow; iRow++)
		{
			WordInfo wordInfo;
			wordInfo.sChapterId = re[iRow*nColumn+1];
			wordInfo.sWordId = re[iRow*nColumn+2];

			mapChapterWords.push_back(wordInfo);
		}

		sqlite3_free_table(re);
	}

	for(int iIndex=0; iIndex<pJsonSync->getArrayItemCount("MapChapterWords"); iIndex++)
	{
		cs::JsonDictionary* pJsonMapChapterWord = pJsonSync->getSubItemFromArray("MapChapterWords", iIndex);
		bool isInsert = true;

		std::string sChapterId = pJsonMapChapterWord->getItemStringValue("ChapterId");
		std::string sWordId = pJsonMapChapterWord->getItemStringValue("WordId");
		for(int iIndexMapChapterWord=0; iIndexMapChapterWord<mapChapterWords.size(); iIndexMapChapterWord++)
		{
			if (sChapterId == mapChapterWords[iIndexMapChapterWord].sChapterId && 
				sWordId == mapChapterWords[iIndexMapChapterWord].sWordId)
			{
				isInsert = false;
				break;
			}
		}

		if (isInsert)
		{
			// Insert Words
			sqlRun.append("insert into MapChapterWords (ChapterId,WordId,Version) values(");
			sqlRun.appendWithFormat("'%s',", sChapterId.c_str());
			sqlRun.appendWithFormat("'%s',", sWordId.c_str());
			sqlRun.appendWithFormat("%d);", iVersion);
		}
	}

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sqlRun.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	CCLOG("sync Map Chapter true");

	if (m_sCurrentChapterId != "")
		this->fetchWordsForChapter(m_sCurrentChapterId);
	return true;
}
