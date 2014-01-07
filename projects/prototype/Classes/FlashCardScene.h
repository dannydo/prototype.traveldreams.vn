#ifndef _FLASH_CARD_H_
#define _FLASH_CARD_H_

#include "cocos2d.h"
#include "GameDataStructure.h"

class FlashCardLayer : public cocos2d::Layer
{
public:
	FlashCardLayer() {};
	virtual ~FlashCardLayer();
	bool init();
	CREATE_FUNC(FlashCardLayer);
	void setNameClassParent(const char* sNameClassParent);

private:
	bool onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;

	void createNodeSlideShow();
	void menuBackCallBack();

	cocos2d::Node* m_pFlashCardDisplay;
	cocos2d::Node* m_pSlideShow;

	bool m_bRunningEffect;

	float m_fXMoved;
	float m_fBeginX;

	int m_iTotalFlashCard;
	int m_iIndexFlashCard;

	const char* m_sNameClassParent;
	

	/*
	cocos2d::Sprite* m_pBackground;
	cocos2d::Sprite* m_pFlashCard;
	cocos2d::LabelTTF* m_pTitleWord;
	cocos2d::LabelTTF* m_pMeaningWord;

	Word m_word;
	*/
};

class FlashCardScene : public cocos2d::CCScene
{
public:
	FlashCardScene():_layer(NULL) {};
	~FlashCardScene();
	bool init();
	CREATE_FUNC(FlashCardScene);

	CC_SYNTHESIZE_READONLY(FlashCardLayer*, _layer, Layer);	
};

#endif;