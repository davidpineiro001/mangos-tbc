#include "AuctionHouseMgr.h"
#include "Globals/ObjectMgr.h"
// ...existing code...

 // ...existing code...
#include "Entities/Item.h"
#include "Entities/Player.h"
#include "Database/DatabaseEnv.h"
#include "Log/Log.h"
#include "World/World.h"

void SeedItemsWithRandomProperties()
{
    uint32 auctionOwnerLowGuid = 11; // AH bot character GUID LOW


    std::unique_ptr<QueryResult> result = WorldDatabase.Query(
        "SELECT itemEntry, count, stackable, maxDurability, charges, flags, "
        "       auctionHouse, auctionDuration, startBid, buyout "
        "FROM ah_seed_table"
    );

    if (!result)
    {
        sLog.outString("SeedItemsWithRandomProperties: no seed data found");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 entry         = fields[0].GetUInt32();
        uint32 count         = fields[1].GetUInt32();
        uint32 stackable     = fields[2].GetUInt32();
        uint32 maxDurability = fields[3].GetUInt32();
        std::string charges  = fields[4].GetString();
        uint32 flags         = fields[5].GetUInt32();
        uint32 auctionHouse  = fields[6].GetUInt32();
        uint32 auctionDuration = fields[7].GetUInt32();
        uint32 startBid      = fields[8].GetUInt32();
        uint32 buyout        = fields[9].GetUInt32();

        Item* item = Item::CreateItem(entry, count, nullptr);
        if (!item)
        {
            sLog.outError("Failed to create item instance for entry %u", entry);
            continue;
        }

        item->SetUInt32Value(ITEM_FIELD_FLAGS, flags);
        item->SetUInt32Value(ITEM_FIELD_DURABILITY, maxDurability);
        item->SetState(ITEM_NEW, nullptr);

        if (!charges.empty())
        {
            // Optional: parse and set charges if needed
        }

        item->SaveToDB();

        // Create auction entry
        AuctionEntry* auction = new AuctionEntry();
        auction->Id = sObjectMgr.GenerateAuctionID();
        auction->auctionHouseEntry = sAuctionHouseStore.LookupEntry(auctionHouse);
        auction->itemGuidLow = item->GetGUIDLow();
        auction->itemTemplate = entry;
        auction->itemCount = count;
        auction->owner = auctionOwnerLowGuid; // server-owned
        auction->startbid = startBid;
        auction->buyout = buyout;
        auction->bid = 0;
        auction->expireTime = time(nullptr) + auctionDuration;

        auction->SaveToDB();
        sAuctionMgr.AddAuction(auction);

        sLog.outString("SeedItemsWithRandomProperties: seeded entry %u guid %u", entry, item->GetGUIDLow());

    } while (result->NextRow());

   sAuctionMgr.LoadAuctionItems();


    sLog.outString("SeedItemsWithRandomProperties: completed");
}
