//���������������������������������������������������������������������������
//�                                                                         �
//� Module: Header file for stock car rules                                 �
//�                                                                         �
//� Description: Implements stock car rules for vehicle races               �
//�                                                                         �
//� This source code module, and all information, data, and algorithms      �
//� associated with it, are part of isiMotor Technology (tm).               �
//�                 PROPRIETARY AND CONFIDENTIAL                            �
//� Copyright (c) 1996-2015 Image Space Incorporated.  All rights reserved. �
//�                                                                         �
//� Change history:                                                         �
//�   tag.2015.09.30: created                                               �
//�                                                                         �
//���������������������������������������������������������������������������

#ifndef _STOCK_CAR_RULES_HPP_
#define _STOCK_CAR_RULES_HPP_

#include "InternalsPlugin.hpp"     // classe base da qual os objetos de plugin devem derivar
#include <cstdio>                  // para logging


class StockCarRulesPlugin : public InternalsPluginV07 // LEMBRETE: a função exportada GetPluginVersion() deve retornar 1 se você estiver derivando de InternalsPluginV01, 2 para InternalsPluginV02, etc.
{
 protected:

  // log
  bool mLogging;                        // se queremos habilitar o log
  char *mLogPath;                       // caminho para gravar os logs
  FILE *mLogFile;                       // o ponteiro para o arquivo

  // controles de comportamento
  float mAdjustFrozenOrder;             // um limite (como uma fração do limite do safety car) para rebaixar veículos na ordem congelada
  long mAllowFrozenAdjustments;         // 0 = não permitir ajustes, 1 = permitir ajustes quando alguém é ultrapassado enquanto causa a bandeira amarela, 2 = ajustar para qualquer ultrapassagem (essencialmente, sem ordem congelada)
  long mAdjustUntilYellowFlagState;     // até qual estado da bandeira amarela parar de ajustar a ordem
  bool mEnableLuckyDogFreePass;         // se o "lucky dog/free pass" está habilitado

  // informações do participante
  struct KnownCar
  {
    // informações básicas ou informações derivadas ao processar as regras da pista
    long mIndex;                        // índice no array mKnownCar
    long mID;                           // ID do slot
    long mLapsDown;                     // número de voltas a menos quando a cautela foi acionada (0 = na volta de liderança)
    float mWorstYellowSeverity;         // pior gravidade de bandeira amarela (uma avaliação de quanto este veículo está causando uma bandeira amarela) na época em que a cautela total foi acionada
    bool mPittedAtDesignatedTime;       // se o veículo entrou nos boxes no tempo designado (dependendo se estava na volta de liderança)
    bool mPittedAtSomeOtherTime;        // se o veículo entrou nos boxes em algum outro momento

    // informações copiadas da última atualização de pontuação
    bool mUnderYellow;                  // se o veículo tomou a cautela total na linha de chegada/partida
    bool mInPits;                       // entre a entrada dos boxes e a saída dos boxes (não sempre preciso para veículos remotos)
    unsigned char mPitState;            // 0=não, 1=pedido, 2=entrando, 3=parado, 4=saindo
    unsigned char mCountLapFlag;        // 0 = não contar volta ou tempo, 1 = contar volta mas não tempo, 2 = contar volta e tempo
    signed char mFinishStatus;          // 0=nada, 1=terminado, 2=abandonou, 3=desclassificado
    unsigned char mPlace;               // posição baseada em 1 (posição atual na corrida)
    short mTotalLaps;                   // voltas completadas
    unsigned char mNumEOLLs;            // número de penalidades de "Fim da Linha Mais Longa"

    char mDriverName[ sizeof( VehicleScoringInfoV01 ) ];    // nome do piloto (isto pode mudar durante uma troca)
    double mPathLateral;                // posição lateral em relação ao "centro" aproximado da pista
    double mLapDist;                    // distância atual na volta

    // Distância total da volta, com uma correção para largadas rolantes, onde mTotalLaps permanece zero durante a primeira
    // passagem pela linha de chegada/partida após o verde. Isso pode ser detectado porque a "bandeira de contagem de volta" muda de 0 para 2.
    double CorrectedTotalLapDist( double fullLapDist ) const
    {
      double ret = ( mTotalLaps * fullLapDist ) + mLapDist;
      if( ( 0 == mTotalLaps ) && ( 0 == mCountLapFlag ) )
        ret -= fullLapDist;
      return( ret );
    }
  };

  long mMaxKnownCars;                   // número de carros conhecidos alocados
  long mNumKnownCars;                   // número de carros conhecidos válidos
  KnownCar *mKnownCar;                  // array de carros conhecidos

  enum PassStatus : unsigned char // isso funciona no VC, mas se houver problemas com outros compiladores, também é possível usar o construto C++11: enum class ObsticalePlans : unsigned char
  {
    PS_NONE = 0,                        // nenhuma ultrapassagem registrada
    PS_PHYSICAL,                        // ultrapassagem física, mas atualmente não legal
    PS_LEGAL                            // uma ultrapassagem legal, mas *talvez* pendente porque alguma outra ultrapassagem ilegal envolvendo um terceiro carro não foi resolvida    
  };
  PassStatus *mFrozenPass;              // array 2-D do status das ultrapassagens na pista sob bandeira amarela (veja o código para uso)

  // ordem na pista em cada faixa (que é registrada e congelada quando a cautela começa, e depois ajustada e mantida durante todo o período de cautela)
  struct Lane
  {
    struct Member
    {
      long mID;                         // ID do slot
      double mDist;                     // distância total da volta quando a cautela foi acionada e o grupo foi congelado
    };

    long mMaxMembers;                   // número de membros alocados
    long mNumMembers;                   // número de membros válidos
    Member *mMember;                    // array de membros

    //
    Lane()                             { mMaxMembers = 0; mNumMembers = 0; mMember = NULL; }
    ~Lane()                            { delete [] mMember; }
    void AddMember( const Member &member )
    {
      if( mNumMembers >= mMaxMembers )
      {
        Member *newMember = new Member[ ++mMaxMembers ];
        for( long i = 0; i < mNumMembers; ++i )
          newMember[ i ] = mMember[ i ];
        delete [] mMember;
        mMember = newMember;
      }
      mMember[ mNumMembers++ ] = member;
    }
  };

  Lane mLane[ TRCOL_MAX_LANES ];        // array de faixas (mas a implementação de stock car usa apenas TRCOL_LEFT_LANE e TRCOL_RIGHT_LANE)

  // armazenar algumas ações para aplicar um pouco mais tarde (quando estivermos certos da ordem correta, considerando possível atraso na rede)
  long mMaxActions;                     // número de ações alocadas
  long mNumActions;                     // número de ações válidas
  TrackRulesActionV01 *mAction;         // array de ações

  // algumas informações de estado
  bool mProcessedLastLapOfCaution;      // Se já processamos a última volta de cautela
  long mLuckyDogID;                     // Acompanhar quem recebeu o "lucky dog" se alguém (-1 se ainda não aplicado)
  double mLeaderLaneChoiceLateral;      // Posição lateral do líder quando solicitado pela primeira vez para fazer uma escolha de faixa (ou DBL_MAX se inválido)
  long mYellowFlagLaps;                 // Número pretendido de voltas com bandeira amarela a serem completadas
  double mCautionET;                    // tempo em que a cautela foi iniciada
  float mRandomGreenDist;               // Um valor aleatório para evitar que a bandeira verde apareça exatamente no mesmo local toda vez

  // informações copiadas da última atualização de pontuação
  long mSession;                        // sessão atual (0=dia de teste 1-4=treino 5-8=qualificação 9=aquele 10-13=corrida)
  double mCurrentET;                    // tempo atual
  double mEndET;                        // tempo de término
  long  mMaxLaps;                       // máximo de voltas
  double mFullLapDist;                  // distância ao redor de uma volta completa da pista
  unsigned char mGamePhase;             // veja InternalsPlugin.hpp
  signed char mYellowFlagState;         // veja InternalsPlugin.hpp

  // método(s) protegido(s)
  void Clear();
  void AddMember( TrackRulesColumnV01 colIndex, const Lane::Member &member );
  bool SafetyCarIsApproachingPitlane( TrackRulesV01 &info, long numLaps );
  void ApplyLuckyDogFreePass( TrackRulesV01 &info );
  const KnownCar *GetFirstCarInRightLaneThatIsOnTheLeadLap() const;

  static int _cdecl CompareFrozen( const void *slot1, const void *slot2 );
  void FreezeOrder( TrackRulesV01 &info );
  void AdjustFrozenOrderNew( TrackRulesV01 &info, Lane &lane );
  void AdjustFrozenOrderOld( TrackRulesV01 &info );
#if 1 // escolha um
  void AdjustFrozenOrder( TrackRulesV01 &info ) { for( long i = 0; i < TRCOL_MAX_LANES; ++i ) AdjustFrozenOrderNew( info, mLane[ i ] ); }
#else
  void AdjustFrozenOrder( TrackRulesV01 &info ) { AdjustFrozenOrderOld( info ); }
#endif
  void RemoveFromOrder( const KnownCar *knownCar );

        KnownCar *GetKnownCarByID( long id );
  const KnownCar *GetKnownCarByID( long id ) const;

  void ClearAllPassStatuses();
  void ClearPassStatuses( const KnownCar *knownCar );
  PassStatus GetPassStatus( const KnownCar *passer, const KnownCar *passee ) const;
  void SetPassStatus( const KnownCar *passer, const KnownCar *passee, PassStatus v );

 public:

  //
  StockCarRulesPlugin();
  ~StockCarRulesPlugin();

  //
  void StartSession();
  void SetEnvironment( const EnvironmentInfoV01 &info ); // pode ser chamado sempre que o ambiente mudar

  //
  bool WantsScoringUpdates()                                { return( true ); } // se queremos atualizações de pontuação
  void UpdateScoring( const ScoringInfoV01 &info );                             // atualiza o plugin com informações de pontuação (aproximadamente cinco vezes por segundo)

  //
  bool WantsTrackRulesAccess()                              { return( true ); } // mudar para true para ler ou escrever a ordem da pista (durante voltas de formação ou de cautela)
  bool AccessTrackRules( TrackRulesV01 &info );                                 // ordem atual da pista passada; retorna true se você quiser mudá-la (nota: isso será chamado imediatamente após UpdateScoring() quando apropriado)

  //
  bool GetCustomVariable( long i, CustomVariableV01 &var );
  void AccessCustomVariable( CustomVariableV01 &var );
  void GetCustomVariableSetting( CustomVariableV01 &var, long i, CustomSettingV01 &setting );
};


#endif // _STOCK_CAR_RULES_HPP_


