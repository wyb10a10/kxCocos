#include "NetworkModule.h"
#include "Protocol.h"
#include "GameNetworkNode.h"
#include "LuaBase.h"
#include "CocosConnector.h"

using namespace KxServer;
using namespace KxCocos;

CNetworkModule::CNetworkModule(void)
{
	m_pBuffData = new CBufferData();
}

CNetworkModule::~CNetworkModule(void)
{
	delete m_pBuffData;
}

void CNetworkModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
    // 收到数据
    Head *head = reinterpret_cast<Head*>(buffer);
	CCLOG("Process %d %d", head->MainCommand(), head->SubCommand());

	CocosConnector *pConnector = dynamic_cast<CocosConnector*>(target);
	if (nullptr != pConnector)
	{
		m_pBuffData->init(buffer, len);
		onLuaRespone(pConnector->getServerId(), m_pBuffData);
		m_pBuffData->clean();
	}
}

void CNetworkModule::processError(IKxComm *target)
{
	//断线事件
	onLuaErrorEvent();
}

void CNetworkModule::processEvent(int eventId, IKxComm* target)
{
    CGameNetworkNode* net = CGameNetworkNode::getInstance();
    ServerConn* pServerConn = net->getServerConn(0);
    if (nullptr == pServerConn)
    {
        return;
    }

	bool success = eventId != KXEVENT_CONNECT_FAILE;
	if (pServerConn->ConnectCallback != nullptr)
	{
		pServerConn->ConnectCallback(success);
		pServerConn->ConnectCallback = nullptr;
	}
}
