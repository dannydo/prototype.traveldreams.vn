#include "InitDatabase.h"
#include "VersionTable.h"

USING_NS_CC; 

InitDatabase* InitDatabase::m_InitDatabase = NULL;

InitDatabase::InitDatabase()
: m_DatabaseSqlite(NULL)
{

}

void InitDatabase::releaseInstance()
{
	if (m_InitDatabase == NULL)
	{
		delete m_InitDatabase;
	}

	sqlite3_close(m_InitDatabase->m_DatabaseSqlite);
	m_InitDatabase->m_DatabaseSqlite = NULL;
	m_InitDatabase = NULL;	
}

InitDatabase* InitDatabase::getInstance()
{
	if (m_InitDatabase == NULL) {
		m_InitDatabase = new InitDatabase();
		m_InitDatabase->init();
	}

	return m_InitDatabase;
}

bool InitDatabase::init()
{
	m_spath = FileUtils::sharedFileUtils()->getWritablePath() + "ohmyword.db3";
	
	if(!FileUtils::getInstance()->isFileExist(m_spath))
	{
		UserDefault::getInstance()->setIntegerForKey("InitDatabase", 0);
	}
	else if (UserDefault::getInstance()->getIntegerForKey("InitDatabase", 0) == 0)
	{
		remove(m_spath.c_str());
	}
 
	int iResult;  
	iResult = sqlite3_open(m_spath.c_str(),&m_DatabaseSqlite);  
	if(iResult != SQLITE_OK)
	{
		sqlite3_close(m_DatabaseSqlite);
		m_DatabaseSqlite = NULL;
		remove(m_spath.c_str());
		return false;
	}

	if (UserDefault::getInstance()->getIntegerForKey("InitDatabase", 0) == 0)
	{
		if (!createDatabase())
		{
			sqlite3_close(m_DatabaseSqlite);
			m_DatabaseSqlite = NULL;
			remove(m_spath.c_str());
			return false;
		}
	}

	return true;
}

bool InitDatabase::createDatabase()
{
	bool bCreateSuccess = true;
	int iResult;
	
	iResult = sqlite3_exec(m_DatabaseSqlite,"CREATE TABLE if not exists Users (UserIdentifier TEXT PRIMARY KEY  NOT NULL, FacebookId TEXT, FacebookToken TEXT, FirstName TEXT, LastName TEXT, CurrentChapter TEXT, CurrentLevel INTEGER, Life INTEGER, LifeTimeRemaining INTEGER, LifeTImeBeginRemain INTEGER, Monney INTEGER, Version INTEGER)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		bCreateSuccess = false;

	iResult = sqlite3_exec(m_DatabaseSqlite,"CREATE TABLE if not exists Chapters (ChapterId TEXT PRIMARY KEY NOT NULL, TotalLevelUnlock INTEGER, TotalStar INTEGER, IsUnlock INTEGER NOT NULL DEFAULT 0, Version INTEGER)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		bCreateSuccess = false;
	
	iResult = sqlite3_exec(m_DatabaseSqlite,"CREATE TABLE if not exists Levels (LevelId INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, ChapterId TEXT, Level INTEGER, WordId TEXT, Star INTEGER, Score INTEGER, BonusQuest INTEGER, TotalBonusQuest INTEGER, IsUnlock INTEGER NOT NULL DEFAULT 0, Version INTEGER)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		bCreateSuccess = false;

	iResult = sqlite3_exec(m_DatabaseSqlite,"CREATE TABLE if not exists UnlockChapters (UnlockChapterId INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, ChapterId TEXT, Request INTEGER, Type TEXT, BeginTime INTEGER, Version INTEGER)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		bCreateSuccess = false;

	iResult = sqlite3_exec(m_DatabaseSqlite,"CREATE TABLE if not exists PowerUps (PowerUpId TEXT PRIMARY KEY NOT NULL, Quantity INTEGER, Version INTEGER)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		bCreateSuccess = false;

	iResult = sqlite3_exec(m_DatabaseSqlite,"CREATE TABLE if not exists Transactions (TransactionsId INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, PowerUpId TEXT, Quantity INTEGER, TotalAmount INTEGER, DateTime INTEGER, Version INTEGER)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		bCreateSuccess = false;

	iResult = sqlite3_exec(m_DatabaseSqlite,"CREATE TABLE if not exists Versions (VersionId INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, VersionSync INTEGER, VersionDatabase INTEGER)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		bCreateSuccess = false;

	std::string sql;
	sql="insert into Versions values(1, 1, 1)";
	iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
		bCreateSuccess = false;

	sql="insert into Users values('ohmyword', '', '', '', '', '', 1, 5, 0, 0, 0, 2)";
	iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
		bCreateSuccess = false;

	return bCreateSuccess;
}

bool InitDatabase::createDataChapterAndLevel(const std::string& sChapterId, const int& iTotalLevel)
{
	char **re;
	int nRow, nColumn;

	String sql = "select * from Chapters where ChapterId=";
	sql.appendWithFormat("'%s'", sChapterId.c_str());
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	bool bCreateSuccess = true;
	if(nRow < 1)
	{
		int iVersion = VersionTable::getInstance()->getVersionInfo().iVersionId + 1;
		char sVersion[10];
		sprintf(sVersion, "%d", iVersion);

		std::string sql;
		sql="insert into Chapters values('";
		sql.append(sChapterId.c_str());
		sql.append("', 0, 0, 1, ");
		sql.append(sVersion);
		sql.append(")");

		int iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
			bCreateSuccess = false;

		for (int iIndex=0; iIndex<iTotalLevel; iIndex++)
		{
			String sql = "insert into Levels (ChapterId,Level,WordId,Star,Score,BonusQuest,TotalBonusQuest,IsUnlock,Version) values(";
			sql.appendWithFormat("'%s',", sChapterId.c_str());
			sql.appendWithFormat("%d,", iIndex+1);
			sql.append("'',0,0,0,0,0,");
			sql.appendWithFormat("%d)", iVersion);

			iResult = sqlite3_exec(m_DatabaseSqlite, sql.getCString(), NULL, NULL, NULL);
			if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
				bCreateSuccess = false;
		}
	}

	return bCreateSuccess;
}
