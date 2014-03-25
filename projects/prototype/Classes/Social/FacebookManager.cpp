/****************************************************************************
Copyright (c) 2012-2013 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "FacebookManager.h"
#include "PluginManager.h"

using namespace cocos2d::plugin;
using namespace cocos2d;

FacebookManager* FacebookManager::s_pManager = NULL;

FacebookManager::FacebookManager()
: _retListener(NULL)
, _facebook(NULL)
{

}

FacebookManager::~FacebookManager()
{
	unloadPlugin();
	if (_retListener)
	{
		delete _retListener;
		_retListener = NULL;
	}
}

FacebookManager* FacebookManager::getInstance()
{
	if (s_pManager == NULL) 
	{
		s_pManager = new FacebookManager();
	}
	return s_pManager;
}

void FacebookManager::purgeManager()
{
	if (s_pManager)
	{
		delete s_pManager;
		s_pManager = NULL;
	}
	PluginManager::end();
}

void FacebookManager::loadPlugin()
{
	if (_retListener == NULL)
	{
		_retListener = new FacebookActionResult();
	}

	{
		_facebook = dynamic_cast<ProtocolUser*>(PluginManager::getInstance()->loadPlugin("UserFacebook"));		
		if (NULL != _facebook)
		{
			TUserDeveloperInfo pFacebookInfo;
			pFacebookInfo["FacebookAppId"] = "440402379424810";
			pFacebookInfo["FacebookScope"] = "";
			if (pFacebookInfo.empty()) {
				char msg[256] = { 0 };
				sprintf(msg, "Developer info is empty. PLZ fill your Facebook info in %s(nearby line %d)", __FILE__, __LINE__);
				MessageBox(msg, "Facebook Warning");
			}
			_facebook->configDeveloperInfo(pFacebookInfo);
			_facebook->setDebugMode(true);
			_facebook->setActionListener(_retListener);
		}
	}
}

void FacebookManager::unloadPlugin()
{
	if (_facebook)
	{
		PluginManager::getInstance()->unloadPlugin("UserFacebook");
		_facebook = NULL;
	}
}

bool FacebookManager::isLogined()
{
	if (_facebook)
	{
		return _facebook->isLogined();
	}

	return false;
}

void FacebookManager::loginByMode()
{
	m_bLogined = false;
	if (_facebook)
	{
	    _facebook->login();
	}
}

void FacebookManager::logoutByMode()
{
    if (_facebook)
	{
        _facebook->logout();
    }
}

void FacebookManager::shareLink(const char* name, const char* caption, const char* description, const char* link, const char* picture)
{
	if (_facebook)
	{
		PluginParam paramName(name);
		PluginParam paramCaption(caption);
		PluginParam paramDescription(description);
		PluginParam paramLink(link);
		PluginParam paramPicture(picture);
		
		 _facebook->callFuncWithParam("shareFacebookLink", &paramName,  &paramCaption,  &paramDescription, &paramLink, &paramPicture, NULL); 
	}
}

void FacebookManager::shareDialog(const char* name, const char* caption, const char* description, const char* link, const char* picture)
{
	if (_facebook)
	{
		PluginParam paramName(name);
		PluginParam paramCaption(caption);
		PluginParam paramDescription(description);
		PluginParam paramLink(link);
		PluginParam paramPicture(picture);
		
		 _facebook->callFuncWithParam("shareFacebookDialog", &paramName,  &paramCaption,  &paramDescription, &paramLink, &paramPicture, NULL); 
	}
}

void FacebookManager::setLogined(bool bLogined)
{
	if (_facebook)
	{
		m_bLogined = bLogined;
	}
}

std::string FacebookManager::getAccessToken() 
{
	std::string sessionID = "";
	if (_facebook)
	{
		 sessionID = _facebook->getSessionID();
	}
	return sessionID;
}

void FacebookActionResult::onActionResult(ProtocolUser* pPlugin, UserActionResultCode code, const char* msg)
{
    char userStatus[1024] = { 0 };
    switch (code)
    {
    case kLoginSucceed:
		FacebookManager::getInstance()->setLogined(true);
    case kLoginFailed:
        sprintf(userStatus, "User of \"%s\" login %s", pPlugin->getPluginName(), (code == kLoginSucceed)? "Successed" : "Failed");
        break;
    case kLogoutSucceed:
        sprintf(userStatus, "User of \"%s\" logout", pPlugin->getPluginName());
        break;
    default:
        break;
    }
    //MessageBox(msg, userStatus);

    // get session ID
    std::string sessionID = pPlugin->getSessionID();
    log("User Session ID of plugin %s is : %s", pPlugin->getPluginName(), sessionID.c_str());

    std::string strStatus = pPlugin->isLogined() ? "online" : "offline";
    log("User status of plugin %s is : %s", pPlugin->getPluginName(), strStatus.c_str());
}
#endif