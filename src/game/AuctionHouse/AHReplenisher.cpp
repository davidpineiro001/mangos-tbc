#include "ObjectMgr.h"
#include "Item.h"
#include "Player.h"
#include "Log.h"
#include "DBCStores.h"
#include "DatabaseEnv.h"

static uint32 ComputeSuffixFactor(ItemTemplate const* proto)
{
    return proto->ItemLevel > 0 ? proto->ItemLevel * 10 : 1;
}

void SeedItemsWithRandomProperties()
{
    Player* auctionOwner = sObjectMgr.GetPlayer(11); // AH bank / dummy player
    if (!auctionOwner)
        return;

    // Example list of item entries
    std::vector<uint32> weaponEntries = {
        4562, 8178, 727, 2140, 4561
    };

    for (uint32 entry : weaponEntries)
    {
        ItemTemplate const* proto = sObjectMgr.GetItemTemplate(entry);
        if (!proto || proto->RandomProperty <= 0)
            continue;

        uint32 suffixFactor = ComputeSuffixFactor(proto);

        // Iterate all suffixes that belong to this RandomProperty group
        for (uint32 i = 1; i < sItemRandomPropertiesStore.GetNumRows(); ++i)
        {
            ItemRandomPropertiesEntry const* rp =
                sItemRandomPropertiesStore.LookupEntry(i);

            if (!rp || rp->RandomPropertyID != proto->RandomProperty)
                continue;

            // Create the item instance
            Item* item = Item::CreateItem(entry, 1, auctionOwner);
            if (!item)
                continue;

            item->SetItemRandomPropertiesId(rp->ID);
            item->
            item->SetItemSuffixFactor(suffixFactor);
            item->SetState(ITEM_NEW, auctionOwner);
            item->SaveToDB();

            sLog.outString(
                "Seeded item %u (%s) with random suffix %d",
                entry, proto->Name1, rp->ID
            );
        }
    }

    sLog.outString("Item seeding complete.");
}
