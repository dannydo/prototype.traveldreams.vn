#ifndef __POWER_UP_TABLE_H__
#define __POWER_UP_TABLE_H__

#include "cocos2d.h"

struct PowerUpInfo
{
	std::string sPowerUpId;
	int iQuantity;
	int iVersion;
};

class PowerUpTable
{
public:
	static PowerUpTable* getInstance();
	PowerUpTable();
	bool init();
	static void releaseInstance();

	inline std::vector<PowerUpInfo> getAllPowerUps() { return m_PowerUps; };
	bool updatePowerUp(const PowerUpInfo& powerUpInfo);

	std::string	syncGetPoverUps();

private:
	void fetchAllPowerUp();

	static PowerUpTable* m_PowerUpTable;
	std::vector<PowerUpInfo> m_PowerUps;
};
#endif