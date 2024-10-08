#include "AuthenticateScramSha.h"
#include "MessageHandler.h"
#include "ThorsCrypto/scram.h"

namespace ThorsAnvil::DB::Mongo::Auth::ScramSha256
{
namespace TC = ThorsAnvil::Crypto;

// Implements the SCRAM-SHA-256 authentication mechanism.
THORS_MONGO_HEADER_ONLY_INCLUDE
void authenticate(MessageHandler& messageHandler, Auth::UserNamePassword const& authInfo)
{
    // Create and send the AuthInit object to Mongo.
    // Read the AuthReply from Mongo.
    TC::ScramClient256      client(authInfo.username, authInfo.password);
    MessageId               authInitId;
    AuthReply               authInitReply;
    bool                    authInitOK = false;
    if (messageHandler.sendMessage(AuthInit{authInfo.database,  "SCRAM-SHA-256", client.getFirstMessage()}, authInitId, OP_MsgFlag::empty, PrinterConfig{}))
    {
        if (messageHandler.recvMessage(authInitReply, authInitId, ParserConfig{}))
        {
            authInitOK = true;
        }
    }

    if (!authInitOK)
    {
        ThorsLogAndThrowCritical("ThorsAnvil::DB::Mongo::AuthenticateScramSha",
                                 "authenticate",
                                 "AuthInit request or response failed");
    }
    if (authInitReply.ok != 1)
    {
        messageHandler.setStreamBad();
        ThorsLogAndThrowCritical("ThorsAnvil::DB::Mongo::AuthenticateScramSha",
                                 "authenticate",
                                 "Handshake FirstMessage: ",
                                 "Code: ", authInitReply.code,
                                 "Name: ", authInitReply.codeName,
                                 "Msg:  ", authInitReply.errmsg);
    }

    // Create and send the AuthCont object to Mongo.
    // Read the AuthReply from Mongo.
    MessageId               authContId;
    AuthReply               authContReply;
    bool                    authContOK = false;
    if (messageHandler.sendMessage(
                                   AuthCont{authInitReply.conversationId,
                                            authInfo.database,
                                            client.getFinalMessage(authInitReply.payload.getData())
                                           },
                                   authContId,
                                   OP_MsgFlag::empty,
                                   PrinterConfig{}))
    {
        if (messageHandler.recvMessage(authContReply, authContId, ParserConfig{}))
        {
            authContOK = true;
        }
    }

    if (!authContOK)
    {
        ThorsLogAndThrowCritical("ThorsAnvil::DB::Mongo::AuthenticateScramSha",
                                 "authenticate",
                                 "AuthCont request or response failed");
    }
    if (authContReply.ok != 1)
    {
        ThorsLogAndThrowCritical("ThorsAnvil::DB::Mongo::AuthenticateScramSha",
                                 "authenticate",
                                 "Handshake Proof: ",
                                 "Code: ", authContReply.code,
                                 "Name: ", authContReply.codeName,
                                 "Msg:  ", authContReply.errmsg);
    }

    // Create and send the AuthCont (second time) object to Mongo.
    // Read the AuthReply from Mongo.
    MessageId               authContId2;
    AuthReply               authContReply2;
    bool                    authContOK2 = false;
    if (messageHandler.sendMessage(AuthCont{authContReply.conversationId, authInfo.database, ""}, authContId2, OP_MsgFlag::empty, PrinterConfig{}))
    {
        if (messageHandler.recvMessage(authContReply2, authContId2, ParserConfig{}))
        {
            authContOK2 = true;
        }
    }

    if (!authContOK2)
    {
        ThorsLogAndThrowCritical("ThorsAnvil::DB::Mongo::AuthenticateScramSha",
                                 "authenticate",
                                 "AuthCont2 request or response failed");
    }
    if (authContReply2.ok != 1)
    {
        ThorsLogAndThrowCritical("ThorsAnvil::DB::Mongo::AuthenticateScramSha",
                                "authenticate",
                                 "Handshake DB Connect: ",
                                 "Code: ", authContReply2.code,
                                 "Name: ", authContReply2.codeName,
                                 "Msg:  ", authContReply2.errmsg);
    }

    // If everything woked correctly.
    // We should be done.
    // We now have a TCP/IP connection that is authenticated and connected to Mongo.
    if (!authContReply2.done)
    {
        ThorsLogAndThrowCritical("ThorsAnvil::DB::Mongo::AuthenticateScramSha",
                                "authenticate",
                                 "Handshake DB Connect: ", "Expected handshake to be complete");
    }
}

}

using namespace ThorsAnvil::DB::Mongo::Auth::ScramSha256;

/*
 * The AuthInit / AuthCont / AuthReply types are simply property bags.
 *
 * They are used to communciate with the Mongo server and are
 * trvial to serialize into BSON (using ThorsSerializer).
 */
THORS_MONGO_HEADER_ONLY_INCLUDE
AuthInit::AuthInit(std::string const& db, std::string const& mechanism, std::string&& payload)
    : saslStart(1)
    , mechanism(mechanism)
    , $db(db)
    , payload(0, std::move(payload))
{}

THORS_MONGO_HEADER_ONLY_INCLUDE
AuthCont::AuthCont(std::int32_t convId, std::string const& db, std::string&& payload)
    : saslContinue(1)
    , conversationId(convId)
    , $db(db)
    , payload(0, std::move(payload))
{}

THORS_MONGO_HEADER_ONLY_INCLUDE
AuthReply::AuthReply()
    : payload(0)
{}
