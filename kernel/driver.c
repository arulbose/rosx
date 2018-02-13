/* Rose RT-Kernel
 * Copyright (C) 2016 Arul Bose<bose.arul@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <RoseRTOS.h>

#define CONFIG_MAX_DEVICE_DESC 5

/* Main device struct */
struct device{
        char device_name[18];
        unsigned int *device_ops; /* initalized in the device driver initialization call */
        int device_open_count;
};

static struct devfile {
    int dd; /* Device descriptor */
    void *private_data;
    struct device *dev;
};

static struct device_descriptor_list device_desc[CONFIG_MAX_DEVICE_DESC]

/* Add new devices in the list */
struct device device_table[] = {
                                 {"simx86", NULL, 0}
			       };

/* Driver core APIs used by application */
int open(char *filename, int flags);
void close(int fd);
int read(int fd, char *dest, int number_of_bytes);
int write(int fd, char *src, int number_of_bytes);
int ioctl(int fd, unsigned int cmd, void *arg);
int poll(int fd, wait_queue);

/* Driver core APIs used by driver */
