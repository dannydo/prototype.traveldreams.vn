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


#ifndef __FACEBOOK_MANAGER_H__
#define __FACEBOOK_MANAGER_H__

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	#include "ProtocolUser.h"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
class FacebookActionResult : public cocos2d::plugin::UserActionListener
{
public:
    virtual void onActionResult(cocos2d::plugin::ProtocolUser* pPlugin, cocos2d::plugin::UserActionResultCode code, const char* msg);
};

class FacebookManager
{
public:
	static FacebookManager* getInstance();
    static void purgeManager();

	bool isLogined();
	void unloadPlugin();
    void loadPlugin();
    void loginByMode();
    void logoutByMode();
	void shareLink(const char* name, const char* caption, const char* description, const char* link, const char* picture);
	void shareDialog(const char* name, const char* caption, const char* description, const char* link, const char* picture);
	inline bool getLogined(){ return m_bLogined; };
	void setLogined(bool bLogined);
	std::string getAccessToken();

private:
    FacebookManager();
    virtual ~FacebookManager();

    static FacebookManager* s_pManager;

    cocos2d::plugin::ProtocolUser* _facebook;
    FacebookActionResult* _retListener;
	bool m_bLogined;
};

#endif // __MY_USER_MANAGER_H__
#endif
