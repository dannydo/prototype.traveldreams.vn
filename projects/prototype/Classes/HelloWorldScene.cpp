#include "HelloWorldScene.h"
#include "SoundManager.h"
#include "EndGameNode.h"
#include "LevelMapScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene(int iLevel)
{
	if ( iLevel > 4)
		iLevel = 1;
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
	auto backgroundLayer = LayerColor::create(ccc4(10, 10, 10, 255)); //64, 88, 119, 255));
	Sprite* pBackground = Sprite::create("Background.png");
	pBackground->setAnchorPoint(ccp(0,0));
	backgroundLayer->addChild(pBackground);

	scene->addChild(backgroundLayer);

    // 'layer' is an autorelease object
    auto boardLayer = HelloWorld::create();	

    // add layer as a child to scene
    scene->addChild(boardLayer);

	//
	boardLayer->m_pWordCollectBoardRenderNode = WordCollectBoardRenderNode::create();	
	scene->addChild(boardLayer->m_pWordCollectBoardRenderNode);

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
	boardLayer->m_pHUDLayer = Layer::create();
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
	boardLayer->m_pBonusWordNode->setPosition( 314.f, 756.f);
	boardLayer->m_pHUDLayer->addChild(boardLayer->m_pBonusWordNode, 10);

	/*Node* btnShowPopup = boardLayer->m_pBonusWordNode->createButtonShowPopupBonus();
	btnShowPopup->setPosition(-80, 300);
	boardLayer->m_pHUDLayer->addChild(btnShowPopup);*/

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

	this->setTouchEnabled(true);	
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	this->scheduleUpdate();

    return true;
}

void HelloWorld::initLevel(int iLevel)
{
	int iNumberOfRow = 8;
	int iNumberOfColumn = 8;
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
	m_GameBoardManager.GenerateGameBoard(iNumberOfRow, iNumberOfColumn, iLevel);

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

	// init batch node for combo effect (used later)
	CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("ComboEffect/combo.plist");
	m_pComboEffectBatchNode = CCSpriteBatchNode::create("ComboEffect/combo.pvr.ccz");
	this->addChild(m_pComboEffectBatchNode);

	// cache anim
	auto animCache = AnimationCache::getInstance();    
    animCache->addAnimationsWithFile("ComboEffect/combo4Animations.plist");
	animCache->addAnimationsWithFile("ComboEffect/combo5Animations.plist");

	// get symbol size
	CCSprite* pSprite;
	pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(0).c_str());
	m_SymbolSize = Point(78.f, 78.f); //pSprite->getContentSize() * 0.5;

	int iRow, iColumn, iFlag;
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
				for( iFlag =0; iFlag < 2; iFlag++)
				{				
					pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(m_GameBoardManager.GetCellValue(iRow, iColumn)).c_str());
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
					if (m_GameBoardManager.GetCellObstacleType( iRow, iColumn) != _OT_NONE_)
					{
						Sprite* pMaskSprite = Sprite::createWithSpriteFrameName( "Lock.png");
						pMaskSprite->setAnchorPoint(Point(0,0));
						pSprite->addChild(pMaskSprite);
					}					
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
		m_fBoardLeftClipPosition + 618, m_fBoardBottomClipPosition + 515);
        //scissorRect.origin.x, scissorRect.origin.y,
        //scissorRect.size.width, scissorRect.size.height);

    CCLayer::visit();
    glDisable(GL_SCISSOR_TEST);
    kmGLPopMatrix();
}

std::string HelloWorld::GetImageFileFromGemID(int iGemID, GemComboType_e eGemComboType)
{
	switch(iGemID)
	{
		case 0:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Orange.png"; //"Candy_Cam.png";
				//case _GCT_COMBO4_:
					//return "Orange_Combo4.png";
				case _GCT_COMBO5_:
					return "Orange_Combo5.png";
				case _GCT_COMBO6_:
					return "Orange_Combo6.png";
				case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "Word_Orange.png";

			}

		}
		case 1:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Red.png"; //"Candy_Do.png";
				//case _GCT_COMBO4_:
					//return "Red_Combo4.png";
				case _GCT_COMBO5_:
					return "Red_Combo5.png";
				case _GCT_COMBO6_:
					return "Red_Combo6.png";
				case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "Word_Red.png";
			}			
		}
		case 2:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Pink.png"; //"Candy_Tim.png";
				//case _GCT_COMBO4_:
					//return "Pink_Combo4.png";
				case _GCT_COMBO5_:
					return "Pink_Combo5.png";
				case _GCT_COMBO6_:
					return "Pink_Combo6.png";
				case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "Word_Pink.png";
			}						
		}
		case 3:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "White.png"; //"Candy_Vang.png";
				//case _GCT_COMBO4_:
					//return "White_Combo4.png";
				case _GCT_COMBO5_:
					return "White_Combo5.png";
				case _GCT_COMBO6_:
					return "White_Combo6.png";
				case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "Word_White.png";
			}									
		}
		case 4:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Blue.png"; //"Candy_XanhDuong.png";
				//case _GCT_COMBO4_:
					//return "Blue_Combo4.png";
				case _GCT_COMBO5_:
					return "Blue_Combo5.png";
				case _GCT_COMBO6_:
					return "Blue_Combo6.png";
				case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "Word_Blue.png";
			}												
		}
		case 5:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Green.png"; //"Candy_XanhLa.png";
				//case _GCT_COMBO4_:
					//return "Green_Combo4.png";
				case _GCT_COMBO5_:
					return "Green_Combo5.png";
				case _GCT_COMBO6_:
					return "Green_Combo6.png";
				case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "Word_Green.png";
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
		m_eTouchMoveState = _TMS_BEGIN_IDENTIFY_;
		m_StartTouchPosition = touchPosition;
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

	Point currentPosition = pTouch->getLocation();
	float fDeltaX = currentPosition.x - m_StartTouchPosition.x;
	float fDeltaY = currentPosition.y - m_StartTouchPosition.y;
	AdjustPosition( bIsBlocked, fDeltaX, fDeltaY, m_SelectedCell.m_iRow, m_SelectedCell.m_iColumn);

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
		if (fabs(fDeltaX) > 5.f)
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
		else if (fabs(fDeltaY) > 5.f)
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

	if (m_eTouchMoveState == _TMS_MOVE_HORIZONTAL_)
	{
		fMoveUnit = fDeltaX/m_SymbolSize.width;
				
		fMoveUnit = round(fMoveUnit);

		iMoveUnit = (int)fMoveUnit;

		m_ePlayingDragEffect = _TMS_MOVE_HORIZONTAL_;

		if (!bIsBlocked && m_GameBoardManager.RecheckAfterMoveV2( m_SelectedCell.m_iRow,-1,  -1, iMoveUnit, 
				m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_DoubleComboList, m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_NewCells))
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
			
			//int iBlankCellCount = 0; //blank cell on the move
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
						if (m_GameBoardManager.IsBlankCell( m_SelectedCell.m_iRow, (iColumn + iTranslationCell + iNumberOfColumn) % iNumberOfColumn))
						{
							iStep--;
						}
					}

					m_BoardViewMatrix[m_SelectedCell.m_iRow][(iColumn + iNumberOfColumn + iTranslationCell) % (iNumberOfColumn)] = tempCellList[iColumn];
					m_BoardViewMirrorMatrix[m_SelectedCell.m_iRow][(iColumn + iNumberOfColumn + iTranslationCell) % (iNumberOfColumn)] = tempCellMirrorList[iColumn];
					/*
					//iBlankCellCount = 0;

					for(int iDelta=1; iDelta<= abs(iMoveUnit); iDelta++)
						if (m_GameBoardManager.IsBlankCell(m_SelectedCell.m_iRow, (iColumn + iDelta * iSign + iNumberOfColumn) % iNumberOfColumn))
							iBlankCellCount++;
					//while (m_GameBoardManager.IsBlankCell(m_SelectedCell.m_iRow, (iColumn + iMoveUnit + iNumberOfColumn + iCheck) % (iNumberOfColumn)))
						//iCheck += iStep;

					m_BoardViewMatrix[m_SelectedCell.m_iRow][(iColumn + iMoveUnit + iNumberOfColumn + iBlankCellCount*iSign) % (iNumberOfColumn)] = tempCellList[iColumn];
					m_BoardViewMirrorMatrix[m_SelectedCell.m_iRow][(iColumn + iMoveUnit + iNumberOfColumn + iBlankCellCount * iSign) % (iNumberOfColumn)] = tempCellMirrorList[iColumn];
					*/
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
				m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_DoubleComboList, m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_NewCells))
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
			
			//int iBlankCellCount = 0; //blank cell on the move
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
						if (m_GameBoardManager.IsBlankCell( (iRow + iNumberOfRow + iTranslationCell) % (iNumberOfRow), m_SelectedCell.m_iColumn))
						{
							iStep--;
						}
					}

					m_BoardViewMatrix[(iRow + iNumberOfRow + iTranslationCell) % (iNumberOfRow)][m_SelectedCell.m_iColumn] = tempCellList[iRow];
					m_BoardViewMirrorMatrix[(iRow + iNumberOfRow + iTranslationCell) % (iNumberOfRow)][m_SelectedCell.m_iColumn] = tempCellMirrorList[iRow];
					/*
					iBlankCellCount = 0;
					for(int iDelta=1; iDelta<= abs(iMoveUnit); iDelta++)
						if (m_GameBoardManager.IsBlankCell( (iRow + iNumberOfRow + iDelta * iSign) % (iNumberOfRow), m_SelectedCell.m_iColumn))
							iBlankCellCount++;

					//while (m_GameBoardManager.IsBlankCell( (iRow + iMoveUnit + iNumberOfRow + iCheck) % (iNumberOfRow), m_SelectedCell.m_iColumn))
						//iCheck += iStep;

					m_BoardViewMatrix[(iRow + iMoveUnit + iNumberOfRow + iBlankCellCount*iSign) % (iNumberOfRow)][m_SelectedCell.m_iColumn] = tempCellList[iRow];
					m_BoardViewMirrorMatrix[(iRow + iMoveUnit + iNumberOfRow + iBlankCellCount*iSign) % (iNumberOfRow)][m_SelectedCell.m_iColumn] = tempCellMirrorList[iRow];*/
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
		PlayEffect2( false, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_DoubleComboList, 
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_NewCells, true);
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
	bool bMoveIsValid = false;
	m_ComputeMoveResult.Reset(false);

	if (m_GameBoardManager.RecheckAfterMoveV2( -1, -1, -1, -1, m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_DoubleComboList, 
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_NewCells))
	{
		PlayEffect2( false, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_DoubleComboList, 
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_NewCells, false);
	}
	else
	{
		m_bIsEffectPlaying = false;

		//check end game
		if (m_GameBoardManager.GetGameWordManager()->IsMainWordUnlocked()) // complete objective ==> win		
		//if (true)
		//if (false)
		{
			// play sound effect 
			SoundManager::PlaySoundEffect(_SET_COMPLETE_WORD_);

			m_bIsEffectPlaying = true;//stop all interaction on board from now

			ShowMainWordUnlockEffect();
			//ExecuteBonusWinGameEffect();						
		}
		else  if (m_GameBoardManager.GetCurrentMove() == 0) // out of move ==> lose
		{
			EndGameNode* pEndGameNode = EndGameNode::createLayoutLose( m_GameBoardManager.GetCurrentScore(), 
				const_cast<char*>(m_GameBoardManager.GetGameWordManager()->GetMainWord().m_sWord), m_GameBoardManager.GetCurrentLevel());
			m_pHUDLayer->addChild( pEndGameNode, 10);

			SoundManager::PlaySoundEffect(_SET_LOSE);

		}
	}
}

void HelloWorld::ExecuteBonusWinGameEffect()
{
	m_ComputeMoveResult.Reset(true);

	//if (m_GameBoardManager.GetCurrentMove() > 0)
	if (m_GameBoardManager.ExecuteEndGameBonus(m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_DoubleComboList, 
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_NewCells))
	{
		PlayEffect2( true, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_DoubleComboList, 
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_NewCells, false);

		m_pStatusLayer->setCurrentMove( m_GameBoardManager.GetCurrentMove());
		//m_pStatusLayer->update(0);
	}
	else
	{
		//ShowWinGamePopup();
		ShowLevelCompleteEffect();
	}
}

void HelloWorld::ShowWinGamePopup()
{
	std::vector<char*> subWordList;
	for(int i=0; i < m_GameBoardManager.GetGameWordManager()->GetSubWordCount(); i++)
	{
		subWordList.push_back(const_cast<char*>( m_GameBoardManager.GetGameWordManager()->GetSubWord(i).m_sWord));
	}

	EndGameNode* pEndGameNode = EndGameNode::createLayoutWin( m_GameBoardManager.GetCurrentScore(),
		const_cast<char*>(m_GameBoardManager.GetGameWordManager()->GetMainWord().m_sWord), subWordList, m_GameBoardManager.GetCurrentLevel());
	pEndGameNode->addYellowStar( m_GameBoardManager.GetEarnedStars());
	m_pHUDLayer->addChild( pEndGameNode, 10);
}

void HelloWorld::UpdatePostionOfSprite(const int& iRow,const int& iColumn, bool bIsMirror)
{
	if (!bIsMirror)
		m_BoardViewMatrix[iRow][iColumn].m_pSprite->setPosition( ccp(m_fBoardLeftPosition + (iColumn-
			m_GameBoardManager.GetColumnNumber()) * m_SymbolSize.width, m_fBoardBottomPosition + (iRow-m_GameBoardManager.GetRowNumber()) * m_SymbolSize.height));
	else
		m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite->setPosition( ccp(m_fBoardLeftPosition + (iColumn-
			m_GameBoardManager.GetColumnNumber()) * m_SymbolSize.width, m_fBoardBottomPosition + (iRow-m_GameBoardManager.GetRowNumber()) * m_SymbolSize.height));
}

void HelloWorld::HorizontalMoveUlti(float fDeltaX)
{
	int iBlankCellOfFullLine = 0;
	for(int iColumn = 0; iColumn< m_iMovingCellListLength; iColumn++)
		//if (m_GameBoardManager.IsBlankCell(m_SelectedCell.m_iRow,iColumn))
		if (m_MovingCellList[iColumn].m_pSprite == NULL)
			iBlankCellOfFullLine++;

	if (fabs(fDeltaX) > (m_iMovingCellListLength - iBlankCellOfFullLine)* m_SymbolSize.width)
		fDeltaX = fDeltaX/fabs(fDeltaX) * (m_iMovingCellListLength - iBlankCellOfFullLine)* m_SymbolSize.width;

	float fMoveUnit = fDeltaX/m_SymbolSize.width;	
	int iSign; //1 or -1

	if (fMoveUnit > 0)
	{
		fMoveUnit = floorf(fMoveUnit);
		iSign = 1;
	}
	else
	{
		fMoveUnit = ceilf(fMoveUnit);
		iSign = -1;
	}
		
	int iMoveUnit = abs((int)fMoveUnit);		
	
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
				iTranslatedOriginCell += iSign;
				if (m_MovingCellList[(iColumn + iTranslatedOriginCell + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite == NULL)
				{
					iStep--;
				}
			}
			iTranslatedMirrorCell = iTranslatedOriginCell;		
			bool bMeetSide = false;

			if ((fDeltaX>0 && fDeltaX > fMoveUnit * m_SymbolSize.width) || (fDeltaX<0 && fDeltaX < fMoveUnit * m_SymbolSize.width))
			{
				while ((m_MovingCellList[(iColumn + iTranslatedMirrorCell + iSign + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite == NULL)
						||  (iColumn + iTranslatedMirrorCell +  iSign == m_iMovingCellListLength) || (iColumn + iTranslatedMirrorCell + iSign == -1))
				{
					if ((iColumn + iTranslatedMirrorCell +  iSign == m_iMovingCellListLength) || (iColumn + iTranslatedMirrorCell + iSign == -1))
						bMeetSide = true;	
					iTranslatedMirrorCell += iSign;

					if (m_MovingCellList[(iColumn + iTranslatedMirrorCell + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite != NULL)
						break;
					else if (bMeetSide)
					{
						iTranslatedMirrorCell += iSign;
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
				m_fBoardLeftPosition + ((iColumn + iTranslatedOriginCell + m_iMovingCellListLength) % m_iMovingCellListLength ) * m_SymbolSize.width + fDeltaX - fMoveUnit  * m_SymbolSize.width);
			
			if (iTranslatedMirrorCell != iTranslatedOriginCell)
			{
				m_MovingCellMirrorList[iColumn].m_pSprite->setVisible(true);
				m_MovingCellMirrorList[iColumn].m_pSprite->setPositionX(
					m_fBoardLeftPosition + ((iColumn + iTranslatedMirrorCell + m_iMovingCellListLength) % m_iMovingCellListLength ) * m_SymbolSize.width + fDeltaX - ( fMoveUnit + bMeetSide*iSign) * m_SymbolSize.width);
			}
			else
			{
				m_MovingCellMirrorList[iColumn].m_pSprite->setVisible(false);
				m_MovingCellMirrorList[iColumn].m_pSprite->setPosition(m_MovingCellList[iColumn].m_pSprite->getPosition());
			}
		}
	}
}

void HelloWorld::VerticalMoveUlti(float fDeltaY)
{
	int iBlankCellOfFullLine = 0;
	for(int iRow = 0; iRow< m_iMovingCellListLength; iRow++)
		//if (m_GameBoardManager.IsBlankCell(iRow, m_SelectedCell.m_iColumn))
		if (m_MovingCellList[iRow].m_pSprite == NULL)
			iBlankCellOfFullLine++;

	if (fabs(fDeltaY) > (m_iMovingCellListLength - iBlankCellOfFullLine)* m_SymbolSize.height)
		fDeltaY = fDeltaY/fabs(fDeltaY) * (m_iMovingCellListLength - iBlankCellOfFullLine)* m_SymbolSize.height;

	float fMoveUnit = fDeltaY/m_SymbolSize.height;	
	int iSign; //1 or -1

	if (fMoveUnit > 0)
	{
		fMoveUnit = floorf(fMoveUnit);
		iSign = 1;
	}
	else
	{
		fMoveUnit = ceilf(fMoveUnit);
		iSign = -1;
	}
		
	int iMoveUnit = abs((int)fMoveUnit);		
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
				iTranslatedOriginCell += iSign;
				if (m_MovingCellList[(iRow + iTranslatedOriginCell + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite == NULL)
				{
					iStep--;
				}
			}

			iTranslatedMirrorCell = iTranslatedOriginCell;		
			bool bMeetSide = false;
			if ((fDeltaY>0 && fDeltaY > fMoveUnit * m_SymbolSize.height) || (fDeltaY<0 && fDeltaY < fMoveUnit * m_SymbolSize.height))
			{
				while ((m_MovingCellList[(iRow + iTranslatedMirrorCell + iSign + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite == NULL)
						||  (iRow + iTranslatedMirrorCell +  iSign == m_iMovingCellListLength) || (iRow + iTranslatedMirrorCell + iSign == -1))
				{
					if ((iRow + iTranslatedMirrorCell +  iSign == m_iMovingCellListLength) || (iRow + iTranslatedMirrorCell + iSign == -1))
						bMeetSide = true;	
					iTranslatedMirrorCell += iSign;

					if (m_MovingCellList[(iRow + iTranslatedMirrorCell + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite != NULL)						
						break;
					else if (bMeetSide)
					{
						iTranslatedMirrorCell += iSign;						
						bMeetSide = false;
					}
				}
			}

			m_MovingCellList[iRow].m_pSprite->setPositionY(
				m_fBoardBottomPosition + ((iRow +  iTranslatedOriginCell + m_iMovingCellListLength) % m_iMovingCellListLength ) * m_SymbolSize.height + fDeltaY - fMoveUnit  * m_SymbolSize.height);
			
			if (iTranslatedMirrorCell != iTranslatedOriginCell)
			{
				m_MovingCellMirrorList[iRow].m_pSprite->setVisible(true);
				m_MovingCellMirrorList[iRow].m_pSprite->setPositionY(
					m_fBoardBottomPosition + ((iRow + iTranslatedMirrorCell + m_iMovingCellListLength) % m_iMovingCellListLength ) * m_SymbolSize.height + fDeltaY - ( fMoveUnit + bMeetSide*iSign) * m_SymbolSize.height);
			}
			else
			{
				m_MovingCellMirrorList[iRow].m_pSprite->setVisible(false);
				m_MovingCellMirrorList[iRow].m_pSprite->setPosition(m_MovingCellList[iRow].m_pSprite->getPosition());
			}

			/*iBlankCellInOriginMoving = 0;
			iBlankCellInMirrorMoving = 0;
			
			bool bMeetSide = false;

			for(int iDelta = 1; iDelta <= iMoveUnit; iDelta++)
				//if (m_GameBoardManager.IsBlankCell( (iRow + iDelta * iSign + m_iMovingCellListLength) % m_iMovingCellListLength, m_SelectedCell.m_iColumn))
				if (m_MovingCellList[(iRow + iDelta * iSign + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite == NULL)
				{
					iBlankCellInOriginMoving++;
					iBlankCellInMirrorMoving++;
				}
				
			
			if ((fDeltaY>0 && fDeltaY > fMoveUnit * m_SymbolSize.height) || (fDeltaY<0 && fDeltaY < fMoveUnit * m_SymbolSize.height))
			{			
				int iDelta = 0;
			
				while //(m_GameBoardManager.IsBlankCell((iRow + (iMoveUnit + iBlankCellInMirrorMoving + iDelta + 1) * iSign + m_iMovingCellListLength) % m_iMovingCellListLength, m_SelectedCell.m_iColumn)
					( m_MovingCellList[(iRow + (iMoveUnit + iBlankCellInMirrorMoving + iDelta + 1) * iSign + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite == NULL
					||  (iRow + (iMoveUnit + iBlankCellInMirrorMoving + iDelta + 1) * iSign == m_iMovingCellListLength) || (iRow + (iMoveUnit + iBlankCellInMirrorMoving + iDelta + 1) * iSign == -1))
				{					
					if ((iRow + (iMoveUnit + iBlankCellInMirrorMoving + iDelta + 1) * iSign == m_iMovingCellListLength) || (iRow + (iMoveUnit + iBlankCellInMirrorMoving + iDelta + 1) * iSign == -1))
						bMeetSide = true;

					iDelta++;

					// this is real condition to continue
					//if (!m_GameBoardManager.IsBlankCell( (iRow + (iMoveUnit + iBlankCellInMirrorMoving + iDelta) * iSign + m_iMovingCellListLength) % m_iMovingCellListLength, m_SelectedCell.m_iColumn))
					if (m_MovingCellList[(iRow + (iMoveUnit + iBlankCellInMirrorMoving + iDelta) * iSign + m_iMovingCellListLength) % m_iMovingCellListLength].m_pSprite != NULL)
						break;
				}

				iBlankCellInMirrorMoving += iDelta;
			}

			m_MovingCellList[iRow].m_pSprite->setPositionY(
				m_fBoardBottomPosition + ((iRow + (iMoveUnit +iBlankCellInOriginMoving) * iSign + m_iMovingCellListLength) % m_iMovingCellListLength ) * m_SymbolSize.height + fDeltaY - fMoveUnit  * m_SymbolSize.height);
			
			if (iBlankCellInMirrorMoving != iBlankCellInOriginMoving)
			{
				m_MovingCellMirrorList[iRow].m_pSprite->setVisible(true);
				m_MovingCellMirrorList[iRow].m_pSprite->setPositionY(
					m_fBoardBottomPosition + ((iRow + (iMoveUnit + iBlankCellInMirrorMoving) * iSign + m_iMovingCellListLength) % m_iMovingCellListLength ) * m_SymbolSize.height + fDeltaY - ( fMoveUnit + bMeetSide*iSign) * m_SymbolSize.height);
			}
			else
			{
				m_MovingCellMirrorList[iRow].m_pSprite->setVisible(false);
				m_MovingCellMirrorList[iRow].m_pSprite->setPosition(m_MovingCellList[iRow].m_pSprite->getPosition());
			}*/
		}
	}
}

void HelloWorld::PlayEffect2( const bool& bIsBonusEndGamePhase,  std::vector<ComboEffectCell>& convertedToComboCells, 
		std::vector<Cell>& basicMatchingDestroyedCells, std::vector<DoubleComboEffectBundle> doubleComboList, 
		std::vector<ComboEffectBundle*>& comboChainList,std::vector<ComboEffectCell>& newComboCells,
		std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,
		std::vector<Cell>& newCells, bool bIsNewMove)
{
	// for test	
	for(int i=0; i< _COMBO_TYPE_COUNT_;i++)
		m_pComboCountRenderNode->UpdateList(i, m_GameBoardManager.GetComboCount(i));

	m_bIsEffectPlaying = true;	

	float fDelayTime = 0.2f;
	float fDestroyTime = 0.35f;
	float fMoveTime = 0.24f; //25f;
	int iNumberOfRow = m_GameBoardManager.GetRowNumber();
	int iNumberOfColumn = m_GameBoardManager.GetColumnNumber();

	//CCLOG("Begin effect");

	if (bIsBonusEndGamePhase)
	{
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
	}


	// play sound
	if (basicMatchingDestroyedCells.size() > 0)
		SoundManager::PlaySoundEffect(_SET_SIMPLE_MATCH_);

	//CCLOG("Destroy cells");

	// destroy cells by basic matching 
	for(auto cell: basicMatchingDestroyedCells)
	{			
		BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime,fDestroyTime);

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
	

	// Double combo is disabled now
	// destroy cells and create temporary double combos?
	/*for(auto doubleCombo : doubleComboList)
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
	}*/

	//CCLOG("Combo chain");
	
	// play combo chain
	int iMaxComboPhase = 0;
	for(auto pComboEffect : comboChainList)
	{
		if (iMaxComboPhase < pComboEffect->m_iPhase)
			iMaxComboPhase = pComboEffect->m_iPhase;

		// play effect
		if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_EXPLOSION_)
			PlayCombo4Effect(pComboEffect, fDelayTime + pComboEffect->m_iPhase* fDestroyTime, fDestroyTime);
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLOR_)
			PlayCombo5Effect(pComboEffect, fDelayTime + pComboEffect->m_iPhase* fDestroyTime, fDestroyTime);
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_DOUBLE_EXPLOSION_)
		{
			Sprite* pComboEffectSprite;

			// play sound
			SoundManager::PlaySoundEffect(_SET_DOUBLE_COMPLE_EFFECT_, fDelayTime + pComboEffect->m_iPhase* fDestroyTime);

			pComboEffectSprite = Sprite::createWithSpriteFrameName("Explosion2.png");		
			pComboEffectSprite->setScale(2.17f);

			//destroy temporary double combo cell too
			BasicDestroyCellUlti( pComboEffect->m_ComboEffectDescription.m_Position.m_iRow, pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn,
				fDelayTime + pComboEffect->m_iPhase* fDestroyTime, fDestroyTime);

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
			BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime + pComboEffect->m_iPhase* fDestroyTime, fDestroyTime);
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
				Sprite* pLetterSprite = AddLetterToGem( cell);
				if (pLetterSprite != NULL)
				{
					pLetterSprite->setOpacity(0);
					pLetterSprite->runAction( 
						Sequence::create( 
							DelayTime::create(fDelayTime),
							EaseIn::create( FadeIn::create(fDestroyTime), 2.f),
							NULL));

					//pLetterSprite->setScale(2.f);//1.5f);
					//pLetterSprite->runAction( 
					//	Sequence::create( 
					//		DelayTime::create(fDelayTime),
					//		EaseIn::create(ScaleTo::create(fDestroyTime, 1.f, 1.f), 2.f),
					//		NULL));			
				}
			}			
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

	//CCLOG("Create new gems");

	std::vector<unsigned char> outputLettersForGems;
	Cell cell;
	//unsigned char iLetter;
	//m_GameBoardManager.GetGameWordManager()->GenerateNewLetters( newCells.size(), outputLettersForGems, bIsNewMove);	
	for(int iGemIndex=0; iGemIndex < newCells.size(); iGemIndex++)
	{
		//iLetter = (unsigned char)outputLettersForGems[iGemIndex];
		cell = newCells[iGemIndex];

		for(int i=0; i< 2; i++)
		{		
			//if (iLetter < 255)
			//	iLetter = iLetter;

			Sprite* pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), _GCT_NONE_).c_str());
				//(iLetter != 255)?_GCT_HAS_LETTER_:_GCT_NONE_ ).c_str());
			
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
				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iLetter = 255; //reset letter of gem
			}
			//pSprite->setScale(0.65f);
					
			m_pBoardBatchNode->addChild(pSprite);
						
			/*
			if (iLetter >= 0 && ((unsigned char)iLetter)!=255 && i==0) //appear at main cells, char -1=255 on Android!!!
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
				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iLetter = 255;//-1;
			}*/
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

	// update display of score/move 
	m_pStatusLayer->setCurrentMove( m_GameBoardManager.GetCurrentMove());
	m_pStatusLayer->setCurrentScore( m_GameBoardManager.GetCurrentScore());
	//m_pStatusLayer->update(0);

	if (bIsBonusEndGamePhase)
	{
		this->runAction( CCSequence::create(
					CCDelayTime::create(fDelayTime + fDestroyTime * (iMaxComboPhase+1)+ fMoveTime *5),
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
						CCDelayTime::create(fDelayTime + fDestroyTime * (iMaxComboPhase+1)+ fMoveTime + fMoveTime),
						CCCallFunc::create( this, callfunc_selector( HelloWorld::CheckBoardStateAfterMove)),
						NULL));		
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
		m_BoardViewMatrix[iRow][iColumn].m_iLetter = 255;
		return;
	}

	m_BoardViewMatrix[iRow][iColumn].m_pSprite->runAction(
		Sequence::create( 
			DelayTime::create(fDelay),
			ScaleTo::create( fEffectDuration, 0.05f, 0.05f),
			RemoveSelf::create( true),
			//FadeOut::create(0.01f),
			NULL));

	if (m_BoardViewMatrix[iRow][iColumn].m_iLetter < 255)
	{		
		m_pWordCollectBoardRenderNode->PlayUnlockLetterEffect(fDelay, m_BoardViewMatrix[iRow][iColumn].m_iLetter, 
			ccp(m_fBoardLeftPosition + iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + iRow * m_SymbolSize.height));



		int iUnlockedLetterIndexOfMainWord;
		std::vector<int> unlockedLettersIndexOfSubWords[_GDS_SUB_WORD_MAX_COUNT_];
		bool bIsMainWordJustUnlocked;
		bool justUnlockedSubWords[_GDS_SUB_WORD_MAX_COUNT_];

		if (m_GameBoardManager.GetGameWordManager()->UnlockLetter( m_BoardViewMatrix[iRow][iColumn].m_iLetter, iUnlockedLetterIndexOfMainWord, unlockedLettersIndexOfSubWords,
																	bIsMainWordJustUnlocked, justUnlockedSubWords))
		{
			m_pBonusWordNode->addLetter(m_BoardViewMatrix[iRow][iColumn].m_iLetter);

			if (iUnlockedLetterIndexOfMainWord >=0)
				m_pWordCollectBoardRenderNode->UnlockCharacter(fDelay, iUnlockedLetterIndexOfMainWord);

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
		else
			m_pWordCollectBoardRenderNode->PlayCharacterAnim(2, false);
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
	m_BoardViewMatrix[iRow][iColumn].m_iLetter = 255; //-1;
	m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite = NULL;
}

Sprite* HelloWorld::AddLetterToGem(const ComboEffectCell& cell)
{
	unsigned char iLetter;
	if (m_GameBoardManager.GetGameWordManager()->GenerateNewLetter( iLetter, cell.m_eGemComboType))	
	{			
		CCLOG("%d, %c:", iLetter, (unsigned char)iLetter);
		Sprite* pCharacterSprite = Sprite::createWithSpriteFrameName(
			m_pWordCollectBoardRenderNode->GetImageInGemFileFromLetter(iLetter).c_str());
				
		//pCharacterSprite->setScale(0.75f);
		//pCharacterSprite->setAnchorPoint(ccp(0,0));
		//pCharacterSprite->setPosition(ccp( 25.f, 25.f));
		pCharacterSprite->setPosition(ccp( 42.f, 42.f));

		switch(cell.m_iGemID)
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

		return pCharacterSprite;
	}
	else
	{
		m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iLetter = 255;//-1;

		return NULL;
	}
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

#include "FlashCardNode.h"

void HelloWorld::ShowMainWordUnlockEffect()
{
	m_pEndGameEffectLayer = LayerColor::create(ccc4( 0,0,0, 100));
	this->getParent()->addChild(m_pEndGameEffectLayer);

	float fDisplayEffectTime = 2.f;
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

	m_pWordCollectBoardRenderNode->PlaySpellingSound();

	/*
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
			NULL));*/

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
	auto pCombo5Anim = AnimationCache::getInstance()->getAnimation("effectCombo5");		
	Cell comboCell = pComboEffectBundle->m_ComboEffectDescription.m_Position;

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

	SoundManager::PlaySoundEffect(_SET_COMBINE_DOUBLE_COMBO_, fDelayTime-0.15f);
}