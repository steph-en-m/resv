//main function for server begins
#include "test.h"
//static MYSQL con;

int main(void){
 unsigned long thid;

 clearScreen(100);//initializing my string
 printf("\nWelcome...\n");
 printf("\nStarting server...\n");
 //call code that creates the server
 myServer::SetStatus(1);
 Idle::SetIdle(0);
// if(Init(&con)){
   //call ordering to cleanup pending jobs since last exit
   CreateThread(NULL,4096,orderProc,NULL,0,&thid);
   server();
// }
  printf("\n\aBye!\n");
 getch();
 return 0;
}

/*MYSQL *getOpenDb(void){
 return &con;
 }*/
