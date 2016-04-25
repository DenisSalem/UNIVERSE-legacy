#include <iostream>

unsigned long int getRandom() {
  timespec tStruct;
  clock_gettime(CLOCK_REALTIME, &tStruct);
  return tStruct.tv_nsec;
}
