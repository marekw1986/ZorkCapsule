#include <clock.h>
#include <stdint.h>
#include <stdio.h>
#include <io.h>
#include <compiler.h>
#include <gclk.h>
#include <system.h>
#include "ztypes.h"

int main(void)
{
	system_init();   /* configures XOSC, DFLL, GCLKs per conf_clocks.h */

    while (1)
    {
        zork_handle();
    }

    return 0;
}
