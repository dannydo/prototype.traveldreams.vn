#include "GameConfigManager.h"
#include "cocos2d.h"
#include "GameBoardManager.h"
#include "GameWordManager.h"

GameConfigManager* GameConfigManager::m_pInstance = NULL;

GameConfigManager::GameConfigManager()
{
}

GameConfigManager::~GameConfigManager()
{
	for(int i=1; i<=5; i++)
		for(auto pObstacle: m_LevelConfigList[i].m_ObstacleConfigList)
			delete pObstacle;
}
	
void GameConfigManager::LoadLevelsConfig()
{
	for(int i=1; i<=5; i++)
		LoadConfigOfLevel(i);
}

const LevelConfig& GameConfigManager::GetLevelConfig(int iLevel)
{
	return m_LevelConfigList[iLevel];	
}

void GameConfigManager::LoadConfigOfLevel(int iLevel)
{
	LevelConfig& levelConfig = m_LevelConfigList[iLevel];

	//if (iLevel == 5)
	//	iLevel = 5;
	char sFileName[20];
	sprintf(sFileName, "Levels/Level%d.data", iLevel);	

	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::getInstance()->getFileData(sFileName, "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	std::string sTemp;	

	// size map
	std::getline(inputStream, sTemp);		
	inputStream >> levelConfig.m_iRowNumber;
	inputStream >> levelConfig.m_iColumnNumber;
	std::getline(inputStream, sTemp);		

	// move count
	std::getline(inputStream, sTemp);				
	inputStream >> levelConfig.m_iNumberOfMove;
	std::getline(inputStream, sTemp);

	// color count
	std::getline(inputStream, sTemp);				
	inputStream >> levelConfig.m_iNumberOfColor;
	std::getline(inputStream, sTemp);

	// score of stars
	std::getline(inputStream, sTemp);				
	inputStream >> levelConfig.m_ScoreOfStars[0];
	inputStream >> levelConfig.m_ScoreOfStars[1];
	inputStream >> levelConfig.m_ScoreOfStars[2];
	std::getline(inputStream, sTemp);

	// map matrix
	std::getline(inputStream, sTemp);		
	int iRow, iColumn;
	for(iRow = levelConfig.m_iRowNumber-1; iRow >=0; iRow--)
		for(iColumn = 0; iColumn < levelConfig.m_iColumnNumber; iColumn++)
		{
			inputStream >> levelConfig.m_BoardMatrix[iRow][iColumn];
		}
	std::getline(inputStream, sTemp);		

	// main word
	std::getline(inputStream, sTemp);
	inputStream >> sTemp;
	levelConfig.m_iMainWordID = GameWordManager::getInstance()->GetWordIndexFromContent(sTemp);		
	
	int iTemp;
	inputStream >> iTemp;
	levelConfig.m_bIsMainWordExistedOnBoard = (iTemp > 0);
	if (levelConfig.m_bIsMainWordExistedOnBoard)
	{
		const Word& word = GameWordManager::getInstance()->GetWord(levelConfig.m_iMainWordID);
		for(int i=0; i< word.m_iWordLength; i++)
		{
			inputStream >> levelConfig.m_MainWordLetterPosition[i].m_iRow;
			levelConfig.m_MainWordLetterPosition[i].m_iRow--;
			inputStream >> levelConfig.m_MainWordLetterPosition[i].m_iColumn;
			levelConfig.m_MainWordLetterPosition[i].m_iColumn--;
		}
	}
	std::getline(inputStream, sTemp);

	// bonus word
	std::getline(inputStream, sTemp);
	inputStream >> levelConfig.m_iBonusWordsCount;
	for(int i=0; i< levelConfig.m_iBonusWordsCount; i++)
	{
		inputStream >> sTemp;
		levelConfig.m_BonusWordIDList[i] = GameWordManager::getInstance()->GetWordIndexFromContent(sTemp);
	}
	std::getline(inputStream, sTemp);

	// obstacle list
	int iObstacleCount;

	std::getline(inputStream, sTemp);
	inputStream >> iObstacleCount;
	for(int i=0; i< iObstacleCount; i++)
	{
		Level_ObstacleConfig* pObstacleConfig = new Level_ObstacleConfig();
		inputStream >> pObstacleConfig->m_iObstacleID;
		inputStream >> pObstacleConfig->m_iCount;
		for(int j=0; j < pObstacleConfig->m_iCount; j++)
		{
			inputStream >> pObstacleConfig->m_ObstaclePositionList[j].m_iRow;
			pObstacleConfig->m_ObstaclePositionList[j].m_iRow--;
			inputStream >> pObstacleConfig->m_ObstaclePositionList[j].m_iColumn;
			pObstacleConfig->m_ObstaclePositionList[j].m_iColumn--;
		}

		levelConfig.m_ObstacleConfigList.push_back( pObstacleConfig);
	}

	delete[] data;
	delete[] orginalData;
}


void GameConfigManager::LoadGameConfig()
{
	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::sharedFileUtils()->getFileData("GameConfig.txt", "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	std::string sComment;

	// read score of gem
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iScoreOfGem;
	std::getline( inputStream, sComment);

	// read combo config
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iComboCombineRatio;
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iCombEffectRatio4;
	inputStream >> m_GameConfig.m_iCombEffectRatio5;
	inputStream >> m_GameConfig.m_iCombEffectRatio6;
	inputStream >> m_GameConfig.m_iCombEffectRatio4_4;
	inputStream >> m_GameConfig.m_iCombEffectRatio4_5;
	inputStream >> m_GameConfig.m_iCombEffectRatio4_6;
	inputStream >> m_GameConfig.m_iCombEffectRatio5_5;
	inputStream >> m_GameConfig.m_iCombEffectRatio5_6;
	inputStream >> m_GameConfig.m_iCombEffectRatio6_6;
	std::getline( inputStream, sComment);

	// read word config
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iScoreOfMainWord;
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iMainWordScoreRatio;
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iScoreOfSubWord;
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iSubWordScoreRatio;	

	delete[] data;
	delete[] orginalData;
}