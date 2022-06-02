

#ifndef LOG_H
#define LOG_H

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

typedef std::shared_ptr<LOG> shared_log;

#endif
