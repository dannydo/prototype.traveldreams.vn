#include "HelloWorldScene.h"
#include "SoundManager.h"
#include "EndGameNode.h"
#include "LevelMapScene.h"
#include "Database\UserTable.h"
#include "Database\GameTracking.h"
#include "ActionExtension.h"
#include "OutOfMovesNode.h"

USING_NS_CC;

Scene* HelloWorld::createScene(GameModeType_e eGameModeType, int iTimeModeStage,  int iExistedCombo4Count, int iExistedCombo5Count, int iExistedCombo6Count)
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
	boardLayer->m_pWordCollectBoardRenderNode->SetStartGameCallback(std::bind( &HelloWorld::OnStartGame, boardLayer));

	// extra layer for score/stars...	
	boardLayer->m_pStatusLayer = StatusLayer::create();
	if (eGameModeType == _GMT_TIME_MODE_)
		boardLayer->m_pStatusLayer->hiddenLayoutMove();
	boardLayer->m_pStatusLayer->setScale(0.88f);
	boardLayer->m_pStatusLayer->setCurrentScore(0);
	boardLayer->m_pStatusLayer->setCurrentMove(0);
	boardLayer->m_pStatusLayer->setSpeedUpdateScore(120.f);
	boardLayer->m_pStatusLayer->setPosition(Point(567.0f, -38.0f));
	boardLayer->m_pHUDLayer->addChild(boardLayer->m_pStatusLayer);

	// init level
	boardLayer->initLevel(eGameModeType, iTimeModeStage, iExistedCombo4Count, iExistedCombo5Count, iExistedCombo6Count);

	Sprite* pSettingSprite = Sprite::create("Footer/btn_setting.png");
	Sprite* pSettingSpriteActive = Sprite::create("Footer/btn_setting.png");
	boardLayer->m_pButtonSettingNode = ButtonNode::createButtonSprite(pSettingSprite, pSettingSpriteActive, CC_CALLBACK_1(HelloWorld::menuCloseCallback, boardLayer));
	boardLayer->m_pButtonSettingNode->setPosition(Point(36.0f, 33.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(boardLayer->m_pButtonSettingNode);
	boardLayer->m_pHUDLayer->addChild(pButtonManagerNode);
	
	boardLayer->addChild(boardLayer->m_pHUDLayer);

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

	m_fIdleTime = 0;
	//m_bIsShowingHint = false;

	this->setTouchEnabled(true);	
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	this->scheduleUpdate();

	m_pSettingNode = NULL;

    return true;
}

void HelloWorld::initLevel(GameModeType_e eGameModeType, int iTimeModeStage, int iExistedCombo4Count, int iExistedCombo5Count, int iExistedCombo6Count)
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

	m_eGameModeType = eGameModeType;
	m_iCurrentTimeModeStage = iTimeModeStage; //only used this field is this is time mode

	const BaseLevelConfig* pBaseLevelConfig;
	const LevelConfig* pNormalLevelConfig = NULL;	
	std::vector<Cell> positionOfLettersOfTimeMode;	

	if (m_eGameModeType == _GMT_NORMAL_)
	{
		string sCurrentChapterID = GameConfigManager::getInstance()->GetCurrentChapterID();
		int iCurrentLevel = GameConfigManager::getInstance()->GetCurrentLevelId();	
	
		m_GameBoardManager.GenerateGameBoard(m_eGameModeType);

		pBaseLevelConfig = m_GameBoardManager.GetLevelConfig();
		pNormalLevelConfig = (LevelConfig*)pBaseLevelConfig;
	}
	else
	{
		m_GameBoardManager.GenerateGameBoard(m_eGameModeType);
		
		m_GameBoardManager.GeneratePositionOfLettersForTimeMode( positionOfLettersOfTimeMode, iExistedCombo4Count, iExistedCombo5Count, iExistedCombo6Count);

		pBaseLevelConfig = m_GameBoardManager.GetLevelConfig();
	}
	//CCLOG("Generate 4-1");

	
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
	if (m_eGameModeType == _GMT_NORMAL_)
	{
		m_pStatusLayer->setCurrentScore( m_GameBoardManager.GetCurrentScore());	
		m_pStatusLayer->setCurrentMove(m_GameBoardManager.GetCurrentMove());
	}
	else
		m_pStatusLayer->setCurrentScore( m_iCurrentTimeModeStage-1);	
		

	m_pStatusLayer->setScoreForStar( pBaseLevelConfig->m_ScoreOfStars[0], pBaseLevelConfig->m_ScoreOfStars[1], pBaseLevelConfig->m_ScoreOfStars[2]);
	//m_pStatusLayer->update(0);

	// init word-collect board
	m_pWordCollectBoardRenderNode->GenerateLabels( eGameModeType, iTimeModeStage);

	// init graphic for gameBoard
	CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("ResourceDemo.plist");
	m_pBoardBatchNode = CCSpriteBatchNode::create("ResourceDemo.pvr.ccz");
	//m_pBoardBatchNode->setBlendFunc( BlendFunc::ALPHA_PREMULTIPLIED);
	this->addChild(m_pBoardBatchNode);

	// init batchNode for text effect
	CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("TestEffect.plist");
	m_pTextEffectBatchNode = CCSpriteBatchNode::create("TestEffect.pvr.ccz");
	this->m_pHUDLayer->addChild(m_pTextEffectBatchNode, 12);
	
	// move hint
	//m_pMoveHintNode = Sprite::createWithSpriteFrameName("Gem_A.png");
	//m_pMoveHintNode->setOpacity(0);
	//m_pBoardBatchNode->addChild(m_pMoveHintNode);

	// init batch node for combo effect (used later)
	CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("ComboEffect/combo.plist");
	m_pComboEffectBatchNode = CCSpriteBatchNode::create("ComboEffect/combo.pvr.ccz");
	this->m_pHUDLayer->addChild(m_pComboEffectBatchNode, 20);

	CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("BonusTime.plist");
	m_pBonusTimeEffectBatchNode = CCSpriteBatchNode::create("BonusTime.pvr.ccz");
	this->m_pHUDLayer->addChild(m_pBonusTimeEffectBatchNode, 19);

	// cache anim
	auto animCache = AnimationCache::getInstance();    
    animCache->addAnimationsWithFile("ComboEffect/combo4Animations.plist");
	animCache->addAnimationsWithFile("ComboEffect/combo6Animations.plist");
	animCache->addAnimationsWithFile("ComboEffect/combo5AnimationsNew.plist");

	animCache->addAnimationsWithFile("ComboEffect/DestroyShieldAnimations.plist"); 

	animCache->addAnimationsWithFile("ComboEffect/BonusTimeAnimations.plist"); 

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
		// add top -3 brush
		pSprite = Sprite::createWithSpriteFrameName( "Bush.png");		
		position = Point(m_fBoardLeftPosition + iColumn * m_SymbolSize.width, m_fBoardBottomPosition + (iNumberOfRow +2) * m_SymbolSize.height - 10.f);		
		pSprite->setPosition(position);
		m_pBoardBatchNode->addChild(pSprite, GetZOrder( iNumberOfRow +2, iColumn, true));

		// add top -2 brush
		pSprite = Sprite::createWithSpriteFrameName( "Bush.png");		
		position = Point(m_fBoardLeftPosition + iColumn * m_SymbolSize.width, m_fBoardBottomPosition + (iNumberOfRow +1)* m_SymbolSize.height - 10.f);				
		pSprite->setPosition(position);
		m_pBoardBatchNode->addChild(pSprite, GetZOrder( iNumberOfRow +1, iColumn, true));

		// add top brushes
		// add brush background
		pSprite = Sprite::createWithSpriteFrameName( "Bush_Background.png");		
		position = Point(m_fBoardLeftPosition + iColumn * m_SymbolSize.width, m_fBoardBottomPosition + iNumberOfRow * m_SymbolSize.height - 10.f);
		pSprite->setPosition(position);
		m_pBoardBatchNode->addChild(pSprite);

		// add brush
		pSprite = Sprite::createWithSpriteFrameName( "Bush.png");		
		pSprite->setPosition(Point(position.x, position.y ));
		m_pBoardBatchNode->addChild(pSprite, GetZOrder(iNumberOfRow, iColumn, true));

		// add bot brushes
		// add brush background
		pSprite = Sprite::createWithSpriteFrameName( "Bush_Background.png");				
		position = Point(m_fBoardLeftPosition + iColumn * m_SymbolSize.width, m_fBoardBottomPosition + -1 * m_SymbolSize.height - 10.f);
		pSprite->setPosition(position);
		m_pBoardBatchNode->addChild(pSprite);

		// add brush 
		pSprite = Sprite::createWithSpriteFrameName( "Bush.png");		
		pSprite->setPosition(Point(position.x, position.y));
		m_pBoardBatchNode->addChild(pSprite, GetZOrder( -1, iColumn, true));
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

				// local drag wall?
				if (m_GameBoardManager.IsLocalDragWall(iRow, iColumn))
				{
					m_BoardViewMatrix[iRow][iColumn].m_bIsDragLocalWall = true;
					pSprite->setColor(Color3B( 255, 100, 120));
				}
			}
			else
			{
				bIsGemContainLetter = false;

				if (pBaseLevelConfig->m_eGameModeType == _GMT_NORMAL_) // pNormalLevelConfig != NULL
				{
					if (pNormalLevelConfig->m_bIsMainWordExistedOnBoard)
					{
						const Word& mainWord = m_GameBoardManager.GetGameWordManager()->GetMainWord();
						int iPositionIndex = 0;

						for(int i=0; i< mainWord.m_iWordLength; i++)
						{			
							if (!mainWord.m_ActivatedCharacterFlags[i])
							{
								if(pNormalLevelConfig->m_MainWordLetterPosition[iPositionIndex].m_iRow == iRow && pNormalLevelConfig->m_MainWordLetterPosition[iPositionIndex].m_iColumn == iColumn)
								{
									bIsGemContainLetter = true;
									break;
								}
								iPositionIndex++;
							}
						}
					}
				}
				else // time mode
				{
					for(auto& cell : positionOfLettersOfTimeMode)
						if (cell.m_iRow == iRow && cell.m_iColumn == iColumn)
						{
							bIsGemContainLetter = true;
							break;
						}
				}

				for( iFlag =0; iFlag < 2; iFlag++)
				{									
					pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(m_GameBoardManager.GetCellValue(iRow, iColumn), 
						bIsGemContainLetter?_GCT_HAS_LETTER_:m_GameBoardManager.GetCellGemComboType(iRow, iColumn) ).c_str());
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
			
	
	if (m_eGameModeType == _GMT_NORMAL_)	
	{
		m_pTimeCountDownNode = NULL; //no time count down node


		// add letter to gems if existing
		if (pNormalLevelConfig->m_bIsMainWordExistedOnBoard)
		{
			const Word& mainWord = m_GameBoardManager.GetGameWordManager()->GetMainWord();
			int iGemLetterBlockID;
			unsigned char iLetter;

			int iPositionIndex = 0;
			for(int i=0; i< mainWord.m_iWordLength; i++)
			{			
				if (!mainWord.m_ActivatedCharacterFlags[i])
				{
					iLetter = mainWord.m_sWord[i];
					iGemLetterBlockID = m_GameBoardManager.AllocFreeGemLetterBlock( iLetter, true);
					AddLetterToGem( pNormalLevelConfig->m_MainWordLetterPosition[iPositionIndex], m_GameBoardManager.GetCellValue( pNormalLevelConfig->m_MainWordLetterPosition[iPositionIndex].m_iRow,
						pNormalLevelConfig->m_MainWordLetterPosition[iPositionIndex].m_iColumn), mainWord.m_sWord[i], iGemLetterBlockID);

					iPositionIndex++;
				}
			}
		}
	

		// add boss if this is versus mode
		m_pBossSprite = NULL;

		if (pNormalLevelConfig->m_bEnableBoss)
		{
			m_pBossSprite = Sprite::createWithSpriteFrameName("BigAlien.png");
			m_pBossSprite->setPosition( ccp(m_fBoardLeftPosition + (pNormalLevelConfig->m_BossConfig.m_Position.m_iColumn + pNormalLevelConfig->m_BossConfig.m_iWidth/4.f)  * m_SymbolSize.width, 
				m_fBoardBottomPosition + (pNormalLevelConfig->m_BossConfig.m_Position.m_iRow + pNormalLevelConfig->m_BossConfig.m_iHeight/4.f) * m_SymbolSize.height));
			//pSprite->setScale(1.f);
			m_pBoardBatchNode->addChild(m_pBossSprite, _BOSS_ZORDER_);

			// add letter to boss
			AddNewLetterToBossSprite(0);

			// add HP Sprite to boss
			AddHitPointSpritesToBossSprite(0);		
		}
	}
	else// time mode
	{		
		const TimeModeLevelConfig* pTimeModeLevelConfig = (TimeModeLevelConfig*)pBaseLevelConfig;		
		int iMaximumEneryOfThisStage, iEnergyLostPersecondOfThisStage;
		if (iTimeModeStage <= pTimeModeLevelConfig->m_ManualStageConfigList.size())
		{
			iMaximumEneryOfThisStage = pTimeModeLevelConfig->m_ManualStageConfigList[iTimeModeStage-1]->m_iMaximumEnergy;
			iEnergyLostPersecondOfThisStage = pTimeModeLevelConfig->m_ManualStageConfigList[iTimeModeStage-1]->m_iEnergyLostRatePersecond;
		}
		else
		{
			int iTotalManualStageCount = pTimeModeLevelConfig->m_ManualStageConfigList.size();

			iMaximumEneryOfThisStage = pTimeModeLevelConfig->m_ManualStageConfigList[iTotalManualStageCount-1]->m_iMaximumEnergy * 
				(100 + pTimeModeLevelConfig->m_iStageConfig_MaximumValueIncreasePercent * (iTimeModeStage - iTotalManualStageCount))/100.f ;

			iEnergyLostPersecondOfThisStage = pTimeModeLevelConfig->m_ManualStageConfigList[iTotalManualStageCount-1]->m_iEnergyLostRatePersecond * 
				(100 + pTimeModeLevelConfig->m_iStageConfig_LostRateIncreasePercent * (iTimeModeStage - iTotalManualStageCount))/100.f ;			
		}		

		m_pTimeCountDownNode = TimeCountDownNode::create( iTimeModeStage, iMaximumEneryOfThisStage, iEnergyLostPersecondOfThisStage);
		//m_pTimeCountDownNode->setPositionY( m_fBoardBottomPosition + (iNumberOfRow  - 0.4f )* m_SymbolSize.height);
		m_pTimeCountDownNode->SetOutOfTimeCallback(std::bind( &HelloWorld::OnTimeMode_OutOfTime, this));
		this->m_pHUDLayer->addChild( m_pTimeCountDownNode, 100);

		// add letter to gem
		const Word& mainWord = m_GameBoardManager.GetGameWordManager()->GetMainWord();
		int iGemLetterBlockID;
		unsigned char iLetter;
		
		for(int i=0; i< mainWord.m_iWordLength; i++)
		{			
			if (!mainWord.m_ActivatedCharacterFlags[i])
			{
				iLetter = mainWord.m_sWord[i];
				iGemLetterBlockID = m_GameBoardManager.AllocFreeGemLetterBlock( iLetter, true);
				AddLetterToGem( positionOfLettersOfTimeMode[i], m_GameBoardManager.GetCellValue( positionOfLettersOfTimeMode[i].m_iRow,
					positionOfLettersOfTimeMode[i].m_iColumn), mainWord.m_sWord[i], iGemLetterBlockID);				
			}
		}				

		// disable user's interaction when playing start game effect
		m_bIsEffectPlaying = true;

		// play special begin of stage of time mode
		PlayBeginTimeModeStageTextEffect();
	}	


	// pre-load hint sprite
	m_pHintSprite = Sprite::createWithSpriteFrameName( "Hint.png");
	m_pHintSprite->setVisible(false);	
	m_pBoardBatchNode->addChild( m_pHintSprite, 200);

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

	// make sure fint hint at begin of game
	this->runAction( CCSequence::create(
				CCDelayTime::create(1.f),
				CCCallFunc::create( this, callfunc_selector( HelloWorld::CheckHintAfterMove)),
				NULL));		
}

void HelloWorld::menuCloseCallback(Object* pSender)
{
	// play sound
	SoundManager::PlaySoundEffect(_SET_BUTTON_PRESS_); 

	if(m_pSettingNode == NULL)
	{
		m_pSettingNode = SettingMenuNode::create();
		m_pSettingNode->setPosition(Point(-505.0f, 0));
		m_pSettingNode->addButtonSetting(m_pButtonSettingNode);
		this->addChild(m_pSettingNode, 98);
	}

	if (m_pSettingNode->getShowSetting() == false)
	{
		m_pSettingNode->show();
	}
	else
	{
		m_pSettingNode->hide();
	}

    //Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //exit(0);
#endif
}

void HelloWorld::visit()
{
	CCLayer::visit();
	/*
	 kmGLPushMatrix();

    glEnable(GL_SCISSOR_TEST);

	EGLView::getInstance()->setScissorInPoints(    // scissorRect is the rectangle you want to show.
		m_fBoardLeftClipPosition, m_fBoardBottomClipPosition,
		m_fBoardLeftClipPosition + m_fMaskWidth, m_fBoardBottomClipPosition + m_fMaskHeight);
        //scissorRect.origin.x, scissorRect.origin.y,
        //scissorRect.size.width, scissorRect.size.height);

    CCLayer::visit();
    glDisable(GL_SCISSOR_TEST);
    kmGLPopMatrix();*/
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
			return "Combo_6.png";
		case _GCT_BONUS_END_GAME_SPECIAL_GEM_:
			return "Combo4_2.png";		
	}

	switch(iGemID)
	{
		case  _BONUS_QUEST_GEM_ID_:
			return "BonusQuestGem.png";			
		default:
		case _GT_CAT_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
				default:
					return "Orange_Cat.png"; //"Candy_Cam.png";
				case _GCT_COMBO4_:
					return "Orange_Combo_4.png";
				case _GCT_COMBO5_:				
					return "Orange_Combo_5.png";				
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
				//case _GCT_COMBO4_:
				case _GCT_HAS_LETTER_:
					return "Green_Letter.png";
				case _GCT_BONUS_END_GAME_CRAZY_PET_:
					return "Green_Combo4_2.png";
			}					
		}
	}

	//return "Candy_6.png";
}

std::string HelloWorld::GetImageFileFromSnapGemID(int iGemID, GemComboType_e eGemComboType)
{
	switch (eGemComboType)
	{		
		case _GCT_COMBO6_:
			return "Combo_6_Snap.png";		
	}

	switch(iGemID)
	{
		default:
		case _GT_CAT_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
				default:
					return "Orange_Cat_Snap.png"; //"Candy_Cam.png";
				case _GCT_COMBO4_:
					return "Orange_Combo_4_Snap.png";
				case _GCT_COMBO5_:				
					return "Orange_Combo_5_Snap.png";	
				case _GCT_HAS_LETTER_:
					return "Orange_Letter_Snap.png";
			}

		}
		case _GT_BIRD_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Red_Bird_Snap.png"; //"Candy_Do.png";
				case _GCT_COMBO4_:
					return "Red_Combo_4_Snap.png";
				case _GCT_COMBO5_:				
					return "Red_Combo_5_Snap.png";
				case _GCT_HAS_LETTER_:
					return "Red_Letter_Snap.png";
			}			
		}
		case _GT_HAMSTER_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Pink_Hamster_Snap.png"; //"Candy_Tim.png";
				case _GCT_COMBO4_:
					return "Pink_Combo_4_Snap.png";
				case _GCT_COMBO5_:				
					return "Pink_Combo_5_Snap.png";				
				case _GCT_HAS_LETTER_:
					return "Pink_Letter_Snap.png";
			}						
		}
		case _GT_SHEEP_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "White_Sheep_Snap.png"; //"Candy_Vang.png";
				case _GCT_COMBO4_:
					return "White_Combo_4_Snap.png";
				case _GCT_COMBO5_:				
					return "White_Combo_5_Snap.png";				
				case _GCT_HAS_LETTER_:
					return "White_Letter_Snap.png";
			}									
		}
		case _GT_ELEPHANT_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Blue_Elephant_Snap.png"; //"Candy_XanhDuong.png";
				case _GCT_COMBO4_:
					return "Blue_Combo_4_Snap.png";		
				case _GCT_COMBO5_:				
					return "Blue_Combo_5_Snap.png";
				case _GCT_HAS_LETTER_:
					return "Blue_Letter_Snap.png";
			}												
		}
		case _GT_FROG_:
		{
			switch (eGemComboType)
			{
				case _GCT_NONE_:
					return "Green_Frog_Snap.png"; //"Candy_XanhLa.png";
				case _GCT_COMBO4_:
					return "Green_Combo_4_Snap.png";
				case _GCT_COMBO5_:				
					return "Green_Combo_5_Snap.png";
				case _GCT_HAS_LETTER_:
					return "Green_Letter_Snap.png";
			}					
		}
	}
}

SpriteFrame* HelloWorld::GetSpriteFrameFromGemID(int iGemID, GemComboType_e eGemComboType)
{
	return SpriteFrameCache::getInstance()->getSpriteFrameByName( GetImageFileFromGemID(iGemID, eGemComboType).c_str());	
}

SpriteFrame* HelloWorld::GetSpriteFrameFromSnapGemID(int iGemID, GemComboType_e eGemComboType)
{
	return SpriteFrameCache::getInstance()->getSpriteFrameByName( GetImageFileFromSnapGemID(iGemID, eGemComboType).c_str());
}

bool HelloWorld::onTouchBegan(Touch *pTouch, Event *pEvent)
{
	//CCLOG("Begin touch began"); 
	if (m_pSettingNode != NULL && m_pSettingNode->getShowSetting())
	{
		return false;
	}

	if (m_bIsEffectPlaying || m_bIsCellDragPlaying || m_bIsEndGamePhase)
		return true;

	// only allow 1 finger at one 1 time
	if (m_pSaveTouch == NULL)
	{
		m_pSaveTouch = pTouch;		
	}
	else
		return true;

	// hide hint
	if (m_pHintSprite->isVisible())
	{
		m_pHintSprite->setVisible(false);
		m_fIdleTime = 0;
	}

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

	//CCLOG("End touch began"); 

	return true;
}

void HelloWorld::onTouchEnded(Touch* pTouch, Event* pEvent)
{
	bool bIsBlocked = false;
	bIsBlocked = m_bIsEndGamePhase; //use this to mark that game is end and all user interaction should be interupted

	if (pTouch == m_pSaveTouch)
		m_pSaveTouch = NULL;

	if (m_eTouchMoveState == _TMS_NONE_)
		return;	

	if ((m_eTouchMoveState == _TMS_MOVE_HORIZONTAL_ && m_GameBoardManager.IsRowLocked(m_SelectedCell.m_iRow, m_SelectedCell.m_iColumn)) || 
		(m_eTouchMoveState == _TMS_MOVE_VERTICAL_ &&  m_GameBoardManager.IsColumnLocked(m_SelectedCell.m_iRow, m_SelectedCell.m_iColumn)))
	{
		bIsBlocked = true;		
	}			

	// remove snap sprites
	RemoveSnap();		

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
	//CCLOG("Begin touch moved");

	if (m_pSaveTouch != pTouch)
		return;

	if (m_eTouchMoveState == _TMS_NONE_)
		return;

	if (m_bIsEndGamePhase) //end game already???
	{		
		onTouchEnded(pTouch, pEvent);
		return;
	}

	Point currentPosition = pTouch->getLocation();
	float fDeltaX = currentPosition.x - m_StartTouchPosition.x;
	float fDeltaY = currentPosition.y - m_StartTouchPosition.y;

	/*if( (currentPosition.x < m_fBoardLeftPosition - m_SymbolSize.width/2.f || currentPosition.y < m_fBoardBottomPosition - m_SymbolSize.height/2.f)
		|| (currentPosition.x > m_fBoardLeftPosition + m_GameBoardManager.GetColumnNumber()* m_SymbolSize.width -m_SymbolSize.width/2.f
		|| currentPosition.y > m_fBoardBottomPosition + m_GameBoardManager.GetRowNumber()* m_SymbolSize.height - m_SymbolSize.height/2.f))
	{		
		//AdjustPosition( fDeltaX, fDeltaY, m_SelectedCell.m_iRow, m_SelectedCell.m_iColumn);
		//m_eTouchMoveState = _TMS_NONE_;
		//return;
	}*/	

	if (m_eTouchMoveState == _TMS_BEGIN_IDENTIFY_)
	{			
		int iRowNumber = m_GameBoardManager.GetRowNumber();
		int iColumnNumber= m_GameBoardManager.GetColumnNumber();
		 
		if ( fabs(fDeltaX) > fabs(fDeltaY) &&  fabs(fDeltaX) > 5.f)
		{
			m_eTouchMoveState = _TMS_MOVE_HORIZONTAL_;

			auto action = Sequence::create(		
				SetAnchorAction::Create( Point( 0.5f, 0)),
				ScaleTo::create( 0.1f, 1.1f, 0.9f),				
				//ResetAnchorToCenterAction::Create(),
				NULL);

			// we must define min/max column because of local drag wall
			int iMinColumn = m_SelectedCell.m_iColumn, iMaxColumn = m_SelectedCell.m_iColumn;
			while (iMinColumn >0 && m_BoardViewMatrix[m_SelectedCell.m_iRow][iMinColumn ].m_bIsDragLocalWall == false)
				iMinColumn--;
			while (iMaxColumn < iColumnNumber-1 && m_BoardViewMatrix[m_SelectedCell.m_iRow][iMaxColumn ].m_bIsDragLocalWall == false)
				iMaxColumn++;

			m_iMovingCellListLength = iMaxColumn- iMinColumn+1; //m_GameBoardManager.GetColumnNumber();
			m_iBeginMovingCellIndex = iMinColumn;
			for(int iColumn = 0; iColumn< m_iMovingCellListLength; iColumn++)
			{
				m_MovingCellList[iColumn] = m_BoardViewMatrix[m_SelectedCell.m_iRow][iColumn + iMinColumn];
				m_MovingCellMirrorList[iColumn] = m_BoardViewMirrorMatrix[m_SelectedCell.m_iRow][iColumn + iMinColumn];
				if (m_MovingCellList[iColumn].m_pSprite != NULL)
				{
					m_MovingCellMirrorList[iColumn].m_pSprite->setPosition(m_MovingCellList[iColumn].m_pSprite->getPosition());
					
					// play drag effect
					m_MovingCellList[iColumn].m_pSprite->runAction(action->clone());// ScaleTo::create( 0.1f, 1.1f, 0.9f));
					m_MovingCellMirrorList[iColumn].m_pSprite->runAction( action->clone()); //ScaleTo::create( 0.1f, 1.1f, 0.9f));
				}
			}
		}
		else if (fabs(fDeltaX) < fabs(fDeltaY) && fabs(fDeltaY) > 5.f)
		{
			m_eTouchMoveState = _TMS_MOVE_VERTICAL_;

			// we must define min/max row because of local drag wall
			int iMinRow = m_SelectedCell.m_iRow, iMaxRow = m_SelectedCell.m_iRow;
			while (iMinRow >0 && m_BoardViewMatrix[iMinRow][ m_SelectedCell.m_iColumn ].m_bIsDragLocalWall == false)
				iMinRow--;
			while (iMaxRow < iRowNumber-1 && m_BoardViewMatrix[iMaxRow][ m_SelectedCell.m_iColumn ].m_bIsDragLocalWall == false)
				iMaxRow++;


			m_iMovingCellListLength = iMaxRow - iMinRow +1;
			m_iBeginMovingCellIndex = iMinRow;
			for(int iRow = 0; iRow< m_iMovingCellListLength; iRow++)
			{
				m_MovingCellList[iRow] = m_BoardViewMatrix[iRow + iMinRow][m_SelectedCell.m_iColumn];
				m_MovingCellMirrorList[iRow] = m_BoardViewMirrorMatrix[iRow + iMinRow][m_SelectedCell.m_iColumn];

				if (m_MovingCellList[iRow].m_pSprite != NULL)
				{
					m_MovingCellMirrorList[iRow].m_pSprite->setPosition(m_MovingCellList[iRow].m_pSprite->getPosition());

					// play drag effect
					m_MovingCellList[iRow].m_pSprite->runAction(  ScaleTo::create( 0.1f, 1.1f, 0.9f));
					m_MovingCellMirrorList[iRow].m_pSprite->runAction( ScaleTo::create( 0.1f, 1.1f, 0.9f));
				}
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

	//CCLOG("End touch moved");

	if ((m_eTouchMoveState == _TMS_MOVE_HORIZONTAL_ && m_GameBoardManager.IsRowLocked(m_SelectedCell.m_iRow, m_SelectedCell.m_iColumn)) || 
		(m_eTouchMoveState == _TMS_MOVE_VERTICAL_ &&  m_GameBoardManager.IsColumnLocked(m_SelectedCell.m_iRow, m_SelectedCell.m_iColumn)))
	{
		onTouchEnded( pTouch, NULL);
		return;
	}	
}

void HelloWorld::update(float fDeltaTime)
{
	if (m_bIsCellDragPlaying)
	{
		//CCLOG("update-draging");

		if (m_ePlayingDragEffect == _TMS_MOVE_HORIZONTAL_)
		{
			//CCLOG("%f", m_pTempSpriteForAction->getPositionX());
			HorizontalMoveUlti(m_pTempSpriteForAction->getPositionX());
		}
		else 
		{
			VerticalMoveUlti(m_pTempSpriteForAction->getPositionY());
		}

		//CCLOG("End update");

		//m_fIdleTime = 0;
	}
	else if (m_eTouchMoveState == _TMS_NONE_)
	{
		m_iMovingCellListLength = 0; 	

		if (!m_bIsEffectPlaying && !m_bIsEndGamePhase && !m_pHintSprite->isVisible())
		{
			m_fIdleTime += fDeltaTime;

			if (m_fIdleTime > 4.f) //should show hint now
			{
				const Hint& hint = m_GameBoardManager.GetHint();
				if (hint.m_Position.m_iRow >=0 && hint.m_Position.m_iColumn >= 0)
				{
					//m_bIsShowingHint = true;
					m_pHintSprite->setVisible(true);
					if (m_BoardViewMatrix[hint.m_Position.m_iRow][hint.m_Position.m_iColumn].m_pSprite != NULL)
					{
						m_pHintSprite->setPosition( m_BoardViewMatrix[hint.m_Position.m_iRow][hint.m_Position.m_iColumn].m_pSprite->getPosition());

						// rotate hint sprite to correct direction
						if (hint.m_DeltaMove.m_iColumn != 0)
						{
							if (hint.m_DeltaMove.m_iColumn > 0)
							{
								// default
								m_pHintSprite->setRotation(0);
							}
							else
							{
								m_pHintSprite->setRotation(180.f);
							}
						}
						else
						{
							if (hint.m_DeltaMove.m_iRow > 0)
							{
								m_pHintSprite->setRotation(-90.f);
							}
							else
							{
								m_pHintSprite->setRotation(90.f);
							}
						}
					}
				}

			}
		}
		else
			m_fIdleTime = 0;
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

	// end drag effect
	FiniteTimeAction* action;
	if (m_eTouchMoveState == _TMS_MOVE_HORIZONTAL_)
		action = Sequence::create(		
					SetAnchorAction::Create( Point( 0.5f, 0)),
					ScaleTo::create( 0.12f, 0.95f, 1.05f),
					ScaleTo::create( 0.063f, 1.05f, 0.95f),
					ScaleTo::create( 0.063f, 1.f, 1.f),
					ResetAnchorToCenterAction::Create(),
					NULL);
	else
		action = Sequence::create(		
					ScaleTo::create( 0.12f, 0.95f, 1.05f),
					ScaleTo::create( 0.063f, 1.05f, 0.95f),
					ScaleTo::create( 0.063f, 1.f, 1.f),					
					NULL);

	for(int i=0; i< m_iMovingCellListLength; i++)
	{
		if (m_MovingCellList[i].m_pSprite != NULL)
		{
			m_MovingCellList[i].m_pSprite->runAction( action->clone()); //ScaleTo::create( 0.11f, 1.f, 1.f));
			m_MovingCellMirrorList[i].m_pSprite->runAction( action->clone());	//ScaleTo::create( 0.11f, 1.f, 1.f));
		}
	}
	delete action;

	if (m_eTouchMoveState == _TMS_MOVE_HORIZONTAL_)
	{				
		fMoveUnit = fDeltaX/m_SymbolSize.width;				
		fMoveUnit = round(fMoveUnit);
		iMoveUnit = (int)fMoveUnit;

		m_ePlayingDragEffect = _TMS_MOVE_HORIZONTAL_;

		if (!bIsBlocked && m_GameBoardManager.RecheckAfterMoveV2( m_iBeginMovingCellIndex, m_iMovingCellListLength, m_SelectedCell.m_iRow,-1,  -1, iMoveUnit, 
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
			for(int iColumn=0; iColumn < m_iMovingCellListLength; iColumn++)  //iNumberOfColumn; iColumn++)
			{
				tempCellList[iColumn] = m_BoardViewMatrix[m_SelectedCell.m_iRow][iColumn + m_iBeginMovingCellIndex];
				tempCellMirrorList[iColumn] = m_BoardViewMirrorMatrix[m_SelectedCell.m_iRow][iColumn + m_iBeginMovingCellIndex];
			}
						
			int iTranslationCell = 0;
			int iSign = 1;
			if (iMoveUnit != 0)
				iSign = iMoveUnit/ abs(iMoveUnit);
			
			for(int iColumn=0; iColumn < m_iMovingCellListLength; iColumn++)
				if (!m_GameBoardManager.IsBlankCell(m_SelectedCell.m_iRow, iColumn + m_iBeginMovingCellIndex))				
				{
					iTranslationCell = 0;
					for(int iStep=1; iStep <= abs(iMoveUnit); iStep++)
					{
						iTranslationCell += iSign;
						if (iTranslationCell < 0)
							iTranslationCell += m_iMovingCellListLength;

						if (m_GameBoardManager.IsBlankCell( m_SelectedCell.m_iRow, (iColumn + iTranslationCell) % m_iMovingCellListLength + m_iBeginMovingCellIndex))
						{
							iStep--;
						}
					}

					m_BoardViewMatrix[m_SelectedCell.m_iRow][(iColumn + m_iMovingCellListLength + iTranslationCell) % m_iMovingCellListLength + m_iBeginMovingCellIndex] = tempCellList[iColumn];
					m_BoardViewMirrorMatrix[m_SelectedCell.m_iRow][(iColumn + m_iMovingCellListLength + iTranslationCell) % m_iMovingCellListLength + m_iBeginMovingCellIndex] = tempCellMirrorList[iColumn];					
				}
		}
		else
		{			
			// play sound
			SoundManager::PlaySoundEffect(_SET_DRAG_FAIL_);

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

		if (!bIsBlocked && m_GameBoardManager.RecheckAfterMoveV2( m_iBeginMovingCellIndex, m_iMovingCellListLength, -1, m_SelectedCell.m_iColumn, iMoveUnit, -1, 
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
			for(int iRow=0; iRow < m_iMovingCellListLength; iRow++)//iNumberOfRow; iRow++)
			{
				tempCellList[iRow] = m_BoardViewMatrix[iRow + m_iBeginMovingCellIndex][m_SelectedCell.m_iColumn];
				tempCellMirrorList[iRow] = m_BoardViewMirrorMatrix[iRow + m_iBeginMovingCellIndex][m_SelectedCell.m_iColumn];
			}
						
			int iTranslationCell = 0;
			int iSign = 1;
			if (iMoveUnit != 0)
				iSign = iMoveUnit/ abs(iMoveUnit);

			for(int iRow=0; iRow < m_iMovingCellListLength; iRow++)
				if (!m_GameBoardManager.IsBlankCell(iRow, m_SelectedCell.m_iColumn))				
				{
					iTranslationCell = 0;
					for(int iStep=1; iStep <= abs(iMoveUnit); iStep++)
					{
						iTranslationCell += iSign;
						if (iTranslationCell < 0)
							iTranslationCell += m_iMovingCellListLength;

						if (m_GameBoardManager.IsBlankCell( (iRow + iTranslationCell) % m_iMovingCellListLength, m_SelectedCell.m_iColumn))
						{
							iStep--;
						}
					}

					m_BoardViewMatrix[(iRow + m_iMovingCellListLength + iTranslationCell) % m_iMovingCellListLength + m_iBeginMovingCellIndex][m_SelectedCell.m_iColumn] = tempCellList[iRow];
					m_BoardViewMirrorMatrix[(iRow + m_iMovingCellListLength + iTranslationCell) % m_iMovingCellListLength + m_iBeginMovingCellIndex][m_SelectedCell.m_iColumn] = tempCellMirrorList[iRow];					
				}
		}
		else
		{			
			// play sound
			SoundManager::PlaySoundEffect(_SET_DRAG_FAIL_);

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

	CCLOG("Adjust 2");
	// now play effect to destroy/move old cells, and generate new cells
	if (bMoveIsValid)
	{				 
		if (m_pTimeCountDownNode != NULL)
			 m_pTimeCountDownNode->SetSuspendingState(true);

		// play sound
		SoundManager::PlaySoundEffect(_SET_DRAG_SUCCESS_);

		// update obstacle manager right begin of move
		m_GameBoardManager.GetObstacleProcessManager()->UpdateAfterMove();

		PlayEffect2( false, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells,
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells,  m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells, true);
	}
}

void HelloWorld::PlayBeginTimeModeStageTextEffect()
{
	auto pTextSprite = Sprite::createWithSpriteFrameName("Stage.png");
	m_pTextEffectBatchNode->addChild(pTextSprite);
	
	float fPosX = pTextSprite->getContentSize().width + 10.f;

	char sStageNumber[10];
	sprintf(sStageNumber, "%d", m_iCurrentTimeModeStage);
	int iStageNumberLetterCount = strlen(sStageNumber);

	char sSpriteName[40];
	Sprite* pLetterSprite;
	for(int i=0; i< iStageNumberLetterCount; i++)
	{
		sprintf(sSpriteName, "StageNumber/%c.png", (unsigned char)sStageNumber[i]);
		pLetterSprite = Sprite::createWithSpriteFrameName(sSpriteName);
		pLetterSprite->setAnchorPoint(Point(0,0));
		pLetterSprite->setPosition(Point( fPosX, 0));
	
		pTextSprite->addChild(pLetterSprite);
		fPosX += pLetterSprite->getContentSize().width - 1.f;
	}
	
	pTextSprite->setPosition( Point( Director::getInstance()->getWinSize().width /2.f - 20.f, 450));	
	pTextSprite->setScale( 0.7f);
	pTextSprite->runAction(
		Sequence::create(
			Spawn::createWithTwoActions(
				ScaleTo::create(0.25f, 1.f),
				MoveBy::create( 0.25f, Point( 0, 50.f))),
			DelayTime::create(0.5f),
			Spawn::createWithTwoActions(
				FadeOut::create(0.2f),
				MoveBy::create( 0.2f, Point( 0, 80.f))),
			RemoveSelf::create(),
			NULL));
}

void HelloWorld::OnStartGame()
{	

	if (m_GameBoardManager.GetLevelConfig()->m_eGameModeType == _GMT_NORMAL_)
	{
		Sprite* pTextSprite = Sprite::createWithSpriteFrameName("Level_Start.png");
		Size textSize = pTextSprite->getContentSize();
		Size winSize = Director::getInstance()->getWinSize();

		pTextSprite->setPosition(Point( -textSize.width/2.f, winSize.height/2.f + 100.f));	
		m_pTextEffectBatchNode->addChild(pTextSprite);

		pTextSprite->runAction(
			Sequence::create(
				MoveTo::create( 0.6f, Point( winSize.width/2.f - 40.f, winSize.height/2.f + 100.f)),
				//MoveTo::create( 0.033f, Point( winSize.width/2.f - 40.f, winSize.height/2.f + 100.f)),
				DelayTime::create(0.9f),
				//MoveTo::create( 0.033f, Point( winSize.width/2.f - 70.f, winSize.height/2.f + 100.f)),
				Spawn::createWithTwoActions(
					EaseBackOut::create( MoveTo::create( 0.5f, Point( winSize.width + textSize.width/2.f, winSize.height/2.f + 100.f))),
					FadeOut::create( 1.f)),
				RemoveSelf::create(),
				NULL));

		SoundManager::PlaySoundEffect(_SET_LETS_FIND_);		
		SoundManager::PlaySpellingOfWord( this, m_GameBoardManager.GetGameWordManager()->GetMainWord(), 2.f, false);
	}
	else
	{
		m_bIsEffectPlaying = false;

		m_pTimeCountDownNode->Start();

		/*char sText[20];
		sprintf( sText, "STAGE %d", m_iCurrentTimeModeStage);
		auto label = LabelTTF::create(sText, "fonts/UTM Cookies.ttf", 80);		
		//label->setColor(Color3B( 100, 100, 100));
		label->setPosition( Point( Director::getInstance()->getWinSize().width /2.f, 450));
		label->disableStroke();		
		this->addChild(label, 2000);

		label->setScale( 0.7f);
		label->runAction(
			Sequence::create(
				Spawn::createWithTwoActions(
					ScaleTo::create(0.25f, 1.f),
					MoveBy::create( 0.25f, Point( 0, 50.f))),
				DelayTime::create(0.5f),
				Spawn::createWithTwoActions(
					FadeOut::create(0.2f),
					MoveBy::create( 0.2f, Point( 0, 80.f))),
				NULL));*/
	}

	// play sound effect 
	SoundManager::PlaySoundEffect(_SET_START_LEVEL_);
}

void HelloWorld::OnCompleteComboChain()
{
	if (!m_GameBoardManager.GetGameWordManager()->IsMainWordUnlocked() && m_GameBoardManager.GetCurrentMove()!=0)
	{
		m_bIsEffectPlaying = false;

		if (m_pTimeCountDownNode != NULL)
			 m_pTimeCountDownNode->SetSuspendingState(false);
	}

	// Play text effect
	if (m_GameBoardManager.GetPhaseMoveOfComboChain() >3)
	{
		Sprite* pTextSprite;

		switch( m_GameBoardManager.GetPhaseMoveOfComboChain()-1)
		{
			case 3:				
				pTextSprite = Sprite::createWithSpriteFrameName("3_Nice.png");
				SoundManager::PlaySoundEffect(_SET_ENCOURAGE_NICE_);
				break;
			case 4:
				pTextSprite = Sprite::createWithSpriteFrameName("4_Lovely.png");
				SoundManager::PlaySoundEffect(_SET_ENCOURAGE_LOVELY_);
				break;
			case 5:
				pTextSprite = Sprite::createWithSpriteFrameName("5_Beautiful.png");
				SoundManager::PlaySoundEffect(_SET_ENCOURAGE_BEAUTIFUL_);
				break;
			case 6:
				pTextSprite = Sprite::createWithSpriteFrameName("6_Briliant.png");
				SoundManager::PlaySoundEffect(_SET_ENCOURAGE_BRILLIANT_);
				break;
			case 7:
			default:
				pTextSprite = Sprite::createWithSpriteFrameName("7_Artistic.png");
				SoundManager::PlaySoundEffect(_SET_ENCOURAGE_ARTISTIC_);
				break;			
		}

		Size winSize = Director::getInstance()->getWinSize();

		pTextSprite->setPosition(Point( winSize.width/2.f, winSize.height/2.f));
		pTextSprite->setOpacity(0);
		pTextSprite->setScale(1.1f);
		m_pTextEffectBatchNode->addChild(pTextSprite);
		
		FiniteTimeAction* pFinalStep;
		if (m_GameBoardManager.GetGameWordManager()->IsMainWordUnlocked() || m_GameBoardManager.GetCurrentMove()==0)
			pFinalStep = Sequence::createWithTwoActions(
							CCCallFunc::create( this, callfunc_selector( HelloWorld::CheckEndGameAtEndChain)),
							RemoveSelf::create());
		else
			pFinalStep = RemoveSelf::create();

		pTextSprite->runAction( 
			Sequence::create(
				Spawn::createWithTwoActions(
					FadeIn::create( 0.3f),
					MoveBy::create( 0.3f, Point( 0, 35.f))),
				DelayTime::create(0.7f),
				Spawn::createWithTwoActions(
					FadeOut::create( 0.3f),
					ScaleTo::create(0.3f, 1.25f)),
				pFinalStep,
				NULL));
	}
	else
		CheckEndGameAtEndChain();
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

	if (m_GameBoardManager.RecheckAfterMoveV2( -1, -1, -1, -1, -1, -1, m_ComputeMoveResult.m_BasicMatchingDestroyedCells,
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells, m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells, false))
	{
		PlayEffect2( false, m_ComputeMoveResult.m_ConvertedComboCells, m_ComputeMoveResult.m_BasicMatchingDestroyedCells,
			m_ComputeMoveResult.m_ComboChainList, m_ComputeMoveResult.m_NewComboCells, m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells,  m_ComputeMoveResult.m_UnlockedLetterCells, m_ComputeMoveResult.m_NewCells, false);		
	}
	else
	{		
		if (!m_GameBoardManager.GetGameWordManager()->IsMainWordUnlocked() && m_GameBoardManager.GetCurrentMove()!=0)
		{
			// delay check hint
			this->runAction( CCSequence::create(
						CCDelayTime::create(0.5f),
						CCCallFunc::create( this, callfunc_selector( HelloWorld::CheckHintAfterMove)),
						NULL));		
		}


		//EndUnlockLetterAnimation();
		OnCompleteComboChain();		

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
			//PlayUnlockLettersOfMainWordAnimation(0.f);			
		}				

		// update obstacle manager after move
		//m_GameBoardManager.GetObstacleProcessManager()->UpdateAfterMove();

		//UpdateObstacleListAfterMove();
	}	
}

// if this called when user move row/column can cause error/lag???
void HelloWorld::CheckHintAfterMove()
{
	if (!m_GameBoardManager.findHintForGame())	
	{	
		if	(m_GameBoardManager.Shuffle(m_ComputeMoveResult.m_OriginalMovedCells, m_ComputeMoveResult.m_TargetMovedCells))
		{
			m_eTouchMoveState = _TMS_NONE_;
			m_bIsCellDragPlaying = false;			
			m_iMovingCellListLength = 0;
			m_bIsEffectPlaying = true;

			//CCLOG("Shuffle");

			MessageBox("Notice", "SHUFFLE!!!");

			auto& originalMovedCells = m_ComputeMoveResult.m_OriginalMovedCells;
			auto& targetMovedCells = m_ComputeMoveResult.m_TargetMovedCells;
			int iUpdatedZOrder;
			CellView tempViewMatrix[_BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];
			CellView tempMirrorViewMatrix[_BOARD_MAX_ROW_NUMBER_][ _BOARD_MAX_COLUMN_NUMBER_];
			
			Point centerPos( m_fBoardLeftPosition + m_GameBoardManager.GetColumnNumber()/2.f * m_SymbolSize.width, 
							m_fBoardBottomPosition + m_GameBoardManager.GetRowNumber()/2.f * m_SymbolSize.height);


			for (int i=0; i < originalMovedCells.size(); i++)
			{				
				Point pos(m_fBoardLeftPosition + targetMovedCells[i].m_iColumn * m_SymbolSize.width, 
							m_fBoardBottomPosition + targetMovedCells[i].m_iRow * m_SymbolSize.height);

				m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn].m_pSprite->stopAllActions();
				//m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn].m_pSprite->setPosition(pos);
				m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn].m_pSprite->runAction(
					Sequence::create( 
						DelayTime::create(0.3f), // + 0.01f * (targetMovedCells[i].m_iRow - originalMovedCells[i].m_iRow ),
						
						EaseOut::create( MoveTo::create( _TME_MOVE_CELL_TIME_, centerPos),			3.f ),
						EaseOut::create( MoveTo::create( _TME_MOVE_CELL_TIME_, pos),			3.f ),
						NULL));	
				if (m_BoardViewMirrorMatrix[originalMovedCells[i].m_iRow ][originalMovedCells[i].m_iColumn].m_pSprite != NULL)
				{
					m_BoardViewMirrorMatrix[originalMovedCells[i].m_iRow ][originalMovedCells[i].m_iColumn].m_pSprite->stopAllActions();
					m_BoardViewMirrorMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn].m_pSprite->setPosition(pos);
				}

				tempViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn] = m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn];
				tempMirrorViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn] = m_BoardViewMirrorMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn];
			}

			for (int i=0; i < originalMovedCells.size(); i++)
			{
				m_BoardViewMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn] = tempViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn];
				m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn] = tempMirrorViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn];

				iUpdatedZOrder = GetZOrder(targetMovedCells[i].m_iRow, targetMovedCells[i].m_iColumn, false);

				m_BoardViewMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite->setZOrder( iUpdatedZOrder);		
				m_BoardViewMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite->setVisible(true);

				if (m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite != NULL)
				{
					m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite->setZOrder(iUpdatedZOrder);
					m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite->setVisible(false);
				}
			}	
			
			if (m_pTimeCountDownNode != NULL)
				 m_pTimeCountDownNode->SetSuspendingState(true);

			this->runAction( CCSequence::create(
				CCDelayTime::create(1.2f),
				CCCallFunc::create( this, callfunc_selector( HelloWorld::CheckBoardStateAfterMove)),
				NULL));		

			//CCLOG("End shuffle");
		}		
		else
			MessageBox("Notice", "Sorry, game is over!!!");

		
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
		//EndUnlockLetterAnimation();		
		OnCompleteComboChain();
	}
}

void HelloWorld::ShowWinGamePopup()
{	
	std::string sCurrentChapterID = GameConfigManager::getInstance()->GetCurrentChapterID();
	int iCurrentLevel = GameConfigManager::getInstance()->GetCurrentLevelId();	 

	// extra, regenerate word for current level
	GameConfigManager::getInstance()->UpdateNewWordForLevel( sCurrentChapterID, iCurrentLevel);

	// show win popup	
	Breadcrumb::getInstance()->getSceneModePopBack();
	LevelMapScene* pLevelMap =  LevelMapScene::create();
	int iBonusQuestCompleted = m_GameBoardManager.GetBonusQuestManager()->CountBonusQuestCompleted();
	pLevelMap->getLayer()->showPopupEndGameWin(m_GameBoardManager.GetCurrentScore(),
		m_GameBoardManager.GetGameWordManager()->GetMainWord(), iCurrentLevel, sCurrentChapterID, m_GameBoardManager.GetEarnedStars(), iBonusQuestCompleted);
	Director::getInstance()->replaceScene(pLevelMap);
	
}

void HelloWorld::ShowFailGamePopup()
{
	std::string sCurrentChapterID = GameConfigManager::getInstance()->GetCurrentChapterID();
	int iCurrentLevel = GameConfigManager::getInstance()->GetCurrentLevelId();	 

	OutOfMovesNode* pOutOfMoveNode = OutOfMovesNode::createLayout( m_GameBoardManager.GetCurrentScore(), 
				m_GameBoardManager.GetGameWordManager()->GetMainWord(), iCurrentLevel, sCurrentChapterID);
	m_pHUDLayer->addChild( pOutOfMoveNode, 100);

	//SoundManager::PlaySoundEffect(_SET_FAIL_);
	UserTable::getInstance()->updateLife(1);

	//Game Tracking Level
	const Word& mainWord = m_GameBoardManager.GetGameWordManager()->GetMainWord();
	
	GameTracking::saveFileTrackingLevel(iCurrentLevel, 0, mainWord.m_iWordLength-mainWord.m_iRemainInactivatedCharacterCount, "Lose");
}


int HelloWorld::GetUnmoveCellsFromCurrentMovingRowOrColumn()
{
	int iBlankCellOfFullLine = 0;
	for(int iIndex = 0; iIndex< m_iMovingCellListLength; iIndex++)		
		if (m_MovingCellList[iIndex].m_pSprite == NULL)
			iBlankCellOfFullLine++;
	return iBlankCellOfFullLine;
}

void MoveAllChildrenToOtherNode(Sprite* pSourceNode, Sprite* pTargetNode)
{
	//SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
	//Sprite* pSprite;
	//pSprite->setDisplayFrame(NULL);

	Object* pChild;
	Sprite* pChildSprite;
	int iZOrder;
	CCARRAY_FOREACH( pSourceNode->getChildren(), pChild)
	{
		pChildSprite = (Sprite*)pChild;
		iZOrder = pChildSprite->getZOrder();
		pChildSprite->retain();
		pChildSprite->removeFromParentAndCleanup(false);				
		pTargetNode->addChild( pChildSprite, iZOrder);
		pChildSprite->release();
	}	
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
				m_fBoardLeftPosition + (m_iBeginMovingCellIndex + (( iColumn + iTranslatedOriginCell) % m_iMovingCellListLength )) * m_SymbolSize.width + fDeltaX - fMoveUnit * m_SymbolSize.width);
			
			if (iTranslatedMirrorCell != iTranslatedOriginCell)
			{
				m_MovingCellMirrorList[iColumn].m_pSprite->setVisible(true);
				m_MovingCellMirrorList[iColumn].m_pSprite->setPositionX(
					m_fBoardLeftPosition + (m_iBeginMovingCellIndex + (( iColumn + iTranslatedMirrorCell) % m_iMovingCellListLength )) * m_SymbolSize.width + fDeltaX - (fMoveUnit + bMeetSide * 1) * m_SymbolSize.width);				
			}
			else
			{
				m_MovingCellMirrorList[iColumn].m_pSprite->setVisible(false);
				
				m_MovingCellMirrorList[iColumn].m_pSprite->setPosition(m_MovingCellList[iColumn].m_pSprite->getPosition());				
				// NOTE: because anchor of moving cell is modified so its position is not at center of cell anymore
				//m_MovingCellMirrorList[iColumn].m_pSprite->setPositionY(m_MovingCellList[iColumn].m_pSprite->getPosition().y);
			}
		}
	}



	// temporary disable snap
	//return;

	// draw snap
	if (m_eTouchMoveState != _TMS_MOVE_HORIZONTAL_)
	{		
		return;
	}

	fMoveUnit = fDeltaX/m_SymbolSize.width;
				
	fMoveUnit = round(fMoveUnit);

	iMoveUnit = (int)fMoveUnit;

	if ( iMoveUnit == m_iSaveLastCellMoveDelta || iMoveUnit == 0 || iMoveUnit + iBlankCellOfFullLine == m_GameBoardManager.GetColumnNumber())
	{		
		if ( (iMoveUnit == 0 || iMoveUnit + iBlankCellOfFullLine == m_GameBoardManager.GetColumnNumber())  && iMoveUnit != m_iSaveLastCellMoveDelta && m_SnapSprites.size() >0 )
		{			
			RemoveSnap();
		}
		
		m_iSaveLastCellMoveDelta = iMoveUnit;

		return;
	}
	
	RemoveSnap();	
	
	m_iSaveLastCellMoveDelta = iMoveUnit;

	int iNumberOfColumn = m_GameBoardManager.GetColumnNumber();	
	int iTranslationCell = 0;	

	m_ComputeMoveResult.m_NewComboCells.clear(); // .Reset(false);
	if ( m_GameBoardManager.FastCheckBlocks( m_iBeginMovingCellIndex, m_iMovingCellListLength, m_SelectedCell.m_iRow, -1, -1, iMoveUnit, m_ComputeMoveResult.m_NewComboCells))
	{		
		// NOTE: because we need to get effective cells from target positions, we need to reverse sign
		// and for easier computation, we convert it to active value
		iMoveUnit =  m_iMovingCellListLength - iBlankCellOfFullLine - iMoveUnit;
		if (iMoveUnit < 0)
			iMoveUnit = 0;

		//m_pMoveHintNode = Sprite::createWithSpriteFrameName("Gem_A.png");
		//m_pMoveHintNode->setOpacity(0);
		//m_pBoardBatchNode->addChild(m_pMoveHintNode);

		Sprite* pSnapSprite, *pSnapMirrorSprite;
		auto snapEffectAction = RepeatForever::create(
									Sequence::create(					
										ScaleTo::create( 0.15f, 0.92f, 1.09f),
										ScaleTo::create( 0.25f, 1.13f, 0.88f),
										ScaleTo::create( 0.15f, 1.09f, 0.92f),
										NULL));

		int iLengthOfBlock, iBeginIndexOfBlock, iTempCalculatedColumn;
		SpriteFrame* pSpriteFrame, *pMirrorSpriteFrame, * pTempSpriteFrame;
		for(auto cell : m_ComputeMoveResult.m_NewComboCells)
		{
			iTempCalculatedColumn = cell.m_iColumn;
			iTranslationCell = 0;
			iLengthOfBlock = iNumberOfColumn;
			iBeginIndexOfBlock = 0;

			if (cell.m_iRow == m_SelectedCell.m_iRow && cell.m_iColumn >= m_iBeginMovingCellIndex && cell.m_iColumn < m_iBeginMovingCellIndex + m_iMovingCellListLength)
			{					
				iTempCalculatedColumn -= m_iBeginMovingCellIndex;
				iLengthOfBlock = m_iMovingCellListLength;
				iBeginIndexOfBlock = m_iBeginMovingCellIndex;
				for(int iStep=1; iStep <= abs(iMoveUnit); iStep++)
				{
					iTranslationCell += 1;

					if (m_GameBoardManager.IsBlankCell( m_SelectedCell.m_iRow, (iTempCalculatedColumn + iTranslationCell) % iLengthOfBlock + iBeginIndexOfBlock))
					{
						iStep--;
					}
				}
			}
			int iCalculatedColumn = (iTempCalculatedColumn + iTranslationCell) % iLengthOfBlock + iBeginIndexOfBlock;
						
			pSpriteFrame = GetSpriteFrameFromSnapGemID( cell.m_iGemID, m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:cell.m_eGemComboType);
			pTempSpriteFrame = m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->getDisplayFrame();
			pTempSpriteFrame->retain();
			m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->setTag((int) pTempSpriteFrame);			
			m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->setDisplayFrame(pSpriteFrame);
									
			m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->runAction(snapEffectAction->clone());

			m_SnapSprites.push_back(m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite);

			if (cell.m_iRow == m_SelectedCell.m_iRow) // change frame of mirror too
			{
				m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->setOpacity(254);
				m_BoardViewMirrorMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->setOpacity(254);

				pMirrorSpriteFrame = GetSpriteFrameFromSnapGemID( cell.m_iGemID, m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:cell.m_eGemComboType);
				pTempSpriteFrame = m_BoardViewMirrorMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->getDisplayFrame();
				pTempSpriteFrame->retain();
				m_BoardViewMirrorMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->setTag((int) pTempSpriteFrame);
				m_BoardViewMirrorMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->setDisplayFrame(pMirrorSpriteFrame);

				m_BoardViewMirrorMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->runAction(snapEffectAction->clone());

				m_SnapSprites.push_back(m_BoardViewMirrorMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite);
			}
			else //change anchor of other snap cell
			{
				m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->runAction( SetAnchorAction::Create(Point( 0.5f, 0)));
				m_SnapNeedResetAnchorSprites.push_back(m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite);
			}
			

			/*
			pSnapSprite = Sprite::createWithSpriteFrameName( GetImageFileFromSnapGemID( cell.m_iGemID, 
				m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:cell.m_eGemComboType).c_str());
			pSnapSprite->runAction( SetAnchorAction::Create(Point( 0.5f, 0.2f)));
			pSnapSprite->setScaleX(1.09f);
			pSnapSprite->setScaleY(0.92f);			

			pSnapMirrorSprite = NULL;

			Size size = m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->getContentSize();
			if (cell.m_iRow != m_SelectedCell.m_iRow)
			{
				MoveAllChildrenToOtherNode( m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite, pSnapSprite);
				m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->addChild(pSnapSprite, -1);
				m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite->setOpacity(0);
				

				pSnapSprite->setTag(0);
			}
			else
			{				
				MoveAllChildrenToOtherNode( m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite, pSnapSprite);
				m_BoardViewMatrix[cell.m_iRow][ iCalculatedColumn ].m_pSprite->addChild(pSnapSprite, -1);
				m_BoardViewMatrix[cell.m_iRow][ iCalculatedColumn ].m_pSprite->setOpacity(0);


				// add hint to mirror too
				pSnapMirrorSprite = Sprite::createWithSpriteFrameName( GetImageFileFromSnapGemID( cell.m_iGemID, 
																				m_BoardViewMatrix[cell.m_iRow][iCalculatedColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:cell.m_eGemComboType).c_str());				
				pSnapMirrorSprite->runAction( SetAnchorAction::Create(Point( 0.5f, 0.2f)));
				pSnapMirrorSprite->setScaleX(1.09f);
				pSnapMirrorSprite->setScaleY(0.92f);

				//if (m_BoardViewMirrorMatrix[cell.m_iRow][ (cell.m_iColumn + iTranslationCell) % iNumberOfColumn ].m_pSprite != NULL)

				MoveAllChildrenToOtherNode( m_BoardViewMirrorMatrix[cell.m_iRow][iCalculatedColumn].m_pSprite, pSnapMirrorSprite);
				m_BoardViewMirrorMatrix[cell.m_iRow][ iCalculatedColumn ].m_pSprite->addChild(pSnapMirrorSprite, -1);
				m_BoardViewMirrorMatrix[cell.m_iRow][ iCalculatedColumn ].m_pSprite->setOpacity(0);

				// we need to mark it so we can reset its scale later
				m_BoardViewMatrix[cell.m_iRow][ iCalculatedColumn ].m_pSprite->setScaleX(1.f);
				m_BoardViewMatrix[cell.m_iRow][ iCalculatedColumn ].m_pSprite->setScaleY(1.f);
				m_BoardViewMirrorMatrix[cell.m_iRow][ iCalculatedColumn ].m_pSprite->setScaleX(1.f);
				m_BoardViewMirrorMatrix[cell.m_iRow][ iCalculatedColumn ].m_pSprite->setScaleY(1.f);
				pSnapSprite->setTag(1);
				pSnapMirrorSprite->setTag(1);
			}

			pSnapSprite->setPosition( Point(size.width/2.f, size.height/2.f ));
			
			m_SnapSprites.push_back(pSnapSprite);	
			pSnapSprite->runAction(snapEffectAction->clone());

			if (pSnapMirrorSprite != NULL)
			{
				pSnapMirrorSprite->setPosition( Point(size.width/2.f, size.height/2.f ));

				m_SnapSprites.push_back(pSnapMirrorSprite);
				pSnapMirrorSprite->runAction(snapEffectAction->clone());
			}*/
		}

	}
}

void HelloWorld::RemoveSnap()
{
	if ( m_SnapSprites.size() >0 )
	{
		//CCLOG("Remove hint");		
		SpriteFrame* pSpriteFrame;
		for(auto sprite:m_SnapSprites)
		{
			/*if (sprite->getParent() != NULL)
			{
				((Sprite*)sprite->getParent())->setOpacity(255);
				if (sprite->getTag() == 1) // parent cell is on drag row/column
				{
					sprite->getParent()->setScaleX(1.1f);
					sprite->getParent()->setScaleY(0.9f);
				}
			}			
			MoveAllChildrenToOtherNode( sprite, (Sprite*)sprite->getParent());
			sprite->removeFromParentAndCleanup(true);*/
			if (sprite->getOpacity() == 254)			
			{			
				sprite->setScaleX(1.1f);
				sprite->setScaleY(0.9f);
			}
			else
			{			
				sprite->setScale(1.f);
			}

			pSpriteFrame = (SpriteFrame*)sprite->getTag();
			sprite->setDisplayFrame(pSpriteFrame);
			sprite->stopAllActions();
			
			delete pSpriteFrame;
		}

		for(auto sprite: m_SnapNeedResetAnchorSprites)
		{
			sprite->runAction(SetAnchorAction::Create(Point( 0.5f, 0.5f)));
		}
		
		m_SnapSprites.clear();
		m_SnapNeedResetAnchorSprites.clear();
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
				m_fBoardBottomPosition + (m_iBeginMovingCellIndex + ((iRow +  iTranslatedOriginCell) % m_iMovingCellListLength )) * m_SymbolSize.height + fDeltaY - fMoveUnit  * m_SymbolSize.height);
			
			if (iTranslatedMirrorCell != iTranslatedOriginCell)
			{
				m_MovingCellMirrorList[iRow].m_pSprite->setVisible(true);
				m_MovingCellMirrorList[iRow].m_pSprite->setPositionY(
					m_fBoardBottomPosition + (m_iBeginMovingCellIndex + (( iRow + iTranslatedMirrorCell) % m_iMovingCellListLength )) * m_SymbolSize.height + fDeltaY - ( fMoveUnit + bMeetSide*1) * m_SymbolSize.height);
			}
			else
			{
				m_MovingCellMirrorList[iRow].m_pSprite->setVisible(false);
				m_MovingCellMirrorList[iRow].m_pSprite->setPosition(m_MovingCellList[iRow].m_pSprite->getPosition());
			}	
		}
	}


	// temporary disable snap
	//return;

	// draw snap
	if (m_eTouchMoveState != _TMS_MOVE_VERTICAL_)
		return;

	fMoveUnit = fDeltaY/m_SymbolSize.height;
				
	fMoveUnit = round(fMoveUnit);

	iMoveUnit = (int)fMoveUnit;
	if ( iMoveUnit == m_iSaveLastCellMoveDelta || iMoveUnit == 0 || iMoveUnit + iBlankCellOfFullLine == m_GameBoardManager.GetRowNumber())
	{		
		if ( (iMoveUnit == 0 || iMoveUnit + iBlankCellOfFullLine == m_GameBoardManager.GetRowNumber()) && iMoveUnit != m_iSaveLastCellMoveDelta && m_SnapSprites.size() >0 )
		{
			RemoveSnap();
		}
		m_iSaveLastCellMoveDelta = iMoveUnit;

		return;
	}

	RemoveSnap();

	
	m_iSaveLastCellMoveDelta = iMoveUnit;

	int iNumberOfRow = m_GameBoardManager.GetRowNumber();
	//iSign = 1;
	//if (iMoveUnit != 0)
		///iSign = -iMoveUnit/ abs(iMoveUnit);
	int iTranslationCell = 0;
	int iLengthOfBlock, iBeginIndexOfBlock, iTempCalculatedRow;
	
	m_ComputeMoveResult.m_NewComboCells.clear();// Reset(false);
	if ( m_GameBoardManager.FastCheckBlocks( m_iBeginMovingCellIndex, m_iMovingCellListLength, -1, m_SelectedCell.m_iColumn, iMoveUnit, -1, m_ComputeMoveResult.m_NewComboCells))
	{		
		// NOTE: because we need to get effective cells from target positions, we need to reverse sign
		// and for easier computation, we convert it to active value
		iMoveUnit =  m_iMovingCellListLength - iBlankCellOfFullLine - iMoveUnit;
		if (iMoveUnit < 0)
			iMoveUnit = 0;

		//m_pMoveHintNode = Sprite::createWithSpriteFrameName("Gem_A.png");
		//m_pMoveHintNode->setOpacity(0);
		//m_pBoardBatchNode->addChild(m_pMoveHintNode);
		Sprite* pSnapSprite, *pSnapMirrorSprite;
		auto snapEffectAction = RepeatForever::create(
									Sequence::create(					
										ScaleTo::create( 0.15f, 0.92f, 1.09f),
										ScaleTo::create( 0.25f, 1.13f, 0.88f),
										ScaleTo::create( 0.15f, 1.09f, 0.92f),
										NULL));
		SpriteFrame* pSpriteFrame, *pMirrorSpriteFrame, * pTempSpriteFrame;

		for(auto cell : m_ComputeMoveResult.m_NewComboCells)
		{
			iTempCalculatedRow = cell.m_iRow;
			iTranslationCell = 0;
			iLengthOfBlock = iNumberOfRow;
			iBeginIndexOfBlock = 0;

			if (cell.m_iColumn == m_SelectedCell.m_iColumn  && cell.m_iRow >= m_iBeginMovingCellIndex && cell.m_iRow < m_iBeginMovingCellIndex + m_iMovingCellListLength)
			{
				iTempCalculatedRow -= m_iBeginMovingCellIndex;
				iLengthOfBlock = m_iMovingCellListLength;
				iBeginIndexOfBlock = m_iBeginMovingCellIndex;

				for(int iStep=1; iStep <= abs(iMoveUnit); iStep++)
				{
					iTranslationCell += 1;					

					if (m_GameBoardManager.IsBlankCell( (iTempCalculatedRow + iTranslationCell) % iLengthOfBlock + iBeginIndexOfBlock,  m_SelectedCell.m_iColumn))
					{
						iStep--;
					}
				}
			}

			int iCalculatedRow = (iTempCalculatedRow+ iTranslationCell) % iLengthOfBlock + iBeginIndexOfBlock;

			pSpriteFrame = GetSpriteFrameFromSnapGemID( cell.m_iGemID, 
				m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:cell.m_eGemComboType);
			pTempSpriteFrame = m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->getDisplayFrame();
			pTempSpriteFrame->retain();
			m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->setTag((int) pTempSpriteFrame);			
			m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->setDisplayFrame(pSpriteFrame);

			m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->runAction(snapEffectAction->clone());
			

			m_SnapSprites.push_back(m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite);

			if (cell.m_iColumn == m_SelectedCell.m_iColumn) // change frame of mirror too
			{
				m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->setOpacity(254);
				m_BoardViewMirrorMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->setOpacity(254);

				pMirrorSpriteFrame = GetSpriteFrameFromSnapGemID( cell.m_iGemID, m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:cell.m_eGemComboType);
				pTempSpriteFrame = m_BoardViewMirrorMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->getDisplayFrame();
				pTempSpriteFrame->retain();
				m_BoardViewMirrorMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->setTag((int) pTempSpriteFrame);
				m_BoardViewMirrorMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->setDisplayFrame(pMirrorSpriteFrame);

				m_BoardViewMirrorMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->runAction(snapEffectAction->clone());

				m_SnapSprites.push_back(m_BoardViewMirrorMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite);
			}		

			/*
			pSnapSprite = Sprite::createWithSpriteFrameName( GetImageFileFromSnapGemID( cell.m_iGemID, 
				m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:cell.m_eGemComboType).c_str());			
			pSnapSprite->runAction( SetAnchorAction::Create(Point( 0.5f, 0.2f)));
			pSnapSprite->setScaleX(1.09f);
			pSnapSprite->setScaleY(0.92f);
			
			pSnapMirrorSprite = NULL;

			Size size = m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->getContentSize();
			if (cell.m_iColumn != m_SelectedCell.m_iColumn)
			{				
				MoveAllChildrenToOtherNode( m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite, pSnapSprite);
				m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->addChild(pSnapSprite, -1);
				m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->setOpacity(0);
				
			} 
			else
			{	
				MoveAllChildrenToOtherNode( m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite, pSnapSprite);
				m_BoardViewMatrix[ iCalculatedRow][cell.m_iColumn].m_pSprite->addChild(pSnapSprite, -1);
				m_BoardViewMatrix[ iCalculatedRow][cell.m_iColumn].m_pSprite->setOpacity(0);


				// add hint to mirror too
				pSnapMirrorSprite = Sprite::createWithSpriteFrameName( GetImageFileFromSnapGemID( cell.m_iGemID, 
					m_BoardViewMatrix[iCalculatedRow][cell.m_iColumn].m_iGemLetterBlockID>=0?_GCT_HAS_LETTER_:cell.m_eGemComboType).c_str());				

				pSnapMirrorSprite->runAction( SetAnchorAction::Create(Point( 0.5f, 0.2f)));
				pSnapMirrorSprite->setScaleX(1.09f);
				pSnapMirrorSprite->setScaleY(0.92f);

				//if ( m_BoardViewMirrorMatrix[ iCalculatedRow][cell.m_iColumn].m_pSprite != NULL)
				MoveAllChildrenToOtherNode( m_BoardViewMirrorMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite, pSnapMirrorSprite);
				m_BoardViewMirrorMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->addChild(pSnapMirrorSprite, -1);
				m_BoardViewMirrorMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->setOpacity(0);


				// we need to mark it so we can reset its scale later
				m_BoardViewMatrix[ iCalculatedRow][cell.m_iColumn].m_pSprite->setScaleX(1.f);
				m_BoardViewMatrix[ iCalculatedRow][cell.m_iColumn].m_pSprite->setScaleY(1.f);
				m_BoardViewMirrorMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->setScaleX(1.f);
				m_BoardViewMirrorMatrix[iCalculatedRow][cell.m_iColumn].m_pSprite->setScaleY(1.f);
				pSnapSprite->setTag(1);
				pSnapMirrorSprite->setTag(1);

			}

			pSnapSprite->setPosition( Point(size.width/2.f, size.height/2.f ));
			pSnapSprite->runAction(snapEffectAction->clone());
			
			m_SnapSprites.push_back(pSnapSprite);
			if (pSnapMirrorSprite != NULL)
			{
				pSnapMirrorSprite->setPosition(Point(size.width/2.f, size.height/2.f ));
				m_SnapSprites.push_back(pSnapMirrorSprite);

				pSnapMirrorSprite->runAction(snapEffectAction->clone());
			}*/
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

	float fDelayTime = 0.1f; //2f;	
	
	int iNumberOfRow = m_GameBoardManager.GetRowNumber();
	int iNumberOfColumn = m_GameBoardManager.GetColumnNumber();

	//if (bIsBonusEndGamePhase)
		//fDelayTime = 0;

	float fTotalDestroyCellTime = fDelayTime+ _TME_BASIC_DESTROY_CELL_TIME_;	

	if (bIsBonusEndGamePhase)
	{		
		fDelayTime = 0;
		fTotalDestroyCellTime = _TME_BASIC_DESTROY_CELL_TIME_;

		// play effect convert normal cells to combo cells
		if (convertedToComboCells.size() > 0) // && convertedToComboCells[0].m_eGemComboType != _GCT_BONUS_END_GAME_CRAZY_PET_) //temporary disable transform of crasy pet
		{
			float fDelayPerConvertedCell = 0.05f;
			auto pCombo5AnimBolt = AnimationCache::getInstance()->getAnimation("effectCombo6_Light");		
			Point rootEffect( 595.f, 30.f);
			int iIndex = 0;

			for(auto cell: convertedToComboCells)
			{
				Point pos(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height);

				Sprite* pSprite;

				if (convertedToComboCells[0].m_eGemComboType != _GCT_BONUS_END_GAME_CRAZY_PET_)
				{					
					// create combo cell
					pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(cell.m_iGemID, cell.m_eGemComboType).c_str());
				}
				else
				{
					//PlayChangeColorEffectOnSprite( m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite, iIndex * fDelayPerConvertedCell );
					pSprite = Sprite::createWithSpriteFrameName( "Bonus_Rocket.png");
				}

				// play effect convert cell
				BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, iIndex * fDelayPerConvertedCell, _TME_BASIC_DESTROY_CELL_TIME_);
				pSprite->setPosition(pos);

				//pSprite->setScale(0.65f);
				m_pBoardBatchNode->addChild(pSprite, GetZOrder( cell.m_iRow, cell.m_iColumn, false) + 100);

				m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;

				pSprite->setOpacity(0);
				pSprite->setScale(2.f);//1.5f);

				pSprite->runAction( 
					Sequence::create( 
						DelayTime::create( iIndex * fDelayPerConvertedCell),
						Spawn::createWithTwoActions(
							FadeIn::create(_TME_BASIC_DESTROY_CELL_TIME_/2.f),
							ScaleTo::create(_TME_BASIC_DESTROY_CELL_TIME_/2.f, 1.f, 1.f)),
										//EaseIn::create( FadeIn::create(_TME_BASIC_DESTROY_CELL_TIME_/3.f), 2.f),
										//EaseIn::create(ScaleTo::create(_TME_BASIC_DESTROY_CELL_TIME_/3.f, 1.f, 1.f), 2.f),
						NULL));				

				

				// bolt effect
				Point vector( pos.x - rootEffect.x, pos.y - rootEffect.y);
				auto fAngle = atan2f( vector.y, vector.x);
				float fDistance = sqrtf( vector.x*vector.x + vector.y*vector.y);

				auto pComboEffectSprite = Sprite::createWithSpriteFrameName("Combo6_000.png");
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

			fDelayTime += _TME_BASIC_DESTROY_CELL_TIME_  + (iIndex+2) * fDelayPerConvertedCell;			
		}
	}


	// play sound
	if (basicMatchingDestroyedCells.size() > 0)
		SoundManager::PlaySoundEffect(_SET_BASIC_MATCH_3_);

	//CCLOG("Destroy cells");

	// destroy cells by basic matching 
	int iTotalDestroyCell = 0;
	for(auto cell: basicMatchingDestroyedCells)
	{			
		if (cell.m_iRow >= 0 && cell.m_iColumn >= 0)
		{
			iTotalDestroyCell++;
			BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime,_TME_BASIC_DESTROY_CELL_TIME_, true);

			PlayEarnScoreEffect( m_GameBoardManager.IncreaseScoreForDestroyCells( 1, _CET_BASIC_MATCHING_), cell, fDelayTime);
		}
		else
		{
			cell.m_iRow = 0;
		}		
	}

	// increase score by basic matching
	//m_GameBoardManager.IncreaseScoreForDestroyCells( iTotalDestroyCell, _CET_BASIC_MATCHING_);

	//CCLOG("Before combo chain");

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
			PlayCombo4_4_4Effect(pComboEffect, fCurrentDelayComboChain, _TME_BASIC_COMBO_EXECUTE_TIME_ * 1.2f);
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
			//CCLOG("Before play end game effect combo");
			PlayComboEndGameBonusEffect(pComboEffect, fCurrentDelayComboChain, _TME_BASIC_COMBO_EXECUTE_TIME_);
			//CCLOG("After play end game effect combo");
		}
		else  if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType >= _CET_BIRD_EXPLOSION_EFFECT_ && pComboEffect->m_ComboEffectDescription.m_eComboEffectType <= _CET_HAMSTER_RUN_RIGHT_DOWN_EFFECT_)
		{
			PlayCrazyPetEndGameBonusEffect(pComboEffect, fCurrentDelayComboChain, _TME_BASIC_COMBO_EXECUTE_TIME_);
		}
		else if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_4_EFFECT_ || pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_5_EFFECT_ )
		{
			PlayActivateCommonDoubleAndTrippleComboEffect(pComboEffect, fCurrentDelayComboChain, _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_, true, true);
		}

		// ******************************
		if (fCurrentDelayComboChain + _TME_BASIC_COMBO_EXECUTE_TIME_> fTotalDestroyCellTime)
			fTotalDestroyCellTime =  fCurrentDelayComboChain + _TME_BASIC_COMBO_EXECUTE_TIME_;		


		// destroy cells by combo
		ComboEffectCell newComboCell;
		if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_4_EFFECT_ || pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_5_EFFECT_ ) 
		{
			// temporary hardcode for sound effect of combo 6-4, 6-5 here
			SoundManager::PlaySoundEffect(_SET_ACTIVATE_DOUBLE_COMBO_, fCurrentDelayComboChain);

			newComboCell.m_eGemComboType = (pComboEffect->m_ComboEffectDescription.m_eComboEffectType==_CET_6_4_EFFECT_)?_GCT_COMBO4_:_GCT_COMBO5_;
			newComboCell.m_iGemID = pComboEffect->m_ComboEffectDescription.m_iGemID;
		}

		iTotalDestroyCell = 0;
		int iScorePerGem = m_GameBoardManager.GetScorePerGemDestroyByCombo(pComboEffect->m_ComboEffectDescription.m_eComboEffectType);
		for(auto cell: pComboEffect->m_DestroyedCells)
		{
			if (cell.m_iDestroyPhaseIndex >=0 && (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_4_EFFECT_ || pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_6_5_EFFECT_ )
				&& cell.m_bIsCompleteDestroyed)
			{			
				newComboCell.m_iRow = cell.m_iRow;
				newComboCell.m_iColumn = cell.m_iColumn;
				BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime + cell.m_fDestroyAtTime, _TME_BASIC_DESTROY_CELL_TIME_); //0.03f);				
				
				AddNewComboCell (newComboCell, fDelayTime + cell.m_fDestroyAtTime, 0.03f, false);
			}
			else if (cell.m_bIsCompleteDestroyed)
			{
				if (cell.m_bIsEarnScore)
				{
					iTotalDestroyCell++;
					PlayEarnScoreEffect( iScorePerGem, cell, fDelayTime + cell.m_fDestroyAtTime);
				}

				BasicDestroyCellUlti( cell.m_iRow, cell.m_iColumn, fDelayTime + cell.m_fDestroyAtTime, _TME_BASIC_DESTROY_CELL_TIME_, true);
				
				if (fDelayTime + cell.m_fDestroyAtTime + _TME_BASIC_DESTROY_CELL_TIME_ > fTotalDestroyCellTime)
					fTotalDestroyCellTime = fDelayTime + cell.m_fDestroyAtTime + _TME_BASIC_DESTROY_CELL_TIME_;
			}		
		}

		// increase score by combo		
		int iActivationScore = m_GameBoardManager.IncreaseScoreForDestroyCells( iTotalDestroyCell, pComboEffect->m_ComboEffectDescription.m_eComboEffectType, pComboEffect->m_ComboEffectDescription.m_iActivationScoreRatio)
			- iTotalDestroyCell * iScorePerGem;
		if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType <= _GCT_COMBO6_)
			PlayEarnScoreEffect( iActivationScore, pComboEffect->m_ComboEffectDescription.m_Position, fCurrentDelayComboChain);
		else
			PlayEarnScoreEffect( iActivationScore, pComboEffect->m_ComboEffectDescription.m_Position, fCurrentDelayComboChain + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_);
		// clean data
		delete pComboEffect;
	}	

	//CCLOG("Before check bonus quest");

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
				SoundManager::PlaySoundEffect(_SET_DESTROY_LETTER_);

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
						const LevelBossConfig& bossLevelConfig = ((LevelConfig*) m_GameBoardManager.GetLevelConfig())->m_BossConfig;

						m_pWordCollectBoardRenderNode->PlayUnlockLetterEffect( iUnlockedLetterOfMainWord, fDelayEffectTime, iLetter,  //m_BoardViewMatrix[iRow][iColumn].m_iLetter, 
							ccp(m_fBoardLeftPosition + (  bossLevelConfig.m_Position.m_iColumn + bossLevelConfig.m_iWidth/4.f)  * m_SymbolSize.width, 
								m_fBoardBottomPosition + (  bossLevelConfig.m_Position.m_iRow + bossLevelConfig.m_iHeight/4.f) * m_SymbolSize.height));

						//m_pWordCollectBoardRenderNode->UnlockLetter(iUnlockedLetterOfMainWord);

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

	//CCLOG("Before create combo cells");

	// create new combo cells, this is right after basic matching cells are destroyed
	for(auto cell: newComboCells)
	{
		AddNewComboCell (cell, fDelayTime + 0.05f, _TME_BASIC_DESTROY_CELL_TIME_);			

		PlayEarnScoreEffect( m_GameBoardManager.IncreaseScoreForCreateCombo(cell.m_eGemComboType), cell, fDelayTime+0.1f);

		switch( cell.m_eGemComboType)
		{
			case _GCT_COMBO4_:
				SoundManager::PlaySoundEffect(_SET_CREATE_COMBO_4_, fDelayTime);			
				break;
			case _GCT_COMBO5_:
				SoundManager::PlaySoundEffect(_SET_CREATE_COMBO_5_, fDelayTime);
				break;
			case _GCT_COMBO6_:
				SoundManager::PlaySoundEffect(_SET_CREATE_COMBO_6_, fDelayTime);
				break;
		}
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
					pOldChildSprite->runAction(						
						Sequence::create(
							DelayTime::create( fDelayTime + cell.m_fEffectiveTime),	
							Spawn::createWithTwoActions(
								ScaleTo::create( 0.133f, 2.f),
								FadeOut::create( 0.133f)),
							RemoveSelf::create(),
							NULL));
					//->removeFromParentAndCleanup(true);

				if (cell.m_iGemLetterBlockID >= 0) //iLetter < 255)
				{
					data = m_GameBoardManager.GetGemLetterData(cell.m_iGemLetterBlockID);							
					AddLetterToGem( cell, m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), data.m_iLetter, cell.m_iGemLetterBlockID,
						i!=0, false, (i==0)?(fDelayTime + cell.m_fEffectiveTime):0, true);

					if (i==0)
					{
						// play sound effect 					
						SoundManager::PlaySoundEffect(_SET_COMPLETE_WORD_, fDelayTime + cell.m_fEffectiveTime);
					}
				}
			}
					
			//m_pBoardBatchNode->addChild(pSprite);						
		}		
	}	
	
	// move cells
	int iUpdatedZOrder;
	/*auto afterMoveEffectAction = Sequence::create(					
					SetAnchorAction::Create( Point( 0.5f, -0.5f)),
					ScaleTo::create( 0.015f, 1.06f, 0.92f),
					ScaleTo::create( 0.106f, 0.95f, 1.05f),
					ScaleTo::create( 0.05f, 1.03f, 0.96f),
					ScaleTo::create( 0.05f, 1.f, 1.f),
					ResetAnchorToCenterAction::Create(),
					NULL);*/

	float fPercentChange;
			/*Sequence::create(					
					SetAnchorAction::Create( Point( 0.5f, -0.5f)),
					ScaleTo::create( 0.015f, 1.f + 0.06f * fPercentChange , 1- 0.08f * fPercentChange),
					ScaleTo::create( 0.106f, 1.f - 0.05f*fPercentChange, 1.f + 0.05f * fPercentChange),
					ScaleTo::create( 0.05f, 1.f + 0.03f * fPercentChange, 1.f - 0.04* fPercentChange),
					ScaleTo::create( 0.05f, 1.f, 1.f),
					ResetAnchorToCenterAction::Create(),
					NULL);*/

	for (int i=0; i < originalMovedCells.size(); i++)
	{		

		fPercentChange = (originalMovedCells[i].m_iRow - targetMovedCells[i].m_iRow + 1.f) * 1.f / iNumberOfRow;

		//if ( m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn].m_pSprite == NULL)
		//	CCLOG("3- Effect NULL, %d, %d", originalMovedCells[i].m_iRow, originalMovedCells[i].m_iColumn);
		m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn].m_pSprite->runAction(
			Sequence::create( 
				DelayTime::create(fTotalDestroyCellTime + 0.027f * originalMovedCells[i].m_iRow), // + 0.01f * (targetMovedCells[i].m_iRow - originalMovedCells[i].m_iRow ),
				EaseOut::create( MoveTo::create( _TME_MOVE_CELL_TIME_,
					ccp(m_fBoardLeftPosition + targetMovedCells[i].m_iColumn * m_SymbolSize.width, 
					m_fBoardBottomPosition + targetMovedCells[i].m_iRow * m_SymbolSize.height)),			1.f),
				//afterMoveEffectAction->clone(),
				SetAnchorAction::Create( Point( 0.5f, -0.5f)),
				ScaleTo::create( 0.015f, 1.f + 0.06f * fPercentChange , 1- 0.08f * fPercentChange),
				ScaleTo::create( 0.106f, 1.f - 0.05f*fPercentChange, 1.f + 0.05f * fPercentChange),
				ScaleTo::create( 0.05f, 1.f + 0.03f * fPercentChange, 1.f - 0.04* fPercentChange),
				ScaleTo::create( 0.05f, 1.f, 1.f),
				ResetAnchorToCenterAction::Create(),

				NULL));




		m_BoardViewMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn] = m_BoardViewMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn];				
		m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn] = m_BoardViewMirrorMatrix[originalMovedCells[i].m_iRow][originalMovedCells[i].m_iColumn];					

		iUpdatedZOrder = GetZOrder(targetMovedCells[i].m_iRow, targetMovedCells[i].m_iColumn, false);

		m_BoardViewMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite->setZOrder( iUpdatedZOrder);		
		if (m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite != NULL)
			m_BoardViewMirrorMatrix[targetMovedCells[i].m_iRow ][targetMovedCells[i].m_iColumn].m_pSprite->setZOrder(iUpdatedZOrder);
	}	
	

	// generate new cells

	// NOTE: drop sound effect not match to animation!!!
	// play sound
	if (newCells.size() > 0)
		SoundManager::PlaySoundEffect(_SET_DROP_, fTotalDestroyCellTime + 0.027f * (m_GameBoardManager.GetRowNumber() - newCells[0].m_iRow));
	else
	{
		fDelayTime = fDelayTime;
	}

	//CCLOG("Create new gems");

	//std::vector<unsigned char> outputLettersForGems;
	NewCellInfo cell;
	//unsigned char iLetter = 255;
	
	//m_GameBoardManager.GetGameWordManager()->GenerateNewLetters( newCells.size(), outputLettersForGems, bIsNewMove);		

	for(int iGemIndex=0; iGemIndex < newCells.size(); iGemIndex++)
	{
		//iLetter = (unsigned char)outputLettersForGems[iGemIndex];
		cell = newCells[iGemIndex];

		fPercentChange = ( iNumberOfRow - cell.m_iRow) * 1.f / iNumberOfRow;

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
						DelayTime::create(fTotalDestroyCellTime + 0.03f* (m_GameBoardManager.GetRowNumber() - cell.m_iRow)),
						//MoveTo::create(_TME_MOVE_CELL_TIME_,	//0.027f
							//ccp(m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, 
								//	m_fBoardBottomPosition + cell.m_iRow  * m_SymbolSize.height)),
						EaseOut::create( MoveTo::create(_TME_MOVE_CELL_TIME_,
								ccp(m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, 
								m_fBoardBottomPosition + cell.m_iRow  * m_SymbolSize.height)), 2.f),
					
						//afterMoveEffectAction->clone(),
						SetAnchorAction::Create( Point( 0.5f, -0.5f)),
						ScaleTo::create( 0.015f, 1.f + 0.06f * fPercentChange , 1- 0.08f * fPercentChange),
						ScaleTo::create( 0.106f, 1.f - 0.05f*fPercentChange, 1.f + 0.05f * fPercentChange),
						ScaleTo::create( 0.05f, 1.f + 0.03f * fPercentChange, 1.f - 0.04* fPercentChange),
						ScaleTo::create( 0.05f, 1.f, 1.f),
						ResetAnchorToCenterAction::Create(),

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
	//delete afterMoveEffectAction;


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
					pSprite = Sprite::createWithSpriteFrameName( GetImageFileFromGemID(m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), _GCT_HAS_LETTER_).c_str());
					Point pos(m_fBoardLeftPosition + cell.m_iColumn  * m_SymbolSize.width, m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height);
					pSprite->setPosition(pos);
					m_pBoardBatchNode->addChild(pSprite,  GetZOrder( cell.m_iRow, cell.m_iColumn, false));

					if (i==0)
					{
						m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite = pSprite;
						Sprite* letterSprite = AddLetterToGem(cell, m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), data.m_iLetter, cell.m_iGemLetterBlockID, false, true);

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
						AddLetterToGem(cell, m_GameBoardManager.GetCellValue(cell.m_iRow, cell.m_iColumn), data.m_iLetter, cell.m_iGemLetterBlockID, true, true);
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
					CCDelayTime::create(fTotalDestroyCellTime+ _TME_MOVE_CELL_TIME_ + 0.1f ),
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
						CCDelayTime::create(fTotalDestroyCellTime + _TME_MOVE_CELL_TIME_*2 - 0.1f),
						CCCallFunc::create( this, callfunc_selector( HelloWorld::CheckBoardStateAfterMove)),
						NULL));		
	}

	// update display of score/move 
	if (m_eGameModeType == _GMT_NORMAL_)
	{
		m_pStatusLayer->setCurrentMove( m_GameBoardManager.GetCurrentMove());
		m_pStatusLayer->setCurrentScore( m_GameBoardManager.GetCurrentScore());
	}	
	//m_pStatusLayer->update(0);


	// ************** check obstacle
	UpdateObstacleListAfterMove();
}

void HelloWorld::PlayEarnScoreEffect(const int& iScore,  const Cell& cell, const float& fDelay)
{
	if (m_pTimeCountDownNode != NULL)
		m_pTimeCountDownNode->AddEnergy(iScore);

	Point position( m_fBoardLeftPosition + cell.m_iColumn * m_SymbolSize.width, m_fBoardBottomPosition + cell.m_iRow * m_SymbolSize.height);
	char sBuffer[12];
	sprintf(sBuffer, "%d", iScore);
	int iLength = strlen(sBuffer);
	char sSpriteName[16] = "ScoreNum_0.png";
	Sprite* pScoreNumberSprite;

	float fScale = 1.2f;
	if (iScore < 50)
		fScale = 1.1f;
	else if (iScore < 100)
		fScale = 1.3f;
	else if (iScore < 200)
		fScale = 1.5f;
	else
		fScale = 1.7f;

	for(int i=0; i < iLength; i++)
	{
		//sprintf( sSpriteName, "ScoreNum_%c.png", sBuffer[i]);
		sSpriteName[9] = sBuffer[i];
		pScoreNumberSprite = Sprite::createWithSpriteFrameName( sSpriteName);
		pScoreNumberSprite->setScale(fScale);
		pScoreNumberSprite->setPosition( Point(position.x - pScoreNumberSprite->getContentSize().width*fScale * ((iLength-1)/2.f-i), position.y));
		pScoreNumberSprite->setVisible(false);
		m_pBoardBatchNode->addChild(pScoreNumberSprite, 100);

		pScoreNumberSprite->runAction( 
			Sequence::create(
				DelayTime::create(fDelay),
				Show::create(),
				DelayTime::create(0.3f),
				FadeOut::create(0.4f),
				RemoveSelf::create(),
				NULL));
	}
}

// utility functions on boss sprite
void HelloWorld::AddNewLetterToBossSprite(const float& fDelayTime)
{
	unsigned char iLetter;
	if (m_GameBoardManager.GetGameWordManager()->GenerateLetterFromMainWord(iLetter))
	{
		Sprite* pLetterSprite = Sprite::createWithSpriteFrameName(
			m_pWordCollectBoardRenderNode->GetImageInGemFileFromLetter(iLetter).c_str());
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
			pSprite->setScale(2.f);//1.5f);
			pSprite->runAction( 
				Sequence::create( 
					DelayTime::create(fDelayTime),
					Spawn::createWithTwoActions( 
						EaseIn::create( FadeIn::create(fEffectTime), 2.f),
						EaseIn::create(ScaleTo::create(fEffectTime, 1.f, 1.f), 2.f)),
					NULL));			

			// add extra effect
			Point position = pSprite->getPosition();

			// active 1 sprite
			Sprite* pFlareSprite = Sprite::createWithSpriteFrameName("Flare.png");
			pFlareSprite->setPosition(position);	
			m_pBoardBatchNode->addChild(pFlareSprite, m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->getZOrder() + 10);	
			pFlareSprite->setVisible(false);// Opacity(0);
			pFlareSprite->setScale(0.2);
			pFlareSprite->setOpacity(0.4f*255);
			pFlareSprite->runAction(
				Sequence::create(
					DelayTime::create( fDelayTime + 0.05f),
					Show::create(),
					Spawn::create(
						Sequence::create(
							ScaleTo::create( 0.1f, 0.45f),
							ScaleTo::create( 0.15f, 1.f),
							ScaleTo::create( 0.15f, 0.3f),
							NULL),
						Sequence::create(
							FadeTo::create( 0.1, 255),
							DelayTime::create( 0.15f),
							FadeTo::create( 0.15f, 0.6f*255),							
							NULL),
						Sequence::create(
							RotateTo::create( 0.1f, 45.f),
							RotateTo::create( 0.15f, -112.5f),
							RotateTo::create( 0.15f, -180.f),
							NULL),
						NULL),
					RemoveSelf::create(),
					NULL));

				// active 2 sprite
				Sprite* pRingSprite = Sprite::createWithSpriteFrameName("CreateCombo_Ring.png");	
				pRingSprite->setPosition(position);
				m_pBoardBatchNode->addChild(pRingSprite, m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->getZOrder() + 10);	
				pRingSprite->setVisible(false); //setOpacity(0);
				pRingSprite->setScale(0.2f);
				pRingSprite->setRotation( 112.5f);
				pRingSprite->runAction(
					Sequence::create(
						DelayTime::create( fDelayTime + 0.05f),
						Show::create(), 
						ScaleTo::create( 0.25f, 0.83f),
						Spawn::create(							
							ScaleTo::create( 0.15f, 1.2f, 1.2f),
							FadeOut::create(0.15f),
							RotateTo::create(0.15f, 180.f),
							NULL),
						RemoveSelf::create(),
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

						// temporary hardcode effect of shield
						if (iObstacleTypeID == 0)
						{
							auto pEffectSprite = Sprite::createWithSpriteFrameName("B_Smoke_01.png");
							cocos2d::Animation* pAnim;

							if (obstacleData.m_bIsActive) 
							{
								// descrease level
								pAnim = AnimationCache::getInstance()->getAnimation("effectDecreaseShield");
							}
							else
							{
								// destroy
								pAnim = AnimationCache::getInstance()->getAnimation("effectDestroyShield");
							}

							Point pos = m_BoardObstaclesList[iBlockID][iObstacleTypeID]->getParent()->getPosition();
							pEffectSprite->setPosition(Point(pos.x, pos.y + 20.f) );
							pEffectSprite->setScale(1.15f);
							pEffectSprite->setVisible(false);
							m_pBoardBatchNode->addChild(pEffectSprite, 200);

							pEffectSprite->runAction( 
								Sequence::create( 
									DelayTime::create(obstacleData.m_fDirtyTime + 0.2f),
									Show::create(),
									Animate::create( pAnim),
									RemoveSelf::create(),
									NULL));
						}



						m_BoardObstaclesList[iBlockID][iObstacleTypeID]->runAction(
							Sequence::create(
								DelayTime::create( obstacleData.m_fDirtyTime + 0.2f),
								ScaleTo::create( 0.25f, 1.1f, 1.1f),
								RemoveSelf::create(),
								NULL));

						Object* childNode = NULL;
						CCARRAY_FOREACH( m_BoardObstaclesList[iBlockID][iObstacleTypeID]->getChildren(), childNode) 
							((Node*)childNode)->runAction(
								ScaleTo::create( 0.25f, 1.6f, 1.6f));

						//m_BoardObstaclesList[iBlockID][iObstacleTypeID]->removeFromParentAndCleanup(true);
						m_BoardObstaclesList[iBlockID][iObstacleTypeID] = NULL;

						AddObstacleToGem(m_BoardViewMatrix[iRow][iColumn].m_pSprite, iBlockID, iObstacleTypeID, true, obstacleData.m_fDirtyTime  + 0.2f);
					}											
					else
						AddObstacleToGem(m_BoardViewMatrix[iRow][iColumn].m_pSprite, iBlockID, iObstacleTypeID, false, obstacleData.m_fDirtyTime  + 0.2f);
					
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
			{
				// increase score			
				m_GameBoardManager.IncreaseScoreForLetterInMainWord();			

				//m_pWordCollectBoardRenderNode->UnlockLetter(iUnlockedLetterIndexOfMainWord);
				//m_pWordCollectBoardRenderNode->UnlockCharacter(fDelay, iUnlockedLetterIndexOfMainWord);

				if (!m_bIsEndGamePhase) //sometimes we end game early to test so we need check this
					m_pWordCollectBoardRenderNode->PlayUnlockLetterEffect( iUnlockedLetterIndexOfMainWord, fDelay, gemLetterData.m_iLetter,  //m_BoardViewMatrix[iRow][iColumn].m_iLetter, 
						ccp(m_fBoardLeftPosition + iColumn  * m_SymbolSize.width, 
							m_fBoardBottomPosition + iRow * m_SymbolSize.height));
			}

			

			if (bIsMainWordJustUnlocked)
				m_GameBoardManager.IncreaseScoreForUnlockMainWord();
			
			// play sound effect 
			SoundManager::PlaySoundEffect(_SET_DESTROY_LETTER_, fDelay);
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
	Sprite* pFlareSprite = Sprite::createWithSpriteFrameName("BasicFlare.png");
	pFlareSprite->setPosition(position);	
	m_pBoardBatchNode->addChild(pFlareSprite, m_BoardViewMatrix[iRow][iColumn].m_pSprite->getZOrder() + 10);	
	pFlareSprite->setVisible(false);// Opacity(0);
	pFlareSprite->setScale(0.8);
	pFlareSprite->setOpacity(0.4f*255);
	pFlareSprite->runAction(
		Sequence::create(
			DelayTime::create( fDelayTime),
			Show::create(), //FadeTo::create( 0.001f, 255),
			Spawn::createWithTwoActions(
				Sequence::create(
					ScaleTo::create( 0.033f, 1.2f),
					ScaleTo::create( 0.067f, 0.5f),
					ScaleTo::create( 0.033f, 0.4f),
					NULL),
				Sequence::create(
					FadeTo::create( 0.033f, 255),
					FadeTo::create( 0.067f, 0.6f*255),
					FadeOut::create(0.033),
					NULL)),			
			RemoveSelf::create(),
			NULL));


	// active 2 sprite
	Sprite* pRingSprite = Sprite::createWithSpriteFrameName("BasicRing.png");	
	pRingSprite->setPosition(position);
	m_pBoardBatchNode->addChild(pRingSprite,  m_BoardViewMatrix[iRow][iColumn].m_pSprite->getZOrder() + 10);	
	pRingSprite->setVisible(false); //setOpacity(0);
	pRingSprite->setScale(0.6f);
	pRingSprite->runAction(
		Sequence::create(
			DelayTime::create( fDelayTime +  0.033f),
			Show::create(), //FadeTo::create( 0.001f, 255),			
			ScaleTo::create( 0.067f, 0.71f, 0.71f),							
			Spawn::createWithTwoActions(
				ScaleTo::create( 0.1f, 0.9f, 0.9f),
				FadeOut::create(0.1)),
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

Sprite* HelloWorld::AddObstacleToGem(Sprite* pGemSprite, const int& iBlockID, const int& iObstacleTypeID, bool bPlayEffect, float fDelayTime)
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
			pMaskSprite->setVisible(false); //setOpacity(0);
			pMaskSprite->runAction(
				Sequence::create(
					DelayTime::create(fDelayTime),
					Show::create(), //FadeIn::create(0.01f),
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
						levelSpriteList[i]->setVisible(false); //setOpacity(0);
						levelSpriteList[i]->runAction(
							Sequence::create(
								DelayTime::create(fDelayTime),
								Show::create(), //FadeIn::create(0.01f),
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

Sprite* HelloWorld::AddLetterToGem(const Cell& cell, const int& iGemID, const unsigned char& iLetter, const int& iGemLetterBlockID, bool bIsMirror, bool bIsBonus, float fDelayTime, bool bPlayEffect)
{
	Sprite* pCharacterSprite;
	if (iGemLetterBlockID >=0)
	{
		if (!bIsBonus)
			pCharacterSprite = Sprite::createWithSpriteFrameName(
				m_pWordCollectBoardRenderNode->GetImageInGemFileFromLetter(iLetter).c_str());
		else
			pCharacterSprite = Sprite::createWithSpriteFrameName(
				m_pWordCollectBoardRenderNode->GetImageInFileFromBonusLetter(iLetter).c_str());
	}
	else
		pCharacterSprite = Sprite::createWithSpriteFrameName("Questionmask.png");
		
	Size parentSpriteSize = m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->getContentSize();
	//pCharacterSprite->setScale(0.85f);
	pCharacterSprite->setPosition(Point(parentSpriteSize.width/2.f, parentSpriteSize.height/2.f));  //( 42.f, 42.f));

	/*switch(iGemID)
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
	}*/

	if (!bIsMirror)
	{
		m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->addChild(pCharacterSprite);
		m_BoardViewMatrix[cell.m_iRow][cell.m_iColumn].m_iGemLetterBlockID = iGemLetterBlockID; //m_iLetter = iLetter;
	}
	else
		m_BoardViewMirrorMatrix[cell.m_iRow][cell.m_iColumn].m_pSprite->addChild(pCharacterSprite);

	if (bPlayEffect && !bIsMirror)
	{
		pCharacterSprite->setVisible(false);
		pCharacterSprite->setScale(2.f);
		pCharacterSprite->runAction(
			Sequence::create(
				DelayTime::create( fDelayTime + 0.33f),				
				Show::create(),
				ScaleTo::create( 0.12f, 1.f),
				ScaleTo::create( 0.064f, 1.1f),
				ScaleTo::create( 0.066f, 1.f),
				NULL));

		// extra effect
		Sprite* pFlareSprite = Sprite::createWithSpriteFrameName("LetterAppear_Flare.png");
		pFlareSprite->setVisible(false);
		pFlareSprite->setRotation(-55.f);
		pCharacterSprite->getParent()->addChild(pFlareSprite, 100);				
		pFlareSprite->setPosition(pCharacterSprite->getPosition());
		pFlareSprite->runAction(
			Sequence::create(
				DelayTime::create( fDelayTime + 0.033f),
				Show::create(),
				Spawn::create(
					Sequence::createWithTwoActions(
						ScaleTo::create( 0.133f, 1.93f),
						ScaleTo::create( 0.417f, 1.6f)),
					Sequence::createWithTwoActions(
						RotateTo::create( 0.133f, -29.f),
						RotateTo::create( 0.417f, 67)),
					Sequence::createWithTwoActions(
						DelayTime::create( 0.133f),
						FadeTo::create( 0.417f, 128)),
					NULL),
				RemoveSelf::create(),
				NULL));


		Sprite* pRingSprite = Sprite::createWithSpriteFrameName("LetterAppear_Ring.png");
		pRingSprite->setVisible(false);
		pRingSprite->setScale( 0.8f);		
		pCharacterSprite->getParent()->addChild(pRingSprite, 100);				
		pRingSprite->setPosition(pCharacterSprite->getPosition());
		pRingSprite->runAction(
			Sequence::create(
				DelayTime::create( fDelayTime + 0.033f),
				Show::create(),
				Spawn::createWithTwoActions(
					ScaleTo::create( 0.467f, 1.35f),
					FadeTo::create( 0.467f, 128)),
				DelayTime::create( 0.083f),
				RemoveSelf::create(),
				NULL));


		Sprite* pStarSprite = Sprite::createWithSpriteFrameName("LetterAppear_Star.png");
		pStarSprite->setVisible(false);
		pStarSprite->setScale( 0.6f);		
		pCharacterSprite->getParent()->addChild(pStarSprite, 100);			
		pStarSprite->setPosition(pCharacterSprite->getPosition());
		pStarSprite->runAction(
			Sequence::create(
				DelayTime::create( fDelayTime + 0.033f),
				Show::create(),
				Spawn::createWithTwoActions(
					ScaleTo::create( 0.467f, 1.3f),
					Sequence::createWithTwoActions(
						DelayTime::create( 0.35f),
						FadeOut::create( 0.117f))),
				RemoveSelf::create(),
				NULL));		
	}

	return pCharacterSprite;
}

void HelloWorld::ShowMainWordUnlockEffect()
{
	//m_pWordCollectBoardRenderNode->PlayCharacterAnim(3, true);

	
	m_pEndGameEffectLayer = LayerColor::create(ccc4( 0,0,0, 0)); //100));	
	this->getParent()->addChild(m_pEndGameEffectLayer, this->getZOrder());	


	const Word& mainWord = m_GameBoardManager.GetGameWordManager()->GetMainWord();

	/*
	SpriteBatchNode* pSpriteBatchNode = SpriteBatchNode::create("ResourceDemo.pvr.ccz");
	m_pEndGameEffectLayer->addChild(pSpriteBatchNode);

	// play unlock main word effect
	float fScaleRatio = 1.55f;
	float fDisplayEffectTime = 0.25f;
	float fDelayPerLetter = 0.06f;
	
	

	float fWordGraphicLength = 0;
	Sprite* letterSpriteList[_GDS_WORD_MAX_LENGTH_];

	for(int i=0; i< mainWord.m_iWordLength; i++)
	{
		letterSpriteList[i] = Sprite::createWithSpriteFrameName(  m_pWordCollectBoardRenderNode->GetImageInGemFileFromLetter(mainWord.m_sWord[i]).c_str());
		letterSpriteList[i]->setAnchorPoint(Point(0,0));
		letterSpriteList[i]->setScale(fScaleRatio);

		fWordGraphicLength += letterSpriteList[i]->getContentSize().width * fScaleRatio - 12.f;
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
				Spawn::createWithTwoActions(
					EaseOut::create( FadeTo::create(fDisplayEffectTime, 255) ,2.f) ,
					EaseOut::create( ScaleTo::create( fDisplayEffectTime, fScaleRatio, fScaleRatio), 2.f)),
				NULL));		

		fPositionXIncrement += letterSpriteList[i]->getContentSize().width * fScaleRatio - 12.f;
	}	*/

	float fSpellingTime = m_pWordCollectBoardRenderNode->PlaySpellingSound() + 0.9f;
	
	/*pSpriteBatchNode->runAction(
		Sequence::create(
			DelayTime::create( fSpellingTime), //mainWord.m_iWordLength * fDelayPerLetter + fDisplayEffectTime + 4.f),
			FadeOut::create(0.5f),
			RemoveSelf::create(),
			NULL));	*/
	
	float fEffectTime = m_pWordCollectBoardRenderNode->PlayUnlockWordEffect();
	//==> should delay show time of effect to match with spelling time!!!
	this->runAction(
		Sequence::create(
			DelayTime::create( MAX( fSpellingTime, fEffectTime)), //mainWord.m_iWordLength * fDelayPerLetter + fDisplayEffectTime + 4.5f),
			CallFunc::create( this,  callfunc_selector(HelloWorld::StartWinBonusPhase)),
			NULL));

	//Game Tracking Level
	std::string sCurrentChapterID = GameConfigManager::getInstance()->GetCurrentChapterID();
	int iCurrentLevel = GameConfigManager::getInstance()->GetCurrentLevelId();

	GameTracking::saveFileTrackingLevel(iCurrentLevel, m_GameBoardManager.GetCurrentMove(), mainWord.m_iWordLength, "Win");
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
	Sprite* pTextSprite = Sprite::createWithSpriteFrameName("Level_Completed.png");
	Size textSize = pTextSprite->getContentSize();
	Size winSize = Director::getInstance()->getWinSize();

	pTextSprite->setPosition(Point( -textSize.width/2.f, winSize.height/2.f + 100.f));
	m_pTextEffectBatchNode->addChild(pTextSprite);

	pTextSprite->runAction(
		Sequence::create(
			MoveTo::create( 0.4f, Point( winSize.width/2.f - 40.f, winSize.height/2.f  + 100.f)),
			DelayTime::create(1.2f),
			Spawn::createWithTwoActions(
				EaseBackOut::create(MoveTo::create( 0.5f, Point( winSize.width + textSize.width/2.f, winSize.height/2.f  + 100.f))),
				FadeOut::create( 0.9f)),
			RemoveSelf::create(),
			NULL));


	//! Load data synchorinize (should replace load async in the future
    /*ArmatureDataManager::getInstance()->addArmatureFileInfo("CCS_Animation/WordClear/WordClear.ExportJson");
	
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
			*/
	this->runAction(
		Sequence::create(
			DelayTime::create(2.1f),  //fAnimDuration),
			CallFunc::create( this,  callfunc_selector(HelloWorld::ShowWinGamePopup)),
			NULL));
			
	SoundManager::PlaySoundEffect(_SET_WIN_);
}

void HelloWorld::ShowLevelFailEffect()
{
	Sprite* pTextSprite = Sprite::createWithSpriteFrameName("Level_Fail.png");
	Size textSize = pTextSprite->getContentSize();
	Size winSize = Director::getInstance()->getWinSize();

	pTextSprite->setPosition(Point( -textSize.width/2.f, winSize.height/2.f + 100.f));
	m_pTextEffectBatchNode->addChild(pTextSprite);

	pTextSprite->runAction(
		Sequence::create(
			MoveTo::create( 0.4f, Point( winSize.width/2.f  - 40.f, winSize.height/2.f + 100.f)),
			DelayTime::create(1.2f),
			Spawn::createWithTwoActions(
				EaseBackOut::create(MoveTo::create( 0.5f, Point( winSize.width + textSize.width/2.f, winSize.height/2.f  + 100.f))),
				FadeOut::create( 1.f)),
			RemoveSelf::create(),
			NULL));

	this->runAction(
		Sequence::create(
			DelayTime::create(2.1f),  //fAnimDuration),
			CallFunc::create( this,  callfunc_selector(HelloWorld::ShowFailGamePopup)),
			NULL));
			
	SoundManager::PlaySoundEffect(_SET_FAIL_);
}

void HelloWorld::PlayCombo4Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime, float fEffectScale, bool bPlaySoundEffect)
{
	Point position(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height);

	auto pFlareSprite = Sprite::createWithSpriteFrameName("Combo4_Flare.png");	
	pFlareSprite->setVisible(false);
	pFlareSprite->setScale( 0.2f);
	pFlareSprite->setPosition(position);
	m_pComboEffectBatchNode->addChild(pFlareSprite);
	pFlareSprite->runAction(
		Sequence::create(
			DelayTime::create(fDelayTime),
			Show::create(),
			ScaleTo::create(0.133f, 1.5f * fEffectScale, 1.5f * fEffectScale),
			Spawn::createWithTwoActions(
				ScaleTo::create( 0.134f, 0.5f, 0.5f),
				Sequence::createWithTwoActions(
					DelayTime::create( 0.067f),
					FadeOut::create( 0.067f))
			),
			RemoveSelf::create(),
			NULL));

	if (pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_DESTROY_ROW_)
	{
		PlayCombo4HelperEffect(pComboEffect, fDelayTime, fDisplayTime, 0);
		PlayCombo4HelperEffect(pComboEffect, fDelayTime, fDisplayTime, 180.f);
	}
	else
	{
		PlayCombo4HelperEffect(pComboEffect, fDelayTime, fDisplayTime, 90.f);
		PlayCombo4HelperEffect(pComboEffect, fDelayTime, fDisplayTime, 270.f);
	}
	/*

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
				NULL));*/
				
	if (bPlaySoundEffect)
		SoundManager::PlaySoundEffect(_SET_ACTIVATE_COMBO_4_, fDelayTime);
}

void HelloWorld::PlayCombo4HelperEffect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime, float fRotation, float fEffectScale)
{
	Point position(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height);	

	auto pLight1Sprite = Sprite::createWithSpriteFrameName("Combo4_light1.png"); // "Com4_Ex_00002.png"); //
	pLight1Sprite->setRotation(fRotation);
	pLight1Sprite->setAnchorPoint( Point( 0, 0.5f));
	pLight1Sprite->setPosition( position);
	pLight1Sprite->setVisible(false);
	pLight1Sprite->setScaleX(0.001f);
	pLight1Sprite->setScaleY(1.75f * fEffectScale);
	m_pComboEffectBatchNode->addChild(pLight1Sprite);
	pLight1Sprite->runAction(		
		Sequence::create(
			DelayTime::create(fDelayTime + 0.033f),						
			Show::create(),
			Spawn::createWithTwoActions(
				//ScaleTo::create( 0.467f,  3.f, 1.75f),
				EaseOut::create( ScaleTo::create( 0.467f,  3.f, 1.75f * fEffectScale), 1.f),
				Sequence::createWithTwoActions(
					DelayTime::create( 0.4f),
					FadeTo::create( 0.067f, 0.5f))),
			RemoveSelf::create(),
			NULL));


	auto pLight2Sprite = Sprite::createWithSpriteFrameName("Combo4_light2.png"); 
	pLight2Sprite->setVisible(false);
	pLight2Sprite->setRotation(fRotation);
	pLight2Sprite->setAnchorPoint( Point( 0, 0.5f));
	pLight2Sprite->setPosition( position);
	pLight2Sprite->setScaleX(0);
	pLight2Sprite->setScaleY(1.75f * fEffectScale);
	m_pComboEffectBatchNode->addChild(pLight2Sprite);

	Point translate(120.f, 0);
	translate.rotateByAngle( Point(0,0), fRotation);
	pLight2Sprite->runAction(
		Sequence::create(
			DelayTime::create(fDelayTime + 0.066f),
			Show::create(),
			Spawn::create(
				ScaleTo::create( 0.201f, 1.f, 1.75f * fEffectScale),
				Sequence::createWithTwoActions(
					DelayTime::create( 0.067f),
					MoveBy::create( 0.4f,  translate)), //Point(120.f, 0))),
				Sequence::createWithTwoActions(
					DelayTime::create( 0.367f),
					FadeTo::create( 0.1f, 128)),
				NULL),
			RemoveSelf::create(),
			NULL));	
}

void HelloWorld::PlayComboEndGameBonusEffect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime)
{
	auto* pOriginalSprite = m_BoardViewMatrix[pComboEffect->m_ComboEffectDescription.m_Position.m_iRow][pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn].m_pSprite;
	if (pOriginalSprite)
	{	
		//BlendFunc blendFunc;// = new BlendFunc();
		//blendFunc.src = GL_ONE;
		//blendFunc.dst = GL_ONE;
		//m_pBonusTimeEffectBatchNode->setBlendFunc(blendFunc);


		Point position = pOriginalSprite->getPosition();
		float fTargetPosY = m_fBoardBottomPosition + (_BOARD_MAX_ROW_NUMBER_) * m_SymbolSize.height;

		auto* pNewRocketSprite = Sprite::createWithSpriteFrameName("Rocket.png"); 
		pNewRocketSprite->setPosition(position);
		m_pBonusTimeEffectBatchNode->addChild(pNewRocketSprite);

		pNewRocketSprite->runAction(
			Sequence::create(
				DelayTime::create(fDelayTime),
				MoveTo::create( fDisplayTime, Point( position.x, fTargetPosY)),
				RemoveSelf::create(),
				NULL));

		auto pFireAnim = AnimationCache::getInstance()->getAnimation("BonusTimeFire");
		auto pFireSprite = Sprite::createWithSpriteFrameName("LightFire/Light_01.png"); 
		pFireSprite->setAnchorPoint(Point( 1.f, 0.5f));
		pFireSprite->setRotation(270.f);
		pFireSprite->setPosition( Point( 0 +20.f, 5.f ));
		//pFireSprite->setVisible(false);
		pNewRocketSprite->addChild(pFireSprite);

		pFireSprite->runAction( RepeatForever::create(Animate::create( pFireAnim)));
		pFireSprite->setScale(0);
		pFireSprite->runAction(
			Sequence::create(
				DelayTime::create(fDelayTime),
				ScaleTo::create( 0.04f, 1.f, 1.f),
				ScaleTo::create( 0.26f, 1.5f, 1.5f),
				ScaleTo::create( 0.1f, 0, 0),
				NULL));



		auto pExplosionAnim = AnimationCache::getInstance()->getAnimation("BonusTimeExplosion");
		auto pExplosionSprite = Sprite::createWithSpriteFrameName( "Explosion/explosion_00000.png");
		pExplosionSprite->setPosition(Point( position.x, fTargetPosY));
		pExplosionSprite->setVisible(false);
		m_pBonusTimeEffectBatchNode->addChild(pExplosionSprite);

		pExplosionSprite->runAction(
			Sequence::create(
				DelayTime::create(fDelayTime + fDisplayTime),
				Show::create(),
				Animate::create( pExplosionAnim),				
				RemoveSelf::create(),
				NULL));		

		pOriginalSprite->removeFromParentAndCleanup(true);

	}
	// sound effect
	SoundManager::PlaySoundEffect(_SET_ACTIVATE_COMBO_4_, fDelayTime);
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

void HelloWorld::PlayCombo4_4Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime, float fEffectScale, bool bPlaySoundEffect)
{
	PlayActivateCommonDoubleAndTrippleComboEffect(pComboEffect, fDelayTime, _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_);

	PlayCombo4Effect( pComboEffect, fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_, fDisplayTime, fEffectScale, false);

	PlayCombo4HelperEffect(pComboEffect, fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_, fDisplayTime, 0, fEffectScale);
	PlayCombo4HelperEffect(pComboEffect, fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_, fDisplayTime, 180.f, fEffectScale);

	/*auto pComboEffectSprite1 = Sprite::createWithSpriteFrameName("SimpleEffect.png");
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
				NULL));*/	

	if (bPlaySoundEffect)
		SoundManager::PlaySoundEffect(_SET_ACTIVATE_DOUBLE_COMBO_, fDelayTime);
}

void HelloWorld::PlayCombo4_4_4Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime)
{
	PlayCombo4_4Effect( pComboEffect, fDelayTime, fDisplayTime, 1.3f, false);

	PlayCombo4HelperEffect(pComboEffect, fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_, fDisplayTime, 45.f, 1.3f);	
	PlayCombo4HelperEffect(pComboEffect, fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_, fDisplayTime, 135.f, 1.3f);	
	PlayCombo4HelperEffect(pComboEffect, fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_, fDisplayTime, 225.f, 1.3f);
	PlayCombo4HelperEffect(pComboEffect, fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_, fDisplayTime, 315.f, 1.3f);	

	SoundManager::PlaySoundEffect(_SET_ACTIVATE_TRIPLE_COMBO_, fDelayTime);
}

void HelloWorld::PlayActivateCommonDoubleAndTrippleComboEffect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime, bool bRemoveTriggerComboCell, bool bExtraEffectOnTriggerComboCellWhenDestroy)
{
	Cell activationCell = pComboEffect->m_ComboEffectDescription.m_Position;
	Point activationPosition(m_fBoardLeftPosition + activationCell.m_iColumn  * m_SymbolSize.width, m_fBoardBottomPosition + activationCell.m_iRow * m_SymbolSize.height);

	m_BoardViewMatrix[activationCell.m_iRow][activationCell.m_iColumn].m_pSprite->setZOrder(1000);		

	if (bRemoveTriggerComboCell)
	{
		if (! bExtraEffectOnTriggerComboCellWhenDestroy)
			m_BoardViewMatrix[activationCell.m_iRow][activationCell.m_iColumn].m_pSprite->runAction(
				Sequence::create(
						//ScaleTo::create( 0.1f, 1.1f, 1.1f),
						DelayTime::create(fDelayTime),
						EaseOut::create( ScaleTo::create( 0.33f, 2.1f, 2.1f), 4),
						ScaleTo::create( 0.13, 1.6f, 1.6f),
					
						//EaseBackOut::create( ScaleTo::create( 0.46f, 1.8f, 1.8f)),
						//FadeTo::create(0.2f, 50),
						DelayTime::create(0.1f),
						RemoveSelf::create(),
						NULL));
		else
			m_BoardViewMatrix[activationCell.m_iRow][activationCell.m_iColumn].m_pSprite->runAction(
				Sequence::create(
						//ScaleTo::create( 0.1f, 1.1f, 1.1f),
						DelayTime::create(fDelayTime),
						EaseOut::create( ScaleTo::create( 0.33f, 2.1f, 2.1f), 4),
						ScaleTo::create( 0.13, 1.6f, 1.6f),
						DelayTime::create(0.1f),
						FadeOut::create(0.15f),
						RemoveSelf::create(),
						NULL));	


		m_BoardViewMatrix[activationCell.m_iRow][activationCell.m_iColumn].m_pSprite = NULL;
		if (m_BoardViewMirrorMatrix[activationCell.m_iRow][activationCell.m_iColumn].m_pSprite)
		{
			m_BoardViewMirrorMatrix[activationCell.m_iRow][activationCell.m_iColumn].m_pSprite->runAction(
				Sequence::create(
					DelayTime::create(fDelayTime + fDisplayTime ),
					RemoveSelf::create( true),					
					NULL));

			m_BoardViewMirrorMatrix[activationCell.m_iRow][activationCell.m_iColumn].m_pSprite = NULL;
		}
	}
	else
	{
		m_BoardViewMatrix[activationCell.m_iRow][activationCell.m_iColumn].m_pSprite->runAction(
			Sequence::create(					
				DelayTime::create(fDelayTime),
				EaseOut::create( ScaleTo::create( 0.33f, 2.1f, 2.1f), 4),
				ScaleTo::create( 0.13, 1.6f, 1.6f),					
				DelayTime::create(0.1f),
				ScaleTo::create( 0.13, 1.f, 1.f),
				NULL));
	}



	Sprite* pSprite;
	Cell comboCell;
	for(int i=0; i< pComboEffect->m_ComboEffectDescription.m_Phase0CellCount; i++)
		if (pComboEffect->m_ComboEffectDescription.m_Phase0CellList[i].m_iDestroyPhaseIndex == 0)
		{
			comboCell = pComboEffect->m_ComboEffectDescription.m_Phase0CellList[i];
			pSprite = m_BoardViewMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_pSprite;
			if (pSprite != NULL)
			{
				pSprite->runAction(
					Sequence::create(
						DelayTime::create(fDelayTime),
					//ScaleTo::create( 0.1f, 1.3f, 1.3f),
						Spawn::create(
							MoveTo::create( 0.36f, activationPosition),
							ScaleTo::create( 0.36f, 1.5f, 1.5f),
							FadeOut::create(0.36f),
							NULL
							),											
						RemoveSelf::create(),
						NULL));
				m_BoardViewMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_pSprite = NULL;
				
				if (m_BoardViewMirrorMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_pSprite)
				{
					m_BoardViewMirrorMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_pSprite->runAction(
						Sequence::create(
							DelayTime::create(fDelayTime + fDisplayTime ),
							RemoveSelf::create( true),					
							NULL));

					m_BoardViewMirrorMatrix[comboCell.m_iRow][comboCell.m_iColumn].m_pSprite = NULL;
				}
			}
		}

}

void HelloWorld::PlayCombo4_5Effect(ComboEffectBundle* pComboEffect, float fDelayTime)
{	
	PlayCombo5Effect(pComboEffect, fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_, _TME_BASIC_COMBO_EXECUTE_TIME_, false);	
	PlayCombo4_4Effect(pComboEffect, fDelayTime, _TME_BASIC_COMBO_EXECUTE_TIME_);

	Cell backupPos = pComboEffect->m_ComboEffectDescription.m_Position;
	for(int i=0; i< pComboEffect->m_ComboEffectDescription.m_Phase0CellCount; i++)
		if (pComboEffect->m_ComboEffectDescription.m_Phase0CellList[i].m_iDestroyPhaseIndex >0)
		{
			pComboEffect->m_ComboEffectDescription.m_Position = pComboEffect->m_ComboEffectDescription.m_Phase0CellList[i];
			PlayCombo5Effect(pComboEffect, fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_ + _TME_COMBO_4_5_PHASE2_DELAY_TIME_ , _TME_BASIC_COMBO_EXECUTE_TIME_, false);
		}

	pComboEffect->m_ComboEffectDescription.m_Position = backupPos;
}

void HelloWorld::PlayCombo5_5_5Effect(ComboEffectBundle* pComboEffect, float fDelayTime)
{
	PlayCombo5Effect(pComboEffect, fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_, _TME_BASIC_COMBO_EXECUTE_TIME_, false);
	PlayCombo4_4_4Effect(pComboEffect, fDelayTime, _TME_BASIC_COMBO_EXECUTE_TIME_);

	Cell backupPos = pComboEffect->m_ComboEffectDescription.m_Position;
	for(int i=0; i< pComboEffect->m_ComboEffectDescription.m_Phase0CellCount; i++)
		if (pComboEffect->m_ComboEffectDescription.m_Phase0CellList[i].m_iDestroyPhaseIndex >0)
		{
			pComboEffect->m_ComboEffectDescription.m_Position = pComboEffect->m_ComboEffectDescription.m_Phase0CellList[i];
			PlayCombo5Effect(pComboEffect, fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_ + _TME_COMBO_4_5_PHASE2_DELAY_TIME_, _TME_BASIC_COMBO_EXECUTE_TIME_, false);
		}

	pComboEffect->m_ComboEffectDescription.m_Position = backupPos;
}

void HelloWorld::PlayCombo5Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime, bool bPlaySoundEffect)
{
	auto pComboEffectSprite = Sprite::createWithSpriteFrameName("Exp_00.png");
	auto pCombo4Anim = AnimationCache::getInstance()->getAnimation("effectCombo5New");

	pComboEffectSprite->setPosition( Point(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height));
	m_pComboEffectBatchNode->addChild(pComboEffectSprite);

	pComboEffectSprite->runAction( 
		Sequence::create( 
			DelayTime::create(fDelayTime),
			Animate::create( pCombo4Anim),
			RemoveSelf::create(),
			NULL));

	if (bPlaySoundEffect)
		SoundManager::PlaySoundEffect(_SET_ACTIVATE_COMBO_5_, fDelayTime);
}

void HelloWorld::PlayCombo5_5Effect(ComboEffectBundle* pComboEffect, float fDelayTime, float fDisplayTime)
{
	PlayActivateCommonDoubleAndTrippleComboEffect(pComboEffect, fDelayTime, _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_, pComboEffect->m_ComboEffectDescription.m_eComboEffectType == _CET_5_5_SECOND_EFFECT_);


	auto pComboEffectSprite = Sprite::createWithSpriteFrameName("Exp_00.png");
	pComboEffectSprite->setScale(1.6f);
	pComboEffectSprite->setOpacity(180);
	pComboEffectSprite->setColor(Color3B( 200, 100, 150));
	auto pCombo5Anim = AnimationCache::getInstance()->getAnimation("effectCombo5New");

	pComboEffectSprite->setPosition( Point(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height));
	m_pComboEffectBatchNode->addChild(pComboEffectSprite);

	pComboEffectSprite->runAction( 
		Sequence::create( 
			DelayTime::create(fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_),
			Animate::create( pCombo5Anim),
			RemoveSelf::create(),
			NULL));	

	// test extra layer
	auto pComboEffectSprite2 = Sprite::createWithSpriteFrameName("Exp_00.png");
	pComboEffectSprite2->setOpacity(180);	
	pComboEffectSprite2->setScaleX(1.6f);
	pComboEffectSprite2->setScaleY(-1.6f);
	pComboEffectSprite2->setColor(Color3B( 100, 255, 200));
	pComboEffectSprite2->setPosition( pComboEffectSprite->getPosition());
	m_pComboEffectBatchNode->addChild(pComboEffectSprite2);

	pComboEffectSprite2->runAction( 
		Sequence::create( 
			DelayTime::create(fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_),
			Animate::create( pCombo5Anim),
			RemoveSelf::create(),
			NULL));	

	// play sound
	SoundManager::PlaySoundEffect(_SET_ACTIVATE_DOUBLE_COMBO_, fDelayTime);
	/*
	Sprite* pComboEffectSprite;	

	pComboEffectSprite = Sprite::createWithSpriteFrameName("Explosion2.png");		
	pComboEffectSprite->setScale(2.17f);	

	pComboEffectSprite->setPosition( Point(m_fBoardLeftPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iColumn  * m_SymbolSize.width, 
			m_fBoardBottomPosition + pComboEffect->m_ComboEffectDescription.m_Position.m_iRow * m_SymbolSize.height));
	
	m_pBoardBatchNode->addChild(pComboEffectSprite, 200);

	pComboEffectSprite->setOpacity(0);
	pComboEffectSprite->runAction(Sequence::create( 
			DelayTime::create(fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_),
			CallFuncN::create( this, callfuncN_selector( HelloWorld::ActivateImageEffect)),
			EaseOut::create( FadeOut::create( fDisplayTime), 2.f),				
			RemoveSelf::create( true),				
			NULL));		*/
}
	
void HelloWorld::PlayCombo6Effect(ComboEffectBundle* pComboEffectBundle, float fDelayTime)
{		
	auto pCombo5AnimFlare = AnimationCache::getInstance()->getAnimation("effectCombo6_Flare");
	auto pCombo5AnimBolt = AnimationCache::getInstance()->getAnimation("effectCombo6_Light");		
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
		auto pComboEffectSprite = Sprite::createWithSpriteFrameName("Combo6_000.png");
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

				auto pComboEffectSprite = Sprite::createWithSpriteFrameName("Combo6_000.png");
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
	SoundManager::PlaySoundEffect(_SET_ACTIVATE_COMBO_6_, fDelayTime);
}

void HelloWorld::PlayCombo6_6Effect(ComboEffectBundle* pComboEffectBundle, float fDelayTime)
{
	if (pComboEffectBundle->m_ComboEffectDescription.m_eComboEffectType == _CET_6_6_EFFECT_)
		PlayActivateCommonDoubleAndTrippleComboEffect(pComboEffectBundle, fDelayTime, _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_);
	else
		PlayActivateCommonDoubleAndTrippleComboEffect(pComboEffectBundle, fDelayTime, _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_, pComboEffectBundle->m_ComboEffectDescription.m_eComboEffectType == _CET_6_6_6_SECOND_EFFECT_);

	auto pCombo5AnimFlare = AnimationCache::getInstance()->getAnimation("effectCombo6_Flare");
	auto pCombo5AnimBolt = AnimationCache::getInstance()->getAnimation("effectCombo6_Light");		
	Cell comboCell = pComboEffectBundle->m_ComboEffectDescription.m_Position;

	auto pTriggerSprite = Sprite::createWithSpriteFrameName("Flare_00000_1.png");
	pTriggerSprite->setPosition(Point(m_fBoardLeftPosition + comboCell.m_iColumn  * m_SymbolSize.width, 
				m_fBoardBottomPosition + comboCell.m_iRow * m_SymbolSize.height));
	m_pComboEffectBatchNode->addChild(pTriggerSprite);
	pTriggerSprite->runAction( 
		Sequence::create(
			DelayTime::create(fDelayTime + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_),
			Animate::create( pCombo5AnimFlare),
			RemoveSelf::create(),
			NULL));	
	
	for(auto destroyedCell : pComboEffectBundle->m_DestroyedCells)
	{
		auto pComboEffectSprite = Sprite::createWithSpriteFrameName("Combo6_000.png");
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
				DelayTime::create(fDelayTime + 0.05f + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_),
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
				DelayTime::create(fDelayTime + 0.42f + _TME_DOUBLE_TRIPLE_COMMONE_ACTIVATE_DELAY_TIME_),
				Animate::create( pCombo5AnimFlare),
				RemoveSelf::create(),
				NULL ));		
	} 

	SoundManager::PlaySoundEffect(_SET_ACTIVATE_DOUBLE_COMBO_, fDelayTime);
}

// unlock letter flow
/*void HelloWorld::PlayUnlockLettersOfMainWordAnimation(const float& fDelayTime)
{
	float fDisplayTime = m_pWordCollectBoardRenderNode->PlayUnlockLettersAnimation(fDelayTime);
	if (fDisplayTime > 0)
	{
		//m_pWordCollectBoardRenderNode->PlayCharacterAnim(5, false);

		this->runAction(
			Sequence::create(
			DelayTime::create( fDisplayTime),
				CallFunc::create( this,  callfunc_selector(HelloWorld::CheckEndGameAtEndChain)),
				NULL));
	}
	else
	{
		//PlayUnlockLettersOfBonusWordsAnimation();
		EndUnlockLetterAnimation();
	}
}*/

void HelloWorld::CheckEndGameAtEndChain()
{
	//m_bIsEffectPlaying = false;

	if (m_bIsEndGamePhase)
	{
		if (m_eGameModeType == _GMT_NORMAL_)		//ShowWinGamePopup();
			ShowLevelCompleteEffect();
	}
	else
	{
		//check end game
		if (m_GameBoardManager.GetGameWordManager()->IsMainWordUnlocked()) // complete objective ==> win		
		//if (true)
		//if (true)
		//if (false)		
		{

			if (m_eGameModeType == _GMT_NORMAL_)
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

					SoundManager::PlayBackgroundMusic(SoundManager::kEndGameBonus);

					m_bIsEffectPlaying = true;//stop all interaction on board from now
					m_bIsEndGamePhase = true; 

					float fExtraDelay = 0.1f;
					if (m_GameBoardManager.GetPhaseMoveOfComboChain() == 2)
						fExtraDelay += 1.f;

					m_pTempSpriteForAction->runAction( 					
						Sequence::create(
							DelayTime::create(fExtraDelay),
							CCCallFunc::create( this, callfunc_selector( HelloWorld::ShowMainWordUnlockEffect)),
							NULL));			

					//ShowMainWordUnlockEffect();
					//ExecuteBonusWinGameEffect();

					UserTable::getInstance()->updateLife(0);
				}
			}
			else //time mode
			{
				/*if (m_pTimeCountDownNode != NULL)
				{
					//already out of time before!!!!
					if (m_pTimeCountDownNode->IsEnergyEmpty())
						return;

					OnTimeMode_StageComplete();	
				}*/
				OnTimeMode_StageComplete();
			}
		}
		else //word is not unlocked yet
		{
			if (m_eGameModeType == _GMT_NORMAL_)
			{
				if (m_GameBoardManager.GetCurrentMove() == 0) // out of move ==> lose
				{
					m_bIsEndGamePhase = true; 
					ShowLevelFailEffect();			
				}
			}
			else //time mode
			{
				if (m_pTimeCountDownNode != NULL)
				{
					//already out of time before!!!!
					if (!m_pTimeCountDownNode->IsEnergyEmpty())
						return;

					OnTimeMode_OutOfTime();				
				}
			}
		}
	}
}

void HelloWorld::OnTimeMode_OutOfTime()
{
	if (m_bIsEndGamePhase)
		return;

	m_bIsEndGamePhase = true;
	//m_bIsEndGamePhase = true;
	
	float fFailCollectWordEffectTime = m_pWordCollectBoardRenderNode->PlayTimeMode_UnlockWordFailEffect();

	/*auto label = LabelTTF::create("OUT OF TIME!", "fonts/UTM Cookies.ttf", 70);		
	label->setColor(Color3B( 200, 30, 30));
	label->setPosition( Point( Director::getInstance()->getWinSize().width /2.f, 470));
	label->disableStroke();
	this->addChild(label, 2000);

	label->runAction( 
		Sequence::createWithTwoActions(
			DelayTime::create(3.f),
			Hide::create()));*/
	auto pTextSprite = Sprite::createWithSpriteFrameName("Time_Out.png");
	m_pTextEffectBatchNode->addChild(pTextSprite);
	pTextSprite->setPosition( Point( Director::getInstance()->getWinSize().width /2.f, 470));	
	pTextSprite->setVisible(false);
	pTextSprite->runAction(
		Sequence::create(
			DelayTime::create( fFailCollectWordEffectTime),
			Show::create(),
			DelayTime::create( 3.f),
			Hide::create(),				
			NULL));


	//label->setScale( 0.7f);	

	this->runAction( CCSequence::create(
		DelayTime::create(fFailCollectWordEffectTime + 3.f),
		CallFunc::create( this, callfunc_selector( HelloWorld::ShowTimeModeResultPopup)),
		NULL));
}

//#include "MainMenuScene.h"
#include "EndGameCustomModeNode.h"
#include "APIService\SyncDataGame.h"

void HelloWorld::ShowTimeModeResultPopup()
{	
	auto timeModeConfig = (TimeModeLevelConfig*)m_GameBoardManager.GetLevelConfig();
	//MainMenuScene* pMainMenu = MainMenuScene::create();
	//Director::getInstance()->replaceScene(pMainMenu);

	auto pEndGameNode = EndGameCustomModeNode::createLayout( m_iCurrentTimeModeStage, MIN( m_iCurrentTimeModeStage, timeModeConfig->m_WordIndexList.size()) , 
					GameWordManager::getInstance()->GetTotalPlayTimeOfTimeModeSession());	
	pEndGameNode->addYellowStar( m_GameBoardManager.GetEarnedStartsOfTimeMode(m_iCurrentTimeModeStage-1));
	m_pHUDLayer->addChild(pEndGameNode, 1000);

	// push result of custom mode to server
	SyncDataGame::getInstance()->pushDataCustomGameMode(timeModeConfig->m_sCustomPackageID);
}

void HelloWorld::OnTimeMode_StageComplete()
{
	m_bIsEndGamePhase = true;
	//m_bIsEndGamePhase = true;

	m_pTimeCountDownNode->StopUpdate();
	

	// show complete mainword effect
	float fEffectTime = m_pWordCollectBoardRenderNode->PlayUnlockWordEffect();

	//float fSpellingTime = 
	m_pWordCollectBoardRenderNode->PlaySpellingSound() + 0.9f;

	//float fDelayTime = MAX( fSpellingTime, fEffectTime);

	/*auto label = LabelTTF::create("STAGE COMPLETE!", "fonts/UTM Cookies.ttf", 70);		
	label->setColor(Color3B( 100, 100, 100));
	label->setPosition( Point( Director::getInstance()->getWinSize().width /2.f, 470));
	label->disableStroke();		
	label->setVisible(false);
	this->addChild(label, 2000);
	
	label->runAction(
		Sequence::create(
			DelayTime::create( fEffectTime),
			Show::create(),				
			NULL));*/
	auto pTextSprite = Sprite::createWithSpriteFrameName("Stage_Complete.png");
	m_pTextEffectBatchNode->addChild(pTextSprite);
	pTextSprite->setPosition( Point( Director::getInstance()->getWinSize().width /2.f, 470));
	pTextSprite->setVisible(false);
	pTextSprite->runAction(
		Sequence::create(
			DelayTime::create( fEffectTime),
			Show::create(),				
			NULL));


	this->runAction( CCSequence::create(
		DelayTime::create(3.f + fEffectTime),
		CallFunc::create( this, callfunc_selector( HelloWorld::TimeMode_StartNextStage)),
		NULL));	
}

#include "Database\CSWordTable.h"
#include "Database\CSPackageTable.h"

void HelloWorld::TimeMode_StartNextStage()
{
	auto timeModeConfig = (TimeModeLevelConfig*)m_GameBoardManager.GetLevelConfig();

	// increase collected count of main word in word list
	int iMainWordIndex = m_GameBoardManager.GetGameWordManager()->GetLoadedIndexOfMainWord();	
	int iCollectedCount;
	for(int i=0; i< timeModeConfig->m_WordIndexList.size(); i++)
	{
		if (timeModeConfig->m_WordIndexList[i] == iMainWordIndex)
		{
			timeModeConfig->m_WordCollectedCountList[i]++;
			iCollectedCount = timeModeConfig->m_WordCollectedCountList[i]++;
			break;
		}
	}

	// update db to increase collect count of main word
	CSPackageInfo packageInfo;
	packageInfo.sPackageId = timeModeConfig->m_sCustomPackageID;
	packageInfo.sPackageName = "";
	CSPackageTable::getInstance()->updateCSPackage(packageInfo);

	CSWordInfo customWordDB;
	customWordDB.sCSWordId = GameWordManager::getInstance()->GetMainWord().m_sWordID;
	customWordDB.sPackageId = timeModeConfig->m_sCustomPackageID;
	customWordDB.iCollectedCount = iCollectedCount;
	CSWordTable::getInstance()->updateCSWord(customWordDB);
	
	// update tracking info
	m_GameBoardManager.GetGameWordManager()->UpdateTimeModeTracking( iMainWordIndex);

	// start new stage
	GameWordManager::getInstance()->GenerateWordForNewLevelOfTimeMode(timeModeConfig);

	int iCombo4Count, iCombo5Count, iCombo6Count;
	m_GameBoardManager.CountComboCellsOnBoard( iCombo4Count, iCombo5Count, iCombo6Count);
	auto newScene = HelloWorld::createScene( m_eGameModeType, m_iCurrentTimeModeStage+1, iCombo4Count, iCombo5Count, iCombo6Count);
	Director::getInstance()->replaceScene(newScene);
}