#include "HelloWorldScene.h"
#include "SoundManager.h"
#include "EndGameNode.h"
#include "LevelMapScene.h"
#include "Database\UserTable.h"
#include "Database\GameTracking.h"

USING_NS_CC;

Scene* HelloWorld::createScene(int iLevel)
{		
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
	auto backgroundLayer = LayerColor::create(ccc4(10, 10, 10, 255)); //64, 88, 119, 255));
	Sprite* pBackground = Sprite::create("Background.pvr.ccz");
	pBackground->setAnchorPoint(ccp(0,0));
	backgroundLayer->addChild(pBackground);

	scene->addChild(backgroundLayer);	

    // 'layer' is an autorelease object
    auto boardLayer = HelloWorld::create();	
	boardLayer->m_pHUDLayer = Layer::create();

    // add layer as a child to scene
    scene->addChild(boardLayer);

	// footer
	Sprite* pFooter = Sprite::create("Footer.pvr.ccz");
	pFooter->setAnchorPoint(ccp(0,0));
	boardLayer->m_pHUDLayer->addChild(pFooter);

	//
	boardLayer->m_pWordCollectBoardRenderNode = WordCollectBoardRenderNode::create();		

	// extra layer for score/stars...	
	boardLayer->m_pStatusLayer = StatusLayer::create();
	boardLayer->m_pStatusLayer->setScale(0.88f);
	boardLayer->m_pStatusLayer->setCurrentScore(0);
	boardLayer->m_pStatusLayer->setCurrentMove(0);
	boardLayer->m_pStatusLayer->setPosition(420.f, 765.f);
	boardLayer->m_pStatusLayer->setSpeedUpdateScore(120.f);
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

	m_pSettingNode = NULL;
	m_isShowSetting = false;

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
			m_fBoardBottomPosition = 60.f + 120.f;	
			m_fBoardBottomClipPosition = 60.f;	
			m_fMaskHeight = 800.f;
			break;
		case 7:
			m_fBoardBottomPosition = 60.f + 120.f;
			m_fBoardBottomClipPosition = 60.f;	
			m_fMaskHeight = 720.f;
			break;
		case 6:
			m_fBoardBottomPosition = 140.f + 120.f;
			m_fBoardBottomClipPosition = 140.f;	
			m_fMaskHeight = 640.f;
			break;
		case 5:
			m_fBoardBottomPosition = 140.f + 120.f;
			m_fBoardBottomClipPosition = 140.f;	
			m_fMaskHeight = 560.f;
			break;
	};

	switch( iNumberOfColumn)
	{
		case 8:
		default:
			m_fBoardLeftPosition = 0 + 40.f;			
			m_fBoardLeftClipPosition = 0.f;
			m_fMaskWidth = 640.f;
			break;
		case 7:		
			m_fBoardLeftPosition = 0.f + 120.f;			
			m_fBoardLeftClipPosition = 0.f;
			m_fMaskWidth = 640.f;
			break;
		case 6:		
			m_fBoardLeftPosition = 0.f + 120.f;			
			m_fBoardLeftClipPosition = 0.f;
			m_fMaskWidth = 640.f;
			break;
		case 5:		
			m_fBoardLeftPosition = 0.f + 120.f;			
			m_fBoardLeftClipPosition = 0.f;
			m_fMaskWidth = 560.f;
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
	m_SymbolSize = Point(80.f, 80.f); //pSprite->getContentSize() * 0.5;

	int iRow, iColumn, iFlag, iObstacleTypeID;
	int iObstacleTypeCount = GameConfigManager::getInstance()->GetObstacleTypeCount();
	ObstacleProcessManager* pObstacleProcessManager = m_GameBoardManager.GetObstacleProcessManager();

	memset( m_BoardObstaclesList, 0, sizeof(m_BoardObstaclesList));
	bool bIsGemContainLetter = false;


	// Add extra brushes at top and bottom rows
	Point position;
	for( iColumn = 0; iColumn < iNumberOfColumn; iColumn++)
	{
		// add top -2 brush
		pSprite = Sprite::createWithSpriteFrameName( "Bush.png");		
		position = Point(m_fBoardLeftPosition + iColumn * m_SymbolSize.width, m_fBoardBottomPosition + -2 * m_SymbolSize.height - 10.f);
		pSprite->setPosition(position);
		m_pBoardBatchNode->addChild(pSprite, GetZOrder(-2, iColumn, true));

		// add top brushes
		// add brush background
		pSprite = Sprite::createWithSpriteFrameName( "Bush_Background.png");		
		position = Point(m_fBoardLeftPosition + iColumn * m_SymbolSize.width, m_fBoardBottomPosition + -1 * m_SymbolSize.height - 10.f);
		pSprite->setPosition(position);
		m_pBoardBatchNode->addChild(pSprite);

		// add brush
		pSprite = Sprite::createWithSpriteFrameName( "Bush.png");		
		pSprite->setPosition(Point(position.x, position.y ));
		m_pBoardBatchNode->addChild(pSprite, GetZOrder(-1, iColumn, true));

		// add bot brushes
		// add brush background
		pSprite = Sprite::createWithSpriteFrameName( "Bush_Background.png");		
		position = Point(m_fBoardLeftPosition + iColumn * m_SymbolSize.width, m_fBoardBottomPosition + iNumberOfRow * m_SymbolSize.height - 10.f);
		pSprite->setPosition(position);
		m_pBoardBatchNode->addChild(pSprite);

		// add brush 
		pSprite = Sprite::createWithSpriteFrameName( "Bush.png");		
		pSprite->setPosition(Point(position.x, position.y));
		m_pBoardBatchNode->addChild(pSprite, GetZOrder( iNumberOfRow, iColumn, true));
	}

	Size spriteSize;

	for(iRow=0; iRow < iNumberOfRow; iRow++)
		for( iColumn = 0; iColumn < iNumberOfColumn; iColumn++)
		{
			if (m_GameBoardManager.IsBlankCell(iRow, iColumn))
			{
				m_BoardViewMatrix[iRow][iColumn].m_pSprite = NULL;
				m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite = NULL;

				pSprite = Sprite::createWithSpriteFrameName( "Bush_Background.png");		
				position = Point(m_fBoardLeftPosition + iColumn * m_SymbolSize.width, m_fBoardBottomPosition + iRow * m_SymbolSize.height - 10.f);
				pSprite->setPosition(position);
				m_pBoardBatchNode->addChild(pSprite);

				// add brush
				pSprite = Sprite::createWithSpriteFrameName( "Bush.png");		
				pSprite->setPosition(Point(position.x, position.y));
				m_pBoardBatchNode->addChild(pSprite, GetZOrder( iRow, iColumn, true));

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

					m_pBoardBatchNode->addChild(pSprite, GetZOrder( iRow, iColumn, false));

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
						spriteSize = pSprite->getContentSize();

						for(iObstacleTypeID = iObstacleTypeCount-1; iObstacleTypeID >=0; iObstacleTypeID--) // from low to highest priority
						{							
							AddObstacleToGem(pSprite, iBlockID, iObstacleTypeID);
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
	

	// add boss if this is versus mode
	m_pBossSprite = NULL;

	if (levelConfig.m_bEnableBoss)
	{
		m_pBossSprite = Sprite::createWithSpriteFrameName("BigAlien.png");
		m_pBossSprite->setPosition( ccp(m_fBoardLeftPosition + (levelConfig.m_BossConfig.m_Position.m_iColumn + levelConfig.m_BossConfig.m_iWidth/4.f)  * m_SymbolSize.width, 
			m_fBoardBottomPosition + (levelConfig.m_BossConfig.m_Position.m_iRow + levelConfig.m_BossConfig.m_iHeight/4.f) * m_SymbolSize.height));
		//pSprite->setScale(1.f);
		m_pBoardBatchNode->addChild(m_pBossSprite, _BOSS_ZORDER_);

		// add letter to boss
		AddNewLetterToBossSprite(0);

		// add HP Sprite to boss
		AddHitPointSpritesToBossSprite(0);
		/*
		unsigned char iLetter;
		if (m_GameBoardManager.GetGameWordManager()->GenerateLetterFromMainWord(iLetter))
		{
			Sprite* pLetterSprite = Sprite::createWithSpriteFrameName(
				m_pWordCollectBoardRenderNode->GetImageFileFromLetter(iLetter).c_str());
			pLetterSprite->setPosition(Point( m_pBossSprite->getContentSize().width/2.f - pLetterSprite->getContentSize().width * 1.6f /4.f, 90.f));
			pLetterSprite->setScale(1.6f);
			m_pBossSprite->setTag(iLetter);

			m_pBossSprite->addChild(pLetterSprite);
		}
		else
			m_pBossSprite->setTag(-1);

		// add hitpoint sprites
		const LevelBossInfo& levelBossInfo = m_GameBoardManager.GetLevelBossInfo();
		Sprite* pHitPointSprite;
		for(int i=0; i< levelBossInfo.m_iCurrentHitPoint;i++)
		{
			pHitPointSprite = Sprite::createWithSpriteFrameName("BossHitPoint.png");
			pHitPointSprite->setPosition( Point((i - levelBossInfo.m_iCurrentHitPoint/2.f + 0.5f ) * pHitPointSprite->getContentSize().width + m_SymbolSize.width, pHitPointSprite->getContentSize().height + 15.f));
			m_pBossSprite->addChild(pHitPointSprite);
		}*/
	}


	// create temp sprite, this's used to animate the drag line
	m_pTempSpriteForAction = Sprite::createWithSpriteFrameName( "Bush_Background.png");
	m_pTempSpriteForAction->retain();
	//m_pTempSpriteForAction->setAnchorPoint(ccp(0,0));
	m_pTempSpriteForAction->setPosition( ccp(0,0));
	m_pTempSpriteForAction->setScale(0.55f);
	
	m_pTempSpriteForAction->setVisible(false);
	m_pBoardBatchNode->addChild(m_pTempSpriteForAction);

	// play sound
	SoundManager::PlayBackgroundMusic(SoundManager::StateBackGroundMusic::kGameMusic);
	Breadcrumb::getInstance()->addSceneMode(SceneMode::kPlayGame);
}

void HelloWorld::menuCloseCallback(Object* pSender)
{
	if(m_pSettingNode == NULL)
	{
		m_pSettingNode = SettingMenuNode::create();
		m_pSettingNode->setPosition(Point(-500.0f, 0));
		this->getParent()->addChild(m_pSettingNode, 100);
	}

	if (m_isShowSetting == false)
	{
		m_isShowSetting = true;
		m_pSettingNode->show();
		this->setTouchEnabled(false);
	}
	else
	{
		m_isShowSetting = false;
		m_pSettingNode->hide();
		this->setTouchEnabled(true);
	}

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
	//	case _GCT_COMBO4_:
//			return "combo4.png";
		//case _GCT_COMBO4_2_:
		//	return "combo4_2.png";
	//	case _GCT_COMBO5_:
//			return "combo5.png";
		//case _GCT_COMBO5_2_:
			//return "combo5_2.png";
		case _GCT_COMBO6_:
			return "combo6.png";
		case _GCT_BONUS_END_GAME_SPECIAL_GEM_:
			return "Combo4_2.png";
		//case _GCT_COMBO6_2_:
			//return "combo6_2.png";
	}

	switch(iGemID)
	{
		case  _BONUS_QUEST_GEM_ID_:
			return "BonusQuestGem.png";			
		case _GT_CAT_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Orange_Cat.png"; //"Candy_Cam.png";
				case _GCT_COMBO4_:
					return "Orange_Combo_4.png";
				case _GCT_COMBO5_:				
					return "Orange_Combo_5.png";
				case _GCT_COMBO6_:				
					return "Orange_Combo6.png";
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "Orange_Letter.png";
				case _GCT_BONUS_END_GAME_CRAZY_PET_:
					return "Orange_Combo4_2.png"; 
			}

		}
		case _GT_BIRD_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Red_Bird.png"; //"Candy_Do.png";
				case _GCT_COMBO4_:
					return "Red_Combo_4.png";
				case _GCT_COMBO5_:				
					return "Red_Combo_5.png";
				case _GCT_COMBO6_:				
					return "Red_Combo6.png";
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "Red_Letter.png";
				case _GCT_BONUS_END_GAME_CRAZY_PET_:
					return "Red_Combo4_2.png";
			}			
		}
		case _GT_HAMSTER_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Pink_Hamster.png"; //"Candy_Tim.png";
				case _GCT_COMBO4_:
					return "Pink_Combo_4.png";
				case _GCT_COMBO5_:				
					return "Pink_Combo_5.png";
				case _GCT_COMBO6_:				
					return "Pink_Combo6.png";
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "Pink_Letter.png";
				case _GCT_BONUS_END_GAME_CRAZY_PET_:
					return "Pink_Combo4_2.png";
			}						
		}
		case _GT_SHEEP_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "White_Sheep.png"; //"Candy_Vang.png";
				case _GCT_COMBO4_:
					return "White_Combo_4.png";
				case _GCT_COMBO5_:				
					return "White_Combo_5.png";
				case _GCT_COMBO6_:				
					return "White_Combo6.png";
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "White_Letter.png";
				case _GCT_BONUS_END_GAME_CRAZY_PET_:
					return "White_Combo4_2.png";
			}									
		}
		case _GT_ELEPHANT_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Blue_Elephant.png"; //"Candy_XanhDuong.png";
				case _GCT_COMBO4_:
					return "Blue_Combo_4.png";		
				case _GCT_COMBO5_:				
					return "Blue_Combo_5.png";
				case _GCT_COMBO6_:				
					return "Blue_Combo6.png";
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "Blue_Letter.png";
				case _GCT_BONUS_END_GAME_CRAZY_PET_:
					return "Blue_Combo4_2.png";
			}												
		}
		case _GT_FROG_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Green_Frog.png"; //"Candy_XanhLa.png";
				case _GCT_COMBO4_:
					return "Green_Combo_4.png";
				case _GCT_COMBO5_:				
					return "Green_Combo_5.png";
				case _GCT_COMBO6_:				
					return "Green_Combo6.png";
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "Green_Letter.png";
				case _GCT_BONUS_END_GAME_CRAZY_PET_:
					return "Green_Combo4_2.png";
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

		m_eTouchMoveState = _TMS_BEGIN_IDENTIFY_;
		/*
		if (m_GameBoardManager.IsComboCell(iRow, iColumn) && m_GameBoardManager.GetObstacleBlockID(iRow, iColumn) < 0)
		{
			m_eTouchMoveState = _TMS_BEGIN_ACTIVATE_COMBO_;			
		}
		else
		{
			m_eTouchMoveState = _TMS_BEGIN_IDENTIFY_;
		}*/

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

	//if (m_eTouchMoveState < _TMS_BEGIN_ACTIVATE_COMBO_) //this is normal move
	{
		Point currentPosition = pTouch->getLocation();
		float fDeltaX = currentPosition.x - m_StartTouchPosition.x;
		float fDeltaY = currentPosition.y - m_StartTouchPosition.y;
		AdjustPosition( bIsBlocked, fDeltaX, fDeltaY, m_SelectedCell.m_iRow, m_SelectedCell.m_iColumn);
	}
	/*else if (m_eTouchMoveState != _TMS_BEGIN_ACTIVATE_COMBO_)
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
	}*/
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
	/*if (m_eTouchMoveState >= _TMS_BEGIN_ACTIVATE_COMBO_ && m_eTouchMoveState <= _TMS_ACTIVATE_COMBO_DOWN_)
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
	}*/
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
			m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_ComboChainList,
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
				m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_ComboChainList,
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
		// update obstacle manager right begin of move
		m_GameBoardManager.GetObstacleProcessManager()->UpdateAfterMove();

		PlayEffect2( false, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells,
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells,  m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells, true);
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

	if (m_GameBoardManager.RecheckAfterMoveV2( -1, -1, -1, -1, m_ComputeMoveResult.m_BasicMatchingDestroyedCells,
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells, false))
	{
		PlayEffect2( false, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells,
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells,  m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells, false);		
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
		//m_GameBoardManager.GetObstacleProcessManager()->UpdateAfterMove();

		//UpdateObstacleListAfterMove();
	}	
}

void HelloWorld::ExecuteBonusWinGameEffect()
{
	m_ComputeMoveResult.Reset(true);

	//if (m_GameBoardManager.GetCurrentMove() > 0)
	if (m_GameBoardManager.ExecuteEndGameBonus(m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells, 
		m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells))
	{
		PlayEffect2( true, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells,
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells,  m_ComputeMoveResult.m_UnlockedLetterCells,  m_ComputeMoveResult.m_NewCells, false);

		m_pStatusLayer->setCurrentMove( m_GameBoardManager.GetCurrentMove());
		//m_pStatusLayer->update(0);
	}
	else
	{
		//PlayUnlockLettersOfBonusWordsAnimation();
		EndUnlockLetterAnimation();		
	}
}

void HelloWorld::ShowWinGamePopup()
{
	std::vector<Word> subWordList;	
	EndGameNode* pEndGameNode = EndGameNode::createLayoutWin( m_GameBoardManager.GetCurrentScore(),
		m_GameBoardManager.GetGameWordManager()->GetMainWord(), m_GameBoardManager.GetCurrentLevel());
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
	if (m_eTouchMoveState != _TMS_MOVE_HORIZONTAL_)
		return;

	fMoveUnit = fDeltaX/m_SymbolSize.width;
				
	fMoveUnit = round(fMoveUnit);

	iMoveUnit = (int)fMoveUnit;

	if ( iMoveUnit == m_iSaveLastCellMoveDelta || iMoveUnit == 0 || iMoveUnit + iBlankCellOfFullLine == m_GameBoardManager.GetColumnNumber())
	{		
		if ( (iMoveUnit == 0 || iMoveUnit + iBlankCellOfFullLine == m_GameBoardManager.GetColumnNumber())  && iMoveUnit != m_iSaveLastCellMoveDelta && m_HintSprites.size() >0 )
		{
			CCLOG("Remove hint");
			for(auto sprite:m_HintSprites)
				sprite->removeFromParentAndCleanup(true);
		
			m_HintSprites.clear();
		}
		return;
	}

	CCLOG("MOve unit: %d", iMoveUnit);

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
	if (m_eTouchMoveState != _TMS_MOVE_VERTICAL_)
		return;

	fMoveUnit = fDeltaY/m_SymbolSize.height;
				
	fMoveUnit = round(fMoveUnit);

	iMoveUnit = (int)fMoveUnit;
	if ( iMoveUnit == m_iSaveLastCellMoveDelta || iMoveUnit == 0 || iMoveUnit + iBlankCellOfFullLine == m_GameBoardManager.GetRowNumber())
	{		
		if ( (iMoveUnit == 0 || iMoveUnit + iBlankCellOfFullLine == m_GameBoardManager.GetRowNumber()) && iMoveUnit != m_iSaveLastCellMoveDelta && m_HintSprites.size() >0 )
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
		std::vector<Cell>& basicMatchingDestroyedCells,
		std::vector<ComboEffectBundle*>& comboChainList,
		std::vector<ComboEffectCell>& newComboCells, std::vector<Cell>& originalMovedCells, std::vector<Cell>& targetMovedCells,		
		std::vector<NewCellInfo>& unlockedLetterCells, std::vector<NewCellInfo>& newCells, bool bIsNewMove)
{
	// for test	
	//for(int i=0; i< _COMBO_TYPE_COUNT_;i++)
	//	m_pComboCountRenderNode->UpdateList(i, m_GameBoardManager.GetComboCount(i));

	m_bIsEffectPlaying = true;	

	float fDelayTime = 0.2f;	
	
	int iNumberOfRow = m_GameBoardManager.GetRowNumber();
	int iNumberOfColumn = m_GameBoardManager.GetColumnNumber();

	//if (bIsBonusEndGamePhase)
		//fDelayTime = 0;

	float fTotalDestroyCellTime = fDelayTime+ _TME_BASIC_DESTROY_CELL_TIME_;
	//CCLOG("Begin effect");

	if (bIsBonusEndGamePhase)
	{
		// play effect convert normal cells to combo cells
		if (convertedToComboCells.size() > 0) // && convertedToComboCells[0].m_eGemComboType != _GCT_BONUS_END_GAME_CRAZY_PET_) //temporary disable transform of crasy pet
		{
			float fDelayPerConvertedCell = 0.05f;
			auto pCombo5AnimBolt = AnimationCache::getInstance()->getAnimation("effectCombo5_Bolt");		
			Point rootEffect( 530.f, 820.f);
			int iIndex = 0;

			for(auto cell: convertedToComboCells)
			{
				Point pos(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height);

				if (convertedToComboCells[0].m_eGemComboType != _GCT_BONUS_END_GAME_CRAZY_PET_)
				{
					BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, iIndex * fDelayPerConvertedCell, _TME_BASIC_DESTROY_CELL_TIME_);

					// create combo cell
					Sprite* pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(cell.m_iGemID, cell.m_eGemComboType).c_str());
					//pSprite->setAnchorPoint(ccp(0,0));					
					pSprite->setPosition(pos);

					//pSprite->setScale(0.65f);
					m_pBoardBatchNode->addChild(pSprite, GetZOrder( cell.m_iRow, cell.m_iColumn, false));

					m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;

					pSprite->setOpacity(0);
					pSprite->runAction( 
						Sequence::create( 
							DelayTime::create( iIndex * fDelayPerConvertedCell),
							FadeIn::create(_TME_BASIC_DESTROY_CELL_TIME_/2.f),
							//EaseIn::create( FadeIn::create(_TME_BASIC_DESTROY_CELL_TIME_/3.f), 2.f),
							NULL));

					pSprite->setScale(2.f);//1.5f);
					pSprite->runAction( 
						Sequence::create( 
							DelayTime::create( iIndex * fDelayPerConvertedCell),
							ScaleTo::create(_TME_BASIC_DESTROY_CELL_TIME_/2.f, 1.f, 1.f),
							//EaseIn::create(ScaleTo::create(_TME_BASIC_DESTROY_CELL_TIME_/3.f, 1.f, 1.f), 2.f),
							NULL));					
				}
				else
					PlayChangeColorEffectOnSprite( m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite, iIndex * fDelayPerConvertedCell );

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
						DelayTime::create( iIndex * fDelayPerConvertedCell),
						Animate::create( pCombo5AnimBolt),
						RemoveSelf::create(),
						NULL));
								
				iIndex++;
			}

			fDelayTime += _TME_BASIC_DESTROY_CELL_TIME_  + (iIndex+3) * fDelayPerConvertedCell;			
		}
	}


	// play sound
	if (basicMatchingDestroyedCells.size() > 0)
		SoundManager::PlaySoundEffect(_SET_SIMPLE_MATCH_);

	//CCLOG("Destroy cells");

	// destroy cells by basic matching 
	int iTotalDestroyCell = 0;
	for(auto cell: basicMatchingDestroyedCells)
	{			
		if (cell.m_iRow >= 0 && cell.m_iColumn >= 0)
		{
			iTotalDestroyCell++;
			BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime,_TME_BASIC_DESTROY_CELL_TIME_, true);
		}
		else
		{
			cell.m_iRow = 0;
		}		
	}

	// increase score by basic matching
	m_GameBoardManager.IncreaseScoreForDestroyCells( iTotalDestroyCell, _CET_BASIC_MATCHING_);

	// play combo chain	
	float fCurrentDelayComboChain;
	for(auto pComboEffect : comboChainList)
	{
		fCurrentDelayComboChain = fDelayTime + pComboEffect->m_fTriggerTime;				

		// play effect
		if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_ROW_ || pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLUMN_)
			PlayCombo4Effect(pComboEffect, fCurrentDelayComboChain, _TME_BASIC_COMBO_EXECUTE_TIME_);
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_EXPLOSION_ || pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_EXPLOSION_SECOND_)
		{
			PlayCombo5Effect(pComboEffect, fCurrentDelayComboChain, _TME_BASIC_COMBO_EXECUTE_TIME_);

			if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_EXPLOSION_)
			{
				PlayChangeColorEffectOnSprite( m_BoardViewMatrix[ pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite, fCurrentDelayComboChain);
			}
		}
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_4_4_EFFECT_)
		{
			PlayCombo4_4Effect(pComboEffect, fCurrentDelayComboChain, _TME_BASIC_COMBO_EXECUTE_TIME_);
		}
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_4_4_4_EFFECT_)
			PlayCombo4_4_4Effect(pComboEffect, fCurrentDelayComboChain, _TME_BASIC_COMBO_EXECUTE_TIME_);
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLOR_ROW_ || pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_COLOR_COLUMN_)
		{
			if (pComboEffect->m_iNormalChainPhase < 0) // trigger 1 time, play change color effect
			{
				PlayChangeColorEffectOnSprite( m_BoardViewMatrix[ pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite, fCurrentDelayComboChain);
			}
			else
				PlayCombo6Effect(pComboEffect, fCurrentDelayComboChain);
		}
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_4_5_EFFECT_) 
		{
			PlayCombo4_5Effect(pComboEffect, fCurrentDelayComboChain);
		}
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_5_5_EFFECT_ || pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_5_5_SECOND_EFFECT_) //big explosion
		{
			PlayCombo5_5Effect(pComboEffect, fCurrentDelayComboChain, _TME_BASIC_COMBO_EXECUTE_TIME_);			

			if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_5_5_EFFECT_)
			{
				PlayChangeColorEffectOnSprite( m_BoardViewMatrix[ pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite, fCurrentDelayComboChain);
			}
		}
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_5_5_5_EFFECT_) 
		{
			PlayCombo5_5_5Effect(pComboEffect, fCurrentDelayComboChain);
		}		
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_6_EFFECT_ || pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_6_6_EFFECT_
			|| pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_6_6_SECOND_EFFECT_)
		{
			PlayCombo6_6Effect(pComboEffect, fCurrentDelayComboChain);
			if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_6_6_EFFECT_)
			{
				PlayChangeColorEffectOnSprite( m_BoardViewMatrix[ pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite, fCurrentDelayComboChain);
			}
		}
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_BONUS_END_GAME__SPECIAL_GEM_EFFECT_)
		{
			PlayComboEndGameBonusEffect(pComboEffect, fCurrentDelayComboChain, _TME_BASIC_COMBO_EXECUTE_TIME_);
		}
		else  if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType >= _CET_BIRD_EXPLOSION_EFFECT_ && pComboEffect->m_ComboEffectDescription.m_eComboEffectType <= _CET_HAMSTER_RUN_RIGHT_DOWN_EFFECT_)
		{
			PlayCrazyPetEndGameBonusEffect(pComboEffect, fCurrentDelayComboChain, _TME_BASIC_COMBO_EXECUTE_TIME_);
		}


		// ******************************
		if (pComboEffect->m_fTriggerTime + _TME_BASIC_COMBO_EXECUTE_TIME_ + fDelayTime> fTotalDestroyCellTime)
			fTotalDestroyCellTime =  fDelayTime + pComboEffect->m_fTriggerTime + _TME_BASIC_COMBO_EXECUTE_TIME_;		


		// destroy cells by combo
		ComboEffectCell newComboCell;
		if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_4_EFFECT_ || pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_5_EFFECT_ ) 
		{
			newComboCell.m_eGemComboType = (pComboEffect->m_ComboEffectDescription.m_eComboEffectType==_CET_6_4_EFFECT_)?_GCT_COMBO4_:_GCT_COMBO5_;
			newComboCell.m_iGemID = pComboEffect->m_ComboEffectDescription.m_iGemID;
		}

		iTotalDestroyCell = 0;
		for(auto cell: pComboEffect->m_DestroyedCells)
		{
			if (cell.m_iDestroyPhaseIndex >=0 && (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_4_EFFECT_ || pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_5_EFFECT_ ))
			{			
				newComboCell.m_iRow = cell.m_iRow;
				newComboCell.m_iColumn = cell.m_iColumn;
				BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime + cell.m_fDestroyAtTime, _TME_BASIC_DESTROY_CELL_TIME_); //0.03f);				
				
				AddNewComboCell (newComboCell, fDelayTime + cell.m_fDestroyAtTime, 0.03f, false);
			}
			else if (cell.m_bIsCompleteDestroyed)
			{
				if (cell.m_bIsEarnScore)
					iTotalDestroyCell++;

				BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime + cell.m_fDestroyAtTime, _TME_BASIC_DESTROY_CELL_TIME_);
				
				if (fDelayTime + cell.m_fDestroyAtTime + _TME_BASIC_DESTROY_CELL_TIME_ > fTotalDestroyCellTime)
					fTotalDestroyCellTime = fDelayTime + cell.m_fDestroyAtTime + _TME_BASIC_DESTROY_CELL_TIME_;
			}		
		}

		// increase score by combo		
		m_GameBoardManager.IncreaseScoreForDestroyCells( iTotalDestroyCell, pComboEffect->m_ComboEffectDescription.m_eComboEffectType, pComboEffect->m_ComboEffectDescription.m_iActivationScoreRatio);

		// clean data
		delete pComboEffect;
	}

	// check logic on bonus quest
	if (!m_bIsEndGamePhase)
	{
		// check destroy bonus gem list
		std::vector<DestroyedByComboCell>& destroyBonusQuestGemList = m_GameBoardManager.GetDestroyBonusQuestGemList();
		//iTotalDestroyCell= 0;
		if (destroyBonusQuestGemList.size() > 0)
		{
			for(auto cell: destroyBonusQuestGemList)
			{			
				//iTotalDestroyCell++;
				BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, cell.m_fDestroyAtTime,_TME_BASIC_DESTROY_CELL_TIME_);
			}

			// check for new bonus quests those're just activated
			if (m_GameBoardManager.GetBonusQuestManager()->IsQuestJustActivated(_BQT_COLLECT_GEM_))
				m_pComboCountRenderNode->GenerateLabels(_BQT_COLLECT_GEM_);
			if (m_GameBoardManager.GetBonusQuestManager()->IsQuestJustActivated(_BQT_COLLECT_COMBO_))
				m_pComboCountRenderNode->GenerateLabels(_BQT_COLLECT_COMBO_);
		
			destroyBonusQuestGemList.clear();
		}		
	}

	// Quest still can be completed at bonus phase
	// update state of bonus quest here (for test only)
	if (m_GameBoardManager.GetBonusQuestManager()->IsQuestActivated(_BQT_COLLECT_GEM_) && !m_GameBoardManager.GetBonusQuestManager()->IsQuestCompleted(_BQT_COLLECT_GEM_))
	{
		auto& collectGemParam = m_GameBoardManager.GetBonusQuestManager()->GetCollectGemParam();
		for(int i=0; i < _MAX_GEM_ID_; i++)
			m_pComboCountRenderNode->UpdateGemList(i, collectGemParam.m_CountPerGemType[i]);
	}
	else if (m_GameBoardManager.GetBonusQuestManager()->IsQuestJustCompleted(_BQT_COLLECT_GEM_) )
	{
		m_GameBoardManager.IncreaseScoreForCompleteBonusQuest();
		m_pComboCountRenderNode->CompleteQuest(_BQT_COLLECT_GEM_);
	}

	if (m_GameBoardManager.GetBonusQuestManager()->IsQuestActivated(_BQT_COLLECT_COMBO_)  && !m_GameBoardManager.GetBonusQuestManager()->IsQuestCompleted(_BQT_COLLECT_COMBO_))
	{
		auto& collectComboParam = m_GameBoardManager.GetBonusQuestManager()->GetCollectComboParam();
		for(int i=0; i < _GCT_SINGLE_COMBO_COUNT_; i++)
			m_pComboCountRenderNode->UpdateComboList(i, collectComboParam.m_CountPerComboGem[i]);
	}
	else if (m_GameBoardManager.GetBonusQuestManager()->IsQuestJustCompleted(_BQT_COLLECT_COMBO_))
	{
		m_GameBoardManager.IncreaseScoreForCompleteBonusQuest();
		m_pComboCountRenderNode->CompleteQuest(_BQT_COLLECT_COMBO_);
	}	


	// compute effect on boss
	if (m_GameBoardManager.GetLevelBossInfo().m_bIsEnable)
	{
		if (m_GameBoardManager.IsBossStateChanged())
		{
			const LevelBossInfo& levelBossInfo = m_GameBoardManager.GetLevelBossInfo();
			float fDelayEffectTime = fDelayTime + levelBossInfo.m_fBossStateChangeAtTime;

			// remove 1 hp
			Remove1HPFromBossSprite(fDelayEffectTime);

			if (levelBossInfo.m_bJustReleaseALetter)
			{
				// play sound effect 
				SoundManager::PlaySoundEffect(_SET_GET_CHARACTER_);

				// remove current letter sprite
				RemoveLetterFromBossSprite(fDelayEffectTime);

				// play effect unlock letter
				unsigned char iLetter;
				if (m_pBossSprite->getTag() > 0)
				{
					iLetter = (unsigned char) m_pBossSprite->getTag();
					int iUnlockedLetterOfMainWord;					
					bool bIsMainWordJustUnlocked;					

					if (m_GameBoardManager.GetGameWordManager()->UnlockLetter( iLetter,  iUnlockedLetterOfMainWord,  bIsMainWordJustUnlocked))
					{
						const LevelBossConfig& bossLevelConfig = m_GameBoardManager.GetLevelConfig().m_BossConfig;

						m_pWordCollectBoardRenderNode->PlayUnlockLetterEffect(fDelayEffectTime, iLetter,  //m_BoardViewMatrix[iRow][iColumn].m_iLetter, 
							ccp(m_fBoardLeftPosition + (  bossLevelConfig.m_Position.m_iColumn + bossLevelConfig.m_iWidth/4.f)  * m_SymbolSize.width, 
								m_fBoardBottomPosition + (  bossLevelConfig.m_Position.m_iRow + bossLevelConfig.m_iHeight/4.f) * m_SymbolSize.height));

						m_pWordCollectBoardRenderNode->UnlockLetter(iUnlockedLetterOfMainWord);

						if (bIsMainWordJustUnlocked)
							m_GameBoardManager.IncreaseScoreForLetterInMainWord();
					}
				}

				// generate new letter
				AddNewLetterToBossSprite(fDelayEffectTime);

				// add HP Sprite to boss
				AddHitPointSpritesToBossSprite(fDelayEffectTime);
			}
			else
			{				
				
			}

			// play effect on fire on boss
			auto flashLightEffect = 
				Sequence::create(
					DelayTime::create(fDelayEffectTime),
					Repeat::create(
						Sequence::create(
							TintTo::create( 0.15f, 100, 200, 100),
							TintTo::create( 0.15f, 255, 255, 155),
							NULL), 2),
					NULL);
				
			flashLightEffect->setTag(-100);
			m_pBossSprite->runAction(flashLightEffect);


			if (levelBossInfo.m_iRemainLettersCount == 0)
				m_GameBoardManager.KillBoss();
		}
	}

	// create new combo cells, this is right after basic matching cells are destroyed
	for(auto cell: newComboCells)
	{
		AddNewComboCell (cell, fDelayTime, _TME_BASIC_DESTROY_CELL_TIME_);			

		m_GameBoardManager.IncreaseScoreForCreateCombo(cell.m_eGemComboType);
	}

	// create gem with unlock letters ==> chua implement dung timing
	GemLetterData data;
	Sprite* pSprite;
	for(auto& cell : unlockedLetterCells)
	{
		for(int i=0; i< 2; i++)
		{								
			if (i!= 0)
			{								
				pSprite = m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite;
			}
			else
			{
				pSprite = m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite;				
			}

			assert(pSprite!= NULL);

			if (cell.m_bIsUnlocked) // && pSprite != NULL) // safe check
			{
				// remove old question mark sprite
				Sprite* pOldChildSprite = (Sprite*)pSprite->getChildren()->getLastObject();
				if (pOldChildSprite != NULL)
					pOldChildSprite->removeFromParentAndCleanup(true);

				if (cell.m_iGemLetterBlockID >= 0) //iLetter < 255)
				{
					data = m_GameBoardManager.GetGemLetterData(cell.m_iGemLetterBlockID);							
					AddLetterToGem( cell, m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), data.m_iLetter, cell.m_iGemLetterBlockID);			
				}
			}
					
			//m_pBoardBatchNode->addChild(pSprite);						
		}		
	}
	
	// move cells
	int iUpdatedZOrder;
	for (int i=0; i < originalMovedCells.size(); i++)
	{		
		//if ( m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn].m_pSprite == NULL)
		//	CCLOG("3- Effect NULL, %d, %d", originalMovedCells[i].m_iRow, originalMovedCells[i].m_iColumn);
		m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn].m_pSprite->runAction(
			Sequence::create( 
				DelayTime::create(fTotalDestroyCellTime + 0.027f * originalMovedCells[i].m_iRow), // + 0.01f * (targetMovedCells[i].m_iRow - originalMovedCells[i].m_iRow ),
				EaseOut::create( MoveTo::create( _TME_MOVE_CELL_TIME_,
					ccp(m_fBoardLeftPosition + targetMovedCells[i].m_iColumn * m_SymbolSize.width, 
					m_fBoardBottomPosition + targetMovedCells[i].m_iRow * m_SymbolSize.height)),			1.f),
				NULL));




		m_BoardViewMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn] = m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn];				
		m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn] = m_BoardViewMirrorMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn];					

		iUpdatedZOrder = GetZOrder(targetMovedCells[i].m_iRow, targetMovedCells[i].m_iColumn, false);

		m_BoardViewMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite->setZOrder( iUpdatedZOrder);		
		if (m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite != NULL)
			m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite->setZOrder(iUpdatedZOrder);
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
					//m_fBoardBottomPosition + (iNumberOfRow + 1) * m_SymbolSize.height));
					m_fBoardBottomPosition + (cell.m_iRow + iNumberOfRow) * m_SymbolSize.height));
				pSprite->runAction(
					Sequence::create(
						DelayTime::create(fTotalDestroyCellTime + 0.027f* (m_GameBoardManager.GetRowNumber() - cell.m_iRow)),
						MoveTo::create(_TME_MOVE_CELL_TIME_,
							ccp(m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, 
									m_fBoardBottomPosition + cell.m_iRow  * m_SymbolSize.height)),
						//EaseOut::create( MoveTo::create(_TME_MOVE_CELL_TIME_,
							//ccp(m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, 
								//	m_fBoardBottomPosition + cell.m_iRow  * m_SymbolSize.height)), 2.f),
							NULL));

				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iGemLetterBlockID = -1; //reset letter of gem				
			}
			//pSprite->setScale(0.65f);

			if (cell.m_iGemLetterBlockID >= 0)
			{
				AddLetterToGem( cell, cell.m_iGemID, '?', -1, (i!=0));				
			}


			/*if (cell.m_bIsUnlocked)
			{
				if (cell.m_iGemLetterBlockID >= 0) //iLetter < 255)
				{
					data = m_GameBoardManager.GetGemLetterData(cell.m_iGemLetterBlockID);							
					AddLetterToGem( cell, m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), data.m_iLetter);			
				}
			}*/
					
			m_pBoardBatchNode->addChild(pSprite, GetZOrder( cell.m_iRow, cell.m_iColumn, false));
		}		
	}
		

	// last check: activation of collect word bonus quest
	if (!bIsBonusEndGamePhase)
	{
		if (m_GameBoardManager.GetBonusQuestManager()->IsQuestJustActivated(_BQT_COLLECT_BONUS_WORD_))
		{			
			auto& collectBonusWordParam = m_GameBoardManager.GetBonusQuestManager()->GetCollectBonusWordParam();
			m_pComboCountRenderNode->GenerateLabels(_BQT_COLLECT_BONUS_WORD_, collectBonusWordParam.m_iRemainLettersCount);

			std::vector<NewCellInfo> bonusWordCellList;
			m_GameBoardManager.CheckAndActivateBonusWordQuest(bonusWordCellList);

			fTotalDestroyCellTime += _TME_MOVE_CELL_TIME_ * 1.4f + 0.3f;
			GemLetterData data;
			Sprite* pSprite;

			for(auto& cell:bonusWordCellList)
			{
				BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fTotalDestroyCellTime, _TME_BASIC_DESTROY_CELL_TIME_);

				data = m_GameBoardManager.GetGemLetterData(cell.m_iGemLetterBlockID);	
				// create new cell contain letter				
				for(int i=0; i< 2; i++)
				{				
					pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn)).c_str());
					Point pos(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height);
					pSprite->setPosition(pos);
					m_pBoardBatchNode->addChild(pSprite,  GetZOrder( cell.m_iRow, cell.m_iColumn, false));

					if (i==0)
					{
						m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
						Sprite* letterSprite = AddLetterToGem(cell, m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), data.m_iLetter, cell.m_iGemLetterBlockID, false);

						// delay fade in
						pSprite->setOpacity(0);
						pSprite->runAction( 
							Sequence::create( 
								DelayTime::create(fTotalDestroyCellTime),
								EaseIn::create( FadeIn::create(_TME_BASIC_DESTROY_CELL_TIME_), 2.f),
								NULL));

						letterSprite->setOpacity(0);
						letterSprite->runAction( 
							Sequence::create( 
								DelayTime::create(fTotalDestroyCellTime),
								EaseIn::create( FadeIn::create(_TME_BASIC_DESTROY_CELL_TIME_), 2.f),
								NULL));
					}
					else
					{
						pSprite->setVisible(false);
						m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;					
						AddLetterToGem(cell, m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), data.m_iLetter, cell.m_iGemLetterBlockID, true);
					}					
				}
			}
		}
	}
	
	// quest still can be completed at bonus phase
	if ((m_GameBoardManager.GetBonusQuestManager()->IsQuestActivated(_BQT_COLLECT_BONUS_WORD_) && !m_GameBoardManager.GetBonusQuestManager()->IsQuestCompleted(_BQT_COLLECT_BONUS_WORD_))
		|| m_GameBoardManager.GetBonusQuestManager()->IsQuestJustCompleted(_BQT_COLLECT_BONUS_WORD_))
	{
		// update display info
		auto& collectBonusWordParam = m_GameBoardManager.GetBonusQuestManager()->GetCollectBonusWordParam();
		m_pComboCountRenderNode->UpdateBonusWordQuest( collectBonusWordParam.m_iRemainLettersCount);

		if (m_GameBoardManager.GetBonusQuestManager()->IsQuestCompleted(_BQT_COLLECT_BONUS_WORD_))
		{
			m_GameBoardManager.IncreaseScoreForCompleteBonusQuest();
			m_pComboCountRenderNode->CompleteQuest(_BQT_COLLECT_BONUS_WORD_);
		}
		
	}


	if (bIsBonusEndGamePhase)
	{
		this->runAction( CCSequence::create(
					CCDelayTime::create(fTotalDestroyCellTime+ _TME_MOVE_CELL_TIME_ *4),
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
						CCDelayTime::create(fTotalDestroyCellTime + _TME_MOVE_CELL_TIME_*2),
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

// utility functions on boss sprite
void HelloWorld::AddNewLetterToBossSprite(const float& fDelayTime)
{
	unsigned char iLetter;
	if (m_GameBoardManager.GetGameWordManager()->GenerateLetterFromMainWord(iLetter))
	{
		Sprite* pLetterSprite = Sprite::createWithSpriteFrameName(
			m_pWordCollectBoardRenderNode->GetImageFileFromLetter(iLetter).c_str());
		pLetterSprite->setPosition(Point( m_pBossSprite->getContentSize().width/2.f - pLetterSprite->getContentSize().width * 1.6f /4.f, 90.f));
		pLetterSprite->setScale(1.6f);
		m_pBossSprite->setTag(iLetter);

		m_pBossSprite->addChild(pLetterSprite);
		
		if (fDelayTime > 0)
		{
			pLetterSprite->setOpacity(0);
			pLetterSprite->runAction(
				Sequence::create(
					DelayTime::create(fDelayTime),
					FadeTo::create( 0.1f, 255),
					NULL));
		}
	}
	else
		m_pBossSprite->setTag(-1);
}

void HelloWorld::AddNewComboCell(const ComboEffectCell& cell, const float& fDelayTime, const float& fEffectTime, bool bCreateMirror)
{
	int iSize = 1;
	if (bCreateMirror)
		iSize = 2;
	for(int i=0; i< iSize; i++)
	{
		Sprite* pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(cell.m_iGemID, cell.m_eGemComboType).c_str());
		//pSprite->setAnchorPoint(ccp(0,0));

		pSprite->setPosition( ccp(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height));

		//pSprite->setScale(0.65f);
		m_pBoardBatchNode->addChild(pSprite,  GetZOrder( cell.m_iRow, cell.m_iColumn, false));

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
					EaseIn::create( FadeIn::create(fEffectTime), 2.f),
					NULL));

			pSprite->setScale(2.f);//1.5f);
			pSprite->runAction( 
				Sequence::create( 
					DelayTime::create(fDelayTime),
					EaseIn::create(ScaleTo::create(fEffectTime, 1.f, 1.f), 2.f),
					NULL));										
		}			
	}
}

void HelloWorld::AddHitPointSpritesToBossSprite(const float& fDelayTime)
{
	const LevelBossInfo& levelBossInfo = m_GameBoardManager.GetLevelBossInfo();
	Sprite* pHitPointSprite;
	for(int i=0; i< levelBossInfo.m_iCurrentHitPoint;i++)
	{
		pHitPointSprite = Sprite::createWithSpriteFrameName("BossHitPoint.png");
		pHitPointSprite->setPosition( Point((i - levelBossInfo.m_iCurrentHitPoint/2.f + 0.5f ) * pHitPointSprite->getContentSize().width + m_SymbolSize.width, pHitPointSprite->getContentSize().height + 15.f));
		m_pBossSprite->addChild(pHitPointSprite);

		if (fDelayTime > 0)
		{
			pHitPointSprite->setOpacity(0);
			pHitPointSprite->runAction(
				Sequence::create(
					DelayTime::create(fDelayTime),
					FadeTo::create( 0.1f, 255),
					NULL));
		}
	}
}

void HelloWorld::RemoveLetterFromBossSprite(const float& fDelayTime)
{
	if (m_pBossSprite->getChildrenCount() > 1)
	{
		Sprite* pLetterSprite = (Sprite*)m_pBossSprite->getChildren()->getObjectAtIndex(0);
		pLetterSprite->runAction(
			Sequence::create( 
			DelayTime::create(fDelayTime - 0.05f),
				RemoveSelf::create(),
				NULL));
	}
}

void HelloWorld::Remove1HPFromBossSprite(const float& fDelayTime)
{
	// remove 1 hp
	if (m_pBossSprite->getChildrenCount() > 1)
	{
		Sprite* pLastHPSprite = (Sprite*)m_pBossSprite->getChildren()->getLastObject();
		pLastHPSprite->runAction(
			Sequence::create( 
			DelayTime::create(fDelayTime),
				RemoveSelf::create(),
				NULL));
	}
}

void HelloWorld::UpdateObstacleListAfterMove()
{
	int iObstacleTypeCount = GameConfigManager::getInstance()->GetObstacleTypeCount();
	int iNumberOfRow = m_GameBoardManager.GetRowNumber();
	int iNumberOfColumn = m_GameBoardManager.GetColumnNumber();

	int iRow, iColumn, iBlockID, iObstacleTypeID;
	ObstacleProcessManager*	pObstacleProcessManager = m_GameBoardManager.GetObstacleProcessManager();


	// remove waiting clear list
	std::vector<int>& waitingClearBlockList = pObstacleProcessManager->GetBlockWaitingClearList();
	for(auto iBlockID : waitingClearBlockList)
	{
		for(iObstacleTypeID = 0; iObstacleTypeID < iObstacleTypeCount; iObstacleTypeID++)
		{
			m_BoardObstaclesList[iBlockID][iObstacleTypeID] = NULL;
		}
	}

	// update dirty list
	Size spriteSize;
	for(iRow =0; iRow < iNumberOfRow; iRow++)
		for(iColumn =0; iColumn < iNumberOfColumn; iColumn++)
		{
			iBlockID = m_GameBoardManager.GetObstacleBlockID(iRow, iColumn);
			if (iBlockID < 0)
				continue;

			for(iObstacleTypeID = 0; iObstacleTypeID < iObstacleTypeCount; iObstacleTypeID++)
			{
				ObstacleData& obstacleData = pObstacleProcessManager->GetObstacleData(iBlockID, iObstacleTypeID);
				if (obstacleData.m_bIsDirty)
				{
					if (m_BoardObstaclesList[iBlockID][iObstacleTypeID] != NULL)
					{	
						m_BoardObstaclesList[iBlockID][iObstacleTypeID]->runAction(
							Sequence::createWithTwoActions( 
								ScaleTo::create( 0.25f, 1.1f, 1.1f),
								RemoveSelf::create()));

						Object* childNode = NULL;
						CCARRAY_FOREACH( m_BoardObstaclesList[iBlockID][iObstacleTypeID]->getChildren(), childNode) 
							((Node*)childNode)->runAction(
								ScaleTo::create( 0.25f, 1.6f, 1.6f));


						//m_BoardObstaclesList[iBlockID][iObstacleTypeID]->removeFromParentAndCleanup(true);
						m_BoardObstaclesList[iBlockID][iObstacleTypeID] = NULL;

						AddObstacleToGem(m_BoardViewMatrix[iRow][iColumn].m_pSprite, iBlockID, iObstacleTypeID, true);
					}											
					else
						AddObstacleToGem(m_BoardViewMatrix[iRow][iColumn].m_pSprite, iBlockID, iObstacleTypeID, false);
					
					obstacleData.m_bIsDirty = false;						
				}
			}
		}
}

void HelloWorld::PlayChangeColorEffectOnSprite(Sprite* pSprite,const float& fDelayTime)
{
	if (pSprite != NULL)
	{
		auto flashLightEffect = RepeatForever::create(
			Sequence::create(
				DelayTime::create( fDelayTime),
				TintTo::create( 0.2f, 100, 200, 100),
				TintTo::create( 0.2f, 255, 255, 155),
				NULL));
		flashLightEffect->setTag(-100);

		pSprite->runAction(flashLightEffect);
	}
}

void HelloWorld::ActivateImageEffect(Node* pSender)
{
	((Sprite*)pSender)->setOpacity(180);
}

void HelloWorld::BasicDestroyCellUlti(const int& iRow, const int & iColumn, const float& fDelay, const float fEffectDuration, bool bPlaySeparateEffect)
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

	if (bPlaySeparateEffect)
		PlaySplashLightDestroyEffectOnCell( iRow, iColumn, fDelay, fEffectDuration);

	if (m_BoardViewMatrix[iRow][iColumn].m_iGemLetterBlockID >= 0)  //m_iLetter < 255)
	{	
		GemLetterData gemLetterData = m_GameBoardManager.FreeGemLetterBlock(m_BoardViewMatrix[iRow][iColumn].m_iGemLetterBlockID);


		m_pWordCollectBoardRenderNode->PlayUnlockLetterEffect(fDelay, gemLetterData.m_iLetter,  //m_BoardViewMatrix[iRow][iColumn].m_iLetter, 
			ccp(m_fBoardLeftPosition + iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + iRow * m_SymbolSize.height));
		
		//m_pWordCollectBoardRenderNode->PlayCharacterAnim(2, false);


		int iUnlockedLetterIndexOfMainWord;		
		bool bIsMainWordJustUnlocked;		

		if (!gemLetterData.m_bIsInMainWord) //this is bonus word - quest
		{
			m_GameBoardManager.GetBonusQuestManager()->IncreaseCollectedLetterOfBonusWord();

			m_GameBoardManager.IncreaseScoreForLetterInBonusWords(1);
		}
		else if (m_GameBoardManager.GetGameWordManager()->UnlockLetter( gemLetterData.m_iLetter, iUnlockedLetterIndexOfMainWord, bIsMainWordJustUnlocked))
		{			
			if (iUnlockedLetterIndexOfMainWord >=0)
				m_pWordCollectBoardRenderNode->UnlockLetter(iUnlockedLetterIndexOfMainWord);
				//m_pWordCollectBoardRenderNode->UnlockCharacter(fDelay, iUnlockedLetterIndexOfMainWord);

			// increase score
			if (iUnlockedLetterIndexOfMainWord >=0)
				m_GameBoardManager.IncreaseScoreForLetterInMainWord();			

			if (bIsMainWordJustUnlocked)
				m_GameBoardManager.IncreaseScoreForUnlockMainWord();
			
			// play sound effect 
			SoundManager::PlaySoundEffect(_SET_GET_CHARACTER_);
		}							
	}

	
	if ( m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite != NULL)
	{
		m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite->runAction(
			Sequence::create(
				DelayTime::create(fDelay + fEffectDuration ),
				RemoveSelf::create( true),
				//FadeOut::create(0.01f),
				NULL));

		m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite = NULL;
	}

	m_BoardViewMatrix[iRow][iColumn].m_pSprite = NULL;
	m_BoardViewMatrix[iRow][iColumn].m_iGemLetterBlockID = -1;
	m_BoardViewMirrorMatrix[iRow][iColumn].m_pSprite = NULL;	
}

void HelloWorld::PlaySplashLightDestroyEffectOnCell(const int& iRow, const int & iColumn, const float& fDelayTime, const float& fEffectDuration)
{	
	Point position(m_fBoardLeftPosition + iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + iRow * m_SymbolSize.height);

	// active 1 sprite
	Sprite* pActive1Sprite = Sprite::createWithSpriteFrameName("Active1.png");

	pActive1Sprite->setPosition(position);	
	m_pBoardBatchNode->addChild(pActive1Sprite, m_BoardViewMatrix[iRow][iColumn].m_pSprite->getZOrder() + 10);	
	pActive1Sprite->setOpacity(0);
	pActive1Sprite->setScale(0.1f);
	pActive1Sprite->runAction(
		Sequence::create(
			DelayTime::create( fDelayTime),
			FadeTo::create( 0.001f, 255),
			ScaleTo::create( 0.06f, 0.033f),
			ScaleTo::create( 0.15f, 0.9f),
			ScaleTo::create( 0.1f, 1.f),
			ScaleTo::create( 0.133f, 1.1f),
			NULL));
	pActive1Sprite->runAction(
		Sequence::create(
			DelayTime::create( fDelayTime + 0.31f),
			FadeOut::create(0.134),
			RemoveSelf::create(),
			NULL));

	// active 2 sprite
	Sprite* pActive2Sprite = Sprite::createWithSpriteFrameName("Active2.png");	
	pActive2Sprite->setPosition(position);
	m_pBoardBatchNode->addChild(pActive2Sprite,  m_BoardViewMatrix[iRow][iColumn].m_pSprite->getZOrder() + 10);	
	pActive2Sprite->setOpacity(0);
	pActive2Sprite->setScale(0.1f);
	pActive2Sprite->runAction(
		Sequence::create(
			DelayTime::create( fDelayTime),
			FadeTo::create( 0.001f, 255),
			ScaleTo::create( 0.0833f, 0.6f, 0.6f),
			ScaleTo::create( 0.1f, 0.8f, 0.8f),
			ScaleTo::create( 0.1f, 1.2f, 1.2f),
			ScaleTo::create( 0.133f, 1.4f, 1.4f),
			NULL));
	pActive2Sprite->runAction(
		Sequence::create(
			DelayTime::create( fDelayTime + 0.31f),
			FadeOut::create(0.134),
			RemoveSelf::create(),
			NULL));
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

Sprite* HelloWorld::AddObstacleToGem(Sprite* pGemSprite, const int& iBlockID, const int& iObstacleTypeID, bool bPlayEffect)
{	
	ObstacleData& obstacleData = m_GameBoardManager.GetObstacleProcessManager()->GetObstacleData(iBlockID, iObstacleTypeID);
	if (obstacleData.m_bIsActive)
	{
		Size spriteSize = pGemSprite->getContentSize();
		Sprite* pMaskSprite = Sprite::createWithSpriteFrameName( GameConfigManager::getInstance()->GetObstacleLevelDescription(iObstacleTypeID, obstacleData.m_iObstacleLevel).m_sSpriteFileName.c_str()); //"Lock.png");														
				
		pGemSprite->addChild(pMaskSprite, 10);

		const ObstacleDescription* pObstacleDesciption = GameConfigManager::getInstance()->GetObstacleDescription(iObstacleTypeID);
		pMaskSprite->setPosition( Point( spriteSize.width /2.f + pObstacleDesciption->m_TranslatePosition.x, 
			spriteSize.height /2.f + pObstacleDesciption->m_TranslatePosition.y));

		// effect
		if (bPlayEffect)
		{
			pMaskSprite->setScale(1.1f);
			pMaskSprite->setOpacity(0);
			pMaskSprite->runAction(
				Sequence::create(
					DelayTime::create(0.2f),
					FadeIn::create(0.01f),
					ScaleTo::create(0.1f, 1.f, 1.f),
					NULL));
		}


		if (pObstacleDesciption->m_bIsDrawLevelLabel)
		{
			char sLevelLabel[4];
			char sTemp[30];
			int iLabelLength, i;
			sprintf( sLevelLabel, "%d", obstacleData.m_iObstacleLevel);
			iLabelLength = strlen(sLevelLabel);
			
			if (iLabelLength > 0)
			{
				Sprite* levelSpriteList[3];
				for(i=0; i<iLabelLength; i++)
				{
					sprintf(sTemp, "Number_%c.png", sLevelLabel[i]);
					levelSpriteList[i] = Sprite::createWithSpriteFrameName(sTemp);
				}
			 
				Size letterSize = levelSpriteList[0]->getContentSize();
				Size obstacleSpriteSize = pMaskSprite->getContentSize();
				for(i=0; i<iLabelLength; i++)
				{
					levelSpriteList[i]->setPosition( Point( obstacleSpriteSize.width/2.f + (i-(iLabelLength-1)/2.f) * letterSize.width, obstacleSpriteSize.height/2.f));
					pMaskSprite->addChild(levelSpriteList[i]);

					// effect
					if (bPlayEffect)
					{
						levelSpriteList[i]->setScale(1.6f);
						levelSpriteList[i]->setOpacity(0);
						levelSpriteList[i]->runAction(
							Sequence::create(
								DelayTime::create(0.2f),
								FadeIn::create(0.01f),
								ScaleTo::create(0.1f, 1.f, 1.f),
								NULL));
					}
				}
			}
		}

		if (obstacleData.m_iObstacleLevel > 0)
			m_BoardObstaclesList[iBlockID][iObstacleTypeID] = pMaskSprite;

		return pMaskSprite;
	}	
	return NULL;
}

Sprite* HelloWorld::AddLetterToGem(const Cell& cell, const int& iGemID, const unsigned char& iLetter, const int& iGemLetterBlockID, bool bIsMirror)
{
	Sprite* pCharacterSprite;
	if (iGemLetterBlockID >=0)
		pCharacterSprite = Sprite::createWithSpriteFrameName(
			m_pWordCollectBoardRenderNode->GetImageInGemFileFromLetter(iLetter).c_str());
	else
		pCharacterSprite = Sprite::createWithSpriteFrameName("Gem_QuestionMask.png");
		
	Size parentSpriteSize = m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->getContentSize();
	pCharacterSprite->setScale(0.8f);
	pCharacterSprite->setPosition(Point(parentSpriteSize.width/2.f, parentSpriteSize.height/2.f));  //( 42.f, 42.f));

	switch(iGemID)
	{
		case _GT_CAT_: //Orange
			pCharacterSprite->setColor(ccc3(252, 234, 160));
			break;
		case _GT_BIRD_: //red
			pCharacterSprite->setColor(ccc3(242, 209, 163));
			break;
		case _GT_HAMSTER_: //pink
			pCharacterSprite->setColor(ccc3(242, 217, 179));
			break;
		case _GT_SHEEP_: //white
			pCharacterSprite->setColor(ccc3(148, 135, 102));
			break;
		case _GT_ELEPHANT_: //blue
			pCharacterSprite->setColor(ccc3(17, 215, 250));
			break;
		case _GT_FROG_: //green
			pCharacterSprite->setColor(ccc3(184, 212, 6));
			break;
	}

	if (!bIsMirror)
	{
		m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->addChild(pCharacterSprite);
		m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iGemLetterBlockID = iGemLetterBlockID; //m_iLetter = iLetter;
	}
	else
		m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->addChild(pCharacterSprite);
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
	//m_pWordCollectBoardRenderNode->PlayCharacterAnim(3, true);

	
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

	//Game Tracking Level
	GameTracking::saveFileTrackingLevel(m_GameBoardManager.GetCurrentLevel(), m_GameBoardManager.GetCurrentMove(), mainWord.m_iWordLength, "Win");
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
	auto pComboEffectSprite = Sprite::createWithSpriteFrameName("SimpleEffect.png");
	pComboEffectSprite->setPosition( Point(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height));
	pComboEffectSprite->setOpacity(0);
	m_pComboEffectBatchNode->addChild(pComboEffectSprite);

	if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_ROW_)
	{
		pComboEffectSprite->runAction(
			Sequence::createWithTwoActions(
				DelayTime::create(fDelayTime),
				ScaleTo::create( fDisplayTime, 20.f, 1.f)));
						
	}
	else
		pComboEffectSprite->runAction(
			Sequence::createWithTwoActions(
				DelayTime::create(fDelayTime),
				ScaleTo::create( fDisplayTime, 1.f, 20.f)));

	pComboEffectSprite->runAction(
			Sequence::create(				
				DelayTime::create(fDelayTime),
				FadeIn::create(0.02f),
				FadeOut::create(fDisplayTime + 0.05f),
				RemoveSelf::create(),
				NULL));

	SoundManager::PlaySoundEffect(_SET_SIMPLE_COMBO_, fDelayTime);
}

void HelloWorld::PlayComboEndGameBonusEffect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime)
{
	auto pComboEffectSprite = Sprite::createWithSpriteFrameName("bonus_effect.png");
	pComboEffectSprite->setAnchorPoint( Point( 0.5f, 0));
	pComboEffectSprite->setPosition( Point(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height));
	pComboEffectSprite->setOpacity(0);
	m_pComboEffectBatchNode->addChild(pComboEffectSprite);

	pComboEffectSprite->runAction(
		Sequence::createWithTwoActions(
			DelayTime::create(fDelayTime),
			ScaleTo::create( fDisplayTime, 1.f, 4.f)));

	pComboEffectSprite->runAction(
		Sequence::createWithTwoActions(
			DelayTime::create(fDelayTime),
			MoveBy::create( fDisplayTime, Point( 0, 300.f))));// 1.f, 50.f)));


	pComboEffectSprite->runAction(
			Sequence::create(				
				DelayTime::create(fDelayTime),
				FadeIn::create(0.02f),
				FadeOut::create(fDisplayTime + 0.05f),
				RemoveSelf::create(),
				NULL));

	SoundManager::PlaySoundEffect(_SET_SIMPLE_COMBO_, fDelayTime);
}

void HelloWorld::PlayCrazyPetEndGameBonusEffect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime)
{
	if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_BIRD_EXPLOSION_EFFECT_ ||
		pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_FROG_EXPLOSION_EFFECT_ ||
		pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_ELEPHANT_EXPLOSION_EFFECT_ )
	{
		Sprite* pGemSprite = m_BoardViewMatrix[pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite;
		pGemSprite->runAction(
			Sequence::create(
				DelayTime::create(fDelayTime),
				MoveBy::create(0.02f, Point(0, 6.f)),
				MoveBy::create(0.03f, Point(0, -7.f)),
				NULL));

		PlayCombo5Effect(pComboEffect, fDelayTime + 0.05f, _TME_BASIC_COMBO_EXECUTE_TIME_);
	}
	else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_SHEEP_RUN_LEFT_EFFECT_)
	{
		Sprite* pGemSprite = m_BoardViewMatrix[pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite;
		pGemSprite->runAction(
			Sequence::create(
				DelayTime::create(fDelayTime),
				MoveBy::create( fDisplayTime * 1.5f, Point( -400.f, 0)),
				RemoveSelf::create(),
				NULL));
	}
	else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_SHEEP_RUN_RIGHT_EFFECT_)
	{
		Sprite* pGemSprite = m_BoardViewMatrix[pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite;
		pGemSprite->runAction(
			Sequence::create(
				DelayTime::create(fDelayTime),
				MoveBy::create( fDisplayTime * 1.5f, Point( 400.f, 0)),
				RemoveSelf::create(),
				NULL));
	}
	else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_CAT_RUN_UP_EFFECT_)
	{
		Sprite* pGemSprite = m_BoardViewMatrix[pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite;
		pGemSprite->runAction(
			Sequence::create(
				DelayTime::create(fDelayTime),
				MoveBy::create( fDisplayTime * 1.5f, Point( 0, 400.f)),
				RemoveSelf::create(),
				NULL));
	}
	else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_CAT_RUN_DOWN_EFFECT_)
	{
		Sprite* pGemSprite = m_BoardViewMatrix[pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite;
		pGemSprite->runAction(
			Sequence::create(
				DelayTime::create(fDelayTime),
				MoveBy::create( fDisplayTime * 1.5f, Point( 0, -400.f)),
				RemoveSelf::create(),
				NULL));
	}
	else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_HAMSTER_RUN_LEFT_UP_EFFECT_)
	{
		Sprite* pGemSprite = m_BoardViewMatrix[pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite;
		pGemSprite->runAction(
			Sequence::create(
				DelayTime::create(fDelayTime),
				MoveBy::create( fDisplayTime * 1.5f, Point( 400, -400.f)),
				RemoveSelf::create(),
				NULL));
	}
	else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_HAMSTER_RUN_RIGHT_DOWN_EFFECT_)
	{
		Sprite* pGemSprite = m_BoardViewMatrix[pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite;
		pGemSprite->runAction(
			Sequence::create(
				DelayTime::create(fDelayTime),
				MoveBy::create( fDisplayTime * 1.5f, Point( -400, 400.f)),
				RemoveSelf::create(),
				NULL));
	}
}

void HelloWorld::PlayCombo4_4Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime)
{
	auto pComboEffectSprite1 = Sprite::createWithSpriteFrameName("SimpleEffect.png");
	auto pComboEffectSprite2 = Sprite::createWithSpriteFrameName("SimpleEffect.png");

	pComboEffectSprite1->setPosition( Point(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height));
	pComboEffectSprite1->setOpacity(0);
	m_pComboEffectBatchNode->addChild(pComboEffectSprite1);

	pComboEffectSprite2->setPosition( pComboEffectSprite1->getPosition());
	pComboEffectSprite2->setOpacity(0);
	m_pComboEffectBatchNode->addChild(pComboEffectSprite2);

	pComboEffectSprite1->runAction(
		Sequence::createWithTwoActions(
		DelayTime::create(fDelayTime),
		ScaleTo::create( fDisplayTime, 20.f, 1.f)));
							
	pComboEffectSprite2->runAction(
		Sequence::createWithTwoActions(
		DelayTime::create(fDelayTime),
		ScaleTo::create( fDisplayTime, 1.f, 20.f)));

	pComboEffectSprite1->runAction(
			Sequence::create(				
				DelayTime::create(fDelayTime),
				FadeIn::create(0.02f),
				FadeOut::create(fDisplayTime + 0.05f),
				RemoveSelf::create(),
				NULL));
	pComboEffectSprite2->runAction(
			Sequence::create(				
				DelayTime::create(fDelayTime),
				FadeIn::create(0.02f),
				FadeOut::create(fDisplayTime + 0.05f),
				RemoveSelf::create(),
				NULL));

	SoundManager::PlaySoundEffect(_SET_SIMPLE_COMBO_, fDelayTime);
}

void HelloWorld::PlayCombo4_4_4Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime)
{
	auto pComboEffectSprite1 = Sprite::createWithSpriteFrameName("SimpleEffect.png");
	auto pComboEffectSprite2 = Sprite::createWithSpriteFrameName("SimpleEffect.png");
	auto pComboEffectSprite3 = Sprite::createWithSpriteFrameName("SimpleEffect.png");
	auto pComboEffectSprite4 = Sprite::createWithSpriteFrameName("SimpleEffect.png");

	pComboEffectSprite1->setPosition( Point(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height));
	pComboEffectSprite1->setOpacity(0);
	m_pComboEffectBatchNode->addChild(pComboEffectSprite1);

	pComboEffectSprite2->setPosition( pComboEffectSprite1->getPosition());
	pComboEffectSprite2->setRotation(90.f);
	pComboEffectSprite2->setOpacity(0);
	m_pComboEffectBatchNode->addChild(pComboEffectSprite2);

	pComboEffectSprite3->setPosition( pComboEffectSprite1->getPosition());
	pComboEffectSprite3->setRotation(45.f);
	pComboEffectSprite3->setOpacity(0);
	m_pComboEffectBatchNode->addChild(pComboEffectSprite3);

	pComboEffectSprite4->setPosition( pComboEffectSprite1->getPosition());
	pComboEffectSprite4->setRotation(135.f);
	pComboEffectSprite4->setOpacity(0);
	m_pComboEffectBatchNode->addChild(pComboEffectSprite4);

	pComboEffectSprite1->runAction(
		Sequence::createWithTwoActions(
		DelayTime::create(fDelayTime),
		ScaleTo::create( fDisplayTime, 20.f, 1.f)));
							
	pComboEffectSprite2->runAction(
		Sequence::createWithTwoActions(
		DelayTime::create(fDelayTime),
		ScaleTo::create( fDisplayTime, 20.f, 1.f)));

	pComboEffectSprite3->runAction(
		Sequence::createWithTwoActions(
		DelayTime::create(fDelayTime),
		ScaleTo::create( fDisplayTime, 20.f, 1.f)));

	pComboEffectSprite4->runAction(
		Sequence::createWithTwoActions(
		DelayTime::create(fDelayTime),
		ScaleTo::create( fDisplayTime, 20.f, 1.f)));

	pComboEffectSprite1->runAction(
			Sequence::create(				
				DelayTime::create(fDelayTime),
				FadeIn::create(0.02f),
				FadeOut::create(fDisplayTime + 0.05f),
				RemoveSelf::create(),
				NULL));
	pComboEffectSprite2->runAction(
			Sequence::create(				
				DelayTime::create(fDelayTime),
				FadeIn::create(0.02f),
				FadeOut::create(fDisplayTime + 0.05f),
				RemoveSelf::create(),
				NULL));

	pComboEffectSprite3->runAction(
			Sequence::create(				
				DelayTime::create(fDelayTime),
				FadeIn::create(0.02f),
				FadeOut::create(fDisplayTime + 0.05f),
				RemoveSelf::create(),
				NULL));

	pComboEffectSprite4->runAction(
			Sequence::create(				
				DelayTime::create(fDelayTime),
				FadeIn::create(0.02f),
				FadeOut::create(fDisplayTime + 0.05f),
				RemoveSelf::create(),
				NULL));

	SoundManager::PlaySoundEffect(_SET_SIMPLE_COMBO_, fDelayTime);
}

void HelloWorld::PlayCombo4_5Effect(ComboEffectBundle* pComboEffect, float fDelayTime)
{
	PlayCombo5Effect(pComboEffect, fDelayTime, _TME_BASIC_COMBO_EXECUTE_TIME_);
	PlayCombo4_4Effect(pComboEffect, fDelayTime, _TME_BASIC_COMBO_EXECUTE_TIME_);

	Cell backupPos = pComboEffect->m_ComboEffectDescription.m_Position;
	for(int i=0; i< pComboEffect->m_ComboEffectDescription.m_Phase0CellCount; i++)
		if (pComboEffect->m_ComboEffectDescription.m_Phase0CellList[i].m_iDestroyPhaseIndex >0)
		{
			pComboEffect->m_ComboEffectDescription.m_Position = pComboEffect->m_ComboEffectDescription.m_Phase0CellList[i];
			PlayCombo5Effect(pComboEffect, fDelayTime + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, _TME_BASIC_COMBO_EXECUTE_TIME_);
		}

	pComboEffect->m_ComboEffectDescription.m_Position = backupPos;
}

void HelloWorld::PlayCombo5_5_5Effect(ComboEffectBundle* pComboEffect, float fDelayTime)
{
	PlayCombo5Effect(pComboEffect, fDelayTime, _TME_BASIC_COMBO_EXECUTE_TIME_);
	PlayCombo4_4_4Effect(pComboEffect, fDelayTime, _TME_BASIC_COMBO_EXECUTE_TIME_);

	Cell backupPos = pComboEffect->m_ComboEffectDescription.m_Position;
	for(int i=0; i< pComboEffect->m_ComboEffectDescription.m_Phase0CellCount; i++)
		if (pComboEffect->m_ComboEffectDescription.m_Phase0CellList[i].m_iDestroyPhaseIndex >0)
		{
			pComboEffect->m_ComboEffectDescription.m_Position = pComboEffect->m_ComboEffectDescription.m_Phase0CellList[i];
			PlayCombo5Effect(pComboEffect, fDelayTime + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, _TME_BASIC_COMBO_EXECUTE_TIME_);
		}

	pComboEffect->m_ComboEffectDescription.m_Position = backupPos;
}

void HelloWorld::PlayCombo5Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime)
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

void HelloWorld::PlayCombo5_5Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime)
{
	Sprite* pComboEffectSprite;

	// play sound
	SoundManager::PlaySoundEffect(_SET_DOUBLE_COMPLE_EFFECT_, fDelayTime);

	pComboEffectSprite = Sprite::createWithSpriteFrameName("Explosion2.png");		
	pComboEffectSprite->setScale(2.17f);	

	pComboEffectSprite->setPosition( Point(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
			m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height));
	
	m_pBoardBatchNode->addChild(pComboEffectSprite, 200);

	pComboEffectSprite->setOpacity(0);
	pComboEffectSprite->runAction(Sequence::create( 
			DelayTime::create(fDelayTime),
			CallFuncN::create( this, callfuncN_selector( HelloWorld::ActivateImageEffect)),
			EaseOut::create( FadeOut::create( fDisplayTime), 2.f),				
			RemoveSelf::create( true),				
			NULL));		
}
	
void HelloWorld::PlayCombo6Effect(ComboEffectBundle* pComboEffectBundle, float fDelayTime)
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

	int i,j;
	DestroyedByComboCell phase0Cell, phase1cell;
	int iSize = pComboEffectBundle->m_DestroyedCells.size();
	for(i=0; i< pComboEffectBundle->m_ComboEffectDescription.m_Phase0CellCount; i++)
	{	
		phase0Cell =  pComboEffectBundle->m_ComboEffectDescription.m_Phase0CellList[i];

		//phase 0 effect
		auto pComboEffectSprite = Sprite::createWithSpriteFrameName("Bolt 2_00000_1.png");
		pComboEffectSprite->setAnchorPoint  (Point( 0, 0.5f));
		pComboEffectSprite->setPosition(Point(m_fBoardLeftPosition + comboCell.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + comboCell.m_iRow * m_SymbolSize.height));

		Point vector(phase0Cell.m_iColumn - comboCell.m_iColumn, phase0Cell.m_iRow - comboCell.m_iRow);
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
		pFlareSprite->setPosition(Point(m_fBoardLeftPosition + phase0Cell.m_iColumn  * m_SymbolSize.width, 
					m_fBoardBottomPosition + phase0Cell.m_iRow * m_SymbolSize.height));
		//pFlareSprite->setOpacity(0);		
		m_pComboEffectBatchNode->addChild(pFlareSprite);
		
		pFlareSprite->runAction( 
			Sequence::create(
				DelayTime::create(fDelayTime + 0.32f),
				Animate::create( pCombo5AnimFlare),
				RemoveSelf::create(),
				NULL ));		



		for(j=0; j< iSize; j++)
		{
			if (pComboEffectBundle->m_DestroyedCells[j].m_iGroupIndex == phase0Cell.m_iGroupIndex &&  pComboEffectBundle->m_DestroyedCells[j].m_iDestroyPhaseIndex >0)
			{
				phase1cell = pComboEffectBundle->m_DestroyedCells[j];

				auto pComboEffectSprite = Sprite::createWithSpriteFrameName("Bolt 2_00000_1.png");
				pComboEffectSprite->setAnchorPoint  (Point( 0, 0.5f));
				pComboEffectSprite->setPosition(Point(m_fBoardLeftPosition + phase0Cell.m_iColumn  * m_SymbolSize.width, 
						m_fBoardBottomPosition + phase0Cell.m_iRow * m_SymbolSize.height));

				Point vector(phase1cell.m_iColumn - phase0Cell.m_iColumn, phase1cell.m_iRow - phase0Cell.m_iRow);
				auto fAngle = atan2f( vector.y, vector.x);
				float fDistance = sqrtf( vector.x*vector.x + vector.y*vector.y);
		
				pComboEffectSprite->setRotation(-fAngle * 180.f / M_PI);
				pComboEffectSprite->setScaleX( fDistance * m_SymbolSize.width / pComboEffectSprite->getContentSize().width);
				pComboEffectSprite->setScaleY(1.5f);

				m_pComboEffectBatchNode->addChild(pComboEffectSprite);
				pComboEffectSprite->runAction( 
					Sequence::create( 
						DelayTime::create(fDelayTime + 0.35f),
						Animate::create( pCombo5AnimBolt),
						RemoveSelf::create(),
						NULL));


				auto pFlareSprite = Sprite::createWithSpriteFrameName("Flare_00000_1.png");	
				pFlareSprite->setPosition(Point(m_fBoardLeftPosition + phase1cell.m_iColumn  * m_SymbolSize.width, 
							m_fBoardBottomPosition + phase1cell.m_iRow * m_SymbolSize.height));
				//pFlareSprite->setOpacity(0);		
				m_pComboEffectBatchNode->addChild(pFlareSprite);
		
				pFlareSprite->runAction( 
					Sequence::create(
						DelayTime::create(fDelayTime + 0.72f),
						Animate::create( pCombo5AnimFlare),
						RemoveSelf::create(),
						NULL ));		
			}
		}		
	}

	/*for(auto destroyedCell : pComboEffectBundle->m_DestroyedCells)
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
		
		pFlareSprite->runAction( 
			Sequence::create(
				DelayTime::create(fDelayTime + 0.42f),
				Animate::create( pCombo5AnimFlare),
				RemoveSelf::create(),
				NULL ));		
	} */

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

void HelloWorld::PlayCombo6_6Effect(ComboEffectBundle* pComboEffectBundle, float fDelayTime)
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
		
		pFlareSprite->runAction( 
			Sequence::create(
				DelayTime::create(fDelayTime + 0.42f),
				Animate::create( pCombo5AnimFlare),
				RemoveSelf::create(),
				NULL ));		
	} 

	SoundManager::PlaySoundEffect(_SET_COMBINE_DOUBLE_COMBO_, fDelayTime-0.15f);
}

// unlock letter flow
void HelloWorld::PlayUnlockLettersOfMainWordAnimation(const float& fDelayTime)
{
	float fDisplayTime = m_pWordCollectBoardRenderNode->PlayUnlockLettersAnimation(fDelayTime);
	if (fDisplayTime > 0)
	{
		//m_pWordCollectBoardRenderNode->PlayCharacterAnim(5, false);

		this->runAction(
			Sequence::create(
			DelayTime::create( fDisplayTime),
				CallFunc::create( this,  callfunc_selector(HelloWorld::EndUnlockLetterAnimation)),
				NULL));
	}
	else
	{
		//PlayUnlockLettersOfBonusWordsAnimation();
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
			// should remove all bonus-quest gem before start bonus phase
			if (m_GameBoardManager.HasBonusQuestGemOnBoard())
			{
				m_ComputeMoveResult.Reset();
				m_GameBoardManager.ClearBonusQuestGemOnBoard( m_ComputeMoveResult.m_BasicMatchingDestroyedCells, m_ComputeMoveResult.m_OriginalMovedCells,
															m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_NewCells);

				PlayEffect2( false, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells,
						m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells,  m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells, false);
			}
			else // start bonus phase
			{
				// play sound effect 
				SoundManager::PlaySoundEffect(_SET_COMPLETE_WORD_);

				m_bIsEffectPlaying = true;//stop all interaction on board from now
				m_bIsEndGamePhase = true; 

				ShowMainWordUnlockEffect();
				//ExecuteBonusWinGameEffect();

				UserTable::getInstance()->updateLife(0);
			}
		}
		else  if (m_GameBoardManager.GetCurrentMove() == 0) // out of move ==> lose
		{
			m_bIsEndGamePhase = true; 

			EndGameNode* pEndGameNode = EndGameNode::createLayoutLose( m_GameBoardManager.GetCurrentScore(), 
				m_GameBoardManager.GetGameWordManager()->GetMainWord(), m_GameBoardManager.GetCurrentLevel());
			m_pHUDLayer->addChild( pEndGameNode, 100);

			SoundManager::PlaySoundEffect(_SET_LOSE);
			UserTable::getInstance()->updateLife(1);

			//Game Tracking Level
			const Word& mainWord = m_GameBoardManager.GetGameWordManager()->GetMainWord();
			GameTracking::saveFileTrackingLevel(m_GameBoardManager.GetCurrentLevel(), 0, mainWord.m_iWordLength-mainWord.m_iRemainInactivatedCharacterCount, "Lose");
		}
	}
}