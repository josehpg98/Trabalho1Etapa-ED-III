

#include<iostream>
#include<string>
#include<memory>
#include<fstream>
#include<vector>
#include"picojson.h"
#include"log.h"

using namespace std;

void deserialize_json(const string& filename, vector<shared_log>& logs);
int main()
{
setlocale(LC_ALL, "Portuguese");
vector<shared_log> logs;
deserialize_json("logNaveSbornia.txt", logs);
cout<<"Objetos lidos: "<<logs.size()<<endl;
return 0;
}

void deserialize_json(const string& filename, vector<shared_log>& logs)
{
try {
ifstream ifs(filename);
string str="";
ifs.seekg(0, ifs.end);
str.resize(ifs.tellg());
ifs.seekg(0, ifs.beg);
ifs.read(&str[0], str.size());
str.resize(ifs.gcount());
ifs.close();
cout<<"Bites lidos: "<<str.size()<<endl;
picojson::value val;
picojson::parse(val, str);
picojson::array arr=val.get<picojson::array>();
if(arr.size()==0)
{
cout<<"Nenhum objeto encontrado"<<endl;
return;
}
for(auto it=arr.begin(); it!=arr.end(); ++it)
{
picojson::object obj=(*it).get<picojson::object>();

}
} catch(const exception& e ) {
cout<<"Exception: "<<e.what()<<endl;
}

}
