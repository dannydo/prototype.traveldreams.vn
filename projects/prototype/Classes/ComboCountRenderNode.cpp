#include "ComboCountRenderNode.h"

ComboCountRenderNode::ComboCountRenderNode()
{
	m_iMoveCount = 0; 
}

ComboCountRenderNode* ComboCountRenderNode::create()
{	
	ComboCountRenderNode* renderNode = new ComboCountRenderNode();
	if( renderNode)
	{		
		renderNode->autorelease();
		renderNode->GenerateLabels();
		return renderNode;
	}

	CC_SAFE_DELETE(renderNode);
	return NULL;
}

void ComboCountRenderNode::draw()
{
}

void ComboCountRenderNode::update(float dt)
{
}

void ComboCountRenderNode::UpdateList(int iTypeID, int iComboCount)
{
	char sText[10];
	sprintf( sText, "%d", iComboCount);

	m_LabelList[iTypeID]->setString(sText);	
}

void ComboCountRenderNode::IncreaseMove()
{
	m_iMoveCount++;

	char sText[20];
	sprintf( sText, "Move: %d", m_iMoveCount);

	m_pMoveCountLabel->setString(sText);
}

void ComboCountRenderNode::GenerateLabels()
{
	char sText[10];
	for(int i=0; i<_COMBO_TYPE_COUNT_; i++)
	{
		m_LabelList[i] = CCLabelTTF::create("", "Arial", 28);
		m_LabelList[i]->setPositionX( i * 120.f);
		m_LabelList[i]->setString("0");
		
		this->addChild(m_LabelList[i]);		
	}

	m_pMoveCountLabel = CCLabelTTF::create("", "Arial", 22);
	m_pMoveCountLabel->setPosition(Point( 100.f, 35.f));
	m_pMoveCountLabel->setString("Move count: 0");
	this->addChild(m_pMoveCountLabel);
}