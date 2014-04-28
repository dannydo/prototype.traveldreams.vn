#ifndef _FLASH_CARD_H_
#define _FLASH_CARD_H_

#include "cocos2d.h"
#include "GameDataStructure.h"
#include "Database\ChapterTable.h"
#include "SettingMenuNode.h"
#include "Database\WordTable.h"
#include "FooterNode.h"
#include "ButtonManagerNode.h"

class FlashCardLayer : public cocos2d::Layer
{
public:
	FlashCardLayer() {};
	virtual ~FlashCardLayer();
	bool init();
	static FlashCardLayer* createLayout(const ChapterInfo& chapterInfo);

private:
	bool onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;

	void createNodeSlideShow();
	void clickButtonLeft(cocos2d::Object* pSender);
	void clickButtonRight(cocos2d::Object* pSender);

	cocos2d::Node* m_pSlideShow;
	cocos2d::LabelTTF* m_pLabelIndex;
	cocos2d::Point m_touchPositionMoved;
	cocos2d::LabelTTF* m_pLabelWord;

	std::vector<WordInfo> m_Words;
	ChapterInfo m_chapterInfo;


	bool m_bIsSwipe;
	bool m_bMoveLeft;
	bool m_bMoveRight;

	float m_fXMoved;
	float m_fBeginX;

	int m_iMoveLeftOrRight;
	int m_iTotalFlashCard;
	int m_iIndexFlashCard;
  
	FooterNode* m_pFooterNode;
};

class FlashCardScene : public cocos2d::CCScene
{
public:
	FlashCardScene():_layer(NULL) {};
	~FlashCardScene();
	bool init();
	static FlashCardScene* createScene(const ChapterInfo& chapterInfo);

	CC_SYNTHESIZE_READONLY(FlashCardLayer*, _layer, Layer);

private:
	ChapterInfo m_chapterInfo;
};

#endif;