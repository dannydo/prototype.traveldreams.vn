#include "ClipMaskNode.h"

USING_NS_CC;

ClipMaskNode::~ClipMaskNode ()
{
}

bool ClipMaskNode::init()
{
	if (!Node::init())
	{
		return false;
	}

	return true;
}

void ClipMaskNode::visit()
{	
	kmGLPushMatrix();
	glEnable(GL_SCISSOR_TEST);

	// scissorRect is the rectangle you want to show.
	EGLView::getInstance()->setScissorInPoints(
		_position.x, _position.y,
		_contentSize.width, _contentSize.height);

	Node::visit();

	glDisable(GL_SCISSOR_TEST);
	kmGLPopMatrix();
}