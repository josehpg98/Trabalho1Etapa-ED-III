#include <string>
#include <iostream>

struct LOG
{
    std::string month;
    int id;
    std::string msg;
    std::string user;

    LOG()
    {
        month = "";
        id = 0;
        msg = "";
        user =  "";
    }
};
