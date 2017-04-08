#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <Arduino.h>

struct state
{
  float angle[3];
  bool launched;
  float dt;
};
#endif
