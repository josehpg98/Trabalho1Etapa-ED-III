

#include<iostream>
#include<string>
#include<memory>
#include<fstream>
#include<vector>
#define PICOJSON_USE_INT64
#include"picojson.h"
#include"ed3_lib.h"
#include"log.h"

using namespace std;

void deserialize_json(const string& filename, vector<shared_log>& logs);
int main()
{
setlocale(LC_ALL, "Portuguese");
vector<shared_log> logs;
deserialize_json("read-files/log2mega.txt", logs);
cout<<"Objetos lidos: "<<logs.size()<<endl;
return 0;
}

void deserialize_json(const string& filename, vector<shared_log>& logs)
{
    FuncTimer ts(__FUNCTION__);
try {
ifstream ifs(filename);
if(!ifs.is_open())
{
cout<<"N�o consegui abrir o arquivo "<<filename<<endl;
return;
}
string str="";
ifs.seekg(0, ifs.end);
str.resize(ifs.tellg());
ifs.seekg(0, ifs.beg);
ifs.read(&str[0], str.size());
str.resize(ifs.gcount());
ifs.close();
cout<<"Bites lidos: "<<str.size()<<endl;
picojson::value val;
cout<<picojson::parse(val, str)<<endl;
picojson::array& arr=val.get<picojson::array>();
if(arr.size()==0)
{
cout<<"Nenhum objeto encontrado"<<endl;
return;
}
logs.reserve(arr.size()+1);
for(auto it=arr.begin(); it!=arr.end(); ++it)
{
picojson::object obj=(*it).get<picojson::object>();
shared_log lg=make_shared<LOG>();
if(obj.count("month")>0)
{
lg->month=obj.at("month").get<string>();
}
if(obj.count("log")>0)
{
lg->id=static_cast<int32>(obj.at("log").get<int64>());
}
if(obj.count("msg")>0)
{
lg->msg=obj.at("msg").get<string>();
}
if(obj.count("msg")>0)
{
lg->msg=obj.at("msg").get<string>();
}
if(obj.count("user")>0)
{
lg->user=obj.at("user").get<string>();
}
logs.push_back(lg);
}
} catch(const exception& e ) {
cout<<"Exception: "<<e.what()<<endl;
}
}
