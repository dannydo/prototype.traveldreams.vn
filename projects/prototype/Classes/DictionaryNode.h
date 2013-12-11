#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include "cocos2d.h"

class DictionaryNode : public cocos2d::Node
{
public:
	bool init() override;

	CREATE_FUNC(DictionaryNode);

private:
	void menuCloseCallBack();
};

#endif;
