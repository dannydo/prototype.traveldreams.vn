#include "SyncDataGame.h"
#include "Constants.h"
#include "Database\ChapterTable.h"
#include "Database\LevelTable.h"
#include "Database\PowerUpTable.h"
#include "Database\TransactionTable.h"
#include "Database\UnlockChapterTable.h"
#include "Database\UserTable.h"
#include "Database\VersionTable.h"
#include "Database\WordTable.h"
#include "Database\InitDatabase.h"

USING_NS_CC; 

SyncDataGame::SyncDataGame()
{
	
}

SyncDataGame::~SyncDataGame()
{

}

bool SyncDataGame::syncRun()
{
	String sJsonData = "{\"sync\":{";

	std::string sJsonUser = UserTable::getInstance()->syncGetUser();
	sJsonData.append(sJsonUser);
	sJsonData.append(",");

	std::string sJsonChapters = ChapterTable::getInstance()->syncGetChapters();
	sJsonData.append(sJsonChapters);
	sJsonData.append(",");

	std::string sJsonLevels = LevelTable::getInstance()->syncGetLevels();
	sJsonData.append(sJsonLevels);
	sJsonData.append(",");

	std::string sJsonWords = WordTable::getInstance()->syncGetWords();
	sJsonData.append(sJsonWords);
	sJsonData.append(",");

	std::string sJsonMapChapterWords = WordTable::getInstance()->syncGetMapChapterWords();
	sJsonData.append(sJsonMapChapterWords);
	sJsonData.append(",");

	/*
	std::string sJsonPowerUps = PowerUpTable::getInstance()->syncGetPoverUps();
	sJsonData.append(sJsonPowerUps);
	sJsonData.append(",");

	std::string sJsonTransactions = TransactionTable::getInstance()->syncGetTransactions();
	sJsonData.append(sJsonTransactions);
	sJsonData.append(",");

	std::string sJsonUnlockChapters = UnlockChapterTable::getInstance()->syncGetUnlockChapters();
	sJsonData.append(sJsonUnlockChapters);
	sJsonData.append(",");
	*/

	sJsonData.appendWithFormat("\"Version\":%d", VersionTable::getInstance()->getVersionInfo().iVersionSync);
	sJsonData.append("}}");

	//CCLOG("%s", sJsonData.getCString());
	//CCLOG("%d", sJsonData.length());
	return true;
}