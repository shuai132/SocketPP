//
// Created by liushuai on 2019/7/12.
//

#ifdef __cplusplus
extern "C" {
#endif

/**
 * return if error
 * @param port
 * @param userdata
 * @return
 */
int kq_server_start_loop(int port, void *userdata);

/**
 * return if connect failed
 * @param host
 * @param port
 * @param userdata
 * @return
 */
int kq_connect_and_loop(const char *host, int port, void *userdata);

// todo: end loop
// void kq_end_loop(void);

#ifdef __cplusplus
}
#endif
