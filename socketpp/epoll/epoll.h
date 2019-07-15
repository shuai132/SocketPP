//
// Created by liushuai on 2019/7/10.
//

#ifdef __cplusplus
extern "C" {
#endif

int ep_server_start_loop(const char *port, void *userdata);

int ep_connect_and_loop(const char *host, int port, void *userdata);

void ep_end_loop(void);

#ifdef __cplusplus
}
#endif
