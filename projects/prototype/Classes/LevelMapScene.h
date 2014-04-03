#ifndef _LEVEL_MAP_H_
#define _LEVEL MAP_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h" 
#include "SettingMenuNode.h" 
#include "FooterNode.h"	   
#include "ScrollManager.h"

class LevelMapLayer : public cocos2d::CCLayer
{
public:
	LevelMapLayer() {};
	virtual ~LevelMapLayer();
	bool init();
	CREATE_FUNC(LevelMapLayer);

	void showPopupTargetGame(const std::string& sChapterID, const int& iLevel);

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
	bool m_bIsSwipe;

	std::string m_sChapterId;

	FooterNode* m_pFooterNode;
	ScrollManager* m_pScrollManager;
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