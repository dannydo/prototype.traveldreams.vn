#include "TimeCountDownNode.h"


TimeCountDownNode* TimeCountDownNode::create(int iMaximumEnergy, int iEnergyLostPerSecond)
{
	TimeCountDownNode* renderNode = new TimeCountDownNode();
	if( renderNode)
	{				
		renderNode->autorelease();
		renderNode->init( iMaximumEnergy, iEnergyLostPerSecond);
		return renderNode;
	}

	CC_SAFE_DELETE(renderNode);
	return NULL;
}

void TimeCountDownNode::init(int iMaximumEnergy, int iEnergyLostPerSecond)
{
	m_bIsStarted = false;
	m_fMaximumEnergy = iMaximumEnergy;
	m_fEnergyLostPersecond = iEnergyLostPerSecond;
	m_fCurrentEnergy = m_fMaximumEnergy /2.f;

	Size winSize = Director::getInstance()->getWinSize();
	
	//_contentSize.width = winSize.width - 50.f;
	//_contentSize.height = 60.f;
	//_position.x = 0.f; //winSize.width / 2.f;
	//anchor left

	_contentSize.width = 468.f;

	fEnergyWidth = m_fCurrentEnergy * _contentSize.width / m_fMaximumEnergy;

	this->scheduleUpdate();


	// 
	m_pBatchNode = SpriteBatchNode::create("ResourceDemo.pvr.ccz");
	this->addChild(m_pBatchNode);

	auto pBackground = Sprite::createWithSpriteFrameName("TimeBoard.png");
	pBackground->setAnchorPoint(Point(0,0));
	m_pBatchNode->addChild(pBackground);

	m_pMirrorMidEnergyBar = Sprite::createWithSpriteFrameName("Bar_compact.png");
	m_pMirrorMidEnergyBar->setAnchorPoint(Point(0, 0.5f));
	m_pMirrorMidEnergyBar->setPosition(Point(73.f, 108.f));
	m_pMirrorMidEnergyBar->setVisible(false);
	m_pBatchNode->addChild(m_pMirrorMidEnergyBar);

	//auto* pLeftBar = Sprite::createWithSpriteFrameName("Bar_lef.png");
//	pLeftBar->setAnchorPoint(Point(0, 0.5f));
	//pLeftBar->setPosition(Point(77.f, 108.f));
	//m_pBatchNode->addChild(pLeftBar,2);

	m_pMidEnergyBar = Scale9Sprite::createWithSpriteFrameName("Bar.png");
	m_pMidEnergyBar->setInsetLeft(16.f);
	m_pMidEnergyBar->setInsetRight(20.f);
	m_pMidEnergyBar->setContentSize(Size( fEnergyWidth, 34.f));
	m_pMidEnergyBar->setAnchorPoint(Point(0, 0.5f));
	m_pMidEnergyBar->setPosition(Point(77.f, 108.f));
	this->addChild(m_pMidEnergyBar);

	/*Sprite::createWithSpriteFrameName("Bar_mid.png");
	m_pMidEnergyBar->setAnchorPoint(Point(0, 0.5f));
	m_pMidEnergyBar->setPosition(Point( pLeftBar->getPositionX() + pLeftBar->getContentSize().width - 0.5f, 108.f));	
	m_pBatchNode->addChild(m_pMidEnergyBar);

	m_pRightEnergyBar = Sprite::createWithSpriteFrameName("Bar_rig.png");
	m_pRightEnergyBar->setAnchorPoint(Point(0, 0.5f));
	m_pRightEnergyBar->setPosition(Point(148.f, 108.f));
	m_pRightEnergyBar->addChild(m_pRightEnergyBar);

	m_pMidEnergyBar->setScaleX( fEnergyWidth / m_pMidEnergyBar->getContentSize().width);
	m_pRightEnergyBar->setPositionX( m_pMidEnergyBar->getPositionX() + m_pMidEnergyBar->getContentSize().width* m_pMidEnergyBar->getScaleX());*/

	this->setPositionY( winSize.height - pBackground->getContentSize().height);
}

void TimeCountDownNode::update(float fDeltaTime)
{
	if (!m_bIsStarted)
		return;

	if (m_fCurrentEnergy <=0 )
		return;

	m_fCurrentEnergy -= fDeltaTime * m_fEnergyLostPersecond;
	if (m_fCurrentEnergy < 0)
		m_fCurrentEnergy = 0;
	fEnergyWidth = m_fCurrentEnergy * _contentSize.width / m_fMaximumEnergy;
	
	if (fEnergyWidth > 40.f)
	{
		m_pMidEnergyBar->setVisible(true);
		m_pMirrorMidEnergyBar->setVisible(false);
	
		m_pMidEnergyBar->setContentSize(Size( fEnergyWidth, 34.f));
	}
	else
	{
		m_pMidEnergyBar->setVisible(false);
		m_pMirrorMidEnergyBar->setVisible(true);

		//m_pMirrorMidEnergyBar->setScaleX( fEnergyWidth / m_pMidEnergyBar->getContentSize().width);
		m_pMirrorMidEnergyBar->setOpacity(fEnergyWidth*255/4);
	}
	//m_pMidEnergyBar->setScaleX( fEnergyWidth / m_pMidEnergyBar->getContentSize().width);
	//m_pRightEnergyBar->setPositionX( m_pMidEnergyBar->getPositionX() + m_pMidEnergyBar->getContentSize().width* m_pMidEnergyBar->getScaleX());

	if (m_fCurrentEnergy == 0)
	{
		m_OutOfTimeCallback();
	}
}

void TimeCountDownNode::draw()
{
	/*
	Size winSize = Director::getInstance()->getWinSize();

	CHECK_GL_ERROR_DEBUG();

	glLineWidth( 12.0f );
	DrawPrimitives::setDrawColor4B(255,255,255,255);	
	DrawPrimitives::drawLine( Point( 0, 0), Point( _contentSize.width, 0));

	glLineWidth( 6.0f );
	DrawPrimitives::setDrawColor4B(0,0,0,255);
	DrawPrimitives::drawLine( Point( 0, 1.f), Point( fEnergyWidth, 1.f));

	CHECK_GL_ERROR_DEBUG();*/
}

