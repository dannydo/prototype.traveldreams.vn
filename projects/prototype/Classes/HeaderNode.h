#ifndef _HEADER_NODE_H_
#define _HEADER_NODE_H_

#include "cocos2d.h"

class HeaderNode : public cocos2d::Node
{

public:
	HeaderNode() {};
	virtual ~HeaderNode();
	bool init() override; 
	CREATE_FUNC(HeaderNode);

private:
};

#endif;