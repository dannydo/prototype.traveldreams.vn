#include "ComboCountRenderNode.h"

ComboCountRenderNode::ComboCountRenderNode()
{	
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
}