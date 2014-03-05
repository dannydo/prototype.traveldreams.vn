#ifndef _LOADING_IMAGE_H_
#define _LOADING_IMAGE_H_

#include "cocos2d.h"
#include "cocos-ext.h"

class LoadingImagetNode : public cocos2d::Node
{
public:
	LoadingImagetNode() {};
	virtual ~LoadingImagetNode();

	static LoadingImagetNode* createLayout(const char* sUrlImage);
	bool init() override;
private:
	void onHttpRequestCompleted(cocos2d::extension::HttpClient *sender, cocos2d::extension::HttpResponse *response);

	const char* m_sUrlImage;
};

#endif;