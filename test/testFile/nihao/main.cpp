#include <bits/stdc++.h>

int main()
{
    FILE *pf = fopen("../mysql.ini", "r"); // 打开文件mysql并设置可读
    if (pf == nullptr){
        std::cout << "mysql.ini is not exit" << std::endl;
        return false;
    }

    while (!feof(pf))    //一直到文件的末尾
    {
        char line[1024] = {0};
        fgets(line, 1024, pf);
        std::string str = line;
        std::cout << str << std::endl;
    }
    return true;
}