#include "CustomPackageDownloaderNode.h"
#include "GameConfigManager.h"
#include "ButtonManagerNode.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

CustomPackageDownloaderNode* CustomPackageDownloaderNode::create()
{
	CustomPackageDownloaderNode* renderNode = new CustomPackageDownloaderNode();
	if( renderNode)
	{				
		renderNode->autorelease();
		renderNode->init();
		return renderNode;
	}

	CC_SAFE_DELETE(renderNode);
	return NULL;
}

CustomPackageDownloaderNode::CustomPackageDownloaderNode()
{
	m_pAssetManager = NULL;
}

CustomPackageDownloaderNode::~CustomPackageDownloaderNode()
{
	//if (m_pAssetManager)
		//delete m_pAssetManager;
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

bool CustomPackageDownloaderNode::init()
{	
	auto pBackground = Sprite::create("CustomModeGUI/panel.png");
	pBackground->setAnchorPoint(Point(0,0));		
	this->addChild(pBackground);

	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event* event) { return true; };
	listener->onTouchMoved = [this](Touch* touch, Event* event) { };
	listener->onTouchEnded = [this](Touch* touch, Event* event) { };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, this);

	
	m_pCodeEditBox = EditBox::create(Size(320.f, 50.f), Scale9Sprite::create("CustomModeGUI/green_edit.png"));
	m_pCodeEditBox->setFont("Arial", 28);
    m_pCodeEditBox->setPosition( Point(60.f, 260.f)); //Point(visibleOrigin.x+visibleSize.width/2, visibleOrigin.y+visibleSize.height/4));
    m_pCodeEditBox->setAnchorPoint(Point(0, 0.5f));
    m_pCodeEditBox->setPlaceHolder("Code");
	m_pCodeEditBox->setInputMode(EditBox::InputMode::ANY);
    //editCode->setDelegate(this);
    addChild(m_pCodeEditBox);

	
	auto pMenu1Sprite = Sprite::create("CustomModeGUI/OK.png");
	auto pButtonMenu1 = ButtonNode::createButtonSprite( pMenu1Sprite, CC_CALLBACK_1(CustomPackageDownloaderNode::startDownloadCallback, this));
	pButtonMenu1->setPosition( Point(160.f, 100.f));

	auto pMenu2Sprite = Sprite::create("CustomModeGUI/EXIT.PNG");
	auto pButtonMenu2 = ButtonNode::createButtonSprite( pMenu2Sprite, CC_CALLBACK_1(CustomPackageDownloaderNode::exitCallback, this));
	pButtonMenu2->setPosition( Point(420.f, 100.f));

	auto pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(pButtonManagerNode);
	pButtonManagerNode->addButtonNode(pButtonMenu1);
	pButtonManagerNode->addButtonNode(pButtonMenu2);

	/*auto menu1Item = MenuItemImage::create("CustomModeGUI/OK.png", "CustomModeGUI/OK.png", CC_CALLBACK_1(CustomPackageDownloaderNode::startDownloadCallback, this));
	menu1Item->setPosition( Point(160.f, 100.f));


	auto menu2Item = MenuItemImage::create("CustomModeGUI/EXIT.PNG","CustomModeGUI/EXIT.PNG", CC_CALLBACK_1(CustomPackageDownloaderNode::exitCallback, this));
	menu2Item->setPosition( Point(420.f, 100.f));	

	m_pMenu = Menu::create( menu1Item, menu2Item, NULL);
	m_pMenu->setPosition(Point::ZERO);
	this->addChild(m_pMenu);
	*/


	m_pProgressLabel = LabelTTF::create("", "Arial", 30);	
    m_pProgressLabel->setPosition(Point(200, 200));
    addChild(m_pProgressLabel);

	//init folder to save data
	m_sPathToSave = FileUtils::getInstance()->getWritablePath();
	m_sPathToSave += "downloaded";	
	createDownloadedDir(m_sPathToSave);

	// add tableView to contain old downloaded package
	m_pTableView = TableView::create(this, Size(250.f, 80.f));
	m_pTableView->setDirection(ScrollView::Direction::HORIZONTAL);
    m_pTableView->setPosition(Point(60, 300));
    m_pTableView->setDelegate(this);
    this->addChild(m_pTableView);

	LoadPackageListFromFile();
    m_pTableView->reloadData();
	

	m_bIsDownloadingPackage = false;

	// try to load custom mode config
	GameConfigManager::getInstance()->GetTimeModeDemoConfig();

	return true;
}

void CustomPackageDownloaderNode::AddNewPackageToList(const CustomPackageInfo& customPackageInfo)
{
	for(auto& package : m_CustomPackageList)
		if (package.m_sCode.compare(customPackageInfo.m_sCode) == 0)
		{
			return;
		}

	m_CustomPackageList.push_back( customPackageInfo);
	m_pTableView->reloadData();
}

void CustomPackageDownloaderNode::SavePackageListToFile()
{
	string sConfigFile = m_sPathToSave + "/packageList.data";
	ofstream ostream(sConfigFile);
	if (ostream.is_open())
	{
		ostream << m_CustomPackageList.size();		
		ostream.put(' ');
		for(auto& package : m_CustomPackageList)
		{
			ostream << package.m_sCode;
			ostream.put(' ');
			ostream << package.m_sPackageFolder;
			ostream.put(' ');
		}

		ostream.close();
	}
}

void CustomPackageDownloaderNode::LoadPackageListFromFile()
{
	string sConfigFile = m_sPathToSave + "/packageList.data";
	ifstream istream(sConfigFile);
	if (istream.is_open())
	{
		int iPackageCount;
		istream >> iPackageCount;
		CustomPackageInfo package;
		for(int i=0; i< iPackageCount ; i++)
		{

			istream >> package.m_sCode;
			istream >> package.m_sPackageFolder;

			m_CustomPackageList.push_back(package);
		}

		istream.close();
	}	
}

bool checkAndGetResultFolderName(const std::string& sOuputFolderUrl, std::string& sFolderName);
void callTrackingURL(const std::string& sTrackingUrl);

void CustomPackageDownloaderNode::startDownloadCallback(Object* sender)
{	
	if (m_bIsDownloadingPackage)
		return;
	m_bIsDownloadingPackage = true;

	if(m_pCodeEditBox->getText()[0] == 0)
		return;

	//m_pMenu->setEnabled(false);

	const char* sBaseUrl = "http://vocab.kiss-concept.com/packages";
	char sVersionUrl[256], sFolderUrl[256];	
	string sDownloadUrl;
	sprintf(sFolderUrl, "%s/folder/%s",sBaseUrl, m_pCodeEditBox->getText());

	std::string sFolderName;
	if (checkAndGetResultFolderName( string(sFolderUrl), m_sResultFolder) && m_sResultFolder.size()!=0)
	{	
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
		sprintf(sVersionUrl, "%s/version/%s",sBaseUrl, m_pCodeEditBox->getText());
		m_pAssetManager = new AssetsManager( sDownloadUrl.c_str(), sVersionUrl, m_sPathToSave.c_str());
		m_pAssetManager->setDelegate(this);
		m_pAssetManager->setConnectionTimeout(3);
		addChild(m_pAssetManager);
		m_pAssetManager->release();

		m_pAssetManager->update();
	}
	else
	{
		m_pProgressLabel->setString("Code is invalid!!");
		//m_pMenu->setEnabled(true);

		m_bIsDownloadingPackage = false;
	}
}

#include "GameWordManager.h"
#include "HelloWorldScene.h"

void CustomPackageDownloaderNode::startCustomGame()
{	
	auto& timeModeConfig = GameConfigManager::getInstance()->GetTimeModeDemoConfig();
	timeModeConfig.m_WordIndexList.clear();
	timeModeConfig.m_WordCollectedCountList.clear();

	std::string sPackageID = m_sResultFolder;
	m_sResultFolder.insert(0, "/");
	m_sResultFolder.insert(0, m_sPathToSave);

	timeModeConfig.m_sCustomPackageID = sPackageID;

	int iWordPackageIndex = GameWordManager::getInstance()->AddAndLoadCustomPackageToList( sPackageID, m_sResultFolder);
	auto& wordList = GameWordManager::getInstance()->GetWordList();
	for(int iWordIndex=0; iWordIndex < wordList.size(); iWordIndex++)
	{
		if (wordList[iWordIndex].m_iPackageIndex == iWordPackageIndex)
		{
			timeModeConfig.m_WordIndexList.push_back( iWordIndex);
			timeModeConfig.m_WordCollectedCountList.push_back(0);
		}
	}

	// start game
	GameWordManager::getInstance()->GenerateWordForNewLevelOfTimeMode(&timeModeConfig, true);	

	auto scene = HelloWorld::createScene( _GMT_TIME_MODE_);
	Director::getInstance()->replaceScene(scene);
}

void CustomPackageDownloaderNode::exitCallback(Object* sender)
{
	//log("override restart!");
	this->removeFromParentAndCleanup(true);
}


void CustomPackageDownloaderNode::onError(cocos2d::extension::AssetsManager::ErrorCode errorCode)
{
	if (errorCode == AssetsManager::ErrorCode::NO_NEW_VERSION)
    {
        m_pProgressLabel->setString("no new version");

		// add new package to list
		CustomPackageInfo customPackageInfo;
		customPackageInfo.m_sCode = m_pCodeEditBox->getText();
		customPackageInfo.m_sPackageFolder = m_sResultFolder;
		AddNewPackageToList(customPackageInfo);

		SavePackageListToFile();

		//startCustomGame();
    }
	else if (errorCode == AssetsManager::ErrorCode::NETWORK)
    {
        m_pProgressLabel->setString("network error");		
    }
	else if (errorCode == AssetsManager::ErrorCode::UNCOMPRESS)
    {
        m_pProgressLabel->setString("Invalid compress file!");		
    }
	
	//m_pMenu->setEnabled(true);
	m_bIsDownloadingPackage = false;
}
 
void CustomPackageDownloaderNode::onProgress(int percent)
{
	char progress[20];
    snprintf(progress, 20, "downloading %d%%", percent);
    m_pProgressLabel->setString(progress);
}
 
void CustomPackageDownloaderNode::onSuccess()
{
	m_bIsDownloadingPackage = false;
	m_pProgressLabel->setString("download completed!");

	//m_pMenu->setEnabled(true);

	// notify server for tracking purpose
	char sTrackingURL[256];
	const char* sBaseUrl = "http://vocab.kiss-concept.com/packages";
	sprintf(sTrackingURL, "%s/track/%s",sBaseUrl, m_pCodeEditBox->getText());
	callTrackingURL(string(sTrackingURL));

	// add new package to list
	CustomPackageInfo customPackageInfo;
	customPackageInfo.m_sCode = m_pCodeEditBox->getText();
	customPackageInfo.m_sPackageFolder = m_sResultFolder;
	AddNewPackageToList(customPackageInfo);

	SavePackageListToFile();

	startCustomGame();
}



void CustomPackageDownloaderNode::tableCellTouched(TableView* table, TableViewCell* cell)
{
    //CCLOG("cell touched at index: %i", cell->getIdx());
	CustomPackageInfo packageInfo = m_CustomPackageList[cell->getIdx()];
	m_sResultFolder = packageInfo.m_sPackageFolder;

	startCustomGame();
}

Size CustomPackageDownloaderNode::tableCellSizeForIndex(TableView *table, unsigned int idx)
{
    if (idx == 2) {
        return Size(100, 100);
    }
    return Size(80, 80);
}

TableViewCell* CustomPackageDownloaderNode::tableCellAtIndex(TableView *table, unsigned int idx)
{
    //auto string = String::createWithFormat("%d", idx);
    TableViewCell *cell = table->dequeueCell();
    if (!cell) {
		cell = new TableViewCell();
        cell->autorelease();        
				
		auto sprite = Sprite::create("CustomModeGUI/YellowSquare.png");
        sprite->setAnchorPoint(Point::ZERO);
        sprite->setPosition(Point(0, 0));
		//sprite->setScale( 0.65f);
        cell->addChild(sprite);

		auto label = LabelTTF::create(m_CustomPackageList[idx].m_sCode.c_str(), "Helvetica", 20.0);
        label->setPosition(Point::ZERO);
		label->setAnchorPoint(Point::ZERO);
        label->setTag(123);
        cell->addChild(label);
    }
    else
    {
        auto label = (LabelTTF*)cell->getChildByTag(123);
        label->setString(m_CustomPackageList[idx].m_sCode.c_str()); //string->getCString());
    }


    return cell;
}

unsigned int CustomPackageDownloaderNode::numberOfCellsInTableView(TableView *table)
{
    //return 20;
	return m_CustomPackageList.size();
}





#include <curl/curl.h>
#include <curl/easy.h>

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