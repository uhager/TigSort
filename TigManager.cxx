// part of TigSort
// author: Ulrike Hager

#include <iostream>
#include <sstream>
//#include <stdlib.h>
#include <fstream>

#include <TROOT.h>
#include <TFolder.h>

#include "TigManager.h"
#include "TigTree.h"



static TigManager* gInstance = NULL;

//---- TigManager
TigManager::TigManager(void)
  : mBuffer(1000)
  ,mAnaEventID(0)
  ,mUnpack(NULL)
  ,mMCSUnpack(NULL)
{
  if ( gInstance == NULL)
    gInstance = this;

  else
    {
      std::cout << "[TigManager::TigManager]  Manager is a singleton class and can only be constructed once." << std::endl;
      exit(1);
    }
}

//---- ~TigManager
TigManager::~TigManager(void)
{
  this->Clear();
}

//---- Clear
void
TigManager::Clear(void)
{
  std::vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++)
    delete (*trees);
  mTrees.clear();	
}

//---- Instance
TigManager&
TigManager::Instance(void)
{
  if ( !gInstance)
    {
      std::cout << "[TigManager::Instance]  Must construct a manager before calling Instance." << std::endl;
      exit(1);
    }
	
  return *gInstance;
}

//---- FlushTreeBuffers
void
TigManager::FlushTreeBuffers()
{
  std::vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++) (*trees)->FlushBuffer();
}

//---- ParseInputFile
void
TigManager::ParseInputFile(std::string configFile)
{
  //  char *inputFile = NULL;
  std::string inputFile = "-"; 
  if (configFile.compare("-") != 0) inputFile = configFile;
  else inputFile = getenv("TIGSORT_INPUT");
  if (inputFile.compare("-") == 0)
    {
      std::cout << "[KoManager::ParseInputFile]  No 'TIGSORT_INPUT' input file specified ";
      exit(1);
    }

  std::string line, token;
  ifstream input(inputFile.c_str());
  this->Clear();
  std::cout << "Parsing TigSort specification file...";

  getline(input,line);
  while ( input)
    {
      std::istringstream stream(line.c_str());
     
      stream >> token;
      if ( token == "" || token[0] == '#')
	{
	  //comment or empty
	}
      else if (token == "buffer"){
       	stream >> mBuffer;
	for (int i=0;i<mTrees.size();i++) mTrees.at(i)->SetBuffer(mBuffer);
      }
      else 
	this->ParsePrimitive(token, input);
	  
      if ( input)
	getline(input,line);
    }
  std::cout << "TigManager input parsing done.\n";
}

//---- ParseDetector
TigDetector*
TigManager::ParseDetector(istream& pStream)
{
  std::string line, token;
  bool	 bail = false;
  TigDetector*	 det = new TigDetector;

  getline(pStream,line);
  while ( pStream && !bail)
    {
      std::istringstream stream(line.c_str());
	
      stream >> token;
      if ( token == "" || token[0] == '#')
	{
	  //comment or blank
	}
      else if ( token.compare("description") == 0)
	{
	  stream >> token;
	  det->ChangeDescription(token);
	}
      else if ( token.compare("end") == 0)
	bail = true;
      else if ( token.compare("datatype") == 0)
	{
	  stream >> token;
	  std::vector<int> parameters;
	  while ( !stream.eof() ) {
	    parameters.push_back(0);
	    stream >> parameters.back();
	  }
	  det->SetDataType(token,parameters);
	}
      else if ( token.compare("name") == 0)
	{	
	  stream >> token;
	  std::cout << ".(" << token << ").";
	  det->ChangeName(token);
	}
      else if ( token.compare("signals") == 0)
	{
	  bool bailSignals = false;
		
	  getline(pStream,line);
	  while ( pStream && !bailSignals)
	    {
	      std::istringstream subStream(line.c_str());
			
	      //	      std::cout << "signals: " << line << std::endl;
	      subStream >> token;
	      if ( token == "" || token[0] == '#') {}  //comment or blank
	      else if ( token.compare("end") == 0)
		bailSignals = true;
	      else if ( token.compare("range") == 0)
		{
		  int minCh, maxCh, minAdd, maxAdd;
		  subStream >> minCh >> maxCh >> std::hex >> minAdd >> std::hex >> maxAdd;
		  //  std::cout << "[TigManager::ParseDetector] range " << minCh << "\t" << maxCh  << "\t" << minAdd  << "\t" << maxAdd << std::endl;
		  det->AddSignals(minCh, maxCh, minAdd, maxAdd);
		}
	      else
		{
		  int channel;
		  std::istringstream ( token ) >> channel;
		  int address;				
		  subStream >> std::hex >> address;
		  det->AddSignal(channel,address);
		  //  std::cout << "added signal: " << channel << " - " << address << std::endl;
		}
	      if ( !bailSignals)
		getline(pStream,line);
	    }
	}
      if ( !bail)
	getline(pStream,line);
    }
  det->Initialize();
  //  std::cout << "new detector: " << det->Name() << " #signals: " << det->Size() << std::endl;
  return det;
}

//---- ParsePrimitive
void
TigManager::ParsePrimitive(std::string pToken, istream& pStream)
{
  if ( pToken.compare("tree") == 0)
    this->ParseTree(pStream);
  else
    {
      std::cout << "[TigManager::ParsePrimitive]  Unknown primtive token: " << pToken << std::endl;
    }
}

//---- ParseScaler
TigScaler*
TigManager::ParseScaler(istream& pStream)
{
  std::string line, token;
  bool	 bail = false;
  TigScaler* scaler = new TigScaler;

  getline(pStream,line);
  while ( pStream && !bail)
    {
      std::istringstream stream(line.c_str());
	
      stream >> token;
      if ( token == "" || token[0] == '#')
	{
	  //comment or blank
	}
      else if ( token.compare("end") == 0)
	bail = true;
      else if ( token.compare("bank") == 0)
	{	
	  stream >> token;
	  std::cout << ".(" << token << ").";
	  scaler->SetBank(token);
	}
      else if ( token.compare("signals") == 0)
	{
	  bool bailSignals = false;
		
	  getline(pStream,line);
	  while ( pStream && !bailSignals)
	    {
	      std::istringstream subStream(line.c_str());
			
	      //	      std::cout << "signals: " << line << std::endl;
	      subStream >> token;
	      if ( token == "" || token[0] == '#') {}  //comment or blank
	      else if ( token.compare("end") == 0)
		bailSignals = true;
	      else
		{
		  int channel;
		  subStream >> channel;
		  scaler->AddRequest(token, channel);
		  //  std::cout << "added signal: " << channel << " - " << token << std::endl;
		}
	      if ( !bailSignals)
		getline(pStream,line);
	    }
	}
      if ( !bail)
	getline(pStream,line);
    }
  scaler->Initialize();
  return scaler;
}

//---- ParseTree
void
TigManager::ParseTree(istream& pStream)
{
  std::string line, token;
  bool	 bail = false;
  TigTree*	 tree = new TigTree;

  getline(pStream,line);
  while ( pStream && !bail)
    {
      std::istringstream stream(line.c_str());
	
      stream >> token;
      if ( token == "" || token[0] == '#')
	{
	  //comment or blank
	}
      else if ( token.compare("description") == 0)
	{
	  stream >> token;
	  tree->ChangeDescription(token);
	}
      else if ( token.compare("end") == 0)
	bail = true;
      else if ( token.compare("name") == 0)
	{	
	  stream >> token;
	  std::cout << ".(" << token << ").";
	  tree->ChangeName(token);
	}
      else if ( token.compare("detector") == 0)
	{
	  TigDetector* det = this->ParseDetector(pStream);
	  tree->AddDetector(det);
	  if (!mUnpack) mUnpack = new TigUnpack;	
	}
      else if ( token.compare("waveform") == 0)
	{
	  TigWaveform* det = this->ParseWaveform(pStream);
	  tree->AddWaveform(det);
	  if (!mUnpack) mUnpack = new TigUnpack;	
	}
      else if ( token.compare("scaler") == 0)
	{
	  TigScaler* scaler = this->ParseScaler(pStream);
	  tree->AddScaler(scaler);
	  if (!mMCSUnpack) mMCSUnpack = new TigMCSUnpack;	
	}
	
      if ( !bail)
	getline(pStream,line);
    }

  tree->SetBuffer(mBuffer);	
  tree->Initialize();
  mTrees.push_back(tree);
}

//---- ParseWaveform
TigWaveform*
TigManager::ParseWaveform(istream& pStream)
{
  std::string line, token;
  bool	 bail = false;
  TigWaveform*	 det = new TigWaveform;

  getline(pStream,line);
  while ( pStream && !bail)
    {
      std::istringstream stream(line.c_str());
	
      stream >> token;
      if ( token == "" || token[0] == '#')
	{
	  //comment or blank
	}
      else if ( token.compare("description") == 0)
	{
	  stream >> token;
	  det->ChangeDescription(token);
	}
      else if ( token.compare("end") == 0)
	bail = true;
      else if ( token.compare("name") == 0)
	{	
	  stream >> token;
	  std::cout << ".(" << token << ").";
	  det->ChangeName(token);
	}
      else if (token.compare("samples")==0)
	{
	  int samples;
	  stream >> samples;
	  det->SetSamples(samples);
	}
      else if ( token.compare("signals") == 0)
	{
	  bool bailSignals = false;
		
	  getline(pStream,line);
	  while ( pStream && !bailSignals)
	    {
	      std::istringstream subStream(line.c_str());
			
	      //	      std::cout << "signals: " << line << std::endl;
	      subStream >> token;
	      if ( token == "" || token[0] == '#') {}  //comment or blank
	      else if ( token.compare("end") == 0)
		bailSignals = true;
	      else if ( token.compare("range") == 0)
		{
		  int minCh, maxCh, minAdd, maxAdd;
		  subStream >> minCh >> maxCh >> std::hex >> minAdd >> std::hex >> maxAdd;
		  //  std::cout << "[TigManager::ParseDetector] range " << minCh << "\t" << maxCh  << "\t" << minAdd  << "\t" << maxAdd << std::endl;
		  det->AddSignals(minCh, maxCh, minAdd, maxAdd);
		}
	      else
		{
		  int channel;
		  std::istringstream ( token ) >> channel;
		  int address;				
		  subStream >> std::hex >> address;
		  det->AddSignal(channel,address);
		  //  std::cout << "added signal: " << channel << " - " << address << std::endl;
		}
	      if ( !bailSignals)
		getline(pStream,line);
	    }
	}
      if ( !bail)
	getline(pStream,line);
    }
  det->Initialize();
  //  std::cout << "new detector: " << det->Name() << " #signals: " << det->Size() << std::endl;
  return det;
}



//---- ProcessScaler
void
TigManager::ProcessScaler(TMidasEvent* pEvent)
{
  if (mMCSUnpack)
    {
      //  std::cout << "Processing scaler" << std::endl;
      WORD*	data;
      int	bankLength, bankType;
      void *ptr;
      int check;
      std::vector<std::string> requestedBanks;
      std::string bankName;

      mTimeStamp = pEvent->GetTimeStamp();

      std::vector<TigTree*>::iterator trees;
      for ( trees=mTrees.begin(); trees< mTrees.end(); trees++) (*trees)->Banks(requestedBanks);

      for (int i = 0; i < requestedBanks.size(); i++)
	{
	  bankName = requestedBanks.at(i);
	  bankLength = pEvent->LocateBank(NULL,bankName.c_str(),&ptr);
	  data = (WORD*) ptr;
	  std::vector<int> values = mMCSUnpack->ProcessData(data, bankLength); 
	  this->ProcessScalerData(bankName,values);
	  //      std::cout << "[TigManager::ProcessScaler] after unpacking: index = " << index << " - bankLength = " << bankLength << std::endl;
	}
      bool fillTrees = false;
      for ( trees=mTrees.begin(); trees< mTrees.end(); trees++){
	if ( (*trees)->FillTree()) fillTrees = true;
      }
      if (fillTrees) mAnaEventID++;
    }
}

//---- ProcessScaler
void
TigManager::ProcessScalerData(std::string pBankName, std::vector<int> pValues)
{
  std::vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++){
    (*trees)->ProcessScaler(pBankName,pValues);
  }
}

//---- ProcessSignal
void						
TigManager::ProcessSignal(TigEvent* pEvent)
{
  std::vector<TigTree*>::iterator trees;

  for (trees= mTrees.begin(); trees < mTrees.end(); trees++)
    (*trees)->ProcessSignal(pEvent);
}

//---- ProcessTig64
void
TigManager::ProcessTig64(TMidasEvent* pEvent, std::string pBankName)
{
  if (mUnpack)
    {
      //  std::cout << "Processing Tig64" << std::endl;
      WORD*	data;
      // UInt_t	bankLength;
      int	bankLength, bankType;
      long	index = 0;
      void *ptr;
      int check;

      mTimeStamp = pEvent->GetTimeStamp();
      bankLength = pEvent->LocateBank(NULL,pBankName.c_str(),&ptr);
      data = (WORD*) ptr;

      while (index < bankLength)
	{
	  check = mUnpack->ProcessData(data+index, bankLength-index); 
	  if (check < 0) break;
	  else index += check;
	  //      std::cout << "[TigManager::ProcessTig64] after unpacking: index = " << index << " - bankLength = " << bankLength << std::endl;
	}
    }
}




