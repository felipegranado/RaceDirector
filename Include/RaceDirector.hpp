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
    ExampleInternalsPlugin() {}
    ~ExampleInternalsPlugin() {}

    void EnterRealtime();          // entering realtime
    void ExitRealtime();           // exiting realtime

    void StartSession();           // session has started
    void EndSession();             // session has ended

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
