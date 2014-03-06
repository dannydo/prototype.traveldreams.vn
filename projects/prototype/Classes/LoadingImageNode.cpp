#include "LoadingImageNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

LoadingImagetNode::~LoadingImagetNode()
{
	
}

LoadingImagetNode* LoadingImagetNode::createLayout(const char* sUrlImage)
{
	LoadingImagetNode* pLoadingImageNode = new LoadingImagetNode();
	pLoadingImageNode->m_sUrlImage = sUrlImage;
	if(pLoadingImageNode->init())
	{
		pLoadingImageNode->autorelease();
		return pLoadingImageNode;
	}

	CC_SAFE_DELETE(pLoadingImageNode);
	return NULL;
}

bool LoadingImagetNode::init()
{
	if(!Node::init())
	{
		return false;
	}

	HttpRequest* m_pRequest = new HttpRequest();
	m_pRequest->setUrl(m_sUrlImage);
	m_pRequest->setRequestType(HttpRequest::Type::GET);
	m_pRequest->setTag("UserInfo");

	m_pRequest->setResponseCallback(this, httpresponse_selector(LoadingImagetNode::onHttpRequestCompleted));
	HttpClient::getInstance()->send(m_pRequest);
	m_pRequest->release();
	m_pRequest = NULL;

	return true;
}

void LoadingImagetNode::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
	std::string sKey = "";
	String strData = "";

	if (response)
    {
		if (response->isSucceed()) 
		{
			try
			{
				std::vector<char> *buffer = response->getResponseData();
				Image* image = new Image();
				void* pTemp = &buffer->front();
				image->initWithImageData((unsigned char*)pTemp, buffer->size()); 

				//In my project, I need the image to be shown, and not saved, so I didn't use 'saveToFile', but just create a Sprite
				Texture2D* texture = new Texture2D();
				texture->initWithImage(image);
				Sprite* pImageSprite = Sprite::createWithTexture(texture); 
				this->addChild(pImageSprite);
			}
			catch (exception e)
			{
			}
			
		}
		else
		{
			CCLOG("response failed");
			CCLOG("error buffer: %s", response->getErrorBuffer());
		}
    }
	else
	{
		CCLOG("response null");
	}
}