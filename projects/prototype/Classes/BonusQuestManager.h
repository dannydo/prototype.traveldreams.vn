#ifndef _BONUS_QUEST_MANAGER_H_
#define _BONUS_QUEST_MANAGER_H_

#include "GameConfigManager.h"
class NewGameBoardManager;

class BonusQuestManager
{
public:
	BonusQuestManager();

	void InitLevel();

	void ActivateBonusQuest();
	void IncreaseComboCellCountForBonusQuest(const GemComboType_e& eGemComboType);
	void IncreaseBasicCellCountForBonusQuest(const int& iGemID);
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
};


#endif _BONUS_QUEST_MANAGER_H_