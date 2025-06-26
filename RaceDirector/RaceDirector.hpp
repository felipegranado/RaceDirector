#ifndef _RACEDIRECTOR_HPP_
#define _RACEDIRECTOR_HPP_

#include "InternalsPlugin.hpp"  // classe base da qual os objetos de plugin devem derivar
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
namespace fs = filesystem;

struct PilotoInfo {

    // VehicleScoringInfoV01
    int mIndex;
    long mID;
    string mNome;
    string mVeiculo;
    string mClasse;
    bool mPit;

    // TelemInfoV01
    double mVelMPH;
    double mVelKPH;
    long mMarcha;
    unsigned char mLimitador;

	double mMelhorVolta;               // best lap time in current session
    bool mJaLogado = false;

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

    int mIdioma;

    // Custom variables
    bool mStartControlEnabled;
	long mStartControlGear;
	long mStartControlLimiter;
	double mStartControlMaxVelKPH;
    int mStartControlPenalty;

    void GerarLog();
    void EscreverLog(const string msg) const;
    void CheckStartControl(PilotoInfo& piloto);
    // void CheckMulticlassQualifying();
    // void CheckFullCourseYellow();
};


#endif // _RACEDIRECTOR_HPP_
