/* SPDX-License-Identifier: GPL-2.0-or-later */
/* 
 * Copyright (C) 2023 bmax121. All Rights Reserved.
 */

#ifndef _KP_LOG_H_
#define _KP_LOG_H_

#include <stdint.h>

#define PREFIX_MAX 48
#define LOG_LINE_MAX (1024 - PREFIX_MAX)

extern void (*printk)(const char *fmt, ...);

/*
 * Runtime log gate, controllable from the manager app via
 * SUPER_CALL: control_feature(superkey, "log", state)
 *
 * Boolean semantics (matches Natives.controlFeature(name, enable)):
 *   state = 0  (off)  -> kp_log_level = WARN : only W/E are printed
 *   state = 1  (on)        -> kp_log_level = VERB : W/E/I/D/V all printed
 *   state < 0  -> query (returns kp_log_level)
 *
 * All levels are compiled in (KP_LOG_COMPILE_LEVEL = VERB) so the switch can
 * turn on I/D/V at runtime; every call site pays a single global read +
 * compare when the gate is active.  printk() arguments are only evaluated
 * inside the if-body, so they cost nothing when the gate is closed.
 * Debug builds keep VERB compilation; to permanently shrink the binary set
 * KP_LOG_COMPILE_LEVEL lower (e.g. WARN) but then I/D/V can never turn on.
 */
#define KP_LOG_OFF 0
#define KP_LOG_ERR 1
#define KP_LOG_WARN 2
#define KP_LOG_INFO 3
#define KP_LOG_DEBUG 4
#define KP_LOG_VERB 5

#ifndef KP_LOG_COMPILE_LEVEL
#define KP_LOG_COMPILE_LEVEL KP_LOG_VERB
#endif

extern int kp_log_level;

/* Single shared runtime gate: one global read + compare per call site.
 * printf argument expressions sit inside printk() (the if-body), so they
 * are never evaluated when the gate is false. */
#define _KP_LOG_GATE(lvl) (kp_log_level >= (lvl) && printk)

#if KP_LOG_COMPILE_LEVEL >= KP_LOG_VERB
#define logkv(fmt, ...)                                                       \
    do {                                                                      \
        if (_KP_LOG_GATE(KP_LOG_VERB))                                        \
            printk("[+] KP V " fmt, ##__VA_ARGS__);                           \
    } while (0)
#define logkfv(fmt, ...)                                                       \
    do {                                                                      \
        if (_KP_LOG_GATE(KP_LOG_VERB))                                        \
            printk("[+] KP V %s: " fmt, __func__, ##__VA_ARGS__);             \
    } while (0)
#else
#define logkv(fmt, ...) do {} while (0)
#define logkfv(fmt, ...) do {} while (0)
#endif

#if KP_LOG_COMPILE_LEVEL >= KP_LOG_DEBUG
#define logkd(fmt, ...)                                                       \
    do {                                                                      \
        if (_KP_LOG_GATE(KP_LOG_DEBUG))                                       \
            printk("[+] KP D " fmt, ##__VA_ARGS__);                           \
    } while (0)
#define logkfd(fmt, ...)                                                      \
    do {                                                                      \
        if (_KP_LOG_GATE(KP_LOG_DEBUG))                                       \
            printk("[+] KP D %s: " fmt, __func__, ##__VA_ARGS__);             \
    } while (0)
#else
#define logkd(fmt, ...) do {} while (0)
#define logkfd(fmt, ...) do {} while (0)
#endif

#if KP_LOG_COMPILE_LEVEL >= KP_LOG_INFO
#define logki(fmt, ...)                                                       \
    do {                                                                      \
        if (_KP_LOG_GATE(KP_LOG_INFO))                                        \
            printk("[+] KP I " fmt, ##__VA_ARGS__);                           \
    } while (0)
#define logkfi(fmt, ...)                                                      \
    do {                                                                      \
        if (_KP_LOG_GATE(KP_LOG_INFO))                                        \
            printk("[+] KP I %s: " fmt, __func__, ##__VA_ARGS__);             \
    } while (0)
#else
#define logki(fmt, ...) do {} while (0)
#define logkfi(fmt, ...) do {} while (0)
#endif

#if KP_LOG_COMPILE_LEVEL >= KP_LOG_WARN
#define logkw(fmt, ...)                                                       \
    do {                                                                      \
        if (_KP_LOG_GATE(KP_LOG_WARN))                                        \
            printk("[-] KP W " fmt, ##__VA_ARGS__);                           \
    } while (0)
#define logkfw(fmt, ...)                                                      \
    do {                                                                      \
        if (_KP_LOG_GATE(KP_LOG_WARN))                                        \
            printk("[-] KP W %s: " fmt, __func__, ##__VA_ARGS__);             \
    } while (0)
#else
#define logkw(fmt, ...) do {} while (0)
#define logkfw(fmt, ...) do {} while (0)
#endif

#if KP_LOG_COMPILE_LEVEL >= KP_LOG_ERR
#define logke(fmt, ...)                                                       \
    do {                                                                      \
        if (_KP_LOG_GATE(KP_LOG_ERR))                                         \
            printk("[-] KP E " fmt, ##__VA_ARGS__);                           \
    } while (0)
#define logkfe(fmt, ...)                                                      \
    do {                                                                      \
        if (_KP_LOG_GATE(KP_LOG_ERR))                                         \
            printk("[-] KP E %s: " fmt, __func__, ##__VA_ARGS__);             \
    } while (0)
#else
#define logke(fmt, ...) do {} while (0)
#define logkfe(fmt, ...) do {} while (0)
#endif

void log_boot(const char *fmt, ...);
const char *get_boot_log();
long kp_log_control(int state);

#endif