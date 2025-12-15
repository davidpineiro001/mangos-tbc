#include "AuctionHouseMgr.h"
#include "AuctionHouseReplenisher.h"

#include "Log.h"
#include <fstream>
#include <sstream>
#include <vector>

void AuctionHouseMgr::UpdateReplenisher()
{
    static uint32 lastLog = 0;
    if (WorldTimer::getMSTimeDiffToNow(lastLog) < 60000)
        return;

    lastLog = WorldTimer::getMSTime();
    sLog.outString("AHReplenisher: tick");
}




void PopulateAuctionHouseFromCSV()
{
    Player* auctionOwner = sObjectMgr.GetPlayer(11); // AH owner ID
    if (!auctionOwner)
        return;

    std::ifstream file("weapon_ids.csv");
    if (!file.is_open())
    {
        sLog.outError("Failed to open weapon_ids.csv");
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ','))
        {
            try
            {
                uint32 entry = std::stoul(cell);
                Item* item = Item::CreateItem(entry, 1, auctionOwner);
                if (!item)
                    continue;

                auctionOwner->AddItem(item);
                sLog.outString("Created AH item %u with guid %u", entry, item->GetGUIDLow());
            }
            catch (...)
            {
                sLog.outError("Invalid entry in CSV: %s", cell.c_str());
            }
        }
    }

    file.close();
}
