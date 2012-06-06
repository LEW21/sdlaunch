#include "config.h"

#include <boost/program_options.hpp>
#include <iostream>
#include "itoa.h"
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
//#include <systemd/sd-daemon.h>
#define SD_LISTEN_FDS_START 3
#include <unistd.h>

#ifdef SDLAUNCH_SOCKET_UNIX
#include <sys/un.h>
#endif

#ifdef SDLAUNCH_SOCKET_IP
#include <netinet/in.h>
#include "HostPort.h"
#endif

namespace po = boost::program_options;

int newServer(const std::string& uri)
{
	int domain = 0;
	sockaddr* address = 0;
	socklen_t addressSize = 0;

	union
	{
#ifdef SDLAUNCH_SOCKET_UNIX
		::sockaddr_un sun;
#endif
#ifdef SDLAUNCH_SOCKET_IP
		::sockaddr_in6 sin6;
#endif
	};

	if (uri[0] == '/' || uri[0] == '.')
	{
#ifdef SDLAUNCH_SOCKET_UNIX
		::memset(&sun, 0, sizeof(::sockaddr_un));

		sun.sun_family = AF_UNIX;

		if (sizeof(sun.sun_path) < (uint) uri.size() + 1)
			throw std::runtime_error("Too long path.");

		::memcpy(sun.sun_path, uri.data(), uri.size());

		domain = PF_UNIX;
		address = (::sockaddr*) &sun;
		addressSize = sizeof(::sockaddr_un);
#else
		throw std::runtime_error("UNIX sockets were disabled during compilation.");
#endif
	}
	else
	{
#ifdef SDLAUNCH_SOCKET_IP
		::memset(&sin6, 0, sizeof(::sockaddr_in6));

		sin6.sin6_family = AF_INET6;

		HostPort hp(uri);

		::memcpy(&sin6.sin6_addr, hp.ip, 16);
		sin6.sin6_port = hp.port;

		domain = PF_INET6;
		address = (::sockaddr*) &sin6;
		addressSize = sizeof(::sockaddr_in6);
#else
		throw std::runtime_error("IP sockets were disabled during compilation.");
#endif
	}

	int serverSocket = ::socket(domain, SOCK_STREAM, 0);
	if (serverSocket == -1)
		throw std::runtime_error("Can't open socket.");

	int error = 0;

	error = ::bind(serverSocket, address, addressSize);
	if (error)
		throw std::runtime_error("Can't bind.");

	error = ::listen(serverSocket, 16);
	if (error)
		throw std::runtime_error("Can't listen.");

	return serverSocket;
}

void setupFDs(const std::vector<std::string>& uris)
{
	for (uint i = 0; i < uris.size(); ++i)
	{
		int fd = newServer(uris[i]);
		if (dup2(fd, SD_LISTEN_FDS_START + i) < 0)
			perror("sdlaunch");
	}

	setenv("LISTEN_PID", itoa(getpid()), 1);
	setenv("LISTEN_FDS", itoa(uris.size()), 1);
}

void execute(const std::vector<std::string>& args)
{
	char** argv = new char*[args.size() + 1];

	for (uint i = 0; i < args.size(); ++i)
		argv[i] = strdup(args[i].c_str());

	argv[args.size()] = 0;

	execvp(argv[0], argv);
}

int main(int argc, char** argv)
{
	std::vector<std::string> uris;
	std::vector<std::string> args;

	po::options_description desc("Options");
	desc.add_options()
		("help,h", "produce help message")
		("bind,b", po::value<std::vector<std::string>>(&uris), "bind location (local path or ip:port)")
	;

	po::options_description hidden("Hidden options");
	hidden.add_options()
		("args", po::value<std::vector<std::string>>(&args))
	;

	po::options_description cmdline_options;
	cmdline_options.add(desc).add(hidden);

	po::positional_options_description p;
	p.add("args", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
			options(cmdline_options).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << "Usage: " << argv[0] << " [options] [command] [arg]+" << std::endl;
		std::cout << desc << std::endl;
		return 1;
	}

	setupFDs(uris);
	execute(args);

	perror("sdlaunch");
	return 1;
}
