////////////////////////////////
// Criado por: Felipe Granado //
////////////////////////////////

#include "RaceDirector.hpp"
#include <cstring>
#include <stdio.h>              // for sample output
#include <cfloat>


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

FILE* logFile = fopen("RaceDirectorLog.txt", "a");


void RaceDirectorPlugin::WriteLog( const char * const openStr, const char * const msg )
{
  if( logFile != NULL )
  {
    // Escrever no log
    char buffer[80];
    fprintf(logFile, "%s%s\n", buffer, msg);

    fprintf( logFile, "%s\n", msg );
  } else {
    // Handle error: arquivo nao pode ser aberto
    fclose( logFile );
  }
}


void RaceDirectorPlugin::RegistrarLimitador( const TelemInfoV01 &telem, const VehicleScoringInfoV01 &vehscoring, const ScoringInfoV01 &scoring)
{
  // Use the incoming data, for now I'll just write some of it to a file to a) make sure it
  // is working, and b) explain the coordinate system a little bit (see header for more info)
  if( logFile != NULL )
  {
    // Registra o tempo, o piloto e o status do Limitador
    fprintf( logFile, "Tempo=%s | Piloto=%s | Limiter=%s\n", scoring.mCurrentET, vehscoring.mDriverName, telem.mSpeedLimiter );
  } else {
    // Handle error: arquivo nao pode ser aberto
    fclose( logFile );
  }
}