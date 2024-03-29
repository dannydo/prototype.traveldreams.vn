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

    // note: should note release singleton directly!!!!
    //if (m_pClient != NULL)
    //m_pClient->release();
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
    m_pRequest = NULL;// new HttpRequest();
    m_pClient = HttpClient::getInstance();
    m_bIsFinishSync = true;
    m_iCountSync = 0;
}

SyncDataGame::~SyncDataGame()
{
}

void SyncDataGame::runSyncDataGame()
{
    if (m_bIsFinishSync && m_iCountSync < 2)
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

        //CCLOG("Sync %s", sJsonData.getCString());
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
        m_pClient->setTimeoutForConnect(10);
        m_pClient->setTimeoutForRead(10);
        m_pRequest->release();
        m_pRequest = NULL;

        m_bIsFinishSync = false;
    }
}

void SyncDataGame::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
    std::string sKey = "";
    sKey.append(response->getHttpRequest()->getTag());
    bool bResult = false;

    if (sKey == "SyncGameData")
    {
        if (response)
        {
            if (response->isSucceed())
            {
                std::vector<char> *buffer = response->getResponseData();

                string strData(buffer->begin(), buffer->end());

                cs::JsonDictionary *pJsonDict = new cs::JsonDictionary();
                pJsonDict->initWithDescription(strData.c_str());
                //CCLOG("Sync %s", strData.c_str());
                if (pJsonDict != NULL)
                {
                    cs::JsonDictionary *pJsonData = pJsonDict->getSubDictionary("data");
                    if(pJsonData != NULL)
                    {
                        if (pJsonData->getItemBoolvalue("result", false))
                        {
                            cs::JsonDictionary *pJsonSync = pJsonData->getSubDictionary("sync");
                            if (pJsonSync != NULL)
                            {
                                bResult = true;
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

                                    UserTable::getInstance()->haveGenerateNextChapter();
                                }
                            }
                        }
                        else
                        {
                            cs::JsonDictionary *pJsonList = pJsonData->getSubDictionary("list");
                            if (pJsonList != NULL)
                            {
                                std::string sKey = pJsonList->getItemStringValue("key");
                                if (sKey == "USER_NOT_EXIST");
                                {
                                    m_bIsFinishSync = true;

                                    UserInfo userInfo = UserTable::getInstance()->getUserInfo();
                                    userInfo.sUserToken = "";
                                    userInfo.sUserIdentifier = "";
                                    UserTable::getInstance()->updateUser(userInfo);

                                    VersionInfo versionInfo = VersionTable::getInstance()->getVersionInfo();
                                    versionInfo.iVersionSync = 0;
                                    VersionTable::getInstance()->updateVersion(versionInfo);

                                    if (m_iCountSync < 2)
                                    {
                                        m_iCountSync++;
                                        this->runSyncDataGame();
                                    }
                                    else
                                    {
                                        m_iCountSync = 0;
                                    }
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }

        m_bIsFinishSync = true;
        SystemEventHandle::getInstance()->onGameSyncCompleted(bResult);
    }
    else if (sKey == "PushDataCustomGameMode")
    {
        if (response)
        {
            if (response->isSucceed())
            {
                //std::vector<char> *buffer = response->getResponseData();
                //string strData(buffer->begin(), buffer->end());
                //CCLOG("Push %s", strData.getCString());
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

    CSPackageInfo csPackageInfo = CSPackageTable::getCSPackageInfo(sPackageId);
    sJsonData.append("\"Package\":{");
    sJsonData.appendWithFormat("\"PackageId\":\"%s\",", csPackageInfo.sPackageId.c_str());
    sJsonData.appendWithFormat("\"HighNumberStage\":%d", csPackageInfo.iStage);
    sJsonData.append("},");

    std::vector<CSWordInfo> csWords;
    CSWordTable::getAllCSWordsForPackage(csWords, sPackageId);
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

    //CCLOG("Push %s", sJsonData.getCString());
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
    m_pClient->setTimeoutForConnect(10);
    m_pClient->setTimeoutForRead(10);
    m_pRequest->release();
    m_pRequest = NULL;
}

void SyncDataGame::reloadDataAfterSync()
{
    if (UserDefault::getInstance()->getIntegerForKey("ReloadDataAfterSync", 0) == 1)
    {
        ChapterTable::getInstance()->refreshChapters();
        LevelTable::getInstance()->refeshChapterLevels();
        WordTable::getInstance()->refreshWordsForChapter();
    }

    UserDefault::getInstance()->setIntegerForKey("ReloadDataAfterSync", 0);
}
