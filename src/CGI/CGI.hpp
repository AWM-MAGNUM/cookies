#ifndef CGI_HPP
#define CGI_HPP

#include <sys/types.h>
#include <fcntl.h>
#include <cstring>
#include <sys/wait.h>
#include "../networking/NetworkClient.hpp"
#include "../response/HttpResponse.hpp"
#include <signal.h>
#include <unistd.h>

class NetworkClient;
class HttpRequest;
class HttpResponse;


class CGI
{
	char** env;
	NetworkClient &client;
	std::string _filePath;

	public:
	int status_code;
	CGI(NetworkClient&, std::string& );
	CGI(const CGI&);
	CGI& operator=(const CGI&);
	~CGI();

	/*setters*/
	void set_environmentVariables(std::string&);

	/*getters*/
	char** get_CGIenvironmentVariables();

	void RUN();
};

#endif