#include "FlashCardCollection.h"
#include "FlashCardScene.h"
#include "FooterNode.h"
#include "GameConfigManager.h"
#include "HeaderNode.h"
#include "ClipMaskNode.h"
#include "Database\WordTable.h"
#include "FunctionCommon.h"
#include "MiniGameScene.h"
#include "ReviseGameScene.h"
#include "PopupConfirmNode.h"

USING_NS_CC;

bool FlashCardCollectionScene::init()
{
	if(CCScene::init())
	{
		this->_layer = FlashCardCollectionLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}
	else
	{
		return false;
	}
}

FlashCardCollectionScene::~FlashCardCollectionScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}

FlashCardCollectionLayer::~FlashCardCollectionLayer()
{
	
}

bool FlashCardCollectionLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	Sprite* pBackground = Sprite::create("FlashCard/background.png");
	pBackground->setPosition(Point(320.0f, 480.0f));
	this->addChild(pBackground);

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(pButtonManagerNode);

	Sprite* pIconFlashCard = Sprite::create("FlashCard/flashcard_icon_btn.png");
	pIconFlashCard->setPosition(Point(-140.0f, 45.0f));

	Sprite* pButtonMiniGameSprite = Sprite::create("FlashCard/minigame-countdown.png");
	pButtonMiniGameSprite->addChild(pIconFlashCard);
	pButtonMiniGameSprite->setPosition(Point(154.0f, 0.0f));

	ButtonNode* pButtonMiniGame = ButtonNode::createButtonSprite(pButtonMiniGameSprite, CC_CALLBACK_1(FlashCardCollectionLayer::clickPlayMiniGame, this));
	pButtonMiniGame->setPosition(Point(320.0f, 810.0f));
	pButtonManagerNode->addButtonNode(pButtonMiniGame);


	m_iWordNew = WordTable::getInstance()->getNumberWordNew();
	m_iWordPlayMiniGame = WordTable::getInstance()->getNumberWordPlayMiniGame(getTimeLocalCurrent());

	m_pLabelMiniGame = LabelTTF::create("MINI GAME", "Arial", 30);
	m_pLabelMiniGame->setColor(ccc3(255.0f, 255.0f, 255.0f));
	m_pLabelMiniGame->setPosition(Point(148.0f, 47.0f));
	pButtonMiniGameSprite->addChild(m_pLabelMiniGame);

	m_iTimeCountDown = 0;
	if (m_iWordPlayMiniGame < 1 && m_iWordNew > 0)
	{
		unsigned long uMinTime = WordTable::getInstance()->getMinTimeNextPlayMiniGame();
		m_iTimeCountDown = uMinTime + _SECONDS_NEXT_PLAY_MINI_GAME_ - getTimeLocalCurrent();
		if (m_iTimeCountDown > 0)
		{
			m_pLabelMiniGame->setString(formatHMSToDisplay(m_iTimeCountDown).getCString());
			this->scheduleUpdate();
		}
		
	}

	if (m_iWordNew > 0)
	{
		Sprite* pIconNew = Sprite::create("FlashCard/noitify_msg.png");
		pIconNew->setPosition(Point(240.0f, 85.0f));
		pButtonMiniGameSprite->addChild(pIconNew);

		char sNumberNew[10];
		sprintf(sNumberNew, "%d", m_iWordNew);
		LabelTTF* pLabelNumber = LabelTTF::create(sNumberNew, "Arial", 25);
		pLabelNumber->setColor(ccc3(255.0f, 255.0f, 255.0f));
		pLabelNumber->setPosition(Point(22.0f, 20.0f));
		pIconNew->addChild(pLabelNumber);
	}

	Sprite* pBackgroundFlashcard = Sprite::create("FlashCard/panel-level_popup.png");
	pBackgroundFlashcard->setPosition(Point(320.0f, 415.0f));
	this->addChild(pBackgroundFlashcard);

	LabelBMFont *pLabelTitle = LabelBMFont::create("Flashcard Collection", "fonts/font-bechic.fnt");
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelTitle->setPosition(Point(320.0f, 673.0f));
	this->addChild(pLabelTitle);
	
	m_pFooterNode = FooterNode::create();
	m_pFooterNode->changeStatusButtonFlashcard(StatusButtonFlashcard::eNoClick);
	m_pFooterNode->removeBackground();
	this->addChild(m_pFooterNode);

	HeaderNode* pHeaderNode = HeaderNode::create();
	this->addChild(pHeaderNode);

	m_pSlideShow = ButtonManagerNode::create();
	m_pSlideShow->AllowSwipingBackground(true);
	m_pSlideShow->setPosition(Point(245.0f, 300.0f));

	ClipMaskNode* pClipMaskNode = ClipMaskNode::create();
	pClipMaskNode->setPosition(Point(75.0f, 155.0f));
	pClipMaskNode->setContentSize(Size(560.0f, 460.0f));
	pClipMaskNode->addChild(m_pSlideShow);
	pBackgroundFlashcard->addChild(pClipMaskNode);

	ChapterTable::getInstance()->refreshChapters();
	m_chapters = ChapterTable::getInstance()->getChaptersInfo();

	WordlMapConfig worlMapConfig = GameConfigManager::getInstance()->GetWordlMapConfig();
	
	int iCountIndex=0;
	for (int iIndex=0; iIndex<m_chapters.size(); iIndex++)
	{
		ChapterInfo chapterInfo = m_chapters[iIndex];
		if(chapterInfo.bIsUnlock)
		{
			int iTotalFlashcardUnlock = chapterInfo.iTotalFlashCardUnlock;
			if (iTotalFlashcardUnlock > 0)
			{
				Sprite* pBackgroundItem = Sprite::create("FlashCard/panel_flashcard_chapter.png");
				pBackgroundItem->setPosition(Point(0.0f, -iIndex*190));
				m_pSlideShow->addChild(pBackgroundItem);

				int iIndexChapter = worlMapConfig.m_WorlMapChapterConfigMap[chapterInfo.sChapterId];

				std::string sPath = worlMapConfig.m_WorlMapChapterConfigs[iIndexChapter].m_sPathData;
				sPath.append("/Flash-Card-icon.png");
				Sprite* pIconChapter = Sprite::create(sPath.c_str());

				ButtonNode* pButtonItem = ButtonNode::createButtonSprite(pIconChapter, CC_CALLBACK_1(FlashCardCollectionLayer::clickOpenFlashCard, this));
				pButtonItem->setTag(iIndex);
				pButtonItem->setPosition(Point(-140.0f, -15 -iIndex*190));
				m_pSlideShow->addButtonNode(pButtonItem);

				LabelBMFont* pLabelChapterName = LabelBMFont::create(worlMapConfig.m_WorlMapChapterConfigs[iIndexChapter].m_sChapterName.c_str(), "fonts/font_small_alert.fnt");
				pLabelChapterName->setPosition(Point(90.0f, 18 -iIndex*190));
				m_pSlideShow->addChild(pLabelChapterName);

				char sTotalFlashCard[10];
				sprintf(sTotalFlashCard, "(%d/%d)", iTotalFlashcardUnlock, chapterInfo.iTotalFlashCard);
				LabelTTF* pLabelTotalFlashCard = LabelTTF::create(sTotalFlashCard, "Arial", 25);
				pLabelTotalFlashCard->setColor(ccc3(0.0f, 0.0f, 0.0f));
				pLabelTotalFlashCard->setPosition(Point(90.0f, -18 -iIndex*190));
				m_pSlideShow->addChild(pLabelTotalFlashCard);

				if (iTotalFlashcardUnlock >= 10)
				{
					Sprite* pButtonPlayReviseGameImage = Sprite::create("AdvanceMode/btn-play-advance-mode.png");
					ButtonNode* pButtonPlayReviseGame = ButtonNode::createButtonSprite(pButtonPlayReviseGameImage, CC_CALLBACK_1(FlashCardCollectionLayer::clickPlayReviseGame, this));
					pButtonPlayReviseGame->setTag(iIndex);
					pButtonPlayReviseGame->setPosition(Point(235.0f, -iIndex*190));
					m_pSlideShow->addButtonNode(pButtonPlayReviseGame);
				}

				iCountIndex++;
				m_maxHeight = iCountIndex*190;
			}
		}
	}

	Breadcrumb::getInstance()->addSceneMode(SceneMode::kFlashCardCollection);

	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);

	m_pScrollManager = new ScrollManager();

	return true;
}

void FlashCardCollectionLayer::update(float dt)
{
	m_iTimeCountDown -= dt;
	if (m_iTimeCountDown < 0)
	{
		m_iTimeCountDown = 0;
		this->unscheduleUpdate();
		m_pLabelMiniGame->setString("MINI GAME");
	}
	else
	{
		m_pLabelMiniGame->setString(formatHMSToDisplay((int)m_iTimeCountDown).getCString());
	}
	
}

void FlashCardCollectionLayer::clickPlayMiniGame(Object* sender)
{
	if (m_iWordNew < 1)
	{
		char sTitle[50];
		sprintf(sTitle, "Total flash cards: %d", WordTable::getInstance()->getTotalWordCollected());
		PopupConfirmNode* pPopup = PopupConfirmNode::createLayout("YOU HAVE COLLECTED ALL FLASH CARDS", sTitle, PopupConfirmActionType::eNone, PopupConfirmType::eOK);
		this->addChild(pPopup);
	}
	else
	{
		if (m_iWordPlayMiniGame < 1)
		{
			char sTitle[50];
			sprintf(sTitle, "YOU HAVE %d NEW FLASH CARDS", m_iWordNew);
			PopupConfirmNode* pPopup = PopupConfirmNode::createLayout(sTitle, "Please come back in", PopupConfirmActionType::eNone, PopupConfirmType::eOK);
			pPopup->addClockMiniGame(m_iTimeCountDown);
			this->addChild(pPopup);
		}
		else
		{
			MiniGameScene* pMiniGame = MiniGameScene::create();
			Director::getInstance()->replaceScene(pMiniGame);
		}
	}
}

bool FlashCardCollectionLayer::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	if(m_pFooterNode->getSettingNode() != NULL && m_pFooterNode->getSettingNode()->getShowSetting() || m_maxHeight < 460)
	{
		return false;
	}

	Point touchPosition = pTouch->getLocation();
	m_fBeginY = touchPosition.y;

	DataTouch dataTouch;
	dataTouch.point = touchPosition;
	dataTouch.lTime = 0;
	dataTouch.fDeltaTime = 0;
	m_pScrollManager->addDataToQueue(dataTouch);

	return true;
}

void FlashCardCollectionLayer::onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	Point touchPosition = pTouch->getLocation();
	m_fYMoved = touchPosition.y - m_fBeginY;

	if (m_fYMoved + m_pSlideShow->getPosition().y <= 300.0f) {
		m_fYMoved = 300.0f - m_pSlideShow->getPosition().y;
	}
	else if(m_fYMoved + m_pSlideShow->getPosition().y >= m_maxHeight + 300 - 460)
	{
		m_fYMoved = m_maxHeight + 300 - 460 - m_pSlideShow->getPosition().y;
	}

	Point point = m_pSlideShow->getPosition();
	m_pSlideShow->setPositionY(point.y + m_fYMoved);
	m_fBeginY = touchPosition.y;

	DataTouch dataTouch;
	dataTouch.point = touchPosition;
	dataTouch.lTime = 0;
	dataTouch.fDeltaTime = 0;
	m_pScrollManager->addDataToQueue(dataTouch);
}

void FlashCardCollectionLayer::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	DataTouch dataTouch = m_pScrollManager->getDistanceScrollY();
	float distanceY = dataTouch.point.y;
	float deltaTime = dataTouch.fDeltaTime;
	
	if(distanceY!=0 && deltaTime!=0)
	{
		float fTime = 0.2f;
		distanceY = distanceY * fTime / deltaTime / 10; 

		
		if (distanceY + m_pSlideShow->getPosition().y <= 300.0f) {
			distanceY = 300.0f - m_pSlideShow->getPosition().y;
		}
		else if(distanceY + m_pSlideShow->getPosition().y >= m_maxHeight + 300 - 460)
		{
			distanceY = m_maxHeight + 300 - 460 - m_pSlideShow->getPosition().y;
		}

		auto actionMove = MoveBy::create(fTime, Point(0.0f, distanceY));
		auto actionEaseOut = EaseOut::create(actionMove, 2.5f);
		m_pSlideShow->stopAllActions();
		m_pSlideShow->runAction(Sequence::create(actionEaseOut, NULL));
	}
}

void FlashCardCollectionLayer::clickOpenFlashCard(Object* sender)
{
	ButtonNode* pSprite = (ButtonNode*)sender;
	int iChapter = pSprite->getTag();

	if (iChapter < m_chapters.size())
	{
		ChapterInfo chapterInfo = m_chapters[iChapter];
		if (chapterInfo.bIsUnlock)
		{
			FlashCardScene* pFlashCard = FlashCardScene::createScene(chapterInfo);
			Director::getInstance()->replaceScene(pFlashCard);
		}
	}
}

void FlashCardCollectionLayer::clickPlayReviseGame(Object* sender)
{
	ButtonNode* pSprite = (ButtonNode*)sender;
	int iChapter = pSprite->getTag();

	if (iChapter < m_chapters.size())
	{
		ChapterInfo chapterInfo = m_chapters[iChapter];
		if (chapterInfo.bIsUnlock)
		{
			ReviseGameScene* pFlashCard = ReviseGameScene::createScene(chapterInfo);
			Director::getInstance()->replaceScene(pFlashCard);
		}
	}
}