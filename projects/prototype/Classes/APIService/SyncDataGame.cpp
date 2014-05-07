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
#include "SystemEventHandle.h"
#include "Database\CSPackageTable.h"
#include "Database\CSWordTable.h"

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
	m_bIsFinishSync = true;
}

SyncDataGame::~SyncDataGame()
{

}

void SyncDataGame::runSyncDataGame()
{
	if (m_bIsFinishSync)
	{
		// Load data from database
		String sJsonData = "data={\"sync\":{";

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

		CCLOG("Sync %s", sJsonData.getCString());
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

		m_bIsFinishSync = false;
	}
}

void SyncDataGame::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
	std::string sKey = "";
	String strData = "";
	sKey.append(response->getHttpRequest()->getTag());
	std::vector<std::string> header = response->getHttpRequest()->getHeaders();
	bool bResult = true;

	if (sKey == "SyncGameData")
	{
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
				CCLOG("Sync %s", strData.getCString());
				cs::JsonDictionary *pJsonData = pJsonDict->getSubDictionary("data");
				if(pJsonData != NULL)
				{
					if (pJsonData->getItemBoolvalue("result", false))
					{
						cs::JsonDictionary *pJsonSync = pJsonData->getSubDictionary("sync");
						if (pJsonSync != NULL)
						{
							int iVersion = pJsonSync->getItemIntValue("Version", 0);
							if (!UserTable::getInstance()->updateDataSyncUser(pJsonSync, iVersion))
								bResult = false;
				
							if(!ChapterTable::getInstance()->updateDataSyncChapters(pJsonSync, iVersion))
								bResult = false;

							if(!LevelTable::getInstance()->updateDataSyncLevels(pJsonSync, iVersion))
								bResult = false;

							if(!WordTable::getInstance()->updateDataSyncWords(pJsonSync, iVersion))
								bResult = false;

							if(!WordTable::getInstance()->updateDataSyncMapChapterWords(pJsonSync, iVersion))
								bResult = false;

							if(bResult)
							{
								VersionInfo versionInfo = VersionTable::getInstance()->getVersionInfo();
								versionInfo.iVersionSync = iVersion;
								VersionTable::getInstance()->updateVersion(versionInfo);
							}
						}
						else
							bResult = false;
					}
					else
						bResult = false;
				}
				else
					bResult = false;
			}
			else
				bResult = false;
		}
		else
			bResult = false;

		m_bIsFinishSync = true;
		SystemEventHandle::getInstance()->onGameSyncCompleted(bResult);
	}
	else if (sKey == "PushDataCustomGameMode")
	{
		if (response)
		{
			if (response->isSucceed()) 
			{
				std::vector<char> *buffer = response->getResponseData();
			
				for (unsigned int i = 0; i < buffer->size(); i++)
				{
					strData.appendWithFormat("%c", (*buffer)[i]);
				}
				//CCLOG("Sync %s", strData.getCString());
			}
		}
	}
}

void SyncDataGame::pushDataCustomGameMode(const std::string& sPackageId)
{
	// Load data from database
	String sJsonData = "data={\"data\":{";

	UserInfo userInfo = UserTable::getInstance()->getUserInfo();
	sJsonData.append("\"User\":{");
	
	sJsonData.appendWithFormat("\"UserId\":\"%s\",", userInfo.sUserIdentifier.c_str());
	sJsonData.appendWithFormat("\"UserToken\":\"%s\",", userInfo.sUserToken.c_str());
	sJsonData.appendWithFormat("\"DeviceId\":\"%s\"", userInfo.sDeviceId.c_str());
	sJsonData.append("},");

	CSPackageInfo csPackageInfo = CSPackageTable::getInstance()->getCSPackageInfo(sPackageId);
	sJsonData.append("\"Package\":{");
	sJsonData.appendWithFormat("\"PackageId\":\"%s\"", csPackageInfo.sPackageId.c_str());
	sJsonData.append("},");

	std::vector<CSWordInfo> csWords = CSWordTable::getInstance()->getAllCSWordsForPackage(sPackageId);
	sJsonData.append("\"WordList\":[");
	int iSize = csWords.size();
	for (int iIndex=0; iIndex<iSize; iIndex++)
	{
		sJsonData.append("{");
		sJsonData.appendWithFormat("\"WordId\":\"%s\",", csWords[iIndex].sCSWordId.c_str());
		sJsonData.appendWithFormat("\"CollectedCount\":\"%d\"", csWords[iIndex].iCollectedCount);

		if (iIndex == iSize-1)
			sJsonData.append("}");
		else
			sJsonData.append("},");
	}
	sJsonData.append("]");
	
	sJsonData.append("}}");

	//CCLOG("Sync %s", sJsonData.getCString());
	//CCLOG("%d", sJsonData.length());
	
	// Post data to server
	String strURL = _CONSTANT_URL_;
	strURL.append("api-custom/pushData");
	m_pRequest = new HttpRequest();
	m_pRequest->setUrl(strURL.getCString());
	m_pRequest->setRequestType(HttpRequest::Type::POST);
	m_pRequest->setTag("PushDataCustomGameMode");
	m_pRequest->setRequestData(sJsonData.getCString(), sJsonData.length());
	m_pRequest->setResponseCallback(this, httpresponse_selector(SyncDataGame::onHttpRequestCompleted));

	m_pClient->send(m_pRequest);
	m_pRequest->release();
	m_pRequest = NULL;

}