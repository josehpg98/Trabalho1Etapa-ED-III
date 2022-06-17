

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



uint32 counter=0;
vector<shared_log> original_logs;
void deserialize_json(const string& filename, vector<shared_log>& logs);
void reset_vector(vector<shared_log>& logs);
int getIndex(string month);
void radix_sort(vector<shared_log>& logs);
void counting_sort(vector<shared_log>& logs);
ostream& operator<<(ostream& os, const vector<shared_log>& logs);
int main()
{
setlocale(LC_ALL, "Portuguese");
try {
vector<shared_log> logs;
deserialize_json("read-files/log2mega.txt", logs);
bool sair=false;
int32 index=1000000;
while(sair==false)
{
cout<<"Escolha uma opção de ordenação:"<<endl;
cout<<"1: Counting sort"<<endl;
cout<<"2: Radix sort"<<endl;
cout<<"0: Sair"<<endl;
int x=-1;
cin>>x;
switch(x)
{
case 1:
{
reset_vector(logs);
counting_sort(logs);
cout<<"O culpado é: "<<logs[index]->user<<endl;
cout<<"Entrada completa:\n"<<logs[index]<<endl;
break;
}
case 2:
{
reset_vector(logs);
radix_sort(logs);
cout<<"O culpado é: "<<logs[index]->user<<endl;
cout<<"Entrada completa:\n"<<logs[index]<<endl;
break;
}
case 0:
{
sair=true;
cout<<"Até mais!"<<endl;
break;
}
}
}
} catch(const exception& e) {
cout<<e.what()<<endl;
}
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

void reset_vector(vector<shared_log>& logs)
{
logs.clear();
logs=original_logs;
}
void deserialize_json(const string& filename, vector<shared_log>& logs)
{
    FuncTimer ts(__FUNCTION__);
try {
ifstream ifs(filename);
if(!ifs.is_open())
{
cout<<"Nï¿½o consegui abrir o arquivo "<<filename<<endl;
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
original_logs.reserve(arr.size()+1);
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
original_logs.push_back(lg);
}
reset_vector(logs);
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

void radix_sort(vector<shared_log>& logs)
{
FuncTimer ts(__FUNCTION__);
vector<vector<shared_log>> months;
months.resize(12);
//Primeiro, vamos classificar os elementos por mês.
for(uint32 i=0; i<logs.size(); i++)
{
counter++;
//Converte o mês em um index para ser usado.
int index=getIndex(logs[i]->month);
months[index].push_back(logs[i]);
}
logs.clear();
//Segundo passo, percorrer os 12 vetores dos mêses e classificar cada um deles.
for(uint32 i=0; i<months.size(); i++)
{
counter++;
//Ignora se o vetor estiver vazio...
if(months[i].size()==0)
{
continue;
}
//Para o radix funcionar, precisamos descobrir quem é o maior elemento.
int higher=months[i][0]->id;
for(uint32 i1=1; i1<months[i].size(); i1++)
{
counter++;
if(months[i][i1]->id>higher)
{
higher=months[i][i1]->id;
}
}
//Terceiro passo, vamos iterar sobre as casas decimais para podermos ordenar por dígitos...
for(uint32 place=1; (higher/place)>0; place*=10)
{
counter++;
//Nossa matriz de classificação  por dígitos vai aqui...
vector<vector<shared_log>> digits;
digits.resize(10);
//Agora percorremos o mês indicado por I e vamos ordenando dígito a dígito...
for(uint32 i1=0; i1<months[i].size(); i1++)
{
counter++;
//Vamos calcular o nosso dígito que nos dará um número entre 0 e 9.
int32 digit=(months[i][i1]->id/place)%10;
if(digits[digit].size()==0)
{
digits[digit].reserve(200000);
}
digits[digit].push_back(months[i][i1]);
}
//E remontamos nosso vetor do mês...
months[i].clear();
for(uint32 x=0; x<digits.size(); x++)
{
counter++;
for(uint32 y=0; y<digits[x].size(); y++)
{
counter++;
months[i].push_back(digits[x][y]);
}
}
}
for(uint32 i1=0; i1<months[i].size(); i1++)
{
counter++;
logs.push_back(months[i][i1]);
}
}
}

void counting_sort(vector<shared_log>& logs)
{
FuncTimer ts(__FUNCTION__);
vector<vector<shared_log>> months;
months.resize(12);
for(uint32 i=0; i<logs.size(); i++)
{
int32 index=getIndex(logs[i]->month);
months[index].push_back(logs[i]);
}
logs.clear();
//Percorrer os mêses e ordenar cada vetor...
for(uint32 i=0; i<months.size(); i++)
{
if(months[i].size()==0)
{
continue;
}
//Primeiro, encontre o maior elemento...
uint32 higher=months[i][0]->id;
for(uint32 i1=0; i1<months[i].size(); i1++)
{
if(months[i][i1]->id>higher)
{
higher=months[i][i1]->id;
}
}
vector<vector<shared_log>> counting;
counting.resize(higher+1);
for(uint32 i1=0; i1<months[i].size(); i1++)
{
counting[months[i][i1]->id].push_back(months[i][i1]);
}
for(uint32 x=0; x<counting.size(); x++)
{
if(counting[x].size()==0)
{
continue;
}
for(uint32 y=0; y<counting[x].size(); y++)
{
logs.push_back(counting[x][y]);
}
}
}
}
