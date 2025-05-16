#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    char buffer[100];
    int fd = open("/dev/virt_sensor", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    read(fd, buffer, sizeof(buffer));
    printf("Sensor Output: %s", buffer);

    close(fd);
    return 0;
}
