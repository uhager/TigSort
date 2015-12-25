#ifndef TIGSCALER_H
#define TIGSCALER_H

#include <vector>
#include <cstring>

using namespace std;

class TigScaler
{
 public:
  TigScaler();
  ~TigScaler();
  void AddRequest(string pName, int pChannel);
  string Bank(){return mBank;}
  void Initialize(void);
  string Name(int pIndex);
  vector<string> Names(void) { return mNames; }
  bool ProcessEvent(vector<int> pData);
  void Reset();
  void SetBank(string pBank){mBank = pBank;}
  //  void SetEventID(int pEventID){mEventID = pEventID;}
  int Size(){return mRequested.size();}
  int* mEventData;

 protected:			//----- protected ------------------
  string mBank;
  vector<int>	mRequested; //  channels requested
  vector<string> mNames; // names for requested channels (branch names in tree)

 private:			//----- private --------------------

};

#endif /* TIGSCALER_H */
