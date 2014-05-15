#ifndef __CS_PACKAGE_TABLE_H__
#define __CS_PACKAGE_TABLE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

struct CSPackageInfo
{
	std::string sPackageId;
	std::string sPackageName;
	int iStage;
	std::string sPackageCode;
	std::string sCreatedBy;
	int iTotalWord;
	int iTotalWordUnlock;

	CSPackageInfo()
	{
		sPackageName = "";
		sPackageId = "";
		iStage = 0;
		sPackageCode = "";
		sCreatedBy = "";
		iTotalWord = 0;
		iTotalWordUnlock = 0;
	}
};

class CSPackageTable
{
public:
	static void getAllCSPackages(std::vector<CSPackageInfo>& csPackages);
	static CSPackageInfo getCSPackageInfo(const std::string& sPackageId);
	static bool updateCSPackage(const CSPackageInfo& csPackageInfo);
	static bool updateTrackingInfoOfCSPackage(const CSPackageInfo& csPackageInfo);
private:
};
#endif