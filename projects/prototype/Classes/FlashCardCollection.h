#ifndef _FLASH_CARD_COLLECTION_H_
#define _FLASH_CARD_COLLECTION_H_

#include "cocos2d.h"
#include "GameDataStructure.h"	 
#include "Database\ChapterTable.h"
#include "SettingMenuNode.h"
#include "FooterNode.h"
#include "ButtonManagerNode.h"	 
#include "ScrollManager.h"

class FlashCardCollectionLayer : public cocos2d::Layer
{
public:
	FlashCardCollectionLayer() {};
	virtual ~FlashCardCollectionLayer();
	bool init();
	CREATE_FUNC(FlashCardCollectionLayer);

	void update(float dt) override;

private:
	bool onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	
	void clickOpenFlashCard(Object* sender);
	void clickPlayMiniGame(Object* sender);
	void clickPlayReviseGame(Object* sender);

	ButtonManagerNode* m_pSlideShow;
	std::vector<ChapterInfo> m_chapters;
	FooterNode* m_pFooterNode;
	ScrollManager* m_pScrollManager;

	cocos2d::LabelTTF* m_pLabelMiniGame;

	float m_fBeginY;
	float m_fYMoved;
	float m_maxHeight;
	int m_iWordPlayMiniGame;
	int m_iWordNew;
	float m_iTimeCountDown;
};

class FlashCardCollectionScene : public cocos2d::CCScene
{
public:
	FlashCardCollectionScene():_layer(NULL) {};
	~FlashCardCollectionScene();
	bool init();
	CREATE_FUNC(FlashCardCollectionScene);

	CC_SYNTHESIZE_READONLY(FlashCardCollectionLayer*, _layer, Layer);
};

#endif;