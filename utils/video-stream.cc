// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
//
// Run a daemon which accepts incoming connections on a socket and pushes
// raw frames to the display.
//
// Usage Example:
//
// video-stream matrix.sock &
// ffmpeg -i $videofile -filter:v scale=64:32,format=rgb24 -f rawvideo unix://matrix.sock

#include <csignal>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>

#include "led-matrix.h"

using rgb_matrix::FrameCanvas;
using rgb_matrix::RGBMatrix;

static int usage(const char *progname, const char *msg = NULL)
{
    if (msg)
    {
        fprintf(stderr, "%s\n", msg);
    }
    fprintf(stderr, "Video streaming daemon for RGB-Matrix. Listens for connections and pushes raw data to the display.\n");
    fprintf(stderr, "usage: %s [options] <socket>\n", progname);
    fprintf(stderr, "\nGeneral LED matrix options:\n");
    rgb_matrix::PrintMatrixFlags(stderr);
    return 1;
}

int sockfd, clientfd;
struct sockaddr_un srv_addr, cli_addr;

void register_cleanup()
{
    auto cleanup = [](int i) {
        if (clientfd)
        {
            fprintf(stderr, "closing clientfd\n");
            close(clientfd);
        }
        if (sockfd)
        {
            fprintf(stderr, "closing socket fd\n");
            close(sockfd);
            remove(srv_addr.sun_path);
        }
        exit(0);
    };
    signal(SIGINT, cleanup);
    signal(SIGABRT, cleanup);
    signal(SIGTERM, cleanup);
    signal(SIGTSTP, cleanup);
}

int main(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "") == 0)
    {
        fprintf(stderr, "missing required argument <socket>\n");
        return 1;
    }

    RGBMatrix::Options matrix_options;
    rgb_matrix::RuntimeOptions runtime_opt;
    if (!rgb_matrix::ParseOptionsFromFlags(&argc, &argv,
                                           &matrix_options, &runtime_opt))
    {
        return usage(argv[0]);
    }

    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
    if (matrix == NULL)
    {
        return 1;
    }
    FrameCanvas *framebuffer = matrix->CreateFrameCanvas();

    register_cleanup();

    int width = matrix_options.cols * matrix_options.chain_length;
    int height = matrix_options.rows * matrix_options.parallel;
    int buf_size = 3 * width * height;

    char buffer[buf_size];

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "failed to open socket: %d\n", sockfd);
        return -1;
    }

    memset((char *)&srv_addr, 0, sizeof(srv_addr));

    srv_addr.sun_family = AF_UNIX;
    strncpy(srv_addr.sun_path, argv[1], 107);

    if (int err = bind(sockfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0)
    {
        fprintf(stderr, "failed to bind socket: %d\n", err);
    }

    chmod(srv_addr.sun_path, 0660);
    listen(sockfd, 1);

    socklen_t cli_len = sizeof(cli_addr);
    while ((clientfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len)))
    {
        if (clientfd < 0)
        {
            fprintf(stderr, "failed to accept client connection: %d\n", clientfd);
        }

        while (int n = read(clientfd, buffer, buf_size))
        {
            if (n < 0)
            {
                fprintf(stderr, "failed to read from client: %d\n", n);
                break;
            }

            char *p_buffer = buffer;
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    framebuffer->SetPixel(x, y, p_buffer[0], p_buffer[1], p_buffer[2]);
                    p_buffer += 3;
                }
            }

            framebuffer = matrix->SwapOnVSync(framebuffer);
        }

        close(clientfd);
        framebuffer->Clear();
        framebuffer = matrix->SwapOnVSync(framebuffer);
    }
    close(sockfd);

    return 0;
}