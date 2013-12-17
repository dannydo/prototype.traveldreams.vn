#ifndef _INTRODUCTION_H_
#define _INTRODUCTION_H_

#include "cocos2d.h"

class IntroductionLayer : public cocos2d::CCLayerColor
{
public:
	IntroductionLayer() {};
	virtual ~IntroductionLayer();
	bool init();
	CREATE_FUNC(IntroductionLayer);

private:
	void menuCloseIntroduction();
	void menuNextImage();
	void menuPreviousImage();

	bool onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;

	cocos2d::Node* m_pNodeImage;
	cocos2d::CCMenu* m_pMenuIntroduction;
	cocos2d::Point m_BeginPoint;
	int m_iImageTag;
};

class IntroductionScene : public cocos2d::CCScene
{
public:
	IntroductionScene():_layer(NULL) {};
	~IntroductionScene();
	bool init();
	CREATE_FUNC(IntroductionScene);

	CC_SYNTHESIZE_READONLY(IntroductionLayer*, _layer, Layer);	
};

#endif