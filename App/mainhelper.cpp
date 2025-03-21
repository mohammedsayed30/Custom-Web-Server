#include <iostream>
#include "../Socket/TcpSocket.cpp"

void ServerRun()
{
    //start the process
    std::cout << "starting----!" << std::endl;
    //define object from TcpSocket
    TcpSocket server;
    //create the server socket to listining for requests
    server.createSocket();
    std::cout << "socket created----!" << std::endl;
   //defin the address of the server
    server.defineServerAddress();
    std::cout << "address----!" << std::endl;
    //binding the server to a specific address  & port
    server.ServerSocketBinding();
    std::cout << "binding----!" << std::endl;
   //listen for any connection
    server.ConnectionListening();
    std::cout << "connection----!" << std::endl;
   //accept the connection from the client 
    server.AcceptClientConnection(); 
    // Close server only when done
    server.closeServer();  
    // Cleanup Winsock AFTER the server stops
    WSACleanup();  
}