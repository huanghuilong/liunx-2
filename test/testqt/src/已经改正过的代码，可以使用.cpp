#pragma once
#include "public.h"
// 主窗口类，继承自QMainWindow  用来显示聊天窗口
class MainWindow : public QMainWindow {
    Q_OBJECT  // Qt宏，启用信号槽机制
    
public:
    explicit MainWindow(int clientfd, QWidget *parent = nullptr);  // 构造函数
    
    // 更新用户数据
    void updateUserData(const User &currentUser, 
                      const std::vector<User> &friends,
                      const std::vector<Group> &groups);

private slots:
    // 槽函数 - 点击好友项
    void onFriendItemClicked(QListWidgetItem *item);
    // 槽函数 - 点击群组项
    void onGroupItemClicked(QListWidgetItem *item);
    // 槽函数 - 点击发送按钮
    void onSendButtonClicked();

private:
    // 初始化UI界面
    void setupUI();
    // 清空聊天区域
    void clearChatArea();
    // 显示聊天历史记录
    void displayChatHistory(int id, bool isGroup);

    // UI组件
    QSplitter *mainSplitter;    // 主分割器，用于分割左右面板
    QWidget *leftPanel;         // 左侧联系人面板
    QWidget *rightPanel;        // 右侧聊天面板
    
    QListWidget *contactList;   // 联系人列表
    
    QTextEdit *chatDisplay;     // 聊天显示区域
    QLineEdit *messageInput;    // 消息输入框
    QPushButton *sendButton;    // 发送按钮
    
    QLabel *currentChatLabel;   // 当前聊天标题
    int clientfd;
    
    // 数据成员
    User currentUser;           // 当前用户
    std::vector<User> friends;  // 好友列表
    std::vector<Group> groups;  // 群组列表
    
    int currentChatId = -1;     // 当前聊天ID(好友ID或群组ID)
    bool isGroupChat = false;   // 是否是群聊
};
#include "MainWindow.h"
#include <QDebug>
// *****************************************聊天界面的实现****************************
// 主窗口构造函数
MainWindow::MainWindow(int clientfd, QWidget *parent) : QMainWindow(parent), clientfd(clientfd) {
    setupUI();  // 初始化UI
    
    setWindowTitle("简约聊天");  // 设置窗口标题
    resize(900, 600);           // 设置窗口大小
    
    // 设置窗口背景色
    setStyleSheet("background-color: #f8f9fa;");
}

// 初始化UI界面
void MainWindow::setupUI() {
    // 主分割布局 - 水平分割
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(1);  // 设置分割线宽度
    
    // 左侧联系人面板
    leftPanel = new QWidget;
    leftPanel->setMinimumWidth(200);  // 设置最小宽度
    leftPanel->setMaximumWidth(300);  // 设置最大宽度
    leftPanel->setStyleSheet("background-color: #ffffff; border-right: 1px solid #e9ecef;");
    
    // 左侧面板布局 - 垂直布局
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);  // 设置边距
    leftLayout->setSpacing(0);                   // 设置间距
    
    // 联系人列表
    contactList = new QListWidget;
    contactList->setStyleSheet(
        "QListWidget { border: none; background: transparent; }"
        "QListWidget::item { height: 60px; border-bottom: 1px solid #e9ecef; padding: 8px; }"
        "QListWidget::item:hover { background-color: #f1f3f5; }"
        "QListWidget::item:selected { background-color: #e9ecef; }"
    );
    
    leftLayout->addWidget(contactList);  // 将联系人列表添加到左侧布局
    
    // 右侧聊天面板
    rightPanel = new QWidget;
    rightPanel->setStyleSheet("background-color: #ffffff;");
    
    // 右侧面板布局 - 垂直布局
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    
    // 当前聊天标题
    currentChatLabel = new QLabel("选择联系人开始聊天");
    currentChatLabel->setStyleSheet(
        "font-size: 16px; font-weight: 500; color: #212529;"
        "padding: 12px 16px; border-bottom: 1px solid #e9ecef;"
    );
    
    // 聊天显示区域
    chatDisplay = new QTextEdit;
    chatDisplay->setReadOnly(true);  // 设置为只读
    chatDisplay->setStyleSheet(
        "QTextEdit { border: none; background-color: #f8f9fa; padding: 16px; }"
    );
    
    // 输入区域面板
    QWidget *inputPanel = new QWidget;
    inputPanel->setStyleSheet("background-color: #ffffff; border-top: 1px solid #e9ecef;");
    
    // 输入区域布局 - 水平布局
    QHBoxLayout *inputLayout = new QHBoxLayout(inputPanel);
    inputLayout->setContentsMargins(16, 8, 16, 8);
    
    // 消息输入框
    messageInput = new QLineEdit;
    messageInput->setPlaceholderText("输入消息...");
    messageInput->setStyleSheet(
        "QLineEdit { border: 1px solid #e9ecef; border-radius: 4px; padding: 8px; }"
    );
    
    // 发送按钮
    sendButton = new QPushButton("发送");
    sendButton->setFixedWidth(80);  // 固定宽度
    sendButton->setStyleSheet(
        "QPushButton { background-color: #4263eb; color: white; border-radius: 4px; }"
        "QPushButton:hover { background-color: #3b5bdb; }"
    );
    
    // 将输入框和按钮添加到输入布局
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);
    
    // 将各个组件添加到右侧布局
    rightLayout->addWidget(currentChatLabel);
    rightLayout->addWidget(chatDisplay, 1);  // 聊天区域可伸缩
    rightLayout->addWidget(inputPanel);
    
    // 将左右面板添加到主分割器
    mainSplitter->addWidget(leftPanel);
    mainSplitter->addWidget(rightPanel);
    
    // 设置中心部件为主分割器
    setCentralWidget(mainSplitter);
    
    qDebug() << "i can into uiset";
    // 连接信号槽
    // 联系人列表项点击信号
    connect(contactList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (item->data(Qt::UserRole + 1).toBool()) {
            onGroupItemClicked(item);  // 如果是群组项
        } else {
            qDebug() << "i can into else and into onFriendItemClicked ";
            onFriendItemClicked(item); // 如果是好友项
        }
    });
    
    // 发送按钮点击信号
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    
    // 输入框回车信号
    connect(messageInput, &QLineEdit::returnPressed, this, &MainWindow::onSendButtonClicked);
}

// 更新用户数据
void MainWindow::updateUserData(const User &currentUser, 
                              const std::vector<User> &friends,
                              const std::vector<Group> &groups) {
    // 保存用户数据
    this->currentUser = currentUser;
    this->friends = friends;
    this->groups = groups;
    
    contactList->clear();  // 清空联系人列表
    
    // 添加好友到联系人列表
    for (const User &user : friends) {
        QListWidgetItem *item = new QListWidgetItem;
        // 设置项数据 - 用户ID和类型标记(好友)
        item->setData(Qt::UserRole, user.getId());
        item->setData(Qt::UserRole + 1, false); // false表示好友
        
        // 创建自定义项部件
        QWidget *itemWidget = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout(itemWidget);
        layout->setContentsMargins(8, 8, 8, 8);
        
        // 头像标签
        QLabel *avatar = new QLabel;
        avatar->setFixedSize(40, 40);
        avatar->setStyleSheet(
            QString("background-color: %1; border-radius: 20px; color: white;"
                   "font-weight: bold; font-size: 16px; qproperty-alignment: AlignCenter;")
            .arg(user.getState() == "online" ? "#4263eb" : "#868e96")  // 根据在线状态设置颜色
        );
        // 显示用户名的第一个字母
        avatar->setText(QString::fromStdString(user.getName()).left(1).toUpper());
        
        // 用户名标签
        QLabel *nameLabel = new QLabel(QString::fromStdString(user.getName()));
        nameLabel->setStyleSheet("font-size: 14px; color: #212529;");
        
        // 在线状态指示器
        QLabel *status = new QLabel;
        status->setFixedSize(8, 8);
        status->setStyleSheet(
            QString("background-color: %1; border-radius: 4px;")
            .arg(user.getState() == "online" ? "#40c057" : "#adb5bd")
        );
        
        // 将组件添加到布局
        layout->addWidget(avatar);
        layout->addWidget(nameLabel, 1);  // 用户名标签可伸缩
        layout->addWidget(status);
        
        // 设置项大小并添加到列表
        item->setSizeHint(itemWidget->sizeHint());
        contactList->addItem(item);
        contactList->setItemWidget(item, itemWidget);
    }
    
    // 添加群组到联系人列表
    for (const Group &group : groups) {
        QListWidgetItem *item = new QListWidgetItem;
        // 设置项数据 - 群组ID和类型标记(群组)
        item->setData(Qt::UserRole, group.getId());
        item->setData(Qt::UserRole + 1, true); // true表示群组
        
        // 创建自定义项部件
        QWidget *itemWidget = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout(itemWidget);
        layout->setContentsMargins(8, 8, 8, 8);
        
        // 群组头像
        QLabel *avatar = new QLabel;
        avatar->setFixedSize(40, 40);
        avatar->setStyleSheet(
            "background-color: #fcc419; border-radius: 20px; color: white;"
            "font-weight: bold; font-size: 16px; qproperty-alignment: AlignCenter;"
        );
        // 显示群组名的第一个字母
        avatar->setText(QString::fromStdString(group.getName()).left(1).toUpper());
        
        // 群组信息垂直布局
        QVBoxLayout *textLayout = new QVBoxLayout;
        textLayout->setSpacing(2);
        
        // 群组名标签
        QLabel *nameLabel = new QLabel(QString::fromStdString(group.getName()));
        nameLabel->setStyleSheet("font-size: 14px; color: #212529;");
        
        // 群组描述标签
        QLabel *descLabel = new QLabel(QString::fromStdString(group.getDesc()));
        descLabel->setStyleSheet("font-size: 12px; color: #868e96;");
        
        // 将标签添加到垂直布局
        textLayout->addWidget(nameLabel);
        textLayout->addWidget(descLabel);
        
        // 将头像和文本布局添加到主布局
        layout->addWidget(avatar);
        layout->addLayout(textLayout, 1);  // 文本布局可伸缩
        
        // 设置项大小并添加到列表
        item->setSizeHint(itemWidget->sizeHint());
        contactList->addItem(item);
        contactList->setItemWidget(item, itemWidget);
    }
}

// 点击好友项槽函数
void MainWindow::onFriendItemClicked(QListWidgetItem *item) {
    // 获取好友ID
    std::cout << "::::::::::::::::::::::::::::::::::::::::::::";
    std::cout << "i can into here" << std::endl;
    int friendId = item->data(Qt::UserRole).toInt();
    currentChatId = friendId;
    isGroupChat = false;
    
    // 查找好友名
    QString friendName;
    for (const User &user : friends) {
        if (user.getId() == friendId) {
            friendName = QString::fromStdString(user.getName());
            break;
        }
    }
    // std::cout << "friendName = " << friendName << std::endl;
    qDebug() << "friendName = " << friendName;
    // 更新当前聊天标题
    currentChatLabel->setText(friendName);
    clearChatArea();  // 清空聊天区域
    displayChatHistory(friendId, false);  // 显示聊天历史
}

// 点击群组项槽函数
void MainWindow::onGroupItemClicked(QListWidgetItem *item) {
    // 获取群组ID
    int groupId = item->data(Qt::UserRole).toInt();
    currentChatId = groupId;
    isGroupChat = true;
    
    // 查找群组名
    QString groupName;
    for (const Group &group : groups) {
        if (group.getId() == groupId) {
            groupName = QString::fromStdString(group.getName());
            break;
        }
    }
    
    // 更新当前聊天标题
    currentChatLabel->setText(groupName);
    clearChatArea();  // 清空聊天区域
    displayChatHistory(groupId, true);  // 显示聊天历史
}

// 发送按钮槽函数
void MainWindow::onSendButtonClicked() {
    // 获取输入的消息并去除首尾空格
    QString message = messageInput->text().trimmed();
    // 如果消息为空或没有选中聊天，直接返回
    if (message.isEmpty() || currentChatId == -1) return;
    
    // 获取当前时间
    QString time = QDateTime::currentDateTime().toString("HH:mm");
    // 获取当前用户名
    QString senderName = QString::fromStdString(currentUser.getName());
    
    // 构造消息HTML(右对齐，蓝色背景)
    QString msgHtml = QString(
        "<div style='margin-bottom: 12px; text-align: right;'>"
        "<div style='font-size: 12px; color: #868e96; margin-bottom: 4px;'>%1</div>"
        "<div style='background: #4263eb; color: white; display: inline-block; "
        "padding: 8px 12px; border-radius: 12px; max-width: 70%%; word-wrap: break-word;'>"
        "%2</div>"
        "</div>"
    ).arg(time).arg(message);
    
    // 添加消息到聊天区域
    chatDisplay->append(msgHtml);
    messageInput->clear();  // 清空输入框
    
    // 模拟自动回复(1秒后)
    QTimer::singleShot(1000, [this]() {
        if (currentChatId != -1) {
            // 根据聊天类型生成不同的回复文本
            QString replyText = isGroupChat ? 
                QString("这是群聊%1的自动回复").arg(currentChatId) :
                QString("这是来自好友%1的自动回复").arg(currentChatId);
            
            // 获取当前时间
            QString time = QDateTime::currentDateTime().toString("HH:mm");
            // 构造回复消息HTML(左对齐，灰色背景)
            QString msgHtml = QString(
                "<div style='margin-bottom: 12px; text-align: left;'>"
                "<div style='font-size: 12px; color: #868e96; margin-bottom: 4px;'>%1</div>"
                "<div style='background: #f1f3f5; color: #212529; display: inline-block; "
                "padding: 8px 12px; border-radius: 12px; max-width: 70%%; word-wrap: break-word;'>"
                "%2</div>"
                "</div>"
            ).arg(time).arg(replyText);
            
            // 添加回复到聊天区域
            chatDisplay->append(msgHtml);
        }
    });
}

// 清空聊天区域
void MainWindow::clearChatArea() {
    chatDisplay->clear();
}

// 显示聊天历史记录
void MainWindow::displayChatHistory(int id, bool isGroup) {
    // 示例消息列表
    QStringList exampleMessages = {
        "你好！最近怎么样？",
        "项目进展如何？",
        "我们明天上午10点开会讨论一下",
        "记得把文档准备好"
    };
    
    // 添加每条示例消息到聊天区域
    foreach (const QString &msg, exampleMessages) {
        QString time = QDateTime::currentDateTime().toString("HH:mm");
        // 构造消息HTML(左对齐，灰色背景)
        QString msgHtml = QString(
            "<div style='margin-bottom: 12px; text-align: left;'>"
            "<div style='font-size: 12px; color: #868e96; margin-bottom: 4px;'>%1</div>"
            "<div style='background: #f1f3f5; color: #212529; display: inline-block; "
            "padding: 8px 12px; border-radius: 12px; max-width: 70%%; word-wrap: break-word;'>"
            "%2</div>"
            "</div>"
        ).arg(time).arg(msg);
        
        chatDisplay->append(msgHtml);
    }
    
    // 滚动到底部
    chatDisplay->verticalScrollBar()->setValue(chatDisplay->verticalScrollBar()->maximum());
}