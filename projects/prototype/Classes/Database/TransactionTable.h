#ifndef __TRANSACTION_TABLE_H__
#define __TRANSACTION_TABLE_H__

#include "cocos2d.h"

struct TransactionInfo
{
	int iTransactionId;
	std::string sPowerUpId;
	int iQuantity;
	int iTotalAmount;
	unsigned long uDateTime;
	int iVersion;
	std::string sType;
};

class TransactionTable
{
public:
	static TransactionTable* getInstance();
	TransactionTable();
	bool init();
	static void releaseInstance();

	bool insertTransaction(const TransactionInfo& transactionInfo);

	std::string	syncGetTransactions();

private:
	unsigned long getTimeLocalCurrent();

	static TransactionTable* m_TransactionTable;
};
#endif