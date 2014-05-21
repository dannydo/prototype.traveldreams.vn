#include "IconLoadingNode.h"

USING_NS_CC;

IconLoadingNode::~IconLoadingNode ()
{
}

bool IconLoadingNode::init()
{
	if (!Node::init())
	{
		return false;
	}

	m_pIconLoadingImage = Sprite::create("popup/loading-icons.png");
	this->addChild(m_pIconLoadingImage);

	this->schedule(schedule_selector(IconLoadingNode::rotateIconLoading), 0.1f);
	m_iRotate = 0;

	return true;
}

void IconLoadingNode::rotateIconLoading(float dt)
{
	m_iRotate += 45;
	m_pIconLoadingImage->setRotation(m_iRotate);
	if (m_iRotate >= 360)
		m_iRotate = 0;
}
