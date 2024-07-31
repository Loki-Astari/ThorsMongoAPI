#include "ThorsMongo.h"
#include "Authenticate.h"
#include "AuthenticateScramSha.h"

using namespace ThorsAnvil::DB::Mongo;

ThorsMongo::ThorsMongo(MongoURL const&                  url,
                       Auth::UserNamePassword const&    authInfo,
                       Compression                      compression,
                       Auth::Client const&              clientInfo)
    : mongoStream(url)
    , messageHandler(mongoStream)
{
    Authenticate    authenticator;
    // TODO
    // authenticator.addAuthenticator("SCRAM-SHA-1", ThorsAnvil::DB::Mongo::Auth::ScramSha1::authenticate);
    authenticator.addAuthenticator("SCRAM-SHA-256", ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate);
    authenticator.handShake(messageHandler, authInfo, compression, clientInfo);
}

ThorsMongo::ThorsMongo(MongoURL const&                  url,
                       Auth::Certificate const&         /*authInfo*/,
                       Compression                      /*compression*/,
                       Auth::Client const&              /*clientInfo*/)
    : mongoStream(url)
    , messageHandler(mongoStream)
{
    throw std::runtime_error("TODO: Certificate authentication has not been done");
}
