/* Copyright (C) 2011-2018 Zeex
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define IN_SAMPGDK

#ifndef SAMPGDK_AMALGAMATION
  #define SAMPGDK_AMALGAMATION
#endif

#include "sampgdk.h"

#if SAMPGDK_WINDOWS
  #ifdef _MSC_VER
    #pragma warning(disable: 4996)
  #endif
  #undef CreateMenu
  #undef DestroyMenu
  #undef GetTickCount
  #undef KillTimer
  #undef SelectObject
  #undef SetTimer
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
#endif

#ifndef SAMPGDK_INTERNAL_ARRAY_H
#define SAMPGDK_INTERNAL_ARRAY_H

struct sampgdk_array {
  void *data;
  int   count;
  int   size;
  int   elem_size;
};

/* Element compaison function. */
typedef int (*sampgdk_array_cmp)(const void *x, const void *y);

int sampgdk_array_new(struct sampgdk_array *a, int size, int elem_size);
void sampgdk_array_free(struct sampgdk_array *a);

/* Fills the aray with zeros. */
int sampgdk_array_zero(struct sampgdk_array *a);

int sampgdk_array_resize(struct sampgdk_array *a, int new_size);
int sampgdk_array_grow(struct sampgdk_array *a);
int sampgdk_array_shrink(struct sampgdk_array *a);
int sampgdk_array_pad(struct sampgdk_array *a);

void *sampgdk_array_get(struct sampgdk_array *a, int index);
void sampgdk_array_set(struct sampgdk_array *a,int index, void *elem);

/* Returns the index of the element pointed to by elem. */
int sampgdk_array_get_index(struct sampgdk_array *a, void *elem);

int sampgdk_array_insert(struct sampgdk_array *a, int index,
    int count, void *elems);
int sampgdk_array_append(struct sampgdk_array *a, void *elem);

int sampgdk_array_remove(struct sampgdk_array *a, int index, int count);
int sampgdk_array_clear(struct sampgdk_array *a);

int sampgdk_array_find(struct sampgdk_array *a, const void *key,
    sampgdk_array_cmp cmp);
int sampgdk_array_find_remove(struct sampgdk_array *a, const void *key,
    sampgdk_array_cmp cmp);

#endif /* !SAMPGDK_INTERNAL_ARRAY_H */

#ifndef SAMPGDK_INTERNAL_HOOK_H
#define SAMPGDK_INTERNAL_HOOK_H

struct _sampgdk_hook;
typedef struct _sampgdk_hook *sampgdk_hook_t;

sampgdk_hook_t sampgdk_hook_new(void *src, void *dst);
void sampgdk_hook_free(sampgdk_hook_t hook);
void *sampgdk_hook_trampoline(sampgdk_hook_t hook);

#define SAMPGDK_HOOK_CALL(hook, return_type, args) \
  ((return_type (*)())sampgdk_hook_code(hook))args

#define SAMPGDK_HOOK_CALL_CC(hook, return_type, callconv, types, args) \
  ((return_type (callconv *)types)sampgdk_hook_trampoline(hook))args

#endif /* !SAMPGDK_INTERNAL_HOOK_H */

#ifndef SAMPGDK_INTERNAL_INIT_H
#define SAMPGDK_INTERNAL_INIT_H

#define SAMPGDK_MODULE_INIT(module) \
  int sampgdk_##module##_init(void)

#define SAMPGDK_MODULE_CLEANUP(module) \
  void sampgdk_##module##_cleanup(void)

int  sampgdk_module_init(void);
void sampgdk_module_cleanup(void);

#endif /* !SAMPGDK_INTERNAL_INIT_H */

#ifndef SAMPGDK_INTERNAL_LIKELY_H
#define SAMPGDK_INTERNAL_LIKELY_H

#if defined __GNUC__
  #define SAMPGDK_LIKELY(x)   __builtin_expect((x), 1)
  #define SAMPGDK_UNLIKELY(x) __builtin_expect((x), 0)
#else
  #define SAMPGDK_LIKELY(x)   (x)
  #define SAMPGDK_UNLIKELY(x) (x)
#endif

#endif /* !SAMPGDK_INTERNAL_LIKELY_H */

#ifndef SAMPGDK_INTERNAL_LOG_H
#define SAMPGDK_INTERNAL_LOG_H

#define SAMPGDK_LOG_DEBUG   0
#define SAMPGDK_LOG_INFO    1
#define SAMPGDK_LOG_WARNING 2
#define SAMPGDK_LOG_ERROR   3

/* The SAMPGDK_MIN_LOG_LEVEL macro allows you to remove calls to logging
 * functions of certain levels from the resulting machine code at compile
 * time if you think that you will never need such log verbosity at run
 * time.
 */
#ifndef SAMPGDK_MIN_LOG_LEVEL
  #ifdef NDEBUG
    #define SAMPGDK_MIN_LOG_LEVEL SAMPGDK_LOG_INFO
  #else
    #define SAMPGDK_MIN_LOG_LEVEL SAMPGDK_LOG_DEBUG
  #endif
#endif

#if SAMPGDK_LOG_DEBUG >= SAMPGDK_MIN_LOG_LEVEL
  #define sampgdk_log_debug(...) \
    sampgdk_log_message(SAMPGDK_LOG_DEBUG, __VA_ARGS__)
#else
  #define sampgdk_log_debug(...)
#endif

#if SAMPGDK_LOG_INFO >= SAMPGDK_MIN_LOG_LEVEL
  #define sampgdk_log_info(...) \
    sampgdk_log_message(SAMPGDK_LOG_INFO, __VA_ARGS__)
#else
  #define sampgdk_log_info(...)
#endif

#if SAMPGDK_LOG_WARNING >= SAMPGDK_MIN_LOG_LEVEL
  #define sampgdk_log_warning(...) \
    sampgdk_log_message(SAMPGDK_LOG_WARNING, __VA_ARGS__)
#else
  #define sampgdk_log_warning(...)
#endif

#if SAMPGDK_LOG_ERROR >= SAMPGDK_MIN_LOG_LEVEL
  #define sampgdk_log_error(...) \
    sampgdk_log_message(SAMPGDK_LOG_ERROR, __VA_ARGS__)
#else
  #define sampgdk_log_error(...)
#endif

void sampgdk_log_message(int level, const char *format, ...);

#endif /* !SAMPGDK_INTERNAL_LOG_H */

#ifndef SAMPGDK_INTERNAL_LOGPRINTF_H
#define SAMPGDK_INTERNAL_LOGPRINTF_H

#include <stdarg.h>

#define SAMPGDK_LOGPRINTF_BUFFER_SIZE 1024

typedef void (*logprintf_t)(const char *format, ...);

extern logprintf_t sampgdk_logprintf_impl;

void sampgdk_do_vlogprintf(const char *format, va_list va);

#endif /* !SAMPGDK_INTERNAL_LOGPRINTF_H */

#ifndef SAMPGDK_INTERNAL_TYPES_H
#define SAMPGDK_INTERNAL_TYPES_H

#if defined _MSC_VER && _MSC_VER <= 1600
  typedef __int8 int8_t;
  typedef unsigned __int8 uint8_t;
  typedef __int16 int16_t;
  typedef unsigned __int16 uint16_t;
  typedef __int32 int32_t;
  typedef unsigned __int32 uint32_t;
#else
  #include <stdint.h>
#endif

#if SAMPGDK_WINDOWS
  #ifndef PRId32
    #define PRId32 "I32d"
  #endif
  #ifndef PRId64
    #define PRId64 "I64d"
  #endif
#else
  #include <inttypes.h>
#endif

#endif /* !SAMPGDK_INTERNAL_TYPES_H */

int sampgdk_amxhooks_init(void);
int sampgdk_callback_init(void);
int sampgdk_fakeamx_init(void);
int sampgdk_log_init(void);
int sampgdk_native_init(void);
int sampgdk_plugin_init(void);
int sampgdk_timer_init(void);
int sampgdk_a_http_init(void);
int sampgdk_a_objects_init(void);
int sampgdk_a_players_init(void);
int sampgdk_a_samp_init(void);
int sampgdk_omp_actor_init(void);
int sampgdk_omp_checkpoint_init(void);
int sampgdk_omp_class_init(void);
int sampgdk_omp_core_init(void);
int sampgdk_omp_database_init(void);
int sampgdk_omp_dialog_init(void);
int sampgdk_omp_gangzone_init(void);
int sampgdk_omp_http_init(void);
int sampgdk_omp_menu_init(void);
int sampgdk_omp_network_init(void);
int sampgdk_omp_npc_init(void);
int sampgdk_omp_object_init(void);
int sampgdk_omp_pickup_init(void);
int sampgdk_omp_player_init(void);
int sampgdk_omp_textdraw_init(void);
int sampgdk_omp_textlabel_init(void);
int sampgdk_omp_variable_init(void);
int sampgdk_omp_vehicle_init(void);

int sampgdk_module_init(void) {
  int error;
  if ((error = sampgdk_amxhooks_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_fakeamx_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_log_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_native_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_plugin_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_timer_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_a_http_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_a_objects_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_a_players_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_a_samp_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_actor_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_checkpoint_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_class_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_core_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_database_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_dialog_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_gangzone_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_http_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_menu_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_network_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_npc_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_object_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_pickup_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_player_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_textdraw_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_textlabel_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_variable_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_omp_vehicle_init()) < 0) {
    return error;
  }
  return 0;
}

void sampgdk_omp_vehicle_cleanup(void);
void sampgdk_omp_variable_cleanup(void);
void sampgdk_omp_textlabel_cleanup(void);
void sampgdk_omp_textdraw_cleanup(void);
void sampgdk_omp_player_cleanup(void);
void sampgdk_omp_pickup_cleanup(void);
void sampgdk_omp_object_cleanup(void);
void sampgdk_omp_npc_cleanup(void);
void sampgdk_omp_network_cleanup(void);
void sampgdk_omp_menu_cleanup(void);
void sampgdk_omp_http_cleanup(void);
void sampgdk_omp_gangzone_cleanup(void);
void sampgdk_omp_dialog_cleanup(void);
void sampgdk_omp_database_cleanup(void);
void sampgdk_omp_core_cleanup(void);
void sampgdk_omp_class_cleanup(void);
void sampgdk_omp_checkpoint_cleanup(void);
void sampgdk_omp_actor_cleanup(void);
void sampgdk_a_samp_cleanup(void);
void sampgdk_a_players_cleanup(void);
void sampgdk_a_objects_cleanup(void);
void sampgdk_a_http_cleanup(void);
void sampgdk_timer_cleanup(void);
void sampgdk_plugin_cleanup(void);
void sampgdk_native_cleanup(void);
void sampgdk_log_cleanup(void);
void sampgdk_fakeamx_cleanup(void);
void sampgdk_callback_cleanup(void);
void sampgdk_amxhooks_cleanup(void);

void sampgdk_module_cleanup(void) {
  sampgdk_omp_vehicle_cleanup();
  sampgdk_omp_variable_cleanup();
  sampgdk_omp_textlabel_cleanup();
  sampgdk_omp_textdraw_cleanup();
  sampgdk_omp_player_cleanup();
  sampgdk_omp_pickup_cleanup();
  sampgdk_omp_object_cleanup();
  sampgdk_omp_npc_cleanup();
  sampgdk_omp_network_cleanup();
  sampgdk_omp_menu_cleanup();
  sampgdk_omp_http_cleanup();
  sampgdk_omp_gangzone_cleanup();
  sampgdk_omp_dialog_cleanup();
  sampgdk_omp_database_cleanup();
  sampgdk_omp_core_cleanup();
  sampgdk_omp_class_cleanup();
  sampgdk_omp_checkpoint_cleanup();
  sampgdk_omp_actor_cleanup();
  sampgdk_a_samp_cleanup();
  sampgdk_a_players_cleanup();
  sampgdk_a_objects_cleanup();
  sampgdk_a_http_cleanup();
  sampgdk_timer_cleanup();
  sampgdk_plugin_cleanup();
  sampgdk_native_cleanup();
  sampgdk_log_cleanup();
  sampgdk_fakeamx_cleanup();
  sampgdk_callback_cleanup();
  sampgdk_amxhooks_cleanup();
}


#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* #include "array.h" */

static void *_sampgdk_array_get_elem_ptr(struct sampgdk_array *a, int index) {
  return (unsigned char *)a->data + (index * a->elem_size);
}

static int _sampgdk_array_normalize_index(struct sampgdk_array *a, int index) {
  if (index >= 0) {
    return index;
  } else {
    return a->count + index;
  }
}

int sampgdk_array_new(struct sampgdk_array *a,
                      int size,
                      int elem_size) {
  assert(a != NULL);
  assert(size > 0);
  assert(elem_size > 0);

  if ((a->data = malloc(elem_size * size)) == NULL) {
    return -ENOMEM;
  }

  a->size = size;
  a->elem_size = elem_size;
  a->count = 0;

  return 0;
}

void sampgdk_array_free(struct sampgdk_array *a) {
  assert(a != NULL);

  free(a->data);
  memset(a, 0, sizeof(*a));
}

int sampgdk_array_zero(struct sampgdk_array *a) {
  assert(a != NULL);

  if (a->data == NULL) {
    return -EINVAL;
  }

  memset(a->data, 0, a->size * a->elem_size);
  return 0;
}

int sampgdk_array_resize(struct sampgdk_array *a, int new_size) {
  void *new_data;

  assert(a != NULL);
  assert(new_size >= 0);
  assert(a->elem_size > 0);

  if (new_size == a->size) {
    return 0;
  }

  if (new_size > 0) {
    new_data = realloc(a->data, a->elem_size * new_size);

    if (new_data == NULL) {
      return -errno;
    }
  } else {
    free(a->data);
    new_data = NULL;
  }

  a->data = new_data;
  a->size = new_size;

  if (a->count > a->size) {
    a->count = a->size;
  }

  return 0;
}

int sampgdk_array_grow(struct sampgdk_array *a) {
  assert(a != NULL);

  if (a->size == 0) {
    return sampgdk_array_resize(a, 1);
  }

  return sampgdk_array_resize(a, (int)(a->size * 2));
}

int sampgdk_array_shrink(struct sampgdk_array *a) {
  assert(a != NULL);

  return sampgdk_array_resize(a, a->count);
}

int sampgdk_array_pad(struct sampgdk_array *a) {
  int space;

  assert(a != NULL);

  if ((space = a->size - a->count) <= 0) {
    return space;
  }

  return (a->count = a->size);
}

void *sampgdk_array_get(struct sampgdk_array *a, int index) {
  assert(a != NULL);

  index = _sampgdk_array_normalize_index(a, index);
  assert(index < a->count);

  return _sampgdk_array_get_elem_ptr(a, index);
}

void sampgdk_array_set(struct sampgdk_array *a, int index, void *elem) {
  assert(a != NULL);

  index = _sampgdk_array_normalize_index(a, index);
  assert(index < a->count);

  memcpy(_sampgdk_array_get_elem_ptr(a, index), elem, a->elem_size);
}

int sampgdk_array_insert(struct sampgdk_array *a,
                         int index,
                         int count,
                         void *elems) {
  int need_count;
  int move_count;

  assert(a != NULL);
  assert(elems != NULL);

  if (count <= 0) {
    return -EINVAL;
  }

  index = _sampgdk_array_normalize_index(a, index);
  assert(index <= a->count);

  need_count = a->count + count - a->size;
  move_count = a->count - index;

  if (need_count > 0) {
    int error;
    int need_size = a->size + need_count;

    do {
      error = sampgdk_array_grow(a);
      if (error < 0) {
        return error;
      }
    } while (a->size < need_size);
  }

  if (move_count > 0) {
    memmove(_sampgdk_array_get_elem_ptr(a, index + count),
            _sampgdk_array_get_elem_ptr(a, index),
            move_count * a->elem_size);
  }

  a->count += count;
  memcpy(_sampgdk_array_get_elem_ptr(a, index), elems, count * a->elem_size);

  return index;
}

int sampgdk_array_remove(struct sampgdk_array *a, int index, int count) {
  int move_count;

  assert(a != NULL);

  index = _sampgdk_array_normalize_index(a, index);
  assert(index < a->count);

  if (count <= 0 || count > a->count - index) {
    return -EINVAL;
  }

  move_count = a->count - index - count;

  if (move_count > 0) {
    memmove(_sampgdk_array_get_elem_ptr(a, index),
            _sampgdk_array_get_elem_ptr(a, index + count),
            move_count * a->elem_size);
   }

  a->count -= count;

  if (a->count <= a->size / 4) {
    return sampgdk_array_resize(a, a->size / 2);
  }

  return index;
}

int sampgdk_array_clear(struct sampgdk_array *a) {
  return sampgdk_array_remove(a, 0, a->count);
}

int sampgdk_array_append(struct sampgdk_array *a, void *elem) {
  assert(a != NULL);
  assert(elem != NULL);

  if (a->count == a->size) {
    int error;

    if ((error = sampgdk_array_grow(a)) < 0) {
      return error;
    }
  }

  a->count++;
  sampgdk_array_set(a, a->count - 1, elem);

  return a->count - 1;
}

int sampgdk_array_get_index(struct sampgdk_array *a, void *elem) {
  assert(elem != NULL);
  return (int)(((unsigned char *)elem - (unsigned char *)a->data) / a->elem_size);
}

int sampgdk_array_find(struct sampgdk_array *a,
                       const void *key,
                       sampgdk_array_cmp cmp) {
  int index;
  void *cur;

  assert(a != NULL);
  assert(cmp != NULL);

  for (index = 0; index < a->count; index++) {
    cur = sampgdk_array_get(a, index);
    if (cmp(key, cur) == 0) {
      return index;
    }
  }

  return -EINVAL;
}

int sampgdk_array_find_remove(struct sampgdk_array *a,
                              const void *key,
                              sampgdk_array_cmp cmp) {
  int index;
  void *cur;

  assert(a != NULL);
  assert(cmp != NULL);

  for (index = 0; index < a->count; index++) {
    cur = sampgdk_array_get(a, index);
    if (cmp(key, cur) == 0) {
      sampgdk_array_remove(a, index, 1);
      return index;
    }
  }

  return -EINVAL;
}

#ifndef SAMPGDK_INTERNAL_PLUGIN_H
#define SAMPGDK_INTERNAL_PLUGIN_H

#include "sampgdk.h"

int sampgdk_plugin_register(void *plugin);
int sampgdk_plugin_unregister(void *plugin);

void *sampgdk_plugin_get_symbol(void *plugin, const char *name);
void *sampgdk_plugin_get_handle(void *address);

/* Returns all currently registered plugins. */
void **sampgdk_plugin_get_plugins(int *number);

#endif /* !SAMPGDK_INTERNAL_PLUGIN_H */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sampgdk.h"

/* #include "init.h" */
/* #include "log.h" */
/* #include "logprintf.h" */

static bool _sampgdk_log_enabled[] = {
  false, /* _SAMPGDK_LOG_DEBUG */
  false, /* _SAMPGDK_LOG_INFO */
  true,  /* _SAMPGDK_LOG_WARNING */
  true , /* _SAMPGDK_LOG_ERROR */
};

static void _sampgdk_log_init_enabled() {
  char *levels;
  char c;
  char op = '\0';

  if ((levels = getenv("SAMPGDK_LOG")) == NULL) {
    return;
  }

  while ((c = *levels++) != '\0') {
    int level = -1;

    switch (c) {
      case '+':
      case '-':
        op = c;
        break;
      case 'd':
        level = SAMPGDK_LOG_DEBUG;
        break;
      case 'i':
        level = SAMPGDK_LOG_INFO;
        break;
      case 'w':
        level = SAMPGDK_LOG_WARNING;
        break;
      case 'e':
        level = SAMPGDK_LOG_ERROR;
        break;
    }

    if (level >= 0) {
      if (op == '+') {
        _sampgdk_log_enabled[level] = true;
      } else if (op == '-') {
        _sampgdk_log_enabled[level] = false;
      }
    }
  }
}

SAMPGDK_MODULE_INIT(log) {
  _sampgdk_log_init_enabled();
  return 0;
}

SAMPGDK_MODULE_CLEANUP(log) {
  /* nothing to do here */
}

void sampgdk_log_message(int level, const char *format, ...) {
  va_list args;
  char level_char;
  char log_format_buf[SAMPGDK_LOGPRINTF_BUFFER_SIZE];

  assert(level >= SAMPGDK_LOG_DEBUG &&
         level <= SAMPGDK_LOG_ERROR);

  if (!_sampgdk_log_enabled[level]) {
    return;
  }

  switch (level) {
    case SAMPGDK_LOG_DEBUG:
      level_char = 'd';
      break;
    case SAMPGDK_LOG_INFO:
      level_char = 'i';
      break;
    case SAMPGDK_LOG_WARNING:
      level_char = 'w';
      break;
    case SAMPGDK_LOG_ERROR:
      level_char = 'e';
      break;
    default:
      return;
  }

#ifdef _MSC_VER
  _snprintf(
#else
  snprintf(
#endif
      log_format_buf,
      sizeof(log_format_buf),
      "[sampgdk:%c] %s",
      level_char,
      format);
#ifdef _MSC_VER
  /* _snprintf does not insert a terminating NUL if the buffer is too small */
  log_format_buf[sizeof(log_format_buf) - 1] = '\0';
#endif
  va_start(args, format);
  sampgdk_do_vlogprintf(log_format_buf, args);
  va_end(args);
}

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "sampgdk.h"

#if SAMPGDK_WINDOWS
  #include <windows.h>
#else
  #include <stdint.h>
  #include <unistd.h>
  #include <sys/mman.h>
#endif

/* #include "log.h" */
/* #include "hook.h" */
/* #include "types.h" */

#ifdef SAMPGDK_64BIT
#  define _SAMPGDK_HOOK_JMP_SIZE 14
#else
#  define _SAMPGDK_HOOK_JMP_SIZE 5
#endif
#define _SAMPGDK_HOOK_MAX_INSN_LEN 15
/* Address-size displacement used by MOV moffs (A0-A3): 64-bit default 8,
 * 4 under the 0x67 address-size override; 4 on 32-bit (2 under 0x67). */
#ifdef SAMPGDK_64BIT
#  define _SAMPGDK_HOOK_ADDR_SIZE(addr32) ((addr32) ? 4 : 8)
#else
#  define _SAMPGDK_HOOK_ADDR_SIZE(addr32) ((addr32) ? 2 : 4)
#endif
/* Trampoline must hold: copied instructions (up to JMP_SIZE-1+MAX_INSN_LEN)
 * + back-jump (JMP_SIZE).  The original formula (JMP_SIZE+MAX_INSN_LEN-1)
 * never accounted for the back-jump. */
#define _SAMPGDK_HOOK_TRAMPOLINE_SIZE \
  (_SAMPGDK_HOOK_JMP_SIZE * 2 + _SAMPGDK_HOOK_MAX_INSN_LEN - 1)

#pragma pack(push, 1)

#ifdef SAMPGDK_64BIT
/* FF 25 00 00 00 00 = jmp [rip+0] (6B) + 8B absolute address */
struct _sampgdk_hook_jmp {
  uint8_t  opcode;    /* 0xFF */
  uint8_t  modrm;     /* 0x25 */
  int32_t  disp;      /* 0 */
  uintptr_t target;   /* 8-byte absolute address */
};
#else
struct _sampgdk_hook_jmp {
  uint8_t opcode;     /* 0xE9 */
  int32_t offset;
};
#endif

#pragma pack(pop)

struct _sampgdk_hook {
  uint8_t trampoline[_SAMPGDK_HOOK_TRAMPOLINE_SIZE];
};

#if SAMPGDK_WINDOWS

static void *_sampgdk_hook_unprotect(void *address, size_t size) {
  DWORD old;

  if (VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &old) == 0) {
    return NULL;
  }

  return address;
}

#else /* SAMPGDK_WINDOWS */

static void *_sampgdk_hook_unprotect(void *address, size_t size) {
  long pagesize;

  pagesize = sysconf(_SC_PAGESIZE);
  address = (void *)((uintptr_t)address & ~((uintptr_t)(pagesize - 1)));

  if (mprotect(address, size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
    return NULL;
  }

  return address;
}

#endif /* !SAMPGDK_WINDOWS */

static size_t _sampgdk_hook_disasm(uint8_t *code, int *reloc) {
  /* Length-only disassembler for x86/x64.
   * Derives instruction length from encoding rules, not from an opcode table.
   * Based on the standard approach used by Detours/mhook etc.
   */
  int len = 0;

  int opsize16 = 0;  /* 0x66 operand-size override */
  int addr32 = 0;    /* 0x67 address-size override */

  /* Consume legacy prefixes */
  while (1) {
    uint8_t b = code[len];
    /* LOCK/REPNE/REPE */  if (b == 0xF0 || b == 0xF2 || b == 0xF3) { len++; continue; }
    /* CS/SS/DS/ES/FS/GS */ if (b == 0x2E || b == 0x36 || b == 0x3E || b == 0x26 || b == 0x64 || b == 0x65) { len++; continue; }
    /* Operand-size override */ if (b == 0x66) { len++; opsize16 = 1; continue; }
    /* Address-size override */ if (b == 0x67) { len++; addr32 = 1; continue; }
    break;
  }

#ifdef SAMPGDK_64BIT
  int rex = 0;
  /* REX prefix (0x40-0x4F) */
  if ((code[len] & 0xF0) == 0x40) {
    rex = code[len] & 0x0F;  /* save full REX for W bit */
    len++;
  }
#else
  int rex = 0;
#endif

  /* VEX/EVEX/XOP prefixes (64-bit only). Skip the whole prefix so the
   * embedded opcode (right after the last prefix byte) is decoded. */
  int vex_map = -1;   /* VEX.pp[1:0] selects the opcode map:
                       * 0=0F, 1=0F38, 2=0F3A, 3=unused */
#ifdef SAMPGDK_64BIT
  if (code[len] == 0x62) {
    /* EVEX: 62 P0 P1 P2 <opcode>. The opcode map is in P0[2:0]
     * (P0 is the byte after the 0x62): 1=0F, 2=0F38, 3=0F3A. */
    vex_map = code[len + 1] & 7;
    len += 4;
  } else if (code[len] == 0xC4) {
    /* VEX 3-byte: C4 P1 P2 <opcode>. Map in P1[2:0]: 1=0F,2=0F38,3=0F3A. */
    vex_map = code[len + 1] & 7;
    len += 3;
  } else if (code[len] == 0xC5) {
    /* VEX 2-byte: C5 P1 <opcode> (map is always 0F) */
    vex_map = 1;  /* 0F map */
    len += 2;
  }
#endif

  /* Check for 2-byte opcode (0x0F prefix) */
  int two_byte = 0;
  int map = 0;   /* 0=1-byte, 1=0F, 2=0F38, 3=0F3A */
  if (code[len] == 0x0F) {
    two_byte = 1;
    len++;
    if (code[len] == 0x38 || code[len] == 0x3A) {
      map = (code[len] == 0x38) ? 2 : 3;
      len++;
    } else {
      map = 1;
    }
  }

  /* Read the primary opcode byte */
  int opcode = code[len++];

  /* Determine if this instruction has ModRM.
   * On x86/x64, most opcodes have ModRM. The exceptions are:
   * - Opcodes 0x00-0x03: ADD/OR/ADC/SBB with ModRM (actually all have ModRM up to 0x3F)
   * - Actually, most opcodes have ModRM. The few that don't:
   *   0x40-0x4F: INC/DEC (x86) / REX prefix (x64 - already handled)
   *   0x50-0x5F: PUSH/POP r32 (PLUS_R style, no ModRM)
   *   0x60-0x6F: various (some have ModRM, some don't)
   *   0x70-0x7F: Jcc rel8 (no ModRM)
   *   0x90-0x97: XCHG/NOP (no ModRM)
   *   0x98-0x9F: various flags/convert (no ModRM)
   *   0xA0-0xA7: MOV moffs (no ModRM, but AL/EAX specific)
   *   0xA8-0xAF: TEST/STOS/LODS/SCAS (no ModRM)
   *   0xB0-0xBF: MOV r8/r32, imm (PLUS_R style, no ModRM)
   *   0xC0-0xC1: Shift/rotate r/m8/32, imm8 (ModRM)
   *   0xC2-0xC3: RET (no ModRM)
   *   0xC4-0xC5: LES/LDS / VEX prefix (no ModRM)
   *   0xC6-0xC7: MOV r/m8/32, imm (ModRM)
   *   0xC8-0xC9: ENTER/LEAVE (no ModRM)
   *   0xCA-0xCB: RET far (no ModRM)
   *   0xCC-0xCE: INT/INTO/IRET (no ModRM)
   *   0xD0-0xD3: Shift/rotate r/m8/32 (ModRM)
   *   0xD4-0xD5: AAM/AAD (no ModRM)
   *   0xD6: SETALC (undocumented, no ModRM)
   *   0xD7: XLAT (no ModRM)
   *   0xE0-0xE3: LOOP/JCXZ (no ModRM)
   *   0xE4-0xE7: IN/OUT (no ModRM)
   *   0xE8-0xE9: CALL/JMP rel (no ModRM)
   *   0xEA-0xEB: JMP far / JMP rel8 (no ModRM)
   *   0xEC-0xEF: IN/OUT (no ModRM)
   *   0xF0-0xF3: LOCK/REP prefixes (already consumed)
   *   0xF4: HLT (no ModRM)
   *   0xF5: CMC (no ModRM)
   *   0xF6-0xF7: Group3 (ModRM)
   *   0xF8-0xFD: CLC/STC/CLI/STI/CLD/STD (no ModRM)
   *   0xFE-0xFF: Group4/5 (ModRM)
   */

  int has_modrm = 1;  /* default: most instructions have ModRM */
  int imm_size = 0;
  int moffs_size = 0;    /* MOV moffs address-size displacement */
  int reloc_offset = 0;
  int riprel = 0;        /* ModRM mod=00 rm=101 on x64 (RIP-relative) */
  int group3 = 0;        /* F6/F7 Group3 opcode, imm decided by ModRM.reg */

  /* Determine ModRM and immediate based on opcode */
  /* Single-byte opcodes (not 0x0F prefix) */
  if (!two_byte) {
    if ((opcode >= 0x00 && opcode <= 0x03) ||  /* ADD/OR/ADC/SBB r/m8,r8 */
        (opcode >= 0x08 && opcode <= 0x0B) ||  /* OR/ADC/SBB/AND r/m8,r8 */
        (opcode >= 0x10 && opcode <= 0x13) ||  /* ADC/SBB/AND/SUB r/m8,r8 */
        (opcode >= 0x18 && opcode <= 0x1B) ||  /* SBB/AND/SUB/XOR r/m8,r8 */
        (opcode >= 0x20 && opcode <= 0x23) ||  /* AND/SUB/XOR/CMP r/m8,r8 */
        (opcode >= 0x28 && opcode <= 0x2B) ||  /* SUB/XOR/CMP/ADD r/m8,r8 */
        (opcode >= 0x30 && opcode <= 0x33) ||  /* XOR/CMP/ADD/OR r/m8,r8 */
        (opcode >= 0x38 && opcode <= 0x3B)) {  /* CMP/ADD/OR/ADC r/m8,r8 */
      has_modrm = 1; /* register/memory forms */
    }
    else if ((opcode == 0x04 || opcode == 0x05) ||  /* ADD AL/EAX, imm */
             (opcode == 0x0C || opcode == 0x0D) ||  /* OR  AL/EAX, imm */
             (opcode == 0x14 || opcode == 0x15) ||  /* ADC AL/EAX, imm */
             (opcode == 0x1C || opcode == 0x1D) ||  /* SBB AL/EAX, imm */
             (opcode == 0x24 || opcode == 0x25) ||  /* AND AL/EAX, imm */
             (opcode == 0x2C || opcode == 0x2D) ||  /* SUB AL/EAX, imm */
             (opcode == 0x34 || opcode == 0x35) ||  /* XOR AL/EAX, imm */
             (opcode == 0x3C || opcode == 0x3D)) {  /* CMP AL/EAX, imm */
      has_modrm = 0;
      imm_size = (opcode & 1) ? 4 : 1; /* EAX forms imm32, AL forms imm8 */
    }
    else if (opcode == 0x06 || opcode == 0x07 ||  /* PUSH/POP ES */
             opcode == 0x0E ||                   /* PUSH CS */
             opcode == 0x16 || opcode == 0x17 ||  /* PUSH/POP SS */
             opcode == 0x1E || opcode == 0x1F ||  /* PUSH/POP DS */
             opcode == 0x27 || opcode == 0x2F ||  /* DAA/DAS */
             opcode == 0x37 || opcode == 0x3F) {  /* AAA/AAS */
      has_modrm = 0; /* one-byte, no ModRM */
    }
    else if (opcode >= 0x40 && opcode <= 0x4F) {
      has_modrm = 0; /* INC/DEC r32 (x86) - on x64 these are REX prefixes, already handled */
    }
    else if (opcode >= 0x50 && opcode <= 0x5F) {
      has_modrm = 0; /* PUSH/POP r32 */
    }
    else if (opcode >= 0x60 && opcode <= 0x6F) {
      if (opcode == 0x60 || opcode == 0x61) {
        has_modrm = 0; /* PUSHA/POPA (x86) */
      } else if (opcode == 0x62) {
        has_modrm = 1; /* BOUND (x86); 64-bit EVEX prefix already consumed */
      } else if (opcode == 0x63) {
        has_modrm = 1; /* MOVSXD (x64) / ARPL (x86) */
      } else if (opcode == 0x68) {
        has_modrm = 0; imm_size = 4; /* PUSH imm32 */
      } else if (opcode == 0x69) {
        has_modrm = 1; imm_size = 4; /* IMUL r32,r/m32,imm32 */
      } else if (opcode == 0x6A) {
        has_modrm = 0; imm_size = 1; /* PUSH imm8 */
      } else if (opcode == 0x6B) {
        has_modrm = 1; imm_size = 1; /* IMUL r32,r/m32,imm8 */
      } else {
        has_modrm = 0; /* INS/OUTS (0x6C-0x6F); 0x64-0x67 prefixes consumed */
      }
    }
    else if (opcode >= 0x70 && opcode <= 0x7F) {
      has_modrm = 0;
      imm_size = 1; /* Jcc rel8 */
    }
    else if (opcode >= 0x80 && opcode <= 0x83) {
      /* Group1 r/m, imm (0x82 is an alias of 0x80: CMP r/m8, imm8) */
      has_modrm = 1;
      imm_size = (opcode == 0x80 || opcode == 0x82 || opcode == 0x83) ? 1 : 4;
    }
    else if (opcode == 0x84 || opcode == 0x85) {
      has_modrm = 1; /* TEST r/m8,r8 / TEST r/m32,r32 */
    }
    else if (opcode == 0x86 || opcode == 0x87) {
      has_modrm = 1; /* XCHG r8,r/m8 / XCHG r32,r/m32 */
    }
    else if (opcode >= 0x88 && opcode <= 0x8F) {
      has_modrm = 1; /* MOV/LEA/POP r/m (0x8F: POP r/m, reg field is 0) */
    }
    else if (opcode >= 0x90 && opcode <= 0x97) {
      has_modrm = 0; /* NOP/XCHG */
    }
    else if (opcode >= 0x98 && opcode <= 0x9F) {
      has_modrm = 0; /* CBW/CWD/CDQ/CWDE/CDQE/WAIT/PUSHF/POPF/SAHF/LAHF */
      if (opcode == 0x9A) imm_size = 6; /* CALL far ptr16:32 */
    }
    else if (opcode >= 0xA0 && opcode <= 0xA3) {
      has_modrm = 0; /* MOV AL/EAX/RAX, moffs / MOV moffs, AL/EAX/RAX */
      moffs_size = _SAMPGDK_HOOK_ADDR_SIZE(addr32);
    }
    else if (opcode >= 0xA4 && opcode <= 0xA7) {
      has_modrm = 0; /* MOVS/CMPS */
    }
    else if (opcode == 0xA8 || opcode == 0xA9) {
      has_modrm = 0; /* TEST AL/EAX, imm */
      imm_size = (opcode == 0xA9) ? 4 : 1;
    }
    else if (opcode >= 0xAA && opcode <= 0xAF) {
      has_modrm = 0; /* STOS/LODS/SCAS */
    }
    else if (opcode >= 0xB0 && opcode <= 0xB7) {
      has_modrm = 0; imm_size = 1; /* MOV r8, imm8 */
    }
    else if (opcode >= 0xB8 && opcode <= 0xBF) {
      has_modrm = 0; /* MOV r32/r64, imm32/imm64 */
      if (rex & 0x08) imm_size = 8;      /* REX.W: MOV r64, imm64 */
      else imm_size = 4;
    }
    else if (opcode >= 0xC0 && opcode <= 0xC1) {
      has_modrm = 1; imm_size = 1; /* Shift/rotate r/m8/32, imm8 */
    }
    else if (opcode >= 0xC2 && opcode <= 0xC3) {
      has_modrm = 0;
      if (opcode == 0xC2) imm_size = 2; /* RET imm16 */
    }
    else if (opcode == 0xC4 || opcode == 0xC5) {
      has_modrm = 1; /* LES/LDS (x86); 64-bit VEX prefixes already consumed */
    }
    else if (opcode >= 0xC6 && opcode <= 0xC7) {
      has_modrm = 1; /* MOV r/m8, imm8 / MOV r/m32, imm32 */
      imm_size = (opcode == 0xC6) ? 1 : 4;
    }
    else if (opcode >= 0xC8 && opcode <= 0xCF) {
      has_modrm = 0;
      if (opcode == 0xC8) imm_size = 3; /* ENTER imm16,imm8 */
      else if (opcode == 0xCA) imm_size = 2; /* RET far imm16 */
      else if (opcode == 0xCD) imm_size = 1; /* INT imm8 */
    }
    else if (opcode >= 0xD0 && opcode <= 0xD3) {
      has_modrm = 1; /* Shift/rotate r/m, 1/CL/imm8 */
    }
    else if (opcode >= 0xD4 && opcode <= 0xD7) {
      has_modrm = 0; /* AAM/AAD/SETALC/XLAT */
      if (opcode == 0xD4 || opcode == 0xD5) imm_size = 1; /* AAM/AAD imm8 */
    }
    else if (opcode >= 0xD8 && opcode <= 0xDF) {
      has_modrm = 1; /* FPU instructions */
    }
    else if (opcode >= 0xE0 && opcode <= 0xE3) {
      has_modrm = 0; imm_size = 1; /* LOOP/JCXZ rel8 */
    }
    else if (opcode == 0xE4 || opcode == 0xE5) {
      has_modrm = 0; imm_size = 1; /* IN AL/EAX, imm8 */
    }
    else if (opcode == 0xE6 || opcode == 0xE7) {
      has_modrm = 0; imm_size = 1; /* OUT imm8, AL/EAX */
    }
    else if (opcode == 0xE8) {
      has_modrm = 0; imm_size = 4; reloc_offset = len; /* CALL rel32 */
    }
    else if (opcode == 0xE9) {
      has_modrm = 0; imm_size = 4; reloc_offset = len; /* JMP rel32 */
    }
    else if (opcode == 0xEA || opcode == 0xEB) {
      has_modrm = 0;
      imm_size = (opcode == 0xEB) ? 1 : 6; /* JMP rel8 / JMP far ptr16:32 */
    }
    else if (opcode >= 0xEC && opcode <= 0xEF) {
      has_modrm = 0; /* IN/OUT AL/EAX, DX */
    }
    else if (opcode == 0xF1) {
      has_modrm = 0; /* INT1/ICEBP */
    }
    else if (opcode == 0xF4) {
      has_modrm = 0; /* HLT */
    }
    else if (opcode == 0xF5) {
      has_modrm = 0; /* CMC */
    }
    else if (opcode >= 0xF6 && opcode <= 0xF7) {
      has_modrm = 1;
      group3 = opcode; /* Group3: imm present only for /0 /1 (TEST) */
    }
    else if (opcode >= 0xF8 && opcode <= 0xFD) {
      has_modrm = 0; /* CLC/STC/CLI/STI/CLD/STD */
    }
    else if (opcode >= 0xFE && opcode <= 0xFF) {
      has_modrm = 1; /* Group4/5 (INC/DEC/CALL/JMP r/m) */
    }
  } else {
    /* 0x0F-prefixed (two-byte) opcodes */
    if (opcode == 0x05 || opcode == 0x06 || opcode == 0x07 ||  /* SYSCALL/CLTS/SYSRET */
        opcode == 0x08 || opcode == 0x09 ||  /* INVD/WBINVD */
        opcode == 0x0B || opcode == 0x0E ||  /* UD2/FEMMS */
        opcode == 0x30 || opcode == 0x31 ||  /* WRMSR/RDTSC */
        opcode == 0x32 || opcode == 0x33 ||  /* RDMSR/RDPMC */
        opcode == 0x34 || opcode == 0x35 ||  /* SYSENTER/SYSEXIT */
        opcode == 0x37 || opcode == 0x77 ||  /* GETSEC/EMMS */
        opcode == 0xA0 || opcode == 0xA1 ||  /* PUSH/POP FS */
        opcode == 0xA2 || opcode == 0xA8 ||  /* CPUID/PUSH GS */
        opcode == 0xA9 || opcode == 0xAA ||  /* POP GS/RSM */
        opcode == 0xB9 ||                     /* UD1 */
        (opcode >= 0xC8 && opcode <= 0xCF)) { /* BSWAP */
      has_modrm = 0;
    } else {
      has_modrm = 1;
      if (opcode >= 0x80 && opcode <= 0x8F) {
        has_modrm = 0; /* Jcc rel32: no ModRM */
        imm_size = 4; /* Jcc rel32 */
        reloc_offset = len;
      }
      if (opcode == 0x70 ||
          (opcode >= 0x71 && opcode <= 0x73) ||
          opcode == 0xA4 || opcode == 0xAC ||
          opcode == 0xBA ||
          opcode == 0xC2 || opcode == 0xC4 ||
          opcode == 0xC5 || opcode == 0xC6) {
        imm_size = 1; /* PSHUFD, PSRLW/PSLLW/PSRAW, SHLD/SHRD, BT, CMPPS/PINSRW/PEXTRW/SHUFPS */
      }
      if (map == 3 && !(opcode >= 0x4A && opcode <= 0x4C)) {
        imm_size = 1; /* 0F 3A map: imm8 except BLENDVPS/VPBLENDVB/VPBLENDVPS */
      }
    }
  }

#ifdef SAMPGDK_64BIT
  /* VEX/EVEX overrides: everything except VZEROUPPER/VZEROALL has ModRM,
   * nothing has a relative branch, and the imm rules follow the map. */
  if (vex_map >= 0) {
    if (vex_map == 1 && opcode == 0x77) {
      has_modrm = 0; /* VZEROUPPER/VZEROALL */
    } else {
      has_modrm = 1;
    }
    reloc_offset = 0;
    imm_size = 0;
    moffs_size = 0;
    group3 = 0;
    if (vex_map == 3 && !(opcode >= 0x4A && opcode <= 0x4C)) {
      imm_size = 1; /* EVEX.0F3A imm8 (BLENDV* variants take XMM0) */
    } else if (vex_map == 1 && (opcode == 0x70 || opcode == 0x71 ||
                                opcode == 0x72 || opcode == 0x73 ||
                                opcode == 0xC2 || opcode == 0xC4 ||
                                opcode == 0xC5 || opcode == 0xC6)) {
      imm_size = 1; /* VEX.0F imm8 forms (incl. vpsrlq/vpsllq shifts) */
    }
  }
#endif

  /* Parse ModRM if present */
  if (has_modrm) {
    int modrm = code[len++];
    int mod = modrm >> 6;
    int rm = modrm & 7;
    int sib = (mod != 3 && rm == 4);

    if (sib) {
      int base = code[len] & 7; /* SIB byte */
      len++;
      if (mod == 0 && base == 5) {
        len += 4; /* SIB base=101 with mod=00: disp32 (absolute address) */
      }
    }

    if (mod == 1) {
      len += 1; /* disp8 */
    } else if (mod == 2) {
      len += 4; /* disp32 */
    } else if (mod == 0 && rm == 5 && !sib) {
      /* RIP-relative on x64, or absolute disp32 on x86 */
      len += 4; /* disp32 */
#ifdef SAMPGDK_64BIT
      if (!addr32) {
        int reg = (modrm >> 3) & 7;
        /* FF 25 00 00 00 00 <8-byte addr>: the absolute indirect jump the
         * hook trampoline itself uses (jmp [rip+0] reads the 8-byte target
         * that immediately follows). This is position-independent: the
         * disp32 stays 0 and the embedded absolute address is copied
         * unchanged, so when a second plugin hooks an already-hooked
         * function, the copied jump keeps chaining to the first plugin.
         * Count the trailing 8-byte target as part of the instruction and
         * do NOT relocate the disp. */
        if (opcode == 0xFF && reg == 4 &&
            code[len - 4] == 0 && code[len - 3] == 0 &&
            code[len - 2] == 0 && code[len - 1] == 0) {
          len += 8; /* embedded 8-byte absolute target */
        } else {
          /* genuine RIP-relative memory operand: the trampoline runs at a
           * different address, so the disp32 must be adjusted by
           * -(trampoline - src) just like rel32 CALL/JMP. mod=00 rm=101
           * with the 0x67 address-size override is absolute (EAX-relative
           * addressing is not RIP-relative). reloc_offset points at the
           * disp32 field (len now includes it). */
          riprel = 1;
          reloc_offset = len - 4;
        }
      }
#endif
    }

    if (group3) {
      int reg = (modrm >> 3) & 7;
      if (reg == 0 || reg == 1) { /* TEST r/m, imm */
        imm_size = (group3 == 0xF7) ? 4 : 1;
      }
    }
  }

  /* Add immediate */
  if (moffs_size > 0) {
    len += moffs_size;
  }
  if (imm_size == 1) {
    len += 1;
  } else if (imm_size == 2) {
    len += 2;
  } else if (imm_size == 3) {
    len += 3; /* ENTER imm16,imm8 */
  } else if (imm_size == 4) {
    if (opsize16 && !riprel && !reloc_offset) {
      len += 2; /* 0x66 operand-size override shrinks imm32 to imm16 */
    } else if (rex & 0x08 && !two_byte && opcode >= 0xB8 && opcode <= 0xBF) {
      len += 8; /* MOV r64, imm64 */
    } else {
      len += 4;
    }
  } else if (imm_size == 6) {
    len += 6; /* far JMP ptr16:32 */
  } else if (imm_size == 8) {
    len += 8; /* MOV r64, imm64 (REX.W) */
  }

  /* Set relocation offset for relative CALL/JMP */
  if (reloc != NULL && reloc_offset > 0) {
    *reloc = reloc_offset;
  }

  return len;
}

static void _sampgdk_hook_write_jmp(void *src, void *dst, int32_t offset) {
  struct _sampgdk_hook_jmp jmp;

#ifdef SAMPGDK_64BIT
  jmp.opcode = 0xFF;
  jmp.modrm  = 0x25;
  jmp.disp   = 0;
  jmp.target = (uintptr_t)dst;
#else
  jmp.opcode = 0xE9;
  jmp.offset = (int32_t)((uint8_t *)dst - ((uint8_t *)src + sizeof(jmp)));
#endif

  memcpy((uint8_t *)src + offset, &jmp, sizeof(jmp));
}

sampgdk_hook_t sampgdk_hook_new(void *src, void *dst) {
  struct _sampgdk_hook *hook;
  size_t orig_size = 0;
  size_t insn_len;

  if ((hook = (sampgdk_hook_t)malloc(sizeof(*hook))) == NULL) {
    return NULL;
  }

  _sampgdk_hook_unprotect(src, _SAMPGDK_HOOK_JMP_SIZE);
  _sampgdk_hook_unprotect(hook->trampoline, _SAMPGDK_HOOK_TRAMPOLINE_SIZE);

  /* We can't just jump to src + 5 as we could end up in the middle of
   * some instruction. So we need to determine the instruction length.
   */
  while (orig_size < _SAMPGDK_HOOK_JMP_SIZE) {
    uint8_t *insn = (uint8_t *)src + orig_size;
    int reloc = 0;

    if ((insn_len = _sampgdk_hook_disasm(insn, &reloc)) == 0) {
      sampgdk_log_error("Unsupported instr at offset %zu (byte: 0x%02X), func=%p, JMP_SIZE=%d",
                        orig_size, insn[0], src, _SAMPGDK_HOOK_JMP_SIZE);
      break;
    }

    memcpy(hook->trampoline + orig_size, insn, insn_len);

    /* If the original code contains a relative JMP/CALL or RIP-relative
     * memory operand, relocate it by -(trampoline - src). Note: this only
     * works when the trampoline is within +/-2GB of src; the heap address
     * from malloc usually is not, so hooking a function whose prologue
     * contains such an instruction requires the trampoline to be allocated
     * near src (see the note in sampgdk_hook_new). */
    if (reloc != 0) {
      int32_t *offset = (int32_t *)(hook->trampoline + orig_size + reloc);
      *offset -= (int32_t)((intptr_t)hook->trampoline - (intptr_t)src);
    }

    orig_size += insn_len;
  }

  if (insn_len == 0) {
    free(hook);
    return NULL;
  }

#ifdef SAMPGDK_64BIT
  /* Absolute JMP (FF 25 + 8B addr): dst is used directly as target. */
  _sampgdk_hook_write_jmp(hook->trampoline, (uint8_t *)src + orig_size, (int32_t)orig_size);
#else
  /* Relative JMP (E9 + rel32): formula is dst - (src_param + 5).
   * To jump to src + orig_size, pass dst = src (not src + orig_size). */
  _sampgdk_hook_write_jmp(hook->trampoline, src, (int32_t)orig_size);
#endif
  _sampgdk_hook_write_jmp(src, dst, 0);

  /* No explicit instruction-cache flush is needed on x86/x64: the
   * hardware's cache-coherency protocol (plus the natural serialization of
   * a call to the patched function, which is always on another thread or
   * after a synchronization point in practice) makes the new bytes visible
   * to every core. The kernel flush APIs would be no-ops here anyway;
   * they only matter on architectures with explicit I-cache maintenance
   * (e.g. ARM), which sampgdk does not target. */

  return hook;
}

void sampgdk_hook_free(sampgdk_hook_t hook) {
  free(hook);
}

void *sampgdk_hook_trampoline(sampgdk_hook_t hook) {
  return hook->trampoline;
}

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "sampgdk.h"

/* #include "logprintf.h" */

/*
 * Gets called instead of the real logprintf when the library has not been
 * initialized yet. See the declaration of sampgdk_logprintf_impl.
 */
static void _sampgdk_logprintf_stub(const char *format, ...) {
  va_list va;

  va_start(va, format);
  vprintf(format, va);
  va_end(va);

  printf("\n");
}

logprintf_t sampgdk_logprintf_impl = &_sampgdk_logprintf_stub;

void sampgdk_do_vlogprintf(const char *format, va_list va) {
  char buffer[SAMPGDK_LOGPRINTF_BUFFER_SIZE];

#ifdef _MSC_VER
  _vsnprintf(buffer, sizeof(buffer), format, va);
  /* _vsnprintf does not insert a terminating NUL if the buffer is too small */
  buffer[sizeof(buffer) - 1] = '\0';
#else
  vsnprintf(buffer, sizeof(buffer), format, va);
#endif

  sampgdk_logprintf_impl("%s", buffer);
}

#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#include "sampgdk.h"

#if SAMPGDK_WINDOWS
  #include <windows.h>
#else
  #include <dlfcn.h>
  #include <string.h>
#endif

/* #include "array.h" */
/* #include "init.h" */
/* #include "plugin.h" */

static struct sampgdk_array _sampgdk_plugins;

SAMPGDK_MODULE_INIT(plugin) {
  return sampgdk_array_new(&_sampgdk_plugins, 1, sizeof(void *));
}

SAMPGDK_MODULE_CLEANUP(plugin) {
  sampgdk_array_free(&_sampgdk_plugins);
}

static int _sampgdk_plugin_compare_handle(const void *key,
                                          const void *elem) {
  const void *handle2 = *(const void **)elem;

  assert(key != NULL);
  assert(elem != NULL);

  if (key < handle2) return -1;
  if (key > handle2) return +1;

  return 0;
}

int sampgdk_plugin_register(void *plugin) {
  assert(plugin != NULL);
  if (sampgdk_array_find(&_sampgdk_plugins,
                         plugin,
                         _sampgdk_plugin_compare_handle) < 0) {
    return sampgdk_array_append(&_sampgdk_plugins, &plugin);
  }
  return -EINVAL;
}

int sampgdk_plugin_unregister(void *plugin) {
  assert(plugin != NULL);
  return sampgdk_array_find_remove(&_sampgdk_plugins,
                                   plugin,
                                   _sampgdk_plugin_compare_handle);
}

void **sampgdk_plugin_get_plugins(int *number) {
  assert(number != NULL);
  *number = _sampgdk_plugins.count;
  return (void **)_sampgdk_plugins.data;
}

#if SAMPGDK_WINDOWS

void *sampgdk_plugin_get_symbol(void *plugin, const char *name)  {
  assert(plugin != NULL);
  assert(name != NULL);
  return (void *)GetProcAddress((HMODULE)plugin, name);
}

void *sampgdk_plugin_get_handle(void *address) {
  MEMORY_BASIC_INFORMATION mbi;
  assert(address != NULL);
  if (VirtualQuery(address, &mbi, sizeof(mbi)) == 0) {
    return NULL;
  }
  return (void *)mbi.AllocationBase;
}

#else /* SAMPGDK_WINDOWS */

void *sampgdk_plugin_get_symbol(void *plugin, const char *name)  {
  assert(plugin != NULL);
  assert(name != NULL);
  return dlsym(plugin, name);
}

void *sampgdk_plugin_get_handle(void *address) {
  Dl_info info;
  assert(address != NULL);
  if (dladdr(address, &info) != 0) {
    return dlopen(info.dli_fname, RTLD_NOW);
  }
  return NULL;
}

#endif /* !SAMPGDK_WINDOWS */

#ifndef SAMPGDK_INTERNAL_AMX_H
#define SAMPGDK_INTERNAL_AMX_H

#include "sampgdk.h"

#define AMX_EXEC_GDK (-10000)

struct sampgdk_amx_api {
  uint16_t *(AMXAPI *Align16)(uint16_t *v);
  uint32_t *(AMXAPI *Align32)(uint32_t *v);
  uint64_t *(AMXAPI *Align64)(uint64_t *v);
  int (AMXAPI *Allot)(AMX *amx, int cells, cell *amx_addr, cell **phys_addr);
  int (AMXAPI *Callback)(AMX *amx, cell index, cell *result, cell *params);
  int (AMXAPI *Cleanup)(AMX *amx);
  int (AMXAPI *Clone)(AMX *amxClone, AMX *amxSource, void *data);
  int (AMXAPI *Exec)(AMX *amx, cell *retval, int index);
  int (AMXAPI *FindNative)(AMX *amx, const char *name, int *index);
  int (AMXAPI *FindPublic)(AMX *amx, const char *funcname, int *index);
  int (AMXAPI *FindPubVar)(AMX *amx, const char *varname, cell *amx_addr);
  int (AMXAPI *FindTagId)(AMX *amx, cell tag_id, char *tagname);
  int (AMXAPI *Flags)(AMX *amx,uint16_t *flags);
  int (AMXAPI *GetAddr)(AMX *amx,cell amx_addr,cell **phys_addr);
  int (AMXAPI *GetNative)(AMX *amx, int index, char *funcname);
  int (AMXAPI *GetPublic)(AMX *amx, int index, char *funcname);
  int (AMXAPI *GetPubVar)(AMX *amx, int index, char *varname, cell *amx_addr);
  int (AMXAPI *GetString)(char *dest,const cell *source, int use_wchar,
                          size_t size);
  int (AMXAPI *GetTag)(AMX *amx, int index, char *tagname, cell *tag_id);
  int (AMXAPI *GetUserData)(AMX *amx, long tag, void **ptr);
  int (AMXAPI *Init)(AMX *amx, void *program);
  int (AMXAPI *InitJIT)(AMX *amx, void *reloc_table, void *native_code);
  int (AMXAPI *MemInfo)(AMX *amx, long *codesize, long *datasize,
                        long *stackheap);
  int (AMXAPI *NameLength)(AMX *amx, int *length);
  AMX_NATIVE_INFO *(AMXAPI *NativeInfo)(const char *name, AMX_NATIVE func);
  int (AMXAPI *NumNatives)(AMX *amx, int *number);
  int (AMXAPI *NumPublics)(AMX *amx, int *number);
  int (AMXAPI *NumPubVars)(AMX *amx, int *number);
  int (AMXAPI *NumTags)(AMX *amx, int *number);
  int (AMXAPI *Push)(AMX *amx, cell value);
  int (AMXAPI *PushArray)(AMX *amx, cell *amx_addr, cell **phys_addr,
                          const cell array[], int numcells);
  int (AMXAPI *PushString)(AMX *amx, cell *amx_addr, cell **phys_addr,
                           const char *string, int pack, int use_wchar);
  int (AMXAPI *RaiseError)(AMX *amx, int error);
  int (AMXAPI *Register)(AMX *amx, const AMX_NATIVE_INFO *nativelist,
                         int number);
  int (AMXAPI *Release)(AMX *amx, cell amx_addr);
  int (AMXAPI *SetCallback)(AMX *amx, AMX_CALLBACK callback);
  int (AMXAPI *SetDebugHook)(AMX *amx, AMX_DEBUG debug);
  int (AMXAPI *SetString)(cell *dest, const char *source, int pack,
                          int use_wchar, size_t size);
  int (AMXAPI *SetUserData)(AMX *amx, long tag, void *ptr);
  int (AMXAPI *StrLen)(const cell *cstring, int *length);
  int (AMXAPI *UTF8Check)(const char *string, int *length);
  int (AMXAPI *UTF8Get)(const char *string, const char **endptr, cell *value);
  int (AMXAPI *UTF8Len)(const cell *cstr, int *length);
  int (AMXAPI *UTF8Put)(char *string, char **endptr, int maxchars, cell value);
};

extern struct sampgdk_amx_api *sampgdk_amx_api;

#define amx_Align16      sampgdk_amx_api->Align16
#define amx_Align32      sampgdk_amx_api->Align32
#define amx_Align64      sampgdk_amx_api->Align64
#define amx_Allot        sampgdk_amx_api->Allot
#define amx_Callback     sampgdk_amx_api->Callback
#define amx_Cleanup      sampgdk_amx_api->Cleanup
#define amx_Clone        sampgdk_amx_api->Clone
#define amx_Exec         sampgdk_amx_api->Exec
#define amx_FindNative   sampgdk_amx_api->FindNative
#define amx_FindPublic   sampgdk_amx_api->FindPublic
#define amx_FindPubVar   sampgdk_amx_api->FindPubVar
#define amx_FindTagId    sampgdk_amx_api->FindTagId
#define amx_Flags        sampgdk_amx_api->Flags
#define amx_GetAddr      sampgdk_amx_api->GetAddr
#define amx_GetNative    sampgdk_amx_api->GetNative
#define amx_GetPublic    sampgdk_amx_api->GetPublic
#define amx_GetPubVar    sampgdk_amx_api->GetPubVar
#define amx_GetString    sampgdk_amx_api->GetString
#define amx_GetTag       sampgdk_amx_api->GetTag
#define amx_GetUserData  sampgdk_amx_api->GetUserData
#define amx_Init         sampgdk_amx_api->Init
#define amx_InitJIT      sampgdk_amx_api->InitJIT
#define amx_MemInfo      sampgdk_amx_api->MemInfo
#define amx_NameLength   sampgdk_amx_api->NameLength
#define amx_NativeInfo   sampgdk_amx_api->NativeInfo
#define amx_NumNatives   sampgdk_amx_api->NumNatives
#define amx_NumPublics   sampgdk_amx_api->NumPublics
#define amx_NumPubVars   sampgdk_amx_api->NumPubVars
#define amx_NumTags      sampgdk_amx_api->NumTags
#define amx_Push         sampgdk_amx_api->Push
#define amx_PushArray    sampgdk_amx_api->PushArray
#define amx_PushString   sampgdk_amx_api->PushString
#define amx_RaiseError   sampgdk_amx_api->RaiseError
#define amx_Register     sampgdk_amx_api->Register
#define amx_Release      sampgdk_amx_api->Release
#define amx_SetCallback  sampgdk_amx_api->SetCallback
#define amx_SetDebugHook sampgdk_amx_api->SetDebugHook
#define amx_SetString    sampgdk_amx_api->SetString
#define amx_SetUserData  sampgdk_amx_api->SetUserData
#define amx_StrLen       sampgdk_amx_api->StrLen
#define amx_UTF8Check    sampgdk_amx_api->UTF8Check
#define amx_UTF8Get      sampgdk_amx_api->UTF8Get
#define amx_UTF8Len      sampgdk_amx_api->UTF8Len
#define amx_UTF8Put      sampgdk_amx_api->UTF8Put

#endif /* !SAMPGDK_INTERNAL_AMX_H */

#ifndef SAMPGDK_INTERNAL_CALLBACK_H
#define SAMPGDK_INTERNAL_CALLBACK_H

#include "sampgdk.h"

/* Callback handler function. */
typedef bool (*sampgdk_callback)(AMX *amx, void *func, cell *retval);

/* Register and unregister a callback in the global callback table.
 *
 * This is usually done only from generated init and cleanup functions of a_*
 * modules. Currently the only module that defines callbacks is a_samp.
 */
int sampgdk_callback_register(const char *name, sampgdk_callback handler);
void sampgdk_callback_unregister(const char *name);

/* Gets the name of the callback with the specified forged index,
 * similar to amx_GetPublic(). The index must be one produced by
 * amxhooks.c (AMX_EXEC_GDK - table position).
 */
bool sampgdk_callback_get(int index, char **name);

/* Executes the callback handler registered for the specified callback.
 *
 * The return value indicates whether the callback returned a "bad" value,
 * i.e. whether the gamemode is allowed to execute the associated public
 * function. This value is specified in the IDL files via the callback's
 * "badret" attribute.
 */
bool sampgdk_callback_invoke(AMX *amx, const char *name,
    int paramcount, cell *retval);

#endif /* !SAMPGDK_INTERNAL_CALLBACK_H */

#ifndef SAMPGDK_INTERNAL_FAKEAMX_H
#define SAMPGDK_INTERNAL_FAKEAMX_H

#include "sampgdk.h"

/* Returns the global fake AMX instance. */
AMX *sampgdk_fakeamx_amx(void);

/* Changes the size of the fake AMX heap. Used in the amx_Allot() hook. */
int sampgdk_fakeamx_resize_heap(int cells);

/* Push a value onto the fake AMX heap. */
int sampgdk_fakeamx_push(int cells, cell *address);
int sampgdk_fakeamx_push_cell(cell value, cell *address);
int sampgdk_fakeamx_push_float(float value, cell *address);
int sampgdk_fakeamx_push_array(const cell *src, int size, cell *address);
int sampgdk_fakeamx_push_string(const char *src, int *size, cell *address);
void sampgdk_fakeamx_pop(cell address);

/* Get stuff back from the heap. Usually used for output parameters. */
void sampgdk_fakeamx_get_cell(cell address, cell *value);
void sampgdk_fakeamx_get_bool(cell address, bool *value);
void sampgdk_fakeamx_get_float(cell address, float *value);
void sampgdk_fakeamx_get_array(cell address, cell *dest, int size);
void sampgdk_fakeamx_get_string(cell address, char *dest, int size);

#endif /* !SAMPGDK_INTERNAL_FAKEAMX_H */

#ifndef SAMPGDK_INTERNAL_NATIVE_H
#define SAMPGDK_INTERNAL_NATIVE_H

#include <stdarg.h>

#include "sampgdk.h"

/* Register a native function in the internal natives table. */
int sampgdk_native_register(const char *name, AMX_NATIVE func);

AMX_NATIVE sampgdk_native_find(const char *name);
AMX_NATIVE sampgdk_native_find_warn(const char *name);
AMX_NATIVE sampgdk_native_find_stub(const char *name);
AMX_NATIVE sampgdk_native_find_warn_stub(const char *name);
AMX_NATIVE sampgdk_native_find_flexible(const char *name, AMX_NATIVE current);

/* Returns all currently registered natives. */
const AMX_NATIVE_INFO *sampgdk_native_get_natives(int *number);

cell sampgdk_native_call(AMX_NATIVE native, cell *params);
cell sampgdk_native_invoke(AMX_NATIVE native, const char *format, va_list args);
cell sampgdk_native_invoke_array(
    AMX_NATIVE native, const char *format, void **args);

#endif /* !SAMPGDK_INTERNAL_NATIVE_H */

#ifndef SAMPGDK_INTERNAL_PARAM_H
#define SAMPGDK_INTERNAL_PARAM_H

#include "sampgdk.h"

void sampgdk_param_get_cell(AMX *amx, int index, cell *param);
void sampgdk_param_get_bool(AMX *amx, int index, bool *param);
void sampgdk_param_get_float(AMX *amx, int index, float *param);
void sampgdk_param_get_string(AMX *amx, int index, char **param);

/* Returns pointer to the start of the parameter list. */
cell *sampgdk_param_get_start(AMX *amx);

#endif /* !SAMPGDK_INTERNAL_PARAM_H */

/* #include "amx.h" */

struct sampgdk_amx_api *sampgdk_amx_api;

#ifndef SAMPGDK_INTERNAL_TIMER_H
#define SAMPGDK_INTERNAL_TIMER_H

#include "sampgdk.h"

/* Timer callback function.
 * There's also a public typedef TimerCallback in <sampgdk/types.h>.
 */
typedef void (SAMPGDK_CALL *sampgdk_timer_callback)(int id, void *param);

int sampgdk_timer_set(int interval, bool repeat,
    sampgdk_timer_callback calback, void *param);
int sampgdk_timer_kill(int timerid);

/* Processes timers created by the specified plugin. */
void sampgdk_timer_process_timers(void *plugin);

#endif /* !SAMPGDK_INTERNAL_TIMER_H */

#include <assert.h>
#include <limits.h>
#include <string.h>

/* #include "amx.h" */
/* #include "array.h" */
/* #include "fakeamx.h" */
/* #include "init.h" */

/* Space reserved for the stack. */
#define _SAMPGDK_FAKEAMX_STACK_SIZE 64

/* The initial size of the heap. */
#define _SAMPGDK_FAKEAMX_HEAP_SIZE 1024

static struct {
  AMX                  amx;
  AMX_HEADER           amxhdr;
  struct sampgdk_array heap;
} _sampgdk_fakeamx;

SAMPGDK_MODULE_INIT(fakeamx) {
  int error;

  memset(&_sampgdk_fakeamx, 0, sizeof(_sampgdk_fakeamx));

  error = sampgdk_array_new(&_sampgdk_fakeamx.heap,
                            _SAMPGDK_FAKEAMX_HEAP_SIZE +
                            _SAMPGDK_FAKEAMX_STACK_SIZE,
                            sizeof(cell));
  if (error < 0) {
    return error;
  }

  _sampgdk_fakeamx.amxhdr.magic = AMX_MAGIC;
  _sampgdk_fakeamx.amxhdr.file_version = MIN_FILE_VERSION;
  _sampgdk_fakeamx.amxhdr.amx_version = MIN_AMX_VERSION;
  /* dat is unused for fake AMX; amx->data is set directly below.
   * char* subtraction yields ptrdiff_t (64-bit on x64, 32-bit on x86).
   * Cast to int32_t truncates explicitly; safe because field is unused.
   */
  _sampgdk_fakeamx.amxhdr.dat = (int32_t)(
      (char *)_sampgdk_fakeamx.heap.data
    - (char *)&_sampgdk_fakeamx.amxhdr);
  _sampgdk_fakeamx.amxhdr.defsize = sizeof(AMX_FUNCSTUBNT);

  _sampgdk_fakeamx.amx.base = (unsigned char *)&_sampgdk_fakeamx.amxhdr;
  _sampgdk_fakeamx.amx.data = (unsigned char *)_sampgdk_fakeamx.heap.data;
  _sampgdk_fakeamx.amx.callback = amx_Callback;
  _sampgdk_fakeamx.amx.stp = _sampgdk_fakeamx.heap.size * sizeof(cell);
  _sampgdk_fakeamx.amx.stk = _sampgdk_fakeamx.amx.stp;
  _sampgdk_fakeamx.amx.flags = AMX_FLAG_NTVREG | AMX_FLAG_RELOC;

  sampgdk_array_pad(&_sampgdk_fakeamx.heap);

  return 0;
}

SAMPGDK_MODULE_CLEANUP(fakeamx) {
  sampgdk_array_free(&_sampgdk_fakeamx.heap);
}

AMX *sampgdk_fakeamx_amx(void) {
  return &_sampgdk_fakeamx.amx;
}

int sampgdk_fakeamx_resize_heap(int cells) {
  int error;
  cell old_size;
  cell new_size;
  cell old_stk;
  cell new_stk;
  cell new_stp;

  assert(cells > 0);

  old_size = _sampgdk_fakeamx.heap.size;
  new_size = cells;

  error = sampgdk_array_resize(&_sampgdk_fakeamx.heap, new_size);
  sampgdk_array_pad(&_sampgdk_fakeamx.heap);
  if (error < 0) {
    return error;
  }

  /* Update data pointers to point at the newly allocated heap.
   * dat is unused; amx->data is used instead.
   */
  _sampgdk_fakeamx.amxhdr.dat = (int32_t)(
      (char *)_sampgdk_fakeamx.heap.data
    - (char *)&_sampgdk_fakeamx.amxhdr);
  _sampgdk_fakeamx.amx.data = (unsigned char *)_sampgdk_fakeamx.heap.data;

  old_stk = _sampgdk_fakeamx.amx.stk;
  new_stk = _sampgdk_fakeamx.amx.stk + (new_size - old_size) * sizeof(cell);
  new_stp = _sampgdk_fakeamx.amx.stp + (new_size - old_size) * sizeof(cell);

  /* Shift stack contents. */
  memmove((unsigned char *)_sampgdk_fakeamx.heap.data
                           + new_stk - _SAMPGDK_FAKEAMX_STACK_SIZE,
          (unsigned char *)_sampgdk_fakeamx.heap.data
                           + old_stk - _SAMPGDK_FAKEAMX_STACK_SIZE,
          _SAMPGDK_FAKEAMX_STACK_SIZE);

  _sampgdk_fakeamx.amx.stk = new_stk;
  _sampgdk_fakeamx.amx.stp = new_stp;

  return 0;
}

int sampgdk_fakeamx_push(int cells, cell *address) {
  cell old_hea, new_hea;
  cell old_heap_size, new_heap_size;

  assert(cells > 0);

  old_hea = _sampgdk_fakeamx.amx.hea;
  new_hea = _sampgdk_fakeamx.amx.hea + cells * sizeof(cell);

  old_heap_size = _sampgdk_fakeamx.heap.size;
  new_heap_size = (new_hea + _SAMPGDK_FAKEAMX_STACK_SIZE) / sizeof(cell);

  if (new_hea >= (cell)(old_heap_size * sizeof(cell))) {
    int error;

    error = sampgdk_fakeamx_resize_heap(new_heap_size);
    if (error < 0) {
      return error;
    }
  }

  _sampgdk_fakeamx.amx.hea = new_hea;

  if (address != NULL) {
    *address = old_hea;
  }

  return 0;
}

int sampgdk_fakeamx_push_cell(cell value, cell *address) {
  int error;

  assert(address != NULL);

  if ((error = sampgdk_fakeamx_push(1, address)) < 0) {
    return error;
  }

  ((cell *)(_sampgdk_fakeamx.heap.data))[*address / sizeof(cell)] = value;

  return 0;
}

int sampgdk_fakeamx_push_float(float value, cell *address) {
  return sampgdk_fakeamx_push_cell(amx_ftoc(value), address);
}

int sampgdk_fakeamx_push_array(const cell *src, int size, cell *address) {
  int error;
  cell *dest;

  assert(address != NULL);
  assert(src != NULL);
  assert(size > 0);

  if ((error = sampgdk_fakeamx_push(size, address)) < 0) {
    return error;
  }

  dest = (cell *)sampgdk_array_get(&_sampgdk_fakeamx.heap, *address / sizeof(cell));
  memcpy(dest, src, size * sizeof(cell));

  return 0;
}

int sampgdk_fakeamx_push_string(const char *src, int *size, cell *address) {
  int src_size;
  int error;

  assert(address != NULL);
  assert(src != NULL);

  src_size = (int)strlen(src) + 1;
  if ((error = sampgdk_fakeamx_push(src_size, address)) < 0) {
    return error;
  }

  amx_SetString((cell *)sampgdk_array_get(&_sampgdk_fakeamx.heap,
                                 *address / sizeof(cell)),
                src, 0, 0, src_size);

  if (size != NULL) {
    *size = src_size;
  }

  return 0;
}

void sampgdk_fakeamx_get_cell(cell address, cell *value) {
  assert(address % sizeof(cell) == 0);
  assert(value != NULL);

  *value = *(cell *)sampgdk_array_get(&_sampgdk_fakeamx.heap,
                                      address / sizeof(cell));
}

void sampgdk_fakeamx_get_bool(cell address, bool *value) {
  cell tmp;

  assert(address % sizeof(cell) == 0);
  assert(value != NULL);

  sampgdk_fakeamx_get_cell(address, &tmp);
  *value = !!tmp;
}

void sampgdk_fakeamx_get_float(cell address, float *value) {
  cell tmp;

  assert(address % sizeof(cell) == 0);
  assert(value != NULL);

  sampgdk_fakeamx_get_cell(address, &tmp);
  *value = amx_ctof(tmp);
}

void sampgdk_fakeamx_get_array(cell address, cell *dest, int size) {
  cell *src;

  assert(address % sizeof(cell) == 0);
  assert(dest != NULL);
  assert(size > 0);

  src = (cell *)sampgdk_array_get(&_sampgdk_fakeamx.heap, address / sizeof(cell));
  memcpy(dest, src, size * sizeof(cell));
}

void sampgdk_fakeamx_get_string(cell address, char *dest, int size) {
  assert(address % sizeof(cell) == 0);
  assert(dest != NULL);

  amx_GetString(dest, (cell *)sampgdk_array_get(&_sampgdk_fakeamx.heap,
                                                address / sizeof(cell)),
                                                0, size);
}

void sampgdk_fakeamx_pop(cell address) {
  assert(address % sizeof(cell) == 0);

  if (_sampgdk_fakeamx.amx.hea > address) {
    _sampgdk_fakeamx.amx.hea = address;
  }
}

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* #include "array.h" */
/* #include "fakeamx.h" */
/* #include "init.h" */
/* #include "likely.h" */
/* #include "native.h" */
/* #include "log.h" */

#define _SAMPGDK_NATIVE_MAX_ARGS     32
#define _SAMPGDK_NATIVE_MAX_ARGS_SIZE 8  /* in bytes */

static struct sampgdk_array _sampgdk_natives;

SAMPGDK_MODULE_INIT(native) {
  int error;
  AMX_NATIVE_INFO null = {NULL, NULL};

  error = sampgdk_array_new(&_sampgdk_natives,
                            128,
                            sizeof(AMX_NATIVE_INFO));
  if (error < 0) {
    return error;
  }

  return sampgdk_array_append(&_sampgdk_natives, &null);
}

SAMPGDK_MODULE_CLEANUP(native) {
  sampgdk_array_free(&_sampgdk_natives);
}

int sampgdk_native_register(const char *name, AMX_NATIVE func) {
  AMX_NATIVE old_func;
  AMX_NATIVE_INFO info;
  AMX_NATIVE_INFO *ptr;
  int i;

  info.name = name;
  info.func = func;

  assert(name != 0);

  old_func = sampgdk_native_find(name);
  if (old_func != NULL) {
    return -1;
  }

  /* Keep natives ordered by name.
   * This allows us to use binary search in sampgdk_native_find().
   */
  for (i = 0; i < _sampgdk_natives.count - 1; i++) {
    ptr = (AMX_NATIVE_INFO *)sampgdk_array_get(&_sampgdk_natives, i);
    if (strcmp(name, ptr->name) <= 0) {
      break;
    }
  }

  return sampgdk_array_insert(&_sampgdk_natives, i, 1, &info);
}

static int _sampgdk_native_compare_bsearch(const void *key,
                                           const void *elem) {
  assert(key != NULL);
  assert(elem != NULL);
  return strcmp((const char *)key, ((const AMX_NATIVE_INFO *)elem)->name);
}

AMX_NATIVE sampgdk_native_find(const char *name) {
  AMX_NATIVE_INFO *info;

  assert(name != NULL);

  if (_sampgdk_natives.data == NULL) {
    /* Perhaps they forgot to initialize? */
    return NULL;
  }

  info = (AMX_NATIVE_INFO *)bsearch(name,
                                    _sampgdk_natives.data,
                                    _sampgdk_natives.count - 1,
                                    _sampgdk_natives.elem_size,
                                    _sampgdk_native_compare_bsearch);
  if (info == NULL) {
    return NULL;
  }

  return info->func;
}

AMX_NATIVE sampgdk_native_find_warn(const char *name) {
  AMX_NATIVE func;

  assert(name != NULL);

  func = sampgdk_native_find(name);
  if (func == NULL) {
    sampgdk_log_warning("Native function not found: %s", name);
  }

  return func;
}

static cell AMX_NATIVE_CALL native_stub(AMX *amx, cell *params) {
  return 0;
}

AMX_NATIVE sampgdk_native_find_stub(const char *name) {
  AMX_NATIVE func;

  assert(name != NULL);

  if ((func = sampgdk_native_find(name)) == NULL) {
    return native_stub;
  }

  return func;
}

AMX_NATIVE sampgdk_native_find_warn_stub(const char *name) {
  AMX_NATIVE func;

  assert(name != NULL);

  if ((func = sampgdk_native_find_warn(name)) == NULL) {
    return native_stub;
  }

  return func;
}

AMX_NATIVE sampgdk_native_find_flexible(const char *name, AMX_NATIVE current) {
  char *always_search;

  if (SAMPGDK_LIKELY(current != NULL && current != native_stub)) {
    return current;
  }

  if (current == NULL) {
    /* This is the first time this native is searched for, do it as usual.
     */
    return sampgdk_native_find_warn_stub(name);
  }

  /* current == native_stub */
  if ((always_search = getenv("SAMGDK_NATIVE_SEARCH_ALWAYS")) != NULL
      && atoi(always_search) != 0) {
    /* Previous attempt to find the native failed, but the always search
     * option is set so search again.
     */
    return sampgdk_native_find_warn_stub(name);
  }

  return current;
}

const AMX_NATIVE_INFO *sampgdk_native_get_natives(int *number) {
  if (number != NULL) {
    *number = _sampgdk_natives.count - 1;
  }
  return (AMX_NATIVE_INFO *)_sampgdk_natives.data;
}

cell sampgdk_native_call(AMX_NATIVE native, cell *params) {
  AMX *amx = sampgdk_fakeamx_amx();
  assert(native != NULL);
  return native(amx, params);
}

cell sampgdk_native_invoke(AMX_NATIVE native,
                           const char *format,
                           va_list args) {
  cell i = 0;
  const char *format_ptr = format;
  unsigned char args_copy[_SAMPGDK_NATIVE_MAX_ARGS *
                          _SAMPGDK_NATIVE_MAX_ARGS_SIZE];
  unsigned char *args_ptr = args_copy;
  void *args_array[_SAMPGDK_NATIVE_MAX_ARGS];

  assert(format_ptr != NULL);

  while (*format_ptr != '\0' && i < _SAMPGDK_NATIVE_MAX_ARGS) {
    switch (*format_ptr) {
      case 'i': /* integer */
      case 'd': /* integer */
        *(int *)args_ptr = va_arg(args, int);
        args_array[i++] = args_ptr;
        args_ptr += _SAMPGDK_NATIVE_MAX_ARGS_SIZE;
        break;
      case 'b': /* boolean */
        *(bool *)args_ptr = !!va_arg(args, int);
        args_array[i++] = args_ptr;
        args_ptr += _SAMPGDK_NATIVE_MAX_ARGS_SIZE;
        break;
      case 'f': /* floating-point */
        *(float *)args_ptr = (float)va_arg(args, double);
        args_array[i++] = args_ptr;
        args_ptr += _SAMPGDK_NATIVE_MAX_ARGS_SIZE;
        break;
      case 'r': /* const reference */
      case 'R': /* non-const reference */
      case 's': /* const string */
      case 'S': /* non-const string */
      case 'a': /* const array */
      case 'A': /* non-const array */
        args_array[i++] = va_arg(args, void *);
        break;
    }
    format_ptr++;
  }

  return sampgdk_native_invoke_array(native, format, args_array);
}

cell sampgdk_native_invoke_array(AMX_NATIVE native, const char *format,
                                 void **args) {
  AMX *amx = sampgdk_fakeamx_amx();
  const char *format_ptr = format;
  cell i = 0;
  cell params[_SAMPGDK_NATIVE_MAX_ARGS + 1];
  cell size[_SAMPGDK_NATIVE_MAX_ARGS] = {0};
  char type[_SAMPGDK_NATIVE_MAX_ARGS];
  int needs_size = -1;
  enum {
    ST_READ_SPEC,
    ST_NEED_SIZE,
    ST_READING_SIZE,
    ST_READING_SIZE_ARG,
    ST_READ_SIZE
  } state = ST_READ_SPEC;
  cell retval;

  assert(format_ptr != NULL);
  assert(args != NULL);

  while (*format_ptr != '\0' && i < _SAMPGDK_NATIVE_MAX_ARGS) {
    switch (state) {
      case ST_READ_SPEC:
        switch (*format_ptr) {
          case 'i': /* integer */
          case 'd': /* integer */
            params[i + 1] = *(int *)args[i];
            break;
          case 'b': /* boolean */
            params[i + 1] = *(bool *)args[i];
            break;
          case 'f': /* floating-point */ {
            float value = *(float *)args[i];
            params[i + 1] = amx_ftoc(value);
            break;
          }
          case 'r': /* const reference */
          case 'R': /* non-const reference */ {
            cell *ptr = (cell *)args[i];
            sampgdk_fakeamx_push_cell(*ptr, &params[i + 1]);
            size[i] = sizeof(cell);
            break;
          }
          case 's': /* const string */ {
            char *str = (char *)args[i];
            int str_size;
            sampgdk_fakeamx_push_string(str, &str_size, &params[i + 1]);
            size[i] = str_size;
            break;
          }
          case 'S': /* non-const string */
          case 'a': /* const array */
          case 'A': /* non-const array */
            needs_size = i;
            state = ST_NEED_SIZE;
            break;
          default:
            sampgdk_log_warning("Unrecognized type specifier '%c'",
                                *format_ptr);
        }
        type[i++] = *format_ptr++;
        break;
      case ST_NEED_SIZE:
        if (*format_ptr == '[') {
          state = ST_READING_SIZE;
        } else {
          sampgdk_log_warning("Bad format string: expected '[' but got '%c'",
                              *format_ptr);
        }
        format_ptr++;
        break;
      case ST_READING_SIZE:
        if (*format_ptr == '*') {
          format_ptr++;
          state = ST_READING_SIZE_ARG;
        } else {
          size[needs_size] = (int)strtol(format_ptr, (char **)&format_ptr, 10);
          state = ST_READ_SIZE;
        }
        break;
      case ST_READING_SIZE_ARG: {
        int index = (int)strtol(format_ptr, (char **)&format_ptr, 10);
        size[needs_size] = *(int *)args[index];
        state = ST_READ_SIZE;
        break;
      }
      case ST_READ_SIZE: {
        if (*format_ptr == ']') {
          switch (type[needs_size]) {
            case 'a':
            case 'A':
            case 'S':
              if (size[needs_size] > 0) {
                sampgdk_fakeamx_push_array((const cell *)args[needs_size],
                                           size[needs_size],
                                           &params[needs_size + 1]);
              } else {
                sampgdk_log_warning("Invalid buffer size");
              }
              break;
          }
          needs_size = -1;
          state = ST_READ_SPEC;
        } else {
          sampgdk_log_warning("Bad format string (expected ']' but got '%c')",
                              *format_ptr);
        }
        format_ptr++;
        break;
      }
    }
  }

  if (*format_ptr != '\0') {
    sampgdk_log_warning("Too many native arguments (at most %d allowed)",
                        _SAMPGDK_NATIVE_MAX_ARGS);
  }

  params[0] = i * sizeof(cell);
  assert(native != NULL);
  retval = native(amx, params);

  while (--i >= 0) {
    if (size[i] > 0) {
      /* If this is an output parameter we have to write the updated value
       * back to the argument.
       */
      switch (type[i]) {
        case 'R':
          sampgdk_fakeamx_get_cell(params[i + 1], (cell *)args[i]);
          break;
        case 'S':
          sampgdk_fakeamx_get_string(params[i + 1], (char *)args[i], size[i]);
          break;
        case 'A':
          sampgdk_fakeamx_get_array(params[i + 1], (cell *)args[i], size[i]);
          break;
      }
      sampgdk_fakeamx_pop(params[i + 1]);
    }
  }

  return retval;
}

#include "sampgdk.h"

/* #include "internal/native.h" */

SAMPGDK_API(const AMX_NATIVE_INFO *, sampgdk_GetNatives(int *number)) {
  return sampgdk_native_get_natives(number);
}

SAMPGDK_API(AMX_NATIVE, sampgdk_FindNative(const char *name)) {
  if (name != NULL) {
    return sampgdk_native_find(name);
  }
  return NULL;
}

SAMPGDK_API(cell, sampgdk_CallNative(AMX_NATIVE native, cell *params)) {
  return sampgdk_native_call(native, params);
}

SAMPGDK_API(cell, sampgdk_InvokeNative(AMX_NATIVE native,
                                       const char *format, ...)) {
  cell retval;
  va_list args;

  va_start(args, format);
  retval = sampgdk_native_invoke(native, format, args);
  va_end(args);

  return retval;
}

SAMPGDK_API(cell, sampgdk_InvokeNativeV(AMX_NATIVE native,
                                        const char *format, va_list args)) {
  return sampgdk_native_invoke(native, format, args);
}

SAMPGDK_API(cell, sampgdk_InvokeNativeArray(AMX_NATIVE native,
                                            const char *format, void **args)) {
  return sampgdk_native_invoke_array(native, format, args);
}

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* #include "amx.h" */
/* #include "array.h" */
/* #include "callback.h" */
/* #include "init.h" */
/* #include "log.h" */
/* #include "param.h" */
/* #include "plugin.h" */

#define _SAMPGDK_CALLBACK_MAX_ARGS 32

typedef bool (PLUGIN_CALL *_sampgdk_callback_filter)(
    AMX *amx,
    const char *name,
    cell *params,
    cell *retval);
typedef bool (PLUGIN_CALL *_sampgdk_callback_filter2)(
    AMX *amx,
    const char *name,
    cell *params,
    cell *retval,
    bool *stop);

struct _sampgdk_callback_info {
  char *name;
  char *func_name;
  void *handler;
};

static struct sampgdk_array _sampgdk_callbacks;

static int _sampgdk_callback_compare_name(const void *key,
                                          const void *elem) {
  assert(key != NULL);
  assert(elem != NULL);
  return strcmp((const char *)key,
                ((const struct _sampgdk_callback_info *)elem)->name);
}

static struct _sampgdk_callback_info *_sampgdk_callback_find(const char *name) {
  assert(name != NULL);

  if (_sampgdk_callbacks.count <= 0) {
    return NULL;
  }

  return (struct _sampgdk_callback_info *)bsearch(name,
                 _sampgdk_callbacks.data,
                 _sampgdk_callbacks.count,
                 _sampgdk_callbacks.elem_size,
                 _sampgdk_callback_compare_name);
}

SAMPGDK_MODULE_INIT(callback) {
  int error;

  error = sampgdk_array_new(&_sampgdk_callbacks,
                            32,
                            sizeof(struct _sampgdk_callback_info));
  if (error < 0) {
    return error;
  }

  error = sampgdk_callback_register(":OnPublicCall", NULL);
  if (error < 0) {
    return error;
  }

  error = sampgdk_callback_register(":OnPublicCall2", NULL);
  if (error < 0) {
    return error;
  }

  return 0;
}

SAMPGDK_MODULE_CLEANUP(callback) {
  int i;
  struct _sampgdk_callback_info *callback;

  for (i = 0; i < _sampgdk_callbacks.count; i++) {
    callback = (struct _sampgdk_callback_info *)sampgdk_array_get(&_sampgdk_callbacks, i);
    free(callback->name);
  }

  sampgdk_array_free(&_sampgdk_callbacks);
}

int sampgdk_callback_register(const char *name,
                              sampgdk_callback handler) {
  int error;
  int count;
  int i;
  struct _sampgdk_callback_info callback;
  struct _sampgdk_callback_info *ptr;

  assert(name != NULL);

  ptr = _sampgdk_callback_find(name);
  if (ptr != NULL) {
    return sampgdk_array_get_index(&_sampgdk_callbacks, ptr);
  }

  callback.handler = (void *)handler;

  callback.name = (char *)malloc(strlen(name) + 1);
  if (callback.name == NULL) {
    return -ENOMEM;
  }

  strcpy(callback.name, name);

  if (callback.name[0] == ':') {
    /* Special callbacks have a name that begins with ':'. This is to ensure
     * that their name is not used by SA-MP or user scripts.
     */
    callback.func_name = callback.name + 1;
  } else {
    callback.func_name = callback.name;
  }

  /* Keep callbacks ordered by name.
   * This allows us to use binary search when searching through callbacks.
   */
  count = _sampgdk_callbacks.count;
  for (i = 0; i < count; i++) {
    ptr = (struct _sampgdk_callback_info *)sampgdk_array_get(
        &_sampgdk_callbacks, i);
    if (strcmp(name, ptr->name) <= 0) {
      break;
    }
  }

  error = sampgdk_array_insert(&_sampgdk_callbacks, i, 1, &callback);
  if (error < 0) {
    free(callback.name);
    return error;
  }

  return error; /* index */
}

void sampgdk_callback_unregister(const char *name) {
  struct _sampgdk_callback_info *callback;

  if ((callback = _sampgdk_callback_find(name)) != NULL) {
    callback->handler = NULL;
  }
}

bool sampgdk_callback_get(int index, char **name) {
  struct _sampgdk_callback_info *callback;

  assert(name != NULL);

  /* index here is the value passed to amx_Exec() for a forged public
   * (AMX_EXEC_GDK - table_position); recover the table position.
   */
  int pos = AMX_EXEC_GDK - index;

  if (pos < 0 || pos >= _sampgdk_callbacks.count) {
    return false;
  }

  callback = (struct _sampgdk_callback_info *)sampgdk_array_get(
      &_sampgdk_callbacks, pos);
  *name = callback->name;

  return true;
}

bool sampgdk_callback_invoke(AMX *amx,
                             const char *name,
                             int paramcount,
                             cell *retval)
{
  cell params[_SAMPGDK_CALLBACK_MAX_ARGS + 1];
  void **plugins;
  int num_plugins;
  int i;

  assert(amx != NULL);

  if (paramcount > _SAMPGDK_CALLBACK_MAX_ARGS) {
    sampgdk_log_error("Too many callback arguments (at most %d allowed)",
                      _SAMPGDK_CALLBACK_MAX_ARGS);
    return true;
  }

  params[0] = paramcount * sizeof(cell);
  memcpy(&params[1], sampgdk_param_get_start(amx), params[0]);

  plugins = sampgdk_plugin_get_plugins(&num_plugins);

  for (i = 0; i < num_plugins; i++) {
    void *plugin = plugins[i];
    struct _sampgdk_callback_info *callback;
    struct _sampgdk_callback_info *callback_filter;
    struct _sampgdk_callback_info *callback_filter2;
    void *func;
    bool do_call = true;
    bool stop = false;

    callback_filter = _sampgdk_callback_find(":OnPublicCall");
    assert(callback_filter != NULL);
    
    func = sampgdk_plugin_get_symbol(plugin, callback_filter->func_name);
    if (func != NULL) {
      do_call = ((_sampgdk_callback_filter)func)(amx, name, params, retval);
    }

    /* callback_filter2 is similar to callback_filter except it can stop
     * propagation of public call to other plugins. It was added for backwards
     * compatibility.
     *
     * callback_filter2's return value overrides that of callback_filter.
     */
    callback_filter2 = _sampgdk_callback_find(":OnPublicCall2");
    assert(callback_filter2 != NULL);
    
    func = sampgdk_plugin_get_symbol(plugin, callback_filter2->func_name);
    if (func != NULL) {
      do_call = !((_sampgdk_callback_filter2)func)(amx,
                                                   name,
                                                   params,
                                                   retval,
                                                   &stop);
    }

    if (stop) {
      return false;
    }

    if (!do_call) {
      continue;
    }

    callback = _sampgdk_callback_find(name);
    if (callback == NULL || callback->handler == NULL) {
      continue;
    }
    
    func = sampgdk_plugin_get_symbol(plugin, callback->func_name);
    if (func != NULL
        && !((sampgdk_callback)callback->handler)(amx, func, retval)) {
      return false;
    }
  }

  return true;
}

#include <assert.h>
#include <stdlib.h>

/* #include "amx.h" */
/* #include "param.h" */

void sampgdk_param_get_cell(AMX *amx, int index, cell *param) {
  assert(param != NULL);
  *param = sampgdk_param_get_start(amx)[index];
}

void sampgdk_param_get_bool(AMX *amx, int index, bool *param) {
  assert(param != NULL);
  *param = !!sampgdk_param_get_start(amx)[index];
}

void sampgdk_param_get_float(AMX *amx, int index, float *param) {
  cell p = sampgdk_param_get_start(amx)[index];
  assert(param != NULL);
  *param = amx_ctof(p);
}

void sampgdk_param_get_string(AMX *amx, int index, char **param) {
  cell amx_addr;
  cell *phys_addr;
  int length;
  char *string;

  amx_addr = sampgdk_param_get_start(amx)[index];
  if (amx_GetAddr(amx, amx_addr, &phys_addr) != AMX_ERR_NONE) {
    return;
  }

  amx_StrLen(phys_addr, &length);
  string = (char *)malloc((length + 1) * sizeof(char));

  if (amx_GetString(string, phys_addr, 0, length + 1) != AMX_ERR_NONE) {
    free(string);
    return;
  }

  assert(param != NULL);
  *param = string;
}

cell *sampgdk_param_get_start(AMX *amx) {
  unsigned char *data =  amx->data != NULL
    ? amx->data
    : amx->base + ((AMX_HEADER *)amx->base)->dat;
  return (cell *)(data + amx->stk);
}

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "sampgdk.h"

/* #include "amx.h" */
/* #include "array.h" */
/* #include "callback.h" */
/* #include "fakeamx.h" */
/* #include "init.h" */
/* #include "log.h" */
/* #include "native.h" */
/* #include "param.h" */
/* #include "hook.h" */

static AMX *_sampgdk_amxhooks_main_amx;

#define _SAMPGDK_AMXHOOKS_FUNC_LIST(C) \
  C(Register) \
  C(FindPublic) \
  C(Exec) \
  C(Allot)

#define _SAMPGDK_AMXHOOKS_DEFINE_HOOK(name) \
  static sampgdk_hook_t _sampgdk_amxhooks_##name##_hook;
_SAMPGDK_AMXHOOKS_FUNC_LIST(_SAMPGDK_AMXHOOKS_DEFINE_HOOK)
#undef _SAMPGDK_AMXHOOKS_DEFINE_HOOK

/* The "funcidx" native uses amx_FindPublic() to get the public function's
 * index but our FindPublic always returns success regardless of the actual
 * result. So here's a fixed version.
 *
 * Thanks to Incognito for finding this bug!
 */
static cell AMX_NATIVE_CALL _sampgdk_amxhooks_funcidx(AMX *amx, cell *params) {
  char *funcname;
  int index;
  int error;

  amx_StrParam(amx, params[1], funcname);
  if (funcname == NULL) {
    return -1;
  }

  error = amx_FindPublic(amx, funcname, &index);
  if (error != AMX_ERR_NONE || index <= AMX_EXEC_GDK) {
    return -1;
  }

  return index;
}

static int AMXAPI _sampgdk_amxhooks_Register(AMX *amx,
                                             const AMX_NATIVE_INFO *nativelist,
                                             int number) {
  int i;
  int count = 0;

  sampgdk_log_debug("amx_Register(%p, %p, %d)", amx, nativelist, number);

#ifndef SAMPGDK_64BIT
  /* On 32-bit, override funcidx to work around our FindPublic hook
   * which always returns success regardless of the actual result.
   *
   * On 64-bit (open.mp), funcidx is handled by the server itself,
   * and (ucell) would truncate the pointer, so skip it.
   */
  {
    AMX_HEADER *hdr = (AMX_HEADER *)amx->base;
    AMX_FUNCSTUBNT *natives =
        (AMX_FUNCSTUBNT *)(amx->base + hdr->natives);
    if (amx_FindNative(amx, "funcidx", &i) == AMX_ERR_NONE) {
      natives[i].address = (ucell)_sampgdk_amxhooks_funcidx;
    }
  }
#endif

  for (i = 0; (i < number || number == -1) && nativelist[i].name != NULL; i++) {
    if (sampgdk_native_register(nativelist[i].name, nativelist[i].func) >= 0) {
      sampgdk_log_debug("Registered native: %s @ %p",
                        nativelist[i].name, nativelist[i].func);
      count++;
    }
  }

  if (count > 0) {
    sampgdk_log_info("Registered %d natives", count);
  }

  return SAMPGDK_HOOK_CALL_CC(_sampgdk_amxhooks_Register_hook, int, AMXAPI,
                              (AMX *, const AMX_NATIVE_INFO *, int),
                              (amx, nativelist, number));
}

static int AMXAPI _sampgdk_amxhooks_FindPublic(AMX *amx,
                                               const char *name,
                                               int *index) {
  int error;
  int index_internal;
  int index_real;

  sampgdk_log_debug("amx_FindPublic(%p, \"%s\", %p)", amx, name, index);

  error = SAMPGDK_HOOK_CALL_CC(_sampgdk_amxhooks_FindPublic_hook, int, AMXAPI,
                               (AMX *, const char *, int *), (amx, name, index));
  sampgdk_log_debug("amx_FindPublic returned %d", error);

  /* We are interested in intercepting public calls against the following
   * AMX instances:
   *
   * - the main AMX (the gamemode)
   * - the fake AMX (this is needed for HTTP() to work)
   */
  if (amx != _sampgdk_amxhooks_main_amx &&
      amx != sampgdk_fakeamx_amx()) {
    return error;
  }

  /* If the public was really found (and I mean REALLY) there's no need
   * to mess with the index.
   */
  if (error == AMX_ERR_NONE && *index >= 0) {
    return AMX_ERR_NONE;
  }

  /* OK, this public officially doesn't exist. Register it in our internal
   * callback table and return success. The forged index is derived from the
   * table position (4.x behavior), not a name hash: open.mp's AMX executor
   * does not tolerate the very large negative indices a 30-bit hash would
   * produce when callers pass the forged index straight to amx_Exec.
   */
  index_internal = sampgdk_callback_register(name, NULL);
  index_real = AMX_EXEC_GDK - index_internal;

  if (index_internal < 0) {
    sampgdk_log_error("Error registering callback: %s",
                      strerror(-index_internal));
  } else if (error == AMX_ERR_NONE && *index < 0) {
    /* If there are other plugins running they better return the same
     * index as we do. Otherwise it would be a total mess and we can't
     * let that happen.
     */
    if (*index != index_real) {
      error = AMX_ERR_NOTFOUND;
      sampgdk_log_warning("Index mismatch for %s (%d != %d)",
                          name, *index, index_real);
    }
  } else if (error != AMX_ERR_NONE) {
    error = AMX_ERR_NONE, *index = index_real;
    sampgdk_log_debug("Registered callback: %s, index = %d", name, *index);
  }

  return error;
}

static int AMXAPI _sampgdk_amxhooks_Exec(AMX *amx, cell *retval, int index) {
  int paramcount;
  int error = AMX_ERR_NONE;
  bool do_exec = true;
  bool do_cleanup = false;

  sampgdk_log_debug("amx_Exec(%p, %p, %d), paramcount = %d, stk = %d",
      amx, retval, index, amx->paramcount, amx->stk);

  /* We have to reset amx->paramcount at this point so if the callback
   * itself calls amx_Exec() it won't pop our arguments off the stack.
   */
  paramcount = amx->paramcount;
  amx->paramcount = 0;

  /* Since filterscripts don't use main() we can assume that the AMX
   * that executes main() is indeed the main AMX i.e. the gamemode.
   */
  if (index == AMX_EXEC_MAIN) {
    /* This extra check is needed in order to stop OnGameModeInit()
     * from being called twice in a row after a gmx.
     */
    if (amx != NULL && _sampgdk_amxhooks_main_amx != amx) {
      _sampgdk_amxhooks_main_amx = amx;

      sampgdk_log_info("Found main AMX, callbacks should work now");
      sampgdk_log_debug("Main AMX instance: %p", amx);

      /* For some odd reason OnGameModeInit() is called before main().
       * Normally callbacks are handled below but this creates an exception.
       */
      sampgdk_callback_invoke(amx, "OnGameModeInit", paramcount, retval);
    }
  } else if (index != AMX_EXEC_CONT && (amx == _sampgdk_amxhooks_main_amx ||
                                        amx == sampgdk_fakeamx_amx())) {
    char *name = NULL;

    if (index <= AMX_EXEC_GDK) {
      /* sampgdk_callback_get() expects the raw forged index and recovers
       * the table position internally (AMX_EXEC_GDK - index).
       */
      sampgdk_callback_get(index, &name);
    } else {
      AMX *main_amx = _sampgdk_amxhooks_main_amx;
      AMX_FUNCSTUBNT *publics = (AMX_FUNCSTUBNT *)(main_amx->base +
          ((AMX_HEADER *)main_amx->base)->publics);
      name = (char *)(publics[index].nameofs + amx->base);
    }

    if (name != NULL) {
      do_exec = sampgdk_callback_invoke(amx, name, paramcount, retval);
    } else {
      sampgdk_log_warning("Unknown callback, index = %d", index);
    }
  }

  if (do_exec) {
    amx->paramcount = paramcount;
    error = SAMPGDK_HOOK_CALL_CC(_sampgdk_amxhooks_Exec_hook, int, AMXAPI,
                                 (AMX *, cell *, int), (amx, retval, index));
    sampgdk_log_debug("amx_Exec returned %d", error);
  }

  /* Suppress the error and also let the other plugin(s) know that we
   * handle the cleanup (see below).
   */
  if (error == AMX_ERR_INDEX && index <= AMX_EXEC_GDK) {
    error = AMX_ERR_NONE;
    do_cleanup = true;
  }

  /* Someone has to clean things up if amx_Exec() didn't run after all.
   */
  if (!do_exec || do_cleanup) {
    amx->paramcount = 0;
    amx->stk += paramcount * sizeof(cell);
    sampgdk_log_debug("Popped %d parameter(s), stk = %d", paramcount, amx->stk);
  }

  return error;
}

static int AMXAPI _sampgdk_amxhooks_Allot(AMX *amx,
                                          int cells,
                                          cell *amx_addr,
                                          cell **phys_addr) {
  int error;

  sampgdk_log_debug("amx_Allot(%p, %d, %p, %p)", amx, cells, amx_addr,
                                                 phys_addr);

  /* There is a bug in amx_Allot() where it returns success even though
   * there's not enough space on the heap:
   *
   * if (amx->stk - amx->hea - cells*sizeof(cell) < STKMARGIN)
   *   return AMX_ERR_MEMORY;
   *
   * The expression on the left is always positive because of the conversion
   * to size_t, which is unsigned.
   *
   * The code below code should fix this.
   */
  #define STKMARGIN (cell)(16 * sizeof(cell))
  if ((size_t)amx->stk < (size_t)(amx->hea + cells*sizeof(cell) + STKMARGIN)) {
    error =  AMX_ERR_MEMORY;
  } else {
    error = SAMPGDK_HOOK_CALL_CC(_sampgdk_amxhooks_Allot_hook, int, AMXAPI,
                                 (AMX *, int, cell *, cell **), (amx, cells, amx_addr, phys_addr));
    sampgdk_log_debug("amx_Allot returned %d", error);
  }

  /* If called against the fake AMX and failed to allocate the requested
   * amount of space, grow the heap and try again.
   */
  if (error == AMX_ERR_MEMORY && amx == sampgdk_fakeamx_amx()) {
    cell new_size = ((amx->hea + STKMARGIN) / sizeof(cell)) + cells + 2;
    cell resize;

    sampgdk_log_debug("Growing fake AMX heap to %d bytes = %d = %d", new_size);
    resize = sampgdk_fakeamx_resize_heap(new_size);

    if (resize >= 0) {
      error = SAMPGDK_HOOK_CALL_CC(_sampgdk_amxhooks_Allot_hook, int, AMXAPI,
                                   (AMX *, int, cell *, cell **), (amx, cells, amx_addr, phys_addr));
    }
  }

  return error;
}

static int _sampgdk_amxhooks_create(void) {
  #define _SAMPGDK_AMXHOOKS_CREATE_HOOK(name) \
    if ((_sampgdk_amxhooks_##name##_hook = \
        sampgdk_hook_new((void *)sampgdk_amx_api->name, \
                              (void *)_sampgdk_amxhooks_##name)) == NULL) \
      goto no_memory;
  _SAMPGDK_AMXHOOKS_FUNC_LIST(_SAMPGDK_AMXHOOKS_CREATE_HOOK)
  return 0;
no_memory:
  return -ENOMEM;
  #undef _SAMPGDK_AMXHOOKS_CREATE_HOOK
}

static void _sampgdk_amxhooks_destroy(void) {
  #define _SAMPGDK_AMXHOOKS_DESTROY_HOOK(name) \
    sampgdk_hook_free(_sampgdk_amxhooks_##name##_hook);
  _SAMPGDK_AMXHOOKS_FUNC_LIST(_SAMPGDK_AMXHOOKS_DESTROY_HOOK)
  #undef _SAMPGDK_AMXHOOKS_DESTROY_HOOK
}

SAMPGDK_MODULE_INIT(amxhooks) {
  int error;

  error = _sampgdk_amxhooks_create();
  if (error < 0) {
    _sampgdk_amxhooks_destroy();
    return error;
  }

  return 0;
}

SAMPGDK_MODULE_CLEANUP(amxhooks) {
  _sampgdk_amxhooks_destroy();
}

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sampgdk.h"

#if SAMPGDK_WINDOWS
  #include <windows.h>
#endif

/* #include "array.h" */
/* #include "init.h" */
/* #include "log.h" */
/* #include "plugin.h" */
/* #include "timer.h" */
/* #include "types.h" */

struct _sampgdk_timer_info {
  bool    is_set;
  void   *plugin;
  int64_t started;
  int     interval;
  void   *callback;
  void   *param;
  bool    repeat;
};

static struct sampgdk_array _sampgdk_timers;

#if SAMPGDK_WINDOWS

static int64_t _sampgdk_timer_now(void) {
  LARGE_INTEGER freq;
  LARGE_INTEGER counter;

  if (QueryPerformanceFrequency(&freq) == 0) {
    sampgdk_log_error("QueryPerformanceFrequency: error %d", GetLastError());
    return 0;
  }
  if (QueryPerformanceCounter(&counter) == 0) {
    sampgdk_log_error("QueryPerformanceCounter: error %d", GetLastError());
    return 0;
  }

  return (int64_t)(1000.0L / freq.QuadPart * counter.QuadPart);
}

#else /* SAMPGDK_WINDOWS */

static int64_t _sampgdk_timer_now(void) {
  struct timespec ts;
  int64_t msec;
  int64_t msec_fract;

  if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0) {
    sampgdk_log_error("clock_gettime: %s", strerror(errno));
    return 0;
  }

  msec = (int64_t)ts.tv_sec * 1000;
  msec_fract = (int64_t)ts.tv_nsec / 1000000L;

  return msec + msec_fract;
}

#endif /* !SAMPGDK_WINDOWS */

static int _sampgdk_timer_find_slot(void) {
  int i;

  for (i = 0; i < _sampgdk_timers.count; i++) {
    struct _sampgdk_timer_info *timer;

    timer = (struct _sampgdk_timer_info *)sampgdk_array_get(&_sampgdk_timers, i);
    if (!timer->is_set) {
      return i;
    }
  }

  return -1;
}

static void _sampgdk_timer_fire(int timerid, int64_t elapsed) {
  struct _sampgdk_timer_info *timer;
  int64_t now = _sampgdk_timer_now();
  int64_t started;

  assert(timerid > 0 && timerid <= _sampgdk_timers.count);
  timer = (struct _sampgdk_timer_info *)sampgdk_array_get(&_sampgdk_timers, timerid - 1);

  assert(timer->is_set);
  started = timer->started;

  sampgdk_log_debug("Firing timer %d, now = %" PRId64 ", elapsed = %" PRId64,
      timerid, now, elapsed);
  ((sampgdk_timer_callback)timer->callback)(timerid, timer->param);

  /* We don't want to kill the same timer twice, so make sure it's not
   * been killed inside the timer callback.
   */
  if (timer->is_set && timer->started == started) {
    if (timer->repeat) {
      timer->started = now - (elapsed - timer->interval);
    } else {
      sampgdk_timer_kill(timerid);
    }
  }
}

SAMPGDK_MODULE_INIT(timer) {
  int error;

  error = sampgdk_array_new(&_sampgdk_timers,
                            8,
                            sizeof(struct _sampgdk_timer_info));
  if (error < 0) {
    return error;
  }

  sampgdk_array_zero(&_sampgdk_timers);

  return 0;
}

SAMPGDK_MODULE_CLEANUP(timer) {
  sampgdk_array_free(&_sampgdk_timers);
}

int sampgdk_timer_set(int interval,
                      bool repeat,
                      sampgdk_timer_callback callback,
                      void *param) {
  struct _sampgdk_timer_info timer;
  int slot;
  int error;
  int timerid;

  assert(callback != NULL);

  timer.is_set   = true;
  timer.interval = interval;
  timer.repeat   = repeat;
  timer.callback = (void *)callback;
  timer.param    = param;
  timer.started  = _sampgdk_timer_now();
  timer.plugin   = sampgdk_plugin_get_handle((void *)callback);

  if (timer.started == 0) {
    return 0; /* error already logged */
  }

  slot = _sampgdk_timer_find_slot();
  if (slot >= 0) {
    sampgdk_array_set(&_sampgdk_timers, slot, &timer);
  } else {
    error = sampgdk_array_append(&_sampgdk_timers, &timer);
    if (error < 0) {
      sampgdk_log_error("Error setting timer: %s", strerror(-error));
      return 0;
    }
    slot = _sampgdk_timers.count - 1;
  }

  /* Timer IDs returned by the SA:MP's SetTimer() API begin
   * with 1, and so do they here.
   */
  timerid = slot + 1;

  sampgdk_log_debug("Created timer: ID = %d, interval = %d, repeat = %s",
      timerid, interval, repeat ? "true" : "false");

  return timerid;
}

int sampgdk_timer_kill(int timerid) {
  struct _sampgdk_timer_info *timer;

  if (timerid <= 0 || timerid > _sampgdk_timers.count) {
    return -EINVAL;
  }

  timer = (struct _sampgdk_timer_info *)sampgdk_array_get(&_sampgdk_timers, timerid - 1);
  if (!timer->is_set) {
    return -EINVAL;
  }

  timer->is_set = false;

  sampgdk_log_debug("Killed timer %d", timerid);

  return 0;
}

void sampgdk_timer_process_timers(void *plugin) {
  int64_t now;
  int64_t elapsed;
  int i;
  struct _sampgdk_timer_info *timer;

  assert(plugin != NULL);

  now = _sampgdk_timer_now();

  for (i = 0; i < _sampgdk_timers.count; i++) {
    timer = (struct _sampgdk_timer_info *)sampgdk_array_get(&_sampgdk_timers, i);

    if (!timer->is_set
        || (plugin != NULL && timer->plugin != plugin)) {
      continue;
    }

    elapsed = now - timer->started;

    if (elapsed >= timer->interval) {
      _sampgdk_timer_fire(i + 1, elapsed);
    }
  }
}

#include <assert.h>
#include <string.h>

#include "sampgdk.h"

/* #include "internal/amx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/logprintf.h" */
/* #include "internal/plugin.h" */
/* #include "internal/timer.h" */

#undef sampgdk_Load
#undef sampgdk_Unload
#undef sampgdk_ProcessTick

#ifdef _MSC_VER
  #include <intrin.h>
  #define _SAMPGDK_RETURN_ADDRESS() _ReturnAddress()
#else
  #define _SAMPGDK_RETURN_ADDRESS() __builtin_return_address(0)
#endif

#ifdef SAMPGDK_EMBEDDED
  #define _SAMPGDK_CALLER_HANDLE() \
    sampgdk_plugin_get_handle(((void *)_sampgdk_init))
#else
  #define _SAMPGDK_CALLER_HANDLE() \
    sampgdk_plugin_get_handle(_SAMPGDK_RETURN_ADDRESS())
#endif

static void _sampgdk_init(void **plugin_data) {
  int error;

  sampgdk_logprintf_impl = (logprintf_t)plugin_data[PLUGIN_DATA_LOGPRINTF];
  sampgdk_amx_api =
      (struct sampgdk_amx_api *)plugin_data[PLUGIN_DATA_AMX_EXPORTS];

  error = sampgdk_module_init();
  if (error  < 0) {
    sampgdk_log_error("Initialization failed: %s", strerror(-error));
  }

  sampgdk_log_info("GDK version: " SAMPGDK_VERSION_STRING);
}

static int _sampgdk_init_plugin(void *plugin, void **plugin_data) {
  int error;
  int num_plugins;

  assert(plugin != NULL);

  (void)sampgdk_plugin_get_plugins(&num_plugins);
  if (num_plugins == 0) {
    _sampgdk_init(plugin_data);
  }

  error = sampgdk_plugin_register(plugin);
  if (error < 0) {
    sampgdk_log_error("Error registering plugin: %s", strerror(-error));
  }

  return error;
}

static void _sampgdk_cleanup(void) {
  sampgdk_module_cleanup();
}

static void _sampgdk_cleanup_plugin(void *plugin) {
  int error;
  int num_plugins;

  assert(plugin != NULL);

  error = sampgdk_plugin_unregister(plugin);
  if (error < 0) {
    sampgdk_log_error("Error unregistering plugin: %s", strerror(-error));
  }

  (void)sampgdk_plugin_get_plugins(&num_plugins);
  if (num_plugins == 0) {
    _sampgdk_cleanup();
  }
}

SAMPGDK_API(unsigned int, sampgdk_Supports(void)) {
  return SUPPORTS_VERSION;
}

SAMPGDK_API(bool, sampgdk_Load(void **ppData, sampgdk_hidden_t hidden)) {
  return _sampgdk_init_plugin(_SAMPGDK_CALLER_HANDLE(), ppData) >= 0;
}

SAMPGDK_API(void, sampgdk_Unload(sampgdk_hidden_t hidden)) {
  _sampgdk_cleanup_plugin(_SAMPGDK_CALLER_HANDLE());
}

SAMPGDK_API(void, sampgdk_ProcessTick(sampgdk_hidden_t hidden)) {
  sampgdk_timer_process_timers(_SAMPGDK_CALLER_HANDLE());
}

SAMPGDK_API(void, sampgdk_logprintf(const char *format, ...)) {
  va_list args;
  va_start(args, format);
  sampgdk_do_vlogprintf(format, args);
  va_end(args);
}

SAMPGDK_API(void, sampgdk_vlogprintf(const char *format, va_list args)) {
  sampgdk_do_vlogprintf(format, args);
}

#include "sampgdk.h"

SAMPGDK_API(int, sampgdk_GetVersion(void)) {
  return SAMPGDK_VERSION_ID;
}

SAMPGDK_API(const char *, sampgdk_GetVersionString(void)) {
  return SAMPGDK_VERSION_STRING;
}

#include "sampgdk.h"

/* #include "internal/timer.h" */

SAMPGDK_NATIVE(int, SetTimer(int interval, bool repeat, TimerCallback callback,
                             void *param)) {
  return sampgdk_timer_set(interval, repeat, callback, param);
}

SAMPGDK_NATIVE(bool, KillTimer(int timerid)) {
  return sampgdk_timer_kill(timerid) >= 0;
}

#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, SendClientMessage(int playerid, int color, const char * message)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell message_;
  sampgdk_log_debug("SendClientMessage(%d, %d, \"%s\")", playerid, color, message);
  native = sampgdk_native_find_flexible("SendClientMessage", native);
  sampgdk_fakeamx_push_string(message, NULL, &message_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)color;
  params[3] = message_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(message_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SendClientMessageToAll(int color, const char * message)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell message_;
  sampgdk_log_debug("SendClientMessageToAll(%d, \"%s\")", color, message);
  native = sampgdk_native_find_flexible("SendClientMessageToAll", native);
  sampgdk_fakeamx_push_string(message, NULL, &message_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)color;
  params[2] = message_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(message_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SendPlayerMessageToPlayer(int playerid, int senderid, const char * message)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell message_;
  sampgdk_log_debug("SendPlayerMessageToPlayer(%d, %d, \"%s\")", playerid, senderid, message);
  native = sampgdk_native_find_flexible("SendPlayerMessageToPlayer", native);
  sampgdk_fakeamx_push_string(message, NULL, &message_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)senderid;
  params[3] = message_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(message_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SendPlayerMessageToAll(int senderid, const char * message)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell message_;
  sampgdk_log_debug("SendPlayerMessageToAll(%d, \"%s\")", senderid, message);
  native = sampgdk_native_find_flexible("SendPlayerMessageToAll", native);
  sampgdk_fakeamx_push_string(message, NULL, &message_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)senderid;
  params[2] = message_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(message_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SendDeathMessage(int killer, int killee, int weapon)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("SendDeathMessage(%d, %d, %d)", killer, killee, weapon);
  native = sampgdk_native_find_flexible("SendDeathMessage", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)killer;
  params[2] = (cell)killee;
  params[3] = (cell)weapon;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SendDeathMessageToPlayer(int playerid, int killer, int killee, int weapon)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SendDeathMessageToPlayer(%d, %d, %d, %d)", playerid, killer, killee, weapon);
  native = sampgdk_native_find_flexible("SendDeathMessageToPlayer", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)killer;
  params[3] = (cell)killee;
  params[4] = (cell)weapon;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GameTextForAll(const char * text, int time, int style)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell text_;
  sampgdk_log_debug("GameTextForAll(\"%s\", %d, %d)", text, time, style);
  native = sampgdk_native_find_flexible("GameTextForAll", native);
  sampgdk_fakeamx_push_string(text, NULL, &text_);
  params[0] = 3 * sizeof(cell);
  params[1] = text_;
  params[2] = (cell)time;
  params[3] = (cell)style;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(text_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GameTextForPlayer(int playerid, const char * text, int time, int style)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell text_;
  sampgdk_log_debug("GameTextForPlayer(%d, \"%s\", %d, %d)", playerid, text, time, style);
  native = sampgdk_native_find_flexible("GameTextForPlayer", native);
  sampgdk_fakeamx_push_string(text, NULL, &text_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = text_;
  params[3] = (cell)time;
  params[4] = (cell)style;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(text_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetTickCount()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetTickCount()");
  native = sampgdk_native_find_flexible("GetTickCount", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetMaxPlayers()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetMaxPlayers()");
  native = sampgdk_native_find_flexible("GetMaxPlayers", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, VectorSize(float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("VectorSize(%f, %f, %f)", x, y, z);
  native = sampgdk_native_find_flexible("VectorSize", native);
  params[0] = 3 * sizeof(cell);
  params[1] = amx_ftoc(x);
  params[2] = amx_ftoc(y);
  params[3] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, GetPlayerPoolSize()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetPlayerPoolSize()");
  native = sampgdk_native_find_flexible("GetPlayerPoolSize", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetVehiclePoolSize()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetVehiclePoolSize()");
  native = sampgdk_native_find_flexible("GetVehiclePoolSize", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetActorPoolSize()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetActorPoolSize()");
  native = sampgdk_native_find_flexible("GetActorPoolSize", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SHA256_PassHash(const char * password, const char * salt, char * ret_hash, int ret_hash_len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell password_;
  cell salt_;
  cell ret_hash_;
  sampgdk_log_debug("SHA256_PassHash(\"%s\", \"%s\", @%p, %d)", password, salt, ret_hash, ret_hash_len);
  native = sampgdk_native_find_flexible("SHA256_PassHash", native);
  sampgdk_fakeamx_push_string(password, NULL, &password_);
  sampgdk_fakeamx_push_string(salt, NULL, &salt_);
  sampgdk_fakeamx_push(ret_hash_len, &ret_hash_);
  params[0] = 4 * sizeof(cell);
  params[1] = password_;
  params[2] = salt_;
  params[3] = ret_hash_;
  params[4] = (cell)ret_hash_len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(ret_hash_, ret_hash, ret_hash_len);
  sampgdk_fakeamx_pop(ret_hash_);
  sampgdk_fakeamx_pop(salt_);
  sampgdk_fakeamx_pop(password_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetSVarInt(const char * varname, int int_value)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell varname_;
  sampgdk_log_debug("SetSVarInt(\"%s\", %d)", varname, int_value);
  native = sampgdk_native_find_flexible("SetSVarInt", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 2 * sizeof(cell);
  params[1] = varname_;
  params[2] = (cell)int_value;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetSVarInt(const char * varname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell varname_;
  sampgdk_log_debug("GetSVarInt(\"%s\")", varname);
  native = sampgdk_native_find_flexible("GetSVarInt", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 1 * sizeof(cell);
  params[1] = varname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetSVarString(const char * varname, const char * string_value)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell varname_;
  cell string_value_;
  sampgdk_log_debug("SetSVarString(\"%s\", \"%s\")", varname, string_value);
  native = sampgdk_native_find_flexible("SetSVarString", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  sampgdk_fakeamx_push_string(string_value, NULL, &string_value_);
  params[0] = 2 * sizeof(cell);
  params[1] = varname_;
  params[2] = string_value_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(string_value_);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetSVarString(const char * varname, char * string_return, int len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell varname_;
  cell string_return_;
  sampgdk_log_debug("GetSVarString(\"%s\", @%p, %d)", varname, string_return, len);
  native = sampgdk_native_find_flexible("GetSVarString", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  sampgdk_fakeamx_push(len, &string_return_);
  params[0] = 3 * sizeof(cell);
  params[1] = varname_;
  params[2] = string_return_;
  params[3] = (cell)len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(string_return_, string_return, len);
  sampgdk_fakeamx_pop(string_return_);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetSVarFloat(const char * varname, float float_value)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell varname_;
  sampgdk_log_debug("SetSVarFloat(\"%s\", %f)", varname, float_value);
  native = sampgdk_native_find_flexible("SetSVarFloat", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 2 * sizeof(cell);
  params[1] = varname_;
  params[2] = amx_ftoc(float_value);
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(float, GetSVarFloat(const char * varname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell varname_;
  sampgdk_log_debug("GetSVarFloat(\"%s\")", varname);
  native = sampgdk_native_find_flexible("GetSVarFloat", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 1 * sizeof(cell);
  params[1] = varname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(bool, DeleteSVar(const char * varname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell varname_;
  sampgdk_log_debug("DeleteSVar(\"%s\")", varname);
  native = sampgdk_native_find_flexible("DeleteSVar", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 1 * sizeof(cell);
  params[1] = varname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetSVarsUpperIndex()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetSVarsUpperIndex()");
  native = sampgdk_native_find_flexible("GetSVarsUpperIndex", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetSVarNameAtIndex(int index, char * ret_varname, int ret_len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell ret_varname_;
  sampgdk_log_debug("GetSVarNameAtIndex(%d, @%p, %d)", index, ret_varname, ret_len);
  native = sampgdk_native_find_flexible("GetSVarNameAtIndex", native);
  sampgdk_fakeamx_push(ret_len, &ret_varname_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)index;
  params[2] = ret_varname_;
  params[3] = (cell)ret_len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(ret_varname_, ret_varname, ret_len);
  sampgdk_fakeamx_pop(ret_varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetSVarType(const char * varname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell varname_;
  sampgdk_log_debug("GetSVarType(\"%s\")", varname);
  native = sampgdk_native_find_flexible("GetSVarType", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 1 * sizeof(cell);
  params[1] = varname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetGameModeText(const char * text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell text_;
  sampgdk_log_debug("SetGameModeText(\"%s\")", text);
  native = sampgdk_native_find_flexible("SetGameModeText", native);
  sampgdk_fakeamx_push_string(text, NULL, &text_);
  params[0] = 1 * sizeof(cell);
  params[1] = text_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(text_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetTeamCount(int count)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SetTeamCount(%d)", count);
  native = sampgdk_native_find_flexible("SetTeamCount", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)count;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, AddPlayerClass(int modelid, float spawn_x, float spawn_y, float spawn_z, float z_angle, int weapon1, int weapon1_ammo, int weapon2, int weapon2_ammo, int weapon3, int weapon3_ammo)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[12];
  sampgdk_log_debug("AddPlayerClass(%d, %f, %f, %f, %f, %d, %d, %d, %d, %d, %d)", modelid, spawn_x, spawn_y, spawn_z, z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
  native = sampgdk_native_find_flexible("AddPlayerClass", native);
  params[0] = 11 * sizeof(cell);
  params[1] = (cell)modelid;
  params[2] = amx_ftoc(spawn_x);
  params[3] = amx_ftoc(spawn_y);
  params[4] = amx_ftoc(spawn_z);
  params[5] = amx_ftoc(z_angle);
  params[6] = (cell)weapon1;
  params[7] = (cell)weapon1_ammo;
  params[8] = (cell)weapon2;
  params[9] = (cell)weapon2_ammo;
  params[10] = (cell)weapon3;
  params[11] = (cell)weapon3_ammo;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, AddPlayerClassEx(int teamid, int modelid, float spawn_x, float spawn_y, float spawn_z, float z_angle, int weapon1, int weapon1_ammo, int weapon2, int weapon2_ammo, int weapon3, int weapon3_ammo)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[13];
  sampgdk_log_debug("AddPlayerClassEx(%d, %d, %f, %f, %f, %f, %d, %d, %d, %d, %d, %d)", teamid, modelid, spawn_x, spawn_y, spawn_z, z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
  native = sampgdk_native_find_flexible("AddPlayerClassEx", native);
  params[0] = 12 * sizeof(cell);
  params[1] = (cell)teamid;
  params[2] = (cell)modelid;
  params[3] = amx_ftoc(spawn_x);
  params[4] = amx_ftoc(spawn_y);
  params[5] = amx_ftoc(spawn_z);
  params[6] = amx_ftoc(z_angle);
  params[7] = (cell)weapon1;
  params[8] = (cell)weapon1_ammo;
  params[9] = (cell)weapon2;
  params[10] = (cell)weapon2_ammo;
  params[11] = (cell)weapon3;
  params[12] = (cell)weapon3_ammo;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, AddStaticVehicle(int modelid, float spawn_x, float spawn_y, float spawn_z, float z_angle, int color1, int color2)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[8];
  sampgdk_log_debug("AddStaticVehicle(%d, %f, %f, %f, %f, %d, %d)", modelid, spawn_x, spawn_y, spawn_z, z_angle, color1, color2);
  native = sampgdk_native_find_flexible("AddStaticVehicle", native);
  params[0] = 7 * sizeof(cell);
  params[1] = (cell)modelid;
  params[2] = amx_ftoc(spawn_x);
  params[3] = amx_ftoc(spawn_y);
  params[4] = amx_ftoc(spawn_z);
  params[5] = amx_ftoc(z_angle);
  params[6] = (cell)color1;
  params[7] = (cell)color2;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, AddStaticVehicleEx(int modelid, float spawn_x, float spawn_y, float spawn_z, float z_angle, int color1, int color2, int respawn_delay, bool addsiren)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  sampgdk_log_debug("AddStaticVehicleEx(%d, %f, %f, %f, %f, %d, %d, %d, %d)", modelid, spawn_x, spawn_y, spawn_z, z_angle, color1, color2, respawn_delay, addsiren);
  native = sampgdk_native_find_flexible("AddStaticVehicleEx", native);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)modelid;
  params[2] = amx_ftoc(spawn_x);
  params[3] = amx_ftoc(spawn_y);
  params[4] = amx_ftoc(spawn_z);
  params[5] = amx_ftoc(z_angle);
  params[6] = (cell)color1;
  params[7] = (cell)color2;
  params[8] = (cell)respawn_delay;
  params[9] = (cell)addsiren;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, AddStaticPickup(int model, int type, float x, float y, float z, int virtualworld)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  sampgdk_log_debug("AddStaticPickup(%d, %d, %f, %f, %f, %d)", model, type, x, y, z, virtualworld);
  native = sampgdk_native_find_flexible("AddStaticPickup", native);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)model;
  params[2] = (cell)type;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  params[5] = amx_ftoc(z);
  params[6] = (cell)virtualworld;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, CreatePickup(int model, int type, float x, float y, float z, int virtualworld)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  sampgdk_log_debug("CreatePickup(%d, %d, %f, %f, %f, %d)", model, type, x, y, z, virtualworld);
  native = sampgdk_native_find_flexible("CreatePickup", native);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)model;
  params[2] = (cell)type;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  params[5] = amx_ftoc(z);
  params[6] = (cell)virtualworld;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, DestroyPickup(int pickup)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DestroyPickup(%d)", pickup);
  native = sampgdk_native_find_flexible("DestroyPickup", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)pickup;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ShowNameTags(bool show)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("ShowNameTags(%d)", show);
  native = sampgdk_native_find_flexible("ShowNameTags", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)show;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ShowPlayerMarkers(int mode)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("ShowPlayerMarkers(%d)", mode);
  native = sampgdk_native_find_flexible("ShowPlayerMarkers", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)mode;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GameModeExit()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GameModeExit()");
  native = sampgdk_native_find_flexible("GameModeExit", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetWorldTime(int hour)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SetWorldTime(%d)", hour);
  native = sampgdk_native_find_flexible("SetWorldTime", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)hour;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetWeaponName(int weaponid, char * name, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell name_;
  sampgdk_log_debug("GetWeaponName(%d, @%p, %d)", weaponid, name, size);
  native = sampgdk_native_find_flexible("GetWeaponName", native);
  sampgdk_fakeamx_push(size, &name_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)weaponid;
  params[2] = name_;
  params[3] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(name_, name, size);
  sampgdk_fakeamx_pop(name_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, EnableTirePopping(bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("EnableTirePopping(%d)", enable);
  native = sampgdk_native_find_flexible("EnableTirePopping", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, EnableVehicleFriendlyFire()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("EnableVehicleFriendlyFire()");
  native = sampgdk_native_find_flexible("EnableVehicleFriendlyFire", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, AllowInteriorWeapons(bool allow)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("AllowInteriorWeapons(%d)", allow);
  native = sampgdk_native_find_flexible("AllowInteriorWeapons", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)allow;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetWeather(int weatherid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SetWeather(%d)", weatherid);
  native = sampgdk_native_find_flexible("SetWeather", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)weatherid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetGravity(float gravity)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SetGravity(%f)", gravity);
  native = sampgdk_native_find_flexible("SetGravity", native);
  params[0] = 1 * sizeof(cell);
  params[1] = amx_ftoc(gravity);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(float, GetGravity()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetGravity()");
  native = sampgdk_native_find_flexible("GetGravity", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(bool, AllowAdminTeleport(bool allow)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("AllowAdminTeleport(%d)", allow);
  native = sampgdk_native_find_flexible("AllowAdminTeleport", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)allow;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetDeathDropAmount(int amount)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SetDeathDropAmount(%d)", amount);
  native = sampgdk_native_find_flexible("SetDeathDropAmount", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)amount;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, CreateExplosion(float x, float y, float z, int type, float radius)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("CreateExplosion(%f, %f, %f, %d, %f)", x, y, z, type, radius);
  native = sampgdk_native_find_flexible("CreateExplosion", native);
  params[0] = 5 * sizeof(cell);
  params[1] = amx_ftoc(x);
  params[2] = amx_ftoc(y);
  params[3] = amx_ftoc(z);
  params[4] = (cell)type;
  params[5] = amx_ftoc(radius);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, EnableZoneNames(bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("EnableZoneNames(%d)", enable);
  native = sampgdk_native_find_flexible("EnableZoneNames", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, UsePlayerPedAnims()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("UsePlayerPedAnims()");
  native = sampgdk_native_find_flexible("UsePlayerPedAnims", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, DisableInteriorEnterExits()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("DisableInteriorEnterExits()");
  native = sampgdk_native_find_flexible("DisableInteriorEnterExits", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetNameTagDrawDistance(float distance)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SetNameTagDrawDistance(%f)", distance);
  native = sampgdk_native_find_flexible("SetNameTagDrawDistance", native);
  params[0] = 1 * sizeof(cell);
  params[1] = amx_ftoc(distance);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, DisableNameTagLOS()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("DisableNameTagLOS()");
  native = sampgdk_native_find_flexible("DisableNameTagLOS", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, LimitGlobalChatRadius(float chat_radius)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("LimitGlobalChatRadius(%f)", chat_radius);
  native = sampgdk_native_find_flexible("LimitGlobalChatRadius", native);
  params[0] = 1 * sizeof(cell);
  params[1] = amx_ftoc(chat_radius);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, LimitPlayerMarkerRadius(float marker_radius)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("LimitPlayerMarkerRadius(%f)", marker_radius);
  native = sampgdk_native_find_flexible("LimitPlayerMarkerRadius", native);
  params[0] = 1 * sizeof(cell);
  params[1] = amx_ftoc(marker_radius);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ConnectNPC(const char * name, const char * script)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell name_;
  cell script_;
  sampgdk_log_debug("ConnectNPC(\"%s\", \"%s\")", name, script);
  native = sampgdk_native_find_flexible("ConnectNPC", native);
  sampgdk_fakeamx_push_string(name, NULL, &name_);
  sampgdk_fakeamx_push_string(script, NULL, &script_);
  params[0] = 2 * sizeof(cell);
  params[1] = name_;
  params[2] = script_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(script_);
  sampgdk_fakeamx_pop(name_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerNPC(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerNPC(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerNPC", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerAdmin(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerAdmin(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerAdmin", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, Kick(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("Kick(%d)", playerid);
  native = sampgdk_native_find_flexible("Kick", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, Ban(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("Ban(%d)", playerid);
  native = sampgdk_native_find_flexible("Ban", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, BanEx(int playerid, const char * reason)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell reason_;
  sampgdk_log_debug("BanEx(%d, \"%s\")", playerid, reason);
  native = sampgdk_native_find_flexible("BanEx", native);
  sampgdk_fakeamx_push_string(reason, NULL, &reason_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = reason_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(reason_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SendRconCommand(const char * command)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell command_;
  sampgdk_log_debug("SendRconCommand(\"%s\")", command);
  native = sampgdk_native_find_flexible("SendRconCommand", native);
  sampgdk_fakeamx_push_string(command, NULL, &command_);
  params[0] = 1 * sizeof(cell);
  params[1] = command_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(command_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerNetworkStats(int playerid, char * retstr, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell retstr_;
  sampgdk_log_debug("GetPlayerNetworkStats(%d, @%p, %d)", playerid, retstr, size);
  native = sampgdk_native_find_flexible("GetPlayerNetworkStats", native);
  sampgdk_fakeamx_push(size, &retstr_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = retstr_;
  params[3] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(retstr_, retstr, size);
  sampgdk_fakeamx_pop(retstr_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetNetworkStats(char * retstr, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell retstr_;
  sampgdk_log_debug("GetNetworkStats(@%p, %d)", retstr, size);
  native = sampgdk_native_find_flexible("GetNetworkStats", native);
  sampgdk_fakeamx_push(size, &retstr_);
  params[0] = 2 * sizeof(cell);
  params[1] = retstr_;
  params[2] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(retstr_, retstr, size);
  sampgdk_fakeamx_pop(retstr_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerVersion(int playerid, char * version, int len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell version_;
  sampgdk_log_debug("GetPlayerVersion(%d, @%p, %d)", playerid, version, len);
  native = sampgdk_native_find_flexible("GetPlayerVersion", native);
  sampgdk_fakeamx_push(len, &version_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = version_;
  params[3] = (cell)len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(version_, version, len);
  sampgdk_fakeamx_pop(version_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, BlockIpAddress(const char * ip_address, int timems)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell ip_address_;
  sampgdk_log_debug("BlockIpAddress(\"%s\", %d)", ip_address, timems);
  native = sampgdk_native_find_flexible("BlockIpAddress", native);
  sampgdk_fakeamx_push_string(ip_address, NULL, &ip_address_);
  params[0] = 2 * sizeof(cell);
  params[1] = ip_address_;
  params[2] = (cell)timems;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(ip_address_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, UnBlockIpAddress(const char * ip_address)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell ip_address_;
  sampgdk_log_debug("UnBlockIpAddress(\"%s\")", ip_address);
  native = sampgdk_native_find_flexible("UnBlockIpAddress", native);
  sampgdk_fakeamx_push_string(ip_address, NULL, &ip_address_);
  params[0] = 1 * sizeof(cell);
  params[1] = ip_address_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(ip_address_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetServerVarAsString(const char * varname, char * value, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell varname_;
  cell value_;
  sampgdk_log_debug("GetServerVarAsString(\"%s\", @%p, %d)", varname, value, size);
  native = sampgdk_native_find_flexible("GetServerVarAsString", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  sampgdk_fakeamx_push(size, &value_);
  params[0] = 3 * sizeof(cell);
  params[1] = varname_;
  params[2] = value_;
  params[3] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(value_, value, size);
  sampgdk_fakeamx_pop(value_);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetServerVarAsInt(const char * varname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell varname_;
  sampgdk_log_debug("GetServerVarAsInt(\"%s\")", varname);
  native = sampgdk_native_find_flexible("GetServerVarAsInt", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 1 * sizeof(cell);
  params[1] = varname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetServerVarAsBool(const char * varname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell varname_;
  sampgdk_log_debug("GetServerVarAsBool(\"%s\")", varname);
  native = sampgdk_native_find_flexible("GetServerVarAsBool", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 1 * sizeof(cell);
  params[1] = varname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetConsoleVarAsString(const char * varname, char * buffer, int len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell varname_;
  cell buffer_;
  sampgdk_log_debug("GetConsoleVarAsString(\"%s\", @%p, %d)", varname, buffer, len);
  native = sampgdk_native_find_flexible("GetConsoleVarAsString", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  sampgdk_fakeamx_push(len, &buffer_);
  params[0] = 3 * sizeof(cell);
  params[1] = varname_;
  params[2] = buffer_;
  params[3] = (cell)len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(buffer_, buffer, len);
  sampgdk_fakeamx_pop(buffer_);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetConsoleVarAsInt(const char * varname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell varname_;
  sampgdk_log_debug("GetConsoleVarAsInt(\"%s\")", varname);
  native = sampgdk_native_find_flexible("GetConsoleVarAsInt", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 1 * sizeof(cell);
  params[1] = varname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetConsoleVarAsBool(const char * varname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell varname_;
  sampgdk_log_debug("GetConsoleVarAsBool(\"%s\")", varname);
  native = sampgdk_native_find_flexible("GetConsoleVarAsBool", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 1 * sizeof(cell);
  params[1] = varname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetServerTickRate()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetServerTickRate()");
  native = sampgdk_native_find_flexible("GetServerTickRate", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NetStats_GetConnectedTime(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NetStats_GetConnectedTime(%d)", playerid);
  native = sampgdk_native_find_flexible("NetStats_GetConnectedTime", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NetStats_MessagesReceived(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NetStats_MessagesReceived(%d)", playerid);
  native = sampgdk_native_find_flexible("NetStats_MessagesReceived", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NetStats_BytesReceived(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NetStats_BytesReceived(%d)", playerid);
  native = sampgdk_native_find_flexible("NetStats_BytesReceived", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NetStats_MessagesSent(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NetStats_MessagesSent(%d)", playerid);
  native = sampgdk_native_find_flexible("NetStats_MessagesSent", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NetStats_BytesSent(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NetStats_BytesSent(%d)", playerid);
  native = sampgdk_native_find_flexible("NetStats_BytesSent", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NetStats_MessagesRecvPerSecond(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NetStats_MessagesRecvPerSecond(%d)", playerid);
  native = sampgdk_native_find_flexible("NetStats_MessagesRecvPerSecond", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, NetStats_PacketLossPercent(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NetStats_PacketLossPercent(%d)", playerid);
  native = sampgdk_native_find_flexible("NetStats_PacketLossPercent", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, NetStats_ConnectionStatus(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NetStats_ConnectionStatus(%d)", playerid);
  native = sampgdk_native_find_flexible("NetStats_ConnectionStatus", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NetStats_GetIpPort(int playerid, char * ip_port, int ip_port_len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell ip_port_;
  sampgdk_log_debug("NetStats_GetIpPort(%d, @%p, %d)", playerid, ip_port, ip_port_len);
  native = sampgdk_native_find_flexible("NetStats_GetIpPort", native);
  sampgdk_fakeamx_push(ip_port_len, &ip_port_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = ip_port_;
  params[3] = (cell)ip_port_len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(ip_port_, ip_port, ip_port_len);
  sampgdk_fakeamx_pop(ip_port_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, CreateMenu(const char * title, int columns, float x, float y, float col1width, float col2width)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  cell title_;
  sampgdk_log_debug("CreateMenu(\"%s\", %d, %f, %f, %f, %f)", title, columns, x, y, col1width, col2width);
  native = sampgdk_native_find_flexible("CreateMenu", native);
  sampgdk_fakeamx_push_string(title, NULL, &title_);
  params[0] = 6 * sizeof(cell);
  params[1] = title_;
  params[2] = (cell)columns;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  params[5] = amx_ftoc(col1width);
  params[6] = amx_ftoc(col2width);
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(title_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, DestroyMenu(int menuid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DestroyMenu(%d)", menuid);
  native = sampgdk_native_find_flexible("DestroyMenu", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)menuid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, AddMenuItem(int menuid, int column, const char * menutext)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell menutext_;
  sampgdk_log_debug("AddMenuItem(%d, %d, \"%s\")", menuid, column, menutext);
  native = sampgdk_native_find_flexible("AddMenuItem", native);
  sampgdk_fakeamx_push_string(menutext, NULL, &menutext_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)menuid;
  params[2] = (cell)column;
  params[3] = menutext_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(menutext_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetMenuColumnHeader(int menuid, int column, const char * columnheader)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell columnheader_;
  sampgdk_log_debug("SetMenuColumnHeader(%d, %d, \"%s\")", menuid, column, columnheader);
  native = sampgdk_native_find_flexible("SetMenuColumnHeader", native);
  sampgdk_fakeamx_push_string(columnheader, NULL, &columnheader_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)menuid;
  params[2] = (cell)column;
  params[3] = columnheader_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(columnheader_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ShowMenuForPlayer(int menuid, int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("ShowMenuForPlayer(%d, %d)", menuid, playerid);
  native = sampgdk_native_find_flexible("ShowMenuForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)menuid;
  params[2] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, HideMenuForPlayer(int menuid, int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("HideMenuForPlayer(%d, %d)", menuid, playerid);
  native = sampgdk_native_find_flexible("HideMenuForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)menuid;
  params[2] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidMenu(int menuid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsValidMenu(%d)", menuid);
  native = sampgdk_native_find_flexible("IsValidMenu", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)menuid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, DisableMenu(int menuid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DisableMenu(%d)", menuid);
  native = sampgdk_native_find_flexible("DisableMenu", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)menuid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, DisableMenuRow(int menuid, int row)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("DisableMenuRow(%d, %d)", menuid, row);
  native = sampgdk_native_find_flexible("DisableMenuRow", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)menuid;
  params[2] = (cell)row;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerMenu(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerMenu(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerMenu", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, TextDrawCreate(float x, float y, const char * text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell text_;
  sampgdk_log_debug("TextDrawCreate(%f, %f, \"%s\")", x, y, text);
  native = sampgdk_native_find_flexible("TextDrawCreate", native);
  sampgdk_fakeamx_push_string(text, NULL, &text_);
  params[0] = 3 * sizeof(cell);
  params[1] = amx_ftoc(x);
  params[2] = amx_ftoc(y);
  params[3] = text_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(text_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, TextDrawDestroy(int text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawDestroy(%d)", text);
  native = sampgdk_native_find_flexible("TextDrawDestroy", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)text;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawLetterSize(int text, float x, float y)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("TextDrawLetterSize(%d, %f, %f)", text, x, y);
  native = sampgdk_native_find_flexible("TextDrawLetterSize", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawTextSize(int text, float x, float y)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("TextDrawTextSize(%d, %f, %f)", text, x, y);
  native = sampgdk_native_find_flexible("TextDrawTextSize", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawAlignment(int text, int alignment)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawAlignment(%d, %d)", text, alignment);
  native = sampgdk_native_find_flexible("TextDrawAlignment", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = (cell)alignment;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawColor(int text, int color)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawColor(%d, %d)", text, color);
  native = sampgdk_native_find_flexible("TextDrawColor", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = (cell)color;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawUseBox(int text, bool use)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawUseBox(%d, %d)", text, use);
  native = sampgdk_native_find_flexible("TextDrawUseBox", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = (cell)use;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawBoxColor(int text, int color)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawBoxColor(%d, %d)", text, color);
  native = sampgdk_native_find_flexible("TextDrawBoxColor", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = (cell)color;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawSetShadow(int text, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawSetShadow(%d, %d)", text, size);
  native = sampgdk_native_find_flexible("TextDrawSetShadow", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawSetOutline(int text, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawSetOutline(%d, %d)", text, size);
  native = sampgdk_native_find_flexible("TextDrawSetOutline", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawBackgroundColor(int text, int color)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawBackgroundColor(%d, %d)", text, color);
  native = sampgdk_native_find_flexible("TextDrawBackgroundColor", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = (cell)color;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawFont(int text, int font)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawFont(%d, %d)", text, font);
  native = sampgdk_native_find_flexible("TextDrawFont", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = (cell)font;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawSetProportional(int text, bool set)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawSetProportional(%d, %d)", text, set);
  native = sampgdk_native_find_flexible("TextDrawSetProportional", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = (cell)set;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawSetSelectable(int text, bool set)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawSetSelectable(%d, %d)", text, set);
  native = sampgdk_native_find_flexible("TextDrawSetSelectable", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = (cell)set;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawShowForPlayer(int playerid, int text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawShowForPlayer(%d, %d)", playerid, text);
  native = sampgdk_native_find_flexible("TextDrawShowForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawHideForPlayer(int playerid, int text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawHideForPlayer(%d, %d)", playerid, text);
  native = sampgdk_native_find_flexible("TextDrawHideForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawShowForAll(int text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawShowForAll(%d)", text);
  native = sampgdk_native_find_flexible("TextDrawShowForAll", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)text;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawHideForAll(int text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawHideForAll(%d)", text);
  native = sampgdk_native_find_flexible("TextDrawHideForAll", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)text;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawSetString(int text, const char * string)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell string_;
  sampgdk_log_debug("TextDrawSetString(%d, \"%s\")", text, string);
  native = sampgdk_native_find_flexible("TextDrawSetString", native);
  sampgdk_fakeamx_push_string(string, NULL, &string_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = string_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(string_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawSetPreviewModel(int text, int modelindex)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TextDrawSetPreviewModel(%d, %d)", text, modelindex);
  native = sampgdk_native_find_flexible("TextDrawSetPreviewModel", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = (cell)modelindex;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawSetPreviewRot(int text, float fRotX, float fRotY, float fRotZ, float fZoom)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("TextDrawSetPreviewRot(%d, %f, %f, %f, %f)", text, fRotX, fRotY, fRotZ, fZoom);
  native = sampgdk_native_find_flexible("TextDrawSetPreviewRot", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = amx_ftoc(fRotX);
  params[3] = amx_ftoc(fRotY);
  params[4] = amx_ftoc(fRotZ);
  params[5] = amx_ftoc(fZoom);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawSetPreviewVehCol(int text, int color1, int color2)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("TextDrawSetPreviewVehCol(%d, %d, %d)", text, color1, color2);
  native = sampgdk_native_find_flexible("TextDrawSetPreviewVehCol", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)text;
  params[2] = (cell)color1;
  params[3] = (cell)color2;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SelectTextDraw(int playerid, int hovercolor)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SelectTextDraw(%d, %d)", playerid, hovercolor);
  native = sampgdk_native_find_flexible("SelectTextDraw", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)hovercolor;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, CancelSelectTextDraw(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("CancelSelectTextDraw(%d)", playerid);
  native = sampgdk_native_find_flexible("CancelSelectTextDraw", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GangZoneCreate(float minx, float miny, float maxx, float maxy)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("GangZoneCreate(%f, %f, %f, %f)", minx, miny, maxx, maxy);
  native = sampgdk_native_find_flexible("GangZoneCreate", native);
  params[0] = 4 * sizeof(cell);
  params[1] = amx_ftoc(minx);
  params[2] = amx_ftoc(miny);
  params[3] = amx_ftoc(maxx);
  params[4] = amx_ftoc(maxy);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GangZoneDestroy(int zone)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GangZoneDestroy(%d)", zone);
  native = sampgdk_native_find_flexible("GangZoneDestroy", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)zone;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GangZoneShowForPlayer(int playerid, int zone, int color)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("GangZoneShowForPlayer(%d, %d, %d)", playerid, zone, color);
  native = sampgdk_native_find_flexible("GangZoneShowForPlayer", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zone;
  params[3] = (cell)color;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GangZoneShowForAll(int zone, int color)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GangZoneShowForAll(%d, %d)", zone, color);
  native = sampgdk_native_find_flexible("GangZoneShowForAll", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)zone;
  params[2] = (cell)color;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GangZoneHideForPlayer(int playerid, int zone)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GangZoneHideForPlayer(%d, %d)", playerid, zone);
  native = sampgdk_native_find_flexible("GangZoneHideForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zone;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GangZoneHideForAll(int zone)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GangZoneHideForAll(%d)", zone);
  native = sampgdk_native_find_flexible("GangZoneHideForAll", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)zone;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GangZoneFlashForPlayer(int playerid, int zone, int flashcolor)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("GangZoneFlashForPlayer(%d, %d, %d)", playerid, zone, flashcolor);
  native = sampgdk_native_find_flexible("GangZoneFlashForPlayer", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zone;
  params[3] = (cell)flashcolor;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GangZoneFlashForAll(int zone, int flashcolor)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GangZoneFlashForAll(%d, %d)", zone, flashcolor);
  native = sampgdk_native_find_flexible("GangZoneFlashForAll", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)zone;
  params[2] = (cell)flashcolor;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GangZoneStopFlashForPlayer(int playerid, int zone)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GangZoneStopFlashForPlayer(%d, %d)", playerid, zone);
  native = sampgdk_native_find_flexible("GangZoneStopFlashForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zone;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GangZoneStopFlashForAll(int zone)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GangZoneStopFlashForAll(%d)", zone);
  native = sampgdk_native_find_flexible("GangZoneStopFlashForAll", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)zone;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, Create3DTextLabel(const char * text, int color, float x, float y, float z, float DrawDistance, int virtualworld, bool testLOS)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  cell text_;
  sampgdk_log_debug("Create3DTextLabel(\"%s\", %d, %f, %f, %f, %f, %d, %d)", text, color, x, y, z, DrawDistance, virtualworld, testLOS);
  native = sampgdk_native_find_flexible("Create3DTextLabel", native);
  sampgdk_fakeamx_push_string(text, NULL, &text_);
  params[0] = 8 * sizeof(cell);
  params[1] = text_;
  params[2] = (cell)color;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  params[5] = amx_ftoc(z);
  params[6] = amx_ftoc(DrawDistance);
  params[7] = (cell)virtualworld;
  params[8] = (cell)testLOS;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(text_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, Delete3DTextLabel(int id)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("Delete3DTextLabel(%d)", id);
  native = sampgdk_native_find_flexible("Delete3DTextLabel", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)id;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, Attach3DTextLabelToPlayer(int id, int playerid, float OffsetX, float OffsetY, float OffsetZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("Attach3DTextLabelToPlayer(%d, %d, %f, %f, %f)", id, playerid, OffsetX, OffsetY, OffsetZ);
  native = sampgdk_native_find_flexible("Attach3DTextLabelToPlayer", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)id;
  params[2] = (cell)playerid;
  params[3] = amx_ftoc(OffsetX);
  params[4] = amx_ftoc(OffsetY);
  params[5] = amx_ftoc(OffsetZ);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, Attach3DTextLabelToVehicle(int id, int vehicleid, float OffsetX, float OffsetY, float OffsetZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("Attach3DTextLabelToVehicle(%d, %d, %f, %f, %f)", id, vehicleid, OffsetX, OffsetY, OffsetZ);
  native = sampgdk_native_find_flexible("Attach3DTextLabelToVehicle", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)id;
  params[2] = (cell)vehicleid;
  params[3] = amx_ftoc(OffsetX);
  params[4] = amx_ftoc(OffsetY);
  params[5] = amx_ftoc(OffsetZ);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, Update3DTextLabelText(int id, int color, const char * text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell text_;
  sampgdk_log_debug("Update3DTextLabelText(%d, %d, \"%s\")", id, color, text);
  native = sampgdk_native_find_flexible("Update3DTextLabelText", native);
  sampgdk_fakeamx_push_string(text, NULL, &text_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)id;
  params[2] = (cell)color;
  params[3] = text_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(text_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, CreatePlayer3DTextLabel(int playerid, const char * text, int color, float x, float y, float z, float DrawDistance, int attachedplayer, int attachedvehicle, bool testLOS)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[11];
  cell text_;
  sampgdk_log_debug("CreatePlayer3DTextLabel(%d, \"%s\", %d, %f, %f, %f, %f, %d, %d, %d)", playerid, text, color, x, y, z, DrawDistance, attachedplayer, attachedvehicle, testLOS);
  native = sampgdk_native_find_flexible("CreatePlayer3DTextLabel", native);
  sampgdk_fakeamx_push_string(text, NULL, &text_);
  params[0] = 10 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = text_;
  params[3] = (cell)color;
  params[4] = amx_ftoc(x);
  params[5] = amx_ftoc(y);
  params[6] = amx_ftoc(z);
  params[7] = amx_ftoc(DrawDistance);
  params[8] = (cell)attachedplayer;
  params[9] = (cell)attachedvehicle;
  params[10] = (cell)testLOS;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(text_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, DeletePlayer3DTextLabel(int playerid, int id)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("DeletePlayer3DTextLabel(%d, %d)", playerid, id);
  native = sampgdk_native_find_flexible("DeletePlayer3DTextLabel", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)id;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, UpdatePlayer3DTextLabelText(int playerid, int id, int color, const char * text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell text_;
  sampgdk_log_debug("UpdatePlayer3DTextLabelText(%d, %d, %d, \"%s\")", playerid, id, color, text);
  native = sampgdk_native_find_flexible("UpdatePlayer3DTextLabelText", native);
  sampgdk_fakeamx_push_string(text, NULL, &text_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)id;
  params[3] = (cell)color;
  params[4] = text_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(text_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ShowPlayerDialog(int playerid, int dialogid, int style, const char * caption, const char * info, const char * button1, const char * button2)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[8];
  cell caption_;
  cell info_;
  cell button1_;
  cell button2_;
  sampgdk_log_debug("ShowPlayerDialog(%d, %d, %d, \"%s\", \"%s\", \"%s\", \"%s\")", playerid, dialogid, style, caption, info, button1, button2);
  native = sampgdk_native_find_flexible("ShowPlayerDialog", native);
  sampgdk_fakeamx_push_string(caption, NULL, &caption_);
  sampgdk_fakeamx_push_string(info, NULL, &info_);
  sampgdk_fakeamx_push_string(button1, NULL, &button1_);
  sampgdk_fakeamx_push_string(button2, NULL, &button2_);
  params[0] = 7 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)dialogid;
  params[3] = (cell)style;
  params[4] = caption_;
  params[5] = info_;
  params[6] = button1_;
  params[7] = button2_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(button2_);
  sampgdk_fakeamx_pop(button1_);
  sampgdk_fakeamx_pop(info_);
  sampgdk_fakeamx_pop(caption_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, gpci(int playerid, char * buffer, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell buffer_;
  sampgdk_log_debug("gpci(%d, @%p, %d)", playerid, buffer, size);
  native = sampgdk_native_find_flexible("gpci", native);
  sampgdk_fakeamx_push(size, &buffer_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = buffer_;
  params[3] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(buffer_, buffer, size);
  sampgdk_fakeamx_pop(buffer_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, AddCharModel(int baseid, int newid, const char * dffname, const char * txdname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell dffname_;
  cell txdname_;
  sampgdk_log_debug("AddCharModel(%d, %d, \"%s\", \"%s\")", baseid, newid, dffname, txdname);
  native = sampgdk_native_find_flexible("AddCharModel", native);
  sampgdk_fakeamx_push_string(dffname, NULL, &dffname_);
  sampgdk_fakeamx_push_string(txdname, NULL, &txdname_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)baseid;
  params[2] = (cell)newid;
  params[3] = dffname_;
  params[4] = txdname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(txdname_);
  sampgdk_fakeamx_pop(dffname_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, AddSimpleModel(int virtualworld, int baseid, int newid, const char * dffname, const char * txdname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell dffname_;
  cell txdname_;
  sampgdk_log_debug("AddSimpleModel(%d, %d, %d, \"%s\", \"%s\")", virtualworld, baseid, newid, dffname, txdname);
  native = sampgdk_native_find_flexible("AddSimpleModel", native);
  sampgdk_fakeamx_push_string(dffname, NULL, &dffname_);
  sampgdk_fakeamx_push_string(txdname, NULL, &txdname_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)virtualworld;
  params[2] = (cell)baseid;
  params[3] = (cell)newid;
  params[4] = dffname_;
  params[5] = txdname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(txdname_);
  sampgdk_fakeamx_pop(dffname_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, AddSimpleModelTimed(int virtualworld, int baseid, int newid, const char * dffname, const char * txdname, int timeon, int timeoff)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[8];
  cell dffname_;
  cell txdname_;
  sampgdk_log_debug("AddSimpleModelTimed(%d, %d, %d, \"%s\", \"%s\", %d, %d)", virtualworld, baseid, newid, dffname, txdname, timeon, timeoff);
  native = sampgdk_native_find_flexible("AddSimpleModelTimed", native);
  sampgdk_fakeamx_push_string(dffname, NULL, &dffname_);
  sampgdk_fakeamx_push_string(txdname, NULL, &txdname_);
  params[0] = 7 * sizeof(cell);
  params[1] = (cell)virtualworld;
  params[2] = (cell)baseid;
  params[3] = (cell)newid;
  params[4] = dffname_;
  params[5] = txdname_;
  params[6] = (cell)timeon;
  params[7] = (cell)timeoff;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(txdname_);
  sampgdk_fakeamx_pop(dffname_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, FindModelFileNameFromCRC(int crc, char * model_str, int model_str_len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell model_str_;
  sampgdk_log_debug("FindModelFileNameFromCRC(%d, @%p, %d)", crc, model_str, model_str_len);
  native = sampgdk_native_find_flexible("FindModelFileNameFromCRC", native);
  sampgdk_fakeamx_push(model_str_len, &model_str_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)crc;
  params[2] = model_str_;
  params[3] = (cell)model_str_len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(model_str_, model_str, model_str_len);
  sampgdk_fakeamx_pop(model_str_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, FindTextureFileNameFromCRC(int crc, char * texture_str, int texture_str_len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell texture_str_;
  sampgdk_log_debug("FindTextureFileNameFromCRC(%d, @%p, %d)", crc, texture_str, texture_str_len);
  native = sampgdk_native_find_flexible("FindTextureFileNameFromCRC", native);
  sampgdk_fakeamx_push(texture_str_len, &texture_str_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)crc;
  params[2] = texture_str_;
  params[3] = (cell)texture_str_len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(texture_str_, texture_str, texture_str_len);
  sampgdk_fakeamx_pop(texture_str_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, RedirectDownload(int playerid, const char * url)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell url_;
  sampgdk_log_debug("RedirectDownload(%d, \"%s\")", playerid, url);
  native = sampgdk_native_find_flexible("RedirectDownload", native);
  sampgdk_fakeamx_push_string(url, NULL, &url_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = url_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(url_);
  return !!(retval);
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnGameModeInit_callback)();
static bool _OnGameModeInit(AMX *amx, void *callback, cell *retval) {
  sampgdk_log_debug("OnGameModeInit()");
  ((OnGameModeInit_callback)callback)();
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnGameModeExit_callback)();
static bool _OnGameModeExit(AMX *amx, void *callback, cell *retval) {
  sampgdk_log_debug("OnGameModeExit()");
  ((OnGameModeExit_callback)callback)();
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerConnect_callback)(int playerid);
static bool _OnPlayerConnect(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerConnect(%d)", playerid);
  retval_ = ((OnPlayerConnect_callback)callback)(playerid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerDisconnect_callback)(int playerid, int reason);
static bool _OnPlayerDisconnect(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int reason;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&reason);
  sampgdk_log_debug("OnPlayerDisconnect(%d, %d)", playerid, reason);
  retval_ = ((OnPlayerDisconnect_callback)callback)(playerid, reason);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerSpawn_callback)(int playerid);
static bool _OnPlayerSpawn(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerSpawn(%d)", playerid);
  retval_ = ((OnPlayerSpawn_callback)callback)(playerid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerDeath_callback)(int playerid, int killerid, int reason);
static bool _OnPlayerDeath(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int killerid;
  int reason;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&killerid);
  sampgdk_param_get_cell(amx, 2, (cell *)&reason);
  sampgdk_log_debug("OnPlayerDeath(%d, %d, %d)", playerid, killerid, reason);
  retval_ = ((OnPlayerDeath_callback)callback)(playerid, killerid, reason);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleSpawn_callback)(int vehicleid);
static bool _OnVehicleSpawn(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int vehicleid;
  sampgdk_param_get_cell(amx, 0, (cell *)&vehicleid);
  sampgdk_log_debug("OnVehicleSpawn(%d)", vehicleid);
  retval_ = ((OnVehicleSpawn_callback)callback)(vehicleid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleDeath_callback)(int vehicleid, int killerid);
static bool _OnVehicleDeath(AMX *amx, void *callback, cell *retval) {
  int vehicleid;
  int killerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 1, (cell *)&killerid);
  sampgdk_log_debug("OnVehicleDeath(%d, %d)", vehicleid, killerid);
  ((OnVehicleDeath_callback)callback)(vehicleid, killerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerText_callback)(int playerid, const char * text);
static bool _OnPlayerText(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  const char * text;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_string(amx, 1, (char * *)&text);
  sampgdk_log_debug("OnPlayerText(%d, \"%s\")", playerid, text);
  retval_ = ((OnPlayerText_callback)callback)(playerid, text);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  free((void *)text);
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerCommandText_callback)(int playerid, const char * cmdtext);
static bool _OnPlayerCommandText(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  const char * cmdtext;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_string(amx, 1, (char * *)&cmdtext);
  sampgdk_log_debug("OnPlayerCommandText(%d, \"%s\")", playerid, cmdtext);
  retval_ = ((OnPlayerCommandText_callback)callback)(playerid, cmdtext);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  free((void *)cmdtext);
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerRequestClass_callback)(int playerid, int classid);
static bool _OnPlayerRequestClass(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int classid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&classid);
  sampgdk_log_debug("OnPlayerRequestClass(%d, %d)", playerid, classid);
  ((OnPlayerRequestClass_callback)callback)(playerid, classid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerEnterVehicle_callback)(int playerid, int vehicleid, bool ispassenger);
static bool _OnPlayerEnterVehicle(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int vehicleid;
  bool ispassenger;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_param_get_bool(amx, 2, (bool *)&ispassenger);
  sampgdk_log_debug("OnPlayerEnterVehicle(%d, %d, %d)", playerid, vehicleid, ispassenger);
  ((OnPlayerEnterVehicle_callback)callback)(playerid, vehicleid, ispassenger);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerExitVehicle_callback)(int playerid, int vehicleid);
static bool _OnPlayerExitVehicle(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int vehicleid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_log_debug("OnPlayerExitVehicle(%d, %d)", playerid, vehicleid);
  ((OnPlayerExitVehicle_callback)callback)(playerid, vehicleid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerStateChange_callback)(int playerid, int newstate, int oldstate);
static bool _OnPlayerStateChange(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int newstate;
  int oldstate;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&newstate);
  sampgdk_param_get_cell(amx, 2, (cell *)&oldstate);
  sampgdk_log_debug("OnPlayerStateChange(%d, %d, %d)", playerid, newstate, oldstate);
  ((OnPlayerStateChange_callback)callback)(playerid, newstate, oldstate);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerEnterCheckpoint_callback)(int playerid);
static bool _OnPlayerEnterCheckpoint(AMX *amx, void *callback, cell *retval) {
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerEnterCheckpoint(%d)", playerid);
  ((OnPlayerEnterCheckpoint_callback)callback)(playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerLeaveCheckpoint_callback)(int playerid);
static bool _OnPlayerLeaveCheckpoint(AMX *amx, void *callback, cell *retval) {
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerLeaveCheckpoint(%d)", playerid);
  ((OnPlayerLeaveCheckpoint_callback)callback)(playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerEnterRaceCheckpoint_callback)(int playerid);
static bool _OnPlayerEnterRaceCheckpoint(AMX *amx, void *callback, cell *retval) {
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerEnterRaceCheckpoint(%d)", playerid);
  ((OnPlayerEnterRaceCheckpoint_callback)callback)(playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerLeaveRaceCheckpoint_callback)(int playerid);
static bool _OnPlayerLeaveRaceCheckpoint(AMX *amx, void *callback, cell *retval) {
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerLeaveRaceCheckpoint(%d)", playerid);
  ((OnPlayerLeaveRaceCheckpoint_callback)callback)(playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnRconCommand_callback)(const char * cmd);
static bool _OnRconCommand(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  const char * cmd;
  sampgdk_param_get_string(amx, 0, (char * *)&cmd);
  sampgdk_log_debug("OnRconCommand(\"%s\")", cmd);
  retval_ = ((OnRconCommand_callback)callback)(cmd);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  free((void *)cmd);
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerRequestSpawn_callback)(int playerid);
static bool _OnPlayerRequestSpawn(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerRequestSpawn(%d)", playerid);
  retval_ = ((OnPlayerRequestSpawn_callback)callback)(playerid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnObjectMoved_callback)(int objectid);
static bool _OnObjectMoved(AMX *amx, void *callback, cell *retval) {
  int objectid;
  sampgdk_param_get_cell(amx, 0, (cell *)&objectid);
  sampgdk_log_debug("OnObjectMoved(%d)", objectid);
  ((OnObjectMoved_callback)callback)(objectid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerObjectMoved_callback)(int playerid, int objectid);
static bool _OnPlayerObjectMoved(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int objectid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&objectid);
  sampgdk_log_debug("OnPlayerObjectMoved(%d, %d)", playerid, objectid);
  ((OnPlayerObjectMoved_callback)callback)(playerid, objectid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerPickUpPickup_callback)(int playerid, int pickupid);
static bool _OnPlayerPickUpPickup(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int pickupid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&pickupid);
  sampgdk_log_debug("OnPlayerPickUpPickup(%d, %d)", playerid, pickupid);
  ((OnPlayerPickUpPickup_callback)callback)(playerid, pickupid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleMod_callback)(int playerid, int vehicleid, int componentid);
static bool _OnVehicleMod(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int vehicleid;
  int componentid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 2, (cell *)&componentid);
  sampgdk_log_debug("OnVehicleMod(%d, %d, %d)", playerid, vehicleid, componentid);
  retval_ = ((OnVehicleMod_callback)callback)(playerid, vehicleid, componentid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnEnterExitModShop_callback)(int playerid, bool enterexit, int interiorid);
static bool _OnEnterExitModShop(AMX *amx, void *callback, cell *retval) {
  int playerid;
  bool enterexit;
  int interiorid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_bool(amx, 1, (bool *)&enterexit);
  sampgdk_param_get_cell(amx, 2, (cell *)&interiorid);
  sampgdk_log_debug("OnEnterExitModShop(%d, %d, %d)", playerid, enterexit, interiorid);
  ((OnEnterExitModShop_callback)callback)(playerid, enterexit, interiorid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehiclePaintjob_callback)(int playerid, int vehicleid, int paintjobid);
static bool _OnVehiclePaintjob(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int vehicleid;
  int paintjobid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 2, (cell *)&paintjobid);
  sampgdk_log_debug("OnVehiclePaintjob(%d, %d, %d)", playerid, vehicleid, paintjobid);
  retval_ = ((OnVehiclePaintjob_callback)callback)(playerid, vehicleid, paintjobid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleRespray_callback)(int playerid, int vehicleid, int color1, int color2);
static bool _OnVehicleRespray(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int vehicleid;
  int color1;
  int color2;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 2, (cell *)&color1);
  sampgdk_param_get_cell(amx, 3, (cell *)&color2);
  sampgdk_log_debug("OnVehicleRespray(%d, %d, %d, %d)", playerid, vehicleid, color1, color2);
  retval_ = ((OnVehicleRespray_callback)callback)(playerid, vehicleid, color1, color2);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleDamageStatusUpdate_callback)(int vehicleid, int playerid);
static bool _OnVehicleDamageStatusUpdate(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int vehicleid;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 1, (cell *)&playerid);
  sampgdk_log_debug("OnVehicleDamageStatusUpdate(%d, %d)", vehicleid, playerid);
  retval_ = ((OnVehicleDamageStatusUpdate_callback)callback)(vehicleid, playerid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnUnoccupiedVehicleUpdate_callback)(int vehicleid, int playerid, int passenger_seat, float new_x, float new_y, float new_z, float vel_x, float vel_y, float vel_z);
static bool _OnUnoccupiedVehicleUpdate(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int vehicleid;
  int playerid;
  int passenger_seat;
  float new_x;
  float new_y;
  float new_z;
  float vel_x;
  float vel_y;
  float vel_z;
  sampgdk_param_get_cell(amx, 0, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 1, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 2, (cell *)&passenger_seat);
  sampgdk_param_get_float(amx, 3, (float *)&new_x);
  sampgdk_param_get_float(amx, 4, (float *)&new_y);
  sampgdk_param_get_float(amx, 5, (float *)&new_z);
  sampgdk_param_get_float(amx, 6, (float *)&vel_x);
  sampgdk_param_get_float(amx, 7, (float *)&vel_y);
  sampgdk_param_get_float(amx, 8, (float *)&vel_z);
  sampgdk_log_debug("OnUnoccupiedVehicleUpdate(%d, %d, %d, %f, %f, %f, %f, %f, %f)", vehicleid, playerid, passenger_seat, new_x, new_y, new_z, vel_x, vel_y, vel_z);
  retval_ = ((OnUnoccupiedVehicleUpdate_callback)callback)(vehicleid, playerid, passenger_seat, new_x, new_y, new_z, vel_x, vel_y, vel_z);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerSelectedMenuRow_callback)(int playerid, int row);
static bool _OnPlayerSelectedMenuRow(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int row;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&row);
  sampgdk_log_debug("OnPlayerSelectedMenuRow(%d, %d)", playerid, row);
  ((OnPlayerSelectedMenuRow_callback)callback)(playerid, row);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerExitedMenu_callback)(int playerid);
static bool _OnPlayerExitedMenu(AMX *amx, void *callback, cell *retval) {
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerExitedMenu(%d)", playerid);
  ((OnPlayerExitedMenu_callback)callback)(playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerInteriorChange_callback)(int playerid, int newinteriorid, int oldinteriorid);
static bool _OnPlayerInteriorChange(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int newinteriorid;
  int oldinteriorid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&newinteriorid);
  sampgdk_param_get_cell(amx, 2, (cell *)&oldinteriorid);
  sampgdk_log_debug("OnPlayerInteriorChange(%d, %d, %d)", playerid, newinteriorid, oldinteriorid);
  ((OnPlayerInteriorChange_callback)callback)(playerid, newinteriorid, oldinteriorid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerKeyStateChange_callback)(int playerid, int newkeys, int oldkeys);
static bool _OnPlayerKeyStateChange(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int newkeys;
  int oldkeys;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&newkeys);
  sampgdk_param_get_cell(amx, 2, (cell *)&oldkeys);
  sampgdk_log_debug("OnPlayerKeyStateChange(%d, %d, %d)", playerid, newkeys, oldkeys);
  ((OnPlayerKeyStateChange_callback)callback)(playerid, newkeys, oldkeys);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnRconLoginAttempt_callback)(const char * ip, const char * password, bool success);
static bool _OnRconLoginAttempt(AMX *amx, void *callback, cell *retval) {
  const char * ip;
  const char * password;
  bool success;
  sampgdk_param_get_string(amx, 0, (char * *)&ip);
  sampgdk_param_get_string(amx, 1, (char * *)&password);
  sampgdk_param_get_bool(amx, 2, (bool *)&success);
  sampgdk_log_debug("OnRconLoginAttempt(\"%s\", \"%s\", %d)", ip, password, success);
  ((OnRconLoginAttempt_callback)callback)(ip, password, success);
  free((void *)ip);
  free((void *)password);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerUpdate_callback)(int playerid);
static bool _OnPlayerUpdate(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerUpdate(%d)", playerid);
  retval_ = ((OnPlayerUpdate_callback)callback)(playerid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerStreamIn_callback)(int playerid, int forplayerid);
static bool _OnPlayerStreamIn(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int forplayerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&forplayerid);
  sampgdk_log_debug("OnPlayerStreamIn(%d, %d)", playerid, forplayerid);
  ((OnPlayerStreamIn_callback)callback)(playerid, forplayerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerStreamOut_callback)(int playerid, int forplayerid);
static bool _OnPlayerStreamOut(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int forplayerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&forplayerid);
  sampgdk_log_debug("OnPlayerStreamOut(%d, %d)", playerid, forplayerid);
  ((OnPlayerStreamOut_callback)callback)(playerid, forplayerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleStreamIn_callback)(int vehicleid, int forplayerid);
static bool _OnVehicleStreamIn(AMX *amx, void *callback, cell *retval) {
  int vehicleid;
  int forplayerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 1, (cell *)&forplayerid);
  sampgdk_log_debug("OnVehicleStreamIn(%d, %d)", vehicleid, forplayerid);
  ((OnVehicleStreamIn_callback)callback)(vehicleid, forplayerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleStreamOut_callback)(int vehicleid, int forplayerid);
static bool _OnVehicleStreamOut(AMX *amx, void *callback, cell *retval) {
  int vehicleid;
  int forplayerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 1, (cell *)&forplayerid);
  sampgdk_log_debug("OnVehicleStreamOut(%d, %d)", vehicleid, forplayerid);
  ((OnVehicleStreamOut_callback)callback)(vehicleid, forplayerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnActorStreamIn_callback)(int actorid, int forplayerid);
static bool _OnActorStreamIn(AMX *amx, void *callback, cell *retval) {
  int actorid;
  int forplayerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&actorid);
  sampgdk_param_get_cell(amx, 1, (cell *)&forplayerid);
  sampgdk_log_debug("OnActorStreamIn(%d, %d)", actorid, forplayerid);
  ((OnActorStreamIn_callback)callback)(actorid, forplayerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnActorStreamOut_callback)(int actorid, int forplayerid);
static bool _OnActorStreamOut(AMX *amx, void *callback, cell *retval) {
  int actorid;
  int forplayerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&actorid);
  sampgdk_param_get_cell(amx, 1, (cell *)&forplayerid);
  sampgdk_log_debug("OnActorStreamOut(%d, %d)", actorid, forplayerid);
  ((OnActorStreamOut_callback)callback)(actorid, forplayerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnDialogResponse_callback)(int playerid, int dialogid, int response, int listitem, const char * inputtext);
static bool _OnDialogResponse(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int dialogid;
  int response;
  int listitem;
  const char * inputtext;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&dialogid);
  sampgdk_param_get_cell(amx, 2, (cell *)&response);
  sampgdk_param_get_cell(amx, 3, (cell *)&listitem);
  sampgdk_param_get_string(amx, 4, (char * *)&inputtext);
  sampgdk_log_debug("OnDialogResponse(%d, %d, %d, %d, \"%s\")", playerid, dialogid, response, listitem, inputtext);
  retval_ = ((OnDialogResponse_callback)callback)(playerid, dialogid, response, listitem, inputtext);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  free((void *)inputtext);
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerTakeDamage_callback)(int playerid, int issuerid, float amount, int weaponid, int bodypart);
static bool _OnPlayerTakeDamage(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int issuerid;
  float amount;
  int weaponid;
  int bodypart;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&issuerid);
  sampgdk_param_get_float(amx, 2, (float *)&amount);
  sampgdk_param_get_cell(amx, 3, (cell *)&weaponid);
  sampgdk_param_get_cell(amx, 4, (cell *)&bodypart);
  sampgdk_log_debug("OnPlayerTakeDamage(%d, %d, %f, %d, %d)", playerid, issuerid, amount, weaponid, bodypart);
  retval_ = ((OnPlayerTakeDamage_callback)callback)(playerid, issuerid, amount, weaponid, bodypart);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerGiveDamage_callback)(int playerid, int damagedid, float amount, int weaponid, int bodypart);
static bool _OnPlayerGiveDamage(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int damagedid;
  float amount;
  int weaponid;
  int bodypart;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&damagedid);
  sampgdk_param_get_float(amx, 2, (float *)&amount);
  sampgdk_param_get_cell(amx, 3, (cell *)&weaponid);
  sampgdk_param_get_cell(amx, 4, (cell *)&bodypart);
  sampgdk_log_debug("OnPlayerGiveDamage(%d, %d, %f, %d, %d)", playerid, damagedid, amount, weaponid, bodypart);
  retval_ = ((OnPlayerGiveDamage_callback)callback)(playerid, damagedid, amount, weaponid, bodypart);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerGiveDamageActor_callback)(int playerid, int damaged_actorid, float amount, int weaponid, int bodypart);
static bool _OnPlayerGiveDamageActor(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int damaged_actorid;
  float amount;
  int weaponid;
  int bodypart;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&damaged_actorid);
  sampgdk_param_get_float(amx, 2, (float *)&amount);
  sampgdk_param_get_cell(amx, 3, (cell *)&weaponid);
  sampgdk_param_get_cell(amx, 4, (cell *)&bodypart);
  sampgdk_log_debug("OnPlayerGiveDamageActor(%d, %d, %f, %d, %d)", playerid, damaged_actorid, amount, weaponid, bodypart);
  retval_ = ((OnPlayerGiveDamageActor_callback)callback)(playerid, damaged_actorid, amount, weaponid, bodypart);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerClickMap_callback)(int playerid, float fX, float fY, float fZ);
static bool _OnPlayerClickMap(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  float fX;
  float fY;
  float fZ;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_float(amx, 1, (float *)&fX);
  sampgdk_param_get_float(amx, 2, (float *)&fY);
  sampgdk_param_get_float(amx, 3, (float *)&fZ);
  sampgdk_log_debug("OnPlayerClickMap(%d, %f, %f, %f)", playerid, fX, fY, fZ);
  retval_ = ((OnPlayerClickMap_callback)callback)(playerid, fX, fY, fZ);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerClickTextDraw_callback)(int playerid, int clickedid);
static bool _OnPlayerClickTextDraw(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int clickedid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&clickedid);
  sampgdk_log_debug("OnPlayerClickTextDraw(%d, %d)", playerid, clickedid);
  retval_ = ((OnPlayerClickTextDraw_callback)callback)(playerid, clickedid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerClickPlayerTextDraw_callback)(int playerid, int playertextid);
static bool _OnPlayerClickPlayerTextDraw(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int playertextid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&playertextid);
  sampgdk_log_debug("OnPlayerClickPlayerTextDraw(%d, %d)", playerid, playertextid);
  retval_ = ((OnPlayerClickPlayerTextDraw_callback)callback)(playerid, playertextid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnIncomingConnection_callback)(int playerid, const char * ip_address, int port);
static bool _OnIncomingConnection(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  const char * ip_address;
  int port;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_string(amx, 1, (char * *)&ip_address);
  sampgdk_param_get_cell(amx, 2, (cell *)&port);
  sampgdk_log_debug("OnIncomingConnection(%d, \"%s\", %d)", playerid, ip_address, port);
  retval_ = ((OnIncomingConnection_callback)callback)(playerid, ip_address, port);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  free((void *)ip_address);
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnTrailerUpdate_callback)(int playerid, int vehicleid);
static bool _OnTrailerUpdate(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int vehicleid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_log_debug("OnTrailerUpdate(%d, %d)", playerid, vehicleid);
  retval_ = ((OnTrailerUpdate_callback)callback)(playerid, vehicleid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleSirenStateChange_callback)(int playerid, int vehicleid, int newstate);
static bool _OnVehicleSirenStateChange(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int vehicleid;
  int newstate;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 2, (cell *)&newstate);
  sampgdk_log_debug("OnVehicleSirenStateChange(%d, %d, %d)", playerid, vehicleid, newstate);
  ((OnVehicleSirenStateChange_callback)callback)(playerid, vehicleid, newstate);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerClickPlayer_callback)(int playerid, int clickedplayerid, int source);
static bool _OnPlayerClickPlayer(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int clickedplayerid;
  int source;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&clickedplayerid);
  sampgdk_param_get_cell(amx, 2, (cell *)&source);
  sampgdk_log_debug("OnPlayerClickPlayer(%d, %d, %d)", playerid, clickedplayerid, source);
  retval_ = ((OnPlayerClickPlayer_callback)callback)(playerid, clickedplayerid, source);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerEditObject_callback)(int playerid, bool playerobject, int objectid, int response, float fX, float fY, float fZ, float fRotX, float fRotY, float fRotZ);
static bool _OnPlayerEditObject(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  bool playerobject;
  int objectid;
  int response;
  float fX;
  float fY;
  float fZ;
  float fRotX;
  float fRotY;
  float fRotZ;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_bool(amx, 1, (bool *)&playerobject);
  sampgdk_param_get_cell(amx, 2, (cell *)&objectid);
  sampgdk_param_get_cell(amx, 3, (cell *)&response);
  sampgdk_param_get_float(amx, 4, (float *)&fX);
  sampgdk_param_get_float(amx, 5, (float *)&fY);
  sampgdk_param_get_float(amx, 6, (float *)&fZ);
  sampgdk_param_get_float(amx, 7, (float *)&fRotX);
  sampgdk_param_get_float(amx, 8, (float *)&fRotY);
  sampgdk_param_get_float(amx, 9, (float *)&fRotZ);
  sampgdk_log_debug("OnPlayerEditObject(%d, %d, %d, %d, %f, %f, %f, %f, %f, %f)", playerid, playerobject, objectid, response, fX, fY, fZ, fRotX, fRotY, fRotZ);
  retval_ = ((OnPlayerEditObject_callback)callback)(playerid, playerobject, objectid, response, fX, fY, fZ, fRotX, fRotY, fRotZ);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerEditAttachedObject_callback)(int playerid, int response, int index, int modelid, int boneid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ, float fScaleX, float fScaleY, float fScaleZ);
static bool _OnPlayerEditAttachedObject(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int response;
  int index;
  int modelid;
  int boneid;
  float fOffsetX;
  float fOffsetY;
  float fOffsetZ;
  float fRotX;
  float fRotY;
  float fRotZ;
  float fScaleX;
  float fScaleY;
  float fScaleZ;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&response);
  sampgdk_param_get_cell(amx, 2, (cell *)&index);
  sampgdk_param_get_cell(amx, 3, (cell *)&modelid);
  sampgdk_param_get_cell(amx, 4, (cell *)&boneid);
  sampgdk_param_get_float(amx, 5, (float *)&fOffsetX);
  sampgdk_param_get_float(amx, 6, (float *)&fOffsetY);
  sampgdk_param_get_float(amx, 7, (float *)&fOffsetZ);
  sampgdk_param_get_float(amx, 8, (float *)&fRotX);
  sampgdk_param_get_float(amx, 9, (float *)&fRotY);
  sampgdk_param_get_float(amx, 10, (float *)&fRotZ);
  sampgdk_param_get_float(amx, 11, (float *)&fScaleX);
  sampgdk_param_get_float(amx, 12, (float *)&fScaleY);
  sampgdk_param_get_float(amx, 13, (float *)&fScaleZ);
  sampgdk_log_debug("OnPlayerEditAttachedObject(%d, %d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f)", playerid, response, index, modelid, boneid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ);
  retval_ = ((OnPlayerEditAttachedObject_callback)callback)(playerid, response, index, modelid, boneid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerSelectObject_callback)(int playerid, int type, int objectid, int modelid, float fX, float fY, float fZ);
static bool _OnPlayerSelectObject(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int type;
  int objectid;
  int modelid;
  float fX;
  float fY;
  float fZ;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&type);
  sampgdk_param_get_cell(amx, 2, (cell *)&objectid);
  sampgdk_param_get_cell(amx, 3, (cell *)&modelid);
  sampgdk_param_get_float(amx, 4, (float *)&fX);
  sampgdk_param_get_float(amx, 5, (float *)&fY);
  sampgdk_param_get_float(amx, 6, (float *)&fZ);
  sampgdk_log_debug("OnPlayerSelectObject(%d, %d, %d, %d, %f, %f, %f)", playerid, type, objectid, modelid, fX, fY, fZ);
  retval_ = ((OnPlayerSelectObject_callback)callback)(playerid, type, objectid, modelid, fX, fY, fZ);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerWeaponShot_callback)(int playerid, int weaponid, int hittype, int hitid, float fX, float fY, float fZ);
static bool _OnPlayerWeaponShot(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int weaponid;
  int hittype;
  int hitid;
  float fX;
  float fY;
  float fZ;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&weaponid);
  sampgdk_param_get_cell(amx, 2, (cell *)&hittype);
  sampgdk_param_get_cell(amx, 3, (cell *)&hitid);
  sampgdk_param_get_float(amx, 4, (float *)&fX);
  sampgdk_param_get_float(amx, 5, (float *)&fY);
  sampgdk_param_get_float(amx, 6, (float *)&fZ);
  sampgdk_log_debug("OnPlayerWeaponShot(%d, %d, %d, %d, %f, %f, %f)", playerid, weaponid, hittype, hitid, fX, fY, fZ);
  retval_ = ((OnPlayerWeaponShot_callback)callback)(playerid, weaponid, hittype, hitid, fX, fY, fZ);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerRequestDownload_callback)(int playerid, int type, int crc);
static bool _OnPlayerRequestDownload(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int type;
  int crc;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&type);
  sampgdk_param_get_cell(amx, 2, (cell *)&crc);
  sampgdk_log_debug("OnPlayerRequestDownload(%d, %d, %d)", playerid, type, crc);
  retval_ = ((OnPlayerRequestDownload_callback)callback)(playerid, type, crc);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnScriptLoadPlayer_callback)(int playerid, bool isEntryScript);
static bool _OnScriptLoadPlayer(AMX *amx, void *callback, cell *retval) {
  int playerid;
  bool isEntryScript;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_bool(amx, 1, (bool *)&isEntryScript);
  sampgdk_log_debug("OnScriptLoadPlayer(%d, %d)", playerid, isEntryScript);
  ((OnScriptLoadPlayer_callback)callback)(playerid, isEntryScript);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnScriptUnloadPlayer_callback)(int playerid, bool isEntryScript);
static bool _OnScriptUnloadPlayer(AMX *amx, void *callback, cell *retval) {
  int playerid;
  bool isEntryScript;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_bool(amx, 1, (bool *)&isEntryScript);
  sampgdk_log_debug("OnScriptUnloadPlayer(%d, %d)", playerid, isEntryScript);
  ((OnScriptUnloadPlayer_callback)callback)(playerid, isEntryScript);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerEnterGangZone_callback)(int playerid, int zoneid);
static bool _OnPlayerEnterGangZone(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int zoneid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&zoneid);
  sampgdk_log_debug("OnPlayerEnterGangZone(%d, %d)", playerid, zoneid);
  ((OnPlayerEnterGangZone_callback)callback)(playerid, zoneid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerLeaveGangZone_callback)(int playerid, int zoneid);
static bool _OnPlayerLeaveGangZone(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int zoneid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&zoneid);
  sampgdk_log_debug("OnPlayerLeaveGangZone(%d, %d)", playerid, zoneid);
  ((OnPlayerLeaveGangZone_callback)callback)(playerid, zoneid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerEnterPlayerGangZone_callback)(int playerid, int zoneid);
static bool _OnPlayerEnterPlayerGangZone(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int zoneid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&zoneid);
  sampgdk_log_debug("OnPlayerEnterPlayerGangZone(%d, %d)", playerid, zoneid);
  ((OnPlayerEnterPlayerGangZone_callback)callback)(playerid, zoneid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerLeavePlayerGangZone_callback)(int playerid, int zoneid);
static bool _OnPlayerLeavePlayerGangZone(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int zoneid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&zoneid);
  sampgdk_log_debug("OnPlayerLeavePlayerGangZone(%d, %d)", playerid, zoneid);
  ((OnPlayerLeavePlayerGangZone_callback)callback)(playerid, zoneid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerClickGangZone_callback)(int playerid, int zoneid);
static bool _OnPlayerClickGangZone(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int zoneid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&zoneid);
  sampgdk_log_debug("OnPlayerClickGangZone(%d, %d)", playerid, zoneid);
  ((OnPlayerClickGangZone_callback)callback)(playerid, zoneid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerClickPlayerGangZone_callback)(int playerid, int zoneid);
static bool _OnPlayerClickPlayerGangZone(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int zoneid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&zoneid);
  sampgdk_log_debug("OnPlayerClickPlayerGangZone(%d, %d)", playerid, zoneid);
  ((OnPlayerClickPlayerGangZone_callback)callback)(playerid, zoneid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnClientCheckResponse_callback)(int playerid, int actionid, int memaddr, int retndata);
static bool _OnClientCheckResponse(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int actionid;
  int memaddr;
  int retndata;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&actionid);
  sampgdk_param_get_cell(amx, 2, (cell *)&memaddr);
  sampgdk_param_get_cell(amx, 3, (cell *)&retndata);
  sampgdk_log_debug("OnClientCheckResponse(%d, %d, %d, %d)", playerid, actionid, memaddr, retndata);
  ((OnClientCheckResponse_callback)callback)(playerid, actionid, memaddr, retndata);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerFinishedDownloading_callback)(int playerid, int virtualworld);
static bool _OnPlayerFinishedDownloading(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int virtualworld;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&virtualworld);
  sampgdk_log_debug("OnPlayerFinishedDownloading(%d, %d)", playerid, virtualworld);
  ((OnPlayerFinishedDownloading_callback)callback)(playerid, virtualworld);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCFinishMove_callback)(int npcid);
static bool _OnNPCFinishMove(AMX *amx, void *callback, cell *retval) {
  int npcid;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_log_debug("OnNPCFinishMove(%d)", npcid);
  ((OnNPCFinishMove_callback)callback)(npcid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCCreate_callback)(int npcid);
static bool _OnNPCCreate(AMX *amx, void *callback, cell *retval) {
  int npcid;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_log_debug("OnNPCCreate(%d)", npcid);
  ((OnNPCCreate_callback)callback)(npcid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCDestroy_callback)(int npcid);
static bool _OnNPCDestroy(AMX *amx, void *callback, cell *retval) {
  int npcid;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_log_debug("OnNPCDestroy(%d)", npcid);
  ((OnNPCDestroy_callback)callback)(npcid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCSpawn_callback)(int npcid);
static bool _OnNPCSpawn(AMX *amx, void *callback, cell *retval) {
  int npcid;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_log_debug("OnNPCSpawn(%d)", npcid);
  ((OnNPCSpawn_callback)callback)(npcid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCRespawn_callback)(int npcid);
static bool _OnNPCRespawn(AMX *amx, void *callback, cell *retval) {
  int npcid;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_log_debug("OnNPCRespawn(%d)", npcid);
  ((OnNPCRespawn_callback)callback)(npcid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCWeaponStateChange_callback)(int npcid, int newState, int oldState);
static bool _OnNPCWeaponStateChange(AMX *amx, void *callback, cell *retval) {
  int npcid;
  int newState;
  int oldState;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_param_get_cell(amx, 1, (cell *)&newState);
  sampgdk_param_get_cell(amx, 2, (cell *)&oldState);
  sampgdk_log_debug("OnNPCWeaponStateChange(%d, %d, %d)", npcid, newState, oldState);
  ((OnNPCWeaponStateChange_callback)callback)(npcid, newState, oldState);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCTakeDamage_callback)(int npcid, int issuerid, float amount, int weaponid, int bodypart);
static bool _OnNPCTakeDamage(AMX *amx, void *callback, cell *retval) {
  int npcid;
  int issuerid;
  float amount;
  int weaponid;
  int bodypart;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_param_get_cell(amx, 1, (cell *)&issuerid);
  sampgdk_param_get_float(amx, 2, (float *)&amount);
  sampgdk_param_get_cell(amx, 3, (cell *)&weaponid);
  sampgdk_param_get_cell(amx, 4, (cell *)&bodypart);
  sampgdk_log_debug("OnNPCTakeDamage(%d, %d, %f, %d, %d)", npcid, issuerid, amount, weaponid, bodypart);
  ((OnNPCTakeDamage_callback)callback)(npcid, issuerid, amount, weaponid, bodypart);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCGiveDamage_callback)(int npcid, int damagedid, float amount, int weaponid, int bodypart);
static bool _OnNPCGiveDamage(AMX *amx, void *callback, cell *retval) {
  int npcid;
  int damagedid;
  float amount;
  int weaponid;
  int bodypart;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_param_get_cell(amx, 1, (cell *)&damagedid);
  sampgdk_param_get_float(amx, 2, (float *)&amount);
  sampgdk_param_get_cell(amx, 3, (cell *)&weaponid);
  sampgdk_param_get_cell(amx, 4, (cell *)&bodypart);
  sampgdk_log_debug("OnNPCGiveDamage(%d, %d, %f, %d, %d)", npcid, damagedid, amount, weaponid, bodypart);
  ((OnNPCGiveDamage_callback)callback)(npcid, damagedid, amount, weaponid, bodypart);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCDeath_callback)(int npcid, int killerid, int reason);
static bool _OnNPCDeath(AMX *amx, void *callback, cell *retval) {
  int npcid;
  int killerid;
  int reason;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_param_get_cell(amx, 1, (cell *)&killerid);
  sampgdk_param_get_cell(amx, 2, (cell *)&reason);
  sampgdk_log_debug("OnNPCDeath(%d, %d, %d)", npcid, killerid, reason);
  ((OnNPCDeath_callback)callback)(npcid, killerid, reason);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCPlaybackStart_callback)(int npcid, int recordid);
static bool _OnNPCPlaybackStart(AMX *amx, void *callback, cell *retval) {
  int npcid;
  int recordid;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_param_get_cell(amx, 1, (cell *)&recordid);
  sampgdk_log_debug("OnNPCPlaybackStart(%d, %d)", npcid, recordid);
  ((OnNPCPlaybackStart_callback)callback)(npcid, recordid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCPlaybackEnd_callback)(int npcid, int recordid);
static bool _OnNPCPlaybackEnd(AMX *amx, void *callback, cell *retval) {
  int npcid;
  int recordid;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_param_get_cell(amx, 1, (cell *)&recordid);
  sampgdk_log_debug("OnNPCPlaybackEnd(%d, %d)", npcid, recordid);
  ((OnNPCPlaybackEnd_callback)callback)(npcid, recordid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCWeaponShot_callback)(int npcid, int weaponid, int hittype, int hitid, float fX, float fY, float fZ);
static bool _OnNPCWeaponShot(AMX *amx, void *callback, cell *retval) {
  int npcid;
  int weaponid;
  int hittype;
  int hitid;
  float fX;
  float fY;
  float fZ;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_param_get_cell(amx, 1, (cell *)&weaponid);
  sampgdk_param_get_cell(amx, 2, (cell *)&hittype);
  sampgdk_param_get_cell(amx, 3, (cell *)&hitid);
  sampgdk_param_get_float(amx, 4, (float *)&fX);
  sampgdk_param_get_float(amx, 5, (float *)&fY);
  sampgdk_param_get_float(amx, 6, (float *)&fZ);
  sampgdk_log_debug("OnNPCWeaponShot(%d, %d, %d, %d, %f, %f, %f)", npcid, weaponid, hittype, hitid, fX, fY, fZ);
  ((OnNPCWeaponShot_callback)callback)(npcid, weaponid, hittype, hitid, fX, fY, fZ);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCFinishNodePoint_callback)(int npcid, int nodeid, int pointid);
static bool _OnNPCFinishNodePoint(AMX *amx, void *callback, cell *retval) {
  int npcid;
  int nodeid;
  int pointid;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_param_get_cell(amx, 1, (cell *)&nodeid);
  sampgdk_param_get_cell(amx, 2, (cell *)&pointid);
  sampgdk_log_debug("OnNPCFinishNodePoint(%d, %d, %d)", npcid, nodeid, pointid);
  ((OnNPCFinishNodePoint_callback)callback)(npcid, nodeid, pointid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCFinishNode_callback)(int npcid, int nodeid);
static bool _OnNPCFinishNode(AMX *amx, void *callback, cell *retval) {
  int npcid;
  int nodeid;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_param_get_cell(amx, 1, (cell *)&nodeid);
  sampgdk_log_debug("OnNPCFinishNode(%d, %d)", npcid, nodeid);
  ((OnNPCFinishNode_callback)callback)(npcid, nodeid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCChangeNode_callback)(int npcid, int newnodeid, int oldnodeid);
static bool _OnNPCChangeNode(AMX *amx, void *callback, cell *retval) {
  int npcid;
  int newnodeid;
  int oldnodeid;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_param_get_cell(amx, 1, (cell *)&newnodeid);
  sampgdk_param_get_cell(amx, 2, (cell *)&oldnodeid);
  sampgdk_log_debug("OnNPCChangeNode(%d, %d, %d)", npcid, newnodeid, oldnodeid);
  ((OnNPCChangeNode_callback)callback)(npcid, newnodeid, oldnodeid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCFinishMovePath_callback)(int npcid, int pathid);
static bool _OnNPCFinishMovePath(AMX *amx, void *callback, cell *retval) {
  int npcid;
  int pathid;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_param_get_cell(amx, 1, (cell *)&pathid);
  sampgdk_log_debug("OnNPCFinishMovePath(%d, %d)", npcid, pathid);
  ((OnNPCFinishMovePath_callback)callback)(npcid, pathid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnNPCFinishMovePathPoint_callback)(int npcid, int pathid, int pointid);
static bool _OnNPCFinishMovePathPoint(AMX *amx, void *callback, cell *retval) {
  int npcid;
  int pathid;
  int pointid;
  sampgdk_param_get_cell(amx, 0, (cell *)&npcid);
  sampgdk_param_get_cell(amx, 1, (cell *)&pathid);
  sampgdk_param_get_cell(amx, 2, (cell *)&pointid);
  sampgdk_log_debug("OnNPCFinishMovePathPoint(%d, %d, %d)", npcid, pathid, pointid);
  ((OnNPCFinishMovePathPoint_callback)callback)(npcid, pathid, pointid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerPickUpPlayerPickup_callback)(int playerid, int pickupid);
static bool _OnPlayerPickUpPlayerPickup(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int pickupid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&pickupid);
  sampgdk_log_debug("OnPlayerPickUpPlayerPickup(%d, %d)", playerid, pickupid);
  ((OnPlayerPickUpPlayerPickup_callback)callback)(playerid, pickupid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPickupStreamIn_callback)(int pickupid, int playerid);
static bool _OnPickupStreamIn(AMX *amx, void *callback, cell *retval) {
  int pickupid;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&pickupid);
  sampgdk_param_get_cell(amx, 1, (cell *)&playerid);
  sampgdk_log_debug("OnPickupStreamIn(%d, %d)", pickupid, playerid);
  ((OnPickupStreamIn_callback)callback)(pickupid, playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPickupStreamOut_callback)(int pickupid, int playerid);
static bool _OnPickupStreamOut(AMX *amx, void *callback, cell *retval) {
  int pickupid;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&pickupid);
  sampgdk_param_get_cell(amx, 1, (cell *)&playerid);
  sampgdk_log_debug("OnPickupStreamOut(%d, %d)", pickupid, playerid);
  ((OnPickupStreamOut_callback)callback)(pickupid, playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerPickupStreamIn_callback)(int pickupid, int playerid);
static bool _OnPlayerPickupStreamIn(AMX *amx, void *callback, cell *retval) {
  int pickupid;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&pickupid);
  sampgdk_param_get_cell(amx, 1, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerPickupStreamIn(%d, %d)", pickupid, playerid);
  ((OnPlayerPickupStreamIn_callback)callback)(pickupid, playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerPickupStreamOut_callback)(int pickupid, int playerid);
static bool _OnPlayerPickupStreamOut(AMX *amx, void *callback, cell *retval) {
  int pickupid;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&pickupid);
  sampgdk_param_get_cell(amx, 1, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerPickupStreamOut(%d, %d)", pickupid, playerid);
  ((OnPlayerPickupStreamOut_callback)callback)(pickupid, playerid);
  return true;
}

SAMPGDK_MODULE_INIT(a_samp) {
  int error;
  if ((error = sampgdk_callback_register("OnVehicleStreamOut", _OnVehicleStreamOut)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleStreamIn", _OnVehicleStreamIn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleSpawn", _OnVehicleSpawn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleSirenStateChange", _OnVehicleSirenStateChange)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleRespray", _OnVehicleRespray)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehiclePaintjob", _OnVehiclePaintjob)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleMod", _OnVehicleMod)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleDeath", _OnVehicleDeath)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleDamageStatusUpdate", _OnVehicleDamageStatusUpdate)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnUnoccupiedVehicleUpdate", _OnUnoccupiedVehicleUpdate)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnTrailerUpdate", _OnTrailerUpdate)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnScriptUnloadPlayer", _OnScriptUnloadPlayer)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnScriptLoadPlayer", _OnScriptLoadPlayer)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnRconLoginAttempt", _OnRconLoginAttempt)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnRconCommand", _OnRconCommand)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerWeaponShot", _OnPlayerWeaponShot)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerUpdate", _OnPlayerUpdate)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerText", _OnPlayerText)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerTakeDamage", _OnPlayerTakeDamage)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerStreamOut", _OnPlayerStreamOut)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerStreamIn", _OnPlayerStreamIn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerStateChange", _OnPlayerStateChange)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerSpawn", _OnPlayerSpawn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerSelectedMenuRow", _OnPlayerSelectedMenuRow)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerSelectObject", _OnPlayerSelectObject)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerRequestSpawn", _OnPlayerRequestSpawn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerRequestDownload", _OnPlayerRequestDownload)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerRequestClass", _OnPlayerRequestClass)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerPickupStreamOut", _OnPlayerPickupStreamOut)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerPickupStreamIn", _OnPlayerPickupStreamIn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerPickUpPlayerPickup", _OnPlayerPickUpPlayerPickup)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerPickUpPickup", _OnPlayerPickUpPickup)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerObjectMoved", _OnPlayerObjectMoved)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerLeaveRaceCheckpoint", _OnPlayerLeaveRaceCheckpoint)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerLeavePlayerGangZone", _OnPlayerLeavePlayerGangZone)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerLeaveGangZone", _OnPlayerLeaveGangZone)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerLeaveCheckpoint", _OnPlayerLeaveCheckpoint)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerKeyStateChange", _OnPlayerKeyStateChange)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerInteriorChange", _OnPlayerInteriorChange)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerGiveDamageActor", _OnPlayerGiveDamageActor)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerGiveDamage", _OnPlayerGiveDamage)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerFinishedDownloading", _OnPlayerFinishedDownloading)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerExitedMenu", _OnPlayerExitedMenu)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerExitVehicle", _OnPlayerExitVehicle)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerEnterVehicle", _OnPlayerEnterVehicle)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerEnterRaceCheckpoint", _OnPlayerEnterRaceCheckpoint)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerEnterPlayerGangZone", _OnPlayerEnterPlayerGangZone)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerEnterGangZone", _OnPlayerEnterGangZone)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerEnterCheckpoint", _OnPlayerEnterCheckpoint)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerEditObject", _OnPlayerEditObject)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerEditAttachedObject", _OnPlayerEditAttachedObject)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerDisconnect", _OnPlayerDisconnect)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerDeath", _OnPlayerDeath)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerConnect", _OnPlayerConnect)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerCommandText", _OnPlayerCommandText)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerClickTextDraw", _OnPlayerClickTextDraw)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerClickPlayerTextDraw", _OnPlayerClickPlayerTextDraw)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerClickPlayerGangZone", _OnPlayerClickPlayerGangZone)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerClickPlayer", _OnPlayerClickPlayer)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerClickMap", _OnPlayerClickMap)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerClickGangZone", _OnPlayerClickGangZone)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPickupStreamOut", _OnPickupStreamOut)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPickupStreamIn", _OnPickupStreamIn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnObjectMoved", _OnObjectMoved)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCWeaponStateChange", _OnNPCWeaponStateChange)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCWeaponShot", _OnNPCWeaponShot)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCTakeDamage", _OnNPCTakeDamage)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCSpawn", _OnNPCSpawn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCRespawn", _OnNPCRespawn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCPlaybackStart", _OnNPCPlaybackStart)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCPlaybackEnd", _OnNPCPlaybackEnd)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCGiveDamage", _OnNPCGiveDamage)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCFinishNodePoint", _OnNPCFinishNodePoint)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCFinishNode", _OnNPCFinishNode)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCFinishMovePathPoint", _OnNPCFinishMovePathPoint)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCFinishMovePath", _OnNPCFinishMovePath)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCFinishMove", _OnNPCFinishMove)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCDestroy", _OnNPCDestroy)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCDeath", _OnNPCDeath)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCCreate", _OnNPCCreate)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnNPCChangeNode", _OnNPCChangeNode)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnIncomingConnection", _OnIncomingConnection)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnGameModeInit", _OnGameModeInit)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnGameModeExit", _OnGameModeExit)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnEnterExitModShop", _OnEnterExitModShop)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnDialogResponse", _OnDialogResponse)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnClientCheckResponse", _OnClientCheckResponse)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnActorStreamOut", _OnActorStreamOut)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnActorStreamIn", _OnActorStreamIn)) < 0) {
    return error;
  }
  return 0;
}

SAMPGDK_MODULE_CLEANUP(a_samp) {
  sampgdk_callback_unregister("OnVehicleStreamOut");
  sampgdk_callback_unregister("OnVehicleStreamIn");
  sampgdk_callback_unregister("OnVehicleSpawn");
  sampgdk_callback_unregister("OnVehicleSirenStateChange");
  sampgdk_callback_unregister("OnVehicleRespray");
  sampgdk_callback_unregister("OnVehiclePaintjob");
  sampgdk_callback_unregister("OnVehicleMod");
  sampgdk_callback_unregister("OnVehicleDeath");
  sampgdk_callback_unregister("OnVehicleDamageStatusUpdate");
  sampgdk_callback_unregister("OnUnoccupiedVehicleUpdate");
  sampgdk_callback_unregister("OnTrailerUpdate");
  sampgdk_callback_unregister("OnScriptUnloadPlayer");
  sampgdk_callback_unregister("OnScriptLoadPlayer");
  sampgdk_callback_unregister("OnRconLoginAttempt");
  sampgdk_callback_unregister("OnRconCommand");
  sampgdk_callback_unregister("OnPlayerWeaponShot");
  sampgdk_callback_unregister("OnPlayerUpdate");
  sampgdk_callback_unregister("OnPlayerText");
  sampgdk_callback_unregister("OnPlayerTakeDamage");
  sampgdk_callback_unregister("OnPlayerStreamOut");
  sampgdk_callback_unregister("OnPlayerStreamIn");
  sampgdk_callback_unregister("OnPlayerStateChange");
  sampgdk_callback_unregister("OnPlayerSpawn");
  sampgdk_callback_unregister("OnPlayerSelectedMenuRow");
  sampgdk_callback_unregister("OnPlayerSelectObject");
  sampgdk_callback_unregister("OnPlayerRequestSpawn");
  sampgdk_callback_unregister("OnPlayerRequestDownload");
  sampgdk_callback_unregister("OnPlayerRequestClass");
  sampgdk_callback_unregister("OnPlayerPickupStreamOut");
  sampgdk_callback_unregister("OnPlayerPickupStreamIn");
  sampgdk_callback_unregister("OnPlayerPickUpPlayerPickup");
  sampgdk_callback_unregister("OnPlayerPickUpPickup");
  sampgdk_callback_unregister("OnPlayerObjectMoved");
  sampgdk_callback_unregister("OnPlayerLeaveRaceCheckpoint");
  sampgdk_callback_unregister("OnPlayerLeavePlayerGangZone");
  sampgdk_callback_unregister("OnPlayerLeaveGangZone");
  sampgdk_callback_unregister("OnPlayerLeaveCheckpoint");
  sampgdk_callback_unregister("OnPlayerKeyStateChange");
  sampgdk_callback_unregister("OnPlayerInteriorChange");
  sampgdk_callback_unregister("OnPlayerGiveDamageActor");
  sampgdk_callback_unregister("OnPlayerGiveDamage");
  sampgdk_callback_unregister("OnPlayerFinishedDownloading");
  sampgdk_callback_unregister("OnPlayerExitedMenu");
  sampgdk_callback_unregister("OnPlayerExitVehicle");
  sampgdk_callback_unregister("OnPlayerEnterVehicle");
  sampgdk_callback_unregister("OnPlayerEnterRaceCheckpoint");
  sampgdk_callback_unregister("OnPlayerEnterPlayerGangZone");
  sampgdk_callback_unregister("OnPlayerEnterGangZone");
  sampgdk_callback_unregister("OnPlayerEnterCheckpoint");
  sampgdk_callback_unregister("OnPlayerEditObject");
  sampgdk_callback_unregister("OnPlayerEditAttachedObject");
  sampgdk_callback_unregister("OnPlayerDisconnect");
  sampgdk_callback_unregister("OnPlayerDeath");
  sampgdk_callback_unregister("OnPlayerConnect");
  sampgdk_callback_unregister("OnPlayerCommandText");
  sampgdk_callback_unregister("OnPlayerClickTextDraw");
  sampgdk_callback_unregister("OnPlayerClickPlayerTextDraw");
  sampgdk_callback_unregister("OnPlayerClickPlayerGangZone");
  sampgdk_callback_unregister("OnPlayerClickPlayer");
  sampgdk_callback_unregister("OnPlayerClickMap");
  sampgdk_callback_unregister("OnPlayerClickGangZone");
  sampgdk_callback_unregister("OnPickupStreamOut");
  sampgdk_callback_unregister("OnPickupStreamIn");
  sampgdk_callback_unregister("OnObjectMoved");
  sampgdk_callback_unregister("OnNPCWeaponStateChange");
  sampgdk_callback_unregister("OnNPCWeaponShot");
  sampgdk_callback_unregister("OnNPCTakeDamage");
  sampgdk_callback_unregister("OnNPCSpawn");
  sampgdk_callback_unregister("OnNPCRespawn");
  sampgdk_callback_unregister("OnNPCPlaybackStart");
  sampgdk_callback_unregister("OnNPCPlaybackEnd");
  sampgdk_callback_unregister("OnNPCGiveDamage");
  sampgdk_callback_unregister("OnNPCFinishNodePoint");
  sampgdk_callback_unregister("OnNPCFinishNode");
  sampgdk_callback_unregister("OnNPCFinishMovePathPoint");
  sampgdk_callback_unregister("OnNPCFinishMovePath");
  sampgdk_callback_unregister("OnNPCFinishMove");
  sampgdk_callback_unregister("OnNPCDestroy");
  sampgdk_callback_unregister("OnNPCDeath");
  sampgdk_callback_unregister("OnNPCCreate");
  sampgdk_callback_unregister("OnNPCChangeNode");
  sampgdk_callback_unregister("OnIncomingConnection");
  sampgdk_callback_unregister("OnGameModeInit");
  sampgdk_callback_unregister("OnGameModeExit");
  sampgdk_callback_unregister("OnEnterExitModShop");
  sampgdk_callback_unregister("OnDialogResponse");
  sampgdk_callback_unregister("OnClientCheckResponse");
  sampgdk_callback_unregister("OnActorStreamOut");
  sampgdk_callback_unregister("OnActorStreamIn");
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(int, CreateActor(int modelid, float x, float y, float z, float rotation)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("CreateActor(%d, %f, %f, %f, %f)", modelid, x, y, z, rotation);
  native = sampgdk_native_find_flexible("CreateActor", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)modelid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  params[5] = amx_ftoc(rotation);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, DestroyActor(int actorid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DestroyActor(%d)", actorid);
  native = sampgdk_native_find_flexible("DestroyActor", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)actorid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsActorStreamedIn(int actorid, int forplayerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsActorStreamedIn(%d, %d)", actorid, forplayerid);
  native = sampgdk_native_find_flexible("IsActorStreamedIn", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = (cell)forplayerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetActorVirtualWorld(int actorid, int vworld)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetActorVirtualWorld(%d, %d)", actorid, vworld);
  native = sampgdk_native_find_flexible("SetActorVirtualWorld", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = (cell)vworld;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetActorVirtualWorld(int actorid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetActorVirtualWorld(%d)", actorid);
  native = sampgdk_native_find_flexible("GetActorVirtualWorld", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)actorid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, ApplyActorAnimation(int actorid, const char * animlib, const char * animname, float fDelta, bool loop, bool lockx, bool locky, bool freeze, int time)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  cell animlib_;
  cell animname_;
  sampgdk_log_debug("ApplyActorAnimation(%d, \"%s\", \"%s\", %f, %d, %d, %d, %d, %d)", actorid, animlib, animname, fDelta, loop, lockx, locky, freeze, time);
  native = sampgdk_native_find_flexible("ApplyActorAnimation", native);
  sampgdk_fakeamx_push_string(animlib, NULL, &animlib_);
  sampgdk_fakeamx_push_string(animname, NULL, &animname_);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = animlib_;
  params[3] = animname_;
  params[4] = amx_ftoc(fDelta);
  params[5] = (cell)loop;
  params[6] = (cell)lockx;
  params[7] = (cell)locky;
  params[8] = (cell)freeze;
  params[9] = (cell)time;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(animname_);
  sampgdk_fakeamx_pop(animlib_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ClearActorAnimations(int actorid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("ClearActorAnimations(%d)", actorid);
  native = sampgdk_native_find_flexible("ClearActorAnimations", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)actorid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetActorPos(int actorid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetActorPos(%d, %f, %f, %f)", actorid, x, y, z);
  native = sampgdk_native_find_flexible("SetActorPos", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetActorPos(int actorid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetActorPos(%d, @%p, @%p, @%p)", actorid, x, y, z);
  native = sampgdk_native_find_flexible("GetActorPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetActorFacingAngle(int actorid, float angle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetActorFacingAngle(%d, %f)", actorid, angle);
  native = sampgdk_native_find_flexible("SetActorFacingAngle", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = amx_ftoc(angle);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetActorFacingAngle(int actorid, float * angle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell angle_;
  sampgdk_log_debug("GetActorFacingAngle(%d, @%p)", actorid, angle);
  native = sampgdk_native_find_flexible("GetActorFacingAngle", native);
  sampgdk_fakeamx_push(1, &angle_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = angle_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(angle_, angle);
  sampgdk_fakeamx_pop(angle_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetActorHealth(int actorid, float health)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetActorHealth(%d, %f)", actorid, health);
  native = sampgdk_native_find_flexible("SetActorHealth", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = amx_ftoc(health);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetActorHealth(int actorid, float * health)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell health_;
  sampgdk_log_debug("GetActorHealth(%d, @%p)", actorid, health);
  native = sampgdk_native_find_flexible("GetActorHealth", native);
  sampgdk_fakeamx_push(1, &health_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = health_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(health_, health);
  sampgdk_fakeamx_pop(health_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetActorInvulnerable(int actorid, bool invulnerable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetActorInvulnerable(%d, %d)", actorid, invulnerable);
  native = sampgdk_native_find_flexible("SetActorInvulnerable", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = (cell)invulnerable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsActorInvulnerable(int actorid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsActorInvulnerable(%d)", actorid);
  native = sampgdk_native_find_flexible("IsActorInvulnerable", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)actorid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidActor(int actorid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsValidActor(%d)", actorid);
  native = sampgdk_native_find_flexible("IsValidActor", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)actorid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(a_actor) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(a_actor) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, HTTP(int index, int type, const char * url, const char * data)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell url_;
  cell data_;
  cell callback_;
  sampgdk_log_debug("HTTP(%d, %d, \"%s\", \"%s\", \"%s\")", index, type, url, data);
  native = sampgdk_native_find_flexible("HTTP", native);
  sampgdk_fakeamx_push_string(url, NULL, &url_);
  sampgdk_fakeamx_push_string(data, NULL, &data_);
  sampgdk_fakeamx_push_string("OnHTTPResponse", NULL, &callback_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)index;
  params[2] = (cell)type;
  params[3] = url_;
  params[4] = data_;
  params[5] = callback_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(callback_);
  sampgdk_fakeamx_pop(data_);
  sampgdk_fakeamx_pop(url_);
  return !!(retval);
}

typedef void (SAMPGDK_CALLBACK_CALL *OnHTTPResponse_callback)(int index, int response_code, const char * data);
static bool _OnHTTPResponse(AMX *amx, void *callback, cell *retval) {
  int index;
  int response_code;
  const char * data;
  sampgdk_param_get_cell(amx, 0, (cell *)&index);
  sampgdk_param_get_cell(amx, 1, (cell *)&response_code);
  sampgdk_param_get_string(amx, 2, (char * *)&data);
  sampgdk_log_debug("OnHTTPResponse(%d, %d, \"%s\")", index, response_code, data);
  ((OnHTTPResponse_callback)callback)(index, response_code, data);
  free((void *)data);
  return true;
}

SAMPGDK_MODULE_INIT(a_http) {
  int error;
  if ((error = sampgdk_callback_register("OnHTTPResponse", _OnHTTPResponse)) < 0) {
    return error;
  }
  return 0;
}

SAMPGDK_MODULE_CLEANUP(a_http) {
  sampgdk_callback_unregister("OnHTTPResponse");
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(int, CreateObject(int modelid, float x, float y, float z, float rX, float rY, float rZ, float DrawDistance)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  sampgdk_log_debug("CreateObject(%d, %f, %f, %f, %f, %f, %f, %f)", modelid, x, y, z, rX, rY, rZ, DrawDistance);
  native = sampgdk_native_find_flexible("CreateObject", native);
  params[0] = 8 * sizeof(cell);
  params[1] = (cell)modelid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  params[5] = amx_ftoc(rX);
  params[6] = amx_ftoc(rY);
  params[7] = amx_ftoc(rZ);
  params[8] = amx_ftoc(DrawDistance);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, AttachObjectToVehicle(int objectid, int vehicleid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  sampgdk_log_debug("AttachObjectToVehicle(%d, %d, %f, %f, %f, %f, %f, %f)", objectid, vehicleid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ);
  native = sampgdk_native_find_flexible("AttachObjectToVehicle", native);
  params[0] = 8 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = (cell)vehicleid;
  params[3] = amx_ftoc(fOffsetX);
  params[4] = amx_ftoc(fOffsetY);
  params[5] = amx_ftoc(fOffsetZ);
  params[6] = amx_ftoc(fRotX);
  params[7] = amx_ftoc(fRotY);
  params[8] = amx_ftoc(fRotZ);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, AttachObjectToObject(int objectid, int attachtoid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ, bool SyncRotation)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  sampgdk_log_debug("AttachObjectToObject(%d, %d, %f, %f, %f, %f, %f, %f, %d)", objectid, attachtoid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, SyncRotation);
  native = sampgdk_native_find_flexible("AttachObjectToObject", native);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = (cell)attachtoid;
  params[3] = amx_ftoc(fOffsetX);
  params[4] = amx_ftoc(fOffsetY);
  params[5] = amx_ftoc(fOffsetZ);
  params[6] = amx_ftoc(fRotX);
  params[7] = amx_ftoc(fRotY);
  params[8] = amx_ftoc(fRotZ);
  params[9] = (cell)SyncRotation;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, AttachObjectToPlayer(int objectid, int playerid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  sampgdk_log_debug("AttachObjectToPlayer(%d, %d, %f, %f, %f, %f, %f, %f)", objectid, playerid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ);
  native = sampgdk_native_find_flexible("AttachObjectToPlayer", native);
  params[0] = 8 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = (cell)playerid;
  params[3] = amx_ftoc(fOffsetX);
  params[4] = amx_ftoc(fOffsetY);
  params[5] = amx_ftoc(fOffsetZ);
  params[6] = amx_ftoc(fRotX);
  params[7] = amx_ftoc(fRotY);
  params[8] = amx_ftoc(fRotZ);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetObjectPos(int objectid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetObjectPos(%d, %f, %f, %f)", objectid, x, y, z);
  native = sampgdk_native_find_flexible("SetObjectPos", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetObjectPos(int objectid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetObjectPos(%d, @%p, @%p, @%p)", objectid, x, y, z);
  native = sampgdk_native_find_flexible("GetObjectPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetObjectRot(int objectid, float rotX, float rotY, float rotZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetObjectRot(%d, %f, %f, %f)", objectid, rotX, rotY, rotZ);
  native = sampgdk_native_find_flexible("SetObjectRot", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = amx_ftoc(rotX);
  params[3] = amx_ftoc(rotY);
  params[4] = amx_ftoc(rotZ);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetObjectRot(int objectid, float * rotX, float * rotY, float * rotZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell rotX_;
  cell rotY_;
  cell rotZ_;
  sampgdk_log_debug("GetObjectRot(%d, @%p, @%p, @%p)", objectid, rotX, rotY, rotZ);
  native = sampgdk_native_find_flexible("GetObjectRot", native);
  sampgdk_fakeamx_push(1, &rotX_);
  sampgdk_fakeamx_push(1, &rotY_);
  sampgdk_fakeamx_push(1, &rotZ_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = rotX_;
  params[3] = rotY_;
  params[4] = rotZ_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(rotX_, rotX);
  sampgdk_fakeamx_get_float(rotY_, rotY);
  sampgdk_fakeamx_get_float(rotZ_, rotZ);
  sampgdk_fakeamx_pop(rotZ_);
  sampgdk_fakeamx_pop(rotY_);
  sampgdk_fakeamx_pop(rotX_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetObjectModel(int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetObjectModel(%d)", objectid);
  native = sampgdk_native_find_flexible("GetObjectModel", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetObjectNoCameraCol(int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SetObjectNoCameraCol(%d)", objectid);
  native = sampgdk_native_find_flexible("SetObjectNoCameraCol", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidObject(int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsValidObject(%d)", objectid);
  native = sampgdk_native_find_flexible("IsValidObject", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, DestroyObject(int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DestroyObject(%d)", objectid);
  native = sampgdk_native_find_flexible("DestroyObject", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, MoveObject(int objectid, float X, float Y, float Z, float Speed, float RotX, float RotY, float RotZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  sampgdk_log_debug("MoveObject(%d, %f, %f, %f, %f, %f, %f, %f)", objectid, X, Y, Z, Speed, RotX, RotY, RotZ);
  native = sampgdk_native_find_flexible("MoveObject", native);
  params[0] = 8 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = amx_ftoc(X);
  params[3] = amx_ftoc(Y);
  params[4] = amx_ftoc(Z);
  params[5] = amx_ftoc(Speed);
  params[6] = amx_ftoc(RotX);
  params[7] = amx_ftoc(RotY);
  params[8] = amx_ftoc(RotZ);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, StopObject(int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("StopObject(%d)", objectid);
  native = sampgdk_native_find_flexible("StopObject", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsObjectMoving(int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsObjectMoving(%d)", objectid);
  native = sampgdk_native_find_flexible("IsObjectMoving", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, EditObject(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("EditObject(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("EditObject", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, EditPlayerObject(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("EditPlayerObject(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("EditPlayerObject", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SelectObject(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SelectObject(%d)", playerid);
  native = sampgdk_native_find_flexible("SelectObject", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, CancelEdit(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("CancelEdit(%d)", playerid);
  native = sampgdk_native_find_flexible("CancelEdit", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, CreatePlayerObject(int playerid, int modelid, float x, float y, float z, float rX, float rY, float rZ, float DrawDistance)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  sampgdk_log_debug("CreatePlayerObject(%d, %d, %f, %f, %f, %f, %f, %f, %f)", playerid, modelid, x, y, z, rX, rY, rZ, DrawDistance);
  native = sampgdk_native_find_flexible("CreatePlayerObject", native);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)modelid;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  params[5] = amx_ftoc(z);
  params[6] = amx_ftoc(rX);
  params[7] = amx_ftoc(rY);
  params[8] = amx_ftoc(rZ);
  params[9] = amx_ftoc(DrawDistance);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, AttachPlayerObjectToPlayer(int objectplayer, int objectid, int attachplayer, float OffsetX, float OffsetY, float OffsetZ, float rX, float rY, float rZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  sampgdk_log_debug("AttachPlayerObjectToPlayer(%d, %d, %d, %f, %f, %f, %f, %f, %f)", objectplayer, objectid, attachplayer, OffsetX, OffsetY, OffsetZ, rX, rY, rZ);
  native = sampgdk_native_find_flexible("AttachPlayerObjectToPlayer", native);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)objectplayer;
  params[2] = (cell)objectid;
  params[3] = (cell)attachplayer;
  params[4] = amx_ftoc(OffsetX);
  params[5] = amx_ftoc(OffsetY);
  params[6] = amx_ftoc(OffsetZ);
  params[7] = amx_ftoc(rX);
  params[8] = amx_ftoc(rY);
  params[9] = amx_ftoc(rZ);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, AttachPlayerObjectToVehicle(int playerid, int objectid, int vehicleid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float RotZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  sampgdk_log_debug("AttachPlayerObjectToVehicle(%d, %d, %d, %f, %f, %f, %f, %f, %f)", playerid, objectid, vehicleid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, RotZ);
  native = sampgdk_native_find_flexible("AttachPlayerObjectToVehicle", native);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = (cell)vehicleid;
  params[4] = amx_ftoc(fOffsetX);
  params[5] = amx_ftoc(fOffsetY);
  params[6] = amx_ftoc(fOffsetZ);
  params[7] = amx_ftoc(fRotX);
  params[8] = amx_ftoc(fRotY);
  params[9] = amx_ftoc(RotZ);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerObjectPos(int playerid, int objectid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("SetPlayerObjectPos(%d, %d, %f, %f, %f)", playerid, objectid, x, y, z);
  native = sampgdk_native_find_flexible("SetPlayerObjectPos", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  params[5] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerObjectPos(int playerid, int objectid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetPlayerObjectPos(%d, %d, @%p, @%p, @%p)", playerid, objectid, x, y, z);
  native = sampgdk_native_find_flexible("GetPlayerObjectPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = x_;
  params[4] = y_;
  params[5] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerObjectRot(int playerid, int objectid, float rotX, float rotY, float rotZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("SetPlayerObjectRot(%d, %d, %f, %f, %f)", playerid, objectid, rotX, rotY, rotZ);
  native = sampgdk_native_find_flexible("SetPlayerObjectRot", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = amx_ftoc(rotX);
  params[4] = amx_ftoc(rotY);
  params[5] = amx_ftoc(rotZ);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerObjectRot(int playerid, int objectid, float * rotX, float * rotY, float * rotZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell rotX_;
  cell rotY_;
  cell rotZ_;
  sampgdk_log_debug("GetPlayerObjectRot(%d, %d, @%p, @%p, @%p)", playerid, objectid, rotX, rotY, rotZ);
  native = sampgdk_native_find_flexible("GetPlayerObjectRot", native);
  sampgdk_fakeamx_push(1, &rotX_);
  sampgdk_fakeamx_push(1, &rotY_);
  sampgdk_fakeamx_push(1, &rotZ_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = rotX_;
  params[4] = rotY_;
  params[5] = rotZ_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(rotX_, rotX);
  sampgdk_fakeamx_get_float(rotY_, rotY);
  sampgdk_fakeamx_get_float(rotZ_, rotZ);
  sampgdk_fakeamx_pop(rotZ_);
  sampgdk_fakeamx_pop(rotY_);
  sampgdk_fakeamx_pop(rotX_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerObjectModel(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayerObjectModel(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("GetPlayerObjectModel", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerObjectNoCameraCol(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerObjectNoCameraCol(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("SetPlayerObjectNoCameraCol", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidPlayerObject(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsValidPlayerObject(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("IsValidPlayerObject", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, DestroyPlayerObject(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("DestroyPlayerObject(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("DestroyPlayerObject", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, MovePlayerObject(int playerid, int objectid, float x, float y, float z, float Speed, float RotX, float RotY, float RotZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  sampgdk_log_debug("MovePlayerObject(%d, %d, %f, %f, %f, %f, %f, %f, %f)", playerid, objectid, x, y, z, Speed, RotX, RotY, RotZ);
  native = sampgdk_native_find_flexible("MovePlayerObject", native);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  params[5] = amx_ftoc(z);
  params[6] = amx_ftoc(Speed);
  params[7] = amx_ftoc(RotX);
  params[8] = amx_ftoc(RotY);
  params[9] = amx_ftoc(RotZ);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, StopPlayerObject(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("StopPlayerObject(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("StopPlayerObject", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerObjectMoving(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPlayerObjectMoving(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("IsPlayerObjectMoving", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetObjectMaterial(int objectid, int materialindex, int modelid, const char * txdname, const char * texturename, int materialcolor)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  cell txdname_;
  cell texturename_;
  sampgdk_log_debug("SetObjectMaterial(%d, %d, %d, \"%s\", \"%s\", %d)", objectid, materialindex, modelid, txdname, texturename, materialcolor);
  native = sampgdk_native_find_flexible("SetObjectMaterial", native);
  sampgdk_fakeamx_push_string(txdname, NULL, &txdname_);
  sampgdk_fakeamx_push_string(texturename, NULL, &texturename_);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = (cell)materialindex;
  params[3] = (cell)modelid;
  params[4] = txdname_;
  params[5] = texturename_;
  params[6] = (cell)materialcolor;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(texturename_);
  sampgdk_fakeamx_pop(txdname_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerObjectMaterial(int playerid, int objectid, int materialindex, int modelid, const char * txdname, const char * texturename, int materialcolor)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[8];
  cell txdname_;
  cell texturename_;
  sampgdk_log_debug("SetPlayerObjectMaterial(%d, %d, %d, %d, \"%s\", \"%s\", %d)", playerid, objectid, materialindex, modelid, txdname, texturename, materialcolor);
  native = sampgdk_native_find_flexible("SetPlayerObjectMaterial", native);
  sampgdk_fakeamx_push_string(txdname, NULL, &txdname_);
  sampgdk_fakeamx_push_string(texturename, NULL, &texturename_);
  params[0] = 7 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = (cell)materialindex;
  params[4] = (cell)modelid;
  params[5] = txdname_;
  params[6] = texturename_;
  params[7] = (cell)materialcolor;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(texturename_);
  sampgdk_fakeamx_pop(txdname_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetObjectMaterialText(int objectid, const char * text, int materialindex, int materialsize, const char * fontface, int fontsize, bool bold, int fontcolor, int backcolor, int textalignment)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[11];
  cell text_;
  cell fontface_;
  sampgdk_log_debug("SetObjectMaterialText(%d, \"%s\", %d, %d, \"%s\", %d, %d, %d, %d, %d)", objectid, text, materialindex, materialsize, fontface, fontsize, bold, fontcolor, backcolor, textalignment);
  native = sampgdk_native_find_flexible("SetObjectMaterialText", native);
  sampgdk_fakeamx_push_string(text, NULL, &text_);
  sampgdk_fakeamx_push_string(fontface, NULL, &fontface_);
  params[0] = 10 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = text_;
  params[3] = (cell)materialindex;
  params[4] = (cell)materialsize;
  params[5] = fontface_;
  params[6] = (cell)fontsize;
  params[7] = (cell)bold;
  params[8] = (cell)fontcolor;
  params[9] = (cell)backcolor;
  params[10] = (cell)textalignment;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(fontface_);
  sampgdk_fakeamx_pop(text_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerObjectMaterialText(int playerid, int objectid, const char * text, int materialindex, int materialsize, const char * fontface, int fontsize, bool bold, int fontcolor, int backcolor, int textalignment)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[12];
  cell text_;
  cell fontface_;
  sampgdk_log_debug("SetPlayerObjectMaterialText(%d, %d, \"%s\", %d, %d, \"%s\", %d, %d, %d, %d, %d)", playerid, objectid, text, materialindex, materialsize, fontface, fontsize, bold, fontcolor, backcolor, textalignment);
  native = sampgdk_native_find_flexible("SetPlayerObjectMaterialText", native);
  sampgdk_fakeamx_push_string(text, NULL, &text_);
  sampgdk_fakeamx_push_string(fontface, NULL, &fontface_);
  params[0] = 11 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = text_;
  params[4] = (cell)materialindex;
  params[5] = (cell)materialsize;
  params[6] = fontface_;
  params[7] = (cell)fontsize;
  params[8] = (cell)bold;
  params[9] = (cell)fontcolor;
  params[10] = (cell)backcolor;
  params[11] = (cell)textalignment;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(fontface_);
  sampgdk_fakeamx_pop(text_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetObjectsDefaultCameraCol(bool disable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SetObjectsDefaultCameraCol(%d)", disable);
  native = sampgdk_native_find_flexible("SetObjectsDefaultCameraCol", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)disable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(a_objects) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(a_objects) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, SetSpawnInfo(int playerid, int team, int skin, float x, float y, float z, float rotation, int weapon1, int weapon1_ammo, int weapon2, int weapon2_ammo, int weapon3, int weapon3_ammo)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[14];
  sampgdk_log_debug("SetSpawnInfo(%d, %d, %d, %f, %f, %f, %f, %d, %d, %d, %d, %d, %d)", playerid, team, skin, x, y, z, rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
  native = sampgdk_native_find_flexible("SetSpawnInfo", native);
  params[0] = 13 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)team;
  params[3] = (cell)skin;
  params[4] = amx_ftoc(x);
  params[5] = amx_ftoc(y);
  params[6] = amx_ftoc(z);
  params[7] = amx_ftoc(rotation);
  params[8] = (cell)weapon1;
  params[9] = (cell)weapon1_ammo;
  params[10] = (cell)weapon2;
  params[11] = (cell)weapon2_ammo;
  params[12] = (cell)weapon3;
  params[13] = (cell)weapon3_ammo;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SpawnPlayer(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SpawnPlayer(%d)", playerid);
  native = sampgdk_native_find_flexible("SpawnPlayer", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerPos(int playerid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetPlayerPos(%d, %f, %f, %f)", playerid, x, y, z);
  native = sampgdk_native_find_flexible("SetPlayerPos", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerPosFindZ(int playerid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetPlayerPosFindZ(%d, %f, %f, %f)", playerid, x, y, z);
  native = sampgdk_native_find_flexible("SetPlayerPosFindZ", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerPos(int playerid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetPlayerPos(%d, @%p, @%p, @%p)", playerid, x, y, z);
  native = sampgdk_native_find_flexible("GetPlayerPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerFacingAngle(int playerid, float angle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerFacingAngle(%d, %f)", playerid, angle);
  native = sampgdk_native_find_flexible("SetPlayerFacingAngle", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(angle);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerFacingAngle(int playerid, float * angle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell angle_;
  sampgdk_log_debug("GetPlayerFacingAngle(%d, @%p)", playerid, angle);
  native = sampgdk_native_find_flexible("GetPlayerFacingAngle", native);
  sampgdk_fakeamx_push(1, &angle_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = angle_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(angle_, angle);
  sampgdk_fakeamx_pop(angle_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerInRangeOfPoint(int playerid, float range, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("IsPlayerInRangeOfPoint(%d, %f, %f, %f, %f)", playerid, range, x, y, z);
  native = sampgdk_native_find_flexible("IsPlayerInRangeOfPoint", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(range);
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  params[5] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(float, GetPlayerDistanceFromPoint(int playerid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("GetPlayerDistanceFromPoint(%d, %f, %f, %f)", playerid, x, y, z);
  native = sampgdk_native_find_flexible("GetPlayerDistanceFromPoint", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerStreamedIn(int playerid, int forplayerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPlayerStreamedIn(%d, %d)", playerid, forplayerid);
  native = sampgdk_native_find_flexible("IsPlayerStreamedIn", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)forplayerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerInterior(int playerid, int interiorid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerInterior(%d, %d)", playerid, interiorid);
  native = sampgdk_native_find_flexible("SetPlayerInterior", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)interiorid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerInterior(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerInterior(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerInterior", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerHealth(int playerid, float health)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerHealth(%d, %f)", playerid, health);
  native = sampgdk_native_find_flexible("SetPlayerHealth", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(health);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerHealth(int playerid, float * health)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell health_;
  sampgdk_log_debug("GetPlayerHealth(%d, @%p)", playerid, health);
  native = sampgdk_native_find_flexible("GetPlayerHealth", native);
  sampgdk_fakeamx_push(1, &health_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = health_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(health_, health);
  sampgdk_fakeamx_pop(health_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerArmour(int playerid, float armour)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerArmour(%d, %f)", playerid, armour);
  native = sampgdk_native_find_flexible("SetPlayerArmour", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(armour);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerArmour(int playerid, float * armour)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell armour_;
  sampgdk_log_debug("GetPlayerArmour(%d, @%p)", playerid, armour);
  native = sampgdk_native_find_flexible("GetPlayerArmour", native);
  sampgdk_fakeamx_push(1, &armour_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = armour_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(armour_, armour);
  sampgdk_fakeamx_pop(armour_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerAmmo(int playerid, int weaponid, int ammo)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("SetPlayerAmmo(%d, %d, %d)", playerid, weaponid, ammo);
  native = sampgdk_native_find_flexible("SetPlayerAmmo", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)weaponid;
  params[3] = (cell)ammo;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerAmmo(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerAmmo(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerAmmo", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerWeaponState(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerWeaponState(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerWeaponState", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerTargetPlayer(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerTargetPlayer(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerTargetPlayer", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerTargetActor(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerTargetActor(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerTargetActor", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerTeam(int playerid, int teamid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerTeam(%d, %d)", playerid, teamid);
  native = sampgdk_native_find_flexible("SetPlayerTeam", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)teamid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerTeam(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerTeam(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerTeam", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerScore(int playerid, int score)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerScore(%d, %d)", playerid, score);
  native = sampgdk_native_find_flexible("SetPlayerScore", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)score;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerScore(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerScore(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerScore", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerDrunkLevel(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerDrunkLevel(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerDrunkLevel", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerDrunkLevel(int playerid, int level)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerDrunkLevel(%d, %d)", playerid, level);
  native = sampgdk_native_find_flexible("SetPlayerDrunkLevel", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)level;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerColor(int playerid, int color)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerColor(%d, %d)", playerid, color);
  native = sampgdk_native_find_flexible("SetPlayerColor", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)color;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerColor(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerColor(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerColor", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerSkin(int playerid, int skinid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerSkin(%d, %d)", playerid, skinid);
  native = sampgdk_native_find_flexible("SetPlayerSkin", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)skinid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerSkin(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerSkin(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerSkin", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GivePlayerWeapon(int playerid, int weaponid, int ammo)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("GivePlayerWeapon(%d, %d, %d)", playerid, weaponid, ammo);
  native = sampgdk_native_find_flexible("GivePlayerWeapon", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)weaponid;
  params[3] = (cell)ammo;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ResetPlayerWeapons(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("ResetPlayerWeapons(%d)", playerid);
  native = sampgdk_native_find_flexible("ResetPlayerWeapons", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerArmedWeapon(int playerid, int weaponid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerArmedWeapon(%d, %d)", playerid, weaponid);
  native = sampgdk_native_find_flexible("SetPlayerArmedWeapon", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)weaponid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerWeaponData(int playerid, int slot, int * weapon, int * ammo)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell weapon_;
  cell ammo_;
  sampgdk_log_debug("GetPlayerWeaponData(%d, %d, @%p, @%p)", playerid, slot, weapon, ammo);
  native = sampgdk_native_find_flexible("GetPlayerWeaponData", native);
  sampgdk_fakeamx_push(1, &weapon_);
  sampgdk_fakeamx_push(1, &ammo_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)slot;
  params[3] = weapon_;
  params[4] = ammo_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(weapon_, weapon);
  sampgdk_fakeamx_get_cell(ammo_, ammo);
  sampgdk_fakeamx_pop(ammo_);
  sampgdk_fakeamx_pop(weapon_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GivePlayerMoney(int playerid, int money)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GivePlayerMoney(%d, %d)", playerid, money);
  native = sampgdk_native_find_flexible("GivePlayerMoney", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)money;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ResetPlayerMoney(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("ResetPlayerMoney(%d)", playerid);
  native = sampgdk_native_find_flexible("ResetPlayerMoney", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, SetPlayerName(int playerid, const char * name)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell name_;
  sampgdk_log_debug("SetPlayerName(%d, \"%s\")", playerid, name);
  native = sampgdk_native_find_flexible("SetPlayerName", native);
  sampgdk_fakeamx_push_string(name, NULL, &name_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = name_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(name_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerMoney(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerMoney(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerMoney", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerState(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerState(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerState", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerIp(int playerid, char * ip, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell ip_;
  sampgdk_log_debug("GetPlayerIp(%d, @%p, %d)", playerid, ip, size);
  native = sampgdk_native_find_flexible("GetPlayerIp", native);
  sampgdk_fakeamx_push(size, &ip_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = ip_;
  params[3] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(ip_, ip, size);
  sampgdk_fakeamx_pop(ip_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerPing(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerPing(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerPing", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerWeapon(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerWeapon(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerWeapon", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerKeys(int playerid, int * keys, int * updown, int * leftright)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell keys_;
  cell updown_;
  cell leftright_;
  sampgdk_log_debug("GetPlayerKeys(%d, @%p, @%p, @%p)", playerid, keys, updown, leftright);
  native = sampgdk_native_find_flexible("GetPlayerKeys", native);
  sampgdk_fakeamx_push(1, &keys_);
  sampgdk_fakeamx_push(1, &updown_);
  sampgdk_fakeamx_push(1, &leftright_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = keys_;
  params[3] = updown_;
  params[4] = leftright_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(keys_, keys);
  sampgdk_fakeamx_get_cell(updown_, updown);
  sampgdk_fakeamx_get_cell(leftright_, leftright);
  sampgdk_fakeamx_pop(leftright_);
  sampgdk_fakeamx_pop(updown_);
  sampgdk_fakeamx_pop(keys_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerName(int playerid, char * name, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell name_;
  sampgdk_log_debug("GetPlayerName(%d, @%p, %d)", playerid, name, size);
  native = sampgdk_native_find_flexible("GetPlayerName", native);
  sampgdk_fakeamx_push(size, &name_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = name_;
  params[3] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(name_, name, size);
  sampgdk_fakeamx_pop(name_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerTime(int playerid, int hour, int minute)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("SetPlayerTime(%d, %d, %d)", playerid, hour, minute);
  native = sampgdk_native_find_flexible("SetPlayerTime", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)hour;
  params[3] = (cell)minute;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerTime(int playerid, int * hour, int * minute)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell hour_;
  cell minute_;
  sampgdk_log_debug("GetPlayerTime(%d, @%p, @%p)", playerid, hour, minute);
  native = sampgdk_native_find_flexible("GetPlayerTime", native);
  sampgdk_fakeamx_push(1, &hour_);
  sampgdk_fakeamx_push(1, &minute_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = hour_;
  params[3] = minute_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(hour_, hour);
  sampgdk_fakeamx_get_cell(minute_, minute);
  sampgdk_fakeamx_pop(minute_);
  sampgdk_fakeamx_pop(hour_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TogglePlayerClock(int playerid, bool toggle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TogglePlayerClock(%d, %d)", playerid, toggle);
  native = sampgdk_native_find_flexible("TogglePlayerClock", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)toggle;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerWeather(int playerid, int weather)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerWeather(%d, %d)", playerid, weather);
  native = sampgdk_native_find_flexible("SetPlayerWeather", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)weather;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ForceClassSelection(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("ForceClassSelection(%d)", playerid);
  native = sampgdk_native_find_flexible("ForceClassSelection", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerWantedLevel(int playerid, int level)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerWantedLevel(%d, %d)", playerid, level);
  native = sampgdk_native_find_flexible("SetPlayerWantedLevel", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)level;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerWantedLevel(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerWantedLevel(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerWantedLevel", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerFightingStyle(int playerid, int style)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerFightingStyle(%d, %d)", playerid, style);
  native = sampgdk_native_find_flexible("SetPlayerFightingStyle", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)style;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerFightingStyle(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerFightingStyle(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerFightingStyle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerVelocity(int playerid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetPlayerVelocity(%d, %f, %f, %f)", playerid, x, y, z);
  native = sampgdk_native_find_flexible("SetPlayerVelocity", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerVelocity(int playerid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetPlayerVelocity(%d, @%p, @%p, @%p)", playerid, x, y, z);
  native = sampgdk_native_find_flexible("GetPlayerVelocity", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayCrimeReportForPlayer(int playerid, int suspectid, int crime)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayCrimeReportForPlayer(%d, %d, %d)", playerid, suspectid, crime);
  native = sampgdk_native_find_flexible("PlayCrimeReportForPlayer", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)suspectid;
  params[3] = (cell)crime;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayAudioStreamForPlayer(int playerid, const char * url, float posX, float posY, float posZ, float distance, bool usepos)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[8];
  cell url_;
  sampgdk_log_debug("PlayAudioStreamForPlayer(%d, \"%s\", %f, %f, %f, %f, %d)", playerid, url, posX, posY, posZ, distance, usepos);
  native = sampgdk_native_find_flexible("PlayAudioStreamForPlayer", native);
  sampgdk_fakeamx_push_string(url, NULL, &url_);
  params[0] = 7 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = url_;
  params[3] = amx_ftoc(posX);
  params[4] = amx_ftoc(posY);
  params[5] = amx_ftoc(posZ);
  params[6] = amx_ftoc(distance);
  params[7] = (cell)usepos;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(url_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, StopAudioStreamForPlayer(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("StopAudioStreamForPlayer(%d)", playerid);
  native = sampgdk_native_find_flexible("StopAudioStreamForPlayer", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerShopName(int playerid, const char * shopname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell shopname_;
  sampgdk_log_debug("SetPlayerShopName(%d, \"%s\")", playerid, shopname);
  native = sampgdk_native_find_flexible("SetPlayerShopName", native);
  sampgdk_fakeamx_push_string(shopname, NULL, &shopname_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = shopname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(shopname_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerSkillLevel(int playerid, int skill, int level)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("SetPlayerSkillLevel(%d, %d, %d)", playerid, skill, level);
  native = sampgdk_native_find_flexible("SetPlayerSkillLevel", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)skill;
  params[3] = (cell)level;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerSurfingVehicleID(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerSurfingVehicleID(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerSurfingVehicleID", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerSurfingObjectID(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerSurfingObjectID(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerSurfingObjectID", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, RemoveBuildingForPlayer(int playerid, int modelid, float fX, float fY, float fZ, float fRadius)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  sampgdk_log_debug("RemoveBuildingForPlayer(%d, %d, %f, %f, %f, %f)", playerid, modelid, fX, fY, fZ, fRadius);
  native = sampgdk_native_find_flexible("RemoveBuildingForPlayer", native);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)modelid;
  params[3] = amx_ftoc(fX);
  params[4] = amx_ftoc(fY);
  params[5] = amx_ftoc(fZ);
  params[6] = amx_ftoc(fRadius);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerLastShotVectors(int playerid, float * fOriginX, float * fOriginY, float * fOriginZ, float * fHitPosX, float * fHitPosY, float * fHitPosZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[8];
  cell fOriginX_;
  cell fOriginY_;
  cell fOriginZ_;
  cell fHitPosX_;
  cell fHitPosY_;
  cell fHitPosZ_;
  sampgdk_log_debug("GetPlayerLastShotVectors(%d, @%p, @%p, @%p, @%p, @%p, @%p)", playerid, fOriginX, fOriginY, fOriginZ, fHitPosX, fHitPosY, fHitPosZ);
  native = sampgdk_native_find_flexible("GetPlayerLastShotVectors", native);
  sampgdk_fakeamx_push(1, &fOriginX_);
  sampgdk_fakeamx_push(1, &fOriginY_);
  sampgdk_fakeamx_push(1, &fOriginZ_);
  sampgdk_fakeamx_push(1, &fHitPosX_);
  sampgdk_fakeamx_push(1, &fHitPosY_);
  sampgdk_fakeamx_push(1, &fHitPosZ_);
  params[0] = 7 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = fOriginX_;
  params[3] = fOriginY_;
  params[4] = fOriginZ_;
  params[5] = fHitPosX_;
  params[6] = fHitPosY_;
  params[7] = fHitPosZ_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(fOriginX_, fOriginX);
  sampgdk_fakeamx_get_float(fOriginY_, fOriginY);
  sampgdk_fakeamx_get_float(fOriginZ_, fOriginZ);
  sampgdk_fakeamx_get_float(fHitPosX_, fHitPosX);
  sampgdk_fakeamx_get_float(fHitPosY_, fHitPosY);
  sampgdk_fakeamx_get_float(fHitPosZ_, fHitPosZ);
  sampgdk_fakeamx_pop(fHitPosZ_);
  sampgdk_fakeamx_pop(fHitPosY_);
  sampgdk_fakeamx_pop(fHitPosX_);
  sampgdk_fakeamx_pop(fOriginZ_);
  sampgdk_fakeamx_pop(fOriginY_);
  sampgdk_fakeamx_pop(fOriginX_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerAttachedObject(int playerid, int index, int modelid, int bone, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ, float fScaleX, float fScaleY, float fScaleZ, int materialcolor1, int materialcolor2)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[16];
  sampgdk_log_debug("SetPlayerAttachedObject(%d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %d, %d)", playerid, index, modelid, bone, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ, materialcolor1, materialcolor2);
  native = sampgdk_native_find_flexible("SetPlayerAttachedObject", native);
  params[0] = 15 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)index;
  params[3] = (cell)modelid;
  params[4] = (cell)bone;
  params[5] = amx_ftoc(fOffsetX);
  params[6] = amx_ftoc(fOffsetY);
  params[7] = amx_ftoc(fOffsetZ);
  params[8] = amx_ftoc(fRotX);
  params[9] = amx_ftoc(fRotY);
  params[10] = amx_ftoc(fRotZ);
  params[11] = amx_ftoc(fScaleX);
  params[12] = amx_ftoc(fScaleY);
  params[13] = amx_ftoc(fScaleZ);
  params[14] = (cell)materialcolor1;
  params[15] = (cell)materialcolor2;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, RemovePlayerAttachedObject(int playerid, int index)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("RemovePlayerAttachedObject(%d, %d)", playerid, index);
  native = sampgdk_native_find_flexible("RemovePlayerAttachedObject", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)index;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerAttachedObjectSlotUsed(int playerid, int index)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPlayerAttachedObjectSlotUsed(%d, %d)", playerid, index);
  native = sampgdk_native_find_flexible("IsPlayerAttachedObjectSlotUsed", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)index;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, EditAttachedObject(int playerid, int index)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("EditAttachedObject(%d, %d)", playerid, index);
  native = sampgdk_native_find_flexible("EditAttachedObject", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)index;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, CreatePlayerTextDraw(int playerid, float x, float y, const char * text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell text_;
  sampgdk_log_debug("CreatePlayerTextDraw(%d, %f, %f, \"%s\")", playerid, x, y, text);
  native = sampgdk_native_find_flexible("CreatePlayerTextDraw", native);
  sampgdk_fakeamx_push_string(text, NULL, &text_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = text_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(text_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawDestroy(int playerid, int text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawDestroy(%d, %d)", playerid, text);
  native = sampgdk_native_find_flexible("PlayerTextDrawDestroy", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawLetterSize(int playerid, int text, float x, float y)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("PlayerTextDrawLetterSize(%d, %d, %f, %f)", playerid, text, x, y);
  native = sampgdk_native_find_flexible("PlayerTextDrawLetterSize", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawTextSize(int playerid, int text, float x, float y)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("PlayerTextDrawTextSize(%d, %d, %f, %f)", playerid, text, x, y);
  native = sampgdk_native_find_flexible("PlayerTextDrawTextSize", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawAlignment(int playerid, int text, int alignment)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerTextDrawAlignment(%d, %d, %d)", playerid, text, alignment);
  native = sampgdk_native_find_flexible("PlayerTextDrawAlignment", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = (cell)alignment;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawColor(int playerid, int text, int color)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerTextDrawColor(%d, %d, %d)", playerid, text, color);
  native = sampgdk_native_find_flexible("PlayerTextDrawColor", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = (cell)color;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawUseBox(int playerid, int text, bool use)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerTextDrawUseBox(%d, %d, %d)", playerid, text, use);
  native = sampgdk_native_find_flexible("PlayerTextDrawUseBox", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = (cell)use;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawBoxColor(int playerid, int text, int color)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerTextDrawBoxColor(%d, %d, %d)", playerid, text, color);
  native = sampgdk_native_find_flexible("PlayerTextDrawBoxColor", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = (cell)color;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawSetShadow(int playerid, int text, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerTextDrawSetShadow(%d, %d, %d)", playerid, text, size);
  native = sampgdk_native_find_flexible("PlayerTextDrawSetShadow", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawSetOutline(int playerid, int text, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerTextDrawSetOutline(%d, %d, %d)", playerid, text, size);
  native = sampgdk_native_find_flexible("PlayerTextDrawSetOutline", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawBackgroundColor(int playerid, int text, int color)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerTextDrawBackgroundColor(%d, %d, %d)", playerid, text, color);
  native = sampgdk_native_find_flexible("PlayerTextDrawBackgroundColor", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = (cell)color;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawFont(int playerid, int text, int font)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerTextDrawFont(%d, %d, %d)", playerid, text, font);
  native = sampgdk_native_find_flexible("PlayerTextDrawFont", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = (cell)font;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawSetProportional(int playerid, int text, bool set)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerTextDrawSetProportional(%d, %d, %d)", playerid, text, set);
  native = sampgdk_native_find_flexible("PlayerTextDrawSetProportional", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = (cell)set;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawSetSelectable(int playerid, int text, bool set)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerTextDrawSetSelectable(%d, %d, %d)", playerid, text, set);
  native = sampgdk_native_find_flexible("PlayerTextDrawSetSelectable", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = (cell)set;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawShow(int playerid, int text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawShow(%d, %d)", playerid, text);
  native = sampgdk_native_find_flexible("PlayerTextDrawShow", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawHide(int playerid, int text)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawHide(%d, %d)", playerid, text);
  native = sampgdk_native_find_flexible("PlayerTextDrawHide", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawSetString(int playerid, int text, const char * string)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell string_;
  sampgdk_log_debug("PlayerTextDrawSetString(%d, %d, \"%s\")", playerid, text, string);
  native = sampgdk_native_find_flexible("PlayerTextDrawSetString", native);
  sampgdk_fakeamx_push_string(string, NULL, &string_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = string_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(string_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawSetPreviewModel(int playerid, int text, int modelindex)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerTextDrawSetPreviewModel(%d, %d, %d)", playerid, text, modelindex);
  native = sampgdk_native_find_flexible("PlayerTextDrawSetPreviewModel", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = (cell)modelindex;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawSetPreviewRot(int playerid, int text, float fRotX, float fRotY, float fRotZ, float fZoom)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  sampgdk_log_debug("PlayerTextDrawSetPreviewRot(%d, %d, %f, %f, %f, %f)", playerid, text, fRotX, fRotY, fRotZ, fZoom);
  native = sampgdk_native_find_flexible("PlayerTextDrawSetPreviewRot", native);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = amx_ftoc(fRotX);
  params[4] = amx_ftoc(fRotY);
  params[5] = amx_ftoc(fRotZ);
  params[6] = amx_ftoc(fZoom);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawSetPreviewVehCol(int playerid, int text, int color1, int color2)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("PlayerTextDrawSetPreviewVehCol(%d, %d, %d, %d)", playerid, text, color1, color2);
  native = sampgdk_native_find_flexible("PlayerTextDrawSetPreviewVehCol", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)text;
  params[3] = (cell)color1;
  params[4] = (cell)color2;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPVarInt(int playerid, const char * varname, int value)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell varname_;
  sampgdk_log_debug("SetPVarInt(%d, \"%s\", %d)", playerid, varname, value);
  native = sampgdk_native_find_flexible("SetPVarInt", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = varname_;
  params[3] = (cell)value;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPVarInt(int playerid, const char * varname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell varname_;
  sampgdk_log_debug("GetPVarInt(%d, \"%s\")", playerid, varname);
  native = sampgdk_native_find_flexible("GetPVarInt", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = varname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetPVarString(int playerid, const char * varname, const char * value)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell varname_;
  cell value_;
  sampgdk_log_debug("SetPVarString(%d, \"%s\", \"%s\")", playerid, varname, value);
  native = sampgdk_native_find_flexible("SetPVarString", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  sampgdk_fakeamx_push_string(value, NULL, &value_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = varname_;
  params[3] = value_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(value_);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPVarString(int playerid, const char * varname, char * value, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell varname_;
  cell value_;
  sampgdk_log_debug("GetPVarString(%d, \"%s\", @%p, %d)", playerid, varname, value, size);
  native = sampgdk_native_find_flexible("GetPVarString", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  sampgdk_fakeamx_push(size, &value_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = varname_;
  params[3] = value_;
  params[4] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(value_, value, size);
  sampgdk_fakeamx_pop(value_);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPVarFloat(int playerid, const char * varname, float value)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell varname_;
  sampgdk_log_debug("SetPVarFloat(%d, \"%s\", %f)", playerid, varname, value);
  native = sampgdk_native_find_flexible("SetPVarFloat", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = varname_;
  params[3] = amx_ftoc(value);
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(float, GetPVarFloat(int playerid, const char * varname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell varname_;
  sampgdk_log_debug("GetPVarFloat(%d, \"%s\")", playerid, varname);
  native = sampgdk_native_find_flexible("GetPVarFloat", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = varname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(bool, DeletePVar(int playerid, const char * varname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell varname_;
  sampgdk_log_debug("DeletePVar(%d, \"%s\")", playerid, varname);
  native = sampgdk_native_find_flexible("DeletePVar", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = varname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPVarsUpperIndex(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPVarsUpperIndex(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPVarsUpperIndex", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPVarNameAtIndex(int playerid, int index, char * varname, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell varname_;
  sampgdk_log_debug("GetPVarNameAtIndex(%d, %d, @%p, %d)", playerid, index, varname, size);
  native = sampgdk_native_find_flexible("GetPVarNameAtIndex", native);
  sampgdk_fakeamx_push(size, &varname_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)index;
  params[3] = varname_;
  params[4] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(varname_, varname, size);
  sampgdk_fakeamx_pop(varname_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPVarType(int playerid, const char * varname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell varname_;
  sampgdk_log_debug("GetPVarType(%d, \"%s\")", playerid, varname);
  native = sampgdk_native_find_flexible("GetPVarType", native);
  sampgdk_fakeamx_push_string(varname, NULL, &varname_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = varname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(varname_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerChatBubble(int playerid, const char * text, int color, float drawdistance, int expiretime)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell text_;
  sampgdk_log_debug("SetPlayerChatBubble(%d, \"%s\", %d, %f, %d)", playerid, text, color, drawdistance, expiretime);
  native = sampgdk_native_find_flexible("SetPlayerChatBubble", native);
  sampgdk_fakeamx_push_string(text, NULL, &text_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = text_;
  params[3] = (cell)color;
  params[4] = amx_ftoc(drawdistance);
  params[5] = (cell)expiretime;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(text_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PutPlayerInVehicle(int playerid, int vehicleid, int seatid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PutPlayerInVehicle(%d, %d, %d)", playerid, vehicleid, seatid);
  native = sampgdk_native_find_flexible("PutPlayerInVehicle", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)vehicleid;
  params[3] = (cell)seatid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerVehicleID(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerVehicleID(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerVehicleID", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerVehicleSeat(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerVehicleSeat(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerVehicleSeat", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, RemovePlayerFromVehicle(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("RemovePlayerFromVehicle(%d)", playerid);
  native = sampgdk_native_find_flexible("RemovePlayerFromVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TogglePlayerControllable(int playerid, bool toggle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TogglePlayerControllable(%d, %d)", playerid, toggle);
  native = sampgdk_native_find_flexible("TogglePlayerControllable", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)toggle;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerPlaySound(int playerid, int soundid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("PlayerPlaySound(%d, %d, %f, %f, %f)", playerid, soundid, x, y, z);
  native = sampgdk_native_find_flexible("PlayerPlaySound", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)soundid;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  params[5] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ApplyAnimation(int playerid, const char * animlib, const char * animname, float fDelta, bool loop, bool lockx, bool locky, bool freeze, int time, bool forcesync)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[11];
  cell animlib_;
  cell animname_;
  sampgdk_log_debug("ApplyAnimation(%d, \"%s\", \"%s\", %f, %d, %d, %d, %d, %d, %d)", playerid, animlib, animname, fDelta, loop, lockx, locky, freeze, time, forcesync);
  native = sampgdk_native_find_flexible("ApplyAnimation", native);
  sampgdk_fakeamx_push_string(animlib, NULL, &animlib_);
  sampgdk_fakeamx_push_string(animname, NULL, &animname_);
  params[0] = 10 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = animlib_;
  params[3] = animname_;
  params[4] = amx_ftoc(fDelta);
  params[5] = (cell)loop;
  params[6] = (cell)lockx;
  params[7] = (cell)locky;
  params[8] = (cell)freeze;
  params[9] = (cell)time;
  params[10] = (cell)forcesync;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(animname_);
  sampgdk_fakeamx_pop(animlib_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ClearAnimations(int playerid, bool forcesync)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("ClearAnimations(%d, %d)", playerid, forcesync);
  native = sampgdk_native_find_flexible("ClearAnimations", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)forcesync;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerAnimationIndex(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerAnimationIndex(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerAnimationIndex", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetAnimationName(int index, char * animlib, int animlib_size, char * animname, int animname_size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell animlib_;
  cell animname_;
  sampgdk_log_debug("GetAnimationName(%d, @%p, %d, @%p, %d)", index, animlib, animlib_size, animname, animname_size);
  native = sampgdk_native_find_flexible("GetAnimationName", native);
  sampgdk_fakeamx_push(animlib_size, &animlib_);
  sampgdk_fakeamx_push(animname_size, &animname_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)index;
  params[2] = animlib_;
  params[3] = (cell)animlib_size;
  params[4] = animname_;
  params[5] = (cell)animname_size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(animlib_, animlib, animlib_size);
  sampgdk_fakeamx_get_string(animname_, animname, animname_size);
  sampgdk_fakeamx_pop(animname_);
  sampgdk_fakeamx_pop(animlib_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerSpecialAction(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerSpecialAction(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerSpecialAction", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerSpecialAction(int playerid, int actionid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerSpecialAction(%d, %d)", playerid, actionid);
  native = sampgdk_native_find_flexible("SetPlayerSpecialAction", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)actionid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, DisableRemoteVehicleCollisions(int playerid, bool disable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("DisableRemoteVehicleCollisions(%d, %d)", playerid, disable);
  native = sampgdk_native_find_flexible("DisableRemoteVehicleCollisions", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)disable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerCheckpoint(int playerid, float x, float y, float z, float size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("SetPlayerCheckpoint(%d, %f, %f, %f, %f)", playerid, x, y, z, size);
  native = sampgdk_native_find_flexible("SetPlayerCheckpoint", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  params[5] = amx_ftoc(size);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, DisablePlayerCheckpoint(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DisablePlayerCheckpoint(%d)", playerid);
  native = sampgdk_native_find_flexible("DisablePlayerCheckpoint", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerRaceCheckpoint(int playerid, int type, float x, float y, float z, float nextx, float nexty, float nextz, float size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  sampgdk_log_debug("SetPlayerRaceCheckpoint(%d, %d, %f, %f, %f, %f, %f, %f, %f)", playerid, type, x, y, z, nextx, nexty, nextz, size);
  native = sampgdk_native_find_flexible("SetPlayerRaceCheckpoint", native);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)type;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  params[5] = amx_ftoc(z);
  params[6] = amx_ftoc(nextx);
  params[7] = amx_ftoc(nexty);
  params[8] = amx_ftoc(nextz);
  params[9] = amx_ftoc(size);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, DisablePlayerRaceCheckpoint(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DisablePlayerRaceCheckpoint(%d)", playerid);
  native = sampgdk_native_find_flexible("DisablePlayerRaceCheckpoint", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerWorldBounds(int playerid, float x_max, float x_min, float y_max, float y_min)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("SetPlayerWorldBounds(%d, %f, %f, %f, %f)", playerid, x_max, x_min, y_max, y_min);
  native = sampgdk_native_find_flexible("SetPlayerWorldBounds", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(x_max);
  params[3] = amx_ftoc(x_min);
  params[4] = amx_ftoc(y_max);
  params[5] = amx_ftoc(y_min);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerMarkerForPlayer(int playerid, int showplayerid, int color)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("SetPlayerMarkerForPlayer(%d, %d, %d)", playerid, showplayerid, color);
  native = sampgdk_native_find_flexible("SetPlayerMarkerForPlayer", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)showplayerid;
  params[3] = (cell)color;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ShowPlayerNameTagForPlayer(int playerid, int showplayerid, bool show)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("ShowPlayerNameTagForPlayer(%d, %d, %d)", playerid, showplayerid, show);
  native = sampgdk_native_find_flexible("ShowPlayerNameTagForPlayer", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)showplayerid;
  params[3] = (cell)show;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerMapIcon(int playerid, int iconid, float x, float y, float z, int markertype, int color, int style)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  sampgdk_log_debug("SetPlayerMapIcon(%d, %d, %f, %f, %f, %d, %d, %d)", playerid, iconid, x, y, z, markertype, color, style);
  native = sampgdk_native_find_flexible("SetPlayerMapIcon", native);
  params[0] = 8 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)iconid;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  params[5] = amx_ftoc(z);
  params[6] = (cell)markertype;
  params[7] = (cell)color;
  params[8] = (cell)style;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, RemovePlayerMapIcon(int playerid, int iconid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("RemovePlayerMapIcon(%d, %d)", playerid, iconid);
  native = sampgdk_native_find_flexible("RemovePlayerMapIcon", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)iconid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, AllowPlayerTeleport(int playerid, bool allow)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("AllowPlayerTeleport(%d, %d)", playerid, allow);
  native = sampgdk_native_find_flexible("AllowPlayerTeleport", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)allow;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerCameraPos(int playerid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetPlayerCameraPos(%d, %f, %f, %f)", playerid, x, y, z);
  native = sampgdk_native_find_flexible("SetPlayerCameraPos", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerCameraLookAt(int playerid, float x, float y, float z, int cut)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("SetPlayerCameraLookAt(%d, %f, %f, %f, %d)", playerid, x, y, z, cut);
  native = sampgdk_native_find_flexible("SetPlayerCameraLookAt", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  params[5] = (cell)cut;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetCameraBehindPlayer(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SetCameraBehindPlayer(%d)", playerid);
  native = sampgdk_native_find_flexible("SetCameraBehindPlayer", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerCameraPos(int playerid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetPlayerCameraPos(%d, @%p, @%p, @%p)", playerid, x, y, z);
  native = sampgdk_native_find_flexible("GetPlayerCameraPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerCameraFrontVector(int playerid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetPlayerCameraFrontVector(%d, @%p, @%p, @%p)", playerid, x, y, z);
  native = sampgdk_native_find_flexible("GetPlayerCameraFrontVector", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerCameraMode(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerCameraMode(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerCameraMode", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, EnablePlayerCameraTarget(int playerid, bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("EnablePlayerCameraTarget(%d, %d)", playerid, enable);
  native = sampgdk_native_find_flexible("EnablePlayerCameraTarget", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerCameraTargetObject(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerCameraTargetObject(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerCameraTargetObject", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerCameraTargetVehicle(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerCameraTargetVehicle(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerCameraTargetVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerCameraTargetPlayer(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerCameraTargetPlayer(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerCameraTargetPlayer", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerCameraTargetActor(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerCameraTargetActor(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerCameraTargetActor", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, GetPlayerCameraAspectRatio(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerCameraAspectRatio(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerCameraAspectRatio", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(float, GetPlayerCameraZoom(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerCameraZoom(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerCameraZoom", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(bool, AttachCameraToObject(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("AttachCameraToObject(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("AttachCameraToObject", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, AttachCameraToPlayerObject(int playerid, int playerobjectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("AttachCameraToPlayerObject(%d, %d)", playerid, playerobjectid);
  native = sampgdk_native_find_flexible("AttachCameraToPlayerObject", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)playerobjectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, InterpolateCameraPos(int playerid, float FromX, float FromY, float FromZ, float ToX, float ToY, float ToZ, int time, int cut)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  sampgdk_log_debug("InterpolateCameraPos(%d, %f, %f, %f, %f, %f, %f, %d, %d)", playerid, FromX, FromY, FromZ, ToX, ToY, ToZ, time, cut);
  native = sampgdk_native_find_flexible("InterpolateCameraPos", native);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(FromX);
  params[3] = amx_ftoc(FromY);
  params[4] = amx_ftoc(FromZ);
  params[5] = amx_ftoc(ToX);
  params[6] = amx_ftoc(ToY);
  params[7] = amx_ftoc(ToZ);
  params[8] = (cell)time;
  params[9] = (cell)cut;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, InterpolateCameraLookAt(int playerid, float FromX, float FromY, float FromZ, float ToX, float ToY, float ToZ, int time, int cut)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  sampgdk_log_debug("InterpolateCameraLookAt(%d, %f, %f, %f, %f, %f, %f, %d, %d)", playerid, FromX, FromY, FromZ, ToX, ToY, ToZ, time, cut);
  native = sampgdk_native_find_flexible("InterpolateCameraLookAt", native);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(FromX);
  params[3] = amx_ftoc(FromY);
  params[4] = amx_ftoc(FromZ);
  params[5] = amx_ftoc(ToX);
  params[6] = amx_ftoc(ToY);
  params[7] = amx_ftoc(ToZ);
  params[8] = (cell)time;
  params[9] = (cell)cut;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerConnected(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerConnected(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerConnected", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerInVehicle(int playerid, int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPlayerInVehicle(%d, %d)", playerid, vehicleid);
  native = sampgdk_native_find_flexible("IsPlayerInVehicle", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerInAnyVehicle(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerInAnyVehicle(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerInAnyVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerInCheckpoint(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerInCheckpoint(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerInCheckpoint", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerInRaceCheckpoint(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerInRaceCheckpoint(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerInRaceCheckpoint", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerVirtualWorld(int playerid, int worldid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerVirtualWorld(%d, %d)", playerid, worldid);
  native = sampgdk_native_find_flexible("SetPlayerVirtualWorld", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)worldid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerVirtualWorld(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerVirtualWorld(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerVirtualWorld", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, EnableStuntBonusForPlayer(int playerid, bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("EnableStuntBonusForPlayer(%d, %d)", playerid, enable);
  native = sampgdk_native_find_flexible("EnableStuntBonusForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, EnableStuntBonusForAll(bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("EnableStuntBonusForAll(%d)", enable);
  native = sampgdk_native_find_flexible("EnableStuntBonusForAll", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TogglePlayerSpectating(int playerid, bool toggle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TogglePlayerSpectating(%d, %d)", playerid, toggle);
  native = sampgdk_native_find_flexible("TogglePlayerSpectating", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)toggle;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerSpectatePlayer(int playerid, int targetplayerid, int mode)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerSpectatePlayer(%d, %d, %d)", playerid, targetplayerid, mode);
  native = sampgdk_native_find_flexible("PlayerSpectatePlayer", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)targetplayerid;
  params[3] = (cell)mode;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerSpectateVehicle(int playerid, int targetvehicleid, int mode)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerSpectateVehicle(%d, %d, %d)", playerid, targetvehicleid, mode);
  native = sampgdk_native_find_flexible("PlayerSpectateVehicle", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)targetvehicleid;
  params[3] = (cell)mode;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, StartRecordingPlayerData(int playerid, int recordtype, const char * recordname)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell recordname_;
  sampgdk_log_debug("StartRecordingPlayerData(%d, %d, \"%s\")", playerid, recordtype, recordname);
  native = sampgdk_native_find_flexible("StartRecordingPlayerData", native);
  sampgdk_fakeamx_push_string(recordname, NULL, &recordname_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)recordtype;
  params[3] = recordname_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(recordname_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, StopRecordingPlayerData(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("StopRecordingPlayerData(%d)", playerid);
  native = sampgdk_native_find_flexible("StopRecordingPlayerData", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, CreateExplosionForPlayer(int playerid, float X, float Y, float Z, int type, float Radius)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  sampgdk_log_debug("CreateExplosionForPlayer(%d, %f, %f, %f, %d, %f)", playerid, X, Y, Z, type, Radius);
  native = sampgdk_native_find_flexible("CreateExplosionForPlayer", native);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(X);
  params[3] = amx_ftoc(Y);
  params[4] = amx_ftoc(Z);
  params[5] = (cell)type;
  params[6] = amx_ftoc(Radius);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(a_players) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(a_players) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, IsValidVehicle(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsValidVehicle(%d)", vehicleid);
  native = sampgdk_native_find_flexible("IsValidVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(float, GetVehicleDistanceFromPoint(int vehicleid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("GetVehicleDistanceFromPoint(%d, %f, %f, %f)", vehicleid, x, y, z);
  native = sampgdk_native_find_flexible("GetVehicleDistanceFromPoint", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, CreateVehicle(int vehicletype, float x, float y, float z, float rotation, int color1, int color2, int respawn_delay, bool addsiren)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  sampgdk_log_debug("CreateVehicle(%d, %f, %f, %f, %f, %d, %d, %d, %d)", vehicletype, x, y, z, rotation, color1, color2, respawn_delay, addsiren);
  native = sampgdk_native_find_flexible("CreateVehicle", native);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)vehicletype;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  params[5] = amx_ftoc(rotation);
  params[6] = (cell)color1;
  params[7] = (cell)color2;
  params[8] = (cell)respawn_delay;
  params[9] = (cell)addsiren;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, DestroyVehicle(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DestroyVehicle(%d)", vehicleid);
  native = sampgdk_native_find_flexible("DestroyVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsVehicleStreamedIn(int vehicleid, int forplayerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsVehicleStreamedIn(%d, %d)", vehicleid, forplayerid);
  native = sampgdk_native_find_flexible("IsVehicleStreamedIn", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)forplayerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetVehiclePos(int vehicleid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetVehiclePos(%d, @%p, @%p, @%p)", vehicleid, x, y, z);
  native = sampgdk_native_find_flexible("GetVehiclePos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehiclePos(int vehicleid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetVehiclePos(%d, %f, %f, %f)", vehicleid, x, y, z);
  native = sampgdk_native_find_flexible("SetVehiclePos", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleZAngle(int vehicleid, float * z_angle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell z_angle_;
  sampgdk_log_debug("GetVehicleZAngle(%d, @%p)", vehicleid, z_angle);
  native = sampgdk_native_find_flexible("GetVehicleZAngle", native);
  sampgdk_fakeamx_push(1, &z_angle_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = z_angle_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(z_angle_, z_angle);
  sampgdk_fakeamx_pop(z_angle_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleRotationQuat(int vehicleid, float * w, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell w_;
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetVehicleRotationQuat(%d, @%p, @%p, @%p, @%p)", vehicleid, w, x, y, z);
  native = sampgdk_native_find_flexible("GetVehicleRotationQuat", native);
  sampgdk_fakeamx_push(1, &w_);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = w_;
  params[3] = x_;
  params[4] = y_;
  params[5] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(w_, w);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  sampgdk_fakeamx_pop(w_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleZAngle(int vehicleid, float z_angle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetVehicleZAngle(%d, %f)", vehicleid, z_angle);
  native = sampgdk_native_find_flexible("SetVehicleZAngle", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = amx_ftoc(z_angle);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleParamsForPlayer(int vehicleid, int playerid, int objective, int doorslocked)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetVehicleParamsForPlayer(%d, %d, %d, %d)", vehicleid, playerid, objective, doorslocked);
  native = sampgdk_native_find_flexible("SetVehicleParamsForPlayer", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)playerid;
  params[3] = (cell)objective;
  params[4] = (cell)doorslocked;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ManualVehicleEngineAndLights()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("ManualVehicleEngineAndLights()");
  native = sampgdk_native_find_flexible("ManualVehicleEngineAndLights", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleParamsEx(int vehicleid, int engine, int lights, int alarm, int doors, int bonnet, int boot, int objective)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  sampgdk_log_debug("SetVehicleParamsEx(%d, %d, %d, %d, %d, %d, %d, %d)", vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
  native = sampgdk_native_find_flexible("SetVehicleParamsEx", native);
  params[0] = 8 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)engine;
  params[3] = (cell)lights;
  params[4] = (cell)alarm;
  params[5] = (cell)doors;
  params[6] = (cell)bonnet;
  params[7] = (cell)boot;
  params[8] = (cell)objective;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleParamsEx(int vehicleid, int * engine, int * lights, int * alarm, int * doors, int * bonnet, int * boot, int * objective)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  cell engine_;
  cell lights_;
  cell alarm_;
  cell doors_;
  cell bonnet_;
  cell boot_;
  cell objective_;
  sampgdk_log_debug("GetVehicleParamsEx(%d, @%p, @%p, @%p, @%p, @%p, @%p, @%p)", vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
  native = sampgdk_native_find_flexible("GetVehicleParamsEx", native);
  sampgdk_fakeamx_push(1, &engine_);
  sampgdk_fakeamx_push(1, &lights_);
  sampgdk_fakeamx_push(1, &alarm_);
  sampgdk_fakeamx_push(1, &doors_);
  sampgdk_fakeamx_push(1, &bonnet_);
  sampgdk_fakeamx_push(1, &boot_);
  sampgdk_fakeamx_push(1, &objective_);
  params[0] = 8 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = engine_;
  params[3] = lights_;
  params[4] = alarm_;
  params[5] = doors_;
  params[6] = bonnet_;
  params[7] = boot_;
  params[8] = objective_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(engine_, engine);
  sampgdk_fakeamx_get_cell(lights_, lights);
  sampgdk_fakeamx_get_cell(alarm_, alarm);
  sampgdk_fakeamx_get_cell(doors_, doors);
  sampgdk_fakeamx_get_cell(bonnet_, bonnet);
  sampgdk_fakeamx_get_cell(boot_, boot);
  sampgdk_fakeamx_get_cell(objective_, objective);
  sampgdk_fakeamx_pop(objective_);
  sampgdk_fakeamx_pop(boot_);
  sampgdk_fakeamx_pop(bonnet_);
  sampgdk_fakeamx_pop(doors_);
  sampgdk_fakeamx_pop(alarm_);
  sampgdk_fakeamx_pop(lights_);
  sampgdk_fakeamx_pop(engine_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetVehicleParamsSirenState(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleParamsSirenState(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleParamsSirenState", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleParamsCarDoors(int vehicleid, int driver, int passenger, int backleft, int backright)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("SetVehicleParamsCarDoors(%d, %d, %d, %d, %d)", vehicleid, driver, passenger, backleft, backright);
  native = sampgdk_native_find_flexible("SetVehicleParamsCarDoors", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)driver;
  params[3] = (cell)passenger;
  params[4] = (cell)backleft;
  params[5] = (cell)backright;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleParamsCarDoors(int vehicleid, int * driver, int * passenger, int * backleft, int * backright)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell driver_;
  cell passenger_;
  cell backleft_;
  cell backright_;
  sampgdk_log_debug("GetVehicleParamsCarDoors(%d, @%p, @%p, @%p, @%p)", vehicleid, driver, passenger, backleft, backright);
  native = sampgdk_native_find_flexible("GetVehicleParamsCarDoors", native);
  sampgdk_fakeamx_push(1, &driver_);
  sampgdk_fakeamx_push(1, &passenger_);
  sampgdk_fakeamx_push(1, &backleft_);
  sampgdk_fakeamx_push(1, &backright_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = driver_;
  params[3] = passenger_;
  params[4] = backleft_;
  params[5] = backright_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(driver_, driver);
  sampgdk_fakeamx_get_cell(passenger_, passenger);
  sampgdk_fakeamx_get_cell(backleft_, backleft);
  sampgdk_fakeamx_get_cell(backright_, backright);
  sampgdk_fakeamx_pop(backright_);
  sampgdk_fakeamx_pop(backleft_);
  sampgdk_fakeamx_pop(passenger_);
  sampgdk_fakeamx_pop(driver_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleParamsCarWindows(int vehicleid, int driver, int passenger, int backleft, int backright)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("SetVehicleParamsCarWindows(%d, %d, %d, %d, %d)", vehicleid, driver, passenger, backleft, backright);
  native = sampgdk_native_find_flexible("SetVehicleParamsCarWindows", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)driver;
  params[3] = (cell)passenger;
  params[4] = (cell)backleft;
  params[5] = (cell)backright;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleParamsCarWindows(int vehicleid, int * driver, int * passenger, int * backleft, int * backright)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell driver_;
  cell passenger_;
  cell backleft_;
  cell backright_;
  sampgdk_log_debug("GetVehicleParamsCarWindows(%d, @%p, @%p, @%p, @%p)", vehicleid, driver, passenger, backleft, backright);
  native = sampgdk_native_find_flexible("GetVehicleParamsCarWindows", native);
  sampgdk_fakeamx_push(1, &driver_);
  sampgdk_fakeamx_push(1, &passenger_);
  sampgdk_fakeamx_push(1, &backleft_);
  sampgdk_fakeamx_push(1, &backright_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = driver_;
  params[3] = passenger_;
  params[4] = backleft_;
  params[5] = backright_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(driver_, driver);
  sampgdk_fakeamx_get_cell(passenger_, passenger);
  sampgdk_fakeamx_get_cell(backleft_, backleft);
  sampgdk_fakeamx_get_cell(backright_, backright);
  sampgdk_fakeamx_pop(backright_);
  sampgdk_fakeamx_pop(backleft_);
  sampgdk_fakeamx_pop(passenger_);
  sampgdk_fakeamx_pop(driver_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleToRespawn(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SetVehicleToRespawn(%d)", vehicleid);
  native = sampgdk_native_find_flexible("SetVehicleToRespawn", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, LinkVehicleToInterior(int vehicleid, int interiorid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("LinkVehicleToInterior(%d, %d)", vehicleid, interiorid);
  native = sampgdk_native_find_flexible("LinkVehicleToInterior", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)interiorid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, AddVehicleComponent(int vehicleid, int componentid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("AddVehicleComponent(%d, %d)", vehicleid, componentid);
  native = sampgdk_native_find_flexible("AddVehicleComponent", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)componentid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, RemoveVehicleComponent(int vehicleid, int componentid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("RemoveVehicleComponent(%d, %d)", vehicleid, componentid);
  native = sampgdk_native_find_flexible("RemoveVehicleComponent", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)componentid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ChangeVehicleColor(int vehicleid, int color1, int color2)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("ChangeVehicleColor(%d, %d, %d)", vehicleid, color1, color2);
  native = sampgdk_native_find_flexible("ChangeVehicleColor", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)color1;
  params[3] = (cell)color2;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ChangeVehiclePaintjob(int vehicleid, int paintjobid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("ChangeVehiclePaintjob(%d, %d)", vehicleid, paintjobid);
  native = sampgdk_native_find_flexible("ChangeVehiclePaintjob", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)paintjobid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleHealth(int vehicleid, float health)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetVehicleHealth(%d, %f)", vehicleid, health);
  native = sampgdk_native_find_flexible("SetVehicleHealth", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = amx_ftoc(health);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleHealth(int vehicleid, float * health)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell health_;
  sampgdk_log_debug("GetVehicleHealth(%d, @%p)", vehicleid, health);
  native = sampgdk_native_find_flexible("GetVehicleHealth", native);
  sampgdk_fakeamx_push(1, &health_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = health_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(health_, health);
  sampgdk_fakeamx_pop(health_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, AttachTrailerToVehicle(int trailerid, int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("AttachTrailerToVehicle(%d, %d)", trailerid, vehicleid);
  native = sampgdk_native_find_flexible("AttachTrailerToVehicle", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)trailerid;
  params[2] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, DetachTrailerFromVehicle(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DetachTrailerFromVehicle(%d)", vehicleid);
  native = sampgdk_native_find_flexible("DetachTrailerFromVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsTrailerAttachedToVehicle(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsTrailerAttachedToVehicle(%d)", vehicleid);
  native = sampgdk_native_find_flexible("IsTrailerAttachedToVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetVehicleTrailer(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleTrailer(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleTrailer", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleNumberPlate(int vehicleid, const char * numberplate)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell numberplate_;
  sampgdk_log_debug("SetVehicleNumberPlate(%d, \"%s\")", vehicleid, numberplate);
  native = sampgdk_native_find_flexible("SetVehicleNumberPlate", native);
  sampgdk_fakeamx_push_string(numberplate, NULL, &numberplate_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = numberplate_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(numberplate_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetVehicleModel(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleModel(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleModel", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetVehicleComponentInSlot(int vehicleid, int slot)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetVehicleComponentInSlot(%d, %d)", vehicleid, slot);
  native = sampgdk_native_find_flexible("GetVehicleComponentInSlot", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)slot;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetVehicleComponentType(int component)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleComponentType(%d)", component);
  native = sampgdk_native_find_flexible("GetVehicleComponentType", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)component;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, RepairVehicle(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("RepairVehicle(%d)", vehicleid);
  native = sampgdk_native_find_flexible("RepairVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleVelocity(int vehicleid, float * X, float * Y, float * Z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell X_;
  cell Y_;
  cell Z_;
  sampgdk_log_debug("GetVehicleVelocity(%d, @%p, @%p, @%p)", vehicleid, X, Y, Z);
  native = sampgdk_native_find_flexible("GetVehicleVelocity", native);
  sampgdk_fakeamx_push(1, &X_);
  sampgdk_fakeamx_push(1, &Y_);
  sampgdk_fakeamx_push(1, &Z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = X_;
  params[3] = Y_;
  params[4] = Z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(X_, X);
  sampgdk_fakeamx_get_float(Y_, Y);
  sampgdk_fakeamx_get_float(Z_, Z);
  sampgdk_fakeamx_pop(Z_);
  sampgdk_fakeamx_pop(Y_);
  sampgdk_fakeamx_pop(X_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleVelocity(int vehicleid, float X, float Y, float Z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetVehicleVelocity(%d, %f, %f, %f)", vehicleid, X, Y, Z);
  native = sampgdk_native_find_flexible("SetVehicleVelocity", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = amx_ftoc(X);
  params[3] = amx_ftoc(Y);
  params[4] = amx_ftoc(Z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleAngularVelocity(int vehicleid, float X, float Y, float Z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetVehicleAngularVelocity(%d, %f, %f, %f)", vehicleid, X, Y, Z);
  native = sampgdk_native_find_flexible("SetVehicleAngularVelocity", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = amx_ftoc(X);
  params[3] = amx_ftoc(Y);
  params[4] = amx_ftoc(Z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleDamageStatus(int vehicleid, int * panels, int * doors, int * lights, int * tires)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell panels_;
  cell doors_;
  cell lights_;
  cell tires_;
  sampgdk_log_debug("GetVehicleDamageStatus(%d, @%p, @%p, @%p, @%p)", vehicleid, panels, doors, lights, tires);
  native = sampgdk_native_find_flexible("GetVehicleDamageStatus", native);
  sampgdk_fakeamx_push(1, &panels_);
  sampgdk_fakeamx_push(1, &doors_);
  sampgdk_fakeamx_push(1, &lights_);
  sampgdk_fakeamx_push(1, &tires_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = panels_;
  params[3] = doors_;
  params[4] = lights_;
  params[5] = tires_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(panels_, panels);
  sampgdk_fakeamx_get_cell(doors_, doors);
  sampgdk_fakeamx_get_cell(lights_, lights);
  sampgdk_fakeamx_get_cell(tires_, tires);
  sampgdk_fakeamx_pop(tires_);
  sampgdk_fakeamx_pop(lights_);
  sampgdk_fakeamx_pop(doors_);
  sampgdk_fakeamx_pop(panels_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, UpdateVehicleDamageStatus(int vehicleid, int panels, int doors, int lights, int tires)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("UpdateVehicleDamageStatus(%d, %d, %d, %d, %d)", vehicleid, panels, doors, lights, tires);
  native = sampgdk_native_find_flexible("UpdateVehicleDamageStatus", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)panels;
  params[3] = (cell)doors;
  params[4] = (cell)lights;
  params[5] = (cell)tires;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleVirtualWorld(int vehicleid, int worldid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetVehicleVirtualWorld(%d, %d)", vehicleid, worldid);
  native = sampgdk_native_find_flexible("SetVehicleVirtualWorld", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)worldid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetVehicleVirtualWorld(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleVirtualWorld(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleVirtualWorld", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleModelInfo(int model, int infotype, float * X, float * Y, float * Z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell X_;
  cell Y_;
  cell Z_;
  sampgdk_log_debug("GetVehicleModelInfo(%d, %d, @%p, @%p, @%p)", model, infotype, X, Y, Z);
  native = sampgdk_native_find_flexible("GetVehicleModelInfo", native);
  sampgdk_fakeamx_push(1, &X_);
  sampgdk_fakeamx_push(1, &Y_);
  sampgdk_fakeamx_push(1, &Z_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)model;
  params[2] = (cell)infotype;
  params[3] = X_;
  params[4] = Y_;
  params[5] = Z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(X_, X);
  sampgdk_fakeamx_get_float(Y_, Y);
  sampgdk_fakeamx_get_float(Z_, Z);
  sampgdk_fakeamx_pop(Z_);
  sampgdk_fakeamx_pop(Y_);
  sampgdk_fakeamx_pop(X_);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(a_vehicles) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(a_vehicles) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, GetActorAnimation(int actorid, char * animationLibrary, int librarySize, char * animationName, int nameSize, float * delta, bool * loop, bool * lockX, bool * lockY, bool * freeze, int * time)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[12];
  cell animationLibrary_;
  cell animationName_;
  cell delta_;
  cell loop_;
  cell lockX_;
  cell lockY_;
  cell freeze_;
  cell time_;
  sampgdk_log_debug("GetActorAnimation(%d, @%p, %d, @%p, %d, @%p, @%p, @%p, @%p, @%p, @%p)", actorid, animationLibrary, librarySize, animationName, nameSize, delta, loop, lockX, lockY, freeze, time);
  native = sampgdk_native_find_flexible("GetActorAnimation", native);
  sampgdk_fakeamx_push(librarySize, &animationLibrary_);
  sampgdk_fakeamx_push(nameSize, &animationName_);
  sampgdk_fakeamx_push(1, &delta_);
  sampgdk_fakeamx_push(1, &loop_);
  sampgdk_fakeamx_push(1, &lockX_);
  sampgdk_fakeamx_push(1, &lockY_);
  sampgdk_fakeamx_push(1, &freeze_);
  sampgdk_fakeamx_push(1, &time_);
  params[0] = 11 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = animationLibrary_;
  params[3] = (cell)librarySize;
  params[4] = animationName_;
  params[5] = (cell)nameSize;
  params[6] = delta_;
  params[7] = loop_;
  params[8] = lockX_;
  params[9] = lockY_;
  params[10] = freeze_;
  params[11] = time_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(animationLibrary_, animationLibrary, librarySize);
  sampgdk_fakeamx_get_string(animationName_, animationName, nameSize);
  sampgdk_fakeamx_get_float(delta_, delta);
  sampgdk_fakeamx_get_bool(loop_, loop);
  sampgdk_fakeamx_get_bool(lockX_, lockX);
  sampgdk_fakeamx_get_bool(lockY_, lockY);
  sampgdk_fakeamx_get_bool(freeze_, freeze);
  sampgdk_fakeamx_get_cell(time_, time);
  sampgdk_fakeamx_pop(time_);
  sampgdk_fakeamx_pop(freeze_);
  sampgdk_fakeamx_pop(lockY_);
  sampgdk_fakeamx_pop(lockX_);
  sampgdk_fakeamx_pop(loop_);
  sampgdk_fakeamx_pop(delta_);
  sampgdk_fakeamx_pop(animationName_);
  sampgdk_fakeamx_pop(animationLibrary_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetActorSkin(int actorid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetActorSkin(%d)", actorid);
  native = sampgdk_native_find_flexible("GetActorSkin", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)actorid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetActorSpawnInfo(int actorid, int * skin, float * spawnX, float * spawnY, float * spawnZ, float * spawnAngle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  cell skin_;
  cell spawnX_;
  cell spawnY_;
  cell spawnZ_;
  cell spawnAngle_;
  sampgdk_log_debug("GetActorSpawnInfo(%d, @%p, @%p, @%p, @%p, @%p)", actorid, skin, spawnX, spawnY, spawnZ, spawnAngle);
  native = sampgdk_native_find_flexible("GetActorSpawnInfo", native);
  sampgdk_fakeamx_push(1, &skin_);
  sampgdk_fakeamx_push(1, &spawnX_);
  sampgdk_fakeamx_push(1, &spawnY_);
  sampgdk_fakeamx_push(1, &spawnZ_);
  sampgdk_fakeamx_push(1, &spawnAngle_);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = skin_;
  params[3] = spawnX_;
  params[4] = spawnY_;
  params[5] = spawnZ_;
  params[6] = spawnAngle_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(skin_, skin);
  sampgdk_fakeamx_get_float(spawnX_, spawnX);
  sampgdk_fakeamx_get_float(spawnY_, spawnY);
  sampgdk_fakeamx_get_float(spawnZ_, spawnZ);
  sampgdk_fakeamx_get_float(spawnAngle_, spawnAngle);
  sampgdk_fakeamx_pop(spawnAngle_);
  sampgdk_fakeamx_pop(spawnZ_);
  sampgdk_fakeamx_pop(spawnY_);
  sampgdk_fakeamx_pop(spawnX_);
  sampgdk_fakeamx_pop(skin_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetActors(char * actors, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell actors_;
  sampgdk_log_debug("GetActors(@%p, %d)", actors, size);
  native = sampgdk_native_find_flexible("GetActors", native);
  sampgdk_fakeamx_push(size, &actors_);
  params[0] = 2 * sizeof(cell);
  params[1] = actors_;
  params[2] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(actors_, actors, size);
  sampgdk_fakeamx_pop(actors_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, SetActorSkin(int actorid, int skin)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetActorSkin(%d, %d)", actorid, skin);
  native = sampgdk_native_find_flexible("SetActorSkin", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)actorid;
  params[2] = (cell)skin;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_actor) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_actor) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, GetPlayerCheckpoint(int playerid, float * centreX, float * centreY, float * centreZ, float * radius)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell centreX_;
  cell centreY_;
  cell centreZ_;
  cell radius_;
  sampgdk_log_debug("GetPlayerCheckpoint(%d, @%p, @%p, @%p, @%p)", playerid, centreX, centreY, centreZ, radius);
  native = sampgdk_native_find_flexible("GetPlayerCheckpoint", native);
  sampgdk_fakeamx_push(1, &centreX_);
  sampgdk_fakeamx_push(1, &centreY_);
  sampgdk_fakeamx_push(1, &centreZ_);
  sampgdk_fakeamx_push(1, &radius_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = centreX_;
  params[3] = centreY_;
  params[4] = centreZ_;
  params[5] = radius_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(centreX_, centreX);
  sampgdk_fakeamx_get_float(centreY_, centreY);
  sampgdk_fakeamx_get_float(centreZ_, centreZ);
  sampgdk_fakeamx_get_float(radius_, radius);
  sampgdk_fakeamx_pop(radius_);
  sampgdk_fakeamx_pop(centreZ_);
  sampgdk_fakeamx_pop(centreY_);
  sampgdk_fakeamx_pop(centreX_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerRaceCheckpoint(int playerid, float * centreX, float * centreY, float * centreZ, float * nextX, float * nextY, float * nextZ, float * radius)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  cell centreX_;
  cell centreY_;
  cell centreZ_;
  cell nextX_;
  cell nextY_;
  cell nextZ_;
  cell radius_;
  sampgdk_log_debug("GetPlayerRaceCheckpoint(%d, @%p, @%p, @%p, @%p, @%p, @%p, @%p)", playerid, centreX, centreY, centreZ, nextX, nextY, nextZ, radius);
  native = sampgdk_native_find_flexible("GetPlayerRaceCheckpoint", native);
  sampgdk_fakeamx_push(1, &centreX_);
  sampgdk_fakeamx_push(1, &centreY_);
  sampgdk_fakeamx_push(1, &centreZ_);
  sampgdk_fakeamx_push(1, &nextX_);
  sampgdk_fakeamx_push(1, &nextY_);
  sampgdk_fakeamx_push(1, &nextZ_);
  sampgdk_fakeamx_push(1, &radius_);
  params[0] = 8 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = centreX_;
  params[3] = centreY_;
  params[4] = centreZ_;
  params[5] = nextX_;
  params[6] = nextY_;
  params[7] = nextZ_;
  params[8] = radius_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(centreX_, centreX);
  sampgdk_fakeamx_get_float(centreY_, centreY);
  sampgdk_fakeamx_get_float(centreZ_, centreZ);
  sampgdk_fakeamx_get_float(nextX_, nextX);
  sampgdk_fakeamx_get_float(nextY_, nextY);
  sampgdk_fakeamx_get_float(nextZ_, nextZ);
  sampgdk_fakeamx_get_float(radius_, radius);
  sampgdk_fakeamx_pop(radius_);
  sampgdk_fakeamx_pop(nextZ_);
  sampgdk_fakeamx_pop(nextY_);
  sampgdk_fakeamx_pop(nextX_);
  sampgdk_fakeamx_pop(centreZ_);
  sampgdk_fakeamx_pop(centreY_);
  sampgdk_fakeamx_pop(centreX_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerCheckpointActive(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerCheckpointActive(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerCheckpointActive", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerRaceCheckpointActive(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerRaceCheckpointActive(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerRaceCheckpointActive", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_checkpoint) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_checkpoint) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, EditPlayerClass(int classid, int team, int skin, float spawnX, float spawnY, float spawnZ, float angle, int weapon1, int ammo1, int weapon2, int ammo2, int weapon3, int ammo3)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[14];
  sampgdk_log_debug("EditPlayerClass(%d, %d, %d, %f, %f, %f, %f, %d, %d, %d, %d, %d, %d)", classid, team, skin, spawnX, spawnY, spawnZ, angle, weapon1, ammo1, weapon2, ammo2, weapon3, ammo3);
  native = sampgdk_native_find_flexible("EditPlayerClass", native);
  params[0] = 13 * sizeof(cell);
  params[1] = (cell)classid;
  params[2] = (cell)team;
  params[3] = (cell)skin;
  params[4] = amx_ftoc(spawnX);
  params[5] = amx_ftoc(spawnY);
  params[6] = amx_ftoc(spawnZ);
  params[7] = amx_ftoc(angle);
  params[8] = (cell)weapon1;
  params[9] = (cell)ammo1;
  params[10] = (cell)weapon2;
  params[11] = (cell)ammo2;
  params[12] = (cell)weapon3;
  params[13] = (cell)ammo3;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetAvailableClasses()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetAvailableClasses()");
  native = sampgdk_native_find_flexible("GetAvailableClasses", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerClass(int classid, int * team, int * skin, float * spawnX, float * spawnY, float * spawnZ, float * angle, int * weapon1, int * ammo1, int * weapon2, int * ammo2, int * weapon3, int * ammo3)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[14];
  cell team_;
  cell skin_;
  cell spawnX_;
  cell spawnY_;
  cell spawnZ_;
  cell angle_;
  cell weapon1_;
  cell ammo1_;
  cell weapon2_;
  cell ammo2_;
  cell weapon3_;
  cell ammo3_;
  sampgdk_log_debug("GetPlayerClass(%d, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p)", classid, team, skin, spawnX, spawnY, spawnZ, angle, weapon1, ammo1, weapon2, ammo2, weapon3, ammo3);
  native = sampgdk_native_find_flexible("GetPlayerClass", native);
  sampgdk_fakeamx_push(1, &team_);
  sampgdk_fakeamx_push(1, &skin_);
  sampgdk_fakeamx_push(1, &spawnX_);
  sampgdk_fakeamx_push(1, &spawnY_);
  sampgdk_fakeamx_push(1, &spawnZ_);
  sampgdk_fakeamx_push(1, &angle_);
  sampgdk_fakeamx_push(1, &weapon1_);
  sampgdk_fakeamx_push(1, &ammo1_);
  sampgdk_fakeamx_push(1, &weapon2_);
  sampgdk_fakeamx_push(1, &ammo2_);
  sampgdk_fakeamx_push(1, &weapon3_);
  sampgdk_fakeamx_push(1, &ammo3_);
  params[0] = 13 * sizeof(cell);
  params[1] = (cell)classid;
  params[2] = team_;
  params[3] = skin_;
  params[4] = spawnX_;
  params[5] = spawnY_;
  params[6] = spawnZ_;
  params[7] = angle_;
  params[8] = weapon1_;
  params[9] = ammo1_;
  params[10] = weapon2_;
  params[11] = ammo2_;
  params[12] = weapon3_;
  params[13] = ammo3_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(team_, team);
  sampgdk_fakeamx_get_cell(skin_, skin);
  sampgdk_fakeamx_get_float(spawnX_, spawnX);
  sampgdk_fakeamx_get_float(spawnY_, spawnY);
  sampgdk_fakeamx_get_float(spawnZ_, spawnZ);
  sampgdk_fakeamx_get_float(angle_, angle);
  sampgdk_fakeamx_get_cell(weapon1_, weapon1);
  sampgdk_fakeamx_get_cell(ammo1_, ammo1);
  sampgdk_fakeamx_get_cell(weapon2_, weapon2);
  sampgdk_fakeamx_get_cell(ammo2_, ammo2);
  sampgdk_fakeamx_get_cell(weapon3_, weapon3);
  sampgdk_fakeamx_get_cell(ammo3_, ammo3);
  sampgdk_fakeamx_pop(ammo3_);
  sampgdk_fakeamx_pop(weapon3_);
  sampgdk_fakeamx_pop(ammo2_);
  sampgdk_fakeamx_pop(weapon2_);
  sampgdk_fakeamx_pop(ammo1_);
  sampgdk_fakeamx_pop(weapon1_);
  sampgdk_fakeamx_pop(angle_);
  sampgdk_fakeamx_pop(spawnZ_);
  sampgdk_fakeamx_pop(spawnY_);
  sampgdk_fakeamx_pop(spawnX_);
  sampgdk_fakeamx_pop(skin_);
  sampgdk_fakeamx_pop(team_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetSpawnInfo(int playerid, int * team, int * skin, float * spawnX, float * spawnY, float * spawnZ, float * angle, int * weapon1, int * ammo1, int * weapon2, int * ammo2, int * weapon3, int * ammo3)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[14];
  cell team_;
  cell skin_;
  cell spawnX_;
  cell spawnY_;
  cell spawnZ_;
  cell angle_;
  cell weapon1_;
  cell ammo1_;
  cell weapon2_;
  cell ammo2_;
  cell weapon3_;
  cell ammo3_;
  sampgdk_log_debug("GetSpawnInfo(%d, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p)", playerid, team, skin, spawnX, spawnY, spawnZ, angle, weapon1, ammo1, weapon2, ammo2, weapon3, ammo3);
  native = sampgdk_native_find_flexible("GetSpawnInfo", native);
  sampgdk_fakeamx_push(1, &team_);
  sampgdk_fakeamx_push(1, &skin_);
  sampgdk_fakeamx_push(1, &spawnX_);
  sampgdk_fakeamx_push(1, &spawnY_);
  sampgdk_fakeamx_push(1, &spawnZ_);
  sampgdk_fakeamx_push(1, &angle_);
  sampgdk_fakeamx_push(1, &weapon1_);
  sampgdk_fakeamx_push(1, &ammo1_);
  sampgdk_fakeamx_push(1, &weapon2_);
  sampgdk_fakeamx_push(1, &ammo2_);
  sampgdk_fakeamx_push(1, &weapon3_);
  sampgdk_fakeamx_push(1, &ammo3_);
  params[0] = 13 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = team_;
  params[3] = skin_;
  params[4] = spawnX_;
  params[5] = spawnY_;
  params[6] = spawnZ_;
  params[7] = angle_;
  params[8] = weapon1_;
  params[9] = ammo1_;
  params[10] = weapon2_;
  params[11] = ammo2_;
  params[12] = weapon3_;
  params[13] = ammo3_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(team_, team);
  sampgdk_fakeamx_get_cell(skin_, skin);
  sampgdk_fakeamx_get_float(spawnX_, spawnX);
  sampgdk_fakeamx_get_float(spawnY_, spawnY);
  sampgdk_fakeamx_get_float(spawnZ_, spawnZ);
  sampgdk_fakeamx_get_float(angle_, angle);
  sampgdk_fakeamx_get_cell(weapon1_, weapon1);
  sampgdk_fakeamx_get_cell(ammo1_, ammo1);
  sampgdk_fakeamx_get_cell(weapon2_, weapon2);
  sampgdk_fakeamx_get_cell(ammo2_, ammo2);
  sampgdk_fakeamx_get_cell(weapon3_, weapon3);
  sampgdk_fakeamx_get_cell(ammo3_, ammo3);
  sampgdk_fakeamx_pop(ammo3_);
  sampgdk_fakeamx_pop(weapon3_);
  sampgdk_fakeamx_pop(ammo2_);
  sampgdk_fakeamx_pop(weapon2_);
  sampgdk_fakeamx_pop(ammo1_);
  sampgdk_fakeamx_pop(weapon1_);
  sampgdk_fakeamx_pop(angle_);
  sampgdk_fakeamx_pop(spawnZ_);
  sampgdk_fakeamx_pop(spawnY_);
  sampgdk_fakeamx_pop(spawnX_);
  sampgdk_fakeamx_pop(skin_);
  sampgdk_fakeamx_pop(team_);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_class) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_class) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, AddServerRule(const char * rule, const char * format)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell rule_;
  cell format_;
  sampgdk_log_debug("AddServerRule(\"%s\", \"%s\")", rule, format);
  native = sampgdk_native_find_flexible("AddServerRule", native);
  sampgdk_fakeamx_push_string(rule, NULL, &rule_);
  sampgdk_fakeamx_push_string(format, NULL, &format_);
  params[0] = 2 * sizeof(cell);
  params[1] = rule_;
  params[2] = format_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(format_);
  sampgdk_fakeamx_pop(rule_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, AllowNickNameCharacter(int character, bool allow)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("AllowNickNameCharacter(%d, %d)", character, allow);
  native = sampgdk_native_find_flexible("AllowNickNameCharacter", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)character;
  params[2] = (cell)allow;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, AreAllAnimationsEnabled()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("AreAllAnimationsEnabled()");
  native = sampgdk_native_find_flexible("AreAllAnimationsEnabled", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, AreInteriorWeaponsAllowed()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("AreInteriorWeaponsAllowed()");
  native = sampgdk_native_find_flexible("AreInteriorWeaponsAllowed", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(int, CallLocalFunction(const char * functionName, const char * specifiers)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell functionName_;
  cell specifiers_;
  sampgdk_log_debug("CallLocalFunction(\"%s\", \"%s\")", functionName, specifiers);
  native = sampgdk_native_find_flexible("CallLocalFunction", native);
  sampgdk_fakeamx_push_string(functionName, NULL, &functionName_);
  sampgdk_fakeamx_push_string(specifiers, NULL, &specifiers_);
  params[0] = 2 * sizeof(cell);
  params[1] = functionName_;
  params[2] = specifiers_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(specifiers_);
  sampgdk_fakeamx_pop(functionName_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, CallRemoteFunction(const char * functionName, const char * specifiers)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell functionName_;
  cell specifiers_;
  sampgdk_log_debug("CallRemoteFunction(\"%s\", \"%s\")", functionName, specifiers);
  native = sampgdk_native_find_flexible("CallRemoteFunction", native);
  sampgdk_fakeamx_push_string(functionName, NULL, &functionName_);
  sampgdk_fakeamx_push_string(specifiers, NULL, &specifiers_);
  params[0] = 2 * sizeof(cell);
  params[1] = functionName_;
  params[2] = specifiers_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(specifiers_);
  sampgdk_fakeamx_pop(functionName_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, ChatTextReplacementToggled()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("ChatTextReplacementToggled()");
  native = sampgdk_native_find_flexible("ChatTextReplacementToggled", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(int, CountRunningTimers()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("CountRunningTimers()");
  native = sampgdk_native_find_flexible("CountRunningTimers", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, EnableAllAnimations(bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("EnableAllAnimations(%d)", enable);
  native = sampgdk_native_find_flexible("EnableAllAnimations", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, GetConsoleVarAsFloat(const char * cvar)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell cvar_;
  sampgdk_log_debug("GetConsoleVarAsFloat(\"%s\")", cvar);
  native = sampgdk_native_find_flexible("GetConsoleVarAsFloat", native);
  sampgdk_fakeamx_push_string(cvar, NULL, &cvar_);
  params[0] = 1 * sizeof(cell);
  params[1] = cvar_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(cvar_);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(float, GetModeRestartTime()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetModeRestartTime()");
  native = sampgdk_native_find_flexible("GetModeRestartTime", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, GetTimerInterval(int timerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetTimerInterval(%d)", timerid);
  native = sampgdk_native_find_flexible("GetTimerInterval", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)timerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetTimerRemaining(int timerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetTimerRemaining(%d)", timerid);
  native = sampgdk_native_find_flexible("GetTimerRemaining", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)timerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetWeaponSlot(int weaponid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetWeaponSlot(%d)", weaponid);
  native = sampgdk_native_find_flexible("GetWeaponSlot", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)weaponid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetWeather()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetWeather()");
  native = sampgdk_native_find_flexible("GetWeather", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetWorldTime()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetWorldTime()");
  native = sampgdk_native_find_flexible("GetWorldTime", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, HideGameTextForAll(int style)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("HideGameTextForAll(%d)", style);
  native = sampgdk_native_find_flexible("HideGameTextForAll", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)style;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsAdminTeleportAllowed()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("IsAdminTeleportAllowed()");
  native = sampgdk_native_find_flexible("IsAdminTeleportAllowed", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsNickNameCharacterAllowed(int character)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsNickNameCharacterAllowed(%d)", character);
  native = sampgdk_native_find_flexible("IsNickNameCharacterAllowed", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)character;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsRepeatingTimer(int timerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsRepeatingTimer(%d)", timerid);
  native = sampgdk_native_find_flexible("IsRepeatingTimer", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)timerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidNickName(const char * name)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell name_;
  sampgdk_log_debug("IsValidNickName(\"%s\")", name);
  native = sampgdk_native_find_flexible("IsValidNickName", native);
  sampgdk_fakeamx_push_string(name, NULL, &name_);
  params[0] = 1 * sizeof(cell);
  params[1] = name_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(name_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidServerRule(const char * rule)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell rule_;
  sampgdk_log_debug("IsValidServerRule(\"%s\")", rule);
  native = sampgdk_native_find_flexible("IsValidServerRule", native);
  sampgdk_fakeamx_push_string(rule, NULL, &rule_);
  params[0] = 1 * sizeof(cell);
  params[1] = rule_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(rule_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidTimer(int timerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsValidTimer(%d)", timerid);
  native = sampgdk_native_find_flexible("IsValidTimer", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)timerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, RemoveServerRule(const char * rule)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell rule_;
  sampgdk_log_debug("RemoveServerRule(\"%s\")", rule);
  native = sampgdk_native_find_flexible("RemoveServerRule", native);
  sampgdk_fakeamx_push_string(rule, NULL, &rule_);
  params[0] = 1 * sizeof(cell);
  params[1] = rule_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(rule_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetModeRestartTime(float seconds)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SetModeRestartTime(%f)", seconds);
  native = sampgdk_native_find_flexible("SetModeRestartTime", native);
  params[0] = 1 * sizeof(cell);
  params[1] = amx_ftoc(seconds);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetServerRule(const char * rule, const char * format)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell rule_;
  cell format_;
  sampgdk_log_debug("SetServerRule(\"%s\", \"%s\")", rule, format);
  native = sampgdk_native_find_flexible("SetServerRule", native);
  sampgdk_fakeamx_push_string(rule, NULL, &rule_);
  sampgdk_fakeamx_push_string(format, NULL, &format_);
  params[0] = 2 * sizeof(cell);
  params[1] = rule_;
  params[2] = format_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(format_);
  sampgdk_fakeamx_pop(rule_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, SetTimerEx(const char * functionName, int interval, bool repeating, const char * specifiers)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell functionName_;
  cell specifiers_;
  sampgdk_log_debug("SetTimerEx(\"%s\", %d, %d, \"%s\")", functionName, interval, repeating, specifiers);
  native = sampgdk_native_find_flexible("SetTimerEx", native);
  sampgdk_fakeamx_push_string(functionName, NULL, &functionName_);
  sampgdk_fakeamx_push_string(specifiers, NULL, &specifiers_);
  params[0] = 4 * sizeof(cell);
  params[1] = functionName_;
  params[2] = (cell)interval;
  params[3] = (cell)repeating;
  params[4] = specifiers_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(specifiers_);
  sampgdk_fakeamx_pop(functionName_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, ToggleChatTextReplacement(bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("ToggleChatTextReplacement(%d)", enable);
  native = sampgdk_native_find_flexible("ToggleChatTextReplacement", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, __open_mp_unused_print(const char * string_)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell string__;
  sampgdk_log_debug("__open_mp_unused_print(\"%s\")", string_);
  native = sampgdk_native_find_flexible("__open_mp_unused_print", native);
  sampgdk_fakeamx_push_string(string_, NULL, &string__);
  params[0] = 1 * sizeof(cell);
  params[1] = string__;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(string__);
  return (int)(retval);
}

SAMPGDK_MODULE_INIT(omp_core) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_core) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, DB_Close(int db)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DB_Close(%d)", db);
  native = sampgdk_native_find_flexible("DB_Close", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)db;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, DB_ExecuteQuery(int db, const char * query)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell query_;
  sampgdk_log_debug("DB_ExecuteQuery(%d, \"%s\")", db, query);
  native = sampgdk_native_find_flexible("DB_ExecuteQuery", native);
  sampgdk_fakeamx_push_string(query, NULL, &query_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)db;
  params[2] = query_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(query_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, DB_FreeResultSet(int result)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DB_FreeResultSet(%d)", result);
  native = sampgdk_native_find_flexible("DB_FreeResultSet", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)result;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, DB_GetDatabaseConnectionCount()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("DB_GetDatabaseConnectionCount()");
  native = sampgdk_native_find_flexible("DB_GetDatabaseConnectionCount", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, DB_GetDatabaseResultSetCount()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("DB_GetDatabaseResultSetCount()");
  native = sampgdk_native_find_flexible("DB_GetDatabaseResultSetCount", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, DB_GetFieldCount(int result)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DB_GetFieldCount(%d)", result);
  native = sampgdk_native_find_flexible("DB_GetFieldCount", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)result;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, DB_GetFieldFloat(int result, int field)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("DB_GetFieldFloat(%d, %d)", result, field);
  native = sampgdk_native_find_flexible("DB_GetFieldFloat", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)result;
  params[2] = (cell)field;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(float, DB_GetFieldFloatByName(int result, const char * field)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell field_;
  sampgdk_log_debug("DB_GetFieldFloatByName(%d, \"%s\")", result, field);
  native = sampgdk_native_find_flexible("DB_GetFieldFloatByName", native);
  sampgdk_fakeamx_push_string(field, NULL, &field_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)result;
  params[2] = field_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(field_);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, DB_GetFieldInt(int result, int field)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("DB_GetFieldInt(%d, %d)", result, field);
  native = sampgdk_native_find_flexible("DB_GetFieldInt", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)result;
  params[2] = (cell)field;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, DB_GetFieldIntByName(int result, const char * field)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell field_;
  sampgdk_log_debug("DB_GetFieldIntByName(%d, \"%s\")", result, field);
  native = sampgdk_native_find_flexible("DB_GetFieldIntByName", native);
  sampgdk_fakeamx_push_string(field, NULL, &field_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)result;
  params[2] = field_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(field_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, DB_GetFieldName(int result, int field, char * output, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell output_;
  sampgdk_log_debug("DB_GetFieldName(%d, %d, @%p, %d)", result, field, output, size);
  native = sampgdk_native_find_flexible("DB_GetFieldName", native);
  sampgdk_fakeamx_push(size, &output_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)result;
  params[2] = (cell)field;
  params[3] = output_;
  params[4] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(output_, output, size);
  sampgdk_fakeamx_pop(output_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, DB_GetFieldString(int result, int field, char * output, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell output_;
  sampgdk_log_debug("DB_GetFieldString(%d, %d, @%p, %d)", result, field, output, size);
  native = sampgdk_native_find_flexible("DB_GetFieldString", native);
  sampgdk_fakeamx_push(size, &output_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)result;
  params[2] = (cell)field;
  params[3] = output_;
  params[4] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(output_, output, size);
  sampgdk_fakeamx_pop(output_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, DB_GetFieldStringByName(int result, const char * field, char * output, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell field_;
  cell output_;
  sampgdk_log_debug("DB_GetFieldStringByName(%d, \"%s\", @%p, %d)", result, field, output, size);
  native = sampgdk_native_find_flexible("DB_GetFieldStringByName", native);
  sampgdk_fakeamx_push_string(field, NULL, &field_);
  sampgdk_fakeamx_push(size, &output_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)result;
  params[2] = field_;
  params[3] = output_;
  params[4] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(output_, output, size);
  sampgdk_fakeamx_pop(output_);
  sampgdk_fakeamx_pop(field_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, DB_GetLegacyDBResult(int result)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DB_GetLegacyDBResult(%d)", result);
  native = sampgdk_native_find_flexible("DB_GetLegacyDBResult", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)result;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, DB_GetMemHandle(int db)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DB_GetMemHandle(%d)", db);
  native = sampgdk_native_find_flexible("DB_GetMemHandle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)db;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, DB_GetRowCount(int result)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DB_GetRowCount(%d)", result);
  native = sampgdk_native_find_flexible("DB_GetRowCount", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)result;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, DB_Open(const char * name, int flags)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell name_;
  sampgdk_log_debug("DB_Open(\"%s\", %d)", name, flags);
  native = sampgdk_native_find_flexible("DB_Open", native);
  sampgdk_fakeamx_push_string(name, NULL, &name_);
  params[0] = 2 * sizeof(cell);
  params[1] = name_;
  params[2] = (cell)flags;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(name_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, DB_SelectNextRow(int result)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("DB_SelectNextRow(%d)", result);
  native = sampgdk_native_find_flexible("DB_SelectNextRow", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)result;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_database) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_database) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, GetPlayerDialogData(int playerid, int * style, char * title, int titleSize, char * body, int bodySize, char * button1, int button1Size, char * button2, int button2Size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[11];
  cell style_;
  cell title_;
  cell body_;
  cell button1_;
  cell button2_;
  sampgdk_log_debug("GetPlayerDialogData(%d, @%p, @%p, %d, @%p, %d, @%p, %d, @%p, %d)", playerid, style, title, titleSize, body, bodySize, button1, button1Size, button2, button2Size);
  native = sampgdk_native_find_flexible("GetPlayerDialogData", native);
  sampgdk_fakeamx_push(1, &style_);
  sampgdk_fakeamx_push(titleSize, &title_);
  sampgdk_fakeamx_push(bodySize, &body_);
  sampgdk_fakeamx_push(button1Size, &button1_);
  sampgdk_fakeamx_push(button2Size, &button2_);
  params[0] = 10 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = style_;
  params[3] = title_;
  params[4] = (cell)titleSize;
  params[5] = body_;
  params[6] = (cell)bodySize;
  params[7] = button1_;
  params[8] = (cell)button1Size;
  params[9] = button2_;
  params[10] = (cell)button2Size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(style_, style);
  sampgdk_fakeamx_get_string(title_, title, titleSize);
  sampgdk_fakeamx_get_string(body_, body, bodySize);
  sampgdk_fakeamx_get_string(button1_, button1, button1Size);
  sampgdk_fakeamx_get_string(button2_, button2, button2Size);
  sampgdk_fakeamx_pop(button2_);
  sampgdk_fakeamx_pop(button1_);
  sampgdk_fakeamx_pop(body_);
  sampgdk_fakeamx_pop(title_);
  sampgdk_fakeamx_pop(style_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerDialogID(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerDialogID(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerDialogID", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, HidePlayerDialog(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("HidePlayerDialog(%d)", playerid);
  native = sampgdk_native_find_flexible("HidePlayerDialog", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_dialog) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_dialog) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(int, CreatePlayerGangZone(int playerid, float minx, float miny, float maxx, float maxy)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("CreatePlayerGangZone(%d, %f, %f, %f, %f)", playerid, minx, miny, maxx, maxy);
  native = sampgdk_native_find_flexible("CreatePlayerGangZone", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(minx);
  params[3] = amx_ftoc(miny);
  params[4] = amx_ftoc(maxx);
  params[5] = amx_ftoc(maxy);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GangZoneGetColourForPlayer(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GangZoneGetColourForPlayer(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("GangZoneGetColourForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GangZoneGetFlashColourForPlayer(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GangZoneGetFlashColourForPlayer(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("GangZoneGetFlashColourForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GangZoneGetPos(int zoneid, float * minX, float * minY, float * maxX, float * maxY)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell minX_;
  cell minY_;
  cell maxX_;
  cell maxY_;
  sampgdk_log_debug("GangZoneGetPos(%d, @%p, @%p, @%p, @%p)", zoneid, minX, minY, maxX, maxY);
  native = sampgdk_native_find_flexible("GangZoneGetPos", native);
  sampgdk_fakeamx_push(1, &minX_);
  sampgdk_fakeamx_push(1, &minY_);
  sampgdk_fakeamx_push(1, &maxX_);
  sampgdk_fakeamx_push(1, &maxY_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)zoneid;
  params[2] = minX_;
  params[3] = minY_;
  params[4] = maxX_;
  params[5] = maxY_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(minX_, minX);
  sampgdk_fakeamx_get_float(minY_, minY);
  sampgdk_fakeamx_get_float(maxX_, maxX);
  sampgdk_fakeamx_get_float(maxY_, maxY);
  sampgdk_fakeamx_pop(maxY_);
  sampgdk_fakeamx_pop(maxX_);
  sampgdk_fakeamx_pop(minY_);
  sampgdk_fakeamx_pop(minX_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsGangZoneFlashingForPlayer(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsGangZoneFlashingForPlayer(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("IsGangZoneFlashingForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsGangZoneVisibleForPlayer(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsGangZoneVisibleForPlayer(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("IsGangZoneVisibleForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerGangZoneFlashing(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPlayerGangZoneFlashing(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("IsPlayerGangZoneFlashing", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerGangZoneVisible(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPlayerGangZoneVisible(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("IsPlayerGangZoneVisible", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerInGangZone(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPlayerInGangZone(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("IsPlayerInGangZone", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerInPlayerGangZone(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPlayerInPlayerGangZone(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("IsPlayerInPlayerGangZone", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidGangZone(int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsValidGangZone(%d)", zoneid);
  native = sampgdk_native_find_flexible("IsValidGangZone", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidPlayerGangZone(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsValidPlayerGangZone(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("IsValidPlayerGangZone", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerGangZoneDestroy(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerGangZoneDestroy(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("PlayerGangZoneDestroy", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerGangZoneFlash(int playerid, int zoneid, int flashColour)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerGangZoneFlash(%d, %d, %d)", playerid, zoneid, flashColour);
  native = sampgdk_native_find_flexible("PlayerGangZoneFlash", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  params[3] = (cell)flashColour;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, PlayerGangZoneGetColour(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerGangZoneGetColour(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("PlayerGangZoneGetColour", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, PlayerGangZoneGetFlashColour(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerGangZoneGetFlashColour(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("PlayerGangZoneGetFlashColour", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, PlayerGangZoneGetPos(int playerid, int zoneid, float * minX, float * minY, float * maxX, float * maxY)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  cell minX_;
  cell minY_;
  cell maxX_;
  cell maxY_;
  sampgdk_log_debug("PlayerGangZoneGetPos(%d, %d, @%p, @%p, @%p, @%p)", playerid, zoneid, minX, minY, maxX, maxY);
  native = sampgdk_native_find_flexible("PlayerGangZoneGetPos", native);
  sampgdk_fakeamx_push(1, &minX_);
  sampgdk_fakeamx_push(1, &minY_);
  sampgdk_fakeamx_push(1, &maxX_);
  sampgdk_fakeamx_push(1, &maxY_);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  params[3] = minX_;
  params[4] = minY_;
  params[5] = maxX_;
  params[6] = maxY_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(minX_, minX);
  sampgdk_fakeamx_get_float(minY_, minY);
  sampgdk_fakeamx_get_float(maxX_, maxX);
  sampgdk_fakeamx_get_float(maxY_, maxY);
  sampgdk_fakeamx_pop(maxY_);
  sampgdk_fakeamx_pop(maxX_);
  sampgdk_fakeamx_pop(minY_);
  sampgdk_fakeamx_pop(minX_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerGangZoneHide(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerGangZoneHide(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("PlayerGangZoneHide", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerGangZoneShow(int playerid, int zoneid, int colour)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("PlayerGangZoneShow(%d, %d, %d)", playerid, zoneid, colour);
  native = sampgdk_native_find_flexible("PlayerGangZoneShow", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  params[3] = (cell)colour;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerGangZoneStopFlash(int playerid, int zoneid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerGangZoneStopFlash(%d, %d)", playerid, zoneid);
  native = sampgdk_native_find_flexible("PlayerGangZoneStopFlash", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, UseGangZoneCheck(int zoneid, bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("UseGangZoneCheck(%d, %d)", zoneid, enable);
  native = sampgdk_native_find_flexible("UseGangZoneCheck", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)zoneid;
  params[2] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, UsePlayerGangZoneCheck(int playerid, int zoneid, bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("UsePlayerGangZoneCheck(%d, %d, %d)", playerid, zoneid, enable);
  native = sampgdk_native_find_flexible("UsePlayerGangZoneCheck", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)zoneid;
  params[3] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_gangzone) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_gangzone) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_MODULE_INIT(omp_http) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_http) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, GetMenuColumnHeader(int menuid, int column, char * header, int len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell header_;
  sampgdk_log_debug("GetMenuColumnHeader(%d, %d, @%p, %d)", menuid, column, header, len);
  native = sampgdk_native_find_flexible("GetMenuColumnHeader", native);
  sampgdk_fakeamx_push(len, &header_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)menuid;
  params[2] = (cell)column;
  params[3] = header_;
  params[4] = (cell)len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(header_, header, len);
  sampgdk_fakeamx_pop(header_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetMenuColumnWidth(int menuid, float * column1Width, float * column2Width)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell column1Width_;
  cell column2Width_;
  sampgdk_log_debug("GetMenuColumnWidth(%d, @%p, @%p)", menuid, column1Width, column2Width);
  native = sampgdk_native_find_flexible("GetMenuColumnWidth", native);
  sampgdk_fakeamx_push(1, &column1Width_);
  sampgdk_fakeamx_push(1, &column2Width_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)menuid;
  params[2] = column1Width_;
  params[3] = column2Width_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(column1Width_, column1Width);
  sampgdk_fakeamx_get_float(column2Width_, column2Width);
  sampgdk_fakeamx_pop(column2Width_);
  sampgdk_fakeamx_pop(column1Width_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetMenuColumns(int menuid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetMenuColumns(%d)", menuid);
  native = sampgdk_native_find_flexible("GetMenuColumns", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)menuid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetMenuItem(int menuid, int column, int row, char * cell_, int len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell cell__;
  sampgdk_log_debug("GetMenuItem(%d, %d, %d, @%p, %d)", menuid, column, row, cell_, len);
  native = sampgdk_native_find_flexible("GetMenuItem", native);
  sampgdk_fakeamx_push(len, &cell__);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)menuid;
  params[2] = (cell)column;
  params[3] = (cell)row;
  params[4] = cell__;
  params[5] = (cell)len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(cell__, cell_, len);
  sampgdk_fakeamx_pop(cell__);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetMenuItems(int menuid, int column)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetMenuItems(%d, %d)", menuid, column);
  native = sampgdk_native_find_flexible("GetMenuItems", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)menuid;
  params[2] = (cell)column;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetMenuPos(int menuid, float * x, float * y)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell x_;
  cell y_;
  sampgdk_log_debug("GetMenuPos(%d, @%p, @%p)", menuid, x, y);
  native = sampgdk_native_find_flexible("GetMenuPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)menuid;
  params[2] = x_;
  params[3] = y_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsMenuDisabled(int menuid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsMenuDisabled(%d)", menuid);
  native = sampgdk_native_find_flexible("IsMenuDisabled", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)menuid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsMenuRowDisabled(int menuid, int row)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsMenuRowDisabled(%d, %d)", menuid, row);
  native = sampgdk_native_find_flexible("IsMenuRowDisabled", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)menuid;
  params[2] = (cell)row;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_menu) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_menu) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, ClearBanList()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("ClearBanList()");
  native = sampgdk_native_find_flexible("ClearBanList", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerRawIp(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerRawIp(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerRawIp", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, IsBanned(const char * ipaddress)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell ipaddress_;
  sampgdk_log_debug("IsBanned(\"%s\")", ipaddress);
  native = sampgdk_native_find_flexible("IsBanned", native);
  sampgdk_fakeamx_push_string(ipaddress, NULL, &ipaddress_);
  params[0] = 1 * sizeof(cell);
  params[1] = ipaddress_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(ipaddress_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SendClientCheck(int playerid, int type, int memoryAddress, int memoryOffset, int byteCount)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("SendClientCheck(%d, %d, %d, %d, %d)", playerid, type, memoryAddress, memoryOffset, byteCount);
  native = sampgdk_native_find_flexible("SendClientCheck", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)type;
  params[3] = (cell)memoryAddress;
  params[4] = (cell)memoryOffset;
  params[5] = (cell)byteCount;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerAdmin(int playerid, bool admin)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerAdmin(%d, %d)", playerid, admin);
  native = sampgdk_native_find_flexible("SetPlayerAdmin", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)admin;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_network) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_network) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, NPC_AddPointToPath(int pathid, float x, float y, float z, float stopRange)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("NPC_AddPointToPath(%d, %f, %f, %f, %f)", pathid, x, y, z, stopRange);
  native = sampgdk_native_find_flexible("NPC_AddPointToPath", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)pathid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  params[5] = amx_ftoc(stopRange);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_AimAt(int npcid, float pointX, float pointY, float pointZ, bool shoot, int shootDelay, bool updateAngle, float offsetFromX, float offsetFromY, float offsetFromZ, int checkInBetweenFlags)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[12];
  sampgdk_log_debug("NPC_AimAt(%d, %f, %f, %f, %d, %d, %d, %f, %f, %f, %d)", npcid, pointX, pointY, pointZ, shoot, shootDelay, updateAngle, offsetFromX, offsetFromY, offsetFromZ, checkInBetweenFlags);
  native = sampgdk_native_find_flexible("NPC_AimAt", native);
  params[0] = 11 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = amx_ftoc(pointX);
  params[3] = amx_ftoc(pointY);
  params[4] = amx_ftoc(pointZ);
  params[5] = (cell)shoot;
  params[6] = (cell)shootDelay;
  params[7] = (cell)updateAngle;
  params[8] = amx_ftoc(offsetFromX);
  params[9] = amx_ftoc(offsetFromY);
  params[10] = amx_ftoc(offsetFromZ);
  params[11] = (cell)checkInBetweenFlags;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_AimAtPlayer(int npcid, int playerid, bool shoot, int shootDelay, bool updateAngle, float offsetX, float offsetY, float offsetZ, float offsetFromX, float offsetFromY, float offsetFromZ, int checkInBetweenFlags)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[13];
  sampgdk_log_debug("NPC_AimAtPlayer(%d, %d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %d)", npcid, playerid, shoot, shootDelay, updateAngle, offsetX, offsetY, offsetZ, offsetFromX, offsetFromY, offsetFromZ, checkInBetweenFlags);
  native = sampgdk_native_find_flexible("NPC_AimAtPlayer", native);
  params[0] = 12 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)playerid;
  params[3] = (cell)shoot;
  params[4] = (cell)shootDelay;
  params[5] = (cell)updateAngle;
  params[6] = amx_ftoc(offsetX);
  params[7] = amx_ftoc(offsetY);
  params[8] = amx_ftoc(offsetZ);
  params[9] = amx_ftoc(offsetFromX);
  params[10] = amx_ftoc(offsetFromY);
  params[11] = amx_ftoc(offsetFromZ);
  params[12] = (cell)checkInBetweenFlags;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_ApplyAnimation(int npcid, const char * animlib, const char * animname, float delta, bool loop, bool lockX, bool lockY, bool freeze, int time)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  cell animlib_;
  cell animname_;
  sampgdk_log_debug("NPC_ApplyAnimation(%d, \"%s\", \"%s\", %f, %d, %d, %d, %d, %d)", npcid, animlib, animname, delta, loop, lockX, lockY, freeze, time);
  native = sampgdk_native_find_flexible("NPC_ApplyAnimation", native);
  sampgdk_fakeamx_push_string(animlib, NULL, &animlib_);
  sampgdk_fakeamx_push_string(animname, NULL, &animname_);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = animlib_;
  params[3] = animname_;
  params[4] = amx_ftoc(delta);
  params[5] = (cell)loop;
  params[6] = (cell)lockX;
  params[7] = (cell)lockY;
  params[8] = (cell)freeze;
  params[9] = (cell)time;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(animname_);
  sampgdk_fakeamx_pop(animlib_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_ChangeNode(int npcid, int nodeid, int linkid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("NPC_ChangeNode(%d, %d, %d)", npcid, nodeid, linkid);
  native = sampgdk_native_find_flexible("NPC_ChangeNode", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)nodeid;
  params[3] = (cell)linkid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_ClearAnimations(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_ClearAnimations(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_ClearAnimations", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_ClearPath(int pathid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_ClearPath(%d)", pathid);
  native = sampgdk_native_find_flexible("NPC_ClearPath", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)pathid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_CloseNode(int nodeid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_CloseNode(%d)", nodeid);
  native = sampgdk_native_find_flexible("NPC_CloseNode", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)nodeid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_Create(const char * name)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell name_;
  sampgdk_log_debug("NPC_Create(\"%s\")", name);
  native = sampgdk_native_find_flexible("NPC_Create", native);
  sampgdk_fakeamx_push_string(name, NULL, &name_);
  params[0] = 1 * sizeof(cell);
  params[1] = name_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(name_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_CreatePath()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("NPC_CreatePath()");
  native = sampgdk_native_find_flexible("NPC_CreatePath", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_Destroy(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_Destroy(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_Destroy", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_DestroyAllPath()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("NPC_DestroyAllPath()");
  native = sampgdk_native_find_flexible("NPC_DestroyAllPath", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_DestroyPath(int pathid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_DestroyPath(%d)", pathid);
  native = sampgdk_native_find_flexible("NPC_DestroyPath", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)pathid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_EnableInfiniteAmmo(int npcid, bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_EnableInfiniteAmmo(%d, %d)", npcid, enable);
  native = sampgdk_native_find_flexible("NPC_EnableInfiniteAmmo", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_EnableReloading(int npcid, bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_EnableReloading(%d, %d)", npcid, enable);
  native = sampgdk_native_find_flexible("NPC_EnableReloading", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_EnterVehicle(int npcid, int vehicleid, int seatid, int moveType)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("NPC_EnterVehicle(%d, %d, %d, %d)", npcid, vehicleid, seatid, moveType);
  native = sampgdk_native_find_flexible("NPC_EnterVehicle", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)vehicleid;
  params[3] = (cell)seatid;
  params[4] = (cell)moveType;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_ExitVehicle(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_ExitVehicle(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_ExitVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_GetAll(char * npcs, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell npcs_;
  sampgdk_log_debug("NPC_GetAll(@%p, %d)", npcs, size);
  native = sampgdk_native_find_flexible("NPC_GetAll", native);
  sampgdk_fakeamx_push(size, &npcs_);
  params[0] = 2 * sizeof(cell);
  params[1] = npcs_;
  params[2] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(npcs_, npcs, size);
  sampgdk_fakeamx_pop(npcs_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetAmmo(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetAmmo(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetAmmo", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetAmmoInClip(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetAmmoInClip(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetAmmoInClip", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetAnimation(int npcid, int * animationId, float * delta, bool * loop, bool * lockX, bool * lockY, bool * freeze, int * time)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  cell animationId_;
  cell delta_;
  cell loop_;
  cell lockX_;
  cell lockY_;
  cell freeze_;
  cell time_;
  sampgdk_log_debug("NPC_GetAnimation(%d, @%p, @%p, @%p, @%p, @%p, @%p, @%p)", npcid, animationId, delta, loop, lockX, lockY, freeze, time);
  native = sampgdk_native_find_flexible("NPC_GetAnimation", native);
  sampgdk_fakeamx_push(1, &animationId_);
  sampgdk_fakeamx_push(1, &delta_);
  sampgdk_fakeamx_push(1, &loop_);
  sampgdk_fakeamx_push(1, &lockX_);
  sampgdk_fakeamx_push(1, &lockY_);
  sampgdk_fakeamx_push(1, &freeze_);
  sampgdk_fakeamx_push(1, &time_);
  params[0] = 8 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = animationId_;
  params[3] = delta_;
  params[4] = loop_;
  params[5] = lockX_;
  params[6] = lockY_;
  params[7] = freeze_;
  params[8] = time_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(animationId_, animationId);
  sampgdk_fakeamx_get_float(delta_, delta);
  sampgdk_fakeamx_get_bool(loop_, loop);
  sampgdk_fakeamx_get_bool(lockX_, lockX);
  sampgdk_fakeamx_get_bool(lockY_, lockY);
  sampgdk_fakeamx_get_bool(freeze_, freeze);
  sampgdk_fakeamx_get_cell(time_, time);
  sampgdk_fakeamx_pop(time_);
  sampgdk_fakeamx_pop(freeze_);
  sampgdk_fakeamx_pop(lockY_);
  sampgdk_fakeamx_pop(lockX_);
  sampgdk_fakeamx_pop(loop_);
  sampgdk_fakeamx_pop(delta_);
  sampgdk_fakeamx_pop(animationId_);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, NPC_GetArmour(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetArmour(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetArmour", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, NPC_GetCurrentPathPointIndex(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetCurrentPathPointIndex(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetCurrentPathPointIndex", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetCustomSkin(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetCustomSkin(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetCustomSkin", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetEnteringVehicle(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetEnteringVehicle(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetEnteringVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetEnteringVehicleID(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetEnteringVehicleID(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetEnteringVehicleID", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetEnteringVehicleSeat(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetEnteringVehicleSeat(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetEnteringVehicleSeat", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetFacingAngle(int npcid, float * angle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell angle_;
  sampgdk_log_debug("NPC_GetFacingAngle(%d, @%p)", npcid, angle);
  native = sampgdk_native_find_flexible("NPC_GetFacingAngle", native);
  sampgdk_fakeamx_push(1, &angle_);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = angle_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(angle_, angle);
  sampgdk_fakeamx_pop(angle_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetFightingStyle(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetFightingStyle(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetFightingStyle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, NPC_GetHealth(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetHealth(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetHealth", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, NPC_GetInterior(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetInterior(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetInterior", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetKeys(int npcid, int * upAnddown, int * leftandRight, int * keys)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell upAnddown_;
  cell leftandRight_;
  cell keys_;
  sampgdk_log_debug("NPC_GetKeys(%d, @%p, @%p, @%p)", npcid, upAnddown, leftandRight, keys);
  native = sampgdk_native_find_flexible("NPC_GetKeys", native);
  sampgdk_fakeamx_push(1, &upAnddown_);
  sampgdk_fakeamx_push(1, &leftandRight_);
  sampgdk_fakeamx_push(1, &keys_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = upAnddown_;
  params[3] = leftandRight_;
  params[4] = keys_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(upAnddown_, upAnddown);
  sampgdk_fakeamx_get_cell(leftandRight_, leftandRight);
  sampgdk_fakeamx_get_cell(keys_, keys);
  sampgdk_fakeamx_pop(keys_);
  sampgdk_fakeamx_pop(leftandRight_);
  sampgdk_fakeamx_pop(upAnddown_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_GetNodeInfo(int nodeid, int * vehnodes, int * pednodes, int * navinode)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell vehnodes_;
  cell pednodes_;
  cell navinode_;
  sampgdk_log_debug("NPC_GetNodeInfo(%d, @%p, @%p, @%p)", nodeid, vehnodes, pednodes, navinode);
  native = sampgdk_native_find_flexible("NPC_GetNodeInfo", native);
  sampgdk_fakeamx_push(1, &vehnodes_);
  sampgdk_fakeamx_push(1, &pednodes_);
  sampgdk_fakeamx_push(1, &navinode_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)nodeid;
  params[2] = vehnodes_;
  params[3] = pednodes_;
  params[4] = navinode_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(vehnodes_, vehnodes);
  sampgdk_fakeamx_get_cell(pednodes_, pednodes);
  sampgdk_fakeamx_get_cell(navinode_, navinode);
  sampgdk_fakeamx_pop(navinode_);
  sampgdk_fakeamx_pop(pednodes_);
  sampgdk_fakeamx_pop(vehnodes_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_GetNodePointCount(int nodeid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetNodePointCount(%d)", nodeid);
  native = sampgdk_native_find_flexible("NPC_GetNodePointCount", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)nodeid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_GetNodePointPosition(int nodeid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("NPC_GetNodePointPosition(%d, @%p, @%p, @%p)", nodeid, x, y, z);
  native = sampgdk_native_find_flexible("NPC_GetNodePointPosition", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)nodeid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_GetNodeType(int nodeid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetNodeType(%d)", nodeid);
  native = sampgdk_native_find_flexible("NPC_GetNodeType", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)nodeid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetPathCount()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("NPC_GetPathCount()");
  native = sampgdk_native_find_flexible("NPC_GetPathCount", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_GetPathPoint(int pathid, int pointIndex, float * x, float * y, float * z, float * stopRange)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  cell x_;
  cell y_;
  cell z_;
  cell stopRange_;
  sampgdk_log_debug("NPC_GetPathPoint(%d, %d, @%p, @%p, @%p, @%p)", pathid, pointIndex, x, y, z, stopRange);
  native = sampgdk_native_find_flexible("NPC_GetPathPoint", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  sampgdk_fakeamx_push(1, &stopRange_);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)pathid;
  params[2] = (cell)pointIndex;
  params[3] = x_;
  params[4] = y_;
  params[5] = z_;
  params[6] = stopRange_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_get_float(stopRange_, stopRange);
  sampgdk_fakeamx_pop(stopRange_);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_GetPathPointCount(int pathid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetPathPointCount(%d)", pathid);
  native = sampgdk_native_find_flexible("NPC_GetPathPointCount", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)pathid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetPlayerAimingAt(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetPlayerAimingAt(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetPlayerAimingAt", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetPlayerMovingTo(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetPlayerMovingTo(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetPlayerMovingTo", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetPos(int npcid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("NPC_GetPos(%d, @%p, @%p, @%p)", npcid, x, y, z);
  native = sampgdk_native_find_flexible("NPC_GetPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetPosMovingTo(int npcid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("NPC_GetPosMovingTo(%d, @%p, @%p, @%p)", npcid, x, y, z);
  native = sampgdk_native_find_flexible("NPC_GetPosMovingTo", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetRecordCount()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("NPC_GetRecordCount()");
  native = sampgdk_native_find_flexible("NPC_GetRecordCount", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetRot(int npcid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("NPC_GetRot(%d, @%p, @%p, @%p)", npcid, x, y, z);
  native = sampgdk_native_find_flexible("NPC_GetRot", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetSkin(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetSkin(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetSkin", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetSpecialAction(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetSpecialAction(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetSpecialAction", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetSurfingObject(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetSurfingObject(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetSurfingObject", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetSurfingOffsets(int npcid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("NPC_GetSurfingOffsets(%d, @%p, @%p, @%p)", npcid, x, y, z);
  native = sampgdk_native_find_flexible("NPC_GetSurfingOffsets", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetSurfingPlayerObject(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetSurfingPlayerObject(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetSurfingPlayerObject", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetSurfingVehicle(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetSurfingVehicle(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetSurfingVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetVehicle(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetVehicle(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetVehicleGearState(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetVehicleGearState(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetVehicleGearState", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, NPC_GetVehicleHealth(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetVehicleHealth(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetVehicleHealth", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, NPC_GetVehicleHydraThrusters(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetVehicleHydraThrusters(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetVehicleHydraThrusters", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetVehicleID(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetVehicleID(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetVehicleID", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetVehicleSeat(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetVehicleSeat(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetVehicleSeat", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, NPC_GetVehicleTrainSpeed(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetVehicleTrainSpeed(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetVehicleTrainSpeed", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, NPC_GetVelocity(int npcid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("NPC_GetVelocity(%d, @%p, @%p, @%p)", npcid, x, y, z);
  native = sampgdk_native_find_flexible("NPC_GetVelocity", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetVirtualWorld(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetVirtualWorld(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetVirtualWorld", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetWeapon(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetWeapon(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetWeapon", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, NPC_GetWeaponAccuracy(int npcid, int weaponid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_GetWeaponAccuracy(%d, %d)", npcid, weaponid);
  native = sampgdk_native_find_flexible("NPC_GetWeaponAccuracy", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, NPC_GetWeaponActualClipSize(int npcid, int weaponid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_GetWeaponActualClipSize(%d, %d)", npcid, weaponid);
  native = sampgdk_native_find_flexible("NPC_GetWeaponActualClipSize", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetWeaponActualReloadTime(int npcid, int weaponid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_GetWeaponActualReloadTime(%d, %d)", npcid, weaponid);
  native = sampgdk_native_find_flexible("NPC_GetWeaponActualReloadTime", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetWeaponClipSize(int npcid, int weaponid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_GetWeaponClipSize(%d, %d)", npcid, weaponid);
  native = sampgdk_native_find_flexible("NPC_GetWeaponClipSize", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetWeaponReloadTime(int npcid, int weaponid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_GetWeaponReloadTime(%d, %d)", npcid, weaponid);
  native = sampgdk_native_find_flexible("NPC_GetWeaponReloadTime", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetWeaponShootTime(int npcid, int weaponid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_GetWeaponShootTime(%d, %d)", npcid, weaponid);
  native = sampgdk_native_find_flexible("NPC_GetWeaponShootTime", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetWeaponSkillLevel(int npcid, int skill)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_GetWeaponSkillLevel(%d, %d)", npcid, skill);
  native = sampgdk_native_find_flexible("NPC_GetWeaponSkillLevel", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)skill;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_GetWeaponState(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_GetWeaponState(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_GetWeaponState", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_HasPathPointInRange(int pathId, float x, float y, float z, float radius)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("NPC_HasPathPointInRange(%d, %f, %f, %f, %f)", pathId, x, y, z, radius);
  native = sampgdk_native_find_flexible("NPC_HasPathPointInRange", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)pathId;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  params[5] = amx_ftoc(radius);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsAiming(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsAiming(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsAiming", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsAimingAtPlayer(int npcid, int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_IsAimingAtPlayer(%d, %d)", npcid, playerid);
  native = sampgdk_native_find_flexible("NPC_IsAimingAtPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsAnyStreamedIn(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsAnyStreamedIn(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsAnyStreamedIn", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsDead(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsDead(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsDead", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsEnteringVehicle(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsEnteringVehicle(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsEnteringVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsInfiniteAmmoEnabled(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsInfiniteAmmoEnabled(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsInfiniteAmmoEnabled", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsInvulnerable(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsInvulnerable(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsInvulnerable", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsMeleeAttacking(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsMeleeAttacking(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsMeleeAttacking", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsMoving(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsMoving(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsMoving", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsMovingToPlayer(int npcid, int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_IsMovingToPlayer(%d, %d)", npcid, playerid);
  native = sampgdk_native_find_flexible("NPC_IsMovingToPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsNodeOpen(int nodeid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsNodeOpen(%d)", nodeid);
  native = sampgdk_native_find_flexible("NPC_IsNodeOpen", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)nodeid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsPlaybackPaused(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsPlaybackPaused(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsPlaybackPaused", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsPlayingNode(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsPlayingNode(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsPlayingNode", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsPlayingNodePaused(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsPlayingNodePaused(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsPlayingNodePaused", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsPlayingPlayback(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsPlayingPlayback(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsPlayingPlayback", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsReloadEnabled(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsReloadEnabled(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsReloadEnabled", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsReloading(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsReloading(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsReloading", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsShooting(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsShooting(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsShooting", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsSpawned(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsSpawned(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsSpawned", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsStreamedIn(int npcid, int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_IsStreamedIn(%d, %d)", npcid, playerid);
  native = sampgdk_native_find_flexible("NPC_IsStreamedIn", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsValid(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsValid(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsValid", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsValidPath(int pathid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsValidPath(%d)", pathid);
  native = sampgdk_native_find_flexible("NPC_IsValidPath", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)pathid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsValidRecord(int recordId)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsValidRecord(%d)", recordId);
  native = sampgdk_native_find_flexible("NPC_IsValidRecord", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)recordId;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_IsVehicleSirenUsed(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_IsVehicleSirenUsed(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_IsVehicleSirenUsed", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_Kill(int npcid, int killerid, int reason)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("NPC_Kill(%d, %d, %d)", npcid, killerid, reason);
  native = sampgdk_native_find_flexible("NPC_Kill", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)killerid;
  params[3] = (cell)reason;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_LoadRecord(const char * filePath)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  cell filePath_;
  sampgdk_log_debug("NPC_LoadRecord(\"%s\")", filePath);
  native = sampgdk_native_find_flexible("NPC_LoadRecord", native);
  sampgdk_fakeamx_push_string(filePath, NULL, &filePath_);
  params[0] = 1 * sizeof(cell);
  params[1] = filePath_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(filePath_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_MeleeAttack(int npcid, int time, bool secondaryAttack)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("NPC_MeleeAttack(%d, %d, %d)", npcid, time, secondaryAttack);
  native = sampgdk_native_find_flexible("NPC_MeleeAttack", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)time;
  params[3] = (cell)secondaryAttack;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_Move(int npcid, float x, float y, float z, int moveType, float moveSpeed, float stopRange)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[8];
  sampgdk_log_debug("NPC_Move(%d, %f, %f, %f, %d, %f, %f)", npcid, x, y, z, moveType, moveSpeed, stopRange);
  native = sampgdk_native_find_flexible("NPC_Move", native);
  params[0] = 7 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  params[5] = (cell)moveType;
  params[6] = amx_ftoc(moveSpeed);
  params[7] = amx_ftoc(stopRange);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_MoveByPath(int npcid, int pathid, int moveType, float moveSpeed, bool reversed)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("NPC_MoveByPath(%d, %d, %d, %f, %d)", npcid, pathid, moveType, moveSpeed, reversed);
  native = sampgdk_native_find_flexible("NPC_MoveByPath", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)pathid;
  params[3] = (cell)moveType;
  params[4] = amx_ftoc(moveSpeed);
  params[5] = (cell)reversed;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_MoveToPlayer(int npcid, int playerid, int moveType, float moveSpeed, float stopRange, int updateDelayMS, int autoRestart)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[8];
  sampgdk_log_debug("NPC_MoveToPlayer(%d, %d, %d, %f, %f, %d, %d)", npcid, playerid, moveType, moveSpeed, stopRange, updateDelayMS, autoRestart);
  native = sampgdk_native_find_flexible("NPC_MoveToPlayer", native);
  params[0] = 7 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)playerid;
  params[3] = (cell)moveType;
  params[4] = amx_ftoc(moveSpeed);
  params[5] = amx_ftoc(stopRange);
  params[6] = (cell)updateDelayMS;
  params[7] = (cell)autoRestart;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_OpenNode(int nodeid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_OpenNode(%d)", nodeid);
  native = sampgdk_native_find_flexible("NPC_OpenNode", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)nodeid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_PausePlayback(int npcid, bool paused)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_PausePlayback(%d, %d)", npcid, paused);
  native = sampgdk_native_find_flexible("NPC_PausePlayback", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)paused;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_PausePlayingNode(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_PausePlayingNode(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_PausePlayingNode", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_PlayNode(int npcid, int nodeid, int moveType, float speed, float radius, bool setAngle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  sampgdk_log_debug("NPC_PlayNode(%d, %d, %d, %f, %f, %d)", npcid, nodeid, moveType, speed, radius, setAngle);
  native = sampgdk_native_find_flexible("NPC_PlayNode", native);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)nodeid;
  params[3] = (cell)moveType;
  params[4] = amx_ftoc(speed);
  params[5] = amx_ftoc(radius);
  params[6] = (cell)setAngle;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_PutInVehicle(int npcid, int vehicleid, int seatid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("NPC_PutInVehicle(%d, %d, %d)", npcid, vehicleid, seatid);
  native = sampgdk_native_find_flexible("NPC_PutInVehicle", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)vehicleid;
  params[3] = (cell)seatid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_RemoveFromVehicle(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_RemoveFromVehicle(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_RemoveFromVehicle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_RemovePointFromPath(int pathid, int pointIndex)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_RemovePointFromPath(%d, %d)", pathid, pointIndex);
  native = sampgdk_native_find_flexible("NPC_RemovePointFromPath", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)pathid;
  params[2] = (cell)pointIndex;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_ResetAnimation(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_ResetAnimation(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_ResetAnimation", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_ResetSurfingData(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_ResetSurfingData(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_ResetSurfingData", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_Respawn(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_Respawn(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_Respawn", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_ResumePlayingNode(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_ResumePlayingNode(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_ResumePlayingNode", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_SetAmmo(int npcid, int ammo)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetAmmo(%d, %d)", npcid, ammo);
  native = sampgdk_native_find_flexible("NPC_SetAmmo", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)ammo;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetAmmoInClip(int npcid, int ammo)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetAmmoInClip(%d, %d)", npcid, ammo);
  native = sampgdk_native_find_flexible("NPC_SetAmmoInClip", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)ammo;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetAngleToPlayer(int npcid, int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetAngleToPlayer(%d, %d)", npcid, playerid);
  native = sampgdk_native_find_flexible("NPC_SetAngleToPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetAngleToPos(int npcid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("NPC_SetAngleToPos(%d, %f, %f, %f)", npcid, x, y, z);
  native = sampgdk_native_find_flexible("NPC_SetAngleToPos", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetAnimation(int npcid, int animationId, float delta, bool loop, bool lockX, bool lockY, bool freeze, int time)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  sampgdk_log_debug("NPC_SetAnimation(%d, %d, %f, %d, %d, %d, %d, %d)", npcid, animationId, delta, loop, lockX, lockY, freeze, time);
  native = sampgdk_native_find_flexible("NPC_SetAnimation", native);
  params[0] = 8 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)animationId;
  params[3] = amx_ftoc(delta);
  params[4] = (cell)loop;
  params[5] = (cell)lockX;
  params[6] = (cell)lockY;
  params[7] = (cell)freeze;
  params[8] = (cell)time;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetArmour(int npcid, float armour)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetArmour(%d, %f)", npcid, armour);
  native = sampgdk_native_find_flexible("NPC_SetArmour", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = amx_ftoc(armour);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetFacingAngle(int npcid, float angle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetFacingAngle(%d, %f)", npcid, angle);
  native = sampgdk_native_find_flexible("NPC_SetFacingAngle", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = amx_ftoc(angle);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetFightingStyle(int npcid, int style)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetFightingStyle(%d, %d)", npcid, style);
  native = sampgdk_native_find_flexible("NPC_SetFightingStyle", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)style;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetHealth(int npcid, float health)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetHealth(%d, %f)", npcid, health);
  native = sampgdk_native_find_flexible("NPC_SetHealth", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = amx_ftoc(health);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetInterior(int npcid, int interiorid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetInterior(%d, %d)", npcid, interiorid);
  native = sampgdk_native_find_flexible("NPC_SetInterior", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)interiorid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetInvulnerable(int npcid, bool toggle)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetInvulnerable(%d, %d)", npcid, toggle);
  native = sampgdk_native_find_flexible("NPC_SetInvulnerable", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)toggle;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetKeys(int npcid, int upAndDown, int leftAndRight, int keys)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("NPC_SetKeys(%d, %d, %d, %d)", npcid, upAndDown, leftAndRight, keys);
  native = sampgdk_native_find_flexible("NPC_SetKeys", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)upAndDown;
  params[3] = (cell)leftAndRight;
  params[4] = (cell)keys;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_SetNodePoint(int nodeid, int pointid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetNodePoint(%d, %d)", nodeid, pointid);
  native = sampgdk_native_find_flexible("NPC_SetNodePoint", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)nodeid;
  params[2] = (cell)pointid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_SetPos(int npcid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("NPC_SetPos(%d, %f, %f, %f)", npcid, x, y, z);
  native = sampgdk_native_find_flexible("NPC_SetPos", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetRot(int npcid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("NPC_SetRot(%d, %f, %f, %f)", npcid, x, y, z);
  native = sampgdk_native_find_flexible("NPC_SetRot", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetSkin(int npcid, int skinid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetSkin(%d, %d)", npcid, skinid);
  native = sampgdk_native_find_flexible("NPC_SetSkin", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)skinid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetSpecialAction(int npcid, int actionid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetSpecialAction(%d, %d)", npcid, actionid);
  native = sampgdk_native_find_flexible("NPC_SetSpecialAction", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)actionid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetSurfingObject(int npcid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetSurfingObject(%d, %d)", npcid, objectid);
  native = sampgdk_native_find_flexible("NPC_SetSurfingObject", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetSurfingOffsets(int npcid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("NPC_SetSurfingOffsets(%d, %f, %f, %f)", npcid, x, y, z);
  native = sampgdk_native_find_flexible("NPC_SetSurfingOffsets", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetSurfingPlayerObject(int npcid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetSurfingPlayerObject(%d, %d)", npcid, objectid);
  native = sampgdk_native_find_flexible("NPC_SetSurfingPlayerObject", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetSurfingVehicle(int npcid, int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetSurfingVehicle(%d, %d)", npcid, vehicleid);
  native = sampgdk_native_find_flexible("NPC_SetSurfingVehicle", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetVehicleGearState(int npcid, int gearState)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetVehicleGearState(%d, %d)", npcid, gearState);
  native = sampgdk_native_find_flexible("NPC_SetVehicleGearState", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)gearState;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetVehicleHealth(int npcid, float health)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetVehicleHealth(%d, %f)", npcid, health);
  native = sampgdk_native_find_flexible("NPC_SetVehicleHealth", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = amx_ftoc(health);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetVehicleHydraThrusters(int npcid, int direction)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetVehicleHydraThrusters(%d, %d)", npcid, direction);
  native = sampgdk_native_find_flexible("NPC_SetVehicleHydraThrusters", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)direction;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetVehicleTrainSpeed(int npcid, float speed)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetVehicleTrainSpeed(%d, %f)", npcid, speed);
  native = sampgdk_native_find_flexible("NPC_SetVehicleTrainSpeed", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = amx_ftoc(speed);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetVelocity(int npcid, float x, float y, float z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("NPC_SetVelocity(%d, %f, %f, %f)", npcid, x, y, z);
  native = sampgdk_native_find_flexible("NPC_SetVelocity", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetVirtualWorld(int npcid, int virtualWorld)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetVirtualWorld(%d, %d)", npcid, virtualWorld);
  native = sampgdk_native_find_flexible("NPC_SetVirtualWorld", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)virtualWorld;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetWeapon(int npcid, int weaponid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetWeapon(%d, %d)", npcid, weaponid);
  native = sampgdk_native_find_flexible("NPC_SetWeapon", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetWeaponAccuracy(int npcid, int weaponid, float accuracy)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("NPC_SetWeaponAccuracy(%d, %d, %f)", npcid, weaponid, accuracy);
  native = sampgdk_native_find_flexible("NPC_SetWeaponAccuracy", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponid;
  params[3] = amx_ftoc(accuracy);
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetWeaponClipSize(int npcid, int weaponid, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("NPC_SetWeaponClipSize(%d, %d, %d)", npcid, weaponid, size);
  native = sampgdk_native_find_flexible("NPC_SetWeaponClipSize", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponid;
  params[3] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetWeaponReloadTime(int npcid, int weaponid, int time)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("NPC_SetWeaponReloadTime(%d, %d, %d)", npcid, weaponid, time);
  native = sampgdk_native_find_flexible("NPC_SetWeaponReloadTime", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponid;
  params[3] = (cell)time;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_SetWeaponShootTime(int npcid, int weaponid, int time)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("NPC_SetWeaponShootTime(%d, %d, %d)", npcid, weaponid, time);
  native = sampgdk_native_find_flexible("NPC_SetWeaponShootTime", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponid;
  params[3] = (cell)time;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_SetWeaponSkillLevel(int npcid, int skill, int level)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("NPC_SetWeaponSkillLevel(%d, %d, %d)", npcid, skill, level);
  native = sampgdk_native_find_flexible("NPC_SetWeaponSkillLevel", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)skill;
  params[3] = (cell)level;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, NPC_SetWeaponState(int npcid, int weaponState)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_SetWeaponState(%d, %d)", npcid, weaponState);
  native = sampgdk_native_find_flexible("NPC_SetWeaponState", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponState;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_Shoot(int npcid, int weaponid, int hitId, int hitType, float endPointX, float endPointY, float endPointZ, float offsetX, float offsetY, float offsetZ, bool isHit, int checkInBetweenFlags)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[13];
  sampgdk_log_debug("NPC_Shoot(%d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %d, %d)", npcid, weaponid, hitId, hitType, endPointX, endPointY, endPointZ, offsetX, offsetY, offsetZ, isHit, checkInBetweenFlags);
  native = sampgdk_native_find_flexible("NPC_Shoot", native);
  params[0] = 12 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)weaponid;
  params[3] = (cell)hitId;
  params[4] = (cell)hitType;
  params[5] = amx_ftoc(endPointX);
  params[6] = amx_ftoc(endPointY);
  params[7] = amx_ftoc(endPointZ);
  params[8] = amx_ftoc(offsetX);
  params[9] = amx_ftoc(offsetY);
  params[10] = amx_ftoc(offsetZ);
  params[11] = (cell)isHit;
  params[12] = (cell)checkInBetweenFlags;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, NPC_Spawn(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_Spawn(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_Spawn", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, NPC_StartPlayback(int npcid, const char * recordName, bool autoUnload, float startX, float startY, float startZ, float rotX, float rotY, float rotZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  cell recordName_;
  sampgdk_log_debug("NPC_StartPlayback(%d, \"%s\", %d, %f, %f, %f, %f, %f, %f)", npcid, recordName, autoUnload, startX, startY, startZ, rotX, rotY, rotZ);
  native = sampgdk_native_find_flexible("NPC_StartPlayback", native);
  sampgdk_fakeamx_push_string(recordName, NULL, &recordName_);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = recordName_;
  params[3] = (cell)autoUnload;
  params[4] = amx_ftoc(startX);
  params[5] = amx_ftoc(startY);
  params[6] = amx_ftoc(startZ);
  params[7] = amx_ftoc(rotX);
  params[8] = amx_ftoc(rotY);
  params[9] = amx_ftoc(rotZ);
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(recordName_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_StartPlaybackEx(int npcid, int recordId, bool autoUnload, float startX, float startY, float startZ, float rotX, float rotY, float rotZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  sampgdk_log_debug("NPC_StartPlaybackEx(%d, %d, %d, %f, %f, %f, %f, %f, %f)", npcid, recordId, autoUnload, startX, startY, startZ, rotX, rotY, rotZ);
  native = sampgdk_native_find_flexible("NPC_StartPlaybackEx", native);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)recordId;
  params[3] = (cell)autoUnload;
  params[4] = amx_ftoc(startX);
  params[5] = amx_ftoc(startY);
  params[6] = amx_ftoc(startZ);
  params[7] = amx_ftoc(rotX);
  params[8] = amx_ftoc(rotY);
  params[9] = amx_ftoc(rotZ);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_StopAim(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_StopAim(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_StopAim", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_StopMeleeAttack(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_StopMeleeAttack(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_StopMeleeAttack", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_StopMove(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_StopMove(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_StopMove", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_StopPlayback(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_StopPlayback(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_StopPlayback", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_StopPlayingNode(int npcid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_StopPlayingNode(%d)", npcid);
  native = sampgdk_native_find_flexible("NPC_StopPlayingNode", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)npcid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_UnloadAllRecords()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("NPC_UnloadAllRecords()");
  native = sampgdk_native_find_flexible("NPC_UnloadAllRecords", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_UnloadRecord(int recordId)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("NPC_UnloadRecord(%d)", recordId);
  native = sampgdk_native_find_flexible("NPC_UnloadRecord", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)recordId;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_UpdateNodePoint(int npcid, int pointid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_UpdateNodePoint(%d, %d)", npcid, pointid);
  native = sampgdk_native_find_flexible("NPC_UpdateNodePoint", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)pointid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, NPC_UseVehicleSiren(int npcid, bool use)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("NPC_UseVehicleSiren(%d, %d)", npcid, use);
  native = sampgdk_native_find_flexible("NPC_UseVehicleSiren", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)npcid;
  params[2] = (cell)use;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_npc) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_npc) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, AttachPlayerObjectToObject(int playerid, int objectid, int parentid, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ, bool syncRotation)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[11];
  sampgdk_log_debug("AttachPlayerObjectToObject(%d, %d, %d, %f, %f, %f, %f, %f, %f, %d)", playerid, objectid, parentid, offsetX, offsetY, offsetZ, rotationX, rotationY, rotationZ, syncRotation);
  native = sampgdk_native_find_flexible("AttachPlayerObjectToObject", native);
  params[0] = 10 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = (cell)parentid;
  params[4] = amx_ftoc(offsetX);
  params[5] = amx_ftoc(offsetY);
  params[6] = amx_ftoc(offsetZ);
  params[7] = amx_ftoc(rotationX);
  params[8] = amx_ftoc(rotationY);
  params[9] = amx_ftoc(rotationZ);
  params[10] = (cell)syncRotation;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, BeginObjectEditing(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("BeginObjectEditing(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("BeginObjectEditing", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, BeginObjectSelecting(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("BeginObjectSelecting(%d)", playerid);
  native = sampgdk_native_find_flexible("BeginObjectSelecting", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, BeginPlayerObjectEditing(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("BeginPlayerObjectEditing(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("BeginPlayerObjectEditing", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, EndObjectEditing(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("EndObjectEditing(%d)", playerid);
  native = sampgdk_native_find_flexible("EndObjectEditing", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetCustomModelPath(int modelid, char * dffPath, int dffSize, char * txdPath, int txdSize)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell dffPath_;
  cell txdPath_;
  sampgdk_log_debug("GetCustomModelPath(%d, @%p, %d, @%p, %d)", modelid, dffPath, dffSize, txdPath, txdSize);
  native = sampgdk_native_find_flexible("GetCustomModelPath", native);
  sampgdk_fakeamx_push(dffSize, &dffPath_);
  sampgdk_fakeamx_push(txdSize, &txdPath_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)modelid;
  params[2] = dffPath_;
  params[3] = (cell)dffSize;
  params[4] = txdPath_;
  params[5] = (cell)txdSize;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(dffPath_, dffPath, dffSize);
  sampgdk_fakeamx_get_string(txdPath_, txdPath, txdSize);
  sampgdk_fakeamx_pop(txdPath_);
  sampgdk_fakeamx_pop(dffPath_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetObjectAttachedData(int objectid, int * parentVehicle, int * parentObject, int * parentPlayer)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell parentVehicle_;
  cell parentObject_;
  cell parentPlayer_;
  sampgdk_log_debug("GetObjectAttachedData(%d, @%p, @%p, @%p)", objectid, parentVehicle, parentObject, parentPlayer);
  native = sampgdk_native_find_flexible("GetObjectAttachedData", native);
  sampgdk_fakeamx_push(1, &parentVehicle_);
  sampgdk_fakeamx_push(1, &parentObject_);
  sampgdk_fakeamx_push(1, &parentPlayer_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = parentVehicle_;
  params[3] = parentObject_;
  params[4] = parentPlayer_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(parentVehicle_, parentVehicle);
  sampgdk_fakeamx_get_cell(parentObject_, parentObject);
  sampgdk_fakeamx_get_cell(parentPlayer_, parentPlayer);
  sampgdk_fakeamx_pop(parentPlayer_);
  sampgdk_fakeamx_pop(parentObject_);
  sampgdk_fakeamx_pop(parentVehicle_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetObjectAttachedOffset(int objectid, float * offsetX, float * offsetY, float * offsetZ, float * rotationX, float * rotationY, float * rotationZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[8];
  cell offsetX_;
  cell offsetY_;
  cell offsetZ_;
  cell rotationX_;
  cell rotationY_;
  cell rotationZ_;
  sampgdk_log_debug("GetObjectAttachedOffset(%d, @%p, @%p, @%p, @%p, @%p, @%p)", objectid, offsetX, offsetY, offsetZ, rotationX, rotationY, rotationZ);
  native = sampgdk_native_find_flexible("GetObjectAttachedOffset", native);
  sampgdk_fakeamx_push(1, &offsetX_);
  sampgdk_fakeamx_push(1, &offsetY_);
  sampgdk_fakeamx_push(1, &offsetZ_);
  sampgdk_fakeamx_push(1, &rotationX_);
  sampgdk_fakeamx_push(1, &rotationY_);
  sampgdk_fakeamx_push(1, &rotationZ_);
  params[0] = 7 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = offsetX_;
  params[3] = offsetY_;
  params[4] = offsetZ_;
  params[5] = rotationX_;
  params[6] = rotationY_;
  params[7] = rotationZ_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(offsetX_, offsetX);
  sampgdk_fakeamx_get_float(offsetY_, offsetY);
  sampgdk_fakeamx_get_float(offsetZ_, offsetZ);
  sampgdk_fakeamx_get_float(rotationX_, rotationX);
  sampgdk_fakeamx_get_float(rotationY_, rotationY);
  sampgdk_fakeamx_get_float(rotationZ_, rotationZ);
  sampgdk_fakeamx_pop(rotationZ_);
  sampgdk_fakeamx_pop(rotationY_);
  sampgdk_fakeamx_pop(rotationX_);
  sampgdk_fakeamx_pop(offsetZ_);
  sampgdk_fakeamx_pop(offsetY_);
  sampgdk_fakeamx_pop(offsetX_);
  return !!(retval);
}

SAMPGDK_NATIVE(float, GetObjectDrawDistance(int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetObjectDrawDistance(%d)", objectid);
  native = sampgdk_native_find_flexible("GetObjectDrawDistance", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(bool, GetObjectMaterial(int objectid, int materialIndex, int * modelid, char * textureLibrary, int textureLibrarySize, char * textureName, int textureNameSize, int * materialColour)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  cell modelid_;
  cell textureLibrary_;
  cell textureName_;
  cell materialColour_;
  sampgdk_log_debug("GetObjectMaterial(%d, %d, @%p, @%p, %d, @%p, %d, @%p)", objectid, materialIndex, modelid, textureLibrary, textureLibrarySize, textureName, textureNameSize, materialColour);
  native = sampgdk_native_find_flexible("GetObjectMaterial", native);
  sampgdk_fakeamx_push(1, &modelid_);
  sampgdk_fakeamx_push(textureLibrarySize, &textureLibrary_);
  sampgdk_fakeamx_push(textureNameSize, &textureName_);
  sampgdk_fakeamx_push(1, &materialColour_);
  params[0] = 8 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = (cell)materialIndex;
  params[3] = modelid_;
  params[4] = textureLibrary_;
  params[5] = (cell)textureLibrarySize;
  params[6] = textureName_;
  params[7] = (cell)textureNameSize;
  params[8] = materialColour_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(modelid_, modelid);
  sampgdk_fakeamx_get_string(textureLibrary_, textureLibrary, textureLibrarySize);
  sampgdk_fakeamx_get_string(textureName_, textureName, textureNameSize);
  sampgdk_fakeamx_get_cell(materialColour_, materialColour);
  sampgdk_fakeamx_pop(materialColour_);
  sampgdk_fakeamx_pop(textureName_);
  sampgdk_fakeamx_pop(textureLibrary_);
  sampgdk_fakeamx_pop(modelid_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetObjectMaterialText(int objectid, int materialIndex, char * text, int textSize, int * materialSize, char * fontFace, int fontFaceSize, int * fontSize, bool * bold, int * fontColour, int * backgroundColour, int * textAlignment)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[13];
  cell text_;
  cell materialSize_;
  cell fontFace_;
  cell fontSize_;
  cell bold_;
  cell fontColour_;
  cell backgroundColour_;
  cell textAlignment_;
  sampgdk_log_debug("GetObjectMaterialText(%d, %d, @%p, %d, @%p, @%p, %d, @%p, @%p, @%p, @%p, @%p)", objectid, materialIndex, text, textSize, materialSize, fontFace, fontFaceSize, fontSize, bold, fontColour, backgroundColour, textAlignment);
  native = sampgdk_native_find_flexible("GetObjectMaterialText", native);
  sampgdk_fakeamx_push(textSize, &text_);
  sampgdk_fakeamx_push(1, &materialSize_);
  sampgdk_fakeamx_push(fontFaceSize, &fontFace_);
  sampgdk_fakeamx_push(1, &fontSize_);
  sampgdk_fakeamx_push(1, &bold_);
  sampgdk_fakeamx_push(1, &fontColour_);
  sampgdk_fakeamx_push(1, &backgroundColour_);
  sampgdk_fakeamx_push(1, &textAlignment_);
  params[0] = 12 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = (cell)materialIndex;
  params[3] = text_;
  params[4] = (cell)textSize;
  params[5] = materialSize_;
  params[6] = fontFace_;
  params[7] = (cell)fontFaceSize;
  params[8] = fontSize_;
  params[9] = bold_;
  params[10] = fontColour_;
  params[11] = backgroundColour_;
  params[12] = textAlignment_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(text_, text, textSize);
  sampgdk_fakeamx_get_cell(materialSize_, materialSize);
  sampgdk_fakeamx_get_string(fontFace_, fontFace, fontFaceSize);
  sampgdk_fakeamx_get_cell(fontSize_, fontSize);
  sampgdk_fakeamx_get_bool(bold_, bold);
  sampgdk_fakeamx_get_cell(fontColour_, fontColour);
  sampgdk_fakeamx_get_cell(backgroundColour_, backgroundColour);
  sampgdk_fakeamx_get_cell(textAlignment_, textAlignment);
  sampgdk_fakeamx_pop(textAlignment_);
  sampgdk_fakeamx_pop(backgroundColour_);
  sampgdk_fakeamx_pop(fontColour_);
  sampgdk_fakeamx_pop(bold_);
  sampgdk_fakeamx_pop(fontSize_);
  sampgdk_fakeamx_pop(fontFace_);
  sampgdk_fakeamx_pop(materialSize_);
  sampgdk_fakeamx_pop(text_);
  return !!(retval);
}

SAMPGDK_NATIVE(float, GetObjectMoveSpeed(int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetObjectMoveSpeed(%d)", objectid);
  native = sampgdk_native_find_flexible("GetObjectMoveSpeed", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(bool, GetObjectMovingTargetPos(int objectid, float * targetX, float * targetY, float * targetZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell targetX_;
  cell targetY_;
  cell targetZ_;
  sampgdk_log_debug("GetObjectMovingTargetPos(%d, @%p, @%p, @%p)", objectid, targetX, targetY, targetZ);
  native = sampgdk_native_find_flexible("GetObjectMovingTargetPos", native);
  sampgdk_fakeamx_push(1, &targetX_);
  sampgdk_fakeamx_push(1, &targetY_);
  sampgdk_fakeamx_push(1, &targetZ_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = targetX_;
  params[3] = targetY_;
  params[4] = targetZ_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(targetX_, targetX);
  sampgdk_fakeamx_get_float(targetY_, targetY);
  sampgdk_fakeamx_get_float(targetZ_, targetZ);
  sampgdk_fakeamx_pop(targetZ_);
  sampgdk_fakeamx_pop(targetY_);
  sampgdk_fakeamx_pop(targetX_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetObjectMovingTargetRot(int objectid, float * rotationX, float * rotationY, float * rotationZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell rotationX_;
  cell rotationY_;
  cell rotationZ_;
  sampgdk_log_debug("GetObjectMovingTargetRot(%d, @%p, @%p, @%p)", objectid, rotationX, rotationY, rotationZ);
  native = sampgdk_native_find_flexible("GetObjectMovingTargetRot", native);
  sampgdk_fakeamx_push(1, &rotationX_);
  sampgdk_fakeamx_push(1, &rotationY_);
  sampgdk_fakeamx_push(1, &rotationZ_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = rotationX_;
  params[3] = rotationY_;
  params[4] = rotationZ_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(rotationX_, rotationX);
  sampgdk_fakeamx_get_float(rotationY_, rotationY);
  sampgdk_fakeamx_get_float(rotationZ_, rotationZ);
  sampgdk_fakeamx_pop(rotationZ_);
  sampgdk_fakeamx_pop(rotationY_);
  sampgdk_fakeamx_pop(rotationX_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetObjectSyncRotation(int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetObjectSyncRotation(%d)", objectid);
  native = sampgdk_native_find_flexible("GetObjectSyncRotation", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetObjectType(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetObjectType(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("GetObjectType", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerAttachedObject(int playerid, int index, int * modelid, int * bone, float * offsetX, float * offsetY, float * offsetZ, float * rotationX, float * rotationY, float * rotationZ, float * scaleX, float * scaleY, float * scaleZ, int * materialColour1, int * materialColour2)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[16];
  cell modelid_;
  cell bone_;
  cell offsetX_;
  cell offsetY_;
  cell offsetZ_;
  cell rotationX_;
  cell rotationY_;
  cell rotationZ_;
  cell scaleX_;
  cell scaleY_;
  cell scaleZ_;
  cell materialColour1_;
  cell materialColour2_;
  sampgdk_log_debug("GetPlayerAttachedObject(%d, %d, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p)", playerid, index, modelid, bone, offsetX, offsetY, offsetZ, rotationX, rotationY, rotationZ, scaleX, scaleY, scaleZ, materialColour1, materialColour2);
  native = sampgdk_native_find_flexible("GetPlayerAttachedObject", native);
  sampgdk_fakeamx_push(1, &modelid_);
  sampgdk_fakeamx_push(1, &bone_);
  sampgdk_fakeamx_push(1, &offsetX_);
  sampgdk_fakeamx_push(1, &offsetY_);
  sampgdk_fakeamx_push(1, &offsetZ_);
  sampgdk_fakeamx_push(1, &rotationX_);
  sampgdk_fakeamx_push(1, &rotationY_);
  sampgdk_fakeamx_push(1, &rotationZ_);
  sampgdk_fakeamx_push(1, &scaleX_);
  sampgdk_fakeamx_push(1, &scaleY_);
  sampgdk_fakeamx_push(1, &scaleZ_);
  sampgdk_fakeamx_push(1, &materialColour1_);
  sampgdk_fakeamx_push(1, &materialColour2_);
  params[0] = 15 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)index;
  params[3] = modelid_;
  params[4] = bone_;
  params[5] = offsetX_;
  params[6] = offsetY_;
  params[7] = offsetZ_;
  params[8] = rotationX_;
  params[9] = rotationY_;
  params[10] = rotationZ_;
  params[11] = scaleX_;
  params[12] = scaleY_;
  params[13] = scaleZ_;
  params[14] = materialColour1_;
  params[15] = materialColour2_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(modelid_, modelid);
  sampgdk_fakeamx_get_cell(bone_, bone);
  sampgdk_fakeamx_get_float(offsetX_, offsetX);
  sampgdk_fakeamx_get_float(offsetY_, offsetY);
  sampgdk_fakeamx_get_float(offsetZ_, offsetZ);
  sampgdk_fakeamx_get_float(rotationX_, rotationX);
  sampgdk_fakeamx_get_float(rotationY_, rotationY);
  sampgdk_fakeamx_get_float(rotationZ_, rotationZ);
  sampgdk_fakeamx_get_float(scaleX_, scaleX);
  sampgdk_fakeamx_get_float(scaleY_, scaleY);
  sampgdk_fakeamx_get_float(scaleZ_, scaleZ);
  sampgdk_fakeamx_get_cell(materialColour1_, materialColour1);
  sampgdk_fakeamx_get_cell(materialColour2_, materialColour2);
  sampgdk_fakeamx_pop(materialColour2_);
  sampgdk_fakeamx_pop(materialColour1_);
  sampgdk_fakeamx_pop(scaleZ_);
  sampgdk_fakeamx_pop(scaleY_);
  sampgdk_fakeamx_pop(scaleX_);
  sampgdk_fakeamx_pop(rotationZ_);
  sampgdk_fakeamx_pop(rotationY_);
  sampgdk_fakeamx_pop(rotationX_);
  sampgdk_fakeamx_pop(offsetZ_);
  sampgdk_fakeamx_pop(offsetY_);
  sampgdk_fakeamx_pop(offsetX_);
  sampgdk_fakeamx_pop(bone_);
  sampgdk_fakeamx_pop(modelid_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerCameraTargetPlayerObj(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerCameraTargetPlayerObj(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerCameraTargetPlayerObj", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerCameraTargetPlayerObject(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerCameraTargetPlayerObject(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerCameraTargetPlayerObject", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerObjectAttachedData(int playerid, int objectid, int * parentVehicle, int * parentObject, int * parentPlayer)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell parentVehicle_;
  cell parentObject_;
  cell parentPlayer_;
  sampgdk_log_debug("GetPlayerObjectAttachedData(%d, %d, @%p, @%p, @%p)", playerid, objectid, parentVehicle, parentObject, parentPlayer);
  native = sampgdk_native_find_flexible("GetPlayerObjectAttachedData", native);
  sampgdk_fakeamx_push(1, &parentVehicle_);
  sampgdk_fakeamx_push(1, &parentObject_);
  sampgdk_fakeamx_push(1, &parentPlayer_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = parentVehicle_;
  params[4] = parentObject_;
  params[5] = parentPlayer_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(parentVehicle_, parentVehicle);
  sampgdk_fakeamx_get_cell(parentObject_, parentObject);
  sampgdk_fakeamx_get_cell(parentPlayer_, parentPlayer);
  sampgdk_fakeamx_pop(parentPlayer_);
  sampgdk_fakeamx_pop(parentObject_);
  sampgdk_fakeamx_pop(parentVehicle_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerObjectAttachedOffset(int playerid, int objectid, float * offsetX, float * offsetY, float * offsetZ, float * rotationX, float * rotationY, float * rotationZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[9];
  cell offsetX_;
  cell offsetY_;
  cell offsetZ_;
  cell rotationX_;
  cell rotationY_;
  cell rotationZ_;
  sampgdk_log_debug("GetPlayerObjectAttachedOffset(%d, %d, @%p, @%p, @%p, @%p, @%p, @%p)", playerid, objectid, offsetX, offsetY, offsetZ, rotationX, rotationY, rotationZ);
  native = sampgdk_native_find_flexible("GetPlayerObjectAttachedOffset", native);
  sampgdk_fakeamx_push(1, &offsetX_);
  sampgdk_fakeamx_push(1, &offsetY_);
  sampgdk_fakeamx_push(1, &offsetZ_);
  sampgdk_fakeamx_push(1, &rotationX_);
  sampgdk_fakeamx_push(1, &rotationY_);
  sampgdk_fakeamx_push(1, &rotationZ_);
  params[0] = 8 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = offsetX_;
  params[4] = offsetY_;
  params[5] = offsetZ_;
  params[6] = rotationX_;
  params[7] = rotationY_;
  params[8] = rotationZ_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(offsetX_, offsetX);
  sampgdk_fakeamx_get_float(offsetY_, offsetY);
  sampgdk_fakeamx_get_float(offsetZ_, offsetZ);
  sampgdk_fakeamx_get_float(rotationX_, rotationX);
  sampgdk_fakeamx_get_float(rotationY_, rotationY);
  sampgdk_fakeamx_get_float(rotationZ_, rotationZ);
  sampgdk_fakeamx_pop(rotationZ_);
  sampgdk_fakeamx_pop(rotationY_);
  sampgdk_fakeamx_pop(rotationX_);
  sampgdk_fakeamx_pop(offsetZ_);
  sampgdk_fakeamx_pop(offsetY_);
  sampgdk_fakeamx_pop(offsetX_);
  return !!(retval);
}

SAMPGDK_NATIVE(float, GetPlayerObjectDrawDistance(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayerObjectDrawDistance(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("GetPlayerObjectDrawDistance", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerObjectMaterial(int playerid, int objectid, int materialIndex, int * modelid, char * textureLibrary, int textureLibrarySize, char * textureName, int textureNameSize, int * materialColour)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[10];
  cell modelid_;
  cell textureLibrary_;
  cell textureName_;
  cell materialColour_;
  sampgdk_log_debug("GetPlayerObjectMaterial(%d, %d, %d, @%p, @%p, %d, @%p, %d, @%p)", playerid, objectid, materialIndex, modelid, textureLibrary, textureLibrarySize, textureName, textureNameSize, materialColour);
  native = sampgdk_native_find_flexible("GetPlayerObjectMaterial", native);
  sampgdk_fakeamx_push(1, &modelid_);
  sampgdk_fakeamx_push(textureLibrarySize, &textureLibrary_);
  sampgdk_fakeamx_push(textureNameSize, &textureName_);
  sampgdk_fakeamx_push(1, &materialColour_);
  params[0] = 9 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = (cell)materialIndex;
  params[4] = modelid_;
  params[5] = textureLibrary_;
  params[6] = (cell)textureLibrarySize;
  params[7] = textureName_;
  params[8] = (cell)textureNameSize;
  params[9] = materialColour_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(modelid_, modelid);
  sampgdk_fakeamx_get_string(textureLibrary_, textureLibrary, textureLibrarySize);
  sampgdk_fakeamx_get_string(textureName_, textureName, textureNameSize);
  sampgdk_fakeamx_get_cell(materialColour_, materialColour);
  sampgdk_fakeamx_pop(materialColour_);
  sampgdk_fakeamx_pop(textureName_);
  sampgdk_fakeamx_pop(textureLibrary_);
  sampgdk_fakeamx_pop(modelid_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerObjectMaterialText(int playerid, int objectid, int materialIndex, char * text, int textSize, int * materialSize, char * fontFace, int fontFaceSize, int * fontSize, bool * bold, int * fontColour, int * backgroundColour, int * textAlignment)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[14];
  cell text_;
  cell materialSize_;
  cell fontFace_;
  cell fontSize_;
  cell bold_;
  cell fontColour_;
  cell backgroundColour_;
  cell textAlignment_;
  sampgdk_log_debug("GetPlayerObjectMaterialText(%d, %d, %d, @%p, %d, @%p, @%p, %d, @%p, @%p, @%p, @%p, @%p)", playerid, objectid, materialIndex, text, textSize, materialSize, fontFace, fontFaceSize, fontSize, bold, fontColour, backgroundColour, textAlignment);
  native = sampgdk_native_find_flexible("GetPlayerObjectMaterialText", native);
  sampgdk_fakeamx_push(textSize, &text_);
  sampgdk_fakeamx_push(1, &materialSize_);
  sampgdk_fakeamx_push(fontFaceSize, &fontFace_);
  sampgdk_fakeamx_push(1, &fontSize_);
  sampgdk_fakeamx_push(1, &bold_);
  sampgdk_fakeamx_push(1, &fontColour_);
  sampgdk_fakeamx_push(1, &backgroundColour_);
  sampgdk_fakeamx_push(1, &textAlignment_);
  params[0] = 13 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = (cell)materialIndex;
  params[4] = text_;
  params[5] = (cell)textSize;
  params[6] = materialSize_;
  params[7] = fontFace_;
  params[8] = (cell)fontFaceSize;
  params[9] = fontSize_;
  params[10] = bold_;
  params[11] = fontColour_;
  params[12] = backgroundColour_;
  params[13] = textAlignment_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(text_, text, textSize);
  sampgdk_fakeamx_get_cell(materialSize_, materialSize);
  sampgdk_fakeamx_get_string(fontFace_, fontFace, fontFaceSize);
  sampgdk_fakeamx_get_cell(fontSize_, fontSize);
  sampgdk_fakeamx_get_bool(bold_, bold);
  sampgdk_fakeamx_get_cell(fontColour_, fontColour);
  sampgdk_fakeamx_get_cell(backgroundColour_, backgroundColour);
  sampgdk_fakeamx_get_cell(textAlignment_, textAlignment);
  sampgdk_fakeamx_pop(textAlignment_);
  sampgdk_fakeamx_pop(backgroundColour_);
  sampgdk_fakeamx_pop(fontColour_);
  sampgdk_fakeamx_pop(bold_);
  sampgdk_fakeamx_pop(fontSize_);
  sampgdk_fakeamx_pop(fontFace_);
  sampgdk_fakeamx_pop(materialSize_);
  sampgdk_fakeamx_pop(text_);
  return !!(retval);
}

SAMPGDK_NATIVE(float, GetPlayerObjectMoveSpeed(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayerObjectMoveSpeed(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("GetPlayerObjectMoveSpeed", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerObjectMovingTargetPos(int playerid, int objectid, float * targetX, float * targetY, float * targetZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell targetX_;
  cell targetY_;
  cell targetZ_;
  sampgdk_log_debug("GetPlayerObjectMovingTargetPos(%d, %d, @%p, @%p, @%p)", playerid, objectid, targetX, targetY, targetZ);
  native = sampgdk_native_find_flexible("GetPlayerObjectMovingTargetPos", native);
  sampgdk_fakeamx_push(1, &targetX_);
  sampgdk_fakeamx_push(1, &targetY_);
  sampgdk_fakeamx_push(1, &targetZ_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = targetX_;
  params[4] = targetY_;
  params[5] = targetZ_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(targetX_, targetX);
  sampgdk_fakeamx_get_float(targetY_, targetY);
  sampgdk_fakeamx_get_float(targetZ_, targetZ);
  sampgdk_fakeamx_pop(targetZ_);
  sampgdk_fakeamx_pop(targetY_);
  sampgdk_fakeamx_pop(targetX_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerObjectMovingTargetRot(int playerid, int objectid, float * rotationX, float * rotationY, float * rotationZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell rotationX_;
  cell rotationY_;
  cell rotationZ_;
  sampgdk_log_debug("GetPlayerObjectMovingTargetRot(%d, %d, @%p, @%p, @%p)", playerid, objectid, rotationX, rotationY, rotationZ);
  native = sampgdk_native_find_flexible("GetPlayerObjectMovingTargetRot", native);
  sampgdk_fakeamx_push(1, &rotationX_);
  sampgdk_fakeamx_push(1, &rotationY_);
  sampgdk_fakeamx_push(1, &rotationZ_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = rotationX_;
  params[4] = rotationY_;
  params[5] = rotationZ_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(rotationX_, rotationX);
  sampgdk_fakeamx_get_float(rotationY_, rotationY);
  sampgdk_fakeamx_get_float(rotationZ_, rotationZ);
  sampgdk_fakeamx_pop(rotationZ_);
  sampgdk_fakeamx_pop(rotationY_);
  sampgdk_fakeamx_pop(rotationX_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerObjectSyncRotation(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayerObjectSyncRotation(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("GetPlayerObjectSyncRotation", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerSurfingPlayerObjectID(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerSurfingPlayerObjectID(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerSurfingPlayerObjectID", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, IsObjectMaterialSlotUsed(int objectid, int materialIndex)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsObjectMaterialSlotUsed(%d, %d)", objectid, materialIndex);
  native = sampgdk_native_find_flexible("IsObjectMaterialSlotUsed", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = (cell)materialIndex;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, IsObjectNoCameraCol(int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsObjectNoCameraCol(%d)", objectid);
  native = sampgdk_native_find_flexible("IsObjectNoCameraCol", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, IsPlayerObjectMaterialSlotUsed(int playerid, int objectid, int materialIndex)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("IsPlayerObjectMaterialSlotUsed(%d, %d, %d)", playerid, objectid, materialIndex);
  native = sampgdk_native_find_flexible("IsPlayerObjectMaterialSlotUsed", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = (cell)materialIndex;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerObjectNoCameraCol(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPlayerObjectNoCameraCol(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("IsPlayerObjectNoCameraCol", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidCustomModel(int modelid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsValidCustomModel(%d)", modelid);
  native = sampgdk_native_find_flexible("IsValidCustomModel", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)modelid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetObjectMoveSpeed(int objectid, float speed)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetObjectMoveSpeed(%d, %f)", objectid, speed);
  native = sampgdk_native_find_flexible("SetObjectMoveSpeed", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)objectid;
  params[2] = amx_ftoc(speed);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetObjectsDefaultCameraCollision(bool disable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("SetObjectsDefaultCameraCollision(%d)", disable);
  native = sampgdk_native_find_flexible("SetObjectsDefaultCameraCollision", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)disable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerObjectMoveSpeed(int playerid, int objectid, float speed)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("SetPlayerObjectMoveSpeed(%d, %d, %f)", playerid, objectid, speed);
  native = sampgdk_native_find_flexible("SetPlayerObjectMoveSpeed", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  params[3] = amx_ftoc(speed);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerObjectNoCameraCollision(int playerid, int objectid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerObjectNoCameraCollision(%d, %d)", playerid, objectid);
  native = sampgdk_native_find_flexible("SetPlayerObjectNoCameraCollision", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)objectid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_object) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_object) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(int, CreatePlayerPickup(int playerid, int model, int type, float x, float y, float z, int virtualWorld)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[8];
  sampgdk_log_debug("CreatePlayerPickup(%d, %d, %d, %f, %f, %f, %d)", playerid, model, type, x, y, z, virtualWorld);
  native = sampgdk_native_find_flexible("CreatePlayerPickup", native);
  params[0] = 7 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)model;
  params[3] = (cell)type;
  params[4] = amx_ftoc(x);
  params[5] = amx_ftoc(y);
  params[6] = amx_ftoc(z);
  params[7] = (cell)virtualWorld;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, DestroyPlayerPickup(int playerid, int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("DestroyPlayerPickup(%d, %d)", playerid, pickupid);
  native = sampgdk_native_find_flexible("DestroyPlayerPickup", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPickupModel(int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPickupModel(%d)", pickupid);
  native = sampgdk_native_find_flexible("GetPickupModel", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPickupPos(int pickupid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetPickupPos(%d, @%p, @%p, @%p)", pickupid, x, y, z);
  native = sampgdk_native_find_flexible("GetPickupPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)pickupid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPickupType(int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPickupType(%d)", pickupid);
  native = sampgdk_native_find_flexible("GetPickupType", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPickupVirtualWorld(int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPickupVirtualWorld(%d)", pickupid);
  native = sampgdk_native_find_flexible("GetPickupVirtualWorld", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerPickupModel(int playerid, int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayerPickupModel(%d, %d)", playerid, pickupid);
  native = sampgdk_native_find_flexible("GetPlayerPickupModel", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerPickupPos(int playerid, int pickupid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetPlayerPickupPos(%d, %d, @%p, @%p, @%p)", playerid, pickupid, x, y, z);
  native = sampgdk_native_find_flexible("GetPlayerPickupPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  params[3] = x_;
  params[4] = y_;
  params[5] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerPickupType(int playerid, int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayerPickupType(%d, %d)", playerid, pickupid);
  native = sampgdk_native_find_flexible("GetPlayerPickupType", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerPickupVirtualWorld(int playerid, int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayerPickupVirtualWorld(%d, %d)", playerid, pickupid);
  native = sampgdk_native_find_flexible("GetPlayerPickupVirtualWorld", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, HidePickupForPlayer(int playerid, int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("HidePickupForPlayer(%d, %d)", playerid, pickupid);
  native = sampgdk_native_find_flexible("HidePickupForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPickupHiddenForPlayer(int playerid, int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPickupHiddenForPlayer(%d, %d)", playerid, pickupid);
  native = sampgdk_native_find_flexible("IsPickupHiddenForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPickupStreamedIn(int playerid, int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPickupStreamedIn(%d, %d)", playerid, pickupid);
  native = sampgdk_native_find_flexible("IsPickupStreamedIn", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerPickupStreamedIn(int playerid, int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPlayerPickupStreamedIn(%d, %d)", playerid, pickupid);
  native = sampgdk_native_find_flexible("IsPlayerPickupStreamedIn", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidPickup(int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsValidPickup(%d)", pickupid);
  native = sampgdk_native_find_flexible("IsValidPickup", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidPlayerPickup(int playerid, int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsValidPlayerPickup(%d, %d)", playerid, pickupid);
  native = sampgdk_native_find_flexible("IsValidPlayerPickup", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPickupModel(int pickupid, int model, bool update)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("SetPickupModel(%d, %d, %d)", pickupid, model, update);
  native = sampgdk_native_find_flexible("SetPickupModel", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)pickupid;
  params[2] = (cell)model;
  params[3] = (cell)update;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPickupPos(int pickupid, float x, float y, float z, bool update)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  sampgdk_log_debug("SetPickupPos(%d, %f, %f, %f, %d)", pickupid, x, y, z, update);
  native = sampgdk_native_find_flexible("SetPickupPos", native);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)pickupid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  params[4] = amx_ftoc(z);
  params[5] = (cell)update;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPickupType(int pickupid, int type, bool update)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("SetPickupType(%d, %d, %d)", pickupid, type, update);
  native = sampgdk_native_find_flexible("SetPickupType", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)pickupid;
  params[2] = (cell)type;
  params[3] = (cell)update;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPickupVirtualWorld(int pickupid, int virtualWorld)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPickupVirtualWorld(%d, %d)", pickupid, virtualWorld);
  native = sampgdk_native_find_flexible("SetPickupVirtualWorld", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)pickupid;
  params[2] = (cell)virtualWorld;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerPickupModel(int playerid, int pickupid, int model, bool update)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetPlayerPickupModel(%d, %d, %d, %d)", playerid, pickupid, model, update);
  native = sampgdk_native_find_flexible("SetPlayerPickupModel", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  params[3] = (cell)model;
  params[4] = (cell)update;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerPickupPos(int playerid, int pickupid, float x, float y, float z, bool update)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  sampgdk_log_debug("SetPlayerPickupPos(%d, %d, %f, %f, %f, %d)", playerid, pickupid, x, y, z, update);
  native = sampgdk_native_find_flexible("SetPlayerPickupPos", native);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  params[5] = amx_ftoc(z);
  params[6] = (cell)update;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerPickupType(int playerid, int pickupid, int type, bool update)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("SetPlayerPickupType(%d, %d, %d, %d)", playerid, pickupid, type, update);
  native = sampgdk_native_find_flexible("SetPlayerPickupType", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  params[3] = (cell)type;
  params[4] = (cell)update;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerPickupVirtualWorld(int playerid, int pickupid, int virtualWorld)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("SetPlayerPickupVirtualWorld(%d, %d, %d)", playerid, pickupid, virtualWorld);
  native = sampgdk_native_find_flexible("SetPlayerPickupVirtualWorld", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  params[3] = (cell)virtualWorld;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ShowPickupForPlayer(int playerid, int pickupid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("ShowPickupForPlayer(%d, %d)", playerid, pickupid);
  native = sampgdk_native_find_flexible("ShowPickupForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)pickupid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_pickup) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_pickup) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, AllowPlayerWeapons(int playerid, bool allow)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("AllowPlayerWeapons(%d, %d)", playerid, allow);
  native = sampgdk_native_find_flexible("AllowPlayerWeapons", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)allow;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ArePlayerWeaponsAllowed(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("ArePlayerWeaponsAllowed(%d)", playerid);
  native = sampgdk_native_find_flexible("ArePlayerWeaponsAllowed", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ClearPlayerWorldBounds(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("ClearPlayerWorldBounds(%d)", playerid);
  native = sampgdk_native_find_flexible("ClearPlayerWorldBounds", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetDefaultPlayerColour(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetDefaultPlayerColour(%d)", playerid);
  native = sampgdk_native_find_flexible("GetDefaultPlayerColour", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetGameText(int playerid, int style, char * message, int len, int time, int remaining)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  cell message_;
  sampgdk_log_debug("GetGameText(%d, %d, @%p, %d, %d, %d)", playerid, style, message, len, time, remaining);
  native = sampgdk_native_find_flexible("GetGameText", native);
  sampgdk_fakeamx_push(len, &message_);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)style;
  params[3] = message_;
  params[4] = (cell)len;
  params[5] = (cell)time;
  params[6] = (cell)remaining;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(message_, message, len);
  sampgdk_fakeamx_pop(message_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerAnimFlags(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerAnimFlags(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerAnimFlags", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerBuildingsRemoved(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerBuildingsRemoved(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerBuildingsRemoved", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerCustomSkin(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerCustomSkin(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerCustomSkin", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerGhostMode(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerGhostMode(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerGhostMode", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(float, GetPlayerGravity(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerGravity(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerGravity", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, GetPlayerMarkerForPlayer(int playerid, int targetid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayerMarkerForPlayer(%d, %d)", playerid, targetid);
  native = sampgdk_native_find_flexible("GetPlayerMarkerForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)targetid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerRotationQuat(int playerid, float * w, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell w_;
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetPlayerRotationQuat(%d, @%p, @%p, @%p, @%p)", playerid, w, x, y, z);
  native = sampgdk_native_find_flexible("GetPlayerRotationQuat", native);
  sampgdk_fakeamx_push(1, &w_);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = w_;
  params[3] = x_;
  params[4] = y_;
  params[5] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(w_, w);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  sampgdk_fakeamx_pop(w_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerSkillLevel(int playerid, int skill)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayerSkillLevel(%d, %d)", playerid, skill);
  native = sampgdk_native_find_flexible("GetPlayerSkillLevel", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)skill;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerSpectateID(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerSpectateID(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerSpectateID", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerSpectateType(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerSpectateType(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerSpectateType", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerSurfingOffsets(int playerid, float * offsetX, float * offsetY, float * offsetZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell offsetX_;
  cell offsetY_;
  cell offsetZ_;
  sampgdk_log_debug("GetPlayerSurfingOffsets(%d, @%p, @%p, @%p)", playerid, offsetX, offsetY, offsetZ);
  native = sampgdk_native_find_flexible("GetPlayerSurfingOffsets", native);
  sampgdk_fakeamx_push(1, &offsetX_);
  sampgdk_fakeamx_push(1, &offsetY_);
  sampgdk_fakeamx_push(1, &offsetZ_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = offsetX_;
  params[3] = offsetY_;
  params[4] = offsetZ_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(offsetX_, offsetX);
  sampgdk_fakeamx_get_float(offsetY_, offsetY);
  sampgdk_fakeamx_get_float(offsetZ_, offsetZ);
  sampgdk_fakeamx_pop(offsetZ_);
  sampgdk_fakeamx_pop(offsetY_);
  sampgdk_fakeamx_pop(offsetX_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayerWeather(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerWeather(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerWeather", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerWorldBounds(int playerid, float * maxX, float * minX, float * maxY, float * minY)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell maxX_;
  cell minX_;
  cell maxY_;
  cell minY_;
  sampgdk_log_debug("GetPlayerWorldBounds(%d, @%p, @%p, @%p, @%p)", playerid, maxX, minX, maxY, minY);
  native = sampgdk_native_find_flexible("GetPlayerWorldBounds", native);
  sampgdk_fakeamx_push(1, &maxX_);
  sampgdk_fakeamx_push(1, &minX_);
  sampgdk_fakeamx_push(1, &maxY_);
  sampgdk_fakeamx_push(1, &minY_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = maxX_;
  params[3] = minX_;
  params[4] = maxY_;
  params[5] = minY_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(maxX_, maxX);
  sampgdk_fakeamx_get_float(minX_, minX);
  sampgdk_fakeamx_get_float(maxY_, maxY);
  sampgdk_fakeamx_get_float(minY_, minY);
  sampgdk_fakeamx_pop(minY_);
  sampgdk_fakeamx_pop(maxY_);
  sampgdk_fakeamx_pop(minX_);
  sampgdk_fakeamx_pop(maxX_);
  return !!(retval);
}

SAMPGDK_NATIVE(float, GetPlayerZAim(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerZAim(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerZAim", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, GetPlayers(char * players, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell players_;
  sampgdk_log_debug("GetPlayers(@%p, %d)", players, size);
  native = sampgdk_native_find_flexible("GetPlayers", native);
  sampgdk_fakeamx_push(size, &players_);
  params[0] = 2 * sizeof(cell);
  params[1] = players_;
  params[2] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(players_, players, size);
  sampgdk_fakeamx_pop(players_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, HasGameText(int playerid, int style)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("HasGameText(%d, %d)", playerid, style);
  native = sampgdk_native_find_flexible("HasGameText", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)style;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, HideGameTextForPlayer(int playerid, int style)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("HideGameTextForPlayer(%d, %d)", playerid, style);
  native = sampgdk_native_find_flexible("HideGameTextForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)style;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerCameraTargetEnabled(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerCameraTargetEnabled(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerCameraTargetEnabled", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerControllable(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerControllable(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerControllable", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerCuffed(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerCuffed(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerCuffed", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerInDriveByMode(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerInDriveByMode(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerInDriveByMode", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerSpawned(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerSpawned(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerSpawned", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerTeleportAllowed(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerTeleportAllowed(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerTeleportAllowed", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerUsingOfficialClient(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerUsingOfficialClient(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerUsingOfficialClient", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerUsingOmp(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerUsingOmp(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerUsingOmp", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerWidescreenToggled(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerWidescreenToggled(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerWidescreenToggled", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerHasClockEnabled(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("PlayerHasClockEnabled(%d)", playerid);
  native = sampgdk_native_find_flexible("PlayerHasClockEnabled", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, RemovePlayerWeapon(int playerid, int weaponid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("RemovePlayerWeapon(%d, %d)", playerid, weaponid);
  native = sampgdk_native_find_flexible("RemovePlayerWeapon", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)weaponid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayerGravity(int playerid, float gravity)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetPlayerGravity(%d, %f)", playerid, gravity);
  native = sampgdk_native_find_flexible("SetPlayerGravity", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = amx_ftoc(gravity);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TogglePlayerGhostMode(int playerid, bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TogglePlayerGhostMode(%d, %d)", playerid, enable);
  native = sampgdk_native_find_flexible("TogglePlayerGhostMode", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TogglePlayerWidescreen(int playerid, bool wide)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("TogglePlayerWidescreen(%d, %d)", playerid, wide);
  native = sampgdk_native_find_flexible("TogglePlayerWidescreen", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)wide;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_player) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_player) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, IsPlayerTextDrawVisible(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsPlayerTextDrawVisible(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("IsPlayerTextDrawVisible", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsTextDrawVisibleForPlayer(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsTextDrawVisibleForPlayer(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("IsTextDrawVisibleForPlayer", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidPlayerTextDraw(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsValidPlayerTextDraw(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("IsValidPlayerTextDraw", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidTextDraw(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsValidTextDraw(%d)", textid);
  native = sampgdk_native_find_flexible("IsValidTextDraw", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, PlayerTextDrawGetAlignment(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawGetAlignment(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetAlignment", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, PlayerTextDrawGetBackgroundCol(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawGetBackgroundCol(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetBackgroundCol", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, PlayerTextDrawGetBoxColor(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawGetBoxColor(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetBoxColor", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, PlayerTextDrawGetColor(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawGetColor(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetColor", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, PlayerTextDrawGetFont(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawGetFont(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetFont", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawGetLetterSize(int playerid, int textid, float * width, float * height)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell width_;
  cell height_;
  sampgdk_log_debug("PlayerTextDrawGetLetterSize(%d, %d, @%p, @%p)", playerid, textid, width, height);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetLetterSize", native);
  sampgdk_fakeamx_push(1, &width_);
  sampgdk_fakeamx_push(1, &height_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  params[3] = width_;
  params[4] = height_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(width_, width);
  sampgdk_fakeamx_get_float(height_, height);
  sampgdk_fakeamx_pop(height_);
  sampgdk_fakeamx_pop(width_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, PlayerTextDrawGetOutline(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawGetOutline(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetOutline", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawGetPos(int playerid, int textid, float * x, float * y)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  sampgdk_log_debug("PlayerTextDrawGetPos(%d, %d, @%p, @%p)", playerid, textid, x, y);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  params[3] = x_;
  params[4] = y_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, PlayerTextDrawGetPreviewModel(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawGetPreviewModel(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetPreviewModel", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawGetPreviewRot(int playerid, int textid, float * rotationX, float * rotationY, float * rotationZ, float * zoom)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[7];
  cell rotationX_;
  cell rotationY_;
  cell rotationZ_;
  cell zoom_;
  sampgdk_log_debug("PlayerTextDrawGetPreviewRot(%d, %d, @%p, @%p, @%p, @%p)", playerid, textid, rotationX, rotationY, rotationZ, zoom);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetPreviewRot", native);
  sampgdk_fakeamx_push(1, &rotationX_);
  sampgdk_fakeamx_push(1, &rotationY_);
  sampgdk_fakeamx_push(1, &rotationZ_);
  sampgdk_fakeamx_push(1, &zoom_);
  params[0] = 6 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  params[3] = rotationX_;
  params[4] = rotationY_;
  params[5] = rotationZ_;
  params[6] = zoom_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(rotationX_, rotationX);
  sampgdk_fakeamx_get_float(rotationY_, rotationY);
  sampgdk_fakeamx_get_float(rotationZ_, rotationZ);
  sampgdk_fakeamx_get_float(zoom_, zoom);
  sampgdk_fakeamx_pop(zoom_);
  sampgdk_fakeamx_pop(rotationZ_);
  sampgdk_fakeamx_pop(rotationY_);
  sampgdk_fakeamx_pop(rotationX_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawGetPreviewVehCol(int playerid, int textid, int * colour1, int * colour2)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell colour1_;
  cell colour2_;
  sampgdk_log_debug("PlayerTextDrawGetPreviewVehCol(%d, %d, @%p, @%p)", playerid, textid, colour1, colour2);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetPreviewVehCol", native);
  sampgdk_fakeamx_push(1, &colour1_);
  sampgdk_fakeamx_push(1, &colour2_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  params[3] = colour1_;
  params[4] = colour2_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(colour1_, colour1);
  sampgdk_fakeamx_get_cell(colour2_, colour2);
  sampgdk_fakeamx_pop(colour2_);
  sampgdk_fakeamx_pop(colour1_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, PlayerTextDrawGetShadow(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawGetShadow(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetShadow", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawGetString(int playerid, int textid, char * string_, int stringSize)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell string__;
  sampgdk_log_debug("PlayerTextDrawGetString(%d, %d, @%p, %d)", playerid, textid, string_, stringSize);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetString", native);
  sampgdk_fakeamx_push(stringSize, &string__);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  params[3] = string__;
  params[4] = (cell)stringSize;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(string__, string_, stringSize);
  sampgdk_fakeamx_pop(string__);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawGetTextSize(int playerid, int textid, float * width, float * height)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell width_;
  cell height_;
  sampgdk_log_debug("PlayerTextDrawGetTextSize(%d, %d, @%p, @%p)", playerid, textid, width, height);
  native = sampgdk_native_find_flexible("PlayerTextDrawGetTextSize", native);
  sampgdk_fakeamx_push(1, &width_);
  sampgdk_fakeamx_push(1, &height_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  params[3] = width_;
  params[4] = height_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(width_, width);
  sampgdk_fakeamx_get_float(height_, height);
  sampgdk_fakeamx_pop(height_);
  sampgdk_fakeamx_pop(width_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawIsBox(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawIsBox(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("PlayerTextDrawIsBox", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawIsProportional(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawIsProportional(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("PlayerTextDrawIsProportional", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawIsSelectable(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("PlayerTextDrawIsSelectable(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("PlayerTextDrawIsSelectable", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, PlayerTextDrawSetPos(int playerid, int textid, float x, float y)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  sampgdk_log_debug("PlayerTextDrawSetPos(%d, %d, %f, %f)", playerid, textid, x, y);
  native = sampgdk_native_find_flexible("PlayerTextDrawSetPos", native);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  params[3] = amx_ftoc(x);
  params[4] = amx_ftoc(y);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(int, TextDrawGetAlignment(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawGetAlignment(%d)", textid);
  native = sampgdk_native_find_flexible("TextDrawGetAlignment", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, TextDrawGetBackgroundColor(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawGetBackgroundColor(%d)", textid);
  native = sampgdk_native_find_flexible("TextDrawGetBackgroundColor", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, TextDrawGetBoxColor(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawGetBoxColor(%d)", textid);
  native = sampgdk_native_find_flexible("TextDrawGetBoxColor", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, TextDrawGetColor(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawGetColor(%d)", textid);
  native = sampgdk_native_find_flexible("TextDrawGetColor", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, TextDrawGetFont(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawGetFont(%d)", textid);
  native = sampgdk_native_find_flexible("TextDrawGetFont", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, TextDrawGetLetterSize(int textid, float * width, float * height)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell width_;
  cell height_;
  sampgdk_log_debug("TextDrawGetLetterSize(%d, @%p, @%p)", textid, width, height);
  native = sampgdk_native_find_flexible("TextDrawGetLetterSize", native);
  sampgdk_fakeamx_push(1, &width_);
  sampgdk_fakeamx_push(1, &height_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = width_;
  params[3] = height_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(width_, width);
  sampgdk_fakeamx_get_float(height_, height);
  sampgdk_fakeamx_pop(height_);
  sampgdk_fakeamx_pop(width_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, TextDrawGetOutline(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawGetOutline(%d)", textid);
  native = sampgdk_native_find_flexible("TextDrawGetOutline", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, TextDrawGetPos(int textid, float * x, float * y)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell x_;
  cell y_;
  sampgdk_log_debug("TextDrawGetPos(%d, @%p, @%p)", textid, x, y);
  native = sampgdk_native_find_flexible("TextDrawGetPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = x_;
  params[3] = y_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, TextDrawGetPreviewModel(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawGetPreviewModel(%d)", textid);
  native = sampgdk_native_find_flexible("TextDrawGetPreviewModel", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, TextDrawGetPreviewRot(int textid, float * rotationX, float * rotationY, float * rotationZ, float * zoom)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell rotationX_;
  cell rotationY_;
  cell rotationZ_;
  cell zoom_;
  sampgdk_log_debug("TextDrawGetPreviewRot(%d, @%p, @%p, @%p, @%p)", textid, rotationX, rotationY, rotationZ, zoom);
  native = sampgdk_native_find_flexible("TextDrawGetPreviewRot", native);
  sampgdk_fakeamx_push(1, &rotationX_);
  sampgdk_fakeamx_push(1, &rotationY_);
  sampgdk_fakeamx_push(1, &rotationZ_);
  sampgdk_fakeamx_push(1, &zoom_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = rotationX_;
  params[3] = rotationY_;
  params[4] = rotationZ_;
  params[5] = zoom_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(rotationX_, rotationX);
  sampgdk_fakeamx_get_float(rotationY_, rotationY);
  sampgdk_fakeamx_get_float(rotationZ_, rotationZ);
  sampgdk_fakeamx_get_float(zoom_, zoom);
  sampgdk_fakeamx_pop(zoom_);
  sampgdk_fakeamx_pop(rotationZ_);
  sampgdk_fakeamx_pop(rotationY_);
  sampgdk_fakeamx_pop(rotationX_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawGetPreviewVehCol(int textid, int * colour1, int * colour2)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell colour1_;
  cell colour2_;
  sampgdk_log_debug("TextDrawGetPreviewVehCol(%d, @%p, @%p)", textid, colour1, colour2);
  native = sampgdk_native_find_flexible("TextDrawGetPreviewVehCol", native);
  sampgdk_fakeamx_push(1, &colour1_);
  sampgdk_fakeamx_push(1, &colour2_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = colour1_;
  params[3] = colour2_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(colour1_, colour1);
  sampgdk_fakeamx_get_cell(colour2_, colour2);
  sampgdk_fakeamx_pop(colour2_);
  sampgdk_fakeamx_pop(colour1_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, TextDrawGetShadow(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawGetShadow(%d)", textid);
  native = sampgdk_native_find_flexible("TextDrawGetShadow", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, TextDrawGetString(int textid, char * string_, int stringSize)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell string__;
  sampgdk_log_debug("TextDrawGetString(%d, @%p, %d)", textid, string_, stringSize);
  native = sampgdk_native_find_flexible("TextDrawGetString", native);
  sampgdk_fakeamx_push(stringSize, &string__);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = string__;
  params[3] = (cell)stringSize;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(string__, string_, stringSize);
  sampgdk_fakeamx_pop(string__);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawGetTextSize(int textid, float * width, float * height)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell width_;
  cell height_;
  sampgdk_log_debug("TextDrawGetTextSize(%d, @%p, @%p)", textid, width, height);
  native = sampgdk_native_find_flexible("TextDrawGetTextSize", native);
  sampgdk_fakeamx_push(1, &width_);
  sampgdk_fakeamx_push(1, &height_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = width_;
  params[3] = height_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(width_, width);
  sampgdk_fakeamx_get_float(height_, height);
  sampgdk_fakeamx_pop(height_);
  sampgdk_fakeamx_pop(width_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawIsBox(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawIsBox(%d)", textid);
  native = sampgdk_native_find_flexible("TextDrawIsBox", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawIsProportional(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawIsProportional(%d)", textid);
  native = sampgdk_native_find_flexible("TextDrawIsProportional", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawIsSelectable(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("TextDrawIsSelectable(%d)", textid);
  native = sampgdk_native_find_flexible("TextDrawIsSelectable", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawSetPos(int textid, float x, float y)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("TextDrawSetPos(%d, %f, %f)", textid, x, y);
  native = sampgdk_native_find_flexible("TextDrawSetPos", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = amx_ftoc(x);
  params[3] = amx_ftoc(y);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, TextDrawSetStringForPlayer(int textid, int playerid, const char * format)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell format_;
  sampgdk_log_debug("TextDrawSetStringForPlayer(%d, %d, \"%s\")", textid, playerid, format);
  native = sampgdk_native_find_flexible("TextDrawSetStringForPlayer", native);
  sampgdk_fakeamx_push_string(format, NULL, &format_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = (cell)playerid;
  params[3] = format_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_pop(format_);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_textdraw) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_textdraw) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(bool, Get3DTextLabelAttachedData(int textid, int * parentPlayerid, int * parentVehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell parentPlayerid_;
  cell parentVehicleid_;
  sampgdk_log_debug("Get3DTextLabelAttachedData(%d, @%p, @%p)", textid, parentPlayerid, parentVehicleid);
  native = sampgdk_native_find_flexible("Get3DTextLabelAttachedData", native);
  sampgdk_fakeamx_push(1, &parentPlayerid_);
  sampgdk_fakeamx_push(1, &parentVehicleid_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = parentPlayerid_;
  params[3] = parentVehicleid_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(parentPlayerid_, parentPlayerid);
  sampgdk_fakeamx_get_cell(parentVehicleid_, parentVehicleid);
  sampgdk_fakeamx_pop(parentVehicleid_);
  sampgdk_fakeamx_pop(parentPlayerid_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, Get3DTextLabelColor(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("Get3DTextLabelColor(%d)", textid);
  native = sampgdk_native_find_flexible("Get3DTextLabelColor", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, Get3DTextLabelDrawDistance(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("Get3DTextLabelDrawDistance(%d)", textid);
  native = sampgdk_native_find_flexible("Get3DTextLabelDrawDistance", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(bool, Get3DTextLabelLOS(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("Get3DTextLabelLOS(%d)", textid);
  native = sampgdk_native_find_flexible("Get3DTextLabelLOS", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, Get3DTextLabelPos(int textid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("Get3DTextLabelPos(%d, @%p, @%p, @%p)", textid, x, y, z);
  native = sampgdk_native_find_flexible("Get3DTextLabelPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, Get3DTextLabelText(int textid, char * text, int len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell text_;
  sampgdk_log_debug("Get3DTextLabelText(%d, @%p, %d)", textid, text, len);
  native = sampgdk_native_find_flexible("Get3DTextLabelText", native);
  sampgdk_fakeamx_push(len, &text_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = text_;
  params[3] = (cell)len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(text_, text, len);
  sampgdk_fakeamx_pop(text_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, Get3DTextLabelVirtualWorld(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("Get3DTextLabelVirtualWorld(%d)", textid);
  native = sampgdk_native_find_flexible("Get3DTextLabelVirtualWorld", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPlayer3DTextLabelAttached(int playerid, int textid, int * parentPlayerid, int * parentVehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell parentPlayerid_;
  cell parentVehicleid_;
  sampgdk_log_debug("GetPlayer3DTextLabelAttached(%d, %d, @%p, @%p)", playerid, textid, parentPlayerid, parentVehicleid);
  native = sampgdk_native_find_flexible("GetPlayer3DTextLabelAttached", native);
  sampgdk_fakeamx_push(1, &parentPlayerid_);
  sampgdk_fakeamx_push(1, &parentVehicleid_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  params[3] = parentPlayerid_;
  params[4] = parentVehicleid_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(parentPlayerid_, parentPlayerid);
  sampgdk_fakeamx_get_cell(parentVehicleid_, parentVehicleid);
  sampgdk_fakeamx_pop(parentVehicleid_);
  sampgdk_fakeamx_pop(parentPlayerid_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayer3DTextLabelColor(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayer3DTextLabelColor(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("GetPlayer3DTextLabelColor", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, GetPlayer3DTextLabelDrawDist(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayer3DTextLabelDrawDist(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("GetPlayer3DTextLabelDrawDist", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(bool, GetPlayer3DTextLabelLOS(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayer3DTextLabelLOS(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("GetPlayer3DTextLabelLOS", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayer3DTextLabelPos(int playerid, int textid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetPlayer3DTextLabelPos(%d, %d, @%p, @%p, @%p)", playerid, textid, x, y, z);
  native = sampgdk_native_find_flexible("GetPlayer3DTextLabelPos", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  params[3] = x_;
  params[4] = y_;
  params[5] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetPlayer3DTextLabelText(int playerid, int textid, char * text, int len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell text_;
  sampgdk_log_debug("GetPlayer3DTextLabelText(%d, %d, @%p, %d)", playerid, textid, text, len);
  native = sampgdk_native_find_flexible("GetPlayer3DTextLabelText", native);
  sampgdk_fakeamx_push(len, &text_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  params[3] = text_;
  params[4] = (cell)len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(text_, text, len);
  sampgdk_fakeamx_pop(text_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetPlayer3DTextLabelVirtualW(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetPlayer3DTextLabelVirtualW(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("GetPlayer3DTextLabelVirtualW", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, Is3DTextLabelStreamedIn(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("Is3DTextLabelStreamedIn(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("Is3DTextLabelStreamedIn", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValid3DTextLabel(int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsValid3DTextLabel(%d)", textid);
  native = sampgdk_native_find_flexible("IsValid3DTextLabel", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsValidPlayer3DTextLabel(int playerid, int textid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("IsValidPlayer3DTextLabel(%d, %d)", playerid, textid);
  native = sampgdk_native_find_flexible("IsValidPlayer3DTextLabel", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, Set3DTextLabelDrawDistance(int textid, float drawDistance)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("Set3DTextLabelDrawDistance(%d, %f)", textid, drawDistance);
  native = sampgdk_native_find_flexible("Set3DTextLabelDrawDistance", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = amx_ftoc(drawDistance);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, Set3DTextLabelLOS(int textid, bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("Set3DTextLabelLOS(%d, %d)", textid, enable);
  native = sampgdk_native_find_flexible("Set3DTextLabelLOS", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, Set3DTextLabelVirtualWorld(int textid, int virtualWorld)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("Set3DTextLabelVirtualWorld(%d, %d)", textid, virtualWorld);
  native = sampgdk_native_find_flexible("Set3DTextLabelVirtualWorld", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)textid;
  params[2] = (cell)virtualWorld;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayer3DTextLabelDrawDist(int playerid, int textid, float drawDistance)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("SetPlayer3DTextLabelDrawDist(%d, %d, %f)", playerid, textid, drawDistance);
  native = sampgdk_native_find_flexible("SetPlayer3DTextLabelDrawDist", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  params[3] = amx_ftoc(drawDistance);
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetPlayer3DTextLabelLOS(int playerid, int textid, bool enable)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  sampgdk_log_debug("SetPlayer3DTextLabelLOS(%d, %d, %d)", playerid, textid, enable);
  native = sampgdk_native_find_flexible("SetPlayer3DTextLabelLOS", native);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)playerid;
  params[2] = (cell)textid;
  params[3] = (cell)enable;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_textlabel) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_textlabel) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_MODULE_INIT(omp_variable) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_variable) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_NATIVE(int, CarColIndexToColour(int index, int alpha)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("CarColIndexToColour(%d, %d)", index, alpha);
  native = sampgdk_native_find_flexible("CarColIndexToColour", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)index;
  params[2] = (cell)alpha;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, CountVehicleOccupants(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("CountVehicleOccupants(%d)", vehicleid);
  native = sampgdk_native_find_flexible("CountVehicleOccupants", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerHydraReactorAngle(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerHydraReactorAngle(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerHydraReactorAngle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetPlayerLandingGearState(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerLandingGearState(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerLandingGearState", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetPlayerSirenState(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerSirenState(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerSirenState", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(float, GetPlayerTrainSpeed(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetPlayerTrainSpeed(%d)", playerid);
  native = sampgdk_native_find_flexible("GetPlayerTrainSpeed", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(bool, GetRandomCarColPair(int modelid, int * colour1, int * colour2, int * colour3, int * colour4)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[6];
  cell colour1_;
  cell colour2_;
  cell colour3_;
  cell colour4_;
  sampgdk_log_debug("GetRandomCarColPair(%d, @%p, @%p, @%p, @%p)", modelid, colour1, colour2, colour3, colour4);
  native = sampgdk_native_find_flexible("GetRandomCarColPair", native);
  sampgdk_fakeamx_push(1, &colour1_);
  sampgdk_fakeamx_push(1, &colour2_);
  sampgdk_fakeamx_push(1, &colour3_);
  sampgdk_fakeamx_push(1, &colour4_);
  params[0] = 5 * sizeof(cell);
  params[1] = (cell)modelid;
  params[2] = colour1_;
  params[3] = colour2_;
  params[4] = colour3_;
  params[5] = colour4_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(colour1_, colour1);
  sampgdk_fakeamx_get_cell(colour2_, colour2);
  sampgdk_fakeamx_get_cell(colour3_, colour3);
  sampgdk_fakeamx_get_cell(colour4_, colour4);
  sampgdk_fakeamx_pop(colour4_);
  sampgdk_fakeamx_pop(colour3_);
  sampgdk_fakeamx_pop(colour2_);
  sampgdk_fakeamx_pop(colour1_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetVehicleCab(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleCab(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleCab", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleColor(int vehicleid, int * color1, int * color2)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell color1_;
  cell color2_;
  sampgdk_log_debug("GetVehicleColor(%d, @%p, @%p)", vehicleid, color1, color2);
  native = sampgdk_native_find_flexible("GetVehicleColor", native);
  sampgdk_fakeamx_push(1, &color1_);
  sampgdk_fakeamx_push(1, &color2_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = color1_;
  params[3] = color2_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_cell(color1_, color1);
  sampgdk_fakeamx_get_cell(color2_, color2);
  sampgdk_fakeamx_pop(color2_);
  sampgdk_fakeamx_pop(color1_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetVehicleDriver(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleDriver(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleDriver", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetVehicleHydraReactorAngle(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleHydraReactorAngle(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleHydraReactorAngle", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetVehicleInterior(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleInterior(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleInterior", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetVehicleLandingGearState(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleLandingGearState(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleLandingGearState", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetVehicleLastDriver(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleLastDriver(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleLastDriver", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleMatrix(int vehicleid, float * rightX, float * rightY, float * rightZ, float * upX, float * upY, float * upZ, float * atX, float * atY, float * atZ)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[11];
  cell rightX_;
  cell rightY_;
  cell rightZ_;
  cell upX_;
  cell upY_;
  cell upZ_;
  cell atX_;
  cell atY_;
  cell atZ_;
  sampgdk_log_debug("GetVehicleMatrix(%d, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p, @%p)", vehicleid, rightX, rightY, rightZ, upX, upY, upZ, atX, atY, atZ);
  native = sampgdk_native_find_flexible("GetVehicleMatrix", native);
  sampgdk_fakeamx_push(1, &rightX_);
  sampgdk_fakeamx_push(1, &rightY_);
  sampgdk_fakeamx_push(1, &rightZ_);
  sampgdk_fakeamx_push(1, &upX_);
  sampgdk_fakeamx_push(1, &upY_);
  sampgdk_fakeamx_push(1, &upZ_);
  sampgdk_fakeamx_push(1, &atX_);
  sampgdk_fakeamx_push(1, &atY_);
  sampgdk_fakeamx_push(1, &atZ_);
  params[0] = 10 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = rightX_;
  params[3] = rightY_;
  params[4] = rightZ_;
  params[5] = upX_;
  params[6] = upY_;
  params[7] = upZ_;
  params[8] = atX_;
  params[9] = atY_;
  params[10] = atZ_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(rightX_, rightX);
  sampgdk_fakeamx_get_float(rightY_, rightY);
  sampgdk_fakeamx_get_float(rightZ_, rightZ);
  sampgdk_fakeamx_get_float(upX_, upX);
  sampgdk_fakeamx_get_float(upY_, upY);
  sampgdk_fakeamx_get_float(upZ_, upZ);
  sampgdk_fakeamx_get_float(atX_, atX);
  sampgdk_fakeamx_get_float(atY_, atY);
  sampgdk_fakeamx_get_float(atZ_, atZ);
  sampgdk_fakeamx_pop(atZ_);
  sampgdk_fakeamx_pop(atY_);
  sampgdk_fakeamx_pop(atX_);
  sampgdk_fakeamx_pop(upZ_);
  sampgdk_fakeamx_pop(upY_);
  sampgdk_fakeamx_pop(upX_);
  sampgdk_fakeamx_pop(rightZ_);
  sampgdk_fakeamx_pop(rightY_);
  sampgdk_fakeamx_pop(rightX_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetVehicleModelCount(int modelid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleModelCount(%d)", modelid);
  native = sampgdk_native_find_flexible("GetVehicleModelCount", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)modelid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetVehicleModelsUsed()) {
  static AMX_NATIVE native;
  cell retval;
  sampgdk_log_debug("GetVehicleModelsUsed()");
  native = sampgdk_native_find_flexible("GetVehicleModelsUsed", native);
  retval = native(sampgdk_fakeamx_amx(), NULL);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleNumberPlate(int vehicleid, char * plate, int len)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[4];
  cell plate_;
  sampgdk_log_debug("GetVehicleNumberPlate(%d, @%p, %d)", vehicleid, plate, len);
  native = sampgdk_native_find_flexible("GetVehicleNumberPlate", native);
  sampgdk_fakeamx_push(len, &plate_);
  params[0] = 3 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = plate_;
  params[3] = (cell)len;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(plate_, plate, len);
  sampgdk_fakeamx_pop(plate_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetVehicleOccupant(int vehicleid, int seatid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("GetVehicleOccupant(%d, %d)", vehicleid, seatid);
  native = sampgdk_native_find_flexible("GetVehicleOccupant", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)seatid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetVehicleOccupiedTick(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleOccupiedTick(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleOccupiedTick", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetVehiclePaintjob(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehiclePaintjob(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehiclePaintjob", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetVehicleRespawnDelay(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleRespawnDelay(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleRespawnDelay", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(int, GetVehicleRespawnTick(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleRespawnTick(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleRespawnTick", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleRotation(int vehicleid, float * x, float * y, float * z)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[5];
  cell x_;
  cell y_;
  cell z_;
  sampgdk_log_debug("GetVehicleRotation(%d, @%p, @%p, @%p)", vehicleid, x, y, z);
  native = sampgdk_native_find_flexible("GetVehicleRotation", native);
  sampgdk_fakeamx_push(1, &x_);
  sampgdk_fakeamx_push(1, &y_);
  sampgdk_fakeamx_push(1, &z_);
  params[0] = 4 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = x_;
  params[3] = y_;
  params[4] = z_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(x_, x);
  sampgdk_fakeamx_get_float(y_, y);
  sampgdk_fakeamx_get_float(z_, z);
  sampgdk_fakeamx_pop(z_);
  sampgdk_fakeamx_pop(y_);
  sampgdk_fakeamx_pop(x_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetVehicleSeats(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleSeats(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleSeats", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleSirenState(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleSirenState(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleSirenState", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, GetVehicleSpawnInfo(int vehicleid, float * spawnX, float * spawnY, float * spawnZ, float * angle, int * colour1, int * colour2)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[8];
  cell spawnX_;
  cell spawnY_;
  cell spawnZ_;
  cell angle_;
  cell colour1_;
  cell colour2_;
  sampgdk_log_debug("GetVehicleSpawnInfo(%d, @%p, @%p, @%p, @%p, @%p, @%p)", vehicleid, spawnX, spawnY, spawnZ, angle, colour1, colour2);
  native = sampgdk_native_find_flexible("GetVehicleSpawnInfo", native);
  sampgdk_fakeamx_push(1, &spawnX_);
  sampgdk_fakeamx_push(1, &spawnY_);
  sampgdk_fakeamx_push(1, &spawnZ_);
  sampgdk_fakeamx_push(1, &angle_);
  sampgdk_fakeamx_push(1, &colour1_);
  sampgdk_fakeamx_push(1, &colour2_);
  params[0] = 7 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = spawnX_;
  params[3] = spawnY_;
  params[4] = spawnZ_;
  params[5] = angle_;
  params[6] = colour1_;
  params[7] = colour2_;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_float(spawnX_, spawnX);
  sampgdk_fakeamx_get_float(spawnY_, spawnY);
  sampgdk_fakeamx_get_float(spawnZ_, spawnZ);
  sampgdk_fakeamx_get_float(angle_, angle);
  sampgdk_fakeamx_get_cell(colour1_, colour1);
  sampgdk_fakeamx_get_cell(colour2_, colour2);
  sampgdk_fakeamx_pop(colour2_);
  sampgdk_fakeamx_pop(colour1_);
  sampgdk_fakeamx_pop(angle_);
  sampgdk_fakeamx_pop(spawnZ_);
  sampgdk_fakeamx_pop(spawnY_);
  sampgdk_fakeamx_pop(spawnX_);
  return !!(retval);
}

SAMPGDK_NATIVE(int, GetVehicleTower(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleTower(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleTower", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return (int)(retval);
}

SAMPGDK_NATIVE(float, GetVehicleTrainSpeed(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("GetVehicleTrainSpeed(%d)", vehicleid);
  native = sampgdk_native_find_flexible("GetVehicleTrainSpeed", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return amx_ctof(retval);
}

SAMPGDK_NATIVE(int, GetVehicles(char * vehicles, int size)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  cell vehicles_;
  sampgdk_log_debug("GetVehicles(@%p, %d)", vehicles, size);
  native = sampgdk_native_find_flexible("GetVehicles", native);
  sampgdk_fakeamx_push(size, &vehicles_);
  params[0] = 2 * sizeof(cell);
  params[1] = vehicles_;
  params[2] = (cell)size;
  retval = native(sampgdk_fakeamx_amx(), params);
  sampgdk_fakeamx_get_string(vehicles_, vehicles, size);
  sampgdk_fakeamx_pop(vehicles_);
  return (int)(retval);
}

SAMPGDK_NATIVE(bool, HasVehicleBeenOccupied(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("HasVehicleBeenOccupied(%d)", vehicleid);
  native = sampgdk_native_find_flexible("HasVehicleBeenOccupied", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsPlayerInModShop(int playerid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsPlayerInModShop(%d)", playerid);
  native = sampgdk_native_find_flexible("IsPlayerInModShop", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)playerid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsVehicleDead(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsVehicleDead(%d)", vehicleid);
  native = sampgdk_native_find_flexible("IsVehicleDead", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsVehicleOccupied(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsVehicleOccupied(%d)", vehicleid);
  native = sampgdk_native_find_flexible("IsVehicleOccupied", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, IsVehicleSirenEnabled(int vehicleid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[2];
  sampgdk_log_debug("IsVehicleSirenEnabled(%d)", vehicleid);
  native = sampgdk_native_find_flexible("IsVehicleSirenEnabled", native);
  params[0] = 1 * sizeof(cell);
  params[1] = (cell)vehicleid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleParamsSirenState(int vehicleid, bool enabled)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetVehicleParamsSirenState(%d, %d)", vehicleid, enabled);
  native = sampgdk_native_find_flexible("SetVehicleParamsSirenState", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)enabled;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleRespawnDelay(int vehicleid, int respawnDelay)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("SetVehicleRespawnDelay(%d, %d)", vehicleid, respawnDelay);
  native = sampgdk_native_find_flexible("SetVehicleRespawnDelay", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)respawnDelay;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, SetVehicleSpawnInfo(int vehicleid, int modelid, float spawnX, float spawnY, float spawnZ, float angle, int colour1, int colour2, int respawnDelay, int interior)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[11];
  sampgdk_log_debug("SetVehicleSpawnInfo(%d, %d, %f, %f, %f, %f, %d, %d, %d, %d)", vehicleid, modelid, spawnX, spawnY, spawnZ, angle, colour1, colour2, respawnDelay, interior);
  native = sampgdk_native_find_flexible("SetVehicleSpawnInfo", native);
  params[0] = 10 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)modelid;
  params[3] = amx_ftoc(spawnX);
  params[4] = amx_ftoc(spawnY);
  params[5] = amx_ftoc(spawnZ);
  params[6] = amx_ftoc(angle);
  params[7] = (cell)colour1;
  params[8] = (cell)colour2;
  params[9] = (cell)respawnDelay;
  params[10] = (cell)interior;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, ToggleVehicleSirenEnabled(int vehicleid, bool enabled)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("ToggleVehicleSirenEnabled(%d, %d)", vehicleid, enabled);
  native = sampgdk_native_find_flexible("ToggleVehicleSirenEnabled", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)vehicleid;
  params[2] = (cell)enabled;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_NATIVE(bool, VehicleCanHaveComponent(int modelid, int componentid)) {
  static AMX_NATIVE native;
  cell retval;
  cell params[3];
  sampgdk_log_debug("VehicleCanHaveComponent(%d, %d)", modelid, componentid);
  native = sampgdk_native_find_flexible("VehicleCanHaveComponent", native);
  params[0] = 2 * sizeof(cell);
  params[1] = (cell)modelid;
  params[2] = (cell)componentid;
  retval = native(sampgdk_fakeamx_amx(), params);
  return !!(retval);
}

SAMPGDK_MODULE_INIT(omp_vehicle) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_vehicle) {
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

SAMPGDK_MODULE_INIT(omp_consts) {
  return 0;
}

SAMPGDK_MODULE_CLEANUP(omp_consts) {
}


