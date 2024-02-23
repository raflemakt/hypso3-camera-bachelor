/*
 * An attempt to make a userspace driver for an I2C device on Linux
 *
 * resources:
 * https://www.kernel.org/doc/html/latest/i2c/dev-interface.html
 *
 */

#include <linux/i2c-dev.h>
#include <i2c/smbus.h>


// Now, you have to decide which adapter you want to access. You should inspect
// /sys/class/i2c-dev/ or run "i2cdetect -l" to decide this. Adapter numbers
// are assigned somewhat dynamically, so you can not assume much about them.
// They can even change from one boot to the next.

// Next thing, open the device file, as follows:

int file;
int adapter_nr = 2;
char filename[20];

snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
file = open(filename, O_RDWR);

if (file < 0) {
    exit(1);
}

// When you have opened the device, you must specify with what device address
// you want to communicate:

int addr = 0x40;

if (ioctl(file, I2C_SLAVE, addr) < 0) {
    exit(1);
}

// Well, you are all set up now. You can now use SMBus commands or plain I2C to
// communicate with your device. SMBus commands are preferred if the device
// supports them. Both are illustrated below:

__u8 reg = 0x10; /* Device register to access */
__s32 res;
char buf[10];

/* Using SMBus commands */
res = i2c_smbus_read_word_data(file, reg);
if (res < 0) {
  /* ERROR HANDLING: I2C transaction failed */
} else {
  /* res contains the read word */
}

/*
 * Using I2C Write, equivalent of
 * i2c_smbus_write_word_data(file, reg, 0x6543)
 */
buf[0] = reg;
buf[1] = 0x43;
buf[2] = 0x65;
if (write(file, buf, 3) != 3) {
  /* ERROR HANDLING: I2C transaction failed */
}

/* Using I2C Read, equivalent of i2c_smbus_read_byte(file) */
if (read(file, buf, 1) != 1) {
  /* ERROR HANDLING: I2C transaction failed */
} else {
  /* buf[0] contains the read byte */
}

// Note that only a subset of the I2C and SMBus protocols can be achieved by
// the means of read() and write() calls. In particular, so-called combined
// transactions (mixing read and write messages in the same transaction) aren't
// supported. For this reason, this interface is almost never used by
// user-space programs.

