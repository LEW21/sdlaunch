#ifndef HostPort_h
#define HostPort_h

#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <stdexcept>

struct HostPort
{
	uint16_t ip[8];
	uint16_t port;

	inline HostPort(const std::string& uri);
};

inline bool isIPv4HostPort(const std::string& uri)
{
	for (char c : uri)
	{
		if (!((c >= '0' && c <= '9') || c == '.' || c == ':'))
		{
			return false;
		}
	}
	return true;
}

inline HostPort::HostPort(const std::string& uri)
{
	::memset(this, 0, sizeof(HostPort));

	uint i = 0;
	char tmp[uri.size() + 1];

	if (uri[0] == '[')
	{
		++i;

		for (; i < uri.size() && uri[i] != ']'; ++i)
			tmp[i - 1] = uri[i];

		tmp[i - 1] = 0;

		if (uri[i] != ']')
			throw std::runtime_error("Invalid IPv6.");

		++i;

		int ret = inet_pton(AF_INET6, tmp, ip);
		if (ret == 0)
			throw std::runtime_error("Invalid IPv6.");
		else if (ret == -1)
			throw std::runtime_error("No IPv6 support.");
	}
	else if (isIPv4HostPort(uri))
	{
		for (; i < uri.size() && uri[i] != ':'; ++i)
			tmp[i] = uri[i];

		tmp[i] = 0;

		uint16_t ipv4[2];
		int ret = inet_pton(AF_INET, tmp, ipv4);
		if (ret == 0)
			throw std::runtime_error("Invalid IPv4.");
		else if (ret == -1)
			throw std::runtime_error("No IPv4 support.");

		ip[5] = 0xffff;
		ip[6] = ipv4[0];
		ip[7] = ipv4[1];
	}
	else
	{
		// TODO hostname
		throw std::runtime_error("Hostnames are not implemented yet.");
	}

	if (uri[i] != ':')
		throw std::runtime_error("No port number provided.");

	++i;
	port = htons(atoi(&uri.data()[i]));
}

#endif
