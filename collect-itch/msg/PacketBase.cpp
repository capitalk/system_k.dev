#include "PacketBase.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <boost/algorithm/string.hpp>

namespace itch
{

PacketBase::PacketBase()
        :m_nBufferPos ( 0 )
{
}

PacketBase::PacketBase ( const std::string& strBuffer )
        : m_strBuffer ( strBuffer )
        , m_nBufferPos ( 0 )
{
}

char PacketBase::PeekChar() const
{
    if ( m_nBufferPos >= m_strBuffer.size() )
        return '\0';
    return m_strBuffer[m_nBufferPos];
}

bool PacketBase::ReadChar ( char& c )
{
    if ( m_nBufferPos >= m_strBuffer.size() )
        return false;
    c = m_strBuffer[m_nBufferPos++];
    return true;
}

bool PacketBase::ReadString ( std::string& str, int nLength, bool bTrim )
{
    if ( m_nBufferPos + nLength > m_strBuffer.size() )
        return false;
    str = m_strBuffer.substr ( m_nBufferPos, nLength );
    m_nBufferPos += nLength;

    if (bTrim)
        boost::trim_right(str);
    
    return true;
}

bool PacketBase::ReadLong ( long& num, int nLength )
{
    std::string str;
    if ( !ReadString ( str, nLength ) )
        return false;

    char *pszEndPtr;
    errno = 0;
    num = strtol ( str.c_str(), &pszEndPtr, 10 );

    // Check for non-digits
    if ( pszEndPtr != NULL && *pszEndPtr != '\0' )
        return false;

    // Check for conversion error
    if ( errno != 0 )
        return false;

    return true;
}

bool PacketBase::ReadInt ( int& num, int nLength )
{
    long value;
    if ( !ReadLong ( value, nLength ) )
        return false;
    num = value;
    return true;
}

bool PacketBase::ReadDbl ( double& num, int nLength )
{
    std::string str;
    if ( !ReadString ( str, nLength ) )
        return false;

    char *pszEndPtr;
    errno = 0;
    num = strtod ( str.c_str(), &pszEndPtr );

    // Check for non-digits
    if ( pszEndPtr != NULL && *pszEndPtr != '\0' )
        return false;

    // Check for conversion error
    if ( errno != 0 )
        return false;

    return true;
}

bool PacketBase::ReadUntilTLF ( std::string& str )
{
    // Read everything between the current position and
    // the position before last (which is known to contain
    // the TLF character). Advance the current position to
    // the TLF character afterwards.
    str.clear();

    // Check to see if the current position is already
    // beyond the TLF character.
    if ( m_nBufferPos >= m_strBuffer.size() - 1 )
        return false;
    else if ( m_nBufferPos < m_strBuffer.size() - 1 )
    {
        str = m_strBuffer.substr ( m_nBufferPos, m_strBuffer.size() - m_nBufferPos - 1 );
        m_nBufferPos = m_strBuffer.size() - 1;
    }
    return true;
}


bool PacketBase::WriteChar ( char c )
{
    m_strBuffer += c;
    m_nBufferPos = m_strBuffer.size();
    return true;
}

bool PacketBase::WriteString ( const char *pszString, int nLength )
{
    if ( strlen ( pszString ) > nLength )
        return false;

    char szBuffer[2048];
    if ( nLength < sizeof ( szBuffer ) - 1 )
    {
        // The stack buffer is just enough to fit the
        // padded string.
        sprintf ( szBuffer, "%-*s", nLength, pszString );
        m_strBuffer += szBuffer;
        m_nBufferPos = m_strBuffer.size();
    }
    else
    {
        // The stack buffer is not large enough and we
        // have to allocate the buffer on the heap.
        char *pszBuffer = ( char * ) malloc ( sizeof ( char ) * ( nLength + 1 ) );
        sprintf ( pszBuffer, "%-*s", nLength, pszString );
        m_strBuffer += pszBuffer;
        m_nBufferPos = m_strBuffer.size();
        free ( pszBuffer );
    }

    return true;
}

bool PacketBase::WriteLong ( long num, int nLength )
{
    char szBuffer[64];
    if ( nLength < sizeof ( szBuffer ) - 1 )
    {
        // The stack buffer is just enough to fit the
        // formatted value including any requested padding.
        sprintf ( szBuffer, "%*ld", nLength, num );

        if ( strlen ( szBuffer ) > nLength )
            return false;

        m_strBuffer += szBuffer;
        m_nBufferPos = m_strBuffer.size();
        return true;
    }
    else
    {
        // The stack buffer is not large enough and we have to
        // allocate the buffer on the heap.
        char *pszBuffer = ( char * ) malloc ( sizeof ( char ) * ( nLength + 1 ) );
        sprintf ( pszBuffer, "%*ld", nLength, num );
        bool rc = ( strlen ( pszBuffer ) <= nLength );
        if ( rc )
        {
            m_strBuffer += pszBuffer;
            m_nBufferPos = m_strBuffer.size();
        }
        free ( pszBuffer );
        return rc;
    }
}

void PacketBase::Assign ( const std::string& strBuffer )
{
    m_strBuffer = strBuffer;
    m_nBufferPos = 0;
}

bool PacketBase::ResetReadPosition(int nNewPosition)
{
    if (nNewPosition < 0 || nNewPosition > m_strBuffer.size())
        return false;
    m_nBufferPos = nNewPosition;
    return true;
}

}
