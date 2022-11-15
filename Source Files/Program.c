#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

char* serializeResponse(char* status, char* contentType, char* body);
char* parseRequest(char* request);
char* getRequestType(char* request);
char* getRequestedResource(char* request);

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    int addrlen = sizeof(address);
    bind(server_fd, (struct sockaddr *)&address, addrlen);
    
    listen(server_fd, 10);
    while(1)
    {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        
        char httpRequest[30000] = {0};
        long valread = read( new_socket , httpRequest, 30000);

        char *httpResponse = parseRequest(httpRequest);
        
       // char *httpResponse = serializeResponse("200 OK","text/plain", "Hello world!");//"HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
        write(new_socket , httpResponse , strlen(httpResponse));
  
        close(new_socket);
    }
    return 0;
}

char* parseRequest(char* request)
{
    char* type = getRequestType(request);
    char * resource = getRequestedResource(request);

    if (strcmp(type,"GET"))
    {
        if (strcmp(resource,"/index.html"))
        {
            return serializeResponse("200 OK","text/plain", "hello text");
        }
        else if (strcmp(resource,"/home.html"))
        {
            return serializeResponse("200 OK","text/html", "Hello HTML");
        }
        else if (strcmp(resource,"/pic.jpg"))
        {
            return serializeResponse("200 OK","text/pic", "Hello PIC");
        }
        else
        {
            return serializeResponse("400 NOT FOUND","text/pic", "");
        }
    } 
    else if (strcmp(type,"PUT"))
    {

    } 
    else 
    {
        return serializeResponse("40 NOT IMPLEMENTED","text/plain", "");
    }
}
/*
GET /index.html HTTP/1.1
Host: localhost:8080
Connection: keep-alive
Cache-Control: max-age=0
sec-ch-ua: " Not;A Brand";v="99", "Google Chrome";v="97", "Chromium";v="97"
sec-ch-ua-mobile: ?0
sec-ch-ua-platform: "Linux"
DNT: 1
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.99 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,**;q=0.8,application/signed-exchange;v=b3;q=0.9
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br
Accept-Language: en,bg;q=0.9
*/
char* getRequestType(char* request)
{
    char string[50] = "Hello! We are learning about strtok";
    char * token = strtok(string, " ");
    token = strtok(NULL, " ");

    //char * type = strtok(request, " ");  //    GET
    // char * resource = strtok(request, " "); //    /index.html
}
char* getRequestedResource(char* request)
{
   // char * resource = strtok(request, " "); //    /index.html
}

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
