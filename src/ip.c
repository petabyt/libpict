// Windows and POSIX compatible PTP/IP backend
// Copyright 2024 by Daniel C (https://github.com/petabyt/libpict)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#ifdef WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <netinet/tcp.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif
#include <libpict.h>

/// @brief Report progress to the client so it can move a progress bar
__attribute__((weak))
void ptp_report_read_progress(unsigned int size) {}

/// @brief Optional function to assign additional properties to a socket after it's created
/// such as binding to a network, etc
__attribute__((weak))
int ptpip_set_extra_socket_settings(struct PtpRuntime *r, int sockfd) {return 0;}

// Dump all communication to a file
//#define DUMP_COMM

struct PtpIpBackend {
	int fd;
	int evfd;
	int vidfd; // Some cameras have a mjpeg stream
	FILE *dump;
};

static void set_receive_timeout(int fd, int sec) {
	struct timeval tv_rcv;
	tv_rcv.tv_sec = sec;
	tv_rcv.tv_usec = 0;
	int rc = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const void *)&tv_rcv, sizeof(tv_rcv));
	if (rc < 0) {
		ptp_verbose_log("Failed to set rcvtimeo: %d", errno);
	}
}

static int set_nonblocking_io(int fd, int enable) {
#ifdef WIN32
	return 0;
#else
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;

	if (enable) {
		flags |= O_NONBLOCK;
	} else {
		flags &= ~O_NONBLOCK;
	}

	return fcntl(fd, F_SETFL, flags);
#endif
}

static int create_socket(struct PtpRuntime *r, const char *addr, int port, long timeout_sec) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd <= 0) {
		ptp_verbose_log("Bad socket fd: %d %d\n", sockfd, errno);
		return -1;
	}

	int rc = ptpip_set_extra_socket_settings(r, sockfd);
	if (rc) {
		ptp_verbose_log("Error binding to wifi network: %d\n", errno);
		return rc;
	}

	int yes = 1;
	// PTP/IP says that TCP_NODELAY must be set to true
	rc = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (const void *)&yes, sizeof(int));
	if (rc < 0) {
		ptp_verbose_log("Failed to set nodelay: %d\n", errno);
	}

	rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&yes, sizeof(int));
	if (rc < 0) {
		ptp_verbose_log("Failed to set reuseaddr: %d\n", errno);
	}

	if (set_nonblocking_io(sockfd, 1) < 0) {
		close(sockfd);
		ptp_verbose_log("Failed to set non-blocking IO\n");
		return -1;
	}

	ptp_verbose_log("Connecting to %s:%d", addr, port);

	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	if (inet_pton(AF_INET, addr, &(sa.sin_addr)) <= 0) {
		close(sockfd);
		ptp_verbose_log("Failed to convert IP address\n");
		return -1;
	}

	if (connect(sockfd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
		if (errno != EINPROGRESS) {
			close(sockfd);
			ptp_verbose_log("Failed to connect to socket\n");
			return -1;
		}
	}

	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(sockfd, &fdset);
	struct timeval tv;
	tv.tv_sec = timeout_sec;
	tv.tv_usec = 0;

	set_receive_timeout(sockfd, 5);

	// Wait for socket event
	if (errno == EINPROGRESS) {
		rc = select(sockfd + 1, NULL, &fdset, NULL, &tv);
		if (rc != 1) {
			ptp_verbose_log("select() returned 0 fds: %d\n", errno);
			return -1;
		}
	}

	int so_error = 0;
	socklen_t len = sizeof(so_error);
	if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void *)&so_error, &len) < 0) {
		close(sockfd);
		ptp_verbose_log("Failed to get socket options\n");
		return -1;
	}

	if (so_error == 0) {
		ptp_verbose_log("Connection established %s:%d (%d)\n", addr, port, sockfd);
		set_nonblocking_io(sockfd, 0);
		return sockfd;
	}
	if (so_error == 111) {
		ptp_verbose_log("Connection refused - probably invalid IP\n");
	} else {
		ptp_verbose_log("Failed to connect: %d\n", so_error);
	}

	close(sockfd);
	return -1;
}

static struct PtpIpBackend *init_comm(struct PtpRuntime *r) {
	if (r->comm_backend == NULL) {
		r->comm_backend = calloc(1, sizeof(struct PtpIpBackend));

#ifdef DUMP_COMM
#warning "Dumping all comms"
		char filepath[256];
		app_get_file_path(filepath, "dump3.jpeg");
		((struct PtpIpBackend *)r->comm_backend)->dump = fopen(filepath, "wb");
		if (((struct PtpIpBackend *)r->comm_backend)->dump == NULL) abort();
#endif
	}

	// Max packet size for TCP
	r->max_packet_size = 0xffff;

	return (struct PtpIpBackend *)r->comm_backend;
}

int ptpip_connect(struct PtpRuntime *r, const char *addr, int port, int extra_tmout) {
	ptp_verbose_log("Extra tmout: %d\n", extra_tmout);

	int fd = create_socket(r, addr, port, 2 + extra_tmout);

	struct PtpIpBackend *b = init_comm(r);

	if (fd > 0) {
		b->fd = fd;
		r->io_kill_switch = 0;
		r->operation_kill_switch = 0;
		return 0;
	} else {
		b->fd = 0;
		return fd;
	}
}

int ptpip_connect_events(struct PtpRuntime *r, const char *addr, int port) {
	int fd = create_socket(r, addr, port, 3);
	struct PtpIpBackend *b = init_comm(r);
	if (fd > 0) {
		b->evfd = fd;
		return 0;
	} else {
		b->evfd = 0;
		return fd;
	}
}

int ptpip_connect_video(struct PtpRuntime *r, const char *addr, int port) {
	int fd = create_socket(r, addr, port, 3);
	struct PtpIpBackend *b = init_comm(r);
	if (fd > 0) {
		b->vidfd = fd;
		return 0;
	} else {
		b->vidfd = 0;
		return fd;
	}
}

int ptpip_close(struct PtpRuntime *r) {
	struct PtpIpBackend *b = init_comm(r);
	if (b->fd) close(b->fd);
	b->fd = 0;
	if (b->evfd) close(b->evfd);
	b->evfd = 0;
	if (b->vidfd) close(b->vidfd);
	b->vidfd = 0;
	return 0;
}

int ptpip_cmd_write(struct PtpRuntime *r, void *data, unsigned int size) {
	if (r->io_kill_switch) {
		ptp_verbose_log("WARN: kill switch on\n");
		return -1;
	}
	struct PtpIpBackend *b = init_comm(r);

#ifdef DUMP_COMM
	fwrite(data, 1, size, b->dump);
#endif

	int result = (int)send(b->fd, data, size, 0);
	if (result < 0) {
		return -1;
	} else {
		return result;
	}
}

int ptpip_cmd_read(struct PtpRuntime *r, void *data, unsigned int size) {
	if (r->io_kill_switch) return -1;
	struct PtpIpBackend *b = init_comm(r); // slow
	int result = (int)read(b->fd, data, size);

#ifdef DUMP_COMM
	fwrite(data, 1, result, b->dump);
#endif

	if (result < 0) {
		// To match behavior of USB backends, return 0 bytes when there are no bytes available
		if (errno == 11) {
			ptp_verbose_log("resource temp unavailable");
			return 0;
		}
		ptp_verbose_log("read(): %d %d\n", result, errno);
		return -1;
	} else {
		ptp_report_read_progress(result);
		return result;
	}
}

int ptpip_event_send(struct PtpRuntime *r, void *data, unsigned int size) {
	if (r->io_kill_switch) return -1;
	struct PtpIpBackend *b = init_comm(r);
	int result = (int)write(b->evfd, data, size);
	if (result < 0) {
		return -1;
	} else {
		return result;
	}
}

int ptpip_event_read(struct PtpRuntime *r, void *data, unsigned int size) {
	if (r->io_kill_switch) return -1;
	struct PtpIpBackend *b = init_comm(r);
	int result = (int)read(b->evfd, data, size);
	if (result < 0) {
		return -1;
	} else {
		return result;
	}
}
