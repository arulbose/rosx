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

int x86_simulator_init();
struct device_ops fops;

int x86_simulator_init()
{
    if ( 0 == register_driver("simx86", &fops)) {
        __early_printk("simx86 Driver registered\n");
    }else{
        __early_printk("simx86 Driver registeration failed\n");
    }

    return 0;
}

static int x86_sim_open(struct devfile *dev)
{
    /* Nothing to do */
    return 0;
}

static int x86_sim_release(struct devfile *dev)
{
    /* Nothing to do */
    return 0;
}

static int x86_sim_write(struct devfile *dev, const char *src, size_t size)
{
     __printk("%c", *src); /* X86 native print */
    return size;
}

struct device_ops fops = {
    .open = x86_sim_open,
    .release = x86_sim_release,
    .write = x86_sim_write
};
