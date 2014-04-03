#include "TransactionTable.h"
#include "InitDatabase.h"
#include "VersionTable.h"

USING_NS_CC; 

TransactionTable* TransactionTable::m_TransactionTable = NULL;

TransactionTable::TransactionTable()
{

}

void TransactionTable::releaseInstance()
{
	if (m_TransactionTable == NULL)
	{
		delete m_TransactionTable;
	}
	m_TransactionTable = NULL;	
}


TransactionTable* TransactionTable::getInstance()
{
	if (m_TransactionTable == NULL) {
		m_TransactionTable = new TransactionTable();
		m_TransactionTable->init();
	}

	return m_TransactionTable;
}

bool TransactionTable::init()
{
	return true;
}

bool TransactionTable::insertTransaction(const TransactionInfo& transactionInfo)
{
	String sql = "insert into Transactions (PowerUpId,Quantity,TotalAmount,DateTime,Version) values(";
	sql.appendWithFormat(" '%s',", transactionInfo.sPowerUpId.c_str());
	sql.appendWithFormat(" %d,", transactionInfo.iQuantity);
	sql.appendWithFormat(" %d,", transactionInfo.iTotalAmount);
	sql.appendWithFormat(" %u,", this->getTimeLocalCurrent());
	sql.appendWithFormat(" %d,)", VersionTable::getInstance()->getVersionInfo().iVersionId + 1);
	sql.appendWithFormat(" '%s'", transactionInfo.sType.c_str());

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	return true;
}

unsigned long TransactionTable::getTimeLocalCurrent()
{
	timeval now;
	gettimeofday(&now, NULL);
	unsigned long iCurrentTime = now.tv_sec + now.tv_usec/1000000 ; //seconds
	return iCurrentTime;
}