#include "test.h"

void server(void){
 SOCKET lfd,cfd;unsigned long id;
 int error,type,priority;DST dst;
 char req[1024]={"\0"};MYSQL con;
 WSADATA wsa;
 struct sockaddr_in server,client;//define the structure
 char buf[1024]={"\0"},cmd[32]={"\0"};
 int len;unsigned long thid;

 if(WSAStartup(MAKEWORD(2,0),&wsa)!=0){printf("\a\a\nError: Winsock could not be started at the moment...\n");return;}
 printf("\aWinsock initialized...OK\nNow creating socket...\n");
 lfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 if(lfd==INVALID_SOCKET)goto end;
  // now zero this memory
  ZeroMemory(&server,sizeof(server));
//define structure components
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);

  //now bind to locate the descripter
  if(bind(lfd,(struct sockaddr *)&server,sizeof(server))==SOCKET_ERROR)goto end;
//now listen to the incoming responses from clients queued
  if(listen(lfd,10)==SOCKET_ERROR)goto end;
  printf("\nSuccess! socket created and listening...\n");
  while(myServer::GetStartup()){
    len = sizeof(client);
    cfd = accept(lfd,(struct sockaddr *)&client,&len);
    if(cfd==INVALID_SOCKET){printf("Invalid socket encountered: %d\a\a\a\n",WSAGetLastError());}
    else{
    //do the business logik
    printf("\nNow serving client...\n");
    //i expect 1000, in my rcv buffer
    memset(buf,NULL,sizeof(buf));
    recv(cfd,buf,sizeof(buf),0);
    shutdown(cfd,SD_RECEIVE);
    printf("\nReceived: %s...\n",buf);
    //determine command set from client
    memset(cmd,NULL,sizeof(cmd));
    sscanf(buf,"%s",cmd);
    //now compare command functions and call respective functions
    error=0;
    if(!strcmp(cmd,"reply")){reply_(buf,cfd);error=1;}
    else if(!strcmp(cmd,"group")){group_(buf,cfd);error=1;}
    else{
    if(!strcmp(cmd,"double"))type =  5;
    else if(!strcmp(cmd,"reverse"))type = 4;
    else if(!strcmp(cmd,"replace"))type = 0;
    else if(!strcmp(cmd,"delete"))type = 1;
    else if(!strcmp(cmd,"encrypt"))type = 2;
    else if(!strcmp(cmd,"decrypt"))type = 3;
    else {printf("\nserver: got unknown command: %s\n",cmd);send(cfd,"unknown Command",(int)strlen("unknown Command"),0);shutdown(cfd,SD_SEND);error=1;}
    }
    if(!error){
     priority = type - 3;
     if(priority<=0){
      priority = -1*priority;                    
      if(priority==0)priority = 1;
      else if(priority>=4)priority = priority - 3;
      }
    //now do the submission to database
    ZeroMemory(&dst,sizeof(dst));
    getBody((int)strlen(cmd),buf,&dst);
    do{}while(Db::IsCaught());
    Db::Catch();
    if(Init(&con)==0){send(cfd,"",(int)strlen(""),0);shutdown(cfd,SD_SEND);Db::Release();}
    else{
    if(mysql_select_db(&con,"wordserver")!=0){send(cfd,"Internal server failure",(int)strlen("Internal server failure"),0);shutdown(cfd,SD_SEND);mysql_close(&con);Db::Release();}
    else{
    sprintf(req,"insert into pending set timepost = %u,typeOfTask=%d,priority=%d,status=0,body=\'%s\'\0",
    (unsigned)time(NULL),type,priority,dst.out);
    
     if(mysql_real_query(&con,req,(int)strlen(req))!=0){send(cfd,mysql_error(&con),(int)strlen(mysql_error(&con)),0);shutdown(cfd,SD_SEND);mysql_close(&con);Db::Release();}
     else{
       id = (unsigned long)mysql_insert_id(&con);
       if(id==0){send(cfd,"could not schedule your job",(int)strlen("could not schedule your job"),0);shutdown(cfd,SD_SEND);mysql_close(&con);Db::Release();}
       else{
        sprintf(req,"use: reply %lu to get progress info",id);
        send(cfd,req,(int)strlen(req),0);shutdown(cfd,SD_SEND);
        mysql_close(&con);Db::Release();
        if(Idle::Close()==FALSE)
        CreateThread(NULL,4096,orderProc,NULL,0,&thid);
       }
     }
    }
    }
    }
    closesocket(cfd);
   }
  }
  closesocket(lfd);
  WSACleanup();

  return;
 end:
 WSACleanup();
 printf("\a\a\nError: Server failure...\n");
 return;
}

BOOL myServer::status = FALSE;

void myServer::SetStatus(BOOL e){status = e;}

BOOL myServer::GetStartup(void){return status;}

/*
if(!strcmp(cmd,"double"))double_((int)strlen(cmd),buf,cfd);
    else if(!strcmp(cmd,"reverse"))reverse_((int)strlen(cmd),buf,cfd);
    else if(!strcmp(cmd,"replace"))replace_((int)strlen(cmd),buf,cfd);
    else if(!strcmp(cmd,"delete"))delete_((int)strlen(cmd),buf,cfd);
    else if(!strcmp(cmd,"encrypt"))encrypt_((int)strlen(cmd),buf,cfd);
    else if(!strcmp(cmd,"decrypt"))decrypt_((int)strlen(cmd),buf,cfd);
    //else if(!strcmp(cmd,"reply"))reply_((int)strlen(cmd),buf,cfd);
//handle admin commands in other if statements
    else {printf("\nserver: got unknown command: %s\n",cmd);send(cfd,"unknown Command",(int)strlen("unknown Command"),0);shutdown(cfd,SD_SEND);}
*/