#include "public.h"

// 定义全局变量（实际分配内存）
bool isMainMenuRunning = false;
sem_t rwsem;
std::atomic_bool g_isLoginSuccess{false};

User g_currentUser;
std::vector<User> g_currentUserFriendList;
std::vector<Group> g_currentUserGroupList;
std::vector<User> g_allUsersList; //全部用户
int firstuserReg = 0;
int userReg = 0;
std::string errno_Msg;

// 初始化信号量（如果需要）
__attribute__((constructor)) 
static void init_globals() {
    sem_init(&rwsem, 0, 0);
}