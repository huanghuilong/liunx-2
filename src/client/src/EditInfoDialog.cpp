#include "EditInfoDialog.h"

// 设置对话框类实现

EditInfoDialog::EditInfoDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("修改信息");
    setFixedSize(360, 300);
    setStyleSheet(R"(
        QWidget {
            background-color: #ffffff;
            font-family: 'Segoe UI';
            color: #212529;
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignCenter);

    usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("新用户名");
    usernameEdit->setStyleSheet(inputStyle());

    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("新密码");
    passwordEdit->setStyleSheet(inputStyle());

    QPushButton *saveBtn = new QPushButton("保存修改");
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet(buttonStyle());

    layout->addWidget(usernameEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(saveBtn);

    connect(saveBtn, &QPushButton::clicked, this, &EditInfoDialog::handleSave);
}


void EditInfoDialog::handleSave() {
    QString name = usernameEdit->text();
    QString pwd = passwordEdit->text();
    if (name.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this, "提示", "请填写完整信息");
        return;
    }
    
    // 发送修改请求
    emit infoChanged(name, pwd);
    
    QMessageBox::information(this, "成功", "修改成功！");
    accept();
}
