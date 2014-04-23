#ifndef __CS_PACKAGE_TABLE_H__
#define __CS_PACKAGE_TABLE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

struct CSPackageInfo
{
	std::string sPackageId;
	std::string sPackageName;

	CSPackageInfo()
	{
		sPackageName = "";
		sPackageId = "";
	}
};

class CSPackageTable
{
public:
	static CSPackageTable* getInstance();
	bool init();
	CSPackageTable();
	static void releaseInstance();

	std::vector<CSPackageInfo>& getAllCSPackages() { return m_CSPackages; };
	CSPackageInfo getCSPackageInfo(const std::string& sPackageId);

	bool updateCSPackage(const CSPackageInfo& csPackageInfo);

private:
	void fetchCSPackages();

	static CSPackageTable* m_CSPackageTable;
	std::vector<CSPackageInfo> m_CSPackages;
};
#endif