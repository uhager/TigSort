// TigSort
// MIDAS data sorting for SHARC / TIGRESS 
// author: Ulrike Hager
// based on ROOT analyzer by K.Olchanski
// using some komodo routines by L. Erikson

#include <sys/time.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <signal.h>

#include <TMidasOnline.h>
#include <TMidasEvent.h>
#include <TMidasFile.h>
#include <XmlOdb.h>
#ifdef OLD_SERVER
#include <midasServer.h>
#endif
#ifdef HAVE_LIBNETDIRECTORY
#include <libNetDirectory/netDirectoryServer.h>
#endif

#include <TSystem.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TTimer.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TFolder.h>

//#include "TigGlobals.h"
#include "TigTimer.h"
#include "TigManager.h"

// data sizes

// Global Variables
int  gRunNumber = 0;
bool gIsRunning = false;
bool gIsPedestalsRun = false;
bool gIsOffline = false;
int  gEventCutoff = 0;

std::string configFile = "-";
TDirectory* gOnlineHistDir = NULL;
TFile* gOutputFile = NULL;
VirtualOdb* gOdb = NULL;


TigManager gCatalystMgr;
#define M_FILE_EXIT 0


void startRun(int transition,int run,int time)
{

  std::cout << "startRun" << std::endl;
  gIsRunning = true;
  gRunNumber = run;
  //gIsPedestalsRun = gOdb->odbReadBool("/experiment/edit on start/Pedestals run");
  printf("Begin run: %d\n", gRunNumber);
  //  std::cout << "run number from odb: " << gOdb->odbReadInt("/Runinfo/Run number") << std::endl;
    
  if(gOutputFile!=NULL)
  {
    gOutputFile->Write();
    gOutputFile->Close();
    gOutputFile=NULL;
  }  

  char filename[1024];
  //  sprintf(filename, "asm%05d.root", run);
  sprintf(filename, "asm%05d.root", run);
  gOutputFile = new TFile(filename,"RECREATE");

#ifdef HAVE_LIBNETDIRECTORY
  NetDirectoryExport(gOutputFile, "outputFile");
#endif

   gOutputFile->cd();  
   //   std::cout << "starting to parse input" << std::endl;
   TigManager::Instance().ParseInputFile(configFile);
   //  std::cout << "parsed input" << std::endl;
  }

void endRun(int transition,int run,int time)
{
  // std::cout << "endRun" << std::endl;
  gIsRunning = false;
  gRunNumber = run;

   TigManager::Instance().FlushTreeBuffers();

#ifdef OLD_SERVER
  if (gManaHistosFolder)
    gManaHistosFolder->Clear();
#endif

   if (gOutputFile)
    {
      gOutputFile->Write();
      gOutputFile->Close();		//close the histogram file
      gOutputFile = NULL;
      
      if (!gIsOffline)        // write runlog
		{
		  time_t startT = gOdb->odbReadUint32("/Runinfo/Start time binary");
		  time_t stopT = gOdb->odbReadUint32("/Runinfo/Stop time binary");
	  
		  std::string time1(ctime(&startT));
		  std::string time2(ctime(&stopT));
		  int duration = difftime(stopT,startT);

		  std::string comment = gOdb->odbReadString("Experiment/Run Parameters/Comment"); 
      
		  std::cout << time1.substr(0, 10).c_str() << time1.substr(19, 5).c_str()
		       << std::setw(8)           << gRunNumber
		       << std::setw(11)          << time1.substr(11, 8).c_str()
		       << std::setw(11)          << time2.substr(11, 8).c_str()
		       << std::setw(7)           << duration << " s  "
		       << comment << std::endl;

		  std::string runlogFile = gOdb->odbReadString("Logger/Data dir");
		  runlogFile +=  "/runlog.txt";
		  std::cout << "runlog file: " << runlogFile << std::endl;
		  ofstream runlog;
		  runlog.open(runlogFile.c_str(), ofstream::out | ofstream::app);
		  runlog << time1.substr(0, 10).c_str() << time1.substr(19, 5).c_str()
			 << std::setw(8)           << gRunNumber
			 << std::setw(11)          << time1.substr(11, 8).c_str()
			 << std::setw(11)          << time2.substr(11, 8).c_str()
			 << std::setw(7)           << duration << " s  "
			 << comment << std::endl;
		  runlog.close();

		}
    }

  printf("End of run %d\n",run);
}

void pauseRun(int transition,int run,int time)
{
}

void resumeRun(int transition,int run,int time)
{
}
void HandleMidasEvent(TMidasEvent& pEvent)
{
  int eventId = pEvent.GetEventId();
  if ( eventId == 1)   //Tig64 data
    {
      TigManager::Instance().ProcessTig64(&pEvent,"WFDN");
    }
  else if ( eventId == 3)   //scaler data
    {
      TigManager::Instance().ProcessScaler(&pEvent);
    }
  else
    {
      // unknown event type
      //  std::cout << "unknown event" << std::endl;
	   //      pEvent.Print();
    }
}

void eventHandler(const void*pheader,const void*pdata,int size)
{
  TMidasEvent event;
  memcpy(event.GetEventHeader(), pheader, sizeof(TMidas_EVENT_HEADER));
  event.SetData(size, (char*)pdata);
  event.SetBankList();
  HandleMidasEvent(event);
}

int ProcessMidasFile(TApplication*app,const char*fname)
{
  TMidasFile f;
  bool tryOpen = f.Open(fname);

  if (!tryOpen)
    {
      printf("Cannot open input file \"%s\"\n",fname);
      return -1;
    }

  // std::string name = fname;
  // name = name.substr(name.find_last_of("/\\")+1);
  // int firstNum = name.find_first_of("0123456789");
  // std::string number = name.substr(firstNum, name.find(".")-firstNum);
  // std::istringstream(number) >> firstNum;
  // 	  startRun(0,firstNum,0);

	  int i=0;
  while (1)
    {
      TMidasEvent event;
      if (!f.Read(&event))
	break;

      int eventId = event.GetEventId();
      //    printf("Have an event of type %d\n",eventId);

         if ((eventId & 0xFFFF) == 0x8000)
     	{
	  // begin run
	  //	  event.Print();

	  //char buf[256];
	  //memset(buf,0,sizeof(buf));
	  //memcpy(buf,event.GetData(),255);
	  //printf("buf is [%s]\n",buf);

	  //
	  // Load ODB contents from the ODB XML file
	  //
	   if (gOdb)
	    delete gOdb;
	   gOdb = new XmlOdb(event.GetData(),event.GetDataSize());
      	  startRun(0,event.GetSerialNumber(),0);
	  std::cout << "Running...";
	  std::cout.flush();

		}
	   else if ((eventId & 0xFFFF) == 0x8001)
	  // if ((eventId & 0xFFFF) == 0x8001)
	{
	  // end run
	  //	  	  event.Print();
	}
      else
	{
	  event.SetBankList();
	  //event.Print();
	  HandleMidasEvent(event);
	}
	
      if((i%5000)==0)
	{
	  //resetClock2time();
	  //	  printf("Processing event %d\n",i);
	  std::cout << "." ;
	  std::cout.flush();
	}
      
      i++;
      if ((gEventCutoff!=0)&&(i>=gEventCutoff))
	{
	  printf("Reached event %d, exiting loop.\n",i);
	  break;
	}
    }

  
  f.Close();

  endRun(0,gRunNumber,0);

  // start the ROOT GUI event loop
  //  app->Run(kTRUE);
  if (gOdb)
    delete gOdb;
  
  return 0;
}

#ifdef HAVE_MIDAS

void MidasPollHandler()
{
  if (!(TMidasOnline::instance()->poll(0)))
    gSystem->ExitLoop();
}

int ProcessMidasOnline(TApplication*app, const char* hostname, const char* exptname)
{
   TMidasOnline *midas = TMidasOnline::instance();

   int err = midas->connect(hostname, exptname, "komodo");
   if (err != 0)
     {
       fprintf(stderr,"Cannot connect to MIDAS, error %d\n", err);
       return -1;
     }

   gOdb = midas;

   midas->setTransitionHandlers(startRun,endRun,pauseRun,resumeRun);
   midas->registerTransitions();

   /* reqister event requests */

   midas->setEventHandler(eventHandler);
   midas->eventRequest("SYSTEM",-1,-1,(1<<1));

   /* fill present run parameters */

   gRunNumber = gOdb->odbReadInt("/runinfo/Run number");

   if ((gOdb->odbReadInt("/runinfo/State") == 3))
     startRun(0,gRunNumber,0);

   printf("Startup: run %d, is running: %d\n",gRunNumber,gIsRunning);
   
   TigTimer tm(100,MidasPollHandler);

   /*---- start main loop ----*/

   //loop_online();
   app->Run(kTRUE);

   /* disconnect from experiment */
   midas->disconnect();

   return 0;
}

#endif

static bool gEnableShowMem = false;

int ShowMem(const char* label)
{
  if (!gEnableShowMem)
    return 0;

  FILE* fp = fopen("/proc/self/statm","r");
  if (!fp)
    return 0;

  int mem = 0;
  fscanf(fp,"%d",&mem);
  fclose(fp);

  if (label)
    printf("memory at %s is %d\n", label, mem);

  return mem;
}


void help()
{
  printf("\nUsage:\n");
  printf("\t-h: print this help message\n");
  printf("\t-c: config file (mapping)\n");
  printf("\t-P: Start the TNetDirectory server on specified tcp port (for use with roody -Plocalhost:9091)\n");
  printf("\t-p: Start the old midas histogram server on specified tcp port (for use with roody -Hlocalhost:9090)\n");
  printf("\t-e: Number of events to read from input data files\n");
  printf("\n");
  exit(1);
}

// Main function call

int main(int argc, char *argv[])
{
   setbuf(stdout,NULL);
   setbuf(stderr,NULL);
 
   signal(SIGILL,  SIG_DFL);
   signal(SIGBUS,  SIG_DFL);
   signal(SIGSEGV, SIG_DFL);
 
   std::vector<std::string> args;
   for (int i=0; i<argc; i++)
     {
       if (strcmp(argv[i],"-h")==0)
	 help(); // does not return
       args.push_back(argv[i]);
     }

   TApplication *app = new TApplication("TigSort", &argc, argv);

   if(gROOT->IsBatch()) {
   	printf("Cannot run in batch mode\n");
	return 1;
   }

   bool forceEnableGraphics = false;
   bool testMode = false;
   int  oldTcpPort = 0;
   int  tcpPort = 0;
   const char* hostname = NULL;
   const char* exptname = NULL;

   for (unsigned int i=1; i<args.size(); i++) // loop over the commandline options
     {
       const char* arg = args[i].c_str();
       //printf("argv[%d] is %s\n",i,arg);
	   
       if (strncmp(arg,"-e",2)==0)  // Event cutoff flag (only applicable in offline mode)
	 gEventCutoff = atoi(arg+2);
       else if (strncmp(arg,"-m",2)==0) // Enable memory debugging
	 gEnableShowMem = true;
       else if (strncmp(arg,"-p",2)==0) // Set the histogram server port
	 oldTcpPort = atoi(arg+2);
       else if (strncmp(arg,"-P",2)==0) // Set the histogram server port
	 tcpPort = atoi(arg+2);
       else if (strcmp(arg,"-T")==0)
	 testMode = true;
       else if (strcmp(arg,"-g")==0)
	 forceEnableGraphics = true;
       else if (strncmp(arg,"-H",2)==0)
	 hostname = strdup(arg+2);
       else if (strncmp(arg,"-E",2)==0)
	 exptname = strdup(arg+2);
       else if (strncmp(arg,"-c",2)==0)
	 configFile = strdup(arg+2);
       else if (strcmp(arg,"-h")==0)
	 help(); // does not return
       else if (arg[0] == '-')
	 help(); // does not return
    }
    
   // TigWindow mainWindow(gClient->GetRoot(), 200, 300);

   gROOT->cd();
    gOnlineHistDir = new TDirectory("histos", "TigSort online plots");
    //   app->Run(kTRUE);

    //  TigManager::Instance().ParseInputFile();

#ifdef OLD_SERVER
   if (oldTcpPort)
     //  VerboseMidasServer(true);
     StartMidasServer(oldTcpPort);
#else
   if (oldTcpPort)
     fprintf(stderr,"ERROR: No support for the old midas server!\n");
#endif
#ifdef HAVE_LIBNETDIRECTORY
   if (tcpPort)
     //VerboseNetDirectoryServer(true);
    StartNetDirectoryServer(tcpPort, gOnlineHistDir);
#else
   if (tcpPort)
     fprintf(stderr,"ERROR: No support for the TNetDirectory server!\n");
#endif


   //   TBrowser *browser = new TBrowser();
   gIsOffline = false;

   for (unsigned int i=1; i<args.size(); i++)
     {
       const char* arg = args[i].c_str();

       if (arg[0] != '-')  
	 {  
	   gIsOffline = true;
	   //gEnableGraphics = false;
	   //gEnableGraphics |= forceEnableGraphics;
	   ProcessMidasFile(app,arg);
	 }
     }

   if (testMode)
     {
       gOnlineHistDir->cd();
       app->Run(kTRUE);
       return 0;
     }

   // if we processed some data files,
   // do not go into online mode.
   if (gIsOffline)
     return 0;
	   
   gIsOffline = false;
   //gEnableGraphics = true;
#ifdef HAVE_MIDAS
   ProcessMidasOnline(app, hostname, exptname);
#endif
   
   return 0;
}

//end



