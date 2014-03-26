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
	//if (m_pCharacter->getAnimation()->getIsComplete())
		//m_pCharacter->getAnimation()->playByIndex(0);
}

bool WordCollectBoardRenderNode::init()
{	
	// init default position of letter
	m_CharacterFirstLinePositions[0] = Point( 75.f, 124.f);
	m_CharacterFirstLinePositions[1] = Point( 115.f, 113.f);
	m_CharacterFirstLinePositions[2] = Point( 156.f, 103.f);
	m_CharacterFirstLinePositions[3] = Point( 197.f, 91.f);
	m_CharacterFirstLinePositions[4] = Point( 238.f, 87.f);
	m_CharacterFirstLinePositions[5] = Point( 279.f, 82.f);
	m_CharacterFirstLinePositions[6] = Point( 320.f, 80.f);
	m_CharacterFirstLinePositions[7] = Point( 360.f, 82.f);
	m_CharacterFirstLinePositions[8] = Point( 402.f, 87.f);
	m_CharacterFirstLinePositions[9] = Point( 443.f, 91.f);
	m_CharacterFirstLinePositions[10] = Point( 484.f, 103.f);
	m_CharacterFirstLinePositions[11] = Point( 524.f, 113.f);
	m_CharacterFirstLinePositions[12] = Point( 568.f, 124.f);	

	m_CharacterSecondLinePositions[0] = Point(100.f, 150.f);
	m_CharacterSecondLinePositions[1] = Point(140.f, 142.f);
	m_CharacterSecondLinePositions[2] = Point(180.f, 133.f);
	m_CharacterSecondLinePositions[3] = Point(220.f, 128.f);
	m_CharacterSecondLinePositions[4] = Point(260.f, 124.f);
	m_CharacterSecondLinePositions[5] = Point(300.f, 120.f);
	m_CharacterSecondLinePositions[6] = Point(340.f, 120.f);
	m_CharacterSecondLinePositions[7] = Point(381.f, 124.f);
	m_CharacterSecondLinePositions[8] = Point(421.f, 128.f);
	m_CharacterSecondLinePositions[9] = Point(461.f, 133.f);
	m_CharacterSecondLinePositions[10] = Point(501.f, 142.f);
	m_CharacterSecondLinePositions[11] = Point(541.f, 150.f);	


	m_pColorNode = NULL;
		 
	// GenerateLabels
	CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("ResourceDemo.plist");
	m_pBatchNode = CCSpriteBatchNode::create("ResourceDemo.pvr.ccz");

	//m_pBatchNode->setPosition(10.f, 866.f);
	this->addChild(m_pBatchNode, 10);

	/*timeval now;
	gettimeofday( &now, NULL);
	m_iPreviousMainWordTapTime = now.tv_sec*1000 + now.tv_usec / 1000;
	m_iUnlockedLetterEffectEndTime = m_iPreviousMainWordTapTime;*/

	// preload character animation
	/*ArmatureDataManager::getInstance()->addArmatureFileInfo("CCS_Animation/PigHero/PigHero.ExportJson");

	m_pCharacter = Armature::create("PigHero");
	m_pCharacter->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);	
	m_pCharacter->getAnimation()->playByIndex(4);//0);
	m_pCharacter->setAnchorPoint(Point(0,0));
	m_pCharacter->setPosition( 5.f, 740.f);
	this->addChild(m_pCharacter);*/

	Size winSize = Director::getInstance()->getWinSize();

	// add background
	auto pBackground = Sprite::createWithSpriteFrameName("Header_BG.png");
	pBackground->setAnchorPoint( Point(0,0));
	pBackground->setPositionY( winSize.height - pBackground->getContentSize().height);
	m_pBatchNode->addChild(pBackground);
	
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
	

	//m_pLetterPanel = Sprite::createWithSpriteFrameName( GetImageFileFromLetter(' ').c_str());
	//m_pBatchNode->addChild(m_pLetterPanel, 2);

	// create labels
	int iWordLength = strlen(m_pMainWord->m_sWord);	
	int iTotalLabelsWidth = 0;

	for(int i=0; i< iWordLength; i++)
	{
		if (!m_pMainWord->m_ActivatedCharacterFlags[i])		
		{
			m_LabelList[i] =  //CCLabelTTF::create("", "Arial", 34);
				Sprite::createWithSpriteFrameName( GetImageFileFromLetter(m_pMainWord->m_sWord[i]).c_str());
		}
		else
			m_LabelList[i] =  //CCLabelTTF::create("", "Arial", 34);
			Sprite::createWithSpriteFrameName( GetImageInGemFileFromLetter(m_pMainWord->m_sWord[i]).c_str());
		//m_LabelList[i]->setAnchorPoint(Point(0,0));		

		if (!m_pMainWord->m_ActivatedCharacterFlags[i])
		{
			m_LabelList[i]->setColor( ccc3(180, 180, 180));
			m_LabelList[i]->setOpacity(110);
		}
		//pLabel->enableShadow(Size(10.f,10.f),0.8f,0.2f);
		//m_LabelList[i]->enableStroke(ccc3(0,0,0), 2.f);		

		iTotalLabelsWidth += m_LabelList[i]->getContentSize().width + 2.f;
				
		m_pBatchNode->addChild(m_LabelList[i], 2);
	}	 

	// break line based on level config
	int iLetterCountOfFirstLine;

	const LevelConfig* pLevelConfig = GameWordManager::getInstance()->GetLevelConfig();
	if (pLevelConfig->m_bBreakLineWhenDisplayMainWord) 
		iLetterCountOfFirstLine = pLevelConfig->m_iLetterCountOfFirstLine-1; //skip "SPACE"; 
	else
		iLetterCountOfFirstLine = m_pMainWord->m_iWordLength;

	// draw first line, note that if has 2 row, must skip "SPACE" at the last of first line	
	char sFirtLineLetter[_WCBRN_MAX_FIRST_LINE_LETTERS_];
	memcpy( sFirtLineLetter, m_pMainWord->m_sWord, iLetterCountOfFirstLine);
	int iIndexPositionOfFirstLine = (_WCBRN_MAX_FIRST_LINE_LETTERS_ - iLetterCountOfFirstLine + 1)/2;
	int i, iZOrder;
	for(i =0; i< iLetterCountOfFirstLine; i++)
	{
		if (!m_pMainWord->m_ActivatedCharacterFlags[i])		
		{
			m_BubbleList[i] = Sprite::createWithSpriteFrameName("Main_Bubble.png");
			iZOrder = 1;
		}
		else
		{
			m_BubbleList[i] = Sprite::createWithSpriteFrameName("Main_Bubble_destroy.png");
			m_BubbleList[i]->setRotation( rand() % 360);
			iZOrder = 0; 
		}
		m_BubbleList[i]->setPosition( Point(m_CharacterFirstLinePositions[iIndexPositionOfFirstLine+i].x, 960.f - m_CharacterFirstLinePositions[iIndexPositionOfFirstLine+i].y - 10.f));		
		m_pBatchNode->addChild(m_BubbleList[i], iZOrder);
				
		m_LabelList[i]->setPosition(Point(m_CharacterFirstLinePositions[iIndexPositionOfFirstLine+i].x, 960.f - m_CharacterFirstLinePositions[iIndexPositionOfFirstLine+i].y - 10.f));
	}	

	// draw second line
	if (pLevelConfig->m_bBreakLineWhenDisplayMainWord)
	{
		int iStartLetterIndexOfSecondLine = pLevelConfig->m_iLetterCountOfFirstLine;
		int iLetterCountOfSecondLine = m_pMainWord->m_iWordLength - pLevelConfig->m_iLetterCountOfFirstLine;
		int iIndexPositionOfSecondLine = (_WCBRN_MAX_SECOND_LINE_LETTERS_ - iLetterCountOfSecondLine + 1)/2;
		for(i = 0; i<iLetterCountOfSecondLine; i++)
		{
			if (!m_pMainWord->m_ActivatedCharacterFlags[iStartLetterIndexOfSecondLine + i])		
			{
				m_BubbleList[iStartLetterIndexOfSecondLine + i] = Sprite::createWithSpriteFrameName("Main_Bubble.png");
				iZOrder = 1;
			}
			else
			{
				m_BubbleList[iStartLetterIndexOfSecondLine + i] = Sprite::createWithSpriteFrameName("Main_Bubble_destroy.png");
				m_BubbleList[iStartLetterIndexOfSecondLine + i]->setRotation( rand() % 360);
				iZOrder = 0;
			}
			m_BubbleList[iStartLetterIndexOfSecondLine + i]->setPosition( Point(m_CharacterSecondLinePositions[iIndexPositionOfSecondLine+i].x, 960.f - m_CharacterSecondLinePositions[iIndexPositionOfSecondLine+i].y - 10.f));		
			m_pBatchNode->addChild(m_BubbleList[iStartLetterIndexOfSecondLine+i], iZOrder);
					
			m_LabelList[iStartLetterIndexOfSecondLine + i]->setPosition(Point(m_CharacterSecondLinePositions[iIndexPositionOfSecondLine+i].x, 960.f - m_CharacterSecondLinePositions[iIndexPositionOfSecondLine+i].y - 10.f));
		}
	}

	// create background
	//Sprite* pBackground;
	/*if ( iTotalLabelsWidth < 240.f)
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
		m_LabelList[i]->setPosition(ccp(m_LabelXPositionList[i], m_fStartPositionX));
	}*/
	
	float fWindowWidth = Director::getInstance()->getWinSize().width;

	// draw meaning
	LabelTTF* pMeaningLabel = CCLabelTTF::create("", "fonts/UTM Cookies.ttf", 24); //"Arial", 17);//
	pMeaningLabel->setString(m_pMainWord->m_sMeaning.c_str());		
	pMeaningLabel->setAnchorPoint(Point(0,0));
	//pMeaningLabel->setPosition(Point( 120.f, 860.f));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	pMeaningLabel->setPosition(Point( (fWindowWidth - pMeaningLabel->getContentSize().width)/2.f, 859.f));	
#else
	pMeaningLabel->setPosition(Point( (fWindowWidth - pMeaningLabel->getContentSize().width)/2.f, 892.f));
#endif
	pMeaningLabel->setColor(ccc3( 160, 160, 160));
	pMeaningLabel->disableStroke();
	this->addChild(pMeaningLabel, 20);	


	Size winSize = Director::getInstance()->getWinSize();
	float fTextWidth = pMeaningLabel->getContentSize().width;

	// draw background of meaning
	Point middlePosition( winSize.width/2.f, winSize.height - 60.f);

	// add header middle
	auto pHeaderMiddle = Sprite::createWithSpriteFrameName("Header_middle.png");
	pHeaderMiddle->setPosition(middlePosition );
	m_pBatchNode->addChild(pHeaderMiddle);
	Size headerMiddleSize = pHeaderMiddle->getContentSize();

	// draw background of meaning
	auto pHeaderLeft = Sprite::createWithSpriteFrameName("Header_left.png");
	pHeaderLeft->setAnchorPoint(Point( 1.f, 0.5f));
	pHeaderLeft->setPosition( Point( middlePosition.x - headerMiddleSize.width/2.f + 0.28f, middlePosition.y));
	m_pBatchNode->addChild(pHeaderLeft);

	auto pHeaderRight = Sprite::createWithSpriteFrameName("Header_right.png");	
	pHeaderRight->setAnchorPoint(Point( 0.f, 0.5f));
	pHeaderRight->setPosition( Point( middlePosition.x + headerMiddleSize.width/2.f - 0.28f , middlePosition.y));
	m_pBatchNode->addChild(pHeaderRight);


	// reset flags
	//memset( m_NewUnlockedLetterFlags, 0, sizeof(m_NewUnlockedLetterFlags));


	// extra tutorial
	if (strcmp( m_pMainWord->m_sWord, "PEN") == 0)
	{
		Sprite* pTutorialSprite = Sprite::create("tutorial.png");
		pTutorialSprite->setScale(1.2f);
		m_pColorNode->addChild(pTutorialSprite);
		pTutorialSprite->setPosition(Point( winSize.width/2.f, winSize.height/2.f));
	}

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
/*void WordCollectBoardRenderNode::UnlockLetter(const int& iLetterIndex)
{
	if (iLetterIndex >= 0 && iLetterIndex < m_pMainWord->m_iWordLength)
		m_NewUnlockedLetterFlags[iLetterIndex] = true;
}*/

/*float WordCollectBoardRenderNode::PlayUnlockLettersAnimation(float fDelayTime)
{
	timeval now;
	gettimeofday( &now, NULL);
	unsigned long currentMilliseconds = now.tv_sec*1000 + now.tv_usec / 1000;
	float fRemainWaitTime = 0;
	if (m_iUnlockedLetterEffectEndTime > currentMilliseconds)
		fRemainWaitTime = (m_iUnlockedLetterEffectEndTime - currentMilliseconds) / 1000.f;
	if (fDelayTime < fRemainWaitTime)
		fDelayTime = fRemainWaitTime;
	//fDelayTime += 0.6f;
	

	float fDisplayPerLetter = 0.25f;
	float fDelayPerLetter = 0.22f;
	float fTotalTime = 0.f;
	for(int iLetterIndex=0; iLetterIndex< m_pMainWord->m_iWordLength; iLetterIndex++)
		if (m_NewUnlockedLetterFlags[iLetterIndex])
		{
			Point savePos = m_BubbleList[iLetterIndex]->getPosition();
			//destroy old buble
			m_BubbleList[iLetterIndex]->runAction(
				Sequence::create(
					DelayTime::create( fDelayTime + fTotalTime),
					Spawn::createWithTwoActions(
						ScaleTo::create( fDisplayPerLetter, 2.f, 2.f),
						FadeTo::create( fDisplayPerLetter, 30)),
					RemoveSelf::create(),
					NULL));				

			m_BubbleList[iLetterIndex] = Sprite::createWithSpriteFrameName("Main_Bubble_destroy.png");
			m_BubbleList[iLetterIndex]->setPosition(savePos);
			m_BubbleList[iLetterIndex]->setRotation( rand() % 360);
			m_pBatchNode->addChild( m_BubbleList[iLetterIndex], 0);

			m_BubbleList[iLetterIndex]->setOpacity(0);
			m_BubbleList[iLetterIndex]->runAction(
				Sequence::createWithTwoActions(
					DelayTime::create( fDelayTime + fTotalTime),
					FadeTo::create(fDisplayPerLetter, 255)));

			//m_LabelList[iLetterIndex]->setColor( ccc3(255, 255, 255));
			//m_LabelList[iLetterIndex]->setOpacity(255);

			Point pos = m_LabelList[iLetterIndex]->getPosition();
			m_LabelList[iLetterIndex]->runAction( 
				Sequence::create(
					DelayTime::create( fDelayTime + fTotalTime ),
					RemoveSelf::create(),
					NULL));			
			m_LabelList[iLetterIndex] = Sprite::createWithSpriteFrameName( GetImageInGemFileFromLetter(m_pMainWord->m_sWord[iLetterIndex]).c_str());			
			m_LabelList[iLetterIndex]->setPosition(pos);
			m_LabelList[iLetterIndex]->setColor( Color3B( 100,100,100));
			m_LabelList[iLetterIndex]->setOpacity( 80);
			m_pBatchNode->addChild(m_LabelList[iLetterIndex], 2);
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
			pSrite->setPosition( pos);
			//pSrite->setPosition(ccp( m_LabelXPositionList[iLetterIndex] + pSrite->getContentSize().width/2.f, 38.f + pSrite->getContentSize().height/2.f)); //_position.y));
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

	// reset flags
	memset( m_NewUnlockedLetterFlags, 0, sizeof(m_NewUnlockedLetterFlags));

	return fTotalTime;
}

/*void WordCollectBoardRenderNode::PlayCharacterAnim(int iAnimIndex, bool bIsLoop)
{
	if (!bIsLoop)
		m_pCharacter->getAnimation()->playByIndex(iAnimIndex, -1, -1,false);
	else
		m_pCharacter->getAnimation()->playByIndex(3, -1, -1,true);
}
*/
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
		sprintf(sFileName, "space.png");
	return std::string(sFileName);
}

std::string WordCollectBoardRenderNode::GetImageInGemFileFromLetter(unsigned char iLetter)
{
	char sFileName[18];
	if (iLetter == ' ')		
		sprintf(sFileName, "space.png");
	else if (iLetter != '?')
		sprintf(sFileName, "Unlock_%c.png", ((unsigned char)iLetter));
	else
		sprintf(sFileName, "Questionmask.png");
	return std::string(sFileName);
}

std::string WordCollectBoardRenderNode::GetImageInFileFromBonusLetter(unsigned char iLetter)
{
	char sFileName[15];
	sprintf(sFileName, "Bonus_%c.png", ((unsigned char)iLetter));	
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

		m_StartGameCallback();
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
			DictionaryNode* pDictionary = DictionaryNode::createLayout(GameWordManager::getInstance()->GetMainWord().m_sWord);
			pDictionary->setPosition(Point(320.0f, 680.0f));
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


float WordCollectBoardRenderNode::PlaySpellingSound()
{
	return SoundManager::PlaySpellingOfWord(this, *m_pMainWord);
	/*
	char sSoundFile[100];

	// check play voice on setting
	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnVoice", 1) == 1)
	{
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			sprintf(sSoundFile, "EnglishSoundPC/Words/%s.wav", m_pMainWord->m_sSoundFile.c_str());
		#else
			sprintf(sSoundFile, "EnglishSound/Words/%s.ogg", m_pMainWord->m_sSoundFile.c_str());
		#endif

		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect( sSoundFile);
		//m_pMainWord->m_sSoundFile.c_str());

		this->runAction(
		Sequence::create( 
			DelayTime::create(3.5f),
			CallFunc::create(this, callfunc_selector(WordCollectBoardRenderNode::PlayVietnameseSpelling)),
			NULL));
	}*/
}

void WordCollectBoardRenderNode::PlayUnlockLetterEffect(const int& iLetterIndex, const float& fDelayEffectTime, const unsigned char& iLetter, const Point& position)
{
	if (!(iLetterIndex >= 0 && iLetterIndex < m_pMainWord->m_iWordLength))		
		return;

	float fDelayTime = fDelayEffectTime + 0.1f;
	Point destinationPosition = m_BubbleList[iLetterIndex]->getPosition();

	// bubble
	Sprite* pBubbleSprite = Sprite::createWithSpriteFrameName("bubble.png");
	pBubbleSprite->setPosition(position);
	pBubbleSprite->setScale(0.3f);
	pBubbleSprite->setOpacity(76);///30%
	pBubbleSprite->setVisible(false);
	m_pBatchNode->addChild(pBubbleSprite,11);
	pBubbleSprite->runAction(
		Sequence::create(			
			DelayTime::create(fDelayTime),
			Show::create(),
			Spawn::createWithTwoActions(
				Sequence::createWithTwoActions(
					ScaleTo::create( 0.1f, 2.5f),
					ScaleTo::create( 0.23f, 0.65f)),
				Sequence::createWithTwoActions(
					DelayTime::create( 0.1f),
					FadeTo::create( 0.23f, 255))),
			MoveTo::create( 0.25f, destinationPosition),
			DelayTime::create( 0.05f),
			Spawn::createWithTwoActions(
				ScaleTo::create( 0.3f, 3.f),
				FadeOut::create( 0.3f)),
			RemoveSelf::create(),
			NULL));	

	// letter
	Sprite* pLetterSrite = Sprite::createWithSpriteFrameName( GetImageFileFromLetter(iLetter).c_str());			
	pLetterSrite->setPosition( position);
	pLetterSrite->setVisible(false);
	m_pBatchNode->addChild(pLetterSrite,1);		
	pLetterSrite->runAction(
		Sequence::create(
			DelayTime::create(fDelayTime),
			Show::create(),
			ScaleTo::create(0.1f, 4.f),
			ScaleTo::create(0.23f, 1.),
			MoveTo::create( 0.25f, destinationPosition),
			RemoveSelf::create(),
			NULL));

	// flare
	Sprite* pFlareSprite = Sprite::createWithSpriteFrameName("LetterAppear_Flare.png");
	pFlareSprite->setPosition(position);
	pFlareSprite->setRotation( -20.f);
	pFlareSprite->setOpacity(128);///50%
	pFlareSprite->setVisible(false);
	m_pBatchNode->addChild(pFlareSprite);
	pFlareSprite->runAction(
		Sequence::create(			
			DelayTime::create(fDelayTime),
			Show::create(),
			Spawn::create(
				Sequence::createWithTwoActions(
					ScaleTo::create( 0.1f, 5.f),
					ScaleTo::create( 0.23f, 2.f)),
				Sequence::createWithTwoActions(
					RotateTo::create( 0.1f, 133.f),
					RotateTo::create( 0.23f, 270.f)),
				FadeTo::create( 0.1f, 1.f),
				NULL),				
			RemoveSelf::create(),
			NULL));	


	Point savePos = m_BubbleList[iLetterIndex]->getPosition();
	//destroy old buble
	m_BubbleList[iLetterIndex]->runAction(
		Sequence::create(
			DelayTime::create( fDelayTime + 0.58f),
			ScaleTo::create( 0.017f, 1.3f),			
			RemoveSelf::create(),
			NULL));				

	m_BubbleList[iLetterIndex] = Sprite::createWithSpriteFrameName("Main_Bubble_destroy.png");
	m_BubbleList[iLetterIndex]->setPosition(savePos);
	m_BubbleList[iLetterIndex]->setRotation( rand() % 360);
	m_pBatchNode->addChild( m_BubbleList[iLetterIndex], -1);

	m_BubbleList[iLetterIndex]->setVisible(false);
	m_BubbleList[iLetterIndex]->runAction(
		Sequence::create(
			DelayTime::create(fDelayTime + 0.63f),
			Show::create(),
			DelayTime::create(0.0167f),
			ScaleTo::create( 0.05f, 1.3f),
			NULL));


	// new letter
	Point pos = m_LabelList[iLetterIndex]->getPosition();
	Sprite* pOldSrite = m_LabelList[iLetterIndex];
	pOldSrite->runAction( 
		Sequence::createWithTwoActions(
			DelayTime::create( fDelayTime + 0.58f),
			RemoveSelf::create()));
			

	Sprite* pNewSrite = Sprite::createWithSpriteFrameName( GetImageInGemFileFromLetter(m_pMainWord->m_sWord[iLetterIndex]).c_str());						
	pNewSrite->setVisible(false);
	pNewSrite->setPosition( pos);	
	m_pBatchNode->addChild(pNewSrite, 2);
	pNewSrite->runAction( 
		Sequence::createWithTwoActions(
			DelayTime::create( fDelayTime + 0.58f),			
			Show::create()));
			
	m_LabelList[iLetterIndex] = pNewSrite;

	// compute end time of effect to correct delay effect (still need this???)
	//timeval now;
	//gettimeofday( &now, NULL);
	//m_iUnlockedLetterEffectEndTime = now.tv_sec*1000 + now.tv_usec / 1000 + (fDelayEffectTime + 0.22f + 0.17f + 1.f) * 1000;	
}

void WordCollectBoardRenderNode::PlayUnlockWordEffect(const float& fMoveTime, const float& fDisplayTime)
{
	/*m_pLetterPanel->runAction(
			Sequence::create(
			//Spawn::createWithTwoActions(
					MoveBy::create( fMoveTime, Point( 0, -250.f)),
					//ScaleTo::create(fMoveTime, 1.6f)),
				DelayTime::create(fDisplayTime),
				FadeOut::create(0.5f),
				NULL));
				*/
	for(int i=0; i< m_pMainWord->m_iWordLength; i++)
	{
		m_LabelList[i]->runAction(
			Sequence::create(
				MoveTo::create( fMoveTime, Point( 0, -400.f)),
				DelayTime::create(fDisplayTime),
				FadeOut::create(0.5f),
				NULL));

	}
}