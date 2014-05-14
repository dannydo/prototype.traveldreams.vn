#ifndef ADVANCE_MODE_MY_PACKAGES_SCENE_H_
#define ADVANCE_MODE_MY_PACKAGES_SCENE_H_

#include "cocos2d.h"
#include "GameDataStructure.h"
#include "Database\ChapterTable.h"
#include "SettingMenuNode.h"
#include "FooterNode.h"
#include "ButtonManagerNode.h"
#include "ScrollManager.h"
#include "Database\CSPackageTable.h"

class AdvanceModeMyPackagesLayer : public cocos2d::Layer
{
public:
	AdvanceModeMyPackagesLayer() {};
	virtual ~AdvanceModeMyPackagesLayer();
	bool init();
	CREATE_FUNC(AdvanceModeMyPackagesLayer);

private:
	bool onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	
	void clickPlayAdvanceMode(cocos2d::Object* sender);
	void clickAddMorePackage(cocos2d::Object* sender);

	cocos2d::Sprite* m_pBackgroundSlideShow;

	ButtonManagerNode* m_pSlideShow;
	FooterNode* m_pFooterNode;
	ScrollManager* m_pScrollManager;

	std::vector<CSPackageInfo> m_csPackageInfos;

	float m_fBeginY;
	float m_fYMoved;
	float m_maxHeight;
	bool m_bIsSwipe;
};

class AdvanceModeMyPackagesScene : public cocos2d::CCScene
{
public:
	AdvanceModeMyPackagesScene():_layer(NULL) {};
	~AdvanceModeMyPackagesScene();
	bool init();
	CREATE_FUNC(AdvanceModeMyPackagesScene);

	CC_SYNTHESIZE_READONLY(AdvanceModeMyPackagesLayer*, _layer, Layer);
};

#endif;