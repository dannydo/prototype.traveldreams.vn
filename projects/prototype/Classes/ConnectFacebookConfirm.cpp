#include "ConnectFacebookConfirm.h"
#include "SystemEventHandle.h"
#include "Database\InitDatabase.h"
#include "WaitingNode.h"
#include "APIService\UserService.h"

USING_NS_CC;

ConnectFacebookConfirm::~ConnectFacebookConfirm ()
{
}

ConnectFacebookConfirm* ConnectFacebookConfirm::createLayout(const UserInfo& userInfo)
{
	ConnectFacebookConfirm* pConnectFacebookConfirm = new ConnectFacebookConfirm();
	pConnectFacebookConfirm->m_UserInfo = userInfo;

	if(pConnectFacebookConfirm->init())
	{
		pConnectFacebookConfirm->autorelease();
		return pConnectFacebookConfirm;
	}

	CC_SAFE_DELETE(pConnectFacebookConfirm);
	return NULL;
}

bool ConnectFacebookConfirm::init()
{
	if (!CustomNode::init())
	{
		return false;
	}

	LayerColor* pBackground = LayerColor::create(ccc4(0, 0, 0, 255), 400.0f, 150.0f);
	pBackground->setPosition(-pBackground->getContentSize().width/2.0f, -pBackground->getContentSize().height/2.0f);
	pBackground->setAnchorPoint(Point(0.5f, 0.5f));
	this->addChild(pBackground);

	LabelTTF* pLabelLocal = LabelTTF::create("Use account local", "Arial", 30);
	MenuItem* pItemLocal = MenuItemLabel::create(pLabelLocal, CC_CALLBACK_0(ConnectFacebookConfirm::clickUseLocal, this));
	pItemLocal->setPositionY(-25.0f);

	LabelTTF* pLabelFacebook = LabelTTF::create("Use account facebook", "Arial", 30);
	MenuItem* pItemFacebook = MenuItemLabel::create(pLabelFacebook, CC_CALLBACK_0(ConnectFacebookConfirm::clickUseFacebook, this));
	pItemFacebook->setPositionY(25.0f);

	Menu* pMenu = Menu::create(pItemLocal, pItemFacebook, NULL);
	pMenu->setPosition(Point::ZERO);
	this->addChild(pMenu);

	return true;
}

bool ConnectFacebookConfirm::onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	return true;
}

void ConnectFacebookConfirm::clickUseLocal()
{
	this->getParent()->removeChild(this);
}

void ConnectFacebookConfirm::clickUseFacebook()
{
	this->removeAllChildren();
	WaitingNode* pWaitingNode = WaitingNode::createLayout("Loading...");
	pWaitingNode->setTag(1099);
	this->getParent()->addChild(pWaitingNode);

	UserInfo userInfo = UserTable::getInstance()->getUserInfo();
	UserService::getInstance()->disableUser(userInfo.sUserToken);
	
	auto actionRunProcess = CallFunc::create(this, callfunc_selector(ConnectFacebookConfirm::runProcess));
	auto actionFinishWord = CallFunc::create(this, callfunc_selector(ConnectFacebookConfirm::finishProcess));
	this->runAction(Sequence::create(DelayTime::create(0.001f), actionRunProcess, actionFinishWord, NULL));
}

void ConnectFacebookConfirm::runProcess()
{
	if(InitDatabase::getInstance()->resetDatabase())
	{
		m_UserInfo.iCurrentLevel = 1;
		m_UserInfo.sCurrentChapterId = "Chapter1";
		m_UserInfo.sFirstName = "";
		m_UserInfo.sLastName = "";
		m_UserInfo.iMonney = 0;
		m_UserInfo.iLife = 5;
		m_UserInfo.iLifeTimeRemaining = 0;
		m_UserInfo.iVersion = 1;
		UserTable::getInstance()->updateUser(m_UserInfo);

		SystemEventHandle::getInstance()->onStartSyncGame(true);
		UserDefault::getInstance()->setIntegerForKey("IsLoginFacebook", 1);
	}
}

void ConnectFacebookConfirm::finishProcess()
{
	this->getParent()->removeChildByTag(1099);
	this->getParent()->removeChild(this);
}