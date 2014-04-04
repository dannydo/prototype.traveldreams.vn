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

	String sql = "select Words .* from Words inner join MapChapterWords on MapChapterWords.WordId  = Words.WordId where MapChapterWords.ChapterId=";
	sql.appendWithFormat("'%s'", sChapterId.c_str());

	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		WordInfo wordInfo;
		wordInfo.sWordId = int(strtod(re[iRow*nColumn+0], NULL));
		wordInfo.iCountCollected = int(strtod(re[iRow*nColumn+1], NULL));
		wordInfo.iVersion = int(strtod(re[iRow*nColumn+2], NULL));

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

bool WordTable::updateWord(WordInfo wordInfo)
{
	String sql = "update Levels Set";
	sql.appendWithFormat(" CountCollected=%d,", wordInfo.iCountCollected);
	sql.appendWithFormat(" Version=%d", VersionTable::getInstance()->getVersionInfo().iVersionId + 1);
	sql.appendWithFormat(" where WordId='%s'", wordInfo.sWordId);

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	return true;
}