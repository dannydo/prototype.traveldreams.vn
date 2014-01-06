#include "StatusLayer.h"
#include "Config.h"

USING_NS_CC;

bool StatusLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	m_SpriteNumberMove = new Array();
	m_SpriteNumberMove->initWithCapacity(0);

	m_SpriteNumberScore = new Array();
	m_SpriteNumberScore->initWithCapacity(0);

	m_pSpriteBatchNode = SpriteBatchNode::create("Score-Star/Score-Star.pvr.ccz");
	this->addChild(m_pSpriteBatchNode);
	SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Score-Star/Score-Star.plist");

	Sprite* background = Sprite::createWithSpriteFrameName("Board.png");
	background->setPosition(Point(background->getContentSize().width/2.0f, background->getContentSize().height/2.0f));
	m_pSpriteBatchNode->addChild(background);
	this->setContentSize(background->getContentSize());

	m_iCurrentScore = 0;
	m_iScoreOld = 0;
	m_iCurrentMove = 0;

	m_centerCircle = Point(117, 88);
	m_fRadiusCircle = 90.0f;

	this->clippingNodeCircle();

	m_pStarBlack1 = Sprite::createWithSpriteFrameName("Gray_Star.png");
	m_pSpriteBatchNode->addChild(m_pStarBlack1);

	m_pStarBlack2 = Sprite::createWithSpriteFrameName("Gray_Star.png");
	m_pSpriteBatchNode->addChild(m_pStarBlack2);

	m_pStarBlack3 = Sprite::createWithSpriteFrameName("Gray_Star.png");
	m_pSpriteBatchNode->addChild(m_pStarBlack3);

	m_pStarYellow1 = Sprite::createWithSpriteFrameName("Gold_Star.png");
	m_pStarYellow1->setVisible(false);
	m_pSpriteBatchNode->addChild(m_pStarYellow1);

	m_pStarYellow2 = Sprite::createWithSpriteFrameName("Gold_Star.png");
	m_pStarYellow2->setVisible(false);
	m_pSpriteBatchNode->addChild(m_pStarYellow2);

	m_pStarYellow3 = Sprite::createWithSpriteFrameName("Gold_Star.png");
	m_pStarYellow3->setVisible(false);
	m_pSpriteBatchNode->addChild(m_pStarYellow3);

	m_fDeltaTime = 0;
	m_iSpeedUpdateScore = 10;
	this->scheduleUpdate();

	return true;
}

void StatusLayer::clippingNodeCircle()
{
    Sprite* pYellowBar = Sprite::create("Score-Star/yellow_bar.png");
	m_pMarkCircle = Sprite::create("Score-Star/mask.png");
	m_pMarkCircle->setPosition(Point(0.0f, -40.0f));
	
	ClippingNode* clipperMask = ClippingNode::create();
    clipperMask->addChild(pYellowBar);
    clipperMask->setStencil(m_pMarkCircle);
	clipperMask->setContentSize(pYellowBar->getContentSize());
	clipperMask->setPosition(Point(m_centerCircle.x, m_centerCircle.y + pYellowBar->getContentSize().height/2.0f));
	m_pMarkCircle->setAnchorPoint(Point(0.5, 1));
    
	this->addChild(clipperMask);
}

void StatusLayer::update(float dt)
{	
	m_fDeltaTime += dt;
	if (m_fDeltaTime >= 0.1)
	{
		this->updateScore();
	}
}

void StatusLayer::updateScore()
{
	if (m_iCurrentScore != 0 && m_iScoreOld < m_iCurrentScore)
	{
		int bonus = m_fDeltaTime*m_iSpeedUpdateScore*10;
		int temp = m_iScoreOld + bonus;

		if (temp > m_iMaxScoreLevel)
		{
			bonus = m_iMaxScoreLevel - m_iScoreOld;
		}

		if (temp > m_iCurrentScore)
		{ 
			temp = m_iCurrentScore;
			if (temp < m_iMaxScoreLevel)
			{
				bonus = m_iCurrentScore - m_iScoreOld;
			}
		}

		this->generateLayoutScore(temp);

		if (m_iScoreOld < m_iMaxScoreLevel)
		{
			float angle = bonus/m_iMaxScoreLevel*180;
			auto actionRotate = RotateBy::create(0.0f, angle);
			m_pMarkCircle->runAction(actionRotate);
			m_iScoreOld = temp;

			if (m_iScoreOld >= m_iScore1Star)
			{
				m_pStarYellow1->setVisible(true);
			}

			if (m_iScoreOld >= m_iScore2Star)
			{
				m_pStarYellow2->setVisible(true);
			}

			if (m_iScoreOld >= m_iScore3Star)
			{
				m_pStarYellow3->setVisible(true);
			}
		}
		m_iScoreOld = temp;
	}
	else if (m_iCurrentScore == 0)
	{
		this->generateLayoutScore(0);
	}
	m_fDeltaTime = 0;
}

void StatusLayer::draw()
{

}
		
void StatusLayer::setCurrentScore(const int& iCurrentScore)
{
	m_iCurrentScore = iCurrentScore;
}

void StatusLayer::setScoreForStar(const float& iScore1Star, const float& iScore2Star, const float& iScore3Star, const float& iMaxScoreLevel)
{
	m_iScore1Star = iScore1Star;
	m_iScore2Star = iScore2Star;
	m_iScore3Star = iScore3Star;
	m_iMaxScoreLevel = iMaxScoreLevel;

	this->setPositionForStar(m_iScore1Star, m_pStarBlack1);
	this->setPositionForStar(m_iScore2Star, m_pStarBlack2);
	this->setPositionForStar(m_iScore3Star, m_pStarBlack3);

	this->setPositionForStar(m_iScore1Star, m_pStarYellow1);
	this->setPositionForStar(m_iScore2Star, m_pStarYellow2);
	this->setPositionForStar(m_iScore3Star, m_pStarYellow3);
}

void StatusLayer::setPositionForStar(const float& iScoreStar, Sprite* pStar)
{
	Point point;
	float angle = _C_PI_ - iScoreStar/m_iMaxScoreLevel*_C_PI_;

	point.x = m_centerCircle.x + m_fRadiusCircle*cos(angle);
	point.y = m_centerCircle.y + m_fRadiusCircle*sin(angle);
	if (angle == 0 || angle == _C_PI_ || point.y < m_centerCircle.y + pStar->getContentSize().height/4.0f)
	{
		point.y = point.y + pStar->getContentSize().height/4.0f;
	}
	pStar->setPosition(point);	

	float angleRotate = iScoreStar/m_iMaxScoreLevel*180;
	if (angleRotate < 90)
	{
		angleRotate = angleRotate - 90;
	}
	else if (angleRotate >= 90)
	{
		angleRotate = angleRotate - 90;
	}
	
	auto action = RotateBy::create(0.0f, angleRotate);
	pStar->runAction(action);
}

void StatusLayer::setCurrentMove(const int& iCurrentMove)
{
	m_iCurrentMove = iCurrentMove;
	this->generateLayoutMove();
}

void StatusLayer::setSpeedUpdateScore(const float& iSpeedUpdateScore)
{
	m_iSpeedUpdateScore = iSpeedUpdateScore;
}

std::vector<int> StatusLayer::generateArrayNumber(int iNumber)
{
	int iTemp = iNumber;
	std::vector<int> arrNumber;
	if (iNumber < 1)
	{
		arrNumber.push_back(0);
	}
	else
	{
		while(iTemp >= 1)
		{
			arrNumber.push_back(iTemp%10);
			iTemp = iTemp/10.0f;
		}
	}

	return arrNumber;
}

void StatusLayer::generateLayoutMove()
{
	std::vector<int> arrNumber = this->generateArrayNumber(m_iCurrentMove);
	int width = 0;
	int height;
	Node* node = Node::create();
	int iTotal = arrNumber.size();

	while(m_SpriteNumberMove->count() > 0)
	{
		m_pSpriteBatchNode->removeChild((Sprite*)m_SpriteNumberMove->getObjectAtIndex(0), false);
		m_SpriteNumberMove->removeObjectAtIndex(0);
	}
	
	while(!arrNumber.empty())
	{
		char sFileName[35];
		sprintf(sFileName, "NumMoves_%d.png", arrNumber.back());
		Sprite* sprite = Sprite::createWithSpriteFrameName(sFileName);
		
		height = sprite->getContentSize().height;
		sprite->setPosition(Point(137 - iTotal*18 + width, 101 + height/2.0f));
		width = width + sprite->getContentSize().width - 5;
		m_pSpriteBatchNode->addChild(sprite);
		arrNumber.pop_back();
		m_SpriteNumberMove->addObject(sprite);
	}
}

void StatusLayer::generateLayoutScore(int iScore)
{
	std::vector<int> arrNumber = this->generateArrayNumber(iScore);
	int width = 0;
	int height;
	Node* node = Node::create();
	int iTotal = arrNumber.size();

	while(m_SpriteNumberScore->count() > 0)
	{
		m_pSpriteBatchNode->removeChild((Sprite*)m_SpriteNumberScore->getObjectAtIndex(0), false);
		m_SpriteNumberScore->removeObjectAtIndex(0);
	}
	
	while(!arrNumber.empty())
	{
		char sFileName[35];
		sprintf(sFileName, "NumScore_%d.png", arrNumber.back());
		Sprite* sprite = Sprite::createWithSpriteFrameName(sFileName);

		height = sprite->getContentSize().height;
		sprite->setPosition(Point(127 - iTotal*8 + width, 42 + height/2.0f));
		width = width + sprite->getContentSize().width - 17;
		m_pSpriteBatchNode->addChild(sprite);
		arrNumber.pop_back();
		m_SpriteNumberScore->addObject(sprite);
	}
}