#include "loginwindow.h"
#include "registerwindow.h"
#include "MainWindow.h"
#include "public.h"
void LoginWindow::setupUI() {
    this->setStyleSheet(R"(
        QWidget {
            background-color: #ffffff;
            font-family: 'Segoe UI';
            color: #212529;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(20);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel *logoLabel = new QLabel;
    logoLabel->setPixmap(QPixmap(":/icons/wechat-logo.png").scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(logoLabel);

    accountEdit = new QLineEdit;
    accountEdit->setPlaceholderText("账号");
    accountEdit->setStyleSheet(R"(
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
    passwordEdit->setStyleSheet(accountEdit->styleSheet());

    mainLayout->addWidget(accountEdit);
    mainLayout->addWidget(passwordEdit);

    loginBtn = new QPushButton("立即登录");
    loginBtn->setCursor(Qt::PointingHandCursor);
    loginBtn->setStyleSheet(R"(
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
    mainLayout->addWidget(loginBtn);

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #dee2e6;");
    mainLayout->addWidget(line);

    QLabel *footerLabel = new QLabel("其他选项");
    footerLabel->setAlignment(Qt::AlignCenter);
    footerLabel->setStyleSheet("color: #6c757d;");
    mainLayout->addWidget(footerLabel);

    QHBoxLayout *optionLayout = new QHBoxLayout;
    registerBtn = new QPushButton("注册");
    registerBtn->setFlat(true);
    registerBtn->setCursor(Qt::PointingHandCursor);
    registerBtn->setStyleSheet("color: #6c757d;");

    QPushButton *forgotBtn = new QPushButton("忘记密码");
    forgotBtn->setFlat(true);
    forgotBtn->setCursor(Qt::PointingHandCursor);
    forgotBtn->setStyleSheet("color: #6c757d;");

    optionLayout->addStretch();
    optionLayout->addWidget(registerBtn);
    optionLayout->addSpacing(20);
    optionLayout->addWidget(forgotBtn);
    optionLayout->addStretch();

    mainLayout->addLayout(optionLayout);

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
    registerWindow->resize(360, 460);
    registerWindow->show();
    this->close(); // 关闭登录窗口
}