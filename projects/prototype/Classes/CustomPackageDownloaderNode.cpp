#include "CustomPackageDownloaderNode.h"

CustomPackageDownloaderNode* CustomPackageDownloaderNode::create()
{
	CustomPackageDownloaderNode* renderNode = new CustomPackageDownloaderNode();
	if( renderNode)
	{				
		renderNode->autorelease();
		renderNode->init();
		return renderNode;
	}

	CC_SAFE_DELETE(renderNode);
	return NULL;
}

CustomPackageDownloaderNode::CustomPackageDownloaderNode()
{
	m_pAssetManager = NULL;
}

CustomPackageDownloaderNode::~CustomPackageDownloaderNode()
{
	if (m_pAssetManager)
		delete m_pAssetManager;
}

bool CustomPackageDownloaderNode::init()
{
	auto pColorNode = DrawNode::create();
	CCPoint vertex[] = { ccp(0,0), ccp(400,0), ccp(400,400), ccp(0,400) };
	pColorNode->drawPolygon(vertex, 4, ccc4f(70, 70, 70, 1.f), 0, ccc4f(70, 70, 70, 1.f) );
	this->addChild(pColorNode);

	
	auto editCode = EditBox::create(Size(280.f, 50.f), Scale9Sprite::create("fonts/green_edit.png"));
	editCode->setFont("Arial", 28);
    editCode->setPosition( Point(60.f, 350.f)); //Point(visibleOrigin.x+visibleSize.width/2, visibleOrigin.y+visibleSize.height/4));
    editCode->setAnchorPoint(Point(0, 0.5f));
    editCode->setPlaceHolder("Code");
	editCode->setInputMode(EditBox::InputMode::ANY);
    //editCode->setDelegate(this);
    addChild(editCode);

	auto label1Button = LabelTTF::create("Download", "Arial", 28);	
	auto menu1Item = MenuItemLabel::create( label1Button, CC_CALLBACK_1(CustomPackageDownloaderNode::startDownloadCallback, this));		
	menu1Item->setPosition( Point(100.f, 50.f));

	auto label2Button = LabelTTF::create("Cancel", "Arial", 28);	
	auto menu2Item = MenuItemLabel::create( label2Button, CC_CALLBACK_1(CustomPackageDownloaderNode::exitCallback, this));		
	menu2Item->setPosition( Point(300.f, 50.f));

	auto menu = Menu::create( menu1Item, menu2Item, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu);

	return true;
}

void CustomPackageDownloaderNode::startDownloadCallback(Object* sender)
{
	log("override restart!");
}


void CustomPackageDownloaderNode::exitCallback(Object* sender)
{
	log("override restart!");
}
