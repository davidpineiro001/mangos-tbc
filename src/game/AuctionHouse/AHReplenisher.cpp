#include "AuctionHouseMgr.h"
#include "ObjectMgr.h"
#include "Item.h"
#include "Player.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "World.h"

void SeedItemsWithRandomProperties()
{
    Player* auctionOwner = sObjectMgr.GetPlayer(11); // AH bank
    if (!auctionOwner)
    {
        sLog.outError("SeedItemsWithRandomProperties: Auction owner not found");
        return;
    }

    QueryResult* result = WorldDatabase.Query(
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

        Item* item = Item::CreateItem(entry, count, auctionOwner);
        if (!item)
        {
            sLog.outError("Failed to create item instance for entry %u", entry);
            continue;
        }

        item->SetUInt32Value(ITEM_FIELD_FLAGS, flags);
        item->SetUInt32Value(ITEM_FIELD_DURABILITY, maxDurability);
        item->SetState(ITEM_NEW, auctionOwner);

        if (!charges.empty())
        {
            // Optional: parse and set charges if needed
        }

        item->SaveToDB();

        // Create auction entry
        AuctionEntry* auction = new AuctionEntry();
        auction->Id = sObjectMgr.GenerateAuctionID();
        auction->auctionHouseEntry = auctionHouse;
        auction->itemGuidLow = item->GetGUIDLow();
        auction->itemTemplate = entry;
        auction->itemCount = count;
        auction->owner = auctionOwner->GetGUIDLow(); // server-owned
        auction->startbid = startBid;
        auction->buyout = buyout;
        auction->bid = 0;
        auction->expireTime = time(nullptr) + auctionDuration;

        auction->SaveToDB();
        sAuctionMgr.AddAuction(auction);

        sLog.outString("SeedItemsWithRandomProperties: seeded entry %u guid %u", entry, item->GetGUIDLow());

    } while (result->NextRow());

    delete result;

    sAuctionMgr.LoadAuctionItems();
    sAuctionMgr.LoadAuctions();

    sLog.outString("SeedItemsWithRandomProperties: completed");
}
