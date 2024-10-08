#include "gtest/gtest.h"

#include "MongoQuery.h"
#include "MongoUpdate.h"
#include "test/Action.h"

#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"
#include <sstream>

ThorsMongo_CreateFieldAccess(People, data);
ThorsMongo_CreateFieldAccess(People, address, city);
ThorsMongo_CreateFieldAccess(Family, mother, address, code);

TEST(MongoUpdateTest, CreateFilterOneLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using PushToPersonData = ThorsMongo_UpdateFromAccess(Push, People, data);

    s << ThorsAnvil::Serialize::jsonExporter(PushToPersonData{12}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ(R"({"$push":{"data":12}})", s.str());
}

TEST(MongoUpdateTest, CreateFilterTwoLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using SetPersonAddressCity = ThorsMongo_UpdateFromAccess(Set, People, address, city);

    s << ThorsAnvil::Serialize::jsonExporter(SetPersonAddressCity{"London"}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ(R"({"$set":{"address.city":"London"}})", s.str());
}

TEST(MongoUpdateTest, CreateFilterThreeLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using SetOnInsertFamilyPersonAddressCode = ThorsMongo_UpdateFromAccess(SetOnInsert, Family, mother, address, code);

    s << ThorsAnvil::Serialize::jsonExporter(SetOnInsertFamilyPersonAddressCode{12}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ(R"({"$setOnInsert":{"mother.address.code":12}})", s.str());
}
