// part of TigSort
// author: Ulrike Hager

#ifndef TIGTIMER_H
#define TIGTIMER_H

#include <sys/time.h>
#include <assert.h>
#include <TTimer.h>

class TigTimer : public TTimer
{
public:
  typedef void (*TimerHandler)(void);

  TigTimer(int period_msec,TimerHandler handler);
  ~TigTimer(){TurnOff();}

  int          fPeriod_msec;
  TimerHandler fHandler;
  double       fLastTime;

  double GetTimeSec();
  Bool_t Notify();

};

#endif /* TIGTIMER_H */
