#include "Scale3Sprite.h"

Scale3Sprite* Scale3Sprite::create( SpriteBatchNode* pBatchNode, SpriteFrame* pParentSpriteFrame, float fInsetLeft, float fInsetRight, int iZOrder)
{
	auto sprite = new Scale3Sprite();
	if (sprite && sprite->init( pBatchNode, pParentSpriteFrame, fInsetLeft, fInsetRight, iZOrder))
	{
		sprite->autorelease();

		return sprite;
	}

	CC_SAFE_DELETE(sprite);
	return NULL;
}

Scale3Sprite::~Scale3Sprite()
{
	CC_SAFE_RELEASE(m_pLeftSprite);
	CC_SAFE_RELEASE(m_pCenterSprite);
	CC_SAFE_RELEASE(m_pRightSprite);
}

Scale3Sprite::Scale3Sprite()
{
}

bool Scale3Sprite::init(SpriteBatchNode* pBatchNode, SpriteFrame* pParentSpriteFrame, float fInsetLeft, float fInsetRight, int iZOrder)
{
	m_pBatchNode = pBatchNode;
	Size totalSize = pParentSpriteFrame->getRect().size;
	float x =0, y = 0;
	float fLeftW = fInsetLeft, fCenterW = totalSize.width- fInsetLeft - fInsetRight, fRightW = fInsetRight;
	
	// calculate boundary of each region
	//left
	Rect leftBounds = Rect( x, y, fLeftW, totalSize.height);

	// center
	x += fLeftW;
	Rect centerBounds = Rect( x, y, fCenterW, totalSize.height);

	// right
	x += fCenterW;
	Rect rightBounds = Rect( x, y, fRightW, totalSize.height);

	AffineTransform t = AffineTransform::IDENTITY;

	if (!pParentSpriteFrame->isRotated())
	{
		t = AffineTransformTranslate(t, pParentSpriteFrame->getRect().origin.x, pParentSpriteFrame->getRect().origin.y);

		leftBounds = RectApplyAffineTransform( leftBounds, t);
		centerBounds = RectApplyAffineTransform( centerBounds, t);
		rightBounds = RectApplyAffineTransform( rightBounds, t);


		// create sprites
		m_pLeftSprite = new Sprite();
		m_pLeftSprite->initWithTexture( pBatchNode->getTexture(), leftBounds);
		m_pLeftSprite->setAnchorPoint(Point(0,0));
		pBatchNode->addChild( m_pLeftSprite);
	

		m_pCenterSprite = new Sprite();
		m_pCenterSprite->initWithTexture( pBatchNode->getTexture(), centerBounds);
		m_pCenterSprite->setAnchorPoint(Point(0,0));
		pBatchNode->addChild( m_pCenterSprite);

		m_pRightSprite = new Sprite();
		m_pRightSprite->initWithTexture( pBatchNode->getTexture(), rightBounds);
		m_pCenterSprite->setAnchorPoint(Point(0,0));
		pBatchNode->addChild( m_pRightSprite);	
	}
	else
	{
		Rect rotatedLeftBounds = leftBounds;
		Rect rotatedCenterBounds = centerBounds;
		Rect rotatedRightBounds = rightBounds;

		t = AffineTransformTranslate( t, totalSize.height + pParentSpriteFrame->getRect().origin.x, pParentSpriteFrame->getRect().origin.y);
		t = AffineTransformRotate( t, 1.57079633f);

		leftBounds = RectApplyAffineTransform( leftBounds, t);
		centerBounds = RectApplyAffineTransform( centerBounds, t);
		rightBounds = RectApplyAffineTransform( rightBounds, t);

		rotatedLeftBounds.origin = leftBounds.origin;
		rotatedCenterBounds.origin = centerBounds.origin;
		rotatedRightBounds.origin = rightBounds.origin;	

		m_pLeftSprite = new Sprite();
		m_pLeftSprite->initWithTexture( pBatchNode->getTexture(), rotatedLeftBounds, true);
		m_pLeftSprite->setAnchorPoint(Point(0,0));
		pBatchNode->addChild( m_pLeftSprite);
	

		m_pCenterSprite = new Sprite();
		m_pCenterSprite->initWithTexture( pBatchNode->getTexture(), rotatedCenterBounds, true);
		m_pCenterSprite->setAnchorPoint(Point(0,0));
		pBatchNode->addChild( m_pCenterSprite);

		m_pRightSprite = new Sprite();
		m_pRightSprite->initWithTexture( pBatchNode->getTexture(), rotatedRightBounds, true);
		m_pRightSprite->setAnchorPoint(Point(0,0));
		pBatchNode->addChild( m_pRightSprite);	
	}

	return true;
}

void Scale3Sprite::setVisible(bool bIsVisible) 
{
	m_pLeftSprite->setVisible(bIsVisible);
	m_pCenterSprite->setVisible(bIsVisible);
	m_pRightSprite->setVisible(bIsVisible);
}

void Scale3Sprite::setContentSize(const Size & size)
{
	Node::setContentSize(size);
	

	float x = _position.x, y = _position.y;

	// set scale of center
	float fSizableWidth = size.width - m_pLeftSprite->getContentSize().width - m_pRightSprite->getContentSize().width;
	float fHorizontalScale = fSizableWidth / m_pCenterSprite->getContentSize().width;
	
	m_pCenterSprite->setScaleX(fHorizontalScale);

	// correct position
	m_pLeftSprite->setPosition(_position);

	x += m_pLeftSprite->getContentSize().width;
	m_pCenterSprite->setPosition(Point( x, y));

	x += m_pCenterSprite->getContentSize().width * fHorizontalScale;
	m_pRightSprite->setPosition(Point( x, y));
}