//
// Created by liushuai on 2019/7/12.
//

#ifdef __cplusplus
extern "C" {
#endif

int kq_start_loop(int port, void *userdata);

void kq_end_loop(void);

#ifdef __cplusplus
}
#endif
