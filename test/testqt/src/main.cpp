#include <QApplication>
#include <QMainWindow>
#include <QListWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QScrollBar>
#include <QDateTime>
#include <QFont>
#include <QTimer>
#include <QDialog>
#include <QMessageBox>
#include <vector>
#include <string>
#include <algorithm>

// 模拟User类 - 表示用户信息
class User {
public:
    // 获取用户ID
    int getId() const { return id; }
    // 获取用户名
    std::string getName() const { return name; }
    // 获取用户状态(在线/离线)
    std::string getState() const { return state; }
    
    // 设置用户ID
    void setId(int id) { this->id = id; }
    // 设置用户名
    void setName(const std::string& name) { this->name = name; }
    // 设置用户状态
    void setState(const std::string& state) { this->state = state; }

    bool operator==(const User& other) const {
        return id == other.id;
    }

private:
    int id = 0;                 // 用户ID
    std::string name;           // 用户名
    std::string state = "online"; // 用户状态，默认为在线
};

// 模拟Group类 - 表示群组信息
class Group {
public:
    // 获取群组ID
    int getId() const { return id; }
    // 获取群组名
    std::string getName() const { return name; }
    // 获取群组描述
    std::string getDesc() const { return desc; }
    
    // 设置群组ID
    void setId(int id) { this->id = id; }
    // 设置群组名
    void setName(const std::string& name) { this->name = name; }
    // 设置群组描述
    void setDesc(const std::string& desc) { this->desc = desc; }

private:
    int id = 0;                 // 群组ID
    std::string name;           // 群组名
    std::string desc;           // 群组描述
};

// 全局变量
User g_currentUser;                         // 当前登录用户
std::vector<User> g_currentUserFriendList;  // 当前用户的好友列表
std::vector<User> g_allUsersList;           // 所有用户列表（模拟数据库）
std::vector<Group> g_currentUserGroupList;  // 当前用户的群组列表

// 添加好友对话框
class AddFriendDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddFriendDialog(const User& user, QWidget *parent = nullptr)
        : QDialog(parent), m_user(user) {
        setWindowTitle("添加好友");
        setFixedSize(400, 200);
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        QLabel *infoLabel = new QLabel(QString("将 %1 添加到通讯录").arg(QString::fromStdString(user.getName())));
        infoLabel->setAlignment(Qt::AlignCenter);
        
        QLabel *idLabel = new QLabel(QString("微信号: %1").arg(user.getId()));
        idLabel->setAlignment(Qt::AlignCenter);
        
        m_messageEdit = new QLineEdit;
        m_messageEdit->setPlaceholderText("请输入验证消息(可选)");
        m_messageEdit->setText("我是");
        
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        QPushButton *cancelBtn = new QPushButton("取消");
        QPushButton *sendBtn = new QPushButton("发送");
        sendBtn->setStyleSheet("background-color: #07C160; color: white;");
        
        buttonLayout->addWidget(cancelBtn);
        buttonLayout->addWidget(sendBtn);
        
        layout->addWidget(infoLabel);
        layout->addWidget(idLabel);
        layout->addWidget(m_messageEdit);
        layout->addLayout(buttonLayout);
        
        connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
        connect(sendBtn, &QPushButton::clicked, this, &AddFriendDialog::onSendClicked);
    }
    
signals:
    void friendRequestSent(const User& user, const QString& message);
    
private slots:
    void onSendClicked() {
        emit friendRequestSent(m_user, m_messageEdit->text());
        accept();
    }
    
private:
    User m_user;
    QLineEdit *m_messageEdit;
};

// 主窗口类
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    
    void updateUserData(const User &currentUser, 
                      const std::vector<User> &friends,
                      const std::vector<Group> &groups);

private slots:
    void onFriendItemClicked(QListWidgetItem *item);
    void onGroupItemClicked(QListWidgetItem *item);
    void onSendButtonClicked();
    void onSearchTextChanged(const QString &text);
    void onAddFriendClicked(const User &user, const QString &message);
    void onSearchResultClicked(QListWidgetItem *item);

private:
    void setupUI();
    void clearChatArea();
    void displayChatHistory(int id, bool isGroup);
    void updateSearchResults(const QString &keyword);
    void createFriendItem(QListWidget *list, const User &user, bool isFriend);
    
    QSplitter *mainSplitter;
    QWidget *leftPanel;
    QWidget *rightPanel;
    QListWidget *contactList;
    QTextEdit *chatDisplay;
    QLineEdit *messageInput;
    QPushButton *sendButton;
    QLabel *currentChatLabel;
    QLineEdit *searchInput;
    QListWidget *searchResults;
    
    User currentUser;
    std::vector<User> friends;
    std::vector<Group> groups;
    int currentChatId = -1;
    bool isGroupChat = false;
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    setWindowTitle("微信风格聊天");
    resize(900, 600);
    setStyleSheet("background-color: #f8f9fa;");
}

void MainWindow::setupUI() {
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(1);
    
    // 左侧面板
    leftPanel = new QWidget;
    leftPanel->setMinimumWidth(200);
    leftPanel->setMaximumWidth(300);
    leftPanel->setStyleSheet("background-color: #ffffff; border-right: 1px solid #e9ecef;");
    
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    
    // 搜索框
    searchInput = new QLineEdit;
    searchInput->setPlaceholderText("搜索");
    searchInput->setStyleSheet(
        "QLineEdit { border: none; padding: 8px; background: #f5f5f5; border-radius: 4px; margin: 5px; }"
    );
    connect(searchInput, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    
    // 搜索结果列表
    searchResults = new QListWidget;
    searchResults->setStyleSheet(
        "QListWidget { border: none; background: transparent; }"
        "QListWidget::item { height: 60px; border-bottom: 1px solid #e9ecef; padding: 8px; }"
    );
    searchResults->hide();
    connect(searchResults, &QListWidget::itemClicked, this, &MainWindow::onSearchResultClicked);
    
    // 联系人列表
    contactList = new QListWidget;
    contactList->setStyleSheet(
        "QListWidget { border: none; background: transparent; }"
        "QListWidget::item { height: 60px; border-bottom: 1px solid #e9ecef; padding: 8px; }"
        "QListWidget::item:hover { background-color: #f1f3f5; }"
        "QListWidget::item:selected { background-color: #e9ecef; }"
    );
    
    leftLayout->addWidget(searchInput);
    leftLayout->addWidget(searchResults);
    leftLayout->addWidget(contactList);
    
    // 右侧面板
    rightPanel = new QWidget;
    rightPanel->setStyleSheet("background-color: #ffffff;");
    
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    
    currentChatLabel = new QLabel("选择联系人开始聊天");
    currentChatLabel->setStyleSheet(
        "font-size: 16px; font-weight: 500; color: #212529;"
        "padding: 12px 16px; border-bottom: 1px solid #e9ecef;"
    );
    
    chatDisplay = new QTextEdit;
    chatDisplay->setReadOnly(true);
    chatDisplay->setStyleSheet(
        "QTextEdit { border: none; background-color: #f8f9fa; padding: 16px; }"
    );
    
    QWidget *inputPanel = new QWidget;
    inputPanel->setStyleSheet("background-color: #ffffff; border-top: 1px solid #e9ecef;");
    
    QHBoxLayout *inputLayout = new QHBoxLayout(inputPanel);
    inputLayout->setContentsMargins(16, 8, 16, 8);
    
    messageInput = new QLineEdit;
    messageInput->setPlaceholderText("输入消息...");
    messageInput->setStyleSheet(
        "QLineEdit { border: 1px solid #e9ecef; border-radius: 4px; padding: 8px; }"
    );
    
    sendButton = new QPushButton("发送");
    sendButton->setFixedWidth(80);
    sendButton->setStyleSheet(
        "QPushButton { background-color: #4263eb; color: white; border-radius: 4px; }"
        "QPushButton:hover { background-color: #3b5bdb; }"
    );
    
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);
    
    rightLayout->addWidget(currentChatLabel);
    rightLayout->addWidget(chatDisplay, 1);
    rightLayout->addWidget(inputPanel);
    
    mainSplitter->addWidget(leftPanel);
    mainSplitter->addWidget(rightPanel);
    
    setCentralWidget(mainSplitter);
    
    connect(contactList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (item->data(Qt::UserRole + 1).toBool()) {
            onGroupItemClicked(item);
        } else {
            onFriendItemClicked(item);
        }
    });
    
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    connect(messageInput, &QLineEdit::returnPressed, this, &MainWindow::onSendButtonClicked);
}

void MainWindow::updateUserData(const User &currentUser, 
                              const std::vector<User> &friends,
                              const std::vector<Group> &groups) {
    this->currentUser = currentUser;
    this->friends = friends;
    this->groups = groups;
    
    contactList->clear();
    
    for (const User &user : friends) {
        createFriendItem(contactList, user, true);
    }
    
    for (const Group &group : groups) {
        QListWidgetItem *item = new QListWidgetItem;
        item->setData(Qt::UserRole, group.getId());
        item->setData(Qt::UserRole + 1, true);
        
        QWidget *itemWidget = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout(itemWidget);
        layout->setContentsMargins(8, 8, 8, 8);
        
        QLabel *avatar = new QLabel;
        avatar->setFixedSize(40, 40);
        avatar->setStyleSheet(
            "background-color: #fcc419; border-radius: 20px; color: white;"
            "font-weight: bold; font-size: 16px; qproperty-alignment: AlignCenter;"
        );
        avatar->setText(QString::fromStdString(group.getName()).left(1).toUpper());
        
        QVBoxLayout *textLayout = new QVBoxLayout;
        textLayout->setSpacing(2);
        
        QLabel *nameLabel = new QLabel(QString::fromStdString(group.getName()));
        nameLabel->setStyleSheet("font-size: 14px; color: #212529;");
        
        QLabel *descLabel = new QLabel(QString::fromStdString(group.getDesc()));
        descLabel->setStyleSheet("font-size: 12px; color: #868e96;");
        
        textLayout->addWidget(nameLabel);
        textLayout->addWidget(descLabel);
        
        layout->addWidget(avatar);
        layout->addLayout(textLayout, 1);
        
        item->setSizeHint(itemWidget->sizeHint());
        contactList->addItem(item);
        contactList->setItemWidget(item, itemWidget);
    }
}

void MainWindow::createFriendItem(QListWidget *list, const User &user, bool isFriend) {
    QListWidgetItem *item = new QListWidgetItem;
    item->setData(Qt::UserRole, user.getId());
    item->setData(Qt::UserRole + 1, false);
    item->setData(Qt::UserRole + 2, isFriend);
    
    QWidget *itemWidget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(itemWidget);
    layout->setContentsMargins(8, 8, 8, 8);
    
    QLabel *avatar = new QLabel;
    avatar->setFixedSize(40, 40);
    avatar->setStyleSheet(
        QString("background-color: %1; border-radius: 20px; color: white;"
               "font-weight: bold; font-size: 16px; qproperty-alignment: AlignCenter;")
        .arg(user.getState() == "online" ? "#4263eb" : "#868e96")
    );
    avatar->setText(QString::fromStdString(user.getName()).left(1).toUpper());
    
    QLabel *nameLabel = new QLabel(QString::fromStdString(user.getName()));
    nameLabel->setStyleSheet("font-size: 14px; color: #212529;");
    
    QLabel *status = new QLabel;
    status->setFixedSize(8, 8);
    status->setStyleSheet(
        QString("background-color: %1; border-radius: 4px;")
        .arg(user.getState() == "online" ? "#40c057" : "#adb5bd")
    );
    
    if (!isFriend) {
        QPushButton *addButton = new QPushButton("添加");
        addButton->setStyleSheet(
            "QPushButton { background-color: #07C160; color: white; border-radius: 4px; padding: 2px 8px; }"
            "QPushButton:hover { background-color: #05A850; }"
        );
        connect(addButton, &QPushButton::clicked, [this, user]() {
            AddFriendDialog *dialog = new AddFriendDialog(user, this);
            connect(dialog, &AddFriendDialog::friendRequestSent, this, &MainWindow::onAddFriendClicked);
            dialog->show();
        });
        layout->addWidget(addButton);
    }
    
    layout->addWidget(avatar);
    layout->addWidget(nameLabel, 1);
    layout->addWidget(status);
    
    item->setSizeHint(itemWidget->sizeHint());
    list->addItem(item);
    list->setItemWidget(item, itemWidget);
}

void MainWindow::onFriendItemClicked(QListWidgetItem *item) {
    int friendId = item->data(Qt::UserRole).toInt();
    currentChatId = friendId;
    isGroupChat = false;
    
    QString friendName;
    for (const User &user : friends) {
        if (user.getId() == friendId) {
            friendName = QString::fromStdString(user.getName());
            break;
        }
    }
    
    currentChatLabel->setText(friendName);
    clearChatArea();
    displayChatHistory(friendId, false);
}

void MainWindow::onGroupItemClicked(QListWidgetItem *item) {
    int groupId = item->data(Qt::UserRole).toInt();
    currentChatId = groupId;
    isGroupChat = true;
    
    QString groupName;
    for (const Group &group : groups) {
        if (group.getId() == groupId) {
            groupName = QString::fromStdString(group.getName());
            break;
        }
    }
    
    currentChatLabel->setText(groupName);
    clearChatArea();
    displayChatHistory(groupId, true);
}

void MainWindow::onSendButtonClicked() {
    QString message = messageInput->text().trimmed();
    if (message.isEmpty() || currentChatId == -1) return;
    
    QString time = QDateTime::currentDateTime().toString("HH:mm");
    QString senderName = QString::fromStdString(currentUser.getName());
    
    QString msgHtml = QString(
        "<div style='margin-bottom: 12px; text-align: right;'>"
        "<div style='font-size: 12px; color: #868e96; margin-bottom: 4px;'>%1</div>"
        "<div style='background: #4263eb; color: white; display: inline-block; "
        "padding: 8px 12px; border-radius: 12px; max-width: 70%%; word-wrap: break-word;'>"
        "%2</div>"
        "</div>"
    ).arg(time).arg(message);
    
    chatDisplay->append(msgHtml);
    messageInput->clear();
    
    QTimer::singleShot(1000, [this]() {
        if (currentChatId != -1) {
            QString replyText = isGroupChat ? 
                QString("这是群聊%1的自动回复").arg(currentChatId) :
                QString("这是来自好友%1的自动回复").arg(currentChatId);
            
            QString time = QDateTime::currentDateTime().toString("HH:mm");
            QString msgHtml = QString(
                "<div style='margin-bottom: 12px; text-align: left;'>"
                "<div style='font-size: 12px; color: #868e96; margin-bottom: 4px;'>%1</div>"
                "<div style='background: #f1f3f5; color: #212529; display: inline-block; "
                "padding: 8px 12px; border-radius: 12px; max-width: 70%%; word-wrap: break-word;'>"
                "%2</div>"
                "</div>"
            ).arg(time).arg(replyText);
            
            chatDisplay->append(msgHtml);
        }
    });
}

void MainWindow::onSearchTextChanged(const QString &text) {
    if (text.isEmpty()) {
        searchResults->hide();
        contactList->show();
    } else {
        updateSearchResults(text);
        searchResults->show();
        contactList->hide();
    }
}

void MainWindow::updateSearchResults(const QString &keyword) {
    searchResults->clear();
    
    bool found = false;
    
    // 搜索好友
    for (const User &user : g_allUsersList) {
        if (QString::fromStdString(user.getName()).contains(keyword, Qt::CaseInsensitive) || 
            QString::number(user.getId()).contains(keyword)) {
            
            bool isFriend = std::find(friends.begin(), friends.end(), user) != friends.end();
            createFriendItem(searchResults, user, isFriend);
            found = true;
        }
    }
    
    if (!found) {
        QListWidgetItem *item = new QListWidgetItem("未找到匹配的用户");
        item->setTextAlignment(Qt::AlignCenter);
        searchResults->addItem(item);
    }
}

void MainWindow::onSearchResultClicked(QListWidgetItem *item) {
    if (item->data(Qt::UserRole + 2).toBool()) {
        // 已经是好友，跳转到聊天
        onFriendItemClicked(item);
    }
}

void MainWindow::onAddFriendClicked(const User &user, const QString &message) {
    // 模拟添加好友成功
    if (std::find(friends.begin(), friends.end(), user) == friends.end()) {
        friends.push_back(user);
        updateUserData(currentUser, friends, groups);
        QMessageBox::information(this, "提示", QString("已发送好友请求给 %1\n验证消息: %2")
                                .arg(QString::fromStdString(user.getName()))
                                .arg(message));
    }
}

void MainWindow::clearChatArea() {
    chatDisplay->clear();
}

void MainWindow::displayChatHistory(int id, bool isGroup) {
    QStringList exampleMessages = {
        "你好！最近怎么样？",
        "项目进展如何？",
        "我们明天上午10点开会讨论一下",
        "记得把文档准备好"
    };
    
    foreach (const QString &msg, exampleMessages) {
        QString time = QDateTime::currentDateTime().toString("HH:mm");
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
    
    chatDisplay->verticalScrollBar()->setValue(chatDisplay->verticalScrollBar()->maximum());
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 设置默认字体
    QFont font("PingFang SC", 12);
    app.setFont(font);
    
    // 初始化测试数据
    g_currentUser.setId(1001);
    g_currentUser.setName("我的账号");
    
    // 创建测试好友数据
    User friend1, friend2, friend3;
    friend1.setId(1002);
    friend1.setName("张三");
    friend1.setState("online");
    friend2.setId(1003);
    friend2.setName("李四");
    friend2.setState("offline");
    friend3.setId(1004);
    friend3.setName("王五");
    friend3.setState("online");
    g_currentUserFriendList = {friend1, friend2, friend3};
    
    // 创建所有用户数据（模拟数据库）
    User user4, user5, user6;
    user4.setId(1005);
    user4.setName("赵六");
    user4.setState("online");
    user5.setId(1006);
    user5.setName("钱七");
    user5.setState("offline");
    user6.setId(1007);
    user6.setName("孙八");
    user6.setState("online");
    g_allUsersList = {friend1, friend2, friend3, user4, user5, user6};
    
    // 创建测试群组数据
    Group group1, group2;
    group1.setId(2001);
    group1.setName("项目组");
    group1.setDesc("项目开发讨论");
    group2.setId(2002);
    group2.setName("同学群");
    group2.setDesc("大学同学交流");
    g_currentUserGroupList = {group1, group2};
    
    // 创建并显示主窗口
    MainWindow mainWin;
    mainWin.updateUserData(g_currentUser, g_currentUserFriendList, g_currentUserGroupList);
    mainWin.show();
    
    return app.exec();
}

#include "main.moc"