#include "TimeCountDownNode.h"


TimeCountDownNode* TimeCountDownNode::create(int iStageIndex, int iMaximumEnergy, int iEnergyLostPerSecond)
{
	TimeCountDownNode* renderNode = new TimeCountDownNode();
	if( renderNode)
	{				
		renderNode->autorelease();
		renderNode->init( iStageIndex, iMaximumEnergy, iEnergyLostPerSecond);
		return renderNode;
	}

	CC_SAFE_DELETE(renderNode);
	return NULL;
}

void TimeCountDownNode::init(int iStageIndex, int iMaximumEnergy, int iEnergyLostPerSecond)
{
	m_bIsStarted = false;
	m_bIsSuspendingDecreaseEnergy = false;
	m_fMaximumEnergy = iMaximumEnergy;
	m_fEnergyLostPersecond = iEnergyLostPerSecond;
	m_fCurrentEnergy = m_fMaximumEnergy /2.f;
	m_fStoredEnergyIncrement = 0;

	m_bIsInNearOutOfEnergyState = false;

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

	m_pBackground = Sprite::createWithSpriteFrameName("TimeBoard.png");
	m_pBackground->setAnchorPoint(Point(0,0));
	m_pBatchNode->addChild(m_pBackground, -1);

	m_pMirrorMidEnergyBar = Sprite::createWithSpriteFrameName("Bar_compact.png");
	m_pMirrorMidEnergyBar->setAnchorPoint(Point(0.2f, 0.5f));
	m_pMirrorMidEnergyBar->setPosition(Point(85.f, 108.f));
	m_pMirrorMidEnergyBar->setVisible(false);
	m_pBatchNode->addChild(m_pMirrorMidEnergyBar);

	//auto* pLeftBar = Sprite::createWithSpriteFrameName("Bar_lef.png");
//	pLeftBar->setAnchorPoint(Point(0, 0.5f));
	//pLeftBar->setPosition(Point(77.f, 108.f));
	//m_pBatchNode->addChild(pLeftBar,2);

	/*m_pMidEnergyBar = Scale9Sprite::createWithSpriteFrameName("Bar.png");
	m_pMidEnergyBar->setInsetLeft(16.f);
	m_pMidEnergyBar->setInsetRight(20.f);
	m_pMidEnergyBar->setContentSize(Size( fEnergyWidth, 34.f));
	m_pMidEnergyBar->setAnchorPoint(Point(0, 0.5f));
	m_pMidEnergyBar->setPosition(Point(77.f, 108.f));
	this->addChild(m_pMidEnergyBar);*/

	m_pMidEnergyBar = Scale3Sprite::create( m_pBatchNode, SpriteFrameCache::getInstance()->getSpriteFrameByName("Bar.png"),
												16.f, 20.f);
	this->addChild(m_pMidEnergyBar);
	m_pMidEnergyBar->setPosition(Point(76.5f, 89.5f));
	m_pMidEnergyBar->setContentSize(Size( fEnergyWidth, 34.f));

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

	this->setPositionY( winSize.height - m_pBackground->getContentSize().height);

	// temporary add stage index label to bar
	char sText[5];
	sprintf( sText, "%d", iStageIndex);
	auto label = LabelTTF::create(sText, "fonts/UTM Cookies.ttf", 35);
	label->setPosition( Point( 40.f, 86.f));
	label->disableStroke();		
	label->setColor(Color3B( 190.f, 190.f, 190.f));
	this->addChild(label, 2000);
}

void TimeCountDownNode::update(float fDeltaTime)
{
	if (!m_bIsStarted)
		return;

	if (m_fCurrentEnergy <=0 )
		return;

	if (m_fStoredEnergyIncrement > 0)
	{
		if (m_fIncrementTimeRemain <= 0)
			m_fIncrementTimeRemain = fDeltaTime;

		float fDeltaIncrement = fDeltaTime/m_fIncrementTimeRemain * m_fStoredEnergyIncrement;
		if (fDeltaIncrement >= m_fStoredEnergyIncrement)
			fDeltaIncrement = m_fStoredEnergyIncrement;

		m_fStoredEnergyIncrement -= fDeltaIncrement;		
		m_fIncrementTimeRemain -= fDeltaTime;

		m_fCurrentEnergy += fDeltaIncrement;
		if (m_fCurrentEnergy > m_fMaximumEnergy)
			m_fCurrentEnergy = m_fMaximumEnergy;		
	}
	else if (!m_bIsSuspendingDecreaseEnergy)
	{
		m_fCurrentEnergy -= fDeltaTime * m_fEnergyLostPersecond;
		if (m_fCurrentEnergy < 0)
			m_fCurrentEnergy = 0;
	}	


	fEnergyWidth = m_fCurrentEnergy * _contentSize.width / m_fMaximumEnergy;
	
	if (fEnergyWidth > 36.f)
	{
		m_pMidEnergyBar->setVisible(true);
		m_pMirrorMidEnergyBar->setVisible(false);
	
		m_pMidEnergyBar->setContentSize(Size( fEnergyWidth, 34.f));

		if (m_bIsInNearOutOfEnergyState)
		{
			m_bIsInNearOutOfEnergyState = false;
			m_pBackground->stopAllActions();
			m_pBackground->setColor(Color3B(255.f, 255.f, 255.f));
		}
	}
	else
	{		
		m_pMidEnergyBar->setVisible(false);
		m_pMirrorMidEnergyBar->setVisible(true);

		//m_pMirrorMidEnergyBar->setScaleX( fEnergyWidth / m_pMidEnergyBar->getContentSize().width);
		m_pMirrorMidEnergyBar->setOpacity(fEnergyWidth*255/4);
		m_pMirrorMidEnergyBar->setScale(fEnergyWidth/40.f);

		if (!m_bIsInNearOutOfEnergyState)
		{
			m_bIsInNearOutOfEnergyState = true;
			m_pBackground->runAction(
				RepeatForever::create(
				Sequence::createWithTwoActions(					
						TintTo::create( 0.3f, 255.f, 30.f, 30.f),
						TintTo::create( 0.3f, 255.f, 255.f, 255.f))));
		}
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

