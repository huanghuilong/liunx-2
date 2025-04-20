#include "PasswordVerifyDialog.h"

// 密码验证对话框类实现
PasswordVerifyDialog::PasswordVerifyDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("验证旧密码");
    resize(360, 480);
    setStyleSheet(R"(
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

    QLabel *title = new QLabel("请输入旧密码验证身份");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 16px; font-weight: bold;");
    mainLayout->addWidget(title);

    passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("旧密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setStyleSheet(R"(
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
    mainLayout->addWidget(passwordEdit);

    QPushButton *verifyBtn = new QPushButton("验证并修改信息");
    verifyBtn->setCursor(Qt::PointingHandCursor);
    verifyBtn->setStyleSheet(R"(
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
    mainLayout->addWidget(verifyBtn);

    connect(verifyBtn, &QPushButton::clicked, this, &PasswordVerifyDialog::verifyPassword);
}

void PasswordVerifyDialog::verifyPassword() {
    QString input = passwordEdit->text();
    if (input.isEmpty()) {
        QMessageBox::warning(this, "验证失败", "请输入旧密码！");
        return;
    }
    
    emit passwordVerified(input);
    accept();
}

