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
	void menuPlayChapterCallBack();
	void menuOpenIntroductionCallBack();
	void menuOpenFlashCardCallBack();
	void openSettingMenu();

	SettingMenuNode* m_pSettingNode	;
	bool m_isShowSetting;
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