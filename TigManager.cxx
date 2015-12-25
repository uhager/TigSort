// part of TigSort
// author: Ulrike Hager

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <fstream>
#include <strstream>
#include <TROOT.h>
#include <TFolder.h>

#include <TigManager.h>
#include <TigTree.h>


using namespace std;

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
      cout << "[TigManager::TigManager]  Manager is a singleton class and can only be constructed once." << endl;
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
  vector<TigTree*>::iterator trees;
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
      cout << "[TigManager::Instance]  Must construct a manager before calling Instance." << endl;
      exit(1);
    }
	
  return *gInstance;
}

//---- FlushTreeBuffers
void
TigManager::FlushTreeBuffers()
{
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++) (*trees)->FlushBuffer();
}

//---- ParseInputFile
void
TigManager::ParseInputFile(string configFile)
{
  //  char *inputFile = NULL;
  string inputFile = "-"; 
  if (configFile.compare("-") != 0) inputFile = configFile;
  else inputFile = getenv("TIGSORT_INPUT");
  if (inputFile.compare("-") == 0)
    {
      cout << "[KoManager::ParseInputFile]  No 'TIGSORT_INPUT' input file specified ";
      exit(1);
    }

  string line, token;
  ifstream input(inputFile.c_str());
  this->Clear();
  cout << "Parsing TigSort specification file...";

  getline(input,line);
  while ( input)
    {
      istrstream stream(line.c_str());
     
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
  cout << "TigManager input parsing done.\n";
}

//---- ParseDetector
TigDetector*
TigManager::ParseDetector(istream& pStream)
{
  string line, token;
  bool	 bail = false;
  TigDetector*	 det = new TigDetector;

  getline(pStream,line);
  while ( pStream && !bail)
    {
      istrstream stream(line.c_str());
	
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
	  vector<int> parameters;
	  while ( !stream.eof() ) {
	    parameters.push_back(0);
	    stream >> parameters.back();
	  }
	  det->SetDataType(token,parameters);
	}
      else if ( token.compare("name") == 0)
	{	
	  stream >> token;
	  cout << ".(" << token << ").";
	  det->ChangeName(token);
	}
      else if ( token.compare("signals") == 0)
	{
	  bool bailSignals = false;
		
	  getline(pStream,line);
	  while ( pStream && !bailSignals)
	    {
	      istrstream subStream(line.c_str());
			
	      //	      cout << "signals: " << line << endl;
	      subStream >> token;
	      if ( token == "" || token[0] == '#') {}  //comment or blank
	      else if ( token.compare("end") == 0)
		bailSignals = true;
	      else if ( token.compare("range") == 0)
		{
		  int minCh, maxCh, minAdd, maxAdd;
		  subStream >> minCh >> maxCh >> hex >> minAdd >> hex >> maxAdd;
		  //  cout << "[TigManager::ParseDetector] range " << minCh << "\t" << maxCh  << "\t" << minAdd  << "\t" << maxAdd << endl;
		  det->AddSignals(minCh, maxCh, minAdd, maxAdd);
		}
	      else
		{
		  int channel;
		  istringstream ( token ) >> channel;
		  int address;				
		  subStream >> hex >> address;
		  det->AddSignal(channel,address);
		  //  cout << "added signal: " << channel << " - " << address << endl;
		}
	      if ( !bailSignals)
		getline(pStream,line);
	    }
	}
      if ( !bail)
	getline(pStream,line);
    }
  det->Initialize();
  //  cout << "new detector: " << det->Name() << " #signals: " << det->Size() << endl;
  return det;
}

//---- ParsePrimitive
void
TigManager::ParsePrimitive(string pToken, istream& pStream)
{
  if ( pToken.compare("tree") == 0)
    this->ParseTree(pStream);
  else
    {
      cout << "[TigManager::ParsePrimitive]  Unknown primtive token: " << pToken << endl;
    }
}

//---- ParseScaler
TigScaler*
TigManager::ParseScaler(istream& pStream)
{
  string line, token;
  bool	 bail = false;
  TigScaler* scaler = new TigScaler;

  getline(pStream,line);
  while ( pStream && !bail)
    {
      istrstream stream(line.c_str());
	
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
	  cout << ".(" << token << ").";
	  scaler->SetBank(token);
	}
      else if ( token.compare("signals") == 0)
	{
	  bool bailSignals = false;
		
	  getline(pStream,line);
	  while ( pStream && !bailSignals)
	    {
	      istrstream subStream(line.c_str());
			
	      //	      cout << "signals: " << line << endl;
	      subStream >> token;
	      if ( token == "" || token[0] == '#') {}  //comment or blank
	      else if ( token.compare("end") == 0)
		bailSignals = true;
	      else
		{
		  int channel;
		  subStream >> channel;
		  scaler->AddRequest(token, channel);
		  //  cout << "added signal: " << channel << " - " << token << endl;
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
  string line, token;
  bool	 bail = false;
  TigTree*	 tree = new TigTree;

  getline(pStream,line);
  while ( pStream && !bail)
    {
      istrstream stream(line.c_str());
	
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
	  cout << ".(" << token << ").";
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
  string line, token;
  bool	 bail = false;
  TigWaveform*	 det = new TigWaveform;

  getline(pStream,line);
  while ( pStream && !bail)
    {
      istrstream stream(line.c_str());
	
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
	  cout << ".(" << token << ").";
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
	      istrstream subStream(line.c_str());
			
	      //	      cout << "signals: " << line << endl;
	      subStream >> token;
	      if ( token == "" || token[0] == '#') {}  //comment or blank
	      else if ( token.compare("end") == 0)
		bailSignals = true;
	      else if ( token.compare("range") == 0)
		{
		  int minCh, maxCh, minAdd, maxAdd;
		  subStream >> minCh >> maxCh >> hex >> minAdd >> hex >> maxAdd;
		  //  cout << "[TigManager::ParseDetector] range " << minCh << "\t" << maxCh  << "\t" << minAdd  << "\t" << maxAdd << endl;
		  det->AddSignals(minCh, maxCh, minAdd, maxAdd);
		}
	      else
		{
		  int channel;
		  istringstream ( token ) >> channel;
		  int address;				
		  subStream >> hex >> address;
		  det->AddSignal(channel,address);
		  //  cout << "added signal: " << channel << " - " << address << endl;
		}
	      if ( !bailSignals)
		getline(pStream,line);
	    }
	}
      if ( !bail)
	getline(pStream,line);
    }
  det->Initialize();
  //  cout << "new detector: " << det->Name() << " #signals: " << det->Size() << endl;
  return det;
}



//---- ProcessScaler
void
TigManager::ProcessScaler(TMidasEvent* pEvent)
{
  if (mMCSUnpack)
    {
      //  cout << "Processing scaler" << endl;
      WORD*	data;
      int	bankLength, bankType;
      void *ptr;
      int check;
      vector<string> requestedBanks;
      string bankName;

      mTimeStamp = pEvent->GetTimeStamp();

      vector<TigTree*>::iterator trees;
      for ( trees=mTrees.begin(); trees< mTrees.end(); trees++) (*trees)->Banks(requestedBanks);

      for (int i = 0; i < requestedBanks.size(); i++)
	{
	  bankName = requestedBanks.at(i);
	  bankLength = pEvent->LocateBank(NULL,bankName.c_str(),&ptr);
	  data = (WORD*) ptr;
	  vector<int> values = mMCSUnpack->ProcessData(data, bankLength); 
	  this->ProcessScalerData(bankName,values);
	  //      cout << "[TigManager::ProcessScaler] after unpacking: index = " << index << " - bankLength = " << bankLength << endl;
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
TigManager::ProcessScalerData(string pBankName, vector<int> pValues)
{
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++){
    (*trees)->ProcessScaler(pBankName,pValues);
  }
}

//---- ProcessSignal
void						
TigManager::ProcessSignal(TigEvent* pEvent)
{
  vector<TigTree*>::iterator trees;

  for (trees= mTrees.begin(); trees < mTrees.end(); trees++)
    (*trees)->ProcessSignal(pEvent);
}

//---- ProcessTig64
void
TigManager::ProcessTig64(TMidasEvent* pEvent, string pBankName)
{
  if (mUnpack)
    {
      //  cout << "Processing Tig64" << endl;
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
	  //      cout << "[TigManager::ProcessTig64] after unpacking: index = " << index << " - bankLength = " << bankLength << endl;
	}
    }
}




