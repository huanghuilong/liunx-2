#include "loginwindow.h"
#include "registerwindow.h"
#include "MainWindow.h"
#include "public.h"
//**********************************************登录窗口类实现*******************************************
void LoginWindow::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    /* ======== 顶部Logo区域 ======== */
    QLabel *logoLabel = new QLabel;
    logoLabel->setPixmap(QPixmap(":/icons/wechat-logo.png").scaled(80, 80, Qt::KeepAspectRatio));
    logoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(logoLabel, 0, Qt::AlignCenter);
    mainLayout->addSpacing(30);

    /* ======== 输入表单区域 ======== */
    QFormLayout *formLayout = new QFormLayout;

    accountEdit = new QLineEdit;
    accountEdit->setPlaceholderText("请输入账号");
    accountEdit->setFixedHeight(40);
    accountEdit->setStyleSheet("QLineEdit { border: 1px solid #E5E5E5; border-radius: 4px; padding: 0 10px; }");

    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setFixedHeight(40);
    passwordEdit->setStyleSheet(accountEdit->styleSheet());

    formLayout->addRow("账号", accountEdit);
    formLayout->addRow("密码", passwordEdit);
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setContentsMargins(20, 0, 20, 0);
    mainLayout->addLayout(formLayout);    

    loginBtn = new QPushButton("登录");
    loginBtn->setFixedHeight(40);
    loginBtn->setStyleSheet(
        "QPushButton { background-color: #07C160; color: white; border-radius: 4px; }"
        "QPushButton:hover { background-color: #05A850; }"
    );
    mainLayout->addSpacing(20);
    mainLayout->addWidget(loginBtn);
    mainLayout->addSpacing(15);

    /* ======== 其他选项 ======== */
    QHBoxLayout *optionLayout = new QHBoxLayout;
    
    registerBtn = new QPushButton("注册账号");
    registerBtn->setFlat(true);
    registerBtn->setStyleSheet("color: #888888;");
    
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::VLine);
    line->setStyleSheet("color: #E5E5E5;");

    QPushButton *forgotBtn = new QPushButton("找回密码");
    forgotBtn->setFlat(true);
    forgotBtn->setStyleSheet("color: #888888;");

    optionLayout->addWidget(registerBtn);
    optionLayout->addWidget(line);
    optionLayout->addWidget(forgotBtn);
    optionLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addLayout(optionLayout);

    /* ======== 底部区域 ======== */
    mainLayout->addStretch();
    
    QLabel *footerLabel = new QLabel("使用其他方式登录");
    footerLabel->setAlignment(Qt::AlignCenter);
    footerLabel->setStyleSheet("color: #888888; border-top: 1px solid #E5E5E5; padding: 15px 0;");
    mainLayout->addWidget(footerLabel);

    // 连接注册按钮信号
    connect(registerBtn, &QPushButton::clicked, this, &LoginWindow::showRegisterWindow);
}

void LoginWindow::handleLogin() {
    // 获取输入数据
    QString account = accountEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();

    // 输入验证
    if(account.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "错误", "账号和密码不能为空");
        return;
    }

    bool ok1, ok2;
    int userId = account.toInt(&ok1);
    int pwd_int = password.toInt(&ok2);

    if(!ok1 || !ok2) {
        QMessageBox::warning(this, "格式错误", "请输入有效的数字账号和密码");
        return;
    }
    
    // 这里可以添加实际登录逻辑
    int id = userId;
    char pwd[50] = {0};
    snprintf(pwd, sizeof(pwd), "%d", pwd_int); // 安全转换,将int类型的pwd_int转换为char类型的pwd

    json js;
    js["msgid"] = LOGIN_MSG;
    js["id"] = id;
    js["password"] = pwd;
    string request = js.dump();

    g_isLoginSuccess = false;

    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
    if (len == -1){
        cerr << "send login msg error:" << request << endl;
    }

    sem_wait(&rwsem); // 等待信号量，由子线程处理完登录的响应消息后，通知这里
        
    if (g_isLoginSuccess) {
        // 进入聊天主菜单页面
        isMainMenuRunning = true;
        // showMainWindow();
        this->close();
    }
}

void LoginWindow::showRegisterWindow() {
    RegisterWindow *registerWindow = new RegisterWindow(clientfd);
    // registerWindow->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    registerWindow->resize(400, 550);
    registerWindow->show();
    this->close(); // 关闭登录窗口
}