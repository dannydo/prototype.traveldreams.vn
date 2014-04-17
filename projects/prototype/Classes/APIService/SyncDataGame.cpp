#include "SyncDataGame.h"
#include "Constants.h"
#include "Database\ChapterTable.h"
#include "Database\LevelTable.h"
#include "Database\PowerUpTable.h"
#include "Database\TransactionTable.h"
#include "Database\UnlockChapterTable.h"
#include "Database\UserTable.h"
#include "Database\VersionTable.h"
#include "Database\WordTable.h"
#include "Database\InitDatabase.h"

USING_NS_CC; 
USING_NS_CC_EXT;

SyncDataGame* SyncDataGame::m_SyncDataGame = NULL;
HttpRequest* SyncDataGame::m_pRequest = NULL;
HttpClient* SyncDataGame::m_pClient = NULL;

void SyncDataGame::releaseInstance()
{
	if (m_pRequest == NULL)
		delete m_pRequest;
	else
		m_pRequest->release();
	m_pRequest = NULL;

	if (m_pClient != NULL)
		m_pClient->release();
	m_pClient = NULL;

	if (m_SyncDataGame == NULL)
		delete m_SyncDataGame;
	m_SyncDataGame = NULL;
}

SyncDataGame* SyncDataGame::getInstance()
{
	if (m_SyncDataGame == NULL) {
		m_SyncDataGame = new SyncDataGame();
	}
	return m_SyncDataGame;
}

SyncDataGame::SyncDataGame()
{
	m_pRequest = new HttpRequest();
	m_pClient = HttpClient::getInstance();
	isFinishSync = false;
}

SyncDataGame::~SyncDataGame()
{

}

void SyncDataGame::runSyncDataGame()
{
	// Load data from database
	String sJsonData = "{\"sync\":{";

	std::string sJsonUser = UserTable::getInstance()->syncGetUser();
	sJsonData.append(sJsonUser);
	sJsonData.append(",");

	std::string sJsonChapters = ChapterTable::getInstance()->syncGetChapters();
	sJsonData.append(sJsonChapters);
	sJsonData.append(",");

	std::string sJsonLevels = LevelTable::getInstance()->syncGetLevels();
	sJsonData.append(sJsonLevels);
	sJsonData.append(",");

	std::string sJsonWords = WordTable::getInstance()->syncGetWords();
	sJsonData.append(sJsonWords);
	sJsonData.append(",");

	std::string sJsonMapChapterWords = WordTable::getInstance()->syncGetMapChapterWords();
	sJsonData.append(sJsonMapChapterWords);
	sJsonData.append(",");

	/*
	std::string sJsonPowerUps = PowerUpTable::getInstance()->syncGetPoverUps();
	sJsonData.append(sJsonPowerUps);
	sJsonData.append(",");

	std::string sJsonTransactions = TransactionTable::getInstance()->syncGetTransactions();
	sJsonData.append(sJsonTransactions);
	sJsonData.append(",");

	std::string sJsonUnlockChapters = UnlockChapterTable::getInstance()->syncGetUnlockChapters();
	sJsonData.append(sJsonUnlockChapters);
	sJsonData.append(",");
	*/

	sJsonData.appendWithFormat("\"Version\":%d", VersionTable::getInstance()->getVersionInfo().iVersionSync);
	sJsonData.append("}}");

	CCLOG("%s", sJsonData.getCString());
	//CCLOG("%d", sJsonData.length());

	
	// Post data to server
	String strURL = _CONSTANT_URL_;
	strURL.append("api/sync");
	m_pRequest = new HttpRequest();
	m_pRequest->setUrl(strURL.getCString());
	m_pRequest->setRequestType(HttpRequest::Type::POST);
	m_pRequest->setTag("SyncGameData");
	m_pRequest->setRequestData(sJsonData.getCString(), sJsonData.length());
	m_pRequest->setResponseCallback(this, httpresponse_selector(SyncDataGame::onHttpRequestCompleted));

	m_pClient->send(m_pRequest);
	m_pRequest->release();
	m_pRequest = NULL;

	isFinishSync = false;
}

void SyncDataGame::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
	std::string sKey = "";
	String strData = "";
	sKey.append(response->getHttpRequest()->getTag());
	std::vector<std::string> header = response->getHttpRequest()->getHeaders();

	if (response)
    {
		if (response->isSucceed()) 
		{
			
			std::vector<char> *buffer = response->getResponseData();
			
			for (unsigned int i = 0; i < buffer->size(); i++)
			{
				strData.appendWithFormat("%c", (*buffer)[i]);
			}

			cs::JsonDictionary *pJsonDict = new cs::JsonDictionary();
			pJsonDict->initWithDescription(strData.getCString());
			cs::JsonDictionary *pJsonSync = pJsonDict->getSubDictionary("sync");
			if (pJsonSync != NULL)
			{
				int iVersion = pJsonSync->getItemIntValue("Version", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);
				VersionInfo versionInfo = VersionTable::getInstance()->getVersionInfo();
				versionInfo.iVersionSync = iVersion;
				VersionTable::getInstance()->updateVersion(versionInfo);

				UserTable::getInstance()->updateDataSyncUser(pJsonSync, iVersion);
				ChapterTable::getInstance()->updateDataSyncChapters(pJsonSync, iVersion);
				LevelTable::getInstance()->updateDataSyncLevels(pJsonSync, iVersion);
				WordTable::getInstance()->updateDataSyncWords(pJsonSync, iVersion);
				WordTable::getInstance()->updateDataSyncMapChapterWords(pJsonSync, iVersion);
			}
		}
	}

	isFinishSync = true;
}