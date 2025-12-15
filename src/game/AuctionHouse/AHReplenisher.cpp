#include "AuctionHouseMgr.h"
#include "ObjectMgr.h"
#include "Item.h"
#include "Player.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "DBCStores.h"
#include "World.h"

static uint32 ComputeSuffixFactor(ItemTemplate const* proto)
{
    // cMaNGOS standard scaling
    return proto->ItemLevel > 0 ? proto->ItemLevel * 10 : 1;
}

void PopulateAuctionHouseFromDB()
{
    AuctionHouseEntry const* house = sAuctionHouseStore.LookupEntry(2); // Neutral AH
    if (!house)
        return;

    QueryResult* result = WorldDatabase.Query
    (
                "SELECT a.entry, "
        "       a.SellPrice * "
        "           CASE a.Quality "
        "                WHEN 2 THEN CAST(1.0 AS DECIMAL(10,2)) "
        "                WHEN 3 THEN CAST(1.15 AS DECIMAL(10,2)) "
        "                ELSE CAST(1.0 AS DECIMAL(10,2)) "
        "           END AS start_price, "
        "       a.SellPrice * "
        "           CASE a.Quality "
        "                WHEN 2 THEN CAST(2.0 AS DECIMAL(10,2)) "
        "                WHEN 3 THEN CAST(2.75 AS DECIMAL(10,2)) "
        "                ELSE CAST(2.0 AS DECIMAL(10,2)) "
        "           END AS buyout_price, "
        "       a.flags, "
        "       a.MaxDurability, "
        "       a.RandomProperty "
        "FROM item_template a "
        "WHERE entry IN (4562, 8178, 727, 2140, 4561, 3192, 2075, 4566, 4564, 2632, 4569, 4570, 2073, 15210, "
        "15268, 4567, 2079, 4571, 31268, 31269, 2078, 31270, 3196, 3195, 15222, 4575, 3184, 15248, 3193, 4568, "
        "15211, 789, 3199, 790, 3198, 15223, 15269, 3740, 15259, 15224, 15230, 15249, 15241, 2072, 15212, 3201, "
        "3206, 2819, 2077, 3210, 15242, 3186, 15231, 865, 15250, 15225, 15232, 3185, 2080, 3197, 15213, 15243, "
        "1990, 15226, 16792, 863, 864, 15260, 1207, 15233, 15234, 15214, 15261, 5215, 1625, 4087, 866, 1640, "
        "15244, 8194, 15251, 8196, 3187, 1613, 3430, 5216, 15215, 15287, 15270, 1994, 15262, 8199, 4088, 1608, "
        "15235, 3208, 4089, 15227, 15252, 1639, 15263, 15245, 15323, 15291, 15279, 15216, 15253, 15236, 15274, "
        "12527, 15228, 15294, 15280, 15254, 15275, 15217, 15295, 15237, 15229, 15264, 15271, 15324, 15218, 15255, "
        "15281, 15265, 15238, 15276, 15256, 15219, 15272, 15282, 15325, 15278, 15266, 15288, 15239, 25310, 15257, "
        "25184, 15246, 25100, 25156, 15220, 25128, 25212, 25254, 25282, 25324, 25170, 25296, 25114, 25198, 25226, "
        "25240, 28531, 25142, 25268, 25311, 25325, 15273, 25241, 25143, 25269, 25101, 25185, 25171, 15267, 25115, "
        "15296, 25157, 25227, 28532, 25129, 25199, 25213, 25255, 25283, 25297, 25130, 25200, 25144, 25228, 25270, "
        "25326, 25172, 25186, 25214, 25298, 15283, 15247, 25242, 25312, 15240, 25158, 25256, 25284, 25102, 25116, "
        "28533, 25173, 25285, 25131, 25187, 15221, 25327, 25145, 15289, 25103, 25201, 25215, 25299, 19921, 25117, "
        "25159, 25271, 25313, 15258, 25229, 25257, 25243, 28534, 25188, 25160, 25104, 25146, 25314, 25132, 25202, "
        "25174, 25216, 25230, 25244, 25286, 25328, 28535, 25118, 25258, 25272, 25300, 25147, 25329, 25105, 25189, "
        "25259, 25175, 25119, 25161, 25315, 25273, 25301, 25203, 25231, 25133, 25217, 25245, 25287, 28536, 25274, "
        "25316, 25232, 25288, 25302, 25204, 25106, 25260, 25162, 25190, 25246, 25120, 25134, 25148, 25176, 25218, "
        "25330, 28537, 25107, 25135, 25289, 25303, 25233, 25149, 25191, 28538, 25247, 25121, 25219, 25261, 25275, "
        "25317, 25163, 25177, 25205, 25331, 25192, 25234, 25206, 25318, 25108, 25304, 25178, 25262, 25276, 25290, "
        "25122, 25136, 25150, 25164, 25220, 25248, 25332, 28539, 25193, 25221, 25165, 25179, 25249, 25109, 25207, "
        "25235, 25291, 25151, 25123, 25137, 25263, 25305, 28540, 25277, 25319, 25333, 25166, 25320, 25236, 25208, "
        "25110, 25334, 25194, 25250, 25264, 25138, 25292, 25152, 25180, 25306, 25124, 25222, 25278, 28541, 25111, "
        "25167, 25293, 25237, 25125, 25223, 28542, 25181, 25321, 25279, 25335, 25139, 25153, 25195, 25209, 25251, "
        "25307, 25265, 25238, 25322, 25294, 25308, 25224, 25266, 25168, 25196, 25336, 25126, 25140, 28543, 25154, "
        "25182, 25210, 25280, 25112, 25252, 25295, 25141, 25169, 25127, 25197, 25309, 25113, 25253, 25323, 25211, "
        "25337, 25281, 25183, 28544, 25225, 25239, 25267, 25155);";
        
    );

    if (!result)
    {
        sLog.outString("AHSeeder: no seed data");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();
        float startprice = fields[1].GetFloat();
        float buyoutprice = fields[2].GetFloat();
        uint32 flags = fields[3].GetUInt32(); // Unused
        uint32 maxDurability = fields[4].GetUInt32(); // Unused
        uint32 randomProperty = fields[5].GetUInt32(); // Unused

        ItemTemplate const* proto = sObjectMgr.GetItemTemplate(entry);
        if (!proto || proto->RandomProperty <= 0)
            continue;

        uint32 suffixFactor = ComputeSuffixFactor(proto);
        uint32 basePrice = startprice;
        uint32 buyout = buyoutprice;

        // Iterate all suffixes that belong to this RandomProperty group
        for (uint32 i = 1; i < sItemRandomPropertiesStore.GetNumRows(); ++i)
        {
            ItemRandomPropertiesEntry const* rp =
                sItemRandomPropertiesStore.LookupEntry(i);

            if (!rp || rp->RandomPropertyID != proto->RandomProperty)
                continue;

            Item* item = Item::CreateItem(entry, 1, nullptr);
            if (!item)
                continue;

            item->SetItemRandomPropertiesId(rp->ID);
            item->SetItemSuffixFactor(suffixFactor);
            item->SetState(ITEM_NEW, nullptr);
            item->SaveToDB();

            AuctionEntry* auction = new AuctionEntry();
            auction->Id = sObjectMgr.GenerateAuctionID();
            auction->itemGuidLow = item->GetGUIDLow();
            auction->itemTemplate = entry;
            auction->itemCount = 1;
            auction->owner = 0; // server-owned
            auction->startbid = startprice;
            auction->bid = 0;
            auction->buyout = buyoutprice;
            auction->itemdurability = maxDurability;
            auction->expireTime = time(nullptr) + 12 * HOUR;
            auction->auctionHouseEntry = 7;

            auction->SaveToDB();
            sAuctionMgr.AddAuction(auction);

            sLog.outString(
                "AHSeeder: %u (%s) suffix %d",
                entry, proto->Name1, rp->ID
            );
        }

    } while (result->NextRow());

    delete result;

    sAuctionMgr.LoadAuctionItems();
    sAuctionMgr.LoadAuctions();

    sLog.outString("AHSeeder: completed");
}
