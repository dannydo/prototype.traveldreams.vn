#ifndef _LEVEL_MAP_H_
#define _LEVEL MAP_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "SettingMenuNode.h"
#include "FooterNode.h"
#include "ScrollManager.h"
#include "GameDataStructure.h"
#include "ButtonManagerNode.h"
#include "Database\LevelTable.h"

class LevelMapLayer : public cocos2d::CCLayer
{
public:
	LevelMapLayer() {};
	virtual ~LevelMapLayer();
	bool init();
	CREATE_FUNC(LevelMapLayer);

	void showPopupEndGameLose(const int& iScore, const Word& mainWord, const int& iCurrentLevel, const std::string sChapterId);
	void showPopupEndGameWin(const int& iScore, const Word& mainWord, const int& iCurrentLevel, const std::string sChapterId, const int& iYellowStar, const int& iBonusQuestCompleted);
	void showPopupQuitLevelFailed( const int& iCurrentLevel, const std::string sChapterId);

	void playEffectUnlockLevel(const bool& bPlayNextLevelGame, const int& iCurrentLevel, const std::string sChapterId);
	void updateLayoutWordMapWhenRetryLevelWin(const int& iCurrentLevel, const std::string sChapterId);

private:
	bool onTouchBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;
	void onTouchMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;
	void onTouchEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

	void menuLevelSelected(Object* pSender);
	void openSettingMenu();
	void menuOpenFlashCardCallBack();

	cocos2d::Node* generateLayoutStarAndBonusQuest(const int& iStarCompleted, const int& iBonusQuestCompleted, const int& iTotalBonusQuest);

	cocos2d::Node* m_pBackgroundNode;

	float m_fBeginY;
	float m_fYMoved;
	float m_maxHeight;

	int m_iCalculatorLevel;

	std::string m_sChapterId;
	std::vector<LevelInfo> m_levels;

	FooterNode* m_pFooterNode;
	ScrollManager* m_pScrollManager;






	void loopUpdateStar();
	void sequenceUpdateStar();
	void updateStar();

	void loopUpdateBonusQuest();
	void sequenceUpdateBonusQuest();
	void updateBonusQuest();

	void effectChangeCurrentLevel();
	void showPopupTargetGame();

	void removeButtonNodeCurrentLevel();

	void finishEffectUnlockLevel();

	ButtonNode* m_buttonPlayPassLevel;
	ButtonManagerNode* m_pButtonManagerNode;
	ButtonNode* m_pButtonNodeCurrentLevel;
	cocos2d::Sprite* m_ButtonNextLevelSprite;

	int m_iYellowStar;
	int m_iCountYellowStar;
	int m_iTotalBonusQuest;
	int m_iBonusQuestCompleted;
	int m_iCountBonusQuest;
	int m_iCurrentLevel;

	cocos2d::Node* m_pNodeStarAndBonusQuestEffect;
};

class LevelMapScene : public cocos2d::CCScene
{
public:
	LevelMapScene():_layer(NULL) {};
	~LevelMapScene();
	bool init();
	CREATE_FUNC(LevelMapScene);

	CC_SYNTHESIZE_READONLY(LevelMapLayer*, _layer, Layer);
};

#endif