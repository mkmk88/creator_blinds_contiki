/*
 * Copyright (c) 2017, Michal Kowalczyk <kowalczykmichal88@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdio.h>
#include <contiki.h>
#include <sys/clock.h>
#include "dev/leds.h"
#include "letmecreate/core/debug.h"
#include "awa/static.h"
#include "blinds_debug.h"

typedef struct {
    AwaStaticClient *awaClient;
} context_t;

static context_t ctx;

PROCESS(main_process, "Main process");
AUTOSTART_PROCESSES(&main_process);

static void set_default_router()
{
    // This is the workaround if router doesn't send Router Advertisment packets and
    // you can't use link-local address for some reason. It assumes that your router
    // is the same as LWM2M bootstrap server.
    uip_ipaddr_t ipaddr;

    uip_ip6addr(&ipaddr, CONFIG_BOOTSTRAP_IPv6_ADDR1, CONFIG_BOOTSTRAP_IPv6_ADDR2, CONFIG_BOOTSTRAP_IPv6_ADDR3,
            CONFIG_BOOTSTRAP_IPv6_ADDR4, CONFIG_BOOTSTRAP_IPv6_ADDR5, CONFIG_BOOTSTRAP_IPv6_ADDR6, CONFIG_BOOTSTRAP_IPv6_ADDR7,
            CONFIG_BOOTSTRAP_IPv6_ADDR8);
    uip_ds6_defrt_add(&ipaddr, 0);
}

static void setup_awa_client(AwaStaticClient *awaClient)
{
    AWA_ASSERT(AwaStaticClient_SetLogLevel(AwaLogLevel_Error));
    AWA_ASSERT(AwaStaticClient_SetEndPointName(awaClient, CONFIG_ENDPOINT_NAME));
    AWA_ASSERT(AwaStaticClient_SetCoAPListenAddressPort(awaClient, "::", CONFIG_COAP_LISTEN_PORT));
    AWA_ASSERT(AwaStaticClient_SetBootstrapServerURI(awaClient, CONFIG_BOOTSTRAP_URI));

    AWA_ASSERT(AwaStaticClient_Init(awaClient));
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(main_process, ev, data)
{
    PROCESS_BEGIN();
    {
        LOG("Window Blinds Controller - START\n");

        set_default_router();

        ctx.awaClient = AwaStaticClient_New();
        if (!ctx.awaClient) {
            debug_hang_up(false);
        }

        setup_awa_client(ctx.awaClient);

        LOG("Awa setup - COMPLETED\n");
        while (1)
        {
            static struct etimer et;
            static int waitTime;

            waitTime = AwaStaticClient_Process(ctx.awaClient);

            etimer_set(&et, (waitTime * CLOCK_SECOND) / 1000);
            PROCESS_YIELD();
            waitTime = 0;
        }

        AwaStaticClient_Free(&ctx.awaClient);
    }

    PROCESS_END();
}

/*---------------------------------------------------------------------------*/
