#ifndef _RACE_DIRECTOR_HPP_
#define _RACE_DIRECTOR_HPP_

#include "InternalsPlugin.hpp"     // classe base da qual os objetos de plugin devem derivar
#include <cstdio>                  // para logging

class RaceDirectorPlugin : public InternalsPluginV07 // LEMBRETE: a funcao exportada GetPluginVersion() deve retornar 1 se voce estiver derivando de InternalsPluginV01, 2 para InternalsPluginV02, etc.
{
  public:

    // SAIDA DO JOGO
    void RegistrarLimitador( const TelemInfoV01 &telem, const VehicleScoringInfoV01 &vehscoring, const ScoringInfoV01 &scoring);

  private:

    // ESCREVER NO LOG
    void WriteLog( const char * const openStr, const char * const msg );
};


#endif // _RACE_DIRECTOR_HPP_