// part of TigSort
// author: Ulrike Hager

#include <iostream>

#include "TigScaler.h"


//---- TigScaler
TigScaler::TigScaler(void)
: mEventData(NULL)
, mBank("MCS0")
{
 }

//---- ~TigScaler
TigScaler::~TigScaler(void)
{
	delete [] mEventData;
}

//---- AddRequest
void 
TigScaler::AddRequest(std::string pName, int pChannel)
{
  mNames.push_back(pName);
  mRequested.push_back(pChannel);
}

//---- Initialize
void
TigScaler::Initialize(void)
{
  long numRequest = mRequested.size();
  mEventData = new int[numRequest];	
  for (int i = 0; i<numRequest; i++) {
    mEventData[i] = 0;
  }
}

//---- Name
std::string
TigScaler::Name(int pIndex)
{
  if (pIndex > mNames.size() ){
    std::cout << "[TigScaler::Name] requested name " << pIndex << " out of  " << mNames.size()  << std::endl;
    return "";
  }
  return mNames.at(pIndex);
}

//---- ProcessEvent
bool 
TigScaler::ProcessEvent(std::vector<int> pData)
{
  for (int i = 0; i<mRequested.size(); i++)
    {
      if (mRequested[i] > pData.size()){
	std::cout << "[TigScaler::ProcessEvent] requested scaler channel not in data"<< std::endl;
	return false;
      }
      mEventData[i] = pData[ mRequested[i] ];
    }
      return true;
}

//---- Reset
void
TigScaler::Reset()
{
  ::memset(mEventData, 0, mRequested.size() * sizeof(int) );
}
