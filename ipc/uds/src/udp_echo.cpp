#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>

#define BUFF_SIZE 1024
#define SOCK_LOCALFILE "/tmp/process_a"

int main (void)
{
   int sock;
   socklen_t addr_size;
   struct sockaddr_un local_addr;
   struct sockaddr_un guest_addr;
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

   while (true)
   {
      addr_size = sizeof(guest_addr);
      recvfrom(sock, buff_rcv, BUFF_SIZE, 0,
                     ( struct sockaddr*)&guest_addr, &addr_size);
      sendto(sock, buff_rcv, strlen(buff_rcv)+1, 0,
                     (struct sockaddr*)&guest_addr, sizeof(guest_addr));// +1: NULL까지 포함해서 전송
   }
}
