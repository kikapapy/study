#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

#define BUS_NAME "kr.kernelpanic.Example"
#define OBJECT_NAME "/kr/kernelpanic/Example"
#define INTERFACE_NAME "kr.kernelpanic.DBusExample"
#define METHOD_NAME "SayHelloWorld"

static int echo_method(sd_bus_message *m, void *userdata, sd_bus_error *error) {
    const char *str;
    const int bufsize = 32;
    //char echo[bufsize];
    int ret = 0;

    ret = sd_bus_message_read(m, "s", &str);
    if (ret < 0) {
        fprintf(stderr, "Failed to parse arguments\n");
        return ret;
    }

    //strncpy(echo, str, bufsize);

    return sd_bus_reply_method_return(m, "s", str);
}

/*
 * 인터페이스를 만드는 매크로
 * Type을 결정짓는 인자는 https://pythonhosted.org/txdbus/dbus_overview.html의 Signature Strings 항목 참조
 * 기본적인 사용 방법은 https://www.freedesktop.org/software/systemd/man/sd_bus_add_object.html 참조
 */
static const sd_bus_vtable dbusexmaple_vtable[] = {
    SD_BUS_VTABLE_START(0),                                 /* 구조체 시작 매크로 */
    SD_BUS_METHOD_WITH_ARGS(                                /* method 인터페이스 */
            METHOD_NAME,                                /* method 이름 */
            SD_BUS_ARGS("s", str),             /* 인자들(ex. "s"=string) */
            SD_BUS_RESULT("s", echo),                 /* 결과값 */
            echo_method,                                /* 처리를 위한 콜백 함수 */
            SD_BUS_VTABLE_UNPRIVILEGED),                    /* interface를 공개(introstectable)하도록 플래그 설정 */
    SD_BUS_VTABLE_END                                       /* 구조체 종료 매크로 */
};

int main() {
    sd_bus_slot *slot = NULL;
    sd_bus *bus = NULL;
    int ret = 0;

    ret = sd_bus_default_user(&bus);
    if (ret < 0) {
        fprintf(stderr, "Failed to connect to dbus-daemon\n");
        goto finish;
    }

    ret = sd_bus_request_name(bus, BUS_NAME, 0);
    if (ret < 0) {
        fprintf(stderr, "Failed to acquire name\n");
        goto finish;
    }

    ret = sd_bus_add_object_vtable(bus,
            &slot,
            OBJECT_NAME,
            INTERFACE_NAME,
            dbusexmaple_vtable,
            NULL);
    if (ret < 0) {
        fprintf(stderr, "Failed to add object interface\n");
        goto finish;
    }

    for (;;) {
        ret = sd_bus_process(bus, NULL);
        if (ret < 0) {
            fprintf(stderr, "Failed to process bus\n");
            goto finish;
        }else if (ret > 0) {
            /* 
             * 만약 sd_bus_process에서 처리가 발생되면, 
             * wait을 하지 않고 다음 입력 처리
             */
            continue;
        }

        ret = sd_bus_wait(bus, (uint64_t)-1);
        if (ret < 0) {
            fprintf(stderr, "Failed to wait on bus\n");
            goto finish;
        }
    }

finish:
    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);

    return 0;
}
