#ifndef _ADVANCE_MODE_TARGET_NODE_H_
#define _ADVANCE_MODE_TARGET_NODE_H_

#include "cocos2d.h"
#include "GameDataStructure.h"
#include "Database\CSPackageTable.h"
#include "CustomPackageDownloadManager.h"
#include "cocos-ext.h"
#include "ButtonManagerNode.h"

class AdvanceModeTargetNode : public cocos2d::Node
{
public:
	static AdvanceModeTargetNode* createLayout(const std::string sPackageId, bool bNeedCheckPackageVersion = true);
	bool init(bool bNeedCheckPackageVersion);

	void OnUpdatePackageComplete(const CSPackageInfo& packageInfo);
	void setIsBackToMyPackage(const bool& isBacktoMyPackage);

private:

	void onHttpRequestCompleted(cocos2d::extension::HttpClient *sender, cocos2d::extension::HttpResponse *response);

	void clickPlayAdvanceMode(cocos2d::Object* sender);
	void clickClose(cocos2d::Object* sender);
	void clickUpdatePackage(cocos2d::Object* sender);
private:
	CSPackageInfo m_csPackageInfo;
	std::string m_sPackageId;


	CustomPackageDownloadManager m_CustomPackageDownloadManager;
	bool m_isBacktoMyPackage;
	ButtonManagerNode* m_pButtonManagerNode;


	cocos2d::extension::HttpRequest* m_pRequest;
	cocos2d::extension::HttpClient* m_pClient;
};

#endif;