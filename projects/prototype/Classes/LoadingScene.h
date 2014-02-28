#ifndef _LOADING_H_
#define _LOADING_H_

#include "cocos2d.h"

class LoadingLayer : public cocos2d::LayerColor
{
public:
	LoadingLayer() {};
	virtual ~LoadingLayer();
	bool init();
	CREATE_FUNC(LoadingLayer);

	void update(float dt) override;

private:
	void loadData();
	void initData();
	void finishLoading();

	bool m_bFinishLoad;
};

class LoadingScene : public cocos2d::Scene
{
public:
	LoadingScene():_layer(NULL) {};
	~LoadingScene();
	bool init();
	CREATE_FUNC(LoadingScene);

	CC_SYNTHESIZE_READONLY(LoadingLayer*, _layer, Layer);	
};

#endif