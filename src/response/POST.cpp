#include "HttpResponse.hpp"

bool	HttpResponse::_isSupportedUploadPath() {
    // Find upload path logic
	if (_uploadPath.empty()) {
		// buildResponse(404);
		return 0;
	}
    return 1;
}

void	HttpResponse::_createFile(std::string &filename) {
	std::ofstream file(_uploadPath.c_str());

    if (file) {
        file << _postBody;
        file.close();
		this->_errCode = 201;
		std::string hostt = _serv.getHost() + ":" + toString(_serv.getPort());
		std::string dirdir = findDirName(_uploadPath, _root) + "/";
		// std::cout << "dir " << dirdir << "\n";
		_headers["Location"] = "http://" + hostt + dirdir + filename;
		buildResponse(201);
		// std::cout << _uploadPath << "\n";
		 /*201 Created success status response code indicates
		that the request has succeeded and has led to the creation of a resource*/
    } 
    else {
        std::cerr << "Failed to create file." << std::endl;
        this->_errCode = 500; // Internal Server Error
		buildResponse(_errCode);
    }
}

void	HttpResponse::processPostMethod() {
	std::string	filename = _generateTempFileName();
	if (_uploadPath[_uploadPath.size() - 1] != '/')
	{
       	_uploadPath += "/";
    }
    std::map<std::string, std::string>::iterator it = _reqHeader.find("Content-Type");
	if(it != _reqHeader.end())
    {
		std::string value = trimHeader((*it).second);
        std::string contentType = getMimeTypes("second", value);
		if (contentType.empty()) {
			buildResponse(415);
			return ;
		}
		filename += contentType;
		_contentType = value;
    }
	// std::cout << "dyal resp: " << _uploadPath << "\n";
    _uploadPath += filename;
	_createFile(filename);
}

std::string extractHeadersPOST(std::string httpResponse)
{
    size_t end_headers = httpResponse.find("\r\n\r\n");
    if (end_headers == std::string::npos)
        return "";
    else
        return httpResponse.substr(0, end_headers + 4);
}

std::string extractBodyPOST(std::string httpResponse)
{
    size_t bodyStart = httpResponse.find("\r\n\r\n");
    if (bodyStart == std::string::npos)
    {
        return "";
    }
    return httpResponse.substr(bodyStart + 4);
}

std::string findContentTypePOST(std::string response)
{
    std::istringstream responseStream(response);
    std::string line;
    std::string contentType;

    while (std::getline(responseStream, line))
    {
        if (strncasecmp(line.c_str(), "Content-Type:", 12) == 0)
        {
            size_t pos = line.find(':');
            if (pos != std::string::npos)
            {
                contentType = line.substr(pos + 1);
                size_t firstNonSpace = contentType.find_first_not_of(" \t");
                size_t lastNonSpace = contentType.find_last_not_of(" \t");
                if (firstNonSpace != std::string::npos && lastNonSpace != std::string::npos)
                {
                    contentType = contentType.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
                    size_t semicolonPos = contentType.find(';');
                    if (semicolonPos != std::string::npos)
                    {
                        contentType = contentType.substr(0, semicolonPos);
                    }
                }
                break;
            }
        }
    }
    return contentType;
}

void HttpResponse::handlePostMethod() {
    if (!_isSupportedMethod("POST")) {
        buildResponse(405);
        return;
    }

    if (_isSupportedUploadPath() && _filePath.find(".py") == std::string::npos && _filePath.find(".php") == std::string::npos) {
        std::ifstream bodyfile(_bodyFileName.c_str());
        std::ostringstream filecontent;
        filecontent << bodyfile.rdbuf();
        _postBody += filecontent.str();
        bodyfile.close();
        processPostMethod();
        return;
    } else {
        int type = _checkRequestedType();
        if (type == FILE_TYPE) {
            _postRequestFile();
            return;
        } else if (type == FOLDER_TYPE) {
            _postRequestFolder();
            return;
        } else if (type == ERROR) {
            buildResponse(404);
            return;
        }
    }

}


void HttpResponse::_postRequestFile() {
    if (_filePath.find(".py") != std::string::npos || _filePath.find(".php") != std::string::npos) {
        CGI cgi(_client, _filePath);
        std::string script_name = Get_File_Name_From_URI();
        cgi.set_environmentVariables(script_name);
        cgi.RUN();
        if (cgi.status_code != 200) {
            buildResponse(cgi.status_code);
            return;
        }
        std::string cgi_headers = extractHeadersPOST(_client.getResponse());
        size_t pos = cgi_headers.find("Set-Cookie");
        if (pos != std::string::npos) {
            cgi_headers = cgi_headers.substr(pos);
            pos = cgi_headers.find("\r\n");
            this->_cookie = cgi_headers.substr(0, pos); // Stocker le cookie extrait
        }
        std::string response_cgi = _client.getResponse();
        _contentType = findContentTypePOST(response_cgi);
        _client.setResponseBody(extractBodyPOST(_client.getResponse()));
        std::stringstream ss;
        ss << _client.getResponseBody().length();
        std::string body_length = ss.str();
        _headers["Content-Length"] = body_length;
        _client.setResponseHeader(createResponseHeader(200, "Nothing"));
        _isText = true;
    }
    else {
        buildResponse(403);
    }
}

void	HttpResponse::isUrihasSlashInTHeEnd() {
	size_t urisize = _client.getRequest().getUri().size();
	if ((_root[_root.size() - 1]) != '/' && _client.getRequest().getUri()[urisize - 1] != '/')
	{
		std::string hostt = _serv.getHost() + ":" + toString(_serv.getPort());
        std::string dirdir = _location.getLocationName().empty() ? findDirName(_filePath, _root) + "/" : _location.getLocationName() + findDirName(_filePath, _root) + "/";
        // std::cout << _filePath << " lastdir: " << dirdir<< "\n";
       _redirection = "http://" + hostt + dirdir;
	   std::string header = createResponseHeader(301, "Default");
    	_client.setResponseHeader(header);
        _client.setResponseBody(_errorPath);
		_slashSetted = true;
		return ;
	}
}

void	HttpResponse::_postRequestFolder() {
	isUrihasSlashInTHeEnd();
	if (_slashSetted == true) {
		return ;
	}
	if (isPostDirHasIndexFiles())
		return;
	else {
		buildResponse(403);
	}
}

bool HttpResponse::isPostDirHasIndexFiles() {
	if (_idxFiles.size() != 0) {
		for (size_t i = 0; i <_idxFiles.size(); i++) {
			std::string path = _filePath + _idxFiles[i];
			// std::cout << "haas index\n" << path << "\n";
			path = deleteRedundantSlash(path);
			std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);

            if (file.is_open())
            {
                 _errCode = 200;
                _filePath = path;
                file.close();
				_postRequestFile();
                return true;
            }
			else {
				buildResponse(404);
				return true;
			}
		}
		return true;
	}
	return false;
}

std::string	HttpResponse::_generateTempFileName() {
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