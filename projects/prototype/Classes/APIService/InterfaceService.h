#ifndef __INTERFACE_SERVICE_H__
#define __INTERFACE_SERVICE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

class InterfaceService
{
public:
	virtual void resultHttpRequestCompleted(cs::JsonDictionary* pJsonDict, std::string sKey) {};
};
#endif