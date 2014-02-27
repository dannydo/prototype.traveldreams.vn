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
		//renderNode->GenerateLabels();
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

void ComboCountRenderNode::UpdateGemList(int iGemID, int iCount)
{
	char sText[10];
	sprintf( sText, "%d", iCount);

	m_LabelGemCountList[iGemID]->setString(sText);	
}

void ComboCountRenderNode::UpdateComboList(int iTypeID, int iComboCount)
{
	char sText[10];
	sprintf( sText, "%d", iComboCount);

	m_LabelComboCountList[iTypeID]->setString(sText);	
}

void ComboCountRenderNode::CompleteQuest(int iType)
{
	m_QuestLabelList[iType]->setString("Complete!");	
}

void ComboCountRenderNode::UpdateBonusWordQuest(int iRemainLetterCount)
{
	char sText[10];
	sprintf( sText, "%d/%d", m_iTotalBonusLetterLength-iRemainLetterCount, m_iTotalBonusLetterLength);

	m_LabelBonusWord->setString(sText);	
}

void ComboCountRenderNode::GenerateLabels(int iType, int iValue)
{
	char sText[10];

	if (iType == 0)
	{
		// gem count
		m_QuestLabelList[iType]= CCLabelTTF::create("Destroy gem:", "Arial", 22);
		m_QuestLabelList[iType]->setPosition(Point( 10, 40));
		this->addChild(m_QuestLabelList[iType]);

	
		for(int i=0; i<_MAX_GEM_ID_; i++)
		{
			m_LabelGemCountList[i] = CCLabelTTF::create("", "Arial", 22);
			m_LabelGemCountList[i]->setPositionX( i * 70.f + 100);
			m_LabelGemCountList[i]->setPositionY(40);
			m_LabelGemCountList[i]->setString("0");
		
			this->addChild(m_LabelGemCountList[i]);		
		}
	} else if (iType == 1)
	{
		// combo count
		m_QuestLabelList[iType]= CCLabelTTF::create("Destroy combo:", "Arial", 22);
		m_QuestLabelList[iType]->setPosition(Point( 10, 10));
		this->addChild(m_QuestLabelList[iType]);
	
		for(int i=0; i<_COMBO_TYPE_COUNT_; i++)
		{
			m_LabelComboCountList[i] = CCLabelTTF::create("", "Arial", 22);
			m_LabelComboCountList[i]->setPositionX( i * 70.f + 100);
			m_LabelComboCountList[i]->setPositionY( 10);
			m_LabelComboCountList[i]->setString("0");
		
			this->addChild(m_LabelComboCountList[i]);		
		}
	}
	else if (iType == 2)
	{
		m_iTotalBonusLetterLength = iValue;
		m_QuestLabelList[iType]= CCLabelTTF::create("Collect bonus word:", "Arial", 22);
		m_QuestLabelList[iType]->setPosition(Point( 10, -20));
		this->addChild(m_QuestLabelList[iType]);

		char sText[10];
		sprintf( sText, "0/%d", iValue);
		m_LabelBonusWord = CCLabelTTF::create(sText, "Arial", 22);
		m_LabelBonusWord->setPositionX( 140);
		m_LabelBonusWord->setPositionY( -20);		
		this->addChild(m_LabelBonusWord);
	}
}