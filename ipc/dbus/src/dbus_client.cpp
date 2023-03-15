#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

int main() {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    sd_bus *bus = NULL;
    const char *helloworld;
    const char *this_ymd;
    int ret;

    /*
     * session 버스에 연결
     * 참고: https://www.freedesktop.org/software/systemd/man/sd_bus_default.html
     */
    ret = sd_bus_default_user(&bus);
    if (ret < 0) {
        fprintf(stderr, "Failed to connect to dbus-daemon\n");
        goto finish;
    }

    /*
     * D-Bus 메시지 호출
     * 참고: https://man7.org/linux/man-pages/man3/sd_bus_call_method.3.html
     */
    ret = sd_bus_call_method(bus,
            "kr.kernelpanic.Example",           /* bus name */
            "/kr/kernelpanic/Example",          /* object path */
            "kr.kernelpanic.DBusExample",       /* interface name */
            "SayHelloWorld",                    /* method name */
            &error,                             /* 실패 시 에러메시지 */
            &m,                                 /* 성공 시 메시지 */
            "ss",                               /* 인자형식(string, string) */
            "Hello", "World");                  /* 인자들 */
    if (ret < 0) {
        fprintf(stderr, "Failed to issue method call\n");
        goto finish;
    }

    /*
     * 성공 메시지를 읽어서 return 값 획득
     * 참고: https://www.freedesktop.org/software/systemd/man/sd_bus_message_read.html
     */
    ret = sd_bus_message_read(m, "s", &helloworld);
    if (ret < 0) {
        fprintf(stderr, "Failed to parse response message\n");
        goto finish;
    }

    printf("SayHelloWorld: %s\n", helloworld);

    ret = sd_bus_call_method(bus,
            "kr.kernelpanic.Example",
            "/kr/kernelpanic/Example",
            "kr.kernelpanic.DBusExample",
            "SayThisYearMonthDay",
            &error,
            &m,
            "iuu",                              /* 인자형식(int, uint, uint) */
            2020, 04, 12);                      /* 인자들 */
    if (ret < 0) {
        fprintf(stderr, "Failed to issue method call\n");
        goto finish;
    }

    ret = sd_bus_message_read(m, "s", &this_ymd);
    if (ret < 0) {
        fprintf(stderr, "Failed to parse response message\n");
        goto finish;
    }

    printf("SayThisYearMonthDay: %s\n", this_ymd);

finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(m);
    sd_bus_unref(bus);

    return 0;
}
