#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE
#error "This should not be included directly: Include "ThorsMongo.h"
#endif

namespace ThorsAnvil::DB::Mongo
{

namespace Action
{

template<typename T>
class GetMore: public MongoActionWriteInterfaceTrivialImpl<GetMore<T>>
{
    friend class ThorsAnvil::Serialize::Traits<GetMore<T>>;
    std::uint64_t                   getMore;
    std::string                     collection;
    std::string                     $db;
    std::optional<std::uint32_t>    batchSize;
    std::optional<std::uint32_t>    maxTimeMS;
    std::optional<std::string>      comment;

    public:
        GetMore(std::string_view collection, std::string_view db, GetMoreConfig const& config, CursorFirst<T>& cursor)
            : getMore(cursor.getId())
            , collection(collection)
            , $db(db)
            , batchSize(config.getBatchSize())
            , maxTimeMS(config.getMaxTimeMS())
            , comment(config.getComment())
        {}
};

}


template<typename T>
inline
void Collection::getMore(FindResult<T>& findResult, GetMoreConfig const& config)
{
    GetMoreResult<T>    response;
    MessageId           messageId;
    if (mongoServer.messageHandler.sendMessage(Action::GetMore<T>{colName(), dbName(), config, findResult.getCursor()},
                                               messageId,
                                               config.getMsgFlag(),
                                               config.getPrinterConfig()))
    {
        if (mongoServer.messageHandler.recvMessage(response, messageId, config.getParserConfig()))
        {
            // The message was sent and reply read correctly.
            // Note: This does not mean it worked.
            //       There may be an error message in the response object
        }
    }
    // Even if there is an error we
    // want to move the data.
    // This will make the data available for the object using getMore.
    swap(findResult, response);
}

}

ThorsAnvil_Template_MakeTrait(1,
                        ThorsAnvil::DB::Mongo::Action::GetMore,          getMore, collection, $db, batchSize, maxTimeMS, comment);