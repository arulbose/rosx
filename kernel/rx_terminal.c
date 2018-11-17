/* RosX RT-Kernel
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

#include <RosX.h>

/* Opens a terminal emulator */

void rx_terminal_thread()
{
    pr_info("\n");
    pr_info("||||||||||||||||||||||||||||    ||||||||||||||||||||||||    ||||||||||||||||||||||||||    ||||                   ||||\n");
    pr_info("||||||||||||||||||||||||||||    ||||||||||||||||||||||||    ||||||||||||||||||||||||||     ||||                 ||||\n");
    pr_info("||||||||||||||||||||||||||||    ||||||||||||||||||||||||    ||||||||||||||||||||||||||      ||||               ||||\n");
    pr_info("||||                    ||||    ||||                ||||    ||||                             ||||             ||||\n"); 
    pr_info("||||                    ||||    ||||                ||||    ||||                              ||||           ||||\n");
    pr_info("||||                    ||||    ||||                ||||    ||||                               ||||         ||||\n");
    pr_info("||||                    ||||    ||||                ||||    ||||                                ||||       ||||\n");
    pr_info("||||                    ||||    ||||                ||||    ||||                                 ||||     ||||\n");
    pr_info("||||                    ||||    ||||                ||||    ||||                                  ||||   ||||\n");
    pr_info("||||                    ||||    ||||                ||||    ||||                                   |||||||||\n");
    pr_info("||||                    ||||    ||||                ||||    ||||                                    ||||||||\n");
    pr_info("||||||||||||||||||||||||||||    ||||                ||||    |||||||||||||||||||||||||                ||||||\n");
    pr_info("||||||||||||||||||||||||||||    ||||                ||||    |||||||||||||||||||||||||                |||||\n");
    pr_info("||||||||||||||||||||||||||||    ||||                ||||    |||||||||||||||||||||||||                ||||||\n");
    pr_info("||||||                          ||||                ||||                         ||||               ||||||||\n");
    pr_info("|||||||||                       ||||                ||||                         ||||              ||||  ||||\n");
    pr_info("||||||||||||                    ||||                ||||                         ||||             ||||    ||||\n");
    pr_info("|||||||||||||||                 ||||                ||||                         ||||            ||||      ||||\n");
    pr_info("||||||||||||||||||              ||||                ||||                         ||||           ||||        ||||\n");
    pr_info("|||||||||||||||||||||           ||||                ||||                         ||||          ||||          ||||\n");
    pr_info("||||||||||||||||||||||||        ||||                ||||                         ||||         ||||            ||||\n");
    pr_info("|||||||||||||||||||||||||||     ||||                ||||                         ||||        ||||              ||||\n");
    pr_info("||||||||||||||||||||||||||||    ||||||||||||||||||||||||    |||||||||||||||||||||||||       ||||                ||||\n");
    pr_info("||||||||||||||||||||||||||||    ||||||||||||||||||||||||    |||||||||||||||||||||||||      ||||                  ||||\n");
    pr_info("||||||||||||||||||||||||||||    ||||||||||||||||||||||||    |||||||||||||||||||||||||     ||||                    ||||\n");
    pr_info("                                                                                                                       Version.%s \n", CONFIG_RELEASE_VERSION);
    pr_info("Real-Time Operating System Xperiment\n");
    pr_info("\n");

    while(1) {
              #ifdef CONFIG_X86_SIMULATOR
	      rx_suspend_task(MYSELF);
              #else
              /* Add waitqueue for real serial ports */
              #endif
    }
}
