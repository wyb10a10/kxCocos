#ifndef __NETWORK_MODULE_H__
#define __NETWORK_MODULE_H__

#include "BaseModule.h"
#include "BufferData.h"

class CNetworkModule : public CBaseModule
{
public:
	CNetworkModule(void);
	~CNetworkModule(void);

    // 处理收到的数据
    virtual void processLogic(char* buffer, unsigned int len, KxServer::IKxComm *target);
    // 处理出现的错误
    virtual void processError(KxServer::IKxComm *target);
    // 处理事件
    virtual void processEvent(int eventId, KxServer::IKxComm* target);

private:
	CBufferData *m_pBuffData;
};

#endif 
