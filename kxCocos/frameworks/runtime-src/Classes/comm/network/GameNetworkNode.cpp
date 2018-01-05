#include "GameNetworkNode.h"
#include <string.h>
#include "CommTools.h"
#include "networkNode/NetworkModule.h"
#include "CocosConnector.h"

using namespace std;
using namespace KxServer;
USING_NS_CC;

namespace KxCocos {

CGameNetworkNode::CGameNetworkNode(void)
: m_pPoller(NULL)
, m_pDefaultModule(NULL)
{
}

CGameNetworkNode::~CGameNetworkNode(void)
{
	KXSAFE_RELEASE(m_pPoller);
	KXSAFE_RELEASE(m_pDefaultModule);
	std::map<int, KxServer::IKxModule*>::iterator it = m_mapKxModule.begin();
	for (; it != m_mapKxModule.end(); ++it)
	{
		KXSAFE_RELEASE(it->second);
	}
}

CGameNetworkNode *CGameNetworkNode::m_pInstance = NULL;
CGameNetworkNode *CGameNetworkNode::getInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new CGameNetworkNode();
        if (!m_pInstance->init())
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }

    return m_pInstance;
}

void CGameNetworkNode::destroy()
{
    if (NULL != m_pInstance)
    {
        m_pInstance->removeFromParent();
        m_pInstance->release();
    }
}

bool CGameNetworkNode::init()
{
    m_pPoller = new KxSelectPoller();
    return true;
}

void CGameNetworkNode::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    Node::visit(renderer, parentTransform, parentFlags);
   if (NULL != m_pPoller)
   {
       // 轮询网络
       m_pPoller->poll();
   }
}

bool CGameNetworkNode::connect(const char* ip, int port, int id, bool ipv6,
	std::function<void(bool)> callBack)
{
	map<int, ServerConn>::iterator iter = m_mapServerConns.find(id);
	if (iter != m_mapServerConns.end())
	{
		return false;
	}

	CocosConnector* pConnector = new CocosConnector(id);
	if((ipv6 && !pConnector->initV6())
	|| (!ipv6 && !pConnector->init()))
	{
		CCLOGERROR("error: m_pConnector init faile %d", pConnector->getCommId());
		SAFE_RELEASE(pConnector);
		return false;
	}

	// 设置回调模块
	pConnector->setModule(getKxModule(id));

	ServerConn conn;
	conn.Ipv6 = ipv6;
	conn.Connector = pConnector;
	conn.ConnectCallback = callBack;
	conn.ConnIP = ip;
	conn.Port = port;
	m_mapServerConns[id] = conn;

	if (!pConnector->isConnecting()
		&& !pConnector->connect(ip, port))
	{
		CCLOGERROR("error: m_pConnector connect faile %d", pConnector->getCommId());
		SAFE_RELEASE(pConnector);
		// 连接失败则应该置Connector为空
		m_mapServerConns[id].Connector = nullptr;
		return false;
	}

	m_pPoller->addCommObject(pConnector, pConnector->getPollType());
	return true;
}

bool CGameNetworkNode::reconnect(int id, std::function<void(bool)> callBack)
{
	if (m_mapServerConns.find(id) == m_mapServerConns.end())
	{
		return false;
	}

	// 使用引用
	ServerConn& connInfo = m_mapServerConns[id];
	connInfo.ConnectCallback = callBack;
	KxTCPConnector *pConnector = connInfo.Connector;
	if (nullptr != pConnector)
	{
		connInfo.ConnectingTimes += 1;
		// 正在重连中，别催我
		if (pConnector->isConnecting() && connInfo.ConnectingTimes < 8)
		{
			CCLOG("reconnectToServer But Connector isConnecting");
			return true;
		}
		else
		{
			// 释放，清理资源
			if (pConnector->getPoller() != nullptr)
			{
				pConnector->getPoller()->removeCommObject(pConnector);
			}
			pConnector->setModule(NULL);
			pConnector->close();
			SAFE_RELEASE(pConnector);
			m_mapServerConns[id].Connector = nullptr;
		}
	}

	connInfo.ConnectingTimes = 0;
	CocosConnector* pNewConnector = new CocosConnector(id);
	if ((connInfo.Ipv6 && !pNewConnector->initV6())
		|| (!connInfo.Ipv6 && !pNewConnector->init()))
	{
		SAFE_RELEASE(pNewConnector);
		return false;
	}

	// 设置回调模块
	pNewConnector->setModule(getKxModule(id));
	if (!pNewConnector->connect(connInfo.ConnIP.c_str(), connInfo.Port))
	{
		SAFE_RELEASE(pNewConnector);
		return false;
	}

	m_pPoller->addCommObject(pNewConnector, pNewConnector->getPollType());
	connInfo.Connector = pNewConnector;
	return true;
}

void CGameNetworkNode::close(int id)
{
	map<int, ServerConn>::iterator iter = m_mapServerConns.find(id);
	if (iter != m_mapServerConns.end())
	{
		KxTCPConnector*& pConnector = iter->second.Connector;
		if (nullptr != pConnector)
		{
			if (pConnector->getPoller() != nullptr)
			{
				pConnector->getPoller()->removeCommObject(pConnector);
			}
			pConnector->close();
			SAFE_RELEASE(pConnector);
		}
		m_mapServerConns.erase(iter);
	}
}

void CGameNetworkNode::closeAll()
{
	map<int, ServerConn>::iterator iter = m_mapServerConns.begin();
	for (; iter != m_mapServerConns.end(); ++iter)
	{
		KxTCPConnector*& pConnector = iter->second.Connector;
		if (nullptr != pConnector)
		{
			if (pConnector->getPoller() != nullptr)
			{
				pConnector->getPoller()->removeCommObject(pConnector);
			}
			pConnector->close();
			SAFE_RELEASE(pConnector);
		}
	}
	m_mapServerConns.clear();
}

int CGameNetworkNode::sendData(char* buffer, unsigned int len, int id)
{
    KxTCPConnector*pConnector = getConnector(id);
    if (nullptr != pConnector && !pConnector->isConnecting())
    {
        return pConnector->sendData(buffer, len);
    }
    return -1;
}

KxTCPConnector *CGameNetworkNode::getConnector(int id)
{
    map<int, ServerConn>::iterator iter = m_mapServerConns.find(id);
    if (iter != m_mapServerConns.end())
    {
        return iter->second.Connector;
    }

    return nullptr;
}

ServerConn* CGameNetworkNode::getServerConn(int id)
{
    map<int, ServerConn>::iterator iter = m_mapServerConns.find(id);
    if (iter != m_mapServerConns.end())
    {
        return &(iter->second);
    }
    return nullptr;
}

void CGameNetworkNode::setDefaultModule(KxServer::IKxModule* module)
{
	KXSAFE_RELEASE(m_pDefaultModule);
	m_pDefaultModule = module;
	KXSAFE_RETAIN(m_pDefaultModule);
}

void CGameNetworkNode::setModuleById(int id, KxServer::IKxModule* module)
{
	m_mapKxModule[id] = module;
}

KxServer::IKxModule* CGameNetworkNode::getKxModule(int id)
{
	std::map<int, KxServer::IKxModule*>::iterator it = m_mapKxModule.find(id);
	if (it != m_mapKxModule.end())
	{
		return it->second;
	}

	return m_pDefaultModule;
}

}