/*
 * Copyright (c) 2017, Michal Kowalczyk <kowalczykmichal88@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdio.h>
#include <contiki.h>
#include <sys/clock.h>
#include "dev/leds.h"
#include "letmecreate/core/debug.h"
#include "awa/static.h"
#include "blinds_debug.h"

#define MAX_NAME_LENGTH 128

typedef struct {
    AwaStaticClient *awaClient;
} context_t;

static context_t ctx;

/*
 * This IPSO object should be used with a generic position actuator from 0 to 100%. This resource
 * optionally allows setting the transition time for an operation that changes the position of
 * the actuator, and for reading the remaining time of the currently active transition.
 */
typedef struct {
    float CurrentPosition; /* Current position or desired position of a positioner actuator. */
    float TransitionTime; /*The time expected to move the actuator to the new position. */
    float RemainingTime; /* The time remaining in an operation. */
    float MinMeasuredValue; /* The minimum value set on the actuator since power ON or reset. */
    float MaxMeasuredValue; /* The maximum value set on the actuator since power ON or reset. */
    float MinLimit; /* The minimum value that can be measured by the sensor. */
    float MaxLimit; /* The maximum value that can be measured by the sensor. */
    char ApplicationType[MAX_NAME_LENGTH]; /* The application type of the sensor or actuator as
                                              a string depending on the use case. */
} IPSOPositioner_t;
/* Object ID */
#define IPSOPositioner_ID       3337
/* Resources IDs */
#define CurrentPosition_ID  5536
#define TransitionTime_ID   5537
#define RemainingTime_ID    5538
#define MinMeasuredValue_ID 5601
#define MaxMeasuredValue_ID 5602
#define MinLimit_ID         5519
#define MaxLimit_ID         5520
#define ApplicationType_ID  5750

static IPSOPositioner_t positioner;

PROCESS(main_process, "Main process");
AUTOSTART_PROCESSES(&main_process);

static void set_default_router()
{
    // This is the workaround if router doesn't send Router Advertisment packets and
    // you can't use link-local address for some reason. It assumes that your router
    // is the same as LWM2M bootstrap server.
    uip_ipaddr_t ipaddr;

    uip_ip6addr(&ipaddr, CONFIG_BOOTSTRAP_IPv6_ADDR1, CONFIG_BOOTSTRAP_IPv6_ADDR2,
            CONFIG_BOOTSTRAP_IPv6_ADDR3, CONFIG_BOOTSTRAP_IPv6_ADDR4, CONFIG_BOOTSTRAP_IPv6_ADDR5,
            CONFIG_BOOTSTRAP_IPv6_ADDR6, CONFIG_BOOTSTRAP_IPv6_ADDR7, CONFIG_BOOTSTRAP_IPv6_ADDR8);
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

static AwaResult handler(AwaStaticClient * client, AwaOperation operation, AwaObjectID objectID,
        AwaObjectInstanceID objectInstanceID, AwaResourceID resourceID, AwaResourceInstanceID
        resourceInstanceID, void ** dataPointer, size_t * dataSize, bool * changed)
{
    return AwaResult_InternalError;
}

static void define_positioner_optional_resource(AwaStaticClient *awaClient, AwaResourceID id,
        const char *name)
{
    AWA_ASSERT(AwaStaticClient_DefineResource(awaClient, IPSOPositioner_ID, id, name,
                AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly));
    AWA_ASSERT(AwaStaticClient_SetResourceOperationHandler(awaClient, IPSOPositioner_ID,
               id, handler));
}

static void define_positioner_object(AwaStaticClient *awaClient)
{
    AWA_ASSERT(AwaStaticClient_DefineObject(awaClient, IPSOPositioner_ID, "IPSOPositioner", 1, 1));
    AWA_ASSERT(AwaStaticClient_SetObjectOperationHandler(awaClient, IPSOPositioner_ID, handler));

    AWA_ASSERT(AwaStaticClient_DefineResource(awaClient, IPSOPositioner_ID, CurrentPosition_ID,
                "CurrentPosition", AwaResourceType_Float, 1, 1, AwaResourceOperations_ReadWrite));
    AWA_ASSERT(AwaStaticClient_SetResourceOperationHandler(awaClient, IPSOPositioner_ID,
                CurrentPosition_ID, handler));

    AWA_ASSERT(AwaStaticClient_DefineResource(awaClient, IPSOPositioner_ID, TransitionTime_ID,
                "TransitionTime", AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadWrite));
    AWA_ASSERT(AwaStaticClient_SetResourceOperationHandler(awaClient, IPSOPositioner_ID,
                TransitionTime_ID, handler));

    define_positioner_optional_resource(awaClient, RemainingTime_ID, "RemainingTime");
    define_positioner_optional_resource(awaClient, MinMeasuredValue_ID, "MinMeasuredValueTime");
    define_positioner_optional_resource(awaClient, MaxMeasuredValue_ID, "MaxMeasuredValueTime");
    define_positioner_optional_resource(awaClient, MinLimit_ID, "MinLimt");
    define_positioner_optional_resource(awaClient, MaxLimit_ID, "MaxLimt");

    AWA_ASSERT(AwaStaticClient_DefineResource(awaClient, IPSOPositioner_ID, ApplicationType_ID,
                "ApplicationType", AwaResourceType_String, 0, 1, AwaResourceOperations_ReadWrite));
    AWA_ASSERT(AwaStaticClient_SetResourceOperationHandler(awaClient, IPSOPositioner_ID,
                ApplicationType_ID, handler));
}

static void create_heater_object(AwaStaticClient *awaClient)
{
    AWA_ASSERT(AwaStaticClient_CreateObjectInstance(awaClient, IPSOPositioner_ID, 0));
    AWA_ASSERT(AwaStaticClient_CreateResource(awaClient, IPSOPositioner_ID, 0, CurrentPosition_ID));
    AWA_ASSERT(AwaStaticClient_CreateResource(awaClient, IPSOPositioner_ID, 0, TransitionTime_ID));
    AWA_ASSERT(AwaStaticClient_CreateResource(awaClient, IPSOPositioner_ID, 0, RemainingTime_ID));
    AWA_ASSERT(AwaStaticClient_CreateResource(awaClient, IPSOPositioner_ID, 0, MinMeasuredValue_ID));
    AWA_ASSERT(AwaStaticClient_CreateResource(awaClient, IPSOPositioner_ID, 0, MaxMeasuredValue_ID));
    AWA_ASSERT(AwaStaticClient_CreateResource(awaClient, IPSOPositioner_ID, 0, MinLimit_ID));
    AWA_ASSERT(AwaStaticClient_CreateResource(awaClient, IPSOPositioner_ID, 0, MaxLimit_ID));
    AWA_ASSERT(AwaStaticClient_CreateResource(awaClient, IPSOPositioner_ID, 0, ApplicationType_ID));
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
        define_positioner_object(ctx.awaClient);
        create_heater_object(ctx.awaClient);

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
