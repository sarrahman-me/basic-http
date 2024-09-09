#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int PORT = 8080;

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd == -1) {
    perror("Kesalahan saat membuat socket");
    exit(EXIT_FAILURE);
  }

  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    perror("Kesalahan saat mengatur opsi socket");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(8080);

  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Kesalahan saat binding host dan port");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 20) < 0) {
    perror("Kesalahan saat melakukan listing koneksi");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  printf("server running on port %d \n", PORT);

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (client_fd < 0) {
      perror("Gagal menerima koneksi");
      continue; // jika ada error lanjut ke permintaan selanjutnya
    }

    char buffer[1024] = {0};
    read(client_fd, buffer, sizeof(buffer));
    printf("Koneksi di terima %s \n", buffer);

    char method[8], path[1024], version[16];
    sscanf(buffer, "%s %s %s", method, path, version);

    printf("Method : %s, Path: %s, Version: %s \n", method, path, version);

    if (strcmp(path, "/") == 0) {
      char *response = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/html\r\n"
                       "Content-Length: 22\r\n\r\n"
                       "<h1>main page</h1>";

      send(client_fd, response, strlen(response), 0);
    } else if (strcmp(path, "/hello") == 0) {
      char *response = "http/1.1 200 ok\r\n"
                       "content-type: text/html\r\n"
                       "content-length: 24\r\n\r\n"
                       "<h1>hello, rahman!</h1>";

      send(client_fd, response, strlen(response), 0);
    } else {
      char *response = "http/1.1 404 Not Found\r\n"
                       "content-type: text/html\r\n"
                       "content-length: 22\r\n\r\n"
                       "<h1>Not Found 404</h1>";

      send(client_fd, response, strlen(response), 0);
    }

    close(client_fd);
  }

  close(server_fd);
  return 0;
}
