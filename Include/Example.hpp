//###########################################################################
//#                                                                         #
//# Module: Internals Example Header File                                   #
//#                                                                         #
//# Description: Declarations for the Internals Example Plugin              #
//#                                                                         #
//#                                                                         #
//# This source code module, and all information, data, and algorithms      #
//# associated with it, are part of CUBE technology (tm).                   #
//#                 PROPRIETARY AND CONFIDENTIAL                            #
//# Copyright (c) 2017 Studio 397 B.V.  All rights reserved.               #
//#                                                                         #
//#                                                                         #
//# Change history:                                                         #
//#   tag.2005.11.30: created                                               #
//#                                                                         #
//###########################################################################

#ifndef _INTERNALS_EXAMPLE_H
#define _INTERNALS_EXAMPLE_H

#include "InternalsPlugin.hpp"


// Esta classe é usada pelo aplicativo para utilizar o plugin com seu propósito original
class ExampleInternalsPlugin : public InternalsPluginV01  // LEMBRETE: a função exportada GetPluginVersion() deve retornar 1 se você estiver derivando desta InternalsPluginV01, 2 para InternalsPluginV02, etc.
{

 public:

  // Constructor/destructor
  ExampleInternalsPlugin() {}
  ~ExampleInternalsPlugin() {}

  // Estas são as funções derivadas da classe base InternalsPlugin
  // que podem ser implementadas.
  void Startup( long version ); // inicialização do jogo
  void Shutdown();              // encerramento do jogo

  void EnterRealtime();         // entrada em tempo real
  void ExitRealtime();          // saída de tempo real

  void StartSession();          // início da sessão
  void EndSession();            // fim da sessão

  // SAÍDA DO JOGO
  long WantsTelemetryUpdates() { return( 1 ); } // ALTERE PARA 1 PARA HABILITAR O EXEMPLO DE TELEMETRIA!
  void UpdateTelemetry( const TelemInfoV01 &info );

  bool WantsGraphicsUpdates() { return( false ); } // // ALTERE PARA TRUE PARA HABILITAR O EXEMPLO DE GRÁFICOS!
  void UpdateGraphics( const GraphicsInfoV01 &info );

  // ENTRADA DO JOGO
  bool HasHardwareInputs() { return( false ); } // ALTERE PARA TRUE PARA HABILITAR O EXEMPLO DE HARDWARE!
  void UpdateHardware( const double fDT ) { mET += fDT; } // atualiza o hardware com o tempo entre frames
  void EnableHardware() { mEnabled = true; }              // mensagem do jogo para habilitar o hardware
  void DisableHardware() { mEnabled = false; }            // mensagem do jogo para desabilitar o hardware

  // Verifica se o plugin deseja assumir o controle de um hardware. Se o plugin assumir o
  // controle, este método retorna true e define o valor do double apontado pelo
  // segundo argumento. Caso contrário, retorna false e deixa o double inalterado
  bool CheckHWControl( const char * const controlName, double &fRetVal );

  bool ForceFeedback( double &forceValue );  // VEJA O CORPO DA FUNÇÃO PARA HABILITAR O EXEMPLO DE FORÇA

  // SAÍDA DE PONTUAÇÃO
  bool WantsScoringUpdates() { return( false ); } // ALTERE PARA TRUE PARA HABILITAR O EXEMPLO DE PONTUAÇÃO!
  void UpdateScoring( const ScoringInfoV01 &info );

  // ENTRADA DE COMENTÁRIOS
  bool RequestCommentary( CommentaryRequestInfoV01 &info );  // VEJA O CORPO DA FUNÇÃO PARA HABILITAR O EXEMPLO DE COMENTÁRIO

 private:

  void WriteToAllExampleOutputFiles( const char * const openStr, const char * const msg );
  double mET; // necessário para o exemplo de hardware
  bool mEnabled; // necessário para o exemplo de hardware
};


#endif // _INTERNALS_EXAMPLE_H

