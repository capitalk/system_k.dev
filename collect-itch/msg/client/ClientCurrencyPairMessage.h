#ifndef CLIENTCURRENCYPAIRMESSAGE_H
#define CLIENTCURRENCYPAIRMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class ClientCurrencyPairMessage : public MessageBase
{
public:

    bool Save ( PacketBase& packet ) const;

    void LogMessage ( FILE *pLogFile ) const;

protected:
    const std::string m_strCurrencyPair;

    ClientCurrencyPairMessage ( char cMessageType, const char *pszCurrencyPair );
};

}

#endif // CLIENTCURRENCYPAIRMESSAGE_H
