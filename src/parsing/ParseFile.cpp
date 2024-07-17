#include "ParseFile.hpp"

ParseFile::ParseFile() {}
ParseFile::ParseFile(ParseFile const &src) {
	*this = src;
}
ParseFile&	ParseFile::operator=(ParseFile const &src) {
	(void)src;
	return *this;
}

ParseFile::~ParseFile() {}

void	trim(std::string& line) {
	std::size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos)
		line.erase(0, pos);
	pos = line.find_last_not_of(" \t");
	if (pos != std::string::npos)
		line.erase(pos + 1);
	
}

std::vector<t_tokens>	ParseFile::readFile(const char *fileName)
{
	std::ifstream file(fileName);

    if (!file.is_open())
		throw ParseFile::ParseFileException("Exception: could not open configuration file");

    std::string line;
    tokenVector tokenVect;
	t_tokens tok;

	while (getline(file, line)) {
		std::size_t start = line.find_first_not_of(" \t");
		if (start != std::string::npos)
			line = line.erase(0, start);
		if (line[0] == '#') {
			continue ;
		}
		else if (line.find_first_of('#') != std::string::npos)
			throw ParseFile::ParseFileException("Error: Unexpected token.");
        if (!line.empty()) {
			tok = ParseFile::setToken(line);
			if (tok._type == "server" && line.find('{') == std::string::npos) {
				tokenVect.push_back(tok);
				getline(file, line);
				if (line.find('{') == std::string::npos) {
					throw ParseFile::ParseFileException("Error: expected '{' after server directive.");
				}
				tok = ParseFile::setToken(line);
			}
            tokenVect.push_back(tok);
        }
	}
	file.close();
	return tokenVect;
}

t_tokens ParseFile::setToken(std::string& line)
{
    t_tokens    token;
    std::size_t start = 0;
	bool	curly_brace = false;

	while (start < line.size() && (line[start] != ' ' && line[start] != '\t' && line[start] != '{'))
		start++;
	if (start < line.size())
	{
		token._type = line.substr(0, start);
		token._value = line.substr(start + 1);
		std::size_t	pos = token._value.find(';');
		if (pos != std::string::npos)
			token._value = token._value.erase(pos, 1);

		pos = token._value.find('{');
		if (pos != std::string::npos) {
			token._value = token._value.erase(pos, 1);
			curly_brace = true;
		}
		trim(token._value);
		trim(token._type);
		if ((token._type == "server" || token._type == "location") && !curly_brace)
			throw ParseFile::ParseFileException("Error: expected '{' after server/location directive.");
	}
	else {
		token._type = line;
		token._value = "";
	}
	return (token);
}
