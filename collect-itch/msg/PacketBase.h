#ifndef PACKETBASE_H
#define PACKETBASE_H

#include <string>

namespace itch
{

class PacketBase
{
public:
    PacketBase();
    PacketBase ( const std::string& strBuffer );

    char PeekChar() const;
    bool ReadChar ( char& c );
    bool ReadString ( std::string& str, int nLength, bool bTrim = true );
    bool ReadInt ( int& num, int nLength );
    bool ReadLong ( long& num, int nLength );
    bool ReadDbl ( double& num, int nLength );

    bool ReadUntilTLF ( std::string& str );

    bool WriteChar ( char c );
    bool WriteString ( const char *pszString, int nLength );
    inline bool WriteString ( const std::string& str, int nLength ) {
        return WriteString ( str.c_str(), nLength );
    }
    bool WriteLong ( long num, int nLength );

    const std::string& GetBuffer() const {
        return m_strBuffer;
    }
    void Assign ( const std::string& strBuffer );
    bool ResetReadPosition(int nNewPosition = 0);

protected:
    std::string m_strBuffer;
    int m_nBufferPos;

};

}

#endif // PACKETBASE_H
