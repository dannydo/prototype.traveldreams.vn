#ifndef _WORLD_MAP_H_
#define _WORLD_MAP_H_

#include "cocos2d.h"
#include "FooterNode.h"
#include "GameConfigManager.h"
#include "ScrollManager.h"
#include "ButtonManagerNode.h"

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

	void updateScrollSlideShow();
	void menuPlayChapterCallBack(cocos2d::Object* sender);

	cocos2d::Node* createLayoutLockChapter();

	cocos2d::LayerColor* m_pBackgroundNode;

	float m_fBeginY;
	float m_fYMoved;
	float m_maxHeight;
	bool m_bIsSwipe;

	FooterNode* m_pFooterNode;
	WordlMapConfig m_wordMapConfig;

	ScrollManager* m_pScrollManager;
	ButtonManagerNode* m_pButtonManagerNode;
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