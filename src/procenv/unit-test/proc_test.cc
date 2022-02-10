#include "unistd.h"
#include "stdlib.h"
#include "proc_env.h"
int main(){
  kunlun::procDaemonize();
  kunlun::procInvokeKeepalive();
  while(1){
    sleep(1);
  }

  return 0;
}
