#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

static int say_hello_world(sd_bus_message *m, void *userdata, sd_bus_error *error) {
    const char *hello;
    const char *world;
    const int bufsize = 32;
    char helloworld[bufsize];
    int ret = 0;

    ret = sd_bus_message_read(m, "ss", &hello, &world);
    if (ret < 0) {
        fprintf(stderr, "Failed to parse arguments\n");
        return ret;
    }

    strncpy(helloworld, hello, bufsize);
    strncat(helloworld, world, bufsize);

    return sd_bus_reply_method_return(m, "s", helloworld);
}

static int say_this_year_month_day(sd_bus_message *m, void *userdata, sd_bus_error *error) {
    int year;
    unsigned int month;
    unsigned int day;
    const int bufsize = 32;
    char date[bufsize];
    int ret = 0;

    ret = sd_bus_message_read(m, "iuu", &year, &month, &day);
    if (ret < 0) {
        fprintf(stderr, "Failed to parse arguments\n");
        return ret;
    }

    snprintf(date, bufsize, "%d-%u-%u", year, month, day);

    return sd_bus_reply_method_return(m, "s", date);
}

/*
 * 인터페이스를 만드는 매크로
 * Type을 결정짓는 인자는 https://pythonhosted.org/txdbus/dbus_overview.html의 Signature Strings 항목 참조
 * 기본적인 사용 방법은 https://www.freedesktop.org/software/systemd/man/sd_bus_add_object.html 참조
 */
static const sd_bus_vtable dbusexmaple_vtable[] = {
    SD_BUS_VTABLE_START(0),                                 /* 구조체 시작 매크로 */
    SD_BUS_METHOD_WITH_ARGS(                                /* method 인터페이스 */
            "SayHelloWorld",                                /* method 이름 */
            SD_BUS_ARGS("s", hello, "s", world),            /* 인자들(ex. "s"=string) */
            SD_BUS_RESULT("s", helloworld),                 /* 결과값 */
            say_hello_world,                                /* 처리를 위한 콜백 함수 */
            SD_BUS_VTABLE_UNPRIVILEGED),                    /* interface를 공개(introstectable)하도록 플래그 설정 */
    SD_BUS_METHOD_WITH_ARGS(
            "SayThisYearMonthDay", 
            SD_BUS_ARGS("i", year, "u", month, "u", day),
            SD_BUS_RESULT("s", date),
            say_this_year_month_day, 
            SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_VTABLE_END                                       /* 구조체 종료 매크로 */
};

int main() {
    sd_bus_slot *slot = NULL;
    sd_bus *bus = NULL;
    int ret = 0;

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
     * Bus Name 요청
     * 참고: https://www.freedesktop.org/software/systemd/man/sd_bus_request_name.html
     */
    ret = sd_bus_request_name(bus, "kr.kernelpanic.Example", 0);
    if (ret < 0) {
        fprintf(stderr, "Failed to acquire name\n");
        goto finish;
    }

    /*
     * Object Path와 Interface 등록
     * 참조: https://www.freedesktop.org/software/systemd/man/sd_bus_add_object.html
     */
    ret = sd_bus_add_object_vtable(bus,
            &slot,
            "/kr/kernelpanic/Example",      /* object path */
            "kr.kernelpanic.DBusExample",   /* interface name */
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
