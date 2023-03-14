#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h> 

#define  BUFF_SIZE   1024
#define  SOCK_LOCALFILE   "/tmp/process_b"
#define  SOCK_TARGETFILE  "/tmp/process_a"

int main (int argc, char **argv)
{
   int sock;
   struct sockaddr_un local_addr;
   struct sockaddr_un target_addr;
   char buff_rcv[BUFF_SIZE+5];


   if (0 == access(SOCK_LOCALFILE, F_OK))
      unlink(SOCK_LOCALFILE);

   sock = socket(PF_FILE, SOCK_DGRAM, 0);
   
   if (-1 == sock)
   {
      printf("socket 생성 실패\n");
      exit(1);
   }

   memset(&local_addr, 0, sizeof(local_addr));
   local_addr.sun_family = AF_UNIX;
   strcpy(local_addr.sun_path, SOCK_LOCALFILE);

   if (-1 == bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)))
   {
      printf("bind() 실행 에러\n");
      exit(1);
   }

   memset(&target_addr, 0, sizeof(target_addr));
   target_addr.sun_family = AF_UNIX;
   strcpy(target_addr.sun_path, SOCK_TARGETFILE);

   sendto(sock, argv[1], strlen(argv[1])+1, 0, 
            (struct sockaddr*)&target_addr, sizeof(target_addr));          // +1: NULL까지 포함해서 전송

   recvfrom(sock, buff_rcv, BUFF_SIZE, 0 , NULL, 0);

   printf("%s\n", buff_rcv);
   close(sock);
   
   return 0;
}
