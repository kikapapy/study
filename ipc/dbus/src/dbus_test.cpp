#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

#include <iostream>
#include <chrono>

#define BUS_NAME "kr.kernelpanic.Example"
#define OBJECT_NAME "/kr/kernelpanic/Example"
#define INTERFACE_NAME "kr.kernelpanic.DBusExample"
#define METHOD_NAME "SayHelloWorld"

int main() {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    sd_bus *bus = NULL;
    const char *echo;
    int ret;

    std::chrono::system_clock::time_point start;
    std::chrono::duration<double> elapsed_seconds;

    ret = sd_bus_default_user(&bus);
    if (ret < 0) {
        fprintf(stderr, "Failed to connect to dbus-daemon\n");
        goto finish;
    }

    start = std::chrono::system_clock::now();

    for (int i = 0; i < 1000; i++)
    {
        ret = sd_bus_call_method(bus,
            BUS_NAME,           /* bus name */
            OBJECT_NAME,        /* object path */
            INTERFACE_NAME,     /* interface name */
            METHOD_NAME,        /* method name */
            &error,             /* 실패 시 에러메시지 */
            &m,                 /* 성공 시 메시지 */
            "s",                /* 인자형식(string, string) */
            "123");             /* 인자들 */
        if (ret < 0) {
            fprintf(stderr, "Failed to issue method call\n");
            goto finish;
        }

        ret = sd_bus_message_read(m, "s", &echo);
        if (ret < 0) {
            fprintf(stderr, "Failed to parse response message\n");
            goto finish;
        }
    }
    
    elapsed_seconds = std::chrono::system_clock::now() - start;
    std::cout << "test : " << elapsed_seconds.count() << std::endl;

finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(m);
    sd_bus_unref(bus);

    return 0;
}
