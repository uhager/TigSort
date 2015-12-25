#include <iostream>
#include <TigTree.h>
#include <TigManager.h>

static const long gIndexOffset = 2; //offset in tree for TigSort EventID, Trigger EventID

//---- TigAssemble
TigAssemble::TigAssemble(void)
  : mEventID(0)
{
  mAnaEventID = TigManager::Instance().AnaEventID();
  TigManager::Instance().IncAnaEventID();
}

//---- ~TigAssemble
TigAssemble::~TigAssemble(void)
{
  vector<int*>::iterator pInt;
  for (pInt=mEventData.begin(); pInt<mEventData.end(); pInt++) delete[] (*pInt);
  mEventData.clear();
  for (int i = mWaveforms.size()-1; i>-1; i--) {
    for (pInt=mWaveforms.at(i).begin(); pInt<mWaveforms.at(i).end(); pInt++) delete[] (*pInt);
    }
  mWaveforms.clear();
  mNWf.clear();
}

//---- AddData
void
TigAssemble::AddData(int pDetector, int pChannel, int pValue)
{
  int* data = mEventData.at(pDetector);
  data[pChannel] = pValue;
}

//---- AddDetector
void
TigAssemble::AddDetector(int pSignals)
{
  mEventData.push_back(new int[pSignals]);
  ::memset(mEventData.back(), -1,  pSignals * sizeof(int));
}

//---- AddWfData
void
TigAssemble::AddWfData(int pDetector, int pChannel, int pSamples, int* pValue)
{
  //  cout << "[TigAssemble::AddWfData] det " << pDetector <<  " ch " << pChannel << " samples " << pSamples  << endl;
  int* data = mWaveforms.at(pDetector).at(pChannel);
  for (int i=0; i<pSamples ; i++){
    data[i] = pValue[i];
    //    cout << mWaveforms.at(pDetector).at(pChannel)[i] << " - " ;
  }
  //  cout << endl;
  mNWf.at(pDetector).at(pChannel) = pSamples;
}

//---- AddWfDetector
void
TigAssemble::AddWfDetector(int pSignals, int pSamples)
{
  //  cout << "[TigAssemble::AddWfDetector] signals " << pSignals << " samples " << pSamples  << endl;
  vector<int*> toAdd;
  vector<int> nsample;
  for (int i=0; i<pSignals; i++){
  int *samples = new int[pSamples];
  ::memset(samples, 0,  pSamples * sizeof(int));
  toAdd.push_back(samples);
  nsample.push_back(0);
  }
  mWaveforms.push_back(toAdd);
  mNWf.push_back(nsample);
}

//---- Data
int*
TigAssemble::Data(int pDet)
{
  return mEventData.at(pDet);
}


//---- Waveform
int*
TigAssemble::Waveform(int pDet, int pChannel)
{
  //  cout << "[TigAssemble::Waveform]" << endl;
  return mWaveforms.at(pDet).at(pChannel);
}

vector<int*>
TigAssemble::WaveformForDetector(int pDet)
{
  //  cout << "[TigAssemble::WaveformForDetector]" << endl;
  return mWaveforms.at(pDet);
}


//////////////////////////////////////////////////
//---- TigTree
TigTree::TigTree(void)
  : mTrigEventID(NULL)
  , mAnaEventID(NULL)
  , mHasEventData(false)
  , mAssembled(NULL)
  , mTree(NULL)
  , mBuffer(1000)
  ,mTimeStamp(0)
{
}

//---- ~TigTree
TigTree::~TigTree(void)
{
  vector<TigDetector*>::iterator detector;
  for (detector= mDetectors.begin(); detector < mDetectors.end(); detector++) delete (*detector);
  vector<TigWaveform*>::iterator wave;
  for (wave = mWaveforms.begin(); wave < mWaveforms.end(); wave++) delete (*wave);
  vector<TigAssemble*>::iterator assemble;
  for (assemble= mAssembled.begin(); assemble < mAssembled.end(); assemble++) delete (*assemble);
  mDetectors.clear();
  mWaveforms.clear();
  mAssembled.clear();
  //  mTree->Delete("all");
}

//---- AddDetector
void
TigTree::AddDetector(TigDetector* pToAdd)
{
  mDetectors.push_back(pToAdd);
}

//---- AddEvent
int
TigTree::AddEvent(int pEventID)
{
  int result;
  for (result=0; result<mAssembled.size(); result++){
    if ( (mAssembled.at(result))->EventID() == pEventID ){
      //cout << "[TigTree::AddEvent] found event " << (mAssembled.at(result))->eventID << " at " << result <<endl;
      return result;
    }
  }
  mAssembled.push_back(new TigAssemble());
  mAssembled.back()->SetEventID(pEventID);
  vector<TigDetector*>::iterator detector;
  for (detector= mDetectors.begin(); detector < mDetectors.end(); detector++) mAssembled.back()->AddDetector( (*detector)->Size());

  vector<TigWaveform*>::iterator wfdetector;
  for (wfdetector= mWaveforms.begin(); wfdetector < mWaveforms.end(); wfdetector++) mAssembled.back()->AddWfDetector( (*wfdetector)->Size(), (*wfdetector)->Samples());

  result = mAssembled.size() - 1;
  return result;
}

//---- AddScaler
void
TigTree::AddScaler(TigScaler* pToAdd)
{
  mScalers.push_back(pToAdd);
}

//---- AddWaveform
void
TigTree::AddWaveform(TigWaveform* pToAdd)
{
  mWaveforms.push_back(pToAdd);
}

//---- Banks
void
TigTree::Banks(vector<string> &pRequested)
{
  for (int i = 0; i<mScalers.size(); i++) pRequested.push_back( mScalers[i]->Bank() );
}

//---- BranchCount
long
TigTree::BranchCount(void) const
{
  //add signals for eventIDs
  return mDetectors.size() + gIndexOffset;
}

//---- FlushBuffer
void
TigTree::FlushBuffer()
{
  vector<TigAssemble*>::iterator assembled;
  for (assembled= mAssembled.begin(); assembled < mAssembled.end(); assembled++) {
    this->FillTree(*assembled);
    delete (*assembled);
  }
  mAssembled.clear();
}

//---- FillTree
bool
TigTree::FillTree()
{
  if ( !mHasEventData ) return false;
  mAnaEventID = TigManager::Instance().AnaEventID();
  mTimeStamp = TigManager::Instance().TimeStamp();
  mTree->Fill();  
  for (int i = 0; i<mScalers.size(); i++) mScalers[i]->Reset();
  mHasEventData = false;
}

bool
TigTree::FillTree(TigAssemble* pAssembled)
{
  //  cout << "[TigTree::FillTree] filling, events " << mAssembled.size() ;
  //  mAnaEventID = TigManager::Instance().AnaEventID();
  mAnaEventID = pAssembled->AnaEventID();
  mTrigEventID = pAssembled->EventID();
  mTimeStamp = TigManager::Instance().TimeStamp();
  if (mDetectors.size() != pAssembled->Size() ){
    cout << "[TigTree::FillTree] detector size mismatch" << endl;
    return 0;
  }
  for (int i =0; i<mDetectors.size(); i++){
    int* detData = pAssembled->Data(i);
    bool process = mDetectors.at(i)->ProcessEvent(detData);
    //	if (process == false ) cout << "[TigTree::FillTree] event size mismatch" << endl;
  }
  for (int i =0; i<mWaveforms.size(); i++){
    vector<int*> pWf = pAssembled->WaveformForDetector(i);
    vector<int> pSamples = pAssembled->NSamplesForDetector(i);
    for (int j=0; j<pWf.size(); j++){
      int samples = pSamples.at(j);
      if (samples < 1) {
	//	cout << "[TigTree::FillTree] " << mWaveforms.at(i)->Name() << " no samples: " << samples << endl;
	continue;
      }
      int* waveform = pWf.at(j);
      bool process = mWaveforms.at(i)->ProcessWfEvent(j, waveform, samples);
      //      cout << "[TigTree::FillTree] " << mWaveforms.at(i)->Name() << " " << j << " "  << mWaveforms.at(i)->mNWf[j] << endl;
    //	if (process == false ) cout << "[TigTree::FillTree] event size mismatch" << endl;
    }
  }
  mTree->Fill();
  for (int i = 0; i<mDetectors.size(); i++) mDetectors[i]->Reset();
  for (int i = 0; i<mWaveforms.size(); i++) mWaveforms[i]->Reset();
	  //  mAnaEventID++;
  return 1;
}

//---- FindScalerBank
bool 
TigTree::FindScalerBank(string pBankName)
{
  for (int i = 0; i<mScalers.size(); i++) if ( pBankName.compare(mScalers[i]->Bank()) == 0 ) return true;
  return false;
}

//---- Initialize
void
TigTree::Initialize(void)
{
  mTree = new TTree(mName.c_str(), mDescription.c_str());
  mTree->Branch("AnalyserEventID",&mAnaEventID,"AnalyserEventID/I");
  mTree->Branch("MidasTimeStamp",&mTimeStamp,"MidasTimeStamp/I");
  if (mDetectors.size()>0) mTree->Branch("TrigEventID",&mTrigEventID,"TrigEventID/I");
  vector<TigDetector*>::iterator detector;
  //  cout << "TigTree::Initialize" << endl;
  for (detector= mDetectors.begin(); detector < mDetectors.end(); detector++)
    {
      char hitName[256], bName[256], bLeaf[256];
      if ((*detector)->Size() == 1){
	sprintf(bLeaf,"%s[%d]/I",(*detector)->Name().c_str(),(*detector)->Size());
	mTree->Branch((*detector)->Name().c_str(),(*detector)->mEventData, bLeaf);	
      }
      else{
      sprintf(hitName,"%s_hits",(*detector)->Name().c_str());
      sprintf(bLeaf,"%s_hits/I",(*detector)->Name().c_str());
      mTree->Branch(hitName,&((*detector)->mHits), bLeaf);
      sprintf(bName,"%s_channel",(*detector)->Name().c_str());
      sprintf(bLeaf,"%s_channel[%s]/I",(*detector)->Name().c_str(),hitName);
      mTree->Branch(bName,(*detector)->mEventChannels, bLeaf);
      sprintf(bName,"%s_value",(*detector)->Name().c_str());
      sprintf(bLeaf,"%s_value[%s]/I",(*detector)->Name().c_str(),hitName);
      mTree->Branch(bName,(*detector)->mEventData, bLeaf);
      }
    }
  vector<TigScaler*>::iterator scaler;
  for (scaler = mScalers.begin(); scaler < mScalers.end(); scaler++)
    {
      for (int i=0; i< (*scaler)->Size() ; i++ )
	{
	  char bLeaf[256];
	  sprintf(bLeaf,"%s/I",(*scaler)->Name(i).c_str());
	  mTree->Branch((*scaler)->Name(i).c_str(),&((*scaler)->mEventData[i]), bLeaf);
	}
    }
  vector<TigWaveform*>::iterator wfDet;
  for (wfDet= mWaveforms.begin(); wfDet < mWaveforms.end(); wfDet++)
    {
      for (int i=0; i<(*wfDet)->Size(); i++){
	char hitName[256], bName[256], bLeaf[256];
	sprintf(hitName,"%s_%d_samples",(*wfDet)->Name().c_str(),(*wfDet)->ChannelForIndex(i));
	sprintf(bLeaf,"%s_%d_samples/I",(*wfDet)->Name().c_str(),(*wfDet)->ChannelForIndex(i));
	mTree->Branch(hitName,&((*wfDet)->mNWf[i]), bLeaf);
	sprintf(bName,"%s_%d_wf",(*wfDet)->Name().c_str(),(*wfDet)->ChannelForIndex(i));
	sprintf(bLeaf,"%s_%d_wf[%s]/I",(*wfDet)->Name().c_str(),(*wfDet)->ChannelForIndex(i),hitName);
	mTree->Branch(bName,((*wfDet)->mWaveforms[i]), bLeaf);

      }
    }

  mTree->SetAutoSave();
}

//---- MakeAssembled
vector<int*>
TigTree::MakeAssembled()
{
  vector<int*> result;
  int* signals;
  vector<TigDetector*>::iterator detector;
  for (detector= mDetectors.begin(); detector < mDetectors.end(); detector++)
   {
     int numSignals = (*detector)->Size();
     signals = new int[numSignals];
    ::memset(signals, -1,  (*detector)->Size() * sizeof(int));
     result.push_back(signals);
    }
 return result;
}

//---- ProcessScaler
bool
TigTree::ProcessScaler(string pBankName, vector<int> pValues)
{
  for (int i = 0; i<mScalers.size(); i++) {
    if ( pBankName.compare(mScalers.at(i)->Bank()) == 0 ) {
      mScalers.at(i)->ProcessEvent(pValues);
      mHasEventData = true;
    }}
  return mHasEventData;
}

//---- ProcessSignal
bool
TigTree::ProcessSignal(TigEvent* pEvent)
{
  bool result = false;
  for (int i = 0; i<mDetectors.size(); i++) {
    bool check = mDetectors.at(i)->FindSignal(pEvent);
    if (check){  
      int assembledNo = this->AddEvent( pEvent->EventID() );
      int pData[] = {-1,-1};
      mDetectors.at(i)->ProcessSignal(pEvent, pData);
      if (pData[0] == -1) continue;
      mAssembled.at(assembledNo)->AddData(i,pData[0],pData[1]);
      result = true;
    }
  }
  for (int i = 0; i<mWaveforms.size(); i++) {
    bool check = mWaveforms.at(i)->FindSignal(pEvent);
    if (check){  
      int assembledNo = this->AddEvent( pEvent->EventID() );
      int channel = -1;
      int samples = mWaveforms.at(i)->Samples() ;
      int *waveform = new int[samples];
      //      cout << "[TigTree::ProcessSignal] waveform samples " << samples << endl;
      mWaveforms.at(i)->ProcessWfSignal(pEvent,&channel, &samples, waveform);
      if (channel == -1) continue;
      // cout << "[TigTree::ProcessSignal] waveform " ;
      // for (int j=0; j<samples; j++) cout << waveform[j] << " - " ;
      // cout << endl;
      mAssembled.at(assembledNo)->AddWfData(i,channel,samples,waveform);
      result = true;
    }
  }

  while (mAssembled.size() > mBuffer)
    {
      this->FillTree(mAssembled.at(0));
      delete mAssembled.front();
      mAssembled.erase(mAssembled.begin());
    }
  return result;
}

