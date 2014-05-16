#include "CustomPackageDownloadManager.h"
#include "GameConfigManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <curl/curl.h>
#include <curl/easy.h>

void createDownloadedDir(const string& pathToSave);
static size_t getStringData(void *ptr, size_t size, size_t nmemb, void *userdata);

CustomPackageDownloadManager::CustomPackageDownloadManager()
{
	m_pAssetManager = NULL;
	m_pNotificationPopup = NULL;
	m_pParentNode = NULL;

	//init folder to save data
	m_sPathToSave = FileUtils::getInstance()->getWritablePath();
	m_sPathToSave += "downloaded";	
	createDownloadedDir(m_sPathToSave);

	m_bIsDownloadingPackage = false;

	m_DownloadPackageCompleteCallback = nullptr;

	// try to load custom mode config
	GameConfigManager::getInstance()->GetTimeModeDemoConfig();

	//init folder to save data
	m_sPathToSave = FileUtils::getInstance()->getWritablePath();
	m_sPathToSave += "downloaded";	
	createDownloadedDir(m_sPathToSave);
}

CustomPackageDownloadManager::~CustomPackageDownloadManager()
{

}

bool checkAndGetResultFolderName(const std::string& sOuputFolderUrl, std::string& sFolderName);
void callTrackingURL(const std::string& sTrackingUrl);

void CustomPackageDownloadManager::StartDownloadPackage(Node* pParentNode, const char* sCode)
{
	if (m_bIsDownloadingPackage)
		return;	

	m_bIsDownloadingPackage = true;

	//m_pMenu->setEnabled(false);

	const char* sBaseUrl = "http://vocab.kiss-concept.com/packages";
	char sVersionUrl[256], sFolderUrl[256];	
	string sDownloadUrl;
	sprintf(sFolderUrl, "%s/folder/%s",sBaseUrl, sCode);

	std::string sFolderName;
	if (checkAndGetResultFolderName( string(sFolderUrl), m_sResultFolder) && m_sResultFolder.size()!=0)
	{	
		m_sCode = sCode;
		m_pParentNode = pParentNode;

		m_pNotificationPopup = WaitingNode::createLayout("Downloading...");
		pParentNode->addChild(m_pNotificationPopup);

		/*
		int iSpaceIndex = m_sResultFolder.find(' ');
		sDownloadUrl = m_sResultFolder.substr(iSpaceIndex+1, m_sResultFolder.size() - iSpaceIndex -1);
		m_sResultFolder = m_sResultFolder.substr(0, iSpaceIndex);*/

		std::stringstream ss(m_sResultFolder);
		std::istream_iterator<std::string> begin(ss);
		std::istream_iterator<std::string> end;
		std::vector<std::string> splitList(begin, end);
		m_sResultFolder = splitList[0];
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		sDownloadUrl = splitList[2];
#else	
		sDownloadUrl = splitList[1];
#endif


		//sprintf(sDownloadUrl, "%s/download/%s",sBaseUrl, m_pCodeEditBox->getText());
		sprintf(sVersionUrl, "%s/version/%s",sBaseUrl, sCode);
		m_pAssetManager = new AssetsManager( sDownloadUrl.c_str(), sVersionUrl, m_sPathToSave.c_str());
		m_pAssetManager->setDelegate(this);
		m_pAssetManager->setConnectionTimeout(3);
		pParentNode->addChild(m_pAssetManager);
		m_pAssetManager->release();

		m_pAssetManager->update();
	}
	else
	{
		// show error
		auto errorPopup = WaitingNode::createLayout("Invalid code!");
		pParentNode->addChild(errorPopup);
		errorPopup->runAction(
			Sequence::createWithTwoActions(
				DelayTime::create(2.f),
				RemoveSelf::create()));

		//m_pProgressLabel->setString("Code is invalid!!");
		//m_pMenu->setEnabled(true);

		m_bIsDownloadingPackage = false;
	}
}

bool CustomPackageDownloadManager::CheckUpdateOfPackage(Node* pParentNode, const std::string& sPackageCode)
{
	//m_pParentNode = NULL; //not need save parent node this case

	const char* sBaseUrl = "http://vocab.kiss-concept.com/packages";
	char sVersionUrl[256], sFolderUrl[256];	
	string sDownloadUrl;
	sprintf(sFolderUrl, "%s/folder/%s",sBaseUrl, sPackageCode.c_str());

	std::string sFolderName;
	if (checkAndGetResultFolderName( string(sFolderUrl), m_sResultFolder) && m_sResultFolder.size()!=0)
	{									
		std::stringstream ss(m_sResultFolder);
		std::istream_iterator<std::string> begin(ss);
		std::istream_iterator<std::string> end;
		std::vector<std::string> splitList(begin, end);
		m_sResultFolder = splitList[0];
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		sDownloadUrl = splitList[2];
#else	
		sDownloadUrl = splitList[1];
#endif

		//char sVersionUrl[256];
		//const char* sBaseUrl = "http://vocab.kiss-concept.com/packages";
		sprintf(sVersionUrl, "%s/version/%s",sBaseUrl, sPackageCode.c_str());

		auto pAssetManager = new AssetsManager( sDownloadUrl.c_str(), sVersionUrl, "");
		pAssetManager->setDelegate(NULL);
		bool bResult = pAssetManager->checkUpdate();	

		pParentNode->addChild(pAssetManager);
		pAssetManager->release();

		return bResult;
	}
	return false;
}

// assets manager callback
void CustomPackageDownloadManager::onError(cocos2d::extension::AssetsManager::ErrorCode errorCode)
{	
	//if (m_pParentNode == NULL)
		//return;

	if (m_pNotificationPopup != NULL)
	{
		m_pNotificationPopup->removeFromParentAndCleanup(true);
		m_pNotificationPopup = NULL;
	}	

	if (errorCode == AssetsManager::ErrorCode::NO_NEW_VERSION)
    {
        /*m_pProgressLabel->setString("no new version");

		bool bAlreadyInList = false;
		for(auto& package: m_CustomPackageList)
		{
			if (package.m_sCode.compare( m_pCodeEditBox->getText()) == 0)
				bAlreadyInList = true;
		}

		if (!bAlreadyInList)
		{
			// add new package to list
			CustomPackageInfo customPackageInfo;
			customPackageInfo.m_sCode = m_pCodeEditBox->getText();
			customPackageInfo.m_sPackageFolder = m_sResultFolder;
			AddNewPackageToList(customPackageInfo);

			SavePackageListToFile();
		}*/

		if (m_DownloadPackageCompleteCallback != nullptr)
		{
			CSPackageInfo packageInfo = CSPackageTable::getCSPackageInfo(m_sResultFolder);
			if (packageInfo.sPackageId.size() == 0)
			{
				packageInfo.sPackageId = m_sResultFolder;
				packageInfo.sPackageCode = m_sCode;		
				GetPackageInfoFromServer(packageInfo);		

				CSPackageTable::updateCSPackage(packageInfo);
			}
			m_DownloadPackageCompleteCallback(packageInfo);
		}

		//startCustomGame();
    }
	else if (errorCode == AssetsManager::ErrorCode::NETWORK)
    {
        //m_pProgressLabel->setString("network error");		

		auto errorPopup = WaitingNode::createLayout("Network error!");
		m_pParentNode->addChild(errorPopup);
		errorPopup->runAction(
			Sequence::createWithTwoActions(
				DelayTime::create(2.5f),
				RemoveSelf::create()));
    }
	else if (errorCode == AssetsManager::ErrorCode::UNCOMPRESS)
    {
        //m_pProgressLabel->setString("Invalid compress file!");		

		auto errorPopup = WaitingNode::createLayout("Invalid package data!");
		m_pParentNode->addChild(errorPopup);
		errorPopup->runAction(
			Sequence::createWithTwoActions(
				DelayTime::create(2.5f),
				RemoveSelf::create()));
    }
	
	//m_pMenu->setEnabled(true);
	m_bIsDownloadingPackage = false;
}

void CustomPackageDownloadManager::onProgress(int percent)
{
	if (percent < 0)
		percent = 0;

	char sText[30];
	sprintf(sText, "Downloading %d%%", percent);
	m_pNotificationPopup->UpdateTitle(sText);
}

void CustomPackageDownloadManager::onSuccess()
{
	if (m_pNotificationPopup != NULL)
	{
		m_pNotificationPopup->removeFromParentAndCleanup(true);
		m_pNotificationPopup = NULL;
	}


	m_bIsDownloadingPackage = false;
	//m_pProgressLabel->setString("download completed!");

	//m_pMenu->setEnabled(true);

	// notify server for tracking purpose
	char sTrackingURL[256];
	const char* sBaseUrl = "http://vocab.kiss-concept.com/packages";
	sprintf(sTrackingURL, "%s/track/%s",sBaseUrl, m_sCode.c_str());
	callTrackingURL(string(sTrackingURL));

	// add new package to list
	/*CustomPackageInfo customPackageInfo;
	customPackageInfo.m_sCode = m_pCodeEditBox->getText();
	customPackageInfo.m_sPackageFolder = m_sResultFolder;
	AddNewPackageToList(customPackageInfo);

	SavePackageListToFile();*/

	//startCustomGame();

	if (m_DownloadPackageCompleteCallback != nullptr)
	{
		CSPackageInfo packageInfo;
		packageInfo.sPackageId = m_sResultFolder;
		packageInfo.sPackageCode = m_sCode;		
		GetPackageInfoFromServer(packageInfo);		

		// save info to db
		CSPackageTable::updateCSPackage(packageInfo);

		m_DownloadPackageCompleteCallback(packageInfo);
	}
}

void CustomPackageDownloadManager::GetPackageInfoFromServer(CSPackageInfo& package)
{	     
	std::string sData;
	char sInfoUrl[256];
	const char* sBaseUrl = "http://vocab.kiss-concept.com/packages";
	sprintf(sInfoUrl, "%s/getInfo/%s",sBaseUrl, package.sPackageCode.c_str());

    void* _curl = curl_easy_init();
    if (! _curl)
    {
        CCLOG("can not init curl");
        return;
    }
       
    	
    CURLcode res;
    curl_easy_setopt(_curl, CURLOPT_URL, sInfoUrl);
    curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, getStringData);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &sData);	
    curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, 3);
    res = curl_easy_perform(_curl);
    long http_code = 0;
	if (res == 0)
		curl_easy_getinfo (_curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (res != 0 || http_code != 200)
    {        
		CCLOG("url is invalid, error code is %d, http code is %d", res, http_code);
        curl_easy_cleanup(_curl);
        return;
    }         
	else // parse data
	{
		auto pJsonDict = new cs::JsonDictionary();
		pJsonDict->initWithDescription(sData.c_str());

		package.iTotalWord = pJsonDict->getItemIntValue("total", 0);		
		package.sPackageName = pJsonDict->getItemStringValue("name");		

		auto pAuthorSubDict = pJsonDict->getSubDictionary("author");
		package.sCreatedBy = pAuthorSubDict->getItemStringValue("fullname");
	}    
}






#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include <dirent.h>
#include <sys/stat.h>
#endif

void createDownloadedDir(const string& pathToSave)
{    
    // Create the folder if it doesn't exist
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    DIR *pDir = NULL;
    
    pDir = opendir (pathToSave.c_str());
    if (! pDir)
    {
        mkdir(pathToSave.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    }
#else
	if ((GetFileAttributesA(pathToSave.c_str())) == INVALID_FILE_ATTRIBUTES)
	{
		CreateDirectoryA(pathToSave.c_str(), 0);
	}
#endif
}


static size_t getStringData(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    string *data = (string*)userdata;
    data->append((char*)ptr, size * nmemb);
    
    return (size * nmemb);
}

void callTrackingURL(const std::string& sTrackingUrl)
{
	void* _curl = curl_easy_init();
    if (! _curl)
    {
        CCLOG("can not init curl");
        return;
    }
        	
    CURLcode res;
    curl_easy_setopt(_curl, CURLOPT_URL, sTrackingUrl.c_str());
    //curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);    
    curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, 3);
    res = curl_easy_perform(_curl);
}

bool checkAndGetResultFolderName(const std::string& sOuputFolderUrl, std::string& sFolderName)
{
    if (sOuputFolderUrl.size() == 0) return false;
    
    void* _curl = curl_easy_init();
    if (! _curl)
    {
        CCLOG("can not init curl");
        return false;
    }
    
    // Clear _version before assign new value.
	sFolderName.clear();
    	
    CURLcode res;
    curl_easy_setopt(_curl, CURLOPT_URL, sOuputFolderUrl.c_str());
    curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, getStringData);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &sFolderName);	
    curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, 3);
    res = curl_easy_perform(_curl);
    long http_code = 0;
	if (res == 0)
		curl_easy_getinfo (_curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (res != 0 || http_code != 200)
    {        
		CCLOG("url is invalid, error code is %d, http code is %d", res, http_code);
        curl_easy_cleanup(_curl);
        return false;
    }             
    return true;
}

