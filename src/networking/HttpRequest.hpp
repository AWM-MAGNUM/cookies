#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../Macros.hpp"
#include "../parsing/ConfigServer.hpp"

class HttpRequest
{
	public:
	enum REQUEST_STATE
	{
		HEADERS,
		BODY,
		REQUEST_READY	
	};

	enum BODY_STATE
	{
		CHUNKED,
		CONTENT_LENGTH,
		NONE,
	};

	private:
		std::string _httpMethod;
		std::string _httpVersion;
		std::string _uri;
		std::map<std::string, std::string> _headerFields;
		std::string bodyFileName;
		std::string queryString;
		REQUEST_STATE request_status;
		BODY_STATE body_status;
		std::string requestData;
		int _errorCode;
		size_t _bodySize;
		std::string	_cookie;

		void	_parseURI();

	public:
		HttpRequest();
		HttpRequest(const HttpRequest&);
		HttpRequest& operator=(const HttpRequest&);
		~HttpRequest();
		void parseHttpRequest(std::string&);

		/*setters*/
		void	setRequest(std::string&);
		void	setMethod(const std::string& method);
		void	setUri(const std::string& uri);
		void	setHttpVersion(const std::string& version);
		bool	setBody(std::string &body);
		void	setHeaderField(std::string &headers);

		void set_bodyStatus(BODY_STATE);
		void set_requestStatus(REQUEST_STATE);
		void setRequestData(std::string&);

		/*getters*/
		std::string	getRequest() const;
		std::string	getMethod() const;
		std::string	&getUri() ;
		std::string	getHttpVersion() const;
		std::string	getHeader(const std::string& headerName) const;
		std::map<std::string, std::string>& getHeaderFields();
		size_t&		getBodysize();
		int			getErrorCode() const;
		std::string &getCookie();
		
		std::string& get_queryString();
		std::string& get_bodyFileName();
		int get_bodyStatus();
		int get_requestStatus();

		void printHeaders();
		void _getChunkedBody(std::string&);
		std::string& getRequestData();

		bool is_body();
};

int	hexToInt(const std::string& str);
std::string trimHeader(const std::string& str);

#endif
