#include "HelloWorldScene.h"
#include "SoundManager.h"
#include "EndGameNode.h"
#include "LevelMapScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene(int iLevel)
{	
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
	auto backgroundLayer = LayerColor::create(ccc4(10, 10, 10, 255)); //64, 88, 119, 255));
	Sprite* pBackground = Sprite::create("Background.png");
	pBackground->setAnchorPoint(ccp(0,0));
	backgroundLayer->addChild(pBackground);

	scene->addChild(backgroundLayer);

    // 'layer' is an autorelease object
    auto boardLayer = HelloWorld::create();	
	boardLayer->m_pHUDLayer = Layer::create();

    // add layer as a child to scene
    scene->addChild(boardLayer);

	//
	boardLayer->m_pWordCollectBoardRenderNode = WordCollectBoardRenderNode::create();		

	// extra layer for score/stars...	
	boardLayer->m_pStatusLayer = StatusLayer::create();
	boardLayer->m_pStatusLayer->setScale(0.88f);
	boardLayer->m_pStatusLayer->setCurrentScore(0);
	boardLayer->m_pStatusLayer->setCurrentMove(0);
	boardLayer->m_pStatusLayer->setPosition(420.f, 695.f);
	boardLayer->m_pStatusLayer->setSpeedUpdateScore(40.f);
	scene->addChild(boardLayer->m_pStatusLayer);

	// init level
	boardLayer->initLevel(iLevel);


	// menu layer with close item	
	auto closeItem = MenuItemImage::create(
                                           "Setting.png", //"CloseNormal.png",
                                           "Setting.png",//"CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, boardLayer));
    
	closeItem->setAnchorPoint(ccp(0,0));
	closeItem->setPosition(ccp(-12, -8));
	//closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
      //                          origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    boardLayer->m_pHUDLayer->addChild(menu, 10);
	//menuLayer->setTouchEnabled(true);
	scene->addChild(boardLayer->m_pHUDLayer);


	// combo count render node
	ComboCountRenderNode* pComboCountRenderNode = ComboCountRenderNode::create();
	pComboCountRenderNode->setPosition(Point(120.f, 30.f));
	boardLayer->m_pHUDLayer->addChild(pComboCountRenderNode);

	boardLayer->m_pComboCountRenderNode = pComboCountRenderNode;	

	// bonus word node
	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	std::vector<Word> subWordList;
	for(int i=0; i< pGameWordManager->GetSubWordCount(); i++)
		subWordList.push_back( pGameWordManager->GetSubWord(i));
	boardLayer->m_pBonusWordNode = BonusWordNodeNew::createLayout(subWordList);
	boardLayer->m_pBonusWordNode->setPosition( 243.f, 756.f);
	boardLayer->m_pHUDLayer->addChild(boardLayer->m_pBonusWordNode, 10);

	/*Node* btnShowPopup = boardLayer->m_pBonusWordNode->createButtonShowPopupBonus();
	btnShowPopup->setPosition(-80, 300);
	boardLayer->m_pHUDLayer->addChild(btnShowPopup);*/

	//
	boardLayer->m_pHUDLayer->addChild(boardLayer->m_pWordCollectBoardRenderNode, 11);

    // return the scene
    return scene;
}

HelloWorld::~HelloWorld()
{
	SoundManager::StopBackgroundMusic();

	if (m_pTempSpriteForAction)
	{
		m_pTempSpriteForAction->release();
		m_pTempSpriteForAction = NULL;
	}

	if (m_pSaveTouch != NULL)
	{
		m_pSaveTouch->release();
		m_pSaveTouch =  NULL;
	}
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    /*auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setAnchorPoint(ccp(0,0));
	closeItem->setPosition(ccp(100,100));
	//closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
      //                          origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 10);*/

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    /*auto label = LabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    label->setPosition(Point(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);*/	
    
	m_fBoardLeftPosition = 6.f + 38.f;
	m_fBoardBottomPosition = 105.f + 40.f;
	m_fBoardLeftClipPosition = 6.f;
	m_fBoardBottomClipPosition = 105;	

	m_bIsEffectPlaying = false;
	m_bIsCellDragPlaying = false;
	m_pSaveTouch = NULL;
	m_bIsEndGamePhase = false;

	this->setTouchEnabled(true);	
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	this->scheduleUpdate();

    return true;
}

void HelloWorld::initLevel(int iLevel)
{	
	/*switch( iLevel)
	{
		case 1:
			iNumberOfRow = 7;
			iNumberOfColumn = 7;
			break;
		case 2:
			iNumberOfRow = 7;
			iNumberOfColumn = 8;
			break;
		case 3:
			iNumberOfRow = 7;
			iNumberOfColumn = 8;
			break;
		case 4:
			iNumberOfRow = 8;
			iNumberOfColumn = 9;
			break;
	};*/
	m_GameBoardManager.GenerateGameBoard(iLevel);
	int iNumberOfRow = m_GameBoardManager.GetRowNumber();
	int iNumberOfColumn = m_GameBoardManager.GetColumnNumber();

	// init board position and mask based on its size
	switch( iNumberOfRow)
	{
		case 8:
		default:
			m_fBoardBottomPosition = 102.f + 37.f;	
			m_fBoardBottomClipPosition = 102.f;	
			m_fMaskHeight = 518.f;
			break;
		case 7:
			m_fBoardBottomPosition = 135.f + 40.f;
			m_fBoardBottomClipPosition = 135.f;	
			m_fMaskHeight = 413.f;
			break;
		case 6:
			m_fBoardBottomPosition = 165.f + 40.f;
			m_fBoardBottomClipPosition = 165.f;	
			m_fMaskHeight = 303.f;
			break;
		case 5:
			m_fBoardBottomPosition = 195.f + 40.f;
			m_fBoardBottomClipPosition = 195.f;	
			m_fMaskHeight = 199.f;
			break;
	};

	switch( iNumberOfColumn)
	{
		case 8:
		default:
			m_fBoardLeftPosition = 6.f + 38.f;			
			m_fBoardLeftClipPosition = 6.f;
			m_fMaskWidth = 618.f;
			break;
		case 7:		
			m_fBoardLeftPosition = 46.f + 38.f;			
			m_fBoardLeftClipPosition = 46.f;
			m_fMaskWidth = 506.f;
			break;
		case 6:		
			m_fBoardLeftPosition = 86.f + 38.f;			
			m_fBoardLeftClipPosition = 86.f;
			m_fMaskWidth = 386.f;
			break;
		case 5:		
			m_fBoardLeftPosition = 126.f + 38.f;			
			m_fBoardLeftClipPosition = 126.f;
			m_fMaskWidth = 272.f;
			break;
	};

	m_eTouchMoveState = _TMS_NONE_;	

	// init data for status layer
	m_pStatusLayer->setCurrentScore( m_GameBoardManager.GetCurrentScore());	
	m_pStatusLayer->setCurrentMove(m_GameBoardManager.GetCurrentMove());
	
	const LevelConfig& levelConfig = m_GameBoardManager.GetLevelConfig();
	m_pStatusLayer->setScoreForStar( levelConfig.m_ScoreOfStars[0], levelConfig.m_ScoreOfStars[1], levelConfig.m_ScoreOfStars[2], levelConfig.m_ScoreOfStars[2]* 1.1f);
	//m_pStatusLayer->update(0);

	// init word-collect board
	m_pWordCollectBoardRenderNode->GenerateLabels();

	// init graphic for gameBoard
	CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("ResourceDemo.plist");
	m_pBoardBatchNode = CCSpriteBatchNode::create("ResourceDemo.pvr.ccz");
	//m_pBoardBatchNode->setBlendFunc( BlendFunc::ALPHA_PREMULTIPLIED);
	this->addChild(m_pBoardBatchNode);

	// move hint
	m_pMoveHintNode = Sprite::createWithSpriteFrameName("Gem_A.png");
	m_pMoveHintNode->setOpacity(0);
	m_pBoardBatchNode->addChild(m_pMoveHintNode);

	// init batch node for combo effect (used later)
	CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("ComboEffect/combo.plist");
	m_pComboEffectBatchNode = CCSpriteBatchNode::create("ComboEffect/combo.pvr.ccz");
	this->m_pHUDLayer->addChild(m_pComboEffectBatchNode, 20);

	// cache anim
	auto animCache = AnimationCache::getInstance();    
    animCache->addAnimationsWithFile("ComboEffect/combo4Animations.plist");
	animCache->addAnimationsWithFile("ComboEffect/combo5Animations.plist");

	ArmatureDataManager::getInstance()->addArmatureFileInfo("CCS_Animation/AnimationDetach/Animation detach.ExportJson");

	// get symbol size
	CCSprite* pSprite;
	pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(0).c_str());
	m_SymbolSize = Point(78.f, 78.f); //pSprite->getContentSize() * 0.5;

	int iRow, iColumn, iFlag, iObstacleTypeID;
	int iObstacleTypeCount = GameConfigManager::getInstance()->GetObstacleTypeCount();
	ObstacleProcessManager* pObstacleProcessManager = m_GameBoardManager.GetObstacleProcessManager();

	memset( m_BoardObstaclesList, 0, sizeof(m_BoardObstaclesList));
	bool bIsGemContainLetter = false;

	for(iRow=0; iRow < iNumberOfRow; iRow++)
		for( iColumn = 0; iColumn < iNumberOfColumn; iColumn++)
		{
			if (m_GameBoardManager.IsBlankCell(iRow, iColumn))
			{
				m_BoardViewMatrix[iRow][iColumn].m_pSprite = NULL;
				m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite = NULL;

				// mask
				pSprite = Sprite::createWithSpriteFrameName( "brick.png");
				//pSprite->setAnchorPoint(ccp(0,0));
				pSprite->setPosition( ccp(m_fBoardLeftPosition + iColumn * m_SymbolSize.width, m_fBoardBottomPosition + iRow * m_SymbolSize.height));
				pSprite->setScale(0.929f);
				m_pBoardBatchNode->addChild(pSprite, 10);

				//continue;
			}
			else
			{
				bIsGemContainLetter = false;
				if (levelConfig.m_bIsMainWordExistedOnBoard)
				{
					const Word& mainWord = m_GameBoardManager.GetGameWordManager()->GetMainWord();

					for(int i=0; i< mainWord.m_iWordLength; i++)
					{			
						if (!mainWord.m_ActivatedCharacterFlags[i])
							if(levelConfig.m_MainWordLetterPosition[i].m_iRow == iRow && levelConfig.m_MainWordLetterPosition[i].m_iColumn == iColumn)
							{
								bIsGemContainLetter = true;
								break;
							}
					}
				}

				for( iFlag =0; iFlag < 2; iFlag++)
				{									
					pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(m_GameBoardManager.GetCellValue(iRow, iColumn), 
						bIsGemContainLetter?_GCT_HAS_LETTER_:_GCT_NONE_ ).c_str());
					//pSprite->setAnchorPoint(ccp(0,0));
					pSprite->setPosition( ccp(m_fBoardLeftPosition + iColumn * m_SymbolSize.width, m_fBoardBottomPosition + iRow * m_SymbolSize.height));
					pSprite->setScale(1.f);

					//m_BoardSpriteMatrix[iRow][iColumn] = pSprite;
					if (iFlag == 0)
					{
						m_BoardViewMatrix[iRow][iColumn].m_pSprite = pSprite;					
					}
					else
					{
						//pSprite->setColor(ccc3( 250, 10, 10));
						pSprite->setVisible(false);
						m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite = pSprite;								
					}

					m_pBoardBatchNode->addChild(pSprite);

					// extra, add obstacle to map
					/*if (m_GameBoardManager.GetCellObstacleType( iRow, iColumn) != _OT_NONE_)
					{
						Sprite* pMaskSprite = Sprite::createWithSpriteFrameName( "Lock.png");
						pMaskSprite->setAnchorPoint(Point(0,0));
						pSprite->addChild(pMaskSprite);
					}*/			
					if (iFlag == 0)
					{
						int iBlockID;
						iBlockID = m_GameBoardManager.GetObstacleBlockID(iRow, iColumn);
						if (iBlockID < 0)
							continue;

						for(iObstacleTypeID = iObstacleTypeCount-1; iObstacleTypeID >=0; iObstacleTypeID--) // from low to highest priority
						{							
							ObstacleData& obstacleData = pObstacleProcessManager->GetObstacleData(iBlockID, iObstacleTypeID);
							if (obstacleData.m_bIsActive)
							{
								Sprite* pMaskSprite = 
									Sprite::createWithSpriteFrameName( GameConfigManager::getInstance()->GetObstacleLevelDescription(iObstacleTypeID, obstacleData.m_iObstacleLevel).m_sSpriteFileName.c_str()); //"Lock.png");														
								pMaskSprite->setAnchorPoint(Point(0,0));
								pSprite->addChild(pMaskSprite, 10);

								if (obstacleData.m_iObstacleLevel > 0)
									m_BoardObstaclesList[iBlockID][iObstacleTypeID] = pMaskSprite;
							}
						}
					}
				}
			}
		}
	

	// add letter to gems if existing
	if (levelConfig.m_bIsMainWordExistedOnBoard)
	{
		const Word& mainWord = m_GameBoardManager.GetGameWordManager()->GetMainWord();
		int iGemLetterBlockID;
		unsigned char iLetter;

		for(int i=0; i< mainWord.m_iWordLength; i++)
		{			
			if (!mainWord.m_ActivatedCharacterFlags[i])
			{
				iLetter = mainWord.m_sWord[i];
				iGemLetterBlockID = m_GameBoardManager.AllocFreeGemLetterBlock( iLetter, true);
				AddLetterToGem( levelConfig.m_MainWordLetterPosition[i], m_GameBoardManager.GetCellValue( levelConfig.m_MainWordLetterPosition[i].m_iRow,
					levelConfig.m_MainWordLetterPosition[i].m_iColumn), mainWord.m_sWord[i], iGemLetterBlockID);
			}
		}
	}


	// create temp sprite, this's used to animate the drag line
	m_pTempSpriteForAction = Sprite::createWithSpriteFrameName( "brick.png");
	m_pTempSpriteForAction->retain();
	//m_pTempSpriteForAction->setAnchorPoint(ccp(0,0));
	m_pTempSpriteForAction->setPosition( ccp(0,0));
	m_pTempSpriteForAction->setScale(0.55f);
	
	m_pTempSpriteForAction->setVisible(false);
	m_pBoardBatchNode->addChild(m_pTempSpriteForAction);

	// play sound
	SoundManager::PlayBackgroundMusic();
}

void HelloWorld::menuCloseCallback(Object* pSender)
{
	auto scene =  LevelMapScene::create(); //HelloWorld::createScene();	

    // run
	Director::getInstance()->replaceScene(scene);

    //Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //exit(0);
#endif
}

void HelloWorld::visit()
{
	 kmGLPushMatrix();

    glEnable(GL_SCISSOR_TEST);

	EGLView::getInstance()->setScissorInPoints(    // scissorRect is the rectangle you want to show.
		m_fBoardLeftClipPosition, m_fBoardBottomClipPosition,
		m_fBoardLeftClipPosition + m_fMaskWidth, m_fBoardBottomClipPosition + m_fMaskHeight);
        //scissorRect.origin.x, scissorRect.origin.y,
        //scissorRect.size.width, scissorRect.size.height);

    CCLayer::visit();
    glDisable(GL_SCISSOR_TEST);
    kmGLPopMatrix();
}

std::string HelloWorld::GetImageFileFromGemID(int iGemID, GemComboType_e eGemComboType)
{
	switch (eGemComboType)
	{		
		case _GCT_COMBO4_:
			return "combo4.png";
		case _GCT_COMBO4_2_:
			return "combo4_2.png";
		case _GCT_COMBO5_:
			return "combo5.png";
		case _GCT_COMBO5_2_:
			return "combo5_2.png";
		case _GCT_COMBO6_:
			return "combo6.png";
		case _GCT_COMBO6_2_:
			return "combo6_2.png";
	}

	switch(iGemID)
	{
		case 0:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Orange.png"; //"Candy_Cam.png";
				case _GCT_COMBO4_:
					return "Orange_Combo4.png";
				case _GCT_COMBO4_2_:
					return "Orange_Combo4_2.png";
				case _GCT_COMBO5_:
				case _GCT_COMBO5_2_:
					return "Orange_Combo5.png";
				case _GCT_COMBO6_:
				case _GCT_COMBO6_2_:
					return "Orange_Combo6.png";
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "alienOrange.png";

			}

		}
		case 1:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Red.png"; //"Candy_Do.png";
				case _GCT_COMBO4_:
					return "Red_Combo4.png";
				case _GCT_COMBO4_2_:
					return "Red_Combo4_2.png";
				case _GCT_COMBO5_:
				case _GCT_COMBO5_2_:
					return "Red_Combo5.png";
				case _GCT_COMBO6_:
				case _GCT_COMBO6_2_:
					return "Red_Combo6.png";
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "alienRed.png";
			}			
		}
		case 2:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Pink.png"; //"Candy_Tim.png";
				case _GCT_COMBO4_:
					return "Pink_Combo4.png";
				case _GCT_COMBO4_2_:
					return "Pink_Combo4_2.png";
				case _GCT_COMBO5_:
				case _GCT_COMBO5_2_:
					return "Pink_Combo5.png";
				case _GCT_COMBO6_:
				case _GCT_COMBO6_2_:
					return "Pink_Combo6.png";
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "alienPink.png";
			}						
		}
		case 3:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "White.png"; //"Candy_Vang.png";
				case _GCT_COMBO4_:
					return "White_Combo4.png";
				case _GCT_COMBO4_2_:
					return "White_Combo4_2.png";
				case _GCT_COMBO5_:
				case _GCT_COMBO5_2_:
					return "White_Combo5.png";
				case _GCT_COMBO6_:
				case _GCT_COMBO6_2_:
					return "White_Combo6.png";
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "alienWhite.png";
			}									
		}
		case 4:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Blue.png"; //"Candy_XanhDuong.png";
				case _GCT_COMBO4_:
					return "Blue_Combo4.png";
				case _GCT_COMBO4_2_:
					return "Blue_Combo4_2.png";
				case _GCT_COMBO5_:
				case _GCT_COMBO5_2_:
					return "Blue_Combo5.png";
				case _GCT_COMBO6_:
				case _GCT_COMBO6_2_:
					return "Blue_Combo6.png";
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "alienBlue.png";
			}												
		}
		case 5:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Green.png"; //"Candy_XanhLa.png";
				case _GCT_COMBO4_:
					return "Green_Combo4.png";
				case _GCT_COMBO4_2_:
					return "Green_Combo4_2.png";
				case _GCT_COMBO5_:
				case _GCT_COMBO5_2_:
					return "Green_Combo5.png";
				case _GCT_COMBO6_:
				case _GCT_COMBO6_2_:
					return "Green_Combo6.png";
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "alienGreen.png";
			}					
		}
	}

	return "Candy_6.png";
}



bool HelloWorld::onTouchBegan(Touch *pTouch, Event *pEvent)
{
	if (m_bIsEffectPlaying || m_bIsCellDragPlaying)
		return true;

	// only allow 1 finger at one 1 time
	if (m_pSaveTouch == NULL)
	{
		m_pSaveTouch = pTouch;		
	}
	else
		return true;

	Point touchPosition = pTouch->getLocation();
	if (touchPosition.x >= m_fBoardLeftPosition - m_SymbolSize.width/2.f && touchPosition.y >= m_fBoardBottomPosition - m_SymbolSize.height/2.f)
		if (touchPosition.x <= m_fBoardLeftPosition + m_GameBoardManager.GetColumnNumber()* m_SymbolSize.width - m_SymbolSize.width/2.f
			&& touchPosition.y <= m_fBoardBottomPosition + m_GameBoardManager.GetRowNumber()* m_SymbolSize.height - m_SymbolSize.height/2.f)
	{
		int iRow = (touchPosition.y - m_fBoardBottomPosition + m_SymbolSize.height/2.f)/m_SymbolSize.height;
		int iColumn = (touchPosition.x - m_fBoardLeftPosition + m_SymbolSize.width/2.f)/m_SymbolSize.width;
		
		if (m_GameBoardManager.IsBlankCell(iRow, iColumn))
			return true;

		m_SelectedCell = Cell(iRow, iColumn );		
		m_StartTouchPosition = touchPosition;
		m_iSaveLastCellMoveDelta = 0;

		if (m_GameBoardManager.IsComboCell(iRow, iColumn) && m_GameBoardManager.GetObstacleBlockID(iRow, iColumn) < 0)
		{
			m_eTouchMoveState = _TMS_BEGIN_ACTIVATE_COMBO_;			
		}
		else
		{
			m_eTouchMoveState = _TMS_BEGIN_IDENTIFY_;
		}

	}

	return true;
}

void HelloWorld::onTouchEnded(Touch* pTouch, Event* pEvent)
{
	if (pTouch == m_pSaveTouch)
		m_pSaveTouch = NULL;

	if (m_eTouchMoveState == _TMS_NONE_)
		return;

	bool bIsBlocked = false;

	if ((m_eTouchMoveState == _TMS_MOVE_HORIZONTAL_ && m_GameBoardManager.IsRowLocked(m_SelectedCell.m_iRow)) || 
		(m_eTouchMoveState == _TMS_MOVE_VERTICAL_ &&  m_GameBoardManager.IsColumnLocked(m_SelectedCell.m_iColumn)))
	{
		bIsBlocked = true;		
	}

	// remove hint sprites
	if ( m_HintSprites.size() >0 )
	{
		for(auto sprite:m_HintSprites)
			sprite->removeFromParentAndCleanup(true);
		
		m_HintSprites.clear();
	}

	if (m_eTouchMoveState < _TMS_BEGIN_ACTIVATE_COMBO_) //this is normal move
	{
		Point currentPosition = pTouch->getLocation();
		float fDeltaX = currentPosition.x - m_StartTouchPosition.x;
		float fDeltaY = currentPosition.y - m_StartTouchPosition.y;
		AdjustPosition( bIsBlocked, fDeltaX, fDeltaY, m_SelectedCell.m_iRow, m_SelectedCell.m_iColumn);
	}
	else if (m_eTouchMoveState != _TMS_BEGIN_ACTIVATE_COMBO_)
	{
		ComboActivateDirection_e eComboDireciton;
		switch (m_eTouchMoveState)
		{	
			default:
			case _TMS_ACTIVATE_COMBO_LEFT_:
				eComboDireciton = _CAD_LEFT_;
				break;
			case _TMS_ACTIVATE_COMBO_UP_:
				eComboDireciton = _CAD_UP_;
				break;
			case _TMS_ACTIVATE_COMBO_RIGHT_:
				eComboDireciton = _CAD_RIGHT_;
				break;
			case _TMS_ACTIVATE_COMBO_DOWN_:
				eComboDireciton = _CAD_DOWN_;
				break;		
		}

		m_ComputeMoveResult.Reset(false);
		m_GameBoardManager.ExecuteComboEffect( m_SelectedCell.m_iRow, m_SelectedCell.m_iColumn, eComboDireciton,
			m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_NewDoubleComboList, 
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_TriggeredCombo5ChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells);

		PlayEffect2( false, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_NewDoubleComboList, 
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_TriggeredCombo5ChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells,  m_ComputeMoveResult.m_UnlockedLetterCells,  m_ComputeMoveResult.m_NewCells, true);
	}
	m_eTouchMoveState = _TMS_NONE_;	
}

void HelloWorld::onTouchCancelled(Touch *pTouch, Event *pEvent)
{
	if (pTouch == m_pSaveTouch)
		m_pSaveTouch = NULL;
}

void HelloWorld::onTouchMoved(Touch *pTouch, Event *pEvent)
{
	if (m_pSaveTouch != pTouch)
		return;

	if (m_eTouchMoveState == _TMS_NONE_)
		return;

	Point currentPosition = pTouch->getLocation();
	float fDeltaX = currentPosition.x - m_StartTouchPosition.x;
	float fDeltaY = currentPosition.y - m_StartTouchPosition.y;

	if( (currentPosition.x < m_fBoardLeftPosition - m_SymbolSize.width/2.f || currentPosition.y < m_fBoardBottomPosition - m_SymbolSize.height/2.f)
		|| (currentPosition.x > m_fBoardLeftPosition + m_GameBoardManager.GetColumnNumber()* m_SymbolSize.width -m_SymbolSize.width/2.f
		|| currentPosition.y > m_fBoardBottomPosition + m_GameBoardManager.GetRowNumber()* m_SymbolSize.height - m_SymbolSize.height/2.f))
	{		
		//AdjustPosition( fDeltaX, fDeltaY, m_SelectedCell.m_iRow, m_SelectedCell.m_iColumn);
		//m_eTouchMoveState = _TMS_NONE_;
		//return;
	}	

	if (m_eTouchMoveState == _TMS_BEGIN_IDENTIFY_)
	{				
		if ( fabs(fDeltaX) > fabs(fDeltaY) &&  fabs(fDeltaX) > 5.f)
		{
			m_eTouchMoveState = _TMS_MOVE_HORIZONTAL_;

			m_iMovingCellListLength = m_GameBoardManager.GetColumnNumber();
			for(int iColumn = 0; iColumn< m_GameBoardManager.GetColumnNumber(); iColumn++)
			{
				m_MovingCellList[iColumn] = m_BoardViewMatrix[m_SelectedCell.m_iRow][iColumn];
				m_MovingCellMirrorList[iColumn] = m_BoardViewMirrorMatrix[m_SelectedCell.m_iRow][iColumn];
				if (m_MovingCellList[iColumn].m_pSprite != NULL)
					m_MovingCellMirrorList[iColumn].m_pSprite->setPosition(m_MovingCellList[iColumn].m_pSprite->getPosition());
			}
		}
		else if (fabs(fDeltaX) < fabs(fDeltaY) && fabs(fDeltaY) > 5.f)
		{
			m_eTouchMoveState = _TMS_MOVE_VERTICAL_;

			m_iMovingCellListLength = m_GameBoardManager.GetRowNumber();
			for(int iRow = 0; iRow< m_GameBoardManager.GetRowNumber(); iRow++)
			{
				m_MovingCellList[iRow] = m_BoardViewMatrix[iRow][m_SelectedCell.m_iColumn];
				m_MovingCellMirrorList[iRow] = m_BoardViewMirrorMatrix[iRow][m_SelectedCell.m_iColumn];

				if (m_MovingCellList[iRow].m_pSprite != NULL)
					m_MovingCellMirrorList[iRow].m_pSprite->setPosition(m_MovingCellList[iRow].m_pSprite->getPosition());
			}
		}
	}

	
	if (m_eTouchMoveState == _TMS_MOVE_HORIZONTAL_)
	{		
		HorizontalMoveUlti(fDeltaX);
	}
	else if (m_eTouchMoveState == _TMS_MOVE_VERTICAL_)
	{	
		VerticalMoveUlti(fDeltaY);
		/*for(int iRow = 0; iRow< m_iMovingCellListLength; iRow++)
		{
			m_BoardSpriteMatrix[iRow][m_SelectedCell.m_iColumn]->setPositionY(
				m_fBoardBottomPosition + (iRow-m_GameBoardManager.GetRowNumber()) * m_SymbolSize.height + fDeltaY);
		}*/
	}

	if ((m_eTouchMoveState == _TMS_MOVE_HORIZONTAL_ && m_GameBoardManager.IsRowLocked(m_SelectedCell.m_iRow)) || 
		(m_eTouchMoveState == _TMS_MOVE_VERTICAL_ &&  m_GameBoardManager.IsColumnLocked(m_SelectedCell.m_iColumn)))
	{
		onTouchEnded( pTouch, NULL);
		return;
	}


	// ************************************ active combo
	if (m_eTouchMoveState >= _TMS_BEGIN_ACTIVATE_COMBO_ && m_eTouchMoveState <= _TMS_ACTIVATE_COMBO_DOWN_)
	{
		int iCurrentRow = (currentPosition.y - m_fBoardBottomPosition + m_SymbolSize.height/2.f)/m_SymbolSize.height;
		int iCurrentColumn = (currentPosition.x - m_fBoardLeftPosition + m_SymbolSize.width/2.f)/m_SymbolSize.width;
		TouchMoveState eNewMoveState = _TMS_BEGIN_ACTIVATE_COMBO_;
		if (iCurrentRow != m_SelectedCell.m_iRow || iCurrentColumn != m_SelectedCell.m_iColumn)
		{
			if (fabs(fDeltaX) > fabs(fDeltaY)) // horizontal move
			{
				if (fDeltaX < 0)
					eNewMoveState = _TMS_ACTIVATE_COMBO_LEFT_;	
				else
					eNewMoveState = _TMS_ACTIVATE_COMBO_RIGHT_;	
			}
			else // vertical move
			{
				if (fDeltaY < 0)
					eNewMoveState = _TMS_ACTIVATE_COMBO_DOWN_;	
				else
					eNewMoveState = _TMS_ACTIVATE_COMBO_UP_;	
			}

		}

		if (m_eTouchMoveState == eNewMoveState)
			return;
		else //draw hint
		{
			m_eTouchMoveState = eNewMoveState;
			DrawComboHint();
		}
	}
}

void HelloWorld::update(float fDeltaTime)
{
	if (m_bIsCellDragPlaying)
	{
		if (m_ePlayingDragEffect == _TMS_MOVE_HORIZONTAL_)
		{
			//CCLOG("%f", m_pTempSpriteForAction->getPositionX());
			HorizontalMoveUlti(m_pTempSpriteForAction->getPositionX());
		}
		else 
		{
			VerticalMoveUlti(m_pTempSpriteForAction->getPositionY());
		}
	}
}

float round(float d)
{
	if (d < 0)
		return floorf(d + 0.65f);
	else
		return ceilf(d - 0.65f);
}

void HelloWorld::AdjustPosition(bool bIsBlocked, float fDeltaX, float fDeltaY, int iRowMove, int iColumnMove)
{
	CCLOG("Debug");
	/*std::vector<Cell> destroyCells;
	std::vector<ComboEffectCell> comboEffectCells;
	std::vector<Cell> destroyedByEffectCells;
	std::vector<Cell> originalMovedCells;
	std::vector<Cell> targetMovedCells;
	std::vector<Cell> newCells;
	std::vector<ComboEffectCell> newComboCells;*/

	bool bMoveIsValid = false;
	float fMoveUnit;
	int iMoveUnit;

	int iNumberOfRow = m_GameBoardManager.GetRowNumber();
	int iNumberOfColumn = m_GameBoardManager.GetColumnNumber();

	m_ComputeMoveResult.Reset(false);

	if (m_eTouchMoveState == _TMS_MOVE_HORIZONTAL_)
	{				
		fMoveUnit = fDeltaX/m_SymbolSize.width;				
		fMoveUnit = round(fMoveUnit);
		iMoveUnit = (int)fMoveUnit;

		m_ePlayingDragEffect = _TMS_MOVE_HORIZONTAL_;

		if (!bIsBlocked && m_GameBoardManager.RecheckAfterMoveV2( m_SelectedCell.m_iRow,-1,  -1, iMoveUnit, 
			m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_NewDoubleComboList, m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_TriggeredCombo5ChainList,
				m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells, true))
		//if (false)
		{
			CCLOG("1");

			bMoveIsValid = true;			
			m_bIsCellDragPlaying = true;			

			// adjust the cell position in 0.15f second
			float fRealDelta = fMoveUnit * m_SymbolSize.width;
			
			m_pTempSpriteForAction->setPositionX(fDeltaX);
			m_pTempSpriteForAction->runAction( 
				Sequence::create(
					MoveTo::create( 0.15f, ccp(fRealDelta, 0)),
					CCCallFunc::create( this, callfunc_selector( HelloWorld::OnEndDragEffect)),
					NULL));			

			// move cell to new position of matrix
			CellView tempCellList[_BOARD_MAX_COLUMN_NUMBER_];
			CellView tempCellMirrorList[_BOARD_MAX_COLUMN_NUMBER_];			
			for(int iColumn=0; iColumn < iNumberOfColumn; iColumn++)
			{
				tempCellList[iColumn] = m_BoardViewMatrix[m_SelectedCell.m_iRow][iColumn];
				tempCellMirrorList[iColumn] = m_BoardViewMirrorMatrix[m_SelectedCell.m_iRow][iColumn];
			}
						
			int iTranslationCell = 0;
			int iSign = 1;
			if (iMoveUnit != 0)
				iSign = iMoveUnit/ abs(iMoveUnit);
			
			for(int iColumn=0; iColumn < iNumberOfColumn; iColumn++)
				if (!m_GameBoardManager.IsBlankCell(m_SelectedCell.m_iRow, iColumn))				
				{
					iTranslationCell = 0;
					for(int iStep=1; iStep <= abs(iMoveUnit); iStep++)
					{
						iTranslationCell += iSign;
						if (iTranslationCell < 0)
							iTranslationCell += iNumberOfColumn;

						if (m_GameBoardManager.IsBlankCell( m_SelectedCell.m_iRow, (iColumn + iTranslationCell) % iNumberOfColumn))
						{
							iStep--;
						}
					}

					m_BoardViewMatrix[m_SelectedCell.m_iRow][(iColumn + iNumberOfColumn + iTranslationCell) % (iNumberOfColumn)] = tempCellList[iColumn];
					m_BoardViewMirrorMatrix[m_SelectedCell.m_iRow][(iColumn + iNumberOfColumn + iTranslationCell) % (iNumberOfColumn)] = tempCellMirrorList[iColumn];					
				}
		}
		else
		{			
			// play sound
			SoundManager::PlaySoundEffect(_SET_WRONG_MOVE_);

			// return cells to their original positions
			m_bIsCellDragPlaying = true;

			m_pTempSpriteForAction->setPositionX(fDeltaX);
			m_pTempSpriteForAction->runAction( 
				Sequence::create(
					MoveTo::create( 0.15f, ccp(0, 0)),
					CCCallFunc::create( this, callfunc_selector( HelloWorld::OnEndDragEffect)),
					NULL));
		}
	}
	else if (m_eTouchMoveState == _TMS_MOVE_VERTICAL_)
	{		
		fMoveUnit = fDeltaY/m_SymbolSize.height;
		
		fMoveUnit = round(fMoveUnit);

		iMoveUnit = (int)fMoveUnit;

		m_ePlayingDragEffect = _TMS_MOVE_VERTICAL_;

		if (!bIsBlocked && m_GameBoardManager.RecheckAfterMoveV2( -1, m_SelectedCell.m_iColumn, iMoveUnit, -1, 
				m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_NewDoubleComboList, m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_TriggeredCombo5ChainList,
				m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells, true))
		//if (false)
		{
			bMoveIsValid = true;					
			m_bIsEffectPlaying = true;
			m_bIsCellDragPlaying = true;

			// adjust the cell position in 0.15f second
			float fRealDelta = fMoveUnit * m_SymbolSize.height;
			
			m_pTempSpriteForAction->setPositionY(fDeltaY);
			m_pTempSpriteForAction->runAction( 
				Sequence::create(
					MoveTo::create( 0.15f, ccp(0, fRealDelta)),
					CCCallFunc::create( this, callfunc_selector( HelloWorld::OnEndDragEffect)),
					NULL));


			// move cell to new position of matrix
			CellView tempCellList[_BOARD_MAX_ROW_NUMBER_];
			CellView tempCellMirrorList[_BOARD_MAX_ROW_NUMBER_];
			for(int iRow=0; iRow < iNumberOfRow; iRow++)
			{
				tempCellList[iRow] = m_BoardViewMatrix[iRow][m_SelectedCell.m_iColumn];
				tempCellMirrorList[iRow] = m_BoardViewMirrorMatrix[iRow][m_SelectedCell.m_iColumn];
			}
						
			int iTranslationCell = 0;
			int iSign = 1;
			if (iMoveUnit != 0)
				iSign = iMoveUnit/ abs(iMoveUnit);

			for(int iRow=0; iRow < iNumberOfRow; iRow++)
				if (!m_GameBoardManager.IsBlankCell(iRow, m_SelectedCell.m_iColumn))				
				{
					iTranslationCell = 0;
					for(int iStep=1; iStep <= abs(iMoveUnit); iStep++)
					{
						iTranslationCell += iSign;
						if (iTranslationCell < 0)
							iTranslationCell += iNumberOfRow;

						if (m_GameBoardManager.IsBlankCell( (iRow + iTranslationCell) % (iNumberOfRow), m_SelectedCell.m_iColumn))
						{
							iStep--;
						}
					}

					m_BoardViewMatrix[(iRow + iNumberOfRow + iTranslationCell) % (iNumberOfRow)][m_SelectedCell.m_iColumn] = tempCellList[iRow];
					m_BoardViewMirrorMatrix[(iRow + iNumberOfRow + iTranslationCell) % (iNumberOfRow)][m_SelectedCell.m_iColumn] = tempCellMirrorList[iRow];					
				}
		}
		else
		{			
			// play sound
			SoundManager::PlaySoundEffect(_SET_WRONG_MOVE_);

			// return cells to their original positions
			m_bIsCellDragPlaying = true;

			m_pTempSpriteForAction->setPositionY(fDeltaY);
			m_pTempSpriteForAction->runAction( 
				Sequence::create(
					MoveTo::create( 0.15f, ccp(0, 0)),
					CCCallFunc::create( this, callfunc_selector( HelloWorld::OnEndDragEffect)),
					NULL));
		}
	}

	CCLOG("2");
	// now play effect to destroy/move old cells, and generate new cells
	if (bMoveIsValid)
	{		
		PlayEffect2( false, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_NewDoubleComboList, 
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_TriggeredCombo5ChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells,  m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells, true);
	}

	/*
	std::vector<Cell> destroyCells;
	std::vector<Cell> originalMovedCells;
	std::vector<Cell> targetMovedCells;
	std::vector<Cell> newCells;
	bool bMoveIsValid = false;
	float fMoveUnit;
	int iMoveUnit;

	int iNumberOfRow = m_GameBoardManager.GetRowNumber();
	int iNumberOfColumn = m_GameBoardManager.GetColumnNumber();

	if (m_eTouchMoveState == _TMS_MOVE_HORIZONTAL_)
	{
		fMoveUnit = fDeltaX/m_SymbolSize.width;
				
		fMoveUnit = round(fMoveUnit);

		iMoveUnit = (int)fMoveUnit;
		if (m_GameBoardManager.RecheckAfterMove( m_SelectedCell.m_iRow - iNumberOfRow,-1,  -1, iMoveUnit, destroyCells, originalMovedCells, targetMovedCells, newCells))
		{
			bMoveIsValid = true;
			// adjust the cell position in 0.15f second
			float fRealDelta = fMoveUnit * m_SymbolSize.width;
			int iColumn;
			for(iColumn=0; iColumn < iNumberOfColumn*3; iColumn++)
			{
				if (iColumn + iMoveUnit < iNumberOfColumn -1 || iColumn + iMoveUnit > iNumberOfColumn * 2)
				{
					m_BoardSpriteMatrix[m_SelectedCell.m_iRow][iColumn]->setPositionX( m_fBoardLeftPosition + 
						((iColumn + iMoveUnit + iNumberOfColumn*3) % (iNumberOfColumn*3) - iNumberOfColumn) * m_SymbolSize.width);
				}
				else
					m_BoardSpriteMatrix[m_SelectedCell.m_iRow][iColumn]->runAction(
						MoveBy::create(0.15f, ccp(fRealDelta - fDeltaX, 0)));
			}		

			// move sprite to new position of matrix
			CCSprite* tempSpriteList[_BOARD_MAX_COLUMN_NUMBER_*3];
			for(iColumn=0; iColumn < iNumberOfColumn*3; iColumn++)
			{
				tempSpriteList[iColumn] = m_BoardSpriteMatrix[m_SelectedCell.m_iRow][iColumn];
			}
		
			// update all on the center horizontal line
			for(iColumn=0; iColumn < iNumberOfColumn*3; iColumn++)
			{
				m_BoardSpriteMatrix[m_SelectedCell.m_iRow][(iColumn + iMoveUnit + iNumberOfColumn*3) % (iNumberOfColumn*3)] = tempSpriteList[iColumn];
			}
			// update top region
			for(iColumn=0; iColumn < iNumberOfColumn; iColumn++)
				tempSpriteList[iColumn] = m_BoardSpriteMatrix[m_SelectedCell.m_iRow + iNumberOfRow][iColumn + iNumberOfColumn];
			for(iColumn=0; iColumn < iNumberOfColumn; iColumn++)
			{
				m_BoardSpriteMatrix[m_SelectedCell.m_iRow + iNumberOfRow][(iColumn + iMoveUnit + iNumberOfColumn) % iNumberOfColumn + iNumberOfColumn] = tempSpriteList[iColumn];
				UpdatePostionOfSprite(m_SelectedCell.m_iRow + iNumberOfRow,(iColumn + iMoveUnit + iNumberOfColumn) % iNumberOfColumn + iNumberOfColumn);
			}
			// update bot region
			for(iColumn=0; iColumn < iNumberOfColumn; iColumn++)
				tempSpriteList[iColumn] = m_BoardSpriteMatrix[m_SelectedCell.m_iRow - iNumberOfRow][iColumn +  iNumberOfColumn];
			for(iColumn=0; iColumn < iNumberOfColumn; iColumn++)
			{
				m_BoardSpriteMatrix[m_SelectedCell.m_iRow - iNumberOfRow][(iColumn + iMoveUnit + iNumberOfColumn) % iNumberOfColumn + iNumberOfColumn] = tempSpriteList[iColumn];
				UpdatePostionOfSprite( m_SelectedCell.m_iRow - iNumberOfRow, (iColumn + iMoveUnit + iNumberOfColumn) % iNumberOfColumn + iNumberOfColumn);
			}
		}
		else
		{
			for(int iColumn=0; iColumn < iNumberOfColumn*3; iColumn++)
			{			
				m_BoardSpriteMatrix[m_SelectedCell.m_iRow][iColumn]->runAction(
					MoveTo::create(0.15f, 
					ccp(m_fBoardLeftPosition + (iColumn-iNumberOfColumn) * m_SymbolSize.width, m_fBoardBottomPosition + (m_SelectedCell.m_iRow-iNumberOfRow) * m_SymbolSize.height)));
			}		
		}
	}
	else if (m_eTouchMoveState == _TMS_MOVE_VERTICAL_)
	{
		fMoveUnit = fDeltaY/m_SymbolSize.height;
		
		fMoveUnit = round(fMoveUnit);

		iMoveUnit = (int)fMoveUnit;
		if (m_GameBoardManager.RecheckAfterMove( -1, m_SelectedCell.m_iColumn  - iNumberOfColumn, iMoveUnit, -1, destroyCells, originalMovedCells, targetMovedCells, newCells))
		{
			bMoveIsValid = true;
			float fRealDelta = fMoveUnit * m_SymbolSize.height;
			int iRow;
			for(iRow=0; iRow < iNumberOfRow*3; iRow++)
			{
				if (iRow + iMoveUnit <  iNumberOfRow -1 || iRow + iMoveUnit > iNumberOfRow* 2)
				{
					m_BoardSpriteMatrix[iRow][m_SelectedCell.m_iColumn]->setPositionY( m_fBoardBottomPosition + 
						((iRow + iMoveUnit + iNumberOfRow*3) % (iNumberOfRow*3) - iNumberOfRow) * m_SymbolSize.height);
				}
				else
					m_BoardSpriteMatrix[iRow][m_SelectedCell.m_iColumn]->runAction(
						MoveBy::create(0.15f, ccp(0, fRealDelta - fDeltaY)));
			}		

			// move sprite
			CCSprite* tempSpriteList[_BOARD_MAX_ROW_NUMBER_*3];
			for(iRow=0; iRow < iNumberOfRow*3; iRow++)
			{
				tempSpriteList[iRow] = m_BoardSpriteMatrix[iRow][m_SelectedCell.m_iColumn];
			}
		
			// update all on the center vertical line
			for(iRow=0; iRow < iNumberOfRow*3; iRow++)
			{
				m_BoardSpriteMatrix[(iRow + iMoveUnit + iNumberOfRow*3) % (iNumberOfRow*3)][m_SelectedCell.m_iColumn] = tempSpriteList[iRow];			
			}
			// update left region
			for(iRow=0; iRow < iNumberOfRow; iRow++)
				tempSpriteList[iRow] = m_BoardSpriteMatrix[iRow + iNumberOfRow][ m_SelectedCell.m_iColumn - iNumberOfColumn];
			for(iRow=0; iRow < iNumberOfRow; iRow++)
			{
				m_BoardSpriteMatrix[(iRow + iMoveUnit + iNumberOfRow) % iNumberOfRow + iNumberOfRow][m_SelectedCell.m_iColumn - iNumberOfColumn] = tempSpriteList[iRow];			
				UpdatePostionOfSprite((iRow + iMoveUnit + iNumberOfRow) % iNumberOfRow + iNumberOfRow, m_SelectedCell.m_iColumn - iNumberOfColumn);
			}
			// update right region
			for(iRow=0; iRow < iNumberOfRow; iRow++)
				tempSpriteList[iRow] = m_BoardSpriteMatrix[iRow + iNumberOfRow][ m_SelectedCell.m_iColumn + iNumberOfColumn];
			for(iRow=0; iRow < iNumberOfRow; iRow++)
			{
				m_BoardSpriteMatrix[(iRow + iMoveUnit + iNumberOfRow) % iNumberOfRow + iNumberOfRow][m_SelectedCell.m_iColumn + iNumberOfColumn] = tempSpriteList[iRow];			
				UpdatePostionOfSprite( (iRow + iMoveUnit + iNumberOfRow) % iNumberOfRow + iNumberOfRow, m_SelectedCell.m_iColumn + iNumberOfColumn);
			}
		}
		else
		{
			for(int iRow=0; iRow < iNumberOfRow*3; iRow++)
			{			
				m_BoardSpriteMatrix[iRow][m_SelectedCell.m_iColumn]->runAction(
					MoveTo::create(0.15f, 
					ccp(m_fBoardLeftPosition + (m_SelectedCell.m_iColumn-iNumberOfColumn) * m_SymbolSize.width, m_fBoardBottomPosition + (iRow-iNumberOfRow) * m_SymbolSize.height)));
			}		
		}
	}

	// now play effect to destroy/move old cells, and generate new cells
	if (bMoveIsValid)
		PlayEffect(destroyCells, originalMovedCells, targetMovedCells, newCells);*/
}

void HelloWorld::OnEndDragEffect()
{
	update(0);	
	m_bIsCellDragPlaying = false;
}

void HelloWorld::CheckBoardStateAfterMove()
{
	// clean old list of chain
	m_GameBoardManager.GetObstacleProcessManager()->CleanWaitingClearList();


	bool bMoveIsValid = false;
	m_ComputeMoveResult.Reset(false);

	if (m_GameBoardManager.RecheckAfterMoveV2( -1, -1, -1, -1, m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_NewDoubleComboList, 
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_TriggeredCombo5ChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells, false))
	{
		PlayEffect2( false, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_NewDoubleComboList, 
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_TriggeredCombo5ChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells,  m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells, false);		
	}
	else
	{		
		/*//check end game
		if (m_GameBoardManager.GetGameWordManager()->IsMainWordUnlocked()) // complete objective ==> win		
		//if (true)
		//if (false)
		{
			// play sound effect 
			SoundManager::PlaySoundEffect(_SET_COMPLETE_WORD_);

			m_bIsEffectPlaying = true;//stop all interaction on board from now
			m_bIsEndGamePhase = true; 

			ShowMainWordUnlockEffect();
			//ExecuteBonusWinGameEffect();						
		}
		else  if (m_GameBoardManager.GetCurrentMove() == 0) // out of move ==> lose
		{
			m_bIsEndGamePhase = true; 

			EndGameNode* pEndGameNode = EndGameNode::createLayoutLose( m_GameBoardManager.GetCurrentScore(), 
				const_cast<char*>(m_GameBoardManager.GetGameWordManager()->GetMainWord().m_sWord), m_GameBoardManager.GetCurrentLevel());
			m_pHUDLayer->addChild( pEndGameNode, 10);

			SoundManager::PlaySoundEffect(_SET_LOSE);

		}
		else //game is not end yet*/
		{
			PlayUnlockLettersOfMainWordAnimation(0.f);			
		}		

		// update obstacle manager after move
		m_GameBoardManager.GetObstacleProcessManager()->UpdateAfterMove();

		UpdateObstacleListAfterMove();
	}	
}

void HelloWorld::ExecuteBonusWinGameEffect()
{
	m_ComputeMoveResult.Reset(true);

	//if (m_GameBoardManager.GetCurrentMove() > 0)
	if (m_GameBoardManager.ExecuteEndGameBonus(m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_NewDoubleComboList, 
		m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_TriggeredCombo5ChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells))
	{
		PlayEffect2( true, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_NewDoubleComboList, 
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_TriggeredCombo5ChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells,  m_ComputeMoveResult.m_UnlockedLetterCells,  m_ComputeMoveResult.m_NewCells, false);

		m_pStatusLayer->setCurrentMove( m_GameBoardManager.GetCurrentMove());
		//m_pStatusLayer->update(0);
	}
	else
	{
		PlayUnlockLettersOfBonusWordsAnimation();
	}
}

void HelloWorld::ShowWinGamePopup()
{
	std::vector<Word> subWordList;
	for(int i=0; i < m_GameBoardManager.GetGameWordManager()->GetSubWordCount(); i++)
	{
		subWordList.push_back(m_GameBoardManager.GetGameWordManager()->GetSubWord(i));
	}

	EndGameNode* pEndGameNode = EndGameNode::createLayoutWin( m_GameBoardManager.GetCurrentScore(),
		m_GameBoardManager.GetGameWordManager()->GetMainWord(), subWordList, m_GameBoardManager.GetCurrentLevel());
	pEndGameNode->addYellowStar( m_GameBoardManager.GetEarnedStars());
	m_pHUDLayer->addChild( pEndGameNode, 100);
}

/*void HelloWorld::UpdatePostionOfSprite(const int& iRow,const int& iColumn, bool bIsMirror)
{
	if (!bIsMirror)
		m_BoardViewMatrix[iRow][iColumn].m_pSprite->setPosition( ccp(m_fBoardLeftPosition + (iColumn-
			m_GameBoardManager.GetColumnNumber()) * m_SymbolSize.width, m_fBoardBottomPosition + (iRow-m_GameBoardManager.GetRowNumber()) * m_SymbolSize.height));
	else
		m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite->setPosition( ccp(m_fBoardLeftPosition + (iColumn-
			m_GameBoardManager.GetColumnNumber()) * m_SymbolSize.width, m_fBoardBottomPosition + (iRow-m_GameBoardManager.GetRowNumber()) * m_SymbolSize.height));
}*/

int HelloWorld::GetUnmoveCellsFromCurrentMovingRowOrColumn()
{
	int iBlankCellOfFullLine = 0;
	for(int iIndex = 0; iIndex< m_iMovingCellListLength; iIndex++)		
		if (m_MovingCellList[iIndex].m_pSprite == NULL)
			iBlankCellOfFullLine++;
	return iBlankCellOfFullLine;
}

void HelloWorld::HorizontalMoveUlti(float fDeltaX)
{	
	int iBlankCellOfFullLine = GetUnmoveCellsFromCurrentMovingRowOrColumn();
	
	if (fabs(fDeltaX) > (m_iMovingCellListLength - iBlankCellOfFullLine)* m_SymbolSize.width)
		fDeltaX = fDeltaX/fabs(fDeltaX) * (m_iMovingCellListLength - iBlankCellOfFullLine)* m_SymbolSize.width;	
	
	// convert negative delta to active value for easier computation
	if (fDeltaX < 0)
		fDeltaX += (m_iMovingCellListLength - iBlankCellOfFullLine)* m_SymbolSize.width;		



	float fMoveUnit = fDeltaX/m_SymbolSize.width;		
		
	fMoveUnit = floorf(fMoveUnit);			
	int iMoveUnit = (int)fMoveUnit;		
	
	//int iBlankCellInOriginMoving, iBlankCellInMirrorMoving;//blank cells on the moving path of origin and mirror
	int iTranslatedOriginCell, iTranslatedMirrorCell;

	if (iMoveUnit >= m_iMovingCellListLength)
		iMoveUnit -= m_iMovingCellListLength;	

	for(int iColumn = 0; iColumn< m_iMovingCellListLength; iColumn++)
	{
		//if (!m_GameBoardManager.IsBlankCell(m_SelectedCell.m_iRow,iColumn))
		if (m_MovingCellList[iColumn].m_pSprite != NULL)
		{
			iTranslatedOriginCell = 0;
			iTranslatedMirrorCell = 0;
			for(int iStep = 1; iStep <= iMoveUnit; iStep++)
			{
				iTranslatedOriginCell += 1;				
				if (m_MovingCellList[(iColumn + iTranslatedOriginCell ) % m_iMovingCellListLength].m_pSprite == NULL)
				{
					iStep--;
				}
			}

			iTranslatedMirrorCell = iTranslatedOriginCell;	
							
			bool bMeetSide = false;

			if (fDeltaX>0 && fDeltaX > fMoveUnit * m_SymbolSize.width) 
			{
				while ((m_MovingCellList[(iColumn + iTranslatedMirrorCell + 1 + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite == NULL)
						||  (iColumn + iTranslatedMirrorCell +  1 == m_iMovingCellListLength)) 
				{
					if (iColumn + iTranslatedMirrorCell +  1 == m_iMovingCellListLength) 
						bMeetSide = true;	

					iTranslatedMirrorCell += 1;										

					if (m_MovingCellList[(iColumn + iTranslatedMirrorCell) % m_iMovingCellListLength].m_pSprite != NULL)
						break;
					else if (bMeetSide)
					{																		
						if (m_MovingCellList[(iColumn + iTranslatedMirrorCell) % m_iMovingCellListLength].m_pSprite == NULL)
							bMeetSide = false;
					}
				}
			}

			/*

			iBlankCellInOriginMoving = 0;
			iBlankCellInMirrorMoving = 0;
			
			bool bMeetSide = false;

			for(int iDelta = 1; iDelta <= iMoveUnit; iDelta++)
				//if (m_GameBoardManager.IsBlankCell(m_SelectedCell.m_iRow, (iColumn + iDelta * iSign + m_iMovingCellListLength) % m_iMovingCellListLength))
				if (m_MovingCellList[(iColumn + iDelta * iSign + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite == NULL)
				{
					iBlankCellInOriginMoving++;
					iBlankCellInMirrorMoving++;
				}
				
			
			if ((fDeltaX>0 && fDeltaX > fMoveUnit * m_SymbolSize.width) || (fDeltaX<0 && fDeltaX < fMoveUnit * m_SymbolSize.width))
			{
				//if(iMoveUnit==1 && iColumn == 4)
				//	iColumn = iColumn;

				int iDelta = 0;
				while //(m_GameBoardManager.IsBlankCell(m_SelectedCell.m_iRow, (iColumn + (iMoveUnit + iBlankCellInMirrorMoving + iDelta + 1) * iSign + m_iMovingCellListLength) % m_iMovingCellListLength)
					((m_MovingCellList[(iColumn + (iMoveUnit + iBlankCellInMirrorMoving + iDelta + 1) * iSign + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite == NULL)
					||  (iColumn + (iMoveUnit + iBlankCellInMirrorMoving + iDelta + 1) * iSign == m_iMovingCellListLength) || (iColumn + (iMoveUnit + iBlankCellInMirrorMoving + iDelta + 1) * iSign == -1))
				{					
					if ((iColumn + (iMoveUnit + iBlankCellInMirrorMoving + iDelta + 1) * iSign == m_iMovingCellListLength) || (iColumn + (iMoveUnit + iBlankCellInMirrorMoving + iDelta + 1) * iSign == -1))
						bMeetSide = true;

					iDelta++;

					// this is real condition to continue
					//if (!m_GameBoardManager.IsBlankCell(m_SelectedCell.m_iRow, (iColumn + (iMoveUnit + iBlankCellInMirrorMoving + iDelta) * iSign + m_iMovingCellListLength) % m_iMovingCellListLength))
					if (m_MovingCellList[ (iColumn + (iMoveUnit + iBlankCellInMirrorMoving + iDelta) * iSign + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite != NULL)
						break;
				}

				iBlankCellInMirrorMoving += iDelta;
			}
			
			m_MovingCellList[iColumn].m_pSprite->setPositionX(
				m_fBoardLeftPosition + ((iColumn + (iMoveUnit +iBlankCellInOriginMoving) * iSign + m_iMovingCellListLength) % m_iMovingCellListLength ) * m_SymbolSize.width + fDeltaX - fMoveUnit  * m_SymbolSize.width);
			
			if (iBlankCellInMirrorMoving != iBlankCellInOriginMoving)
			{
				m_MovingCellMirrorList[iColumn].m_pSprite->setVisible(true);
				m_MovingCellMirrorList[iColumn].m_pSprite->setPositionX(
					m_fBoardLeftPosition + ((iColumn + (iMoveUnit + iBlankCellInMirrorMoving) * iSign + m_iMovingCellListLength) % m_iMovingCellListLength ) * m_SymbolSize.width + fDeltaX - ( fMoveUnit + bMeetSide*iSign) * m_SymbolSize.width);
			}
			else
			{
				m_MovingCellMirrorList[iColumn].m_pSprite->setVisible(false);
				m_MovingCellMirrorList[iColumn].m_pSprite->setPosition(m_MovingCellList[iColumn].m_pSprite->getPosition());
			}*/

			m_MovingCellList[iColumn].m_pSprite->setPositionX(
				m_fBoardLeftPosition + ((iColumn + iTranslatedOriginCell) % m_iMovingCellListLength ) * m_SymbolSize.width + fDeltaX - fMoveUnit * m_SymbolSize.width);
			
			if (iTranslatedMirrorCell != iTranslatedOriginCell)
			{
				m_MovingCellMirrorList[iColumn].m_pSprite->setVisible(true);
				m_MovingCellMirrorList[iColumn].m_pSprite->setPositionX(
					m_fBoardLeftPosition + ((iColumn + iTranslatedMirrorCell) % m_iMovingCellListLength ) * m_SymbolSize.width + fDeltaX - (fMoveUnit + bMeetSide * 1) * m_SymbolSize.width);
			}
			else
			{
				m_MovingCellMirrorList[iColumn].m_pSprite->setVisible(false);
				m_MovingCellMirrorList[iColumn].m_pSprite->setPosition(m_MovingCellList[iColumn].m_pSprite->getPosition());
			}
		}
	}


	// draw hint
	fMoveUnit = fDeltaX/m_SymbolSize.width;
				
	fMoveUnit = round(fMoveUnit);

	iMoveUnit = (int)fMoveUnit;

	if ( iMoveUnit == m_iSaveLastCellMoveDelta || iMoveUnit == 0 )
	{		
		if ( iMoveUnit == 0  && iMoveUnit != m_iSaveLastCellMoveDelta && m_HintSprites.size() >0 )
		{
			for(auto sprite:m_HintSprites)
				sprite->removeFromParentAndCleanup(true);
		
			m_HintSprites.clear();
		}
		return;
	}

	if ( m_HintSprites.size() >0 )
	{
		for(auto sprite:m_HintSprites)
			sprite->removeFromParentAndCleanup(true);
		
		m_HintSprites.clear();
	}

	
	m_iSaveLastCellMoveDelta = iMoveUnit;

	int iNumberOfColumn = m_GameBoardManager.GetColumnNumber();	
	int iTranslationCell = 0;
	
	m_ComputeMoveResult.Reset(false);
	if ( m_GameBoardManager.FastCheckBlocks( m_SelectedCell.m_iRow, -1, -1, iMoveUnit, m_ComputeMoveResult.m_BasicMatchingDestroyedCells))
	{		
		// NOTE: because we need to get effective cells from target positions, we need to reverse sign
		// and for easier computation, we convert it to active value
		iMoveUnit =  m_iMovingCellListLength - iBlankCellOfFullLine - iMoveUnit;
		if (iMoveUnit < 0)
			iMoveUnit = 0;

		m_pMoveHintNode = Sprite::createWithSpriteFrameName("Gem_A.png");
		m_pMoveHintNode->setOpacity(0);
		m_pBoardBatchNode->addChild(m_pMoveHintNode);

		for(auto cell : m_ComputeMoveResult.m_BasicMatchingDestroyedCells)
		{
			Sprite* pHintSprite = Sprite::createWithSpriteFrameName("Orange_Border.png");
			
			Size size = m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->getContentSize();
			if (cell.m_iRow != m_SelectedCell.m_iRow)
				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->addChild(pHintSprite);
			else
			{
				iTranslationCell = 0;
				for(int iStep=1; iStep <= abs(iMoveUnit); iStep++)
				{
					iTranslationCell += 1;

					if (m_GameBoardManager.IsBlankCell( m_SelectedCell.m_iRow, (cell.m_iColumn + iTranslationCell) % iNumberOfColumn))
					{
						iStep--;
					}
				}

				m_BoardViewMatrix[cell.m_iRow][ (cell.m_iColumn + iTranslationCell) % iNumberOfColumn ].m_pSprite->addChild(pHintSprite);
			}

			pHintSprite->setPosition( Point(size.width/2.f, size.height/2.f ));
			
			m_HintSprites.push_back(pHintSprite);			
		}

	}
}

void HelloWorld::VerticalMoveUlti(float fDeltaY)
{
	int iBlankCellOfFullLine = GetUnmoveCellsFromCurrentMovingRowOrColumn();

	if (fabs(fDeltaY) > (m_iMovingCellListLength - iBlankCellOfFullLine)* m_SymbolSize.height)
		fDeltaY = fDeltaY/fabs(fDeltaY) * (m_iMovingCellListLength - iBlankCellOfFullLine)* m_SymbolSize.height;

	// convert negative delta to active value for easier computation
	if (fDeltaY < 0)
		fDeltaY += (m_iMovingCellListLength - iBlankCellOfFullLine)* m_SymbolSize.height;		


	float fMoveUnit = fDeltaY/m_SymbolSize.height;	
	fMoveUnit = floorf(fMoveUnit);	
		
	int iMoveUnit = (int)fMoveUnit; //abs((int)fMoveUnit);		
	//int iBlankCellInOriginMoving, iBlankCellInMirrorMoving;//blank cells on the moving path of origin and mirror
	int iTranslatedOriginCell, iTranslatedMirrorCell;

	if (iMoveUnit >= m_iMovingCellListLength)
		iMoveUnit -= m_iMovingCellListLength;

	for(int iRow = 0; iRow< m_iMovingCellListLength; iRow++)
	{
		//if (!m_GameBoardManager.IsBlankCell(iRow, m_SelectedCell.m_iColumn))
		if (m_MovingCellList[iRow].m_pSprite != NULL)
		{
			iTranslatedOriginCell = 0;
			iTranslatedMirrorCell = 0;
			for(int iStep = 1; iStep <= iMoveUnit; iStep++)
			{
				iTranslatedOriginCell += 1;				

				if (m_MovingCellList[(iRow + iTranslatedOriginCell) % m_iMovingCellListLength].m_pSprite == NULL)
				{
					iStep--;
				}
			}
			
			iTranslatedMirrorCell = iTranslatedOriginCell;	

			bool bMeetSide = false;

			if ((fDeltaY>0 && fDeltaY > fMoveUnit * m_SymbolSize.height) || (fDeltaY<0 && fDeltaY < fMoveUnit * m_SymbolSize.height))
			{
				while ((m_MovingCellList[(iRow + iTranslatedMirrorCell + 1 + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite == NULL)
						||  (iRow + iTranslatedMirrorCell +  1 == m_iMovingCellListLength))
				{
					if (iRow + iTranslatedMirrorCell +  1 == m_iMovingCellListLength)
						bMeetSide = true;	

					iTranslatedMirrorCell += 1;

					if (m_MovingCellList[(iRow + iTranslatedMirrorCell ) % m_iMovingCellListLength].m_pSprite != NULL)						
						break;
					else if (bMeetSide)
					{
						//if (iSign < 0)
							//iTranslatedMirrorCell += iSign;						
						//if (iSign > 0)
							//bMeetSide = false;
						if (m_MovingCellList[(iRow + iTranslatedMirrorCell) % m_iMovingCellListLength].m_pSprite == NULL)
							bMeetSide = false;
					}
				}
			}

			m_MovingCellList[iRow].m_pSprite->setPositionY(
				m_fBoardBottomPosition + ((iRow +  iTranslatedOriginCell) % m_iMovingCellListLength ) * m_SymbolSize.height + fDeltaY - fMoveUnit  * m_SymbolSize.height);
			
			if (iTranslatedMirrorCell != iTranslatedOriginCell)
			{
				m_MovingCellMirrorList[iRow].m_pSprite->setVisible(true);
				m_MovingCellMirrorList[iRow].m_pSprite->setPositionY(
					m_fBoardBottomPosition + ((iRow + iTranslatedMirrorCell) % m_iMovingCellListLength ) * m_SymbolSize.height + fDeltaY - ( fMoveUnit + bMeetSide*1) * m_SymbolSize.height);
			}
			else
			{
				m_MovingCellMirrorList[iRow].m_pSprite->setVisible(false);
				m_MovingCellMirrorList[iRow].m_pSprite->setPosition(m_MovingCellList[iRow].m_pSprite->getPosition());
			}	
		}
	}



	// draw hint
	fMoveUnit = fDeltaY/m_SymbolSize.height;
				
	fMoveUnit = round(fMoveUnit);

	iMoveUnit = (int)fMoveUnit;
	if ( iMoveUnit == m_iSaveLastCellMoveDelta || iMoveUnit == 0 )
	{		
		if ( iMoveUnit == 0  && iMoveUnit != m_iSaveLastCellMoveDelta && m_HintSprites.size() >0 )
		{
			for(auto sprite:m_HintSprites)
				sprite->removeFromParentAndCleanup(true);
		
			m_HintSprites.clear();
		}
		return;
	}

	if ( m_HintSprites.size() >0 )
	{
		for(auto sprite:m_HintSprites)
			sprite->removeFromParentAndCleanup(true);
		
		m_HintSprites.clear();
	}

	
	m_iSaveLastCellMoveDelta = iMoveUnit;

	int iNumberOfRow = m_GameBoardManager.GetRowNumber();
	//iSign = 1;
	//if (iMoveUnit != 0)
		///iSign = -iMoveUnit/ abs(iMoveUnit);
	int iTranslationCell = 0;
	
	m_ComputeMoveResult.Reset(false);
	if ( m_GameBoardManager.FastCheckBlocks( -1, m_SelectedCell.m_iColumn, iMoveUnit, -1, m_ComputeMoveResult.m_BasicMatchingDestroyedCells))
	{		
		// NOTE: because we need to get effective cells from target positions, we need to reverse sign
		// and for easier computation, we convert it to active value
		iMoveUnit =  m_iMovingCellListLength - iBlankCellOfFullLine - iMoveUnit;
		if (iMoveUnit < 0)
			iMoveUnit = 0;

		m_pMoveHintNode = Sprite::createWithSpriteFrameName("Gem_A.png");
		m_pMoveHintNode->setOpacity(0);
		m_pBoardBatchNode->addChild(m_pMoveHintNode);

		for(auto cell : m_ComputeMoveResult.m_BasicMatchingDestroyedCells)
		{
			Sprite* pHintSprite = Sprite::createWithSpriteFrameName("Orange_Border.png");
			//pHintSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, m_fBoardBottomPosition + cell.m_iRow  * m_SymbolSize.height));
			Size size = m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->getContentSize();
			if (cell.m_iColumn != m_SelectedCell.m_iColumn)
				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->addChild(pHintSprite);
			else
			{
				iTranslationCell = 0;
				for(int iStep=1; iStep <= abs(iMoveUnit); iStep++)
				{
					iTranslationCell += 1;					

					if (m_GameBoardManager.IsBlankCell( (cell.m_iRow + iTranslationCell) % iNumberOfRow,  m_SelectedCell.m_iColumn))
					{
						iStep--;
					}
				}

				m_BoardViewMatrix[ (cell.m_iRow+ iTranslationCell) % iNumberOfRow][cell.m_iColumn].m_pSprite->addChild(pHintSprite);
			}

			pHintSprite->setPosition( Point(size.width/2.f, size.height/2.f ));
			
			m_HintSprites.push_back(pHintSprite);
		}

	}
}

void HelloWorld::PlayEffect2( const bool& bIsBonusEndGamePhase,  std::vector<ComboEffectCell>& convertedToComboCells, 
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboCreationInfo> doubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList,  std::vector<ComboEffectBundle*>& triggeredCombo5ChainList,
		std::vector<ComboEffectCell>& newComboCells, std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,		
		std::vector<NewCellInfo>& unlockedLetterCells, std::vector<NewCellInfo>& newCells, bool bIsNewMove)
{
	// for test	
	for(int i=0; i< _COMBO_TYPE_COUNT_;i++)
		m_pComboCountRenderNode->UpdateList(i, m_GameBoardManager.GetComboCount(i));

	m_bIsEffectPlaying = true;	

	float fDelayTime = 0.2f;
	float fDestroyTime = 0.35f;
	float fMoveTime = 0.3f; //25f;
	float fDelayPerNormalComboChain = fDestroyTime;
	float fDelayPerCombo5Chain = 0.45f;
	
	int iNumberOfRow = m_GameBoardManager.GetRowNumber();
	int iNumberOfColumn = m_GameBoardManager.GetColumnNumber();

	//if (bIsBonusEndGamePhase)
		//fDelayTime = 0;

	float fTotalDestroyCellTime = fDestroyTime;

	//CCLOG("Begin effect");

	if (bIsBonusEndGamePhase)
	{
		// play effect convert normal cells to combo cells
		if (convertedToComboCells.size() > 0)
		{
			float fDelayPerConvertedCell = 0.1f;
			auto pCombo5AnimBolt = AnimationCache::getInstance()->getAnimation("effectCombo5_Bolt");		
			Point rootEffect( 530.f, 820.f);
			int iIndex = 0;

			for(auto cell: convertedToComboCells)
			{
				BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime + (iIndex+1) * fDelayPerConvertedCell,fDestroyTime);

				// create combo cell
				Sprite* pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(cell.m_iGemID, cell.m_eGemComboType).c_str());
				//pSprite->setAnchorPoint(ccp(0,0));
				Point pos(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height);
				pSprite->setPosition(pos);

				//pSprite->setScale(0.65f);
				m_pBoardBatchNode->addChild(pSprite);

				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;

				pSprite->setOpacity(0);
				pSprite->runAction( 
					Sequence::create( 
						DelayTime::create(fDelayTime  + (iIndex+1) * fDelayPerConvertedCell),
						EaseIn::create( FadeIn::create(fDestroyTime), 2.f),
						NULL));

				pSprite->setScale(2.f);//1.5f);
				pSprite->runAction( 
					Sequence::create( 
						DelayTime::create(fDelayTime  + (iIndex+1) * fDelayPerConvertedCell),
						EaseIn::create(ScaleTo::create(fDestroyTime, 1.f, 1.f), 2.f),
						NULL));					


				// bolt effect
				Point vector( pos.x - rootEffect.x, pos.y - rootEffect.y);
				auto fAngle = atan2f( vector.y, vector.x);
				float fDistance = sqrtf( vector.x*vector.x + vector.y*vector.y);

				auto pComboEffectSprite = Sprite::createWithSpriteFrameName("Bolt 2_00000_1.png");
				pComboEffectSprite->setAnchorPoint  (Point( 0, 0.5f));
				pComboEffectSprite->setPosition(rootEffect);
				pComboEffectSprite->setRotation(-fAngle * 180.f / M_PI);
				pComboEffectSprite->setScaleX( fDistance / pComboEffectSprite->getContentSize().width);
				pComboEffectSprite->setScaleY(1.5f);

				m_pComboEffectBatchNode->addChild(pComboEffectSprite);
				pComboEffectSprite->runAction( 
					Sequence::create( 
						DelayTime::create(fDelayTime + iIndex * fDelayPerConvertedCell),
						Animate::create( pCombo5AnimBolt),
						RemoveSelf::create(),
						NULL));
				
				iIndex++;
			}

			fDelayTime += fDestroyTime + (iIndex+3) * fDelayPerConvertedCell;			
		}
	}


	// play sound
	if (basicMatchingDestroyedCells.size() > 0)
		SoundManager::PlaySoundEffect(_SET_SIMPLE_MATCH_);

	//CCLOG("Destroy cells");

	// destroy cells by basic matching 
	for(auto cell: basicMatchingDestroyedCells)
	{			
		if (cell.m_iRow >= 0 && cell.m_iColumn >= 0)
			BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime,fDestroyTime);
		else
		{
			cell.m_iRow = 0;
		}

		/*m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->runAction(
			Sequence::create( 
				DelayTime::create(fDelayTime),
				ScaleTo::create( fDestroyTime, 0.05f, 0.05f),
				RemoveSelf::create( true),
				//FadeOut::create(0.01f),
				NULL));

		if (m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iCharacterID >= 0)
		{
			m_pWordCollectBoardRenderNode->UnlockCharacter(m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iCharacterID);
		}

		//m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->runAction(
			//Sequence::create(
				//DelayTime::create(fDelayTime + fDestroyTime),
				//RemoveSelf::create( true),
				////FadeOut::create(0.01f),
				//NULL));

		m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = NULL;
		m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iCharacterID = -1;
		m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = NULL;*/
	}

	// increase score by basic matching
	m_GameBoardManager.IncreaseScoreForDestroyCells( basicMatchingDestroyedCells.size(), _CET_BASIC_MATCHING_);
	
	
	// destroy cells and create double combos
	for(auto doubleCombo : doubleComboList)
	{
		// play sound
		SoundManager::PlaySoundEffect(_SET_COMBINE_DOUBLE_COMBO_, fDelayTime);

		// destroy cells
		BasicDestroyCellUlti( doubleCombo.m_Cell1.m_iRow, doubleCombo.m_Cell1.m_iColumn, fDelayTime,fDestroyTime);
		BasicDestroyCellUlti( doubleCombo.m_Cell2.m_iRow, doubleCombo.m_Cell2.m_iColumn, fDelayTime,fDestroyTime);

		// create double combo cell
		for(int iFlag =0; iFlag < 2; iFlag++)
		{
			Sprite* pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID( 0, doubleCombo.m_eComboType).c_str());			

			pSprite->setPosition( ccp(m_fBoardLeftPosition + doubleCombo.m_Position.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + doubleCombo.m_Position.m_iRow * m_SymbolSize.height));

			//pSprite->setScale(0.65f);
			m_pBoardBatchNode->addChild(pSprite);

			if (iFlag == 0)
			{
				m_BoardViewMatrix[doubleCombo.m_Position.m_iRow][doubleCombo.m_Position.m_iColumn].m_pSprite = pSprite;

				pSprite->setOpacity(0);
				pSprite->runAction( 
					Sequence::create( 
						DelayTime::create(fDelayTime),
						EaseIn::create( FadeIn::create(fDestroyTime - 0.05f), 2.f),
						NULL));

				pSprite->setScale(1.5f);
				pSprite->runAction( 
					Sequence::create( 
						DelayTime::create(fDelayTime),
						EaseIn::create(ScaleTo::create(fDestroyTime - 0.05f, 1.f, 1.f), 2.f),
						NULL));					
			}
			else
			{
				m_BoardViewMirrorMatrix[doubleCombo.m_Position.m_iRow][doubleCombo.m_Position.m_iColumn].m_pSprite = pSprite;
				pSprite->setVisible(false);
			}
		}
	}

	//CCLOG("Combo chain");
		
	// play chain of triggered combo 5
	if (triggeredCombo5ChainList.size() > 0)
	{		
		for(auto pComboEffect : triggeredCombo5ChainList)
		{
			PlayCombo5Effect(pComboEffect, fDelayTime + pComboEffect->m_iActivatedByCombo5Phase * fDelayPerCombo5Chain, fDelayPerCombo5Chain);

			// destroy cells by combo
			for(auto cell: pComboEffect->m_DestroyedCells)
			{
				BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime + (pComboEffect->m_iActivatedByCombo5Phase + 1)* fDelayPerCombo5Chain - 0.02f, fDestroyTime);			
			}

			// increase score by combo		
			m_GameBoardManager.IncreaseScoreForDestroyCells( basicMatchingDestroyedCells.size(), pComboEffect->m_ComboEffectDescription.m_eComboEffectType);

			// clean data
			delete pComboEffect;
		}

		fTotalDestroyCellTime = fDelayTime + fDelayPerCombo5Chain * triggeredCombo5ChainList.size();
	}

	// play combo chain	
	float fCurrentDelayComboChain;
	for(auto pComboEffect : comboChainList)
	{
		fCurrentDelayComboChain = fDelayTime + pComboEffect->m_iActivatedByCombo5Phase * fDelayPerCombo5Chain + pComboEffect->m_iNormalChainPhase* fDelayPerNormalComboChain;
		if (fCurrentDelayComboChain + fDestroyTime > fTotalDestroyCellTime)
			fTotalDestroyCellTime = fCurrentDelayComboChain + fDestroyTime;

		// play effect
		if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_EXPLOSION_)
			PlayCombo4Effect(pComboEffect, fCurrentDelayComboChain, fDestroyTime);
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLOR_)
		{
			if (pComboEffect->m_iNormalChainPhase < 0)
			{
				if (m_BoardViewMatrix[ pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite != NULL)
				{
					auto flashLightEffect = RepeatForever::create(
						Sequence::create(
							DelayTime::create( fCurrentDelayComboChain),
							TintTo::create( 0.2f, 100, 200, 100),
							TintTo::create( 0.2f, 255, 255, 155),
							NULL));
					flashLightEffect->setTag(-100);

					m_BoardViewMatrix[ pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite->runAction(flashLightEffect);
				}
			}
			//else
			//	PlayCombo5Effect(pComboEffect, fDelayTime + pComboEffect->m_iPhase* fDestroyTime, fDestroyTime);
		}
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_DOUBLE_EXPLOSION_)
		{
			Sprite* pComboEffectSprite;

			// play sound
			SoundManager::PlaySoundEffect(_SET_DOUBLE_COMPLE_EFFECT_, fCurrentDelayComboChain);

			pComboEffectSprite = Sprite::createWithSpriteFrameName("Explosion2.png");		
			pComboEffectSprite->setScale(2.17f);

			//destroy temporary double combo cell too
			BasicDestroyCellUlti( pComboEffect->m_ComboEffectDescription.m_Position.m_iRow, pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn,
				fCurrentDelayComboChain, fDestroyTime);

			pComboEffectSprite->setPosition( Point(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height));

			m_pBoardBatchNode->addChild(pComboEffectSprite);

			pComboEffectSprite->setOpacity(0);
			pComboEffectSprite->runAction(Sequence::create( 
					DelayTime::create(fCurrentDelayComboChain),
					CallFuncN::create( this, callfuncN_selector( HelloWorld::ActivateImageEffect)),
					EaseOut::create( FadeOut::create( fDestroyTime), 2.f),				
					RemoveSelf::create( true),				
					NULL));		
		}

		// ******************************
		/*
		Sprite* pComboEffectSprite;

		if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_DOUBLE_EXPLOSION_)
		{
			// play sound
			SoundManager::PlaySoundEffect(_SET_DOUBLE_COMPLE_EFFECT_, fDelayTime + pComboEffect->m_iPhase* fDestroyTime);

			pComboEffectSprite = Sprite::createWithSpriteFrameName("Explosion2.png");		
			pComboEffectSprite->setScale(2.17f);

			//destroy temporary double combo cell too
			BasicDestroyCellUlti( pComboEffect->m_ComboEffectDescription.m_Position.m_iRow, pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn,
				fDelayTime + pComboEffect->m_iPhase* fDestroyTime, fDestroyTime);
		}
		else
		{
			// play sound
			SoundManager::PlaySoundEffect(_SET_SIMPLE_COMBO_, fDelayTime + pComboEffect->m_iPhase* fDestroyTime);

			pComboEffectSprite = Sprite::createWithSpriteFrameName("Explosion.png");
			pComboEffectSprite->setScale(1.3f);
		}

		pComboEffectSprite->setPosition( Point(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height));

		m_pBoardBatchNode->addChild(pComboEffectSprite);

		pComboEffectSprite->setOpacity(0);
		pComboEffectSprite->runAction(Sequence::create( 
				DelayTime::create(fDelayTime + pComboEffect->m_iPhase* fDestroyTime),
				CallFuncN::create( this, callfuncN_selector( HelloWorld::ActivateImageEffect)),
				EaseOut::create( FadeOut::create( fDestroyTime), 2.f),				
				RemoveSelf::create( true),				
				NULL));		
		*/


		// destroy cells by combo
		for(auto cell: pComboEffect->m_DestroyedCells)
		{
			BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fCurrentDelayComboChain, fDestroyTime);
			/*m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->runAction(
				Sequence::create( 
					DelayTime::create(fDelayTime + pComboEffect->m_iPhase* fDestroyTime),
					FadeOut::create( fDestroyTime),
					//ScaleTo::create( fDestroyTime, 0.05f, 0.05f),
					RemoveSelf::create( true),
					//FadeOut::create(0.01f),
					NULL));

			if (m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iCharacterID >= 0)
			{
				m_pWordCollectBoardRenderNode->UnlockCharacter(m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iCharacterID);
			}

			//m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->runAction(
				//Sequence::create(
					//DelayTime::create(fDelayTime + fDestroyTime),
					//RemoveSelf::create( true),
					////FadeOut::create(0.01f),
					//NULL));

			m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = NULL;
			m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iCharacterID = -1;
			m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = NULL;*/
		}

		// increase score by combo		
		m_GameBoardManager.IncreaseScoreForDestroyCells( basicMatchingDestroyedCells.size(), pComboEffect->m_ComboEffectDescription.m_eComboEffectType);

		// clean data
		delete pComboEffect;
	}


	// create new combo cells
	for(auto cell: newComboCells)
	{
		for(int i=0; i< 2; i++)
		{
			Sprite* pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(cell.m_iGemID, cell.m_eGemComboType).c_str());
			//pSprite->setAnchorPoint(ccp(0,0));

			pSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height));

			//pSprite->setScale(0.65f);
			m_pBoardBatchNode->addChild(pSprite);

			if (i!= 0)
			{			
				pSprite->setVisible(false);
				m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
				
			}
			else
			{
				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;				

				pSprite->setOpacity(0);
				pSprite->runAction( 
					Sequence::create( 
						DelayTime::create(fDelayTime),
						EaseIn::create( FadeIn::create(fDestroyTime), 2.f),
						NULL));

				pSprite->setScale(2.f);//1.5f);
				pSprite->runAction( 
					Sequence::create( 
						DelayTime::create(fDelayTime),
						EaseIn::create(ScaleTo::create(fDestroyTime, 1.f, 1.f), 2.f),
						NULL));			
				
				//
				/*Sprite* pLetterSprite = GenerateAndAddLetterToComboGem( cell, fDelayTime);
				if (pLetterSprite != NULL)
				{
					pLetterSprite->setOpacity(0);
					pLetterSprite->runAction( 
						Sequence::create( 
							DelayTime::create(fDelayTime),
							EaseIn::create( FadeIn::create(fDestroyTime), 2.f),
							NULL));
					
				}*/
			}			
		}
	}

	// create gem with unlock letters
	GemLetterData data;
	Sprite* pSprite;
	for(auto& cell : unlockedLetterCells)
	{
		for(int i=0; i< 2; i++)
		{					
			/*Sprite* pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID( cell.m_iGemID,  //m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), //_GCT_NONE_).c_str());
						_GCT_HAS_LETTER_).c_str());
			
			//pSprite->setAnchorPoint(ccp(0,0));
			//pSprite->setColor(ccc3( 255-(m_iMoveCount+1)*5,  255, 255));

			pSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height));

			if (i!= 0)
			{				
				pSprite->setVisible(false);
				m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
			}
			else
			{
				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iGemLetterBlockID = -1; //reset letter of gem				
			}
			//pSprite->setScale(0.65f);*/

			if (i!= 0)
			{								
				pSprite = m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite;
			}
			else
			{
				pSprite = m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite;				
			}

			if (cell.m_bIsUnlocked)
			{
				if (cell.m_iGemLetterBlockID >= 0) //iLetter < 255)
				{
					data = m_GameBoardManager.GetGemLetterData(cell.m_iGemLetterBlockID);							
					AddLetterToGem( cell, m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), data.m_iLetter, cell.m_iGemLetterBlockID);			
				}
			}
					
			//m_pBoardBatchNode->addChild(pSprite);						
		}		
	}

	//CCLOG("Move gems");
	// move cells
	for (int i=0; i < originalMovedCells.size(); i++)
	{		
		//if ( m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn].m_pSprite == NULL)
		//	CCLOG("3- Effect NULL, %d, %d", originalMovedCells[i].m_iRow, originalMovedCells[i].m_iColumn);
		m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn].m_pSprite->runAction(
			Sequence::create( 
				DelayTime::create(fTotalDestroyCellTime + 0.027f * originalMovedCells[i].m_iRow), // + 0.01f * (targetMovedCells[i].m_iRow - originalMovedCells[i].m_iRow ),
				EaseOut::create( MoveTo::create( fMoveTime,
					ccp(m_fBoardLeftPosition + targetMovedCells[i].m_iColumn * m_SymbolSize.width, 
					m_fBoardBottomPosition + targetMovedCells[i].m_iRow * m_SymbolSize.height)),			1.f),
				NULL));


		m_BoardViewMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn] = m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn];		
		
		m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn] = m_BoardViewMirrorMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn];		
		
		//m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite->setVisible(false);
		//UpdatePostionOfSprite(targetMovedCells[i].m_iRow, targetMovedCells[i].m_iColumn, true);		

		//if (m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite == NULL)
		//	CCLOG("3- Effect Mirror NULL");
	}	


	// generate new cells
	// play sound
	SoundManager::PlaySoundEffect(_SET_DROP_GEM_, fTotalDestroyCellTime);

	//CCLOG("Create new gems");

	//std::vector<unsigned char> outputLettersForGems;
	NewCellInfo cell;
	//unsigned char iLetter = 255;
	
	//m_GameBoardManager.GetGameWordManager()->GenerateNewLetters( newCells.size(), outputLettersForGems, bIsNewMove);		

	for(int iGemIndex=0; iGemIndex < newCells.size(); iGemIndex++)
	{
		//iLetter = (unsigned char)outputLettersForGems[iGemIndex];
		cell = newCells[iGemIndex];

		for(int i=0; i< 2; i++)
		{					
			Sprite* pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID( cell.m_iGemID,  //m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), //_GCT_NONE_).c_str());
						(cell.m_iGemLetterBlockID >= 0)?_GCT_HAS_LETTER_:_GCT_NONE_ ).c_str());
			
			//pSprite->setAnchorPoint(ccp(0,0));
			//pSprite->setColor(ccc3( 255-(m_iMoveCount+1)*5,  255, 255));
			if (i!= 0)
			{
				pSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height));

				pSprite->setVisible(false);
				m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
			}
			else
			{
				pSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, 
					m_fBoardBottomPosition + (cell.m_iRow + iNumberOfRow) * m_SymbolSize.height));
				pSprite->runAction(
					Sequence::create(
						DelayTime::create(fTotalDestroyCellTime + 0.027f* (m_GameBoardManager.GetRowNumber() - cell.m_iRow)),
						EaseOut::create( MoveTo::create(fMoveTime * 1.4f,
							ccp(m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, 
									m_fBoardBottomPosition + cell.m_iRow  * m_SymbolSize.height)), 1.f),
							NULL));

				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iGemLetterBlockID = -1; //reset letter of gem				
			}
			//pSprite->setScale(0.65f);

			/*if (cell.m_bIsUnlocked)
			{
				if (cell.m_iGemLetterBlockID >= 0) //iLetter < 255)
				{
					data = m_GameBoardManager.GetGemLetterData(cell.m_iGemLetterBlockID);							
					AddLetterToGem( cell, m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), data.m_iLetter);			
				}
			}*/
					
			m_pBoardBatchNode->addChild(pSprite);						
		}		
	}

	
	/*
	bool bFirstCell = true;	
	int iCharacterID;
	if (m_iMoveCount % 5 == 0)
		iCharacterID = m_pWordCollectBoardRenderNode->GetNextCharacterID();
	for(auto cell: newCells)
	{		
		for(int i=0; i< 2; i++)
		{		
			Sprite* pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn)).c_str());
			//pSprite->setAnchorPoint(ccp(0,0));
			//pSprite->setColor(ccc3( 255-(m_iMoveCount+1)*5,  255, 255));
			if (i!= 0)
			{
				pSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height));

				pSprite->setVisible(false);
				m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
			}
			else
			{
				pSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, 
					m_fBoardBottomPosition + (cell.m_iRow + iNumberOfRow*2) * m_SymbolSize.height));
				pSprite->runAction(
					Sequence::create(
						DelayTime::create(fDelayTime + fDestroyTime * (iMaxComboPhase+1)),
						MoveTo::create(fMoveTime,
							ccp(m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, 
									m_fBoardBottomPosition + cell.m_iRow  * m_SymbolSize.height)),
							NULL));

				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
			}
			//pSprite->setScale(0.65f);
					
			m_pBoardBatchNode->addChild(pSprite);
				
			if (m_iMoveCount % 5 == 0 && bFirstCell && iCharacterID >= 0)
			{				
				Sprite* pCharacterSprite = Sprite::createWithSpriteFrameName(
					m_pWordCollectBoardRenderNode->GetImageFileFromCharacterID(iCharacterID).c_str());
				
				pCharacterSprite->setScale(0.5f);
				//pCharacterSprite->setAnchorPoint(ccp(0,0));
				pCharacterSprite->setPosition(ccp( 25.f, 25.f));

				pSprite->addChild(pCharacterSprite);

				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iCharacterID = iCharacterID;
			}
			else if (i == 0)
			{
				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iCharacterID = -1;
			}

			if (bFirstCell)
				bFirstCell = false;
			
		}		
	}
	*/
	

	if (bIsBonusEndGamePhase)
	{
		this->runAction( CCSequence::create(
					CCDelayTime::create(fTotalDestroyCellTime+ fMoveTime *4),
					CCCallFunc::create( this, callfunc_selector( HelloWorld::ExecuteBonusWinGameEffect)),
					NULL));		
	}
	else
	{
		// 
		if (bIsNewMove)
		{
			m_GameBoardManager.DecreaseMove();		
			m_GameBoardManager.GetGameWordManager()->UpdateParamForNewMove();
		}
	
		this->runAction( CCSequence::create(
						CCDelayTime::create(fTotalDestroyCellTime + fMoveTime + fMoveTime),
						CCCallFunc::create( this, callfunc_selector( HelloWorld::CheckBoardStateAfterMove)),
						NULL));		
	}

	// update display of score/move 
	m_pStatusLayer->setCurrentMove( m_GameBoardManager.GetCurrentMove());
	m_pStatusLayer->setCurrentScore( m_GameBoardManager.GetCurrentScore());
	//m_pStatusLayer->update(0);


	// ************** check obstacle
	UpdateObstacleListAfterMove();
}

void HelloWorld::UpdateObstacleListAfterMove()
{
	int iObstacleTypeCount = GameConfigManager::getInstance()->GetObstacleTypeCount();
	int iNumberOfRow = m_GameBoardManager.GetRowNumber();
	int iNumberOfColumn = m_GameBoardManager.GetColumnNumber();

	int iRow, iColumn, iBlockID;
	ObstacleProcessManager*	pObstacleProcessManager = m_GameBoardManager.GetObstacleProcessManager();
	for(iRow =0; iRow < iNumberOfRow; iRow++)
		for(iColumn =0; iColumn < iNumberOfColumn; iColumn++)
		{
			iBlockID = m_GameBoardManager.GetObstacleBlockID(iRow, iColumn);
			if (iBlockID < 0)
				continue;

			for(int iObstacleTypeID = 0; iObstacleTypeID < iObstacleTypeCount; iObstacleTypeID++)
			{
				ObstacleData& obstacleData = pObstacleProcessManager->GetObstacleData(iBlockID, iObstacleTypeID);
				if (obstacleData.m_bIsDirty)
				{
					if (m_BoardObstaclesList[iBlockID][iObstacleTypeID] != NULL)
					{										
						m_BoardObstaclesList[iBlockID][iObstacleTypeID]->removeFromParentAndCleanup(true);
						m_BoardObstaclesList[iBlockID][iObstacleTypeID] = NULL;
					}											

					if (obstacleData.m_bIsActive)
					{
						Sprite* pMaskSprite = 
							Sprite::createWithSpriteFrameName( GameConfigManager::getInstance()->GetObstacleLevelDescription(iObstacleTypeID, obstacleData.m_iObstacleLevel).m_sSpriteFileName.c_str()); //"Lock.png");														
						pMaskSprite->setAnchorPoint(Point(0,0));
						m_BoardViewMatrix[iRow][iColumn].m_pSprite->addChild(pMaskSprite, 10);

						if (obstacleData.m_iObstacleLevel > 0)
							m_BoardObstaclesList[iBlockID][iObstacleTypeID] = pMaskSprite;
					}
					obstacleData.m_bIsDirty = false;						
				}
			}
		}
}

void HelloWorld::ActivateImageEffect(Node* pSender)
{
	((Sprite*)pSender)->setOpacity(180);
}

void HelloWorld::BasicDestroyCellUlti(const int& iRow, const int & iColumn, const float& fDelay, const float& fEffectDuration)
{
	// NOTE: following code to hot fix error cause by cells destroy by double combo and other effect!!!
	if (m_BoardViewMatrix[iRow][iColumn].m_pSprite == NULL)
	{
		m_BoardViewMatrix[iRow][iColumn].m_iGemLetterBlockID = -1;
		return;
	}

	if (m_BoardViewMatrix[iRow][iColumn].m_pSprite->getActionByTag(-100) != NULL)
	{
		m_BoardViewMatrix[iRow][iColumn].m_pSprite->stopAllActions();
	}

	m_BoardViewMatrix[iRow][iColumn].m_pSprite->runAction(
		Sequence::create( 
			DelayTime::create(fDelay),			
			ScaleTo::create( fEffectDuration, 0.05f, 0.05f),
			RemoveSelf::create( true),
			//FadeOut::create(0.01f),
			NULL));

	if (m_BoardViewMatrix[iRow][iColumn].m_iGemLetterBlockID >= 0)  //m_iLetter < 255)
	{	
		GemLetterData gemLetterData = m_GameBoardManager.FreeGemLetterBlock(m_BoardViewMatrix[iRow][iColumn].m_iGemLetterBlockID);


		m_pWordCollectBoardRenderNode->PlayUnlockLetterEffect(fDelay, gemLetterData.m_iLetter,  //m_BoardViewMatrix[iRow][iColumn].m_iLetter, 
			ccp(m_fBoardLeftPosition + iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + iRow * m_SymbolSize.height));
		
		m_pWordCollectBoardRenderNode->PlayCharacterAnim(2, false);


		int iUnlockedLetterIndexOfMainWord;
		std::vector<int> unlockedLettersIndexOfSubWords[_GDS_SUB_WORD_MAX_COUNT_];
		bool bIsMainWordJustUnlocked;
		bool justUnlockedSubWords[_GDS_SUB_WORD_MAX_COUNT_];

		if (m_GameBoardManager.GetGameWordManager()->UnlockLetter( gemLetterData.m_iLetter, gemLetterData.m_bIsInMainWord, iUnlockedLetterIndexOfMainWord, unlockedLettersIndexOfSubWords,
																	bIsMainWordJustUnlocked, justUnlockedSubWords))
		{
			m_pBonusWordNode->addLetter(gemLetterData.m_iLetter);

			if (iUnlockedLetterIndexOfMainWord >=0)
				m_pWordCollectBoardRenderNode->UnlockLetter(iUnlockedLetterIndexOfMainWord);
				//m_pWordCollectBoardRenderNode->UnlockCharacter(fDelay, iUnlockedLetterIndexOfMainWord);

			// increase score
			if (iUnlockedLetterIndexOfMainWord >=0)
				m_GameBoardManager.IncreaseScoreForLetterInMainWord();

			int iUnlockedLettersOfSubWordCount = 0;
			for(auto letters : unlockedLettersIndexOfSubWords)
				iUnlockedLettersOfSubWordCount += letters.size();
			m_GameBoardManager.IncreaseScoreForLetterInSubWords(iUnlockedLettersOfSubWordCount);

			if (bIsMainWordJustUnlocked)
				m_GameBoardManager.IncreaseScoreForUnlockMainWord();
			
			for(int i=0; i< _GDS_SUB_WORD_MAX_COUNT_; i++)
				if (justUnlockedSubWords[i])
				{
					m_GameBoardManager.IncreaseScoreForUnlockSubWord(i);
				}			

			// play sound effect 
			SoundManager::PlaySoundEffect(_SET_GET_CHARACTER_);
		}							
	}

	
	if ( m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite != NULL)
	{
		m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite->runAction(
			Sequence::create(
				DelayTime::create(fDelay),
				RemoveSelf::create( true),
				//FadeOut::create(0.01f),
				NULL));

		m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite = NULL;
	}

	m_BoardViewMatrix[iRow][iColumn].m_pSprite = NULL;
	m_BoardViewMatrix[iRow][iColumn].m_iGemLetterBlockID = -1;
	m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite = NULL;	
}

/*Sprite* HelloWorld::GenerateAndAddLetterToComboGem(const ComboEffectCell& cell, const float& fDelayTime)
{
	unsigned char iLetter;
	if (m_GameBoardManager.GetGameWordManager()->GenerateNewLetter( iLetter, cell.m_eGemComboType))	
	{	
		return AddLetterToGem(cell, cell.m_iGemID, iLetter, fDelayTime);		
	}
	else
	{
		m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iLetter = 255;//-1;

		return NULL;
	}
}

Sprite* HelloWorld::AddLetterToGem(const Cell& cell, const int& iGemID, const unsigned char& iLetter, const float& fDelayTime)
{
	Sprite* pCharacterSprite = Sprite::createWithSpriteFrameName(
			m_pWordCollectBoardRenderNode->GetImageInGemFileFromLetter(iLetter).c_str());
					
	pCharacterSprite->setPosition(ccp( 42.f, 42.f));

	switch(iGemID)
	{
		case 0: //Orange
			pCharacterSprite->setColor(ccc3(252, 234, 160));
			break;
		case 1: //red
			pCharacterSprite->setColor(ccc3(242, 209, 163));
			break;
		case 2: //pink
			pCharacterSprite->setColor(ccc3(242, 217, 179));
			break;
		case 3: //white
			pCharacterSprite->setColor(ccc3(148, 135, 102));
			break;
		case 4: //blue
			pCharacterSprite->setColor(ccc3(17, 215, 250));
			break;
		case 5: //green
			pCharacterSprite->setColor(ccc3(184, 212, 6));
			break;
	}


	m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->addChild(pCharacterSprite);

	m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iLetter = iLetter;

	// add effect to letter	
	pCharacterSprite->setScale(2.f);
	pCharacterSprite->runAction(
		Sequence::createWithTwoActions(
			DelayTime::create( fDelayTime),
			ScaleTo::create( 0.5f, 1.f, 1.f)));

	// add effect create character
	auto pEffect = Armature::create("Animation detach");
	pEffect->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);	
	pEffect->getAnimation()->playByIndex(0, -1, -1, 0);
	pEffect->setPosition(m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->getPosition());

	m_pHUDLayer->addChild(pEffect);
	//m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->addChild(pEffect);
	
	this->getActionManager()->addAction(
		Sequence::createWithTwoActions(
			DelayTime::create(pEffect->getAnimation()->getRawDuration()/60.f),
			RemoveSelf::create()), pEffect, false);
			//RemoveSelf::create()));

	return pCharacterSprite;
}
*/

Sprite* HelloWorld::AddLetterToGem(const Cell& cell, const int& iGemID, const unsigned char& iLetter, const int& iGemLetterBlockID)
{
	Sprite* pCharacterSprite = Sprite::createWithSpriteFrameName(
			m_pWordCollectBoardRenderNode->GetImageInGemFileFromLetter(iLetter).c_str());
					
	pCharacterSprite->setPosition(ccp( 42.f, 42.f));

	switch(iGemID)
	{
		case 0: //Orange
			pCharacterSprite->setColor(ccc3(252, 234, 160));
			break;
		case 1: //red
			pCharacterSprite->setColor(ccc3(242, 209, 163));
			break;
		case 2: //pink
			pCharacterSprite->setColor(ccc3(242, 217, 179));
			break;
		case 3: //white
			pCharacterSprite->setColor(ccc3(148, 135, 102));
			break;
		case 4: //blue
			pCharacterSprite->setColor(ccc3(17, 215, 250));
			break;
		case 5: //green
			pCharacterSprite->setColor(ccc3(184, 212, 6));
			break;
	}


	m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->addChild(pCharacterSprite);
	m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iGemLetterBlockID = iGemLetterBlockID; //m_iLetter = iLetter;

	return pCharacterSprite;
}

/*void HelloWorld::PlayBonusEndEffect( std::vector<ComboEffectCell>& convertedToComboCells,
	std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboEffectBundle> doubleComboList, 
	std::vector<ComboEffectBundle*>& comboChainList,std::vector<ComboEffectCell>& newComboCells,
	std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
	std::vector<Cell>& newCells)
{	
	m_bIsEffectPlaying = true;	

	float fDelayTime = 0.2f;
	float fDestroyTime = 0.25f;
	float fMoveTime = 0.24f;
	int iNumberOfRow = m_GameBoardManager.GetRowNumber();
	int iNumberOfColumn = m_GameBoardManager.GetColumnNumber();

	
	// play effect convert normal cells to combo cells
	if (convertedToComboCells.size() > 0)
	{
		for(auto cell: convertedToComboCells)
		{
			BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime,fDestroyTime);

			// create combo cell
			Sprite* pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(cell.m_iGemID, cell.m_eGemComboType).c_str());
			//pSprite->setAnchorPoint(ccp(0,0));

			pSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height));

			//pSprite->setScale(0.65f);
			m_pBoardBatchNode->addChild(pSprite);

			m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;

			pSprite->setOpacity(0);
			pSprite->runAction( 
				Sequence::create( 
					DelayTime::create(fDelayTime),
					EaseIn::create( FadeIn::create(fDestroyTime), 2.f),
					NULL));

			pSprite->setScale(2.f);//1.5f);
			pSprite->runAction( 
				Sequence::create( 
					DelayTime::create(fDelayTime),
					EaseIn::create(ScaleTo::create(fDestroyTime, 1.f, 1.f), 2.f),
					NULL));						
		}

		fDelayTime += fDestroyTime;
	}


	// play sound
	if (basicMatchingDestroyedCells.size() > 0)
		SoundManager::PlaySoundEffect(_SET_SIMPLE_MATCH_);	

	// destroy cells by basic matching 
	for(auto cell: basicMatchingDestroyedCells)
	{			
		BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime,fDestroyTime);		
	}

	// increase score by basic matching
	m_GameBoardManager.IncreaseScoreForDestroyCells( basicMatchingDestroyedCells.size(), _CET_BASIC_MATCHING_);
	

	// destroy cells and create temporary double combos?
	for(auto doubleCombo : doubleComboList)
	{
		// play sound
		SoundManager::PlaySoundEffect(_SET_COMBINE_DOUBLE_COMBO_, fDelayTime);

		// destroy cells
		BasicDestroyCellUlti( doubleCombo.m_CreationInfo.m_Cell1.m_iRow, doubleCombo.m_CreationInfo.m_Cell1.m_iColumn, fDelayTime,fDestroyTime);
		BasicDestroyCellUlti( doubleCombo.m_CreationInfo.m_Cell2.m_iRow, doubleCombo.m_CreationInfo.m_Cell2.m_iColumn, fDelayTime,fDestroyTime);

		// create temporary double combo
		Sprite* pSprite = Sprite::createWithSpriteFrameName("Candy_0006_Layer-19.png");

		pSprite->setPosition( ccp(m_fBoardLeftPosition + doubleCombo.m_CreationInfo.m_Position.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + doubleCombo.m_CreationInfo.m_Position.m_iRow * m_SymbolSize.height));

		pSprite->setScale(0.65f);
		m_pBoardBatchNode->addChild(pSprite);

		m_BoardViewMatrix[doubleCombo.m_CreationInfo.m_Position.m_iRow][doubleCombo.m_CreationInfo.m_Position.m_iColumn].m_pSprite = pSprite;

		pSprite->setOpacity(0);
		pSprite->runAction( 
			Sequence::create( 
				DelayTime::create(fDelayTime),
				EaseIn::create( FadeIn::create(fDestroyTime - 0.05f), 2.f),
				NULL));

		pSprite->setScale(1.5f);
		pSprite->runAction( 
			Sequence::create( 
				DelayTime::create(fDelayTime),
				EaseIn::create(ScaleTo::create(fDestroyTime - 0.05f, 0.65f, 0.65f), 2.f),
				NULL));					
	}

	CCLOG("Combo chain");
	
	// play combo chain
	int iMaxComboPhase = 0;
	for(auto pComboEffect : comboChainList)
	{
		if (iMaxComboPhase < pComboEffect->m_iPhase)
			iMaxComboPhase = pComboEffect->m_iPhase;

		// play effect
		Sprite* pComboEffectSprite;

		if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_DOUBLE_EXPLOSION_)
		{
			// play sound
			SoundManager::PlaySoundEffect(_SET_DOUBLE_COMPLE_EFFECT_, fDelayTime + pComboEffect->m_iPhase* fDestroyTime);

			pComboEffectSprite = Sprite::createWithSpriteFrameName("Explosion2.png");		
			pComboEffectSprite->setScale(2.17f);

			//destroy temporary double combo cell too
			BasicDestroyCellUlti( pComboEffect->m_ComboEffectDescription.m_Position.m_iRow, pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn,
				fDelayTime + pComboEffect->m_iPhase* fDestroyTime, fDestroyTime);
		}
		else
		{
			// play sound
			SoundManager::PlaySoundEffect(_SET_SIMPLE_COMBO_, fDelayTime + pComboEffect->m_iPhase* fDestroyTime);

			pComboEffectSprite = Sprite::createWithSpriteFrameName("Explosion.png");
			pComboEffectSprite->setScale(1.3f);
		}

		pComboEffectSprite->setPosition( Point(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height));

		m_pBoardBatchNode->addChild(pComboEffectSprite);

		pComboEffectSprite->setOpacity(0);
		pComboEffectSprite->runAction(Sequence::create( 
				DelayTime::create(fDelayTime + pComboEffect->m_iPhase* fDestroyTime),
				CallFuncN::create( this, callfuncN_selector( HelloWorld::ActivateImageEffect)),
				EaseOut::create( FadeOut::create( fDestroyTime), 2.f),				
				RemoveSelf::create( true),				
				NULL));		

		// destroy cells by combo
		for(auto cell: pComboEffect->m_DestroyedCells)
		{
			BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime + pComboEffect->m_iPhase* fDestroyTime, fDestroyTime);		
		}

		// increase score by combo		
		m_GameBoardManager.IncreaseScoreForDestroyCells( basicMatchingDestroyedCells.size(), pComboEffect->m_ComboEffectDescription.m_eComboEffectType);

		// clean data
		delete pComboEffect;
	}

	// create new combo cells
	for(auto cell: newComboCells)
	{
		for(int i=0; i< 2; i++)
		{
			Sprite* pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(cell.m_iGemID, cell.m_eGemComboType).c_str());
			//pSprite->setAnchorPoint(ccp(0,0));

			pSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height));

			//pSprite->setScale(0.65f);
			m_pBoardBatchNode->addChild(pSprite);

			if (i!= 0)
			{			
				pSprite->setVisible(false);
				m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
			}
			else
			{
				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;

				pSprite->setOpacity(0);
				pSprite->runAction( 
					Sequence::create( 
						DelayTime::create(fDelayTime),
						EaseIn::create( FadeIn::create(fDestroyTime), 2.f),
						NULL));

				pSprite->setScale(2.f);//1.5f);
				pSprite->runAction( 
					Sequence::create( 
						DelayTime::create(fDelayTime),
						EaseIn::create(ScaleTo::create(fDestroyTime, 1.f, 1.f), 2.f),
						NULL));			
			}
		}
	}

	CCLOG("Move gems");

	// move cells
	for (int i=0; i < originalMovedCells.size(); i++)
	{		
		//if ( m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn].m_pSprite == NULL)
		//	CCLOG("3- Effect NULL, %d, %d", originalMovedCells[i].m_iRow, originalMovedCells[i].m_iColumn);
		m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn].m_pSprite->runAction(
			Sequence::create( 
				DelayTime::create(fDelayTime + fDestroyTime * (iMaxComboPhase+1) + 0.027f * originalMovedCells[i].m_iRow), // + 0.01f * (targetMovedCells[i].m_iRow - originalMovedCells[i].m_iRow ),
				EaseOut::create( MoveTo::create( fMoveTime,
					ccp(m_fBoardLeftPosition + targetMovedCells[i].m_iColumn * m_SymbolSize.width, 
					m_fBoardBottomPosition + targetMovedCells[i].m_iRow * m_SymbolSize.height)),			1.f),
				NULL));				


		m_BoardViewMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn] = m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn];		
		
		m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn] = m_BoardViewMirrorMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn];		
		//m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite->setVisible(false);
		//UpdatePostionOfSprite(targetMovedCells[i].m_iRow, targetMovedCells[i].m_iColumn, true);		

		//if (m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite == NULL)
		//	CCLOG("3- Effect Mirror NULL");
	}	

	// generate new cells
	// play sound
	SoundManager::PlaySoundEffect(_SET_DROP_GEM_, fDelayTime + fDestroyTime * (iMaxComboPhase+1));
	
	CCLOG("Create new gems");

	std::vector<unsigned char> outputLettersForGems;
	Cell cell;
	unsigned char iLetter;
	m_GameBoardManager.GetGameWordManager()->GenerateNewLetters( newCells.size(), outputLettersForGems, false);	
	for(int iGemIndex=0; iGemIndex < newCells.size(); iGemIndex++)
	{
		iLetter = outputLettersForGems[iGemIndex];
		cell = newCells[iGemIndex];

		for(int i=0; i< 2; i++)
		{		
			Sprite* pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn),
				(iLetter != 255 && i==0)?_GCT_HAS_LETTER_:_GCT_NONE_ ).c_str());
			//pSprite->setAnchorPoint(ccp(0,0));
			//pSprite->setColor(ccc3( 255-(m_iMoveCount+1)*5,  255, 255));
			if (i!= 0)
			{
				pSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height));

				pSprite->setVisible(false);
				m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
			}
			else
			{
				pSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, 
					m_fBoardBottomPosition + (cell.m_iRow + iNumberOfRow) * m_SymbolSize.height));
				pSprite->runAction(
					Sequence::create(
						DelayTime::create(fDelayTime + fDestroyTime * (iMaxComboPhase+1) + 0.027f* (m_GameBoardManager.GetRowNumber() - cell.m_iRow)),
						EaseOut::create( MoveTo::create(fMoveTime * 1.4f,
							ccp(m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, 
									m_fBoardBottomPosition + cell.m_iRow  * m_SymbolSize.height)), 1.f),
							NULL));		

				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
			}
			//pSprite->setScale(0.65f);
					
			m_pBoardBatchNode->addChild(pSprite);
			
			if (iLetter !=255 && i==0) //appear at main cells, char -1=255 on Android!!!
			{			
				CCLOG("%d, %c:", iLetter, (unsigned char)iLetter);
				Sprite* pCharacterSprite = Sprite::createWithSpriteFrameName(
					m_pWordCollectBoardRenderNode->GetImageInGemFileFromLetter(iLetter).c_str());
				
				//pCharacterSprite->setScale(0.75f);
				//pCharacterSprite->setAnchorPoint(ccp(0,0));
				//pCharacterSprite->setPosition(ccp( 25.f, 25.f));
				pCharacterSprite->setPosition(ccp( 42.f, 42.f));

				switch(m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn))
				{
					case 0: //Orange
						pCharacterSprite->setColor(ccc3(252, 234, 160));
						break;
					case 1: //red
						pCharacterSprite->setColor(ccc3(242, 209, 163));
						break;
					case 2: //pink
						pCharacterSprite->setColor(ccc3(242, 217, 179));
						break;
					case 3: //white
						pCharacterSprite->setColor(ccc3(148, 135, 102));
						break;
					case 4: //blue
						pCharacterSprite->setColor(ccc3(17, 215, 250));
						break;
					case 5: //green
						pCharacterSprite->setColor(ccc3(184, 212, 6));
						break;
				}

				pSprite->addChild(pCharacterSprite);

				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iLetter = iLetter;
			}
			else if (i == 0)
			{
				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iLetter = 255;
			}
		}		
	}	

	// update display of score/move 
	m_pStatusLayer->setCurrentMove( m_GameBoardManager.GetCurrentMove());
	m_pStatusLayer->setCurrentScore( m_GameBoardManager.GetCurrentScore());
	//m_pStatusLayer->update(0);
	
	this->runAction( CCSequence::create(
					CCDelayTime::create(fDelayTime + fDestroyTime * (iMaxComboPhase+1)+ fMoveTime *5),
					CCCallFunc::create( this, callfunc_selector( HelloWorld::ExecuteBonusWinGameEffect)),
					NULL));		
}
*/

void HelloWorld::ShowMainWordUnlockEffect()
{
	m_pWordCollectBoardRenderNode->PlayCharacterAnim(3, true);

	
	m_pEndGameEffectLayer = LayerColor::create(ccc4( 0,0,0, 100));
	this->getParent()->addChild(m_pEndGameEffectLayer);

	/*float fDisplayEffectTime = 2.f;
	float fDelayTime = 0.35f;

	FlashCardNode* pFlashCard = FlashCardNode::createLayout( m_GameBoardManager.GetGameWordManager()->GetMainWord());
	pFlashCard->setPosition(Point(320, 480));
	m_pEndGameEffectLayer->addChild(pFlashCard);
	pFlashCard->displayEffect( fDisplayEffectTime, fDelayTime);

	this->runAction(
		Sequence::create(
			DelayTime::create( fDisplayEffectTime + fDelayTime*2 + 1.f),
			CallFunc::create( this,  callfunc_selector(HelloWorld::StartWinBonusPhase)),
			NULL));

	m_pWordCollectBoardRenderNode->PlaySpellingSound();*/

	
	SpriteBatchNode* pSpriteBatchNode = SpriteBatchNode::create("ResourceDemo.pvr.ccz");
	m_pEndGameEffectLayer->addChild(pSpriteBatchNode);

	// play unlock main word effect
	float fScaleRatio = 1.75f;
	float fDisplayEffectTime = 0.25f;
	float fDelayPerLetter = 0.06f;

	const Word& mainWord = m_GameBoardManager.GetGameWordManager()->GetMainWord();
	

	float fWordGraphicLength = 0;
	Sprite* letterSpriteList[_GDS_WORD_MAX_LENGTH_];

	for(int i=0; i< mainWord.m_iWordLength; i++)
	{
		letterSpriteList[i] = Sprite::createWithSpriteFrameName(  m_pWordCollectBoardRenderNode->GetImageFileFromLetter(mainWord.m_sWord[i]).c_str());
		letterSpriteList[i]->setAnchorPoint(Point(0,0));
		letterSpriteList[i]->setScale(fScaleRatio);

		fWordGraphicLength += letterSpriteList[i]->getContentSize().width * fScaleRatio + 2.f;
	}

	Size winSize = Director::getInstance()->getWinSize();
	float fLetterStartPosX = (winSize.width - fWordGraphicLength) /2.f;
	float fLetterPosY = (winSize.height - letterSpriteList[0]->getContentSize().height)/2.f;
	float fPositionXIncrement = fLetterStartPosX;

	for(int i=0; i< mainWord.m_iWordLength; i++)
	{
		letterSpriteList[i]->setPosition( Point(fPositionXIncrement, fLetterPosY));
		letterSpriteList[i]->setOpacity(30);
		letterSpriteList[i]->setScale(5.f);
		pSpriteBatchNode->addChild(letterSpriteList[i]);

		letterSpriteList[i]->runAction( 
			Sequence::create(
				DelayTime::create( i*fDelayPerLetter),
				EaseOut::create( FadeTo::create(fDisplayEffectTime, 255) ,2.f) ,
				NULL));
		letterSpriteList[i]->runAction( 
			Sequence::create(
				DelayTime::create( i*fDelayPerLetter),
				EaseOut::create( ScaleTo::create( fDisplayEffectTime, fScaleRatio, fScaleRatio), 2.f), 
				NULL));		

		fPositionXIncrement += letterSpriteList[i]->getContentSize().width * fScaleRatio + 2.f;
	}

	m_pWordCollectBoardRenderNode->PlaySpellingSound();
	
	pSpriteBatchNode->runAction(
		Sequence::create(
			DelayTime::create( mainWord.m_iWordLength * fDelayPerLetter + fDisplayEffectTime + 4.f),
			FadeOut::create(0.5f),
			RemoveSelf::create(),
			NULL));

	this->runAction(
		Sequence::create(
			DelayTime::create( mainWord.m_iWordLength * fDelayPerLetter + fDisplayEffectTime + 4.5f),
			CallFunc::create( this,  callfunc_selector(HelloWorld::StartWinBonusPhase)),
			NULL));

}

using namespace cocos2d::extension::armature;

void HelloWorld::StartWinBonusPhase()
{
	m_pEndGameEffectLayer->setColor(ccc3(255,255,255));
	m_pEndGameEffectLayer->setOpacity(0);

	//! Load data synchorinize (should replace load async in the future
    ArmatureDataManager::getInstance()->addArmatureFileInfo("CCS_Animation/EatingTime/EatingTime.ExportJson");
	
	//play
	Armature* pAmarture = Armature::create("EatingTime");
	pAmarture->getAnimation()->playByIndex(0);		
	pAmarture->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);	
	m_pEndGameEffectLayer->addChild(pAmarture);

	Size winSize = Director::getInstance()->getWinSize();	
	pAmarture->setPosition( winSize.width/2.f, winSize.height/2.f);

	float fAnimDuration = pAmarture->getAnimation()->getRawDuration() / 60.f;

	pAmarture->runAction(
		Sequence::create(
			DelayTime::create( fAnimDuration),
			RemoveSelf::create(),
			NULL));

	this->runAction(
		Sequence::create(
			DelayTime::create( fAnimDuration + 0.5f),
			CallFunc::create( this,  callfunc_selector(HelloWorld::ExecuteBonusWinGameEffect)),
			NULL));
	
}

void HelloWorld::ShowLevelCompleteEffect()
{
	//! Load data synchorinize (should replace load async in the future
    ArmatureDataManager::getInstance()->addArmatureFileInfo("CCS_Animation/WordClear/WordClear.ExportJson");
	
	//play
	Armature* pAmarture = Armature::create("WordClear");
	pAmarture->getAnimation()->playByIndex(0);	
	pAmarture->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);	
	m_pEndGameEffectLayer->addChild(pAmarture);

	Size winSize = Director::getInstance()->getWinSize();	
	pAmarture->setPosition( winSize.width/2.f, winSize.height/2.f );

	float fAnimDuration = pAmarture->getAnimation()->getRawDuration() / 60.f;

	pAmarture->runAction(
		Sequence::create(
			DelayTime::create( fAnimDuration + 0.02f),
			RemoveSelf::create(),
			NULL));

	this->runAction(
		Sequence::create(
			DelayTime::create( fAnimDuration),
			CallFunc::create( this,  callfunc_selector(HelloWorld::ShowWinGamePopup)),
			NULL));

	SoundManager::PlaySoundEffect(_SET_WIN_);
}


void HelloWorld::PlayCombo4Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime)
{
	auto pComboEffectSprite = Sprite::createWithSpriteFrameName("Com4_Ex_00000.png");
	auto pCombo4Anim = AnimationCache::getInstance()->getAnimation("effectCombo4");		

	pComboEffectSprite->setPosition( Point(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height));
	m_pComboEffectBatchNode->addChild(pComboEffectSprite);

	pComboEffectSprite->runAction( 
		Sequence::create( 
			DelayTime::create(fDelayTime),
			Animate::create( pCombo4Anim),
			RemoveSelf::create(),
			NULL));

	SoundManager::PlaySoundEffect(_SET_SIMPLE_COMBO_, fDelayTime);
}
	
void HelloWorld::PlayCombo5Effect(ComboEffectBundle* pComboEffectBundle, float fDelayTime, float fDisplayTime)
{	
	auto pCombo5AnimFlare = AnimationCache::getInstance()->getAnimation("effectCombo5_Flare");
	auto pCombo5AnimBolt = AnimationCache::getInstance()->getAnimation("effectCombo5_Bolt");		
	Cell comboCell = pComboEffectBundle->m_ComboEffectDescription.m_Position;

	auto pTriggerSprite = Sprite::createWithSpriteFrameName("Flare_00000_1.png");
	pTriggerSprite->setPosition(Point(m_fBoardLeftPosition + comboCell.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + comboCell.m_iRow * m_SymbolSize.height));
	m_pComboEffectBatchNode->addChild(pTriggerSprite);
	pTriggerSprite->runAction( 
		Sequence::create(
			DelayTime::create(fDelayTime),
			Animate::create( pCombo5AnimFlare),
			RemoveSelf::create(),
			NULL));

	for(auto destroyedCell : pComboEffectBundle->m_DestroyedCells)
	{
		auto pComboEffectSprite = Sprite::createWithSpriteFrameName("Bolt 2_00000_1.png");
		pComboEffectSprite->setAnchorPoint  (Point( 0, 0.5f));
		pComboEffectSprite->setPosition(Point(m_fBoardLeftPosition + comboCell.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + comboCell.m_iRow * m_SymbolSize.height));

		Point vector(destroyedCell.m_iColumn - comboCell.m_iColumn, destroyedCell.m_iRow - comboCell.m_iRow);
		auto fAngle = atan2f( vector.y, vector.x);
		float fDistance = sqrtf( vector.x*vector.x + vector.y*vector.y);
		
		pComboEffectSprite->setRotation(-fAngle * 180.f / M_PI);
		pComboEffectSprite->setScaleX( fDistance * m_SymbolSize.width / pComboEffectSprite->getContentSize().width);
		pComboEffectSprite->setScaleY(1.5f);

		m_pComboEffectBatchNode->addChild(pComboEffectSprite);
		pComboEffectSprite->runAction( 
			Sequence::create( 
				DelayTime::create(fDelayTime + 0.05f),
				Animate::create( pCombo5AnimBolt),
				RemoveSelf::create(),
				NULL));



		auto pFlareSprite = Sprite::createWithSpriteFrameName("Flare_00000_1.png");	
		pFlareSprite->setPosition(Point(m_fBoardLeftPosition + destroyedCell.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + destroyedCell.m_iRow * m_SymbolSize.height));
		//pFlareSprite->setOpacity(0);		
		m_pComboEffectBatchNode->addChild(pFlareSprite);

		/*pFlareSprite->runAction( 
			Sequence::createWithTwoActions(
				DelayTime::create(fDelayTime),
				FadeTo::create(0.5f, 255)));
				//ScaleTo::create(fDisplayTime, 1.f, 1.f)));*/
		pFlareSprite->runAction( 
			Sequence::create(
				DelayTime::create(fDelayTime + 0.42f),
				Animate::create( pCombo5AnimFlare),
				RemoveSelf::create(),
				NULL ));		
	}

	/*
	auto pTriggerSprite = Sprite::createWithSpriteFrameName("flare.png");	
	pTriggerSprite->setPosition(Point(m_fBoardLeftPosition + comboCell.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + comboCell.m_iRow * m_SymbolSize.height));
	m_pComboEffectBatchNode->addChild(pTriggerSprite);
	pTriggerSprite->setScale(2.5f);
	pTriggerSprite->setOpacity(0);
	pTriggerSprite->runAction( 
		Sequence::createWithTwoActions(
			DelayTime::create(fDelayTime),
			FadeTo::create(0.001f, 255)));
			//ScaleTo::create(fDisplayTime, 1.f, 1.f)));
	pTriggerSprite->runAction( 
		Sequence::create(
			DelayTime::create(fDelayTime + fDisplayTime),
			FadeOut::create(0.05f),
			RemoveSelf::create(),
			NULL ));

	for(auto destroyedCell : pComboEffectBundle->m_DestroyedCells)
	{
		auto pComboEffectSprite = Sprite::createWithSpriteFrameName("Bolt 2_00000.png");
		pComboEffectSprite->setAnchorPoint(Point(0,0));
		pComboEffectSprite->setPosition(Point(m_fBoardLeftPosition + comboCell.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + comboCell.m_iRow * m_SymbolSize.height));

		Point vector(destroyedCell.m_iColumn - comboCell.m_iColumn, destroyedCell.m_iRow - comboCell.m_iRow);
		auto fAngle = atan2f( vector.y, vector.x);
		float fDistance = sqrtf( vector.x*vector.x + vector.y*vector.y);
		
		pComboEffectSprite->setRotation(-fAngle * 180.f / M_PI);
		pComboEffectSprite->setScaleX( fDistance * m_SymbolSize.width / pComboEffectSprite->getContentSize().width);
		pComboEffectSprite->setScaleY(1.5f);

		m_pComboEffectBatchNode->addChild(pComboEffectSprite);
		pComboEffectSprite->runAction( 
			Sequence::create( 
				DelayTime::create(fDelayTime),
				Animate::create( pCombo5Anim),
				RemoveSelf::create(),
				NULL));



		auto pFlareSprite = Sprite::createWithSpriteFrameName("flare.png");	
		pFlareSprite->setPosition(Point(m_fBoardLeftPosition + destroyedCell.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + destroyedCell.m_iRow * m_SymbolSize.height));
		pFlareSprite->setOpacity(0);
		pFlareSprite->setScale(2.5f);
		m_pComboEffectBatchNode->addChild(pFlareSprite);

		pFlareSprite->runAction( 
			Sequence::createWithTwoActions(
				DelayTime::create(fDelayTime),
				FadeTo::create(0.015f, 255)));
				//ScaleTo::create(fDisplayTime, 1.f, 1.f)));
		pFlareSprite->runAction( 
			Sequence::create(
				DelayTime::create(fDelayTime + fDisplayTime + 0.25f),
				FadeOut::create(0.15f),
				RemoveSelf::create(),
				NULL ));		
	}
	*/
	SoundManager::PlaySoundEffect(_SET_COMBINE_DOUBLE_COMBO_, fDelayTime-0.15f);
}

// unlock letter flow
void HelloWorld::PlayUnlockLettersOfMainWordAnimation(const float& fDelayTime)
{
	float fDisplayTime = m_pWordCollectBoardRenderNode->PlayUnlockLettersAnimation(fDelayTime);
	if (fDisplayTime > 0)
	{
		m_pWordCollectBoardRenderNode->PlayCharacterAnim(5, false);

		this->runAction(
			Sequence::create(
			DelayTime::create( fDisplayTime),
				CallFunc::create( this,  callfunc_selector(HelloWorld::PlayUnlockLettersOfBonusWordsAnimation)),
				NULL));
	}
	else
	{
		PlayUnlockLettersOfBonusWordsAnimation();
	}
}

void HelloWorld::PlayUnlockLettersOfBonusWordsAnimation()
{
	float fDisplayTime = m_pBonusWordNode->displayEffect(0.8f);
	if (fDisplayTime > 0)
	{
		m_pWordCollectBoardRenderNode->PlayCharacterAnim(6, false);

		this->runAction(
			Sequence::create(
			DelayTime::create( fDisplayTime + 0.1f),
				CallFunc::create( this,  callfunc_selector(HelloWorld::EndUnlockLetterAnimation)),
				NULL));
	}
	else
	{
		EndUnlockLetterAnimation();
	}

	
}

void HelloWorld::EndUnlockLetterAnimation()
{
	m_bIsEffectPlaying = false;

	if (m_bIsEndGamePhase)
	{
		//ShowWinGamePopup();
		ShowLevelCompleteEffect();
	}
	else
	{
		//check end game
		if (m_GameBoardManager.GetGameWordManager()->IsMainWordUnlocked()) // complete objective ==> win		
		//if (true)
		//if (false)
		{
			// play sound effect 
			SoundManager::PlaySoundEffect(_SET_COMPLETE_WORD_);

			m_bIsEffectPlaying = true;//stop all interaction on board from now
			m_bIsEndGamePhase = true; 

			ShowMainWordUnlockEffect();
			//ExecuteBonusWinGameEffect();						
		}
		else  if (m_GameBoardManager.GetCurrentMove() == 0) // out of move ==> lose
		{
			m_bIsEndGamePhase = true; 

			EndGameNode* pEndGameNode = EndGameNode::createLayoutLose( m_GameBoardManager.GetCurrentScore(), 
				m_GameBoardManager.GetGameWordManager()->GetMainWord(), m_GameBoardManager.GetCurrentLevel());
			m_pHUDLayer->addChild( pEndGameNode, 100);

			SoundManager::PlaySoundEffect(_SET_LOSE);
		}
	}
}

// NOTE: only implement combo4 now
void HelloWorld::DrawComboHint()
{
	for(auto sprite:m_HintSprites)
		sprite->removeFromParentAndCleanup(true);
	m_HintSprites.clear();

	if (m_eTouchMoveState == _TMS_BEGIN_ACTIVATE_COMBO_)
		return;
	ComboActivateDirection_e eComboDireciton;
	switch (m_eTouchMoveState)
	{	
		default:
		case _TMS_ACTIVATE_COMBO_LEFT_:
			eComboDireciton = _CAD_LEFT_;
			break;
		case _TMS_ACTIVATE_COMBO_UP_:
			eComboDireciton = _CAD_UP_;
			break;
		case _TMS_ACTIVATE_COMBO_RIGHT_:
			eComboDireciton = _CAD_RIGHT_;
			break;
		case _TMS_ACTIVATE_COMBO_DOWN_:
			eComboDireciton = _CAD_DOWN_;
			break;		
	}


	// should get effective area from gameBoardManager
	m_ComputeMoveResult.Reset(false);
	
	m_GameBoardManager.PreCheckComboEffect( m_SelectedCell.m_iRow, m_SelectedCell.m_iColumn, eComboDireciton, m_ComputeMoveResult.m_BasicMatchingDestroyedCells);
	
	m_pMoveHintNode = Sprite::createWithSpriteFrameName("Gem_A.png");
	m_pMoveHintNode->setOpacity(0);
	m_pBoardBatchNode->addChild(m_pMoveHintNode);

	for(auto cell : m_ComputeMoveResult.m_BasicMatchingDestroyedCells)
	{
		Sprite* pHintSprite = Sprite::createWithSpriteFrameName("Blue_Border.png");
		//pHintSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, m_fBoardBottomPosition + cell.m_iRow  * m_SymbolSize.height));
		Size size = m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->getContentSize();
		m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->addChild(pHintSprite);		

		pHintSprite->setPosition( Point(size.width/2.f, size.height/2.f ));
			
		m_HintSprites.push_back(pHintSprite);
	}	
}