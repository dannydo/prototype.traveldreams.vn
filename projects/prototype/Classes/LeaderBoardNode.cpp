#include "LeaderBoardNode.h"
#include "APIService\UserService.h"
#include "APIService\Constants.h"
#include "Database\UserTable.h"
#include "Database\LevelTable.h"
#include "LoadingImageNode.h"
#include "GameConfigManager.h"
#include "FunctionCommon.h"

USING_NS_CC;
USING_NS_CC_EXT;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "Social\FacebookManager.h"
using namespace cocos2d::plugin;
#endif

LeaderBoardtNode::~LeaderBoardtNode()
{
	UserService::getInstance()->removeCallBackList(this);
}

LeaderBoardtNode* LeaderBoardtNode::createLayout(const int& iLevel, const std::string sChapterId)
{
	LeaderBoardtNode* pLeaderBoardNode = new LeaderBoardtNode();
	pLeaderBoardNode->m_iLevel = iLevel;
	pLeaderBoardNode->m_sChapterId = sChapterId;

	if(pLeaderBoardNode->init())
	{
		pLeaderBoardNode->autorelease();
		return pLeaderBoardNode;
	}

	CC_SAFE_DELETE(pLeaderBoardNode);
	return NULL;
}

bool LeaderBoardtNode::init()
{
	if (!CustomNode::init())
	{
		return false;
	}

	m_pSpriteBackground = Sprite::create("Target-End-Game/panel_high_score.png");
	this->addChild(m_pSpriteBackground);
	this->setContentSize(m_pSpriteBackground->getContentSize());

	LabelBMFont *pLabelHighScore = LabelBMFont::create("HIGH SCORE", "fonts/font_score.fnt");
	pLabelHighScore->setPosition(Point(12.0f, 60.0f));
	this->addChild(pLabelHighScore);

	m_pSlideShow = Node::create();
	this->addChild(m_pSlideShow);

	m_pButtonManagerNode = ButtonManagerNode::create();
	m_pButtonManagerNode->AllowSwipingBackground(true);
	m_pSlideShow->addChild(m_pButtonManagerNode);

	m_pScrollManager = new ScrollManager();
	m_iLeaderBoardCount = 0;

	for(int iIndex=0; iIndex<6; iIndex++)
	{
		m_arrIndex[iIndex] = -1;
	}

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

	/*
	UserService::getInstance()->addCallBackList(this);
	m_iConnectServer = UserDefault::getInstance()->getIntegerForKey("NumberConnectServer", 0);
	m_iConnectServer++;
	UserDefault::getInstance()->setIntegerForKey("NumberConnectServer", m_iConnectServer);
	UserService::getInstance()->getLeaderBoardLevel(m_sChapterId ,m_iLevel, m_iConnectServer);
	*/

	m_bIsSwipe = false;

	return true;
}

void LeaderBoardtNode::addItemToSlide(const int& iScore, const char* sName, const char* sFacebookId, const int& iRank, const int& iIndex)
{
	Node* pNodeItem = Node::create();

	Sprite* pLoadAvatar = Sprite::create("Target-End-Game/border_avatar.png");
	pLoadAvatar->setPosition(Point(50.0f, 95.0f));
	pNodeItem->addChild(pLoadAvatar);

	std::string urlAvatar = "https://graph.facebook.com/";
	urlAvatar.append(sFacebookId);
	urlAvatar.append("/picture");

	LoadingImagetNode* avatar = LoadingImagetNode::createLayout(urlAvatar.c_str());
	avatar->setPosition(Point(50.0f, 95.0f));
	pNodeItem->addChild(avatar);

	LabelBMFont *pLabelScore = LabelBMFont::create(formatNumber(iScore).getCString(), "fonts/font_small_alert.fnt");
	pLabelScore->setAnchorPoint(Point(0.0f, 0.5f));
	pLabelScore->setPosition(Point(57.0f, 8.0f));
	pLabelScore->setScale(0.8);
	pNodeItem->addChild(pLabelScore);

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

	Sprite* pLineImage = Sprite::create("Target-End-Game/line.png");
	pLineImage->setPosition(Point(160.0f, 55.0f));
	pNodeItem->addChild(pLineImage);

	pNodeItem->setContentSize(Size(160.0f, 160.0f));
	pNodeItem->setPosition(Point(-320.0f + iIndex*160, -100.0f));
	pNodeItem->setTag(iIndex);
	m_pSlideShow->addChild(pNodeItem);

	Sprite* pIconAskLife = Sprite::create("Target-End-Game/btn_ask_life.png");
	ButtonNode* pButtonAskLife = ButtonNode::createButtonSprite(pIconAskLife, CC_CALLBACK_1(LeaderBoardtNode::clickAskLife, this));
	pButtonAskLife->setPosition(Point(-205.0f + iIndex*160, -5.0f));
	m_pButtonManagerNode->addButtonNode(pButtonAskLife);
}

void LeaderBoardtNode::clickAskLife(cocos2d::Object* sender)
{

}

void LeaderBoardtNode::clickInviteFriends(Object* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	FacebookManager::getInstance()->inviteFriends("Invite friends play game", "", "", "", "", "");
#else
	MessageBox("Facebook not run with platform window", "Facebook");
#endif
}

bool LeaderBoardtNode::onTouchCustomNodeBegan(Touch* pTouch,  Event* pEvent)
{
	if(m_bIsSwipe)
	{
		return false;
	}
	m_bIsSwipe = true;

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

void LeaderBoardtNode::onTouchCustomNodeMoved(Touch* pTouch,  Event* pEvent)
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

void LeaderBoardtNode::onTouchCustomNodeEnded(Touch* pTouch,  Event* pEvent)
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

	m_bIsSwipe = false;
}

void LeaderBoardtNode::resultHttpRequestCompleted(cs::JsonDictionary* pJsonDict, std::string sKey)
{
	if (pJsonDict != NULL)
	{
		LevelInfo levelInfo =  LevelTable::getInstance()->getLevel(m_sChapterId, m_iLevel);
		UserInfo userInfo = UserTable::getInstance()->getUserInfo();
		String sTag = "LeaderBoardLevel";
		sTag.appendWithFormat("_%d", m_iConnectServer);

		if (sKey == sTag.getCString())
		{
			cs::JsonDictionary* jsonData = pJsonDict->getSubDictionary("data");
			if (jsonData != NULL)
			{
				if (jsonData->getItemBoolvalue("result", false))
				{
					m_iLeaderBoardCount = jsonData->getArrayItemCount("list");

					if (levelInfo.bIsUnlock)
					{
						// find index of me
						int iRank = 1;
						for(int iIndex=0; iIndex < m_iLeaderBoardCount; iIndex++)
						{
							cs::JsonDictionary* pJsonItem = jsonData->getSubItemFromArray("list", iIndex);
							int iScore = pJsonItem->getItemIntValue("Score", 0);

							if (iScore > levelInfo.iScore)
								iRank++;
							else
								break;
						}

						if (iRank == 1)
						{
							this->addItemToSlide(levelInfo.iScore, "Me", userInfo.sFacebookId.c_str(), iRank, 0);

							if (m_iLeaderBoardCount > 5)
								m_iLeaderBoardCount = 5;

							for(int iIndex=0; iIndex < m_iLeaderBoardCount; iIndex++)
							{
								iRank++;
								cs::JsonDictionary* pJsonItem = jsonData->getSubItemFromArray("list", iIndex);
								this->parseJsonToLeadeBoard(pJsonItem, iRank, iIndex+1);
							}
						}
						else
						{
							if (iRank > m_iLeaderBoardCount)
							{
								if (iRank >= 6)
								{
									this->addItemToSlide(levelInfo.iScore, "Me", userInfo.sFacebookId.c_str(), iRank, 5);

									cs::JsonDictionary* pJsonItemLeft1 = jsonData->getSubItemFromArray("list", iRank-2);
									this->parseJsonToLeadeBoard(pJsonItemLeft1, iRank-1, 4);

									cs::JsonDictionary* pJsonItemLeft2 = jsonData->getSubItemFromArray("list", iRank-3);
									this->parseJsonToLeadeBoard(pJsonItemLeft2, iRank-2, 3);
								}
								else
								{
									this->addItemToSlide(levelInfo.iScore, "Me", userInfo.sFacebookId.c_str(), iRank, iRank-1);

									cs::JsonDictionary* pJsonItemLeft1 = jsonData->getSubItemFromArray("list", iRank-2);
									this->parseJsonToLeadeBoard(pJsonItemLeft1, iRank-1, iRank-2);

									cs::JsonDictionary* pJsonItemLeft2 = jsonData->getSubItemFromArray("list", iRank-3);
									this->parseJsonToLeadeBoard(pJsonItemLeft2, iRank-2, iRank-3);
								}
							}
							else
							{
								if (iRank >= 6)
								{
									this->addItemToSlide(levelInfo.iScore, "Me", userInfo.sFacebookId.c_str(), iRank, 4);

									cs::JsonDictionary* pJsonItemRight = jsonData->getSubItemFromArray("list", iRank-1);
									this->parseJsonToLeadeBoard(pJsonItemRight, iRank+1, 5);

									cs::JsonDictionary* pJsonItemLeft = jsonData->getSubItemFromArray("list", iRank-2);
									this->parseJsonToLeadeBoard(pJsonItemLeft, iRank-1, 3);
								}
								else
								{
									this->addItemToSlide(levelInfo.iScore, "Me", userInfo.sFacebookId.c_str(), iRank, iRank-1);

									cs::JsonDictionary* pJsonItemRight = jsonData->getSubItemFromArray("list", iRank-1);
									this->parseJsonToLeadeBoard(pJsonItemRight, iRank+1, iRank);

									cs::JsonDictionary* pJsonItemLeft = jsonData->getSubItemFromArray("list", iRank-2);
									this->parseJsonToLeadeBoard(pJsonItemLeft, iRank-1, iRank-2);
								}
							}

							int iCount = 0;
							for (int iIndex=0; iIndex<m_iLeaderBoardCount; iIndex++)
							{
								if(iCount > 2)
									break;

								if (iIndex < iRank-2)
								{
									iCount++;
									cs::JsonDictionary* pJsonItem = jsonData->getSubItemFromArray("list", iIndex);
									this->parseJsonToLeadeBoard(pJsonItem, iIndex+1, iIndex);
								}
								else if (iIndex > iRank-1)
								{
									iCount++;
									cs::JsonDictionary* pJsonItem = jsonData->getSubItemFromArray("list", iIndex);
									this->parseJsonToLeadeBoard(pJsonItem, iIndex+2, iIndex+1);
								}
							}
						}

						m_iLeaderBoardCount = m_iLeaderBoardCount + 1;
						if (m_iLeaderBoardCount > 6 )
							m_iLeaderBoardCount = 6;
					}
					else
					{
						if (m_iLeaderBoardCount > 6)
							m_iLeaderBoardCount = 6;

						for(int iIndex=0; iIndex < m_iLeaderBoardCount; iIndex++)
						{
							cs::JsonDictionary* pJsonItem = jsonData->getSubItemFromArray("list", iIndex);
							this->parseJsonToLeadeBoard(pJsonItem, iIndex+1, iIndex);
						}
					}

					m_iLeaderBoardCount = m_iLeaderBoardCount + 1;
					m_fMinPositionLeft = -((m_iLeaderBoardCount-4)*160.0f);

					Node* pInviteFriendsNode = Node::create();
					Sprite* pButtonInviteImage = Sprite::create("Target-End-Game/btn-add-friend-small.png");

					LabelTTF* pLabelTCreatedBy = LabelTTF::create("Invite Friends", "Arial", 22);
					pLabelTCreatedBy->setColor(ccc3(255.0f, 255.0f, 255.0f));
					pLabelTCreatedBy->setPosition(Point(55.0f, -10.0f));
					pButtonInviteImage->addChild(pLabelTCreatedBy);

					ButtonNode* pButtonInvite = ButtonNode::createButtonSprite(pButtonInviteImage, CC_CALLBACK_1(LeaderBoardtNode::clickInviteFriends, this));
					pButtonInvite->setPosition(Point(80.0f, 70.0f));

					ButtonManagerNode* pButtonManager = ButtonManagerNode::create();
					pButtonManager->addButtonNode(pButtonInvite);
					pInviteFriendsNode->addChild(pButtonManager);

					pInviteFriendsNode->setContentSize(Size(160.0f, 160.0f));
					pInviteFriendsNode->setPosition(Point(-320.0f + (m_iLeaderBoardCount-1)*160, -100.0f));
					m_pSlideShow->addChild(pInviteFriendsNode);
				}
				else
				{

				}
			}
		}
	}
}

void LeaderBoardtNode::parseJsonToLeadeBoard(cs::JsonDictionary* pJsonItem, const int& iRank, const int& iIndex)
{
	if(iRank > 0 && iIndex >= 0)
	{
		int iScore = pJsonItem->getItemIntValue("Score", 0);
		const char* sName = pJsonItem->getItemStringValue("FirstName");
		std::string sFacebookId = pJsonItem->getItemStringValue("FacebookId");
		sFacebookId.pop_back();

		this->addItemToSlide(iScore, sName, sFacebookId.c_str(), iRank, iIndex);
	}
}