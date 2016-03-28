#include <iostream>

#include <TTree.h>

#include "TigWaveform.h"

TigWaveform::TigWaveform()
  : mSamples(4096)
  , mNWf(NULL)
{
}

TigWaveform::~TigWaveform()
{
  delete mWaveforms;
}

void
TigWaveform::Initialize()
{
  //  std::cout << "[TigWaveform::Initialize] " << mName << " size " << mAddresses.size() << " samples " << mSamples << std::endl;
  long numAddresses = mAddresses.size();
  mWaveforms = new int*[numAddresses];
  for (int i=0; i<numAddresses; i++) {
    mWaveforms[i] = new int[mSamples];	
    memset(mWaveforms[i],0,mSamples * sizeof(int));
  }
  mNWf = new int[numAddresses];
  memset(mNWf, 0,  mAddresses.size() * sizeof(int));
}

bool
TigWaveform::ProcessWfEvent(int pChannelIndex, int* pData, int size)
{
  // std::cout << "[TigWaveform::ProcessWfEvent] " << mName << std::endl;
  //bool result = false; 
  //  mHits = 0;
  if (pChannelIndex>mAddresses.size()-1){
    std::cout << "[TigWaveform::ProcessWfEvent] " << mName << " index out of bounds" << std::endl;
    return false;
  }
  for (int i=0; i<size; i++)
    {
      if (pData[i] > -1){
	mWaveforms[pChannelIndex][i] = pData[i];
	if (mNWf[pChannelIndex]==0) mNWf[pChannelIndex]= size;
	//	std::cout << mWaveforms[pChannelIndex][i] << " - " ;
      }
      else return true;
    }
  //  std::cout << std::endl;
  //  std::cout << "[TigWaveform::ProcessWfEvent] ch " << pChannelIndex << " mNWf[ch] " << mNWf[pChannelIndex]   << std::endl;
  return true;
}

bool 
TigWaveform::ProcessWfSignal(TigEvent* pEvent,int *pChannel, int *pSamples, int *&pWf)
{
  //  std::cout << "[TigWaveform::ProcessWfSignal]" << std::endl;

  if (mAddresses.find(pEvent->Address()) != mAddresses.end() && pEvent->Waveform().size())
    {
      int addr = pEvent->Address();
      mHasEventData = true;
      std::vector<short> wf = pEvent->Waveform();
      *pChannel = mAddresses[addr];
      *pSamples = wf.size();
      //      std::cout << "Channel " << *pChannel << " pSamples " << *pSamples << " mSamples " << mSamples << std::endl;
      for (int i=0; i<(*pSamples); i++) {
	 (pWf[i]) = wf.at(i);
        // std::cout << "i " << i << " wf.at(i) " << wf.at(i);
	// std::cout << " pWf[i] " << pWf[i] << std::endl;
      }
      return true;
    }
 return false;
}

void
TigWaveform::Reset()
{
  //  std::cout << "[TigWaveform::Reset]" << std::endl;
  for (int i=0; i<mAddresses.size(); i++) {
    memset(mWaveforms[i],0,mSamples * sizeof(int));
  }
  memset(mNWf, 0,  mAddresses.size() * sizeof(int));
  
}

void
TigWaveform::SetSamples(int samples)
{
  //  std::cout << "[TigWaveform::SetSamples]" << std::endl;
  mSamples = samples;
}

