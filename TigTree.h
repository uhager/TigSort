// part of TigSort
// author: Ulrike Hager

#ifndef TIGTREE_H
#define TIGTREE_H

#include <vector>
#include <string>

#include <TTree.h>

#include "TigEvent.h"
#include "TigDetector.h"
#include "TigScaler.h"
#include "TigWaveform.h"



class TigAssemble
{
 public:
  TigAssemble(void);
  ~TigAssemble(void);

  void AddData(int pDetector, int pChannel, int pValue);
  void AddDetector(int pSignal);
  void AddWfData(int pDetector, int pChannel, int pSamples, int* pValue);
  void AddWfDetector(int pSignals, int pSamples);

  int AnaEventID(){return mAnaEventID;}
  int* Data(int pDet);
  int EventID(){return mEventID;}
  void SetDetectors(std::vector<int*> pDets){mEventData = pDets;}
  void SetEventID(int pEventID){mEventID = pEventID;}
  void SetAnaEventID(int pEventID){mAnaEventID = pEventID;}
  int Size(){return mEventData.size();}
  int* Waveform(int pDet, int pChannel);
  std::vector<int*> WaveformForDetector(int pDet);
  std::vector<int> NSamplesForDetector(int pDet){return mNWf.at(pDet);}

 private:
  int mEventID;
  int mAnaEventID;
  std::vector<int*> mEventData;
  std::vector<std::vector<int*> > mWaveforms;
  std::vector<std::vector<int> > mNWf;
};

class TigTree
{
 public:				//----- public ---------------------
  TigTree(void);
  ~TigTree(void);

  void AddDetector(TigDetector* pToAdd);
  void AddScaler(TigScaler* pToAdd);
  void AddWaveform(TigWaveform* pToAdd);
  void Banks(std::vector<std::string> &pRequested);
  int Buffer(){return mBuffer;}
  void ChangeDescription(std::string pDesc)	{ mDescription = pDesc; }
  void ChangeName(std::string pName)			{ mName = pName; }
  void DeleteTTree(){mTree->Delete();}
  bool FillTree();
  bool FillTree(TigAssemble* pAssembled);
  bool FindScalerBank(std::string pBankName);
  void FlushBuffer();
  void Initialize(void);
  std::string Name(void) { return mName; }
  bool ProcessSignal(TigEvent* pEvent);
  bool ProcessScaler(std::string pBankName, std::vector<int> pValues);
  void SetBuffer(int pBuffer){mBuffer=pBuffer;}

 protected:			//----- protected ------------------
  int AddEvent(int pEventID);
  long BranchCount(void) const;
  void ProcessValues(void);

  std::vector<int*> MakeAssembled();
  int mAnaEventID;
  std::vector<TigAssemble*> mAssembled;
  int mBuffer;
  std::string mDescription;
  std::vector<TigDetector*>   mDetectors;
  bool mHasEventData;
  std::string mName;
  std::vector<TigScaler*>   mScalers;
  std::vector<TigWaveform*>   mWaveforms;
  int mTimeStamp;
  TTree* mTree;
  int mTrigEventID;

 private:			//----- private --------------------
  TigTree(const TigTree& pToCopy);
  TigTree&				operator = (const TigTree& pToCopy);
};


#endif /* TIGTREE_H*/
