#include "registerwindow.h"
#include "loginwindow.h"

// 注册窗口类实现
void RegisterWindow::setupUI() {
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    /* ======== 标题 ======== */
    QLabel *titleLabel = new QLabel("注册新账号");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);

    /* ======== 输入表单区域 ======== */
    QFormLayout *formLayout = new QFormLayout;

    // 用户名输入框
    usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("请输入用户名");
    usernameEdit->setFixedHeight(40);
    
    // // 账号输入框
    // accountEdit = new QLineEdit;
    // accountEdit->setPlaceholderText("请输入账号");
    // accountEdit->setFixedHeight(40);
    
    // 密码输入框
    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setFixedHeight(40);
    
    // 确认密码输入框
    confirmPwdEdit = new QLineEdit;
    confirmPwdEdit->setEchoMode(QLineEdit::Password);
    confirmPwdEdit->setPlaceholderText("请再次输入密码");
    confirmPwdEdit->setFixedHeight(40);
    
    // 统一设置样式
    QString editStyle = "QLineEdit { border: 1px solid #E5E5E5; border-radius: 4px; padding: 0 10px; }";
    usernameEdit->setStyleSheet(editStyle);
    // accountEdit->setStyleSheet(editStyle);
    passwordEdit->setStyleSheet(editStyle);
    confirmPwdEdit->setStyleSheet(editStyle);

    // 添加表单行
    formLayout->addRow("用户名", usernameEdit);
    // formLayout->addRow("账号", accountEdit);
    formLayout->addRow("密码", passwordEdit);
    formLayout->addRow("确认密码", confirmPwdEdit);
    
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setContentsMargins(20, 0, 20, 0);
    mainLayout->addLayout(formLayout);

    /* ======== 注册按钮 ======== */
    registerBtn = new QPushButton("注册");
    registerBtn->setFixedHeight(45);
    registerBtn->setStyleSheet(
        "QPushButton { background-color: #07C160; color: white; border-radius: 4px; font-size: 16px; }"
        "QPushButton:hover { background-color: #05A850; }"
    );
    mainLayout->addSpacing(20);
    mainLayout->addWidget(registerBtn);
    mainLayout->addSpacing(15);

    /* ======== 底部返回登录 ======== */
    backBtn = new QPushButton("已有账号？返回登录");
    backBtn->setFlat(true);
    backBtn->setStyleSheet("color: #888888;");
    mainLayout->addStretch();
    mainLayout->addWidget(backBtn, 0, Qt::AlignCenter);

    // 连接按钮信号
    connect(registerBtn, &QPushButton::clicked, this, &RegisterWindow::handleRegister);
    connect(backBtn, &QPushButton::clicked, this, &RegisterWindow::showLoginWindow);
}
    
void RegisterWindow::handleRegister() {
    // 获取输入数据
    QString username = usernameEdit->text().trimmed();
    // QString account = accountEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();
    QString confirmPwd = confirmPwdEdit->text().trimmed();

    // 输入验证
    if(username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "错误", "所有字段都必须填写");
        return;
    }

    if(password != confirmPwd) {
        QMessageBox::warning(this, "错误", "两次输入的密码不一致");
        return;
    }

    // 这里可以添加实际的注册逻辑
    // #############################实现的逻辑#################################
    char name[50] = {0};
    char pwd[50] = {0};

    QByteArray byteArray = username.toLocal8Bit();
    strncpy(name, byteArray.constData(), sizeof(name) - 1); // 安全复制，防止溢出
    // 保证以null结尾
    name[sizeof(name) - 1] = '\0'; 

    byteArray = password.toLocal8Bit();
    strncpy(pwd, byteArray.constData(), sizeof(pwd) - 1); // 安全复制，防止溢出
    // 保证以null结尾
    pwd[sizeof(pwd) - 1] = '\0'; 

    json js;
    js["msgid"] = REG_MSG;
    js["name"] = name;
    js["password"] = pwd;
    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send reg msg error:" << request << endl;
    }
    
    sem_wait(&rwsem); // 等待信号量，子线程处理完注册消息会通知
    // ############################# 结束 ###################################
    qDebug() << "注册信息 - 用户名:" << username 
            //  << "账号:" << account 
                << "密码:" << password;
    QString messageRES = QString("用户 ID：%1").arg(userReg);
    QMessageBox::information(this, "注册成功", messageRES);
    showLoginWindow();
}
    
void RegisterWindow::showLoginWindow() {
    LoginWindow *loginWindow = new LoginWindow(clientfd);
    loginWindow->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    loginWindow->resize(400, 550);
    loginWindow->show();
    this->close(); // 关闭注册窗口
}