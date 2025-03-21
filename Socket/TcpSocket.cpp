
#include <iostream> 
#include <sstream> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string> 
#include <winsock2.h> 
#include <ws2tcpip.h>  
#pragma comment(lib, "ws2_32.lib")
//to use the function of the routing 
#include "../Routing/routing.cpp"

//const for the buffer size
const int BUFFER_SIZE = 30750;

/**
 * this class for creating a socket object for connection
 * @package TcpSocket
 */
class TcpSocket
{
    
    private:
        //hold the server Socket
        int serverSocket;
        //to hold the number of recieved data from the client
        int  bytesReceived;
        //for windows
        WSADATA wsaData;
        //hold the client Socket
        int clientSocket;
        //store the address of the server
        struct sockaddr_in serverAddress;
        //hold the address length
        int length=sizeof(serverAddress);
        //to hold the recieved data from the client
        char buffer[ BUFFER_SIZE] = {0};
        
    public:

         //read log messages
        void log(const std::string &message)
        {
            std::cout << message << std::endl;
        }

        //get the last error message
        void exitWithError(const std::string &errorMessage)
        {
            std::cout << WSAGetLastError() << std::endl;
            log("ERROR: " + errorMessage);
            exit(1);
        }  

       /**
         * Creating the Server Socket for network communication
         * @param AF_INET --> It specifies the IPv4 protocol family.
         * @param SOCK_STREAM --> It defines that the TCP type socket.
         * @return  serverSocket
         * */
        int createSocket()
        {
            //specify the windows socket version
            if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
            {
                //if the startup up failed
                exitWithError("WSAStartup failed");
            }

            //create the server socket 
            serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            //if the process of starting server socket failed
            if(serverSocket < 0){
                //the process failed
                exitWithError("Cannot create socket");
                return 1;
            }
            return 0;
        }

        /**
         * Creating the Server Socket Address
         * @param NULL
         * @return NULL
         * */
        void defineServerAddress()
        {
            //IPv4 Internet protocols
            serverAddress.sin_family = AF_INET;
            //set the server port
            serverAddress.sin_port = htons(8000);
            //make the socket  listen to all the available IPs
            serverAddress.sin_addr.s_addr = INADDR_ANY;
        }

         /**
         * binding the Server Socket 
         * @param NULL
         * @return NULL
         * */
        void ServerSocketBinding()
        {
            //ties the serverSocket to a specific IP address and port number
            if ( bind(serverSocket, (struct sockaddr*)&serverAddress, 
            sizeof(serverAddress)) < 0) {
                //if the binding process goes wrong
                exitWithError("Cannot connect socket to address");
                return ;
            }
        }

          /**
         * Puts the socket into "listening" mode that can accept incoming connection requests.
         * @param NULL
         * @return NULL
         * */
        void ConnectionListening()
        {
            /**
             * Puts the socket into "listening" mode
             * Sets the "backlog" queue size
             * **/
            if(listen(serverSocket, 10) < 0){
                //if the listing process goes wrong
                exitWithError("Socket listen failed");
            }
        }

          /**
         *  accept the connection request that is recieved 
         *  on the socket the application was listening to 
         * @param NULL
         * @return NULL
         * */
        void AcceptClientConnection()
        {
            //using infinit loop to always accept requests untile i terminate the server
            while(true){
                //accept the client request for connection
                clientSocket = accept(serverSocket, (sockaddr *)&serverAddress, &length);
                //check if the connection failed or not 
                if(clientSocket < 0){
                    //connetion failed
                    exitWithError("connection  failed");
                    return;
                }
                //recieve the data from the cleint 
                RecieveClientData();
                std::cout << " data recieved ----!" << std::endl;
                //display the data that sended from the client
                DisplayClientData();
                std::cout << " data displayed ----!" << std::endl;
                //send the intended response based on the client request 
                sendResponseToClient();  
                std::cout << "response got sended,Great job!" << std::endl;
               //close the client connection because its stateless and wait for another connection from the cleint
                closesocket(clientSocket); 
                std::cout << "client closed!" << std::endl;
           }
        }

        /**
         * recieve data from the client
         * @param NULL
         * @return NULL
         * */
        void RecieveClientData()
        {
            //accept the client request for connection
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            //if the bytesReceived is negative the recieved process failed
            if(bytesReceived < 0){
                //there is no recieved data
                exitWithError("Failed to receive bytes from client socket connection");
                //exit from this function
                return;
            }
           
        }

        /**
         * display the recieved client data
         * @param NULL
         * @return NULL
         * */
        void DisplayClientData()
        {
            //accept the client request for connection
            std::cout << "Client Request:\n" << buffer << std::endl;
        }
        
        /**
         * 
         * send  the response to the client
         * @param NULL
         * @return NULL
         * 
         * */
        void sendResponseToClient() {
            //get the URI that entered by the client 
            std::string url = extractUrlFromBuffer(buffer);
            // Extract the full path and read the content
            std::string fileContent=readFileContent(url);
           //check if it is empty or not 
            if (fileContent.empty()) {
                // Handle file not found or read error using http response
                std::string notFoundResponse =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: 58\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "<html><body><h1>404 Not Found</h1></body></html>";
                //send http  response that this endpoint not exist 
                send(clientSocket, notFoundResponse.c_str(), notFoundResponse.size(), 0);
            } 
            //if the file has content
            else {
                // Determine the content length of the file
                size_t contentLength = fileContent.size();
        
                // Determine the MIME type (implement getMimeType function as needed)
                std::string mimeType = "text/html";
        
                // Construct the HTTP response headers
                std::ostringstream responseStream;
                responseStream << "HTTP/1.1 200 OK\r\n"
                               << "Content-Type: " << mimeType << "\r\n"
                               << "Content-Length: " << contentLength << "\r\n"
                               << "Connection: close\r\n"
                               << "\r\n";
        
                // Append the file content to the response
                responseStream << fileContent;
        
                // Convert the response to a string
                std::string httpResponse = responseStream.str();
        
                // Send the HTTP response to the client with the requires response
                send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);
            }
        }

        /**
         * close the server side 
         * @param NULL
         * @return NULL 
         * */
       void closeServer()
       {
            closesocket(serverSocket);
       }
  
};


