#include "dbConnectionpool.h"
#include <iostream>
#include <muduo/base/Logging.h>

// 连接池的构造
ConnectionPool::ConnectionPool(){
    if (!loadConfigFile()) // 如果加载失败了
    {
        return;
    }
    // 创建初始数量的连接
    for (int i = 0; i < _initSize; i++){
        MySQL *p = new MySQL();
        p->connect();
        p->refreshAliveTime(); // 所有进队列前都要刷新一下开始空闲的起始时间，可以试试在构造函数中是否可行
        _connectionQue.push(p);
        _connectionCnt ++;
    }

    // 启动一个新的线程，作为连接的生产者 linux thread -》 pthread_create
    thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();

    // 启动一个新的定时线程，扫描多余的空闲连接，超过maxIdleTime时间的空闲连接，进行多余的连接回收
    thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}   

ConnectionPool* ConnectionPool::getConnectionPool(){
    static ConnectionPool pool; // lock和unlock
    return &pool;
}

bool ConnectionPool::loadConfigFile(){
    FILE *pf = fopen("mysql.ini", "r"); // 打开文件mysql并设置可读
    if (pf == nullptr){
        LOG_INFO << "mysql.ini is not exit";
        return false;
    }

    while (!feof(pf))    //一直到文件的末尾
    {
        char line[1024] = {0};
        fgets(line, 1024, pf);
        std::string str = line;

        // std::cout << str << std::endl;
        int idx = str.find('=', 0);
        // std::cout << "idx = "  << idx << std::endl;
        if (idx == -1){  //无效的配置项
            continue;
        }

        int endidx = str.find('\n', idx);
        std::string key = str.substr(0, idx);
        std::string value = str.substr(idx + 1, endidx - idx - 1);

        std::cout << "配置文件的内容：" << key << " " << value << std::endl;
        if (key == "ip") { _ip = value; }
        else if (key == "port") { _port = atoi(value.c_str()); }
        else if (key == "username") { _username = value; }
        else if (key == "password") { _password = value; }
        else if (key == "initSite") { _initSize = atoi(value.c_str()); }
        else if (key == "maxSize")  { _maxSize = atoi(value.c_str()); }
        else if (key == "maxIdleTime") { _maxIdleTime = atoi(value.c_str()); }
        else if (key == "ConnectionTimeOut") { _connectionTimeout = atoi(value.c_str()); }
        else if (key == "dbname") { _dbname = value; }
    }
    return true;
}

    // 运行在独立的线程中，专门负责生产新的连接
void ConnectionPool::produceConnectionTask()
{
    std::cout << "i can into and produceConnectionTask" << std::endl;
    for(;;){
        unique_lock<mutex> lock(_queueMutex);
        while (!_connectionQue.empty()){// 当连接池的队列不为空的时候
            std::cout << "我还在等待中 " << std::endl;
            cv.wait(lock);              // 队列不为空的时候，这个线程就为等待状态
        }
        if (_connectionCnt < _maxSize){// 如果线程池的数量小于最大的连接数量的时候,继续创建新的连接
            std::cout << "我要创建一个全新的世界" << std::endl;
            MySQL *p = new MySQL();
            p->connect();
            p->refreshAliveTime();
            _connectionQue.push(p);
            _connectionCnt++;
            std::cout << "i can make and the _connectionCnt = " << _connectionCnt << std::endl;
        }
        // 通知消费者线程可以消费连接了
        cv.notify_all();
    }
}

void ConnectionPool::scannerConnectionTask(){
    for(;;){
        // 通过sleep模拟定时效果
        this_thread::sleep_for(chrono::seconds(_maxIdleTime));

        // 扫描整个队列，释放多余的连接
        unique_lock<mutex> lock(_queueMutex);
        if (_connectionCnt > _initSize){
            MySQL *p = _connectionQue.front();
            if (p->getAliveTime() > (_maxIdleTime * 1000)){
                    _connectionQue.pop();
                    _connectionCnt--;
                    delete p; // 调用~connection()的释放连接
            }
            else{
                break;
            }
        }
    }
}


std::shared_ptr<MySQL> ConnectionPool::getConnection(){

    unique_lock<mutex> lock(_queueMutex);
    while(_connectionQue.empty()){
        // std::cout << "等待的时间是：" << _connectionTimeout << std::endl;
        if(cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout))){
            if(_connectionQue.empty()){//获取空闲连接超时了
            LOG_INFO << "获取空闲连接超时了.....获取连接失败 ";
            return nullptr;
            }
        } 
    }
    // 智能指针析构的时候，会把mysql资源直接delete掉
    // 调用connection的析构函数，connection就被close掉了
    // 这里需要自定义一下shared_prt释放资源的方式，把MySQL资源直接归还到queue中 
    std::shared_ptr<MySQL> sp(_connectionQue.front(),
        [&](MySQL *pcon){
            // 这里是在服务器应用线程中调用的，所以一定要考虑队列的线程安全
            unique_lock<mutex> lock(_queueMutex);
            pcon->refreshAliveTime();// 在进入队列之前都要刷新一下时间
            _connectionQue.push(pcon);
            }
        );
    _connectionQue.pop();
    if (_connectionQue.empty()){ // 谁消费了队列中的最后一个connection，谁负责通知生产者添加一个新的连接
        cv.notify_all();
    }
    return sp;
}