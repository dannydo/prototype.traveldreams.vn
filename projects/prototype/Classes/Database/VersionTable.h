#ifndef __VERSION_TABLE_H__
#define __VERSION_TABLE_H__

#include "cocos2d.h"

struct VersionInfo
{
	int iVersionId;
	int iVersionSync;
	int iVersionDatabase;

	VersionInfo()
	{
		iVersionId = 0;
		iVersionSync = 1;
		iVersionDatabase = 1;
	}
};

class VersionTable
{
public:
	static VersionTable* getInstance();
	VersionTable();
	bool init();
	static void releaseInstance();

	inline VersionInfo& getVersionInfo() { return m_VersionInfo; };
	bool updateVersion(const VersionInfo& versionInfo);
	void refreshVersionInfo();

private:
	void fetchVersion();

	static VersionTable* m_VersionTable;
	VersionInfo m_VersionInfo;
};
#endif