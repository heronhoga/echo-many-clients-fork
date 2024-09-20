#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv){
  if(argc != 3){
    printf("Usage: %s <server_ip> <port>\n", argv[0]);
    exit(0);
  }

  int port = atoi(argv[2]);
  printf("Port: %d\n", port);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1) {
    perror("Socket creation failed");
    exit(1);
  }

  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
  serverAddress.sin_port = htons(port);

  if(connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
    perror("Connection failed");
    close(sockfd);
    exit(1);
  }

  printf("[+]Connected to the server\n");

  char message[BUFFER_SIZE];
  char response[BUFFER_SIZE];

  while (1) {
    printf("Enter message to send to server (type 'exit' to quit): ");
    fgets(message, BUFFER_SIZE, stdin);
    message[strcspn(message, "\n")] = 0;

    if (strcmp(message, "exit") == 0) {
      break;
    }

    send(sockfd, message, strlen(message), 0);

    int bytes_received = recv(sockfd, response, BUFFER_SIZE, 0);
    if(bytes_received > 0) {
      response[bytes_received] = '\0';
      printf("Echo from server: %s\n", response);
    } else {
      printf("Failed to receive response from server.\n");
    }
  }

  close(sockfd);
  return 0;
}
