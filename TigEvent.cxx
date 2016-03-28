// part of TigSort
// author: Ulrike Hager

#include <iostream>
#include <cmath>

#include "TigEvent.h"

//---- TigEvent
TigEvent::TigEvent(void)
  :mCharge(-1)
  ,mCFD(-1)
  ,mAddress(-1)
  ,mEventID(-1)
   //  ,mWaveform(NULL)
  ,mTimestamp(-1)
  ,mLifetime(-1)
  ,mTriggersRequested(-1)
  ,mTriggersAccepted(-1)
  ,mTimestampUp(-1)
{
}

//---- ~TigEvent
TigEvent::~TigEvent(void)
{
}

//---- AddWfSample
void
TigEvent::AddWfSample(int pSample)
{
  short sSample = pSample;
  if ( sSample & (1<<13) ) { sSample |= 0x3 << 14; }   // charge bit extension 14 to 16 bits 
  mWaveform.push_back(sSample+8192);
}

//---- CalcWfBase
int
TigEvent::CalcWfBase(std::vector<int> pParameters)
{
  if (mWaveform.size()<250){
    std::cout << "[TigEvent::CalcWfBase] incomplete waveform, length " << mWaveform.size() << std::endl;
    return -1;
  }
  if (pParameters.size()<4){
    std::cout << "[TigEvent::CalcWfBase] too few parameters " << pParameters.size() << std::endl;
    return -1;
  }
  int result = 0;
  for (int i=pParameters.at(0); i<=pParameters.at(1);i++) result += fabs(mWaveform.at(i));
  for (int i=pParameters.at(2); i<=pParameters.at(3);i++) result += fabs(mWaveform.at(i));
  int numBins = pParameters.at(1) - pParameters.at(0) +pParameters.at(3) - pParameters.at(2) +2;
  result /= numBins;
  return result;
}

//---- CalcWfEnergy
int
TigEvent::CalcWfEnergy(std::vector<int> pParameters)
{
  if (mWaveform.size()<250){
    std::cout << "[TigEvent::CalcWfEnergy] incomplete waveform, length " << mWaveform.size() << std::endl;
    return -1;
  }
  if (pParameters.size()<6){
    std::cout << "[TigEvent::CalcWfEnergy] too few parameters " << pParameters.size() << std::endl;
    return -1;
  }
  int result = 0;
  for (int i=pParameters.at(0); i<=pParameters.at(1);i++) result -= mWaveform.at(i);
  for (int i=pParameters.at(4); i<=pParameters.at(5);i++) result += mWaveform.at(i);
  for (int i=pParameters.at(2); i<=pParameters.at(3);i++) result -= mWaveform.at(i);
  result = fabs(result);
  return result;
}

//---- CalcWfMaxBin
int
TigEvent::CalcWfMaxBin()
{
  int result = 0;
  int maxValue = -1000000;
    for (int i=0; i<mWaveform.size();i++) {
      if (mWaveform.at(i) > maxValue) {
	maxValue = mWaveform.at(i);
	result = i;
      }
    }
  return result;
}

//---- CalcWfMinBin
int
TigEvent::CalcWfMinBin()
{
  int result = 0;
  int minValue = 1000000;
    for (int i=0; i<mWaveform.size();i++) {
      if (mWaveform.at(i) <= minValue) {
	minValue = mWaveform.at(i);
	result = i;
      }
    }
  // std::cout << "wf: " ;
  // for (int i=0; i<mWaveform.size();i++) {
  //   if (i==result) std::cout << " _" << mWaveform.at(i) << "_";
  //   else std::cout << " " << mWaveform.at(i);
  //   }
  // std::cout << std::endl;
  return result;
}

//---- CalcWfEnergy
int
TigEvent::CalcWfPeak(std::vector<int> pParameters)
{
  if (mWaveform.size()<250){
    std::cout << "[TigEvent::CalcWfPeak] incomplete waveform, length " << mWaveform.size() << std::endl;
    return -1;
  }
  if (pParameters.size()<2){
    std::cout << "[TigEvent::CalcWfPeak] too few parameters " << pParameters.size() << std::endl;
    return -1;
  }
  int result = 0;
  int numBins = pParameters.at(1) - pParameters.at(0) + 1;
  for (int i=pParameters.at(0); i<=pParameters.at(1);i++) result += (int)(fabs((float)mWaveform.at(i)/(float)numBins));
  return result;
}

//---- Value
int
TigEvent::Value(int pDataType, std::vector<int> pParameters)
{
  switch(pDataType){
  case 0:
    return mCharge;
  case 1:
    return mCFD;
   case 2:
    return CalcWfEnergy(pParameters);
  case 3:
    return CalcWfPeak(pParameters);
  case 4:
    return CalcWfBase(pParameters);
  case 5:
    return CalcWfMaxBin();
  case 6:
    return CalcWfMinBin();
  case 7:
    return mTimestamp;
  case 8:
    return mTimestampUp;
  case 9:
    return mLifetime;
  case 10:
    return mTriggersAccepted;
  case 11:
    return mTriggersRequested;
  default:
    return 0;
  }
}
