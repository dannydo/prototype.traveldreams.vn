#ifndef _FLASH_CARD_COLLECTION_H_
#define _FLASH_CARD_COLLECTION_H_

#include "cocos2d.h"
#include "GameDataStructure.h"	 
#include "Database\ChapterTable.h"
#include "SettingMenuNode.h"
#include "FooterNode.h"

class FlashCardCollectionLayer : public cocos2d::LayerColor
{
public:
	FlashCardCollectionLayer() {};
	virtual ~FlashCardCollectionLayer();
	bool init();
	CREATE_FUNC(FlashCardCollectionLayer);

private:
	bool onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	
	void openFlashCard(const int& iIndexButton);

	cocos2d::Node* m_pSlideShow;
	cocos2d::Sprite* m_pButton1;
	cocos2d::Sprite* m_pButton2;
	cocos2d::Sprite* m_pButton3;
	cocos2d::Sprite* m_pButton4;  
	cocos2d::Point m_touchPositionMoved;

	std::vector<ChapterInfo> m_chapters;

	int m_iMoveLeftOrRight;
	float m_fXMoved;
	float m_fBeginX;
	int m_iTotalPage;
	int m_iCurrentPage;
	bool m_bIsSwipe;

	const char* m_sNameClassParent;
	FooterNode* m_pFooterNode;
};

class FlashCardCollectionScene : public cocos2d::CCScene
{
public:
	FlashCardCollectionScene():_layer(NULL) {};
	~FlashCardCollectionScene();
	bool init();
	CREATE_FUNC(FlashCardCollectionScene);

	CC_SYNTHESIZE_READONLY(FlashCardCollectionLayer*, _layer, LayerColor);	
};

#endif;