#pragma once
#include "public.h"

class CreateGroupDialog : public QDialog {
    Q_OBJECT
public:
    explicit CreateGroupDialog(QWidget *parent = nullptr);
signals:
    void groupCreationRequested(const QString &name, const QString &desc);
};