#include "ReviseGameScene.h"
#include "FooterNode.h"
#include "SettingMenuNode.h"
#include "FunctionCommon.h"
#include "GameWordManager.h"
#include "Database\UserTable.h"
#include "Database\ChapterTable.h"

USING_NS_CC;


ReviseGameScene* ReviseGameScene::createScene(const ChapterInfo& chapterInfo)
{
	ReviseGameScene* pReviseGameScene = new ReviseGameScene();	  
	pReviseGameScene->m_chapterInfo = chapterInfo;
	if(pReviseGameScene->init())
	{
		pReviseGameScene->autorelease();
		return pReviseGameScene;
	}

	CC_SAFE_DELETE(pReviseGameScene);
	return NULL;
}

bool ReviseGameScene::init()
{
	if(CCScene::init())
	{
		this->_layer = ReviseGameLayer::createLayout(m_chapterInfo);
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}
	else
	{
		return false;
	}
}

ReviseGameScene::~ReviseGameScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}

ReviseGameLayer::~ReviseGameLayer()
{
	
}

ReviseGameLayer* ReviseGameLayer::createLayout(const ChapterInfo& chapterInfo)
{
	ReviseGameLayer* pReviseGameLayer = new ReviseGameLayer();	  
	pReviseGameLayer->m_chapterInfo = chapterInfo;
	if(pReviseGameLayer->init())
	{
		pReviseGameLayer->autorelease();
		return pReviseGameLayer;
	}

	CC_SAFE_DELETE(pReviseGameLayer);
	return NULL;
}

bool ReviseGameLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	Sprite* pBackground = Sprite::create("FlashCard/background.png");
	pBackground->setPosition(Point(320.0f, 480.0f));
	this->addChild(pBackground);

	m_WordsCollected = WordTable::getInstance()->getWordInfoCollectedForChapter(m_chapterInfo.sChapterId);
	m_ChapterWords = WordTable::getInstance()->getAllWordsForChapter(m_chapterInfo.sChapterId);
	
	FooterNode* pFooterNode = FooterNode::create();
	pFooterNode->changeStatusButtonFlashcard(StatusButtonFlashcard::eNoClick);
	pFooterNode->removeBackground();
	pFooterNode->removeButtonFlashcard();
	this->addChild(pFooterNode);

	m_pHeaderNode = HeaderNode::create();
	this->addChild(m_pHeaderNode);

	m_iIndexWordCollected = 0;
	Breadcrumb::getInstance()->addSceneMode(SceneMode::kReviseGame);

	m_iStage = 0;

	m_pLabelClock = LabelTTF::create("5", "Arial", 120);
	m_pLabelClock->setColor(ccc3(0, 0, 0));
	m_pLabelClock->setPosition(Point(330.0f, 650.0f));
	this->addChild(m_pLabelClock, 1000);

	m_bIsEndGame = false;
	m_iSeconds = 5;
	this->createLayout();

	this->schedule(schedule_selector(ReviseGameLayer::updateSeconds), 1.0f);

	return true;
}

void ReviseGameLayer::updateSeconds(float dt)
{
	if (m_iSeconds > 0 && m_bIsEndGame == false)
	{
		m_iSeconds--;
		char sSeconds[2];
		sprintf(sSeconds, "%d", m_iSeconds);
		m_pLabelClock->setString(sSeconds);
		if (m_iSeconds == 0)
		{
			addPopupEndGame();
		}
	}
	else if (m_bIsEndGame) {
		m_iSeconds =5;
		char sSeconds[2];
		sprintf(sSeconds, "%d", m_iSeconds);
		m_pLabelClock->setString(sSeconds);
	}
}

void ReviseGameLayer::addPopupEndGame()
{
	m_bIsEndGame = true;

	m_pLabelClock->setVisible(false);
	m_pPopupEndGame = LayerColor::create(ccc4(7, 25, 44, 150));
	m_pPopupEndGame->setContentSize(CCSizeMake(640.0f, 960.0f));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event* event) { return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, m_pPopupEndGame);
	this->addChild(m_pPopupEndGame);

	Sprite* pBackgroundPopUp = Sprite::create("AdvanceMode/panel-AM-short.png");
	pBackgroundPopUp->setPosition(Point(320.0f, 595.0f));
	m_pPopupEndGame->addChild(pBackgroundPopUp);

	LabelBMFont *pLabelTitle = LabelBMFont::create("GAME OVER", "fonts/font-bechic.fnt");
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelTitle->setPosition(Point(320.0f, 872.0f));
	m_pPopupEndGame->addChild(pLabelTitle);

	Sprite* pPanel = Sprite::create("AdvanceMode/panel-target-end-game.png");
	pPanel->setPosition(Point(320.0f, 640.0f));
	m_pPopupEndGame->addChild(pPanel);

	if (m_chapterInfo.iStageRevise < m_iStage)
	{
		Sprite* pNewRecord = Sprite::create("AdvanceMode/new-record.png");
		pNewRecord->setPosition(Point(548.0f, 726.0f));
		m_pPopupEndGame->addChild(pNewRecord);
		m_chapterInfo.iStageRevise = m_iStage;
		ChapterTable::getInstance()->updateChapter(m_chapterInfo);
	}

	LabelBMFont *pLabelStage = LabelBMFont::create("Score", "fonts/font_title-popup.fnt", 310.0f);
	pLabelStage->setAnchorPoint(Point(0.0f, 0.5f));
	pLabelStage->setPosition(Point(150.0f, 670));
	m_pPopupEndGame->addChild(pLabelStage);

	char sStage[10];
	sprintf(sStage, "%d", m_iStage);
	LabelBMFont *pLabelNumberStage = LabelBMFont::create(sStage, "fonts/font_score.fnt");
	pLabelNumberStage->setAnchorPoint(Point(1.0f, 0.5f));
	pLabelNumberStage->setPosition(Point(350.0f, 670));
	pLabelNumberStage->setScale(1.5);
	m_pPopupEndGame->addChild(pLabelNumberStage);

	LabelBMFont *pLabelBest= LabelBMFont::create("Best", "fonts/font_title-popup.fnt", 310.0f);
	pLabelBest->setAnchorPoint(Point(0.0f, 0.5f));
	pLabelBest->setPosition(Point(150.0f, 585));
	m_pPopupEndGame->addChild(pLabelBest);

	char sBestStage[10];
	sprintf(sBestStage, "%d", m_iStage);
	LabelBMFont *pLabelNumberBest = LabelBMFont::create(sBestStage, "fonts/font_score.fnt");
	pLabelNumberBest->setAnchorPoint(Point(1.0f, 0.5f));
	pLabelNumberBest->setPosition(Point(350.0f, 585));
	pLabelNumberBest->setScale(1.5);
	m_pPopupEndGame->addChild(pLabelNumberBest);

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	m_pPopupEndGame->addChild(pButtonManagerNode);

	Sprite* m_pButtonPlayImage = Sprite::create("AdvanceMode/btn_replay.png");
	ButtonNode* pButtonPlay = ButtonNode::createButtonSprite(m_pButtonPlayImage, CC_CALLBACK_1(ReviseGameLayer::clickRetry, this));
	pButtonPlay->setPosition(Point(320.0f, 410.0f));
	pButtonManagerNode->addButtonNode(pButtonPlay);
}

void ReviseGameLayer::clickRetry(Object* sender)
{
	m_iStage = 0;
	m_pLabelClock->setVisible(true);
	this->removeChild(m_pPopupEndGame);

	auto actionPlayEffectWin = CallFunc::create(this, callfunc_selector(ReviseGameLayer::playEffectWin));
	auto actionplayEffectAddLayout = CallFunc::create(this, callfunc_selector(ReviseGameLayer::playEffectAddLayout));
	auto actionupdateIsEndGame = CallFunc::create(this, callfunc_selector(ReviseGameLayer::updateIsEndGame));
	this->runAction(Sequence::create(DelayTime::create(0.3f), actionPlayEffectWin, DelayTime::create(0.2f), actionplayEffectAddLayout,  DelayTime::create(0.2f), actionupdateIsEndGame, NULL));
	
}

void ReviseGameLayer::randownIndexWordCollected()
{
	srand(time(NULL));
	m_iIndexWordCollected = rand() % (m_WordsCollected.size());
}

void ReviseGameLayer::randownIndexWords()
{
	while(!m_arrIndexWord.empty())
	{
		m_arrIndexWord.pop_back();
	}

	srand(time(NULL));

	while(1)
	{
		int bBreakWhile = true;
		int iIndexWord = rand() % (m_ChapterWords.size());
		if (m_ChapterWords[iIndexWord].sWordId == m_MaintWordInfo.sWordId)
			bBreakWhile = false;

		if (bBreakWhile)
		{
			for(int iIndex=0; iIndex<m_arrIndexWord.size(); iIndex++)
			{
				if (m_arrIndexWord[iIndex] == iIndexWord)
				{
					bBreakWhile = false;
				}
			}
		}


		if (bBreakWhile)
		{
			m_arrIndexWord.push_back(iIndexWord);
		}

		if (m_arrIndexWord.size() == 3)
		{
			break;
		}
	}
}

void ReviseGameLayer::createLayout()
{
	m_pLabelClock->setVisible(true);

	this->randownIndexWordCollected();

	m_MaintWordInfo = m_WordsCollected[m_iIndexWordCollected];
	int iIndex = GameWordManager::getInstance()->GetLoadedWordIndexFromID(m_MaintWordInfo.sWordId);	
	auto& wordMain = GameWordManager::getInstance()->GetWord(iIndex);

	m_pFlashCard = FlashCardNode::createLayout(wordMain);
	m_pFlashCard->setVisibleContentFlashCard(false);
	m_pFlashCard->setScale(0.8f);
	m_pFlashCard->setPosition(Point(80.0f, 220.0f));
	this->addChild(m_pFlashCard);

	this->randownIndexWords();

	m_pChooseImageNode = Node::create();
	this->addChild(m_pChooseImageNode);

	LabelBMFont *pLabelTitle = LabelBMFont::create("CHOOSE RIGHT PICTURE", "fonts/font_score.fnt");
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelTitle->setPosition(Point(330.0f, 467.0f));
	m_pChooseImageNode->addChild(pLabelTitle);

	ButtonManagerNode* pButtonManageNode = ButtonManagerNode::create();
	m_pChooseImageNode->addChild(pButtonManageNode);

	srand(time(NULL));
	int iIndexPicture = rand() % (4);

	ButtonNode* pButtonNodeMain = this->createButtonPictureFlashcard(m_MaintWordInfo, iIndexPicture);
	pButtonNodeMain->setTag(1);
	pButtonManageNode->addButtonNode(pButtonNodeMain);

	for (int iIndex=0; iIndex<m_arrIndexWord.size(); iIndex++)
	{
		int iIndexWord = m_arrIndexWord[iIndex];

		if (iIndex < iIndexPicture)
		{
			ButtonNode* pButtonNode = this->createButtonPictureFlashcard(m_ChapterWords[iIndexWord], iIndex);
			pButtonNode->setTag(0);
			pButtonManageNode->addButtonNode(pButtonNode);
		}
		else
		{
			ButtonNode* pButtonNode = this->createButtonPictureFlashcard(m_ChapterWords[iIndexWord], iIndex+1);
			pButtonNode->setTag(0);
			pButtonManageNode->addButtonNode(pButtonNode);
		}
	}
}

ButtonNode* ReviseGameLayer::createButtonPictureFlashcard(const WordInfo& wordInfo, const int& iIndexPicture)
{
	int iIndex = GameWordManager::getInstance()->GetLoadedWordIndexFromID(wordInfo.sWordId);
	auto& word3 = GameWordManager::getInstance()->GetWord(iIndex);
	std::string sPath = GameWordManager::getInstance()->GetPackagePathFromWord(word3);;
	sPath.append("/FlashCard/");
	sPath.append(word3.m_sFlashCardImage);

	Sprite* pFlashCardImage = Sprite::create(sPath.c_str());
	pFlashCardImage->setScale(0.5f);
	pFlashCardImage->setPosition(Point(109.0f, 78.5f));

	Sprite* pBackground = Sprite::create("FlashCard/answer.png");
	pBackground->setTag(10);
	pBackground->addChild(pFlashCardImage);
	ButtonNode* pButtonNode = ButtonNode::createButtonSprite(pBackground, CC_CALLBACK_1(ReviseGameLayer::clickChoosePicture, this));

	if (iIndexPicture == 0)
	{
		Sprite* pIconA = Sprite::create("FlashCard/a_icon.png");
		pIconA->setPosition(Point(109.0f, 0.0f));
		pBackground->addChild(pIconA);
		pButtonNode->setPosition(Point(200.0f, 363.0f));
	}
	else if (iIndexPicture == 1)
	{
		Sprite* pIconB = Sprite::create("FlashCard/b_icon.png");
		pIconB->setPosition(Point(109.0f, 0.0f));
		pBackground->addChild(pIconB);
		pButtonNode->setPosition(Point(443.0f, 363.0f));
	}
	else if (iIndexPicture == 2)
	{
		Sprite* pIconC = Sprite::create("FlashCard/c_icon.png");
		pIconC->setPosition(Point(109.0f, 0.0f));
		pBackground->addChild(pIconC);
		pButtonNode->setPosition(Point(200.0f, 160.0f));
	}
	else if (iIndexPicture == 3)
	{
		Sprite* pIconD = Sprite::create("FlashCard/d_icon.png");
		pIconD->setPosition(Point(109.0f, 0.0f));
		pBackground->addChild(pIconD);
		pButtonNode->setPosition(Point(443.0f, 160.0f));
	}

	return pButtonNode;
}

void ReviseGameLayer::clickChoosePicture(Object* sender)
{
	m_bIsEndGame = true;

	ButtonNode* pButtonNode = (ButtonNode*)sender;
	if (pButtonNode->getTag() == 1)
	{
		// Choose correct
		Sprite* pIconCorrect = Sprite::create("FlashCard/check.png");
		pIconCorrect->setPosition(Point(109.0f, 78.5f));
		pButtonNode->getChildByTag(10)->addChild(pIconCorrect);

		m_pFlashCard->setVisibleContentFlashCard(true);
		m_iStage++;

		auto actionPlayEffectWin = CallFunc::create(this, callfunc_selector(ReviseGameLayer::playEffectWin));
		auto actionplayEffectAddLayout = CallFunc::create(this, callfunc_selector(ReviseGameLayer::playEffectAddLayout));
		auto actionupdateIsEndGame = CallFunc::create(this, callfunc_selector(ReviseGameLayer::updateIsEndGame));
		this->runAction(Sequence::create(DelayTime::create(0.3f), actionPlayEffectWin, DelayTime::create(0.2f), actionplayEffectAddLayout,  DelayTime::create(0.2f), actionupdateIsEndGame, NULL));
	}
	else
	{
		// Choose incorrect
		Sprite* pIconWrong = Sprite::create("FlashCard/wrong_icon.png");
		pIconWrong->setPosition(Point(109.0f, 78.5f));
		pButtonNode->getChildByTag(10)->addChild(pIconWrong);
		
		auto actionPlayEffectLose = CallFunc::create(this, callfunc_selector(ReviseGameLayer::playEffectLose));
		this->runAction(Sequence::create(DelayTime::create(0.3f), actionPlayEffectLose, NULL));
	}
}

void ReviseGameLayer::playEffectLose()
{
	m_pLabelClock->setVisible(false);
	addPopupEndGame();
}
  
void ReviseGameLayer::playEffectWin()
{
	m_pLabelClock->setVisible(false);

	auto actionFlashcardMoveLeft = MoveBy::create(0.2f, Point(-640.0f, 0.0f));
	auto actionChooseImageMoveLeft = MoveBy::create(0.2f, Point(-640.0f, -70.0f));

	m_pFlashCard->runAction(actionFlashcardMoveLeft);
	m_pChooseImageNode->runAction(actionChooseImageMoveLeft);
}

void ReviseGameLayer::playEffectAddLayout()
{
	this->removeChild(m_pFlashCard);
	this->removeChild(m_pChooseImageNode);
	this->createLayout();

	m_pFlashCard->setScale(0);
	m_pChooseImageNode->setScale(0);

	auto actionFlashcardScaleOut = ScaleTo::create(0.2f, 0.8f);
	auto actionChooseImageScaleOut = ScaleTo::create(0.2f, 1.0f);

	m_pFlashCard->runAction(actionFlashcardScaleOut);
	m_pChooseImageNode->runAction(actionChooseImageScaleOut);
}

void ReviseGameLayer::updateIsEndGame()
{
	m_bIsEndGame = false;
}