#include "HttpResponse.hpp"

HttpResponse::HttpResponse(NetworkClient &client) :
	_client(client),
	_serv(client.getConfigServer()),
	_bodyFileName(""),
    _postBody(""),
	_errCode(0),
	_statusCode(""),
	_isCgi(false),
    _maxBodySize(0),
	_root(""),
    _uploadPath(""),
	_index(""),
    _idxFiles(),
	_errorPath(""),
	_autoindex(0),
    _methods(),
    _uri(""),
	_fd(0),
	_filePath(""),
    _buffer(""),
	_contentType(""),
    _reqHeader(), 
    _isText(false),
    _slashSetted(false),
    _cookie("")
    {}

HttpResponse::~HttpResponse(){}

bool	HttpResponse::isText() const{
	return this->_isText;
}

void	HttpResponse::locateErrorPage(int errCode) {
	for (std::map<int, std::string>::iterator it = _errorPage.begin(); it != _errorPage.end(); it++) {
		if (it->first == errCode) {
			_errorPath = it->second + "/" + toString(errCode) + ".html";
			_errorPath = deleteRedundantSlash(_errorPath);
			// std::cout << _errorPath << "\n";
			return ;
		}
	}
	_errorPath = "";
}

void	HttpResponse::_handleDefaultErrors() {
	if (_errCode == 505) {
		buildResponse(505);
		return ;
	}
	if (_errCode == 501) {
		buildResponse(501);
		return ;
	}
	if (_errCode == 400 || _errCode == 414) {
		buildResponse(_errCode);
		return ;
	}
}

void	HttpResponse::generateResponse(HttpRequest &req) {
	_errCode = req.getErrorCode();
    _cookie = req.getCookie();
	// std::cout << "errcode result from req:" << _errCode << "\n";
	// std::cout << "filePath loli: "<< _filePath << "\n";
	_uri = getRequestedResource(req);
	_filePath = deleteRedundantSlash(_uri);
    _reqHeader = req.getHeaderFields();
	if (_filePath.empty()) {
		buildResponse(404);
		return;
	}
	checkHttpVersion(req);
	if (_errCode != 0) {
		_handleDefaultErrors();
		return ;
	}
    if (_serv.getMaxBodySize() < req.getBodysize()) {
        buildResponse(413);
		return ; /*Content Too Large response status code indicates that
			// the request entity is larger than limits defined by server*/
    }
	if (!_redirection.empty()) {
		std::string header = createResponseHeader(301, "Default");
    	_client.setResponseHeader(header);
        _client.setResponseBody(_errorPath);
		return;
	}
	if (req.getMethod() == "GET") {
	 	handleGetMethod();
		return ;
	}
	if (req.getMethod() == "POST") {
        _bodyFileName = req.get_bodyFileName();
		handlePostMethod();
		return ;
	}
	if (req.getMethod() == "DELETE") {
		handleDeleteMethod(); //where u put the Delete method
		return ;
	}
	return ;
}

void	HttpResponse::findStatusCode(int code) {
    switch (code)
    {
        case 0:
            _statusCode = "200 OK";
            break;
        case 201:
            _statusCode = "201 Created";
            break;
        case 400:
            _statusCode = "400 Bad Request";
            break;
        case 301:
            _statusCode = "301 Moved Permanently";
            break;
        case 302:
            _statusCode = "302 Moved Temporarily";
            break;
        case 403:
            _statusCode = "403 Forbidden";
            break;
        case 404:
            _statusCode = "404 Not Found";
            break;
        case 405:
            _statusCode = "405 Method Not Allowed";
            break;
        case 413:
            _statusCode = "413 Payload Too Large";
            break;
		case 414:
			_statusCode = "414 URI Too Long";
			break;
        case 415:
			_statusCode = "415 Unsupported Media Type";
            break;
        case 500:
            _statusCode = "500 Internal Server Error";
            break;
        case 501:
            _statusCode = "501 Not Implemented";
            break;
        case 504:
            _statusCode = "504 Gateway Timeout";
            break;
        case 505:
            _statusCode = "505 HTTP Version Not Supported";
            break;
        default:
            _statusCode = "200 OK";
            break;
    }
}

std::string	HttpResponse::getContentLength(std::string path) {
	struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0) 
    {
		_fileSize = fileStat.st_size;
		// std::cout << "file exist of size: " << _fileSize << "\n";
        std::ostringstream oss;
        oss << fileStat.st_size;
        return oss.str();
    }
    return "0";
}

off_t HttpResponse::getFileSize() {
    return _fileSize;
}

std::string HttpResponse::generateDate()
{
    time_t t = time(0);
    struct tm *current_time = localtime(&t);
    char date_buffer[80];
    strftime(date_buffer, sizeof(date_buffer), "%a, %d %b %Y %H:%M:%S GMT", current_time);
    return (std::string(date_buffer));
}

// std::string	HttpResponse::createResponseHeader(int errCode, std::string flag) {
// 	std::string	respHeader;

// 	_headers["Server"] = "Webserv/1.0";
// 	if (!_redirection.empty()) {
// 		_headers["Location"] = _redirection;
// 		_errCode = 301;
// 	}
//     if (flag == "Default") {
//         if (!_errorPath.empty()) {
//             _headers["Content-Length"] = getContentLength(_errorPath);
//             _isText = false;
//         }
// 		else {
// 			std::stringstream sse;
//             findStatusCode(errCode);
//             sse << "<!DOCTYPE html>";
//             sse << "<html>";
//             sse << "<head><title>" << errCode << "</title></head>";
//             sse << "<body>";
//             sse << "<center><h1>" << _statusCode << "</h1></center>";
//             sse << "<hr><center>Welcome to our Webserv</center>";
//             sse << "</body>" << "</html>";
//             _errorPath = sse.str();
//     		_fileSize = _errorPath.size();
// 			_headers["Content-Length"] = toString(_fileSize);
//             _isText = true;
//         }
//     	_headers["Content-Type"] = "text/html";
//         if (_errCode == 201) {
//             _headers["Content-Type"] = _contentType;
//             _errorPath = "";
// 			_headers["Content-Length"] = "0";
//             // _isText = false;
//         }
// 	}
//     else {
//         if (_headers.find("Content-Length") == _headers.end()) {
// 		    _headers["Content-Length"] = getContentLength(_filePath);
//         }
//         // if (_headers.find("Content-Type") == _headers.end())
//         // {
//         //     std::cout << "kidkhol hna\n";
//         //     _headers["Content-Type"] = getContentType(_filePath);
//         // }
// 		// else
//         _headers["Content-Type"] = _contentType;
// 	}
//     if (!_cookie.empty()) {
//         _headers["Set-Cookie"] = "";
//     }
// 	_headers["Date"] = generateDate();
// 	std::stringstream ss;

// 	findStatusCode(errCode);
// 	_errCode = errCode;
//     if (_errCode == 0)
//         findStatusCode(0);
//     ss << "HTTP/1.1 " << _statusCode << "\r\n";
//     for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
// 	{
// 		ss << it->first << ": " << it->second << "\r\n";
// 	}
// 	ss << "\r\n";
// 	_buffer = ss.str();
// 	return _buffer;
// }


std::string HttpResponse::createResponseHeader(int errCode, std::string flag) {
    std::string respHeader;

    _headers["Server"] = "Webserv/1.0";
    if (!_redirection.empty()) {
        _headers["Location"] = _redirection;
        _errCode = 301;
    }
    if (flag == "Default") {
        if (!_errorPath.empty()) {
            _headers["Content-Length"] = getContentLength(_errorPath);
            _isText = false;
        } else {
            std::stringstream sse;
            findStatusCode(errCode);
            sse << "<!DOCTYPE html>";
            sse << "<html>";
            sse << "<head><title>" << errCode << "</title></head>";
            sse << "<body>";
            sse << "<center><h1>" << _statusCode << "</h1></center>";
            sse << "<hr><center>Welcome to our Webserv</center>";
            sse << "</body>" << "</html>";
            _errorPath = sse.str();
            _fileSize = _errorPath.size();
            _headers["Content-Length"] = toString(_fileSize);
            _isText = true;
        }
        _headers["Content-Type"] = "text/html";
        if (_errCode == 201) {
            _headers["Content-Type"] = _contentType;
            _errorPath = "";
            _headers["Content-Length"] = "0";
        }
    } else {
        if (_headers.find("Content-Length") == _headers.end()) {
            _headers["Content-Length"] = getContentLength(_filePath);
        }
        _headers["Content-Type"] = _contentType;
    }

    if (!_cookie.empty()) {
        _headers["Set-Cookie"] = _cookie;
        // std::ofstream logFile("cgi_debug.log", std::ios_base::app);
        // logFile << "Set-Cookie: " << _cookie << std::endl;
        // std::cout << "Set-Cookie: " << _cookie << std::endl;
    }

    _headers["Date"] = generateDate();
    std::stringstream ss;

    findStatusCode(errCode);
    _errCode = errCode;
    if (_errCode == 0)
        findStatusCode(0);
    ss << "HTTP/1.1 " << _statusCode << "\r\n";
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it) {
        ss << it->first << ": " << it->second << "\r\n";
    }
    ss << "\r\n";
    _buffer = ss.str();

    // std::cout << "Response Headers:\n" << _buffer << std::endl;

    return _buffer;
}






void	HttpResponse::buildResponse(int errCode) {
	_errCode = errCode;
	std::string statusCodeStr = toString(_errCode);
    locateErrorPage(_errCode);
    std::string header = createResponseHeader(_errCode, "Default");

    _client.setResponseHeader(header);
    std::cout << _errorPath << "\n";
    _client.setResponseBody(_errorPath);
}

void	HttpResponse::checkHttpVersion(HttpRequest &req) {
	std::string const &version = req.getHttpVersion();
	if (version != "HTTP/1.1") {
		_errCode = 505;
		return ;
	}
}

std::string HttpResponse::deleteRedundantSlash(std::string uri)
{
    std::string newUri;
    std::string::size_type i = 0;

    while (i < uri.size()) {
		// Add current character to newUri
        newUri.push_back(uri[i]);
        // If the current character is a slash, skip all subsequent slashes
        if (uri[i] == '/') {
            while (i < uri.size() && uri[i] == '/') {
                ++i;
            }
        } else {
            ++i;
        }
    }
    return newUri;
}

bool isDirectory(const char* path) {
    struct stat fileInfo;
    
    if (stat(path, &fileInfo) != 0) {
        return false;
    }
    
    return S_ISDIR(fileInfo.st_mode);
}

std::string	HttpResponse::getRequestedResource(HttpRequest &req) {
	_locations = _serv.getLocation();
	std::string location;
    for (std::vector<ConfigLocation>::iterator it = _locations.begin(); it != _locations.end(); ++it)
    {
		// std::cout << "'" << req.getUri() << "'";
		// std::cout << " --> " << "'" << it->getLocationName() << "'\n";
        size_t pos = req.getUri().find(it->getLocationName());
        if (pos != std::string::npos && pos == 0)
        {
			// std::cout << "dkhaaaal\n";
			if (req.getUri().size() > it->getLocationName().size()
				&& it->getLocationName() != "/"
				&& req.getUri()[it->getLocationName().size()] != '/');
			else if (it->getLocationName().size() > location.size())
			{
            	_location = *it;
				location = it->getLocationName();
			}
		}
	}
	if (req.getUri().find(_location.getRoot()) == std::string::npos)
	{
            _root = _location.getRoot();
            _maxBodySize = _serv.getMaxBodySize();
            _idxFiles = _location.getIndex();
            _autoindex =_location.getAutoIndex();
            _errorPage =_location.getErrorPage();
            _methods = _location.getMethods();
            _uploadPath = _location.getUpload();
			if (_location.getRedirect() == true)
				_redirection = _location.getRedirection();
            if (_location.getSuppCgi() == true)
                _interpreter = _location.getInterpreter();

            int idx = req.getUri().find(_location.getLocationName());
            std::string locationName = _location.getLocationName();
            std::string relativePath = (req.getUri().find_first_of(locationName) == 0) ?
                                      req.getUri().substr(locationName.length()) :
                                      req.getUri().substr(0, idx);
            _filePath = _constructPath(relativePath, _root, "");

            // std::cout << "fff " << _filePath << "\n";
            if (isDirectory(_filePath.c_str()) && req.getMethod() == "GET" && _isSupportedMethod("GET") && _location.getRedirect() == false) {
                size_t urisize = _client.getRequest().getUri().size();
                if ((_root[_root.size() - 1]) != '/' && _client.getRequest().getUri()[urisize - 1] != '/')
                {
                    // std::cout << "ma fhamtch1\n";
                    std::string hostt = _serv.getHost() + ":" + toString(_serv.getPort());
                    std::string dirdir = _location.getLocationName().empty() ? findDirName(_filePath, _root) + "/" : _location.getLocationName() + findDirName(_filePath, _root) + "/";
                    // std::cout << _filePath << " lastdir: " << dirdir<< "\n";
                    _redirection = "http://" + hostt + dirdir;
                    return _filePath;
                }
            }
            if (_filePath[_filePath.length() - 1] == '/')
                {_filePath = _filePath.substr(0, _filePath.length() - 1);}

            if (_autoindex)
            {
                // std::cout << "root " << _root << "\n";
                _filePath = _constructPath(relativePath, _root, "");
                return _filePath;
            }
        return _filePath;
	}
	_root = _serv.getRoot();
    _idxFiles = _serv.getIndex();
    _autoindex = _serv.getAutoIndex();
    _errorPage = _serv.getErrorPage();
    _maxBodySize = _serv.getMaxBodySize();
    _methods.push_back("POST");
    _methods.push_back("GET");
    _methods.push_back("DELETE");
    _filePath = _constructPath(req.getUri(), _root, "");
    if (isDirectory(_filePath.c_str()) && req.getMethod() == "GET" && _isSupportedMethod("GET")) {
        size_t urisize = _client.getRequest().getUri().size();
        if ((_root[_root.size() - 1]) != '/' && _client.getRequest().getUri()[urisize - 1] != '/')
        {
            // std::cout << "ma fhamtch2\n";
            std::string hostt = _serv.getHost() + ":" + toString(_serv.getPort());
            std::string dirdir = _location.getLocationName().empty() ? findDirName(_filePath, _root) + "/" : _location.getLocationName() + findDirName(_filePath, _root) + "/";
            // std::cout << _filePath << " lastdir: " << dirdir<< "\n";
            _redirection = "http://" + hostt + dirdir;
            return _filePath;
        }
    }
 	if (_autoindex)
    {
		if (req.getUri().find(_root) == std::string::npos)
		{
            _filePath = _constructPath(req.getUri(), _root, "");
		}
		return _filePath;
    }
    return _filePath;
}
