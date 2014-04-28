#ifndef _CLIP_MASK_NODE_H_
#define _CLIP_MASK_NODE_H_

#include "cocos2d.h"
#include "CustomNode.h"

class ClipMaskNode : public cocos2d::Node
{
public:
	ClipMaskNode() {};
	virtual ~ClipMaskNode();
	bool init() override;
	virtual void visit() override;

	CREATE_FUNC(ClipMaskNode);
private:
};

#endif;