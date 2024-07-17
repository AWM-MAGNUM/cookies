#ifndef NETWORKCLIENT_HPP
#define NETWORKCLIENT_HPP

#include <netinet/in.h>
#include <cstring>
#include "../parsing/ConfigServer.hpp"
#include "HttpRequest.hpp"  // Ensure this contains the complete definition of HttpRequest

class HttpRequest;
class NetworkClient {
	public:
		NetworkClient();
		NetworkClient(int socketDescriptor, int serverSocket);
		NetworkClient(const NetworkClient& source);
		~NetworkClient();
		NetworkClient& operator=(const NetworkClient& source);

		bool hasFileBeenAccessed() const;
		void markFileAsAccessed(bool accessed);

		bool wasHeaderDispatched() const;
		void markHeaderAsDispatched(bool dispatched);

		std::string retrieveResponseContent() const;
		void updateResponseContent(const std::string& content);

		std::string retrieveBodyContent() const;
		void updateBodyContent(const std::string& content);

		std::string retrieveHeaderContent() const;
		void updateHeaderContent(const std::string& content);

		int fetchServerSocket() const;
		int fetchConnectionSocket() const;
		void assignConnectionSocket(int socket);
		sockaddr_in* fetchClientInfo();
		socklen_t* fetchAddressLength();

		bool isResponsePrepared() const;
		void setServer(const ConfigServer& server);
		friend bool operator==(const NetworkClient& lhs, const NetworkClient& rhs);
		const ConfigServer& getConfigServer() const;
		void saveRequestData(size_t nb_bytes) {
			std::string str_bytes(this->_buffer, nb_bytes);
			this->_req.setRequestData(str_bytes);
		};

		ConfigServer& getServer();

		void setRequest(HttpRequest req);
		void setResponseHeader(std::string respHeader);
		void setResponseBody(std::string body);
		void setHeaderSent(bool value);
		void setResponse(std::string response);
		void setOpenFile(bool value);
		void set_Response(std::string response, size_t RespSize);

		HttpRequest& getRequest();
		std::string getResponseHeader();
		std::string getResponseBody();
		bool getHeaderSent();
		std::string getResponse();
		bool getOpenFile();
		void openFileForReading();
		bool isFileOpen() const;
		void readFromFile(char* buffer, std::streamsize bufferSize);
		std::streamsize bytesRead() const;

		char _buffer[1024];
		std::ifstream _file;
		std::streamsize bytes_read;
		void setBytesSent(std::size_t bytes);
    	std::size_t getBytesSent() const;

	private:
		bool	_respReady;
		HttpRequest _req;
		int serverSocketId;
		int connectionSocketId;
		ConfigServer server;
		socklen_t clientAddressSize;
		sockaddr_in clientDetails;
		std::string responseHeader;
		std::string responseBody;
		std::string fullResponse;
		bool headerDispatched;
		bool fileAccessed;
		bool _headersSent;
		bool _openFile;
		std::string _response;
		std::size_t bytesSent;
		size_t		_respSize;
};

#endif // NETWORK_CLIENT_HPP
