#include "HttpResponse.hpp"

bool HttpResponse::checkFilePermission(const std::string& filePath) 
{
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) != 0)
        return false;

    if (!S_ISREG(fileStat.st_mode))
        return false;

    if ((fileStat.st_mode & S_IWUSR) == 0)
        return false;

    return true;
}

void HttpResponse::handleDeleteMethod() 
{
    std::string root = _serv.getRoot();
    std::string filePath = _filePath;
    
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) != 0) 
    {
        buildResponse(404);
        return;
    }

    if (S_ISDIR(fileStat.st_mode)) 
    {
        buildResponse(403);
        return;
    }

    if (checkFilePermission(filePath)) 
    {
        if (unlink(filePath.c_str()) == 0) 
            buildResponse(204);
        else 
            buildResponse(403);
    } 
    else
        buildResponse(403);
}
