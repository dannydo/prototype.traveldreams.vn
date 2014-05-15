#ifndef ADVANCE_MODE_POPULAR_PACKAGES_SCENE_H_
#define ADVANCE_MODE_POPULAR_PACKAGES_SCENE_H_

#include "cocos2d.h" 
#include "FooterNode.h"
#include "ButtonManagerNode.h"	 
#include "ScrollManager.h"	 
#include "Database\CSPackageTable.h"
#include "APIService\InterfaceService.h" 
#include "cocos-ext.h"
#include "CustomPackageDownloadManager.h"

using namespace cocos2d;
USING_NS_CC_EXT;

class AdvanceModePopularPackagesLayer : public cocos2d::Layer, InterfaceService
{
public:
	AdvanceModePopularPackagesLayer() {};
	virtual ~AdvanceModePopularPackagesLayer();
	bool init();
	CREATE_FUNC(AdvanceModePopularPackagesLayer);

	void resultHttpRequestCompleted(cs::JsonDictionary* pJsonDict, std::string sKey) override;

private:
	bool onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	void onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent) override;
	
	void clickDownloadPackage(cocos2d::Object* sender);
	void clickAddPackage(cocos2d::Object* sender);	
	void clickAddPackageCode(cocos2d::Object* sender);


	void OnDownloadPackageComplete(const CSPackageInfo& packageInfo);
private:
	cocos2d::Sprite* m_pBackgroundSlideShow;
	EditBox* m_pCodeEditBox;

	ButtonManagerNode* m_pSlideShow;
	FooterNode* m_pFooterNode;
	ScrollManager* m_pScrollManager;

	float m_fBeginY;
	float m_fYMoved;
	float m_maxHeight;
	bool m_bIsSwipe;

	int m_iConnectServer;
	int m_iTotalPackage;

	CustomPackageDownloadManager m_CustomPackageDownloadManager;
	std::vector<CSPackageInfo> m_csPackageInfos;
};

class AdvanceModePopularPackagesScene : public cocos2d::CCScene
{
public:
	AdvanceModePopularPackagesScene():_layer(NULL) {};
	~AdvanceModePopularPackagesScene();
	bool init();
	CREATE_FUNC(AdvanceModePopularPackagesScene);

	CC_SYNTHESIZE_READONLY(AdvanceModePopularPackagesLayer*, _layer, Layer);
};

#endif;