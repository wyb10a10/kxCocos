/*
* 通讯协议 主要定义主命令和包头
*
*/

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

enum NetMainCMD
{
    CMD_LOGIN = 1,      // 登录
    CMD_USER,		    // 用户数据
};

inline int MakeCommand(int main, int sub)
{
    return (main << 16) | (sub & 0x0000ffff);
}

struct Head
{
    int length;
    int cmd;
    int id;

    inline void MakeCommand(int main, int sub)
    {
        cmd = (main << 16) | (sub & 0x0000ffff);
    }

    inline int SubCommand()
    {
        return cmd & 0x0000ffff;
    }

    inline int MainCommand()
    {
        return cmd >> 16;
    }

    inline void* data()
    {
        return this + 1;
    }
};

#endif
