////////////////////////////////
// Criado por: Felipe Granado //
////////////////////////////////

#include "RaceDirector.hpp"
#include <cstring>
#include <cfloat>

using namespace std;

// plugin information

extern "C" __declspec( dllexport )
const char * __cdecl GetPluginName()                   { return( "Race Director - 2024.07.25" ); }

extern "C" __declspec( dllexport )
PluginObjectType __cdecl GetPluginType()               { return( PO_INTERNALS ); }

extern "C" __declspec( dllexport )
int __cdecl GetPluginVersion()                         { return( 7 ); }  // Funcionalidade do InternalsPluginV07 (se voce mudar esse valor de retorno, deve derivar da classe apropriada!)

extern "C" __declspec( dllexport )
PluginObject * __cdecl CreatePluginObject()            { return( static_cast< PluginObject * >( new RaceDirectorPlugin ) ); }

extern "C" __declspec( dllexport )
void __cdecl DestroyPluginObject( PluginObject *obj )  { delete( static_cast< RaceDirectorPlugin * >( obj ) ); }


// Classe RaceDirector

void RaceDirectorPlugin::EscreverLog()
{
    // Buffer para armazenar o nome do arquivo com data e hora
    char filename[100];

    // Obter o tempo atual
    time_t t = time(nullptr);
    tm* now = localtime(&t);

    // Formatar o nome do arquivo com a data e hora
    strftime(filename, sizeof(filename), "RD-%Y_%m_%d-%H_%M_%S.txt", now);

    // Abrir o arquivo no modo "append" (acrescentar dados ao final)
    FILE* logFile = fopen(filename, "a");
    
    // Verificar se o arquivo foi aberto com sucesso
    if (logFile != NULL)
    {
        // Escrever a mensagem (msg) no arquivo
        fprintf(logFile, "%s\n", msg);
    }
    else
    {
        // Se falhar, imprimir mensagem de erro (opcional)
        cout << "Erro ao abrir o arquivo de log: " << filename << endl;
    }
}


void RaceDirectorPlugin::Startup( long version )
{
    EscreverLog("a", "--- LOG INICIADO (version %.3f) ---", (float) version / 1000.0f);
}


void RaceDirectorPlugin::StartSession()
{
    EscreverLog("a", "--- SESSAO INICIADA ---");
}


// Função chamada quando a sessão é encerrada
void ExampleInternalsPlugin::EndSession()
{
    EscreverLog("a", "--- SESSAO ENCERRADA ---");
}


// Função chamada quando o plugin é descarregado ou o servidor é desligado
void RaceDirectorPlugin::Shutdown()
{
    if (logFile != NULL)
    {
        EscreverLog("a", "--- LOG ENCERRADO ---");
        fclose(logFile);  // Fechar o arquivo quando o plugin for descarregado
        logFile = NULL;    // Limpar o ponteiro
    }
}


void RaceDirectorPlugin::RegistrarLimitador( const TelemInfoV01 &telem, const VehicleScoringInfoV01 &vehscoring, const ScoringInfoV01 &scoring)
{
    // Registra o tempo, o piloto e o status do Limitador
    EscreverLog( "a", "Tempo=%.2f | Piloto=%s | Limiter=%d\n", scoring.mCurrentET, vehscoring.mDriverName, (int) telem.mSpeedLimiter  );
}
