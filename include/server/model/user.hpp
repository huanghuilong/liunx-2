#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
using namespace std;

// User表的ORM类
class User
{
public:
    User(int id = -1, string name = "", string pwd = "", string state = "offline")
    {
        this->id = id;
        this->name = name;
        this->password = pwd;
        this->state = state;
    }

    void setId(int id) { this->id = id; }
    void setName(string name) { this->name = name; }
    void setPwd(string pwd) { this->password = pwd; }
    void setState(string state) { this->state = state; }

    int getId() const{ return this->id; }
    string getName() const{ return this->name; }
    string getPwd() const{ return this->password; }
    string getState() const{ return this->state; }

    // 在 User 类定义中添加
    bool operator==(const User& other) const {
        return this->id == other.id;  // 或其他唯一标识比较
    }
protected:
    int id;
    string name;
    string password;
    string state;
};

#endif