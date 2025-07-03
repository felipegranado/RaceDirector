#ifndef _RACEDIRECTOR_HPP_
#define _RACEDIRECTOR_HPP_

#include "InternalsPlugin.hpp"  // classe base da qual os objetos de plugin devem derivar
#include <ctime>              // para manipulação de data e hora
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>
#include <Windows.h>

using namespace std;

struct PilotoInfo {

    // VehicleScoringInfoV01
    int mIndex = -1;
    long mID = -1;
    string mNome;
    string mVeiculo;
    string mClasse;
    bool mPit = false;


    // TelemInfoV01
    double mVelMPH = 0.0;
    double mVelKPH = 0.0;
	long mMarcha = 0; // 0 = desativado, 1+ marchas, etc.
	unsigned char mLimitador = 0; // 0 = off, 1 = on


    // StartControl
	bool mVerificouLargada = false; // se a largada do piloto foi verificada
	bool mMarchaLargada = false; // marcha que o piloto largou
    bool mDetectouMudanca = false; // detecta reducao de marcha
    int mMarchaAnterior = 0; // para controle de marcha
    bool mJaLogado = false;
    
    
    // TrackRulesParticipantV01
    short mFrozenOrder = -1;
    short mPlace = -1;
    float mYellowSeverity = 0.0f;
    double mCurrentRelativeDistance = 0.0;
    long mRelativeLaps = 0;
    TrackRulesColumnV01 mColumnAssignment = TRCOL_INVALID;
    long mPositionAssignment = -1;
    unsigned char mPitsOpen = 0;
    bool mUpToSpeed = false;
    double mGoalRelativeDistance = 0.0;
    string mMessage;

};


class RaceDirectorPlugin : public InternalsPluginV07 // LEMBRETE: a funcao exportada GetPluginVersion() deve retornar 1 se voce estiver derivando de InternalsPluginV01, 2 para InternalsPluginV02, etc.
{
  public:

    RaceDirectorPlugin();
    //~RaceDirectorPlugin();


    // These are the functions derived from base class InternalsPlugin that can be implemented.
    void StartSession();              // session has started
    // void EndSession();             // session has ended

    // telemetria
    long WantsTelemetryUpdates();
    void UpdateTelemetry( const TelemInfoV01 &info );

    // pontuacao
    bool WantsScoringUpdates() { return(true); }
    void UpdateScoring( const ScoringInfoV01 &info );

    // mensagem
    bool WantsToDisplayMessage(MessageInfoV01 &msgInfo);

    // ambiente
    void SetEnvironment(const EnvironmentInfoV01& info);

	bool WantsTrackRulesAccess() { return(true); } // permite acesso às regras de pista
    bool AccessTrackRules(TrackRulesV01& info); // permite acesso às regras de pista

    // variaveis personalizadas
    bool GetCustomVariable( long i, CustomVariableV01 &var );
    void AccessCustomVariable( CustomVariableV01 &var );
    void GetCustomVariableSetting( CustomVariableV01 &var, long i, CustomSettingV01 &setting );


  protected:

    vector<PilotoInfo> mPilotos;
    long mNumPilotos;
    long mMaxPilotos;
    
    unsigned char mFase;
    bool mAtualizaTelemetria;

    bool mLogEnabled;
    string mLogLocal;

    bool mAplicaPenalidade;
    string mMensagem;
	queue<string> mFilaMensagens;

    float mTempoInicioCorrida = -1.0f; // marcar quando a fase mudou para corrida
    float mDuracaoVerificacao = 10.0f; // segundos de verificação após a largada

    int mIdioma;

    // Custom variables
    bool mStartControlEnabled;
	long mStartControlGear;
	long mStartControlLimiter;
	double mStartControlMaxVelKPH;
    int mStartControlPenalty;

    // Full Course Yellow
    bool mFCYAtivo = false;
    bool mContagemFCY = false;
    double mTempoFCY = 0.0;
    int mPilotoCausador = -1;
    double mVelocidadeFCY = 80.0; // Exemplo: 80 km/h
    double mContagemParaFCY = 10.0; // segundos
    double mContagemParaVerde = 10.0; // segundos

    void GerarLog();
    void EscreverLog(const string msg) const;
    void CheckStartControl(PilotoInfo& piloto);
    void CheckDownshift(PilotoInfo& piloto);
    // void CheckMulticlassQualifying();
    void CheckFullCourseYellow(const TrackRulesParticipantV01* participants, int numParticipants, double currentET);
};


#endif // _RACEDIRECTOR_HPP_
