#include "Config.hpp"
#include "ParseFile.hpp"

Config::Config() :
	_serverCount(0),
	_fileName(DEFAULT_CONFIG)
{}

Config::Config(const char *fileName) :
	_serverCount(0),
	_fileName(fileName),
	_servers()
{}

Config::Config(const Config &other) {
	*this = other;
}

Config&	Config::operator=(const Config &other) {
	if (this != &other) {
		this->_fileName = other._fileName;
		this->_serverCount = other._serverCount;
		this->_servers = other._servers;
		this->_tokens = other._tokens;
	}
	return *this;
}

Config::~Config() {}

const std::vector<ConfigServer>	&Config::getServers() const{
    return this->_servers;
}

ConfigServer Config::parseServerConfig(std::vector<t_tokens>::iterator& it) {

	++it;
	int lis = 0, serv = 0, rt = 0, bd = 0;
	int	loc = 0, aut = 0, ind = 0, err = 0;
	ConfigServer	server;
    // Parse server configuration settings
	while (it != this->_tokens.end() && it->_type != "}" && it->_value != "}") {
		if (it->_type == "location") {
			server.setLocation(this->_tokens ,it);
			loc++;
			it++;
			continue;
		}
		else if (it->_type == "listen") {
			server.setListen(it->_value);
			lis++;
		}
		else if (it->_type == "server_name") {
			server.setServerName(it->_value);
			serv++;
		}
		else if (it->_type == "client_max_body_size") {
			server.setBodySize(it->_value);
			bd++;
		}
		else if (it->_type == "autoindex") {
			server.setAutoIndex(it->_value);
			aut++;
		}
		else if (it->_type == "root") {
			server.setRoot(it->_value);
			rt++;
		}
		else if (it->_type == "index") {
			server.setIndex(it->_value);
			ind++;
		}
		else if (it->_type == "error_page") {
			server.setErrorPage(it->_value);
			err++;
		}
		else if (it->_type.empty()) {
			it++;
			continue;
		}
		else {
			throw ParseServerException("Error: Unexpected parametre.");
			break;
		}
		it++;
	}
	if (lis != 1)
		throw ParseServerException("Error: Should have one listen parametre.");
	else if (serv > 1)
		throw ParseServerException("Error: Must have one server_name parametre.(Duplicate)");
	else if (rt > 1)
		throw ParseServerException("Error: Must have one root parametre.(Duplicate)");
	else if (loc < 1)
		throw ParseServerException("Error: Should have at least one block of location.");
	else if (bd > 1)
		throw ParseServerException("Error: Must have one body_size parametre.(Duplicate)");
	else if (aut > 1)
		throw ParseServerException("Error: Must set one autoindex parametre.(Duplicate)");
	else if ((rt == 1 && ind != 1) || ind > 1)
		throw ParseServerException("Error: Should have one index parametre.(Duplicate)...");
	else if (err > 1)
		throw ParseServerException("Error: Must set one error_page parametre.(Duplicate)");
	// std::cout << "end of server\n";
	if (it->_type != "}")
		throw ParseServerException("Error: expected '}' in the end of server directive.");
	return (server);
}

void	Config::parse()
{
	try {
		this->_tokens = ParseFile::readFile(this->_fileName);
		std::vector<t_tokens>::iterator it = _tokens.begin();
		while (it != this->_tokens.end()) 
		{
			if (it->_type.empty()) 
			{
				it++;
				continue;
			}
			else if (it->_type == "server" && it->_value != "}")
				this->_servers.push_back(parseServerConfig(it));
			else
				throw ParseServerException("Error: Unexpected token.");
			it++;
		}
		this->_serverCount = this->_servers.size();
		if (this->_serverCount == 0)
			throw ParseServerException("Error: Must have at least one server.");
		while (this->_serverCount > 1) 
		{
			for (size_t i = 0; i < this->_serverCount - 1; i++) 
			{
				for (size_t j = i + 1; j < this->_serverCount; j++) 
				{
					if ((this->_servers[i].getPort() == this->_servers[j].getPort()
						&& this->_servers[i].getHost() == this->_servers[j].getHost())
						|| (this->_servers[i].getPort() == this->_servers[j].getPort()
						&& this->_servers[i].getServerName() == this->_servers[j].getServerName()))
						throw ParseServerException("Error: Same Server.");
				}
			}
			break;
		}
		// std::cout << "Host: " << _servers[0].getHost() << ", Port: " << _servers[0].getPort() 
		// << ", ServerName: " << _servers[0].getServerName()
		// << ", BodySize: " << _servers[0].getMaxBodySize() << std::endl;
		// for (std::vector<t_tokens>::iterator it =_tokens.begin(); it != this->_tokens.end(); ++it) 
		// 	std::cout << "type: " << it->_type << ", Value: " << it->_value << std::endl;
	} catch(const std::exception &e) {
		std::cout << e.what() << std::endl;
		exit(1);
	}
}
