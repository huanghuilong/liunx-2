#pragma once
#include "public.h"
#include "AddFriendDialog.h"
#include <QMainWindow>
#include <QListWidget>
#include <QSplitter>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QScrollBar>
#include <QDateTime>
#include <QFont>
#include <QTimer>
#include <QApplication>
#include <QWidgetList>
#include <map>
#include <vector>
#include "EditInfoDialog.h"
#include "PasswordVerifyDialog.h"
#include "CreateGroupDialog.h"


// 聊天消息结构
struct ChatMessage {
    QString senderName;
    QString message;
    QString time;
    bool isSelf;
};

// 主窗口类，继承自QMainWindow  用来显示聊天窗口
class MainWindow : public QMainWindow {
    Q_OBJECT  // Qt宏，启用信号槽机制
    
public:
    explicit MainWindow(const json &responsejs, int clientfd, QWidget *parent = nullptr);  // 构造函数
    
    // 更新用户数据
    void updateUserData(const User &currentUser, 
                      const std::vector<User> &friends,
                      const std::vector<Group> &groups,
                      const std::vector<User> &AllUser);// 初始化更新用户数据

    // 接收消息的槽函数
    void onReceiveMessage(const QString &senderName, const QString &message, const QString &time, bool isSelf = false);
    void onGroupMessageReceived(const QString &senderName, const QString &message, const QString &time,const int senderId, bool isSelf);

private slots:
    // 槽函数 - 点击好友项
    void onFriendItemClicked(QListWidgetItem *item);
    // 槽函数 - 点击群组项
    void onGroupItemClicked(QListWidgetItem *item);
    // 槽函数 - 点击发送按钮
    void onSendButtonClicked();
    // 为搜索框设计的槽函数
    void onSearchTextChanged(const QString &text);
    void onAddFriendClicked(const User &user, const QString &message);
    void onSearchResultClicked(QListWidgetItem *item);
    // 添加设置按钮点击槽函数
    void onSettingsClicked();

    // 发送创建群组的请求
    void sendCreateGroupRequest(const QString &name, const QString &desc);

private:
    // 初始化UI界面
    void setupUI();
    // 清空聊天区域
    void clearChatArea();
    // 显示聊天历史记录
    void displayChatHistory(int id, bool isGroup);
    // 用来更新搜索出来的用户
    void updateSearchResults(const QString &keyword); 
    //用来添加左边的用户框和群组聊天框
    void createFriendItem(QListWidget *list, const User &user, bool isFriend);
    void createGroupItem(QListWidget *list, const Group &group, bool isFriend);
    // 显示消息
    void displayMessage(const ChatMessage &msg);


    void showSettingsDialog();
    void showAddFriendDialog(const User &user);
    void showEditInfoDialog();

    // UI组件
    QSplitter *mainSplitter;    // 主分割器，用于分割左右面板
    QWidget *leftPanel;         // 左侧联系人面板
    QWidget *rightPanel;        // 右侧聊天面板
    
    QListWidget *contactList;   // 联系人列表
    
    QWidget *titleWidget;        // 聊天对方信息窗口
    QHBoxLayout *titleLayout;     // 聊天对方信息容器 
    QLabel *currentChatLabel;   // 当前聊天标题
    QPushButton *createGroupBtn; // 创建群组按钮

    QTextEdit *chatDisplay;     // 聊天显示区域

    QWidget *inputPanel;        // 输入面板   
    QLineEdit *messageInput;    // 消息输入框
    QPushButton *sendButton;    // 发送按钮
    QPushButton *settingsButton;//设置按钮
    // 聊天框的按钮
    QLineEdit *searchInput;
    QListWidget *searchResults;
    
    int clientfd;

    const json &responsejs;           //传输进来的json消息
    
    // 数据成员
    User currentUser;           // 当前用户
    std::vector<User> AllUser;  // 所有的用户
    std::vector<User> friends;  // 好友列表
    std::vector<Group> groups;  // 群组列表
    
    int currentChatId = -1;     // 当前聊天ID(好友ID或群组ID)
    bool isGroupChat = false;   // 是否是群聊

    // 聊天历史记录
    std::map<int, std::vector<ChatMessage>> chatHistory;  // 存储所有聊天记录
};
