# include "../headers/libunixsocket.h"
# include "../headers++/unixbase.hpp"
# include "../headers++/socket.hpp"
# include "../headers++/exception.hpp"
# include <string>
# include <string.h>

# include <unistd.h>
# include <sys/socket.h>
# include <sys/types.h>

# define READ  1
# define WRITE 2

namespace libsocket
{
	using std::string;

	class unix_stream_client : public unix_socket
	{
		private:
		string peer_path;

		public:

		unix_stream_client(void);
		unix_stream_client(const char* path, int socket_flags=0);
# if __cplusplus == 201103L
		unix_stream_client(const string& path, int socket_flags=0);
# endif
		void connect(const char* path, int socket_flags=0);
		void connect(const string& path, int socket_flags=0);

		void shutdown(int method=WRITE);

		ssize_t snd(const void* buf, size_t buflen, int send_flags=0);
		ssize_t rcv(void* buf, size_t len, int recv_flags=0);

		friend unix_stream_client& operator<<(unix_stream_client& sock,const char* data);
		friend unix_stream_client& operator<<(unix_stream_client& sock,string& data);
	};

	unix_stream_client::unix_stream_client(void) : peer_path("") {}

	unix_stream_client::unix_stream_client(const char* path, int socket_flags)
	{
		sfd = create_unix_stream_socket(path,socket_flags);

		if ( sfd < 0 )
			throw socket_exception(__FILE__,__LINE__,"unix_stream_client::unix_stream_client: Could not create and connect UNIX socket!\n");
	}

# if __cplusplus == 201103L
	unix_stream_client::unix_stream_client(const string& path, int socket_flags)
		: unix_stream_client(path.c_str(),socket_flags) {}
# endif

	void unix_stream_client::connect(const char* path, int socket_flags)
	{
		sfd = create_unix_stream_socket(path,socket_flags);

		if ( sfd < 0 )
			throw socket_exception(__FILE__,__LINE__,"unix_stream_client::unix_stream_client: Could not create and connect UNIX socket!\n");
	}

	void unix_stream_client::connect(const string& path, int socket_flags)
	{
		sfd = create_unix_stream_socket(path.c_str(),socket_flags);

		if ( sfd < 0 )
			throw socket_exception(__FILE__,__LINE__,"unix_stream_client::unix_stream_client: Could not create and connect UNIX socket!\n");
	}

	void unix_stream_client::shutdown(int method)
	{
		int ret = shutdown_unix_stream_socket(sfd,method);

		if ( ret < 0 )
			throw socket_exception(__FILE__,__LINE__,"unix_stream_client::shutdown: Could not shut the socket down!\n");
	}

	/* *********** */

	ssize_t unix_stream_client::snd(const void* buf, size_t buflen, int send_flags)
	{
		if ( buf == NULL )
			throw socket_exception(__FILE__,__LINE__,"unix_stream_client::snd: Buffer pointer is NULL!\n");

		if ( buflen == 0 )
			throw socket_exception(__FILE__,__LINE__,"unix_stream_client::snd: Buffer length is 0\n");

		ssize_t retval;

		retval = send(sfd,buf,buflen,send_flags);

		if ( retval < 0 )
			throw socket_exception(__FILE__,__LINE__,"unix_stream_client::snd: Error while sending!\n");

		return retval;
	}

	ssize_t unix_stream_client::rcv(void* buf, size_t buflen, int rcv_flags)
	{
		if ( buf == NULL )
			throw socket_exception(__FILE__,__LINE__,"unix_stream_client::rcv: Buffer pointer is NULL!\n");
		if ( buflen == 0 )
			throw socket_exception(__FILE__,__LINE__,"unix_stream_client::rcv: Buffer length is null!\n");

		ssize_t retval;

		if ( 0 < (retval = recv(sfd,buf,buflen,rcv_flags)))
				throw socket_exception(__FILE__,__LINE__,"unix_stream_client::rcv: recv() failed!\n");

		return retval;
	}

	unix_stream_client& operator<<(unix_stream_client& sock,const char* data)
	{
		sock.snd(data,strlen(data),0);

		return sock;
	}

	unix_stream_client& operator<<(unix_stream_client& sock,string& data)
	{
		sock.snd(data.c_str(),data.size(),0);

		return sock;
	}
}
