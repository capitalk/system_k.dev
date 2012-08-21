#include "ExpandableRingBuffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace itch
{

ExpandableRingBuffer::ExpandableRingBuffer(int nBlockSize)
        : m_nBlockSize(nBlockSize <= 0 ? DEFAULT_RINGBUFFER_BLOCKSIZE : nBlockSize)
        , m_pcBufferHead(NULL)
        , m_nBufferLength(0)
        , m_nDataOffset(0)
        , m_nDataLength(0)
{
}

ExpandableRingBuffer::~ExpandableRingBuffer()
{
    if (m_pcBufferHead != NULL)
    {
        free(m_pcBufferHead);
        m_pcBufferHead = NULL;
    }
}

void ExpandableRingBuffer::WriteData(const void* pBuffer, int nLength)
{
    if (nLength <= 0)
        return;

    const unsigned char *pcBuffer = (const unsigned char *)pBuffer;

    if (m_pcBufferHead == NULL || m_nDataLength == 0)
    {
        assert(m_nDataOffset == 0);
        // Simple: the buffer is either not allocated or has no data.
        int nNewBufferLength = ((nLength + m_nBlockSize - 1)/m_nBlockSize)*m_nBlockSize;
        if (nNewBufferLength > m_nBufferLength)
        {
            // Re-allocate the buffer only when it is unable to accommodate
            // the incoming data.
            m_nBufferLength = nNewBufferLength;
            m_pcBufferHead = (unsigned char *)realloc(m_pcBufferHead, m_nBufferLength);
        }
        m_nDataLength = nLength;
        memcpy(m_pcBufferHead, pcBuffer, nLength);
    }
    else
    {
        if (m_nBufferLength - m_nDataLength < nLength)
        {
            // The remaining free space in the buffer is not enough
            // to accommodate the incoming data. Let's expand the buffer.
            int nNewBufferLength = ((m_nDataLength + nLength + m_nBlockSize - 1)/m_nBlockSize)*m_nBlockSize;
            m_pcBufferHead = (unsigned char *)realloc(m_pcBufferHead, nNewBufferLength);

            if (m_nDataLength > 0)
            {
                // The existing data may need to be re-aligned to the new
                // buffer size by "unwrapping" it.
                int nDataEndOffset = (m_nDataOffset + m_nDataLength)%m_nBufferLength;
                if (nDataEndOffset < m_nDataOffset)
                {
                    // Yep, the data is wrapped all right, and we shall want to
                    // move a portion of it to the end of the buffer.

                    int nNewFreeLength = nNewBufferLength - m_nBufferLength;
                    int nMoveLength = (nDataEndOffset < nNewFreeLength ? nDataEndOffset : nNewFreeLength);

                    memmove(m_pcBufferHead + m_nBufferLength, m_pcBufferHead, nMoveLength);

                    // See if some data still remains in the beginning of the buffer,
                    // in which case it must be moved to the buffer head.
                    if (nMoveLength < nDataEndOffset)
                    {
                        memmove(m_pcBufferHead + nMoveLength, m_pcBufferHead, nDataEndOffset - nMoveLength);
                    }
                }
            }

            m_nBufferLength = nNewBufferLength;
        }

        // At this point we know for sure that we have enough free space in
        // the buffer. What we don't know is whether there is a single contiguous
        // block, or two blocks.
        int nDataEndOffset = (m_nDataOffset + m_nDataLength)%m_nBufferLength;
        if (nDataEndOffset > m_nDataOffset)
        {
            // The buffer free space is fragmented. We may have to issue
            // two copy operations to deal with the whole buffer.
            int nTrailingBufferLength = m_nBufferLength - nDataEndOffset;
            assert(nTrailingBufferLength > 0);

            int nMoveLength = (nLength > nTrailingBufferLength ? nTrailingBufferLength : nLength);
            memcpy(m_pcBufferHead + nDataEndOffset, pcBuffer, nMoveLength);
            if (nMoveLength < nLength)
                memcpy(m_pcBufferHead, pcBuffer + nMoveLength, nLength - nMoveLength);
        }
        else
        {
            // The buffer free space is not fragmented, therefore
            // a single copy operation will suffice.
            memcpy(m_pcBufferHead + nDataEndOffset, pcBuffer, nLength);
        }

        m_nDataLength += nLength;
    }
}

unsigned char ExpandableRingBuffer::PeekByte() const
{
    if (m_pcBufferHead == NULL || m_nDataLength == 0)
        return '\0';
    return m_pcBufferHead[m_nDataOffset];
}

int ExpandableRingBuffer::ReadData(void* pBuffer, int nLength)
{
    if (m_pcBufferHead == NULL || m_nDataLength == 0)
        return 0;

    unsigned char *pcBuffer = (unsigned char *)pBuffer;

    if (nLength > m_nDataLength)
        nLength = m_nDataLength;

    if (m_nDataOffset + nLength > m_nBufferLength)
    {
        // The data block we're trying to read is fragmented
        // and will require two copy operations.
        int nMoveLength = m_nBufferLength - m_nDataOffset;
        memcpy(pcBuffer, m_pcBufferHead + m_nDataOffset, nMoveLength);
        memcpy(pcBuffer + nMoveLength, m_pcBufferHead, nLength - nMoveLength);
    }
    else
    {
        // The data block is not fragmented and we can accomplish
        // the desired operation with a single copy operation.
        memcpy(pcBuffer, m_pcBufferHead + m_nDataOffset, nLength);
    }

    m_nDataOffset = (m_nDataOffset + nLength)%m_nBufferLength;
    m_nDataLength -= nLength;

    // A special case: when there's no data left in the buffer (we've
    // just read the last of it), reset the data offset to zero.
    if (m_nDataLength == 0)
        m_nDataOffset = 0;

    return nLength;
}

int ExpandableRingBuffer::ReadDataUtilByte(void* pBuffer, int nMaxLength, unsigned char cByte, bool bInclusive)
{
    if (m_pcBufferHead == NULL || m_nDataLength == 0)
        return 0;

    int nByteOffset = OffsetOfByte(cByte);
    if (nByteOffset < 0)
        nByteOffset = m_nDataLength;
    else if (bInclusive)
        ++nByteOffset;

    int nCopyLength = (nMaxLength > nByteOffset ? nByteOffset : nMaxLength);
    return ReadData(pBuffer, nCopyLength);
}

int ExpandableRingBuffer::OffsetOfByte(unsigned char cByte) const
{
    if (m_pcBufferHead == NULL || m_nDataLength == 0)
        return -1;

    if (m_nDataOffset + m_nDataLength > m_nBufferLength)
    {
        // The data block is fragmented. We may need to search twice.
        int nTrailingLength = m_nBufferLength - m_nDataOffset;
        unsigned char *ptr = (unsigned char *)memchr(m_pcBufferHead + m_nDataLength, cByte, nTrailingLength);
        if (ptr == NULL)
        {
            // Let's see what's in the wrapped block
            ptr = (unsigned char *)memchr(m_pcBufferHead, cByte, m_nDataLength - nTrailingLength);
            if (ptr != NULL)
                return ptr - m_pcBufferHead + nTrailingLength;
        }
        else
        {
            return ptr - (m_pcBufferHead + m_nDataOffset);
        }
    }
    else
    {
        // The data block is not fragmented, we just need to search once.
        unsigned char *ptr = (unsigned char *)memchr(m_pcBufferHead + m_nDataOffset, cByte, m_nDataLength);
        if (ptr != NULL)
            return ptr - (m_pcBufferHead + m_nDataOffset);
    }

    return -1;
}


}
