#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation() :
	_locationName(""),
	_root(""),
	_index(),
	_methods(),
	_maxBodySize(100000),
	_autoindex(false),
	_upload(""),
	_errorPage(),
	_redirect(false),
	_redirection(""),
	_redirectCode(0),
	_interpreter(),
	_supportCgi(false)
{}

ConfigLocation::ConfigLocation(const ConfigLocation &src) {
	*this = src;
}

ConfigLocation& ConfigLocation::operator=(const ConfigLocation &src) {
	if (this != &src) {
		this->_locationName = src._locationName;
		this->_root = src._root;
		this->_index = src._index;
		this->_methods = src._methods;
		this->_maxBodySize = src._maxBodySize;
		this->_autoindex = src._autoindex;
		this->_upload = src._upload;
		this->_errorPage = src._errorPage;
		this->_redirect = src._redirect;
		this->_redirection = src._redirection;
		this->_redirectCode = src._redirectCode;
		this->_supportCgi = src._supportCgi;
	}
	return *this;
}

ConfigLocation::~ConfigLocation() {}

void	ConfigLocation::setLocationName(std::string& locationName) {
	if (locationName.empty() || locationName.find_first_of(" \t") != std::string::npos)
		throw ConfigLocationException("Error: Wrong location name.");
	this->_locationName = locationName;
}

std::string& ConfigLocation::getLocationName() {
	return this->_locationName;
}

void	ConfigLocation::setRoot(std::string& root) {
	if (root.empty() || root.find_first_of(" \t") != std::string::npos)
		throw ConfigLocationException("Error: Wrong root!");
	this->_root = root;
}

std::string& ConfigLocation::getRoot() {
	return this->_root;
}

void	ConfigLocation::setIndex(std::string& index) {
	this->_index = splitVal(index);
}

std::vector<std::string>&	ConfigLocation::getIndex() {
	return this->_index;
}

void	ConfigLocation::setMethods(std::string& methods) {
	if (methods.empty())
		throw ConfigLocationException("Error: Empty methods!");
	this->_methods = splitVal(methods);
	std::vector<std::string>::iterator it = _methods.begin();
	for (; it != _methods.end(); ++it) {
		if (*it != "POST" && *it != "GET" && *it != "DELETE"
			&& *it != "HEAD" && *it != "PUT")
			throw ConfigLocationException("Error: Unexpected value for methods!");
	}
}

std::vector<std::string>& ConfigLocation::getMethods() {
	return this->_methods;
}

void	ConfigLocation::setBodySize(std::string& bodySize) {
	if (bodySize.empty())
		throw ConfigLocationException("Error: Empty max_body_size!");
	std::size_t sizeEnd = bodySize.size() - 1;
    char unit = std::tolower(bodySize[sizeEnd]);

    for (std::string::size_type i = 0; i < sizeEnd; ++i) {
        if (!std::isdigit(bodySize[i]))
            throw  ConfigLocationException("Invalid max_body_size.");
    }
    if (unit != 'm' && unit != 'g' && unit != 't' && unit != 'k' && !std::isdigit(unit)) {
        throw ConfigLocationException("Invalid max_body_size.");}
	this->_maxBodySize = parseMaxBodySize(unit, bodySize, sizeEnd);
}

size_t&	ConfigLocation::getMaxBodySize() {
	return this->_maxBodySize;
}

void	ConfigLocation::setAutoIndex(std::string& autoindex) {	
	if (autoindex != "ON" && autoindex != "OFF")
		throw ConfigLocationException("Error: Unexpected value for Autoindex!");
	if (autoindex == "ON")
		this->_autoindex = true;
	else
		this->_autoindex = false;
}

bool&	ConfigLocation::getAutoIndex() {
	return this->_autoindex;
}

void	ConfigLocation::setUpload(std::string& upload) {	
	if (isAllSpacesOrTabs(upload)) {
		this->_upload = "";
		return ;
	}
	if (upload.find_first_of(" \t") != std::string::npos)
		throw ConfigLocationException("Error: Wrong Upload!");
	this->_upload = upload;
}

std::string&	ConfigLocation::getUpload() {
	return this->_upload;
}

void	ConfigLocation::setErrorPage(std::string& errorPage) {	
	if (errorPage.empty())
		throw ConfigLocationException("Error: Empty errorPage!");
	std::vector<std::string> val;
	std::map<int , std::string>	errors;

	val = splitVal(errorPage);
	if (val.size() % 2 != 0)
		throw ConfigLocationException("Error: Invalid error_page!");
	for (size_t pos = 0; pos < val.size(); pos++) {
		size_t	code = isNum(val[pos++]);
		std::string	path = val[pos];
		errors[code] = path;
	}
	this->_errorPage = errors;
}

std::map<int, std::string>&	ConfigLocation::getErrorPage() {
	return this->_errorPage;
}

void	ConfigLocation::setRedirect(bool redirect) {
	this->_redirect = redirect;
}

bool&	ConfigLocation::getRedirect() {
	return this->_redirect;
}

void	ConfigLocation::setRedirection(std::string& redirection) {
	std::vector<std::string> args = splitArgs(redirection);

	if (args.size() > 2)
		throw ConfigLocationException("Error: Invalid redirection args!");
	if (args.size() == 1 && isUrl(args[0]))
		this->_redirection = args[0];
	else if (args.size() == 2) {
		this->_redirectCode = isNum(args[0]);
		this->_redirection = args[1];
	}
	// else
	// 	throw ConfigLocationException("Error: Invalid redirection URL!");
}

std::string&	ConfigLocation::getRedirection() {
	return (this->_redirection);
}

int&	ConfigLocation::getRedirectCode() {
	return (this->_redirectCode);
}

void	ConfigLocation::setInterpreter(std::string& interpreter) {
	std::vector<std::string> val;
	std::map<std::string , std::string>	interpreters;

	val = splitVal(interpreter);
	if (val.size() % 2 != 0)
		throw ConfigLocationException("Error: Invalid interpreter!");
	for (size_t pos = 0; pos < val.size(); pos++) {
		std::string	extension;
		if (val[pos] == "php" || val[pos] == "py") {
			extension = val[pos++];
		}
		else
			throw ConfigLocationException("Error: Invalid extension format-type!");
		std::string	interpret = val[pos];
		interpreters[extension] = interpret;
	}
	this->_interpreter = interpreters;
	this->_supportCgi = true;
}

std::map<std::string, std::string>&	ConfigLocation::getInterpreter() {
	return this->_interpreter;
}

bool&	ConfigLocation::getSuppCgi() {
	return this->_supportCgi;
}