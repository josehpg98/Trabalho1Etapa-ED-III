

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
int getIndex(string month);
uint32 findMax(const vector<shared_log>& logs);
void ccsort(vector<shared_log>& logs);
void sort_month(vector<shared_log>& logs);
ostream& operator<<(ostream& os, const vector<shared_log>& logs);
int main()
{
setlocale(LC_ALL, "Portuguese");
vector<shared_log> logs;
deserialize_json("read-files/log2mega.txt", logs);
cout<<"Ordenando vetor..."<<endl;
ccsort(logs);
cout<<"Vetor Ordenado!"<<endl;
cout<<logs.size()<<endl;
cout<<"Poss�veis culpados:"<<endl;
cout<<"�ndice 1000000: "<<logs[1000000]<<endl;
cout<<"�ndice 1000001: "<<logs[1000001]<<endl;
return 0;
}

ostream& operator<<(ostream& os, const vector<shared_log>& logs)
{
for(uint32 i=0; i<logs.size(); i++)
{
if(i>5)
{
break;
}
cout<<logs[i]<<endl;
}
return os;
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

int getIndex(string month)
{
month[0]=tolower(month[0]);
static map<string, int32> months={
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

uint32 findMax(const vector<shared_log>& logs)
{
if(logs.size()==0)
{
return 0;
}
uint32 x=logs[0]->id;
for(uint32 i=1; i<logs.size(); i++)
{
if(logs[i]->id>x)
{
x=logs[i]->id;
}
}
return x;
}

void ccsort(vector<shared_log>& logs)
{
FuncTimer ts(__FUNCTION__);
vector<vector<shared_log>> months;
months.resize(12);
for(uint32 i=0; i<logs.size(); i++)
{
int32 x=getIndex(logs[i]->month);
if((x<0)||(x>11))
{
continue;
}
months[x].push_back(logs[i]);
}
logs.clear();
for(uint32 i=0; i<months.size(); i++)
{
sort_month(months[i]);
for(uint32 i1=0; i1<months[i].size(); i1++)
{
logs.push_back(months[i][i1]);
}
}
}

void sort_month(vector<shared_log>& logs)
{
vector<vector<shared_log>> arr;
arr.resize(findMax(logs)+1);
for(uint32 i=0; i<logs.size(); i++)
{
arr[logs[i]->id].push_back(logs[i]);
}
logs.clear();
for(uint32 i=0; i<arr.size(); i++)
{
if(arr[i].size()==0)
{
continue;
}
for(uint32 i1=0; i1<arr[i].size(); i1++)
{
logs.push_back(arr[i][i1]);
}
}
}
