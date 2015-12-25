// part of TigSort
// author: Ulrike Hager

#ifndef TIGTREE_H
#define TIGTREE_H

#include <vector>
#include <string>
#include <TTree.h>
#include <TigEvent.h>
#include <TigDetector.h>
#include <TigScaler.h>
#include <TigWaveform.h>

using namespace std;

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
  void SetDetectors(vector<int*> pDets){mEventData = pDets;}
  void SetEventID(int pEventID){mEventID = pEventID;}
  void SetAnaEventID(int pEventID){mAnaEventID = pEventID;}
  int Size(){return mEventData.size();}
  int* Waveform(int pDet, int pChannel);
  vector<int*> WaveformForDetector(int pDet);
  vector<int> NSamplesForDetector(int pDet){return mNWf.at(pDet);}

 private:
  int mEventID;
  int mAnaEventID;
  vector<int*> mEventData;
  vector<vector<int*> > mWaveforms;
  vector<vector<int> > mNWf;
};

class TigTree
{
 public:				//----- public ---------------------
  TigTree(void);
  ~TigTree(void);

  void AddDetector(TigDetector* pToAdd);
  void AddScaler(TigScaler* pToAdd);
  void AddWaveform(TigWaveform* pToAdd);
  void Banks(vector<string> &pRequested);
  int Buffer(){return mBuffer;}
  void ChangeDescription(string pDesc)	{ mDescription = pDesc; }
  void ChangeName(string pName)			{ mName = pName; }
  void DeleteTTree(){mTree->Delete();}
  bool FillTree();
  bool FillTree(TigAssemble* pAssembled);
  bool FindScalerBank(string pBankName);
  void FlushBuffer();
  void Initialize(void);
  string Name(void) { return mName; }
  bool ProcessSignal(TigEvent* pEvent);
  bool ProcessScaler(string pBankName, vector<int> pValues);
  void SetBuffer(int pBuffer){mBuffer=pBuffer;}

 protected:			//----- protected ------------------
  int AddEvent(int pEventID);
  long BranchCount(void) const;
  void ProcessValues(void);

  vector<int*> MakeAssembled();
  int mAnaEventID;
  vector<TigAssemble*> mAssembled;
  int mBuffer;
  string mDescription;
  vector<TigDetector*>   mDetectors;
  bool mHasEventData;
  string mName;
  vector<TigScaler*>   mScalers;
  vector<TigWaveform*>   mWaveforms;
  int mTimeStamp;
  TTree* mTree;
  int mTrigEventID;

 private:			//----- private --------------------
  TigTree(const TigTree& pToCopy);
  TigTree&				operator = (const TigTree& pToCopy);
};


#endif /* TIGTREE_H*/
