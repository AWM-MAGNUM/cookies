#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cctype>
#include <cstring>
#include "NetworkClient.hpp"
#include "../parsing/Config.hpp"
#include "../parsing/ConfigServer.hpp"
#include "HttpRequest.hpp"
#include "../response/HttpResponse.hpp"


class RequestError {
	private:
		int _error_number;

	public:
		RequestError(int error_number) : _error_number(error_number) {
		}
		int getErrorNumber() const  {
			return (this->_error_number);
		}
};

class WebServer {
	public:
	    WebServer(const Config& config);
	    ~WebServer();

	    void			run();
	    NetworkClient&	GetRightClient(int fd);
	    void			CheckRequestStatus(NetworkClient &client);

	private:
		void setupServerSockets();
		void acceptNewClient(int serverSocket);
		void closeClient(int clientSocket);
		void processClientRequests(int clientSocket);
		void sendDataToClient(NetworkClient& client);
		NetworkClient* findClientBySocket(int socket);
		const ConfigServer& matchServerByFd(int fd);
		const ConfigServer& matchServerByName(const std::string& host, int port);
		void	sendResponse(HttpRequest &req, NetworkClient &client);
		void	addSocketFd(int fd);


		fd_set masterSet, readSet, writeSet;
		int					highestFd;
		std::vector<int>	serverSockets;
		std::map<int, NetworkClient>	clients;
		std::vector<ConfigServer>*		serverConfigs;
		std::map<int, std::string>		clientRequests; // Stockage des requêtes par client
		int currentClientIndex;
};
#endif

