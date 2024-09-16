#ifndef _RACE_DIRECTOR_HPP_
#define _RACE_DIRECTOR_HPP_

#include "InternalsPlugin.hpp"     // classe base da qual os objetos de plugin devem derivar
#include <cstdio>                  // para logging
#include <stdio.h>                 // para saída de exemplo
#include <math.h>                  // para arco da tangente, raiz quadrada


class RaceDirectorPlugin : public InternalsPluginV07 // LEMBRETE: a funcao exportada GetPluginVersion() deve retornar 1 se voce estiver derivando de InternalsPluginV01, 2 para InternalsPluginV02, etc.
{
  public:

    // Construtor / Destrutor
    RaceDirectorPlugin() {}
    ~RaceDirectorPlugin() {}

  // These are the functions derived from base class InternalsPlugin
    // that can be implemented.
    void Startup( long version );  // game startup
    void Shutdown();               // game shutdown

    void EnterRealtime();          // entering realtime
    void ExitRealtime();           // exiting realtime

    void StartSession();           // session has started
    void EndSession();             // session has ended

    // GAME OUTPUT
    long WantsTelemetryUpdates() { return( 1 ); } // CHANGE TO 1 TO ENABLE TELEMETRY EXAMPLE!
    void UpdateTelemetry( const TelemInfoV01 &info );

    // SCORING OUTPUT
    bool WantsScoringUpdates() { return( false ); } // CHANGE TO TRUE TO ENABLE SCORING EXAMPLE!
    void UpdateScoring( const ScoringInfoV01 &info );

    //
    bool GetCustomVariable( long i, CustomVariableV01 &var );
    void AccessCustomVariable( CustomVariableV01 &var );
    void GetCustomVariableSetting( CustomVariableV01 &var, long i, CustomSettingV01 &setting );

};

    // SAIDA DO JOGO
    void RegistrarLimitador( const TelemInfoV01 &telem, const VehicleScoringInfoV01 &vehscoring, const ScoringInfoV01 &scoring);

  private:

    // ESCREVER NO LOG
    void EscreverLog( const char* const openStr, const char* const msg );

    // necessário para o exemplo de hardware
    double mET;
    bool mEnabled;
};


#endif // _RACE_DIRECTOR_HPP_
