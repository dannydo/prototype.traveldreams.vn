#ifndef _ICON_LOADING_NODE_H_
#define _ICON_LOADING_NODE_H_

#include "cocos2d.h"

USING_NS_CC;

class IconLoadingNode : public cocos2d::Node
{
public:
	IconLoadingNode() {};
	virtual ~IconLoadingNode();
	bool init() override;

	CREATE_FUNC(IconLoadingNode);

private:
	void rotateIconLoading(float dt);

	cocos2d::Sprite* m_pIconLoadingImage;
	float m_iRotate;

};

#endif;