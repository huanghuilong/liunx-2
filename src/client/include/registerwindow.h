#pragma once
#include "public.h"

class RegisterWindow : public QWidget {
public:
    RegisterWindow(int clientfd, QWidget *parent = nullptr) : QWidget(parent), clientfd(clientfd) {
        setupUI();
    }

private:
    QLineEdit *usernameEdit;
    // QLineEdit *accountEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPwdEdit;
    QPushButton *registerBtn;
    QPushButton *backBtn;

    int clientfd;

    void setupUI();
    void handleRegister();
    void showLoginWindow();//展示登录的界面
};