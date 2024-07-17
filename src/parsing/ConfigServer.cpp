#include "ConfigServer.hpp"

ConfigServer::ConfigServer() : 
_host("127.0.0.1"),
_port(8080),
_serverName(""),
_maxBodySize(100000),
_autoindex(false),
_root("/var/www"),
_index(std::vector<std::string>()),
_errorPage(std::map<int , std::string>())
{}

ConfigServer::ConfigServer(const ConfigServer &src) {
	*this = src;
}

ConfigServer&	ConfigServer::operator=(const ConfigServer &src) {
	if (this != &src)
	{
		this->_host = src._host;
		this->_port = src._port;
		this->_serverName = src._serverName;
		this->_maxBodySize = src._maxBodySize;
		this->_location = src._location;
		this->_autoindex = src._autoindex;
		this->_root = src._root;
		this->_index = src._index;
		this->_errorPage = src._errorPage;
	}
	return *this;
}

ConfigServer::~ConfigServer() {
	// std::cout << "ok\n";
}

int	ConfigServer::getSocket() const {
    return this->socketFD;
}

void	ConfigServer::setSocket(int sock) {
	this->socketFD = sock;
}

void	ConfigServer::setListen(std::string& listen) {
	int	count = 0;
	std::size_t	pos = listen.find(':');
	if (pos == std::string::npos)
		throw ConfigServerException("Invalid host:port format - colon delimiter not found");
	std::istringstream iss(listen);
    std::string hostPart;
    std::string portPart;
	
    getline(iss, hostPart, ':');
    getline(iss, portPart);
    if (isValidIPAddress(hostPart))
    	this->_host = hostPart;
	else
		throw ConfigServerException("Invalid IP address format");
	if (portPart.empty())
		return throw ConfigServerException("Port must be setted");
    for (size_t i = 0; i < portPart.length(); ++i) {
        if (!isdigit(portPart[i]))
			throw ConfigServerException("Invalid port - contains non-digit characters");
        // Port part contains non-digit characters
		std::istringstream(portPart) >> this->_port;
	}
	count++;
}

const std::string &ConfigServer::getHost() const {
	return (this->_host);
}

const size_t &ConfigServer::getPort() const {
	return (this->_port);
}

void	ConfigServer::setServerName(std::string& serverName) {
	if (serverName.empty() || serverName.find_first_of(" \t") != std::string::npos)
		throw ConfigServerException("Error: Wrong ServerName");
	this->_serverName = serverName;
}

const std::string &ConfigServer::getServerName() const {
	return this->_serverName;
}

void	ConfigServer::setBodySize(std::string& bodySize) {
	if (bodySize.empty())
		throw ConfigServerException("Empty max_body_size");
	std::size_t sizeEnd = bodySize.size() - 1;
    char unit = std::tolower(bodySize[sizeEnd]);

    for (std::string::size_type i = 0; i < sizeEnd; ++i) {
        if (!std::isdigit(bodySize[i]))
            throw  ConfigServerException("Invalid max_body_size.");
    }
    if (unit != 'm' && unit != 'g' && unit != 't' && unit != 'k' && !std::isdigit(unit)) {
        throw  ConfigServerException("Invalid max_body_size."); }
	this->_maxBodySize = parseMaxBodySize(unit, bodySize, sizeEnd);
}

const size_t &ConfigServer::getMaxBodySize() const {
	return this->_maxBodySize;
}

void	ConfigServer::setLocation(std::vector<t_tokens> &tok, std::vector<t_tokens>::iterator& it) {
	this->_location.push_back(parseLocation(tok, it));
}

std::vector<ConfigLocation>	&ConfigServer::getLocation() {
	return this->_location;
}

void	ConfigServer::setAutoIndex(std::string& autoindex) {	
	if (autoindex != "ON" && autoindex != "OFF")
		throw ConfigServerException("Error: Wrong Autoindex!");
	if (autoindex == "ON")
		this->_autoindex = true;
	else
		this->_autoindex = false;
}

bool	&ConfigServer::getAutoIndex() {
	return this->_autoindex;
}

void	ConfigServer::setRoot(std::string& root) {
	if (root.empty() || root.find_first_of(" \t") != std::string::npos)
		throw ConfigServerException("Error: Wrong root!");
	this->_root = root;
}

std::string	&ConfigServer::getRoot() {
	return this->_root;
}

void	ConfigServer::setIndex(std::string &index) {
	this->_index = splitVal(index);
}

std::vector<std::string>	&ConfigServer::getIndex() {
	return this->_index;
}

void	ConfigServer::setErrorPage(std::string& errorPage) {	
	if (errorPage.empty())
		throw ConfigServerException("Error: Empty errorPage!");
	std::vector<std::string> val;
	std::map<int , std::string>	errors;

	val = splitVal(errorPage);
	if (val.size() % 2 != 0)
		throw ConfigServerException("Error: Invalid error_page!");
	for (size_t pos = 0; pos < val.size(); pos++) {
		size_t	code = isNum(val[pos++]);
		std::string	path = val[pos];
		errors[code] = path;
	}
	this->_errorPage = errors;
}

std::map<int, std::string>	&ConfigServer::getErrorPage() {
	return this->_errorPage;
}
