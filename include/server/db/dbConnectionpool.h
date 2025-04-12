#pragma once 

#include <string>
#include <queue>
#include "db.h"
#include <mutex>
#include <memory>
#include <functional>
#include <condition_variable>

class MySQL;
class ConnectionPool
{
public:
    // 获取连接池对象实例
    static ConnectionPool* getConnectionPool();
    // 给外部提供接口，从连接池中获取一个可用的空闲连接
    std::shared_ptr<MySQL> getConnection();
private:
    ConnectionPool();       //单例#1 构造函数私有化
    bool loadConfigFile();  //从配置文件中，加载配置项

    // 运行在独立的线程中，专门负责生产新的连接
    void produceConnectionTask();

    // 扫描超过maxIdleTime时间的空闲连接，进行多余的连接回收
    void scannerConnectionTask();

    std::string _ip;         // MySQL的ip地址
    unsigned short _port;   // MySQL的端口号 3306
    std::string _username;  // MySQL的登录用户名
    std::string _password;  // MySQL的登录密码
    std::string _dbname;    // 连接的数据库的名称
    int _initSize;          // 连接池的初始连接量
    int _maxSize;           // 连接池的最大连接量
    int _maxIdleTime;       // 连接池最大空闲时间
    int _connectionTimeout; // 连接池获取连接的超时时间

    std::queue<MySQL*> _connectionQue;  //存储MySQL连接的队列
    std::mutex _queueMutex;                  // 维护连接队列的线程安全互斥锁
    std::atomic_int _connectionCnt;
    std::condition_variable cv; // 设置条件变量，用于连接生产线程和消费线程之间的通信
};