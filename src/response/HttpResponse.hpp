#ifndef HTTPRESPONE_HPP
# define HTTPRESPONE_HPP

#include "../Macros.hpp"
#include "../parsing/Config.hpp"
#include "../networking/NetworkClient.hpp"
#include "../networking/HttpRequest.hpp"
#include "../CGI/CGI.hpp"

#define FILE_TYPE 1
#define FOLDER_TYPE 0
#define ERROR 2

class NetworkClient;
class HttpRequest;
class HttpResponse {
	public:
		HttpResponse(NetworkClient &client);
		~HttpResponse();

		void	generateResponse(HttpRequest &req);
		std::string Get_File_Name_From_URI();
		void	buildResponse(int errCode);
		void	locateErrorPage(int errCode);
		void	checkHttpVersion(HttpRequest &req);
		std::string	getContentLength(std::string path);
		std::string	createResponseHeader(int errCode, std::string flag);
		void	findStatusCode(int code);
		std::string	getRequestedResource(HttpRequest &req);
		std::string generateDate();
		std::string deleteRedundantSlash(std::string uri);
		bool	isText() const;
		void	handleGetMethod();
		void	isUrihasSlashInTHeEnd();
		bool	isDirHasIndexFiles();
		
		off_t	getFileSize();
		
		void	handlePostMethod();
		void	processPostMethod();
		bool	isPostDirHasIndexFiles();
		void	_postRequestFolder();
		void	_postRequestFile();

		void	handleDeleteMethod();
		bool checkFilePermission(const std::string& filePath);

	private:
		NetworkClient&	_client;
		ConfigServer	_serv;
		std::string		cookies;
		std::string		_bodyFileName;
		std::string		_postBody;
		int				_errCode;
		std::string		_statusCode;
		bool			_isCgi;
		size_t			_maxBodySize;
		std::string		_root;
		std::string		_uploadPath;
		std::string		_index;
		std::vector<std::string>	_idxFiles;
		std::map<int, std::string>	_errorPage;
		std::string		_errorPath;
		int				_autoindex;
		std::vector<std::string>	_methods;
		std::string		_redirection;
		std::vector<ConfigLocation>	_locations;
		ConfigLocation	_location;
		std::string	_uri;
		int			_fd;
		std::map<std::string, std::string> _headers;
		std::string _filePath;
		std::string _buffer;
		off_t		_fileSize;
		std::string _contentType;
		std::map<std::string, std::string> _reqHeader;
		bool	_isText;
		bool	_slashSetted;
		std::map<std::string, std::string>	_interpreter;
		std::string	_cookie;

		void	_handleDefaultErrors();
		bool	_isSupportedMethod(std::string meth);
		bool	_isSupportedUploadPath();
		std::string	_constructPath(const std::string& requestPath, const std::string &root, const std::string &index);
		int		_checkRequestedType();
		void	_isFile();
		void	_isFolder();
		void	_getAutoIndex();
		// std::string	_findDirectoryName();
		std::string	_generateTempFileName();
		void	_createFile(std::string &filename);
};

std::string getContentType(std::string filename);
std::string	getMimeTypes(std::string flag, std::string extension);
std::string findDirName(const std::string& path, const std::string& root);

#endif