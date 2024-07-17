// #include "WebServer.hpp"

// void WebServer::sendResponse(HttpRequest &req, NetworkClient &client) 
// { 
//     HttpResponse resp(client);
//     resp.generateResponse(req);

//     if (!client.getHeaderSent()) 
// 	{
//         client.setResponse(client.getResponseHeader());
//         client.setHeaderSent(true);
//         std::cout << client.getResponse();
//     }
// 	else 
// 	{
//         if (!client.getResponseBody().empty()) 
// 		{
//             char buffer[1024] = {0};
//             if (!client.getOpenFile()) 
// 			{
//                 client._file.open(client.getResponseBody().c_str(), std::ios::in | std::ios::binary);
//                 if (!client._file.is_open()) 
// 				{
//                     std::cerr << "Failed to open file: " << client.getResponseBody() << std::endl;
//                     closeClient(client.fetchConnectionSocket());
//                     return;
//                 }
//                 client.setOpenFile(true);
//             }
//             if (client._file.good()) 
// 			{
//                 client._file.read(buffer, sizeof(buffer));
//                 std::size_t bytesRead = static_cast<std::size_t>(client._file.gcount());
//                 std::cout << "BytesRead: " << bytesRead << "\n";
//                 client.setResponse(std::string(buffer, bytesRead));
//             } 
// 			else 
// 			{
//                 std::cerr << "error read: " << client.getResponseBody() << std::endl;
//                 closeClient(client.fetchConnectionSocket());
//                 return;
//             }
//         }
//     }

//     std::cout << "Outside SockCl: " << client.fetchConnectionSocket() << "\n";
//     ssize_t bytesSent = send(client.fetchConnectionSocket(), client.getResponse().c_str(), client.getResponse().length(), 0);
//     std::cout << "BytesSent: " << bytesSent << "\n";
//     if (bytesSent <= 0)
//         closeClient(client.fetchConnectionSocket());
// }

#include "WebServer.hpp"

void WebServer::sendResponse(HttpRequest &req, NetworkClient &client) 
{ 
    HttpResponse resp(client);
    resp.generateResponse(req);

    if (!client.getHeaderSent()) 
	{
        client.setResponse(client.getResponseHeader());
        client.setHeaderSent(true);
        // std::cout << client.getResponse();
    }
	else 
	{
        if (!client.getResponseBody().empty())
		{
            char buffer[1024];
            if (resp.isText() == true) {
                //std::cout << "ewew"<<client.getResponseBody() << std::endl;
                ssize_t bytesSent = send(client.fetchConnectionSocket(), client.getResponseBody().c_str(), client.getResponseBody().length(), 0);
				if (bytesSent < 0 || bytesSent == (int)client.getResponseBody().length()) {
                    std::remove(req.get_bodyFileName().c_str());
					closeClient(client.fetchConnectionSocket());
				}
				return;
            } 
            else if (!client.getOpenFile())
			{
                // client._file.open(client.getResponseBody().c_str(), std::ios::in | std::ios::binary);
				client.openFileForReading();
                if (!client.getOpenFile())
				{
                    std::cerr << "Failed to open file: " << client.getResponseBody() << std::endl;
                    closeClient(client.fetchConnectionSocket());
                    return;
                }
                // client.setOpenFile(true);
            }
            if (client._file.good())
			{
                client.readFromFile(buffer, 1024);
                std::size_t bytesRead = static_cast<std::size_t>(client.bytesRead());
                // std::cout << "BytesRead: " << bytesRead << "\n";
                client.setResponse(std::string(buffer, bytesRead));
            }
			else 
			{
                // std::cout << "IN ELSE, SockCli: " << client.fetchConnectionSocket() << "\n";
				// std::cerr << "Nothing left to read " << client.getResponseBody() << std::endl;
                closeClient(client.fetchConnectionSocket());
                return;
            }
        }
        else {
            closeClient(client.fetchConnectionSocket());
            return;
        }
    }

    ssize_t bytesSent = send(client.fetchConnectionSocket(), client.getResponse().c_str(), client.getResponse().length(), 0);
    if (bytesSent <= 0) {
        closeClient(client.fetchConnectionSocket());
    }
}

// // #include "WebServer.hpp"
// // void WebServer::sendResponse(HttpRequest &req, NetworkClient &client)
// // {	
// // 	HttpResponse resp(client);
// // 	resp.generateResponse(req);
// // 	if (client.getHeaderSent() == false)
// // 	{
// // 		client.setResponse(client.getResponseHeader());
// // 		client.setHeaderSent(true);
// // 		// std::cout << client.getResponse();
// // 	}
// // 	else
// // 	{
// // 		// std::cout << "Body: " << client.getResponseBody() << "\n";
// // 		if (client.getResponseBody().length() > 0)
// // 		{
// // 			char buffer[1024];
// // 			if (client.getOpenFile() == false)
// // 			{
// // 				client._file.open(client.getResponseBody().c_str(), std::ios::in | std::ios::binary);
// // 				client.setOpenFile(true);
// // 			}
// // 			if (client._file.good())
// // 			{
// // 				client._file.read(buffer, 1024);
// // 				client.bytes_read = client._file.gcount();
// //                 std::size_t bytesToRead = static_cast<std::size_t>(client.bytes_read);
// // 				// std::cout << "BYtesRead: " << bytesToRead << "\n";
// // 				client.setResponse(std::string(buffer, bytesToRead));
// // 			}
// // 			else
// // 			{
// // 				// std::cout << "IN ELSE, SockCli: " << client.fetchConnectionSocket() << "\n";
// // 				ssize_t bytesSent = send(client.fetchConnectionSocket(), client.getResponseBody().c_str(), client.getResponseBody().length(), 0);
// // 				if (bytesSent < 0 || bytesSent == (int)client.getResponseBody().length()) {

// // 					closeClient(client.fetchConnectionSocket());
// // 				}
// // 				return;
// // 			}
// // 		}
// // 	}
// // 	// std::cout << "Outside SockCl: " <<client.fetchConnectionSocket() << "\n";
// // 	ssize_t bytesSent = 0;
// //     bytesSent = send(client.fetchConnectionSocket(), client.getResponse().c_str(), client.getResponse().length(), 0);
// // 	// std::cout << "BytesSent: " << bytesSent << "\n";
// //      if (bytesSent <= 0 ) {
// // 		closeClient(client.fetchConnectionSocket());
// // 	}
// // }

// // void WebServer::sendResponse(HttpRequest &req, NetworkClient &client)
// // {	
// // 	HttpResponse resp(client);
// // 	resp.generateResponse(req);
// // 	if (client.getHeaderSent() == false)
// // 	{
// // 		client.setResponse(client.getResponseHeader());
// // 		client.setHeaderSent(true);
// // 		std::cout << client.getResponse();

// // 	}
// // 	else
// // 	{
// // 		// std::cout << "Body: " << client.getResponseBody() << "\n";
// // 		if (client.getResponseBody().length() > 0)
// // 		{
// // 			char buffer[1024] = {0};
// // 			if (client.getOpenFile() == false)
// // 			{
// //                 client.openFileForReading();
// //                 if (client.getOpenFile() && client._file.good()) {
// //                     client.readFromFile(buffer, 1024);
// //                     std::size_t bytesToRead = static_cast<std::size_t>(client.bytesRead());
// //                     // std::cout << "BYtesRead: " << bytesToRead << "\n";
// //                     client.setResponse(std::string(buffer, bytesToRead));
// //                 }
// // 				else if (!client.getOpenFile()) {
// // 					if (resp.isText() == true) {
// // 						ssize_t bytesSent = send(client.fetchConnectionSocket(), client.getResponseBody().c_str(), client.getResponseBody().length(), 0);
// // 						if (bytesSent < 0 || bytesSent == (int)client.getResponseBody().length()) {
// // 							closeClient(client.fetchConnectionSocket());
// // 						}
// // 						return;
// // 					}
// // 						std::cerr << "Failed to open file: " << client.getResponseBody() << std::endl;
// // 						closeClient(client.fetchConnectionSocket());
// // 						return;
// // 				}
// //             }
// // 		}
// // 	}
// // 	// std::cout << "Outside SockCl: " <<client.fetchConnectionSocket() << "\n";
// // 	ssize_t bytesSent = 0;
// //     bytesSent = send(client.fetchConnectionSocket(), client.getResponse().c_str(), client.getResponse().length(), 0);
// // 	// std::cout << "BytesSent: " << bytesSent << "\n";
// //      if (bytesSent <= 0 ) {
// // 		closeClient(client.fetchConnectionSocket());
// // 	}
// // }


