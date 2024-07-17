#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : 
	_httpMethod(""),
	_httpVersion(""),
	_uri(""),
	bodyFileName(""),
	queryString(""),
	request_status(HEADERS),
	body_status(NONE),
	_errorCode(0),
	_bodySize(0),
	_cookie("") {}

HttpRequest::HttpRequest(const HttpRequest &httprequest) :
	_httpMethod(httprequest._httpMethod),
	_httpVersion(httprequest._httpVersion),
	_uri(httprequest._uri),
	_headerFields(httprequest._headerFields),
	bodyFileName(httprequest.bodyFileName),
	queryString(httprequest.queryString),
	_errorCode(httprequest._errorCode),
	_bodySize(httprequest._bodySize),
	_cookie(httprequest._cookie) {}


HttpRequest &HttpRequest::operator=(const HttpRequest &httprequest)
{
	if (this != &httprequest)
	{
		this->_httpMethod = httprequest._httpMethod;
		this->_httpVersion = httprequest._httpVersion;
		this->_uri = httprequest._uri;
		this->_headerFields = httprequest._headerFields;
		this->bodyFileName = httprequest.bodyFileName;
		this->queryString = httprequest.queryString;
		this->_errorCode = httprequest._errorCode;
		this->_bodySize = httprequest._bodySize;
		this->_cookie = httprequest._cookie;
	}
	return (*this);
}

HttpRequest::~HttpRequest() {}


void trim_front(std::string &str)
{
	std::size_t i = 0;
	while (i < str.size() && (str[i] == ' ' || str[i] == '\t'))
		i++;
	str = str.substr(i);
}

void trim_back(std::string &str)
{
	std::size_t pos;

	pos = str.size();
	while (pos > 0 && (str[pos - 1] == ' ' || str[pos - 1] == '\t'))
		pos--;
	if (pos == 0)
		return;
	// sus error
	str = str.substr(0, pos);
}

void str_trim(std::string &str)
{
	trim_front(str);
	trim_back(str);
}

std::vector<std::string> split(std::string &str, char delim)
{
	std::vector<std::string> split_str;
	std::string tmp;
	size_t pos;

	pos = str.find(delim);
	while (pos != std::string::npos)
	{
		tmp = str.substr(0, pos);
		str = str.substr(pos + 1);
		split_str.push_back(tmp);
		pos = str.find(delim);
	}
	split_str.push_back(str);
	return split_str;
}

bool isHexadecimal(std::string &str)
{

	if (str.empty())
		return false;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isxdigit(str[i]))
			return false;
	}
	return true;
}

bool isNumber(std::string &str)
{

	if (str.empty())
		return false;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(str[i]))
			return false;
	}
	return true;
}

std::string Generate_Random_File_Name()
{
	const char* alphanum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-";
    const int charsetSize = sizeof(alphanum) - 1;
	std::string	tempName;

	std::srand(static_cast<unsigned int>(time(NULL)));
    for (int i = 0; i < 10; ++i) {
		int randomIndex = std::rand() % charsetSize;
        tempName += alphanum[randomIndex];
	}
    return tempName;
}

bool	HttpRequest::is_body()
{
	std::map<std::string, std::string>::iterator it_length = this->_headerFields.find("Content-Length");
	std::map<std::string, std::string>::iterator it_chunk = this->_headerFields.find("Transfer-Encoding");
	if (it_length != this->_headerFields.end()) {
		this->body_status = HttpRequest::CONTENT_LENGTH;
		return true;
	}
	else if (it_chunk != this->_headerFields.end() && it_chunk->second == "chunked") {
		this->body_status = HttpRequest::CHUNKED;
		return true;
	}
	else if (it_chunk != _headerFields.end() \
		&& _headerFields["Transfer-Encoding"].find("chunked") == std::string::npos) {
		this->_errorCode = 501; //Not implemented
		return false;
	}
	else if (it_length == _headerFields.end() && it_chunk == _headerFields.end()) {
		this->_errorCode = 400; //Bad Request
		return false;
	}
	return false;
}

void HttpRequest::setMethod(const std::string& method)
{
	this->_httpMethod = method;
	if (this->_httpMethod != "POST" && this->_httpMethod !="GET" && this->_httpMethod != "DELETE")
        this->_errorCode = 501; /*Not Implemented method*/
}

void HttpRequest::setUri(const std::string& uri) {
	this->_uri = uri;
}

void HttpRequest::setHttpVersion(const std::string &version) {
	this->_httpVersion = version;
}

void HttpRequest::setRequest(std::string &request_line)
{
	std::vector<std::string> request = split(request_line, ' ');
	setMethod(request[0]);
	setUri(request[1]);
	setHttpVersion(request[2]);
}

void HttpRequest::printHeaders()
{
	std::map<std::string, std::string>::iterator it;

	for (it = this->_headerFields.begin(); it != this->_headerFields.end(); ++it)
		std::cout << it->first << ":" << it->second << std::endl;
}

void HttpRequest::setHeaderField(std::string &headers) {
    std::string CRLF("\r\n");
    size_t crlf_pos, delim_pos;
    std::string key, value;
    std::string header_line;

    while (headers.find(CRLF) != std::string::npos) {
        crlf_pos = headers.find(CRLF);
        header_line = headers.substr(0, crlf_pos);
        headers = headers.substr(crlf_pos + 2);
        if (header_line.empty())
            break;
        delim_pos = header_line.find(":");
        if (delim_pos != std::string::npos) {
            key = header_line.substr(0, delim_pos);
            value = header_line.substr(delim_pos + 1);
            trim_front(value);
            if (key == "Cookie") {
                _cookie = value;
                // std::cout << "Cookie detected: " << _cookie << std::endl;
            }
            this->_headerFields.insert(std::pair<std::string, std::string>(key, value));
            // std::cout << "Header: " << key << " = " << value << std::endl;
        }
    }
    // std::cout << "Final Cookie Header: " << this->_cookie << std::endl;
}


int	hexToInt(const std::string& str) {
    int intValue;

    std::istringstream(str) >> std::hex >> intValue;
    return intValue;
}

void HttpRequest::_getChunkedBody(std::string &body)
{
    std::string CRLF("\r\n");
    std::size_t crlf_pos;
    std::fstream bodyDataFile;
    
    bodyDataFile.open(this->bodyFileName.c_str(), std::ios::app | std::ios::out | std::ios::in | std::ios::binary);
	if (!bodyDataFile.is_open()) {
		this->request_status = HttpRequest::REQUEST_READY;
		this->_errorCode = 500; //internalSeral error
		return ;
	}
    while (!body.empty())
    {
        crlf_pos = body.find(CRLF);
        if (crlf_pos == std::string::npos)
        {
            break; // Wait for more data to arrive
        }

        std::string chunkSizeStr = body.substr(0, crlf_pos);
        body = body.substr(crlf_pos + 2); // Remove chunk size line and CRLF

        // Convert chunk size from hex to decimal
        std::size_t chunkSize = hexToInt(chunkSizeStr);

        if (chunkSize == 0)
        {
            this->request_status = HttpRequest::REQUEST_READY;
            break;
        }

        if (body.size() < chunkSize + 2) // Check if we have the whole chunk and trailing CRLF
        {
            body = chunkSizeStr + CRLF + body; // Put the size back and wait for more data
            break;
        }

        std::string chunk = body.substr(0, chunkSize);
        body = body.substr(chunkSize + 2); // Remove chunk and trailing CRLF

        this->_bodySize += chunk.size();
        bodyDataFile.write(chunk.c_str(), chunk.size());
    }

    bodyDataFile.close();
}

bool HttpRequest::setBody(std::string &body)
{
	if (this->bodyFileName.empty())
		this->bodyFileName = "/tmp/" + Generate_Random_File_Name();
	if (this->body_status == HttpRequest::CHUNKED)
	{
			//std::cout << body << std::endl;
			//std::cout << "************************" << std::endl;

		_getChunkedBody(body);
		if (this->request_status == HttpRequest::REQUEST_READY)
			return true;
	}
	else
	{
		std::map<std::string, std::string>::iterator it_chunk = this->_headerFields.find("Content-Length");
		size_t content_length = atoi(it_chunk->second.c_str());
		size_t bytes_left = content_length - (size_t)this->_bodySize;
		std::fstream bodyDataFile;
		bodyDataFile.open(this->bodyFileName.c_str(), std::fstream::app | std::fstream::out | std::fstream::in);
		if (!bodyDataFile.is_open()) {
			this->_errorCode = 500; //internalServer error
			return true;
		}
		if (bytes_left > 0 && body.size() <= bytes_left)
		{
			bodyDataFile << body;
			this->_bodySize += body.size();
			body = "";
		}
		bodyDataFile.close();
		if (this->_bodySize == content_length)
			return true;
	}
	return false;
}

void HttpRequest::setRequestData(std::string &data)
{
	this->requestData += data;
}

void HttpRequest::_parseURI() {
    size_t questionMarkPos = this->_uri.find_first_of('?'); // Check for query parameters

    if (questionMarkPos != std::string::npos) {
        this->queryString = this->_uri.substr(questionMarkPos + 1);
        this->_uri = this->_uri.substr(0, questionMarkPos);
    } else {
        this->queryString.clear();
    }
    if (this->_uri.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=") != std::string::npos)
        this->_errorCode = 400; // Bad Request
    if (this->_uri.length() > 2048)
        this->_errorCode = 414; // 414 URI Too Long
}

void HttpRequest::parseHttpRequest(std::string &read_request)
{
	std::string CRLF("\r\n");
	std::string request_line;
	std::string headers_lines;
	std::string rest;
	size_t pos;

	pos = read_request.find(CRLF);
	if (pos != std::string::npos)
	{
		request_line = read_request.substr(0, pos);
		headers_lines = read_request.substr(pos + 2);
		std::vector<std::string> request = split(request_line, ' ');
		setMethod(request[0]);
		if (this->_errorCode != 501) {
			setUri(request[1]);
			setHttpVersion(request[2]);
			_parseURI();
			if (this->_errorCode != 400 && this->_errorCode != 414) {
				// setRequest(request_line);
				setHeaderField(headers_lines);
				read_request = headers_lines;
				if (this->_httpMethod == "POST")
					this->request_status = HttpRequest::BODY;
				else
					this->request_status = HttpRequest::REQUEST_READY;
			}
			else
				this->request_status = HttpRequest::REQUEST_READY;
		}
		else
			this->request_status = HttpRequest::REQUEST_READY;
	}
}

void HttpRequest::set_bodyStatus(BODY_STATE status)
{
	this->body_status = status;
}

void HttpRequest::set_requestStatus(REQUEST_STATE status)
{
	this->request_status = status;
}

std::string HttpRequest::getMethod() const {
	return this->_httpMethod;
}

std::string HttpRequest::getHttpVersion() const {
	return this->_httpVersion;
}

std::string	&HttpRequest::getUri()  {
	return this->_uri;
}

std::string HttpRequest::getHeader(const std::string& headerName) const 
{
    std::map<std::string, std::string>::const_iterator it = _headerFields.find(headerName);
    if (it != _headerFields.end()) 
        return it->second;
    return "";
}

 std::map<std::string, std::string>& HttpRequest::getHeaderFields() { 
    return this->_headerFields; 
}

size_t &HttpRequest::getBodysize() {
	return this->_bodySize;
}

std::string &HttpRequest::get_bodyFileName()
{
	return (this->bodyFileName);
}

int HttpRequest::getErrorCode() const {
    return this->_errorCode;
}

std::string &HttpRequest::get_queryString() {
	return (this->queryString);
}

std::string &HttpRequest::getRequestData()
{
	return (this->requestData);
}

std::string &HttpRequest::getCookie()
{
	return (this->_cookie);
}

int HttpRequest::get_bodyStatus()
{
	return (this->body_status);
}

int HttpRequest::get_requestStatus()
{
	return (this->request_status);
}

std::string trimHeader(const std::string& str) 
{
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");

    if (first == std::string::npos || last == std::string::npos)
        return "";

    return str.substr(first, (last - first + 1));
}
