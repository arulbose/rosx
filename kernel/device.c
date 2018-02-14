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
#include <rose/errno.h>

static struct devfile desc[CONFIG_MAX_DEVICE_DESC];

/* Add new devices in the list */
static struct device device_table[] = {
                                 {"simx86", NULL, 0}
			       };

/* Driver core APIs used by application */

int dev_open(const char *filename, int flags)
{
    int device_table_id;
    int desc_id;
    int ret;
    
    for(device_table_id = 0; device_table_id < (sizeof(device_table)/sizeof(struct device)); device_table_id ++ )
    {
        if( 0 == strcmp(filename, device_table[device_table_id].name ))
        { /* Device found */
            if(device_table[device_table_id].device_ops == NULL)
               return -ENXIO; /* device has not been initialized */

            /* Find a free devfile desc slot */
            for(desc_id = 0; desc_id < (sizeof(desc)/sizeof(struct devfile)); desc_id ++ )
            {
                if(desc[desc_id].dev == NULL)
                {
                    /* initialize device file desc */
                    desc[desc_id].dev = &device_table[device_table_id];
                    desc[desc_id].private_data = NULL;
                    /* Now call the respective driver open */ 
                    if(0 == (ret =  desc[desc_id].dev->device_ops->open(&desc[desc_id]))){
                        desc[desc_id].dev->device_open_count ++; /* Increment device count */
                        return desc_id; /* success return the device descriptor for the application to use it */
                    }else{
                      /* Device open failed at driver level ; do clean-up and return error */
                      desc[desc_id].dev = NULL;
                      desc[desc_id].private_data = NULL;
                      return ret; /* Just pass the driver error */
                    }
                }            
            }
                return -ENOMEM;
        }
    }
    return -ENODEV;
}

/* Device close */
int dev_close(int desc_id)
{
    int ret;
    if(desc[desc_id].dev != NULL) {
        /* Call device release */
        if(0 == (ret =  desc[desc_id].dev->device_ops->release(&desc[desc_id]))){
            /* success do all clean up */
            desc[desc_id].dev->device_open_count --;
            desc[desc_id].dev = NULL;
            desc[desc_id].private_data = NULL;
            return 0;
        }else{
            return ret; /* Return device specific error */
        }
    }
    return -ENODEV;
}

int dev_read(int desc_id, char *dest, int number_of_bytes)
{
    int ret;
    if(desc[desc_id].dev != NULL) {
        /* Call device release */
        ret = desc[desc_id].dev->device_ops->read(&desc[desc_id], dest, number_of_bytes );
        return ret;
    }
    return -ENODEV;
}

int dev_write(int desc_id, const char *src, int number_of_bytes)
{
    int ret;
    if(desc[desc_id].dev != NULL) {
        /* Call device release */
        ret = desc[desc_id].dev->device_ops->write(&desc[desc_id], src, number_of_bytes );
        return ret;
    }
    return -ENODEV;
}

int dev_ioctl(int desc_id, unsigned int cmd, void *arg)
{
    int ret;
    if(desc[desc_id].dev != NULL) {
        /* Call device release */
        ret = desc[desc_id].dev->device_ops->ioctl(&desc[desc_id], cmd, arg );
        return ret;
    }
    return -ENODEV;
}

int register_driver(char *name, struct device_ops *ops)
{

}