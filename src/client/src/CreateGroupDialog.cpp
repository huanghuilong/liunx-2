#include "CreateGroupDialog.h"


// 在MainWindow.cpp中添加以下实现
CreateGroupDialog::CreateGroupDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("创建群组");
    setFixedSize(360, 240);
    setStyleSheet(R"(
        QWidget {
            background-color: #ffffff;
            font-family: 'Segoe UI';
            color: #212529;
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 20, 30, 20);
    layout->setSpacing(15);

    QLabel *titleLabel = new QLabel("创建群组");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    QLineEdit *nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("群组名称");
    nameEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #ced4da;
            border-radius: 6px;
            padding: 10px;
            background-color: #ffffff;
        }
    )");

    QLineEdit *descEdit = new QLineEdit;
    descEdit->setPlaceholderText("群组描述");
    descEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #ced4da;
            border-radius: 6px;
            padding: 10px;
            background-color: #ffffff;
        }
    )");

    QPushButton *createBtn = new QPushButton("创建群组");
    createBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #4263eb;
            color: white;
            border-radius: 6px;
            padding: 10px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #3b5bdb;
        }
    )");

    layout->addWidget(nameEdit);
    layout->addWidget(descEdit);
    layout->addWidget(createBtn);

    connect(createBtn, &QPushButton::clicked, this, [=]() {
        if (nameEdit->text().isEmpty()) {
            QMessageBox::warning(this, "提示", "请输入群组名称");
            return;
        }
        emit groupCreationRequested(nameEdit->text(), descEdit->text());
        accept();
    });
}
