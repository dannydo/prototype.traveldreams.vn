#include "LeaderBoardNode.h"
#include "APIService\UserService.h"

#include "APIService\Constants.h"
#include "Database\UserTable.h"
#include "Database\LevelTable.h"

#include "LoadingImageNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

LeaderBoardtNode::~LeaderBoardtNode()
{
	UserService::getInstance()->removeCallBackList(this);
}

LeaderBoardtNode* LeaderBoardtNode::createLayout(const int& iLevel)
{
	LeaderBoardtNode* pLeaderBoardNode = new LeaderBoardtNode();
	pLeaderBoardNode->m_iLevel = iLevel;
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

	m_pSpriteBackground = Sprite::create("Target-End-Game/panel-avatar.png");
	m_pSpriteBackground->setAnchorPoint(Point(0.5f, 0.5f));
	m_pSpriteBackground->setPosition(Point(0.0f, 0.0f));
	this->addChild(m_pSpriteBackground);
	this->setContentSize(m_pSpriteBackground->getContentSize());

	m_pSlideShow = Node::create();
	m_pSlideShow->setAnchorPoint(Point(0.0f, 0.5f));
	m_pSlideShow->setPositionX(0.0f);
	this->addChild(m_pSlideShow);

	Sprite* pSpriteLeft = Sprite::create("Target-End-Game/bleft.png");
	pSpriteLeft->setAnchorPoint(Point(0.5f, 0.5f));
	pSpriteLeft->setPosition(Point(-304.0f, 0.0f));
	this->addChild(pSpriteLeft);

	Sprite* pSpriteRight = Sprite::create("Target-End-Game/bright.png");
	pSpriteRight->setAnchorPoint(Point(0.5f, 0.5f));
	pSpriteRight->setPosition(Point(305.0f, 0.0f));
	this->addChild(pSpriteRight);

	m_pScrollManager = new ScrollManager();
	m_iLeaderBoardCount = 0;

	for(int iIndex=0; iIndex<6; iIndex++)
	{
		m_arrIndex[iIndex] = -1;
	}

	UserInfo userInfo = UserTable::getInstance()->getUserInfo();
	if (userInfo.sFacebookId != "")
	{
		UserService::getInstance()->addCallBackList(this);
		UserService::getInstance()->getLeaderBoardLevel(m_iLevel);
	}

	m_bIsSwipe = false;

	return true;
}

void LeaderBoardtNode::addItemToSlide(const int& iScore, const char* sName, const char* sFacebookId, const int& iRank, const int& iIndex)
{
	Node* pNodeItem = Node::create();

	Sprite* pLoadAvatar = Sprite::create("Target-End-Game/avatar.png");
	pLoadAvatar->setAnchorPoint(Point(0.5f, 0.0f));
	pLoadAvatar->setPositionY(30.0f);
	pNodeItem->addChild(pLoadAvatar);

	std::string urlAvatar = "https://graph.facebook.com/";
	urlAvatar.append(sFacebookId);
	urlAvatar.append("/picture");

	LoadingImagetNode* avatar = LoadingImagetNode::createLayout(urlAvatar.c_str());
	avatar->setAnchorPoint(Point(0.5f, 0.0f));
	avatar->setPositionY(61.0f);
	avatar->setScale(1.2f);
	pNodeItem->addChild(avatar);

	Sprite* pBacground = Sprite::create("Target-End-Game/leaderboard_frame.png");
	pBacground->setAnchorPoint(Point(0.5f, 0.0f));
	pBacground->setPosition(Point(0.0f, -8.0f));
	pNodeItem->addChild(pBacground);

	char sScore[10];
	sprintf( sScore, "%d", iScore);
	LabelTTF* pLabelScore = LabelTTF::create(sScore, "fonts/UTM Cookies.ttf", 18);
	pLabelScore->setColor(ccc3(241, 231, 207));
	pLabelScore->setAnchorPoint(Point(0.5f, 0.0f));
	pLabelScore->setPositionY(7.0f);
	pNodeItem->addChild(pLabelScore);

	char sTitle[50];
	sprintf(sTitle, "%d.%s", iRank, sName);
	LabelTTF* pLabelTitle = LabelTTF::create(sTitle, "fonts/UTM Cookies.ttf", 18);
	pLabelTitle->setColor(ccc3(85, 83, 78));
	if (iRank == 1)
		pLabelTitle->setColor(ccc3(238, 41, 16));
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.0f));
	pLabelTitle->setPositionY(105.0f);
	pNodeItem->addChild(pLabelTitle);
	pNodeItem->setContentSize(Size(150.0f, 150.0f));

	pNodeItem->setPosition(Point(-235.0f + iIndex*118, -100.0f));
	pNodeItem->setTag(iIndex);
	m_pSlideShow->addChild(pNodeItem);
}

bool LeaderBoardtNode::onTouchCustomNodeBegan(Touch* pTouch,  Event* pEvent)
{
	if(m_bIsSwipe)
	{
		return false;
	}
	m_bIsSwipe = true;

	if (m_iLeaderBoardCount > 5)
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
	LevelInfo levelInfo =  LevelTable::getInstance()->fetchLevel(m_iLevel);
	UserInfo userInfo = UserTable::getInstance()->getUserInfo();

	if (sKey == "LeaderBoardLevel")
	{
		m_iLeaderBoardCount = pJsonDict->getArrayItemCount("data");

		if (levelInfo.bIsUnlock)
		{
			// find index of me
			int iRank = 1;
			for(int iIndex=0; iIndex < m_iLeaderBoardCount; iIndex++)
			{
				cs::JsonDictionary* pJsonItem = pJsonDict->getSubItemFromArray("data", iIndex);
				int iScore = int(strtod(pJsonItem->getItemStringValue("Score"), NULL));

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
					cs::JsonDictionary* pJsonItem = pJsonDict->getSubItemFromArray("data", iIndex);
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

						cs::JsonDictionary* pJsonItemLeft1 = pJsonDict->getSubItemFromArray("data", iRank-2);
						this->parseJsonToLeadeBoard(pJsonItemLeft1, iRank-1, 4);

						cs::JsonDictionary* pJsonItemLeft2 = pJsonDict->getSubItemFromArray("data", iRank-3);
						this->parseJsonToLeadeBoard(pJsonItemLeft2, iRank-2, 3);
					}
					else
					{
						this->addItemToSlide(levelInfo.iScore, "Me", userInfo.sFacebookId.c_str(), iRank, iRank-1);

						cs::JsonDictionary* pJsonItemLeft1 = pJsonDict->getSubItemFromArray("data", iRank-2);
						this->parseJsonToLeadeBoard(pJsonItemLeft1, iRank-1, iRank-2);

						cs::JsonDictionary* pJsonItemLeft2 = pJsonDict->getSubItemFromArray("data", iRank-3);
						this->parseJsonToLeadeBoard(pJsonItemLeft2, iRank-2, iRank-3);
					}
				}
				else
				{
					if (iRank >= 6)
					{
						this->addItemToSlide(levelInfo.iScore, "Me", userInfo.sFacebookId.c_str(), iRank, 4);

						cs::JsonDictionary* pJsonItemRight = pJsonDict->getSubItemFromArray("data", iRank-1);
						this->parseJsonToLeadeBoard(pJsonItemRight, iRank+1, 5);

						cs::JsonDictionary* pJsonItemLeft = pJsonDict->getSubItemFromArray("data", iRank-2);
						this->parseJsonToLeadeBoard(pJsonItemLeft, iRank-1, 3);
					}
					else
					{
						this->addItemToSlide(levelInfo.iScore, "Me", userInfo.sFacebookId.c_str(), iRank, iRank-1);

						cs::JsonDictionary* pJsonItemRight = pJsonDict->getSubItemFromArray("data", iRank-1);
						this->parseJsonToLeadeBoard(pJsonItemRight, iRank+1, iRank);

						cs::JsonDictionary* pJsonItemLeft = pJsonDict->getSubItemFromArray("data", iRank-2);
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
						cs::JsonDictionary* pJsonItem = pJsonDict->getSubItemFromArray("data", iIndex);
						this->parseJsonToLeadeBoard(pJsonItem, iIndex+1, iIndex);
					}
					else if (iIndex > iRank)
					{
						iCount++;
						cs::JsonDictionary* pJsonItem = pJsonDict->getSubItemFromArray("data", iIndex);
						this->parseJsonToLeadeBoard(pJsonItem, iIndex+1, iIndex);
					}
				}
			}

			m_iLeaderBoardCount = m_iLeaderBoardCount + 1;
			if (m_iLeaderBoardCount > 6 )
				m_iLeaderBoardCount = 6;
			
			m_fMinPositionLeft = -((m_iLeaderBoardCount-5)*118.0f);
		}
		else
		{
			if (m_iLeaderBoardCount > 6)
				m_iLeaderBoardCount = 6;

			m_fMinPositionLeft = -((m_iLeaderBoardCount-5)*118.0f);

			for(int iIndex=0; iIndex < m_iLeaderBoardCount; iIndex++)
			{
				cs::JsonDictionary* pJsonItem = pJsonDict->getSubItemFromArray("data", iIndex);
				this->parseJsonToLeadeBoard(pJsonItem, iIndex+1, iIndex);
			}
		}
	}
}

void LeaderBoardtNode::parseJsonToLeadeBoard(cs::JsonDictionary* pJsonItem, const int& iRank, const int& iIndex)
{
	if(iRank > 0 && iIndex >= 0)
	{
		int iScore = int(strtod(pJsonItem->getItemStringValue("Score"), NULL));
		const char* sName = pJsonItem->getItemStringValue("FirstName");
		const char* sFacebookId = pJsonItem->getItemStringValue("FacebookId");

		this->addItemToSlide(iScore, sName, sFacebookId, iRank, iIndex);
	}
}