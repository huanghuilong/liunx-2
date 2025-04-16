#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QFrame>
#include <QDialog>
#include <QHBoxLayout>
// #include <temp.h>

// 模拟旧密码
const QString correctPassword = "123456";

// 修改信息弹窗类
class EditInfoDialog : public QDialog {
    Q_OBJECT
public:
    EditInfoDialog(QWidget *parent = nullptr) : QDialog(parent) {
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

private slots:
    void handleSave() {
        QString name = usernameEdit->text();
        QString pwd = passwordEdit->text();
        if (name.isEmpty() || pwd.isEmpty()) {
            QMessageBox::warning(this, "提示", "请填写完整信息");
            return;
        }
        QMessageBox::information(this, "成功", "修改成功！");
        accept();
    }

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

// 主界面类
class ModifyInfoWindow : public QWidget {
    Q_OBJECT
public:
    ModifyInfoWindow(QWidget *parent = nullptr) : QWidget(parent) {
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

        connect(verifyBtn, &QPushButton::clicked, this, &ModifyInfoWindow::verifyPassword);
    }

private slots:
    void verifyPassword() {
        QString input = passwordEdit->text();
        if (input == correctPassword) {
            EditInfoDialog dlg(this);
            dlg.exec();
        } else {
            QMessageBox::warning(this, "验证失败", "旧密码错误！");
        }
    }

private:
    QLineEdit *passwordEdit;
};

// main 函数
// #include "moc_temp.cpp"
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ModifyInfoWindow window;
    window.show();
    return app.exec();
}

#include <QObject>
// #include "moc_temp.cpp"


#include "main.moc"