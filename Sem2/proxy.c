#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <netinet/tcp.h>
#include <time.h>
#define PORT 80
#define BUF_SIZE 65536
#define URL_SIZE 2048
#define CACHE_SIZE 110
#define TTL 300 // Time To Live in seconds

typedef struct {
    char *url;
    char *response;
    size_t response_size;
    time_t timestamp;
    pthread_rwlock_t lock;
	pthread_mutex_t mutex;
	pthread_cond_t condvar;
	int loading;
} CacheEntry;

int first_url_in_cache = 0, first_thread = 0;
pthread_mutex_t first_lock = PTHREAD_MUTEX_INITIALIZER;
CacheEntry cache[CACHE_SIZE];
pthread_rwlock_t cache_lock = PTHREAD_RWLOCK_INITIALIZER;
pthread_cond_t first_url_cv = PTHREAD_COND_INITIALIZER;

void init_cache() {
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache[i].url = NULL;
        cache[i].response = NULL;
        cache[i].response_size = 0;
        cache[i].timestamp = 0;
        pthread_rwlock_init(&cache[i].lock, NULL);
		pthread_mutex_init(&cache[i].mutex,NULL);
		pthread_cond_init(&cache[i].condvar,NULL);
		cache[i].loading = 0;
    }
}

int find_in_cache(const char *url) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].url && strcmp(cache[i].url, url) == 0) {
            return i;
        }
    }
    return -1;
}

int find_free_cache_slot() {
    time_t oldest = time(NULL);
    int oldest_index = -1;

    for (int i = 0; i < CACHE_SIZE; i++) {
        if (!cache[i].url) {
            return i;
        }
        if (cache[i].timestamp < oldest) {
            oldest = cache[i].timestamp;
            oldest_index = i;
        }
    }
    return oldest_index;
}

void store_in_cache(const char *url, const char *response, size_t response_size) {
    pthread_rwlock_rdlock(&cache_lock);
	printf("start store_in_cache\n");
    int idx = find_in_cache(url);
    if (idx == -1) {
        idx = find_free_cache_slot();
        pthread_rwlock_unlock(&cache_lock);
        pthread_rwlock_wrlock(&cache_lock);
        pthread_rwlock_wrlock(&cache[idx].lock);
		printf("found free cache slot\n");

        if (cache[idx].url) {
            free(cache[idx].url);
            cache[idx].url = NULL;
        }
        if (cache[idx].response) {
            free(cache[idx].response);
            cache[idx].response = NULL;
        }

        cache[idx].url = strdup(url);
        pthread_rwlock_unlock(&cache[idx].lock);
        pthread_rwlock_unlock(&cache_lock);
		printf("rewrite url\n");

		pthread_mutex_lock(&first_lock);
		if(first_url_in_cache == 0)
			first_url_in_cache = 1;
		pthread_cond_broadcast(&first_url_cv);
		pthread_mutex_unlock(&first_lock);
    }
    else{
        pthread_rwlock_unlock(&cache_lock);
    }
    //pthread_rwlock_wrlock(&cache_lock);
    pthread_rwlock_wrlock(&cache[idx].lock);

    cache[idx].response = malloc(response_size);
    if (!cache[idx].response) {
        pthread_rwlock_unlock(&cache[idx].lock);
       // pthread_rwlock_unlock(&cache_lock);
        perror("malloc failed");
        return;
    }

    cache[idx].response_size = response_size;
    memcpy(cache[idx].response, response, response_size);
    cache[idx].timestamp = time(NULL);
    pthread_rwlock_unlock(&cache[idx].lock);

    pthread_mutex_lock(&cache[idx].mutex);
    cache[idx].loading = 1;
    pthread_cond_broadcast(&cache[idx].condvar);
    pthread_mutex_unlock(&cache[idx].mutex);
    //pthread_rwlock_unlock(&cache_lock);
	printf("store in cache\n");
}

char *get_from_cache(const char *url, size_t *response_size) {
	pthread_mutex_lock(&first_lock);
	if(first_thread == 1){
		while(first_url_in_cache == 0)
			pthread_cond_wait(&first_url_cv,&first_lock);
	}
	else if(first_thread == 0)
		first_thread = 1;
	pthread_mutex_unlock(&first_lock);

	pthread_rwlock_rdlock(&cache_lock);
    int idx = find_in_cache(url);

    if (idx != -1) {
		pthread_mutex_lock(&cache[idx].mutex);
		while(cache[idx].loading == 0)
			pthread_cond_wait(&cache[idx].condvar, &cache[idx].mutex);
		pthread_mutex_unlock(&cache[idx].mutex);

        pthread_rwlock_rdlock(&cache[idx].lock);

        if (time(NULL) - cache[idx].timestamp < TTL) {
            char *response = malloc(cache[idx].response_size);
            *response_size = cache[idx].response_size;
            if (!response) {
                perror("malloc failed");
                pthread_rwlock_unlock(&cache[idx].lock);
				pthread_rwlock_unlock(&cache_lock);
                return NULL;
            }

            memcpy(response, cache[idx].response, cache[idx].response_size);
            pthread_rwlock_unlock(&cache[idx].lock);
            pthread_rwlock_unlock(&cache_lock);
            return response;
        }
        else{
            pthread_rwlock_unlock(&cache[idx].lock);
            pthread_rwlock_unlock(&cache_lock);
            pthread_rwlock_wrlock(&cache_lock);
            pthread_rwlock_wrlock(&cache[idx].lock);

            cache[idx].response_size = 0;
            cache[idx].timestamp = 0;
			
			pthread_mutex_lock(&cache[idx].mutex);
			cache[idx].loading = 0;
            if (cache[idx].url) {
                free(cache[idx].url);
                cache[idx].url = NULL;
            }
            if (cache[idx].response) {
                free(cache[idx].response);
                cache[idx].response = NULL;
            }
			pthread_mutex_unlock(&cache[idx].mutex);
            pthread_rwlock_unlock(&cache[idx].lock);
            pthread_rwlock_unlock(&cache_lock);
        }

    }
    else{
        pthread_rwlock_unlock(&cache_lock);
    }
	printf("not found in cache\n");
    return NULL;
}


int get_host(const char* request, char* buf) {
    const char* host_start = strstr(request, "Host: ");
    if (!host_start) {
        fprintf(stderr, "Host header not found in request\n");
        return -1;
    }

    host_start += 6; // Пропускаем "Host: "
    const char* host_end = strchr(host_start, '\r');
    if (!host_end) {
        fprintf(stderr, "Invalid Host header format\n");
        return -1;
    }
    // Проверяем наличие порта
    const char* colon = strchr(host_start, ':');
    if (colon && colon < host_end) {
        host_end = colon;
    }

    size_t host_len = host_end - host_start;
    strncpy(buf, host_start, host_len);
    buf[host_len] = '\0'; // Добавляем null-терминатор
    return 0;
}

// Резолвим днс хоста
int resolve_host(const char* host, struct sockaddr_in* server_addr) {
    struct hostent *he;
    printf("Resolving host: %s\n", host);

    if ((he = gethostbyname(host)) == NULL) {
        herror("gethostbyname failed");
        return -1;
    }

    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    server_addr->sin_addr = *((struct in_addr *)he->h_addr_list[0]);
    return 0;
}

int read_info(int clt_sock, char** data, long* data_size){
    char buf[BUF_SIZE];
    long read_bytes;
	printf("read data\n");
    while (1) {
        read_bytes = read(clt_sock, buf, BUF_SIZE);
        if (read_bytes == 0) {
            if (*data == NULL) {
                fprintf(stderr, "No data received from client\n");
                return -1;
            }
            break;
        }
		else if(read_bytes < 0){
            perror("read failed");
            free(*data);
            *data = NULL;
            return -1;
		}

        char* temp = realloc(*data, *data_size + read_bytes);
        if (!temp) {
            perror("realloc failed");
            return -1;
        }

        *data = temp;
        memcpy(*data + *data_size, buf, read_bytes);
        *data_size += read_bytes;

        if (strstr(*data, "\r\n\r\n")) {
            break;
        }

    }
    return 0;
}

void transfer_data(int src_sock, int dest_sock, char* url) {
    char buf[BUF_SIZE]; // Фиксированный размер буфера
    long bytes_read;
    size_t total_size = 0;
    char *response = NULL;
	printf("start transfer data\n");
    while ((bytes_read = read(src_sock, buf, sizeof(buf))) > 0) {
        long bytes_written = 0;
        while (bytes_written < bytes_read) {
            long result = write(dest_sock, buf + bytes_written, bytes_read - bytes_written);
            if (result < 0) {
                perror("Write failed");
                if(response)
                    free(response);
                return;
            }

            bytes_written += result;
        }

        char* temp = realloc(response, total_size + bytes_read);
        if (!temp) {
            perror("realloc failed");
            free(response);
            return;
        }
        response = temp;

        memcpy(response + total_size, buf, bytes_read);
        total_size += bytes_read;
    }
    if (response) {
        store_in_cache(url, response, total_size);
        free(response);
    }

    if (bytes_read == 0) {
        printf("Connection closed by peer\n");
    } else if (bytes_read < 0) {
        perror("Read failed");
    }
}

int extract_url(const char* request, char* url, size_t url_size) {
    const char* first_space = strchr(request, ' ');
    if (!first_space) {
        fprintf(stderr, "Invalid HTTP request format: no first space found\n");
        return -1;
    }

    const char* second_space = strchr(first_space + 1, ' ');
    if (!second_space) {
        fprintf(stderr, "Invalid HTTP request format: no second space found\n");
        return -1;
    }

    size_t length = second_space - (first_space + 1);
    if (length >= url_size) {
        fprintf(stderr, "URL is too long to fit in the buffer\n");
        return -1;
    }

    strncpy(url, first_space + 1, length);
    url[length] = '\0'; // Null-terminate the URL string
    return 0;
}

void* thread_func(void* arg){
    int clt_sock = *(int*)arg;
    free(arg);
    long data_size = 0;
    char* data = NULL;

    //unsigned int timeout = 1800000;
    //setsockopt(clt_sock, IPPROTO_TCP, TCP_USER_TIMEOUT, &timeout, sizeof(timeout));

    if(read_info(clt_sock, &data,&data_size)){
        close(clt_sock);
        free(data);
        return NULL;
    }
	printf("readed info\n");
    //printf("Request from client:\n%s\n", data);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    char host[_SC_HOST_NAME_MAX + 1];

    if(get_host(data, host)) {
        fprintf(stderr, "Host not found in request\n");
        close(clt_sock);
        free(data);
        return NULL;
    }

    // Резолвим dns
    if (resolve_host(host, &server_addr) < 0) {
        fprintf(stderr, "Failed to resolve host: %s\n", host);
        close(clt_sock);
        free(data);
        return NULL;
    }

    size_t response_size;
    char url[URL_SIZE];
    if(extract_url(data,url, URL_SIZE)) {
        close(clt_sock);
        free(data);
        return NULL;
    }

    char* response = get_from_cache(url, &response_size);
    if(response){
        ssize_t bytes_written = write(clt_sock, response, response_size);
        if (bytes_written < 0) {
            perror("Write to server failed");
        }
        else{
            printf("%s found in cache\n", host);
        }
        free(data);
        free(response);
        close(clt_sock);
        return NULL;
    }

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Server socket creation failed");
        close(clt_sock);
        free(data);
        return NULL;
    }

    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to target server failed");
        close(server_socket);
        close(clt_sock);
        free(data);
        return NULL;
    }

    // Отправка запроса на целевой сервер
    int byte_written = write(server_socket, data, data_size);
    if (byte_written < 0) {
        perror("Write to server failed");
        free(data);
        close(server_socket);
        close(clt_sock);
        return NULL;
    }
    // Чтение ответа от сервера и отправка обратно клиенту
    transfer_data(server_socket, clt_sock, url);
    printf("response has been received\n");
    // Закрытие соединений

    close(server_socket);
    close(clt_sock);
    free(data);
    return NULL;
}

int main(void) {
    struct sockaddr_in srv_sockaddr;
    struct sockaddr_in clt_sockaddr;
    init_cache();

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    memset(&srv_sockaddr, 0, sizeof(struct sockaddr_in));
    srv_sockaddr.sin_family = AF_INET;
    srv_sockaddr.sin_addr.s_addr = INADDR_ANY;
    srv_sockaddr.sin_port = htons(PORT);
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        close(server_sock);
        return -1;
    }

    int err = bind(server_sock, (struct sockaddr *) &srv_sockaddr, sizeof(srv_sockaddr));
    if(err == -1){
        perror("bind failed: ");
        close(server_sock);
        return -1;
    }
    err = listen(server_sock,100);
    if(err == -1){
        perror("listen error: ");
        close(server_sock);
        return -1;
    }
    printf("HTTP proxy listening on port 80\n");

    unsigned int len = 0;
    pthread_t thread;
    while(1){
        memset(&clt_sockaddr, 0, sizeof(struct sockaddr_in));
        int clt_sock = accept(server_sock, (struct sockaddr *) &clt_sockaddr,&len);
        if(clt_sock == -1){
            perror("accept failed: ");
            close(server_sock);
            continue;
        }
        int *clt_sock_ptr = malloc(sizeof(int));
		if(!clt_sock_ptr){
			perror("malloc failed");
			close(server_sock);
			continue;
		}
        *clt_sock_ptr = clt_sock;

        err = pthread_create(&thread, NULL, thread_func,(void*)clt_sock_ptr);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            close(clt_sock);
            close(server_sock);
            free(clt_sock_ptr);
            continue;
        }

        pthread_detach(thread);
    }

    return 0;
}
