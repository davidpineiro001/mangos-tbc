#ifndef AUCTIONHOUSE_REPLENISHER_H
#define AUCTIONHOUSE_REPLENISHER_H

#include "Entities/Player.h" // For Player*, Item* references
#include "AuctionHouseMgr.h"
#include <cstdint>

// This is part of the AuctionHouseMgr class
class AHReplenisher
{
public:
    void UpdateReplenisher();
};

// Free function to populate AH from CSV
void SeedItemsWithRandomProperties();

#endif // AUCTIONHOUSE_REPLENISHER_H
