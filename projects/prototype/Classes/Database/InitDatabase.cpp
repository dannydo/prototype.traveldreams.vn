#include "InitDatabase.h"

USING_NS_CC; 

InitDatabase* InitDatabase::m_InitDatabase = NULL;

InitDatabase::InitDatabase()
: m_DatabaseSqlite(NULL)
{

}

InitDatabase::~InitDatabase()
{
	if (m_DatabaseSqlite)
	{
		sqlite3_close(m_DatabaseSqlite);
		m_DatabaseSqlite = NULL;
	}
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
	std::string path = FileUtils::sharedFileUtils()->getWritablePath() + "ohmyword.db3";

	std::string sql;  
	int iResult;  
	iResult = sqlite3_open(path.c_str(),&m_DatabaseSqlite);  
	if(iResult != SQLITE_OK)
	{
		sqlite3_close(m_DatabaseSqlite);
		m_DatabaseSqlite = NULL;
		return false;
	}

	createDatabase();
	
	return true;
}

bool InitDatabase::createDatabase()
{
	bool bCreateSuccess = true;
	int iResult;
	iResult = sqlite3_exec(m_DatabaseSqlite,"create table if not exists Users(UserIdentifier text primary key, UserId integer, FacebookId text, FacebookToken text, FirstName text, LastName text, CurrentChapter integer, CurrentLevel integer, Life integer, LifeTimeRemaining integer, LifeTimeBeginRemain integer)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
	{
		bCreateSuccess = false;
	}
	else 
	{
		std::string sql;
		sql="insert into Users values('vandao', 0, '', '', '', '', 1, 1, 5, 0, 0)";
		iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK)
			bCreateSuccess = false;
	}

	iResult = sqlite3_exec(m_DatabaseSqlite,"create table if not exists Chapters(Chapter integer primary key, Name text, MinLevel integer, MaxLevel integer, TotalStar integer, TotalScore integer, IsUnlock integer, IsUpdate integer)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		bCreateSuccess = false;

	iResult = sqlite3_exec(m_DatabaseSqlite,"create table if not exists Levels(Level integer primary key, Chapter integer, Star integer, Score integer, BonusQuest integer, IsUnlock integer, IsUpdate integer)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		bCreateSuccess = false;

	iResult = sqlite3_exec(m_DatabaseSqlite,"create table if not exists Friends(FriendId integer primary key, FacebookId text, FirstName text, LastName text, CurrentChapter integer, CurrentLevel integer)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		bCreateSuccess = false;

	return bCreateSuccess;
}