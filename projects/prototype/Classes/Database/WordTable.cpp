#include "WordTable.h"
#include "InitDatabase.h"
#include "VersionTable.h"

USING_NS_CC; 

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
	return true;
}

void WordTable::fetchWordsForChapter(const std::string& sChapterId)
{
	while(!m_ChapterWords.empty())
	{
		m_ChapterWords.pop_back();
	}

	char **re;
	int nRow, nColumn;

	String sql = "select * from Words inner join MapChapterWords on MapChapterWords.WordId  = Words.WordId where MapChapterWords.ChapterId=";
	sql.appendWithFormat("'%s' order by OrderUnlock desc", sChapterId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		WordInfo wordInfo;
		wordInfo.sWordId = re[iRow*nColumn+0];
		wordInfo.iCountCollected = int(strtod(re[iRow*nColumn+1], NULL));
		wordInfo.iVersion = int(strtod(re[iRow*nColumn+2], NULL));
		wordInfo.iMapChapterWordId = int(strtod(re[iRow*nColumn+3], NULL));
		wordInfo.sChapterId = re[iRow*nColumn+4];
		wordInfo.iOrderUnlock = int(strtod(re[iRow*nColumn+7], NULL));
		wordInfo.bIsNew = bool(strtod(re[iRow*nColumn+8], NULL));

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

	return WordInfo::WordInfo();
}

void WordTable::refreshWordsForChapter(const std::string& sChapterId)
{
	this->fetchWordsForChapter(sChapterId);
}


bool WordTable::updateWord(const WordInfo& wordInfo)
{
	String sql = "update Words Set";
	sql.appendWithFormat(" CountCollected=%d,", wordInfo.iCountCollected);
	sql.appendWithFormat(" Version=%d", VersionTable::getInstance()->getVersionInfo().iVersionId + 1);
	sql.appendWithFormat(" where WordId='%s';", wordInfo.sWordId.c_str());

	sql.append("update MapChapterWords Set");
	sql.appendWithFormat(" IsNew=%d,", wordInfo.bIsNew);
	sql.appendWithFormat(" OrderUnlock=%d,", wordInfo.iOrderUnlock);
	sql.appendWithFormat(" Version=%d", VersionTable::getInstance()->getVersionInfo().iVersionId + 1);
	sql.appendWithFormat(" where MapChapterWordId=%d;", wordInfo.iMapChapterWordId);

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