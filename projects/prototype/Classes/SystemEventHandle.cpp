#include "SystemEventHandle.h"
#include "APIService\SyncDataGame.h"
#include "Database\UserTable.h"
#include "Database\InitDatabase.h"
#include "ConnectFacebookConfirm.h"
#include "APIService\UserService.h"
#include "WaitingNode.h"

USING_NS_CC; 

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "Social\FacebookManager.h"
using namespace cocos2d::plugin;
#endif

SystemEventHandle* SystemEventHandle::m_SystemEventHandle = NULL;

SystemEventHandle::SystemEventHandle()
{
	m_bIsConnectFacebook = false;
}

void SystemEventHandle::releaseInstance()
{
	if (m_SystemEventHandle == NULL)
	{
		delete m_SystemEventHandle;
	}

	m_SystemEventHandle = NULL;
}

SystemEventHandle* SystemEventHandle::getInstance()
{
	if (m_SystemEventHandle == NULL) {
		m_SystemEventHandle = new SystemEventHandle();
		m_SystemEventHandle->init();
	}

	return m_SystemEventHandle;
}

bool SystemEventHandle::init()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	FacebookManager::getInstance()->loadPlugin();
#endif
	return true;
}

void SystemEventHandle::onStartSyncGame()
{
	// Show popup sync game data
	WaitingNode* pWaitingNode = WaitingNode::createLayout("Loading...");
	pWaitingNode->setPosition(320.0f, 480.0f);
	pWaitingNode->setTag(1001);
	Director::getInstance()->getRunningScene()->addChild(pWaitingNode);

	SyncDataGame::getInstance()->runSyncDataGame();
}

void SystemEventHandle::onGameSyncCompleted(const bool& bResult)
{
	if (m_bIsConnectFacebook)
	{
		if (bResult)
		{
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			FacebookManager::getInstance()->loginByMode();
		#endif
		}
		else
		{
			// Show popup error
			Director::getInstance()->getRunningScene()->removeChildByTag(1000);
			MessageBox("Sync Game Data Error!", "");
		}

		m_bIsConnectFacebook= false;
	}
	else
	{
		// Destroy popup sync
		Director::getInstance()->getRunningScene()->removeChildByTag(1001);
		if(!bResult)
		{
			MessageBox("Sync Game Data Error!", "");
		}
	}
}

void SystemEventHandle::onStartConnectFacebook()
{
	// Show popup connect facebook
	WaitingNode* pWaitingNode = WaitingNode::createLayout("Connecting...");
	pWaitingNode->setPosition(320.0f, 480.0f);
	pWaitingNode->setTag(1000);
	Director::getInstance()->getRunningScene()->addChild(pWaitingNode);

	m_bIsConnectFacebook = true;
	UserDefault::getInstance()->setIntegerForKey("IsLoginFacebook", 0);
	SyncDataGame::getInstance()->runSyncDataGame();
}

void SystemEventHandle::onLoginFacebookResult(const bool& bResult)
{
	if (bResult)
	{
		// Check token facebook with server
		int iConnectServer = UserDefault::getInstance()->getIntegerForKey("NumberConnectServer", 0);
		iConnectServer++;
		UserDefault::getInstance()->setIntegerForKey("NumberConnectServer", iConnectServer);
		std::string sUserIdentifier = UserTable::getInstance()->getUserInfo().sUserIdentifier;
		int iUserId = int(strtod(sUserIdentifier.c_str(), 0));

		std::string sFacebookToken = "";
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			sFacebookToken = FacebookManager::getInstance()->getAccessToken();
		#endif

		UserService::getInstance()->checkUserFacebook(sFacebookToken, iUserId, iConnectServer);
	}
	else
	{
		// Show popup error
		MessageBox("Login", "Connect facebook error!");
		Director::getInstance()->getRunningScene()->removeChildByTag(1000);
	}
}

void SystemEventHandle::onCheckUserFacebookResult(cs::JsonDictionary* pJsonDict, std::string sKey)
{
	if(pJsonDict != NULL)
	{
		cs::JsonDictionary* pJsonData = pJsonDict->getSubDictionary("data");
		if (pJsonData != NULL)
		{
			bool bResult = pJsonData->getItemBoolvalue("result", false);
			if (bResult)
			{
				std::string sKey = pJsonData->getItemStringValue("key");
				cs::JsonDictionary* pJsonUser = pJsonData->getSubDictionary("User");
				if (pJsonUser != NULL)
				{
					UserInfo userInfo = UserTable::getInstance()->getUserInfo();
					userInfo.sUserIdentifier = pJsonUser->getItemStringValue("UserId");
					userInfo.sFacebookId = pJsonUser->getItemStringValue("FacebookId");
					userInfo.sFacebookToken = pJsonUser->getItemStringValue("FacebookToken");
					userInfo.sUserToken = pJsonUser->getItemStringValue("UserToken");
					userInfo.ulLifeTimeBeginRemain = this->getTimeLocalCurrent();

					Director::getInstance()->getRunningScene()->removeChildByTag(1000);
					if (sKey == "USER_NOT_MAP_FACEBOOK")
					{
						// Show popup
						if (UserTable::getInstance()->getUserInfo().sFacebookId == "")
						{
							ConnectFacebookConfirm* pConfirm = ConnectFacebookConfirm::createLayout(userInfo);
							Director::getInstance()->getRunningScene()->addChild(pConfirm);
							pConfirm->setPosition(320.0f, 480.0f);
						}
						else
						{
							// Delete and init data, update user and Sync data
							if(InitDatabase::getInstance()->resetDatabase())
							{
								UserTable::getInstance()->updateUser(userInfo);
								this->onStartSyncGame();
								UserDefault::getInstance()->setIntegerForKey("IsLoginFacebook", 1);
							}
						}
					}
					else if (sKey == "USER_MAP_FACEBOOK")
					{
						// Set connect facebook success
						UserTable::getInstance()->updateUser(userInfo);
						UserDefault::getInstance()->setIntegerForKey("IsLoginFacebook", 1);
					}
					else if (sKey == "USER_NOT_EXIST")
					{
						// Sync data with get and set connect facebook success
						userInfo.sUserIdentifier = "ohmyword";
						UserTable::getInstance()->updateUser(userInfo);
						this->onStartSyncGame();
						UserDefault::getInstance()->setIntegerForKey("IsLoginFacebook", 1);
					}
				}
			}
		}
	}
}

unsigned long SystemEventHandle::getTimeLocalCurrent()
{
	timeval now;
	gettimeofday(&now, NULL);
	unsigned long iCurrentTime = now.tv_sec + now.tv_usec/1000000 ; //seconds
	return iCurrentTime;
}