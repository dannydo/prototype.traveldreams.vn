#include "UserService.h"
#include "Constants.h"
#include "Database\UserTable.h"
#include "SystemEventHandle.h"

USING_NS_CC;
USING_NS_CC_EXT;

UserService* UserService::m_UserService = NULL;
HttpRequest* UserService::m_pRequest = NULL;
HttpClient* UserService::m_pClient = NULL;

UserService::UserService()
{
    //m_pRequest = new HttpRequest();
    m_pRequest = NULL;
    m_pClient = HttpClient::getInstance();
}

void UserService::releaseInstance()
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

    if (m_UserService == NULL)
        delete m_UserService;
    m_UserService = NULL;
}

UserService* UserService::getInstance()
{
    if (m_UserService == NULL) {
        m_UserService = new UserService();
        m_UserService->m_iCurrentLevel = -1;
        m_UserService->m_sCurrentChapter = "";
        m_UserService->m_sDataLeaderBoard = "";

        m_UserService->m_sCurrentPackageId = "";
        m_UserService->m_sDataLeaderBoardAdvanceMode = "";
    }
    return m_UserService;
}

void UserService::checkUserFacebook(const std::string strAccessToken, const int& iUserId, const int& iTag)
{
    String strURL = _CONSTANT_URL_;
    strURL.append("api/checkUserFacebook/");
    strURL.append(strAccessToken);
    strURL.appendWithFormat("/%d", iUserId);
    //CCLOG("%s", strURL.getCString());

    m_pRequest = new HttpRequest();
    m_pRequest->setUrl(strURL.getCString());
    m_pRequest->setRequestType(HttpRequest::Type::GET);

    String sTag = "CheckUserFacebook";
    sTag.appendWithFormat("_%d", iTag);
    m_pRequest->setTag(sTag.getCString());

    m_pRequest->setResponseCallback(this, httpresponse_selector(UserService::onCheckUserFacebookCompleted));
    m_pClient->send(m_pRequest);
    m_pClient->setTimeoutForConnect(10);
    m_pClient->setTimeoutForRead(10);
    m_pRequest->release();
    m_pRequest = NULL;
}

void UserService::getLeaderBoardLevel(const std::string& sChapterId, const int& iLevel, const int& iTag)
{
    String sTag = "LeaderBoardLevel";
    sTag.appendWithFormat("_%d", iTag);

    if (m_iCurrentLevel != iLevel || m_sCurrentChapter != sChapterId)
    {
        m_iCurrentLevel = iLevel;
        m_sCurrentChapter = sChapterId;
    }
    else if (m_sDataLeaderBoard != "")
    {
        for(auto callBack : m_callBackList)
        {
            callBack->resultHttpRequestCompleted(this->parseStringToJson(m_sDataLeaderBoard.c_str()), sTag.getCString());
        }

        return;
    }

    UserInfo userInfo = UserTable::getInstance()->getUserInfo();
    String strURL = _CONSTANT_URL_;
    strURL.append("api/getLeaderBoard/");
    strURL.appendWithFormat("%s/", m_sCurrentChapter.c_str());
    strURL.appendWithFormat("%d/", m_iCurrentLevel);
    //strURL.append("878821822132320");
    strURL.append(userInfo.sFacebookId);

    m_pRequest = new HttpRequest();
    m_pRequest->setUrl(strURL.getCString());
    m_pRequest->setRequestType(HttpRequest::Type::GET);
    m_pRequest->setTag(sTag.getCString());

    m_pRequest->setResponseCallback(this, httpresponse_selector(UserService::onHttpRequestCompleted));
    m_pClient->send(m_pRequest);
    m_pClient->setTimeoutForConnect(10);
    m_pClient->setTimeoutForRead(10);
    m_pRequest->release();
    m_pRequest = NULL;
}

void UserService::disableUser(const std::string& sUserToken)
{
    String strURL = _CONSTANT_URL_;
    strURL.append("api/disableUser/");
    strURL.appendWithFormat("%s", sUserToken.c_str());
    CCLOG("%s", strURL.getCString());

    m_pRequest = new HttpRequest();
    m_pRequest->setUrl(strURL.getCString());
    m_pRequest->setRequestType(HttpRequest::Type::GET);
    m_pRequest->setTag("DisableUser");

    m_pClient->send(m_pRequest);
    m_pClient->setTimeoutForConnect(10);
    m_pClient->setTimeoutForRead(10);
    m_pRequest->release();
    m_pRequest = NULL;
}

void UserService::getLeaderBoardAdvanceMode(const std::string& sPackageId, const int& iTag)
{
    String sTag = "LeaderBoardAdvanceMode";
    sTag.appendWithFormat("_%d", iTag);

    if (m_sCurrentPackageId != sPackageId)
    {
        m_sCurrentPackageId = sPackageId;
    }
    else if (m_sDataLeaderBoardAdvanceMode != "")
    {
        for(auto callBack : m_callBackList)
        {
            callBack->resultHttpRequestCompleted(this->parseStringToJson(m_sDataLeaderBoardAdvanceMode.c_str()), sTag.getCString());
        }

        return;
    }

    UserInfo userInfo = UserTable::getInstance()->getUserInfo();
    String strURL = _CONSTANT_URL_;
    strURL.append("api-custom/getLeaderBoard/");
    //strURL.appendWithFormat("%s/", "878821822132320");
    strURL.appendWithFormat("%s/", userInfo.sFacebookId.c_str());
    strURL.append(m_sCurrentPackageId);

    m_pRequest = new HttpRequest();
    m_pRequest->setUrl(strURL.getCString());
    m_pRequest->setRequestType(HttpRequest::Type::GET);
    m_pRequest->setTag(sTag.getCString());

    m_pRequest->setResponseCallback(this, httpresponse_selector(UserService::onHttpRequestCompleted));
    m_pClient->send(m_pRequest);
    m_pClient->setTimeoutForConnect(10);
    m_pClient->setTimeoutForRead(10);
    m_pRequest->release();
    m_pRequest = NULL;
}

void UserService::getVersionGame()
{
    String strURL = _CONSTANT_URL_;
    strURL.append("api/getVersionGame/");

    m_pRequest = new HttpRequest();
    m_pRequest->setUrl(strURL.getCString());
    m_pRequest->setRequestType(HttpRequest::Type::GET);
    m_pRequest->setTag("GetVersionGame");

    m_pRequest->setResponseCallback(this, httpresponse_selector(UserService::onGetVersionGameCompleted));
    m_pClient->send(m_pRequest);
    m_pClient->setTimeoutForConnect(1);
    m_pClient->setTimeoutForRead(1);
    m_pRequest->release();
    m_pRequest = NULL;
}

void UserService::onCheckUserFacebookCompleted(HttpClient *sender, HttpResponse *response)
{
    std::string sKey = "";
    string strData = "";
    sKey.append(response->getHttpRequest()->getTag());

    if (response)
    {
        if (response->isSucceed())
        {
            std::vector<char> *buffer = response->getResponseData();
            string strDataTemp(buffer->begin(), buffer->end());
            strData = strDataTemp;
        }
    }

    //MessageBox(strData.getCString(), "Error");
    SystemEventHandle::getInstance()->onCheckUserFacebookResult(this->parseStringToJson(strData.c_str()), sKey);
}

void UserService::onGetVersionGameCompleted(HttpClient *sender, HttpResponse *response)
{
    std::string sKey = "";
    string strData = "";
    sKey.append(response->getHttpRequest()->getTag());

    if (response)
    {
        if (response->isSucceed())
        {
            std::vector<char> *buffer = response->getResponseData();
            if ( buffer->size() > 0)
            {
                string strData(buffer->begin(), buffer->end());

                cs::JsonDictionary *pJsonDict = new cs::JsonDictionary();
                pJsonDict->initWithDescription(strData.c_str());

                if (pJsonDict != NULL)
                {
                    cs::JsonDictionary *pJsonData = pJsonDict->getSubDictionary("data");
                    if(pJsonData != NULL)
                    {
                        if (pJsonData->getItemBoolvalue("result", false))
                        {
                            cs::JsonDictionary *pJsonList = pJsonData->getSubDictionary("list");
                            if (pJsonList != NULL)
                            {
                                int iVersionGame = pJsonList->getItemIntValue("VersionGame", 0);
                                UserDefault::getInstance()->setIntegerForKey("VersionGame", iVersionGame);
                            }
                        }
                    }
                }
            }
        }
    }
}

void UserService::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
    std::string sKey = "";
    std::string strData = "";

    sKey.append(response->getHttpRequest()->getTag());
    if (response)
    {
        if (response->isSucceed())
        {
            std::vector<char> *buffer = response->getResponseData();
            string strDataTemp(buffer->begin(), buffer->end());
            strData = strDataTemp;
        }
    }

    if (strstr(sKey.c_str(), "LeaderBoardLevel") != NULL)
    {
        m_sDataLeaderBoard = strData;
    }
    else if (strstr(sKey.c_str(), "LeaderBoardAdvanceMode") != NULL)
    {
        m_sDataLeaderBoardAdvanceMode = strData;
    }

    for(auto callBack : m_callBackList)
    {
        //if(((Object*)callBack)->retainCount() == 0)
        //    continue;

        callBack->resultHttpRequestCompleted(this->parseStringToJson(strData.c_str()), sKey);
    }
}

void UserService::addCallBackList(InterfaceService* callBack)
{
    m_callBackList.push_back(callBack);
}

void UserService::removeCallBackList(InterfaceService *callBack)
{
    int iIndex;
    for(iIndex=0; iIndex<m_callBackList.size(); iIndex++)
    {
        if(m_callBackList[iIndex] == callBack)
            break;
    }

    if(iIndex < m_callBackList.size())
    {
        m_callBackList[iIndex] = m_callBackList[m_callBackList.size()-1];
        m_callBackList[m_callBackList.size()-1] = callBack;

        m_callBackList.pop_back();
    }
}

cs::JsonDictionary* UserService::parseStringToJson(const char* sJson)
{
    cs::JsonDictionary *pJsonDict = new cs::JsonDictionary();
    pJsonDict->initWithDescription(sJson);

    return pJsonDict;
}
