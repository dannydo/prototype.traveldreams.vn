#ifndef _CUSTOM_PACKAGE_DOWNLOAD_MANAGER_H_ 
#define _CUSTOM_PACKAGE_DOWNLOAD_MANAGER_H_

#include "cocos-ext.h"
#include "AssetsManager\AssetsManager.h"
#include "WaitingNode.h"
#include "Database\CSPackageTable.h"

using namespace cocos2d;
USING_NS_CC_EXT;

class CustomPackageDownloadManager : public cocos2d::extension::AssetsManagerDelegateProtocol
{

public:
	CustomPackageDownloadManager();
	~CustomPackageDownloadManager();

	void StartDownloadPackage(Node* pParentNode, const char* sCode);

	void SetDownloadPackageCompleteCallback( std::function<void(const CSPackageInfo&)> downloadPackageCompleteCallback) { m_DownloadPackageCompleteCallback = downloadPackageCompleteCallback;}

	void GetPackageInfoFromServer(CSPackageInfo& package);

	bool CheckUpdateOfPackage(Node* pParentNode, const std::string& sPackageCode);
private:
	// assets manager callback
	virtual void onError(cocos2d::extension::AssetsManager::ErrorCode errorCode);
    virtual void onProgress(int percent);
    virtual void onSuccess();
private:
	AssetsManager* m_pAssetManager;
	std::string m_sPathToSave;

	std::string m_sCode;

	Node* m_pParentNode;
	WaitingNode* m_pNotificationPopup;

	bool m_bIsDownloadingPackage;
	std::string m_sResultFolder;

	std::function<void(const CSPackageInfo&)> m_DownloadPackageCompleteCallback;
	//std::function<void()> m_DownloadPackageErrorCallback;
};

#endif _CUSTOM_PACKAGE_DOWNLOAD_MANAGER_H_