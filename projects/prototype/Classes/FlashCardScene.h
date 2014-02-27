#ifndef _FLASH_CARD_H_
#define _FLASH_CARD_H_

#include "cocos2d.h"
#include "GameDataStructure.h"
#include "Database\ChapterTable.h"
#include "SettingMenuNode.h"
#include "Database\LevelTable.h"

class FlashCardLayer : public cocos2d::LayerColor
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

	void openStringMenu();
	void createNodeSlideShow();
	void showSlideShowFlashCard();
	void updateLayout();
	void playVoiceWord();

	cocos2d::Node* createLayoutFlashCard(const int& iIndexFlashCard);

	cocos2d::Node* m_pFlashCardDisplay;
	cocos2d::Node* m_pSlideShow;
	cocos2d::LabelTTF* m_pLabelIndex;
	cocos2d::Point m_touchPositionMoved;

	std::vector<LevelInfo> m_levelInfo;
	ChapterInfo m_chapterInfo;
	Word wordInfo;

	bool m_bIsSwipe;

	float m_fXMoved;
	float m_fBeginX;

	int m_iMoveLeftOrRight;
	int m_iTotalFlashCard;
	int m_iIndexFlashCard;

	SettingMenuNode* m_pSettingNode	;
	bool m_isShowSetting;
};

class FlashCardScene : public cocos2d::CCScene
{
public:
	FlashCardScene():_layer(NULL) {};
	~FlashCardScene();
	bool init();
	static FlashCardScene* createScene(const ChapterInfo& chapterInfo);

	CC_SYNTHESIZE_READONLY(FlashCardLayer*, _layer, LayerColor);

private:
	ChapterInfo m_chapterInfo;
};

#endif;