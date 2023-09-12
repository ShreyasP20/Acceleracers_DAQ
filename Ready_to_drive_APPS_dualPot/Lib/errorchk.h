#include <stdlib.h>
float dev;
float diff;
float calcError(int i,float x, float y)
{
  if(x>y){diff =x-y;}
  else{diff =y-x;}
  
  float error = (diff / x)*100;
  if(error <=20){
    dev = map(error, 0, 20, 0, 9);
    }else{
      dev = map(error, 21, 100, 10, 100);
      }
      return dev;
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}