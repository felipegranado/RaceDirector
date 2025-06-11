#ifndef _RACEDIRECTOR_HPP_
#define _RACEDIRECTOR_HPP_

#include "InternalsPlugin.hpp"  // classe base da qual os objetos de plugin devem derivar
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>

struct PilotoInfo {

    // VehicleScoringInfoV01
    int mIndex;
    long mID;
    std::string mNome;
    std::string mVeiculo;
    std::string mClasse;
    bool mEmPit;

    // TelemInfoV01
    double mVelMPH;
    double mVelKPH;
    long mMarcha;
    unsigned char mLimitador;

	double mMelhorVolta;               // best lap time in current session
 
};


class RaceDirectorPlugin : public InternalsPluginV07 // LEMBRETE: a funcao exportada GetPluginVersion() deve retornar 1 se voce estiver derivando de InternalsPluginV01, 2 para InternalsPluginV02, etc.
{
  public:

    RaceDirectorPlugin();
    ~RaceDirectorPlugin();


    // These are the functions derived from base class InternalsPlugin that can be implemented.
    void StartSession() override;              // session has started
    // void EndSession();             // session has ended

    // telemetria
    long WantsTelemetryUpdates() override;
    void UpdateTelemetry( const TelemInfoV01 &info ) override;

    // pontuacao
    bool WantsScoringUpdates() override { return(true); }
    void UpdateScoring( const ScoringInfoV01 &info ) override;

    // mensagem
    bool WantsToDisplayMessage(MessageInfoV01& msgInfo) override;

    // ambiente
    void SetEnvironment(const EnvironmentInfoV01& info) override;

    // variaveis personalizadas
    bool GetCustomVariable( long i, CustomVariableV01 &var ) override;
    void AccessCustomVariable( CustomVariableV01 &var ) override;
    void GetCustomVariableSetting( CustomVariableV01 &var, long i, CustomSettingV01 &setting ) override;


  private:

    std::vector<PilotoInfo> mPilotos;
    
    long mNumPilotos;
    long mMaxPilotos;
    
    unsigned char mFase;
    bool mAtualizaTelemetria;

    bool mLogHabilitado;
    std::string mLogLocal;
    std::ofstream mLogArquivo;

    bool mAplicaPenalidade;
    std::string mMensagem;

    int rdIdioma = 0;
    int rdPenalidade = 0;

    // Custom variables
    bool mStartControlEnabled;
	long mStartControlGear;
	long mStartControlLimiter;
	double mStartControlMaxVelKPH;
    int mStartControlPenalty;

    void GerarLog();
    void EscreverLog(const std::string& msg);
    void CheckStartControl();
    // void CheckMulticlassQualifying();
    // void CheckFullCourseYellow();
};


#endif // _RACEDIRECTOR_HPP_
