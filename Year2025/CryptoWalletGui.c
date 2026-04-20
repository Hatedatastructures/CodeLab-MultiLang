//加密货币和区块链，未完成：成熟的ui，p2p网络
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <wincrypt.h>
#include <commctrl.h>
#include <time.h>
#include <assert.h>
#pragma comment(lib, "comctl32.lib")
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#include <uxtheme.h>
#include <winuser.h>
#pragma comment(lib, "uxtheme.lib")

// 自定义消息定义
#define EM_SETBKGNDCOLOR (WM_USER + 1)
#define EM_SETTEXTCOLOR (WM_USER + 2)
#define GWL_USERDATA (-21)

// 定义常量
#define BLOCK_SIZE 1024
#define DIFFICULTY 4
#define REWARD 50
// 窗口控件ID
#define IDC_GENERATE_WALLET 101
#define IDC_SEND_TRANSACTION 102
#define IDC_VIEW_BALANCE 103
#define IDC_ADDRESS 104
#define IDC_BALANCE 105
#define IDC_RECEIVER_ADDRESS 106
#define IDC_AMOUNT 107
#define IDC_FEE 108
#define IDC_QUERY_ADDRESS 109

// 数据结构定义
typedef struct Transaction
{
    char sender[64];
    char receiver[64];
    double amount;
    double fee;
    char signature[256];
    long timestamp;
} Transaction;

typedef struct WalletBalance
{
    char address[64];
    double balance;
    double pending_balance;
} WalletBalance;

typedef struct Block
{
    int index;
    char previous_hash[65];
    Transaction transactions[10];
    int transaction_count;
    long timestamp;
    int nonce;
    char hash[65];
} Block;

typedef struct Blockchain
{
    Block* chain;
    int length;
    int capacity;
} Blockchain;

// 钱包结构
typedef struct Wallet
{
    BCRYPT_KEY_HANDLE key_pair;
    char address[64];
} Wallet;

// 节点信息
typedef struct NodeInfo
{
    char ip[16];
    int port;
} NodeInfo;

// 节点列表
typedef struct NodeList
{
    NodeInfo* nodes;
    int count;
    int capacity;
} NodeList;

// 网络节点结构
typedef struct Node
{
    char ip[16];
    int port;
    Blockchain blockchain;
    SOCKET socket;
    struct Node* next;
    HANDLE thread;
    CRITICAL_SECTION lock;
    NodeList node_list;  // 添加节点列表
} Node;

// 在主窗口过程中添加全局变量
static Wallet g_wallet;  // 全局钱包对象
static Node* g_node;     // 全局节点对象
// 网络消息类型
typedef enum
{
    MSG_BLOCK,           // 区块消息
    MSG_TRANSACTION,     // 交易消息
    MSG_PING,            // 心跳检测
    MSG_PONG,            // 心跳响应
    MSG_NODE_LIST,       // 节点列表
    MSG_CHAIN_REQUEST,   // 区块链请求
    MSG_CHAIN_RESPONSE,  // 区块链响应
    MSG_NODE_DISCOVERY,  // 节点发现
    MSG_SYNC_REQUEST,    // 同步请求
    MSG_SYNC_RESPONSE    // 同步响应
} MessageType;

// 网络消息头
typedef struct
{
    MessageType type;
    int length;
} MessageHeader;

// 网络消息
typedef struct
{
    MessageHeader header;
    void* data;
} Message;

// 函数声明
// 函数声明
void generate_key_pair(Wallet* wallet);
void generate_address(Wallet* wallet);
void test_transaction_signing();
void init_blockchain(Blockchain* chain);
void free_blockchain(Blockchain* chain);
void blockchain_add_block(Blockchain* chain, Block new_block);
void init_node(Node* node, const char* ip, int port);
void free_node(Node* node);
void generate_key_pair(Wallet* wallet);
void generate_address(Wallet* wallet);
void sign_transaction(Transaction* tx, BCRYPT_KEY_HANDLE key);
int verify_transaction(Transaction* tx);
void calculate_hash(char* data, char* output_hash);
Block create_genesis_block();
int is_chain_valid(Blockchain* chain);
void mine_block(Block* block);
void add_block(Blockchain* chain, Block new_block);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void cleanup_resources();
void test_transaction_signing();
void broadcast_transaction(Node* node, Transaction* tx);
void request_blockchain_sync(Node* node);
int is_block_valid(Block* block);

void init_blockchain(Blockchain* chain)
{
    chain->length = 0;
    chain->capacity = 10;
    chain->chain = (Block*)malloc(sizeof(Block) * chain->capacity);
}

void free_blockchain(Blockchain* chain)
{
    free(chain->chain);
    chain->chain = NULL;
    chain->length = 0;
    chain->capacity = 0;
}

void blockchain_add_block(Blockchain* chain, Block new_block)
{
    if (chain->length >= chain->capacity) 
    {
        chain->capacity *= 2;
        chain->chain = (Block*)realloc(chain->chain, sizeof(Block) * chain->capacity);
    }
    chain->chain[chain->length++] = new_block;
}
void add_block(Blockchain* chain, Block new_block)
{
    // 验证新区块
    if (!is_block_valid(&new_block)) 
    {
        printf("无效区块，无法添加\n");
        return;
    }

    // 检查新区块是否链接到最后一个区块
    if (chain->length > 0) 
    {
        Block* last_block = &chain->chain[chain->length - 1];
        if (strcmp(new_block.previous_hash, last_block->hash) != 0) 
        {
            printf("新区块未正确链接到最后一个区块\n");
            return;
        }
    }

    // 添加新区块
    if (chain->length >= chain->capacity) 
    {
        chain->capacity *= 2;
        chain->chain = (Block*)realloc(chain->chain, sizeof(Block) * chain->capacity);
        if (!chain->chain) 
        {
            printf("内存分配失败\n");
            return;
        }
    }

    chain->chain[chain->length++] = new_block;
    printf("成功添加新区块 #%d\n", new_block.index);
}
void init_node(Node* node, const char* ip, int port)
{
    strncpy(node->ip, ip, sizeof(node->ip) - 1);
    node->port = port;
    init_blockchain(&node->blockchain);
    node->socket = INVALID_SOCKET;
    node->next = NULL;
    node->thread = NULL;
    InitializeCriticalSection(&node->lock);
}

void free_node(Node* node)
{
    if (node->socket != INVALID_SOCKET) 
    {
        closesocket(node->socket);
    }
    if (node->thread) 
    {
        CloseHandle(node->thread);
    }
    free_blockchain(&node->blockchain);
    DeleteCriticalSection(&node->lock);
}

// 节点列表初始化
void init_node_list(NodeList* list)
{
    list->count = 0;
    list->capacity = 10;
    list->nodes = (NodeInfo*)malloc(sizeof(NodeInfo) * list->capacity);
}

// 添加节点
void add_node(NodeList* list, const char* ip, int port)
{
    if (list->count >= list->capacity) 
    {
        list->capacity *= 2;
        list->nodes = (NodeInfo*)realloc(list->nodes, sizeof(NodeInfo) * list->capacity);
    }
    strncpy(list->nodes[list->count].ip, ip, sizeof(list->nodes[list->count].ip) - 1);
    list->nodes[list->count].port = port;
    list->count++;
}

// 查找节点
int find_node(NodeList* list, const char* ip, int port)
{
    for (int i = 0; i < list->count; i++) 
    {
        if (strcmp(list->nodes[i].ip, ip) == 0 && list->nodes[i].port == port) 
        {
            return i;
        }
    }
    return -1;
}

// 删除节点
void remove_node(NodeList* list, int index)
{
    if (index < 0 || index >= list->count) 
    {
        return;
    }
    memmove(&list->nodes[index], &list->nodes[index + 1],(list->count - index - 1) * sizeof(NodeInfo));
    list->count--;
}

// 清理节点列表
void free_node_list(NodeList* list)
{
    free(list->nodes);
    list->nodes = NULL;
    list->count = 0;
    list->capacity = 0;
}

// 发送消息
void send_message(Node* node, Message* msg)
{
    if (node == NULL || msg == NULL) 
    {
        printf("节点或消息为空\n");
        return;
    }

    if (node->socket == INVALID_SOCKET) 
    {
        printf("节点套接字无效\n");
        return;
    }
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    MessageHeader* header = &msg->header;
    memcpy(buffer, header, sizeof(MessageHeader));
    if (header->length > 0 && msg->data != NULL) 
    {
        memcpy(buffer + sizeof(MessageHeader), msg->data, header->length);
    }
    int sent = send(node->socket, buffer, sizeof(MessageHeader) + header->length, 0);
    if (sent < 0) 
    {
        printf("发送消息失败\n");
        closesocket(node->socket);
        node->socket = INVALID_SOCKET;
    } 
    else 
    {
        printf("消息发送成功\n");
    }
}
// 节点发现相关函数
void start_node_discovery(Node* node)
{
    if (node == NULL)
    {
        printf("节点为空，无法启动发现\n");
        return;
    }
    Message discovery_msg;
    discovery_msg.header.type = MSG_NODE_DISCOVERY;
    discovery_msg.header.length = sizeof(NodeInfo);
    // 创建自身节点信息
    NodeInfo self_info;
    strncpy(self_info.ip, node->ip, sizeof(self_info.ip));
    self_info.port = node->port;
    discovery_msg.data = &self_info;
    // 向已知节点广播发现消息
    Node* current = node;
    while (current != NULL)
    {
        if (current->socket != INVALID_SOCKET)
        {
            send_message(current, &discovery_msg);
            printf("向节点 %s:%d 发送发现消息\n", current->ip, current->port);
        }
        current = current->next;
    }
    // 设置定时器，每30秒执行一次发现
    SetTimer(NULL, 1, 30000, (TIMERPROC)start_node_discovery);
}

void handle_node_discovery(Node* node, Message* msg)
{
    if (node == NULL || msg == NULL)
    {
        printf("节点或消息为空\n");
        return;
    }
    NodeInfo* info = (NodeInfo*)msg->data;
    if (info == NULL)
    {
        printf("节点信息为空\n");
        return;
    }

    // 检查是否为新节点
    if (find_node(&node->node_list, info->ip, info->port) == -1)
    {
        // 添加新节点到列表
        add_node(&node->node_list, info->ip, info->port);
        printf("发现新节点: %s:%d\n", info->ip, info->port);
        // 请求区块链同步
        request_blockchain_sync(node);
        // 创建自身节点信息
        NodeInfo self_info;
        strncpy(self_info.ip, node->ip, sizeof(self_info.ip));
        self_info.port = node->port;
        // 发送响应消息
        Message response_msg;
        response_msg.header.type = MSG_NODE_DISCOVERY;
        response_msg.header.length = sizeof(NodeInfo);
        response_msg.data = &self_info;
        send_message(node, &response_msg);
    }
}

// 区块广播相关函数
void broadcast_block(Node* node, Block* block)
{
    if (node == NULL || block == NULL)
    {
        printf("节点或区块为空\n");
        return;
    }
    // 验证区块有效性
    if (!is_block_valid(block))
    {
        printf("区块无效，无法广播\n");
        return;
    }
    // 创建区块消息
    Message block_msg;
    block_msg.header.type = MSG_BLOCK;
    block_msg.header.length = sizeof(Block);
    block_msg.data = block;
    // 向所有已知节点广播
    Node* current = node;
    while (current != NULL)
    {
        if (current->socket != INVALID_SOCKET)
        {
            send_message(current, &block_msg);
            printf("广播区块 %d 到节点 %s:%d\n", block->index, current->ip, current->port);
        }
        current = current->next;
    }
    // 将区块添加到本地链
    add_block(&node->blockchain, *block);
}

// 交易广播相关函数 
void broadcast_transaction(Node* node, Transaction* tx)
{
    Message tx_msg;
    tx_msg.header.type = MSG_TRANSACTION;
    tx_msg.header.length = sizeof(Transaction);
    tx_msg.data = tx;
    Node* current = node;
    while (current != NULL)
    {
        send_message(current, &tx_msg);
        current = current->next;
    }
}

// 网络同步相关函数
void request_blockchain_sync(Node* node)
{
    if (node == NULL)
    {
        printf("节点为空，无法请求同步\n");
        return;
    }
    // 创建同步请求消息
    Message sync_msg;
    sync_msg.header.type = MSG_SYNC_REQUEST;
    sync_msg.header.length = 0;
    // 向所有已知节点发送同步请求
    Node* current = node;
    while (current != NULL)
    {
        if (current->socket != INVALID_SOCKET)
        {
            send_message(current, &sync_msg);
            printf("向节点 %s:%d 发送同步请求\n", current->ip, current->port);
        }
        current = current->next;
    }
    // 设置定时器，每60秒执行一次同步
    SetTimer(NULL, 2, 60000, (TIMERPROC)request_blockchain_sync);
}

void handle_sync_request(Node* node, Message* msg)
{
    if (node == NULL || msg == NULL)
    {
        printf("节点或消息为空\n");
        return;
    }
    // 创建同步响应消息
    Message sync_response;
    sync_response.header.type = MSG_SYNC_RESPONSE;
    sync_response.header.length = sizeof(Blockchain);
    sync_response.data = &node->blockchain;
    // 发送响应
    send_message(node, &sync_response);
    printf("处理来自 %s:%d 的同步请求\n", node->ip, node->port);
}

void handle_sync_response(Node* node, Message* msg)
{
    if (node == NULL || msg == NULL)
    {
        printf("节点或消息为空\n");
        return;
    }
    Blockchain* remote_chain = (Blockchain*)msg->data;
    if (remote_chain == NULL)
    {
        printf("远程链为空\n");
        return;
    }
    // 验证远程链有效性
    if (is_chain_valid(remote_chain))
    {
        // 检查链长度
        if (remote_chain->length > node->blockchain.length)
        {
            // 释放当前链
            free_blockchain(&node->blockchain);  
            // 复制远程链
            node->blockchain = *remote_chain;
            printf("从 %s:%d 同步区块链成功，当前高度：%d\n", node->ip, node->port, node->blockchain.length);
        }
    }
    else
    {
        printf("从 %s:%d 接收的区块链无效\n",node->ip, node->port);
    }
}

// 交易签名
void sign_transaction(Transaction* tx, BCRYPT_KEY_HANDLE key)
{
    char data[1024];
    snprintf(data, sizeof(data), "%s%s%.2f%.2f%ld",tx->sender,tx->receiver,tx->amount,tx->fee,tx->timestamp);
    BCRYPT_HASH_HANDLE hHash = NULL;
    BYTE pbHash[32];
    DWORD cbHash = sizeof(pbHash);
    NTSTATUS status;
    BCRYPT_ALG_HANDLE hSha256Alg = NULL;
    status = BCryptOpenAlgorithmProvider(&hSha256Alg,BCRYPT_SHA256_ALGORITHM,NULL,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("打开SHA256算法提供程序失败\n");
        return;
    }
    status = BCryptCreateHash(hSha256Alg,&hHash,NULL,0,NULL,0,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("创建哈希对象失败\n");
        return;
    }
    status = BCryptHashData(hHash,(PUCHAR)data,(ULONG)strlen(data),0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("哈希计算失败\n");
        BCryptDestroyHash(hHash);
        return;
    }
    status = BCryptFinishHash(hHash,pbHash,cbHash,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("完成哈希计算失败\n");
        BCryptDestroyHash(hHash);
        return;
    }
    BYTE pbSignature[256];
    DWORD cbSignature = sizeof(pbSignature);
    status = BCryptSignHash(key, NULL, pbHash, cbHash, pbSignature, sizeof(pbSignature), &cbSignature, 0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("签名失败\n");
        BCryptDestroyHash(hHash);
        return;
    }
    DWORD cbBase64 = 0;
    CryptBinaryToStringA(pbSignature, cbSignature,CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,NULL, &cbBase64);
    char* pszBase64 = (char*)malloc(cbBase64);
    if (!pszBase64)
    {
        printf("内存分配失败\n");
        BCryptDestroyHash(hHash);
        return;
    }
    CryptBinaryToStringA(pbSignature, cbSignature,CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,pszBase64, &cbBase64);
    strncpy(tx->signature, pszBase64, sizeof(tx->signature) - 1);
    free(pszBase64);
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hSha256Alg, 0);
}

// 交易验证
int verify_transaction(Transaction* tx)
{
    char data[1024];
    snprintf(data, sizeof(data), "%s%s%.2f%.2f%ld",tx->sender,tx->receiver,tx->amount,tx->fee,tx->timestamp);
    BCRYPT_HASH_HANDLE hHash = NULL;
    BYTE pbHash[32];
    DWORD cbHash = sizeof(pbHash);
    NTSTATUS status;
    BCRYPT_ALG_HANDLE hSha256Alg = NULL;
    status = BCryptOpenAlgorithmProvider(&hSha256Alg,BCRYPT_SHA256_ALGORITHM,NULL,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("打开SHA256算法提供程序失败\n");
        return 0;
    }
    status = BCryptCreateHash(hSha256Alg,&hHash,NULL,0,NULL,0,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("创建哈希对象失败\n");
        return 0;
    }
    status = BCryptHashData(hHash,(PUCHAR)data,(ULONG)strlen(data),0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("哈希计算失败\n");
        BCryptDestroyHash(hHash);
        return 0;
    }
    status = BCryptFinishHash(hHash,pbHash,cbHash,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("完成哈希计算失败\n");
        BCryptDestroyHash(hHash);
        return 0;
    }
    BYTE pbSignature[256];
    DWORD cbSignature = sizeof(pbSignature);
    if (!CryptStringToBinaryA(tx->signature, 0,CRYPT_STRING_BASE64,pbSignature, &cbSignature,NULL, NULL))
    {
        printf("签名解码失败\n");
        BCryptDestroyHash(hHash);
        return 0;
    }
    BCRYPT_KEY_HANDLE hPublicKey = NULL;
    status = BCryptImportKeyPair(hSha256Alg,NULL,BCRYPT_RSAPUBLIC_BLOB,&hPublicKey,(PUCHAR)tx->sender,(ULONG)strlen(tx->sender),0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("导入公钥失败\n");
        BCryptDestroyHash(hHash);
        return 0;
    }
    status = BCryptVerifySignature(hPublicKey,NULL,pbHash,cbHash,pbSignature,cbSignature,0);
    BCryptDestroyKey(hPublicKey);
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hSha256Alg, 0);
    return BCRYPT_SUCCESS(status);
}

void calculate_hash(char* data, char* output_hash)
{
    char hash[65] = { 0 };
    BCRYPT_HASH_HANDLE hHash = NULL;
    BYTE pbHash[32];
    DWORD cbHash = sizeof(pbHash);
    NTSTATUS status;
    BCRYPT_ALG_HANDLE hSha256Alg = NULL;
    status = BCryptOpenAlgorithmProvider(&hSha256Alg,BCRYPT_SHA256_ALGORITHM,NULL,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("打开SHA256算法提供程序失败\n");
        return;
    }
    status = BCryptCreateHash(hSha256Alg,&hHash,NULL,0,NULL,0,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("创建哈希对象失败\n");
        return;
    }
    status = BCryptHashData(hHash,(PUCHAR)data,(ULONG)strlen(data),0);

    if (!BCRYPT_SUCCESS(status))
    {
        printf("哈希计算失败\n");
        BCryptDestroyHash(hHash);
        return;
    }
    status = BCryptFinishHash(hHash,pbHash,cbHash,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("完成哈希计算失败\n");
        BCryptDestroyHash(hHash);
        return;
    }
    for (int i = 0; i < cbHash; i++)
    {
        sprintf(&hash[i * 2], "%02x", pbHash[i]);
    }
    strncpy(output_hash, hash, 64);
    output_hash[64] = '\0';
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hSha256Alg, 0);
}

Block create_genesis_block()
{
    Block genesis_block;
    memset(&genesis_block, 0, sizeof(genesis_block));
    genesis_block.index = 0;
    strcpy(genesis_block.previous_hash, "0");
    genesis_block.timestamp = time(NULL);
    genesis_block.nonce = 0;
    char data[1024];
    snprintf(data, sizeof(data), "%d%s%ld%d",genesis_block.index,genesis_block.previous_hash,genesis_block.timestamp,genesis_block.nonce);
    calculate_hash(data, genesis_block.hash);
    return genesis_block;
}

// 验证区块链有效性
int is_chain_valid(Blockchain* chain)
{
    if (chain == NULL || chain->length == 0)
    {
        return 0;  // 空链或无效链
    }
    // 验证创世区块
    if (chain->length == 1)
    {
        return 1;  // 只有创世区块，认为是有效的
    }
    // 验证其他区块
    for (int i = 1; i < chain->length; i++)
    {
        Block* current_block = &chain->chain[i];
        Block* previous_block = &chain->chain[i - 1];
        // 验证区块索引
        if (current_block->index != previous_block->index + 1)
        {
            return 0;
        }
        // 验证前一个区块的哈希值
        if (strcmp(current_block->previous_hash, previous_block->hash) != 0)
        {
            return 0;
        }
        // 验证当前区块的哈希值
        char data[1024];
        snprintf(data, sizeof(data), "%d%s%ld%d",current_block->index,current_block->previous_hash,current_block->timestamp,current_block->nonce);
        char calculated_hash[65];
        calculate_hash(data, calculated_hash);
        if (strcmp(calculated_hash, current_block->hash) != 0)
        {
            return 0;
        }
        // 验证交易
        for (int j = 0; j < current_block->transaction_count; j++)
        {
            if (!verify_transaction(&current_block->transactions[j]))
            {
                return 0;
            }
        }
    }
    return 1;
}

void mine_block(Block* block)
{
    char target[DIFFICULTY + 1];
    memset(target, '0', DIFFICULTY);
    target[DIFFICULTY] = '\0';
    while (1)
    {
        char data[1024];
        snprintf(data, sizeof(data), "%d%s%ld%d",block->index,block->previous_hash,block->timestamp,block->nonce);
        char hash[65];
        calculate_hash(data, hash);
        if (strncmp(hash, target, DIFFICULTY) == 0)
        {
            strcpy(block->hash, hash);
            break;
        }
        block->nonce++;
    }
}
void cleanup_resources()
{
    // 清理节点资源
    if (g_node)
    {
        free_node(g_node);
        free(g_node);
        g_node = NULL;
    }
    // 清理钱包资源
    if (g_wallet.key_pair)
    {
        BCryptDestroyKey(g_wallet.key_pair);
        g_wallet.key_pair = NULL;
    }
}

// 窗口初始化
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "CryptoWalletClass";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassEx(&wcex))
    {
        return FALSE;
    }
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HWND hWnd = CreateWindowW(L"CryptoWalletClass",L"加密货币钱包",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, 0,800, 600,NULL,NULL,hInstance,NULL);
    HFONT hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL,FALSE, FALSE, FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE,L"微软雅黑");
    if (hFont)
    {
        SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
    }
    if (!hWnd)
    {
        return FALSE;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(1), RT_RCDATA);
    if (hRes)
    {
        HGLOBAL hData = LoadResource(hInstance, hRes);
        if (hData)
        {
            LPVOID pData = LockResource(hData);
            DWORD dwSize = SizeofResource(hInstance, hRes);
            SetWindowTheme(hWnd, L"DarkMode_Explorer", NULL);
            SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(30, 30, 46)));
            COLORREF darkGray = RGB(40, 40, 40);
            COLORREF lightBlue = RGB(100, 149, 237);
            COLORREF lightGreen = RGB(144, 238, 144);
            COLORREF lightPurple = RGB(147, 112, 219);
            COLORREF white = RGB(255, 255, 255);
            HWND hChild = NULL;
            while ((hChild = FindWindowEx(hWnd, hChild, NULL, NULL)) != NULL)
            {
                if (GetWindowLongPtr(hChild, GWL_STYLE) & BS_PUSHBUTTON)
                {
                    SetWindowTheme(hChild, L"DarkMode_Explorer", NULL);
                    SendMessage(hChild, BM_SETSTYLE, BS_FLAT, TRUE);
                    SendMessage(hChild, BM_SETIMAGE, IMAGE_ICON,(LPARAM)NULL);
                }
                if (GetWindowLongPtr(hChild, GWL_STYLE) & ES_AUTOHSCROLL)
                {
                    SetWindowTheme(hChild, L"DarkMode_Explorer", NULL);
                    SendMessage(hChild, WM_CTLCOLOREDIT,
                    (WPARAM)GetDC(hChild),
                    (LPARAM)CreateSolidBrush(RGB(40, 40, 40)));
                    SendMessage(hChild, WM_SETFONT, (WPARAM)hFont, TRUE);
                    SetWindowLongPtr(hChild, GWL_EXSTYLE,
                    GetWindowLongPtr(hChild, GWL_EXSTYLE) | WS_EX_CLIENTEDGE);
                    SetWindowLongPtr(hChild, GWL_STYLE,
                    GetWindowLongPtr(hChild, GWL_STYLE) | ES_WANTRETURN);
                    SendMessage(hChild, EM_SETBKGNDCOLOR, 0, RGB(40, 40, 40));
                    SendMessage(hChild, EM_SETTEXTCOLOR, 0, RGB(255, 255, 255));
                    SendMessage(hChild, EM_SETSEL, 0, -1);
                }
            }
        }
    }
    g_node = (Node*)malloc(sizeof(Node));
    if (g_node) 
    {
        init_node(g_node, "127.0.0.1", 8080);
    }

    return TRUE;
}
// 窗口过程
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        CreateWindowW(L"BUTTON", L"钱包操作",WS_VISIBLE | WS_CHILD | BS_GROUPBOX,10, 10, 780, 140,hWnd, NULL, NULL, NULL);
        CreateWindowW(L"BUTTON", L"交易操作",WS_VISIBLE | WS_CHILD | BS_GROUPBOX,10, 160, 780, 120,hWnd, NULL, NULL, NULL);
        CreateWindowW(L"BUTTON", L"查询操作",WS_VISIBLE | WS_CHILD | BS_GROUPBOX,10, 290, 780, 60,hWnd, NULL, NULL, NULL);
        CreateWindowW(L"BUTTON", L"生成钱包",WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,20, 30, 120, 30,hWnd, (HMENU)IDC_GENERATE_WALLET, NULL, NULL);
        CreateWindowW(L"BUTTON", L"发送交易",WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,20, 180, 120, 30,hWnd, (HMENU)IDC_SEND_TRANSACTION, NULL, NULL);
        CreateWindowW(L"BUTTON", L"查看余额",WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,20, 310, 120, 30,hWnd, (HMENU)IDC_VIEW_BALANCE, NULL, NULL);
        CreateWindowW(L"STATIC", L"地址:",WS_VISIBLE | WS_CHILD | SS_RIGHT,160, 35, 50, 20,hWnd, NULL, NULL, NULL);
        CreateWindowW(L"STATIC", L"余额:",WS_VISIBLE | WS_CHILD | SS_RIGHT,160, 75, 50, 20,hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"",WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL,220, 30, 400, 25,hWnd, (HMENU)IDC_ADDRESS, NULL, NULL);
        CreateWindowW(L"EDIT", L"",WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL,220, 70, 100, 25,hWnd, (HMENU)IDC_BALANCE, NULL, NULL);
        CreateWindowW(L"STATIC", L"接收地址:",WS_VISIBLE | WS_CHILD | SS_RIGHT,20, 200, 80, 20,hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"",WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,110, 195, 300, 25,hWnd, (HMENU)IDC_RECEIVER_ADDRESS, NULL, NULL);
        CreateWindowW(L"STATIC", L"金额:",WS_VISIBLE | WS_CHILD | SS_RIGHT,20, 230, 50, 20,hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"",WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,110, 225, 100, 25,hWnd, (HMENU)IDC_AMOUNT, NULL, NULL);
        CreateWindowW(L"STATIC", L"手续费:",WS_VISIBLE | WS_CHILD | SS_RIGHT,20, 260, 50, 20,hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"",WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,110, 255, 100, 25,hWnd, (HMENU)IDC_FEE, NULL, NULL);
        CreateWindowW(L"STATIC", L"查询地址:",WS_VISIBLE | WS_CHILD | SS_RIGHT,20, 320, 80, 20,hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"",WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,110, 315, 300, 25,hWnd, (HMENU)IDC_QUERY_ADDRESS, NULL, NULL);
        HFONT hFont = CreateFontW(14, 0, 0, 0, FW_NORMAL,FALSE, FALSE, FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE,L"微软雅黑");
        if (hFont)
        {
            SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
            HWND hChild = NULL;
            while ((hChild = FindWindowEx(hWnd, hChild, NULL, NULL)) != NULL)
            {
                SendMessage(hChild, WM_SETFONT, (WPARAM)hFont, TRUE);
                if (GetWindowLongPtr(hChild, GWL_STYLE) & BS_PUSHBUTTON)
                {
                    int btnId = GetWindowLongPtr(hChild, GWL_ID);
                    COLORREF btnColor = RGB(100, 149, 237);
                    if (btnId == IDC_SEND_TRANSACTION) 
                    {
                        btnColor = RGB(144, 238, 144);
                    }
                    else if (btnId == IDC_VIEW_BALANCE) 
                    {
                        btnColor = RGB(147, 112, 219);
                    }
                    SetWindowLongPtr(hChild, GWL_STYLE,
                    GetWindowLongPtr(hChild, GWL_STYLE) | BS_FLAT);
                    SetWindowRgn(hChild, CreateRoundRectRgn(0, 0, 120, 30, 8, 8), TRUE);
                    SetWindowLongPtr(hChild, GWL_USERDATA, (LONG_PTR)btnColor);
                }
            }
        }
    }
    break;
    case WM_COMMAND:
    {
        if (HIWORD(wParam) == EN_SETFOCUS) 
        {
            HWND hEdit = (HWND)lParam;
            SendMessage(hEdit, EM_SETBKGNDCOLOR, 0, RGB(60, 60, 60));
            SendMessage(hEdit, EM_SETTEXTCOLOR, 0, RGB(255, 255, 255));
        }
        else if (HIWORD(wParam) == EN_KILLFOCUS) 
        {
            HWND hEdit = (HWND)lParam;
            SendMessage(hEdit, EM_SETBKGNDCOLOR, 0, RGB(40, 40, 40));
            SendMessage(hEdit, EM_SETTEXTCOLOR, 0, RGB(255, 255, 255));
        }
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDC_GENERATE_WALLET:
        {
            Wallet wallet;
            generate_key_pair(&wallet);
            generate_address(&wallet);
            SetWindowTextW(GetDlgItem(hWnd, IDC_ADDRESS), (LPCWSTR)wallet.address);
            SetWindowTextW(GetDlgItem(hWnd, IDC_BALANCE), L"0.0");
            Blockchain blockchain;
            blockchain.length = 1;
            blockchain.chain = (Block*)malloc(sizeof(Block));
            blockchain.chain[0] = create_genesis_block();
            wprintf(L"新钱包创建成功\n地址: %s\n", wallet.address);
        }
        break;
        case IDC_SEND_TRANSACTION:
        {
            wchar_t receiver[64];
            wchar_t amount[32];
            wchar_t fee[32];
            GetWindowTextW(GetDlgItem(hWnd, IDC_RECEIVER_ADDRESS), receiver, 64);
            GetWindowTextW(GetDlgItem(hWnd, IDC_AMOUNT), amount, 32);
            GetWindowTextW(GetDlgItem(hWnd, IDC_FEE), fee, 32);
            Transaction tx;
            wcstombs(tx.receiver, receiver, 64);
            tx.amount = _wtof(amount);
            tx.fee = _wtof(fee);
            tx.timestamp = time(NULL);
            wchar_t sender[64];
            GetWindowTextW(GetDlgItem(hWnd, IDC_ADDRESS), sender, 64);
            wcstombs(tx.sender, sender, 64);
            sign_transaction(&tx, g_wallet.key_pair);
            broadcast_transaction(g_node, &tx);
            wprintf(L"交易已创建\n接收者: %s\n金额: %.2f\n手续费: %.2f\n",receiver, tx.amount, tx.fee);
        }
        break;
        case IDC_VIEW_BALANCE:
        {
            wchar_t address[64];
            GetWindowTextW(GetDlgItem(hWnd, IDC_QUERY_ADDRESS), address, 64);
            // 查询余额逻辑
            wchar_t balance[32];
            swprintf(balance, 32, L"%.2f", 0.0);
            SetWindowTextW(GetDlgItem(hWnd, IDC_BALANCE), balance);
            wprintf(L"地址 %s 的余额为: %.2f\n", address, 0.0);
        }
        break;
        }
    }
    break;
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
        if (lpDrawItem->CtlType == ODT_BUTTON)
        {
            int btnId = lpDrawItem->CtlID;
            COLORREF btnColor = RGB(100, 149, 237);
            if (btnId == IDC_SEND_TRANSACTION) 
            {
                btnColor = RGB(144, 238, 144);
            }
            else if (btnId == IDC_VIEW_BALANCE) 
            {
                btnColor = RGB(147, 112, 219);
            }
            HBRUSH hBrush = CreateSolidBrush(btnColor);
            FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, hBrush);
            DeleteObject(hBrush);
            char btnText[32];
            GetWindowTextA(lpDrawItem->hwndItem, btnText, 32);
            DrawTextA(lpDrawItem->hDC, btnText, -1, &lpDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            if (lpDrawItem->itemState & ODS_SELECTED) 
            {
                DrawEdge(lpDrawItem->hDC, &lpDrawItem->rcItem, EDGE_SUNKEN, BF_RECT);
            }
            else 
            {
                DrawEdge(lpDrawItem->hDC, &lpDrawItem->rcItem, EDGE_RAISED, BF_RECT);
            }
        }
        return TRUE;
    }
    break;
    case WM_CTLCOLOREDIT:
    {
        HDC hdc = (HDC)wParam;
        SetBkColor(hdc, RGB(40, 40, 40));
        SetTextColor(hdc, RGB(255, 255, 255));
        return (LRESULT)CreateSolidBrush(RGB(40, 40, 40));
    }
    break;
    case WM_DESTROY:
        cleanup_resources();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
// 主函数
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icc);
    // 设置控制台编码和字体
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // 设置控制台字体为支持中文的字体
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 16;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
    if (!InitInstance(hInstance, nCmdShow))
    {
        wprintf(L"主窗口初始化失败\n");
        return FALSE;
    }
    wprintf(L"程序已启动，等待用户操作...\n");
    test_transaction_signing();
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

// 测试交易签名和验证
void test_transaction_signing()
{
    printf("\n测试交易签名和验证...\n");
    Wallet wallet;
    generate_key_pair(&wallet);
    generate_address(&wallet);
    Transaction tx;
    strcpy(tx.sender, wallet.address);
    strcpy(tx.receiver, "receiver_address");
    tx.amount = 1.0;
    tx.fee = 0.01;
    tx.timestamp = time(NULL);
    sign_transaction(&tx, wallet.key_pair);
    printf("交易签名成功\n");
    if (verify_transaction(&tx))
    {
        printf("交易验证成功\n");
    }
    else
    {
        printf("交易验证失败\n");
    }
    tx.amount = 2.0;
    if (!verify_transaction(&tx))
    {
        printf("篡改检测成功\n");
    }
    else
    {
        printf("篡改检测失败\n");
    }
}

// 生成密钥对
void generate_key_pair(Wallet* wallet)
{
    BCRYPT_ALG_HANDLE hAlgorithm = NULL;
    BCRYPT_KEY_HANDLE hKey = NULL;
    NTSTATUS status;
    status = BCryptOpenAlgorithmProvider(&hAlgorithm,BCRYPT_RSA_ALGORITHM,NULL,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("打开算法提供程序失败\n");
        return;
    }
    status = BCryptGenerateKeyPair(hAlgorithm,&hKey,2048,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("生成密钥对失败\n");
        BCryptCloseAlgorithmProvider(hAlgorithm, 0);
        return;
    }
    status = BCryptFinalizeKeyPair(hKey, 0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("完成密钥生成失败\n");
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlgorithm, 0);
        return;
    }
    wallet->key_pair = hKey;
    BCryptCloseAlgorithmProvider(hAlgorithm, 0);
}

// 生成地址
void generate_address(Wallet* wallet)
{
    BYTE pbPublicKey[512];
    ULONG cbPublicKey;
    NTSTATUS status;
    status = BCryptExportKey(wallet->key_pair,NULL,BCRYPT_RSAPUBLIC_BLOB,pbPublicKey,sizeof(pbPublicKey),&cbPublicKey,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("导出公钥失败\n");
        return;
    }
    BCRYPT_HASH_HANDLE hHash = NULL;
    BYTE pbHash[32];
    DWORD cbHash = sizeof(pbHash);
    BCRYPT_ALG_HANDLE hSha256Alg = NULL;
    status = BCryptOpenAlgorithmProvider(&hSha256Alg,BCRYPT_SHA256_ALGORITHM,NULL,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("打开SHA256算法提供程序失败\n");
        return;
    }
    status = BCryptCreateHash(hSha256Alg,&hHash,NULL,0,NULL,0,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("创建哈希对象失败\n");
        return;
    }
    status = BCryptHashData(hHash,pbPublicKey,cbPublicKey,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("哈希计算失败\n");
        BCryptDestroyHash(hHash);
        return;
    }
    status = BCryptFinishHash(hHash,pbHash,cbHash,0);
    if (!BCRYPT_SUCCESS(status))
    {
        printf("完成哈希计算失败\n");
        BCryptDestroyHash(hHash);
        return;
    }
    char address[64] = { 0 };
    const char base58_chars[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    unsigned char temp[32] = { 0 };
    unsigned char result[64] = { 0 };
    int result_len = 0;
    temp[0] = 0x00;
    memcpy(temp + 1, pbHash, 20);
    int zeros = 0;
    while (zeros < 21 && temp[zeros] == 0)
    {
        zeros++;
    }
    int length = 21 - zeros;
    unsigned char buffer[21 * 138 / 100 + 1] = { 0 };
    int buffer_len = 0;
    for (int i = zeros; i < 21; i++) 
    {
        int carry = temp[i];
        int j = 0;
        for (int k = buffer_len - 1; k >= 0; k--, j++) 
        {
            carry += 256 * buffer[k];
            buffer[k] = carry % 58;
            carry /= 58;
        }
        while (carry > 0) 
        {
            buffer[buffer_len++] = carry % 58;
            carry /= 58;
        }
    }
    for (int i = 0; i < buffer_len; i++) 
    {
        result[result_len++] = base58_chars[buffer[buffer_len - 1 - i]];
    }
    for (int i = 0; i < zeros; i++)
    {
        result[result_len++] = '1';
    }
    result[result_len] = '\0';
    strncpy(wallet->address, result, sizeof(wallet->address) - 1);
    BCryptDestroyHash(hHash);
}

int is_block_valid(Block* block) 
{
    // 验证区块索引
    if (block->index < 0) 
    {
        return 0;
    }
    // 验证时间戳
    if (block->timestamp > time(NULL)) 
    {
        return 0;
    }
    // 验证哈希值
    char data[1024];
    snprintf(data, sizeof(data), "%d%s%ld%d",block->index,block->previous_hash,block->timestamp,block->nonce);
    char calculated_hash[65];
    calculate_hash(data, calculated_hash);
    if (strcmp(calculated_hash, block->hash) != 0) 
    {
        return 0;
    }

    // 验证交易
    for (int i = 0; i < block->transaction_count; i++) 
    {
        if (!verify_transaction(&block->transactions[i])) 
        {
            return 0;
        }
    }

    return 1;
}
