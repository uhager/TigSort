// part of TigSort
// author: Ulrike Hager

#ifndef TIGUNPACK_H
#define TIGUNPACK_H

#include <stdint.h>

#include "TigEvent.h"

#ifndef WORD
#define WORD uint32_t
#endif

class TigUnpack
{
 public:
  TigUnpack(void);
  virtual ~TigUnpack(void); 
   int ProcessData(WORD* pData, long pMaxLength);
protected:			//----- protected ------------------
 
private:			//----- private --------------------
 TigUnpack(const TigUnpack& pToCopy);
 TigUnpack&	operator = (const TigUnpack& pToCopy);

};


class TigMCSUnpack
{
 public:
  TigMCSUnpack(void);
  virtual ~TigMCSUnpack(void); 
   std::vector<int> ProcessData(WORD* pData, long pMaxLength);
protected:			//----- protected ------------------
 
private:			//----- private --------------------
 TigMCSUnpack(const TigMCSUnpack& pToCopy);
 TigMCSUnpack&	operator = (const TigMCSUnpack& pToCopy);

};


#endif /* TIGUNPACK_H*/
