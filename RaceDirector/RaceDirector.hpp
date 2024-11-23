#ifndef _RACEDIRECTOR_HPP_
#define _RACEDIRECTOR_HPP_

#include "InternalsPlugin.hpp"  // classe base da qual os objetos de plugin devem derivar
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
namespace fsm = filesystem;

struct PilotoInfo {

    // VehicleScoringInfoV01
    int mIndex;
    int mID;
    string mPiloto;
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


    void AttScoring(int i, const VehicleScoringInfoV01& vsi) {
        mIndex = i;
        mID = vsi.mID;
        mPiloto = vsi.mDriverName;
        mVeiculo = vsi.mVehicleName;
        mClasse = vsi.mVehicleClass;
        mEmPit = vsi.mInPits;
    }

    void AttTelemetria(const TelemInfoV01& tlm) {
        mVelKPH = sqrt(tlm.mLocalVel.x * tlm.mLocalVel.x + tlm.mLocalVel.y * tlm.mLocalVel.y + tlm.mLocalVel.z * tlm.mLocalVel.z) * 2.23694;
        mVelKPH = sqrt(tlm.mLocalVel.x * tlm.mLocalVel.x + tlm.mLocalVel.y * tlm.mLocalVel.y + tlm.mLocalVel.z * tlm.mLocalVel.z) * 3.6;
        mMarcha = tlm.mGear;
        mLimitador = tlm.mSpeedLimiter;
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

    // variaveis personalizadas
    bool GetCustomVariable( long i, CustomVariableV01 &var );
    void AccessCustomVariable( CustomVariableV01 &var );
    void GetCustomVariableSetting( CustomVariableV01 &var, long i, CustomSettingV01 &setting );


  protected:

    long mMaxPilotos;
    long mNumPilotos;
    vector<PilotoInfo> mPilotos;

    bool atualizaTelemetria;
    unsigned char faseSessao;

    bool aplicaPenalidade;


    int rdIdioma = 0;
    int rdPenalidade = 0;

    // Custom variables
    int mLigado;
    int mLimitador;
    int mMarcha;
    double mMaxVelKPH;
    int mPenalidade;

    string mMensagem;

    int mLog;
    string mLocal;

    void StartControl(PilotoInfo& piloto);

    void NomeArquivo();
    void EscreverLog(const string msg) const;
};


#endif // _RACEDIRECTOR_HPP_
