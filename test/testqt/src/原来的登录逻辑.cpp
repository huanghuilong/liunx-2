//##############################原来的登录逻辑#####################################
    // for (;;)
    // {
    //     // 显示首页面菜单 登录、注册、退出
    //     cout << "========================" << endl;
    //     cout << "1. login" << endl;
    //     cout << "2. register" << endl;
    //     cout << "3. quit" << endl;
    //     cout << "========================" << endl;
    //     cout << "choice:";
    //     int choice = 0;
    //     cin >> choice;
    //     cin.get(); // 读掉缓冲区残留的回车

    //     switch (choice)
    //     {
    //     case 1: // login业务
    //     {
    //         int id = 0;
    //         char pwd[50] = {0};
    //         cout << "userid:";
    //         cin >> id;
    //         cin.get(); // 读掉缓冲区残留的回车
    //         cout << "userpassword:";
    //         cin.getline(pwd, 50);

    //         json js;
    //         js["msgid"] = LOGIN_MSG;
    //         js["id"] = id;
    //         js["password"] = pwd;
    //         string request = js.dump();

    //         g_isLoginSuccess = false;

    //         int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
    //         if (len == -1)
    //         {
    //             cerr << "send login msg error:" << request << endl;
    //         }

    //         sem_wait(&rwsem); // 等待信号量，由子线程处理完登录的响应消息后，通知这里
                
    //         if (g_isLoginSuccess) 
    //         {
    //             // 进入聊天主菜单页面
    //             isMainMenuRunning = true;
    //             mainMenu(clientfd);
    //         }
    //     }
    //     break;
    //     case 2: // register业务
    //     {
            // char name[50] = {0};
            // char pwd[50] = {0};
            // cout << "username:";
            // cin.getline(name, 50);
            // cout << "userpassword:";
            // cin.getline(pwd, 50);

            // json js;
            // js["msgid"] = REG_MSG;
            // js["name"] = name;
            // js["password"] = pwd;
            // string request = js.dump();

            // int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
            // if (len == -1)
            // {
            //     cerr << "send reg msg error:" << request << endl;
            // }
            
            // sem_wait(&rwsem); // 等待信号量，子线程处理完注册消息会通知
    //     }
    //     break;
    //     case 3: // quit业务
    //         close(clientfd);
    //         sem_destroy(&rwsem);
    //         exit(0);
    //     default:
    //         cerr << "invalid input!" << endl;
    //         break;
    //     }
    // }
    //#################################################