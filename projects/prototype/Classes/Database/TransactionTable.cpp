#include "TransactionTable.h"
#include "InitDatabase.h"
#include "VersionTable.h"
#include "FunctionCommon.h"

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
	sql.appendWithFormat(" %u,", getTimeLocalCurrent());
	sql.appendWithFormat(" %d,", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);
	sql.appendWithFormat(" '%s')", transactionInfo.sType.c_str());

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	return true;
}

std::string	TransactionTable::syncGetTransactions()
{
	char **re;
	int nRow, nColumn;
		
	String sql = "select * from Transactions where Version>";
	sql.appendWithFormat("%d ", VersionTable::getInstance()->getVersionInfo().iVersionSync);
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	String sJsonData = "\"Transactions\":[";
	for (int iRow=1; iRow<=nRow; iRow++)
	{
		sJsonData.append("{");
		sJsonData.appendWithFormat("\"PowerUpId\": \"%s\",", re[iRow*nColumn+1]);
		sJsonData.appendWithFormat("\"Quantity\": %s,", re[iRow*nColumn+2]);
		sJsonData.appendWithFormat("\"TotalAmount\": %s,", re[iRow*nColumn+3]);
		sJsonData.appendWithFormat("\"DateTime\": %s,", re[iRow*nColumn+4]);
		sJsonData.appendWithFormat("\"Version\": %s,", re[iRow*nColumn+5]);
		sJsonData.appendWithFormat("\"Type\": %s", re[iRow*nColumn+6]);

		if (iRow == nRow)
			sJsonData.append("}");
		else
			sJsonData.append("},");
	}
	sJsonData.append("]");
	sqlite3_free_table(re);

	return sJsonData.getCString();
}