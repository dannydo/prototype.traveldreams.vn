#include "PowerUpTable.h"
#include "InitDatabase.h"
#include "VersionTable.h"

USING_NS_CC; 

PowerUpTable* PowerUpTable::m_PowerUpTable = NULL;

PowerUpTable::PowerUpTable()
{

}

void PowerUpTable::releaseInstance()
{
	if (m_PowerUpTable == NULL)
	{
		delete m_PowerUpTable;
	}
	m_PowerUpTable = NULL;	
}


PowerUpTable* PowerUpTable::getInstance()
{
	if (m_PowerUpTable == NULL) {
		m_PowerUpTable = new PowerUpTable();
		m_PowerUpTable->init();
	}

	return m_PowerUpTable;
}

bool PowerUpTable::init()
{
	fetchAllPowerUp();
	return true;
}

void PowerUpTable::fetchAllPowerUp()
{
	char **re;
	int nRow, nColumn;
		
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), "select * from PowerUps", &re, &nRow, &nColumn,NULL);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		PowerUpInfo powerUpInfo;
		powerUpInfo.sPowerUpId = re[iRow*nColumn+0];
		powerUpInfo.iQuantity = int(strtod(re[iRow*nColumn+1], NULL));
		powerUpInfo.iVersion = int(strtod(re[iRow*nColumn+2], NULL));

		m_PowerUps.push_back(powerUpInfo);
	}

	sqlite3_free_table(re);
}

bool PowerUpTable::updatePowerUp(const PowerUpInfo& powerUpInfo)
{
	String sql = "update PowerUps Set";
	sql.appendWithFormat(" Quantity=%d,", powerUpInfo.iQuantity);
	sql.appendWithFormat(" Version=%d", VersionTable::getInstance()->getVersionInfo().iVersionId + 1);
	sql.appendWithFormat(" where PowerUpId='%s'", powerUpInfo.sPowerUpId.c_str());

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	for(int iIndex=0; iIndex<m_PowerUps.size(); iIndex++)
	{
		if(m_PowerUps[iIndex].sPowerUpId == powerUpInfo.sPowerUpId)
		{
			m_PowerUps[iIndex] = powerUpInfo;
		}
	}

	return true;
}