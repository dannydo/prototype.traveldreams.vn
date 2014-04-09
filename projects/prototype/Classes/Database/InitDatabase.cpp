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

	std::string sqlRun = "";
	sqlRun.append("CREATE TABLE if not exists Users (UserIdentifier TEXT PRIMARY KEY  NOT NULL, FacebookId TEXT, FacebookToken TEXT, FirstName TEXT, LastName TEXT, CurrentChapter TEXT, CurrentLevel INTEGER, Life INTEGER, LifeTimeRemaining INTEGER, LifeTImeBeginRemain INTEGER, Monney INTEGER, Version INTEGER);");
	sqlRun.append("CREATE TABLE if not exists Chapters (ChapterId TEXT PRIMARY KEY NOT NULL, TotalLevelUnlock INTEGER, TotalStar INTEGER, IsUnlock INTEGER NOT NULL DEFAULT 0, Version INTEGER, TotalFlashCardUnlock INTEGER, CountFlashCardNew INTEGER, TotalFlashCard INTEGER);");
	sqlRun.append("CREATE TABLE if not exists Levels (LevelId INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, ChapterId TEXT, Level INTEGER, WordId TEXT, Star INTEGER, Score INTEGER, BonusQuest INTEGER, TotalBonusQuest INTEGER, IsUnlock INTEGER NOT NULL DEFAULT 0, Version INTEGER);");
	sqlRun.append("CREATE TABLE if not exists UnlockChapters (UnlockChapterId INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, ChapterId TEXT, Request INTEGER, Type TEXT, BeginTime INTEGER, Version INTEGER);");
	sqlRun.append("CREATE TABLE if not exists PowerUps (PowerUpId TEXT PRIMARY KEY NOT NULL, Quantity INTEGER, Version INTEGER);");
	sqlRun.append("CREATE TABLE if not exists Transactions (TransactionsId INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, PowerUpId TEXT, Quantity INTEGER, TotalAmount INTEGER, DateTime INTEGER, Version INTEGER, Type TEXT);");
	sqlRun.append("CREATE TABLE if not exists Versions (VersionId INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, VersionSync INTEGER, VersionDatabase INTEGER);");
	sqlRun.append("CREATE TABLE if not exists Words (WordId TEXT PRIMARY KEY NOT NULL, CountCollected INTEGER, Version INTEGER);");
	sqlRun.append("CREATE TABLE if not exists MapChapterWords (MapChapterWordId INTEGER PRIMARY KEY NOT NULL, ChapterId TEXT, WordId TEXT, Version INTEGER, OrderUnlock INTEGER, IsNew INTEGER);");
	sqlRun.append("insert into Versions values(1, 1, 1);");
	sqlRun.append("insert into Users values('ohmyword', '', '', '', '', '', 1, 5, 0, 0, 0, 2);");

	iResult = sqlite3_exec(m_DatabaseSqlite, sqlRun.c_str(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
		bCreateSuccess = false;

	return bCreateSuccess;
}

bool InitDatabase::createDataChapterAndLevel(const std::string& sChapterId, std::vector<std::string>& wordList, std::vector<int>& mapLevels)
{
	bool bCreateSuccess = true;

	if(wordList.size() > 0)
	{
		char **re;
		int nRow, nColumn;

		String sql = "select * from Chapters where ChapterId=";
		sql.appendWithFormat("'%s'", sChapterId.c_str());
		sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

		if(nRow < 1)
		{
			String sqlRun = "";
			int iVersion = VersionTable::getInstance()->getVersionInfo().iVersionId + 1;
			char sVersion[10];
			sprintf(sVersion, "%d", iVersion);

			sqlRun.append("insert into Chapters values('");
			sqlRun.append(sChapterId.c_str());
			sqlRun.append("', 0, 0, 1, ");
			sqlRun.appendWithFormat("%d,", iVersion);
			sqlRun.append("0, 0,");
			sqlRun.appendWithFormat("%d);", wordList.size());

			int countCollected;
			for (int iIndex=0; iIndex<wordList.size(); iIndex++)
			{
				countCollected = 0;

				sqlRun.append("insert into MapChapterWords (ChapterId,WordId,Version,OrderUnlock,IsNew) values(");
				sqlRun.appendWithFormat("'%s',", sChapterId.c_str());
				sqlRun.appendWithFormat("'%s',", wordList[iIndex].c_str());
				sqlRun.appendWithFormat("%d,", iVersion);
				sqlRun.append("0, 0);");

				if (mapLevels[iIndex] != -1)
				{
					countCollected = 1;
					sqlRun.append("insert into Levels (ChapterId,Level,WordId,Star,Score,BonusQuest,TotalBonusQuest,IsUnlock,Version) values(");
					sqlRun.appendWithFormat("'%s',", sChapterId.c_str());
					sqlRun.appendWithFormat("%d,", mapLevels[iIndex]);
					sqlRun.appendWithFormat("'%s',", wordList[iIndex].c_str());
					sqlRun.append("0,0,0,0,0,");
					sqlRun.appendWithFormat("%d);", iVersion);
				}

				// Check WordId is exists on database
				String sql = "select * from Words where WordId=";
				sql.appendWithFormat("'%s'", wordList[iIndex].c_str());
				sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);
				if(nRow < 1)
				{
					sqlRun.append("insert into Words (WordId,CountCollected,Version) values(");
					sqlRun.appendWithFormat("'%s',", wordList[iIndex].c_str());
					sqlRun.appendWithFormat("%d,", countCollected);
					sqlRun.appendWithFormat("%d);", iVersion);
				}
			}

			int iResult = sqlite3_exec(m_DatabaseSqlite, sqlRun.getCString(), NULL, NULL, NULL);
			if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
				bCreateSuccess = false;
		}
	}

	return bCreateSuccess;
}
