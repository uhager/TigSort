// part of TigSort
// author: Ulrike Hager

#include <iostream>
#include <TigDetector.h>

//---- TigDetector
TigDetector::TigDetector(void)
: mEventData(NULL)
, mEventChannels(NULL)
, mHasEventData(false)
, mDataType(0)
, mHits(0)
{
int param[] = {10,15, 200,205, 165,176};
 mParameters = vector<int>(param, param + sizeof(param)/sizeof(int) ); 
}

//---- ~TigDetector
TigDetector::~TigDetector(void)
{
	delete [] mEventData;
	delete [] mEventChannels;
}


//---- AddSignal
void
TigDetector::AddSignal(int pChannel, int pAddress)
{
  int index = this->Size();
  mAddresses[pAddress] = index ;
  mChannels[index] = pChannel;
}

//---- AddSignals
void 
TigDetector::AddSignals(int minCh, int maxCh, int minAdd, int maxAdd)
{
  int numCh = maxCh - minCh +1;
  bool channelInv = false;
  int chStep = 1;
  if (numCh<0) chStep *= -1;
  int addStep;
  if (minAdd < maxAdd) addStep = (maxAdd - minAdd +1)/numCh;
  else addStep = (maxAdd - minAdd -1)/numCh;
  // cout << "[TigDetector::AddSignals] minAdd " << hex << minAdd << " maxAdd " << maxAdd << " addStep " << dec << addStep << endl; 
  for (int i=minCh, add = minAdd; i<maxCh+1; i = i+chStep, add=add+addStep){
    //cout << "[TigDetector::AddSignals] adding " << dec<< i << " - " << hex << add << endl; 
    this->AddSignal(i, add);
  }
}


//---- FindSignal
bool
TigDetector::FindSignal(TigEvent* pEvent)
{
 bool result = false; 
 if (mAddresses.find(pEvent->Address()) != mAddresses.end()) result = true;
 return result;
}

//---- Initialize
void
TigDetector::Initialize(void)
{

  long numAddresses = mAddresses.size();
  mEventData = new int[numAddresses];	
  mEventChannels = new int[numAddresses];	
  for (int i = 0; i<numAddresses; i++) {
    mEventChannels[i] = 0;
    mEventData[i] = -1;
  }
}

//---- ProcessEvent
bool
TigDetector::ProcessEvent(int* pData)
{
  // bool result = false; 
  mHits = 0;
  for (int i=0; i<this->Size(); i++)
    {
      if (pData[i] > -1){
	mEventData[mHits] = pData[i];
	mEventChannels[mHits] = mChannels[i];
	mHits++;
      }
    }
  return true;
}

//---- ProcessSignal
bool
TigDetector::ProcessSignal(TigEvent* pEvent, int (&pData)[2])
{
  bool result = false; 
  if (mAddresses.find(pEvent->Address()) != mAddresses.end())
    {
      int addr = pEvent->Address();
      mHasEventData = true;
      result = true;
      pData[0] = mAddresses[addr]; 
      pData[1] = pEvent->Value(mDataType, mParameters);
      // cout << "[TigDetector::ProcessSignal] channel " << pData[0] << " data " << pData[1] << endl;
       return result;
    }
 // cout << "unknown channel " << pChannel << endl;
 return result;
}

//---- Reset
void
TigDetector::Reset()
{
  if (this->Size() == 1) mEventData[0] = -1; 
  mHits = 0;
}

//---- SetDataType
void
TigDetector::SetDataType(string pTypeString, vector<int> pParameters)
{
  if (pTypeString.compare("Charge") == 0) mDataType = 0;
  else if (pTypeString.compare("CFD") == 0) mDataType = 1;
  else if (pTypeString.compare("WfEnergy") == 0) mDataType = 2;
  else if (pTypeString.compare("WfPeak") == 0) mDataType = 3;
  else if (pTypeString.compare("WfBase") == 0) mDataType = 4;
  else if (pTypeString.compare("WfMaxBin") == 0) mDataType = 5;
  else if (pTypeString.compare("WfMinBin") == 0) mDataType = 6;
  else if (pTypeString.compare("Timestamp") == 0) mDataType = 7;
  else if (pTypeString.compare("TimestampUp") == 0) mDataType = 8;
  else if (pTypeString.compare("Lifetime") == 0) mDataType = 9;
  else if (pTypeString.compare("TriggersAccepted") == 0) mDataType = 10;
  else if (pTypeString.compare("TriggersRequested") == 0) mDataType = 11;
  else mDataType = 0;
  if ( (mDataType == 2) || (mDataType == 3) || (mDataType == 4)  ){
    if (pParameters.size()>0) {
      switch (mDataType){
      case 2:
	if (pParameters.size() >= 6 ) mParameters = pParameters;
	break;
      case 3:
	if (pParameters.size() >= 2 ) mParameters = pParameters;	
	break;
      case 4:
	if (pParameters.size() >= 4 ) mParameters = pParameters;
	break;
      }      
    }
    cout << "[TigDetector::SetDataType] parameters: " ;
    for (int i=0; i<mParameters.size(); i++) cout << mParameters.at(i) << ", ";
    cout << endl;
  }
  //  cout << "[TigDetector::SetDataType] " << pTypeString << " data type is " << mDataType << endl;
}

