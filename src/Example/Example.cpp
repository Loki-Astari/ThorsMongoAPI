#include "ThorsMongo/ThorsMongo.h"
#include "ThorSerialize/Traits.h"
#include <iostream>
#include <string>

// The structure I want to store.
// C++ class declarations.
class Address
{
    // To give the serializer accesses to private data.
    friend class ThorsAnvil::Serialize::Traits<Address>;

    std::string                 street1;
    std::string                 street2;
    std::string                 city;
    std::string                 country;
    std::string                 postCode;

    public:
        Address()
        {}
        Address(std::string const& street1, std::string const& street2, std::string const& city, std::string const& country, std::string const& postCode)
            : street1(street1)
            , street2(street2)
            , city(city)
            , country(country)
            , postCode(postCode)
        {}
};
class Person
{
    // To give the serializer accesses to private data.
    friend class ThorsAnvil::Serialize::Traits<Person>;

    std::string                 name;
    std::uint32_t               age;
    std::vector<std::string>    alergies;
    Address                     address;

    public:
        Person()
        {}
        Person(std::string const& name, std::uint32_t age, Address const& address)
            : name(name)
            , age(age)
            , address(address)
        {}
};

// Declare the types serializable using ThorsSerialize
ThorsAnvil_MakeTrait(Address, street1, street2, city, country, postCode);
ThorsAnvil_MakeTrait(Person, name, age, alergies, address);
ThorsMongo_CreateFieldAccess(Person, name);
ThorsMongo_CreateFieldAccess(Person, age);
using FindEqName = ThorsMongo_FilterFromAccess(Eq, Person, name);


void addPeopleToMongo(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::vector<Person> const& people)
{
    auto result = mongo["test"]["People"].insert(people);
    if (result)
    {
        // Data was inserted successfully
        std::cout << "Inserted ID:\n";
        for (auto const& id: result.inserted)
        {
            std::cout << "\t" << id << "\n";
        }
    }
    else
    {
        std::cout << "Error: " << result << "\n";
    }
}

void findPeopleInMongoByName(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::string const& name)
{
    // Retuns a C++ range of all the objects that match the query.
    // You can use a range in a loop or other algorithm to extract the values.
    auto range = mongo["test"]["People"].find<Person>(FindEqName{name});

    if (range)
    {
        // Print all the returned people
        for (auto const& person: range) {
            std::cout << ThorsAnvil::Serialize::jsonExporter(person);
        }
    }
    else
    {
        std::cout << "Error: " << range << "\n";
    }
}

void removePeopelFromMongoByName(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::string const& name, ThorsAnvil::DB::Mongo::Remove rem)
{
    using ThorsAnvil::DB::Mongo::Query;
    using ThorsAnvil::DB::Mongo::Remove;
    auto result = mongo["test"]["People"].remove(Query<FindEqName>{name, rem});
    if (result)
    {
        std::cerr << "Removed: " << result.n << "\n";
    }
    else
    {
        std::cerr << "Error: " << result << "\n";
    }
};

void removePeopelFromMongoOlderThan(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::uint32_t minAge, ThorsAnvil::DB::Mongo::Remove rem)
{
    using ThorsAnvil::DB::Mongo::Query;
    using ThorsAnvil::DB::Mongo::Remove;
    using FindGtAge = ThorsMongo_FilterFromAccess(Gt, Person, age);

    auto result = mongo["test"]["People"].remove(Query<FindGtAge>{minAge, rem});
    if (result)
    {
        std::cerr << "Removed: " << result.n << "\n";
    }
    else
    {
        std::cerr << "Error: " << result << "\n";
    }
};


void replacePerson(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::string const& name, Person const& p)
{

    auto result = mongo["test"]["People"].findAndReplaceOne(FindEqName{name}, p);
    if (result)
    {
        if (!result.value)
        {
            std::cerr << "No original value\n";
        }
        else
        {
            std::cerr << "Orginal Value: " << ThorsAnvil::Serialize::jsonExporter(*result.value) << "\n";
        }
    }
    else
    {
        std::cerr << "Error: " << result << "\n";
    }
}

void removePerson(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::string const& name)
{
    auto result = mongo["test"]["People"].findAndRemoveOne<Person>(FindEqName{name});
    if (result)
    {
        if (!result.value)
        {
            std::cerr << "No original value\n";
        }
        else
        {
            std::cerr << "Orginal Value: " << ThorsAnvil::Serialize::jsonExporter(*result.value) << "\n";
        }
    }
    else
    {
        std::cerr << "Error: " << result << "\n";
    }
}

void updateAddressCity(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::string const& name, std::uint32_t newAge)
{
    using ThorsAnvil::DB::Mongo::Query;
    using ThorsAnvil::DB::Mongo::Remove;
    using SetAge = ThorsMongo_UpdateFromAccess(Set, Person, age);

    std::cerr << "Checking: " <<  ThorsAnvil::Serialize::jsonExporter(SetAge{newAge}) << "\n";
    auto result = mongo["test"]["People"].findAndUpdateOne<Person>(FindEqName{name}, SetAge{newAge});
    if (result)
    {
        if (!result.value)
        {
            std::cerr << "No original value\n";
        }
        else
        {
            std::cerr << "Orginal Value: " << ThorsAnvil::Serialize::jsonExporter(*result.value) << "\n";
        }
    }
    else
    {
        std::cerr << "Error: " << result << "\n";
    }
};

int main()
{
    std::cout << "Mongo - HWorld\n";

    using ThorsAnvil::DB::Mongo::ThorsMongo;
    using ThorsAnvil::DB::Mongo::Compression;
    using ThorsAnvil::DB::Mongo::Auth::Client;
    ThorsMongo          mongo({"localhost"},
                              {"test", "testPassword", "test"},
                              Compression::Snappy | Compression::ZStd,
                              Client{"ExampleApp", {}}
                             );

    addPeopleToMongo(mongo, {
                                Person{"John",  32, {"32 Ridge Street", "Fullwright", "Nottingham", "UK", "AB34 5HG"}},
                                Person{"Mark",  25, {"11 Done  Street", "Fullwright", "Nottingham", "UK", "AB34 1HG"}},
                                Person{"Tom",   23, {"13 Ridge Street", "Fullwright", "Nottingham", "UK", "AB34 2HG"}},
                                Person{"Scott", 18, {"22 Ridge Street", "Fullwright", "Nottingham", "UK", "AB34 3HG"}},
                                Person{"Scott", 36, {"1 Dove Lane", "Fullwright", "Nottingham", "UK", "AB34 3HG"}},
                                Person{"Scott", 22, {"1 Dove Lane", "Fullwright", "Nottingham", "UK", "AB34 3HG"}},
                            });

    findPeopleInMongoByName(mongo, "Scott");
    removePeopelFromMongoByName(mongo, "Scott", ThorsAnvil::DB::Mongo::Remove::One);
    updateAddressCity(mongo, "Mark", 123);
    findPeopleInMongoByName(mongo, "Mark");
    replacePerson(mongo, "Tom", Person{"John", 22, {"Ace House", "Land of the good", "London", "UK", "LW1"}});
    removePerson(mongo, "Scott");
    removePeopelFromMongoOlderThan(mongo, 0, ThorsAnvil::DB::Mongo::Remove::All);
}
