#include "registerwindow.h"
#include "loginwindow.h"

void RegisterWindow::setupUI() {
    this->setStyleSheet(R"(
        QWidget {
            background-color: #ffffff;
            font-family: 'Segoe UI';
            color: #212529;
        }
    )");

    this->resize(360, 480);  // 和 LoginWindow 一致的窗口大小

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(20);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel *logoLabel = new QLabel;
    logoLabel->setPixmap(QPixmap(":/icons/wechat-logo.png").scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(logoLabel);

    usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("用户名");
    usernameEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #ced4da;
            border-radius: 6px;
            padding: 10px;
            background-color: #ffffff;
        }
        QLineEdit:focus {
            border: 1px solid #86b7fe;
            outline: none;
        }
    )");

    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("密码");
    passwordEdit->setStyleSheet(usernameEdit->styleSheet());

    confirmPwdEdit = new QLineEdit;
    confirmPwdEdit->setEchoMode(QLineEdit::Password);
    confirmPwdEdit->setPlaceholderText("确认密码");
    confirmPwdEdit->setStyleSheet(usernameEdit->styleSheet());

    mainLayout->addWidget(usernameEdit);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addWidget(confirmPwdEdit);

    registerBtn = new QPushButton("立即注册");
    registerBtn->setCursor(Qt::PointingHandCursor);
    registerBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #0d6efd;
            color: white;
            font-weight: bold;
            border: none;
            border-radius: 6px;
            padding: 10px;
        }
        QPushButton:hover {
            background-color: #0b5ed7;
        }
    )");
    mainLayout->addWidget(registerBtn);

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #dee2e6;");
    mainLayout->addWidget(line);

    QLabel *footerLabel = new QLabel("已有账号？");
    footerLabel->setAlignment(Qt::AlignCenter);
    footerLabel->setStyleSheet("color: #6c757d;");
    mainLayout->addWidget(footerLabel);

    QHBoxLayout *optionLayout = new QHBoxLayout;
    backBtn = new QPushButton("返回登录");
    backBtn->setFlat(true);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet("color: #6c757d;");

    optionLayout->addStretch();
    optionLayout->addWidget(backBtn);
    optionLayout->addStretch();

    mainLayout->addLayout(optionLayout);

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
    if (userReg == firstuserReg) {
        QMessageBox::warning(this, "错误", "用户名已存在，请重新输入");
        return;
    }
    else {
        firstuserReg = userReg;
        QString messageRES = QString("用户 ID：%1").arg(userReg);
        QMessageBox::information(this, "注册成功", messageRES);
        showLoginWindow();
    }
    
    
}
    
void RegisterWindow::showLoginWindow() {
    LoginWindow *loginWindow = new LoginWindow(clientfd);
    loginWindow->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    loginWindow->resize(360, 460);
    loginWindow->show();
    this->close(); // 关闭注册窗口
}