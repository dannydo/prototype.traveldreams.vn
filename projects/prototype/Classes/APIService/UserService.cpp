#include "UserService.h"
#include "Constants.h"

USING_NS_CC; 
USING_NS_CC_EXT;

UserService* UserService::m_UserService = NULL;

UserService::UserService()
{

}

UserService::~UserService()
{

}

UserService* UserService::getInstance()
{
	if (m_UserService == NULL) {
		m_UserService = new UserService();
	}
	return m_UserService;
}

HttpRequest* UserService::registryUser(const std::string strAccessToken)
{
	String strURL = _CONSTANT_URL_;
	strURL.append("api/registry/");
	strURL.append(strAccessToken);

	CCLOG("registryUser: %s", strURL.getCString());

	HttpRequest* request = new HttpRequest();
	request->setUrl(strURL.getCString());
	request->setRequestType(HttpRequest::Type::GET);
	request->setTag("GET registry token");

	return request;
}