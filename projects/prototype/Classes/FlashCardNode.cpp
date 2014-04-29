#include "FlashCardNode.h"
#include "GameWordManager.h"
#include "SoundManager.h"

USING_NS_CC;

FlashCardNode::~FlashCardNode ()
{
}

FlashCardNode* FlashCardNode::createLayout(const Word& word)
{
	FlashCardNode* pFlashCardNode = new FlashCardNode();
	pFlashCardNode->m_pWord = &word;


	if(pFlashCardNode->init())
	{
		pFlashCardNode->autorelease();
		return pFlashCardNode;
	}

	CC_SAFE_DELETE(pFlashCardNode);
	return NULL;
}

bool FlashCardNode::init()
{
	if (!Node::init())
	{
		return false;
	}

	LayerColor* pBackgroundNode = LayerColor::create(ccc4(255, 244, 215, 255));
	pBackgroundNode->setContentSize(Size(400.0f, 290.0f));
	pBackgroundNode->setAnchorPoint(Point(0.5f, 0.5f));
	pBackgroundNode->setPosition(Point(120.0f, 384.0f));
	this->addChild(pBackgroundNode);

	m_pFlashCardContent = Node::create();
	this->addChild(m_pFlashCardContent);

	std::string sPath = GameWordManager::getInstance()->GetPackagePathFromWord(*m_pWord);;
	sPath.append("/FlashCard/");
	sPath.append(m_pWord->m_sFlashCardImage);
	Sprite* pFlashCardImage = Sprite::create(sPath.c_str());
	pFlashCardImage->setPosition(Point(320.0f, 520.0f));
	m_pFlashCardContent->addChild(pFlashCardImage);

	LabelTTF* pLabelWordMean = LabelTTF::create(m_pWord->m_sMeaning.c_str(), "Arial", 30.0f);
	pLabelWordMean->setColor(ccc3(0.0f, 0.0f, 0.0f));
	pLabelWordMean->setPosition(Point(320.0f, 399.0f));
	m_pFlashCardContent->addChild(pLabelWordMean);

	Sprite* pFlashCardBG = Sprite::create("FlashCard/flashcard_frame.png");
	pFlashCardBG->setPosition(Point(320.0f, 580.0f));
	this->addChild(pFlashCardBG);
	
	LabelBMFont *pLabelWord = LabelBMFont::create(m_pWord->m_sWord, "fonts/font-bechic.fnt");
	pLabelWord->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelWord->setPosition(Point(320.0f, 730.0f));
	this->addChild(pLabelWord);

	LabelTTF* pLabelWordPhonetic = LabelTTF::create(m_pWord->m_sPhonetic.c_str(), "Arial", 28.0f);
	pLabelWordPhonetic->setColor(ccc3(0.0f, 0.0f, 0.0f));
	pLabelWordPhonetic->setPosition(Point(320.0f, 679.0f));
	this->addChild(pLabelWordPhonetic);

	return true;
}

void FlashCardNode::addLayoutDescriptionWord()
{
	m_pNodeDescription = Node::create();
	this->addChild(m_pNodeDescription);

	Sprite* pBackground = Sprite::create("FlashCard/flashcard_anwser_panel.png");
	pBackground->setPosition(Point(320.0f, 225.0f));
	m_pNodeDescription->addChild(pBackground);

	LabelTTF* pLabelWordSentence = LabelTTF::create(m_pWord->m_sSentence.c_str(), "Arial", 30.0f, Size(335.0f, 160.0f), TextHAlignment::LEFT, TextVAlignment::CENTER);
	pLabelWordSentence->setColor(ccc3(0.0f, 0.0f, 0.0f));
	pLabelWordSentence->setPosition(Point(320.0f, 225.0f));
	m_pNodeDescription->addChild(pLabelWordSentence);

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	m_pNodeDescription->addChild(pButtonManagerNode);

	Sprite* pIconSound = Sprite::create("FlashCard/btn_voice.png");
	ButtonNode* pButtonSound = ButtonNode::createButtonSprite(pIconSound, CC_CALLBACK_1(FlashCardNode::playVoiceSentence, this));
	pButtonSound->setPosition(Point(510.0f, 105.0f));
	pButtonManagerNode->addButtonNode(pButtonSound);
}

void FlashCardNode::addButtonPlaySoundWord()
{
	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	this->addChild(pButtonManagerNode);

	Sprite* pIconSound = Sprite::create("FlashCard/btn_voice.png");
	ButtonNode* pButtonSound = ButtonNode::createButtonSprite(pIconSound, CC_CALLBACK_1(FlashCardNode::playVoiceWord, this));
	pButtonSound->setPosition(Point(515.0f, 784.0f));
	pButtonManagerNode->addButtonNode(pButtonSound);
}

void FlashCardNode::addLayoutQuestion()
{
	m_pFlashCardContent->setVisible(false);

	m_pButtonManagerNodeQuestion = ButtonManagerNode::create();
	this->addChild(m_pButtonManagerNodeQuestion);

	Sprite* pIconQuestion = Sprite::create("FlashCard/question_mark.png");
	m_pButtonQuestion = ButtonNode::createButtonSprite(pIconQuestion, CC_CALLBACK_1(FlashCardNode::clickHintQuestion, this));
	m_pButtonQuestion->setPosition(Point(320.0f, 560.0f));
	m_pButtonManagerNodeQuestion->addButtonNode(m_pButtonQuestion);

	Sprite* pIconCoin = Sprite::create("FlashCard/coin_flashcard.png");
	pIconCoin->setPosition(Point(180.0f, 430.0f));
	m_pButtonManagerNodeQuestion->addChild(pIconCoin);

	Sprite* pIconLight = Sprite::create("FlashCard/light_tube.png");
	pIconLight->setPosition(Point(440.0f, 460.0f));
	m_pButtonManagerNodeQuestion->addChild(pIconLight);

	m_pLabelGold = LabelTTF::create("+100", "Arial", 35.0f);
	m_pLabelGold->setColor(ccc3(0.0f, 0.0f, 0.0f));
	m_pLabelGold->setPosition(Point(255.0f, 429.0f));
	m_pButtonManagerNodeQuestion->addChild(m_pLabelGold);
}

void FlashCardNode::removeButtonManageQuestion()
{
	m_pFlashCardContent->setVisible(true);

	this->removeChild(m_pButtonManagerNodeQuestion);
}

void FlashCardNode::playVoiceWord(Object* pSender)
{	
	SoundManager::PlaySpellingOfWord(this, *m_pWord);
}

void FlashCardNode::playVoiceSentence(Object* pSender)
{
	SoundManager::PlaySentenceOfWord(this, *m_pWord);
}

void FlashCardNode::clickHintQuestion(Object* pSender)
{
	LabelTTF* pLabelWordSentence = LabelTTF::create(m_pWord->m_sSentence.c_str(), "Arial", 30.0f, Size(335.0f, 160.0f), TextHAlignment::LEFT, TextVAlignment::CENTER);
	pLabelWordSentence->setColor(ccc3(0.0f, 0.0f, 0.0f));
	pLabelWordSentence->setPosition(Point(320.0f, 560.0f));
	m_pButtonManagerNodeQuestion->addChild(pLabelWordSentence);
	m_pButtonManagerNodeQuestion->removeButtonNode(m_pButtonQuestion);
}