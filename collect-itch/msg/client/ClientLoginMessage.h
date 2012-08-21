#ifndef CLIENTLOGINMESSAGE_H
#define CLIENTLOGINMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class ClientLoginMessage : public MessageBase
{
public:
    static const char TYPE = 'L';

    ClientLoginMessage ( const char *pszUsername, const char *pszPassword, bool bMarketDataUnsubscribe );
    
    bool Save ( PacketBase& packet ) const;
    
    void LogMessage ( FILE *pLogFile ) const;

protected:
    const std::string m_strUsername;
    const std::string m_strPassword;
    const bool m_bMarketDataUnsubscribe;
};

}

#endif // CLIENTLOGINMESSAGE_H
