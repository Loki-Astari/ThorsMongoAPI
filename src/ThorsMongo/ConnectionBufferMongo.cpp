#include "ConnectionBufferMongo.h"

#include "snappy.h"

using namespace ThorsAnvil::DB::Mongo;


THORS_MONGO_HEADER_ONLY_INCLUDE
ConnectionBufferMongo::ConnectionBufferMongo(ConnectionBufferMongo&& move) noexcept
    : SocketStreamBuffer(std::move(move))
    , inMessageSize(std::exchange(move.inMessageSize, 0))
    , outMessageSize(std::exchange(move.outMessageSize, 0))
    , underflowNeeds(std::exchange(move.underflowNeeds, 0))
    , outMessagePlaced(std::exchange(move.outMessagePlaced, 0))
{}

THORS_MONGO_HEADER_ONLY_INCLUDE
ConnectionBufferMongo& ConnectionBufferMongo::operator=(ConnectionBufferMongo&& move) noexcept
{
    SocketStreamBuffer::operator=(std::move(move));
    inMessageSize   = std::exchange(move.inMessageSize, 0);
    outMessageSize  = std::exchange(move.outMessageSize, 0);
    underflowNeeds  = std::exchange(move.underflowNeeds, 0);
    outMessagePlaced= std::exchange(move.outMessagePlaced, 0);
    return *this;
}

THORS_MONGO_HEADER_ONLY_INCLUDE
ConnectionBufferMongo::int_type ConnectionBufferMongo::underflow()
{
    /*
     * Ensures that at least one character is available in the input area by updating the pointers
     * to the input area (if needed) * and reading more data in from the input sequence
     * (if applicable).
     *
     * Returns the value of that character (converted to int_type with Traits::to_int_type(c)) on success
     * or Traits::eof() on failure.
     *
     * The function may update gptr, egptr and eback pointers to define the location of newly
     * loaded data (if any).
     *
     * On failure, the function ensures that either gptr() == nullptr or gptr() == egptr.
     * The base class version of the function does nothing. The derived classes may override this function
     * to allow updates to the get area in the case of exhaustion.
     */
    if (gptr() == egptr())
    {
        std::size_t  newDataCount = 0;

        // Check to see if we are in the middle of reading a message.
        if ((gptr() - eback()) == inMessageSize)
        {
            // So we are reading a new message.
            // ================================

            // We are about to reset the eback(), gptr(), egptr()
            // The "SocketStreamBuffer" requires we record the amount of data that was read so
            // we can keep track of the amount of the input position correctly.
            incrementInCount(inMessageSize);

            // Set the buffer back to empty.
            setg(eback(), eback(), eback());
            inMessageSize = 0;

            // The message header is 4 std::uint32_t objects.
            // Force the read of the whole header block this will give us size information in the
            // first 4 bytes and the OpCode in the last 4 bytes.
            // If the OpCode is OP_Compress then we need to read the full message to decompress.
            ThorsSocket::IOData result = getSocket().getMessageData(egptr(), kSizeOpMsgHeaderSize);
            newDataCount += result.dataSize;

            if (newDataCount != kSizeOpMsgHeaderSize)
            {
                ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "underflow", "Failed to get next message header: Got: ", newDataCount);
                return traits::eof();
            }

            // Yes I know this looks risky.
            // Alignment comment.
            // The underlying streambuffer guarantees that the beginning of the read buffer "eback()"
            // is aligned to alignof(std::max_align_t). This means that the buffer is aligned for
            // all alignment types.
            // Details: See: 6.7.6 Alignment [basic.align]
            OpMsgHeader* headerInfo = reinterpret_cast<OpMsgHeader*>(eback());

            if (headerInfo->opCode == OpCode::OP_COMPRESSED)
            {
                getSocket().getMessageData(egptr() + kSizeOpMsgHeaderSize, kSizeOpCompressedMessage);

                // See: "Alignment comment" above
                OpCompressedBlock*      message = reinterpret_cast<OpCompressedBlock*>(eback());
                switch (message->compression)
                {
                    case Compression::Snappy:
                    {
                        // Retrieve the compressed data into a separate buffer.
                        std::uint32_t           compressMessageSize = message->messageSize - kSizeOpCompressedBlock;
                        std::vector<char>       compressedData;
                        compressedData.resize(compressMessageSize);
                        getSocket().getMessageData(&compressedData[0], compressMessageSize);

                        // Fix the message header with the uncompressed info.
                        message->messageSize = kSizeOpMsgHeaderSize + message->uncompressedSize;
                        message->opCode      = message->originalOpCode;
                        reserveInputSize(message->messageSize);

                        // Add the uncompressed data to the underlying buffer.
                        bool suc = snappy::RawUncompress(&compressedData[0], compressMessageSize, eback() + kSizeOpMsgHeaderSize);
                        if (!suc)
                        {
                            ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "underflow", "Failed to uncompress data with snappy");
                            return traits::eof();
                        }

                        inMessageSize       = message->messageSize;
                        newDataCount        = inMessageSize;
                        break;
                    }
                    // TODO Support ZLib and ZStd
                    default:
                    {
                        ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "underflow", "Failed to uncompress. Only support snappy compression. Compression: ", static_cast<unsigned int>(message->compression));
                        return traits::eof();
                    }
                }
            }
            else if (headerInfo->opCode == OpCode::OP_MSG)
            {
                inMessageSize       = headerInfo->messageSize;

                // Make sure the buffer has enough space for the whole message.
                reserveInputSize(inMessageSize);
            }
            else
            {
                ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "underflow", "Unknown message opCode: ",  static_cast<unsigned int>(headerInfo->opCode));
                return traits::eof();
            }
            setg(eback(), gptr(), egptr() + newDataCount);
        }
        std::size_t extraData = 0;

        // See: "Alignment comment" above
        OpMsgBlock* messageBlock = reinterpret_cast<OpMsgBlock*>(eback());

        // We now have at least the Message header block. So we know the message size.
        std::size_t maxAvailableData = inMessageSize - messageReadSize();
        if (underflowNeeds > maxAvailableData)
        {
            ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "underflow", "Should never need to read more bytes than in the message! Message Size: ", inMessageSize, "already read: ", (gptr() - eback()), " requested bytes: ", underflowNeeds);
            return traits::eof();
        }

        // Try and read as much of the message as we can (whole message in one go if possible).
        // But we may not need everything so don't block if it is not available.
        // Note this read could be zero sized if we just read a compressed block.
        if (missingData() != 0)
        {
            ThorsSocket::IOData result = getSocket().tryGetMessageData(egptr(), missingData());
            newDataCount += result.dataSize;
            extraData    += result.dataSize;

            // If we still don't have enough data to satisfy the underlying request
            // Then we need to block and wait until we have it.
            if (newDataCount < underflowNeeds && result.stillOpen)
            {
                // Must get at least 'underflowNeeds' bytes.
                // So if not enough data was retrieved read blocking until we have data or there is an error
                result = getSocket().getMessageData(egptr(), (underflowNeeds - newDataCount));
                newDataCount += result.dataSize;
                extraData    += result.dataSize;
            }

            if (newDataCount < underflowNeeds)
            {
                ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "underflow", "Should never need to read more bytes than in the message! Message Size: ", inMessageSize, " requested bytes: ", underflowNeeds);
                return traits::eof();
            }
        }
        setg(eback(), gptr(), egptr() + extraData);

        if (missingData() == 0 && messageBlock->flags & OP_MsgFlag::checksumPresent)
        {
            // Calculate the expected checksum.
            Checksum        checksum;
            checksum.append(std::string_view(eback(), inMessageSize - kSizeUInt32));
            std::uint32_t   checksumValue       = checksum.checksum();

            // Retrieve the checksum provided on the stream.
            std::uint32_t   checksumFromStream;
            std::copy(egptr() - kSizeUInt32, egptr(), reinterpret_cast<char*>(&checksumFromStream));

            // XOR the two values.
            // Put that back on the stream.
            // The next level up will then see a zero as success and anything else as failure.
            std::uint32_t   check = checksumValue ^ checksumFromStream;
            std::copy(reinterpret_cast<char*>(&check), reinterpret_cast<char*>(&check) + kSizeUInt32, egptr() - kSizeUInt32);

            if (check != 0)
            {
                ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "underflow", "Checksum does not match expected checksum");
            }
        }
    }
    return gptr() == egptr() ? traits::eof() : traits::to_int_type(*gptr());
}


// Used by `xsgetn` to set the "underflowNeeds" value but
// put it back to the original value after the call to underflow().
struct UnderflowCountSetter
{
    std::uint32_t& underflowNeeds;
    UnderflowCountSetter(std::uint32_t& underflowNeeds, std::uint32_t value)
        : underflowNeeds(underflowNeeds)
    {
        underflowNeeds = value;
    }
    ~UnderflowCountSetter()
    {
        underflowNeeds = 1;
    }
};


THORS_MONGO_HEADER_ONLY_INCLUDE
std::streamsize ConnectionBufferMongo::xsgetn(char_type* dest, std::streamsize count)
{
    /*
     * Reads count characters from the input sequence and stores them into a character array pointed to by dest.
     *
     * The characters are read as if by repeated calls to sbumpc().
     *
     * That is, if less than count characters are immediately available, the function calls uflow() to
     * provide more until traits::eof() is returned. Classes derived from std::basic_streambuf are permitted
     * to provide more efficient implementations of this function.
     */


    // Check to see if the request is a valid length.
    // Note: If messageLeft == 0 then we can go to the next message.
    std::uint32_t    messageLeft = inMessageSize - messageReadSize();
    if (messageLeft != 0 && count > messageLeft)
    {
        ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "xsgetn", "Can not read more data than left in the message: requested: ", count, " data left: ", messageLeft);
        return 0;
    }

    // See how much we have locally in the buffer.
    std::streamsize currentBufferSize = availableData();

    if (currentBufferSize < count)
    {
        UnderflowCountSetter    set(underflowNeeds, (count - currentBufferSize));
        if (underflow() == traits::eof())
        {
            ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "xsgetn", "Underflow failed to retrieve enough data");
            return 0;
        }
    }

    currentBufferSize = egptr() - gptr();
    std::uint32_t readable = std::min(count, currentBufferSize);
    std::copy(gptr(), gptr() + readable, dest);
    gbump(readable);

    return readable;
}

THORS_MONGO_HEADER_ONLY_INCLUDE
ConnectionBufferMongo::int_type ConnectionBufferMongo::overflow(int_type ch)
{
    if (ch == traits::eof()) {
        return traits_type::to_int_type('a');
    }

    // Should not call overflow when message size is not zero.
    // Writes fill up the buffer. Then sync() will send the data
    // out to the stream and reset outMessageSize to zero.
    if (outMessageSize != 0)
    {
        ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "overflow", "Unexpected call to overflow. The outMessageSize is set and we reservered space. outMessageSize: ", outMessageSize, " Buffer: [pbase(): " , pbase(), " pptr(): " , pptr(), " epptr: ", epptr(), "]");
        return traits::eof();
    }

    // There may be multiple calls to overflow until we have the size of the buffer stored.
    pbase()[outMessagePlaced] = ch;
    ++outMessagePlaced;
    if (outMessagePlaced == kSizeUInt32)
    {
        std::copy(pbase(), pbase() + kSizeUInt32, reinterpret_cast<char*>(&outMessageSize));
        if (outMessageSize < kSizeSmallestPossibleMessage)
        {
            ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "overflow", "Minimum message size is 26 bytes.");
            return traits::eof();
        }
        // OK. So we have the message size.
        //     Make sure we have enough space for the message and set up the buffer.
        reserveOutputSize(outMessageSize);
        setp(pbase(), pbase() + outMessageSize);
    }
    return ch;
}

THORS_MONGO_HEADER_ONLY_INCLUDE
std::streamsize ConnectionBufferMongo::xsputn(char_type const* source, std::streamsize count)
{
    /*
     * Writes count characters to the output sequence from the character array whose first element is pointed to by source.
     *
     * The characters are written as if by repeated calls to sputc().
     * Writing stops when either count characters are written or a call to sputc() would have returned Traits::eof().
     *
     * If the put area becomes full (pptr() == epptr()), this function may call overflow(),
     * or achieve the effect of calling overflow() by some other, unspecified, means.
     */
    std::size_t sizeCount = 0;
    while (outMessageSize == 0 && outMessagePlaced < kSizeUInt32)
    {
        if (overflow(source[sizeCount]) == traits::eof()) {
            return 0;
        }
        ++sizeCount;
    }
    std::streamsize spaceInBuffer = epptr() - pptr();
    if (count > spaceInBuffer)
    {
        ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "xsputn", "Failed to put data in buffer as it was not large enough. ExtraSpaceneeded: ", (count - spaceInBuffer));
        return 0;
    }

    // Put everything in the buffer.
    // We don't write until we have the complete message.
    // This is because we may want to compress the buffer before sending (most Mongo servers support this).
    std::copy(source + sizeCount, source + count, pptr() + sizeCount);
    pbump(count);

    return count;
}

THORS_MONGO_HEADER_ONLY_INCLUDE
int ConnectionBufferMongo::sync()
{
    if (pptr() != epptr())
    {
        ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "sync", "unexpected sync! The buffer is not yet full");
        return -1;
    }
    OpMsgBlock* messageBlock = reinterpret_cast<OpMsgBlock*>(pbase());
    if (messageBlock->flags & OP_MsgFlag::checksumPresent)
    {
        // The checksum pushed by the user is simply overwritten.
        // There is no way for them to calculate the info so the expectation is that they write
        // a 32 bit value and the end of the message. See MessageHandler
        Checksum    checksum;
        checksum.append(std::string_view(pbase(), outMessageSize - kSizeUInt32));

        std::uint32_t   checksumValue = checksum.checksum();
        std::copy(reinterpret_cast<char*>(&checksumValue), reinterpret_cast<char*>(&checksumValue) + kSizeUInt32, epptr() - kSizeUInt32);
    }
    switch (useCompressionOnWrite)
    {
        case Compression::None:
        {
            // output the normal message
            writeToStream(pbase(), epptr() - pbase());
            break;
        }
        case Compression::Snappy:
        {
            std::string output;
            std::uint32_t   objectToCompressSize = (pptr() - pbase()) - kSizeOpMsgHeaderSize;
            snappy::Compress(pbase() + kSizeOpMsgHeaderSize, objectToCompressSize, &output);

            OpCompressedBlock compressedMessageHeader;

            // Get a copy of the original header
            std::copy(pbase(), pbase() + kSizeOpMsgHeaderSize, reinterpret_cast<char*>(&compressedMessageHeader));
            // Now convert that to a compressed message header.
            compressedMessageHeader.originalOpCode      = compressedMessageHeader.opCode;
            compressedMessageHeader.opCode              = OpCode::OP_COMPRESSED;
            compressedMessageHeader.uncompressedSize    = compressedMessageHeader.messageSize - kSizeOpMsgHeaderSize;
            compressedMessageHeader.compression         = Compression::Snappy;
            compressedMessageHeader.messageSize         = kSizeOpCompressedBlock + output.size();

            // output the compressed message.
            writeToStream(reinterpret_cast<char const*>(&compressedMessageHeader), kSizeOpCompressedBlock);
            writeToStream(&output[0], output.size());
            break;
        }
        // TODO ZLib ZStd
        default:
        {
            ThorsLogCritical("ThorsAnvil::DB::Mongo::ConnectionBufferMongo", "sync", "Unsupoorted compression mechanism suggested");
        }
    }
    outMessageSize = 0;
    outMessagePlaced = 0;
    setp(pbase(), pbase());
    return 0;
}
