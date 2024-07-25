////////////////////////////////
// Criado por: Felipe Granado //
////////////////////////////////

#include "RaceDirector.hpp"
#include <cstring>
#include <cfloat>


// plugin information

extern "C" __declspec( dllexport )
const char * __cdecl GetPluginName()                   { return( "Race Director - 2024.07.25" ); }

extern "C" __declspec( dllexport )
PluginObjectType __cdecl GetPluginType()               { return( PO_INTERNALS ); }

extern "C" __declspec( dllexport )
int __cdecl GetPluginVersion()                         { return( 7 ); }  // Funcionalidade do InternalsPluginV07 (se você mudar esse valor de retorno, deve derivar da classe apropriada!)

extern "C" __declspec( dllexport )
PluginObject * __cdecl CreatePluginObject()            { return( static_cast< PluginObject * >( new RaceDirectorPlugin ) ); }

extern "C" __declspec( dllexport )
void __cdecl DestroyPluginObject( PluginObject *obj )  { delete( static_cast< RaceDirectorPlugin * >( obj ) ); }


// Classe RaceDirector

RaceDirectorPlugin::RaceDirectorPlugin()
{
  mLogging = false;
  mLogPath = NULL;
  mLogFile = NULL;

  mAdjustFrozenOrder = 0.25f; // Similar to old PLR option "Adjust Frozen Order"
  mAllowFrozenAdjustments = 1;
  mAdjustUntilYellowFlagState = 4;
  mEnableLuckyDogFreePass = true;

  mKnownCar = NULL;
  mFrozenPass = NULL;

  // mLane[].mMember already nulled out
  mAction = NULL;
  Clear();
}