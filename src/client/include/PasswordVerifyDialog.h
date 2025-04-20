#pragma once
#include "public.h"

// 密码验证对话框类实现
class PasswordVerifyDialog : public QDialog {
    Q_OBJECT
public:
    explicit PasswordVerifyDialog(QWidget *parent = nullptr);
private slots:
    void verifyPassword();

signals:
    void passwordVerified(const QString &password);

private:
    QLineEdit *passwordEdit;
};