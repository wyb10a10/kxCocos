/*
 * 处理多连接
 *  
 */
#ifndef __COCOS_CONNECTOR_H__
#define __COCOS_CONNECTOR_H__

#include "KxTCPConnector.h"

class CocosConnector : public KxServer::KxTCPConnector
{
public:
	CocosConnector(int serverId);
	virtual ~CocosConnector();
	int getServerId() { return m_nServerId; }

protected:
	int m_nServerId;
};

#endif
