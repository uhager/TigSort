#ifndef TIGMANAGER_H
#define TIGMANAGER_H

#include <string>
#include <vector>

#include <TFolder.h>
#include <TObjArray.h>
#include <TList.h>
#include <TMidasEvent.h>

#include "TigTree.h"
#include "TigEvent.h"
#include "TigDetector.h"
#include "TigScaler.h"
#include "TigWaveform.h"
#include "TigUnpack.h"

class TigManager
{
 public:			//----- public ---------------------
  TigManager();
  virtual ~TigManager(void);

  static TigManager&			Instance(void);
  int AnaEventID(){return mAnaEventID;}
  void FlushTreeBuffers();
  void IncAnaEventID(){mAnaEventID++;}
  void ParseInputFile(std::string configFile);
  void ProcessTig64(TMidasEvent* pEvent, std::string pBankName);
  void ProcessSignal(TigEvent* pEvent);
  void ProcessScaler(TMidasEvent* pEvent);
  void ProcessScalerData(std::string pBankName, std::vector<int> pValues);
  int TimeStamp(){return mTimeStamp;}
  void Write(void){}


 protected:			//----- protected ------------------
  void Clear();
  TigDetector *ParseDetector(istream& pStream);
  TigScaler *ParseScaler(istream& pStream);
  TigWaveform *ParseWaveform(istream& pStream);
  void ParsePrimitive(std::string pToken, istream& pStream);
  void ParseTree(istream& pStream);
  int mBuffer;
  std::vector<TigTree*> mTrees;
  TigUnpack* mUnpack;
  TigMCSUnpack* mMCSUnpack;
  int mAnaEventID; // global event ID used by all trees
  int mTimeStamp; // MIDAS event time stamp used by all trees

 private:					//----- private --------------------
  TigManager(const TigManager& pToCopy);
  TigManager&					operator = (const TigManager& pToCopy);
};


#endif /* TIGMANAGER_H */
