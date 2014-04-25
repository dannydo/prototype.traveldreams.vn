#ifndef _CUSTOM_PACKAGE_DOWNLOADER_NODE_H_
#define _CUSTOM_PACKAGE_DOWNLOADER_NODE_H_

#include "cocos2d.h"
#include "cocos-ext.h"
#include "AssetsManager\AssetsManager.h"

using namespace cocos2d;
USING_NS_CC_EXT;

struct CustomPackageInfo
{
public:
	std::string m_sPackageFolder;
	std::string m_sCode;
};

class CustomPackageDownloaderNode : public cocos2d::Node, public cocos2d::extension::AssetsManagerDelegateProtocol, //, public cocos2d::extension::EditBoxDelegate
									public cocos2d::extension::TableViewDataSource, public cocos2d::extension::TableViewDelegate
{
public:
	static CustomPackageDownloaderNode* create();


	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) {};
    virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) {}
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell);
    virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, unsigned int idx);
    virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, unsigned int idx);
    virtual unsigned int numberOfCellsInTableView(cocos2d::extension::TableView *table);
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

	void AddNewPackageToList(const CustomPackageInfo& customPackageInfo);
	void SavePackageListToFile();
	void LoadPackageListFromFile();
private:
	AssetsManager* m_pAssetManager;
	std::string m_sPathToSave;

	std::vector<CustomPackageInfo> m_CustomPackageList;
	TableView* m_pTableView;

	bool m_bIsDownloadingPackage;
	std::string m_sResultFolder;

	//Menu* m_pMenu;
	EditBox* m_pCodeEditBox;
	LabelTTF* m_pProgressLabel;
};

#endif