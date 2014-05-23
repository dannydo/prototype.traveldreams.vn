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
#include "SystemEventHandle.h"

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
			pFacebookInfo["FacebookAppId"] = ""; //674930442556432
			pFacebookInfo["FacebookScope"] = "publish_actions";
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
	if (_facebook)
	{
		m_bIsFinishRun = false;
		m_bIsLogout = false;
	    _facebook->login();
	}
}

void FacebookManager::logoutByMode()
{
    if (_facebook) 
	{
		m_bIsFinishRun = false;
		m_bIsLogout = true;
        _facebook->logout();
    }
}

void FacebookManager::shareLink(const char* name, const char* caption, const char* description, const char* link, const char* picture)
{
	if (_facebook)
	{
		m_bIsFinishRun = false;
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
		m_bIsFinishRun = false;
		PluginParam paramName(name);
		PluginParam paramCaption(caption);
		PluginParam paramDescription(description);
		PluginParam paramLink(link);
		PluginParam paramPicture(picture);
		
		_facebook->callFuncWithParam("shareFacebookDialog", &paramName,  &paramCaption,  &paramDescription, &paramLink, &paramPicture, NULL); 
	}
}

void FacebookManager::inviteFriends(const char* message, const char* title, const char* filters, const char* to, const char* suggestions, const char* data)
{
	if (_facebook)
	{
		m_bIsFinishRun = false;
		PluginParam paramMessage(message);
		PluginParam paramTitle(title);
		PluginParam paramFilters(filters);
		PluginParam paramTo(to);
		PluginParam paramSuggestions(suggestions);
		PluginParam paramData(data);
		
		_facebook->callFuncWithParam("requestDialog", &paramMessage,  &paramTitle,  &paramFilters, &paramTo, &paramSuggestions, &paramData, NULL); 
	}
}

void FacebookManager::openURL(const char* pszUrl)
{
	if (_facebook)
	{
		m_bIsFinishRun = false;
		PluginParam paramURL(pszUrl);
		PluginParam paramTemp("");
		_facebook->callFuncWithParam("openURL", &paramURL, &paramTemp, NULL);
	}
}

int FacebookManager::getAppVersionCode()
{
	if (_facebook)
	{
		return _facebook->callIntFuncWithParam("getAppVersionCode", NULL);
	}

	return 0;
}

void FacebookManager::autoOpenActiveSession()
{
	if (_facebook)
	{
		_facebook->callFuncWithParam("autoOpenActiveSession", NULL);
	}
}

bool FacebookManager::isNetworkOnline()
{
	if (_facebook)
	{
		return _facebook->callBoolFuncWithParam("isNetworkOnline", NULL);
	}

	return false;
}

std::string FacebookManager::getDeviceId()
{
	if (_facebook)
	{
		return _facebook->callStringFuncWithParam("getDeviceId", NULL);
	}

	return "";
}

std::string FacebookManager::getDeviceName()
{
	if (_facebook)
	{
		return _facebook->callStringFuncWithParam("getDeviceName", NULL);
	}

	return "";
}

std::string FacebookManager::getOSVersion()
{
	if (_facebook)
	{
		return _facebook->callStringFuncWithParam("getOSVersion", NULL);
	}

	return "";
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

void FacebookManager::setFinishRun(const bool& bIsFinishRun)
{
	m_bIsFinishRun = bIsFinishRun;
}

void FacebookActionResult::onActionResult(ProtocolUser* pPlugin, UserActionResultCode code, const char* msg)
{
    char userStatus[1024] = { 0 };
    switch (code)
    {
    case kLoginSucceed:
		{
			SystemEventHandle::getInstance()->onLoginFacebookResult(true);
		break;
		}
    case kLoginFailed:
		{
			UserDefault::getInstance()->setIntegerForKey("IsLoginFacebook", 0);
			if (FacebookManager::getInstance()->getIsLogOut() == false)
				SystemEventHandle::getInstance()->onLoginFacebookResult(false);

			break;
		}
    case kLogoutSucceed:
		{
			UserDefault::getInstance()->setIntegerForKey("IsLoginFacebook", 0);
			break;
		}
    default:
        break;
    }

    FacebookManager::getInstance()->setFinishRun(true);

    //MessageBox(msg, userStatus);

    // get session ID
    //std::string sessionID = pPlugin->getSessionID();
    //log("User Session ID of plugin %s is : %s", pPlugin->getPluginName(), sessionID.c_str());

    //std::string strStatus = pPlugin->isLogined() ? "online" : "offline";
    //log("User status of plugin %s is : %s", pPlugin->getPluginName(), strStatus.c_str());
}
#endif
