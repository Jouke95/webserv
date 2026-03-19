#ifndef WEBSERV_ENDPOINTBASECLASS_HPP
#define WEBSERV_ENDPOINTBASECLASS_HPP

class AEndpoint
{
private:
	
public:
	AEndpoint();
	~AEndpoint();
	AEndpoint(const AEndpoint& other) = delete;
	const AEndpoint& operator=(const AEndpoint& other) = delete;
};

#endif