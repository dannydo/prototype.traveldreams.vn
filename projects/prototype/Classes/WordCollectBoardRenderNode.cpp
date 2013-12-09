#include "WordCollectBoardRenderNode.h"
#include "GameBoardManager.h"
#include "SoundManager.h"

WordCollectBoardRenderNode* WordCollectBoardRenderNode::create()
{
	WordCollectBoardRenderNode* renderNode = new WordCollectBoardRenderNode();
	if( renderNode)
	{				
		renderNode->autorelease();
		renderNode->init();
		return renderNode;
	}

	CC_SAFE_DELETE(renderNode);
	return NULL;
}


WordCollectBoardRenderNode::WordCollectBoardRenderNode()
{
	m_iNextCharacterID = -1;	
	for (int i=0; i< _GDS_WORD_MAX_LENGTH_; i++)
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

bool WordCollectBoardRenderNode::init()
{
	LoadWords();
	GetWordIndex();

	// GenerateLabels
	CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("ResourceDemo.plist");
	m_pBatchNode = CCSpriteBatchNode::create("ResourceDemo.pvr.ccz");

	m_pBatchNode->setPosition(150.f, 820.f);
	this->addChild(m_pBatchNode);

	int iWordLength = strlen(m_WordList[m_iWordIndex].m_sWord);
	for(int i=0; i< iWordLength; i++)
	{
		m_LabelList[i] =  //CCLabelTTF::create("", "Arial", 34);
			Sprite::createWithSpriteFrameName( GetImageFileFromCharacterID(m_WordList[m_iWordIndex].m_sWord[i]).c_str());
		m_LabelList[i]->setAnchorPoint(Point(0,0));
		m_LabelList[i]->setColor( ccc3(50, 50, 50));
		//pLabel->enableShadow(Size(10.f,10.f),0.8f,0.2f);
		//m_LabelList[i]->enableStroke(ccc3(0,0,0), 2.f);		
		m_LabelList[i]->setPosition(ccp(this->_position.x + i* 50.f, _position.y));
		m_pBatchNode->addChild(m_LabelList[i]);
	}	 

	// draw meaning of word
	LabelTTF* pMeaningLabel = CCLabelTTF::create("", "Arial", 24);
	pMeaningLabel->setAnchorPoint(Point(0,0));
	pMeaningLabel->setPosition(Point( 180.f, 780.f));
	pMeaningLabel->setString(m_WordList[m_iWordIndex].m_sMeaning.c_str());
	this->addChild(pMeaningLabel);


	this->setTouchEnabled(true);	
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);	
	this->scheduleUpdate();

	return true;
}

void WordCollectBoardRenderNode::UnlockCharacter(int iCharacterID)
{
	int iWordLength = strlen(m_WordList[m_iWordIndex].m_sWord);

	if (iCharacterID >= 0 && iCharacterID < iWordLength && !m_ActivedCharacterFlags[iCharacterID])
	{
		m_ActivedCharacterFlags[iCharacterID] = true;
		m_LabelList[iCharacterID]->setColor( ccc3(255, 255, 255));

		Sprite* pSrite = Sprite::createWithSpriteFrameName( GetImageFileFromCharacterID( m_WordList[m_iWordIndex].m_sWord[iCharacterID]).c_str());
		pSrite->setColor( ccc3(100, 100, 100));
		pSrite->setPosition(ccp(this->_position.x + iCharacterID* 50.f, _position.y));
		m_pBatchNode->addChild(pSrite);

		pSrite->runAction( ScaleTo::create( 0.35f, 4.f, 4.f));
		pSrite->runAction( FadeOut::create(0.35f));

		bool bAreAllCharacterUnlocked = true;		
		for(int i=0; i< iWordLength; i++)
		{
			if (!m_ActivedCharacterFlags[i])
			{
				bAreAllCharacterUnlocked = false;
				break;
			}
		}

		if (bAreAllCharacterUnlocked)
			onTouchBegan(NULL, NULL);
	}
}

int WordCollectBoardRenderNode::GetNextCharacterID()
{
	/*for(int i=0; i< _WCBRN_WORD_LENGTH_; i++)
		if (!m_ActivedCharacterFlags[i])
			return i;
	return -1;*/
	m_iNextCharacterID++;
	int iWordLength = strlen(m_WordList[m_iWordIndex].m_sWord);
	if (m_iNextCharacterID >=0 && m_iNextCharacterID < iWordLength)
		return m_iNextCharacterID;
	else 
		return -1;
}

 std::string WordCollectBoardRenderNode::GetImageFileFromCharacterID(int iCharacterID)
 {
	 return  GetImageFileFromCharacterID(m_WordList[m_iWordIndex].m_sWord[iCharacterID]);
 }

std::string WordCollectBoardRenderNode::GetImageFileFromCharacterID(char iCharacter)
{
	char sFileName[10];
	sprintf(sFileName, "%c.png", ((unsigned char)iCharacter));
	return std::string(sFileName);
}

void WordCollectBoardRenderNode::LoadWords()
{
	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::sharedFileUtils()->getFileData("WordList.txt", "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	inputStream >> m_iWordCount;
	std::string sTemp;
	for(int iWordIndex = 0; iWordIndex < m_iWordCount; iWordIndex++)
	{
		inputStream >> m_WordList[iWordIndex].m_sWord;		
		
		// read \r to temp
		std::getline(inputStream, sTemp);		
		std::getline(inputStream, m_WordList[iWordIndex].m_sMeaning);
		//strcpy( m_WordList[iWordIndex].m_sMeaning, sTemp.c_str());

		inputStream >> m_WordList[iWordIndex].m_sSoundFile;
		inputStream >> m_WordList[iWordIndex].m_sSoundVietnameseFile;
		//std::getline(inputStream, m_WordList[iWordIndex].m_sSoundFile);
		//std::getline(inputStream, m_WordList[iWordIndex].m_sSoundVietnameseFile);
	}
}

void WordCollectBoardRenderNode::GetWordIndex()
{
	int iPreviousWordIndex = UserDefault::getInstance()->getIntegerForKey("wordIndex", -1);
	m_iWordIndex = (iPreviousWordIndex + 1) % m_iWordCount;

	UserDefault::getInstance()->setIntegerForKey("wordIndex", m_iWordIndex);
}

#include "SimpleAudioEngine.h"

bool WordCollectBoardRenderNode::onTouchBegan(Touch *pTouch, Event *pEvent)
{
	Point touchPosition;
	if (pTouch)
		touchPosition = pTouch->getLocation();
	if (pTouch == NULL || touchPosition.y > 830)
	{
		//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("pew-pew-lei.wav");
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
			m_WordList[m_iWordIndex].m_sSoundFile.c_str());

		this->runAction(
			Sequence::create( 
				DelayTime::create(3.5f),
				CallFunc::create(this, callfunc_selector(WordCollectBoardRenderNode::PlayVietnameseSpelling)),
				NULL));
		return true;
	}
	else
		return false;
}

void WordCollectBoardRenderNode::PlayVietnameseSpelling()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
		m_WordList[m_iWordIndex].m_sSoundVietnameseFile.c_str());
}