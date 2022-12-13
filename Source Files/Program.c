#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

char* serializeResponse(char* status, char* contentType, char* body);
char* parseRequest(char* request);
char* filename = "image.html";
char* html = "<!DOCTYPE html><html><head><title>KM</title></head><body><p>You requested HTML</p></body></html>";

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
        
        write(new_socket , httpResponse , strlen(httpResponse));
  
        close(new_socket);
    }
    return 0;
}
    /*
GET /index.html HTTP/1.1
    */
char* parseRequest(char* request)
{
    char * requestMethod = strtok(request, " ");

    char * resource = strtok(NULL, " ");

    char * version = strtok(NULL, " ");

    if (strcmp(requestMethod,"GET") == 0)
    {
        if (strcmp(resource,"/index.html") == 0)
        {
            return serializeResponse("200 OK","text/plain", "You requested plain text");
        }
        else if (strcmp(resource,"/home.html") == 0)
        {
            return serializeResponse("200 OK","text/html", html);
        }
        else if (strcmp(resource,"/image.html") == 0)
        {
            FILE    *textfile;
            char    *image;
            long    numbytes;
            
            textfile = fopen(filename, "r");
            
            fseek(textfile, 0L, SEEK_END);
            numbytes = ftell(textfile);
            fseek(textfile, 0L, SEEK_SET);  
        
            image = (char*)calloc(numbytes, sizeof(char));   
            
            fread(image, sizeof(char), numbytes, textfile);
            fclose(textfile);
           
            return serializeResponse("200 OK","text/html", image);
        }
        else
        {
            return serializeResponse("404 NOT FOUND","text/pic", "");
        }
    } 
    else if (strcmp(requestMethod,"DELETE"))
    {
        if (strcmp(resource,"/image.html") == 0)
        {
            remove(filename);
        }
    } 
    else 
    {
        return serializeResponse("401 NOT IMPLEMENTED","text/plain", "");
    }
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
