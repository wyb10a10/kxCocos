#include "KxSockV6.h"
#include "KxLog.h"

namespace KxServer {
	
/*
#if (KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
// inet
const char* inet_ntop_(int af, const void* src, char* dst, int cnt)
{
    struct sockaddr_in srcaddr;

    memset(&srcaddr, 0, sizeof(struct sockaddr_in));
    memcpy(&(srcaddr.sin_addr), src, sizeof(srcaddr.sin_addr));

    srcaddr.sin_family = af;
    if (WSAAddressToStringA((struct sockaddr*) &srcaddr, sizeof(struct sockaddr_in), 0, dst, (LPDWORD)&cnt) != 0)
    {
        return nullptr;
    }
    return dst;
}
#endif*/

bool KxSockV6::init(KXSOCK_TYPE type)
{
    if (KXINVALID_COMMID != m_Sock)
    {
        return false;
    }

    m_SockType = type;
	switch (m_SockType)
	{
	case KXSOCK_TCP:
	{
		return true;
	}

	case KXSOCK_UDP:
		m_Sock = socket(AF_INET6, SOCK_DGRAM, 0);
		break;

	default:
		break;
	}
    if (m_Sock == KXINVALID_COMMID)
    {
        echoSockError("init");
        return false;
    }
	return true;
}

int KxSockV6::connect(const char* addr, int port)
{
	KXLOGDEBUG("debug: KxSockV6::connect %s : %d socket %d", addr, port, m_Sock);
	addrinfo *allres;
	addrinfo *res;
	addrinfo hints;
	int ret = 0;
	char szPort[16] = {};
	snprintf(szPort, sizeof(szPort), "%d", port);
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	int airet = getaddrinfo(addr, szPort, &hints, &allres);

	for (res = allres; res != NULL; res = allres->ai_next)
	{
		m_Sock = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (m_IsNonBlock)
		{
			setSockNonblock();
		}
		setSockKeepAlive();
		ret = ::connect(m_Sock, res->ai_addr, res->ai_addrlen);
		// 如果非阻塞, 应该要连接到ret==0才返回
		break;
	}
	freeaddrinfo(allres);
	return ret;
}

int KxSockV6::bind(const char* addr, int port)
{
	int ret = 0;
	addrinfo *res;
	addrinfo addrInfo;
	memset(&addrInfo, 0, sizeof(addrInfo));
	addrInfo.ai_flags = AI_PASSIVE;
	addrInfo.ai_family = AF_INET6;
	addrInfo.ai_socktype = SOCK_STREAM;
	addrInfo.ai_protocol = 0;
	char szPort[16] = {};
	snprintf(szPort, sizeof(szPort), "%d", port);
	getaddrinfo(addr, szPort, &addrInfo, &res);
	ret = ::bind(m_Sock, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);
	if (ret < 0)
	{
		echoSockError("bind");
	}
	return ret;
}

KXCOMMID KxSockV6::accept()
{
	sockaddr_in6 name;
	socklen_t len = sizeof(sockaddr_in6);
    KXCOMMID ret = ::accept(m_Sock, (sockaddr*)&name, &len);
    if (ret != KXINVALID_COMMID)
	{
		char addrStr[INET6_ADDRSTRLEN] = {};
		inet_ntop(name.sin6_family, name.sin6_addr.s6_addr, addrStr, INET6_ADDRSTRLEN);
		KXLOGDEBUG("debug: KxSockV6::accept ip %s prot %d socketId %d", addrStr, name.sin6_port, ret);
    }
    else
    {
        echoSockError("accept");
    }
    return ret;
}

}
