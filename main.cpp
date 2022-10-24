

#include <format>
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <vector>
#define PICOJSON_USE_INT64
#include "picojson.h"
#include "ed3_lib.h"
#include "log.h"

using namespace std;

vector<shared_log> original_logs;

void deserialize_json(const string &filename, vector<shared_log> &logs);
void reset_vector(vector<shared_log> &logs);
int getIndex(string month);
void radix_sort(vector<shared_log> &logs);
void counting_sort(vector<shared_log> &logs);
bool special_sort_radix(vector<shared_log> &logs, uint32 find_index, shared_log &out_element);
bool special_sort_counting(vector<shared_log> &logs, uint32 find_index, shared_log &out_element);
void find_logs_by_id_binary_search(const vector<shared_log> &logs, vector<shared_log> &results, int32 value);
void find_logs_by_id_interpolation_search(const vector<shared_log> &logs, vector<shared_log> &results, int32 value);
void find_logs_by_id_linear_search(const vector<shared_log> &logs, vector<shared_log> &results, int32 value);
ostream &operator<<(ostream &os, const vector<shared_log> &logs);

void log_write(const string &s)
{
    static ofstream ofn("zlog.txt");
    ofn << s << endl;
    cout << s << endl;
}
#define _log(str, ...) log_write(vformat(str, make_format_args(__VA_ARGS__)))
bool operator<(const shared_log &l1, const shared_log &l2)
{
    return l1->id < l2->id;
}
int main()
{
    setlocale(LC_ALL, "Portuguese");

    try
    {
        vector<shared_log> logs;

        deserialize_json("read-files/log2mega.txt", logs);

        bool sair = false;
        int32 index = 1000000;

        cout << endl;
        bool sorted = false;
        while (sair == false)
        {
            // system("cls");
            cout << "Escolha uma opcao de ordenacao:" << endl;
            cout << "1: Counting sort" << endl;
            cout << "2: Radix sort" << endl;
            cout << "3: Special_sort com radix_sort:" << endl;
            cout << "4: Special_sort com counting_sort:" << endl;
            cout << "5: Buscar registros com busca binária" << endl;
            cout << "6: Buscar registros com busca interpolada" << endl;
            cout << "7: Buscar registros com busca linear" << endl;
            cout << "0: Sair" << endl;
            int x = -1;
            cout << "Escolha: ";
            cin >> x;
            switch (x)
            {
            case 1:
            {
                system("cls");
                reset_vector(logs);
                counting_sort(logs);
                cout << "\nO culpado eh: " << logs[index]->user << endl;
                cout << "\nEntrada completa:\n"
                     << logs[index] << endl;
                sorted = true;
                getchar();
                break;
            }
            case 2:
            {
                system("cls");
                reset_vector(logs);
                radix_sort(logs);
                cout << "\nO culpado eh: " << logs[index]->user << endl;
                cout << "\nEntrada completa:\n"
                     << logs[index] << endl;
                sorted = true;
                getchar();
                break;
            }
            case 3:
            {
                system("cls");
                shared_log lg;
                uint32 find_index = 1000000;
                reset_vector(logs);
                special_sort_radix(logs, find_index, lg);
                cout << "\nO culpado eh: " << lg->user << endl;
                cout << "\nEntrada completa:\n"
                     << lg << endl;
                sorted = true;
                getchar();
                break;
            }
            case 4:
            {
                system("cls");
                shared_log lg;
                uint32 find_index = 1000000;
                reset_vector(logs);
                special_sort_counting(logs, find_index, lg);
                cout << "\nO culpado eh: " << lg->user << endl;
                cout << "\nEntrada completa:\n"
                     << lg << endl;
                sorted = true;
                getchar();
                break;
            }
            case 5:
            case 6:
            case 7:
            {
                int id = -1;
                cout << "Digite o número de log a buscar: ";
                cin >> id;
                cout << endl;
                if (id < 0)
                {
                    cout << "Log inválido!" << endl;
                    break;
                }
                if (sorted == false)
                {
                    radix_sort(logs);
                    sorted = true;
                }
                vector<shared_log> results;
                if (x == 5)
                {
                    find_logs_by_id_binary_search(logs, results, id);
                }
                else if (x == 6)
                {
                    find_logs_by_id_interpolation_search(logs, results, id);
                }
                else if (x == 7)
                {
                    find_logs_by_id_linear_search(logs, results, id);
                }
                cout << "Total de resultados encontrados para o id " << id << ": " << results.size() << endl;
                if (results.size() > 0)
                {
uint32 j=0;
                    for (auto &it : results)
                    {
                         cout<<it<<endl;
j++;
if(j>2)
{
break;
}
                    }
                }
                break;
            }
            case 0:
            {
                sair = true;
                cout << "Ate mais!" << endl;
                break;
            }
            }
        }
    }
    catch (const exception &e)
    {
        cout << e.what() << endl;
    }
    return 0;
}

ostream &operator<<(ostream &os, const vector<shared_log> &logs)
{
    for (uint32 i = 0; i < logs.size(); i++)
    {
        if (i > 5)
        {
            break;
        }
        cout << logs[i] << endl;
    }
    return os;
}

void reset_vector(vector<shared_log> &logs)
{
    logs.clear();
    logs = original_logs;
}

void deserialize_json(const string &filename, vector<shared_log> &logs)
{
    FuncTimer ts(__FUNCTION__);

    try
    {
        int64 start = gettimestamp();
        ifstream ifs(filename);

        if (!ifs.is_open())
        {
            cout << "Nao consegui abrir o arquivo " << filename << endl;
            return;
        }

        string str = "";

        ifs.seekg(0, ifs.end);
        str.resize(ifs.tellg());
        ifs.seekg(0, ifs.beg);
        ifs.read(&str[0], str.size());
        str.resize(ifs.gcount());
        ifs.close();
        int64 final = (gettimestamp() - start);
        cout << "Tempo de leitura: " << final << " mcs" << endl;
        cout << "Bites lidos: " << str.size() << endl;
        picojson::value val;
        cout << picojson::parse(val, str) << endl;
        picojson::array &arr = val.get<picojson::array>();

        if (arr.size() == 0)
        {
            cout << "Nenhum objeto encontrado" << endl;
            return;
        }

        original_logs.reserve(arr.size() + 1);
        for (auto it = arr.begin(); it != arr.end(); ++it)
        {
            picojson::object obj = (*it).get<picojson::object>();
            shared_log lg = make_shared<LOG>();

            if (obj.count("month") > 0)
            {
                lg->month = obj.at("month").get<string>();
            }
            if (obj.count("log") > 0)
            {
                lg->id = static_cast<int32>(obj.at("log").get<int64>());
            }
            if (obj.count("msg") > 0)
            {
                lg->msg = obj.at("msg").get<string>();
            }
            if (obj.count("msg") > 0)
            {
                lg->msg = obj.at("msg").get<string>();
            }
            if (obj.count("user") > 0)
            {
                lg->user = obj.at("user").get<string>();
            }

            original_logs.push_back(lg);
        }
        reset_vector(logs);
    }
    catch (const exception &e)
    {
        cout << "Exception: " << e.what() << endl;
    }
}

int getIndex(string month)
{
    for (uint32 i = 0; i < month.size(); i++)
    {
        month[i] = tolower(month[i]);
    }
    static map<string, int32> months = {
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
        {"december", 11}};

    auto it = months.find(month);
    if (it == months.end())
    {
        return -1;
    }
    return it->second;
}

void radix_sort(vector<shared_log> &logs)
{
    FuncTimer ts(__FUNCTION__);

    vector<vector<shared_log>> months;
    months.resize(12);

    // Primeiro, vamos classificar os elementos por mes.
    for (uint32 i = 0; i < logs.size(); i++)
    {
        // Converte o mes em um index para ser usado.
        int index = getIndex(logs[i]->month);
        months[index].push_back(logs[i]);
    }
    logs.clear();

    // Segundo passo, percorrer os 12 vetores dos meses e classificar cada um deles.
    for (uint32 i = 0; i < months.size(); i++)
    {
        // Ignora se o vetor estiver vazio...
        if (months[i].size() == 0)
        {
            continue;
        }

        // Para o radix funcionar, precisamos descobrir quem eh o maior elemento.
        int higher = months[i][0]->id;
        for (uint32 i1 = 1; i1 < months[i].size(); i1++)
        {
            if (months[i][i1]->id > higher)
            {
                higher = months[i][i1]->id;
            }
        }

        // Terceiro passo, vamos iterar sobre as casas decimais para podermos ordenar por digitos...
        for (uint32 place = 1; (higher / place) > 0; place *= 10)
        {
            // Nossa matriz de classificacao  por digitos vai aqui...
            vector<vector<shared_log>> digits;
            digits.resize(10);

            // Agora percorremos o mes indicado por I e vamos ordenando digito a digito...
            for (uint32 i1 = 0; i1 < months[i].size(); i1++)
            {
                // Vamos calcular o nosso digito que nos dara um numero entre 0 e 9.
                int32 digit = (months[i][i1]->id / place) % 10;
                digits[digit].push_back(months[i][i1]);
            }

            // E remontamos nosso vetor do mes...
            months[i].clear();
            for (uint32 x = 0; x < digits.size(); x++)
            {
                for (uint32 y = 0; y < digits[x].size(); y++)
                {
                    months[i].push_back(digits[x][y]);
                }
            }
        }

        for (uint32 i1 = 0; i1 < months[i].size(); i1++)
        {
            logs.push_back(months[i][i1]);
        }
    }
}

void counting_sort(vector<shared_log> &logs)
{
    FuncTimer ts(__FUNCTION__);

    vector<vector<shared_log>> months;
    months.resize(12);

    for (uint32 i = 0; i < logs.size(); i++)
    {
        int32 index = getIndex(logs[i]->month);
        months[index].push_back(logs[i]);
    }
    logs.clear();

    // Percorrer os meses e ordenar cada vetor...
    for (uint32 i = 0; i < months.size(); i++)
    {
        if (months[i].size() == 0)
        {
            continue;
        }

        // Primeiro, encontre o maior elemento...
        uint32 higher = months[i][0]->id;
        for (uint32 i1 = 0; i1 < months[i].size(); i1++)
        {
            if (months[i][i1]->id > higher)
            {
                higher = months[i][i1]->id;
            }
        }

        vector<vector<shared_log>> counting;
        counting.resize(higher + 1);

        for (uint32 i1 = 0; i1 < months[i].size(); i1++)
        {
            counting[months[i][i1]->id].push_back(months[i][i1]);
        }

        for (uint32 x = 0; x < counting.size(); x++)
        {
            if (counting[x].size() == 0)
            {
                continue;
            }

            for (uint32 y = 0; y < counting[x].size(); y++)
            {
                logs.push_back(counting[x][y]);
            }
        }
    }
}

bool special_sort_radix(vector<shared_log> &logs, uint32 find_index, shared_log &out_element)
{
    FuncTimer sc(__FUNCTION__);
    vector<vector<shared_log>> months;
    months.resize(12);

    // Primeiro, classificar os logs quanto ao mes...
    for (uint32 i = 0; i < logs.size(); i++)
    {
        months[getIndex(logs[i]->month)].push_back(logs[i]);
    }

    // Segundo, calcular em que mes e indice estara o registro solicitado pelo parametro find_index;
    int32 month_index = -1;
    int32 sz_index = -1;
    int32 general_index = 0;

    for (int32 i = 0; i < months.size(); i++)
    {
        int32 last = general_index;
        general_index += months[i].size();

        if ((general_index > find_index) && (months[i].size() > 0))
        {
            sz_index = (find_index - last);
            month_index = i;
            break;
        }
    }

    if ((month_index < 0) || (sz_index < 0))
    {
        return false;
    }

    radix_sort(months[month_index]);
    out_element = months[month_index][sz_index];

    return true;
}

bool special_sort_counting(vector<shared_log> &logs, uint32 find_index, shared_log &out_element)
{
    FuncTimer sc(__FUNCTION__);
    vector<vector<shared_log>> months;
    months.resize(12);

    // Primeiro, classificar os logs quanto ao mes...
    for (uint32 i = 0; i < logs.size(); i++)
    {
        months[getIndex(logs[i]->month)].push_back(logs[i]);
    }

    // Segundo, calcular em que mes e indice estara o registro solicitado pelo parametro find_index;
    int32 month_index = -1;
    int32 sz_index = -1;
    int32 general_index = 0;

    for (int32 i = 0; i < months.size(); i++)
    {
        int32 last = general_index;
        general_index += months[i].size();

        if ((general_index > find_index) && (months[i].size() > 0))
        {
            sz_index = (find_index - last);
            month_index = i;
            break;
        }
    }

    if ((month_index < 0) || (sz_index < 0))
    {
        return false;
    }

    counting_sort(months[month_index]);
    out_element = months[month_index][sz_index];

    return true;
}

struct RangeFinder
{
    uint32 count(const vector<shared_log> &logs, uint32 id)
    {
        uint32 x = 0;
        for (auto &it : logs)
        {
            if (it->id == id)
            {
                x++;
            }
        }
        return x;
    }
    uint32 find_left(const vector<shared_log> &arr, uint32 start_index)
    {
        uint32 x = start_index;
        while ((x > 0) && (arr[x - 1]->id == arr[start_index]->id))
        {
            x--;
        }
        return x;
    }
    uint32 find_right(const vector<shared_log> &arr, uint32 start_index)
    {
        uint32 x = start_index;
        while ((x < arr.size() - 1) && (arr[x + 1]->id == arr[start_index]->id))
        {
            x++;
        }
        return x;
    }
    void copy(const vector<shared_log> &source, vector<shared_log> &arr, uint32 start, uint32 end)
    {
        uint32 x = 0;
        for (uint32 i = start; i <= end; i++)
        {
            if (i >= source.size())
            {
                break;
            }
            arr.push_back(source[i]);
            x++;
        }
    }
};

void find_logs_by_id_binary_search(const vector<shared_log> &logs, vector<shared_log> &results, int32 value)
{
    struct BinarySearch
    {
        bool operator()(const vector<shared_log> &arr, uint32 val, uint32 *start, uint32 *end)
        {
            uint32 m1 = 0;
            uint32 m2 = arr.size() - 1;
            uint32 x = arr.size();
            uint32 index = arr.size();
            if (arr.size() == 0)
            {
                return false;
            }
            while (m1 < m2)
            {
                x = (m1 + m2) / 2;
                if (arr[x]->id == val)
                {
                    index = x;
                    break;
                }
                else if (val < arr[x]->id)
                {
                    m2 = (x - 1);
                }
                else
                {
                    m1 = (x + 1);
                }
            }
            if (arr[m2]->id == val)
            {
                index = m2;
            }
            if (index < arr.size())
            {
                RangeFinder rf;
                *start = rf.find_left(arr, index);
                *end = rf.find_right(arr, index);
                return true;
            }
            return false;
        }
    };
    static BinarySearch bs;
    static RangeFinder rf;
    vector<vector<shared_log>> htable;
    htable.resize(12);
    for (uint32 i = 0; i < logs.size(); i++)
    {
        htable[getIndex(logs[i]->month)].push_back(logs[i]);
    }
    FuncTimer ts(__FUNCTION__);
    for (uint32 i = 0; i < htable.size(); i++)
    {
        if (htable[i].size() == 0)
        {
            continue;
        }
        uint32 start_index = 0, end_index = 0;
        if (bs(htable[i], value, &start_index, &end_index))
        {
            rf.copy(htable[i], results, start_index, end_index);
        }
    }
}

void find_logs_by_id_interpolation_search(const vector<shared_log> &logs, vector<shared_log> &results, int32 value)
{
    struct InterpolationSearch
    {
        bool operator()(const vector<shared_log> &arr, uint32 val, uint32 *start, uint32 *end)
        {
            int32 m1 = 0;
            int32 m2 = (arr.size() - 1);
            int32 distancia = val - arr[m1]->id;
            int32 intervalo = arr[m2]->id - arr[m1]->id;
            int32 fracao = distancia / intervalo;
            int32 intervalo_indice = (m2 - m1);
            int32 chute = (m1 + fracao) * intervalo_indice;
            int32 i = 0;
            int32 index = arr.size();
            while (i < m2)
            {
                int32 temp = chute;
                chute += pow(2, i);
                if (chute > m2)
                {
                    chute = m2;
                }
                if (arr[chute]->id == val)
                {
                    index = chute;
                    break;
                }
                else if (arr[chute]->id < val)
                {
                    i++;
                }
                else if (arr[chute]->id > val)
                {
                    m1 = 0;
                    m2 = (arr.size() - 1);
                    int32 x = arr.size();
                    while (m1 < m2)
                    {
                        x = (m1 + m2) / 2;
                        if (arr[x]->id == val)
                        {
                            index = x;
                            break;
                        }
                        else if (val < arr[x]->id)
                        {
                            m2 = (x - 1);
                        }
                        else
                        {
                            m1 = (x + 1);
                        }
                    }
                    if (arr[m2]->id == val)
                    {
                        index = m2;
                    }
                    break;
                }
            }
            if (index < arr.size())
            {
                RangeFinder rf;
                *start = rf.find_left(arr, index);
                *end = rf.find_right(arr, index);
                return true;
            }
            return false;
        }
    };
    static InterpolationSearch bs;
    static RangeFinder rf;
    vector<vector<shared_log>> htable;
    htable.resize(12);
    for (uint32 i = 0; i < logs.size(); i++)
    {
        htable[getIndex(logs[i]->month)].push_back(logs[i]);
    }
    FuncTimer ts(__FUNCTION__);
    for (uint32 i = 0; i < htable.size(); i++)
    {
        uint32 start = 0, end = 0;
        if (bs(htable[i], value, &start, &end))
        {
            rf.copy(htable[i], results, start, end);
        }
    }
}

void find_logs_by_id_linear_search(const vector<shared_log> &logs, vector<shared_log> &results, int32 value)
{
    FuncTimer ts(__FUNCTION__);
    for (auto it = logs.begin(); it != logs.end(); ++it)
    {
        if ((*it)->id == value)
        {
            results.push_back((*it));
        }
    }
}
