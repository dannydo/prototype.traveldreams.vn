#include "TrackingTable.h"
#include "InitDatabase.h"
#include "FunctionCommon.h"
#include "UserTable.h"
#include "CSWordTable.h"
#include "CSPackageTable.h"

USING_NS_CC; 

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "Social\FacebookManager.h"
using namespace cocos2d::plugin;
#endif

TrackingTable* TrackingTable::m_TrackingTable = NULL;

TrackingTable::TrackingTable()
{

}

void TrackingTable::releaseInstance()
{
	if (m_TrackingTable == NULL)
	{
		delete m_TrackingTable;
	}
	m_TrackingTable = NULL;	
}


TrackingTable* TrackingTable::getInstance()
{
	if (m_TrackingTable == NULL) {
		m_TrackingTable = new TrackingTable();
		m_TrackingTable->init();
	}

	return m_TrackingTable;
}

bool TrackingTable::init()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	FacebookManager::getInstance()->loadPlugin();
#endif

	return true;
}

bool TrackingTable::insertTracking(const TrackingInfo& trackingInfo)
{
	String sql = "insert into Trackings (Device,Player,StartTime,Event,Data) values(";
	sql.appendWithFormat(" '%s',", trackingInfo.sDeviceJson.c_str());
	sql.appendWithFormat(" '%s',", trackingInfo.sPlayerJson.c_str());
	sql.appendWithFormat(" %u,", trackingInfo.uStartTime);
	sql.appendWithFormat(" '%s',", getStringEventTracking(trackingInfo.eEventTracking).c_str());
	sql.appendWithFormat(" '%s')", trackingInfo.sDataJson.c_str());

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	return true;
}

std::string	TrackingTable::genrateJsonTrackingPushToServer()
{
	char **re;
	int nRow, nColumn;
		
	String sql = "select * from Trackings order by TrackingId asc limit ";
	sql.appendWithFormat("%d", _LIMIT_PUSH_TRACKING_TO_SERVER_);
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	String sJsonData = "data=[";
	for (int iRow=1; iRow<=nRow; iRow++)
	{
		sJsonData.append("{");
		sJsonData.appendWithFormat("\"Device\":%s,", re[iRow*nColumn+1]);
		sJsonData.appendWithFormat("\"Player\":%s,", re[iRow*nColumn+2]);
		sJsonData.appendWithFormat("\"StartTime\":%s,", re[iRow*nColumn+3]);
		sJsonData.appendWithFormat("\"Event\":\"%s\",", re[iRow*nColumn+4]);
		sJsonData.appendWithFormat("\"Data\":%s", re[iRow*nColumn+5]);

		if (iRow == nRow)
			sJsonData.append("}");
		else
			sJsonData.append("},");
	}
	sJsonData.append("]");
	sqlite3_free_table(re);

	return sJsonData.getCString();
}

bool TrackingTable::deleteTrackingDonePushToServer()
{
	String sql = "delete  from Trackings where TrackingId in (select TrackingId from Trackings order by TrackingId asc limit ";
	sql.appendWithFormat("%d)", _LIMIT_PUSH_TRACKING_TO_SERVER_);
	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);

	if(iResult != SQLITE_OK)
		return false;

	return true;
}

std::string TrackingTable::getStringEventTracking(EventTracking_e eEventTracking)
{
	std::string sEventTracking = "";

	switch (eEventTracking)
	{
	case _ET_PLAY_LEVEL_:
		sEventTracking = "PLAY_LEVEL";
		break;
	case _ET_PLAY_ADVANCE_:
		sEventTracking = "PLAY_ADVANCE";
		break;
	case _ET_OPEN_GAME_:
		sEventTracking = "OPEN_GAME";
		break;
	default:
		break;
	}

	return sEventTracking;
}

bool TrackingTable::trackingOpenGame()
{
	TrackingInfo trackingInfo;
	trackingInfo.sDeviceJson = getDeviceJson();
	trackingInfo.sPlayerJson = getPlayerJson();
	trackingInfo.uStartTime = getTimeLocalCurrent();
	trackingInfo.eEventTracking = EventTracking_e::_ET_OPEN_GAME_;
	trackingInfo.sDataJson = "{}";

	return insertTracking(trackingInfo);
}

bool TrackingTable::trackingPlayLevelNormalMode(const unsigned long startTime, const int& iLevel, const std::string sChapterId, std::string sWordId, 
		const char* sStatusGame, const int& iMoveLeft, const int& iLeterLeft, const int& iScore, const int& iStar, 
		const int& iTotalMove, const int& iTotalBonusQuest, const int& iBonusQuestCompleted)
{
	TrackingInfo trackingInfo;
	trackingInfo.sDeviceJson = getDeviceJson();
	trackingInfo.sPlayerJson = getPlayerJson();
	trackingInfo.uStartTime = startTime;
	trackingInfo.eEventTracking = EventTracking_e::_ET_PLAY_LEVEL_;

	String sJsonData = "{";

	sJsonData.append("\"Level\":{");
	sJsonData.appendWithFormat("\"Level\":%d,", iLevel);
	sJsonData.appendWithFormat("\"ChapterId\":\"%s\",", sChapterId.c_str());
	sJsonData.appendWithFormat("\"WordId\":\"%s\",", sWordId.c_str());
	sJsonData.appendWithFormat("\"StatusGame\":\"%s\",", sStatusGame);
	sJsonData.appendWithFormat("\"MoveLeft\":%d,", iMoveLeft);
	sJsonData.appendWithFormat("\"LetterLeft\":%d,", iLeterLeft);
	sJsonData.appendWithFormat("\"Score\":%d,", iScore);
	sJsonData.appendWithFormat("\"Star\":%d,", iStar);
	sJsonData.appendWithFormat("\"TotalMove\":%d,", iTotalMove);
	sJsonData.appendWithFormat("\"TotalBonusQuest\":%d,", iTotalBonusQuest);
	sJsonData.appendWithFormat("\"BonusQuestCompleted\":%d", iBonusQuestCompleted);
	sJsonData.append("}");

	sJsonData.append("}");

	trackingInfo.sDataJson = sJsonData.getCString();

	return insertTracking(trackingInfo);
}

bool TrackingTable::trackingPlayAdvanceMode(const unsigned long startTime, const std::string& sPackageId, const int& iStage, const std::vector<std::string>& sCSWordId)
{
	TrackingInfo trackingInfo;
	trackingInfo.sDeviceJson = getDeviceJson();
	trackingInfo.sPlayerJson = getPlayerJson();
	trackingInfo.uStartTime = startTime;
	trackingInfo.eEventTracking = EventTracking_e::_ET_PLAY_ADVANCE_;

	String sJsonData = "{";

	sJsonData.append("\"Package\":{");
	sJsonData.appendWithFormat("\"PackageId\":\"%s\",", sPackageId.c_str());
	sJsonData.appendWithFormat("\"Stage\":%d,", iStage);
	
	sJsonData.append("\"WordList\":[");
	int iSize = sCSWordId.size();
	for (int iIndex = 0; iIndex < iSize; iIndex++)
	{
		if (iIndex == iSize-1)
			sJsonData.appendWithFormat("\"%s\"", sCSWordId[iIndex].c_str());
		else
			sJsonData.appendWithFormat("\"%s\",", sCSWordId[iIndex].c_str());
	}
	sJsonData.append("]");
	sJsonData.append("}");

	sJsonData.append("}");

	trackingInfo.sDataJson = sJsonData.getCString();

	return insertTracking(trackingInfo);
}

std::string TrackingTable::getDeviceJson()
{
	std::string sDeviceId = "windows";
	std::string sDeviceName = "PC";
	std::string sOSVersion = "windows 8";

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	sDeviceId = FacebookManager::getInstance()->getDeviceId();
	sDeviceName = FacebookManager::getInstance()->getDeviceName();
	sOSVersion = FacebookManager::getInstance()->getOSVersion();
#endif

	String sJson = "{";
	sJson.appendWithFormat("\"DeviceId\":\"%s\",", sDeviceId.c_str());
	sJson.appendWithFormat("\"DeviceName\":\"%s\",", sDeviceName.c_str());
	sJson.appendWithFormat("\"OSVersion\":\"%s\"", sOSVersion.c_str());
	sJson.append("}");

	return sJson.getCString();
}

std::string TrackingTable::getPlayerJson()
{
	UserInfo userInfo = UserTable::getInstance()->getUserInfo();
	String sJson = "{";

	if (userInfo.sUserIdentifier != "ohmyword")
		sJson.appendWithFormat("\"UserId\":\"%s\",", userInfo.sUserIdentifier.c_str());
	else
		sJson.appendWithFormat("\"UserId\":\"%s\",", "");

	sJson.appendWithFormat("\"UserToken\":\"%s\"", userInfo.sUserToken.c_str());
	sJson.append("}");

	return sJson.getCString();
}