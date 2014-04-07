#include "GameConfigManager.h"
#include "cocos2d.h"
#include "GameBoardManager.h"
#include "GameWordManager.h"
#include "cocos-ext.h"
#include "Database\WordTable.h"
#include "Database\LevelTable.h"

USING_NS_CC;
USING_NS_CC_EXT;

GameConfigManager* GameConfigManager::m_pInstance = NULL;

GameConfigManager::GameConfigManager()
{
}

GameConfigManager::~GameConfigManager()
{
	/*for(int i=1; i<=_MAX_GAME_LEVEL_; i++)
		for(auto pObstacle: m_LevelConfigList[i].m_ObstacleConfigList)
			delete pObstacle;*/
	m_LevelConfig.Clear();
 
	for(auto pObstacleDescription : m_ObstacleDescriptionArray)
		delete pObstacleDescription;
}

LevelConfig& GameConfigManager::GetLevelConfig(const std::string& sChapterID, const int& iLevelId)
{
	if (iLevelId != m_iCurrentLevelID || sChapterID != m_sCurrentChapterID)
	{
		this->LoadConfigOfLevel(sChapterID, iLevelId);
		this->GetChapterConfig(sChapterID);

		m_sCurrentChapterID = sChapterID;
		m_iCurrentLevelID = iLevelId;
	}

	return m_LevelConfig;
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

void GameConfigManager::LoadConfigOfLevel(const std::string& sChapterID, const int& iLevelId)
{
	WordlMapConfig::WordMapChapterConfig wordMapChapterConfig = this->GetWordMapChapterConfig(sChapterID);

	std::string sPathFileData = wordMapChapterConfig.m_sPathData;
	char sFileName[25];
	sprintf(sFileName, "/Level%d.data", iLevelId);
	sPathFileData.append(sFileName);

	m_LevelConfig.Clear();
	LevelConfig& levelConfig = m_LevelConfig;

	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::getInstance()->getFileData(sPathFileData.c_str(), "r", &iDataSize);
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
	/*std::getline(inputStream, sTemp);
	sTemp.erase( sTemp.size()-1, 1);  // remove \r at the end of line
	//levelConfig.m_iMainWordID = GameWordManager::getInstance()->GetWordIndexFromContent(sTemp);		
	levelConfig.m_sMainWordID = sTemp;*/
	inputStream >> levelConfig.m_iNumberLetterOfMainWord;
	
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
		//int iMainWordIndex = GameWordManager::getInstance()->GetLoadedWordIndexFromID(levelConfig.m_sMainWordID);
		//const Word& word = GameWordManager::getInstance()->GetWord(iMainWordIndex);
		for(int i=0; i< levelConfig.m_iNumberLetterOfMainWord; i++) //word.m_iWordLength; i++)
		{
			inputStream >> levelConfig.m_MainWordLetterPosition[i].m_iRow;
			levelConfig.m_MainWordLetterPosition[i].m_iRow--;
			inputStream >> levelConfig.m_MainWordLetterPosition[i].m_iColumn;
			levelConfig.m_MainWordLetterPosition[i].m_iColumn--;
		}
	}
	std::getline(inputStream, sTemp);

	/*// bonus word
	std::getline(inputStream, sTemp);
	inputStream >> levelConfig.m_iBonusWordsCount;
	std::getline(inputStream, sTemp);
	for(int i=0; i< levelConfig.m_iBonusWordsCount; i++)
	{
		//inputStream >> sTemp;
		std::getline(inputStream, sTemp);
		sTemp.erase( sTemp.size()-1, 1); // remove \r
		levelConfig.m_BonusWordIDList[i] = GameWordManager::getInstance()->GetWordIndexFromContent(sTemp);
	}*/
	//std::getline(inputStream, sTemp);

	// display of main word
	std::getline(inputStream, sTemp);
	inputStream >> iTemp;
	levelConfig.m_bBreakLineWhenDisplayMainWord = (iTemp != 0);
	if (levelConfig.m_bBreakLineWhenDisplayMainWord)
		inputStream >> levelConfig.m_iLetterCountOfFirstLine; //note: include space
	std::getline(inputStream, sTemp);

	// config of dropping of main word
	if (!levelConfig.m_bIsMainWordExistedOnBoard)
	{
		// ratio
		std::getline(inputStream, sTemp);		
		inputStream >> levelConfig.m_iInitRateOfMainLetter;
		inputStream >> levelConfig.m_iIncreasePercentAfterEachMoveOfMainLetter;
		inputStream >> levelConfig.m_iDecreasePercentAfterLetterDestroyedOfMainLetter;
		inputStream >> levelConfig.m_iDecreasePercentAfterLetterAppearedOfMainLetter;
		inputStream >> levelConfig.m_iRatioBetweenLettersOfMainWord;
		std::getline(inputStream, sTemp);		

		// drop on all column or not
		std::getline(inputStream, sTemp);
		inputStream >> iTemp;
		levelConfig.m_bMainWordCanDropOnAllColumn = (iTemp != 0);
		if (!levelConfig.m_bMainWordCanDropOnAllColumn)
		{
			memset( levelConfig.m_MainWordDropOnColumnsFlagList, 0, sizeof(levelConfig.m_MainWordDropOnColumnsFlagList));

			int iColumnCount;
			inputStream >> iColumnCount;
			for(int i=0; i< iColumnCount; i++)
			{
				inputStream >> iTemp;
				iTemp--;
				if (iTemp < _BOARD_MAX_COLUMN_NUMBER_)
					levelConfig.m_MainWordDropOnColumnsFlagList[iTemp] =  true;
			}
		}
		std::getline(inputStream, sTemp);
	}

	// end game bonus type
	std::getline(inputStream, sTemp);
	inputStream >> iTemp;
	levelConfig.m_eEndGameBonusType = (EndGameBonusType)iTemp;
	std::getline(inputStream, sTemp);

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
				case _BQT_COLLECT_BONUS_WORD_: //collect bonus word
				{
					levelConfig.m_BonusQuestConfig.m_IsBonusEnabledQuestFlags[iTemp] = true;
					inputStream >> levelConfig.m_BonusQuestConfig.m_CollectBonusWordQuest.m_iBonusWordCount;
					inputStream >> levelConfig.m_BonusQuestConfig.m_CollectBonusWordQuest.m_iRequiredLetterCount;
					std::getline(inputStream, sTemp);
					for(int j=0; j< levelConfig.m_BonusQuestConfig.m_CollectBonusWordQuest.m_iBonusWordCount; j++)
					{
						std::getline(inputStream, sTemp);
						while (sTemp[sTemp.length()-1] == '\r' || sTemp[sTemp.length()-1] == ' ')
							sTemp.erase( sTemp.size()-1, 1);  // remove \r at the end of line
						levelConfig.m_BonusQuestConfig.m_CollectBonusWordQuest.m_BonusWordIDList[j] = sTemp;
							//GameWordManager::getInstance()->GetWordIndexFromContent(sTemp);
					}
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
	inputStream >> m_GameConfig.m_iBonusScoreCreateCombo4;
	inputStream >> m_GameConfig.m_iBonusScoreCreateCombo5;
	inputStream >> m_GameConfig.m_iBonusScoreCreateCombo6;
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
	inputStream >> m_GameConfig.m_iBonusScoreCompleteBonusQuest;	
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >> m_GameConfig.m_iBonusScoreActivateBonusEndGameCombo;
	std::getline( inputStream, sComment);
	std::getline( inputStream, sComment);
	inputStream >>  m_GameConfig.m_iEndGameComboEffectDestroyCellRatio;


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
	cs::JsonDictionary* pObstacleLevellDict, *pObstacleTranslatePosDict;

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

		// read translate position
		pObstacleTranslatePosDict = pObstacleDetailDict->getSubDictionary("translatePosition");
		pObstacleDescription->m_TranslatePosition.x = pObstacleTranslatePosDict->getItemIntValue("x",0);
		pObstacleDescription->m_TranslatePosition.y = pObstacleTranslatePosDict->getItemIntValue("y",0);
		CC_SAFE_DELETE(pObstacleTranslatePosDict);

		// read draw level label config
		pObstacleDescription->m_bIsDrawLevelLabel = pObstacleDetailDict->getItemBoolvalue("drawLevelLabel", false);
		if (pObstacleDescription->m_bIsDrawLevelLabel)
		{
			pObstacleDescription->m_sLevelLabelImageFolder = pObstacleDetailDict->getItemStringValue("levelLabelImageFolder");
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

void GameConfigManager::LoadWordMapConfig()
{
	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::getInstance()->getFileData("GameData/WorldMapConfig.data", "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	inputStream >> m_WordlMapConfig.m_sFileNameBackgound;
	inputStream >> m_WordlMapConfig.m_iTotalChapter;	

	std::string sTemp;
	for(int iIndex = 0; iIndex < m_WordlMapConfig.m_iTotalChapter; iIndex++)
	{
		WordlMapConfig::WordMapChapterConfig wordMapChapterConfig;
		inputStream >> wordMapChapterConfig.m_sChapterId;
		inputStream >> wordMapChapterConfig.m_sPathData;

		float fX, fY;
		inputStream >> fX;
		inputStream >> fY;
		wordMapChapterConfig.m_position = Point(fX, fY);

		inputStream >> wordMapChapterConfig.m_iTotalevel;
		inputStream >> wordMapChapterConfig.m_hasUnlock;
		wordMapChapterConfig.m_iRequest = 0;

		if(wordMapChapterConfig.m_hasUnlock == 1)
		{
			inputStream >> wordMapChapterConfig.m_iRequest;
		}
		
		
		m_WordlMapConfig.m_WorlMapChapterConfigMap[wordMapChapterConfig.m_sChapterId] = iIndex;
		m_WordlMapConfig.m_WorlMapChapterConfigs.push_back(wordMapChapterConfig);
	}

	delete[] data;
	delete[] orginalData;
}

ChapterConfig& GameConfigManager::GetChapterConfig(const std::string& sChapterID)
{
	if (sChapterID != m_sCurrentChapterID)
	{
		m_sCurrentChapterID = sChapterID;
		this->LoadConfigOfChapter(sChapterID);
	}

	return m_ChapterConfig;
}

void GameConfigManager::LoadConfigOfChapter(const std::string& sChapterID)
{
	WordlMapConfig::WordMapChapterConfig wordMapChapterConfig = this->GetWordMapChapterConfig(sChapterID);
	ChapterConfig& chapterConfig = m_ChapterConfig;
	chapterConfig.m_WordIDList.clear();

	std::string sPathFileData = wordMapChapterConfig.m_sPathData;
	sPathFileData.append("/ChapterConfig.data");

	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::getInstance()->getFileData(sPathFileData.c_str(), "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	inputStream >> chapterConfig.m_iTotalBackgroundImage;
	chapterConfig.m_iTotalevel = wordMapChapterConfig.m_iTotalevel;

	std::string sTemp;
	int iIndex;
	for(iIndex = 0; iIndex < chapterConfig.m_iTotalevel; iIndex++)
	{
		float fX, fY;
		inputStream >> fX;
		inputStream >> fY;
		chapterConfig.m_PositionLevel[iIndex] = Point(fX, fY);
	}

	int iWordCount;
	inputStream >> iWordCount;
	std::getline( inputStream, sTemp);	
	for(iIndex = 0; iIndex < iWordCount; iIndex++)	
	{			
		std::getline( inputStream, sTemp);
		//inputStream >> sTemp;
		chapterConfig.m_WordIDList.push_back(sTemp.substr(0, sTemp.size()-1)); //not count /r at the end of line
	}

	delete[] data;
	delete[] orginalData;
}

void GameConfigManager::GenerateWordsForLevels(const string& sChapterID, std::vector<string>& wordList, std::vector<int>& levelList)
{
	ChapterConfig& chapterConfig = GetChapterConfig(sChapterID);	
	 
	int iIndex;
	int iWordIndex;
	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	std::vector<int> letterCountList;
	int iLetterCount;

	for(auto wordID : chapterConfig.m_WordIDList)
	{
		iWordIndex  = pGameWordManager->GetLoadedWordIndexFromID(wordID);
		iLetterCount = 0;

		const Word& word = pGameWordManager->GetWord(iWordIndex);
		for(int i=0; i< word.m_iWordLength; i++)
		{
			if (word.m_sWord[i] != ' ')
			{
				iLetterCount++;
			}
		}
		letterCountList.push_back(iLetterCount);
		
		wordList.push_back(wordID);
		levelList.push_back(-1);
	}
	
	std::vector<int> sortedWordIndexList;
	
	// we sort word from shortest to longest
	int i,j, iWordCount = letterCountList.size();
	for(i=0; i < letterCountList.size(); i++)
		sortedWordIndexList.push_back(i);

	int iTemp;
	for(i=0; i < iWordCount-1; i++)
		for(j=i+1; j < iWordCount; j++)
			if (letterCountList[sortedWordIndexList[i]] > letterCountList[sortedWordIndexList[j]])
			{
				iTemp = sortedWordIndexList[i];
				sortedWordIndexList[i] = sortedWordIndexList[j];
				sortedWordIndexList[j] = iTemp;
			}

	// we sort level list based required word length from shorted to longest
	std::vector<int> sortedLevelIndexList;
	std::vector<int> requiredLetterOfLevelList;
	for(i=1; i <= chapterConfig.m_iTotalevel; i++)
	{
		LevelConfig& levelConfig = GetLevelConfig(sChapterID, i);
		sortedLevelIndexList.push_back(i-1);
		requiredLetterOfLevelList.push_back(levelConfig.m_iNumberLetterOfMainWord);
	}

	for(i=0; i < chapterConfig.m_iTotalevel-1; i++)
		for(j=i+1; j < chapterConfig.m_iTotalevel; j++)
			if (requiredLetterOfLevelList[sortedLevelIndexList[i]] > requiredLetterOfLevelList[sortedLevelIndexList[j]])
			{
				iTemp = sortedLevelIndexList[i];
				sortedLevelIndexList[i] = sortedLevelIndexList[j];
				sortedLevelIndexList[j] = iTemp;
			}

	// map word to level
	int iSortedLevelIndex = 0;
	int iSortedWordIndex = 0;
	while (iSortedLevelIndex < chapterConfig.m_iTotalevel)
	{
		if (iSortedWordIndex >= iWordCount)// there's a level not match with any word!!!!
			MessageBox("Error", "Data of game chapter is invalid!");

		int i1 = letterCountList[sortedWordIndexList[iSortedWordIndex]];
		int i2 = requiredLetterOfLevelList[sortedLevelIndexList[iSortedLevelIndex]];

		if (letterCountList[sortedWordIndexList[iSortedWordIndex]] >= requiredLetterOfLevelList[sortedLevelIndexList[iSortedLevelIndex]])
		{
			levelList[sortedWordIndexList[iSortedWordIndex]] = sortedLevelIndexList[iSortedLevelIndex]+1;
			
			iSortedLevelIndex++;
			iSortedWordIndex++;
		}
		else
		{
			iSortedWordIndex++;			
		}
	}
}

// temporary used this instead of pre-calculation
int CountLetterOfWord(const int& iWordLength, const char* sWord)
{
	int iIndex, iCount = 0;
	for(iIndex = 0; iIndex < iWordLength; iIndex++)
		if (sWord[iIndex] != ' ')
			iCount++;
	return iCount;
}

void GameConfigManager::UpdateNewWordForLevel(const std::string& sChapterID, const int& iLevel)
{
	LevelInfo& levelInfo = LevelTable::getInstance()->getLevel( sChapterID, iLevel);
	LevelConfig& levelConfig = GetLevelConfig(sChapterID, iLevel);

	std::vector<WordInfo>& wordList = WordTable::getInstance()->getAllWordsForChapter(sChapterID);
	std::vector<int> candidate0IndexList;
	std::vector<int> candidate1IndexList;
	std::vector<int> candidate2IndexList;
	int iIndex;

	// first: not-appear-yet word has highest priority
	for(iIndex = 0; iIndex < wordList.size(); iIndex++)
	{
		int iWordIndex = GameWordManager::getInstance()->GetLoadedWordIndexFromID(wordList[iIndex].sWordId);
		const Word& word = GameWordManager::getInstance()->GetWord(iWordIndex);

		if (levelConfig.m_iNumberLetterOfMainWord <= CountLetterOfWord(word.m_iWordLength, word.m_sWord))
		{		
			if (wordList[iIndex].iCountCollected == 0)
				candidate0IndexList.push_back(iIndex);
			else if (wordList[iIndex].iCountCollected == 1)
				candidate1IndexList.push_back(iIndex);
			else
				if (wordList[iIndex].iCountCollected == 0)
					candidate2IndexList.push_back(iIndex);
		}
	}
	
	if (candidate0IndexList.size() > 0)
	{
		levelInfo.sWordId = wordList[candidate0IndexList[ rand() % candidate0IndexList.size()]].sWordId;	
	}
	else if (candidate1IndexList.size() > 0)
	{
		levelInfo.sWordId = wordList[candidate1IndexList[ rand() % candidate1IndexList.size()]].sWordId;
	}
	else
		levelInfo.sWordId = wordList[candidate2IndexList[ rand() % candidate2IndexList.size()]].sWordId;		

	LevelTable::getInstance()->updateLevel(levelInfo);
}

WordlMapConfig::WordMapChapterConfig& GameConfigManager::GetWordMapChapterConfig(const std::string& sChapterID)
{
	int iIndex = m_WordlMapConfig.m_WorlMapChapterConfigMap[sChapterID];
	return m_WordlMapConfig.m_WorlMapChapterConfigs[iIndex];
}

int GameConfigManager::CountLevelOfPreviousChapters(const std::string& sChapterID)
{
	int iIndexCurrentChapter = m_WordlMapConfig.m_WorlMapChapterConfigMap[sChapterID];

	int iCountLevel = 0;
	for(int iIndex=0; iIndex<iIndexCurrentChapter; iIndex++)
	{
		iCountLevel += m_WordlMapConfig.m_WorlMapChapterConfigs[iIndex].m_iTotalevel;
	}

	return iCountLevel;
}
