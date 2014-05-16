#include "MiniGameScene.h"
#include "FooterNode.h"
#include "SettingMenuNode.h"
#include "FunctionCommon.h"
#include "GameWordManager.h"
#include "Database\UserTable.h"
#include "Database\ChapterTable.h"
#include "SoundManager.h"

USING_NS_CC;


bool MiniGameScene::init()
{
	if(CCScene::init())
	{
		this->_layer = MiniGameLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}
	else
	{
		return false;
	}
}

MiniGameScene::~MiniGameScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}

MiniGameLayer::~MiniGameLayer()
{
	
}

bool MiniGameLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	m_WordsNew = WordTable::getInstance()->getAllWordNew(getTimeLocalCurrent());
	m_iTotalWordNew = WordTable::getInstance()->getNumberWordPlayMiniGame(getTimeLocalCurrent());
	if (m_iTotalWordNew > m_WordsNew.size())
		m_iTotalWordNew = m_WordsNew.size();

	Sprite* pBackground = Sprite::create("FlashCard/background.png");
	pBackground->setPosition(Point(320.0f, 480.0f));
	this->addChild(pBackground);

	char sTotalWordNew[20];
	if (m_iTotalWordNew > 0)
		sprintf(sTotalWordNew, "(%d/%d)", 1, m_iTotalWordNew);
	else
		sprintf(sTotalWordNew, "(%d/%d)", 0, m_iTotalWordNew);

	m_pLabelIndex = LabelTTF::create(sTotalWordNew, "Arial", 30);
	m_pLabelIndex->setColor(ccc3(0.0f, 0.0f, 0.0f));
	m_pLabelIndex->setPosition(Point(320.0f, 850.0f));
	this->addChild(m_pLabelIndex);

	FooterNode* pFooterNode = FooterNode::create();
	pFooterNode->changeStatusButtonFlashcard(StatusButtonFlashcard::eNoClick);
	pFooterNode->removeBackground();
	this->addChild(pFooterNode);

	m_pHeaderNode = HeaderNode::create();
	this->addChild(m_pHeaderNode);

	m_uTimeLocalCurrent = getTimeLocalCurrent();
	m_iIndexWordNewCount = 0;
	m_iIndexWordNew = 0;
	this->createLayout();

	Breadcrumb::getInstance()->addSceneMode(SceneMode::kFlashCard);

	return true;
}

void MiniGameLayer::randownIndexWordNew()
{
	srand(time(NULL));
	
	while(1)
	{
		if (m_arrIndexWordNew.size() == m_WordsNew.size())
		{
			break;
		}

		int bBreakWhile = true;
		int iIndexWordNew = rand() % (m_WordsNew.size());
		for(int iIndex=0; iIndex<m_arrIndexWordNew.size(); iIndex++)
		{
			if (m_arrIndexWordNew[iIndex] == iIndexWordNew)
			{
				bBreakWhile = false;
			}
		}

		if (bBreakWhile)
		{
			m_iIndexWordNew = iIndexWordNew;
			m_arrIndexWordNew.push_back(iIndexWordNew);
			break;
		}
	}
}

void MiniGameLayer::randownIndexWords()
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
		if (m_ChapterWords[iIndexWord].sWordId == m_WordsNew[m_iIndexWordNew].sWordId)
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

void MiniGameLayer::createLayout()
{
	if (m_iIndexWordNewCount < m_iTotalWordNew)
	{
		this->randownIndexWordNew();

		m_MaintWordInfo = m_WordsNew[m_iIndexWordNew];
		int iIndex = GameWordManager::getInstance()->GetLoadedWordIndexFromID(m_MaintWordInfo.sWordId);	
		auto& wordMain = GameWordManager::getInstance()->GetWord(iIndex);

		// spelling
		SoundManager::PlaySpellingOfWord( this,wordMain, 0.2f, false);

		m_pFlashCard = FlashCardNode::createLayout(wordMain);
		m_pFlashCard->addButtonPlaySoundWord();
		m_pFlashCard->setScale(0.7f);
		m_pFlashCard->addLayoutQuestion();
		m_pFlashCard->setPosition(Point(100.0f, 260.0f));
		this->addChild(m_pFlashCard);

		m_ChapterWords = WordTable::getInstance()->getAllWordsForChapter(m_MaintWordInfo.sChapterId);
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

		char sTotalWordNew[20];
		sprintf(sTotalWordNew, "(%d/%d)", m_iIndexWordNewCount+1, m_iTotalWordNew);
		m_pLabelIndex->setString(sTotalWordNew);
	}
	else
	{
		// Win Game
		LabelBMFont *pLabelTitle = LabelBMFont::create("NO WORD FOR PLAY MINI GAME", "fonts/font-bechic.fnt");
		pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
		pLabelTitle->setPosition(Point(330.0f, 480.0f));
		this->addChild(pLabelTitle);
	}
	m_iIndexWordNewCount++;
}

ButtonNode* MiniGameLayer::createButtonPictureFlashcard(const WordInfo& wordInfo, const int& iIndexPicture)
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
	ButtonNode* pButtonNode = ButtonNode::createButtonSprite(pBackground, CC_CALLBACK_1(MiniGameLayer::clickChoosePicture, this));

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

void MiniGameLayer::clickChoosePicture(Object* sender)
{
	ButtonNode* pButtonNode = (ButtonNode*)sender;
	if (pButtonNode->getTag() == 1)
	{
		// Choose correct
		Sprite* pIconCorrect = Sprite::create("FlashCard/check.png");
		pIconCorrect->setPosition(Point(109.0f, 78.5f));
		pButtonNode->getChildByTag(10)->addChild(pIconCorrect);

		auto actionPlayEffectWin = CallFunc::create(this, callfunc_selector(MiniGameLayer::playEffectWin));
		auto actionplayEffectAddDescription = CallFunc::create(this, callfunc_selector(MiniGameLayer::playEffectAddDescription));
		auto actionplayEffectAddButtonCollect = CallFunc::create(this, callfunc_selector(MiniGameLayer::playEffectAddButtonCollect));
		this->runAction(Sequence::create(DelayTime::create(0.3f), actionPlayEffectWin, DelayTime::create(0.3f), actionplayEffectAddDescription, DelayTime::create(0.3f), actionplayEffectAddButtonCollect, NULL));
	}
	else
	{
		// Choose incorrect
		Sprite* pIconWrong = Sprite::create("FlashCard/wrong_icon.png");
		pIconWrong->setPosition(Point(109.0f, 78.5f));
		pButtonNode->getChildByTag(10)->addChild(pIconWrong);

		auto actionPlayEffectLose = CallFunc::create(this, callfunc_selector(MiniGameLayer::playEffectLose));
		auto actionplayEffectAddLayout = CallFunc::create(this, callfunc_selector(MiniGameLayer::playEffectAddLayout));
		this->runAction(Sequence::create(DelayTime::create(0.3f), actionPlayEffectLose, DelayTime::create(0.25f), actionplayEffectAddLayout, NULL));
	}
}

void MiniGameLayer::clickCollect(Object* sender)
{
	m_pHeaderNode->updateLayoutMonney(UserTable::getInstance()->getUserInfo().iMonney);

	auto actionplayEffectAddLayout = CallFunc::create(this, callfunc_selector(MiniGameLayer::playEffectAddLayout));
	auto actionplayEffectCollect = CallFunc::create(this, callfunc_selector(MiniGameLayer::playEffectCollect));

	m_pFlashCard->runAction(Sequence::create(actionplayEffectCollect, DelayTime::create(0.3f), actionplayEffectAddLayout, NULL));
}

void MiniGameLayer::playEffectCollect()
{
	auto actionFlashcardScaleOut = ScaleTo::create(0.3f, 0.2f);
	auto actionFlashcardMove = MoveTo::create(0.3f, Point(640.0f, -40.0f));

	auto actionplayEffectAddLayout = CallFunc::create(this, callfunc_selector(MiniGameLayer::playEffectAddLayout));
	m_pFlashCard->runAction(Spawn::create(actionFlashcardScaleOut, actionFlashcardMove, NULL));
}

void MiniGameLayer::playEffectLose()
{
	auto actionFlashcardMoveLeft = MoveBy::create(0.2f, Point(-640.0f, 0.0f));
	auto actionChooseImageMoveLeft = MoveBy::create(0.2f, Point(-640.0f, -70.0f));

	m_pFlashCard->runAction(actionFlashcardMoveLeft);
	m_pChooseImageNode->runAction(actionChooseImageMoveLeft);

	m_MaintWordInfo.uTimeBeginPlayMiniGame = m_uTimeLocalCurrent;
	m_MaintWordInfo.bIsCollected = false;
	WordTable::getInstance()->updateWord(m_MaintWordInfo);

	for(int iIndex=0; iIndex<m_WordsNew.size(); iIndex++)
	{
		if(m_WordsNew[iIndex].sWordId == m_MaintWordInfo.sWordId && m_WordsNew[iIndex].sChapterId != m_MaintWordInfo.sChapterId)
		{
			m_arrIndexWordNew.push_back(iIndex);
		}
	}
}

void MiniGameLayer::playEffectWin()
{
	UserInfo userInfo =  UserTable::getInstance()->getUserInfo();
	userInfo.iMonney = userInfo.iMonney + _GIFT_COIN_;
	UserTable::getInstance()->updateUser(userInfo);

	m_MaintWordInfo.bIsCollected = true;
	m_MaintWordInfo.uTimeBeginPlayMiniGame = 0;
	WordTable::getInstance()->updateWord(m_MaintWordInfo);

	// Update chapter
	ChapterInfo chapterInfo = ChapterTable::getInstance()->getChapterInfo(m_MaintWordInfo.sChapterId);
	chapterInfo.iTotalFlashCardUnlock++;
	if (chapterInfo.iTotalFlashCardUnlock > chapterInfo.iTotalFlashCard)
	{
		chapterInfo.iTotalFlashCardUnlock = chapterInfo.iTotalFlashCard;
	}
	ChapterTable::getInstance()->updateChapter(chapterInfo);

	for(int iIndex=0; iIndex<m_WordsNew.size(); iIndex++)
	{
		if(m_WordsNew[iIndex].sWordId == m_MaintWordInfo.sWordId && m_WordsNew[iIndex].sChapterId != m_MaintWordInfo.sChapterId)
		{
			chapterInfo = ChapterTable::getInstance()->getChapterInfo(m_WordsNew[iIndex].sChapterId);
			chapterInfo.iTotalFlashCardUnlock++;
			if (chapterInfo.iTotalFlashCardUnlock > chapterInfo.iTotalFlashCard)
			{
				chapterInfo.iTotalFlashCardUnlock = chapterInfo.iTotalFlashCard;
			}
			ChapterTable::getInstance()->updateChapter(chapterInfo);

			m_arrIndexWordNew.push_back(iIndex);
		}
	}

	m_pFlashCard->removeButtonManageQuestion();

	auto actionChooseImageScaleOut = ScaleTo::create(0.3f, 0.0f);
	m_pChooseImageNode->runAction(actionChooseImageScaleOut);

	auto actionFlashcardScaleOut = ScaleTo::create(0.3f, 0.9f);
	auto actionFlashcardMove = MoveBy::create(0.3f, Point(-70.0f, -160.0f));

	m_pFlashCard->runAction(Spawn::create(actionFlashcardScaleOut, actionFlashcardMove, NULL));
}

void MiniGameLayer::playEffectAddDescription()
{
	m_pFlashCard->addLayoutDescriptionWord();
	m_pFlashCard->getNodeDescription()->setPosition(Point(0.0f, -320.0f));

	auto actionDescriptionMove = MoveBy::create(0.3f, Point(0.0f, 320.0f));
	m_pFlashCard->getNodeDescription()->runAction(actionDescriptionMove);
}

void MiniGameLayer::playEffectAddButtonCollect()
{
	Sprite* pBtnCollect = Sprite::create("FlashCard/btn_collect.png");
	ButtonNode* pButtonNode = ButtonNode::createButtonSprite(pBtnCollect, CC_CALLBACK_1(MiniGameLayer::clickCollect, this));
	pButtonNode->setPosition(Point(320.0f, 30.0f));
	pButtonNode->setScale(0.5f);
	
	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(pButtonNode);
	m_pFlashCard->addChild(pButtonManagerNode);

	auto actionScaleOut = ScaleTo::create(0.2f, 1.2f);
	auto actionScaleIn = ScaleTo::create(0.1f, 1.0f);

	pButtonNode->runAction(Sequence::create(actionScaleOut, actionScaleIn, NULL));
}

void MiniGameLayer::playEffectAddLayout()
{
	this->removeChild(m_pFlashCard);
	this->removeChild(m_pChooseImageNode);
	this->createLayout();

	if (m_iIndexWordNewCount <= m_iTotalWordNew)
	{
		m_pFlashCard->setScale(0);
		m_pChooseImageNode->setScale(0);

		auto actionFlashcardScaleOut = ScaleTo::create(0.2f, 0.7f);
		auto actionChooseImageScaleOut = ScaleTo::create(0.2f, 1.0f);

		m_pFlashCard->runAction(actionFlashcardScaleOut);
		m_pChooseImageNode->runAction(actionChooseImageScaleOut);
	}
}