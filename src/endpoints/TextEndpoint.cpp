#include "endpoints/TextEndpoint.hpp"
#include <fstream>

HttpResponse TextEndpoint::handle()
{
	if (_request.getMethod() == "POST")
	{
		handlePost();
	}
	
	if (_request.getMethod() == "GET")
	{
		handleGet();
	}
	
	if (_request.getMethod() == "DELETE")
	{
		handleDelete();
	}

	if (_response.getStatusCode() == 0)
	{
		_response.setStatusCode(200);
		_response.setStatusMessage("OK");
	}
	return _response;
}

// Retrieves text from file
int TextEndpoint::handleGet()
{
	std::ifstream file("text/text.txt");
	if (!file.is_open())
	{
		_response.setStatusMessage("File does 'text.txt' not exist.");
		_response.setStatusCode(400);
		return 1;
	}
	std::string content((std::istreambuf_iterator<char>(file)),
	std::istreambuf_iterator<char>());

	file.close();
	_response.setBody(content);
	return 0;
}

// Deletes file's content
int TextEndpoint::handleDelete()
{
	std::ofstream file("text/text.txt");
	if (!file.is_open())
	{
		_response.setStatusMessage("File does 'text.txt' not exist.");
		_response.setStatusCode(400);
		return 1;
	}
	file << std::endl;

	file.close();
	return 0;
}

// Puts request body into file
int TextEndpoint::handlePost()
{
	std::ofstream file("text/text.txt");
	if (!file.is_open())
	{
		_response.setStatusMessage("File does 'text.txt' not exist.");
		_response.setStatusCode(400);
		return 1;
	}
	file << _request.getBody() << std::endl;

	file.close();
	return 0;
}


