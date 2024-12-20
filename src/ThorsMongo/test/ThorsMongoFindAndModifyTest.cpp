#include "gtest/gtest.h"

#include "ThorsMongo.h"
#include "test/Action.h"

using ThorsAnvil::DB::Mongo::FAModifyConfig;
using ThorsAnvil::DB::Mongo::FARemoveConfig;
using ThorsAnvil::DB::Mongo::Action::FindAndModify;
using ThorsAnvil::DB::Mongo::SortOrder;
using ThorsAnvil::DB::Mongo::Projection;
using ThorsAnvil::DB::Mongo::PrinterConfig;
using ThorsAnvil::DB::Mongo::WriteConcern;
using namespace ThorsAnvil::DB::Mongo::QueryOp;


TEST(ThorsMongoFindAndModifyTest, FAModifyConfigDefault)
{
    FAModifyConfig    config;

    EXPECT_FALSE(config.getSort().has_value());
    EXPECT_FALSE(config.getReturnNew().has_value());
    EXPECT_FALSE(config.getFields().has_value());
    EXPECT_FALSE(config.getUpsert().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getCollation().has_value());
    EXPECT_FALSE(config.getHint().has_value());
    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
}

TEST(ThorsMongoFindAndModifyTest, FAModifyConfigSetSort)
{
    FAModifyConfig    config;

    config.setSort({{"name", SortOrder::Ascending}, {"age", SortOrder::Descending}});
    EXPECT_TRUE(config.getSort().has_value());

    EXPECT_FALSE(config.getReturnNew().has_value());
    EXPECT_FALSE(config.getFields().has_value());
    EXPECT_FALSE(config.getUpsert().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getCollation().has_value());
    EXPECT_FALSE(config.getHint().has_value());
    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
}

TEST(ThorsMongoFindAndModifyTest, FAModifyConfigSetNew)
{
    FAModifyConfig    config;

    EXPECT_FALSE(config.getSort().has_value());
    config.setReturnNew(true);
    EXPECT_TRUE(config.getReturnNew().has_value());

    EXPECT_FALSE(config.getFields().has_value());
    EXPECT_FALSE(config.getUpsert().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getCollation().has_value());
    EXPECT_FALSE(config.getHint().has_value());
    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
}

TEST(ThorsMongoFindAndModifyTest, FAModifyConfigSetFields)
{
    FAModifyConfig    config;

    EXPECT_FALSE(config.getSort().has_value());
    EXPECT_FALSE(config.getReturnNew().has_value());
    Projection      projection = Projection::create<People>();
    config.setFields(projection);
    EXPECT_TRUE(config.getFields().has_value());

    EXPECT_FALSE(config.getUpsert().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getCollation().has_value());
    EXPECT_FALSE(config.getHint().has_value());
    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
}

TEST(ThorsMongoFindAndModifyTest, FAModifyConfigSetUpsert)
{
    FAModifyConfig    config;

    EXPECT_FALSE(config.getSort().has_value());
    EXPECT_FALSE(config.getReturnNew().has_value());
    EXPECT_FALSE(config.getFields().has_value());
    config.setUpsert(true);
    EXPECT_TRUE(config.getUpsert().has_value());

    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getCollation().has_value());
    EXPECT_FALSE(config.getHint().has_value());
    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
}

TEST(ThorsMongoFindAndModifyTest, FAModifyConfigSetBypassDocumentValidation)
{
    FAModifyConfig    config;

    EXPECT_FALSE(config.getSort().has_value());
    EXPECT_FALSE(config.getReturnNew().has_value());
    EXPECT_FALSE(config.getFields().has_value());
    EXPECT_FALSE(config.getUpsert().has_value());
    config.setBypassDocumentValidation(true);
    EXPECT_TRUE(config.getBypassDocumentValidation().has_value());

    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getCollation().has_value());
    EXPECT_FALSE(config.getHint().has_value());
    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
}

TEST(ThorsMongoFindAndModifyTest, FAModifyConfigSetWriteConcern)
{
    FAModifyConfig    config;

    EXPECT_FALSE(config.getSort().has_value());
    EXPECT_FALSE(config.getReturnNew().has_value());
    EXPECT_FALSE(config.getFields().has_value());
    EXPECT_FALSE(config.getUpsert().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    config.setWriteConcern(WriteConcern{});
    EXPECT_TRUE(config.getWriteConcern().has_value());

    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getCollation().has_value());
    EXPECT_FALSE(config.getHint().has_value());
    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
}

TEST(ThorsMongoFindAndModifyTest, FAModifyConfigSetMaxTimeMS)
{
    FAModifyConfig    config;

    EXPECT_FALSE(config.getSort().has_value());
    EXPECT_FALSE(config.getReturnNew().has_value());
    EXPECT_FALSE(config.getFields().has_value());
    EXPECT_FALSE(config.getUpsert().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    config.setMaxTimeMS(235);
    EXPECT_TRUE(config.getMaxTimeMS().has_value());

    EXPECT_FALSE(config.getCollation().has_value());
    EXPECT_FALSE(config.getHint().has_value());
    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
}

TEST(ThorsMongoFindAndModifyTest, FAModifyConfigSetCollation)
{
    FAModifyConfig    config;

    EXPECT_FALSE(config.getSort().has_value());
    EXPECT_FALSE(config.getReturnNew().has_value());
    EXPECT_FALSE(config.getFields().has_value());
    EXPECT_FALSE(config.getUpsert().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    config.setCollation({"USA", {}, {}, {}, {}, {}, {}, {}});
    EXPECT_TRUE(config.getCollation().has_value());

    EXPECT_FALSE(config.getHint().has_value());
    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
}

TEST(ThorsMongoFindAndModifyTest, FAModifyConfigSetHint)
{
    FAModifyConfig    config;

    EXPECT_FALSE(config.getSort().has_value());
    EXPECT_FALSE(config.getReturnNew().has_value());
    EXPECT_FALSE(config.getFields().has_value());
    EXPECT_FALSE(config.getUpsert().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getCollation().has_value());
    config.setHint("Bob");
    EXPECT_TRUE(config.getHint().has_value());

    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
}

TEST(ThorsMongoFindAndModifyTest, FAModifyConfigSetComment)
{
    FAModifyConfig    config;

    EXPECT_FALSE(config.getSort().has_value());
    EXPECT_FALSE(config.getReturnNew().has_value());
    EXPECT_FALSE(config.getFields().has_value());
    EXPECT_FALSE(config.getUpsert().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getCollation().has_value());
    EXPECT_FALSE(config.getHint().has_value());
    config.setComment("Comment");
    EXPECT_TRUE(config.getComment().has_value());

    EXPECT_FALSE(config.getLet().has_value());
}

TEST(ThorsMongoFindAndModifyTest, FAModifyConfigSetLet)
{
    FAModifyConfig    config;

    EXPECT_FALSE(config.getSort().has_value());
    EXPECT_FALSE(config.getReturnNew().has_value());
    EXPECT_FALSE(config.getFields().has_value());
    EXPECT_FALSE(config.getUpsert().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getCollation().has_value());
    EXPECT_FALSE(config.getHint().has_value());
    EXPECT_FALSE(config.getComment().has_value());
    config.setLet({{"name","1"}, {"Bob","2"}});
    EXPECT_TRUE(config.getLet().has_value());
}

TEST(ThorsMongoFindAndModifyTest, UpdateCurrentDateWithDate)
{
    using Search = NameField<Eq<std::string>>;
    using Update = CurrentDate<AgeField<SetDate>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update;
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0x87, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x87\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x32\x00\x00\x00"
                        "\x03" "$currentDate\x00"
                            "\x1F\x00\x00\x00"
                                "\x03" "age\x00"
                                    "\x15\x00\x00\x00"
                                        "\x02" "$type\x00" "\x05\x00\x00\x00" "date\x00"
                                        "\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdateIncrement)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Inc<AgeField<int>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, Update{3});

    EXPECT_EQ(0x6E, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x6E\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x19\x00\x00\x00"
                        "\x03" "$inc\x00"
                            "\x0E\x00\x00\x00"
                                "\x10" "age\x00" "\x03\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdateMin)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Min<AgeField<std::uint32_t>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, Update{3});

    EXPECT_EQ(0x6E, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x6E\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x19\x00\x00\x00"
                        "\x03" "$min\x00"
                            "\x0E\x00\x00\x00"
                                "\x10" "age\x00" "\x03\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdateMax)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Max<AgeField<std::uint32_t>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, Update{3});

    EXPECT_EQ(0x6E, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x6E\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x19\x00\x00\x00"
                        "\x03" "$max\x00"
                            "\x0E\x00\x00\x00"
                                "\x10" "age\x00" "\x03\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdateMul)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Mul<AgeField<std::uint32_t>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, Update{3});

    EXPECT_EQ(0x6E, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x6E\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x19\x00\x00\x00"
                        "\x03" "$mul\x00"
                            "\x0E\x00\x00\x00"
                                "\x10" "age\x00" "\x03\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdateRename)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Rename<AgeField<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, Update{"originalAge"});

    EXPECT_EQ(0x7D, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x7D\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x28\x00\x00\x00"
                        "\x03" "$rename\x00"
                            "\x1A\x00\x00\x00"
                                "\x02" "age\x00" "\x0C\x00\x00\x00" "originalAge\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdateSet)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Set<AgeField<std::uint32_t>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update{255};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0x6E, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x6E\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x19\x00\x00\x00"
                        "\x03" "$set\x00"
                            "\x0E\x00\x00\x00"
                                "\x10" "age\x00"    "\xff\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdateSetOnInsert)
{
    using Search = NameField<Eq<std::string>>;
    using Update = SetOnInsert<AgeField<std::uint32_t>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update{255};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0x76, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x76\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x21\x00\x00\x00"
                        "\x03" "$setOnInsert\x00"
                            "\x0E\x00\x00\x00"
                                "\x10" "age\x00"    "\xff\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdateUnSet)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Unset<AgeField<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update;
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0x71, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x71\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x1C\x00\x00\x00"
                        "\x03" "$unset\x00"
                            "\x0F\x00\x00\x00"
                                "\x02" "age\x00"    "\x01\x00\x00\x00" "\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}


TEST(ThorsMongoFindAndModifyTest, UpdateAddToSet)
{
    using Search = NameField<Eq<std::string>>;
    using Update = AddToSet<FoodField<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update{"Ham"};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0x78, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x78\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x23\x00\x00\x00"
                        "\x03" "$addToSet\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "food\x00"    "\x04\x00\x00\x00" "Ham\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdatePopFront)
{
    using Search = NameField<Eq<std::string>>;
    using Update = PopFront<FoodField<std::int32_t>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update;
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0x6F, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x6F\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x1A\x00\x00\x00"
                        "\x03" "$pop\x00"
                            "\x0F\x00\x00\x00"
                                "\x10" "food\x00"    "\xFF\xFF\xFF\xFF"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdatePopBack)
{
    using Search = NameField<Eq<std::string>>;
    using Update = PopBack<FoodField<std::int32_t>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update;
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0x6F, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x6F\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x1A\x00\x00\x00"
                        "\x03" "$pop\x00"
                            "\x0F\x00\x00\x00"
                                "\x10" "food\x00"    "\x01\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdatePull)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Pull<FoodField<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update{"Log"};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0x74, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x74\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x1F\x00\x00\x00"
                        "\x03" "$pull\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "food\x00"    "\x04\x00\x00\x00" "Log\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdatePullWithTest)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Pull<FoodField<Lt<std::string>>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update{"Tile"};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0x7F, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x7F\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x2A\x00\x00\x00"
                        "\x03" "$pull\x00"
                            "\x1E\x00\x00\x00"
                                "\x03" "food\x00"
                                    "\x13\x00\x00\x00"
                                        "\x02" "$lt\x00" "\x05\x00\x00\x00" "Tile\x00"
                                        "\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdatePush)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Push<FoodField<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update{"Honey"};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0x76, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x76\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x21\x00\x00\x00"
                        "\x03" "$push\x00"
                            "\x15\x00\x00\x00"
                                "\x02" "food\x00"    "\x06\x00\x00\x00" "Honey\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdatePullAll)
{
    using Search = NameField<Eq<std::string>>;
    using Update = PullAll<FoodField<std::vector<std::string>>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update{{"Honey", "Jam", "Toast"}};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0x99, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x99\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x44\x00\x00\x00"
                        "\x03" "$pullAll\x00"
                            "\x35\x00\x00\x00"
                                "\x04" "food\x00"
                                    "\x2A\x00\x00\x00"
                                        "\x02" "0\x00" "\x06\x00\x00\x00" "Honey\x00"
                                        "\x02" "1\x00" "\x04\x00\x00\x00" "Jam\x00"
                                        "\x02" "2\x00" "\x06\x00\x00\x00" "Toast\x00"
                                        "\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

