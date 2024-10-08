#include "Authenticate.h"
#include "ConnectionBufferMongo.h"

#if !defined(__WINNT__)
#include <sys/utsname.h>
#endif

using namespace ThorsAnvil::DB::Mongo;

/*
 * This is the message we send to Mongo as soon as the connection is established.
 * Mongo will return a HandShakeReply.
 *
 * The handshake reply will contain the appropriate information to start an
 * authentication seasion (which should be the next message to Mongo).
 *
 * Mongo may provide multiple authentication mechanisms in the reply.
 * The authentication mechanisms are ordered by preference.
 * Use the first one that you know how to use.
 */
THORS_MONGO_HEADER_ONLY_INCLUDE
Auth::HandShake::HandShake(std::string const& username, std::string const& db, Auth::Client const& client, std::vector<std::string> const& compression)
    //: isMaster(true)
    : hello(1)
    , saslSupportedMechs(db + "." + username)
    , $db(db)
    // TODO What should this be?
    , hostInfo("BatCave.local:27017")
    , client(client)
    , compression(compression)
{}

THORS_MONGO_HEADER_ONLY_INCLUDE
void Authenticate::handShake(MessageHandler& messageHandler,
                       Auth::UserNamePassword const& authInfo,
                       Compression useCompression,
                       Auth::Client const& clientInfo)
{
    using std::string_literals::operator""s;

    try
    {
        // Set up what compression algorithms we support.
        std::vector<std::string>    compression;
        if (useCompression & Compression::Snappy) {
            compression.emplace_back("snappy");
        }
        /*
        TODO Need to support compression below before we add it here.
        if (useCompression & Compression::ZLib) {
            compression.emplace_back("zlib");
        }
        if (useCompression & Compression::ZStd) {
            compression.emplace_back("zstd");
        }
        */
        authenticteHandShake(messageHandler, authInfo, clientInfo, compression);
    }
    catch (...)
    {
        // If there is an exception.
        // Make sure the stream is in a bad state.
        messageHandler.setStreamBad();

        // Still re-throw as without authentication.
        // We want to force application exit unless explicitly caught.
        throw;
    }
}

THORS_MONGO_HEADER_ONLY_INCLUDE
void Authenticate::authenticteHandShake(
                               MessageHandler& messageHandler,
                               Auth::UserNamePassword const& authInfo,
                               Auth::Client const& clientInfo,
                               std::vector<std::string> const& compression)
{
    // Send handshake
    MessageId               handShakeId;
    Auth::HandShakeReply    reply;
    bool                    handShakeOK = false;

    if (messageHandler.sendMessage(Auth::HandShake{authInfo.username, authInfo.database, clientInfo, compression}, handShakeId, OP_MsgFlag::empty, PrinterConfig{}))
    {
        if (messageHandler.recvMessage(reply, handShakeId, ParserConfig{}))
        {
            handShakeOK = true;
        }
    }

    // Check the handshake worked.
    if (!handShakeOK)
    {
        ThorsLogAndThrowCritical("ThorsAnvil::DB::Mongo::Authenticate",
                                 "authenticteHandShake",
                                 "Handshake request or response failed");
    }
    if (reply.ok != 1)
    {
        messageHandler.setStreamBad();
        ThorsLogAndThrowCritical("ThorsAnvil::DB::Mongo::Authenticate",
                                 "authenticteHandShake",
                                 "Handshake Request Failed: ",
                                 "Code: ", reply.codeName,
                                 "Msg:  ", reply.errmsg);
    }

    /*
     * Will receive only compression methods that we mentioned we could support
     * in the initial handshake.
     *
     * If we start supporting more compression techniques we need to update this.
     */
    // TODO Check for ZLib / ZStd
    auto find = std::find(std::begin(reply.compression), std::end(reply.compression), "snappy");
    if (find != std::end(reply.compression))
    {
        messageHandler.setCompression(Compression::Snappy);
    }

    // Check the authentication mechanisms supported by the server.
    bool authenticated = false;
    for (auto const& auth: reply.saslSupportedMechs)
    {
        // See if we support the same mecahanism.
        // See: test/IntegrationConnectionMongoTest.cpp for examples.
        auto find = authenticators.find(auth);
        if (find != authenticators.end())
        {
            // We only try and authenticate with the first match.
            // The matches are tried in order provided by the server
            // as this is the servers preferred order.
            authenticated = true;
            find->second(messageHandler, authInfo);
            break;
        }
    }

    // If we did not find any authentication mechanisms
    // Then must throw as the server will not allow any actions.
    if (!authenticated)
    {
        ThorsLogAndThrowCritical("ThorsAnvil::DB::Mongo::Authenticate",
                                 "authenticteHandShake",
                                 "No compatable authentication mechanism found");
    }
}
