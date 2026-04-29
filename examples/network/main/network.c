// VMU Pro Network Example
//
// Fetches a page from the internet using BSD sockets and displays
// status info on the screen. Assumes WiFi is already connected.

#include "vmupro_sdk.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static const char *TAG = "[NetExample]";

#define HOST "example.com"
#define PORT "80"
#define HTTP_PATH "/"

#define RX_BUF_SIZE 4096

// HTTP/1.0 keeps things simple — server closes when done, no chunked encoding
static const char *HTTP_REQUEST =
    "GET " HTTP_PATH " HTTP/1.0\r\n"
    "Host: " HOST "\r\n"
    "User-Agent: VMUPro/2.0\r\n"
    "Connection: close\r\n"
    "\r\n";

// Result fields shared with the UI
static char status_line[64] = "Press A to fetch";
static char detail_line[64] = "";
static int bytes_received = 0;
static char first_body_line[80] = "";

// Find the first non-empty line of the HTTP body and copy it out.
// Skips response headers (separated from body by \r\n\r\n).
static void extract_first_body_line(const char *response, int len)
{
    const char *body = NULL;
    for (int i = 0; i < len - 3; i++)
    {
        if (response[i] == '\r' && response[i + 1] == '\n' &&
            response[i + 2] == '\r' && response[i + 3] == '\n')
        {
            body = response + i + 4;
            break;
        }
    }
    if (!body)
        return;

    while (*body == '\r' || *body == '\n' || *body == ' ' || *body == '\t')
    {
        body++;
    }

    int out = 0;
    while (*body && *body != '\r' && *body != '\n' && out < (int)sizeof(first_body_line) - 1)
    {
        first_body_line[out++] = *body++;
    }
    first_body_line[out] = '\0';
}

// Connect to host:port over TCP, send the HTTP request, read the full response.
// Updates status_line / detail_line / bytes_received as it goes.
static void do_http_get(void)
{
    strncpy(status_line, "Resolving DNS...", sizeof(status_line) - 1);
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Resolving %s", HOST);

    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res = NULL;

    int err = getaddrinfo(HOST, PORT, &hints, &res);
    if (err != 0 || res == NULL)
    {
        vmupro_snprintf(status_line, sizeof(status_line), "DNS failed");
        vmupro_snprintf(detail_line, sizeof(detail_line), "err=%d", err);
        vmupro_log(VMUPRO_LOG_ERROR, TAG, "getaddrinfo failed: %d", err);
        return;
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    char addr_str[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &addr->sin_addr, addr_str, sizeof(addr_str));
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Resolved to %s", addr_str);

    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock < 0)
    {
        vmupro_snprintf(status_line, sizeof(status_line), "Socket failed");
        vmupro_snprintf(detail_line, sizeof(detail_line), "errno=%d", errno);
        vmupro_log(VMUPRO_LOG_ERROR, TAG, "socket() failed: %d", errno);
        freeaddrinfo(res);
        return;
    }

    strncpy(status_line, "Connecting...", sizeof(status_line) - 1);
    vmupro_snprintf(detail_line, sizeof(detail_line), "%s:%s", addr_str, PORT);

    if (connect(sock, res->ai_addr, res->ai_addrlen) != 0)
    {
        vmupro_snprintf(status_line, sizeof(status_line), "Connect failed");
        vmupro_snprintf(detail_line, sizeof(detail_line), "errno=%d", errno);
        vmupro_log(VMUPRO_LOG_ERROR, TAG, "connect() failed: %d", errno);
        close(sock);
        freeaddrinfo(res);
        return;
    }
    freeaddrinfo(res);

    strncpy(status_line, "Sending request...", sizeof(status_line) - 1);

    int req_len = strlen(HTTP_REQUEST);
    int sent = 0;
    while (sent < req_len)
    {
        int n = send(sock, HTTP_REQUEST + sent, req_len - sent, 0);
        if (n <= 0)
        {
            vmupro_snprintf(status_line, sizeof(status_line), "Send failed");
            vmupro_snprintf(detail_line, sizeof(detail_line), "errno=%d", errno);
            vmupro_log(VMUPRO_LOG_ERROR, TAG, "send() failed: %d", errno);
            close(sock);
            return;
        }
        sent += n;
    }

    strncpy(status_line, "Reading response...", sizeof(status_line) - 1);

    char *rx_buf = malloc(RX_BUF_SIZE);
    if (!rx_buf)
    {
        vmupro_snprintf(status_line, sizeof(status_line), "Out of memory");
        close(sock);
        return;
    }

    int total = 0;
    while (total < RX_BUF_SIZE - 1)
    {
        int n = recv(sock, rx_buf + total, RX_BUF_SIZE - 1 - total, 0);
        if (n < 0)
        {
            vmupro_log(VMUPRO_LOG_WARN, TAG, "recv() returned %d, errno=%d", n, errno);
            break;
        }
        if (n == 0)
            break; // server closed
        total += n;
    }
    rx_buf[total] = '\0';
    bytes_received = total;

    vmupro_log(VMUPRO_LOG_INFO, TAG, "Received %d bytes", total);

    extract_first_body_line(rx_buf, total);

    vmupro_snprintf(status_line, sizeof(status_line), "Got %d bytes", total);
    vmupro_snprintf(detail_line, sizeof(detail_line), "from %s", HOST);

    free(rx_buf);
    close(sock);
}

static void render_screen(void)
{
    vmupro_display_clear(VMUPRO_COLOR_BLACK);

    vmupro_set_font(VMUPRO_FONT_LARGE);
    vmupro_draw_text("Network", 10, 10, VMUPRO_COLOR_VMUGREEN, VMUPRO_COLOR_BLACK);
    vmupro_draw_text("Example", 10, 40, VMUPRO_COLOR_VMUGREEN, VMUPRO_COLOR_BLACK);

    vmupro_set_font(VMUPRO_FONT_MEDIUM);
    vmupro_draw_text(status_line, 10, 90, VMUPRO_COLOR_WHITE, VMUPRO_COLOR_BLACK);
    if (detail_line[0])
    {
        vmupro_draw_text(detail_line, 10, 115, VMUPRO_COLOR_GREY, VMUPRO_COLOR_BLACK);
    }

    if (first_body_line[0])
    {
        vmupro_set_font(VMUPRO_FONT_SMALL);
        vmupro_draw_text("Body preview:", 10, 150, VMUPRO_COLOR_YELLOW, VMUPRO_COLOR_BLACK);
        vmupro_draw_text(first_body_line, 10, 170, VMUPRO_COLOR_WHITE, VMUPRO_COLOR_BLACK);
    }

    vmupro_set_font(VMUPRO_FONT_SMALL);
    vmupro_draw_text("A: Fetch  B: Exit", 10, 220, VMUPRO_COLOR_GREY, VMUPRO_COLOR_BLACK);

    vmupro_push_double_buffer_frame();
}

void app_main(void)
{
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Network example starting");

    vmupro_start_double_buffer_renderer();

    bool running = true;
    while (running)
    {
        vmupro_btn_read();

        if (vmupro_btn_pressed(Btn_A))
        {
            // Reset display state then run the request
            first_body_line[0] = '\0';
            bytes_received = 0;
            render_screen();
            do_http_get();
        }

        if (vmupro_btn_pressed(Btn_B))
        {
            running = false;
        }

        render_screen();
        vmupro_sleep_ms(16);
    }

    vmupro_stop_double_buffer_renderer();
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Network example exiting");
}
