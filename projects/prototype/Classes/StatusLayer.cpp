#include "StatusLayer.h"
#include "Config.h"

USING_NS_CC;

bool StatusLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Sprite* background = Sprite::create("Score-Star/Board.png");
	this->addChild(background);
	
	m_SpriteNumberMove = Node::create();
	m_SpriteNumberMove->setAnchorPoint(Point(0.5, 0.5));
	this->addChild(m_SpriteNumberMove);

	m_iCurrentScore = 0;
	m_iScoreOld = 0;
	m_iCurrentMove = 0;

	m_centerCircle = Point(82.5, 82.5);
	m_fRadiusCircle = 90.0f;

	this->clippingNodeCircle();

	m_pStarBlack1 = Sprite::create("Score-Star/Gray_Star.png");
	this->addChild(m_pStarBlack1);

	m_pStarBlack2 = Sprite::create("Score-Star/Gray_Star.png");
	this->addChild(m_pStarBlack2);

	m_pStarBlack3 = Sprite::create("Score-Star/Gray_Star.png");
	this->addChild(m_pStarBlack3);

	m_pStarYellow1 = Sprite::create("Score-Star/Gold_Star.png");
	m_pStarYellow1->setVisible(false);
	this->addChild(m_pStarYellow1);

	m_pStarYellow2 = Sprite::create("Score-Star/Gold_Star.png");
	m_pStarYellow2->setVisible(false);
	this->addChild(m_pStarYellow2);

	m_pStarYellow3 = Sprite::create("Score-Star/Gold_Star.png");
	m_pStarYellow3->setVisible(false);
	this->addChild(m_pStarYellow3);

	m_fDeltaTime = 0;
	m_iSpeedUpdateScore = 10;
	this->scheduleUpdate();
	this->setPosition(Point(35.0f, 25.0f));

	return true;
}

void StatusLayer::clippingNodeCircle()
{
    Sprite* pYellowBar = Sprite::create("Score-Star/yellow_bar.png");
	m_pMarkCircle = Sprite::create("Score-Star/mask.png");
	
	ClippingNode* clipperMask = ClippingNode::create();
    clipperMask->addChild(pYellowBar);
    clipperMask->setStencil(m_pMarkCircle);
	clipperMask->setContentSize(pYellowBar->getContentSize());
	m_pMarkCircle->setAnchorPoint(Point(0.5f, 1.0f));
	m_pMarkCircle->setRotation(-15.0f/180.0f);
    
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

		if (m_iScoreOld < m_iMaxScoreLevel)
		{
			float angle = bonus/m_iMaxScoreLevel*125;
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

	m_fDeltaTime = 0;
}

void StatusLayer::draw()
{

}
		
void StatusLayer::setCurrentScore(const int& iCurrentScore)
{
	m_iCurrentScore = iCurrentScore;
}

void StatusLayer::setScoreForStar(const float& iScore1Star, const float& iScore2Star, const float& iScore3Star)
{
	m_iScore1Star = iScore1Star;
	m_iScore2Star = iScore2Star;
	m_iScore3Star = iScore3Star;
	m_iMaxScoreLevel = iScore3Star*120.0f/100.0f;

	Point point;
	float angle = _C_PI_-(iScore1Star*115/iScore3Star -15)/180.0f*_C_PI_;
	point.x = m_fRadiusCircle*cos(angle);
	point.y = m_fRadiusCircle*sin(angle);
	m_pStarYellow1->setPosition(point);
	m_pStarBlack1->setPosition(point);

	float angleRotate = (iScore1Star*115/iScore3Star -15.0f) - 90.0f;
	m_pStarBlack1->setRotation(angleRotate);
	m_pStarYellow1 ->setRotation(angleRotate);

	angle = _C_PI_-(iScore2Star*115/iScore3Star -15)/180.0f*_C_PI_;
	point.x = m_fRadiusCircle*cos(angle);
	point.y = m_fRadiusCircle*sin(angle);
	m_pStarYellow2->setPosition(point);
	m_pStarBlack2->setPosition(point);

	angleRotate = (iScore2Star*115/iScore3Star - 15.0f) - 90.0f;
	m_pStarBlack2->setRotation(angleRotate);
	m_pStarYellow2 ->setRotation(angleRotate);


	angle = _C_PI_-100.0f/180.0f*_C_PI_;
	point.x = m_fRadiusCircle*cos(angle);
	point.y = m_fRadiusCircle*sin(angle);
	m_pStarYellow3->setPosition(point);
	m_pStarBlack3->setPosition(point);

	angleRotate = 100.0f - 90.0f;
	m_pStarBlack3->setRotation(angleRotate);
	m_pStarYellow3 ->setRotation(angleRotate);
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

	m_SpriteNumberMove->removeAllChildren();

	while(!arrNumber.empty())
	{
		char sFileName[50];
		sprintf(sFileName, "Score-Star/NumMoves/%d.png", arrNumber.back());
		Sprite* sprite = Sprite::create(sFileName);
		sprite->setPosition(Point(15.0f + width, 25.0f));
		width = width + sprite->getContentSize().width;

		m_SpriteNumberMove->addChild(sprite);
		arrNumber.pop_back();
	}

	m_SpriteNumberMove->setPositionX(width/4.0f - (iTotal-1)*15 - 32);
}

void StatusLayer::hiddenLayoutMove()
{
	m_SpriteNumberMove->setVisible(false);
}
