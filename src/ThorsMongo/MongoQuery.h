#ifndef THORSANVIL_DB_MONGO_MONGO_QUERY_H
#define THORSANVIL_DB_MONGO_MONGO_QUERY_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"
#include <iterator>

#define THORMONGO_BUILD_FIELD_NAME_(TC, TypeName, IG, FN)           #FN "."
#define LAST_THORMONGO_BUILD_FIELD_NAME_(TC, TypeName, IG, FN)      #FN
#define THORMONGO_BUILD_FIELD_NAME(TypeName, ...)                   REP_CMD_N(THORMONGO_BUILD_FIELD_NAME_, 00, TypeName, Ignore, __VA_ARGS__)

#define THORMONGO_BULD_TYPE_INFO_(TC, TypeName, Last, FN)           using Type ## FN = typename ThorsAnvil::Serialize::Traits<Type ## Last>::THOR_BUILD_NAME(Type, FN);
#define LAST_THORMONGO_BULD_TYPE_INFO_(TC, TypeName, Last, FN)      using TypeBase = typename ThorsAnvil::Serialize::Traits<Type ## Last>::THOR_BUILD_NAME(Type, FN)
#define THORMONGO_BULD_TYPE_INFO(TypeName, First, TC, ...)          REP_CMD_N(THORMONGO_BULD_TYPE_INFO_, TC, TypeName, First, __VA_ARGS__)

#define THORMONGO_BUILD_USE_FIRST(value, ...)                       value
#define THORMONGO_BUILD_USE_FIRST_Q(value, ...)                     #value

#define THORMONGO_NAME_(TC, TypeName, NS, FN)                       NS ::
#define LAST_THORMONGO_NAME_(TC, TypeName, NS, FN)                  NS :: FN
#define THORMONGO_NAME(TypeName, TC, ...)                           REP_CMD_N(THORMONGO_NAME_, TC, TypeName, TypeName, __VA_ARGS__)

/*
 * Creating a filter.
 *      Automate building filters.
 */
#define ThorsMongo_CreateFieldAccess(TypeName, ...)                                                 \
namespace ThorsAnvil::FieldAccess:: THORMONGO_NAME(TypeName, 0, __VA_ARGS__) {                      \
    template<typename T>                                                                            \
    struct Access                                                                                   \
    {                                                                                               \
        using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;                                    \
        Access(CType init) : THORMONGO_BUILD_USE_FIRST(__VA_ARGS__, 1)(std::move(init)) {}          \
        T          THORMONGO_BUILD_USE_FIRST(__VA_ARGS__, 1);                                       \
    };                                                                                              \
    using TypeFirst = ::TypeName;                                                                   \
    THORMONGO_BULD_TYPE_INFO(TypeName, First, 00, __VA_ARGS__);                                     \
    using TypeOperat = ThorsAnvil::DB::Mongo::RemoveOptional<TypeBase>;                             \
}                                                                                                   \
ThorsAnvil_Template_MakeOverride(1,                                                                 \
    ThorsAnvil::FieldAccess:: THORMONGO_NAME(TypeName, 0, __VA_ARGS__) ::Access,                    \
    {THORMONGO_BUILD_USE_FIRST_Q(__VA_ARGS__, 1), THORMONGO_BUILD_FIELD_NAME(TypeName, __VA_ARGS__)}\
);                                                                                                  \
ThorsAnvil_Template_MakeTrait(1,                                                                    \
    ThorsAnvil::FieldAccess:: THORMONGO_NAME(TypeName, 0, __VA_ARGS__) ::Access,                    \
    THORMONGO_BUILD_USE_FIRST(__VA_ARGS__, 1)                                                       \
);


#define ThorsMongo_FilterFromAccess(Name, Operator, TypeName, ...)                                  \
using Name = ThorsAnvil::FieldAccess:: THORMONGO_NAME(TypeName, 0, __VA_ARGS__) ::Access<           \
                ThorsAnvil::DB::Mongo::QueryOp::Operator<                                           \
                    typename ThorsAnvil::FieldAccess:: THORMONGO_NAME(TypeName, 0, __VA_ARGS__) ::TypeOperat \
                >                                                                                   \
             >

#define ThorsMongo_UpdateFromAccessNorm(Name, Action, TypeName, ...)                                \
using Name = ThorsAnvil::DB::Mongo::QueryOp:: Action <                                              \
                ThorsAnvil::FieldAccess:: THORMONGO_NAME(TypeName, 0, __VA_ARGS__) ::Access<        \
                    typename ThorsAnvil::FieldAccess:: THORMONGO_NAME(TypeName, 0, __VA_ARGS__) ::TypeOperat \
                >                                                                                   \
             >

#define ThorsMongo_UpdateFromAccessValue(Name, Action, TypeName, ...)                               \
using Name = ThorsAnvil::DB::Mongo::QueryOp:: Action <                                              \
                ThorsAnvil::FieldAccess:: THORMONGO_NAME(TypeName, 0, __VA_ARGS__) ::Access<        \
                    typename ThorsAnvil::FieldAccess:: THORMONGO_NAME(TypeName, 0, __VA_ARGS__)::TypeOperat::value_type \
                >                                                                                   \
             >


#define ThorsMongo_CreateFilter(Name, Operator, TypeName, ...)                                      \
    ThorsMongo_CreateFieldAccess(TypeName, __VA_ARGS__);                                            \
    ThorsMongo_FilterFromAccess(Name, Operator, TypeName, __VA_ARGS__)


#define ThorsMongo_CreateUpdateNorm(Name, Action, TypeName, ...)                                    \
    ThorsMongo_CreateFieldAccess(TypeName, __VA_ARGS__);                                            \
    ThorsMongo_UpdateFromAccessNorm(Name, Action, TypeName, __VA_ARGS__)

#define ThorsMongo_CreateUpdateValue(Name, Action, TypeName, ...)                                   \
    ThorsMongo_CreateFieldAccess(TypeName, __VA_ARGS__);                                            \
    ThorsMongo_UpdateFromAccessValue(Name, Action, TypeName, __VA_ARGS__)


namespace ThorsAnvil::DB::Mongo
{

namespace Concept
{
template<typename T>
concept IsQuery = requires {typename T::Query; };
}

enum class Remove {All, One};

template<typename T, typename Hint = std::string>
struct Query
{
    using CType = ConstructorType<T>;
    Query(CType init, Remove r = Remove::All, std::optional<Collation> collation = {}, std::optional<Hint> hint = {})
        : q(std::move(init))
        , limit(r == Remove::One ? 1 : 0)
        , collation(collation)
        , hint(hint)
    {}
    T                               q;
    std::uint32_t                   limit;
    std::optional<Collation>        collation;
    std::optional<Hint>             hint;
};

namespace QueryOp
{

/*** Extractos ***/
template<typename T>
struct NormalExtractor { using CType = T;};
template<typename T>
struct ValueExtractor { using CType = typename T::value_type;};


enum class BsonType {   Double = 1,     String = 2,             Object = 3,         Array = 4,
                        BinaryData = 5, ObjectId = 7,           Boolean = 8,        Date = 9,
                        Null = 10,      RegularExpression = 11, JavaScript = 13,    Int32 = 16,
                        Timestamp = 17, Int64 = 18,             Decimal128 = 19,    MinKey = -1,
                        MaxKey = 127
                    };

/*
 * query-comparison
 * https://www.mongodb.com/docs/manual/reference/operator/query-comparison/
 *      Equal                       => Eq
 *      Not Equal                   => Ne
 *      Greater                     => Gt
 *      Greater or Equal            => Gte
 *      Less                        => Lt
 *      Less or Equal               => Lte
 *      Matches a value in array    => In
 *      Matches no values in array  => Nin
 *
 *          https://www.mongodb.com/docs/manual/reference/operator/query/eq/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/ne/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/gt/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/gte/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/lt/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/lt/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/nin/
 */
template<typename T>
struct Eq
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Eq(CType init)
        : $eq(std::move(init))
    {}
    T       $eq;
};
template<typename T>
struct Ne
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Ne(CType init)
        : $ne(std::move(init))
    {}
    T       $ne;
};
template<typename T>
struct Gt
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Gt(CType init)
        : $gt(std::move(init))
    {}
    T       $gt;
};
template<typename T>
struct Gte
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Gte(CType init)
        : $gte(std::move(init))
    {}
    T       $gte;
};
template<typename T>
struct Lt
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Lt(CType init)
        : $lt(std::move(init))
    {}
    T       $lt;
};
template<typename T>
struct Lte
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Lte(CType init)
        : $lte(std::move(init))
    {}
    T       $lte;
};
template<typename T>
struct In
{
    using Query = bool;
    using CType = std::initializer_list<T>;
    In(CType init)
        : $in(std::move(init))
    {}
    std::vector<T>  $in;
};
template<typename T>
struct Nin
{
    using Query = bool;
    using CType = std::initializer_list<T>;
    Nin(CType init)
        : $nin(std::move(init))
    {}
    std::vector<T>  $nin;
};

/*
 * query-logical
 * https://www.mongodb.com/docs/manual/reference/operator/query-logical/
 *      Logical AND of two Values           => And
 *      Logical OR  of two Values           => Or
 *      Logical NOR of two Values           => Nor => NOR(A, B) => !(OR(A,B))
 *      Logical NOT                         => Not
 *
 *          https://www.mongodb.com/docs/manual/reference/operator/query/and/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/or/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/nor/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/not/
 */
template<typename LHS, typename RHS>
struct And
{
    using Query = bool;
    using CType = And<LHS, RHS>;
    using LP = ConstructorType<LHS>;
    using RP = ConstructorType<RHS>;
    And(LP lhs, RP rhs)
        : $and(std::move(lhs), std::move(rhs))
    {}
    std::tuple<LHS, RHS> $and;
};
template<typename LHS, typename RHS>
struct Or
{
    using Query = bool;
    using CType = Or<LHS, RHS>;
    using LP = ConstructorType<LHS>;
    using RP = ConstructorType<RHS>;
    Or(LP lhs, RP rhs)
        : $or(std::move(lhs), std::move(rhs))
    {}
    std::tuple<LHS, RHS>  $or;
};
template<typename LHS, typename RHS>
struct Nor
{
    using Query = bool;
    using CType = Nor<LHS, RHS>;
    using LP = ConstructorType<LHS>;
    using RP = ConstructorType<RHS>;
    Nor(LP lhs, RP rhs)
        : $nor(std::move(lhs), std::move(rhs))
    {}
    std::tuple<LHS, RHS> $nor;
};
template<typename T>
struct Not
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Not(CType init)
        : $not(std::move(init))
    {}
    T       $not;
};
/*
 * query-element
 * https://www.mongodb.com/docs/manual/reference/operator/query-element/
 *      An element exists in the object     =>      Exists
 *          https://www.mongodb.com/docs/manual/reference/operator/query/exists/
 *      An element has a specific Type      =>      Type
 *          https://www.mongodb.com/docs/manual/reference/operator/query/exists/
 */
struct Exists
{
    using Query = bool;
    using CType = bool;
    Exists(bool init)
        : $exists(init)
    {}
    bool   $exists;
};
struct Type
{
    using Query = bool;
    using CType = BsonType;
    Type(BsonType type)
        : $type(static_cast<std::int32_t>(type))
    {}
    std::int32_t        $type;
};
/*
 * query-evaluation
 * https://www.mongodb.com/docs/manual/reference/operator/query-evaluation/
 *      Expression:                             =>  TODO
 *          https://www.mongodb.com/docs/manual/reference/operator/query/expr/
 *          I think this is totally possible using the current schema using C++ classes.
 *          BUT. I don;t think that is the best idea.
 *               I want to think about a better way to represent expressions better.
 *      JsonSchema:                             =>  TODO
 *          https://www.mongodb.com/docs/manual/reference/operator/query/jsonSchema/
 *          This I don't quite understand.
 *          So open to suggestions here.
 *      Mod field and test remainder            =>  Mod
 *          https://www.mongodb.com/docs/manual/reference/operator/query/mod/
 *      Regular Expression                      =>  RegEx
 *          https://www.mongodb.com/docs/manual/reference/operator/query/regex/
 *      Text search of whole object             =>  Text
 *          https://www.mongodb.com/docs/manual/reference/operator/query/text/
 *          Can't seem to get his working.
 *      Where uses javascript expression.       => TODO
 *          Don't understand how this works.
 *
 */
// Expr         TODO
// JsonSchema   TODO
struct Mod
{
    using Query = bool;
    using CType = std::pair<std::uint32_t, std::uint32_t>;
    Mod(CType init)
        : $mod({init.first, init.second})
    {}

    std::array<std::uint32_t, 2>    $mod;
};
struct RegEx
{
    using Query = bool;
    using CType = std::pair<std::string, std::string>;
    RegEx(CType init)
        : $regex(std::move(init.first))
        , $options(std::move(init.second))
    {}

    std::string     $regex;
    std::string     $options;
};

#if 0
struct TextSearch
{
    using Query = bool;
    std::string                 $search;
    std::optional<std::string>  $language;
    std::optional<bool>         $caseSensitive;
    std::optional<bool>         $diacriticSensitive;
};
struct Text
{
    using Query = bool;
    Text(std::string search, std::optional<std::string> language = {}, std::optional<bool> cs = {}, std::optional<bool> ds = {})
        : $text{std::move(search), std::move(language), std::move(cs), std::move(ds)}
    {}
    TextSearch        $text;
};
#endif
// Where TODO
/*
 * query-array
 *      https://www.mongodb.com/docs/manual/reference/operator/query-array/
 *      Has all the following elements:                                 => All
 *      An array has an element that matches multiple considitions:     => ElemMatch
 *      An array has a specific size:                                   => Size
 *
 *          https://www.mongodb.com/docs/manual/reference/operator/query/all/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/elemMatch/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/size/
 */
template<typename T>
struct All
{
    using Query = bool;
    using CType = std::vector<T>;
    All(CType init)
        : $all(std::move(init))
    {}
    std::vector<T>      $all;
};
template<typename T>
struct Elements
{
    using Query = bool;
    std::optional<T>    $eq;
    std::optional<T>    $ne;
    std::optional<T>    $gt;
    std::optional<T>    $gte;
    std::optional<T>    $lt;
    std::optional<T>    $lte;
};
template<typename T>
struct ElemMatch
{
    using Query = bool;
    ElemMatch(std::optional<T> eq, std::optional<T> ne, std::optional<T> gt, std::optional<T> gte, std::optional<T> lt, std::optional<T> lte)
        : $elemMatch{std::move(eq), std::move(ne), std::move(gt), std::move(gte), std::move(lt), std::move(lte)}
    {}
    Elements<T>         $elemMatch;
};
struct Size
{
    using Query = bool;
    using CType = std::uint32_t;
    Size(std::uint32_t init)
        : $size{init}
    {}
    std::uint32_t       $size;
};

/*
 * Bitwise Query operator
 *  https://www.mongodb.com/docs/manual/reference/operator/query-bitwise/
 *      AllClear
 *      AllSet
 *      AnyClear
 *      AnySet
 *
 *
 *          https://www.mongodb.com/docs/manual/reference/operator/query/bitsAllClear/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/bitsAllSet/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/bitsAnyClear/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/bitsAnySet/
 */
struct AllClear
{
    using Query = bool;
    using CType = std::uint32_t;
    AllClear(CType init)
        : $bitsAllClear{std::move(init)}
    {}
    std::uint32_t       $bitsAllClear;
};
struct AllSet
{
    using Query = bool;
    using CType = std::uint32_t;
    AllSet(CType init)
        : $bitsAllSet{std::move(init)}
    {}
    std::uint32_t       $bitsAllSet;
};
struct AnyClear
{
    using Query = bool;
    using CType = std::uint32_t;
    AnyClear(CType init)
        : $bitsAnyClear{std::move(init)}
    {}
    std::uint32_t       $bitsAnyClear;
};
struct AnySet
{
    using Query = bool;
    using CType = std::uint32_t;
    AnySet(CType init)
        : $bitsAnySet{std::move(init)}
    {}
    std::uint32_t       $bitsAnySet;
};


}
}

ThorsAnvil_Template_MakeTrait(2, ThorsAnvil::DB::Mongo::Query,              q, limit, collation, hint);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Eq,        $eq);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Ne,        $ne);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Gt,        $gt);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Gte,       $gte);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Lt,        $lt);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Lte,       $lte);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::In,        $in);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Nin,       $nin);
ThorsAnvil_Template_MakeTrait(2, ThorsAnvil::DB::Mongo::QueryOp::And,       $and);
ThorsAnvil_Template_MakeTrait(2, ThorsAnvil::DB::Mongo::QueryOp::Or,        $or);
ThorsAnvil_Template_MakeTrait(2, ThorsAnvil::DB::Mongo::QueryOp::Nor,       $nor);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Not,       $not);
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::Exists,    $exists);
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::Type,      $type);
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::Mod,       $mod);
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::RegEx,     $regex, $options);
#if 0
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::TextSearch,$search, $language, $caseSensitive, $diacriticSensitive);
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::Text,      $text);
#endif
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::All,       $all);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Elements,  $eq, $ne, $gt, $gte, $lt, $lte);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::ElemMatch, $elemMatch);
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::Size,      $size);
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::AllClear,  $bitsAllClear);
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::AllSet,    $bitsAllSet);
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::AnyClear,  $bitsAnyClear);
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::AnySet,    $bitsAnySet);


#endif

#if 0
#include "test/Action.h"

// ThorsMongo_CreateFieldAccess(MotherAddressCode, Eq, Family, mother, address, code);
namespace ThorsAnvil::FieldAccess:: THORMONGO_NAME(Family, 0, mother, address, code) {                                                           \
    template<typename T>                                                                            \
    struct Access                                                                                   \
    {                                                                                               \
        using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;                                    \
        Access(CType init) : THORMONGO_BUILD_USE_FIRST(mother, address, code, 1)(std::move(init)) {}          \
        T             THORMONGO_BUILD_USE_FIRST(mother, address, code, 1);                                    \
    };                                                                                              \
    using TypeFirst = ::Family;                                                                     \
    THORMONGO_BULD_TYPE_INFO(Family, First, 00, mother, address, code);                                     \
}                                                                                                   \
ThorsAnvil_Template_MakeOverride(1,                                                                 \
    ThorsAnvil::FieldAccess:: THORMONGO_NAME(Family, 0, mother, address, code) ::Access,                                                          \
    {THORMONGO_BUILD_USE_FIRST_Q(mother, address, code, 1), THORMONGO_BUILD_FIELD_NAME(Family, mother, address, code)}\
);                                                                                                  \
ThorsAnvil_Template_MakeTrait(1,                                                                    \
    ThorsAnvil::FieldAccess:: THORMONGO_NAME(Family, 0, mother, address, code) ::Access,                                                          \
    THORMONGO_BUILD_USE_FIRST(mother, address, code, 1)                                                       \
);
// ThorsMongo_FilterFromAccess(MotherAddressCode, Eq, Family, mother, address, code);
using MotherAddressCode    =
    ThorsAnvil::FieldAccess::
        THORMONGO_NAME(Family, 0, mother, address, code)
            ::Access<ThorsAnvil::DB::Mongo::QueryOp::
                Eq<ThorsAnvil::FieldAccess::
                    THORMONGO_NAME(Family, 0, mother, address, code)
                        ::TypeOperat>>;


#endif
