#ifndef _ADVANCE_MODE_TARGET_NODE_H_
#define _ADVANCE_MODE_TARGET_NODE_H_

#include "cocos2d.h"
#include "GameDataStructure.h"
#include "Database\CSPackageTable.h"
#include "CustomPackageDownloadManager.h"

class AdvanceModeTargetNode : public cocos2d::Node
{
public:
	static AdvanceModeTargetNode* createLayout(const std::string sPackageId, bool bNeedCheckPackageVersion = true);
	bool init(bool bNeedCheckPackageVersion);

	void OnUpdatePackageComplete(const CSPackageInfo& packageInfo);
private:
	void clickPlayAdvanceMode(cocos2d::Object* sender);
	void clickClose(cocos2d::Object* sender);
	void clickUpdatePackage(cocos2d::Object* sender);

	CSPackageInfo m_csPackageInfo;
	std::string m_sPackageId;

	CustomPackageDownloadManager m_CustomPackageDownloadManager;
};

#endif;