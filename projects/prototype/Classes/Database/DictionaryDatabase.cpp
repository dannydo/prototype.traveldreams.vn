#include "DictionaryDatabase.h"
#include "GameBoardManager.h"
#include "sqlite3.h"

USING_NS_CC; 

DictionaryDatabase* DictionaryDatabase::m_DictionaryDatabase = NULL;

DictionaryDatabase::DictionaryDatabase()
: m_DatabaseSqlite(NULL)
{

}

void DictionaryDatabase::releaseInstance()
{
	if (m_DictionaryDatabase == NULL)
	{
		delete m_DictionaryDatabase;
	}

	sqlite3_close(m_DictionaryDatabase->m_DatabaseSqlite);
	m_DictionaryDatabase->m_DatabaseSqlite = NULL;
	m_DictionaryDatabase = NULL;	
}

DictionaryDatabase* DictionaryDatabase::getInstance()
{
	if (m_DictionaryDatabase == NULL) {
		m_DictionaryDatabase = new DictionaryDatabase();
		if (!m_DictionaryDatabase->init())
		{
			Director::getInstance()->end();
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				exit(0);
			#endif
		}
	}

	return m_DictionaryDatabase;
}

bool DictionaryDatabase::init()
{
	std::string sPath = FileUtils::sharedFileUtils()->getWritablePath() + "dictionary.db3";
	
	if(!FileUtils::getInstance()->isFileExist(sPath))
	{
		unsigned long iDataSize;
		unsigned char* orginalData = cocos2d::CCFileUtils::sharedFileUtils()->getFileData("Dictionary/dictionary.sqlite3", "r", &iDataSize);
		
		FILE *fp = fopen(sPath.c_str(), "wb");
		if (!fp)
		{
			CCLOG("can not create file %s", sPath.c_str());
			return false;
		}

		fwrite(orginalData, iDataSize, 1, fp);
		fclose(fp);

		CC_SAFE_DELETE_ARRAY(orginalData);
	}

	std::string sql;  
	int iResult;  
	iResult = sqlite3_open(sPath.c_str(),&m_DatabaseSqlite);  
	if(iResult != SQLITE_OK)
	{
		sqlite3_close(m_DatabaseSqlite);
		m_DatabaseSqlite = NULL;
		return false;
	}
	
	return true;
}

WordDictionaryInfo DictionaryDatabase::fectMeanWord(const char* sWord)
{	
	std::string sLovercase = "";
	sLovercase.append(sWord);
	std::transform(sLovercase.begin(), sLovercase.end(), sLovercase.begin(), ::tolower);

	char **re;
	int nRow, nColumn;

	String sql = "select * from Words where Word=";
	sql.appendWithFormat("'%s'", sLovercase.c_str());
	CCLOG("%s", sql.getCString());

	sqlite3_get_table(m_DatabaseSqlite, sql.getCString(), &re, &nRow, &nColumn,NULL);
	CCLOG("row is %d,column is %d", nRow, nColumn);

	WordDictionaryInfo wordDictInfo;
	if (nRow > 0)
	{
		wordDictInfo.sWord = re[nColumn+0];
		wordDictInfo.sPhonetic = re[nColumn+1];
		wordDictInfo.sMean = re[nColumn+2];
	}

	sqlite3_free_table(re);

	return wordDictInfo;
}