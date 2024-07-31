#ifndef THORSANVIL_DB_MONGO_THORSMONGO_FIND_H
#define THORSANVIL_DB_MONGO_THORSMONGO_FIND_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"
#include "ThorsMongoGetMore.h"

namespace ThorsAnvil::DB::Mongo
{

class FindConfig: public ActionConfig<FindConfig>
{
    std::optional<Sort>             sort;
    std::optional<Projection>       projection;
    std::optional<Hint>             hint;
    std::optional<std::uint32_t>    skip;                   // Default 0
    std::optional<std::uint32_t>    limit;                  // Default no-limit (0 => no-limit)
    std::optional<std::uint32_t>    batchSize;              // No Default. 0 => no values returned on initial call.
    std::optional<bool>             singleBatch;            // Default false
    std::optional<std::string>      comment;
    std::optional<std::uint32_t>    maxTimeMS;              // Default no-timeout  0 => no-timeout
    std::optional<ReadConcern>      readConcern;
    std::optional<Limit>            max;                    // Using max requires hint.
    std::optional<Limit>            min;                    // Using min requires hint.
    std::optional<bool>             returnKey;              // Default false
    std::optional<bool>             showRecordId;
    std::optional<bool>             tailable;
    std::optional<bool>             awaitData;
    std::optional<bool>             noCursorTimeout;
    std::optional<bool>             allowPartialResults;
    std::optional<Collation>        collation;
    std::optional<bool>             allowDiskUse;
    std::optional<Let>              let;

    public:
        std::optional<Sort>             const& getSort()               const {return sort;}
        std::optional<Projection>       const& getProjection()         const {return projection;}
        std::optional<Hint>             const& getHint()               const {return hint;}
        std::optional<std::uint32_t>    const& getSkip()               const {return skip;}
        std::optional<std::uint32_t>    const& getLimit()              const {return limit;}
        std::optional<std::uint32_t>    const& getBatchSize()          const {return batchSize;}
        std::optional<bool>             const& getSingleBatch()        const {return singleBatch;}
        std::optional<std::string>      const& getComment()            const {return comment;}
        std::optional<std::uint32_t>    const& getMaxTimeMS()          const {return maxTimeMS;}
        std::optional<ReadConcern>      const& getReadConcern()        const {return readConcern;}
        std::optional<Limit>            const& getMax()                const {return max;}
        std::optional<Limit>            const& getMin()                const {return min;}
        std::optional<bool>             const& getReturnKey()          const {return returnKey;}
        std::optional<bool>             const& getShowRecordId()       const {return showRecordId;}
        std::optional<bool>             const& getTailable()           const {return tailable;}
        std::optional<bool>             const& getAwaitData()          const {return awaitData;}
        std::optional<bool>             const& getNoCursorTimeout()    const {return noCursorTimeout;}
        std::optional<bool>             const& getAllowPartialResults()const {return allowPartialResults;}
        std::optional<Collation>        const& getCollation()          const {return collation;}
        std::optional<bool>             const& getAllowDiskUse()       const {return allowDiskUse;}
        std::optional<Let>              const& getLet()                const {return let;}

        FindConfig&     setSort(Sort value)                 {sort.emplace(value);return *this;}
        FindConfig&     setProjection(Projection value)     {projection.emplace(value);return *this;}
        FindConfig&     setHint(Hint value)                 {hint.emplace(value);return *this;}
        FindConfig&     setSkip(std::uint32_t value)        {skip.emplace(value);return *this;}
        FindConfig&     setLimit(std::uint32_t value)       {limit.emplace(value);return *this;}
        FindConfig&     setBatchSize(std::uint32_t value)   {batchSize.emplace(value);return *this;}
        FindConfig&     setSingleBatch(bool value)          {singleBatch.emplace(value);return *this;}
        FindConfig&     setComment(std::string value)       {comment.emplace(value);return *this;}
        FindConfig&     setMaxTimeMS(std::uint32_t value)   {maxTimeMS.emplace(value);return *this;}
        FindConfig&     setReadConcern(ReadConcern value)   {readConcern.emplace(value);return *this;}
        FindConfig&     setMax(Limit value)                 {max.emplace(value);return *this;}
        FindConfig&     setMin(Limit value)                 {min.emplace(value);return *this;}
        FindConfig&     setReturnKey(bool value)            {returnKey.emplace(value);return *this;}
        FindConfig&     setShowRecordId(bool value)         {showRecordId.emplace(value);return *this;}
        FindConfig&     setTailable(bool value)             {tailable.emplace(value);return *this;}
        FindConfig&     setAwaitData(bool value)            {awaitData.emplace(value);return *this;}
        FindConfig&     setNoCursorTimeout(bool value)      {noCursorTimeout.emplace(value);return *this;}
        FindConfig&     setAllowPartialResults(bool value)  {allowPartialResults.emplace(value);return *this;}
        FindConfig&     setCollation(Collation value)       {collation.emplace(value);return *this;}
        FindConfig&     setAllowDiskUse(bool value)         {allowDiskUse.emplace(value);return *this;}
        FindConfig&     setLet(Let value)                   {let.emplace(value);return *this;}
};


template<typename T>
class FindResult: public CmdReplyBase, public MongoActionReadInterfaceTrivialImpl<FindResult<T>>
{
    friend class ThorsAnvil::Serialize::Traits<FindResult<T>>;
    friend class TestFindResult<T>;

    Nref<Collection>            owner;
    GetMoreConfig               moreConfig;
    std::size_t                 index;
    CursorFirst<T>              cursor;
    TimeStamp                   operationTime;
    ClusterTime                 $clusterTime;

    public:
        FindResult(Collection& owner, FindConfig const& config)
            : owner(owner)
            , index(0)
        {
            std::optional<std::uint32_t> const& batchSize   = config.getBatchSize();
            if (batchSize.has_value()) {
                moreConfig.setBatchSize(batchSize.value());
            }
            std::optional<std::uint32_t> const& maxTimeMS   = config.getMaxTimeMS();
            if (maxTimeMS.has_value()) {
                moreConfig.setMaxTimeMS(maxTimeMS.value());
            }
            std::optional<std::string> const&   comment     = config.getComment();
            if (comment.has_value()) {
                moreConfig.setComment(comment.value());
            }
        }
        ~FindResult()
        {
            if (cursor.getId() != 0) {
                owner.get().killCursor(cursor.getId());
            }
        }
        friend void swap(FindResult<T>& lhs, GetMoreResult<T>& rhs) noexcept
        {
            using std::swap;
            swap(static_cast<CmdReplyBase&>(lhs), static_cast<CmdReplyBase&>(rhs));
            swap(lhs.cursor                     , rhs.cursor);
            swap(lhs.operationTime              , rhs.operationTime);
            swap(lhs.$clusterTime               , rhs.$clusterTime);
        }

    private:
        friend class CursorIterator<T>;
        // These functions are for the CursorIterator.
        bool increment()
        {
            ++index;
            if (cursor.data().size() == index)
            {
                index = 0;
                owner.get().getMore(*this, moreConfig);
            }
            return index != cursor.data().size();
        }
        T& current()
        {
            return cursor.firstBatch[index];
        }
    private:
        friend class Collection;
        // These functions are for Collection.
        CursorFirst<T>& getCursor() {return cursor;}
};

}

ThorsAnvil_Template_ExpandTrait(1,
                        ThorsAnvil::DB::Mongo::CmdReplyBase,
                        ThorsAnvil::DB::Mongo::FindResult,              cursor, operationTime, $clusterTime);

#endif
