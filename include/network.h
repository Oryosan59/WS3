#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h> // For size_t

#define DEFAULT_RECV_PORT 12345
#define DEFAULT_SEND_PORT 12346
#define NET_BUFFER_SIZE 1024

// ネットワーク通信の状態を保持する構造体
typedef struct {
    int recv_socket;
    int send_socket;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr_recv; // 受信元クライアントアドレス
    struct sockaddr_in client_addr_send; // 送信先クライアントアドレス
    socklen_t client_addr_len;
    bool client_addr_known;
} NetworkContext;

// 関数のプロトタイプ宣言
bool network_init(NetworkContext* ctx, int recv_port, int send_port);
void network_close(NetworkContext* ctx);
ssize_t network_receive(NetworkContext* ctx, char* buffer, size_t buffer_size);
bool network_send(NetworkContext* ctx, const char* data, size_t data_len);
bool network_update_send_address(NetworkContext* ctx); // 受信アドレスを送信アドレスに設定するヘルパー

#endif // NETWORK_H
