#include "GameConfigManager.h"
#include "cocos2d.h"
#include "GameBoardManager.h"
#include "GameWordManager.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

GameConfigManager* GameConfigManager::m_pInstance = NULL;

GameConfigManager::GameConfigManager()
{
}

GameConfigManager::~GameConfigManager()
{
	for(int i=1; i<=8; i++)
		for(auto pObstacle: m_LevelConfigList[i].m_ObstacleConfigList)
			delete pObstacle;

	for(auto pObstacleDescription : m_ObstacleDescriptionArray)
		delete pObstacleDescription;
}
	
void GameConfigManager::LoadLevelsConfig()
{
	for(int i=1; i<=8; i++)
		LoadConfigOfLevel(i);
}

LevelConfig& GameConfigManager::GetLevelConfig(int iLevel)
{
	return m_LevelConfigList[iLevel];	
}

const ObstacleLevelDescription& GameConfigManager::GetObstacleLevelDescription(const int& iObstacleTypeID, const int& iLevel)
{
	ObstacleDescription* pObstacleDescription = m_ObstacleDescriptionArray[iObstacleTypeID];
	if (iLevel> 0 && pObstacleDescription->m_LevelList.size() >= iLevel)
		return pObstacleDescription->m_LevelList[iLevel-1];
	else
		return pObstacleDescription->m_LevelList[0];
	//for(int i=0; i < pObstacleDescription->m_LevelList.size(); i++)
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
	std::getline(inputStream, sTemp);
	sTemp.erase( sTemp.size()-1, 1);  // remove \r at the end of line
	levelConfig.m_iMainWordID = GameWordManager::getInstance()->GetWordIndexFromContent(sTemp);		
	
	memset( levelConfig.m_MainWordUnlockedFlagList, 0, sizeof(levelConfig.m_MainWordUnlockedFlagList));
	int iAlreadyUnlockedLetterCount, iTemp;
	inputStream >> iAlreadyUnlockedLetterCount;
	for(int i=0; i< iAlreadyUnlockedLetterCount; i++)
	{
		inputStream >> iTemp; 
		levelConfig.m_MainWordUnlockedFlagList[iTemp-1] = true;
	}

	//int iTemp;
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
	std::getline(inputStream, sTemp);
	for(int i=0; i< levelConfig.m_iBonusWordsCount; i++)
	{
		//inputStream >> sTemp;
		std::getline(inputStream, sTemp);
		sTemp.erase( sTemp.size()-1, 1); // remove \r
		levelConfig.m_BonusWordIDList[i] = GameWordManager::getInstance()->GetWordIndexFromContent(sTemp);
	}
	//std::getline(inputStream, sTemp);

	// boss config
	std::getline(inputStream, sTemp);
	inputStream >> iTemp;
	levelConfig.m_bEnableBoss = (iTemp >0);
	if (levelConfig.m_bEnableBoss)
	{
		inputStream >> levelConfig.m_BossConfig.m_Position.m_iRow;
		levelConfig.m_BossConfig.m_Position.m_iRow--;
		inputStream >> levelConfig.m_BossConfig.m_Position.m_iColumn;
		levelConfig.m_BossConfig.m_Position.m_iColumn--;
		inputStream >> levelConfig.m_BossConfig.m_iHeight;
		inputStream >> levelConfig.m_BossConfig.m_iWidth;		
		inputStream >> levelConfig.m_BossConfig.m_HitPointPerLetter;
	}
	std::getline(inputStream, sTemp);

	// obstacle list
	int iObstacleCount;
	
	std::getline(inputStream, sTemp);
	inputStream >> iObstacleCount;
	for(int i=0; i< iObstacleCount; i++)
	{
		Level_ObstacleConfig* pObstacleConfig = new Level_ObstacleConfig();
		inputStream >> sTemp;
		pObstacleConfig->m_iObstacleID = m_ObstaleNameToIDMap[sTemp];
		inputStream >> pObstacleConfig->m_iObstacleLevel;
		inputStream >> pObstacleConfig->m_iCount;
		for(int j=0; j < pObstacleConfig->m_iCount; j++)
		{
			inputStream >> pObstacleConfig->m_ObstaclePositionList[j].m_iRow;
			pObstacleConfig->m_ObstaclePositionList[j].m_iRow--;
			inputStream >> pObstacleConfig->m_ObstaclePositionList[j].m_iColumn;
			pObstacleConfig->m_ObstaclePositionList[j].m_iColumn--;
		}

		inputStream >> iTemp;
		pObstacleConfig->m_bEnableGenerateByDrop = (iTemp >0);
		if (pObstacleConfig->m_bEnableGenerateByDrop)
		{
			inputStream >> iTemp;
			pObstacleConfig->m_bCanDropOnAllColumn = (iTemp > 0);
			if (pObstacleConfig->m_bCanDropOnAllColumn)
				inputStream >> pObstacleConfig->m_iDropOnAllColumnRate;
			else
			{
				int iColumnCount, iColumnIndex;
				inputStream >> iColumnCount;
				for(int j=0; j< iColumnCount; j++)
				{
					inputStream >> iColumnIndex;
					iColumnIndex--;
					inputStream >> iTemp;
					pObstacleConfig->m_DropOnColumnsRateList[iColumnIndex] = iTemp;
				}
			}
		}

		levelConfig.m_ObstacleConfigList.push_back( pObstacleConfig);
	}	
	std::getline(inputStream, sTemp);
	
	// bonus quest
	std::getline(inputStream, sTemp); //read comment line
	inputStream >> levelConfig.m_BonusQuestConfig.m_iBonusQuestCount;
	if (levelConfig.m_BonusQuestConfig.m_iBonusQuestCount > 0)
	{
		inputStream >> iTemp;
		levelConfig.m_BonusQuestConfig.m_bIsBonusGemAppearOnStartGame = (iTemp == 1);
		if (levelConfig.m_BonusQuestConfig.m_bIsBonusGemAppearOnStartGame)
		{
			Cell pos;
			for(int i=0; i< levelConfig.m_BonusQuestConfig.m_iBonusQuestCount; i++)
			{
				inputStream >> pos.m_iRow;
				inputStream >> pos.m_iColumn;
				levelConfig.m_BonusQuestConfig.m_PositionOfBonusGemAtStartGame[i] = Cell(pos.m_iRow-1, pos.m_iColumn-1);
			}
		}
		else
		{
			for(int i=0; i< levelConfig.m_BonusQuestConfig.m_iBonusQuestCount; i++)
				inputStream >> levelConfig.m_BonusQuestConfig.m_BonusGemAppearAtMoves[i];
		}
		memset( levelConfig.m_BonusQuestConfig.m_IsBonusEnabledQuestFlags, 0, sizeof(levelConfig.m_BonusQuestConfig.m_IsBonusEnabledQuestFlags));
		// read detail quest description
		for(int i=0; i< levelConfig.m_BonusQuestConfig.m_iBonusQuestCount; i++)
		{
			inputStream >> iTemp;
			iTemp--;
			switch (iTemp)
			{
				case _BQT_COLLECT_GEM_:	//collect gem quest
				{
					levelConfig.m_BonusQuestConfig.m_IsBonusEnabledQuestFlags[iTemp] = true;				
					for(int j=0; j < _MAX_GEM_ID_; j++)
						inputStream >> levelConfig.m_BonusQuestConfig.m_CollectGemQuest.m_CountPerGemType[j];
					break;
				}
				case _BQT_COLLECT_COMBO_:	//collect combo quest
				{
					levelConfig.m_BonusQuestConfig.m_IsBonusEnabledQuestFlags[iTemp] = true;
					for(int j=0; j < _GCT_SINGLE_COMBO_COUNT_; j++)
						inputStream >> levelConfig.m_BonusQuestConfig.m_CollectComboQuest.m_CountPerComboGem[j];
					break;
				}
			}
		}
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
	//std::getline( inputStream, sComment);	
	//inputStream >> m_GameConfig.m_iComboCombineRatio;

	// destroy cell ratio
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	//simple combo 4,5,6
	inputStream >> m_GameConfig.m_iCombEffectDestroyCellRatio4;
	inputStream >> m_GameConfig.m_iCombEffectDestroyCellRatio5;
	inputStream >> m_GameConfig.m_iCombEffectDestroyCellRatio6;
	std::getline( inputStream, sComment);
	
	//double combo: 4-4, 4-5, 5-5, 6-4, 6-5, 6-6
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iCombEffectDestroyCellRatio4_4;
	inputStream >> m_GameConfig.m_iCombEffectDestroyCellRatio4_5;
	inputStream >> m_GameConfig.m_iCombEffectDestroyCellRatio5_5;
	inputStream >> m_GameConfig.m_iCombEffectDestroyCellRatio6_4;
	inputStream >> m_GameConfig.m_iCombEffectDestroyCellRatio6_5;
	inputStream >> m_GameConfig.m_iCombEffectDestroyCellRatio6_6;
	std::getline( inputStream, sComment);

	//tripple combo: 4-4-4, 5-5-5, 6-6-6
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iCombEffectDestroyCellRatio4_4_4;
	inputStream >> m_GameConfig.m_iCombEffectDestroyCellRatio5_5_5;
	inputStream >> m_GameConfig.m_iCombEffectDestroyCellRatio6_6_6;
	std::getline( inputStream, sComment);


	// *********** bonus score when create combo
	//simple combo 4,5,6
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iBonusScoreCreateComboCreate4;
	inputStream >> m_GameConfig.m_iBonusScoreCreateComboCreate5;
	inputStream >> m_GameConfig.m_iBonusScoreCreateComboCreate6;
	std::getline( inputStream, sComment);

	
	// *********** bonus score when activate combo
	//simple combo 4,5,6
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iBonusScoreActivateCombo4;
	inputStream >> m_GameConfig.m_iBonusScoreActivateCombo5;
	inputStream >> m_GameConfig.m_iBonusScoreActivateCombo6;
	std::getline( inputStream, sComment);

	//double combo: 4-4, 4-5, 5-5, 6-4, 6-5, 6-6
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iBonusScoreActivateCombo4_4;
	inputStream >> m_GameConfig.m_iBonusScoreActivateCombo4_5;
	inputStream >> m_GameConfig.m_iBonusScoreActivateCombo5_5;
	inputStream >> m_GameConfig.m_iBonusScoreActivateCombo6_4;
	inputStream >> m_GameConfig.m_iBonusScoreActivateCombo6_5;
	inputStream >> m_GameConfig.m_iBonusScoreActivateCombo6_6;
	std::getline( inputStream, sComment);

	//tripple combo: 4-4-4, 5-5-5, 6-6-6
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iBonusScoreActivateCombo4_4_4;
	inputStream >> m_GameConfig.m_iBonusScoreActivateCombo5_5_5;
	inputStream >> m_GameConfig.m_iBonusScoreActivateCombo6_6_6;
	std::getline( inputStream, sComment);

	// read word config
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iScoreLetterOfMainWord;
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iScoreRatioCompleteMainWord;
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iScoreLetterOfBonusWord;
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iScoreRatioCompleteBonusWord;	

	delete[] data;
	delete[] orginalData;	
}

void GameConfigManager::LoadObstacleConfig()
{	
	const char* sFileName = "ObstacleConfig.data";
	const char *des = NULL;
	std::string jsonpath;
    cs::JsonDictionary *pJsonDict = NULL;
    jsonpath = FileUtils::getInstance()->fullPathForFilename(sFileName);

	unsigned long size = 0;
    des = (char*)(FileUtils::getInstance()->getFileData(jsonpath.c_str(),"r" , &size));
	if(NULL == des || strcmp(des, "") == 0)
	{
		printf("read json file[%s] error!\n", sFileName);
		return;
	}

	std::string strDes(des);
    pJsonDict = new cs::JsonDictionary();
    pJsonDict->initWithDescription(strDes.c_str());


	// start to get data from dictionary
	int iObstacleCount = pJsonDict->getArrayItemCount("obstacleList");
	for(int i=0; i < iObstacleCount; i++)
	{
		const char* sObstacleName = pJsonDict->getStringValueFromArray("obstacleList", i);
		assert(sObstacleName != NULL);
		m_ObstaleNameToIDMap[sObstacleName] = i;
		
		m_ObstacleDescriptionArray.push_back(NULL);
	}

	// read obstacle detail
	string sObstacleName;
	cs::JsonDictionary* pObstacleDetailDict;
	cs::JsonDictionary* pObstacleLevellDict;

	for(int i=0; i < iObstacleCount; i++)
	{
		ObstacleDescription* pObstacleDescription = new ObstacleDescription();
		pObstacleDetailDict = pJsonDict->getSubItemFromArray("obstacles", i);
		
		sObstacleName = pObstacleDetailDict->getItemStringValue("name");
		pObstacleDescription->m_iObstacleID = m_ObstaleNameToIDMap[sObstacleName];
		pObstacleDescription->m_bIsWrapGem = pObstacleDetailDict->getItemBoolvalue("wrapGame", true);

		// obstacles that has ability to infect other cells
		pObstacleDescription->m_bIsInfectable = pObstacleDetailDict->getItemBoolvalue("infectable", false);
		////** pObstacleDescription->infactableObjects = pJsonDict->getArrayItemCount("infactableObjects", true);

		pObstacleDescription->m_bCanBeMoveThrough = pObstacleDetailDict->getItemBoolvalue("canBeMoveThrough", false);
		pObstacleDescription->m_bDropable = pObstacleDetailDict->getItemBoolvalue("dropable", true);
		pObstacleDescription->m_bLockRow = pObstacleDetailDict->getItemBoolvalue("lockRow", false);
		pObstacleDescription->m_bLockColumn = pObstacleDetailDict->getItemBoolvalue("lockColumn", false);

		int iLevelCount = pObstacleDetailDict->getArrayItemCount("levelList");
		for(int iLevel=0; iLevel < iLevelCount; iLevel++)
		{
			pObstacleLevellDict = pObstacleDetailDict->getSubItemFromArray( "levelList", iLevel);

			ObstacleLevelDescription levelObstacle;
			levelObstacle.m_iLevelID = pObstacleLevellDict->getItemIntValue("level", 0);
			levelObstacle.m_sSpriteFileName = pObstacleLevellDict->getItemStringValue("spriteFileName");

			pObstacleDescription->m_LevelList.push_back(levelObstacle);

			CC_SAFE_DELETE(pObstacleLevellDict);
		}

		pObstacleDescription->m_bDecreaseLevelAfterDestroyed = pObstacleDetailDict->getItemBoolvalue("decreaseLevelAfterDestroyed", true);
		pObstacleDescription->m_bDecreaseLevelAfterMoved = pObstacleDetailDict->getItemBoolvalue("decreaseLevelAfterMoved", true);
		
		string sTargetTransformName = pObstacleDetailDict->getItemStringValue("transformToObjectAtLevel0");
		if (sTargetTransformName.size() > 0)
			pObstacleDescription->m_TransformToObjectAtLevel0 = m_ObstaleNameToIDMap[sTargetTransformName];
		else
			pObstacleDescription->m_TransformToObjectAtLevel0 = -1;
		pObstacleDescription->m_iLevelOfTransformedObject = pObstacleDetailDict->getItemIntValue("levelOfTransformedObject", 0);
		
		pObstacleDescription->m_bIsDestroyable = pObstacleDetailDict->getItemBoolvalue("destroyable", true);
		pObstacleDescription->m_bMatchAroundToDestroyed = pObstacleDetailDict->getItemBoolvalue("matchAroundToDestroy", true);
		
		// how to appear
		pObstacleDescription->m_bAppearByDrop = pObstacleDetailDict->getItemBoolvalue("appearByDrop", true);
		//pObstacleDescription->m_iDropRate = pObstacleDetailDict->getItemIntValue("dropRate", 0);
		pObstacleDescription->m_bAppearByTransform = pObstacleDetailDict->getItemBoolvalue("appearByTransform", true);
		pObstacleDescription->m_iTransformRate = pObstacleDetailDict->getItemIntValue("transfromRate", 0);

		m_ObstacleDescriptionArray[ pObstacleDescription->m_iObstacleID] = pObstacleDescription;

		CC_SAFE_DELETE(pObstacleDetailDict);
	}

	// read priority list
	//for(int i=0; i< pJsonDict->getArrayItemCount(
	/*
	"priorityList":
        [            
            "SHIELD",
            "LOCK"            
        ],*/

	// read compatible list
	
	CC_SAFE_DELETE(pJsonDict);
    CC_SAFE_DELETE_ARRAY(des);
}