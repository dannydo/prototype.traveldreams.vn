#include "TrackingService.h"
#include "Constants.h"
#include "Database\TrackingTable.h"

USING_NS_CC; 
USING_NS_CC_EXT;

TrackingService* TrackingService::m_TrackingService = NULL;
HttpRequest* TrackingService::m_pRequest = NULL;
HttpClient* TrackingService::m_pClient = NULL;

void TrackingService::releaseInstance()
{
	if (m_pRequest == NULL)
		delete m_pRequest;
	else
		m_pRequest->release();
	m_pRequest = NULL;

	// note: should note release singleton directly!!!!
	//if (m_pClient != NULL)
		//m_pClient->release();
	m_pClient = NULL;

	if (m_TrackingService == NULL)
		delete m_TrackingService;
	m_TrackingService = NULL;
}

TrackingService* TrackingService::getInstance()
{
	if (m_TrackingService == NULL) {
		m_TrackingService = new TrackingService();
	}
	return m_TrackingService;
}

TrackingService::TrackingService()
{
	//m_pRequest = new HttpRequest();
	m_pRequest = NULL;
	m_pClient = HttpClient::getInstance();
	m_bIsFinishTracking = true;
}

TrackingService::~TrackingService()
{

}

void TrackingService::pushTrackingToServer()
{			
	if (m_bIsFinishTracking)
	{
		// Load data from database
		std::string sJsonData = TrackingTable::getInstance()->genrateJsonTrackingPushToServer();
		
		//CCLOG("Tracking %s", sJsonData.c_str());	  //can cause crash
		//CCLOG("%d", sJsonData.length());
	
		// Post data to server
		String strURL = _CONSTANT_URL_;
		strURL.append("api/tracking");
		m_pRequest = new HttpRequest();
		m_pRequest->setUrl(strURL.getCString());
		m_pRequest->setRequestType(HttpRequest::Type::POST);
		m_pRequest->setTag("Tracking");
		m_pRequest->setRequestData(sJsonData.c_str(), sJsonData.length());
		m_pRequest->setResponseCallback(this, httpresponse_selector(TrackingService::onHttpRequestCompleted));

		m_pClient->send(m_pRequest);
		m_pRequest->release();
		m_pRequest = NULL;

		m_bIsFinishTracking = false;
	}
}

void TrackingService::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{	
	std::string sKey = "";
	sKey.append(response->getHttpRequest()->getTag());
	std::vector<std::string> header = response->getHttpRequest()->getHeaders();
	bool bResult = true;

	if (response)
	{
		if (response->isSucceed()) 
		{
			std::vector<char> *buffer = response->getResponseData();
			
			string strData(buffer->begin(), buffer->end());

			cs::JsonDictionary *pJsonDict = new cs::JsonDictionary();
			pJsonDict->initWithDescription(strData.c_str());
			//CCLOG("Tracking %s", strData.getCString()); //can cause crash
			cs::JsonDictionary *pJsonData = pJsonDict->getSubDictionary("data");
			if(pJsonData != NULL)
			{
				if (pJsonData->getItemBoolvalue("result", false))
				{
					TrackingTable::getInstance()->deleteTrackingDonePushToServer();
				}
			}
		}
	}

	m_bIsFinishTracking = true;
}