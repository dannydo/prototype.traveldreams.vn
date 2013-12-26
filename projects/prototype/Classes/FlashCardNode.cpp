#include "FlashCardNode.h"

USING_NS_CC;

FlashCardNode* FlashCardNode::createLayout(const Word word)
{
	FlashCardNode* pFlashCard = new FlashCardNode();
	pFlashCard->m_word = word;
	if(pFlashCard->init())
	{
		pFlashCard->autorelease();
		return pFlashCard;
	}

	CC_SAFE_DELETE(pFlashCard);
	return NULL;
}

bool FlashCardNode::init()
{
	if(!Node::init())
	{
		return false;
	}

	m_pBackground = Sprite::create("FlashCard/background.png");
	m_pBackground->setOpacity(0);
	this->addChild(m_pBackground);

	std::string sPath = "FlashCard/";
	sPath.append(m_word.m_sFlashCardImage);
	m_pFlashCard = Sprite::create(sPath.c_str());
	m_pFlashCard->setPosition(Point(96.0f, 3.0f));
	m_pFlashCard->setOpacity(0);
	this->addChild(m_pFlashCard);

	Node* pWord = Node::create();
	m_pTitleWord = LabelTTF::create(m_word.m_sWord, "Arial", 25);
	m_pTitleWord->setColor(ccc3(13, 118, 200));
	m_pTitleWord->setPositionY(10);
	m_pTitleWord->setOpacity(0);
	pWord->addChild(m_pTitleWord);

	m_pMeaningWord = LabelTTF::create(m_word.m_sMeaning.c_str(), "Arial", 18);
	m_pMeaningWord->setColor(ccc3(102, 102, 102));
	m_pMeaningWord->setPositionY(-30);
	m_pMeaningWord->setOpacity(0);
	pWord->addChild(m_pMeaningWord);

	pWord->setContentSize(CCSizeMake(106.0f, 180.0f));
	pWord->setPosition(-108.0f, 3.0f);
	this->addChild(pWord);

	return true;
}

void FlashCardNode::displayEffect(const float& fTimeDisplay, const float& fTimeEffect)
{
	DelayTime* delayDisplay = DelayTime::create(fTimeDisplay);
	DelayTime* delayEffect = DelayTime::create(fTimeEffect);
	
	auto actionFadeOutMeaningWord = FadeOut::create(fTimeEffect);
	auto actionFadeInMeaningWord = FadeIn::create(fTimeEffect);
	m_pMeaningWord->runAction(Sequence::create(actionFadeInMeaningWord, delayEffect->clone(), delayDisplay->clone(), actionFadeOutMeaningWord, NULL));

	auto actionFadeOutTitleWord = FadeOut::create(fTimeEffect);
	auto actionFadeInTitleWord = FadeIn::create(fTimeEffect);
	m_pTitleWord->runAction(Sequence::create(actionFadeInTitleWord, delayEffect->clone(), delayDisplay->clone(), actionFadeOutTitleWord, NULL));
	
	auto actionFadeOutFlashCard = FadeOut::create(fTimeEffect);
	auto actionFadeInFlashCard = FadeIn::create(fTimeEffect);
	m_pFlashCard->runAction(Sequence::create(actionFadeInFlashCard, delayEffect->clone(), delayDisplay->clone(), actionFadeOutFlashCard, NULL));

	auto actionFadeOutBackground = FadeOut::create(fTimeEffect);
	auto actionFadeInBackground = FadeIn::create(fTimeEffect);
	m_pBackground->runAction(Sequence::create(actionFadeInBackground, delayEffect->clone(), delayDisplay->clone(), actionFadeOutBackground, NULL));

	auto actionRemove = CallFunc::create(this, callfunc_selector(FlashCardNode::removeLayout));
	this->runAction(Sequence::create(delayEffect->clone(), delayDisplay->clone(), delayEffect->clone(), NULL));
}

void FlashCardNode::removeLayout()
{
	this->getParent()->removeChild(this);
}