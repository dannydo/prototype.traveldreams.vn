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

	Sprite* pSpriteBackground = Sprite::create("Target-End-Game/panel-avatar.png");
	pSpriteBackground->setAnchorPoint(Point(0.5f, 0.5f));
	pSpriteBackground->setPosition(Point(0.0f, 0.0f));
	this->addChild(pSpriteBackground);
	this->setContentSize(pSpriteBackground->getContentSize());

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

	
	m_fSlideShowPositionX = 0.0f;
	m_bMoveSlideShow = false;	  

	m_iNumMove = 0;
	m_iLeaderBoardCount = 0;
	for(int iIndex=0; iIndex<6; iIndex++)
	{
		m_arrIndex[iIndex] = -1;
	}

	UserService::getInstance()->addCallBackList(this);
	UserService::getInstance()->getLeaderBoardLevel(m_iLevel);

	return true;
}

void LeaderBoardtNode::addItemToSlide(const int& iScore, const char* sName, const char* sFacebookId, const int& iRank, const int& iIndex)
{
	Node* pNodeItem = Node::create();

	char sScore[10];
	sprintf( sScore, "%d", iScore);
	LabelTTF* pLabelScore = LabelTTF::create(sScore, "Arial", 20);
	pLabelScore->setColor(ccc3(0, 0, 0));
	pLabelScore->setAnchorPoint(Point(0.5f, 0.0f));
	pNodeItem->addChild(pLabelScore);

	Sprite* pSpriteBackgroundAvatar = Sprite::create("Target-End-Game/avatar border.png");
	pSpriteBackgroundAvatar->setAnchorPoint(Point(0.5f, 0.0f));
	pSpriteBackgroundAvatar->setPositionY(27.0f);
	pNodeItem->addChild(pSpriteBackgroundAvatar);

	std::string urlAvatar = "https://graph.facebook.com/";
	urlAvatar.append(sFacebookId);
	urlAvatar.append("/picture");

	LoadingImagetNode* avatar = LoadingImagetNode::createLayout(urlAvatar.c_str());
	avatar->setAnchorPoint(Point(0.5f, 0.0f));
	avatar->setPositionY(61.0f);
	avatar->setScale(1.2f);
	pNodeItem->addChild(avatar);

	char sTitle[50];
	sprintf(sTitle, "%d.%s", iRank, sName);
	LabelTTF* pLabelTitle = LabelTTF::create(sTitle, "Arial", 20);
	pLabelTitle->setColor(ccc3(0, 0, 0));
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.0f));
	pLabelTitle->setPositionY(100.0f);
	pNodeItem->addChild(pLabelTitle);
	pNodeItem->setContentSize(Size(150.0f, 150.0f));

	pNodeItem->setPosition(Point(-240.0f + iIndex*160, -100.0f));
	pNodeItem->setTag(iIndex);
	m_pSlideShow->addChild(pNodeItem);
}

void LeaderBoardtNode::addButtonShowMe()
{
	MenuItemImage* pShowMeItem = MenuItemImage::create(
		"Target-End-Game/Me-button.png",
		"Target-End-Game/Me-button.png",
		CC_CALLBACK_0(LeaderBoardtNode::menuShowMeCallback, this));
	pShowMeItem->setPosition(Point(230.0f, 85.0f));

	Menu* pMenu = Menu::create(pShowMeItem, NULL);
	pMenu->setPosition(Point::ZERO);
	this->addChild(pMenu);
}

void LeaderBoardtNode::menuShowMeCallback()
{
}


void LeaderBoardtNode::onTouchCustomNodeBegan(Touch* pTouch,  Event* pEvent)
{
	Point touchPosition = pTouch->getLocation();
	m_fBeginX = touchPosition.x;
}

void LeaderBoardtNode::onTouchCustomNodeMoved(Touch* pTouch,  Event* pEvent)
{
	if (m_iLeaderBoardCount > 4)
	{
		if (m_bMoveSlideShow == false)
		{
			Point touchPosition = pTouch->getLocation();
			m_fXMoved = touchPosition.x - m_fBeginX;
			m_fBeginX = touchPosition.x;
			m_fSlideShowPositionX += m_fXMoved;

			if(m_fSlideShowPositionX >= 0.0f)
			{
				m_fSlideShowPositionX = 0.0f;
				m_fXMoved = m_fSlideShowPositionX - m_pSlideShow->getPositionX();
			}
	
			if(m_fSlideShowPositionX <= m_fMinPositionLeft)
			{
				m_fSlideShowPositionX = m_fMinPositionLeft;
				m_fXMoved = m_fSlideShowPositionX - m_pSlideShow->getPositionX();
			}

			if (m_fXMoved != 0)
			{
				m_iNumMove++;
				m_bMoveSlideShow = true;
				auto actionMove = MoveBy::create(0.1f, Point(m_fXMoved, 0.0f));
				auto actionUpdate = CallFunc::create(this, callfunc_selector(LeaderBoardtNode::updateScrollSlideShow));
				m_pSlideShow->runAction(Sequence::create(actionMove, actionUpdate, NULL));
				m_fBeginX = touchPosition.x;
			}
		}
	}
}

void LeaderBoardtNode::onTouchCustomNodeEnded(Touch* pTouch,  Event* pEvent)
{
	if (m_iNumMove == 1)
	{
		
		if (fabsf(m_fXMoved) < 100)
		{
			auto actionUpdate = CallFunc::create(this, callfunc_selector(LeaderBoardtNode::updateScrollSlideShow));
			float fDeltaMove = 320.0f;
			if (m_fXMoved > 0)
			{
				if (m_fSlideShowPositionX < 0.0f)
				{
					if(m_fSlideShowPositionX + fDeltaMove > 0.0f)
					{
						fDeltaMove = -m_fSlideShowPositionX;
					}

					m_fSlideShowPositionX += fDeltaMove;
					auto actionMove = MoveBy::create(0.3f, Point(fDeltaMove, 0.0f));
					m_pSlideShow->runAction(Sequence::create(DelayTime::create(0.1f)->clone(), actionMove, actionUpdate, NULL));
				}
			}
			else
			{
				if (m_fSlideShowPositionX > m_fMinPositionLeft)
				{
					fDeltaMove = -320.0f;
					if(m_fSlideShowPositionX + fDeltaMove < m_fMinPositionLeft)
					{
						fDeltaMove = m_fMinPositionLeft - m_fSlideShowPositionX;
					}

					m_fSlideShowPositionX += fDeltaMove;
					auto actionMove = MoveBy::create(0.3f, Point(fDeltaMove, 0.0f));
					m_pSlideShow->runAction(Sequence::create(DelayTime::create(0.1f)->clone(), actionMove, actionUpdate, NULL));
				}
			}
		}
	}
	m_iNumMove = 0;
}

void LeaderBoardtNode::updateScrollSlideShow()
{
	m_bMoveSlideShow = false;
}

void LeaderBoardtNode::resultHttpRequestCompleted(cs::JsonDictionary* pJsonDict, std::string sKey)
{
	LevelInfo levelInfo =  LevelTable::getInstance()->fetchLevel(m_iLevel);
	UserInfo userInfo = UserTable::getInstance()->getUserInfo();

	if (sKey == "LeaderBoardLevel")
	{
		m_iLeaderBoardCount = pJsonDict->getArrayItemCount("data");
		int iWidth = 160;

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
			
			m_fMinPositionLeft = -((m_iLeaderBoardCount-4)*160.0f);
		}
		else
		{
			if (m_iLeaderBoardCount > 6)
				m_iLeaderBoardCount = 6;

			m_fMinPositionLeft = -((m_iLeaderBoardCount-4)*160.0f);

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