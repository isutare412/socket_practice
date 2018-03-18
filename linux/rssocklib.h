#define LISTEN_QUEUE_SIZE 10

// 1. make socket
// 2. set sockopt to reuse time-waiting port
// 3. bind "port" to socket
// 4. return socket file descriptor
int rsserv_sockbind(short port);

// 1. accept client from "sock"
// 2. leave connection log to stdout
// 2. return accepted client socket file descriptor
int rsserv_acceptlog(int sock);

// print error from errno then exit with EXIT_FAILURE
void handle_error(const char* msg);
