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
	background->setPosition(Point(background->getContentSize().width/2.0f, background->getContentSize().height/2.0f));
	this->addChild(background);
	this->setContentSize(background->getContentSize());

	m_iCurrentScore = 0;
	m_iScoreOld = 0;
	m_pScore = Node::create();
	m_pScore->setPosition(Point(background->getContentSize().width/2.0f, 37));
	this->addChild(m_pScore);

	m_iCurrentMove = 0;
	m_pMove = Node::create();
	m_pMove->setPosition(Point(background->getContentSize().width/2.0f, 95));
	this->addChild(m_pMove);

	m_centerCircle = Point(117, 88);
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

	m_iSpeedUpdateScore = 0.02;

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
	this->generateLayoutMove();
	this->sequenceUpdateScore();
}

void StatusLayer::updateScore()
{
	if (m_iCurrentScore != 0)
	{
		int bonus = m_iDeltaUpdateScore;
		m_iScoreOld = m_iScoreOld + bonus;
		if (m_iScoreOld > m_iCurrentScore)
		{
			m_iScoreOld = m_iCurrentScore;
			bonus = m_iCurrentScore - m_iScoreOld; 
		}

		this->generateLayoutScore(m_iScoreOld);

		float angle = bonus/m_iMaxScoreLevel*180;
		auto actionRotate = RotateBy::create(0.0f, angle);
		m_pMarkCircle->runAction(actionRotate);

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
}

void StatusLayer::draw()
{

}

void StatusLayer::sequenceUpdateScore()
{
	auto actionUpdateScore = CallFunc::create(this, callfunc_selector(StatusLayer::updateScore));
	auto delay = DelayTime::create(m_iSpeedUpdateScore);
	auto actionLoopUpdateScore = CallFunc::create(this, callfunc_selector(StatusLayer::loopUpdateScore));
	this->runAction(Sequence::create(actionUpdateScore, delay->clone(), actionLoopUpdateScore, NULL));
}

void StatusLayer::loopUpdateScore()
{
	if (m_iScoreOld < m_iCurrentScore)
	{
		this->sequenceUpdateScore();
	}
}

void StatusLayer::setCurrentScore(const int& iCurrentScore)
{
	m_iDeltaUpdateScore = (iCurrentScore - m_iCurrentScore)/10;
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
}

void StatusLayer::setSpeedUpdateScore(const int& iSpeedUpdateScore)
{
	m_iSpeedUpdateScore = iSpeedUpdateScore;
}

std::vector<int> StatusLayer::generateArrayNumber(int iNumber)
{
	int iTemp = iNumber;
	std::vector<int> arrNumber;
	while(iTemp >= 1)
	{
		arrNumber.push_back(iTemp%10);
		iTemp = iTemp/10.0f;
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
	
	while(!arrNumber.empty())
	{
		char sFileName[35];
		sprintf(sFileName, "Score-Star/NumMoves/NumMoves_%d.png", arrNumber.back());
		Sprite* sprite = Sprite::create(sFileName);
		
		height = sprite->getContentSize().height;
		sprite->setPosition(Point(width, height/2.0f));
		width = width + sprite->getContentSize().width - 5;
		node->addChild(sprite);
		arrNumber.pop_back();
	}

	this->removeChild(m_pMove);
	m_pMove = node;									   
	m_pMove->setContentSize(CCSizeMake(width, height));
	m_pMove->setPosition(Point(117 - width/2.0f + width/(2.0f*iTotal), 100));
	this->addChild(m_pMove);
}

void StatusLayer::generateLayoutScore(int iScore)
{
	std::vector<int> arrNumber = this->generateArrayNumber(iScore);
	int width = 0;
	int height;
	Node* node = Node::create();
	int iTotal = arrNumber.size();
	
	while(!arrNumber.empty())
	{
		char sFileName[35];
		sprintf(sFileName, "Score-Star/NumScore/NumScore_%d.png", arrNumber.back());
		Sprite* sprite = Sprite::create(sFileName);

		height = sprite->getContentSize().height;
		sprite->setPosition(Point(width, height/2.0f));
		width = width + sprite->getContentSize().width - 17;
		node->addChild(sprite);
		arrNumber.pop_back();
	}

	this->removeChild(m_pScore);
	m_pScore = node;									   
	m_pScore->setContentSize(CCSizeMake(width, height));
	m_pScore->setPosition(Point(117 - width/2.0f + width/(2.0f*iTotal), 42));
	this->addChild(m_pScore);
}