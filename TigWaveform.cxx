#include <iostream>
#include <TigWaveform.h>

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
  //  cout << "[TigWaveform::Initialize] " << mName << " size " << mAddresses.size() << " samples " << mSamples << endl;
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
  // cout << "[TigWaveform::ProcessWfEvent] " << mName << endl;
  //bool result = false; 
  //  mHits = 0;
  if (pChannelIndex>mAddresses.size()-1){
    cout << "[TigWaveform::ProcessWfEvent] " << mName << " index out of bounds" << endl;
    return false;
  }
  for (int i=0; i<size; i++)
    {
      if (pData[i] > -1){
	mWaveforms[pChannelIndex][i] = pData[i];
	if (mNWf[pChannelIndex]==0) mNWf[pChannelIndex]= size;
	//	cout << mWaveforms[pChannelIndex][i] << " - " ;
      }
      else return true;
    }
  //  cout << endl;
  //  cout << "[TigWaveform::ProcessWfEvent] ch " << pChannelIndex << " mNWf[ch] " << mNWf[pChannelIndex]   << endl;
  return true;
}

bool 
TigWaveform::ProcessWfSignal(TigEvent* pEvent,int *pChannel, int *pSamples, int *&pWf)
{
  //  cout << "[TigWaveform::ProcessWfSignal]" << endl;

  if (mAddresses.find(pEvent->Address()) != mAddresses.end() && pEvent->Waveform().size())
    {
      int addr = pEvent->Address();
      mHasEventData = true;
      vector<short> wf = pEvent->Waveform();
      *pChannel = mAddresses[addr];
      *pSamples = wf.size();
      //      cout << "Channel " << *pChannel << " pSamples " << *pSamples << " mSamples " << mSamples << endl;
      for (int i=0; i<(*pSamples); i++) {
	 (pWf[i]) = wf.at(i);
        // cout << "i " << i << " wf.at(i) " << wf.at(i);
	// cout << " pWf[i] " << pWf[i] << endl;
      }
      return true;
    }
 return false;
}

void
TigWaveform::Reset()
{
  //  cout << "[TigWaveform::Reset]" << endl;
  for (int i=0; i<mAddresses.size(); i++) {
    memset(mWaveforms[i],0,mSamples * sizeof(int));
  }
  memset(mNWf, 0,  mAddresses.size() * sizeof(int));
  
}

void
TigWaveform::SetSamples(int samples)
{
  //  cout << "[TigWaveform::SetSamples]" << endl;
  mSamples = samples;
}

