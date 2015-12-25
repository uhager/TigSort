#ifndef TIGDETECTOR_H
#define TIGDETECTOR_H

#include <vector>
#include <string>
#include <TTree.h>
#include <TigEvent.h>

using namespace std;

class TigDetector
{
 public:				//----- public ---------------------
  TigDetector(void);
  ~TigDetector(void);

  void AddSignal(int pChannel, int pAddress);
  void AddSignals(int minCh, int maxCh, int minAdd, int maxAdd);
  void ChangeDescription(string pDesc)	{ mDescription = pDesc; }
  void ChangeName(string pName)			{ mName = pName; }
  int ChannelForIndex(int pInd){return mChannels[pInd];}
  // int EventID(){return mEventID;}
  bool FindSignal(TigEvent* pEvent);
  virtual void Initialize(void);
  string Name(void) { return mName; }
  bool ProcessEvent(int* pData);
  bool ProcessSignal(TigEvent* pEvent,int (&pData)[2]);
  //void				Write(void);
  virtual void Reset();
  void SetDataType(string pTypeString, vector<int> pParameters);
  //  void SetEventID(int pEventID){mEventID = pEventID;}
  int Size(){return mAddresses.size();}
  int* mEventData;
  int* mEventChannels;
  int mHits;


 protected:			//----- protected ------------------
  int mDataType;   // 0: charge (default), 1: CFD, 2: wf energy, 3: wf peak, 4: wf baseline
  string mDescription;
  // int mEventID;
  bool mHasEventData;
  string mName;
  vector<int>	mParameters; // parameters needed for waveform analysis, i.e. bins
   map<int,int> mAddresses; //mAddresses[address] = index
   map<int,int> mChannels; // mChannel[index] = detector strip, translation between mEventData and mEventChannel position
 private:			//----- private --------------------
};

#endif /* TIGDETECTOR_H*/
