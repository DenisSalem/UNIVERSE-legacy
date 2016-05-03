#include <iostream>
#include <cstdlib>

unsigned long int getRandom() {
  timespec tStruct;
  clock_gettime(CLOCK_REALTIME, &tStruct);
  return tStruct.tv_nsec;
}
