// part of TigSort
// author: Ulrike Hager

#ifndef TIGWAVEFORM_H
#define TIGWAVEFORM_H


#include "TigDetector.h"



class TigWaveform : public TigDetector
{
 public:	
  TigWaveform(void);
  ~TigWaveform(void);
  void Initialize(void);
  bool ProcessWfEvent(int pChannel, int* pData, int size);
  bool ProcessWfSignal(TigEvent* pEvent,int *pChannel, int *pSamples, int *&pWf);
  void Reset();
  int Samples(){return mSamples;}
  void SetSamples(int samples);
  int **mWaveforms;
   int *mNWf;  // number of samples in waveform

 protected:	
  int mSamples; // number of samples in waveform
};

#endif /* TIGWAVEFORM_H*/
