#include "WebServer.hpp"

std::string trimm(const std::string& str) 
{
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");

    if (first == std::string::npos || last == std::string::npos)
        return "";

    return str.substr(first, (last - first + 1));
}

NetworkClient& WebServer::GetRightClient(int fd) 
{
    std::map<int, NetworkClient>::iterator it = this->clients.find(fd);
    if (it != this->clients.end()) {
        return it->second;
    }
    else
        throw std::runtime_error("BUG: Potential Server error");
}

WebServer::WebServer(const Config& config)
    : highestFd(0),
      currentClientIndex(0) // Initialize the current client index
{
    serverConfigs = new std::vector<ConfigServer>(config.getServers());

    FD_ZERO(&masterSet);
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);

    setupServerSockets();
}

WebServer::~WebServer() 
{
    for (size_t i = 0; i < serverSockets.size(); ++i) 
        close(serverSockets[i]);
    for (std::map<int, NetworkClient>::iterator it = clients.begin(); it != clients.end(); ++it)
        close(it->second.fetchConnectionSocket());
    delete serverConfigs;
}

void setSocketNonBlocking(int socket_fd) 
{
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) 
    {
        std::cerr << "Error getting socket flags: " << strerror(errno) << std::endl;
        close(socket_fd);
        return;
    }

    flags |= O_NONBLOCK;
    if (fcntl(socket_fd, F_SETFL, flags) == -1) 
    {
        std::cerr << "Error setting socket non-blocking: " << strerror(errno) << std::endl;
        close(socket_fd);
        return;
    }
    // std::cout << "Socket " << socket_fd << " set to non-blocking mode." << std::endl;
}

void WebServer::addSocketFd(int fd)
{
	this->serverSockets.push_back(fd);
	FD_SET(fd, &(this->readSet));
}

void WebServer::setupServerSockets() 
{
     for (size_t i = 0; i < serverConfigs->size(); ++i) 
    {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
        {
            std::cerr << "Error opening socket for server " << (*serverConfigs)[i].getServerName() << ": " << strerror(errno) << std::endl;
            continue;
        }
        int optval = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) 
        {
            std::cerr << "Error setting socket options for server " << (*serverConfigs)[i].getServerName() << ": " << strerror(errno) << std::endl;
            close(sockfd);
            continue;
        }

        sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr((*serverConfigs)[i].getHost().c_str());
        serv_addr.sin_port = htons((*serverConfigs)[i].getPort());

        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        {
            std::cerr << "ERROR on binding for server " << (*serverConfigs)[i].getServerName() << " on " << (*serverConfigs)[i].getHost() << ":" << (*serverConfigs)[i].getPort() << ": " << strerror(errno) << std::endl;
            close(sockfd);
            continue;
        }
        setSocketNonBlocking(sockfd);
        if (listen(sockfd, SOMAXCONN) < 0) 
        {
            std::cerr << "Error listening on socket for server " << (*serverConfigs)[i].getServerName() << ": " << strerror(errno) << std::endl;
            close(sockfd);
            continue;
        }
        addSocketFd(sockfd);
        FD_SET(sockfd, &this->masterSet);
        //you should add the one of write
        if (sockfd > highestFd) 
            this->highestFd = sockfd;
       
        (*serverConfigs)[i].setSocket(sockfd);
        
       std::cout << "Server " << (*serverConfigs)[i].getServerName() 
                 << " set up on " << (*serverConfigs)[i].getHost() 
                 << ":" << (*serverConfigs)[i].getPort() << " with socket: " << sockfd << std::endl;
    }
}

void WebServer::CheckRequestStatus(NetworkClient &client)
{
	HttpRequest &request = client.getRequest();
	std::string &request_data = request.getRequestData();
	std::string CRLF_delim("\r\n\r\n");
	size_t pos;

	if (request.get_requestStatus() == HttpRequest::HEADERS)
	{
		pos = request_data.find(CRLF_delim);
		if (pos != std::string::npos)
			request.parseHttpRequest(request_data);
		if (request_data.empty() && (request.getMethod() == "GET" || request.getMethod() == "DELETE"))
			request.set_requestStatus(HttpRequest::REQUEST_READY);
	}
	if (request.get_requestStatus() == HttpRequest::BODY)
	{
		if (request.is_body()) {
            if (request.setBody(request_data))
                request.set_requestStatus(HttpRequest::REQUEST_READY);
        }
        else if (request.getErrorCode() == 400 || request.getErrorCode() == 501)
            request.set_requestStatus(HttpRequest::REQUEST_READY);
    }
}

void WebServer::processClientRequests(int fd) {
    NetworkClient &client = GetRightClient(fd);
    char *buffer = client._buffer;
    size_t client_buffer_size = sizeof(client._buffer);
    size_t bytes_received;

    bzero(buffer, client_buffer_size);
    bytes_received = read(client.fetchConnectionSocket(), buffer, client_buffer_size - 1);
    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            close(fd);
            closeClient(fd);
            FD_CLR(fd, &(this->readSet));
            return;
        }
    }
	client.saveRequestData(bytes_received);
	// std::cout<< client.getRequest().getRequestData() << std::endl;
    CheckRequestStatus(client);
    if (client.getRequest().get_requestStatus() == HttpRequest::REQUEST_READY) {
        // std::cout << "Meth: " << client.getRequest().getMethod() <<  "\n";
    // client.getRequest().printHeaders();
    // std::cout << "\n";
        // std::cout << "size of body " << client.getRequest().getBodysize();
    //     std::string hostHeader = client.getRequest().getHeader("Host");
    //     hostHeader = trimm(hostHeader);

    //     size_t portPos = hostHeader.find(":");
    //     int port = 80; // Default to 80 if no port is specified
    //     if (portPos != std::string::npos) {
    //         port = std::atoi(hostHeader.substr(portPos + 1).c_str());
    //         hostHeader = hostHeader.substr(0, portPos);
    //     } else {
    //         port = client.getServer().getPort();
    //     }

    //     const ConfigServer &clientServer = matchServerByName(client.getRequest().getHeader("Host"), port);
    //    // std::cout << "Port in processClientRequests: " << clientServer.getPort() << std::endl; // Debugging output
    //     client.setServer(clientServer);
        FD_SET(fd, &this->writeSet);

    }
}


void WebServer::run() {
    fd_set readcpy;
    fd_set writecpy;
    while (true) {
        readcpy = this->readSet;
        writecpy = this->writeSet;
        this->highestFd = *std::max_element(this->serverSockets.begin(), this->serverSockets.end());
        // std::cout << "this->highestFd " << this->highestFd << std::endl;
        int maxFd = this->highestFd;
        // std::cout << "this->highestFd2 " << maxFd<< std::endl;
        signal(SIGPIPE, SIG_IGN);
        if (select(maxFd + 1, &readcpy, &writecpy, NULL, NULL) < 0) {
            std::cerr << "Error in select()." << std::endl;
        }
        for (int i = 3; i <= maxFd; i++) {
                if (FD_ISSET(i, &writecpy)) {
                    NetworkClient &client = GetRightClient(i);
                    sendDataToClient(client);
                }
                else if (FD_ISSET(i, &readcpy)) {
                    if (FD_ISSET(i , &(this->masterSet))) {
                        acceptNewClient(i);                                               
                    } else {
                        processClientRequests(i);
                    }
                }
        }   
    }
}

const ConfigServer& WebServer::matchServerByFd(int fd) 
{
    for (size_t i = 0; i < serverConfigs->size(); ++i) 
    {
        if ((*serverConfigs)[i].getSocket() == fd) 
            return (*serverConfigs)[i];
    }
   // std::cerr << "No matching server found for socket fd: " << fd << std::endl;
    return (*serverConfigs)[0];
}

void WebServer::acceptNewClient(int serverSocket) {
    sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSocket < 0) {
        std::cerr << "Failed to accept client." << std::endl;
        return;
    }
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    // long option_len = 1;
    // setsockopt(clientSocket, SOL_SOCKET, SO_NOSIGPIPE , &option_len, sizeof(option_len));
    // setSocketNonBlocking(clientSocket);
    const ConfigServer& associatedServer = matchServerByFd(serverSocket);
    NetworkClient newClient(clientSocket, serverSocket);
    newClient.setServer(associatedServer);
    clients[clientSocket] = newClient;
    addSocketFd(clientSocket);
    // FD_SET(clientSocket, &this->readSet);
    // if (clientSocket > highestFd)
    //     highestFd = clientSocket;
}


void WebServer::closeClient(int clientSocket) 
{
    std::map<int, NetworkClient>::iterator it = clients.find(clientSocket);
    if (it != clients.end()) 
    {
        close(it->first);
        FD_CLR(it->first, &masterSet);
        FD_CLR(it->first, &readSet);
        FD_CLR(it->first, &writeSet);
        clients.erase(it);
        std::cout << "Client with socket " << clientSocket << " has been closed and removed." << std::endl;
    } 
    else
        std::cerr << "Attempt to close non-existent client socket." << std::endl;
}

NetworkClient* WebServer::findClientBySocket(int socket) 
{
    std::map<int, NetworkClient>::iterator it = clients.find(socket);
    if (it != clients.end())
        return &it->second;
    return NULL;
}

const ConfigServer& WebServer::matchServerByName(const std::string& host, int port) 
{
    std::string hostName = trimm(host);
    size_t pos = hostName.find(":");
    bool isLocalhost = false;

    if (pos != std::string::npos)
        hostName = hostName.substr(0, pos);

    if (hostName == "localhost" || hostName == "127.0.0.1")
        isLocalhost = true;

    for (std::vector<ConfigServer>::const_iterator it = serverConfigs->begin(); it != serverConfigs->end(); ++it) 
    {
        std::ostringstream portStr;
        portStr << it->getPort();
        if ((it->getHost() == "localhost" || it->getHost() == "127.0.0.1") && isLocalhost)
        {
            if (static_cast<size_t>(port) == it->getPort())
            {
                // std::cout << "Matched server: " << it->getServerName() << " with host: " << it->getHost() << " on port: " << it->getPort() << std::endl;
                return *it;
            }
        }
        else if (it->getHost() == hostName && static_cast<size_t>(port) == it->getPort())
        {
        //    std::cout << "\n******* ✅ ✅ ✅ ✅Matched server: " << it->getServerName() << " with host: " << it->getHost() << " on port: " << it->getPort() << std::endl;
            return *it;
        }
    }

//    std::cerr << "\n******* ❌ ❌ ❌ ❌No matching server found for host: " << host << " on port: " << port << ". Defaulting to first server." << std::endl;
    return (*serverConfigs)[0];
}

void WebServer::sendDataToClient(NetworkClient& client) 
{
    sendResponse(client.getRequest(), client);
}