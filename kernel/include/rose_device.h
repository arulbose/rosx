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

#ifndef __ROSE_DEVICE_H
#define __ROSE_DEVICE_H

#include <RoseRTOS.h>

struct device;
struct devfile;

struct device_ops {
    int (*open)(struct devfile *);
    int (*release)(struct devfile *);
    int (*read)(struct devfile *, char *, size_t);
    int (*write)(struct devfile *, const char *, size_t);
    int (*ioctl)(struct devfile *, unsigned int, void *);

}; 
/* Main device struct */
struct device{
        const char name[16];
        struct device_ops *device_ops; /* initalized during the device driver initialization call */
        int device_open_count;
};

/* Device file passed to device api calls */
struct devfile {
    struct device *dev;
    void *private_data;
};

/* Driver core APIs used by application */
int dev_open(const char *filename, int flags);
int dev_close(int fd);
int dev_read(int fd, char *dest, int number_of_bytes);
int dev_write(int fd, const char *src, int number_of_bytes);
int dev_ioctl(int fd, unsigned int cmd, void *arg);

#endif /* __ROSE_DEVICE_H */
