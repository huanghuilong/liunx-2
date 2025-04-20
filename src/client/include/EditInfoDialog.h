#pragma once
#include "public.h"

// 设置对话框类实现
class EditInfoDialog : public QDialog {
    Q_OBJECT
public:
    explicit EditInfoDialog(QWidget *parent = nullptr);
private slots:
    void handleSave();

signals:
    void infoChanged(const QString &username, const QString &password);

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;

    QString inputStyle() {
        return R"(
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
        )";
    }

    QString buttonStyle() {
        return R"(
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
        )";
    }
};