#ifndef _BONUS_QUEST_MANAGER_H_
#define _BONUS_QUEST_MANAGER_H_

#include "GameConfigManager.h"
class NewGameBoardManager;

struct BonusQuestCollectBonusWordExDescription : BonusQuestCollectBonusWordDescription
{
public:
	int m_iSelectedBonusWordID;
	int m_iRequiredLetterCount;
	int m_iBonusWordCount;

	void Set(BonusQuestCollectBonusWordDescription quest)
	{
		this->m_iRequiredLetterCount = quest.m_iRequiredLetterCount;
		this->m_iBonusWordCount = quest.m_iBonusWordCount;
		//	memcpy( m_BonusWordIDList, quest.m_BonusWordIDList, sizeof(m_BonusWordIDList));
		m_iSelectedBonusWordID = 0;
	}
};

struct BonusQuestCollectBonusParam
{
public:
	int m_iRemainLettersCount;

	BonusQuestCollectBonusParam()
	{
		m_iRemainLettersCount;
	}
};

class BonusQuestManager
{
public:
	BonusQuestManager();

	void InitLevel();

	void ActivateBonusQuest();
	void IncreaseComboCellCountForBonusQuest(const GemComboType_e& eGemComboType);
	void IncreaseBasicCellCountForBonusQuest(const int& iGemID);
	void IncreaseCollectedLetterOfBonusWord();

	inline bool IsQuestJustActivated(const BonusQuestType& eQuestType, bool bResetFlag = true) { 
		if (m_IsBonusQuestDirtyFlags[eQuestType] && !m_IsBonusQuestActivatedFlags[eQuestType])
		{
			if (bResetFlag)
			{
				m_IsBonusQuestActivatedFlags[eQuestType] = true;
				m_IsBonusQuestDirtyFlags[eQuestType] = false;
			}
			return true;
		}
		return false;
	}

	inline bool IsQuestActivated(const BonusQuestType& eQuestType) { return m_IsBonusQuestActivatedFlags[eQuestType];}

	inline bool IsQuestJustCompleted(const BonusQuestType& eQuestType, bool bResetFlag = true) { 
		if (m_IsBonusQuestDirtyFlags[eQuestType] && m_IsBonusQuestCompletedFlags[eQuestType])
		{	
			if (bResetFlag)
			{
				m_IsBonusQuestDirtyFlags[eQuestType] = false;
			}
			return true;
		}
		return false;
	}

	inline const BonusQuestCollectGemDescription& GetCollectGemParam() { return m_CollectGemParam;}
	inline const BonusQuestCollectComboDescription& GetCollectComboParam() { return m_CollectComboParam;}
	inline const BonusQuestCollectBonusParam& GetCollectBonusWordParam() { return m_CollectBonusWordParam;}

	void GetLettersOfCollectBonusWordQuest(std::vector<char>& letterList, const int& iMaxLetterCount);
private:
	friend class NewGameBoardManager;
	NewGameBoardManager* m_pGameBoardManager;
	const LevelConfig* m_pLevelConfig;

	bool m_IsBonusEnabledQuestFlags[_BQT_TYPE_COUNT_];
	bool m_IsBonusQuestActivatedFlags[_BQT_TYPE_COUNT_];
	bool m_IsBonusQuestDirtyFlags[_BQT_TYPE_COUNT_];
	bool m_IsBonusQuestCompletedFlags[_BQT_TYPE_COUNT_];

	//bool m_bIsEnableCollectGemQuest;
	//bool m_bIsCollectedGemQuestActivated;
	BonusQuestCollectGemDescription m_CollectGemParam;
	BonusQuestCollectGemDescription m_CollectGemObjective;

	//bool m_bIsEnableCollectComboQuest;
	//bool m_bIsCollectedComboQuestActivated;
	BonusQuestCollectComboDescription m_CollectComboParam;
	BonusQuestCollectComboDescription m_CollectComboObjective;

	BonusQuestCollectBonusParam m_CollectBonusWordParam;
	BonusQuestCollectBonusWordExDescription m_CollectBonusWordObjective;
};


#endif _BONUS_QUEST_MANAGER_H_