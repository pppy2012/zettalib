#include "zthread.h"
#include "stdio.h"
#include "unistd.h"

class TestThread : public kunlun::ZThread{
public:
 int run(){
   while(m_state){
     printf("thread running\n");
     sleep(1);
   }
   return 0;
 }
};


int main(){
  TestThread t;
  t.start();
  while(1){
    sleep(1);
  }
  return 0;
}
