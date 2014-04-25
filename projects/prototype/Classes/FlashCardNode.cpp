#include "FlashCardNode.h"
#include "GameWordManager.h"

USING_NS_CC;

FlashCardNode::~FlashCardNode ()
{
}

FlashCardNode* FlashCardNode::createLayout(const Word& word)
{
	FlashCardNode* pFlashCardNode = new FlashCardNode();
	pFlashCardNode->m_Word = word;


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

	std::string sPath = GameWordManager::getInstance()->GetPackagePathFromWord(m_Word);;
	sPath.append("/FlashCard/");
	sPath.append(m_Word.m_sFlashCardImage);
	Sprite* pFlashCardImage = Sprite::create(sPath.c_str());
	pFlashCardImage->setPosition(Point(-6.0f, -20.0f));
	this->addChild(pFlashCardImage);

	Sprite* pFlashCardBG = Sprite::create("FlashCard/flashcard_frame.png");
	pFlashCardBG->setPosition(Point(0.0f, 0.0f));
	this->addChild(pFlashCardBG);

	return true;
}