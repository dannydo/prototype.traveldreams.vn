#include "WordCollectBoardRenderNode.h"


WordCollectBoardRenderNode* WordCollectBoardRenderNode::create()
{
	WordCollectBoardRenderNode* renderNode = new WordCollectBoardRenderNode();
	if( renderNode)
	{		
		renderNode->autorelease();
		renderNode->GenerateLabels();
		return renderNode;
	}

	CC_SAFE_DELETE(renderNode);
	return NULL;
}


WordCollectBoardRenderNode::WordCollectBoardRenderNode()
{
	m_iNextCharacterID = -1;	
	for (int i=0; i< _WCBRN_WORD_LENGTH_; i++)
		m_ActivedCharacterFlags[i] = false;
}

void WordCollectBoardRenderNode::draw()
{
}

void WordCollectBoardRenderNode::update(float dt)
{
}

void WordCollectBoardRenderNode::UpdateList()
{
	/*
	if (m_LabelList.size() < m_pCountDownCellList->size())
	{
		for (int i=m_LabelList.size(); i<m_pCountDownCellList->size(); i++)
		{
			CCLabelTTF* pLabel = CCLabelTTF::create("", "Arial", 34);
			pLabel->setColor( ccc3(240, 30, 50));
			//pLabel->enableShadow(Size(10.f,10.f),0.8f,0.2f);
			pLabel->enableStroke(ccc3(0,0,0), 2.f);
			CCPoint position = m_GetCenterOfCellDelegate(m_pCountDownCellList->at(i));
			pLabel->setPosition(ccp(position.x - 20.f, position.y - 20.f));
			this->addChild(pLabel);
			m_LabelList.push_back(pLabel);
		}
	}

	char sText[10];
	for(int i=0; i<m_pCountDownCellList->size(); i++)
	{
		if(m_pCountDownCellList->at(i).m_iCurrentCountDown == 0)
		{
			m_LabelList.at(i)->setString("");
			continue;
		}
		CCPoint position = m_GetCenterOfCellDelegate(m_pCountDownCellList->at(i));
		m_LabelList.at(i)->setPosition(ccp(position.x - 20.f, position.y - 20.f));

		//itoa(m_pCountDownCellList->at(i).m_iCurrentCountDown, sText, 10);
		sprintf( sText, "%d", m_pCountDownCellList->at(i).m_iCurrentCountDown);
		m_LabelList.at(i)->setString(sText);

		m_LabelList.at(i)->setScale(4);
		m_LabelList.at(i)->runAction(
			CCScaleTo::create( 0.5f, 1.f, 1.f));
	}

	if (m_LabelList.size() > m_pCountDownCellList->size())
	{
		for (int i=m_pCountDownCellList->size(); i<m_LabelList.size(); i++)
		{
			m_LabelList.at(i)->setString("");
		}
	}*/
}

void WordCollectBoardRenderNode::GenerateLabels()
{
	CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("ResourceDemo.plist");
	m_pBatchNode = CCSpriteBatchNode::create("ResourceDemo.pvr.ccz");

	m_pBatchNode->setPosition(50.f, 600.f);
	this->addChild(m_pBatchNode);

	for(int i=0; i< _WCBRN_WORD_LENGTH_; i++)
	{
		m_LabelList[i] =  //CCLabelTTF::create("", "Arial", 34);
			Sprite::createWithSpriteFrameName( GetImageFileFromCharacterID(i).c_str());
		m_LabelList[i]->setColor( ccc3(50, 50, 50));
		//pLabel->enableShadow(Size(10.f,10.f),0.8f,0.2f);
		//m_LabelList[i]->enableStroke(ccc3(0,0,0), 2.f);		
		m_LabelList[i]->setPosition(ccp(this->_position.x + i* 50.f, _position.y));
		m_pBatchNode->addChild(m_LabelList[i]);
	}	 
}

void WordCollectBoardRenderNode::UnlockCharacter(int iCharacterID)
{
	if (iCharacterID >= 0 && iCharacterID < _WCBRN_WORD_LENGTH_ && !m_ActivedCharacterFlags[iCharacterID])
	{
		m_ActivedCharacterFlags[iCharacterID] = true;
		m_LabelList[iCharacterID]->setColor( ccc3(255, 255, 255));

		Sprite* pSrite = Sprite::createWithSpriteFrameName( GetImageFileFromCharacterID(iCharacterID).c_str());
		pSrite->setColor( ccc3(100, 100, 100));
		pSrite->setPosition(ccp(this->_position.x + iCharacterID* 50.f, _position.y));
		m_pBatchNode->addChild(pSrite);

		pSrite->runAction( ScaleTo::create( 0.35f, 4.f, 4.f));
		pSrite->runAction( FadeOut::create(0.35f));
	}
}

int WordCollectBoardRenderNode::GetNextCharacterID()
{
	/*for(int i=0; i< _WCBRN_WORD_LENGTH_; i++)
		if (!m_ActivedCharacterFlags[i])
			return i;
	return -1;*/
	m_iNextCharacterID++;
	if (m_iNextCharacterID >=0 && m_iNextCharacterID < _WCBRN_WORD_LENGTH_)
		return m_iNextCharacterID;
	else 
		return -1;
}

std::string WordCollectBoardRenderNode::GetImageFileFromCharacterID(int iCharacterID)
{
	switch(iCharacterID)
	{
		case 0:
			return "A.png";
		case 1:
			return "P.png";
		case 2:
			return "P.png";
		case 3:
			return "L.png";
		case 4:
			return "E.png";
	}
}