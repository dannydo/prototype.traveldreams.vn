#ifndef _CUSTOM_PACKAGE_DOWNLOADER_NODE_H_
#define _CUSTOM_PACKAGE_DOWNLOADER_NODE_H_

#include "cocos2d.h"
#include "cocos-ext.h"
#include "AssetsManager\AssetsManager.h"

using namespace cocos2d;
USING_NS_CC_EXT;

class CustomPackageDownloaderNode : public cocos2d::Node//, public cocos2d::extension::EditBoxDelegate
{
public:
	static CustomPackageDownloaderNode* create();
private:
	CustomPackageDownloaderNode();
	~CustomPackageDownloaderNode();

	bool init();

	void startDownloadCallback(Object* sender);
	void exitCallback(Object* sender);

private:
	AssetsManager* m_pAssetManager;
	EditBox* m_pCodeEditBox;
};

#endif