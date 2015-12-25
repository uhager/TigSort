#ifndef TIGEVENT_H
#define TIGEVENT_H

#include <vector>
#include <string>

using namespace std;

class TigEvent
{
public:				//----- public ---------------------
  TigEvent(void);
  ~TigEvent(void);
  int Address(){return mAddress;}
  void AddWfSample(int pSample);
  int CalcWfBase(vector<int> pParameters);
  int CalcWfEnergy(vector<int> pParameters);
  int CalcWfMaxBin();
  int CalcWfMinBin();
  int CalcWfPeak(vector<int> pParameters);
  int CFD(){return mCFD;}
  int Charge(){return mCharge;}
  int EventID(){return mEventID;}
  int Lifetime(){return mLifetime;}
  void SetAddress(int pAddress){mAddress=pAddress;}
  void SetCFD(int pCFD){mCFD=pCFD;}
  void SetCharge(int pCharge){mCharge=pCharge;}
  void SetEventID(int pEventID){mEventID = pEventID;}
  void SetLifetime(int pLifetime){mLifetime = pLifetime;}
  void SetTimestamp(int pTimestamp){mTimestamp = pTimestamp;}
  void SetTimestampUp(int pTimestampUp){mTimestampUp = pTimestampUp;}
  void SetTriggersAccepted(int pTriggers){mTriggersAccepted = pTriggers;}
  void SetTriggersRequested(int pTriggers){mTriggersRequested = pTriggers;}
  int Timestamp(){return mTimestamp;}
  int TimestampUp(){return mTimestampUp;}
  int TriggersAccepted(){return mTriggersAccepted;}
  int TriggersRequested(){return mTriggersRequested;}
  int Value(int pDataType, vector<int> pParameters);
  vector<short> Waveform(){return mWaveform;}

protected:			//----- protected ------------------
  int mAddress;
  int mCharge;
  int mCFD;
  int mEventID;
  int mLifetime;
  int mTimestamp;
  int mTimestampUp;
  int mTriggersAccepted;
  int mTriggersRequested;
  vector<short> mWaveform;
 
private:			//----- private --------------------
					TigEvent(const TigEvent& pToCopy);
TigEvent&				operator = (const TigEvent& pToCopy);
};

#endif /* TIGEVENT_H */
