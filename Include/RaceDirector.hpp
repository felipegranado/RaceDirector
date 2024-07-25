#ifndef _RACE_DIRECTOR_HPP_
#define _RACE_DIRECTOR_HPP_

#include "InternalsPlugin.hpp"     // classe base da qual os objetos de plugin devem derivar
#include <cstdio>                  // para logging

class RaceDirectorPlugin : public InternalsPluginV07 // LEMBRETE: a função exportada GetPluginVersion() deve retornar 1 se você estiver derivando de InternalsPluginV01, 2 para InternalsPluginV02, etc.
{
  public:

    // SAÍDA DO JOGO
    long WantsTelemetryUpdates() { return( 1 ); } // ALTERE PARA 1 PARA HABILITAR O EXEMPLO DE TELEMETRIA!
    void RegistrarVelocidade( const TelemInfoV01 &telem, const VehicleScoringInfoV01 &vehscoring, const ScoringInfoV01 &scoring);

  private:

    void WriteLog( const char * const openStr, const char * const msg );
};


#endif // _RACE_DIRECTOR_HPP_