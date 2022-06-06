

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

vector<vector<shared_log>> separed(12);


void deserialize_json(const string& filename, vector<shared_log>& logs);
void sort_logs(vector<shared_log>& logs);
int getIndex(string month);
vector<shared_log>  findElement(int32 index, int32* outindex);
int main()
{
setlocale(LC_ALL, "Portuguese");
vector<shared_log> logs;
deserialize_json("read-files/log_10.txt", logs);
cout<<"Objetos lidos: "<<logs.size()<<endl;
sort_logs(logs);
for(uint32 i=0; i<separed.size(); i++)
{
for(uint32 i1=0; i1<separed[i].size(); i1++)
{
    cout<<separed[i][i1];
}
}
/*
for(int i=0; i<12; i++)
{
vector<shared_log> s=findElement(i);
cout<<"Array size: "<<s.size()<<endl;
if(s.size()>0)
{
cout<<s[0]<<endl;
}
}
*/
int32 index=0;
vector<shared_log> s=findElement(2, &index);
cout<<"Array size: "<<s.size()<<endl;
if(s.size()>0)
{
cout<<s[index]<<endl;
}

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


void sort_logs(vector<shared_log>& logs)
{
for(auto& it : logs)
{
    int32 index=getIndex(it->month);
    separed[index].push_back(it);
}
}

int getIndex(string month)
{
month[0]=tolower(month[0]);
map<string, int32> months={
{"january", 0},
{"february", 1},
{"march", 2},
{"april", 3},
{"may", 4},
{"june", 5},
{"july", 6},
{"august", 7},
{"september", 8},
{"october", 9},
{"november", 10},
{"december", 11}
};
auto it=months.find(month);
if(it==months.end())
{
return -1;
}
return it->second;
}

vector<shared_log>  findElement(int32 index, int32* outindex)
{
    int32 x=0;
    for(int i=0; i<separed.size(); i++)
    {
int last=x;        
x+=separed[i].size();
if((x>index)&&(separed[i].size()>0))
{
    if(outindex!=NULL)
    {
        *outindex=(index-last);
    }
return separed[i];
}
}
return vector<shared_log>();
    }
