/*
The MIT License

Copyright (c) 2014- High-Mobility GmbH (https://high-mobility.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "hmkit_core.h"
#include "hmkit_core_connectivity_hal.h"
#include "btstack.h"
#include <gio/gio.h>

gboolean timerthread_exit = false;
gboolean timeout_callback(gpointer data);
gpointer superThread(gpointer data);
int hm_link_exit(void);
int hm_link_main(void);

gboolean timeout_callback(gpointer data)
{
    UNUSED(data);
    hmkit_core_clock();

	if(timerthread_exit)
	{
    	//printf("@@@@@@@@ timeout_callback() g_thread_exit()  @@@@@@@\n");
		g_thread_exit(NULL);
	}

    return TRUE;
}

static void sigint_handler(int param){
    UNUSED(param);

    printf("CTRL-C - SIGINT received, shutting down..\n");
    log_info("sigint_handler: shutting down");

	hmkit_core_connectivity_hal_advertisement_stop();

    // reset anyway
    //btstack_stdin_reset();

    // power down
    hci_power_control(HCI_POWER_OFF);
    //hci_close();

    log_info("Good bye, see you.\n");
    //printf("@@@@@@@@ C layer: CTRL-C - SIGINT, exit() shut down.. @@@@@@@\n");
	sleep(1);
    exit(0);
}

int hm_link_exit(void)
{
	timerthread_exit = true;

	// call signal handler explicitly
	sigint_handler(0);

    //printf("@@@@@@@@ hm_link_exit() Exit @@@@@@@\n");
    return 0;
}


gpointer superThread(gpointer data){
    UNUSED(data);
    GMainLoop *loop = NULL;

  //printf("$$$$$$$$$$$$$ Enter hm_main.c; superThread  $$$$$$$$$$$$$$\n");

    g_timeout_add(60, timeout_callback, loop);

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

  //printf("$$$$$$$$$$$$$ Exit hm_main.c; superThread  $$$$$$$$$$$$$$\n");

    return 0;
}


int hm_link_main(void){

  //printf("$$$$$$$$$$$$$ Enter hm_main.c; hm_link_main $$$$$$$$$$$$$$\n");

    //Init core
    hmkit_core_init();

    // handle CTRL-c
    signal(SIGINT, sigint_handler);

    //Start core alive ping
    g_thread_new ("SuperThread", superThread, NULL);

  //printf("$$$$$$ hm_main.c; hm_link_main, call btstacl run loop ******* \n");

    // go to btstack loop
    btstack_run_loop_execute();    

  //printf("$$$$$$$$$$$$$ EIXT:  hm_main.c; hm_link_main $$$$$$$$$$$$$$$$\n");

    return 0;
}
