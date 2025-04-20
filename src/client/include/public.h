// #pragma once
// #include "json.hpp"
// #include <iostream>
// #include <thread>
// #include <string>
// #include <vector>
// #include <chrono>
// #include <ctime>
// #include <unordered_map>
// #include <functional>
// using namespace std;
// using json = nlohmann::json;

// #include <unistd.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <semaphore.h>
// #include <atomic>

// #include "group.hpp"
// #include "user.hpp"
// #include "public.hpp"

// #include <QApplication>
// #include <QWidget>
// #include <QVBoxLayout>
// #include <QFormLayout>
// #include <QLineEdit>
// #include <QPushButton>
// #include <QMessageBox>
// #include <QDebug>

// #include <QLabel>
// #include <QSpacerItem>
// #include <QCheckBox>
// #include <QFrame>
// #include <iostream>
// #include <string>

// #include <QMainWindow>
// #include <QListWidget>
// #include <QSplitter>
// #include <QHBoxLayout>
// #include <QTextEdit>
// #include <QScrollBar>
// #include <QDateTime>
// #include <QFont>
// #include <QTimer>

// // 控制主菜单页面程序
// bool isMainMenuRunning = false;

// // 用于读写线程之间的通信
// sem_t rwsem;
// // 记录登录状态
// atomic_bool g_isLoginSuccess{false};


// // 接收线程
// void readTaskHandler(int clientfd);
// // 获取系统时间（聊天信息需要添加时间信息）
// string getCurrentTime();
// // 主聊天页面程序
// void mainMenu(int);
// // 显示当前登录成功用户的基本信息
// void showCurrentUserData();

// // 聊天界面的定义
// User g_currentUser;                         // 当前登录用户
// std::vector<User> g_currentUserFriendList;  // 当前用户的好友列表
// std::vector<Group> g_currentUserGroupList;  // 当前用户的群组列表



#pragma once
#include "json.hpp"
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <unordered_map>
#include <functional>
using namespace std;
using json = nlohmann::json;

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <atomic>

#include "group.hpp"
#include "user.hpp"
#include "public.hpp"

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QSpacerItem>
#include <QCheckBox>
#include <QFrame>
#include <QMainWindow>
#include <QListWidget>
#include <QSplitter>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QScrollBar>
#include <QDateTime>
#include <QFont>
#include <QTimer>

// 声明全局变量（不分配内存）
extern bool isMainMenuRunning;
extern sem_t rwsem;
extern std::atomic_bool g_isLoginSuccess;

// 用户相关全局变量声明 
extern User g_currentUser;
extern std::vector<User> g_currentUserFriendList;
extern std::vector<Group> g_currentUserGroupList;
extern std::vector<User> g_allUsersList;           // 所有用户列表（模拟数据库）
extern std::vector<Group> g_allGroupsList;         // 所有群组列表（模拟数据库）
extern int userReg;
extern int firstuserReg;

// 函数声明
void readTaskHandler(int clientfd);
string getCurrentTime();
void mainMenu(int clientfd);
void showCurrentUserData();