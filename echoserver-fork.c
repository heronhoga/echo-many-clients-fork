#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BACKLOG 10
#define BUFFER_SIZE 1024

int main(int argc, char **argv){
  if(argc != 2){
    printf("Usage: %s <port>\n", argv[0]);
    exit(0);
  }

  int port = atoi(argv[1]);
  printf("Port: %d\n", port);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Socket creation failed");
    exit(1);
  }

  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
    perror("Bind failed");
    close(sockfd);
    exit(1);
  }
  printf("[+]Bind\n");

  if (listen(sockfd, BACKLOG) < 0) {
    perror("Listen failed");
    close(sockfd);
    exit(1);
  }
  printf("[+]Listening for clients\n");

  int client_count = 0;

  while(1){
    int client_socket = accept(sockfd, NULL, NULL);
    if (client_socket < 0) {
      perror("Accept failed");
      continue;
    }

    client_count++;
    printf("[+] Client %d connected\n", client_count);

    if (fork() == 0) {
      close(sockfd);
      char buffer[BUFFER_SIZE];
      char client_id[50];
      snprintf(client_id, sizeof(client_id), "Client %d", client_count);

      while (1) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
          break; 
        }

        buffer[bytes_received] = '\0';
        printf("%s: Received from client: %s\n", client_id, buffer);
        send(client_socket, buffer, bytes_received, 0);
      }

      close(client_socket);
      exit(0);
    }

    close(client_socket); 
    waitpid(-1, NULL, WNOHANG);
  }

  close(sockfd);
  return 0;
}
