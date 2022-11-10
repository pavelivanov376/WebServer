#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

char* serializeResponse(char* status, char* contentType, char* body);
char* serializeResponse(char* status, char* contentType, char* body)
{
    char* str= malloc(10000);
    strcat(str, "HTTP/1.1 "); 
    strcat(str, status);
    strcat(str, "\nContent-Type: ");
    strcat(str, contentType);
    strcat(str, "\nContent-Length: ");
    const int n = snprintf(NULL, 0, "%lu", strlen(body));
    char buf[n+1];
    snprintf(buf, n+1, "%lu", strlen(body));
    strcat(str, buf);
    strcat(str, "\n\n");
    strcat(str, body);
    return str;
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    int addrlen = sizeof(address);

    char *httpResponse = serializeResponse("200 OK","text/plain", "Hello world!");//"HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    
    listen(server_fd, 10);

    while(1)
    {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        
        char httpRequest[30000] = {0};
        valread = read( new_socket , httpRequest, 30000);

        printf("%s\n",httpRequest);
        
        write(new_socket , httpResponse , strlen(httpResponse));

        close(new_socket);
    }
    return 0;
}