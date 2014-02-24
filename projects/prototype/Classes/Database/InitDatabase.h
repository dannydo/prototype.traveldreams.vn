#ifndef __INIT_DATABASE_H__
#define __INIT_DATABASE_H__

#include "cocos2d.h"
#include "sqlite3.h"

class InitDatabase
{
public:
	static InitDatabase* getInstance();
	bool init();
	inline sqlite3* getDatabseSqlite() { return m_DatabaseSqlite; };

private:
	InitDatabase();
	virtual ~InitDatabase();
	bool createDatabase();

	static InitDatabase* m_InitDatabase;
	sqlite3* m_DatabaseSqlite;
};
#endif