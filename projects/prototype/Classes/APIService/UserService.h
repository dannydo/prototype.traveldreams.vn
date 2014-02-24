#ifndef __USER_SERVICE_H__
#define __USER_SERVICE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

class UserService
{
public:
	static UserService* getInstance();

	cocos2d::extension::HttpRequest* registryUser(const std::string strAccessToken);

private:
	UserService();
	virtual ~UserService();

	static UserService* m_UserService;
};
#endif