#include "Log.hpp"
#include <ctime>

using namespace std;
namespace fsm = filesystem;

Log::Log()
{
    mLog = 0;
    mLocal = "";
}

Log::~Log()
{
    mLog = 0;
    mLocal = "";
}

int Log::GetLog() const {
    return mLog;
}

const string& Log::GetLocal() const {
    return mLocal;
}


void Log::SetLog(int log) {
    mLog = log;
}

void Log::SetLocal(const std::string& local) {
    mLocal = local;
}


// Cria o arquivo com o caminhop completo em MessageInfoV01 com data e hora do sistema
void Log::SetNomeArquivo()
{
    auto agora = chrono::system_clock::now();
    auto tempo = chrono::floor<chrono::milliseconds>(agora);


    time_t t = chrono::system_clock::to_time_t(tempo);
    tm now;
    localtime_s(&now, &t);

    ostringstream oss;

    oss << "RD-" << put_time(&now, "%Y_%m_%d-%H_%M_%S") << ".txt";

    mLocal +=  oss.str();

/*
    // Obter o tempo atual
    time_t t = time(nullptr);
    tm now;
    localtime_s(&now, &t);

    // Formatar o nome do arquivo com a data e hora
    char filename[32];
    strftime(filename, sizeof(filename), "RD-%Y_%m_%d-%H_%M_%S.txt", &now);
*/
}

/*
void Log::EscreverLog(const char* openStr, const char* fmt, ...)
{
    if (registrarLog == 0)
        return;

    errno_t err = fopen_s(&logFile, mLocal.c_str(), openStr);

    if (err == 0 && logFile != NULL)
    {
        char buffer[512];  // Buffer para armazenar a mensagem formatada
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);  // Formata a string com os parâmetros variáveis
        va_end(args);

        fprintf(logFile, "%s\n", buffer);  // Escreve a mensagem formatada no arquivo

        fclose(logFile);
    }
}
*/

// Escrever o Log no arquivo criado
void Log::EscreverLog2(const string msg) const
{
    if (mLog == 0)
        return;

    ofstream arquivo(mLocal, std::ios::app);

    if (arquivo.is_open())
    {
        arquivo << msg << endl;
        // clog << "TESTE DE MENSAGEM DE LOG" << endl;
    }
    else
    {
        // cerr << "TESTE DE LOG DE ERRO" << endl;
    }

    /*
    errno_t err = fopen_s(&logFile, mLocal.c_str(), "a");

    if (err == 0 && logFile != nullptr)
    {

        fprintf_s( logFile, "%s\n", msg.c_str());  // Escreve a mensagem formatada no arquivo

        fclose(logFile);
    }
    */
}

// Limpar variaveis
void Log::Limpar()
{
    mLocal = "";
}