#pragma once
#include "public.h"

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