/*
 * 基础Socket通讯对象
 * 定义了非阻塞，非延迟，发送和接收接口
 * 定义Socket错误异常
 *
 *  2013-04-12 By 宝爷
 *  
 */
#ifndef __KX_SOCKETV6__
#define __KX_SOCKETV6__

#include "KxSock.h"

namespace KxServer {

class KxSockV6 : public KxSock
{
public:
	// 初始化Socket
	bool init(KXSOCK_TYPE type);
	// 链接指定的IP和端口
	int connect(const char* addr, int port);
	// 绑定到指定的Ip和端口
	int bind(const char* addr, int port);
	// Accept返回一个Socket连接对象
	KXCOMMID accept();
};

}

#endif
