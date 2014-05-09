#include "LeaderBoardAdvanceModeNode.h"
#include "APIService\UserService.h"
#include "LoadingImageNode.h"
#include "Database\UserTable.h"
#include "Database\CSPackageTable.h"

USING_NS_CC;
USING_NS_CC_EXT;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "Social\FacebookManager.h"
using namespace cocos2d::plugin;
#endif

LeaderBoardAdvanceModeNode::~LeaderBoardAdvanceModeNode()
{
	UserService::getInstance()->removeCallBackList(this);
}

LeaderBoardAdvanceModeNode* LeaderBoardAdvanceModeNode::createLayout(const std::string sPackageId)
{
	LeaderBoardAdvanceModeNode* pLeaderBoardNode = new LeaderBoardAdvanceModeNode();
	pLeaderBoardNode->m_sPackageId = sPackageId;

	if(pLeaderBoardNode->init())
	{
		pLeaderBoardNode->autorelease();
		return pLeaderBoardNode;
	}

	CC_SAFE_DELETE(pLeaderBoardNode);
	return NULL;
}

bool LeaderBoardAdvanceModeNode::init()
{
	if (!CustomNode::init())
	{
		return false;
	}

	m_pSpriteBackground = Sprite::create("Target-End-Game/panel_high_score.png");
	this->addChild(m_pSpriteBackground);
	this->setContentSize(m_pSpriteBackground->getContentSize());

	LabelBMFont *pLabelHighScore = LabelBMFont::create("LEADER BOARD", "fonts/font_score.fnt");
	pLabelHighScore->setPosition(Point(12.0f, 60.0f));
	this->addChild(pLabelHighScore);

	m_pSlideShow = Node::create();
	this->addChild(m_pSlideShow);

	m_pButtonManagerNode = ButtonManagerNode::create();
	m_pButtonManagerNode->AllowSwipingBackground(true);

	Sprite* pImageNo1 = Sprite::create("Target-End-Game/btn_no1.png");
	ButtonNode* pButtonNo1 = ButtonNode::createButtonSprite(pImageNo1, CC_CALLBACK_1(LeaderBoardAdvanceModeNode::clickNo1, this));
	pButtonNo1->setPosition(Point(-230.0f, 60.0f));
	m_pButtonManagerNode->addButtonNode(pButtonNo1);

	Sprite* pImageMe = Sprite::create("Target-End-Game/btn_me.png");
	ButtonNode* pButtonMe = ButtonNode::createButtonSprite(pImageMe, CC_CALLBACK_1(LeaderBoardAdvanceModeNode::clickMe, this));
	pButtonMe->setPosition(Point(230.0f, 60.0f));
	m_pButtonManagerNode->addButtonNode(pButtonMe);

	this->addChild(m_pButtonManagerNode);

	m_pScrollManager = new ScrollManager();
	m_iLeaderBoardCount = 0;
	m_fMinPositionLeft = 0;
	m_pointNo1 = m_pSlideShow->getPosition();
	m_pointMe = m_pSlideShow->getPosition();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	FacebookManager::getInstance()->loadPlugin();
	UserInfo userInfo = UserTable::getInstance()->getUserInfo();
	if (userInfo.sFacebookId != "" && UserDefault::getInstance()->getIntegerForKey("IsLoginFacebook", 0) == 1 && FacebookManager::getInstance()->isLogined())
	{
		UserService::getInstance()->addCallBackList(this);
		m_iConnectServer = UserDefault::getInstance()->getIntegerForKey("NumberConnectServer", 0);
		m_iConnectServer++;
		UserDefault::getInstance()->setIntegerForKey("NumberConnectServer", m_iConnectServer);
		UserService::getInstance()->getLeaderBoardLevel(m_sChapterId ,m_iLevel, m_iConnectServer);
	}
#endif

	UserService::getInstance()->addCallBackList(this);
	m_iConnectServer = UserDefault::getInstance()->getIntegerForKey("NumberConnectServer", 0);
	m_iConnectServer++;
	UserDefault::getInstance()->setIntegerForKey("NumberConnectServer", m_iConnectServer);
	UserService::getInstance()->getLeaderBoardAdvanceMode(m_sPackageId, m_iConnectServer);
	

	return true;
}

void LeaderBoardAdvanceModeNode::clickNo1(cocos2d::Object* pSender)
{
	auto actionMoveTo = MoveTo::create(0.3f, m_pointNo1);
	m_pSlideShow->runAction(actionMoveTo);
}

void LeaderBoardAdvanceModeNode::clickMe(cocos2d::Object* pSender)
{
	float fX = m_pSlideShow->getPositionX();
	if (fX < m_pointMe.x)
	{
		auto actionMoveTo = MoveBy::create(0.3f, Point(-(fX - m_pointMe.x), m_pointMe.y));
		m_pSlideShow->runAction(actionMoveTo);
	}
	else if (fX > m_pointMe.x)
	{
		float fDistanceX = -fX + m_pointMe.x;
		if (fDistanceX < m_fMinPositionLeft)
			fDistanceX = m_fMinPositionLeft;

		auto actionMoveTo = MoveBy::create(0.3f, Point(fDistanceX, m_pointMe.y));
		m_pSlideShow->runAction(actionMoveTo);
	}
}

void LeaderBoardAdvanceModeNode::addItemToSlide(const int& iStage, const char* sName, const char* sFacebookId, const int& iRank)
{
	Node* pNodeItem = Node::create();

	Sprite* pLoadAvatar = Sprite::create("Target-End-Game/border_avatar.png");
	pLoadAvatar->setPosition(Point(80.0f, 95.0f));
	pNodeItem->addChild(pLoadAvatar);

	std::string urlAvatar = "https://graph.facebook.com/";
	urlAvatar.append(sFacebookId);
	urlAvatar.append("/picture");

	LoadingImagetNode* avatar = LoadingImagetNode::createLayout(urlAvatar.c_str());
	avatar->setPosition(Point(81.0f, 95.0f));
	pNodeItem->addChild(avatar);

	char sStage[5];
	sprintf(sStage, "%d", iStage);
	LabelBMFont *pLabelStage = LabelBMFont::create(sStage, "fonts/font_small_alert.fnt");
	pLabelStage->setAnchorPoint(Point(0.0f, 0.5f));
	pLabelStage->setPosition(Point(57.0f, 8.0f));
	pLabelStage->setScale(0.85);
	pNodeItem->addChild(pLabelStage);

	LabelBMFont *pLabelName = LabelBMFont::create(sName, "fonts/font-small-green-alert.fnt");
	pLabelName->setScale(0.85f);
	pLabelName->setAnchorPoint(Point(0.0f, 0.5f));
	pLabelName->setPosition(Point(57.0f, 45.0f));
	pNodeItem->addChild(pLabelName);

	char sRank[10];
	sprintf(sRank, "%d", iRank);
	LabelBMFont *pLabelRank = LabelBMFont::create(sRank, "fonts/font-bechic.fnt");
	pLabelRank->setAnchorPoint(Point(0.0f, 0.5f));
	pLabelRank->setPosition(Point(25.0f, 23.0f));
	pNodeItem->addChild(pLabelRank);

	pNodeItem->setContentSize(Size(160.0f, 160.0f));
	pNodeItem->setPosition(Point(-320.0f + (iRank-1)*160, -100.0f));
	m_pSlideShow->addChild(pNodeItem);
}

void LeaderBoardAdvanceModeNode::parseJsonToLeadeBoard(cs::JsonDictionary* pJsonItem, const int& iRank)
{
	if(iRank > 0)
	{
		int iScore = pJsonItem->getItemIntValue("HighNumberStage", 0);
		const char* sName = pJsonItem->getItemStringValue("FirstName");
		std::string sFacebookId = pJsonItem->getItemStringValue("FacebookIdCustom");
		sFacebookId.pop_back();

		this->addItemToSlide(iScore, sName, sFacebookId.c_str(), iRank);
	}
}

void LeaderBoardAdvanceModeNode::resultHttpRequestCompleted(cs::JsonDictionary* pJsonDict, std::string sKey)
{
	if (pJsonDict != NULL)
	{
		UserInfo userInfo = UserTable::getInstance()->getUserInfo();
		CSPackageInfo csPackageInfo = CSPackageTable::getInstance()->getCSPackageInfo(m_sPackageId);

		String sTag = "LeaderBoardAdvanceMode";
		sTag.appendWithFormat("_%d", m_iConnectServer);
		
		if (sKey == sTag.getCString())
		{
			cs::JsonDictionary* jsonData = pJsonDict->getSubDictionary("data");
			if (jsonData != NULL)
			{
				if (jsonData->getItemBoolvalue("result", false))
				{
					cs::JsonDictionary* pJsonList = jsonData->getSubDictionary("list");
					if (pJsonList != NULL)
					{
						int iStageMaxMe = csPackageInfo.iStage;
						cs::JsonDictionary* pJsonMe = pJsonList->getSubDictionary("Me");
						if (pJsonMe != NULL)
						{
							iStageMaxMe = pJsonMe->getItemIntValue("HighNumberStage", 0);
							if (iStageMaxMe < csPackageInfo.iStage)
							{
								iStageMaxMe = csPackageInfo.iStage;
							}
						}

						m_iLeaderBoardCount = pJsonList->getArrayItemCount("Friends");
					
						// find index of me
						int iRankMe = 1;
						for(int iIndex=0; iIndex < m_iLeaderBoardCount; iIndex++)
						{
							cs::JsonDictionary* pJsonItem = pJsonList->getSubItemFromArray("Friends", iIndex);
							int iStage = pJsonItem->getItemIntValue("HighNumberStage", 0);

							if (iStage > iStageMaxMe)
								iRankMe++;
							else
								break;
						}

						// Add me to slide
						this->addItemToSlide(iStageMaxMe, "Me", userInfo.sFacebookId.c_str(), iRankMe);
						m_pointMe.x = -(iRankMe-1)*160;

						// Add friends to slide
						int iCountRank = 0;
						for (int iIndex=0; iIndex<m_iLeaderBoardCount; iIndex++)
						{
							iCountRank++;
							if (iCountRank == iRankMe)
								iCountRank++;

							cs::JsonDictionary* pJsonItem = pJsonList->getSubItemFromArray("Friends", iIndex);
							this->parseJsonToLeadeBoard(pJsonItem, iCountRank);
						}

						m_iLeaderBoardCount = m_iLeaderBoardCount + 1;
						m_fMinPositionLeft = -((m_iLeaderBoardCount-4)*160.0f);
					}
				}
				else
				{

				}
			}
		}
	}
}

bool LeaderBoardAdvanceModeNode::onTouchCustomNodeBegan(Touch* pTouch,  Event* pEvent)
{
	if (m_iLeaderBoardCount > 3)
	{
		Point touchPosition = pTouch->getLocation();
		m_fBeginX = touchPosition.x;

		CCRect *pRectButton = new CCRect(m_pSpriteBackground->getOffsetPosition().x, 
				m_pSpriteBackground->getOffsetPosition().y, 
				m_pSpriteBackground->getTextureRect().size.width, 
				m_pSpriteBackground->getTextureRect().size.height);

		Point touchButton = m_pSpriteBackground->convertToNodeSpace(touchPosition);

		if(pRectButton->containsPoint(touchButton))
		{
			DataTouch dataTouch;
			dataTouch.point = touchPosition;
			dataTouch.lTime = 0;
			dataTouch.fDeltaTime = 0;
			m_pScrollManager->addDataToQueue(dataTouch);
			return true;
		}
	}

	return false;
}

void LeaderBoardAdvanceModeNode::onTouchCustomNodeMoved(Touch* pTouch,  Event* pEvent)
{
	if (m_pButtonManagerNode->IsInTapMode())
		return;

	Point touchPosition = pTouch->getLocation();
	m_fXMoved = touchPosition.x - m_fBeginX;
	
	if(m_fXMoved + m_pSlideShow->getPositionX() >= 0.0f)
	{
		m_fXMoved = -m_pSlideShow->getPositionX();
	}
	else if(m_fXMoved + m_pSlideShow->getPositionX() <= m_fMinPositionLeft)
	{
		m_fXMoved = m_fMinPositionLeft - m_pSlideShow->getPositionX();
	}

	m_pSlideShow->setPositionX(m_pSlideShow->getPosition().x + m_fXMoved);
	m_fBeginX = touchPosition.x;

	DataTouch dataTouch;
	dataTouch.point = touchPosition;
	dataTouch.lTime = 0;
	dataTouch.fDeltaTime = 0;
	m_pScrollManager->addDataToQueue(dataTouch);
}

void LeaderBoardAdvanceModeNode::onTouchCustomNodeEnded(Touch* pTouch,  Event* pEvent)
{
	if (m_pButtonManagerNode->IsInTapMode())
		return;

	DataTouch dataTouch = m_pScrollManager->getDistanceScrollX();
	float distanceX = dataTouch.point.x;
	float deltaTime = dataTouch.fDeltaTime;
	
	if(distanceX!=0 && deltaTime!=0)
	{
		float fTime = 0.2f;
		distanceX = distanceX * fTime / deltaTime / 10; 

		if (distanceX + m_pSlideShow->getPositionX() >= 0.0f) {
			distanceX = -m_pSlideShow->getPositionX();
		}
		else if(distanceX + m_pSlideShow->getPositionX() <= m_fMinPositionLeft)
		{
			distanceX = m_fMinPositionLeft - m_pSlideShow->getPositionX();
		}

		auto actionMove = MoveBy::create(fTime, Point(distanceX, 0.0f));
		auto actionEaseOut = EaseOut::create(actionMove, 2.5f);
		m_pSlideShow->stopAllActions();
		m_pSlideShow->runAction(Sequence::create(actionEaseOut, NULL));
	}
}