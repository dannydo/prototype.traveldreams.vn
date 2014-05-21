#include "PackageService.h"
#include "Constants.h"
#include "Database\TrackingTable.h"

USING_NS_CC; 
USING_NS_CC_EXT;

PackageService* PackageService::m_TrackingService = NULL;
HttpRequest* PackageService::m_pRequest = NULL;
HttpClient* PackageService::m_pClient = NULL;

void PackageService::releaseInstance()
{
	if (m_pRequest == NULL)
		delete m_pRequest;
	else
		m_pRequest->release();
	m_pRequest = NULL;

	// note: should note release singleton directly!!!!
	//if (m_pClient != NULL)
	//	m_pClient->release();
	m_pClient = NULL;

	if (m_TrackingService == NULL)
		delete m_TrackingService;
	m_TrackingService = NULL;
}

PackageService* PackageService::getInstance()
{
	if (m_TrackingService == NULL) {
		m_TrackingService = new PackageService();
		m_TrackingService->m_callBack = NULL;
	}
	return m_TrackingService;
}

PackageService::PackageService()
{
	//m_pRequest = new HttpRequest();
	m_pRequest = NULL;
	m_pClient = HttpClient::getInstance();	
}

PackageService::~PackageService()
{

}

void PackageService::getPackagePopular(const int& iStart, const int& iNumberRecords, const int& iTag)
{
	String strURL = _CONSTANT_VOCAB_URL_;
	strURL.append("packages/getTopDownload");
	strURL.appendWithFormat("/%d", iStart);
	strURL.appendWithFormat("/%d", iNumberRecords);

	m_pRequest = new HttpRequest();
	m_pRequest->setUrl(strURL.getCString());
	m_pRequest->setRequestType(HttpRequest::Type::GET);

	String sTag = "GetPackagePopular";
	sTag.appendWithFormat("_%d", iTag);
	m_pRequest->setTag(sTag.getCString());

	m_pRequest->setResponseCallback(this, httpresponse_selector(PackageService::onHttpRequestCompleted));
	m_pClient->send(m_pRequest);
	m_pRequest->release();
	m_pRequest = NULL;
}

void PackageService::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
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
			
			if ( buffer->size() > 0)
			{
				string strData(buffer->begin(), buffer->end());

				if (m_callBack != NULL)
				{
					cs::JsonDictionary *pJsonDict = new cs::JsonDictionary();
					pJsonDict->initWithDescription(strData.c_str());
					m_callBack->resultHttpRequestCompleted(pJsonDict, sKey);
				}
			}
		}
	}	
}

void PackageService::setInterfaceServiceCallBack(InterfaceService* callBack)
{
	m_callBack = callBack;
}

void PackageService::removeInterfaceServiceCallBack()
{
	m_callBack = NULL;
}