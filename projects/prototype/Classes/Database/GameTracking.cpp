#include "GameTracking.h"
#include "GameDataStructure.h"

USING_NS_CC;

void GameTracking::saveFileTrackingLevel(const int& ilevel, const int& iMove, const int& iLetterUnlock, const char* sStatusGame)
{
#if (_TURN_ON_WIRTE_FILE_TRACKING == 1)
	std::string sPath = FileUtils::sharedFileUtils()->getWritablePath() + "tracking.txt";
	
	std::string sTitile = "";
	if(!FileUtils::getInstance()->isFileExist(sPath))
	{
		sTitile = "level\tmove\tletter unlock\tstatus game";
	}

	FILE *fp = fopen(sPath.c_str(), "a+");
	if (!fp)
	{
		CCLOG("can not create file %s", sPath.c_str());
		return;
	}
	
	if (sTitile != "")
	{
		fprintf(fp, "%s\n", sTitile.c_str());
	}

	fprintf(fp, "%d\t%d\t%d\t%s\n", ilevel, iMove, iLetterUnlock, sStatusGame);

	fclose(fp);
#endif
}