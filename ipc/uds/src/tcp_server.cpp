#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>

#define  BUFF_SIZE   1024
#define  FILE_SERVER "/tmp/test_server"

int main (void)
{
   int server_socket;
   int client_socket;
   unsigned int client_addr_size;
   int option;

   struct sockaddr_un server_addr;
   struct sockaddr_un client_addr;

   char buff_rcv[BUFF_SIZE+5];
   char buff_snd[BUFF_SIZE+5];

   if (0 == access(FILE_SERVER, F_OK))
      unlink(FILE_SERVER);

   server_socket = socket(PF_FILE, SOCK_STREAM, 0);
   
   if (-1 == server_socket)
   {
      printf("server socket 생성 실패\n");
      exit(1);
   }

   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sun_family = AF_UNIX;
   strcpy(server_addr.sun_path, FILE_SERVER);

   if (-1 == bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
   {
      printf("bind() 실행 에러\n");
      exit(1);
   }

   while (true)
   {
      if (-1 == listen(server_socket, 5))
      {
         printf("대기상태 모드 설정 실패\n");
         exit(1);
      }

      client_addr_size = sizeof(client_addr);
      client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);

      if (-1 == client_socket)
      {
         printf("클라이언트 연결 수락 실패\n");
         exit(1);
      }

      read(client_socket, buff_rcv, BUFF_SIZE);
      printf("receive: %s\n", buff_rcv);
      
      sprintf(buff_snd, "%d : %s", strlen(buff_rcv), buff_rcv);
      write(client_socket, buff_snd, strlen(buff_snd)+1); // +1: NULL까지 포함해서 전송
      close(client_socket);
   }
   return 0;      
}
