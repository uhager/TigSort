#ifndef TIGSCALER_H
#define TIGSCALER_H

#include <vector>
#include <cstring>



class TigScaler
{
 public:
  TigScaler();
  ~TigScaler();
  void AddRequest(std::string pName, int pChannel);
  std::string Bank(){return mBank;}
  void Initialize(void);
  std::string Name(int pIndex);
  std::vector<std::string> Names(void) { return mNames; }
  bool ProcessEvent(std::vector<int> pData);
  void Reset();
  void SetBank(std::string pBank){mBank = pBank;}
  //  void SetEventID(int pEventID){mEventID = pEventID;}
  int Size(){return mRequested.size();}
  int* mEventData;

 protected:			//----- protected ------------------
  std::string mBank;
  std::vector<int>	mRequested; //  channels requested
  std::vector<std::string> mNames; // names for requested channels (branch names in tree)

 private:			//----- private --------------------

};

#endif /* TIGSCALER_H */
