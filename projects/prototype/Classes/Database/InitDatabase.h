#ifndef __INIT_DATABASE_H__
#define __INIT_DATABASE_H__

#include "cocos2d.h"
#include "sqlite3.h"
#include "UserTable.h"

class InitDatabase
{
public:
	static InitDatabase* getInstance();
	InitDatabase();
	bool init();
	static void releaseInstance();

	bool createDataChapterAndLevel(const std::string& sChapterId);
	bool initDataChapter1AndLevel(UserInfo& userInfo);

	inline sqlite3* getDatabseSqlite() { return m_DatabaseSqlite; };
	bool resetDatabase();

private:
	bool createDatabase();

	static InitDatabase* m_InitDatabase;
	sqlite3* m_DatabaseSqlite;
	std::string m_spath;
};
#endif