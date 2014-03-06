#ifndef __USER_SERVICE_H__
#define __USER_SERVICE_H__

#include "cocos2d.h"
#include "cocos-ext.h"	
#include "InterfaceService.h"

class UserService : public cocos2d::Object
{
public:
	static UserService* getInstance();
	UserService();

	static void releaseInstance();

	void registryUser(const std::string strAccessToken);
	void getUserInfo();
	void getLeaderBoardLevel(const int& iLevel);
	void addCallBackList(InterfaceService* callBack);
	void removeCallBackList(InterfaceService *callBack);

	cs::JsonDictionary* parseStringToJson(const char* sJson);

private:
	void onHttpRequestCompleted(cocos2d::extension::HttpClient *sender, cocos2d::extension::HttpResponse *response);

	static UserService* m_UserService;
	std::vector<InterfaceService*> m_callBackList;

	static cocos2d::extension::HttpRequest* m_pRequest;
	static cocos2d::extension::HttpClient* m_pClient;
};
#endif