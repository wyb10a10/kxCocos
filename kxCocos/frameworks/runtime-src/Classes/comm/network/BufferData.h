/*
* 数据缓冲区对象
*
* 2015-5-21 By 宝爷
*/
#ifndef __BUFFERDATA_H__
#define __BUFFERDATA_H__

#include <stdio.h>
#include <memory.h>

class CBufferData
{
public:
    CBufferData();
    CBufferData(const CBufferData &data);
    virtual ~CBufferData();

    CBufferData &operator=(const CBufferData &data);

    bool init(int bufferSize);
    bool init(char* buffer, unsigned int dataLength);
    bool init(CBufferData* pBuffData);

    void clean();

    template<typename T>
    bool writeData(T data)
    {
        if (!checkBufferSize(sizeof(data)))
        {
            return false;
        }
        else
        {
            memcpy(m_Buffer + m_DataLength, &data, sizeof(data));
            m_DataLength += sizeof(data);
            return true;
        }
    }
    bool writeData(const void* data, unsigned int length);
	// 更新一块空的buffer，主要用于占位使用
	bool writeEmptyBuffer(unsigned int length);

    template<typename T>
    bool readData(T& data)
    {
        if (!checkDataLength(sizeof(data)))
        {
            return false;
        }

        memcpy(&data, m_Buffer + m_Offset, sizeof(data));
        m_Offset += sizeof(data);
        return true;
    }
    bool readData(void* data, unsigned int dataSize);

    inline void resetOffset() { m_Offset = 0; }
    inline void setIsReadModel(bool isReadModel){ m_IsReadMode = isReadModel; }
    inline void updateOffset(unsigned int offset) { m_Offset = offset; }
    inline unsigned int getOffset() { return m_Offset; }
    inline unsigned int getDataLength() { return m_DataLength; }
    inline unsigned int getBufferSize() { return m_BufferSize; }

	char *getBuffer() const { return m_Buffer; };
    void resetDataLength() { m_Offset = 0; m_DataLength = 0; }

private:
    bool checkDataLength(unsigned int dataLength);
    bool checkBufferSize(unsigned int newLength);

private:
    bool m_IsInit;              // 防止重复初始化
    bool m_IsReadMode;          // 读写模式
    unsigned int m_Offset;      // 当前读取偏移
    unsigned int m_DataLength;  // 当前实际数据长度
    unsigned int m_BufferSize;  // 缓冲区大小
    char* m_Buffer;
};

#endif
