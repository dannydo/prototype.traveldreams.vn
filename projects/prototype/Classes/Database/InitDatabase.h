#ifndef __INIT_DATABASE_H__
#define __INIT_DATABASE_H__

#include "cocos2d.h"
#include "sqlite3.h"

class InitDatabase
{
public:
	static InitDatabase* getInstance();
	InitDatabase();
	bool init();
	static void releaseInstance();

	bool createDataChapterAndLevel(const std::string& sChapterId, std::vector<std::string>& wordList, std::vector<int>& mapLevels);

	inline sqlite3* getDatabseSqlite() { return m_DatabaseSqlite; };

private:
	bool createDatabase();

	static InitDatabase* m_InitDatabase;
	sqlite3* m_DatabaseSqlite;
	std::string m_spath;
};
#endif