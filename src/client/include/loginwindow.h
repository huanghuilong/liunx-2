#pragma once
#include "public.h"

// 登录界面类定义
class LoginWindow : public QWidget {
    Q_OBJECT
public:
    LoginWindow( int clientfd, QWidget *parent = nullptr) 
        : clientfd(clientfd), QWidget(parent){
        // 界面初始化
        setupUI();

        setWindowTitle("聊天客户端");
        // 连接登录按钮信号
        connect(loginBtn, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    }

private:
    QLineEdit *accountEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginBtn;
    QPushButton *registerBtn;
    int clientfd;

    void setupUI();
    void handleLogin();
    void showRegisterWindow();//展示注册的界面
    void showMainWindow();
};