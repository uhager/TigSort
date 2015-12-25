// based on
// Version of tig-format edited by GH

#include <iostream>
#include <vector>
#include <strings.h>
#include <stdio.h>
#include "TigUnpack.h"
#include <TigManager.h>

using namespace std;

//----- TigUnpack
TigUnpack::TigUnpack(void)
{
}

//---- ~TigUnpack
TigUnpack::~TigUnpack(void)
{
}

//---- ProcessData
int
TigUnpack::ProcessData(WORD* pData, long pMaxLength)
{
bool bail = false;
 long pChannel, pValue;
 int eventID = 0;
 long index;
 uint32_t type, value, trigger_num;
 TigEvent* event = new TigEvent;

 for ( index = 0; index < pMaxLength; index++)
	{
	  type = pData[index] >> 28;
	  value = pData[index] & 0x0fffffff;
	  //	  cout << "data[" << index << "] " << hex<< pData[index] << endl;
      switch( type ){
      case 0x0: 
	pValue = value & 0x3fff;
	event->AddWfSample(pValue);
	pValue = ((value & 0x0fffffff)>>14) & 0x3fff;
	event->AddWfSample(pValue);
	break;
      case 0x1:            break;           /*  trapeze data */
      case 0x4:                             /*      CFD Time */
	pValue = value & 0x00ffffff;
	event->SetCFD(pValue);
	//	 cout << "[TigUnpack::ProcessData] found CFD: eventID " << event->EventID() << " CFD " << event->CFD() << " charge " << event->Charge() << endl;
	break;
      case 0x5:                             /*        Charge */
	pValue = value & 0xfffffff;
	if (event->Charge() == -1) {
	  event->SetCharge(pValue);
	  //	 cout << "[TigUnpack::ProcessData] found charge: eventID " << event->EventID() << " CFD " << event->CFD() << " charge " << event->Charge() << endl;
	}else {
	 cout << "[TigUnpack::ProcessData] found event that already has charge! eventID " << event->EventID() << " address " << event->Address() << " charge " << event->Charge() << endl;
	  TigManager::Instance().ProcessSignal(event);
	  delete event;
	   event = new TigEvent;
	   event->SetEventID(eventID);
	   event->SetCharge(pValue);
	}
         break;
      case 0x8:                        /*  Event header */
	pValue = value & 0x00ffffff;
	event->SetEventID(pValue);
	eventID = pValue;
	break;
      case 0xa:
	uint32_t subtype;
	subtype = (pData[index] & 0x0f000000) >> 24;
	pValue = value & 0x00ffffff;
	if (subtype==0) event->SetTimestamp(pValue);
	else if (subtype==1) event->SetTimestampUp(pValue);
	else if (subtype==2) event->SetLifetime(pValue);
	else if (subtype==4) event->SetTriggersRequested(pValue);
	else if (subtype==5) event->SetTriggersAccepted(pValue);
         break;
      case 0xb: break;                      /*  Trigger Pattern*/
      case 0xc:                            /*   New Channel */
	if (event->Address() == -1) event->SetAddress(value);
	else {
	  TigManager::Instance().ProcessSignal(event);
	  delete event;
	  event = new TigEvent;
	  event->SetEventID(eventID);
	  event->SetAddress(value);
	}
 	 break;
      case 0xe: break;                                    /* Event Trailer */
      case 0xf:                                     /* EventBuilder Timeout*/
                fprintf(stderr,"Reconstruction error 3\n"); return(-1);
      default:  fprintf(stderr,"Reconstruction error 4\n"); return(-1);
      }
   }
 // cout << "[TigUnpack::ProcessData] processing event, channel " <<  pChannel << dec << endl;
	  TigManager::Instance().ProcessSignal(event);
   delete event;
	return index;
}

/************ TigMCSUnpack ****************/

//----- TigUnpack
TigMCSUnpack::TigMCSUnpack(void)
{
}

//---- ~TigMCSUnpack
TigMCSUnpack::~TigMCSUnpack(void)
{
}

//---- ProcessData
vector<int>
TigMCSUnpack::ProcessData(WORD* pData, long pMaxLength)
{
 long pValue;
 long index;
 vector<int> values;

 for ( index = 0; index < pMaxLength; index++)
   {
     pValue = pData[index] & 0xffffffff;
     values.push_back(pValue);
   }
 // TigManager::Instance().ProcessScaler(values);
 return values;
}	  
	  

