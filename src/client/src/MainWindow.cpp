#include "MainWindow.h"
#include <QDebug>

// 主窗口构造函数
MainWindow::MainWindow(const json &responsejs, int clientfd, QWidget *parent) : QMainWindow(parent), clientfd(clientfd), responsejs(responsejs) {
    setupUI();

    setWindowTitle("简约聊天");
    setFixedSize(900, 600);  // 禁止拉伸，更有应用感
    setStyleSheet("background-color: #f1f3f5;");  // 更柔和的背景色
}

void MainWindow::setupUI() {
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(2);

    // ===== 左侧联系人面板 =====
    leftPanel = new QWidget;
    leftPanel->setFixedWidth(260);
    leftPanel->setStyleSheet("background-color: #ffffff; border-right: 1px solid #dee2e6;");

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(8, 8, 8, 8);
    leftLayout->setSpacing(8);

    // 搜索框
    searchInput = new QLineEdit;
    searchInput->setPlaceholderText("🔍 搜索联系人");
    searchInput->setStyleSheet(
        "QLineEdit { background: #f8f9fa; border: 1px solid #ced4da; border-radius: 6px; padding: 8px 12px; }"
    );
    connect(searchInput, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    // 搜索结果
    searchResults = new QListWidget;
    searchResults->setStyleSheet(
        "QListWidget { background: transparent; border: none; }"
        "QListWidget::item { height: 50px; border-bottom: 1px solid #e9ecef; padding: 10px; }"
    );
    searchResults->hide();
    connect(searchResults, &QListWidget::itemClicked, this, &MainWindow::onSearchResultClicked);

    // 联系人列表
    contactList = new QListWidget;
    contactList->setStyleSheet(
        "QListWidget { background: transparent; border: none; }"
        "QListWidget::item { height: 50px; border-bottom: 1px solid #e9ecef; padding: 10px; }"
        "QListWidget::item:hover { background-color: #f1f3f5; }"
        "QListWidget::item:selected { background-color: #dbe4ff; color: #364fc7; }"
    );

    leftLayout->addWidget(searchInput);
    leftLayout->addWidget(searchResults);
    leftLayout->addWidget(contactList);

    // ===== 右侧聊天面板 =====
    rightPanel = new QWidget;
    rightPanel->setStyleSheet("background-color: #ffffff;");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    currentChatLabel = new QLabel("💬 选择联系人开始聊天");
    currentChatLabel->setStyleSheet(
        "font-size: 18px; font-weight: 600; color: #343a40;"
        "padding: 12px 20px; border-bottom: 1px solid #e9ecef; background-color: #f8f9fa;"
    );

    chatDisplay = new QTextEdit;
    chatDisplay->setReadOnly(true);
    chatDisplay->setStyleSheet("QTextEdit { background-color: #f1f3f5; border: none; padding: 16px; font-size: 14px; }");

    // ===== 输入面板 =====
    QWidget *inputPanel = new QWidget;
    inputPanel->setStyleSheet("background-color: #ffffff; border-top: 1px solid #e9ecef;");

    QHBoxLayout *inputLayout = new QHBoxLayout(inputPanel);
    inputLayout->setContentsMargins(12, 8, 12, 8);
    inputLayout->setSpacing(8);

    messageInput = new QLineEdit;
    messageInput->setPlaceholderText("✏️ 输入消息...");
    messageInput->setStyleSheet(
        "QLineEdit { border: 1px solid #ced4da; border-radius: 6px; padding: 8px 12px; }"
    );

    sendButton = new QPushButton("发送");
    sendButton->setFixedWidth(80);
    sendButton->setStyleSheet(
        "QPushButton { background-color: #4263eb; color: white; border-radius: 6px; padding: 8px; font-weight: 500; }"
        "QPushButton:hover { background-color: #3b5bdb; }"
    );

    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);

    rightLayout->addWidget(currentChatLabel);
    rightLayout->addWidget(chatDisplay, 1);
    rightLayout->addWidget(inputPanel);

    // 拼装主布局
    mainSplitter->addWidget(leftPanel);
    mainSplitter->addWidget(rightPanel);
    setCentralWidget(mainSplitter);

    // ===== 信号连接 =====
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
                            const std::vector<Group> &groups,
                            const std::vector<User> &AllUser) {
    // 保存用户数据
    this->currentUser = currentUser;
    this->friends = friends;
    this->groups = groups;
    this->AllUser = AllUser;

    contactList->clear();  // 清空联系人列表

    // 添加好友到联系人列表
    for (const User &user : friends) {
        createFriendItem(contactList, user, true);
    }

    // 添加群组到联系人列表
    for (const Group &group : groups) {
        createGroupItem(contactList, group, true);
    }
}

void MainWindow::createGroupItem(QListWidget *list, const Group &group, bool isFriend) {
    // 添加群组到好友列表
    QListWidgetItem *item = new QListWidgetItem;
    // 设置项数据 - 群组ID和类型标记(群组)
    item->setData(Qt::UserRole, group.getId());
    item->setData(Qt::UserRole + 1, true); // true表示群组

    // 创建自定义项部件
    QWidget *itemWidget = new QWidget;
    itemWidget->setAttribute(Qt::WA_TransparentForMouseEvents); // 新增此行

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

void MainWindow::createFriendItem(QListWidget *list, const User &user, bool isFriend) {
    // 添加好友到联系人列表
    QListWidgetItem *item = new QListWidgetItem;
    // 设置项数据 - 用户ID和类型标记(好友)
    item->setData(Qt::UserRole, user.getId());
    item->setData(Qt::UserRole + 1, false);
    item->setData(Qt::UserRole + 2, isFriend);
    
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
        .arg(user.getState() == "online" ? "#4263eb" : "#868e96")
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
    
    // 将组件添加到布局
    layout->addWidget(avatar);
    layout->addWidget(nameLabel, 1);
    layout->addWidget(status);
    
    // 设置项大小并添加到列表
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
    
    // 显示与当前好友的聊天历史
    if (chatHistory.find(friendId) != chatHistory.end()) {
        for (const ChatMessage &msg : chatHistory[friendId]) {
            displayMessage(msg);
        }
    }
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
    
    // 显示与当前群组的聊天历史
    if (chatHistory.find(groupId) != chatHistory.end()) {
        for (const ChatMessage &msg : chatHistory[groupId]) {
            displayMessage(msg);
        }
    }
}

void MainWindow::displayMessage(const ChatMessage &msg) {
    QString msgHtml = QString(
        "<div style='margin-bottom: 12px; text-align: %1;'>"
        "<div style='font-size: 12px; color: #868e96; margin-bottom: 4px;'>%2</div>"
        "<div style='background: %3; color: %4; display: inline-block; "
        "padding: 8px 12px; border-radius: 12px; max-width: 70%%; word-wrap: break-word;'>"
        "%5</div>"
        "</div>"
    ).arg(
        msg.isSelf ? "right" : "left",
        msg.time,
        msg.isSelf ? "#4263eb" : "#f1f3f5",
        msg.isSelf ? "white" : "#212529",
        msg.message
    );
    
    chatDisplay->append(msgHtml);
    chatDisplay->verticalScrollBar()->setValue(chatDisplay->verticalScrollBar()->maximum());
}

void MainWindow::onReceiveMessage(const QString &senderName, const QString &message, const QString &time, bool isSelf) {
    // 创建消息对象
    ChatMessage msg;
    msg.senderName = senderName;
    msg.message = message;
    msg.time = time;
    msg.isSelf = isSelf;

    // 获取发送者ID
    int senderId = -1;
    if (isSelf) {
        senderId = currentChatId;
    } else {
        // 从好友列表中查找发送者ID
        for (const User &user : friends) {
            if (QString::fromStdString(user.getName()) == senderName) {
                senderId = user.getId();
                break;
            }
        }
    }

    // 如果找到了发送者ID，保存消息
    if (senderId != -1) {
        chatHistory[senderId].push_back(msg);
        
        // 如果当前正在与发送者聊天，则显示消息
        if (currentChatId == senderId) {
            displayMessage(msg);
        }
    }
}

void MainWindow::onSendButtonClicked() {
    QString message = messageInput->text().trimmed();
    if (message.isEmpty() || currentChatId == -1) return;
     
    QString time = QDateTime::currentDateTime().toString("HH:mm");
    QString senderName = QString::fromStdString(currentUser.getName());

    // 显示自己发送的消息
    onReceiveMessage(senderName, message, time, true);
    messageInput->clear();

    // --------------------------chat业务的实现---------------------------------
    int friendid = currentChatId;
    string messages = message.toStdString();

    json js;
    js["msgid"] = ONE_CHAT_MSG;
    js["id"] = currentUser.getId();
    js["name"] = currentUser.getName();
    js["toid"] = friendid;
    js["msg"] = messages;
    js["time"] = getCurrentTime();
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (-1 == len)
    {
        cerr << "send chat msg error -> " << buffer << endl;
    }
    // ------------------------------------------------------------------------
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
    // ------------------------------添加好友业务----------------------------------
    int friendid = user.getId();
    json js;
    js["msgid"] = ADD_FRIEND_MSG;
    js["id"] = g_currentUser.getId();
    js["friendid"] = friendid;
    
    string buffer = js.dump();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (-1 == len)
    {
        cerr << "send addfriend msg error -> " << buffer << endl;
    }

    if (std::find(friends.begin(), friends.end(), user) == friends.end()) {
        friends.push_back(user);
        updateUserData(currentUser, friends, groups, AllUser);
        QMessageBox::information(this, "提示", QString("已发送好友请求给 %1\n验证消息: %2")
                                .arg(QString::fromStdString(user.getName()))
                                .arg(message));
    }
}

void MainWindow::clearChatArea() {
    chatDisplay->clear();
}

#include "MainWindow.moc"