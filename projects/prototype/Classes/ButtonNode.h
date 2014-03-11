#ifndef _BUTTON_NODE_H_
#define _BUTTON_NODE_H_

#include "cocos2d.h"



typedef std::function<void(cocos2d::Object*)> buttonNodeCallback;

class ButtonNode : public cocos2d::Node
{

public:
	ButtonNode() {};
	virtual ~ButtonNode();
	static ButtonNode* createButtonSprite(cocos2d::Sprite *pSpriteButton, const buttonNodeCallback& callBack);
	bool init() override;

	inline cocos2d::Sprite* getButtonSprite() { return m_ButtonSprite; };
	inline buttonNodeCallback getCallBack() { return m_callBack; };

private:
	cocos2d::Sprite* m_ButtonSprite;
	buttonNodeCallback m_callBack;
};

#endif;