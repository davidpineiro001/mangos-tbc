#include "ObjectMgr.h"
#include "Item.h"
#include "Player.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "DBCStores.h"
#include "World.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

void SeedItemsWithRandomProperties(const std::string& csvFile)
{
    Player* auctionOwner = sObjectMgr.GetPlayer(11); // AH bank
    if (!auctionOwner)
    {
        sLog.outError("SeedItemsWithRandomProperties: auction owner not found!");
        return;
    }

    std::ifstream file(csvFile);
    if (!file.is_open())
    {
        sLog.outError("SeedItemsWithRandomProperties: failed to open CSV file %s", csvFile.c_str());
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
                ItemTemplate const* proto = sObjectMgr.GetItemTemplate(entry);
                if (!proto)
                {
                    sLog.outError("Invalid item template %u", entry);
                    continue;
                }

                if (proto->RandomProperty <= 0)
                    continue; // skip items without random suffix

                uint32 suffixFactor = proto->ItemLevel > 0 ? proto->ItemLevel * 10 : 1;

                // Create an item instance for every suffix
                for (uint32 i = 1; i < sItemRandomPropertiesStore.GetNumRows(); ++i)
                {
                    ItemRandomPropertiesEntry const* rp = sItemRandomPropertiesStore.LookupEntry(i);
                    if (!rp || rp->RandomPropertyID != proto->RandomProperty)
                        continue;

                    Item* item = Item::CreateItem(entry, 1, auctionOwner);
                    if (!item)
                        continue;

                    item->SetItemRandomPropertiesId(rp->ID);
                    item->SetItemSuffixFactor(suffixFactor);
                    item->SetState(ITEM_NEW, auctionOwner);
                    item->SaveToDB();

                    sLog.outString("Created item %u (%s) suffix %d, guid %u",
                        entry, proto->Name1, rp->ID, item->GetGUIDLow());
                }
            }
            catch (...)
            {
                sLog.outError("Invalid CSV entry: %s", cell.c_str());
            }
        }
    }

    file.close();
    sLog.outString("SeedItemsWithRandomProperties: completed");
}
