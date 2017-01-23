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
#ifndef __BLINDS_DEBUG_H__
#define __BLINDS_DEBUG_H__

#include <stdbool.h>
#include "awa/static.h"
#include "project-conf.h"

#if CONFIG_DEBUG
#define AWA_ASSERT(x) \
    if ((x) != AwaError_Success) { \
        debug_hang_up(false); \
    }
#else
#define AWA_ASSERT(x) x;
#endif

#if CONFIG_DEBUG
#define LOG(fmt, ...) do { printf("%s:%d:%s(): " fmt, __FILE__, __LINE__, \
    __func__,  ##__VA_ARGS__); } while (0)
#define LOGE(fmt, ...) LOG("ERROR " fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) LOG("Warning " fmt, ##__VA_ARGS__)
#else
#define LOG(fmt, ...)
#define LOGE(fmt, ...)
#define LOGW(fmt, ...)
#endif

void debug_hang_up(bool success);

#endif
