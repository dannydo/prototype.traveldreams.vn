#ifndef _CUSTOM_PACKAGE_DOWNLOADER_NODE_H_
#define _CUSTOM_PACKAGE_DOWNLOADER_NODE_H_

#include "cocos2d.h"
#include "cocos-ext.h"
#include "AssetsManager\AssetsManager.h"

using namespace cocos2d;
USING_NS_CC_EXT;

class CustomPackageDownloaderNode : public cocos2d::Node, public cocos2d::extension::AssetsManagerDelegateProtocol //, public cocos2d::extension::EditBoxDelegate
{
public:
	static CustomPackageDownloaderNode* create();
private:
	CustomPackageDownloaderNode();
	~CustomPackageDownloaderNode();

	bool init();

	void startDownloadCallback(Object* sender);
	void exitCallback(Object* sender);

	// assets manager callback
	virtual void onError(cocos2d::extension::AssetsManager::ErrorCode errorCode);
    virtual void onProgress(int percent);
    virtual void onSuccess();

	//
	void startCustomGame();
private:
	AssetsManager* m_pAssetManager;
	std::string m_sPathToSave;

	std::string m_sResultFolder;

	Menu* m_pMenu;
	EditBox* m_pCodeEditBox;
	LabelTTF* m_pProgressLabel;
};

#endif