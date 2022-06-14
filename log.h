

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

std::ostream& operator<<(std::ostream& os, const shared_log& s)
{
    os<<"Month: "<<s->month<<"\tLOG_ID: "<<s->id<<"\tUser: "<<s->user<<"\tMSG:"<<s->msg<<std::endl;
    return os;
}

#endif
