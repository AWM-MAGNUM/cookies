#include "HttpResponse.hpp"

std::string HttpResponse::_constructPath(const std::string& requestPath, const std::string &root, const std::string &index) {
	std::string path = requestPath;

    if (path.empty() || path[0] != '/') {
        path = "/" + path;
    }
	if (!path.empty() && path[path.length() - 1] == '/') {
        path += index;
    }
    // Check if there is no extension by finding the last dot in the string
    else if (path.find_last_of('.') == std::string::npos) {
        path += "/" + index;
    }
    std::string filePath = root + path;
    return filePath;
}

std::string getContentType(std::string filename) {
    std::string extension;
    if (filename.empty()) {
        return "text/html";
    }
    try {
	    extension = filename.substr(filename.find_last_of('.'));
    }
    catch (std::exception &e) {
        return "text/html";
    }
	return getMimeTypes("", extension);
}

bool HttpResponse::isDirHasIndexFiles() {
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
				_isFile();
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

// std::string	HttpResponse::_findDirectoryName() {
// 	size_t rootPos = _filePath.find(_root);
//     if (rootPos == std::string::npos)
//         return "";

//     // Remove root from the path
//     std::string dirname = _filePath.substr(rootPos + _root.length());

// 	//Create a string stream from the path
//     std::istringstream iss(dirname);

//     // Tokenize the path by '/'
//     std::string token;
//     std::string lastDirName;
//     while (std::getline(iss, token, '/')) {
//         if (!token.empty()) {
//             lastDirName = token;
//         }
//     }
//     return lastDirName;
// }

std::string findDirName(const std::string& path, const std::string& root)
{
	// Ensure root ends with '/'
    std::string adjustedRoot = root;
    if (!root.empty() && root[root.length() - 1] != '/')
    {
        adjustedRoot += '/';
    }

    // Find the position where root ends in the path
    size_t rootPos = path.find(adjustedRoot);
    if (rootPos == std::string::npos)
        return "";

    // Remove root from the path
    std::string dirname = path.substr(rootPos + adjustedRoot.length());

    // Find the last '/' character in the remaining path
    size_t pos = dirname.find_last_of('/');
    if (pos == std::string::npos)
        return "";

    // Extract the dirname
    dirname = dirname.substr(0, pos);
    return "/" + dirname;
}

void	HttpResponse::_getAutoIndex() {
	if (_autoindex) {
		std::string path = _filePath;
    	DIR *dir = opendir(path.c_str());

    	if (dir == NULL) {
        	return;
    	}
		std::string directory = _location.getLocationName().empty() ? findDirName(_filePath, _root) + "/" : _location.getLocationName() + findDirName(_filePath, _root) + "/";
		// _findDirectoryName();
		// std::cout << directory << "\n";

		std::ostringstream listeningfile;
		listeningfile << "<!DOCTYPE html>\n" << "<html lang=\"en\">\n" << "\t<head>\n" << "\t\t<meta charset=\"UTF-8\">\n";
		listeningfile << "\t\t\t<title>Index of " << directory << "</title>\n";
		listeningfile << "</head>\n<body bgcolor=\"white\"><h1>Index of " << directory << "</h1><hr><pre>";
		struct dirent *ent;

    	while ((ent = readdir(dir)) != NULL) {
			if (ent->d_name[0] == '.' && strcmp(ent->d_name, "..") != 0) {
					continue;
			}

			// Display the entry with a slash at the beginning if it's a directory
			if (ent->d_type == DT_DIR && strcmp(ent->d_name, "..") != 0) {
				listeningfile << "<a href=\"" << directory << ent->d_name << "\">/" << ent->d_name << "</a><br>";
			} else if (strcmp(ent->d_name, "..") == 0) {
				listeningfile << "<a href=\"" << directory << ent->d_name << "\">" << ent->d_name << "/</a><br>";
			}
			else {
				listeningfile << "<a href=\"" << directory << ent->d_name << "\">" << ent->d_name << "</a><br>";
			}
		}
		closedir(dir);
		listeningfile << "</pre><hr></body></html>";
		_fileSize = listeningfile.str().size();
		char tempDirTemplate[] = "/tmp/tmpDirXXXXXX";
    	char *tempDir = mkdtemp(tempDirTemplate);
		if (tempDir)
		{
			chmod(tempDir, S_IRWXU | S_IRWXG | S_IRWXO);
			// Créer le chemin complet du fichier HTML
			char tempFileName[1024];
			strcpy(tempFileName, tempDir);
			strcat(tempFileName, "/index.html");

			FILE *tempFile = std::fopen(tempFileName, "w");
			if (tempFile)
			{
				std::fputs(listeningfile.str().c_str(), tempFile);
				std::fclose(tempFile);
				_filePath = tempFileName;
				_isText = false;
				_client.setResponseBody(tempFileName);
			} else
			{
				_errCode = 500;
				buildResponse(_errCode);
				std::cerr << "Error: Failed to Creat Temporary file." << std::endl;
				return ;
			}
		}
    	else {
			_errCode = 500;
			buildResponse(_errCode);
			return;
		}
		_contentType = getContentType(_filePath);
		_client.setResponseHeader(createResponseHeader(200, "Nothing"));
	}
	else
		buildResponse(403);
}

std::string extractBody(std::string httpResponse)
{
    size_t bodyStart = httpResponse.find("\r\n\r\n");
    if (bodyStart == std::string::npos)
    {
        return "";
    }
    std::string body = httpResponse.substr(bodyStart + 4); // Skip the double newline
    return body;
}

std::string extractHeaders(std::string httpResponse)
{
    size_t end_headers = httpResponse.find("\r\n\r\n");
    if (end_headers == std::string::npos)
        return "";
    else
        return httpResponse.substr(0, end_headers);
}

std::string HttpResponse::Get_File_Name_From_URI()
{
    size_t lastSlashPos = _filePath.find_last_of('/');

    if (lastSlashPos != std::string::npos)
    {
		
        return _filePath.substr(lastSlashPos + 1);
    }

    const char *cstr = _filePath.c_str();
    const char *fileName = std::strrchr(cstr, '/');

    if (fileName)
    {
        return fileName + 1;
    }

    return _filePath;
}

std::string findContentType(std::string response)
{
    std::istringstream responseStream(response);
    std::string line;
    std::string contentType;

    while (std::getline(responseStream, line))
    {
        // Recherche du champ "Content-Type" (insensible à la casse)
        if (strncasecmp(line.c_str(), "Content-Type:", 12) == 0)
        {
            size_t pos = line.find(':');
            if (pos != std::string::npos)
            {
                contentType = line.substr(pos + 1);

                // Supprime les espaces autour de la valeur du champ
                size_t firstNonSpace = contentType.find_first_not_of(" \t");
                size_t lastNonSpace = contentType.find_last_not_of(" \t");
                if (firstNonSpace != std::string::npos && lastNonSpace != std::string::npos)
                {
                    contentType = contentType.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);

                    // Chercher la position du premier point-virgule
                    size_t semicolonPos = contentType.find(';');
                    if (semicolonPos != std::string::npos)
                    {
                        // Extraire la sous-chaîne avant le point-virgule
                        contentType = contentType.substr(0, semicolonPos);
                    }
                }
                break;
            }
        }
    }
    return contentType;
}

// void	HttpResponse::_isFile() 
// {
//     // Handle file
// 	std::string script_name = Get_File_Name_From_URI();
// 	std::string filePath = _client.getRequest().getUri();
//     std::ifstream file(_filePath.c_str(), std::ios::in | std::ios::binary);
// 	if (file) 
// 	{
// 		std::string extension = _filePath.substr(_filePath.find_last_of('.'));

// 		if (extension == ".php" || extension == ".py")
// 		{
// 			size_t pos;
// 			CGI cgi(_client, _filePath);
// 			cgi.set_environmentVariables(script_name);
// 			cgi.RUN();
// 			if (cgi.status_code != 200)
// 			{  
// 				// std::cout << "ERROCODE CGI " << cgi.status_code << std::endl;
// 				buildResponse(cgi.status_code);
// 				return;
// 			}
// 			std::string cgi_headers = extractHeaders(_client.getResponse());
// 			//std::cout << "Headers CGI: " << cgi_headers << "\n";
// 			pos = cgi_headers.find("Set-Cookie");
// 			if (pos != std::string::npos)
// 			{
// 				cgi_headers = cgi_headers.substr(pos);
// 				pos = cgi_headers.find("\r\n");
// 				this->cookies = cgi_headers.substr(0, pos);
// 			}
// 			std::string response_cgi = _client.getResponse();
// 			_contentType = findContentType(response_cgi);
// 			_client.setResponseBody(extractBody(_client.getResponse()));
// 			std::stringstream ss;
// 			ss << _client.getResponseBody().length();
// 			std::string body_length = ss.str();
// 			_headers["Content-Length"] = body_length;
// 			_client.setResponseHeader(createResponseHeader(200, "Nothing"));
// 			_isText = true;
// 			return;
// 		}
// 		// std::cout << "the file exist: " << _filePath<< "\n";
// 		_contentType = getContentType(_filePath);
// 		std::string header = createResponseHeader(200, "Nothing");
// 		_client.setResponseHeader(header);
// 		_client.setResponseBody(_filePath);
// 		// std::cout << _client.getResponseBody() << std::endl;

// 		return ;
// 	}
// 	else {
// 		buildResponse(404);
// 		return ;
// 	}
// }


void HttpResponse::_isFile() 
{
    std::string script_name = Get_File_Name_From_URI();
    std::ifstream file(_filePath.c_str(), std::ios::in | std::ios::binary);
    if (file) 
    {
        std::string extension = _filePath.substr(_filePath.find_last_of('.'));
        if (extension == ".php" || extension == ".py")
        {
            size_t pos;
            CGI cgi(_client, _filePath);
            cgi.set_environmentVariables(script_name);
            cgi.RUN();
            if (cgi.status_code != 200)
            {
                buildResponse(cgi.status_code);
                return;
            }
            std::string cgi_headers = extractHeaders(_client.getResponse());

			
            pos = cgi_headers.find("Set-Cookie");
            if (pos != std::string::npos)
            {
                cgi_headers = cgi_headers.substr(pos);
                pos = cgi_headers.find("\r\n");
                this->_cookie = cgi_headers.substr(0, pos);
            }
            std::string response_cgi = _client.getResponse();
            _contentType = findContentType(response_cgi);
            _client.setResponseBody(extractBody(_client.getResponse()));
            std::stringstream ss;
            ss << _client.getResponseBody().length();
            std::string body_length = ss.str();
            _headers["Content-Length"] = body_length;
            _client.setResponseHeader(createResponseHeader(200, "Nothing"));
            _isText = true;
            return;
        }
        _contentType = getContentType(_filePath);
        std::string header = createResponseHeader(200, "Nothing");
        _client.setResponseHeader(header);
        _client.setResponseBody(_filePath);
        return;
    }
    else {
        buildResponse(404);
        return;
    }
}
int	HttpResponse::_checkRequestedType() {
	struct stat path_stat;
	if (stat(_filePath.c_str(), &path_stat) != 0) {
		_errCode = 404;
        return ERROR;
    }
	if (S_ISREG(path_stat.st_mode))
		return FILE_TYPE;
	else if (S_ISDIR(path_stat.st_mode))
		return FOLDER_TYPE;
	return 3;
}

void	HttpResponse::_isFolder() {
	// std::cout << "foldeeer\n";
	// isUrihasSlashInTHeEnd();
	if (isDirHasIndexFiles())
		return;
	else {
		_getAutoIndex();
		return ;
	}
}

void	HttpResponse::handleGetMethod() {
	if (!_isSupportedMethod("GET")) {
		buildResponse(405);
		return ;
	}
	int type = _checkRequestedType();
	if (type == FILE_TYPE) {
		_isFile();
		return;
	}
	else if (type == FOLDER_TYPE) {
		_isFolder();
		return ;
	}
	else if (type == ERROR){
		buildResponse(404);
		return ;
	}
}

bool	HttpResponse::_isSupportedMethod(std::string meth) {
	
	std::vector<std::string>::iterator	it = _methods.begin();

	for (; it != _methods.end(); ++it) {
		if (*it == meth)
			return true;
	}
    return false;
}
