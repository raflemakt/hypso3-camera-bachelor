#include <stdio.h>
#include <stdint.h>
#include <unistd.h> // for sleep() - optional
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/types.h>
#include <linux/i2c.h>

const char* path = "/dev/i2c-42";
uint8_t devAddr = 0;
uint8_t regAddr = 0x013a;
int fdiic;
int value;

int init(const char *path, uint8_t devAddr) {
    fdiic = open(path, O_RDWR);
    if (fdiic < 0) {
        printf("Failed to open '%s'", path);
        return -1;
    }
    printf("Successfully opend '%s'", path);
    return 0;
}

int iicread(uint8_t regAddr, union i2c_smbus_data *value){
    if(ioctl(fdiic, I2C_SLAVE, devAddr) < 0){
        printf("Cant read");
	return -1;
    }
    struct i2c_mbus_ioctl_data args;
    args.read_write = I2C_SMBUS_READ;
    args.command = regAddr;
    args.size = I2C_SMBUS_BYTE_DATA;
    args.data = value;

    return ioctl(fdiic, I2C_SMBUS, &args);
}

int main() {

    init(path, 0);
    iicread(regAddr, value);
    return 0;
}

