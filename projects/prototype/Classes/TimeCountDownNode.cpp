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
	m_fMaximumEnergy = iMaximumEnergy;
	m_fEnergyLostPersecond = iEnergyLostPerSecond;
	m_fCurrentEnergy = m_fMaximumEnergy /2.f;

	Size winSize = Director::getInstance()->getWinSize();
	
	_contentSize.width = winSize.width - 50.f;
	_contentSize.height = 60.f;
	_position.x = 25.f; //winSize.width / 2.f;
	//anchor left

	fEnergyWidth = m_fCurrentEnergy * _contentSize.width / m_fMaximumEnergy;

	this->scheduleUpdate();
}

void TimeCountDownNode::update(float fDeltaTime)
{
	if (m_fCurrentEnergy <=0 )
		return;

	m_fCurrentEnergy -= fDeltaTime * m_fEnergyLostPersecond;
	if (m_fCurrentEnergy < 0)
		m_fCurrentEnergy = 0;
	fEnergyWidth = m_fCurrentEnergy * _contentSize.width / m_fMaximumEnergy;

	if (m_fCurrentEnergy == 0)
	{
		m_OutOfTimeCallback();
	}
}

void TimeCountDownNode::draw()
{
	Size winSize = Director::getInstance()->getWinSize();

	CHECK_GL_ERROR_DEBUG();

	glLineWidth( 12.0f );
	DrawPrimitives::setDrawColor4B(255,255,255,255);	
	DrawPrimitives::drawLine( Point( 0, 0), Point( _contentSize.width, 0));

	glLineWidth( 6.0f );
	DrawPrimitives::setDrawColor4B(0,0,0,255);
	DrawPrimitives::drawLine( Point( 0, 1.f), Point( fEnergyWidth, 1.f));

	CHECK_GL_ERROR_DEBUG();
}

