
// #include "HttpRequest.hpp"

// // void	HttpRequest::_matchServer() {
// // 	if (_headerFields.find("Host") != _headerFields.end())
// //     {
// //         std::string host = trimHeader(_headerFields["Host"]);

// // 		std::cout << "'" << host << "'" << std::endl;
// //         for (std::vector<ConfigServer>::iterator it = _confServ.begin(); it != _confServ.end(); it++)
// //         {
// // 			std::string hostServerName = it->getServerName() + ':' + toString(it->getPort());
// // 			std::string hostServerIp = it->getHost() + ":" + toString(it->getPort());
            
// // 			// check if the host is found in the servers vector
// //             if (hostServerIp == host || hostServerName == host)
// //             {
// // 				std::cout << "Matched server: " << hostServerName << " with host: " << host << std::endl;
// //                 _serv = *it;
// //                 return ;
// //             }
// //         }
// //     }
// // 	_serv = _confServ[0];
// // }

// void	HttpRequest::parseBody(size_t &bodypos) 
// {
// 	// _matchServer();
//     long contentLength = 0;
//     if (this->_method == "POST" && is_body(contentLength)) 
//     {
//         if (this->isChunked)
// 		{
//             _getChunkedBody(bodypos);
// 			this->_bodySize = this->_body.size();
//             // std::cout << "Extracted body: " << this->_body << std::endl;
// 			if (this->_serv.getMaxBodySize() < this->_bodySize)
// 				this->_errorCode = 413; /*Content Too Large response status code indicates that
// 			// the request entity is larger than limits defined by server*/
// 		}
//         else if (contentLength > 0)
//         {
//             // if (bodypos + contentLength <= this->_request.size()) 
//             // {
// 				std::string requestString = this->_request;
//                 std::string bodyContent = requestString.substr(bodypos, contentLength);
//                 this->_body += bodyContent;
// 				this->_bodySize = contentLength;
// 				if (this->_serv.getMaxBodySize() < this->_bodySize)
// 					this->_errorCode = 413;
//                 // std::cout << "Extracted body: " << this->_body << std::endl;
//             // } 
//             // else 
//             //     std::cerr << "Error: bodypos is out of range. Request size: " << this->_request.size() << " bodypos: " << bodypos << std::endl;
//         }
//     }
// }

// bool	HttpRequest::is_body(long& contentLength) {
// 	std::map<std::string, std::string>::const_iterator it = _headerFields.find("Content-Length");

// 	if (it != _headerFields.end()) {
//         contentLength = std::strtol(it->second.c_str(), NULL, 10);
//         return true;
//     }
// 	std::string	transfer_encod("Transfer-Encoding");

// 	if (_headerFields.find("Transfer-Encoding") != _headerFields.end() 
// 		&& _headerFields[transfer_encod].find("chunked") != std::string::npos) {
// 		this->isChunked = true;
// 		return true;
// 	}
// 	else if (_headerFields.find("Transfer-Encoding") != _headerFields.end() 
// 		&& _headerFields[transfer_encod].find("chunked") == std::string::npos) {
// 		this->_errorCode = 501; //Not implemented
// 		return false;
// 	}
// 	else if (this->_method == "POST" && _headerFields.find("Content-Length") == _headerFields.end()
// 		&& _headerFields.find("Transfer-Encoding") == _headerFields.end()) {
// 		this->_errorCode = 400; //Bad Request
// 		return false;
// 	}
// 	return false; /*ayaetina makaynch body*/
// }

// void	HttpRequest::_getChunkedBody(size_t &bodypos) {
// 	// Implementation for chunked transfer encoding
// 	std::string	tmp = this->_request.substr(bodypos);
// 	size_t	bodySize = tmp.size();

// 	for (size_t i = 0; i < bodySize; i++) {
// 		std::string	chunk = "";
// 		size_t	j = i;
// 		for (; tmp[j] != '\r'; j++) {
// 			chunk += tmp[j];
// 		}
// 		i = j + 2;
// 		int	chunkedSize = hexToInt(chunk);
// 		if (chunkedSize == 0)
// 			break ;
// 		this->_body += tmp.substr(i, chunkedSize);
// 		i += chunkedSize + 1;
// 	}
// }

// // std::string	HttpRequest::_findUploadPath() {
//     // Find upload path logic
// 	// size_t	len = this->_confServ.getLocation().size();
// 	// std::string	download = this->_confServ.uploads; // get the uploadPath in the conf File if there is a section named upload

// 	// std::cout << "Upload Path: " << download << std::endl;

// 	// for(size_t i = 0; i < len; i++) {
// 	// 	if (this->_uri.find(this->_confServ.loc[i].name) != std::string::npos) //find the location Name in the uri
// 	// 		download = this->_confServ.loc[i].uploads; //get the upload Path
// 	// }
// 	// return download;
//     // return std::string();
// // }

// void	HttpRequest::_createFile(const std::string& name, const std::string& reqBody) {
// 	std::ofstream file(name.c_str());

//     if (file) {
//         file << reqBody;
//         file.close();
// 		this->_errorCode = 201; /*201 Created success status response code indicates
// 		that the request has succeeded and has led to the creation of a resource*/
//     } 
//     else {
//         std::cerr << "Failed to create file." << std::endl;
//         this->_errorCode = 500; // Internal Server Error
//     }
// }

// std::string	HttpRequest::_generateTempFileName() {
// 	const char* alphanum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-";
//     const int charsetSize = sizeof(alphanum) - 1;
// 	std::string	tempName;

// 	std::srand(static_cast<unsigned int>(time(NULL)));
//     for (int i = 0; i < 10; ++i) {
// 		int randomIndex = std::rand() % charsetSize;
//         tempName += alphanum[randomIndex];
// 	}
//     return tempName;
// }

// int	HttpRequest::hexToInt(const std::string& str) {
//     int intValue;

//     std::istringstream(str) >> std::hex >> intValue;
//     return intValue;
// }

