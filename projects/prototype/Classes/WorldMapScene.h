#ifndef _WORLD_MAP_H_
#define _WORLD_MAP_H_

#include "cocos2d.h"  
#include "SettingMenuNode.h"

class WorldMapLayer : public cocos2d::CCLayer
{
public:
	WorldMapLayer() {};
	virtual ~WorldMapLayer();
	bool init();
	CREATE_FUNC(WorldMapLayer);

private:
	bool onTouchBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;
	void onTouchMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;
	void onTouchEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

	void loadConfigWordMap();
	void updateScrollSlideShow();

	void menuPlayChapterCallBack(cocos2d::Object* sender);
	void menuOpenIntroductionCallBack();
	void menuOpenFlashCardCallBack();
	void openSettingMenu();

	cocos2d::LayerColor* m_pBackgroundNode;
	SettingMenuNode* m_pSettingNode	;
	bool m_isShowSetting;

	int m_iTotalChapter;
	std::vector<cocos2d::Point> m_pointChapter;

	float m_fBeginY;
	float m_fYMoved;
	float m_maxHeight;
	bool m_bMoveSlideShow;
};

class WorldMapScene : public cocos2d::CCScene
{
public:
	WorldMapScene():_layer(NULL) {};
	~WorldMapScene();
	bool init();
	CREATE_FUNC(WorldMapScene);

	CC_SYNTHESIZE_READONLY(WorldMapLayer*, _layer, Layer);	
};

#endif