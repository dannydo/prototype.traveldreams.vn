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
	m_pRequest = new HttpRequest();
	m_pClient = HttpClient::getInstance();
}

void UserService::releaseInstance()
{
	if (m_pRequest == NULL)
		delete m_pRequest;
	else
		m_pRequest->release();
	m_pRequest = NULL;

	if (m_pClient != NULL)
		m_pClient->release();
	m_pClient = NULL;

	if (m_UserService == NULL)
		delete m_UserService;
	m_UserService = NULL;
}

UserService* UserService::getInstance()
{
	if (m_UserService == NULL) {
		m_UserService = new UserService();
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
	m_pRequest->release();
	m_pRequest = NULL;
}

void UserService::getLeaderBoardLevel(const std::string& sChapterId, const int& iLevel, const int& iTag)
{
	UserInfo userInfo = UserTable::getInstance()->getUserInfo();
	String strURL = _CONSTANT_URL_;
	strURL.append("api/getLeaderBoard/");
	strURL.appendWithFormat("%s/", sChapterId.c_str());
	strURL.appendWithFormat("%d/", iLevel);
	strURL.append("1000000903936281");

	m_pRequest = new HttpRequest();
	m_pRequest->setUrl(strURL.getCString());
	m_pRequest->setRequestType(HttpRequest::Type::GET);

	String sTag = "LeaderBoardLevel";
	sTag.appendWithFormat("_%d", iTag);
	m_pRequest->setTag(sTag.getCString());

	m_pRequest->setResponseCallback(this, httpresponse_selector(UserService::onHttpRequestCompleted));
	m_pClient->send(m_pRequest);
	m_pRequest->release();
	m_pRequest = NULL;
}

void UserService::onCheckUserFacebookCompleted(HttpClient *sender, HttpResponse *response)
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
		}
	}

	//CCLOG("%s", strData.getCString());
	SystemEventHandle::getInstance()->onCheckUserFacebookResult(this->parseStringToJson(strData.getCString()), sKey);
	
}

void UserService::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
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
		}
    }

	for(auto callBack : m_callBackList)
	{
		callBack->resultHttpRequestCompleted(this->parseStringToJson(strData.getCString()), sKey);
	}
}

void UserService::addCallBackList(InterfaceService* callBack)
{
	m_callBackList.push_back(callBack);
}

void UserService::removeCallBackList(InterfaceService *callBack)
{
	for(int iIndex=0; iIndex<m_callBackList.size(); iIndex++)
	{
		if(m_callBackList[iIndex] = callBack )
		{
			m_callBackList[iIndex] = m_callBackList[m_callBackList.size()-1];
			m_callBackList[m_callBackList.size()-1] = callBack;
			break;
		}
	}

	if(!m_callBackList.empty())
	{
		m_callBackList.pop_back();
	}
}

cs::JsonDictionary* UserService::parseStringToJson(const char* sJson)
{
	cs::JsonDictionary *pJsonDict = new cs::JsonDictionary();
	pJsonDict->initWithDescription(sJson);
	
	return pJsonDict;
}