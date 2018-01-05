/*
*   负责网络的初始化, 网络状态监视, 以及网络轮询
*	管理网络连接
*
*/
#ifndef __GAMENETWORKNODE_H__
#define __GAMENETWORKNODE_H__

#include <functional>
#include "cocos2d.h"
#include "KxServer.h"

namespace KxCocos {

struct ServerConn
{
	bool						Ipv6;				// 是否IPV6
    std::string                 ConnIP;				// 连接的ip地址
    int                         Port;               // 连接的端口
    int                         ConnectingTimes;    // 正在重连中的次数（大于10次强制关闭重启）
    KxServer::KxTCPConnector*   Connector;          // 连接
    std::function<void(bool)>   ConnectCallback;

    ServerConn() :Port(0)
        , ConnectingTimes(0)
        , Connector(nullptr)
        , ConnectCallback(nullptr)
    {
    }
};

class CGameNetworkNode : public cocos2d::Node
{
private:
    CGameNetworkNode(void);
    ~CGameNetworkNode(void);

public:
    static CGameNetworkNode *getInstance();
    static void destroy();

	// 节点回调
    bool init();
    void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags);

	// 连接管理
	bool connect(const char* ip, int port, int id = 0, bool ipv6 = false, std::function<void(bool)> callBack = nullptr);
	bool reconnect(int id, std::function<void(bool)> callBack = nullptr);
	void close(int id);
	void closeAll();

	int sendData(char* buffer, unsigned int len, int id = 0);
	KxServer::KxTCPConnector* getConnector(int id);
	ServerConn* getServerConn(int id);

	// Module管理
	void setDefaultModule(KxServer::IKxModule* module);
	void setModuleById(int id, KxServer::IKxModule* module);

private:
	KxServer::IKxModule* getKxModule(int id);

private:
    KxServer::IKxCommPoller*            m_pPoller;                      // 轮询器
	KxServer::IKxModule*				m_pDefaultModule;				// 默认处理模块
	std::map<int, KxServer::IKxModule*> m_mapKxModule;					// 处理模块
    std::map<int, ServerConn>           m_mapServerConns;               // 服务器连接
	static CGameNetworkNode*            m_pInstance;
};
}

#endif 
