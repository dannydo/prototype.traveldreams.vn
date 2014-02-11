#include "WordCollectBoardRenderNode.h"
#include "GameBoardManager.h"
#include "SoundManager.h"
#include "GameWordManager.h"
#include "DictionaryNode.h"

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
	//m_iNextCharacterID = -1;	
	//for (int i=0; i< _GDS_WORD_MAX_LENGTH_; i++)
		//m_ActivedCharacterFlags[i] = false;
}

/*void WordCollectBoardRenderNode::draw()
{
}*/

/*
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
	}
}*/

void WordCollectBoardRenderNode::update(float dt)
{
	if (m_pCharacter->getAnimation()->getIsComplete())
		m_pCharacter->getAnimation()->playByIndex(0);
}

bool WordCollectBoardRenderNode::init()
{	
	m_pColorNode = NULL;
		 
	// GenerateLabels
	CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("ResourceDemo.plist");
	m_pBatchNode = CCSpriteBatchNode::create("ResourceDemo.pvr.ccz");

	m_pBatchNode->setPosition(10.f, 866.f);
	this->addChild(m_pBatchNode, 10);

	timeval now;
	gettimeofday( &now, NULL);
	m_iPreviousMainWordTapTime = now.tv_sec*1000 + now.tv_usec / 1000;
	m_iUnlockedLetterEffectEndTime = m_iPreviousMainWordTapTime;

	// preload character animation
	ArmatureDataManager::getInstance()->addArmatureFileInfo("CCS_Animation/PigHero/PigHero.ExportJson");

	m_pCharacter = Armature::create("PigHero");
	m_pCharacter->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);	
	m_pCharacter->getAnimation()->playByIndex(4);//0);
	m_pCharacter->setAnchorPoint(Point(0,0));
	m_pCharacter->setPosition( 5.f, 740.f);
	this->addChild(m_pCharacter);

	this->setTouchEnabled(true);	
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);	
	this->scheduleUpdate();

	return true;
}

void WordCollectBoardRenderNode::GenerateLabels()
{
	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	m_pMainWord = &pGameWordManager->GetMainWord();

	
	
	m_pColorNode = DrawNode::create();
	CCPoint vertex[] = { ccp(0,0), ccp(1000,0), ccp(1000,1000), ccp(0,1000) };
	m_pColorNode->drawPolygon(vertex, 4, ccc4f(0, 0, 0, 0.5f), 0, ccc4f(0, 0, 0, 0.5f) );
	this->addChild(m_pColorNode, 9);

	Sprite* pTip = Sprite::create("tip-msg.png");
	pTip->setScale(1.2f);
	pTip->setPosition(Point( 310.f, 790.f));
	m_pColorNode->addChild(pTip);



	// create labels
	int iWordLength = strlen(m_pMainWord->m_sWord);	
	int iTotalLabelsWidth = 0;

	for(int i=0; i< iWordLength; i++)
	{
		m_LabelList[i] =  //CCLabelTTF::create("", "Arial", 34);
			Sprite::createWithSpriteFrameName( GetImageFileFromLetter(m_pMainWord->m_sWord[i]).c_str());
		m_LabelList[i]->setAnchorPoint(Point(0,0));		

		if (!m_pMainWord->m_ActivatedCharacterFlags[i])
		{
			m_LabelList[i]->setColor( ccc3(180, 180, 180));
			m_LabelList[i]->setOpacity(80);
		}
		//pLabel->enableShadow(Size(10.f,10.f),0.8f,0.2f);
		//m_LabelList[i]->enableStroke(ccc3(0,0,0), 2.f);		

		iTotalLabelsWidth += m_LabelList[i]->getContentSize().width + 2.f;
				
		m_pBatchNode->addChild(m_LabelList[i], 2);
	}	 

	// create background
	//Sprite* pBackground;
	if ( iTotalLabelsWidth < 240.f)
		m_pBackground = Sprite::createWithSpriteFrameName("Main_Board_Small.png");
	else if ( iTotalLabelsWidth < 410.f)
		m_pBackground = Sprite::createWithSpriteFrameName("Main_Board_Medium.png");
	else
		m_pBackground = Sprite::createWithSpriteFrameName("Main_Board_Large.png");

	m_pBackground->setAnchorPoint( Point(0,0));
	m_pBackground->setPosition( Point( 0, 0));
	m_pBatchNode->addChild(m_pBackground);

	float fBackgroundWidth = m_pBackground->getContentSize().width;
	m_fStartPositionX = (fBackgroundWidth - iTotalLabelsWidth) / 2.f;
	
	for(int i=0; i< iWordLength; i++)
	{
		if (i==0)
			m_LabelXPositionList[0] = m_fStartPositionX;
		else
			m_LabelXPositionList[i] = m_LabelXPositionList[i-1] + m_LabelList[i-1]->getContentSize().width + 1.f;		
		m_LabelList[i]->setPosition(ccp(m_LabelXPositionList[i], 38.f));
	}

	// draw meaning of word
	LabelTTF* pMeaningLabel = CCLabelTTF::create("", "HelveticaNeueLTCom-Ex.ttf", 19); //"Arial", 17);//
	pMeaningLabel->setString(m_pMainWord->m_sMeaning.c_str());
	pMeaningLabel->setAnchorPoint(Point(0,0));
	//pMeaningLabel->setPosition(Point( 120.f, 860.f));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	pMeaningLabel->setPosition(Point( (fBackgroundWidth- pMeaningLabel->getContentSize().width)/2.f, 860.f));	
#else
	pMeaningLabel->setPosition(Point( (fBackgroundWidth- pMeaningLabel->getContentSize().width)/2.f, 878.f));
#endif
	pMeaningLabel->setColor(ccc3( 90, 90, 90));
	pMeaningLabel->disableStroke();
	this->addChild(pMeaningLabel, 20);	

	// reset flags
	memset( m_NewUnlockedLetterFlags, 0, sizeof(m_NewUnlockedLetterFlags));
}

/*void WordCollectBoardRenderNode::UnlockCharacter(const float& fDelayTime, const int& iLetterIndex)
{	
	m_LabelList[iLetterIndex]->setColor( ccc3(255, 255, 255));
	m_LabelList[iLetterIndex]->setOpacity(255);

	Sprite* pSrite = Sprite::createWithSpriteFrameName( GetImageFileFromLetter(m_pMainWord->m_sWord[iLetterIndex]).c_str());
	pSrite->setColor( ccc3(100, 100, 100));
	pSrite->setPosition(ccp( m_LabelXPositionList[iLetterIndex], 20.f)); //_position.y));
	m_pBatchNode->addChild(pSrite);
		

	pSrite->runAction( 
		Sequence::createWithTwoActions(
			DelayTime::create( fDelayTime),
			ScaleTo::create( 0.35f, 4.f, 4.f)));
	pSrite->runAction( 
		Sequence::createWithTwoActions(
			DelayTime::create( fDelayTime),
			FadeOut::create(0.35f)));
	
	bool bAreAllCharacterUnlocked = true;		
	for(int i=0; i< m_pMainWord->m_iWordLength; i++)
	{
		if (!m_pMainWord->m_ActivatedCharacterFlags[i])
		{
			bAreAllCharacterUnlocked = false;
			break;
		}
	}	

	PlayCharacterAnim(2, bAreAllCharacterUnlocked);
}
*/
void WordCollectBoardRenderNode::UnlockLetter(const int& iLetterIndex)
{
	if (iLetterIndex >= 0 && iLetterIndex < m_pMainWord->m_iWordLength)
		m_NewUnlockedLetterFlags[iLetterIndex] = true;
}

float WordCollectBoardRenderNode::PlayUnlockLettersAnimation(float fDelayTime)
{
	timeval now;
	gettimeofday( &now, NULL);
	unsigned long currentMilliseconds = now.tv_sec*1000 + now.tv_usec / 1000;
	float fRemainWaitTime = 0;
	if (m_iUnlockedLetterEffectEndTime > currentMilliseconds)
		fRemainWaitTime = (m_iUnlockedLetterEffectEndTime - currentMilliseconds) / 1000.f;
	if (fDelayTime < fRemainWaitTime)
		fDelayTime = fRemainWaitTime;
	fDelayTime += 0.8f;
	

	float fDisplayPerLetter = 0.25f;
	float fDelayPerLetter = 0.22f;
	float fTotalTime = 0.f;
	for(int iLetterIndex=0; iLetterIndex< m_pMainWord->m_iWordLength; iLetterIndex++)
		if (m_NewUnlockedLetterFlags[iLetterIndex])
		{
			//m_LabelList[iLetterIndex]->setColor( ccc3(255, 255, 255));
			//m_LabelList[iLetterIndex]->setOpacity(255);

			m_LabelList[iLetterIndex]->runAction(
				Sequence::createWithTwoActions(
					DelayTime::create( fDelayTime + fTotalTime ),
					TintTo::create(fDisplayPerLetter, 255,255,255)));
			m_LabelList[iLetterIndex]->runAction(
				Sequence::createWithTwoActions(
					DelayTime::create( fDelayTime + fTotalTime),
					FadeTo::create(fDisplayPerLetter, 255)));

			Sprite* pSrite = Sprite::createWithSpriteFrameName( GetImageFileFromLetter(m_pMainWord->m_sWord[iLetterIndex]).c_str());			
			//pSrite->setColor( ccc3(100, 100, 100));
			pSrite->setOpacity(0.f);
			pSrite->setPosition(ccp( m_LabelXPositionList[iLetterIndex] + pSrite->getContentSize().width/2.f, 38.f + pSrite->getContentSize().height/2.f)); //_position.y));
			m_pBatchNode->addChild(pSrite);
		

			pSrite->runAction( 
				Sequence::createWithTwoActions(
					DelayTime::create( fDelayTime + fTotalTime),
					ScaleTo::create( fDisplayPerLetter, 2.5f, 2.5f)));
			pSrite->runAction( 
				Sequence::create(
					DelayTime::create( fDelayTime + fTotalTime),
					FadeTo::create( 0.001f, 255),
					FadeOut::create( fDisplayPerLetter),
					NULL));
				
			fTotalTime += fDelayPerLetter;
		}	

	if (fTotalTime > 0)
	{
		m_pBackground->runAction(
			Sequence::create(
				DelayTime::create( fDelayTime - 0.37f),
				ScaleTo::create( 0.08f, 1.1f, 1.1f),
				ScaleTo::create( 0.08f, 1.f, 1.f),
				NULL));

		fTotalTime  += fDisplayPerLetter *2 + fDelayTime;
	}

	// reset flags
	memset( m_NewUnlockedLetterFlags, 0, sizeof(m_NewUnlockedLetterFlags));

	return fTotalTime;
}

void WordCollectBoardRenderNode::PlayCharacterAnim(int iAnimIndex, bool bIsLoop)
{
	if (!bIsLoop)
		m_pCharacter->getAnimation()->playByIndex(iAnimIndex, -1, -1,false);
	else
		m_pCharacter->getAnimation()->playByIndex(3, -1, -1,true);
}

/*int WordCollectBoardRenderNode::GetNextCharacterID()
{	
	m_iNextCharacterID++;
	int iWordLength = strlen(m_WordList[m_iWordIndex].m_sWord);
	if (m_iNextCharacterID >=0 && m_iNextCharacterID < iWordLength)
		return m_iNextCharacterID;
	else 
		return -1;
}

 std::string WordCollectBoardRenderNode::GetImageFileFromCharacterID(int iCharacterID)
 {
	 return  GetImageFileFromCharacterID(m_pMainWord->m_sWord[iCharacterID]);
 }*/

std::string WordCollectBoardRenderNode::GetImageFileFromLetter(unsigned char iLetter)
{	
	char sFileName[10];
	if (iLetter != ' ')
		sprintf(sFileName, "%c.png", iLetter);
	else
		sprintf(sFileName, "_.png", iLetter);
	return std::string(sFileName);
}

std::string WordCollectBoardRenderNode::GetImageInGemFileFromLetter(unsigned char iLetter)
{
	char sFileName[10];
	sprintf(sFileName, "Gem_%c.png", ((unsigned char)iLetter));
	return std::string(sFileName);
}

/*void WordCollectBoardRenderNode::LoadWords()
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
*/
#include "SimpleAudioEngine.h"

bool WordCollectBoardRenderNode::onTouchBegan(Touch *pTouch, Event *pEvent)
{
	if (m_pColorNode != NULL)
	{
		m_pColorNode->runAction(RemoveSelf::create());
		m_pColorNode = NULL;
	}

	Point touchPosition;
	if (pTouch)
		touchPosition = pTouch->getLocation();
	if (pTouch == NULL || touchPosition.y > 830)
	{
		timeval now;
		gettimeofday(&now, NULL);
		unsigned long iCurrentTime = now.tv_sec * 1000 + now.tv_usec/1000 ; //miliseconds
		if (iCurrentTime - m_iPreviousMainWordTapTime < 400)
		{
			Size winSize = Director::getInstance()->getWinSize();

			// show dictionary
			DictionaryNode* pDictionary = DictionaryNode::create();
			this->getParent()->addChild(pDictionary, 20);			
		}
		else
		{
			//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("pew-pew-lei.wav");
			PlaySpellingSound();

			m_iPreviousMainWordTapTime = iCurrentTime;
		}		

		return true;
	}
	else
		return false;
}

void WordCollectBoardRenderNode::PlaySpellingSound()
{
	char sSoundFile[40];
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	sprintf(sSoundFile, "EnglishSoundPC/%s.wav", m_pMainWord->m_sSoundFile.c_str());
#else
	sprintf(sSoundFile, "EnglishSound/%s.ogg", m_pMainWord->m_sSoundFile.c_str());
#endif

	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect( sSoundFile);
		//m_pMainWord->m_sSoundFile.c_str());

	/*this->runAction(
		Sequence::create( 
			DelayTime::create(3.5f),
			CallFunc::create(this, callfunc_selector(WordCollectBoardRenderNode::PlayVietnameseSpelling)),
			NULL));*/

}


/*void WordCollectBoardRenderNode::PlayVietnameseSpelling()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
		m_pMainWord->m_sSoundVietnameseFile.c_str());
}*/

void WordCollectBoardRenderNode::PlayUnlockLetterEffect(const float& fDelayEffectTime, const unsigned char& iLetter, const Point& position)
{
	Sprite* pSrite = Sprite::createWithSpriteFrameName( GetImageFileFromLetter(iLetter).c_str());
	//pSrite->setColor( ccc3(100, 100, 100));
	pSrite->setScale(0.f);
	pSrite->setPosition( Point(position.x - 10.f, position.y-866.f) ); //_position.y));
	m_pBatchNode->addChild(pSrite);
		
	pSrite->runAction(
		Sequence::create(
			DelayTime::create(fDelayEffectTime),
			ScaleTo::create(0.1f, 4.6f, 4.6f),
			ScaleTo::create(0.2f, 1.f, 0.8f),
			ScaleTo::create(0.14f, 1.f, 0.8f),
			//EaseBackIn::create( 	ScaleTo::create(0.3f, 2.f, 2.f)),
			MoveTo::create(	1.0f, Point(70.f, -73.f)),
			RemoveSelf::create(),
			NULL));


	timeval now;
	gettimeofday( &now, NULL);
	m_iUnlockedLetterEffectEndTime = now.tv_sec*1000 + now.tv_usec / 1000 + (fDelayEffectTime + 0.22f + 0.17f + 1.f) * 1000;	
}