#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include "cocos2d.h"

class DictionaryNode : public cocos2d::Node
{
public:
	~DictionaryNode();
	static DictionaryNode* createLayout(const char* sWordDict);
	bool init() override;

private:
	void menuCloseCallBack();
	void onTouchBackground(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent);

	cocos2d::Sprite* m_pBackground;
	const char* m_sWordDict;
};

#endif;
