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

struct DriverInfo {

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

struct RscPenaltyID {
    int mID = -1; // ID do piloto
    string mDriverName; // Nome do piloto
    string mVehicleName; // Nome do veiculo
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
    bool WantsToDisplayMessage(MessageInfoV01 &msg);

    // ambiente
    void SetEnvironment(const EnvironmentInfoV01& info);

	bool WantsTrackRulesAccess() { return(true); } // permite acesso às regras de pista
    bool AccessTrackRules(TrackRulesV01& info); // permite acesso às regras de pista

    // variaveis personalizadas
    bool GetCustomVariable( long i, CustomVariableV01 &var );
    void AccessCustomVariable( CustomVariableV01 &var );
    void GetCustomVariableSetting( CustomVariableV01 &var, long i, CustomSettingV01 &setting );

private:

    vector<DriverInfo> mDrivers;
    long mNumPilotos;
    long mMaxPilotos;
    
    // unsigned char mFase;

	// Message to MessageInfoV01
	queue<string> mFilaMensagem;

    float mTempoInicioCorrida = -1.0f; // marcar quando a fase mudou para corrida
    float mDuracaoVerificacao = 10.0f; // segundos de verificação após a largada

    // Full Course Yellow
    bool mFCYAtivo = false;
    
    double mTempoFCY = 0.0;
    int mPilotoCausador = -1;
    double mVelocidadeFCY = 80.0; // Exemplo: 80 km/h
    double mContagemParaFCY = 10.0; // segundos
    double mContagemParaVerde = 10.0; // segundos

    // void CheckMulticlassQualifying();
    
	void InicializarPilotos(const ScoringInfoV01& info);


    // Log
    bool mLogHabilitar;
    string mLogPasta;
    ofstream mLogArquivo;
    
    void CriarLog();
    void EscreverLog(double tempo, const char* msg) const;
	

    // Full Course Yellow

    double mInicioFase = -1;
    double mTempoLimite = -1;
    double mContagemFCY = -1;
    double mContagemGreen = -1;
    double mVelocidadeKPH = -1;

    enum FaseFCY
    {
        FASE_VERDE = 0,
        FASE_LIMITE,
        FASE_CONTAGEM_FCY,
        FASE_FCY,
        FASE_CONTAGEM_GREEN
    };

    FaseFCY mFase;

    void FullCourseYellow(TrackRulesV01& info);


    // Rolling Start Control

    bool mTelemEnabled = false;

    bool mRscEnabled = true;
    long mRscGear = 0;
    long mRscLimiter = 0;
    double mRscMaxVelKPH = 0;
    int mRscPenalty = 0;

    vector<RscPenaltyID> mRscPenaltyList;

    void CheckStartControl(const TelemInfoV01& info);
    // void CheckStartControl(DriverInfo& piloto);
    // void CheckDownshift(DriverInfo& piloto);
};



class FullCourseYellow {
public:

    bool mEnabled;

    float mVerifyTimeFCY;
    float mCountdownFCY;
    float mVerifyTimeGreen;
    float mCountdownGreen;

    


    

private:

    enum class State { GREEN, FCY_COUNTDOWN_1, FCY_COUNTDOWN_2, FCY, GREEN_COUNTDOWN_1, GREEN_COUNTDOWN_2 };

    State mState;

    float mTempoEstado = 0.0f;

    string mGlobalMessage;

    float countdownTime = -1.0f;


    float mCount1FCY = -1.0f;
    float mStartCount = 0.0f;
    int mPilotoCausadorID = -1;
    float mCountPos = -1.0f;

    // Configurações editáveis
    float mCountPreSetting = 10.0f;
    float mCountYellow = 10.0f;
    float mCountGreen = 10.0f;

    TrackRulesStageV01 mStage;




};


#endif // _RACEDIRECTOR_HPP_
