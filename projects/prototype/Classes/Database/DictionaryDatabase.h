#ifndef __DICTIONARY_DATABASE_H__
#define __DICTIONARY_DATABASE_H__

#include "cocos2d.h"
#include "sqlite3.h"

struct WordDictionaryInfo
{
	std::string sWord;
	std::string sPhonetic;
	std::string sMean;
};

class DictionaryDatabase
{
public:
	static DictionaryDatabase* getInstance();
	DictionaryDatabase();
	bool init();
	static void releaseInstance();
	WordDictionaryInfo fectMeanWord(const char* sWord);

private:
	bool createDatabase();

	static DictionaryDatabase* m_DictionaryDatabase;
	sqlite3* m_DatabaseSqlite;
};
#endif