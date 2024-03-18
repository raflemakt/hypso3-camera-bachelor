/*
 * An attempt to make a userspace driver for an I2C device on Linux
 *
 * resources:
 * https://www.kernel.org/doc/html/latest/i2c/dev-interface.html
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h> // for sleep() - optional
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>

// Now, you have to decide which adapter you want to access. You should inspect
// /sys/class/i2c-dev/ or run "i2cdetect -l" to decide this. Adapter numbers
// are assigned somewhat dynamically, so you can not assume much about them.
// They can even change from one boot to the next.

// Next thing, open the device file, as follows:

int file;
int adapter_nr = 2;
char* filename = "/dev/i2c-22";

int main()
{

    file = open(filename, O_RDWR);

    if (file < 0)
    {
	printf("Failed to open i2c decice");
        return -1;
    }

    // When you have opened the device, you must specify with what device address
    // you want to communicate:

    int addr = 0x40;

    if (ioctl(file, I2C_SLAVE, addr) < 0)
    {
	printf("failed to communicate address");
        return -1;
    }

    // Well, you are all set up now. You can now use SMBus commands or plain I2C to
    // communicate with your device. SMBus commands are preferred if the device
    // supports them. Both are illustrated below:

    int reg = 0x013a; // Temperature register
    char* res;
    char buf[10];

    /*
    // Using SMBus commands 
    res = i2c_smbus_read_word_data(file, reg);
    if (res < 0)
    {
        // ERROR HANDLING: I2C transaction failed 
	printf("I2C transaction failed");
    }
    printf(res);

     // Using I2C Write, equivalent of
    // i2c_smbus_write_word_data(file, reg, 0x6543)
    
    buf[0] = reg;
    buf[1] = 0x43;
    buf[2] = 0x65;
*/
    //if (write(file, buf, 3) != 3){}

    // Using I2C Read, equivalent of i2c_smbus_read_byte(file)

    if(write(file, &reg, sizeof(reg)) == -1){
	printf("Failed write\n");
	perror("write\n");
    }
    uint8_t data;

    printf("Before data: 0x%02X\n", data);
    if (read(file, &data, sizeof(data)) == -1)
    {
        // ERROR HANDLING: I2C transaction failed
	printf("Failed read\n");
    }
    
    
        // buf[0] contains the read byte
	printf("Recieved data: 0x%02X\n", data);
    


    return 0;
}
