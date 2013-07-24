#ifndef EXPANDABLERINGBUFFER_H
#define EXPANDABLERINGBUFFER_H

#define DEFAULT_RINGBUFFER_BLOCKSIZE 8192

namespace itch
{

class ExpandableRingBuffer
{
public:
    ExpandableRingBuffer(int nBlockSize = DEFAULT_RINGBUFFER_BLOCKSIZE);
    virtual ~ExpandableRingBuffer();

    void WriteData(const void *pBuffer, int nLength);
    int ReadData(void *pBuffer, int nLength);
    unsigned char PeekByte() const;
    int ReadDataUtilByte(void *pBuffer, int nMaxLength, unsigned char cByte, bool bInclusive=true);
    bool ContainsByte(unsigned char cByte) const {
        return OffsetOfByte(cByte) >= 0;
    }

    int GetDataLength() const {
        return m_nDataLength;
    }

private:
    const int m_nBlockSize;

    unsigned char *m_pcBufferHead;
    int m_nBufferLength;
    int m_nDataOffset;
    int m_nDataLength;

    int OffsetOfByte(unsigned char cByte) const;
};

}

#endif // EXPANDABLERINGBUFFER_H
