/*

#ifndef _RACEDIRECTOR_HPP
#define _RACEDIRECTOR_HPP

#include "InternalsPlugin.hpp"  // classe base da qual os objetos de plugin devem derivar
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
namespace fsm = filesystem;


struct StartControl {
    int mID;
    string mNome;
    // float mVelMPH;
    float mVelKPH;
    int mMarcha;
    int mLimitador;
    bool mEmPit;
    // bool mPenalty;
};


struct PilotoInfo {

    // VehicleScoringInfoV01
    int mIndex;
    int mID;
    string mNome;
    string mVeiculo;
    string mClasse;
    bool mEmPit;

    // TelemInfoV01
    float mVelMPH;
    float mVelKPH;
    int mMarcha;
    unsigned char mLimitador;

    // MultiSessionRulesV01
    float mTreinoMelhorTempo;               // best practice time
    int mClassIndice;                      // once qualifying begins, this becomes valid and ranks participants according to practice time if possible
    array<float, 4> mClassTempo;                   // best qualification time in up to 4 qual sessions
    array<float, 4> mCorridaPosicaoFinal;          // final race place in up to 4 race sessions
    array<float, 4> mCorridaTempoFinal;            // final race time in up to 4 race sessions
    bool mAutorizado;                   // whether vehicle is allowed to participate in current session
    int mPosicaoLargada;                   // 1-based grid position for current race session (or upcoming race session if it is currently warmup), or -1 if currently disconnected

    // TrackRulesV01

    void AttTelemetria(const TelemInfoV01& tlm) {
        mID = tlm.mID;
        mVelKPH = sqrt(tlm.mLocalVel.x * tlm.mLocalVel.x + tlm.mLocalVel.y * tlm.mLocalVel.y + tlm.mLocalVel.z * tlm.mLocalVel.z) * 2.23694;
        mVelKPH = sqrt(tlm.mLocalVel.x * tlm.mLocalVel.x + tlm.mLocalVel.y * tlm.mLocalVel.y + tlm.mLocalVel.z * tlm.mLocalVel.z) * 3.6;
        mMarcha = tlm.mGear;
        mLimitador = tlm.mSpeedLimiter;
    }

    
    void AttScoring(int i, const VehicleScoringInfoV01& vsi) {
        mIndex = i;
        mID = vsi.mID;
        mPiloto = vsi.mDriverName;
        mVeiculo = vsi.mVehicleName;
        mClasse = vsi.mVehicleClass;
        mEmPit = vsi.mInPits;
    }

    void AttMultiSessao(const MultiSessionParticipantV01& msp) {
        mTreinoMelhorTempo = msp.mBestPracticeTime;
        mClassIndice = msp.mQualParticipantIndex;
        
        for (int i = 0; i < 4; ++i) {
            mClassTempo[i] = msp.mQualificationTime[i];
            mCorridaPosicaoFinal[i] = msp.mFinalRacePlace[i];
            mCorridaTempoFinal[i] = msp.mFinalRaceTime[i];
        }

        mAutorizado = msp.mServerScored;
        mPosicaoLargada = msp.mGridPosition;
    }
    
};


class RaceDirectorPlugin : public InternalsPluginV07 // LEMBRETE: a funcao exportada GetPluginVersion() deve retornar 1 se voce estiver derivando de InternalsPluginV01, 2 para InternalsPluginV02, etc.
{
  public:

    // Construtor / Destrutor
    RaceDirectorPlugin();
    //~RaceDirectorPlugin() {}

    // These are the functions derived from base class InternalsPlugin that can be implemented.
    void StartSession();              // session has started
    // void EndSession();             // session has ended

    // telemetria
    long WantsTelemetryUpdates();
    void UpdateTelemetry( const TelemInfoV01 &info );

    // SCORING OUTPUT
    bool WantsScoringUpdates() { return(true); }
    void UpdateScoring( const ScoringInfoV01 &info );

    // mensagem
    bool WantsToDisplayMessage(MessageInfoV01& msgInfo);

    // localizacao
    void SetEnvironment(const EnvironmentInfoV01& info);


    bool WantsMultiSessionRulesAccess();
    bool AccessMultiSessionRules(MultiSessionRulesV01& info);

    // variaveis personalizadas
    bool GetCustomVariable( long i, CustomVariableV01 &var );
    void AccessCustomVariable( CustomVariableV01 &var );
    void GetCustomVariableSetting( CustomVariableV01 &var, long i, CustomSettingV01 &setting );


  protected:

    // unordered_map<int, PilotoInfo> mPilotos2;

    long mMaxPilotos;
    long mNumPilotos;
    vector<PilotoInfo> mPilotos;

    StartControl* mStartControl;

    int atualizaTelemetria;
    int telemetriaPilotos;

    unsigned char faseSessao;

    bool aplicaPenalidade;

    int rdIdioma = 0;
    int rdPenalidade = 0;

    // Custom variables
    int mLigado;
    int mLimitador;
    int mMarchaLimite;
    double mMaxVelKPH;
    int mPenalidade;

    string mMensagem;

    int mLog;
    string mLocal;

    const string& ProcurarPilotoPorID(int id) const;

    // void StartControl(PilotoInfo& piloto);

    void NomeArquivo();
    void EscreverLog(const string msg) const;
};

#endif // _RACEDIRECTOR_HPP

*/


#ifndef _RACEDIRECTOR_HPP
#define _RACEDIRECTOR_HPP

#include "InternalsPlugin.hpp"  // classe base da qual os objetos de plugin devem derivar
#include <chrono>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;


struct PilotoInfo {

    // VehicleScoringInfoV01
    int mID;
    string mNome;
    string mVeiculo;
    string mClasse;
    bool mEmPit;

    // TelemInfoV01
    float mVelMPH;
    float mVelKPH;
    int mMarcha;
    unsigned char mLimitador;

    // Penalizacao
    int mPenalidade;
};

class RaceDirectorPlugin : public InternalsPluginV07 {

private:

    ofstream logFile;
    unordered_map<int, PilotoInfo> pilotos;  // Lista de pilotos
    int mFase;                           // Bandeira verde detectada
    int mSessao;                           // Sessão de corrida ativa
    int mTelemetria;


    // Custom Plugin Variable
    int mLog;

    bool mStartControl;
    int mStartControlGear;          // Marcha permitida na largada
    float mStartControlSpeedKPH;      // Velocidade máxima permitida na largada
    int mStartControlLimiter;      // Limitador permitido?
    int mStartControlPenalty;

    // Environment
    string mLocal;

    // Message
    string mMensagem;


    // void ApplyPenalty(int vehicleID);           // Aplicar penalidade
    void NomeArquivo();
    void EscreverLog(const string msg) const;

public:

    RaceDirectorPlugin();
    // ~RaceDirectorPlugin();

    void SetEnvironment(const EnvironmentInfoV01& info);

    // Métodos principais
    void StartSession() override;
    // void EndSession() override;

    bool WantsScoringUpdates() { return(true); }
    void UpdateScoring(const ScoringInfoV01& info);

    long WantsTelemetryUpdates();
    void UpdateTelemetry(const TelemInfoV01& info);


    bool WantsToDisplayMessage(MessageInfoV01& msgInfo);

    // Variáveis customizáveis pelo usuário
    bool GetCustomVariable(long i, CustomVariableV01& var);
    void AccessCustomVariable(CustomVariableV01& var);
    void GetCustomVariableSetting(CustomVariableV01& var, long i, CustomSettingV01& setting);

};

#endif // _RACEDIRECTOR_HPP