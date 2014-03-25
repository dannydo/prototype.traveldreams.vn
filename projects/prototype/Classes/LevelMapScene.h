#ifndef _LEVEL_MAP_H_
#define _LEVEL MAP_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h" 
#include "SettingMenuNode.h" 
#include "FooterNode.h"

class LevelMapLayer : public cocos2d::CCLayer
{
public:
	LevelMapLayer() {};
	virtual ~LevelMapLayer();
	bool init();
	CREATE_FUNC(LevelMapLayer);

	void showPopupTargetGame(const int& iLevel);

private:
	bool onTouchBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;
	void onTouchMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;
	void onTouchEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

	void menuLevelSelected(Object* pSender);
	void openSettingMenu();
	void menuOpenFlashCardCallBack();

	void loadConfigChapter(const int& iChapter);
	void updateScrollSlideShow();

	cocos2d::Node* generateLayoutStarAndBonusQuest(const int& iStarCompleted, const int& iBonusQuestCompleted, const int& iTotalBonusQuest);

	cocos2d::Node* m_pBackgroundNode;

	int m_iTotalImageBG;
	int m_iTotalLevel;
	std::vector<cocos2d::Point> m_pointLevel;

	float m_fBeginY;
	float m_fYMoved;
	float m_maxHeight;
	bool m_bMoveSlideShow;

	FooterNode* m_pFooterNode;
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