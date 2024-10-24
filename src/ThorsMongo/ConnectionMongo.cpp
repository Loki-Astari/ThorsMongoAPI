#include "ConnectionMongo.h"

using namespace ThorsAnvil::DB::Mongo;

THORS_MONGO_HEADER_ONLY_INCLUDE
ConnectionMongo::ConnectionMongo(MongoURL const& url)
    : ThorsSocket::BaseSocketStream<ConnectionBufferMongo>({url.hostname, url.port})
{}
