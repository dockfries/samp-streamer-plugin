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

#ifndef SAMPGDK_BOOL_H
#define SAMPGDK_BOOL_H

/* bool */
#if !defined __cplusplus && !defined HAVE_BOOL
  /* If HAVE_BOOL is not defined we attempt to detect stdbool.h first,
   * then define our own "bool" type.
   */
  #if defined __STDC__ && defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L\
      || defined HAVE_STDBOOL_H
    /* Have a C99-conformant compiler. */
    #include <stdbool.h>
  #else
    typedef unsigned char bool;
    #define true 1
    #define false 0
    #define __bool_true_false_are_defined
  #endif
#else
  /* Make sure their "bool" is one byte in size. This is required for binary
   * compatibility with C++ code. */
  typedef int sizeof_bool_must_be_1[sizeof(bool) == 1 ? 1 : -1];
#endif

#endif /* !SAMPGDK_BOOL_H */

#ifndef SAMPGDK_PLATFORM_H
#define SAMPGDK_PLATFORM_H

#if !defined _M_IX86 && !defined __i386__ \
    && !defined _M_X64 && !defined __x86_64__ \
    && !defined RC_INVOKED
  #error Unsupported architecture
#endif

#if defined _M_X64 || defined __x86_64__ || defined __64BIT__
  #define SAMPGDK_64BIT 1
#endif

#if (defined __CYGWIN32__ || defined RC_INVOKED) && !defined WIN32
  #define WIN32
#endif

#if defined WIN32 || defined _WIN32 || defined __WIN32__
  #define SAMPGDK_LINUX 0
  #define SAMPGDK_WINDOWS 1
#endif

#if defined __linux__ || defined __linux || defined linux
  #if !defined LINUX
    #define LINUX
  #endif
  #define SAMPGDK_LINUX 1
  #define SAMPGDK_WINDOWS 0
#endif

#if defined __GNUC__
  #define SAMPGDK_DEPRECATED_API(type, rest) \
    SAMPGDK_API(type, rest) __attribute__((deprecated))
#elif defined _MSC_VER
  #define SAMPGDK_DEPRECATED_API(return_type, rest) \
    __declspec(deprecated) SAMPGDK_API(return_type, rest)
#else
  #define SAMPGDK_DEPRECATED_API(return_type, rest)
#endif

#if SAMPGDK_WINDOWS
  #ifdef _MSC_VER
    #define SAMPGDK_CDECL __cdecl
    #define SAMPGDK_STDCALL __stdcall
  #else
    #ifdef SAMPGDK_64BIT
      #define SAMPGDK_CDECL
      #define SAMPGDK_STDCALL
    #else
      #define SAMPGDK_CDECL __attribute__((cdecl))
      #define SAMPGDK_STDCALL __attribute__((stdcall))
    #endif
  #endif
#elif SAMPGDK_LINUX
  #ifdef SAMPGDK_64BIT
    #define SAMPGDK_CDECL
    #define SAMPGDK_STDCALL
  #else
    #define SAMPGDK_CDECL __attribute__((cdecl))
    #define SAMPGDK_STDCALL __attribute__((stdcall))
  #endif
#endif

#if SAMPGDK_LINUX && defined IN_SAMPGDK && !defined _GNU_SOURCE
  #define _GNU_SOURCE
#endif

#endif /* !SAMPGDK_PLATFORM_H */

#ifndef SAMPGDK_SDK_H
#define SAMPGDK_SDK_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/platform.h> */

/* stdint.h */
#if !defined HAVE_STDINT_H
  #if (!defined __STDC__ && __STDC_VERSION__ >= 199901L /* C99 or newer */)\
    || (defined _MSC_VER && _MSC_VER >= 1600 /* Visual Studio 2010 and later */)\
    || defined __GNUC__ /* GCC, MinGW, etc */
    #define HAVE_STDINT_H 1
  #endif
#endif

/* size_t */
#include <stddef.h>

/* alloca() */
#if SAMPGDK_WINDOWS
  #undef HAVE_ALLOCA_H
  #include <malloc.h> /* for _alloca() */
  #if !defined alloca
    #define alloca _alloca
  #endif
#elif SAMPGDK_LINUX
  #if defined __GNUC__
    #define HAVE_ALLOCA_H 1
    #if !defined alloca
      #define alloca __builtin_alloca
    #endif
  #endif
#endif

#if defined __INTEL_COMPILER
  /* ... */
#elif defined __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wignored-attributes"
#elif defined __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wattributes"
#endif

#include "amx/amx.h"
#include "plugincommon.h"

#if defined __INTEL_COMPILER
  /* ... */
#elif defined __clang_
  #pragma clang diagnostic pop
#elif defined __GNUC__
  #pragma GCC diagnostic pop
#endif

/**
 * \addtogroup sdk
 * @{
 */

/**
 * \brief Gets called before Load() to check for compatibility
 *
 * The Supports() function indicates what possibilities this
 * plugin has. The SUPPORTS_VERSION flag is required to check
 * for compatibility with the server.
 *
 * \returns combination of SUPPORTS_* flags
 */
PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports();

/**
 * \brief Gets called when the plugin is loaded
 *
 * The Load() function gets passed on exported functions from
 * the SA-MP Server, like the AMX Functions and logprintf().
 * Should return true if loading the plugin has succeeded.
 *
 * \param ppData plugin data
 *
 * \returns \c true if the plugin has successfully loaded and
 * \c false otherwise
 */
PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData);

/**
 * \brief Gets called when the plugin is unloaded
 *
 * The Unload() function is called when the server shuts down,
 * meaning this plugin gets shut down with it.
 */
PLUGIN_EXPORT void PLUGIN_CALL Unload();

/**
 * \brief Gets called when a new script is loaded
 *
 * The AmxLoad() function gets called when a new gamemode or
 * filterscript gets loaded with the server. In here we register
 * the native functions we like to add to the scripts.
 *
 * \param amx pointer to the script's AMX object
 *
 * \returns one of AMX error codes
 */
PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx);

/**
 * \brief Gets called when a script is unloaded
 *
 * When a gamemode is over or a filterscript gets unloaded, this
 * function gets called. No special actions needed in here.
 *
 * \param amx pointer to the script's AMX object
 *
 * \returns one of AMX error codes
 */
PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx);

/**
 * \brief Gets called on every server tick
 *
 * Each tick corresponds to one iteration of the server's internal
 * event loop. The interval between ticks depends on many factors,
 * but it's possible to set the minimum tick rate via server.cfg
 * (default is 5ms).
 */
PLUGIN_EXPORT void PLUGIN_CALL ProcessTick();

/** @} */

#endif /* !SAMPGDK_SDK_H */

#ifndef SAMPGDK_EXPORT_H
#define SAMPGDK_EXPORT_H

/* #include <sampgdk/platform.h> */
/* #include <sampgdk/sdk.h> */

#undef SAMPGDK_EXPORT
#undef SAMPGDK_CALL

#ifdef __cplusplus
  #define SAMPGDK_EXTERN_C extern "C"
#else
  #define SAMPGDK_EXTERN_C
#endif

#if defined SAMPGDK_STATIC || defined SAMPGDK_AMALGAMATION
  #define SAMPGDK_EMBEDDED
#endif

#ifdef SAMPGDK_EMBEDDED
  #define SAMPGDK_CALL
#else
  #define SAMPGDK_CALL SAMPGDK_CDECL
#endif

#ifdef SAMPGDK_EMBEDDED
  #define SAMPGDK_EXPORT SAMPGDK_EXTERN_C
#else
  #if SAMPGDK_LINUX
    #if defined IN_SAMPGDK
      #define SAMPGDK_EXPORT SAMPGDK_EXTERN_C __attribute__((visibility("default")))
    #else
      #define SAMPGDK_EXPORT SAMPGDK_EXTERN_C
    #endif
  #elif SAMPGDK_WINDOWS
    #if defined IN_SAMPGDK
      #define SAMPGDK_EXPORT SAMPGDK_EXTERN_C __declspec(dllexport)
    #else
      #define SAMPGDK_EXPORT SAMPGDK_EXTERN_C __declspec(dllimport)
    #endif
  #else
    #error Unsupported operating system
  #endif
#endif

#define SAMPGDK_API(return_type, rest) \
  SAMPGDK_EXPORT return_type SAMPGDK_CALL rest

#undef SAMPGDK_NATIVE_EXPORT
#undef SAMPGDK_NATIVE_CALL

#define SAMPGDK_NATIVE_EXPORT SAMPGDK_EXPORT
#define SAMPGDK_NATIVE_CALL SAMPGDK_CALL
#define SAMPGDK_NATIVE(return_type, rest) \
  SAMPGDK_NATIVE_EXPORT return_type SAMPGDK_NATIVE_CALL sampgdk_ ## rest

#undef SAMPGDK_CALLBACK_EXPORT
#undef SAMPGDK_CALLBACK_CALL

#define SAMPGDK_CALLBACK_EXPORT PLUGIN_EXPORT
#define SAMPGDK_CALLBACK_CALL PLUGIN_CALL
#define SAMPGDK_CALLBACK(return_type, rest) \
  SAMPGDK_CALLBACK_EXPORT return_type SAMPGDK_CALLBACK_CALL rest

#endif /* !SAMPGDK_EXPORT_H */

#ifndef SAMPGDK_CORE_H
#define SAMPGDK_CORE_H

#include <stdarg.h>

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/sdk.h> */

/**
 * \defgroup core      Core
 * \defgroup interop   Interop
 * \defgroup version   Version
 * \defgroup sdk       SA-MP SDK
 * \defgroup natives   SA-MP Natives
 * \defgroup callbacks SA-MP Callbacks
 */

/**
 * \addtogroup core
 * @{
 */

/**
 * \brief Hidden parameter type, do not use this
 */
typedef int sampgdk_hidden_t;

/**
 * \brief Returns supported SDK version
 *
 * This function always returns SUPPORTS_VERSION. Its sole purpose is to
 * make sure that the version of the SDK is compatible with the one that
 * was used for building the library.
 *
 * \code
 * PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
 *   return sampgdk_Supports() | SUPPORTS_PROCESS_TICK;
 * }
 * \endcode
 *
 * \returns SUPPORTS_VERSION
 */
SAMPGDK_API(unsigned int, sampgdk_Supports(void));

/**
 * \brief Initializes the library
 *
 * Allocates memory for internal data structures and sets everything
 * up. Also keeps track of currently loaded plugins and registers the
 * calling plugin for callback handling.
 *
 * This function should be called from Load().
 *
 * \param ppData pointer to plugin data as passed to Load()
 *
 * \returns \c true on success and \c false otherwise
 *
 * \see sampgdk_Unload()
 */
SAMPGDK_API(bool, sampgdk_Load(void **ppData, sampgdk_hidden_t));

/**
 * \brief Shuts everything down, opposite of sampgdk_Load()
 *
 * This function should be called from Unload().
 *
 * \see sampgdk_Load()
 */
SAMPGDK_API(void, sampgdk_Unload(sampgdk_hidden_t));

/**
 * \brief Processes timers created by the calling plugin
 *
 * Goes through the list of created timers and, if necessary, fires
 * them one by one in the order of increasing IDs.
 *
 * If timer precision is important it's better to call this function
 * on every server tick. The plugin's ProcessTick() function might be
 * a good place for that.
 */
SAMPGDK_API(void, sampgdk_ProcessTick(sampgdk_hidden_t));

/**
 * \brief Prints a message to the server log
 *
 * \note The resulting message cannot be longer than 1024 characters.
 *
 * \param format printf-style format string
 * \param ... further arguments to logprintf()
 *
 * \see sampgdk_vlogprintf()
 */
SAMPGDK_API(void, sampgdk_logprintf(const char *format, ...));

/**
 * \brief Prints a message to the server log
 *
 * This function is identica to sampgdk_logprintf() except it takes
 * a \c va_list instead of variable arguments.
 *
 * \param format printf-style format string
 * \param args further arguments to logprintf()
 *
 * \see sampgdk_logprintf()
 */
SAMPGDK_API(void, sampgdk_vlogprintf(const char *format, va_list args));

/** @} */

#define sampgdk_Load(ppData)  sampgdk_Load(ppData, 0)
#define sampgdk_Unload()      sampgdk_Unload(0)
#define sampgdk_ProcessTick() sampgdk_ProcessTick(0)

#ifdef __cplusplus

/**
 * \brief Main namespace
 */
namespace sampgdk {

/**
 * \addtogroup core
 * @{
 */

/// \brief C++ wrapper around sampgdk_Supports()
inline unsigned int Supports() {
  return sampgdk_Supports();
}

/// \brief C++ wrapper around sampgdk_Load()
inline bool Load(void **ppData) {
  return sampgdk_Load(ppData);
}

/// \brief C++ wrapper around sampgdk_Unload()
inline void Unload() {
  sampgdk_Unload();
}

/// \brief C++ wrapper around sampgdk_ProcessTick()
inline void ProcessTick() {
  sampgdk_ProcessTick();
}

/// \brief C++ wrapper around sampgdk_logprintf()
inline void logprintf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  sampgdk_vlogprintf(format, args);
  va_end(args);
}

/// \brief C++ wrapper around sampgdk_vlogprintf()
inline void vlogprintf(const char *format, va_list args) {
  sampgdk_vlogprintf(format, args);
}

/** @} */

} // namespace sampgdk

#endif /* __cplusplus */

#endif /* !SAMPGDK_CORE_H */

#ifndef SAMPGDK_TYPES_H
#define SAMPGDK_TYPES_H

/* #include <sampgdk/export.h> */

/**
 * \brief Defines the signature of a timer callback function
 * \ingroup natives
 *
 * \param timerid timer ID as returned by SetTimer()
 * \param param user-supplied data as passed to SetTimer()
 */
typedef void (SAMPGDK_CALL *TimerCallback)(int timerid, void *param);

#endif /* !SAMPGDK_TYPES_H */

#ifndef SAMPGDK_VERSION_H
#define SAMPGDK_VERSION_H

/* #include <sampgdk/export.h> */

/**
 * \addtogroup version
 * @{
 */

/**
 * \brief Major version
 */
#define SAMPGDK_VERSION_MAJOR 5

/**
 * \brief Minor version
 */
#define SAMPGDK_VERSION_MINOR 0

/**
 * \brief Patch version
 */
#define SAMPGDK_VERSION_PATCH 0

/**
 * \brief Library version number in the form of \c 0xAABBCC00 where
 * \c AA, \c BB and \c CC are the major, minor and patch numbers
 */
#define SAMPGDK_VERSION_ID 83886080

/**
 * \brief Library version string in the form of \c x.y.z where \c x,
 * \c y and \c z are the major, minor and patch numbers
 */
#define SAMPGDK_VERSION_STRING "5.0.0"

/**
 * \brief Gets library version number
 *
 * \returns version number
 *
 * \see SAMPGDK_VERSION_ID
 * \see sampgdk_GetVersionString()
 */
SAMPGDK_API(int, sampgdk_GetVersion(void));

/**
 * \brief Gets library version string
 *
 * \returns version string
 *
 * \see SAMPGDK_VERSION_STRING
 * \see sampgdk_GetVersion()
 */
SAMPGDK_API(const char *, sampgdk_GetVersionString(void));

#ifdef __cplusplus

namespace sampgdk {

/// \brief C++ wrapper around sampgdk_GetVersion()
inline int GetVersion() {
  return sampgdk_GetVersion();
}

/// \brief C++ wrapper around sampgdk_GetVersionString()
inline const char *GetVersionString() {
  return sampgdk_GetVersionString();
}

} // namespace sampgdk

#endif /* __cplusplus */

/** @} */

#endif /* !SAMPGDK_VERSION_H */

#ifndef SAMPGDK_INTEROP_H
#define SAMPGDK_INTEROP_H

#include <stdarg.h>

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */

/**
 * \addtogroup interop
 * @{
 */

/**
 * \brief Returns all currently registered native functions
 *
 * This function can be used to get the names and addresses of all native
 * functions that have been registered with amx_Register(), by both the
 * server and plugins.
 *
 * \note The returned array is NULL-terminated.
 *
 * \param number where to store the number of natives (optional).
 *
 * \returns pointer to array of registered native functions
 *
 * \see sampgdk_FindNative()
 * \see sampgdk_CallNative()
 * \see sampgdk_InvokeNative()
 */
SAMPGDK_API(const AMX_NATIVE_INFO *, sampgdk_GetNatives(int *number));

/**
 * \brief Finds a native function by name
 *
 * Searches for a native function with the specified name and returns its
 * address. In order to be found the function must be registered with
 * amx_Register() prior to the call.
 *
 * \param name name of the native function
 *
 * \returns function's address or \c NULL if not found
 *
 * \see sampgdk_GetNatives()
 * \see sampgdk_CallNative()
 * \see sampgdk_InvokeNative()
 */
SAMPGDK_API(AMX_NATIVE, sampgdk_FindNative(const char *name));

/**
 * \brief Calls a native function
 *
 * This function is suitable for calling simple natives that either have only
 * value parameters or don't have any parameters at all. If you have to pass
 * a reference or a string use sampgdk_InvokeNative() instead.
 *
 * \note The first element of \p params must contain the number of arguments
 * multiplied by \c sizeof(cell).
 *
 * \param native pointer to the native function
 * \param params parameters to be passed to the function as its second argument
 *
 * \returns function's return value
 *
 * \see sampgdk_GetNatives()
 * \see sampgdk_FindNative()
 * \see sampgdk_InvokeNative()
 */
SAMPGDK_API(cell, sampgdk_CallNative(AMX_NATIVE native, cell *params));

/**
 * \brief Calls a native function with arguments
 *
 * Argument types are specified via \p format where each character, or
 * *specifier*, corresponds to a single argument. The following format
 * specifiers are supported:
 *
 * Specifier | C/C++ type    | Description
 * :-------- | :------------ | :------------------------------------------
 * i         | int           | integer value
 * d         | int           | integer value (same as 'i')
 * b         | bool          | boolean value
 * f         | double        | floating-point value
 * r         | const cell *  | const reference (input only)
 * R         | cell *        | non-const reference (both input and output)
 * s         | const char *  | const string (input only)
 * S         | char *        | non-const string (both input and output)
 * a         | const cell *  | const array (input only)
 * A         | cell *        | non-const array (both input and output)
 *
 * \remarks For the 'S', 'a' and 'A' specifiers you have to specify the size
 * of the string/array in square brackets, e.g. "a[100]" (fixed size)
 * or s[*2] (size passed via 2nd argument).
 *
 * \note In Pawn variadic functions always take their variable arguments
 * (those represented by "...") by reference. This means that for such
 * functions you have to use the 'r' specifier where you would normally
 * use 'b', 'i' 'd' or 'f'.
 *
 * \param native pointer to the native function.
 * \param format argument types
 * \param ... arguments themselves
 *
 * \returns function's return value
 *
 * \see sampgdk_GetNatives()
 * \see sampgdk_FindNative()
 * \see sampgdk_InvokeNativeV()
 * \see sampgdk_InvokeNativeArray()
 */
SAMPGDK_API(cell, sampgdk_InvokeNative(AMX_NATIVE native,
    const char *format, ...));

/**
* \brief Calls a native function with arguments
*
* This function is identical to sampgdk_InvokeNative() except it takes
* \c va_list instead of variable arguments.
*
* \see sampgdk_GetNatives()
* \see sampgdk_FindNative()
* \see sampgdk_InvokeNative()
* \see sampgdk_InvokeNativeArray()
*/
SAMPGDK_API(cell, sampgdk_InvokeNativeV(AMX_NATIVE native,
    const char *format, va_list args));

/**
* \brief Calls a native function with an array of arguments
*
* This function is similar to sampgdk_InvokeNative() but the arguments
* are passed as an array where each element is a pointer pointing to
* the actual value.
*
* Argument types are specified via \p format where each character, or
* *specifier*, corresponds to a single argument. See sampgdk_InvokeNative()
* for the list of supported format specifiers.
*
* \param native pointer to the native function.
* \param format argument types
* \param args arguments themselves
*
* \returns function's return value
*
* \see sampgdk_GetNatives()
* \see sampgdk_FindNative()
* \see sampgdk_InvokeNative()
*/
SAMPGDK_API(cell, sampgdk_InvokeNativeArray(AMX_NATIVE native,
    const char *format, void **args));

/**
 * \brief A generic catch-all callback that gets called whenever some
 * AMX public function is executed
 *
 * This is the publics "filter" callback. It is called whenever the
 * server calls \c amx_Exec(), which practically means that you can
 * use it to hook *any* callback, even those that are called by other
 * plugins.
 *
 * \param amx AMX on which the function is called
 * \param name function name
 * \param params function arguments as stored on the AMX stack, with
 *        \c params[0] being set to the number of arguments multiplied
 *        by \c sizeof(cell)
 * \param retval where to store the return value (can be \c NULL)
 *
 * \returns \c true if the public is allowed to execute
 */
SAMPGDK_CALLBACK(bool, OnPublicCall(AMX *amx, const char *name,
    cell *params, cell *retval));

/**
 * \brief A generic catch-all callback that gets called whenever some
 * AMX public function is executed
 *
 * This callback is similar to \c OnPublicCall but also allows you to
 * stop the call from being propagated to other plugins or the gamemode
 * by setting the \c stop parameter to \c true.
 *
 * \param amx AMX on which the function is called
 * \param name function name
 * \param params function arguments as stored on the AMX stack, with
 *        \c params[0] being set to the number of arguments multiplied
 *        by \c sizeof(cell)
 * \param retval where to store the return value (can be \c NULL)
 * \param stop whether to stop public call propagation (\c false by default)
 *
 * \returns \c true if the public is allowed to execute
 */
SAMPGDK_CALLBACK(bool, OnPublicCall2(AMX *amx, const char *name,
    cell *params, cell *retval, bool *stop));

/** @} */

#ifdef __cplusplus

namespace sampgdk {

/**
  * \addtogroup interop
  * @{
  */

/// \brief C++ wrapper around sampgdk_GetNatives()
inline const AMX_NATIVE_INFO *GetNatives(int &number) {
  return sampgdk_GetNatives(&number);
}

/// \brief C++ wrapper around sampgdk_GetNatives()
inline const AMX_NATIVE_INFO *GetNatives() {
  return sampgdk_GetNatives(0);
}

/// \brief C++ wrapper around sampgdk_FindNative()
inline AMX_NATIVE FindNative(const char *name) {
  return sampgdk_FindNative(name);
}

/// \brief C++ wrapper around sampgdk_CallNative()
inline cell CallNative(AMX_NATIVE native, cell *params) {
  return sampgdk_CallNative(native, params);
}

/// \brief C++ wrapper around sampgdk_InvokeNative()
inline cell InvokeNative(AMX_NATIVE native, const char *format, ...) {
  va_list args;
  va_start(args, format);
  cell retval = sampgdk_InvokeNativeV(native, format, args);
  va_end(args);
  return retval;
}

/// \brief C++ wrapper around sampgdk_InvokeNativeV()
inline cell InvokeNativeV(AMX_NATIVE native, const char *format,
    va_list args) {
  return sampgdk_InvokeNativeV(native, format, args);
}

/// \brief C++ wrapper around sampgdk_InvokeNativeArray()
inline cell InvokeNativeArray(AMX_NATIVE native, const char *format,
    void **args) {
  return sampgdk_InvokeNativeArray(native, format, args);
}

/** @} */

} // namespace sampgdk

#endif /* __cplusplus */

#endif /* !SAMPGDK_INTEROP_H */

#ifndef SAMPGDK_A_SAMP_H
#define SAMPGDK_A_SAMP_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */

#define MAX_PLAYER_NAME (24)
#define MAX_CLIENT_MESSAGE (144)
#define MAX_PLAYERS (1000)
#define MAX_VEHICLES (2000)
#define MAX_ACTORS (1000)
#define INVALID_PLAYER_ID (0xFFFF)
#define INVALID_VEHICLE_ID (0xFFFF)
#define INVALID_ACTOR_ID (0xFFFF)
#define NO_TEAM (255)
#define MAX_OBJECTS (1000)
#define INVALID_OBJECT_ID (0xFFFF)
#define MAX_GANG_ZONES (1024)
#define MAX_TEXT_DRAWS (2048)
#define MAX_PLAYER_TEXT_DRAWS (256)
#define MAX_MENUS (128)
#define MAX_3DTEXT_GLOBAL (1024)
#define MAX_3DTEXT_PLAYER (1024)
#define MAX_PICKUPS (4096)
#define INVALID_MENU (0xFF)
#define INVALID_TEXT_DRAW (0xFFFF)
#define INVALID_GANG_ZONE (-1)
#define INVALID_3DTEXT_ID (0xFFFF)
#define SERVER_VARTYPE_NONE (0)
#define SERVER_VARTYPE_INT (1)
#define SERVER_VARTYPE_STRING (2)
#define SERVER_VARTYPE_FLOAT (3)
#define TEXT_DRAW_FONT_SPRITE_DRAW (4)
#define TEXT_DRAW_FONT_MODEL_PREVIEW (5)
#define DIALOG_STYLE_MSGBOX (0)
#define DIALOG_STYLE_INPUT (1)
#define DIALOG_STYLE_LIST (2)
#define DIALOG_STYLE_PASSWORD (3)
#define DIALOG_STYLE_TABLIST (4)
#define DIALOG_STYLE_TABLIST_HEADERS (5)
#define PLAYER_STATE_NONE (0)
#define PLAYER_STATE_ONFOOT (1)
#define PLAYER_STATE_DRIVER (2)
#define PLAYER_STATE_PASSENGER (3)
#define PLAYER_STATE_EXIT_VEHICLE (4)
#define PLAYER_STATE_ENTER_VEHICLE_DRIVER (5)
#define PLAYER_STATE_ENTER_VEHICLE_PASSENGER (6)
#define PLAYER_STATE_WASTED (7)
#define PLAYER_STATE_SPAWNED (8)
#define PLAYER_STATE_SPECTATING (9)
#define PLAYER_MARKERS_MODE_OFF (0)
#define PLAYER_MARKERS_MODE_GLOBAL (1)
#define PLAYER_MARKERS_MODE_STREAMED (2)
#define WEAPON_FIST (0)
#define WEAPON_BRASSKNUCKLE (1)
#define WEAPON_GOLFCLUB (2)
#define WEAPON_NITESTICK (3)
#define WEAPON_KNIFE (4)
#define WEAPON_BAT (5)
#define WEAPON_SHOVEL (6)
#define WEAPON_POOLSTICK (7)
#define WEAPON_KATANA (8)
#define WEAPON_CHAINSAW (9)
#define WEAPON_DILDO (10)
#define WEAPON_DILDO2 (11)
#define WEAPON_VIBRATOR (12)
#define WEAPON_VIBRATOR2 (13)
#define WEAPON_FLOWER (14)
#define WEAPON_CANE (15)
#define WEAPON_GRENADE (16)
#define WEAPON_TEARGAS (17)
#define WEAPON_MOLTOV (18)
#define WEAPON_COLT45 (22)
#define WEAPON_SILENCED (23)
#define WEAPON_DEAGLE (24)
#define WEAPON_SHOTGUN (25)
#define WEAPON_SAWEDOFF (26)
#define WEAPON_SHOTGSPA (27)
#define WEAPON_UZI (28)
#define WEAPON_MP5 (29)
#define WEAPON_AK47 (30)
#define WEAPON_M4 (31)
#define WEAPON_TEC9 (32)
#define WEAPON_RIFLE (33)
#define WEAPON_SNIPER (34)
#define WEAPON_ROCKETLAUNCHER (35)
#define WEAPON_HEATSEEKER (36)
#define WEAPON_FLAMETHROWER (37)
#define WEAPON_MINIGUN (38)
#define WEAPON_SATCHEL (39)
#define WEAPON_BOMB (40)
#define WEAPON_SPRAYCAN (41)
#define WEAPON_FIREEXTINGUISHER (42)
#define WEAPON_CAMERA (43)
#define WEAPON_NIGHTVISION (44)
#define WEAPON_INFRARED (45)
#define WEAPON_PARACHUTE (46)
#define WEAPON_VEHICLE (49)
#define WEAPON_DROWN (53)
#define WEAPON_COLLISION (54)
#define VEHICLE_LANDSTALKER (400)
#define VEHICLE_BRAVURA (401)
#define VEHICLE_BUFFALO (402)
#define VEHICLE_LINERUNNER (403)
#define VEHICLE_PERRENIAL (404)
#define VEHICLE_SENTINEL (405)
#define VEHICLE_DUMPER (406)
#define VEHICLE_FIRETRUCK (407)
#define VEHICLE_TRASHMASTER (408)
#define VEHICLE_STRETCH (409)
#define VEHICLE_MANANA (410)
#define VEHICLE_INFERNUS (411)
#define VEHICLE_VOODOO (412)
#define VEHICLE_PONY (413)
#define VEHICLE_MULE (414)
#define VEHICLE_CHEETAH (415)
#define VEHICLE_AMBULANCE (416)
#define VEHICLE_LEVIATHAN (417)
#define VEHICLE_MOONBEAM (418)
#define VEHICLE_ESPERANTO (419)
#define VEHICLE_TAXI (420)
#define VEHICLE_WASHINGTON (421)
#define VEHICLE_BOBCAT (422)
#define VEHICLE_MRWHOOPEE (423)
#define VEHICLE_BFINJECTION (424)
#define VEHICLE_HUNTER (425)
#define VEHICLE_PREMIER (426)
#define VEHICLE_ENFORCER (427)
#define VEHICLE_SECURICAR (428)
#define VEHICLE_BANSHEE (429)
#define VEHICLE_PREDATOR (430)
#define VEHICLE_BUS (431)
#define VEHICLE_RHINO (432)
#define VEHICLE_BARRACKS (433)
#define VEHICLE_HOTKNIFE (434)
#define VEHICLE_ARTICLETRAILER1 (435)
#define VEHICLE_PREVION (436)
#define VEHICLE_COACH (437)
#define VEHICLE_CABBIE (438)
#define VEHICLE_STALLION (439)
#define VEHICLE_RUMPO (440)
#define VEHICLE_RCBANDIT (441)
#define VEHICLE_ROMERO (442)
#define VEHICLE_PACKER (443)
#define VEHICLE_MONSTER (444)
#define VEHICLE_ADMIRAL (445)
#define VEHICLE_SQUALO (446)
#define VEHICLE_SEASPARROW (447)
#define VEHICLE_PIZZABOY (448)
#define VEHICLE_TRAM (449)
#define VEHICLE_ARTICLETRAILER2 (450)
#define VEHICLE_TURISMO (451)
#define VEHICLE_SPEEDER (452)
#define VEHICLE_REEFER (453)
#define VEHICLE_TROPIC (454)
#define VEHICLE_FLATBED (455)
#define VEHICLE_YANKEE (456)
#define VEHICLE_CADDY (457)
#define VEHICLE_SOLAIR (458)
#define VEHICLE_BERKLEYSRCVAN (459)
#define VEHICLE_SKIMMER (460)
#define VEHICLE_PCJ600 (461)
#define VEHICLE_FAGGIO (462)
#define VEHICLE_FREEWAY (463)
#define VEHICLE_RCBARON (464)
#define VEHICLE_RCRAIDER (465)
#define VEHICLE_GLENDALE (466)
#define VEHICLE_OCEANIC (467)
#define VEHICLE_SANCHEZ (468)
#define VEHICLE_SPARROW (469)
#define VEHICLE_PATRIOT (470)
#define VEHICLE_QUAD (471)
#define VEHICLE_COASTGUARD (472)
#define VEHICLE_DINGHY (473)
#define VEHICLE_HERMES (474)
#define VEHICLE_SABRE (475)
#define VEHICLE_RUSTLER (476)
#define VEHICLE_ZR350 (477)
#define VEHICLE_WALTON (478)
#define VEHICLE_REGINA (479)
#define VEHICLE_COMET (480)
#define VEHICLE_BMX (481)
#define VEHICLE_BURRITO (482)
#define VEHICLE_CAMPER (483)
#define VEHICLE_MARQUIS (484)
#define VEHICLE_BAGGAGE (485)
#define VEHICLE_DOZER (486)
#define VEHICLE_MAVERICK (487)
#define VEHICLE_SANNEWSMAVERICK (488)
#define VEHICLE_RANCHER (489)
#define VEHICLE_FBIRANCHER (490)
#define VEHICLE_VIRGO (491)
#define VEHICLE_GREENWOOD (492)
#define VEHICLE_JETMAX (493)
#define VEHICLE_HOTRINGRACER (494)
#define VEHICLE_SANDKING (495)
#define VEHICLE_BLISTACOMPACT (496)
#define VEHICLE_POLICEMAVERICK (497)
#define VEHICLE_BOXVILLE (498)
#define VEHICLE_BENSON (499)
#define VEHICLE_MESA (500)
#define VEHICLE_RCGOBLIN (501)
#define VEHICLE_HOTRINGRACERA (502)
#define VEHICLE_HOTRINGRACERB (503)
#define VEHICLE_BLOODRINGBANGER (504)
#define VEHICLE_RANCHERLURE (505)
#define VEHICLE_SUPERGT (506)
#define VEHICLE_ELEGANT (507)
#define VEHICLE_JOURNEY (508)
#define VEHICLE_BIKE (509)
#define VEHICLE_MOUNTAINBIKE (510)
#define VEHICLE_BEAGLE (511)
#define VEHICLE_CROPDUST (512)
#define VEHICLE_STUNTPLANE (513)
#define VEHICLE_TANKER (514)
#define VEHICLE_ROADTRAIN (515)
#define VEHICLE_NEBULA (516)
#define VEHICLE_MAJESTIC (517)
#define VEHICLE_BUCCANEER (518)
#define VEHICLE_SHAMAL (519)
#define VEHICLE_HYDRA (520)
#define VEHICLE_FCR900 (521)
#define VEHICLE_NRG500 (522)
#define VEHICLE_HPV1000 (523)
#define VEHICLE_CEMENTTRUCK (524)
#define VEHICLE_TOWTRUCK (525)
#define VEHICLE_FORTUNE (526)
#define VEHICLE_CADRONA (527)
#define VEHICLE_FBITRUCK (528)
#define VEHICLE_WILLARD (529)
#define VEHICLE_FORKLIFT (530)
#define VEHICLE_TRACTOR (531)
#define VEHICLE_COMBINE (532)
#define VEHICLE_FELTZER (533)
#define VEHICLE_REMINGTON (534)
#define VEHICLE_SLAMVAN (535)
#define VEHICLE_BLADE (536)
#define VEHICLE_FREIGHT (537)
#define VEHICLE_BROWNSTREAK (538)
#define VEHICLE_VORTEX (539)
#define VEHICLE_VINCENT (540)
#define VEHICLE_BULLET (541)
#define VEHICLE_CLOVER (542)
#define VEHICLE_SADLER (543)
#define VEHICLE_FIRETRUCKLA (544)
#define VEHICLE_HUSTLER (545)
#define VEHICLE_INTRUDER (546)
#define VEHICLE_PRIMO (547)
#define VEHICLE_CARGOBOB (548)
#define VEHICLE_TAMPA (549)
#define VEHICLE_SUNRISE (550)
#define VEHICLE_MERIT (551)
#define VEHICLE_UTILITYVAN (552)
#define VEHICLE_NEVADA (553)
#define VEHICLE_YOSEMITE (554)
#define VEHICLE_WINDSOR (555)
#define VEHICLE_MONSTERA (556)
#define VEHICLE_MONSTERB (557)
#define VEHICLE_URANUS (558)
#define VEHICLE_JESTER (559)
#define VEHICLE_SULTAN (560)
#define VEHICLE_STRATUM (561)
#define VEHICLE_ELEGY (562)
#define VEHICLE_RAINDANCE (563)
#define VEHICLE_RCTIGER (564)
#define VEHICLE_FLASH (565)
#define VEHICLE_TAHOMA (566)
#define VEHICLE_SAVANNA (567)
#define VEHICLE_BANDITO (568)
#define VEHICLE_FREIGHTFLATTRAILER (569)
#define VEHICLE_STREAKTRAILER (570)
#define VEHICLE_KART (571)
#define VEHICLE_MOWER (572)
#define VEHICLE_DUNERIDE (573)
#define VEHICLE_SWEEPER (574)
#define VEHICLE_BROADWAY (575)
#define VEHICLE_TORNADO (576)
#define VEHICLE_AT400 (577)
#define VEHICLE_DFT30 (578)
#define VEHICLE_HUNTLEY (579)
#define VEHICLE_STAFFORD (580)
#define VEHICLE_BF400 (581)
#define VEHICLE_NEWSVAN (582)
#define VEHICLE_TUG (583)
#define VEHICLE_PETROLTRAILER (584)
#define VEHICLE_EMPEROR (585)
#define VEHICLE_WAYFARER (586)
#define VEHICLE_EUROS (587)
#define VEHICLE_HOTDOG (588)
#define VEHICLE_CLUB (589)
#define VEHICLE_FREIGHTBOXTRAILER (590)
#define VEHICLE_ARTICLETRAILER3 (591)
#define VEHICLE_ANDROMADA (592)
#define VEHICLE_DODO (593)
#define VEHICLE_RCCAM (594)
#define VEHICLE_LAUNCH (595)
#define VEHICLE_POLICECARLSPD (596)
#define VEHICLE_POLICECARSFPD (597)
#define VEHICLE_POLICECARLVPD (598)
#define VEHICLE_POLICERANGER (599)
#define VEHICLE_PICADOR (600)
#define VEHICLE_SWAT (601)
#define VEHICLE_ALPHA (602)
#define VEHICLE_PHOENIX (603)
#define VEHICLE_GLENDALESHIT (604)
#define VEHICLE_SADLERSHIT (605)
#define VEHICLE_BAGGAGETRAILERA (606)
#define VEHICLE_BAGGAGETRAILERB (607)
#define VEHICLE_TUGSTAIRSTRAILER (608)
#define VEHICLE_BOXBURG (609)
#define VEHICLE_FARMTRAILER (610)
#define VEHICLE_UTILITYTRAILER (611)
#define KEY_ACTION (1)
#define KEY_CROUCH (2)
#define KEY_FIRE (4)
#define KEY_SPRINT (8)
#define KEY_SECONDARY_ATTACK (16)
#define KEY_JUMP (32)
#define KEY_LOOK_RIGHT (64)
#define KEY_HANDBRAKE (128)
#define KEY_LOOK_LEFT (256)
#define KEY_SUBMISSION (512)
#define KEY_LOOK_BEHIND (512)
#define KEY_WALK (1024)
#define KEY_ANALOG_UP (2048)
#define KEY_ANALOG_DOWN (4096)
#define KEY_ANALOG_LEFT (8192)
#define KEY_ANALOG_RIGHT (16384)
#define KEY_YES (65536)
#define KEY_NO (131072)
#define KEY_CTRL_BACK (262144)
#define KEY_UP (-128)
#define KEY_DOWN (128)
#define KEY_LEFT (-128)
#define KEY_RIGHT (128)
#define BODY_PART_TORSO (3)
#define BODY_PART_GROIN (4)
#define BODY_PART_LEFT_ARM (5)
#define BODY_PART_RIGHT_ARM (6)
#define BODY_PART_LEFT_LEG (7)
#define BODY_PART_RIGHT_LEG (8)
#define BODY_PART_HEAD (9)
#define CLICK_SOURCE_SCOREBOARD (0)
#define EDIT_RESPONSE_CANCEL (0)
#define EDIT_RESPONSE_FINAL (1)
#define EDIT_RESPONSE_UPDATE (2)
#define SELECT_OBJECT_GLOBAL_OBJECT (1)
#define SELECT_OBJECT_PLAYER_OBJECT (2)
#define BULLET_HIT_TYPE_NONE (0)
#define BULLET_HIT_TYPE_PLAYER (1)
#define BULLET_HIT_TYPE_VEHICLE (2)
#define BULLET_HIT_TYPE_OBJECT (3)
#define BULLET_HIT_TYPE_PLAYER_OBJECT (4)
#define DOWNLOAD_REQUEST_EMPTY (0)
#define DOWNLOAD_REQUEST_MODEL_FILE (1)
#define DOWNLOAD_REQUEST_TEXTURE_FILE (2)

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SendClientMessage">SendClientMessage on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SendClientMessage(int playerid, int color, const char * message));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SendClientMessageToAll">SendClientMessageToAll on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SendClientMessageToAll(int color, const char * message));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SendPlayerMessageToPlayer">SendPlayerMessageToPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SendPlayerMessageToPlayer(int playerid, int senderid, const char * message));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SendPlayerMessageToAll">SendPlayerMessageToAll on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SendPlayerMessageToAll(int senderid, const char * message));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SendDeathMessage">SendDeathMessage on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SendDeathMessage(int killer, int killee, int weapon));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SendDeathMessageToPlayer">SendDeathMessageToPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SendDeathMessageToPlayer(int playerid, int killer, int killee, int weapon));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GameTextForAll">GameTextForAll on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GameTextForAll(const char * text, int time, int style));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GameTextForPlayer">GameTextForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GameTextForPlayer(int playerid, const char * text, int time, int style));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetTickCount">GetTickCount on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetTickCount());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetMaxPlayers">GetMaxPlayers on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetMaxPlayers());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/VectorSize">VectorSize on open.mp</a>
 */
SAMPGDK_NATIVE(float, VectorSize(float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerPoolSize">GetPlayerPoolSize on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerPoolSize());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehiclePoolSize">GetVehiclePoolSize on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehiclePoolSize());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetActorPoolSize">GetActorPoolSize on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetActorPoolSize());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SHA256_PassHash">SHA256_PassHash on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SHA256_PassHash(const char * password, const char * salt, char * ret_hash, int ret_hash_len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetSVarInt">SetSVarInt on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetSVarInt(const char * varname, int int_value));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetSVarInt">GetSVarInt on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetSVarInt(const char * varname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetSVarString">SetSVarString on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetSVarString(const char * varname, const char * string_value));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetSVarString">GetSVarString on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetSVarString(const char * varname, char * string_return, int len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetSVarFloat">SetSVarFloat on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetSVarFloat(const char * varname, float float_value));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetSVarFloat">GetSVarFloat on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetSVarFloat(const char * varname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DeleteSVar">DeleteSVar on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DeleteSVar(const char * varname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetSVarsUpperIndex">GetSVarsUpperIndex on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetSVarsUpperIndex());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetSVarNameAtIndex">GetSVarNameAtIndex on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetSVarNameAtIndex(int index, char * ret_varname, int ret_len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetSVarType">GetSVarType on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetSVarType(const char * varname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetGameModeText">SetGameModeText on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetGameModeText(const char * text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetTeamCount">SetTeamCount on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetTeamCount(int count));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AddPlayerClass">AddPlayerClass on open.mp</a>
 */
SAMPGDK_NATIVE(int, AddPlayerClass(int modelid, float spawn_x, float spawn_y, float spawn_z, float z_angle, int weapon1, int weapon1_ammo, int weapon2, int weapon2_ammo, int weapon3, int weapon3_ammo));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AddPlayerClassEx">AddPlayerClassEx on open.mp</a>
 */
SAMPGDK_NATIVE(int, AddPlayerClassEx(int teamid, int modelid, float spawn_x, float spawn_y, float spawn_z, float z_angle, int weapon1, int weapon1_ammo, int weapon2, int weapon2_ammo, int weapon3, int weapon3_ammo));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AddStaticVehicle">AddStaticVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(int, AddStaticVehicle(int modelid, float spawn_x, float spawn_y, float spawn_z, float z_angle, int color1, int color2));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AddStaticVehicleEx">AddStaticVehicleEx on open.mp</a>
 */
SAMPGDK_NATIVE(int, AddStaticVehicleEx(int modelid, float spawn_x, float spawn_y, float spawn_z, float z_angle, int color1, int color2, int respawn_delay, bool addsiren));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AddStaticPickup">AddStaticPickup on open.mp</a>
 */
SAMPGDK_NATIVE(int, AddStaticPickup(int model, int type, float x, float y, float z, int virtualworld));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CreatePickup">CreatePickup on open.mp</a>
 */
SAMPGDK_NATIVE(int, CreatePickup(int model, int type, float x, float y, float z, int virtualworld));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DestroyPickup">DestroyPickup on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DestroyPickup(int pickup));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ShowNameTags">ShowNameTags on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ShowNameTags(bool show));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ShowPlayerMarkers">ShowPlayerMarkers on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ShowPlayerMarkers(int mode));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GameModeExit">GameModeExit on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GameModeExit());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetWorldTime">SetWorldTime on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetWorldTime(int hour));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetWeaponName">GetWeaponName on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetWeaponName(int weaponid, char * name, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/EnableTirePopping">EnableTirePopping on open.mp</a>
 */
SAMPGDK_NATIVE(bool, EnableTirePopping(bool enable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/EnableVehicleFriendlyFire">EnableVehicleFriendlyFire on open.mp</a>
 */
SAMPGDK_NATIVE(bool, EnableVehicleFriendlyFire());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AllowInteriorWeapons">AllowInteriorWeapons on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AllowInteriorWeapons(bool allow));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetWeather">SetWeather on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetWeather(int weatherid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetGravity">SetGravity on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetGravity(float gravity));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetGravity">GetGravity on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetGravity());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AllowAdminTeleport">AllowAdminTeleport on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AllowAdminTeleport(bool allow));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetDeathDropAmount">SetDeathDropAmount on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetDeathDropAmount(int amount));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CreateExplosion">CreateExplosion on open.mp</a>
 */
SAMPGDK_NATIVE(bool, CreateExplosion(float x, float y, float z, int type, float radius));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/EnableZoneNames">EnableZoneNames on open.mp</a>
 */
SAMPGDK_NATIVE(bool, EnableZoneNames(bool enable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/UsePlayerPedAnims">UsePlayerPedAnims on open.mp</a>
 */
SAMPGDK_NATIVE(bool, UsePlayerPedAnims());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DisableInteriorEnterExits">DisableInteriorEnterExits on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DisableInteriorEnterExits());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetNameTagDrawDistance">SetNameTagDrawDistance on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetNameTagDrawDistance(float distance));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DisableNameTagLOS">DisableNameTagLOS on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DisableNameTagLOS());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/LimitGlobalChatRadius">LimitGlobalChatRadius on open.mp</a>
 */
SAMPGDK_NATIVE(bool, LimitGlobalChatRadius(float chat_radius));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/LimitPlayerMarkerRadius">LimitPlayerMarkerRadius on open.mp</a>
 */
SAMPGDK_NATIVE(bool, LimitPlayerMarkerRadius(float marker_radius));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ConnectNPC">ConnectNPC on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ConnectNPC(const char * name, const char * script));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerNPC">IsPlayerNPC on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerNPC(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerAdmin">IsPlayerAdmin on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerAdmin(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Kick">Kick on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Kick(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Ban">Ban on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Ban(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/BanEx">BanEx on open.mp</a>
 */
SAMPGDK_NATIVE(bool, BanEx(int playerid, const char * reason));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SendRconCommand">SendRconCommand on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SendRconCommand(const char * command));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerNetworkStats">GetPlayerNetworkStats on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerNetworkStats(int playerid, char * retstr, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetNetworkStats">GetNetworkStats on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetNetworkStats(char * retstr, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerVersion">GetPlayerVersion on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerVersion(int playerid, char * version, int len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/BlockIpAddress">BlockIpAddress on open.mp</a>
 */
SAMPGDK_NATIVE(bool, BlockIpAddress(const char * ip_address, int timems));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/UnBlockIpAddress">UnBlockIpAddress on open.mp</a>
 */
SAMPGDK_NATIVE(bool, UnBlockIpAddress(const char * ip_address));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetServerVarAsString">GetServerVarAsString on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetServerVarAsString(const char * varname, char * value, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetServerVarAsInt">GetServerVarAsInt on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetServerVarAsInt(const char * varname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetServerVarAsBool">GetServerVarAsBool on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetServerVarAsBool(const char * varname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetConsoleVarAsString">GetConsoleVarAsString on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetConsoleVarAsString(const char * varname, char * buffer, int len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetConsoleVarAsInt">GetConsoleVarAsInt on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetConsoleVarAsInt(const char * varname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetConsoleVarAsBool">GetConsoleVarAsBool on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetConsoleVarAsBool(const char * varname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetServerTickRate">GetServerTickRate on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetServerTickRate());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NetStats_GetConnectedTime">NetStats_GetConnectedTime on open.mp</a>
 */
SAMPGDK_NATIVE(int, NetStats_GetConnectedTime(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NetStats_MessagesReceived">NetStats_MessagesReceived on open.mp</a>
 */
SAMPGDK_NATIVE(int, NetStats_MessagesReceived(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NetStats_BytesReceived">NetStats_BytesReceived on open.mp</a>
 */
SAMPGDK_NATIVE(int, NetStats_BytesReceived(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NetStats_MessagesSent">NetStats_MessagesSent on open.mp</a>
 */
SAMPGDK_NATIVE(int, NetStats_MessagesSent(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NetStats_BytesSent">NetStats_BytesSent on open.mp</a>
 */
SAMPGDK_NATIVE(int, NetStats_BytesSent(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NetStats_MessagesRecvPerSecond">NetStats_MessagesRecvPerSecond on open.mp</a>
 */
SAMPGDK_NATIVE(int, NetStats_MessagesRecvPerSecond(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NetStats_PacketLossPercent">NetStats_PacketLossPercent on open.mp</a>
 */
SAMPGDK_NATIVE(float, NetStats_PacketLossPercent(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NetStats_ConnectionStatus">NetStats_ConnectionStatus on open.mp</a>
 */
SAMPGDK_NATIVE(int, NetStats_ConnectionStatus(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NetStats_GetIpPort">NetStats_GetIpPort on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NetStats_GetIpPort(int playerid, char * ip_port, int ip_port_len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CreateMenu">CreateMenu on open.mp</a>
 */
SAMPGDK_NATIVE(int, CreateMenu(const char * title, int columns, float x, float y, float col1width, float col2width));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DestroyMenu">DestroyMenu on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DestroyMenu(int menuid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AddMenuItem">AddMenuItem on open.mp</a>
 */
SAMPGDK_NATIVE(int, AddMenuItem(int menuid, int column, const char * menutext));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetMenuColumnHeader">SetMenuColumnHeader on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetMenuColumnHeader(int menuid, int column, const char * columnheader));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ShowMenuForPlayer">ShowMenuForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ShowMenuForPlayer(int menuid, int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/HideMenuForPlayer">HideMenuForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, HideMenuForPlayer(int menuid, int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidMenu">IsValidMenu on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidMenu(int menuid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DisableMenu">DisableMenu on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DisableMenu(int menuid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DisableMenuRow">DisableMenuRow on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DisableMenuRow(int menuid, int row));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerMenu">GetPlayerMenu on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerMenu(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawCreate">TextDrawCreate on open.mp</a>
 */
SAMPGDK_NATIVE(int, TextDrawCreate(float x, float y, const char * text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawDestroy">TextDrawDestroy on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawDestroy(int text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawLetterSize">TextDrawLetterSize on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawLetterSize(int text, float x, float y));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawTextSize">TextDrawTextSize on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawTextSize(int text, float x, float y));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawAlignment">TextDrawAlignment on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawAlignment(int text, int alignment));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawColor">TextDrawColor on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawColor(int text, int color));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawUseBox">TextDrawUseBox on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawUseBox(int text, bool use));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawBoxColor">TextDrawBoxColor on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawBoxColor(int text, int color));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawSetShadow">TextDrawSetShadow on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawSetShadow(int text, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawSetOutline">TextDrawSetOutline on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawSetOutline(int text, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawBackgroundColor">TextDrawBackgroundColor on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawBackgroundColor(int text, int color));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawFont">TextDrawFont on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawFont(int text, int font));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawSetProportional">TextDrawSetProportional on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawSetProportional(int text, bool set));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawSetSelectable">TextDrawSetSelectable on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawSetSelectable(int text, bool set));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawShowForPlayer">TextDrawShowForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawShowForPlayer(int playerid, int text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawHideForPlayer">TextDrawHideForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawHideForPlayer(int playerid, int text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawShowForAll">TextDrawShowForAll on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawShowForAll(int text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawHideForAll">TextDrawHideForAll on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawHideForAll(int text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawSetString">TextDrawSetString on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawSetString(int text, const char * string));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawSetPreviewModel">TextDrawSetPreviewModel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawSetPreviewModel(int text, int modelindex));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawSetPreviewRot">TextDrawSetPreviewRot on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawSetPreviewRot(int text, float fRotX, float fRotY, float fRotZ, float fZoom));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawSetPreviewVehCol">TextDrawSetPreviewVehCol on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawSetPreviewVehCol(int text, int color1, int color2));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SelectTextDraw">SelectTextDraw on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SelectTextDraw(int playerid, int hovercolor));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CancelSelectTextDraw">CancelSelectTextDraw on open.mp</a>
 */
SAMPGDK_NATIVE(bool, CancelSelectTextDraw(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneCreate">GangZoneCreate on open.mp</a>
 */
SAMPGDK_NATIVE(int, GangZoneCreate(float minx, float miny, float maxx, float maxy));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneDestroy">GangZoneDestroy on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GangZoneDestroy(int zone));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneShowForPlayer">GangZoneShowForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GangZoneShowForPlayer(int playerid, int zone, int color));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneShowForAll">GangZoneShowForAll on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GangZoneShowForAll(int zone, int color));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneHideForPlayer">GangZoneHideForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GangZoneHideForPlayer(int playerid, int zone));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneHideForAll">GangZoneHideForAll on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GangZoneHideForAll(int zone));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneFlashForPlayer">GangZoneFlashForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GangZoneFlashForPlayer(int playerid, int zone, int flashcolor));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneFlashForAll">GangZoneFlashForAll on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GangZoneFlashForAll(int zone, int flashcolor));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneStopFlashForPlayer">GangZoneStopFlashForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GangZoneStopFlashForPlayer(int playerid, int zone));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneStopFlashForAll">GangZoneStopFlashForAll on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GangZoneStopFlashForAll(int zone));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Create3DTextLabel">Create3DTextLabel on open.mp</a>
 */
SAMPGDK_NATIVE(int, Create3DTextLabel(const char * text, int color, float x, float y, float z, float DrawDistance, int virtualworld, bool testLOS));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Delete3DTextLabel">Delete3DTextLabel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Delete3DTextLabel(int id));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Attach3DTextLabelToPlayer">Attach3DTextLabelToPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Attach3DTextLabelToPlayer(int id, int playerid, float OffsetX, float OffsetY, float OffsetZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Attach3DTextLabelToVehicle">Attach3DTextLabelToVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Attach3DTextLabelToVehicle(int id, int vehicleid, float OffsetX, float OffsetY, float OffsetZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Update3DTextLabelText">Update3DTextLabelText on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Update3DTextLabelText(int id, int color, const char * text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CreatePlayer3DTextLabel">CreatePlayer3DTextLabel on open.mp</a>
 */
SAMPGDK_NATIVE(int, CreatePlayer3DTextLabel(int playerid, const char * text, int color, float x, float y, float z, float DrawDistance, int attachedplayer, int attachedvehicle, bool testLOS));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DeletePlayer3DTextLabel">DeletePlayer3DTextLabel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DeletePlayer3DTextLabel(int playerid, int id));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/UpdatePlayer3DTextLabelText">UpdatePlayer3DTextLabelText on open.mp</a>
 */
SAMPGDK_NATIVE(bool, UpdatePlayer3DTextLabelText(int playerid, int id, int color, const char * text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ShowPlayerDialog">ShowPlayerDialog on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ShowPlayerDialog(int playerid, int dialogid, int style, const char * caption, const char * info, const char * button1, const char * button2));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetTimer">SetTimer on open.mp</a>
 */
SAMPGDK_NATIVE(int, SetTimer(int interval, bool repeat, TimerCallback callback, void * param));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/KillTimer">KillTimer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, KillTimer(int timerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/gpci">gpci on open.mp</a>
 */
SAMPGDK_NATIVE(bool, gpci(int playerid, char * buffer, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AddCharModel">AddCharModel on open.mp</a>
 */
SAMPGDK_NATIVE(int, AddCharModel(int baseid, int newid, const char * dffname, const char * txdname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AddSimpleModel">AddSimpleModel on open.mp</a>
 */
SAMPGDK_NATIVE(int, AddSimpleModel(int virtualworld, int baseid, int newid, const char * dffname, const char * txdname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AddSimpleModelTimed">AddSimpleModelTimed on open.mp</a>
 */
SAMPGDK_NATIVE(int, AddSimpleModelTimed(int virtualworld, int baseid, int newid, const char * dffname, const char * txdname, int timeon, int timeoff));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/FindModelFileNameFromCRC">FindModelFileNameFromCRC on open.mp</a>
 */
SAMPGDK_NATIVE(bool, FindModelFileNameFromCRC(int crc, char * model_str, int model_str_len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/FindTextureFileNameFromCRC">FindTextureFileNameFromCRC on open.mp</a>
 */
SAMPGDK_NATIVE(bool, FindTextureFileNameFromCRC(int crc, char * texture_str, int texture_str_len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/RedirectDownload">RedirectDownload on open.mp</a>
 */
SAMPGDK_NATIVE(bool, RedirectDownload(int playerid, const char * url));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool SendClientMessage(int playerid, int color, const char * message) {
  return sampgdk_SendClientMessage(playerid, color, message);
}

inline bool SendClientMessageToAll(int color, const char * message) {
  return sampgdk_SendClientMessageToAll(color, message);
}

inline bool SendPlayerMessageToPlayer(int playerid, int senderid, const char * message) {
  return sampgdk_SendPlayerMessageToPlayer(playerid, senderid, message);
}

inline bool SendPlayerMessageToAll(int senderid, const char * message) {
  return sampgdk_SendPlayerMessageToAll(senderid, message);
}

inline bool SendDeathMessage(int killer, int killee, int weapon) {
  return sampgdk_SendDeathMessage(killer, killee, weapon);
}

inline bool SendDeathMessageToPlayer(int playerid, int killer, int killee, int weapon) {
  return sampgdk_SendDeathMessageToPlayer(playerid, killer, killee, weapon);
}

inline bool GameTextForAll(const char * text, int time, int style) {
  return sampgdk_GameTextForAll(text, time, style);
}

inline bool GameTextForPlayer(int playerid, const char * text, int time, int style) {
  return sampgdk_GameTextForPlayer(playerid, text, time, style);
}

inline int GetTickCount() {
  return sampgdk_GetTickCount();
}

inline int GetMaxPlayers() {
  return sampgdk_GetMaxPlayers();
}

inline float VectorSize(float x, float y, float z) {
  return sampgdk_VectorSize(x, y, z);
}

inline int GetPlayerPoolSize() {
  return sampgdk_GetPlayerPoolSize();
}

inline int GetVehiclePoolSize() {
  return sampgdk_GetVehiclePoolSize();
}

inline int GetActorPoolSize() {
  return sampgdk_GetActorPoolSize();
}

inline bool SHA256_PassHash(const char * password, const char * salt, char * ret_hash, int ret_hash_len) {
  return sampgdk_SHA256_PassHash(password, salt, ret_hash, ret_hash_len);
}

inline bool SetSVarInt(const char * varname, int int_value) {
  return sampgdk_SetSVarInt(varname, int_value);
}

inline int GetSVarInt(const char * varname) {
  return sampgdk_GetSVarInt(varname);
}

inline bool SetSVarString(const char * varname, const char * string_value) {
  return sampgdk_SetSVarString(varname, string_value);
}

inline bool GetSVarString(const char * varname, char * string_return, int len) {
  return sampgdk_GetSVarString(varname, string_return, len);
}

inline bool SetSVarFloat(const char * varname, float float_value) {
  return sampgdk_SetSVarFloat(varname, float_value);
}

inline float GetSVarFloat(const char * varname) {
  return sampgdk_GetSVarFloat(varname);
}

inline bool DeleteSVar(const char * varname) {
  return sampgdk_DeleteSVar(varname);
}

inline int GetSVarsUpperIndex() {
  return sampgdk_GetSVarsUpperIndex();
}

inline bool GetSVarNameAtIndex(int index, char * ret_varname, int ret_len) {
  return sampgdk_GetSVarNameAtIndex(index, ret_varname, ret_len);
}

inline int GetSVarType(const char * varname) {
  return sampgdk_GetSVarType(varname);
}

inline bool SetGameModeText(const char * text) {
  return sampgdk_SetGameModeText(text);
}

inline bool SetTeamCount(int count) {
  return sampgdk_SetTeamCount(count);
}

inline int AddPlayerClass(int modelid, float spawn_x, float spawn_y, float spawn_z, float z_angle, int weapon1, int weapon1_ammo, int weapon2, int weapon2_ammo, int weapon3, int weapon3_ammo) {
  return sampgdk_AddPlayerClass(modelid, spawn_x, spawn_y, spawn_z, z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
}

inline int AddPlayerClassEx(int teamid, int modelid, float spawn_x, float spawn_y, float spawn_z, float z_angle, int weapon1, int weapon1_ammo, int weapon2, int weapon2_ammo, int weapon3, int weapon3_ammo) {
  return sampgdk_AddPlayerClassEx(teamid, modelid, spawn_x, spawn_y, spawn_z, z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
}

inline int AddStaticVehicle(int modelid, float spawn_x, float spawn_y, float spawn_z, float z_angle, int color1, int color2) {
  return sampgdk_AddStaticVehicle(modelid, spawn_x, spawn_y, spawn_z, z_angle, color1, color2);
}

inline int AddStaticVehicleEx(int modelid, float spawn_x, float spawn_y, float spawn_z, float z_angle, int color1, int color2, int respawn_delay, bool addsiren = false) {
  return sampgdk_AddStaticVehicleEx(modelid, spawn_x, spawn_y, spawn_z, z_angle, color1, color2, respawn_delay, addsiren);
}

inline int AddStaticPickup(int model, int type, float x, float y, float z, int virtualworld = 0) {
  return sampgdk_AddStaticPickup(model, type, x, y, z, virtualworld);
}

inline int CreatePickup(int model, int type, float x, float y, float z, int virtualworld = 0) {
  return sampgdk_CreatePickup(model, type, x, y, z, virtualworld);
}

inline bool DestroyPickup(int pickup) {
  return sampgdk_DestroyPickup(pickup);
}

inline bool ShowNameTags(bool show) {
  return sampgdk_ShowNameTags(show);
}

inline bool ShowPlayerMarkers(int mode) {
  return sampgdk_ShowPlayerMarkers(mode);
}

inline bool GameModeExit() {
  return sampgdk_GameModeExit();
}

inline bool SetWorldTime(int hour) {
  return sampgdk_SetWorldTime(hour);
}

inline bool GetWeaponName(int weaponid, char * name, int size) {
  return sampgdk_GetWeaponName(weaponid, name, size);
}

inline bool EnableTirePopping(bool enable) {
  return sampgdk_EnableTirePopping(enable);
}

inline bool EnableVehicleFriendlyFire() {
  return sampgdk_EnableVehicleFriendlyFire();
}

inline bool AllowInteriorWeapons(bool allow) {
  return sampgdk_AllowInteriorWeapons(allow);
}

inline bool SetWeather(int weatherid) {
  return sampgdk_SetWeather(weatherid);
}

inline bool SetGravity(float gravity) {
  return sampgdk_SetGravity(gravity);
}

inline float GetGravity() {
  return sampgdk_GetGravity();
}

inline bool AllowAdminTeleport(bool allow) {
  return sampgdk_AllowAdminTeleport(allow);
}

inline bool SetDeathDropAmount(int amount) {
  return sampgdk_SetDeathDropAmount(amount);
}

inline bool CreateExplosion(float x, float y, float z, int type, float radius) {
  return sampgdk_CreateExplosion(x, y, z, type, radius);
}

inline bool EnableZoneNames(bool enable) {
  return sampgdk_EnableZoneNames(enable);
}

inline bool UsePlayerPedAnims() {
  return sampgdk_UsePlayerPedAnims();
}

inline bool DisableInteriorEnterExits() {
  return sampgdk_DisableInteriorEnterExits();
}

inline bool SetNameTagDrawDistance(float distance) {
  return sampgdk_SetNameTagDrawDistance(distance);
}

inline bool DisableNameTagLOS() {
  return sampgdk_DisableNameTagLOS();
}

inline bool LimitGlobalChatRadius(float chat_radius) {
  return sampgdk_LimitGlobalChatRadius(chat_radius);
}

inline bool LimitPlayerMarkerRadius(float marker_radius) {
  return sampgdk_LimitPlayerMarkerRadius(marker_radius);
}

inline bool ConnectNPC(const char * name, const char * script) {
  return sampgdk_ConnectNPC(name, script);
}

inline bool IsPlayerNPC(int playerid) {
  return sampgdk_IsPlayerNPC(playerid);
}

inline bool IsPlayerAdmin(int playerid) {
  return sampgdk_IsPlayerAdmin(playerid);
}

inline bool Kick(int playerid) {
  return sampgdk_Kick(playerid);
}

inline bool Ban(int playerid) {
  return sampgdk_Ban(playerid);
}

inline bool BanEx(int playerid, const char * reason) {
  return sampgdk_BanEx(playerid, reason);
}

inline bool SendRconCommand(const char * command) {
  return sampgdk_SendRconCommand(command);
}

inline bool GetPlayerNetworkStats(int playerid, char * retstr, int size) {
  return sampgdk_GetPlayerNetworkStats(playerid, retstr, size);
}

inline bool GetNetworkStats(char * retstr, int size) {
  return sampgdk_GetNetworkStats(retstr, size);
}

inline bool GetPlayerVersion(int playerid, char * version, int len) {
  return sampgdk_GetPlayerVersion(playerid, version, len);
}

inline bool BlockIpAddress(const char * ip_address, int timems) {
  return sampgdk_BlockIpAddress(ip_address, timems);
}

inline bool UnBlockIpAddress(const char * ip_address) {
  return sampgdk_UnBlockIpAddress(ip_address);
}

inline bool GetServerVarAsString(const char * varname, char * value, int size) {
  return sampgdk_GetServerVarAsString(varname, value, size);
}

inline int GetServerVarAsInt(const char * varname) {
  return sampgdk_GetServerVarAsInt(varname);
}

inline bool GetServerVarAsBool(const char * varname) {
  return sampgdk_GetServerVarAsBool(varname);
}

inline bool GetConsoleVarAsString(const char * varname, char * buffer, int len) {
  return sampgdk_GetConsoleVarAsString(varname, buffer, len);
}

inline int GetConsoleVarAsInt(const char * varname) {
  return sampgdk_GetConsoleVarAsInt(varname);
}

inline bool GetConsoleVarAsBool(const char * varname) {
  return sampgdk_GetConsoleVarAsBool(varname);
}

inline int GetServerTickRate() {
  return sampgdk_GetServerTickRate();
}

inline int NetStats_GetConnectedTime(int playerid) {
  return sampgdk_NetStats_GetConnectedTime(playerid);
}

inline int NetStats_MessagesReceived(int playerid) {
  return sampgdk_NetStats_MessagesReceived(playerid);
}

inline int NetStats_BytesReceived(int playerid) {
  return sampgdk_NetStats_BytesReceived(playerid);
}

inline int NetStats_MessagesSent(int playerid) {
  return sampgdk_NetStats_MessagesSent(playerid);
}

inline int NetStats_BytesSent(int playerid) {
  return sampgdk_NetStats_BytesSent(playerid);
}

inline int NetStats_MessagesRecvPerSecond(int playerid) {
  return sampgdk_NetStats_MessagesRecvPerSecond(playerid);
}

inline float NetStats_PacketLossPercent(int playerid) {
  return sampgdk_NetStats_PacketLossPercent(playerid);
}

inline int NetStats_ConnectionStatus(int playerid) {
  return sampgdk_NetStats_ConnectionStatus(playerid);
}

inline bool NetStats_GetIpPort(int playerid, char * ip_port, int ip_port_len) {
  return sampgdk_NetStats_GetIpPort(playerid, ip_port, ip_port_len);
}

inline int CreateMenu(const char * title, int columns, float x, float y, float col1width, float col2width = 0.0) {
  return sampgdk_CreateMenu(title, columns, x, y, col1width, col2width);
}

inline bool DestroyMenu(int menuid) {
  return sampgdk_DestroyMenu(menuid);
}

inline int AddMenuItem(int menuid, int column, const char * menutext) {
  return sampgdk_AddMenuItem(menuid, column, menutext);
}

inline bool SetMenuColumnHeader(int menuid, int column, const char * columnheader) {
  return sampgdk_SetMenuColumnHeader(menuid, column, columnheader);
}

inline bool ShowMenuForPlayer(int menuid, int playerid) {
  return sampgdk_ShowMenuForPlayer(menuid, playerid);
}

inline bool HideMenuForPlayer(int menuid, int playerid) {
  return sampgdk_HideMenuForPlayer(menuid, playerid);
}

inline bool IsValidMenu(int menuid) {
  return sampgdk_IsValidMenu(menuid);
}

inline bool DisableMenu(int menuid) {
  return sampgdk_DisableMenu(menuid);
}

inline bool DisableMenuRow(int menuid, int row) {
  return sampgdk_DisableMenuRow(menuid, row);
}

inline int GetPlayerMenu(int playerid) {
  return sampgdk_GetPlayerMenu(playerid);
}

inline int TextDrawCreate(float x, float y, const char * text) {
  return sampgdk_TextDrawCreate(x, y, text);
}

inline bool TextDrawDestroy(int text) {
  return sampgdk_TextDrawDestroy(text);
}

inline bool TextDrawLetterSize(int text, float x, float y) {
  return sampgdk_TextDrawLetterSize(text, x, y);
}

inline bool TextDrawTextSize(int text, float x, float y) {
  return sampgdk_TextDrawTextSize(text, x, y);
}

inline bool TextDrawAlignment(int text, int alignment) {
  return sampgdk_TextDrawAlignment(text, alignment);
}

inline bool TextDrawColor(int text, int color) {
  return sampgdk_TextDrawColor(text, color);
}

inline bool TextDrawUseBox(int text, bool use) {
  return sampgdk_TextDrawUseBox(text, use);
}

inline bool TextDrawBoxColor(int text, int color) {
  return sampgdk_TextDrawBoxColor(text, color);
}

inline bool TextDrawSetShadow(int text, int size) {
  return sampgdk_TextDrawSetShadow(text, size);
}

inline bool TextDrawSetOutline(int text, int size) {
  return sampgdk_TextDrawSetOutline(text, size);
}

inline bool TextDrawBackgroundColor(int text, int color) {
  return sampgdk_TextDrawBackgroundColor(text, color);
}

inline bool TextDrawFont(int text, int font) {
  return sampgdk_TextDrawFont(text, font);
}

inline bool TextDrawSetProportional(int text, bool set) {
  return sampgdk_TextDrawSetProportional(text, set);
}

inline bool TextDrawSetSelectable(int text, bool set) {
  return sampgdk_TextDrawSetSelectable(text, set);
}

inline bool TextDrawShowForPlayer(int playerid, int text) {
  return sampgdk_TextDrawShowForPlayer(playerid, text);
}

inline bool TextDrawHideForPlayer(int playerid, int text) {
  return sampgdk_TextDrawHideForPlayer(playerid, text);
}

inline bool TextDrawShowForAll(int text) {
  return sampgdk_TextDrawShowForAll(text);
}

inline bool TextDrawHideForAll(int text) {
  return sampgdk_TextDrawHideForAll(text);
}

inline bool TextDrawSetString(int text, const char * string) {
  return sampgdk_TextDrawSetString(text, string);
}

inline bool TextDrawSetPreviewModel(int text, int modelindex) {
  return sampgdk_TextDrawSetPreviewModel(text, modelindex);
}

inline bool TextDrawSetPreviewRot(int text, float fRotX, float fRotY, float fRotZ, float fZoom = 1.0) {
  return sampgdk_TextDrawSetPreviewRot(text, fRotX, fRotY, fRotZ, fZoom);
}

inline bool TextDrawSetPreviewVehCol(int text, int color1, int color2) {
  return sampgdk_TextDrawSetPreviewVehCol(text, color1, color2);
}

inline bool SelectTextDraw(int playerid, int hovercolor) {
  return sampgdk_SelectTextDraw(playerid, hovercolor);
}

inline bool CancelSelectTextDraw(int playerid) {
  return sampgdk_CancelSelectTextDraw(playerid);
}

inline int GangZoneCreate(float minx, float miny, float maxx, float maxy) {
  return sampgdk_GangZoneCreate(minx, miny, maxx, maxy);
}

inline bool GangZoneDestroy(int zone) {
  return sampgdk_GangZoneDestroy(zone);
}

inline bool GangZoneShowForPlayer(int playerid, int zone, int color) {
  return sampgdk_GangZoneShowForPlayer(playerid, zone, color);
}

inline bool GangZoneShowForAll(int zone, int color) {
  return sampgdk_GangZoneShowForAll(zone, color);
}

inline bool GangZoneHideForPlayer(int playerid, int zone) {
  return sampgdk_GangZoneHideForPlayer(playerid, zone);
}

inline bool GangZoneHideForAll(int zone) {
  return sampgdk_GangZoneHideForAll(zone);
}

inline bool GangZoneFlashForPlayer(int playerid, int zone, int flashcolor) {
  return sampgdk_GangZoneFlashForPlayer(playerid, zone, flashcolor);
}

inline bool GangZoneFlashForAll(int zone, int flashcolor) {
  return sampgdk_GangZoneFlashForAll(zone, flashcolor);
}

inline bool GangZoneStopFlashForPlayer(int playerid, int zone) {
  return sampgdk_GangZoneStopFlashForPlayer(playerid, zone);
}

inline bool GangZoneStopFlashForAll(int zone) {
  return sampgdk_GangZoneStopFlashForAll(zone);
}

inline int Create3DTextLabel(const char * text, int color, float x, float y, float z, float DrawDistance, int virtualworld, bool testLOS = false) {
  return sampgdk_Create3DTextLabel(text, color, x, y, z, DrawDistance, virtualworld, testLOS);
}

inline bool Delete3DTextLabel(int id) {
  return sampgdk_Delete3DTextLabel(id);
}

inline bool Attach3DTextLabelToPlayer(int id, int playerid, float OffsetX, float OffsetY, float OffsetZ) {
  return sampgdk_Attach3DTextLabelToPlayer(id, playerid, OffsetX, OffsetY, OffsetZ);
}

inline bool Attach3DTextLabelToVehicle(int id, int vehicleid, float OffsetX, float OffsetY, float OffsetZ) {
  return sampgdk_Attach3DTextLabelToVehicle(id, vehicleid, OffsetX, OffsetY, OffsetZ);
}

inline bool Update3DTextLabelText(int id, int color, const char * text) {
  return sampgdk_Update3DTextLabelText(id, color, text);
}

inline int CreatePlayer3DTextLabel(int playerid, const char * text, int color, float x, float y, float z, float DrawDistance, int attachedplayer = INVALID_PLAYER_ID, int attachedvehicle = INVALID_VEHICLE_ID, bool testLOS = false) {
  return sampgdk_CreatePlayer3DTextLabel(playerid, text, color, x, y, z, DrawDistance, attachedplayer, attachedvehicle, testLOS);
}

inline bool DeletePlayer3DTextLabel(int playerid, int id) {
  return sampgdk_DeletePlayer3DTextLabel(playerid, id);
}

inline bool UpdatePlayer3DTextLabelText(int playerid, int id, int color, const char * text) {
  return sampgdk_UpdatePlayer3DTextLabelText(playerid, id, color, text);
}

inline bool ShowPlayerDialog(int playerid, int dialogid, int style, const char * caption, const char * info, const char * button1, const char * button2) {
  return sampgdk_ShowPlayerDialog(playerid, dialogid, style, caption, info, button1, button2);
}

inline int SetTimer(int interval, bool repeat, TimerCallback callback, void * param) {
  return sampgdk_SetTimer(interval, repeat, callback, param);
}

inline bool KillTimer(int timerid) {
  return sampgdk_KillTimer(timerid);
}

inline bool gpci(int playerid, char * buffer, int size) {
  return sampgdk_gpci(playerid, buffer, size);
}

inline int AddCharModel(int baseid, int newid, const char * dffname, const char * txdname) {
  return sampgdk_AddCharModel(baseid, newid, dffname, txdname);
}

inline int AddSimpleModel(int virtualworld, int baseid, int newid, const char * dffname, const char * txdname) {
  return sampgdk_AddSimpleModel(virtualworld, baseid, newid, dffname, txdname);
}

inline int AddSimpleModelTimed(int virtualworld, int baseid, int newid, const char * dffname, const char * txdname, int timeon, int timeoff) {
  return sampgdk_AddSimpleModelTimed(virtualworld, baseid, newid, dffname, txdname, timeon, timeoff);
}

inline bool FindModelFileNameFromCRC(int crc, char * model_str, int model_str_len) {
  return sampgdk_FindModelFileNameFromCRC(crc, model_str, model_str_len);
}

inline bool FindTextureFileNameFromCRC(int crc, char * texture_str, int texture_str_len) {
  return sampgdk_FindTextureFileNameFromCRC(crc, texture_str, texture_str_len);
}

inline bool RedirectDownload(int playerid, const char * url) {
  return sampgdk_RedirectDownload(playerid, url);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  SendClientMessage
#define SendClientMessage sampgdk_SendClientMessage

#undef  SendClientMessageToAll
#define SendClientMessageToAll sampgdk_SendClientMessageToAll

#undef  SendPlayerMessageToPlayer
#define SendPlayerMessageToPlayer sampgdk_SendPlayerMessageToPlayer

#undef  SendPlayerMessageToAll
#define SendPlayerMessageToAll sampgdk_SendPlayerMessageToAll

#undef  SendDeathMessage
#define SendDeathMessage sampgdk_SendDeathMessage

#undef  SendDeathMessageToPlayer
#define SendDeathMessageToPlayer sampgdk_SendDeathMessageToPlayer

#undef  GameTextForAll
#define GameTextForAll sampgdk_GameTextForAll

#undef  GameTextForPlayer
#define GameTextForPlayer sampgdk_GameTextForPlayer

#undef  GetTickCount
#define GetTickCount sampgdk_GetTickCount

#undef  GetMaxPlayers
#define GetMaxPlayers sampgdk_GetMaxPlayers

#undef  VectorSize
#define VectorSize sampgdk_VectorSize

#undef  GetPlayerPoolSize
#define GetPlayerPoolSize sampgdk_GetPlayerPoolSize

#undef  GetVehiclePoolSize
#define GetVehiclePoolSize sampgdk_GetVehiclePoolSize

#undef  GetActorPoolSize
#define GetActorPoolSize sampgdk_GetActorPoolSize

#undef  SHA256_PassHash
#define SHA256_PassHash sampgdk_SHA256_PassHash

#undef  SetSVarInt
#define SetSVarInt sampgdk_SetSVarInt

#undef  GetSVarInt
#define GetSVarInt sampgdk_GetSVarInt

#undef  SetSVarString
#define SetSVarString sampgdk_SetSVarString

#undef  GetSVarString
#define GetSVarString sampgdk_GetSVarString

#undef  SetSVarFloat
#define SetSVarFloat sampgdk_SetSVarFloat

#undef  GetSVarFloat
#define GetSVarFloat sampgdk_GetSVarFloat

#undef  DeleteSVar
#define DeleteSVar sampgdk_DeleteSVar

#undef  GetSVarsUpperIndex
#define GetSVarsUpperIndex sampgdk_GetSVarsUpperIndex

#undef  GetSVarNameAtIndex
#define GetSVarNameAtIndex sampgdk_GetSVarNameAtIndex

#undef  GetSVarType
#define GetSVarType sampgdk_GetSVarType

#undef  SetGameModeText
#define SetGameModeText sampgdk_SetGameModeText

#undef  SetTeamCount
#define SetTeamCount sampgdk_SetTeamCount

#undef  AddPlayerClass
#define AddPlayerClass sampgdk_AddPlayerClass

#undef  AddPlayerClassEx
#define AddPlayerClassEx sampgdk_AddPlayerClassEx

#undef  AddStaticVehicle
#define AddStaticVehicle sampgdk_AddStaticVehicle

#undef  AddStaticVehicleEx
#define AddStaticVehicleEx sampgdk_AddStaticVehicleEx

#undef  AddStaticPickup
#define AddStaticPickup sampgdk_AddStaticPickup

#undef  CreatePickup
#define CreatePickup sampgdk_CreatePickup

#undef  DestroyPickup
#define DestroyPickup sampgdk_DestroyPickup

#undef  ShowNameTags
#define ShowNameTags sampgdk_ShowNameTags

#undef  ShowPlayerMarkers
#define ShowPlayerMarkers sampgdk_ShowPlayerMarkers

#undef  GameModeExit
#define GameModeExit sampgdk_GameModeExit

#undef  SetWorldTime
#define SetWorldTime sampgdk_SetWorldTime

#undef  GetWeaponName
#define GetWeaponName sampgdk_GetWeaponName

#undef  EnableTirePopping
#define EnableTirePopping sampgdk_EnableTirePopping

#undef  EnableVehicleFriendlyFire
#define EnableVehicleFriendlyFire sampgdk_EnableVehicleFriendlyFire

#undef  AllowInteriorWeapons
#define AllowInteriorWeapons sampgdk_AllowInteriorWeapons

#undef  SetWeather
#define SetWeather sampgdk_SetWeather

#undef  SetGravity
#define SetGravity sampgdk_SetGravity

#undef  GetGravity
#define GetGravity sampgdk_GetGravity

#undef  AllowAdminTeleport
#define AllowAdminTeleport sampgdk_AllowAdminTeleport

#undef  SetDeathDropAmount
#define SetDeathDropAmount sampgdk_SetDeathDropAmount

#undef  CreateExplosion
#define CreateExplosion sampgdk_CreateExplosion

#undef  EnableZoneNames
#define EnableZoneNames sampgdk_EnableZoneNames

#undef  UsePlayerPedAnims
#define UsePlayerPedAnims sampgdk_UsePlayerPedAnims

#undef  DisableInteriorEnterExits
#define DisableInteriorEnterExits sampgdk_DisableInteriorEnterExits

#undef  SetNameTagDrawDistance
#define SetNameTagDrawDistance sampgdk_SetNameTagDrawDistance

#undef  DisableNameTagLOS
#define DisableNameTagLOS sampgdk_DisableNameTagLOS

#undef  LimitGlobalChatRadius
#define LimitGlobalChatRadius sampgdk_LimitGlobalChatRadius

#undef  LimitPlayerMarkerRadius
#define LimitPlayerMarkerRadius sampgdk_LimitPlayerMarkerRadius

#undef  ConnectNPC
#define ConnectNPC sampgdk_ConnectNPC

#undef  IsPlayerNPC
#define IsPlayerNPC sampgdk_IsPlayerNPC

#undef  IsPlayerAdmin
#define IsPlayerAdmin sampgdk_IsPlayerAdmin

#undef  Kick
#define Kick sampgdk_Kick

#undef  Ban
#define Ban sampgdk_Ban

#undef  BanEx
#define BanEx sampgdk_BanEx

#undef  SendRconCommand
#define SendRconCommand sampgdk_SendRconCommand

#undef  GetPlayerNetworkStats
#define GetPlayerNetworkStats sampgdk_GetPlayerNetworkStats

#undef  GetNetworkStats
#define GetNetworkStats sampgdk_GetNetworkStats

#undef  GetPlayerVersion
#define GetPlayerVersion sampgdk_GetPlayerVersion

#undef  BlockIpAddress
#define BlockIpAddress sampgdk_BlockIpAddress

#undef  UnBlockIpAddress
#define UnBlockIpAddress sampgdk_UnBlockIpAddress

#undef  GetServerVarAsString
#define GetServerVarAsString sampgdk_GetServerVarAsString

#undef  GetServerVarAsInt
#define GetServerVarAsInt sampgdk_GetServerVarAsInt

#undef  GetServerVarAsBool
#define GetServerVarAsBool sampgdk_GetServerVarAsBool

#undef  GetConsoleVarAsString
#define GetConsoleVarAsString sampgdk_GetConsoleVarAsString

#undef  GetConsoleVarAsInt
#define GetConsoleVarAsInt sampgdk_GetConsoleVarAsInt

#undef  GetConsoleVarAsBool
#define GetConsoleVarAsBool sampgdk_GetConsoleVarAsBool

#undef  GetServerTickRate
#define GetServerTickRate sampgdk_GetServerTickRate

#undef  NetStats_GetConnectedTime
#define NetStats_GetConnectedTime sampgdk_NetStats_GetConnectedTime

#undef  NetStats_MessagesReceived
#define NetStats_MessagesReceived sampgdk_NetStats_MessagesReceived

#undef  NetStats_BytesReceived
#define NetStats_BytesReceived sampgdk_NetStats_BytesReceived

#undef  NetStats_MessagesSent
#define NetStats_MessagesSent sampgdk_NetStats_MessagesSent

#undef  NetStats_BytesSent
#define NetStats_BytesSent sampgdk_NetStats_BytesSent

#undef  NetStats_MessagesRecvPerSecond
#define NetStats_MessagesRecvPerSecond sampgdk_NetStats_MessagesRecvPerSecond

#undef  NetStats_PacketLossPercent
#define NetStats_PacketLossPercent sampgdk_NetStats_PacketLossPercent

#undef  NetStats_ConnectionStatus
#define NetStats_ConnectionStatus sampgdk_NetStats_ConnectionStatus

#undef  NetStats_GetIpPort
#define NetStats_GetIpPort sampgdk_NetStats_GetIpPort

#undef  CreateMenu
#define CreateMenu sampgdk_CreateMenu

#undef  DestroyMenu
#define DestroyMenu sampgdk_DestroyMenu

#undef  AddMenuItem
#define AddMenuItem sampgdk_AddMenuItem

#undef  SetMenuColumnHeader
#define SetMenuColumnHeader sampgdk_SetMenuColumnHeader

#undef  ShowMenuForPlayer
#define ShowMenuForPlayer sampgdk_ShowMenuForPlayer

#undef  HideMenuForPlayer
#define HideMenuForPlayer sampgdk_HideMenuForPlayer

#undef  IsValidMenu
#define IsValidMenu sampgdk_IsValidMenu

#undef  DisableMenu
#define DisableMenu sampgdk_DisableMenu

#undef  DisableMenuRow
#define DisableMenuRow sampgdk_DisableMenuRow

#undef  GetPlayerMenu
#define GetPlayerMenu sampgdk_GetPlayerMenu

#undef  TextDrawCreate
#define TextDrawCreate sampgdk_TextDrawCreate

#undef  TextDrawDestroy
#define TextDrawDestroy sampgdk_TextDrawDestroy

#undef  TextDrawLetterSize
#define TextDrawLetterSize sampgdk_TextDrawLetterSize

#undef  TextDrawTextSize
#define TextDrawTextSize sampgdk_TextDrawTextSize

#undef  TextDrawAlignment
#define TextDrawAlignment sampgdk_TextDrawAlignment

#undef  TextDrawColor
#define TextDrawColor sampgdk_TextDrawColor

#undef  TextDrawUseBox
#define TextDrawUseBox sampgdk_TextDrawUseBox

#undef  TextDrawBoxColor
#define TextDrawBoxColor sampgdk_TextDrawBoxColor

#undef  TextDrawSetShadow
#define TextDrawSetShadow sampgdk_TextDrawSetShadow

#undef  TextDrawSetOutline
#define TextDrawSetOutline sampgdk_TextDrawSetOutline

#undef  TextDrawBackgroundColor
#define TextDrawBackgroundColor sampgdk_TextDrawBackgroundColor

#undef  TextDrawFont
#define TextDrawFont sampgdk_TextDrawFont

#undef  TextDrawSetProportional
#define TextDrawSetProportional sampgdk_TextDrawSetProportional

#undef  TextDrawSetSelectable
#define TextDrawSetSelectable sampgdk_TextDrawSetSelectable

#undef  TextDrawShowForPlayer
#define TextDrawShowForPlayer sampgdk_TextDrawShowForPlayer

#undef  TextDrawHideForPlayer
#define TextDrawHideForPlayer sampgdk_TextDrawHideForPlayer

#undef  TextDrawShowForAll
#define TextDrawShowForAll sampgdk_TextDrawShowForAll

#undef  TextDrawHideForAll
#define TextDrawHideForAll sampgdk_TextDrawHideForAll

#undef  TextDrawSetString
#define TextDrawSetString sampgdk_TextDrawSetString

#undef  TextDrawSetPreviewModel
#define TextDrawSetPreviewModel sampgdk_TextDrawSetPreviewModel

#undef  TextDrawSetPreviewRot
#define TextDrawSetPreviewRot sampgdk_TextDrawSetPreviewRot

#undef  TextDrawSetPreviewVehCol
#define TextDrawSetPreviewVehCol sampgdk_TextDrawSetPreviewVehCol

#undef  SelectTextDraw
#define SelectTextDraw sampgdk_SelectTextDraw

#undef  CancelSelectTextDraw
#define CancelSelectTextDraw sampgdk_CancelSelectTextDraw

#undef  GangZoneCreate
#define GangZoneCreate sampgdk_GangZoneCreate

#undef  GangZoneDestroy
#define GangZoneDestroy sampgdk_GangZoneDestroy

#undef  GangZoneShowForPlayer
#define GangZoneShowForPlayer sampgdk_GangZoneShowForPlayer

#undef  GangZoneShowForAll
#define GangZoneShowForAll sampgdk_GangZoneShowForAll

#undef  GangZoneHideForPlayer
#define GangZoneHideForPlayer sampgdk_GangZoneHideForPlayer

#undef  GangZoneHideForAll
#define GangZoneHideForAll sampgdk_GangZoneHideForAll

#undef  GangZoneFlashForPlayer
#define GangZoneFlashForPlayer sampgdk_GangZoneFlashForPlayer

#undef  GangZoneFlashForAll
#define GangZoneFlashForAll sampgdk_GangZoneFlashForAll

#undef  GangZoneStopFlashForPlayer
#define GangZoneStopFlashForPlayer sampgdk_GangZoneStopFlashForPlayer

#undef  GangZoneStopFlashForAll
#define GangZoneStopFlashForAll sampgdk_GangZoneStopFlashForAll

#undef  Create3DTextLabel
#define Create3DTextLabel sampgdk_Create3DTextLabel

#undef  Delete3DTextLabel
#define Delete3DTextLabel sampgdk_Delete3DTextLabel

#undef  Attach3DTextLabelToPlayer
#define Attach3DTextLabelToPlayer sampgdk_Attach3DTextLabelToPlayer

#undef  Attach3DTextLabelToVehicle
#define Attach3DTextLabelToVehicle sampgdk_Attach3DTextLabelToVehicle

#undef  Update3DTextLabelText
#define Update3DTextLabelText sampgdk_Update3DTextLabelText

#undef  CreatePlayer3DTextLabel
#define CreatePlayer3DTextLabel sampgdk_CreatePlayer3DTextLabel

#undef  DeletePlayer3DTextLabel
#define DeletePlayer3DTextLabel sampgdk_DeletePlayer3DTextLabel

#undef  UpdatePlayer3DTextLabelText
#define UpdatePlayer3DTextLabelText sampgdk_UpdatePlayer3DTextLabelText

#undef  ShowPlayerDialog
#define ShowPlayerDialog sampgdk_ShowPlayerDialog

#undef  SetTimer
#define SetTimer sampgdk_SetTimer

#undef  KillTimer
#define KillTimer sampgdk_KillTimer

#undef  gpci
#define gpci sampgdk_gpci

#undef  AddCharModel
#define AddCharModel sampgdk_AddCharModel

#undef  AddSimpleModel
#define AddSimpleModel sampgdk_AddSimpleModel

#undef  AddSimpleModelTimed
#define AddSimpleModelTimed sampgdk_AddSimpleModelTimed

#undef  FindModelFileNameFromCRC
#define FindModelFileNameFromCRC sampgdk_FindModelFileNameFromCRC

#undef  FindTextureFileNameFromCRC
#define FindTextureFileNameFromCRC sampgdk_FindTextureFileNameFromCRC

#undef  RedirectDownload
#define RedirectDownload sampgdk_RedirectDownload

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnGameModeInit">OnGameModeInit on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnGameModeInit());

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnGameModeExit">OnGameModeExit on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnGameModeExit());

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerConnect">OnPlayerConnect on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerConnect(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerDisconnect">OnPlayerDisconnect on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerDisconnect(int playerid, int reason));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerSpawn">OnPlayerSpawn on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerSpawn(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerDeath">OnPlayerDeath on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerDeath(int playerid, int killerid, int reason));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnVehicleSpawn">OnVehicleSpawn on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleSpawn(int vehicleid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnVehicleDeath">OnVehicleDeath on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleDeath(int vehicleid, int killerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerText">OnPlayerText on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerText(int playerid, const char * text));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerCommandText">OnPlayerCommandText on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerCommandText(int playerid, const char * cmdtext));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerRequestClass">OnPlayerRequestClass on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerRequestClass(int playerid, int classid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerEnterVehicle">OnPlayerEnterVehicle on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerEnterVehicle(int playerid, int vehicleid, bool ispassenger));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerExitVehicle">OnPlayerExitVehicle on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerExitVehicle(int playerid, int vehicleid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerStateChange">OnPlayerStateChange on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerStateChange(int playerid, int newstate, int oldstate));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerEnterCheckpoint">OnPlayerEnterCheckpoint on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerEnterCheckpoint(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerLeaveCheckpoint">OnPlayerLeaveCheckpoint on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerLeaveCheckpoint(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerEnterRaceCheckpoint">OnPlayerEnterRaceCheckpoint on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerEnterRaceCheckpoint(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerLeaveRaceCheckpoint">OnPlayerLeaveRaceCheckpoint on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerLeaveRaceCheckpoint(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnRconCommand">OnRconCommand on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnRconCommand(const char * cmd));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerRequestSpawn">OnPlayerRequestSpawn on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerRequestSpawn(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnObjectMoved">OnObjectMoved on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnObjectMoved(int objectid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerObjectMoved">OnPlayerObjectMoved on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerObjectMoved(int playerid, int objectid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerPickUpPickup">OnPlayerPickUpPickup on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerPickUpPickup(int playerid, int pickupid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnVehicleMod">OnVehicleMod on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleMod(int playerid, int vehicleid, int componentid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnEnterExitModShop">OnEnterExitModShop on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnEnterExitModShop(int playerid, bool enterexit, int interiorid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnVehiclePaintjob">OnVehiclePaintjob on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnVehiclePaintjob(int playerid, int vehicleid, int paintjobid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnVehicleRespray">OnVehicleRespray on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleRespray(int playerid, int vehicleid, int color1, int color2));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnVehicleDamageStatusUpdate">OnVehicleDamageStatusUpdate on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleDamageStatusUpdate(int vehicleid, int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnUnoccupiedVehicleUpdate">OnUnoccupiedVehicleUpdate on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnUnoccupiedVehicleUpdate(int vehicleid, int playerid, int passenger_seat, float new_x, float new_y, float new_z, float vel_x, float vel_y, float vel_z));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerSelectedMenuRow">OnPlayerSelectedMenuRow on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerSelectedMenuRow(int playerid, int row));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerExitedMenu">OnPlayerExitedMenu on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerExitedMenu(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerInteriorChange">OnPlayerInteriorChange on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerInteriorChange(int playerid, int newinteriorid, int oldinteriorid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerKeyStateChange">OnPlayerKeyStateChange on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerKeyStateChange(int playerid, int newkeys, int oldkeys));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnRconLoginAttempt">OnRconLoginAttempt on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnRconLoginAttempt(const char * ip, const char * password, bool success));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerUpdate">OnPlayerUpdate on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerUpdate(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerStreamIn">OnPlayerStreamIn on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerStreamIn(int playerid, int forplayerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerStreamOut">OnPlayerStreamOut on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerStreamOut(int playerid, int forplayerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnVehicleStreamIn">OnVehicleStreamIn on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleStreamIn(int vehicleid, int forplayerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnVehicleStreamOut">OnVehicleStreamOut on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleStreamOut(int vehicleid, int forplayerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnActorStreamIn">OnActorStreamIn on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnActorStreamIn(int actorid, int forplayerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnActorStreamOut">OnActorStreamOut on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnActorStreamOut(int actorid, int forplayerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnDialogResponse">OnDialogResponse on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnDialogResponse(int playerid, int dialogid, int response, int listitem, const char * inputtext));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerTakeDamage">OnPlayerTakeDamage on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerTakeDamage(int playerid, int issuerid, float amount, int weaponid, int bodypart));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerGiveDamage">OnPlayerGiveDamage on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerGiveDamage(int playerid, int damagedid, float amount, int weaponid, int bodypart));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerGiveDamageActor">OnPlayerGiveDamageActor on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerGiveDamageActor(int playerid, int damaged_actorid, float amount, int weaponid, int bodypart));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerClickMap">OnPlayerClickMap on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerClickMap(int playerid, float fX, float fY, float fZ));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerClickTextDraw">OnPlayerClickTextDraw on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerClickTextDraw(int playerid, int clickedid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerClickPlayerTextDraw">OnPlayerClickPlayerTextDraw on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerClickPlayerTextDraw(int playerid, int playertextid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnIncomingConnection">OnIncomingConnection on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnIncomingConnection(int playerid, const char * ip_address, int port));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnTrailerUpdate">OnTrailerUpdate on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnTrailerUpdate(int playerid, int vehicleid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnVehicleSirenStateChange">OnVehicleSirenStateChange on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleSirenStateChange(int playerid, int vehicleid, int newstate));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerClickPlayer">OnPlayerClickPlayer on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerClickPlayer(int playerid, int clickedplayerid, int source));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerEditObject">OnPlayerEditObject on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerEditObject(int playerid, bool playerobject, int objectid, int response, float fX, float fY, float fZ, float fRotX, float fRotY, float fRotZ));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerEditAttachedObject">OnPlayerEditAttachedObject on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerEditAttachedObject(int playerid, int response, int index, int modelid, int boneid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ, float fScaleX, float fScaleY, float fScaleZ));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerSelectObject">OnPlayerSelectObject on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerSelectObject(int playerid, int type, int objectid, int modelid, float fX, float fY, float fZ));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerWeaponShot">OnPlayerWeaponShot on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerWeaponShot(int playerid, int weaponid, int hittype, int hitid, float fX, float fY, float fZ));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerRequestDownload">OnPlayerRequestDownload on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerRequestDownload(int playerid, int type, int crc));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnScriptLoadPlayer">OnScriptLoadPlayer on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnScriptLoadPlayer(int playerid, bool isEntryScript));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnScriptUnloadPlayer">OnScriptUnloadPlayer on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnScriptUnloadPlayer(int playerid, bool isEntryScript));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerEnterGangZone">OnPlayerEnterGangZone on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerEnterGangZone(int playerid, int zoneid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerLeaveGangZone">OnPlayerLeaveGangZone on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerLeaveGangZone(int playerid, int zoneid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerEnterPlayerGangZone">OnPlayerEnterPlayerGangZone on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerEnterPlayerGangZone(int playerid, int zoneid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerLeavePlayerGangZone">OnPlayerLeavePlayerGangZone on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerLeavePlayerGangZone(int playerid, int zoneid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerClickGangZone">OnPlayerClickGangZone on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerClickGangZone(int playerid, int zoneid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerClickPlayerGangZone">OnPlayerClickPlayerGangZone on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerClickPlayerGangZone(int playerid, int zoneid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnClientCheckResponse">OnClientCheckResponse on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnClientCheckResponse(int playerid, int actionid, int memaddr, int retndata));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerFinishedDownloading">OnPlayerFinishedDownloading on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerFinishedDownloading(int playerid, int virtualworld));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCFinishMove">OnNPCFinishMove on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCFinishMove(int npcid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCCreate">OnNPCCreate on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCCreate(int npcid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCDestroy">OnNPCDestroy on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCDestroy(int npcid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCSpawn">OnNPCSpawn on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCSpawn(int npcid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCRespawn">OnNPCRespawn on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCRespawn(int npcid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCWeaponStateChange">OnNPCWeaponStateChange on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCWeaponStateChange(int npcid, int newState, int oldState));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCTakeDamage">OnNPCTakeDamage on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCTakeDamage(int npcid, int issuerid, float amount, int weaponid, int bodypart));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCGiveDamage">OnNPCGiveDamage on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCGiveDamage(int npcid, int damagedid, float amount, int weaponid, int bodypart));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCDeath">OnNPCDeath on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCDeath(int npcid, int killerid, int reason));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCPlaybackStart">OnNPCPlaybackStart on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCPlaybackStart(int npcid, int recordid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCPlaybackEnd">OnNPCPlaybackEnd on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCPlaybackEnd(int npcid, int recordid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCWeaponShot">OnNPCWeaponShot on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCWeaponShot(int npcid, int weaponid, int hittype, int hitid, float fX, float fY, float fZ));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCFinishNodePoint">OnNPCFinishNodePoint on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCFinishNodePoint(int npcid, int nodeid, int pointid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCFinishNode">OnNPCFinishNode on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCFinishNode(int npcid, int nodeid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCChangeNode">OnNPCChangeNode on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCChangeNode(int npcid, int newnodeid, int oldnodeid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCFinishMovePath">OnNPCFinishMovePath on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCFinishMovePath(int npcid, int pathid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnNPCFinishMovePathPoint">OnNPCFinishMovePathPoint on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnNPCFinishMovePathPoint(int npcid, int pathid, int pointid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerPickUpPlayerPickup">OnPlayerPickUpPlayerPickup on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerPickUpPlayerPickup(int playerid, int pickupid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPickupStreamIn">OnPickupStreamIn on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPickupStreamIn(int pickupid, int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPickupStreamOut">OnPickupStreamOut on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPickupStreamOut(int pickupid, int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerPickupStreamIn">OnPlayerPickupStreamIn on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerPickupStreamIn(int pickupid, int playerid));

/**
 * \ingroup callbacks
 * \see <a href="https://open.mp/docs/scripting/callbacks/OnPlayerPickupStreamOut">OnPlayerPickupStreamOut on open.mp</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerPickupStreamOut(int pickupid, int playerid));

#endif /* !SAMPGDK_A_SAMP_H */

#ifndef SAMPGDK_A_ACTOR_H
#define SAMPGDK_A_ACTOR_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CreateActor">CreateActor on open.mp</a>
 */
SAMPGDK_NATIVE(int, CreateActor(int modelid, float x, float y, float z, float rotation));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DestroyActor">DestroyActor on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DestroyActor(int actorid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsActorStreamedIn">IsActorStreamedIn on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsActorStreamedIn(int actorid, int forplayerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetActorVirtualWorld">SetActorVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetActorVirtualWorld(int actorid, int vworld));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetActorVirtualWorld">GetActorVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetActorVirtualWorld(int actorid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ApplyActorAnimation">ApplyActorAnimation on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ApplyActorAnimation(int actorid, const char * animlib, const char * animname, float fDelta, bool loop, bool lockx, bool locky, bool freeze, int time));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ClearActorAnimations">ClearActorAnimations on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ClearActorAnimations(int actorid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetActorPos">SetActorPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetActorPos(int actorid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetActorPos">GetActorPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetActorPos(int actorid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetActorFacingAngle">SetActorFacingAngle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetActorFacingAngle(int actorid, float angle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetActorFacingAngle">GetActorFacingAngle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetActorFacingAngle(int actorid, float * angle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetActorHealth">SetActorHealth on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetActorHealth(int actorid, float health));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetActorHealth">GetActorHealth on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetActorHealth(int actorid, float * health));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetActorInvulnerable">SetActorInvulnerable on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetActorInvulnerable(int actorid, bool invulnerable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsActorInvulnerable">IsActorInvulnerable on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsActorInvulnerable(int actorid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidActor">IsValidActor on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidActor(int actorid));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline int CreateActor(int modelid, float x, float y, float z, float rotation) {
  return sampgdk_CreateActor(modelid, x, y, z, rotation);
}

inline bool DestroyActor(int actorid) {
  return sampgdk_DestroyActor(actorid);
}

inline bool IsActorStreamedIn(int actorid, int forplayerid) {
  return sampgdk_IsActorStreamedIn(actorid, forplayerid);
}

inline bool SetActorVirtualWorld(int actorid, int vworld) {
  return sampgdk_SetActorVirtualWorld(actorid, vworld);
}

inline int GetActorVirtualWorld(int actorid) {
  return sampgdk_GetActorVirtualWorld(actorid);
}

inline bool ApplyActorAnimation(int actorid, const char * animlib, const char * animname, float fDelta, bool loop, bool lockx, bool locky, bool freeze, int time) {
  return sampgdk_ApplyActorAnimation(actorid, animlib, animname, fDelta, loop, lockx, locky, freeze, time);
}

inline bool ClearActorAnimations(int actorid) {
  return sampgdk_ClearActorAnimations(actorid);
}

inline bool SetActorPos(int actorid, float x, float y, float z) {
  return sampgdk_SetActorPos(actorid, x, y, z);
}

inline bool GetActorPos(int actorid, float * x, float * y, float * z) {
  return sampgdk_GetActorPos(actorid, x, y, z);
}

inline bool SetActorFacingAngle(int actorid, float angle) {
  return sampgdk_SetActorFacingAngle(actorid, angle);
}

inline bool GetActorFacingAngle(int actorid, float * angle) {
  return sampgdk_GetActorFacingAngle(actorid, angle);
}

inline bool SetActorHealth(int actorid, float health) {
  return sampgdk_SetActorHealth(actorid, health);
}

inline bool GetActorHealth(int actorid, float * health) {
  return sampgdk_GetActorHealth(actorid, health);
}

inline bool SetActorInvulnerable(int actorid, bool invulnerable = true) {
  return sampgdk_SetActorInvulnerable(actorid, invulnerable);
}

inline bool IsActorInvulnerable(int actorid) {
  return sampgdk_IsActorInvulnerable(actorid);
}

inline bool IsValidActor(int actorid) {
  return sampgdk_IsValidActor(actorid);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  CreateActor
#define CreateActor sampgdk_CreateActor

#undef  DestroyActor
#define DestroyActor sampgdk_DestroyActor

#undef  IsActorStreamedIn
#define IsActorStreamedIn sampgdk_IsActorStreamedIn

#undef  SetActorVirtualWorld
#define SetActorVirtualWorld sampgdk_SetActorVirtualWorld

#undef  GetActorVirtualWorld
#define GetActorVirtualWorld sampgdk_GetActorVirtualWorld

#undef  ApplyActorAnimation
#define ApplyActorAnimation sampgdk_ApplyActorAnimation

#undef  ClearActorAnimations
#define ClearActorAnimations sampgdk_ClearActorAnimations

#undef  SetActorPos
#define SetActorPos sampgdk_SetActorPos

#undef  GetActorPos
#define GetActorPos sampgdk_GetActorPos

#undef  SetActorFacingAngle
#define SetActorFacingAngle sampgdk_SetActorFacingAngle

#undef  GetActorFacingAngle
#define GetActorFacingAngle sampgdk_GetActorFacingAngle

#undef  SetActorHealth
#define SetActorHealth sampgdk_SetActorHealth

#undef  GetActorHealth
#define GetActorHealth sampgdk_GetActorHealth

#undef  SetActorInvulnerable
#define SetActorInvulnerable sampgdk_SetActorInvulnerable

#undef  IsActorInvulnerable
#define IsActorInvulnerable sampgdk_IsActorInvulnerable

#undef  IsValidActor
#define IsValidActor sampgdk_IsValidActor

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_A_ACTOR_H */

#ifndef SAMPGDK_A_HTTP_H
#define SAMPGDK_A_HTTP_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */

#define HTTP_GET (1)
#define HTTP_POST (2)
#define HTTP_HEAD (3)
#define HTTP_ERROR_BAD_HOST (1)
#define HTTP_ERROR_NO_SOCKET (2)
#define HTTP_ERROR_CANT_CONNECT (3)
#define HTTP_ERROR_CANT_WRITE (4)
#define HTTP_ERROR_CONTENT_TOO_BIG (5)
#define HTTP_ERROR_MALFORMED_RESPONSE (6)

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/HTTP">HTTP on open.mp</a>
 */
SAMPGDK_NATIVE(bool, HTTP(int index, int type, const char * url, const char * data));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool HTTP(int index, int type, const char * url, const char * data) {
  return sampgdk_HTTP(index, type, url, data);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  HTTP
#define HTTP sampgdk_HTTP

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

SAMPGDK_CALLBACK(void, OnHTTPResponse(int index, int response_code, const char * data));

#endif /* !SAMPGDK_A_HTTP_H */

#ifndef SAMPGDK_A_OBJECTS_H
#define SAMPGDK_A_OBJECTS_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */

#define OBJECT_MATERIAL_SIZE_32x32 (10)
#define OBJECT_MATERIAL_SIZE_64x32 (20)
#define OBJECT_MATERIAL_SIZE_64x64 (30)
#define OBJECT_MATERIAL_SIZE_128x32 (40)
#define OBJECT_MATERIAL_SIZE_128x64 (50)
#define OBJECT_MATERIAL_SIZE_128x128 (60)
#define OBJECT_MATERIAL_SIZE_256x32 (70)
#define OBJECT_MATERIAL_SIZE_256x64 (80)
#define OBJECT_MATERIAL_SIZE_256x128 (90)
#define OBJECT_MATERIAL_SIZE_256x256 (100)
#define OBJECT_MATERIAL_SIZE_512x64 (110)
#define OBJECT_MATERIAL_SIZE_512x128 (120)
#define OBJECT_MATERIAL_SIZE_512x256 (130)
#define OBJECT_MATERIAL_SIZE_512x512 (140)
#define OBJECT_MATERIAL_TEXT_ALIGN_LEFT (0)
#define OBJECT_MATERIAL_TEXT_ALIGN_CENTER (1)
#define OBJECT_MATERIAL_TEXT_ALIGN_RIGHT (2)

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CreateObject">CreateObject on open.mp</a>
 */
SAMPGDK_NATIVE(int, CreateObject(int modelid, float x, float y, float z, float rX, float rY, float rZ, float DrawDistance));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AttachObjectToVehicle">AttachObjectToVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AttachObjectToVehicle(int objectid, int vehicleid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AttachObjectToObject">AttachObjectToObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AttachObjectToObject(int objectid, int attachtoid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ, bool SyncRotation));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AttachObjectToPlayer">AttachObjectToPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AttachObjectToPlayer(int objectid, int playerid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetObjectPos">SetObjectPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetObjectPos(int objectid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectPos">GetObjectPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetObjectPos(int objectid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetObjectRot">SetObjectRot on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetObjectRot(int objectid, float rotX, float rotY, float rotZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectRot">GetObjectRot on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetObjectRot(int objectid, float * rotX, float * rotY, float * rotZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectModel">GetObjectModel on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetObjectModel(int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetObjectNoCameraCol">SetObjectNoCameraCol on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetObjectNoCameraCol(int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidObject">IsValidObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidObject(int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DestroyObject">DestroyObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DestroyObject(int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/MoveObject">MoveObject on open.mp</a>
 */
SAMPGDK_NATIVE(int, MoveObject(int objectid, float X, float Y, float Z, float Speed, float RotX, float RotY, float RotZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/StopObject">StopObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, StopObject(int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsObjectMoving">IsObjectMoving on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsObjectMoving(int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/EditObject">EditObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, EditObject(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/EditPlayerObject">EditPlayerObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, EditPlayerObject(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SelectObject">SelectObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SelectObject(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CancelEdit">CancelEdit on open.mp</a>
 */
SAMPGDK_NATIVE(bool, CancelEdit(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CreatePlayerObject">CreatePlayerObject on open.mp</a>
 */
SAMPGDK_NATIVE(int, CreatePlayerObject(int playerid, int modelid, float x, float y, float z, float rX, float rY, float rZ, float DrawDistance));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AttachPlayerObjectToPlayer">AttachPlayerObjectToPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AttachPlayerObjectToPlayer(int objectplayer, int objectid, int attachplayer, float OffsetX, float OffsetY, float OffsetZ, float rX, float rY, float rZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AttachPlayerObjectToVehicle">AttachPlayerObjectToVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AttachPlayerObjectToVehicle(int playerid, int objectid, int vehicleid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float RotZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerObjectPos">SetPlayerObjectPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerObjectPos(int playerid, int objectid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerObjectPos">GetPlayerObjectPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerObjectPos(int playerid, int objectid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerObjectRot">SetPlayerObjectRot on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerObjectRot(int playerid, int objectid, float rotX, float rotY, float rotZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerObjectRot">GetPlayerObjectRot on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerObjectRot(int playerid, int objectid, float * rotX, float * rotY, float * rotZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerObjectModel">GetPlayerObjectModel on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerObjectModel(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerObjectNoCameraCol">SetPlayerObjectNoCameraCol on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerObjectNoCameraCol(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidPlayerObject">IsValidPlayerObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidPlayerObject(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DestroyPlayerObject">DestroyPlayerObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DestroyPlayerObject(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/MovePlayerObject">MovePlayerObject on open.mp</a>
 */
SAMPGDK_NATIVE(int, MovePlayerObject(int playerid, int objectid, float x, float y, float z, float Speed, float RotX, float RotY, float RotZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/StopPlayerObject">StopPlayerObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, StopPlayerObject(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerObjectMoving">IsPlayerObjectMoving on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerObjectMoving(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetObjectMaterial">SetObjectMaterial on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetObjectMaterial(int objectid, int materialindex, int modelid, const char * txdname, const char * texturename, int materialcolor));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerObjectMaterial">SetPlayerObjectMaterial on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerObjectMaterial(int playerid, int objectid, int materialindex, int modelid, const char * txdname, const char * texturename, int materialcolor));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetObjectMaterialText">SetObjectMaterialText on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetObjectMaterialText(int objectid, const char * text, int materialindex, int materialsize, const char * fontface, int fontsize, bool bold, int fontcolor, int backcolor, int textalignment));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerObjectMaterialText">SetPlayerObjectMaterialText on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerObjectMaterialText(int playerid, int objectid, const char * text, int materialindex, int materialsize, const char * fontface, int fontsize, bool bold, int fontcolor, int backcolor, int textalignment));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetObjectsDefaultCameraCol">SetObjectsDefaultCameraCol on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetObjectsDefaultCameraCol(bool disable));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline int CreateObject(int modelid, float x, float y, float z, float rX, float rY, float rZ, float DrawDistance = 0.0) {
  return sampgdk_CreateObject(modelid, x, y, z, rX, rY, rZ, DrawDistance);
}

inline bool AttachObjectToVehicle(int objectid, int vehicleid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ) {
  return sampgdk_AttachObjectToVehicle(objectid, vehicleid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ);
}

inline bool AttachObjectToObject(int objectid, int attachtoid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ, bool SyncRotation = false) {
  return sampgdk_AttachObjectToObject(objectid, attachtoid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, SyncRotation);
}

inline bool AttachObjectToPlayer(int objectid, int playerid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ) {
  return sampgdk_AttachObjectToPlayer(objectid, playerid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ);
}

inline bool SetObjectPos(int objectid, float x, float y, float z) {
  return sampgdk_SetObjectPos(objectid, x, y, z);
}

inline bool GetObjectPos(int objectid, float * x, float * y, float * z) {
  return sampgdk_GetObjectPos(objectid, x, y, z);
}

inline bool SetObjectRot(int objectid, float rotX, float rotY, float rotZ) {
  return sampgdk_SetObjectRot(objectid, rotX, rotY, rotZ);
}

inline bool GetObjectRot(int objectid, float * rotX, float * rotY, float * rotZ) {
  return sampgdk_GetObjectRot(objectid, rotX, rotY, rotZ);
}

inline int GetObjectModel(int objectid) {
  return sampgdk_GetObjectModel(objectid);
}

inline bool SetObjectNoCameraCol(int objectid) {
  return sampgdk_SetObjectNoCameraCol(objectid);
}

inline bool IsValidObject(int objectid) {
  return sampgdk_IsValidObject(objectid);
}

inline bool DestroyObject(int objectid) {
  return sampgdk_DestroyObject(objectid);
}

inline int MoveObject(int objectid, float X, float Y, float Z, float Speed, float RotX = -1000.0, float RotY = -1000.0, float RotZ = -1000.0) {
  return sampgdk_MoveObject(objectid, X, Y, Z, Speed, RotX, RotY, RotZ);
}

inline bool StopObject(int objectid) {
  return sampgdk_StopObject(objectid);
}

inline bool IsObjectMoving(int objectid) {
  return sampgdk_IsObjectMoving(objectid);
}

inline bool EditObject(int playerid, int objectid) {
  return sampgdk_EditObject(playerid, objectid);
}

inline bool EditPlayerObject(int playerid, int objectid) {
  return sampgdk_EditPlayerObject(playerid, objectid);
}

inline bool SelectObject(int playerid) {
  return sampgdk_SelectObject(playerid);
}

inline bool CancelEdit(int playerid) {
  return sampgdk_CancelEdit(playerid);
}

inline int CreatePlayerObject(int playerid, int modelid, float x, float y, float z, float rX, float rY, float rZ, float DrawDistance = 0.0) {
  return sampgdk_CreatePlayerObject(playerid, modelid, x, y, z, rX, rY, rZ, DrawDistance);
}

inline bool AttachPlayerObjectToPlayer(int objectplayer, int objectid, int attachplayer, float OffsetX, float OffsetY, float OffsetZ, float rX, float rY, float rZ) {
  return sampgdk_AttachPlayerObjectToPlayer(objectplayer, objectid, attachplayer, OffsetX, OffsetY, OffsetZ, rX, rY, rZ);
}

inline bool AttachPlayerObjectToVehicle(int playerid, int objectid, int vehicleid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float RotZ) {
  return sampgdk_AttachPlayerObjectToVehicle(playerid, objectid, vehicleid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, RotZ);
}

inline bool SetPlayerObjectPos(int playerid, int objectid, float x, float y, float z) {
  return sampgdk_SetPlayerObjectPos(playerid, objectid, x, y, z);
}

inline bool GetPlayerObjectPos(int playerid, int objectid, float * x, float * y, float * z) {
  return sampgdk_GetPlayerObjectPos(playerid, objectid, x, y, z);
}

inline bool SetPlayerObjectRot(int playerid, int objectid, float rotX, float rotY, float rotZ) {
  return sampgdk_SetPlayerObjectRot(playerid, objectid, rotX, rotY, rotZ);
}

inline bool GetPlayerObjectRot(int playerid, int objectid, float * rotX, float * rotY, float * rotZ) {
  return sampgdk_GetPlayerObjectRot(playerid, objectid, rotX, rotY, rotZ);
}

inline int GetPlayerObjectModel(int playerid, int objectid) {
  return sampgdk_GetPlayerObjectModel(playerid, objectid);
}

inline bool SetPlayerObjectNoCameraCol(int playerid, int objectid) {
  return sampgdk_SetPlayerObjectNoCameraCol(playerid, objectid);
}

inline bool IsValidPlayerObject(int playerid, int objectid) {
  return sampgdk_IsValidPlayerObject(playerid, objectid);
}

inline bool DestroyPlayerObject(int playerid, int objectid) {
  return sampgdk_DestroyPlayerObject(playerid, objectid);
}

inline int MovePlayerObject(int playerid, int objectid, float x, float y, float z, float Speed, float RotX = -1000.0, float RotY = -1000.0, float RotZ = -1000.0) {
  return sampgdk_MovePlayerObject(playerid, objectid, x, y, z, Speed, RotX, RotY, RotZ);
}

inline bool StopPlayerObject(int playerid, int objectid) {
  return sampgdk_StopPlayerObject(playerid, objectid);
}

inline bool IsPlayerObjectMoving(int playerid, int objectid) {
  return sampgdk_IsPlayerObjectMoving(playerid, objectid);
}

inline bool SetObjectMaterial(int objectid, int materialindex, int modelid, const char * txdname, const char * texturename, int materialcolor = 0) {
  return sampgdk_SetObjectMaterial(objectid, materialindex, modelid, txdname, texturename, materialcolor);
}

inline bool SetPlayerObjectMaterial(int playerid, int objectid, int materialindex, int modelid, const char * txdname, const char * texturename, int materialcolor = 0) {
  return sampgdk_SetPlayerObjectMaterial(playerid, objectid, materialindex, modelid, txdname, texturename, materialcolor);
}

inline bool SetObjectMaterialText(int objectid, const char * text, int materialindex = 0, int materialsize = OBJECT_MATERIAL_SIZE_256x128, const char * fontface = "Arial", int fontsize = 24, bool bold = true, int fontcolor = 0xFFFFFFFF, int backcolor = 0, int textalignment = 0) {
  return sampgdk_SetObjectMaterialText(objectid, text, materialindex, materialsize, fontface, fontsize, bold, fontcolor, backcolor, textalignment);
}

inline bool SetPlayerObjectMaterialText(int playerid, int objectid, const char * text, int materialindex = 0, int materialsize = OBJECT_MATERIAL_SIZE_256x128, const char * fontface = "Arial", int fontsize = 24, bool bold = true, int fontcolor = 0xFFFFFFFF, int backcolor = 0, int textalignment = 0) {
  return sampgdk_SetPlayerObjectMaterialText(playerid, objectid, text, materialindex, materialsize, fontface, fontsize, bold, fontcolor, backcolor, textalignment);
}

inline bool SetObjectsDefaultCameraCol(bool disable) {
  return sampgdk_SetObjectsDefaultCameraCol(disable);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  CreateObject
#define CreateObject sampgdk_CreateObject

#undef  AttachObjectToVehicle
#define AttachObjectToVehicle sampgdk_AttachObjectToVehicle

#undef  AttachObjectToObject
#define AttachObjectToObject sampgdk_AttachObjectToObject

#undef  AttachObjectToPlayer
#define AttachObjectToPlayer sampgdk_AttachObjectToPlayer

#undef  SetObjectPos
#define SetObjectPos sampgdk_SetObjectPos

#undef  GetObjectPos
#define GetObjectPos sampgdk_GetObjectPos

#undef  SetObjectRot
#define SetObjectRot sampgdk_SetObjectRot

#undef  GetObjectRot
#define GetObjectRot sampgdk_GetObjectRot

#undef  GetObjectModel
#define GetObjectModel sampgdk_GetObjectModel

#undef  SetObjectNoCameraCol
#define SetObjectNoCameraCol sampgdk_SetObjectNoCameraCol

#undef  IsValidObject
#define IsValidObject sampgdk_IsValidObject

#undef  DestroyObject
#define DestroyObject sampgdk_DestroyObject

#undef  MoveObject
#define MoveObject sampgdk_MoveObject

#undef  StopObject
#define StopObject sampgdk_StopObject

#undef  IsObjectMoving
#define IsObjectMoving sampgdk_IsObjectMoving

#undef  EditObject
#define EditObject sampgdk_EditObject

#undef  EditPlayerObject
#define EditPlayerObject sampgdk_EditPlayerObject

#undef  SelectObject
#define SelectObject sampgdk_SelectObject

#undef  CancelEdit
#define CancelEdit sampgdk_CancelEdit

#undef  CreatePlayerObject
#define CreatePlayerObject sampgdk_CreatePlayerObject

#undef  AttachPlayerObjectToPlayer
#define AttachPlayerObjectToPlayer sampgdk_AttachPlayerObjectToPlayer

#undef  AttachPlayerObjectToVehicle
#define AttachPlayerObjectToVehicle sampgdk_AttachPlayerObjectToVehicle

#undef  SetPlayerObjectPos
#define SetPlayerObjectPos sampgdk_SetPlayerObjectPos

#undef  GetPlayerObjectPos
#define GetPlayerObjectPos sampgdk_GetPlayerObjectPos

#undef  SetPlayerObjectRot
#define SetPlayerObjectRot sampgdk_SetPlayerObjectRot

#undef  GetPlayerObjectRot
#define GetPlayerObjectRot sampgdk_GetPlayerObjectRot

#undef  GetPlayerObjectModel
#define GetPlayerObjectModel sampgdk_GetPlayerObjectModel

#undef  SetPlayerObjectNoCameraCol
#define SetPlayerObjectNoCameraCol sampgdk_SetPlayerObjectNoCameraCol

#undef  IsValidPlayerObject
#define IsValidPlayerObject sampgdk_IsValidPlayerObject

#undef  DestroyPlayerObject
#define DestroyPlayerObject sampgdk_DestroyPlayerObject

#undef  MovePlayerObject
#define MovePlayerObject sampgdk_MovePlayerObject

#undef  StopPlayerObject
#define StopPlayerObject sampgdk_StopPlayerObject

#undef  IsPlayerObjectMoving
#define IsPlayerObjectMoving sampgdk_IsPlayerObjectMoving

#undef  SetObjectMaterial
#define SetObjectMaterial sampgdk_SetObjectMaterial

#undef  SetPlayerObjectMaterial
#define SetPlayerObjectMaterial sampgdk_SetPlayerObjectMaterial

#undef  SetObjectMaterialText
#define SetObjectMaterialText sampgdk_SetObjectMaterialText

#undef  SetPlayerObjectMaterialText
#define SetPlayerObjectMaterialText sampgdk_SetPlayerObjectMaterialText

#undef  SetObjectsDefaultCameraCol
#define SetObjectsDefaultCameraCol sampgdk_SetObjectsDefaultCameraCol

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_A_OBJECTS_H */

#ifndef SAMPGDK_A_PLAYERS_H
#define SAMPGDK_A_PLAYERS_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */

#define SPECIAL_ACTION_NONE (0)
#define SPECIAL_ACTION_DUCK (1)
#define SPECIAL_ACTION_USEJETPACK (2)
#define SPECIAL_ACTION_ENTER_VEHICLE (3)
#define SPECIAL_ACTION_EXIT_VEHICLE (4)
#define SPECIAL_ACTION_DANCE1 (5)
#define SPECIAL_ACTION_DANCE2 (6)
#define SPECIAL_ACTION_DANCE3 (7)
#define SPECIAL_ACTION_DANCE4 (8)
#define SPECIAL_ACTION_HANDSUP (10)
#define SPECIAL_ACTION_USECELLPHONE (11)
#define SPECIAL_ACTION_SITTING (12)
#define SPECIAL_ACTION_STOPUSECELLPHONE (13)
#define SPECIAL_ACTION_DRINK_BEER (20)
#define SPECIAL_ACTION_SMOKE_CIGGY (21)
#define SPECIAL_ACTION_DRINK_WINE (22)
#define SPECIAL_ACTION_DRINK_SPRUNK (23)
#define SPECIAL_ACTION_CUFFED (24)
#define SPECIAL_ACTION_CARRY (25)
#define SPECIAL_ACTION_PISSING (68)
#define FIGHT_STYLE_NORMAL (4)
#define FIGHT_STYLE_BOXING (5)
#define FIGHT_STYLE_KUNGFU (6)
#define FIGHT_STYLE_KNEEHEAD (7)
#define FIGHT_STYLE_GRABKICK (15)
#define FIGHT_STYLE_ELBOW (16)
#define WEAPONSKILL_PISTOL (0)
#define WEAPONSKILL_PISTOL_SILENCED (1)
#define WEAPONSKILL_DESERT_EAGLE (2)
#define WEAPONSKILL_SHOTGUN (3)
#define WEAPONSKILL_SAWNOFF_SHOTGUN (4)
#define WEAPONSKILL_SPAS12_SHOTGUN (5)
#define WEAPONSKILL_MICRO_UZI (6)
#define WEAPONSKILL_MP5 (7)
#define WEAPONSKILL_AK47 (8)
#define WEAPONSKILL_M4 (9)
#define WEAPONSKILL_SNIPERRIFLE (10)
#define WEAPONSTATE_UNKNOWN (-1)
#define WEAPONSTATE_NO_BULLETS (0)
#define WEAPONSTATE_LAST_BULLET (1)
#define WEAPONSTATE_MORE_BULLETS (2)
#define WEAPONSTATE_RELOADING (3)
#define MAX_PLAYER_ATTACHED_OBJECTS (10)
#define PLAYER_VARTYPE_NONE (0)
#define PLAYER_VARTYPE_INT (1)
#define PLAYER_VARTYPE_STRING (2)
#define PLAYER_VARTYPE_FLOAT (3)
#define MAX_CHATBUBBLE_LENGTH (144)
#define MAPICON_LOCAL (0)
#define MAPICON_GLOBAL (1)
#define MAPICON_LOCAL_CHECKPOINT (2)
#define MAPICON_GLOBAL_CHECKPOINT (3)
#define CAMERA_CUT (2)
#define CAMERA_MOVE (1)
#define SPECTATE_MODE_NORMAL (1)
#define SPECTATE_MODE_FIXED (2)
#define SPECTATE_MODE_SIDE (3)
#define PLAYER_RECORDING_TYPE_NONE (0)
#define PLAYER_RECORDING_TYPE_DRIVER (1)
#define PLAYER_RECORDING_TYPE_ONFOOT (2)

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetSpawnInfo">SetSpawnInfo on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetSpawnInfo(int playerid, int team, int skin, float x, float y, float z, float rotation, int weapon1, int weapon1_ammo, int weapon2, int weapon2_ammo, int weapon3, int weapon3_ammo));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SpawnPlayer">SpawnPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SpawnPlayer(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerPos">SetPlayerPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerPos(int playerid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerPosFindZ">SetPlayerPosFindZ on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerPosFindZ(int playerid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerPos">GetPlayerPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerPos(int playerid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerFacingAngle">SetPlayerFacingAngle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerFacingAngle(int playerid, float angle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerFacingAngle">GetPlayerFacingAngle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerFacingAngle(int playerid, float * angle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerInRangeOfPoint">IsPlayerInRangeOfPoint on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerInRangeOfPoint(int playerid, float range, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerDistanceFromPoint">GetPlayerDistanceFromPoint on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetPlayerDistanceFromPoint(int playerid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerStreamedIn">IsPlayerStreamedIn on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerStreamedIn(int playerid, int forplayerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerInterior">SetPlayerInterior on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerInterior(int playerid, int interiorid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerInterior">GetPlayerInterior on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerInterior(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerHealth">SetPlayerHealth on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerHealth(int playerid, float health));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerHealth">GetPlayerHealth on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerHealth(int playerid, float * health));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerArmour">SetPlayerArmour on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerArmour(int playerid, float armour));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerArmour">GetPlayerArmour on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerArmour(int playerid, float * armour));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerAmmo">SetPlayerAmmo on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerAmmo(int playerid, int weaponid, int ammo));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerAmmo">GetPlayerAmmo on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerAmmo(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerWeaponState">GetPlayerWeaponState on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerWeaponState(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerTargetPlayer">GetPlayerTargetPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerTargetPlayer(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerTargetActor">GetPlayerTargetActor on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerTargetActor(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerTeam">SetPlayerTeam on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerTeam(int playerid, int teamid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerTeam">GetPlayerTeam on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerTeam(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerScore">SetPlayerScore on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerScore(int playerid, int score));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerScore">GetPlayerScore on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerScore(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerDrunkLevel">GetPlayerDrunkLevel on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerDrunkLevel(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerDrunkLevel">SetPlayerDrunkLevel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerDrunkLevel(int playerid, int level));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerColor">SetPlayerColor on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerColor(int playerid, int color));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerColor">GetPlayerColor on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerColor(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerSkin">SetPlayerSkin on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerSkin(int playerid, int skinid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerSkin">GetPlayerSkin on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerSkin(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GivePlayerWeapon">GivePlayerWeapon on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GivePlayerWeapon(int playerid, int weaponid, int ammo));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ResetPlayerWeapons">ResetPlayerWeapons on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ResetPlayerWeapons(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerArmedWeapon">SetPlayerArmedWeapon on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerArmedWeapon(int playerid, int weaponid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerWeaponData">GetPlayerWeaponData on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerWeaponData(int playerid, int slot, int * weapon, int * ammo));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GivePlayerMoney">GivePlayerMoney on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GivePlayerMoney(int playerid, int money));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ResetPlayerMoney">ResetPlayerMoney on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ResetPlayerMoney(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerName">SetPlayerName on open.mp</a>
 */
SAMPGDK_NATIVE(int, SetPlayerName(int playerid, const char * name));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerMoney">GetPlayerMoney on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerMoney(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerState">GetPlayerState on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerState(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerIp">GetPlayerIp on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerIp(int playerid, char * ip, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerPing">GetPlayerPing on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerPing(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerWeapon">GetPlayerWeapon on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerWeapon(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerKeys">GetPlayerKeys on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerKeys(int playerid, int * keys, int * updown, int * leftright));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerName">GetPlayerName on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerName(int playerid, char * name, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerTime">SetPlayerTime on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerTime(int playerid, int hour, int minute));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerTime">GetPlayerTime on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerTime(int playerid, int * hour, int * minute));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TogglePlayerClock">TogglePlayerClock on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TogglePlayerClock(int playerid, bool toggle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerWeather">SetPlayerWeather on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerWeather(int playerid, int weather));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ForceClassSelection">ForceClassSelection on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ForceClassSelection(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerWantedLevel">SetPlayerWantedLevel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerWantedLevel(int playerid, int level));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerWantedLevel">GetPlayerWantedLevel on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerWantedLevel(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerFightingStyle">SetPlayerFightingStyle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerFightingStyle(int playerid, int style));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerFightingStyle">GetPlayerFightingStyle on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerFightingStyle(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerVelocity">SetPlayerVelocity on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerVelocity(int playerid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerVelocity">GetPlayerVelocity on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerVelocity(int playerid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayCrimeReportForPlayer">PlayCrimeReportForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayCrimeReportForPlayer(int playerid, int suspectid, int crime));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayAudioStreamForPlayer">PlayAudioStreamForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayAudioStreamForPlayer(int playerid, const char * url, float posX, float posY, float posZ, float distance, bool usepos));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/StopAudioStreamForPlayer">StopAudioStreamForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, StopAudioStreamForPlayer(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerShopName">SetPlayerShopName on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerShopName(int playerid, const char * shopname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerSkillLevel">SetPlayerSkillLevel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerSkillLevel(int playerid, int skill, int level));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerSurfingVehicleID">GetPlayerSurfingVehicleID on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerSurfingVehicleID(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerSurfingObjectID">GetPlayerSurfingObjectID on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerSurfingObjectID(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/RemoveBuildingForPlayer">RemoveBuildingForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, RemoveBuildingForPlayer(int playerid, int modelid, float fX, float fY, float fZ, float fRadius));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerLastShotVectors">GetPlayerLastShotVectors on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerLastShotVectors(int playerid, float * fOriginX, float * fOriginY, float * fOriginZ, float * fHitPosX, float * fHitPosY, float * fHitPosZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerAttachedObject">SetPlayerAttachedObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerAttachedObject(int playerid, int index, int modelid, int bone, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ, float fScaleX, float fScaleY, float fScaleZ, int materialcolor1, int materialcolor2));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/RemovePlayerAttachedObject">RemovePlayerAttachedObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, RemovePlayerAttachedObject(int playerid, int index));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerAttachedObjectSlotUsed">IsPlayerAttachedObjectSlotUsed on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerAttachedObjectSlotUsed(int playerid, int index));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/EditAttachedObject">EditAttachedObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, EditAttachedObject(int playerid, int index));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CreatePlayerTextDraw">CreatePlayerTextDraw on open.mp</a>
 */
SAMPGDK_NATIVE(int, CreatePlayerTextDraw(int playerid, float x, float y, const char * text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawDestroy">PlayerTextDrawDestroy on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawDestroy(int playerid, int text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawLetterSize">PlayerTextDrawLetterSize on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawLetterSize(int playerid, int text, float x, float y));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawTextSize">PlayerTextDrawTextSize on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawTextSize(int playerid, int text, float x, float y));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawAlignment">PlayerTextDrawAlignment on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawAlignment(int playerid, int text, int alignment));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawColor">PlayerTextDrawColor on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawColor(int playerid, int text, int color));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawUseBox">PlayerTextDrawUseBox on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawUseBox(int playerid, int text, bool use));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawBoxColor">PlayerTextDrawBoxColor on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawBoxColor(int playerid, int text, int color));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawSetShadow">PlayerTextDrawSetShadow on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawSetShadow(int playerid, int text, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawSetOutline">PlayerTextDrawSetOutline on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawSetOutline(int playerid, int text, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawBackgroundColor">PlayerTextDrawBackgroundColor on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawBackgroundColor(int playerid, int text, int color));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawFont">PlayerTextDrawFont on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawFont(int playerid, int text, int font));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawSetProportional">PlayerTextDrawSetProportional on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawSetProportional(int playerid, int text, bool set));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawSetSelectable">PlayerTextDrawSetSelectable on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawSetSelectable(int playerid, int text, bool set));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawShow">PlayerTextDrawShow on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawShow(int playerid, int text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawHide">PlayerTextDrawHide on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawHide(int playerid, int text));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawSetString">PlayerTextDrawSetString on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawSetString(int playerid, int text, const char * string));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawSetPreviewModel">PlayerTextDrawSetPreviewModel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawSetPreviewModel(int playerid, int text, int modelindex));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawSetPreviewRot">PlayerTextDrawSetPreviewRot on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawSetPreviewRot(int playerid, int text, float fRotX, float fRotY, float fRotZ, float fZoom));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawSetPreviewVehCol">PlayerTextDrawSetPreviewVehCol on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawSetPreviewVehCol(int playerid, int text, int color1, int color2));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPVarInt">SetPVarInt on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPVarInt(int playerid, const char * varname, int value));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPVarInt">GetPVarInt on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPVarInt(int playerid, const char * varname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPVarString">SetPVarString on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPVarString(int playerid, const char * varname, const char * value));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPVarString">GetPVarString on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPVarString(int playerid, const char * varname, char * value, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPVarFloat">SetPVarFloat on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPVarFloat(int playerid, const char * varname, float value));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPVarFloat">GetPVarFloat on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetPVarFloat(int playerid, const char * varname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DeletePVar">DeletePVar on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DeletePVar(int playerid, const char * varname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPVarsUpperIndex">GetPVarsUpperIndex on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPVarsUpperIndex(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPVarNameAtIndex">GetPVarNameAtIndex on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPVarNameAtIndex(int playerid, int index, char * varname, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPVarType">GetPVarType on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPVarType(int playerid, const char * varname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerChatBubble">SetPlayerChatBubble on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerChatBubble(int playerid, const char * text, int color, float drawdistance, int expiretime));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PutPlayerInVehicle">PutPlayerInVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PutPlayerInVehicle(int playerid, int vehicleid, int seatid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerVehicleID">GetPlayerVehicleID on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerVehicleID(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerVehicleSeat">GetPlayerVehicleSeat on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerVehicleSeat(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/RemovePlayerFromVehicle">RemovePlayerFromVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, RemovePlayerFromVehicle(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TogglePlayerControllable">TogglePlayerControllable on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TogglePlayerControllable(int playerid, bool toggle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerPlaySound">PlayerPlaySound on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerPlaySound(int playerid, int soundid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ApplyAnimation">ApplyAnimation on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ApplyAnimation(int playerid, const char * animlib, const char * animname, float fDelta, bool loop, bool lockx, bool locky, bool freeze, int time, bool forcesync));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ClearAnimations">ClearAnimations on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ClearAnimations(int playerid, bool forcesync));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerAnimationIndex">GetPlayerAnimationIndex on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerAnimationIndex(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetAnimationName">GetAnimationName on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetAnimationName(int index, char * animlib, int animlib_size, char * animname, int animname_size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerSpecialAction">GetPlayerSpecialAction on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerSpecialAction(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerSpecialAction">SetPlayerSpecialAction on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerSpecialAction(int playerid, int actionid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DisableRemoteVehicleCollisions">DisableRemoteVehicleCollisions on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DisableRemoteVehicleCollisions(int playerid, bool disable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerCheckpoint">SetPlayerCheckpoint on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerCheckpoint(int playerid, float x, float y, float z, float size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DisablePlayerCheckpoint">DisablePlayerCheckpoint on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DisablePlayerCheckpoint(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerRaceCheckpoint">SetPlayerRaceCheckpoint on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerRaceCheckpoint(int playerid, int type, float x, float y, float z, float nextx, float nexty, float nextz, float size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DisablePlayerRaceCheckpoint">DisablePlayerRaceCheckpoint on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DisablePlayerRaceCheckpoint(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerWorldBounds">SetPlayerWorldBounds on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerWorldBounds(int playerid, float x_max, float x_min, float y_max, float y_min));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerMarkerForPlayer">SetPlayerMarkerForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerMarkerForPlayer(int playerid, int showplayerid, int color));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ShowPlayerNameTagForPlayer">ShowPlayerNameTagForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ShowPlayerNameTagForPlayer(int playerid, int showplayerid, bool show));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerMapIcon">SetPlayerMapIcon on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerMapIcon(int playerid, int iconid, float x, float y, float z, int markertype, int color, int style));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/RemovePlayerMapIcon">RemovePlayerMapIcon on open.mp</a>
 */
SAMPGDK_NATIVE(bool, RemovePlayerMapIcon(int playerid, int iconid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AllowPlayerTeleport">AllowPlayerTeleport on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AllowPlayerTeleport(int playerid, bool allow));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerCameraPos">SetPlayerCameraPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerCameraPos(int playerid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerCameraLookAt">SetPlayerCameraLookAt on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerCameraLookAt(int playerid, float x, float y, float z, int cut));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetCameraBehindPlayer">SetCameraBehindPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetCameraBehindPlayer(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCameraPos">GetPlayerCameraPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerCameraPos(int playerid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCameraFrontVector">GetPlayerCameraFrontVector on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerCameraFrontVector(int playerid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCameraMode">GetPlayerCameraMode on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerCameraMode(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/EnablePlayerCameraTarget">EnablePlayerCameraTarget on open.mp</a>
 */
SAMPGDK_NATIVE(bool, EnablePlayerCameraTarget(int playerid, bool enable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCameraTargetObject">GetPlayerCameraTargetObject on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerCameraTargetObject(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCameraTargetVehicle">GetPlayerCameraTargetVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerCameraTargetVehicle(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCameraTargetPlayer">GetPlayerCameraTargetPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerCameraTargetPlayer(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCameraTargetActor">GetPlayerCameraTargetActor on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerCameraTargetActor(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCameraAspectRatio">GetPlayerCameraAspectRatio on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetPlayerCameraAspectRatio(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCameraZoom">GetPlayerCameraZoom on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetPlayerCameraZoom(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AttachCameraToObject">AttachCameraToObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AttachCameraToObject(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AttachCameraToPlayerObject">AttachCameraToPlayerObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AttachCameraToPlayerObject(int playerid, int playerobjectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/InterpolateCameraPos">InterpolateCameraPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, InterpolateCameraPos(int playerid, float FromX, float FromY, float FromZ, float ToX, float ToY, float ToZ, int time, int cut));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/InterpolateCameraLookAt">InterpolateCameraLookAt on open.mp</a>
 */
SAMPGDK_NATIVE(bool, InterpolateCameraLookAt(int playerid, float FromX, float FromY, float FromZ, float ToX, float ToY, float ToZ, int time, int cut));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerConnected">IsPlayerConnected on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerConnected(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerInVehicle">IsPlayerInVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerInVehicle(int playerid, int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerInAnyVehicle">IsPlayerInAnyVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerInAnyVehicle(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerInCheckpoint">IsPlayerInCheckpoint on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerInCheckpoint(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerInRaceCheckpoint">IsPlayerInRaceCheckpoint on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerInRaceCheckpoint(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerVirtualWorld">SetPlayerVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerVirtualWorld(int playerid, int worldid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerVirtualWorld">GetPlayerVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerVirtualWorld(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/EnableStuntBonusForPlayer">EnableStuntBonusForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, EnableStuntBonusForPlayer(int playerid, bool enable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/EnableStuntBonusForAll">EnableStuntBonusForAll on open.mp</a>
 */
SAMPGDK_NATIVE(bool, EnableStuntBonusForAll(bool enable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TogglePlayerSpectating">TogglePlayerSpectating on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TogglePlayerSpectating(int playerid, bool toggle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerSpectatePlayer">PlayerSpectatePlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerSpectatePlayer(int playerid, int targetplayerid, int mode));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerSpectateVehicle">PlayerSpectateVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerSpectateVehicle(int playerid, int targetvehicleid, int mode));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/StartRecordingPlayerData">StartRecordingPlayerData on open.mp</a>
 */
SAMPGDK_NATIVE(bool, StartRecordingPlayerData(int playerid, int recordtype, const char * recordname));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/StopRecordingPlayerData">StopRecordingPlayerData on open.mp</a>
 */
SAMPGDK_NATIVE(bool, StopRecordingPlayerData(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CreateExplosionForPlayer">CreateExplosionForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, CreateExplosionForPlayer(int playerid, float X, float Y, float Z, int type, float Radius));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool SetSpawnInfo(int playerid, int team, int skin, float x, float y, float z, float rotation, int weapon1, int weapon1_ammo, int weapon2, int weapon2_ammo, int weapon3, int weapon3_ammo) {
  return sampgdk_SetSpawnInfo(playerid, team, skin, x, y, z, rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
}

inline bool SpawnPlayer(int playerid) {
  return sampgdk_SpawnPlayer(playerid);
}

inline bool SetPlayerPos(int playerid, float x, float y, float z) {
  return sampgdk_SetPlayerPos(playerid, x, y, z);
}

inline bool SetPlayerPosFindZ(int playerid, float x, float y, float z) {
  return sampgdk_SetPlayerPosFindZ(playerid, x, y, z);
}

inline bool GetPlayerPos(int playerid, float * x, float * y, float * z) {
  return sampgdk_GetPlayerPos(playerid, x, y, z);
}

inline bool SetPlayerFacingAngle(int playerid, float angle) {
  return sampgdk_SetPlayerFacingAngle(playerid, angle);
}

inline bool GetPlayerFacingAngle(int playerid, float * angle) {
  return sampgdk_GetPlayerFacingAngle(playerid, angle);
}

inline bool IsPlayerInRangeOfPoint(int playerid, float range, float x, float y, float z) {
  return sampgdk_IsPlayerInRangeOfPoint(playerid, range, x, y, z);
}

inline float GetPlayerDistanceFromPoint(int playerid, float x, float y, float z) {
  return sampgdk_GetPlayerDistanceFromPoint(playerid, x, y, z);
}

inline bool IsPlayerStreamedIn(int playerid, int forplayerid) {
  return sampgdk_IsPlayerStreamedIn(playerid, forplayerid);
}

inline bool SetPlayerInterior(int playerid, int interiorid) {
  return sampgdk_SetPlayerInterior(playerid, interiorid);
}

inline int GetPlayerInterior(int playerid) {
  return sampgdk_GetPlayerInterior(playerid);
}

inline bool SetPlayerHealth(int playerid, float health) {
  return sampgdk_SetPlayerHealth(playerid, health);
}

inline bool GetPlayerHealth(int playerid, float * health) {
  return sampgdk_GetPlayerHealth(playerid, health);
}

inline bool SetPlayerArmour(int playerid, float armour) {
  return sampgdk_SetPlayerArmour(playerid, armour);
}

inline bool GetPlayerArmour(int playerid, float * armour) {
  return sampgdk_GetPlayerArmour(playerid, armour);
}

inline bool SetPlayerAmmo(int playerid, int weaponid, int ammo) {
  return sampgdk_SetPlayerAmmo(playerid, weaponid, ammo);
}

inline int GetPlayerAmmo(int playerid) {
  return sampgdk_GetPlayerAmmo(playerid);
}

inline int GetPlayerWeaponState(int playerid) {
  return sampgdk_GetPlayerWeaponState(playerid);
}

inline int GetPlayerTargetPlayer(int playerid) {
  return sampgdk_GetPlayerTargetPlayer(playerid);
}

inline int GetPlayerTargetActor(int playerid) {
  return sampgdk_GetPlayerTargetActor(playerid);
}

inline bool SetPlayerTeam(int playerid, int teamid) {
  return sampgdk_SetPlayerTeam(playerid, teamid);
}

inline int GetPlayerTeam(int playerid) {
  return sampgdk_GetPlayerTeam(playerid);
}

inline bool SetPlayerScore(int playerid, int score) {
  return sampgdk_SetPlayerScore(playerid, score);
}

inline int GetPlayerScore(int playerid) {
  return sampgdk_GetPlayerScore(playerid);
}

inline int GetPlayerDrunkLevel(int playerid) {
  return sampgdk_GetPlayerDrunkLevel(playerid);
}

inline bool SetPlayerDrunkLevel(int playerid, int level) {
  return sampgdk_SetPlayerDrunkLevel(playerid, level);
}

inline bool SetPlayerColor(int playerid, int color) {
  return sampgdk_SetPlayerColor(playerid, color);
}

inline int GetPlayerColor(int playerid) {
  return sampgdk_GetPlayerColor(playerid);
}

inline bool SetPlayerSkin(int playerid, int skinid) {
  return sampgdk_SetPlayerSkin(playerid, skinid);
}

inline int GetPlayerSkin(int playerid) {
  return sampgdk_GetPlayerSkin(playerid);
}

inline bool GivePlayerWeapon(int playerid, int weaponid, int ammo) {
  return sampgdk_GivePlayerWeapon(playerid, weaponid, ammo);
}

inline bool ResetPlayerWeapons(int playerid) {
  return sampgdk_ResetPlayerWeapons(playerid);
}

inline bool SetPlayerArmedWeapon(int playerid, int weaponid) {
  return sampgdk_SetPlayerArmedWeapon(playerid, weaponid);
}

inline bool GetPlayerWeaponData(int playerid, int slot, int * weapon, int * ammo) {
  return sampgdk_GetPlayerWeaponData(playerid, slot, weapon, ammo);
}

inline bool GivePlayerMoney(int playerid, int money) {
  return sampgdk_GivePlayerMoney(playerid, money);
}

inline bool ResetPlayerMoney(int playerid) {
  return sampgdk_ResetPlayerMoney(playerid);
}

inline int SetPlayerName(int playerid, const char * name) {
  return sampgdk_SetPlayerName(playerid, name);
}

inline int GetPlayerMoney(int playerid) {
  return sampgdk_GetPlayerMoney(playerid);
}

inline int GetPlayerState(int playerid) {
  return sampgdk_GetPlayerState(playerid);
}

inline bool GetPlayerIp(int playerid, char * ip, int size) {
  return sampgdk_GetPlayerIp(playerid, ip, size);
}

inline int GetPlayerPing(int playerid) {
  return sampgdk_GetPlayerPing(playerid);
}

inline int GetPlayerWeapon(int playerid) {
  return sampgdk_GetPlayerWeapon(playerid);
}

inline bool GetPlayerKeys(int playerid, int * keys, int * updown, int * leftright) {
  return sampgdk_GetPlayerKeys(playerid, keys, updown, leftright);
}

inline int GetPlayerName(int playerid, char * name, int size) {
  return sampgdk_GetPlayerName(playerid, name, size);
}

inline bool SetPlayerTime(int playerid, int hour, int minute) {
  return sampgdk_SetPlayerTime(playerid, hour, minute);
}

inline bool GetPlayerTime(int playerid, int * hour, int * minute) {
  return sampgdk_GetPlayerTime(playerid, hour, minute);
}

inline bool TogglePlayerClock(int playerid, bool toggle) {
  return sampgdk_TogglePlayerClock(playerid, toggle);
}

inline bool SetPlayerWeather(int playerid, int weather) {
  return sampgdk_SetPlayerWeather(playerid, weather);
}

inline bool ForceClassSelection(int playerid) {
  return sampgdk_ForceClassSelection(playerid);
}

inline bool SetPlayerWantedLevel(int playerid, int level) {
  return sampgdk_SetPlayerWantedLevel(playerid, level);
}

inline int GetPlayerWantedLevel(int playerid) {
  return sampgdk_GetPlayerWantedLevel(playerid);
}

inline bool SetPlayerFightingStyle(int playerid, int style) {
  return sampgdk_SetPlayerFightingStyle(playerid, style);
}

inline int GetPlayerFightingStyle(int playerid) {
  return sampgdk_GetPlayerFightingStyle(playerid);
}

inline bool SetPlayerVelocity(int playerid, float x, float y, float z) {
  return sampgdk_SetPlayerVelocity(playerid, x, y, z);
}

inline bool GetPlayerVelocity(int playerid, float * x, float * y, float * z) {
  return sampgdk_GetPlayerVelocity(playerid, x, y, z);
}

inline bool PlayCrimeReportForPlayer(int playerid, int suspectid, int crime) {
  return sampgdk_PlayCrimeReportForPlayer(playerid, suspectid, crime);
}

inline bool PlayAudioStreamForPlayer(int playerid, const char * url, float posX = 0.0, float posY = 0.0, float posZ = 0.0, float distance = 50.0, bool usepos = false) {
  return sampgdk_PlayAudioStreamForPlayer(playerid, url, posX, posY, posZ, distance, usepos);
}

inline bool StopAudioStreamForPlayer(int playerid) {
  return sampgdk_StopAudioStreamForPlayer(playerid);
}

inline bool SetPlayerShopName(int playerid, const char * shopname) {
  return sampgdk_SetPlayerShopName(playerid, shopname);
}

inline bool SetPlayerSkillLevel(int playerid, int skill, int level) {
  return sampgdk_SetPlayerSkillLevel(playerid, skill, level);
}

inline int GetPlayerSurfingVehicleID(int playerid) {
  return sampgdk_GetPlayerSurfingVehicleID(playerid);
}

inline int GetPlayerSurfingObjectID(int playerid) {
  return sampgdk_GetPlayerSurfingObjectID(playerid);
}

inline bool RemoveBuildingForPlayer(int playerid, int modelid, float fX, float fY, float fZ, float fRadius) {
  return sampgdk_RemoveBuildingForPlayer(playerid, modelid, fX, fY, fZ, fRadius);
}

inline bool GetPlayerLastShotVectors(int playerid, float * fOriginX, float * fOriginY, float * fOriginZ, float * fHitPosX, float * fHitPosY, float * fHitPosZ) {
  return sampgdk_GetPlayerLastShotVectors(playerid, fOriginX, fOriginY, fOriginZ, fHitPosX, fHitPosY, fHitPosZ);
}

inline bool SetPlayerAttachedObject(int playerid, int index, int modelid, int bone, float fOffsetX = 0.0, float fOffsetY = 0.0, float fOffsetZ = 0.0, float fRotX = 0.0, float fRotY = 0.0, float fRotZ = 0.0, float fScaleX = 1.0, float fScaleY = 1.0, float fScaleZ = 1.0, int materialcolor1 = 0, int materialcolor2 = 0) {
  return sampgdk_SetPlayerAttachedObject(playerid, index, modelid, bone, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ, materialcolor1, materialcolor2);
}

inline bool RemovePlayerAttachedObject(int playerid, int index) {
  return sampgdk_RemovePlayerAttachedObject(playerid, index);
}

inline bool IsPlayerAttachedObjectSlotUsed(int playerid, int index) {
  return sampgdk_IsPlayerAttachedObjectSlotUsed(playerid, index);
}

inline bool EditAttachedObject(int playerid, int index) {
  return sampgdk_EditAttachedObject(playerid, index);
}

inline int CreatePlayerTextDraw(int playerid, float x, float y, const char * text) {
  return sampgdk_CreatePlayerTextDraw(playerid, x, y, text);
}

inline bool PlayerTextDrawDestroy(int playerid, int text) {
  return sampgdk_PlayerTextDrawDestroy(playerid, text);
}

inline bool PlayerTextDrawLetterSize(int playerid, int text, float x, float y) {
  return sampgdk_PlayerTextDrawLetterSize(playerid, text, x, y);
}

inline bool PlayerTextDrawTextSize(int playerid, int text, float x, float y) {
  return sampgdk_PlayerTextDrawTextSize(playerid, text, x, y);
}

inline bool PlayerTextDrawAlignment(int playerid, int text, int alignment) {
  return sampgdk_PlayerTextDrawAlignment(playerid, text, alignment);
}

inline bool PlayerTextDrawColor(int playerid, int text, int color) {
  return sampgdk_PlayerTextDrawColor(playerid, text, color);
}

inline bool PlayerTextDrawUseBox(int playerid, int text, bool use) {
  return sampgdk_PlayerTextDrawUseBox(playerid, text, use);
}

inline bool PlayerTextDrawBoxColor(int playerid, int text, int color) {
  return sampgdk_PlayerTextDrawBoxColor(playerid, text, color);
}

inline bool PlayerTextDrawSetShadow(int playerid, int text, int size) {
  return sampgdk_PlayerTextDrawSetShadow(playerid, text, size);
}

inline bool PlayerTextDrawSetOutline(int playerid, int text, int size) {
  return sampgdk_PlayerTextDrawSetOutline(playerid, text, size);
}

inline bool PlayerTextDrawBackgroundColor(int playerid, int text, int color) {
  return sampgdk_PlayerTextDrawBackgroundColor(playerid, text, color);
}

inline bool PlayerTextDrawFont(int playerid, int text, int font) {
  return sampgdk_PlayerTextDrawFont(playerid, text, font);
}

inline bool PlayerTextDrawSetProportional(int playerid, int text, bool set) {
  return sampgdk_PlayerTextDrawSetProportional(playerid, text, set);
}

inline bool PlayerTextDrawSetSelectable(int playerid, int text, bool set) {
  return sampgdk_PlayerTextDrawSetSelectable(playerid, text, set);
}

inline bool PlayerTextDrawShow(int playerid, int text) {
  return sampgdk_PlayerTextDrawShow(playerid, text);
}

inline bool PlayerTextDrawHide(int playerid, int text) {
  return sampgdk_PlayerTextDrawHide(playerid, text);
}

inline bool PlayerTextDrawSetString(int playerid, int text, const char * string) {
  return sampgdk_PlayerTextDrawSetString(playerid, text, string);
}

inline bool PlayerTextDrawSetPreviewModel(int playerid, int text, int modelindex) {
  return sampgdk_PlayerTextDrawSetPreviewModel(playerid, text, modelindex);
}

inline bool PlayerTextDrawSetPreviewRot(int playerid, int text, float fRotX, float fRotY, float fRotZ, float fZoom = 1.0) {
  return sampgdk_PlayerTextDrawSetPreviewRot(playerid, text, fRotX, fRotY, fRotZ, fZoom);
}

inline bool PlayerTextDrawSetPreviewVehCol(int playerid, int text, int color1, int color2) {
  return sampgdk_PlayerTextDrawSetPreviewVehCol(playerid, text, color1, color2);
}

inline bool SetPVarInt(int playerid, const char * varname, int value) {
  return sampgdk_SetPVarInt(playerid, varname, value);
}

inline int GetPVarInt(int playerid, const char * varname) {
  return sampgdk_GetPVarInt(playerid, varname);
}

inline bool SetPVarString(int playerid, const char * varname, const char * value) {
  return sampgdk_SetPVarString(playerid, varname, value);
}

inline bool GetPVarString(int playerid, const char * varname, char * value, int size) {
  return sampgdk_GetPVarString(playerid, varname, value, size);
}

inline bool SetPVarFloat(int playerid, const char * varname, float value) {
  return sampgdk_SetPVarFloat(playerid, varname, value);
}

inline float GetPVarFloat(int playerid, const char * varname) {
  return sampgdk_GetPVarFloat(playerid, varname);
}

inline bool DeletePVar(int playerid, const char * varname) {
  return sampgdk_DeletePVar(playerid, varname);
}

inline int GetPVarsUpperIndex(int playerid) {
  return sampgdk_GetPVarsUpperIndex(playerid);
}

inline bool GetPVarNameAtIndex(int playerid, int index, char * varname, int size) {
  return sampgdk_GetPVarNameAtIndex(playerid, index, varname, size);
}

inline int GetPVarType(int playerid, const char * varname) {
  return sampgdk_GetPVarType(playerid, varname);
}

inline bool SetPlayerChatBubble(int playerid, const char * text, int color, float drawdistance, int expiretime) {
  return sampgdk_SetPlayerChatBubble(playerid, text, color, drawdistance, expiretime);
}

inline bool PutPlayerInVehicle(int playerid, int vehicleid, int seatid) {
  return sampgdk_PutPlayerInVehicle(playerid, vehicleid, seatid);
}

inline int GetPlayerVehicleID(int playerid) {
  return sampgdk_GetPlayerVehicleID(playerid);
}

inline int GetPlayerVehicleSeat(int playerid) {
  return sampgdk_GetPlayerVehicleSeat(playerid);
}

inline bool RemovePlayerFromVehicle(int playerid) {
  return sampgdk_RemovePlayerFromVehicle(playerid);
}

inline bool TogglePlayerControllable(int playerid, bool toggle) {
  return sampgdk_TogglePlayerControllable(playerid, toggle);
}

inline bool PlayerPlaySound(int playerid, int soundid, float x, float y, float z) {
  return sampgdk_PlayerPlaySound(playerid, soundid, x, y, z);
}

inline bool ApplyAnimation(int playerid, const char * animlib, const char * animname, float fDelta, bool loop, bool lockx, bool locky, bool freeze, int time, bool forcesync = false) {
  return sampgdk_ApplyAnimation(playerid, animlib, animname, fDelta, loop, lockx, locky, freeze, time, forcesync);
}

inline bool ClearAnimations(int playerid, bool forcesync = false) {
  return sampgdk_ClearAnimations(playerid, forcesync);
}

inline int GetPlayerAnimationIndex(int playerid) {
  return sampgdk_GetPlayerAnimationIndex(playerid);
}

inline bool GetAnimationName(int index, char * animlib, int animlib_size, char * animname, int animname_size) {
  return sampgdk_GetAnimationName(index, animlib, animlib_size, animname, animname_size);
}

inline int GetPlayerSpecialAction(int playerid) {
  return sampgdk_GetPlayerSpecialAction(playerid);
}

inline bool SetPlayerSpecialAction(int playerid, int actionid) {
  return sampgdk_SetPlayerSpecialAction(playerid, actionid);
}

inline bool DisableRemoteVehicleCollisions(int playerid, bool disable) {
  return sampgdk_DisableRemoteVehicleCollisions(playerid, disable);
}

inline bool SetPlayerCheckpoint(int playerid, float x, float y, float z, float size) {
  return sampgdk_SetPlayerCheckpoint(playerid, x, y, z, size);
}

inline bool DisablePlayerCheckpoint(int playerid) {
  return sampgdk_DisablePlayerCheckpoint(playerid);
}

inline bool SetPlayerRaceCheckpoint(int playerid, int type, float x, float y, float z, float nextx, float nexty, float nextz, float size) {
  return sampgdk_SetPlayerRaceCheckpoint(playerid, type, x, y, z, nextx, nexty, nextz, size);
}

inline bool DisablePlayerRaceCheckpoint(int playerid) {
  return sampgdk_DisablePlayerRaceCheckpoint(playerid);
}

inline bool SetPlayerWorldBounds(int playerid, float x_max, float x_min, float y_max, float y_min) {
  return sampgdk_SetPlayerWorldBounds(playerid, x_max, x_min, y_max, y_min);
}

inline bool SetPlayerMarkerForPlayer(int playerid, int showplayerid, int color) {
  return sampgdk_SetPlayerMarkerForPlayer(playerid, showplayerid, color);
}

inline bool ShowPlayerNameTagForPlayer(int playerid, int showplayerid, bool show) {
  return sampgdk_ShowPlayerNameTagForPlayer(playerid, showplayerid, show);
}

inline bool SetPlayerMapIcon(int playerid, int iconid, float x, float y, float z, int markertype, int color, int style = MAPICON_LOCAL) {
  return sampgdk_SetPlayerMapIcon(playerid, iconid, x, y, z, markertype, color, style);
}

inline bool RemovePlayerMapIcon(int playerid, int iconid) {
  return sampgdk_RemovePlayerMapIcon(playerid, iconid);
}

inline bool AllowPlayerTeleport(int playerid, bool allow) {
  return sampgdk_AllowPlayerTeleport(playerid, allow);
}

inline bool SetPlayerCameraPos(int playerid, float x, float y, float z) {
  return sampgdk_SetPlayerCameraPos(playerid, x, y, z);
}

inline bool SetPlayerCameraLookAt(int playerid, float x, float y, float z, int cut = CAMERA_CUT) {
  return sampgdk_SetPlayerCameraLookAt(playerid, x, y, z, cut);
}

inline bool SetCameraBehindPlayer(int playerid) {
  return sampgdk_SetCameraBehindPlayer(playerid);
}

inline bool GetPlayerCameraPos(int playerid, float * x, float * y, float * z) {
  return sampgdk_GetPlayerCameraPos(playerid, x, y, z);
}

inline bool GetPlayerCameraFrontVector(int playerid, float * x, float * y, float * z) {
  return sampgdk_GetPlayerCameraFrontVector(playerid, x, y, z);
}

inline int GetPlayerCameraMode(int playerid) {
  return sampgdk_GetPlayerCameraMode(playerid);
}

inline bool EnablePlayerCameraTarget(int playerid, bool enable) {
  return sampgdk_EnablePlayerCameraTarget(playerid, enable);
}

inline int GetPlayerCameraTargetObject(int playerid) {
  return sampgdk_GetPlayerCameraTargetObject(playerid);
}

inline int GetPlayerCameraTargetVehicle(int playerid) {
  return sampgdk_GetPlayerCameraTargetVehicle(playerid);
}

inline int GetPlayerCameraTargetPlayer(int playerid) {
  return sampgdk_GetPlayerCameraTargetPlayer(playerid);
}

inline int GetPlayerCameraTargetActor(int playerid) {
  return sampgdk_GetPlayerCameraTargetActor(playerid);
}

inline float GetPlayerCameraAspectRatio(int playerid) {
  return sampgdk_GetPlayerCameraAspectRatio(playerid);
}

inline float GetPlayerCameraZoom(int playerid) {
  return sampgdk_GetPlayerCameraZoom(playerid);
}

inline bool AttachCameraToObject(int playerid, int objectid) {
  return sampgdk_AttachCameraToObject(playerid, objectid);
}

inline bool AttachCameraToPlayerObject(int playerid, int playerobjectid) {
  return sampgdk_AttachCameraToPlayerObject(playerid, playerobjectid);
}

inline bool InterpolateCameraPos(int playerid, float FromX, float FromY, float FromZ, float ToX, float ToY, float ToZ, int time, int cut = CAMERA_CUT) {
  return sampgdk_InterpolateCameraPos(playerid, FromX, FromY, FromZ, ToX, ToY, ToZ, time, cut);
}

inline bool InterpolateCameraLookAt(int playerid, float FromX, float FromY, float FromZ, float ToX, float ToY, float ToZ, int time, int cut = CAMERA_CUT) {
  return sampgdk_InterpolateCameraLookAt(playerid, FromX, FromY, FromZ, ToX, ToY, ToZ, time, cut);
}

inline bool IsPlayerConnected(int playerid) {
  return sampgdk_IsPlayerConnected(playerid);
}

inline bool IsPlayerInVehicle(int playerid, int vehicleid) {
  return sampgdk_IsPlayerInVehicle(playerid, vehicleid);
}

inline bool IsPlayerInAnyVehicle(int playerid) {
  return sampgdk_IsPlayerInAnyVehicle(playerid);
}

inline bool IsPlayerInCheckpoint(int playerid) {
  return sampgdk_IsPlayerInCheckpoint(playerid);
}

inline bool IsPlayerInRaceCheckpoint(int playerid) {
  return sampgdk_IsPlayerInRaceCheckpoint(playerid);
}

inline bool SetPlayerVirtualWorld(int playerid, int worldid) {
  return sampgdk_SetPlayerVirtualWorld(playerid, worldid);
}

inline int GetPlayerVirtualWorld(int playerid) {
  return sampgdk_GetPlayerVirtualWorld(playerid);
}

inline bool EnableStuntBonusForPlayer(int playerid, bool enable) {
  return sampgdk_EnableStuntBonusForPlayer(playerid, enable);
}

inline bool EnableStuntBonusForAll(bool enable) {
  return sampgdk_EnableStuntBonusForAll(enable);
}

inline bool TogglePlayerSpectating(int playerid, bool toggle) {
  return sampgdk_TogglePlayerSpectating(playerid, toggle);
}

inline bool PlayerSpectatePlayer(int playerid, int targetplayerid, int mode = SPECTATE_MODE_NORMAL) {
  return sampgdk_PlayerSpectatePlayer(playerid, targetplayerid, mode);
}

inline bool PlayerSpectateVehicle(int playerid, int targetvehicleid, int mode = SPECTATE_MODE_NORMAL) {
  return sampgdk_PlayerSpectateVehicle(playerid, targetvehicleid, mode);
}

inline bool StartRecordingPlayerData(int playerid, int recordtype, const char * recordname) {
  return sampgdk_StartRecordingPlayerData(playerid, recordtype, recordname);
}

inline bool StopRecordingPlayerData(int playerid) {
  return sampgdk_StopRecordingPlayerData(playerid);
}

inline bool CreateExplosionForPlayer(int playerid, float X, float Y, float Z, int type, float Radius) {
  return sampgdk_CreateExplosionForPlayer(playerid, X, Y, Z, type, Radius);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  SetSpawnInfo
#define SetSpawnInfo sampgdk_SetSpawnInfo

#undef  SpawnPlayer
#define SpawnPlayer sampgdk_SpawnPlayer

#undef  SetPlayerPos
#define SetPlayerPos sampgdk_SetPlayerPos

#undef  SetPlayerPosFindZ
#define SetPlayerPosFindZ sampgdk_SetPlayerPosFindZ

#undef  GetPlayerPos
#define GetPlayerPos sampgdk_GetPlayerPos

#undef  SetPlayerFacingAngle
#define SetPlayerFacingAngle sampgdk_SetPlayerFacingAngle

#undef  GetPlayerFacingAngle
#define GetPlayerFacingAngle sampgdk_GetPlayerFacingAngle

#undef  IsPlayerInRangeOfPoint
#define IsPlayerInRangeOfPoint sampgdk_IsPlayerInRangeOfPoint

#undef  GetPlayerDistanceFromPoint
#define GetPlayerDistanceFromPoint sampgdk_GetPlayerDistanceFromPoint

#undef  IsPlayerStreamedIn
#define IsPlayerStreamedIn sampgdk_IsPlayerStreamedIn

#undef  SetPlayerInterior
#define SetPlayerInterior sampgdk_SetPlayerInterior

#undef  GetPlayerInterior
#define GetPlayerInterior sampgdk_GetPlayerInterior

#undef  SetPlayerHealth
#define SetPlayerHealth sampgdk_SetPlayerHealth

#undef  GetPlayerHealth
#define GetPlayerHealth sampgdk_GetPlayerHealth

#undef  SetPlayerArmour
#define SetPlayerArmour sampgdk_SetPlayerArmour

#undef  GetPlayerArmour
#define GetPlayerArmour sampgdk_GetPlayerArmour

#undef  SetPlayerAmmo
#define SetPlayerAmmo sampgdk_SetPlayerAmmo

#undef  GetPlayerAmmo
#define GetPlayerAmmo sampgdk_GetPlayerAmmo

#undef  GetPlayerWeaponState
#define GetPlayerWeaponState sampgdk_GetPlayerWeaponState

#undef  GetPlayerTargetPlayer
#define GetPlayerTargetPlayer sampgdk_GetPlayerTargetPlayer

#undef  GetPlayerTargetActor
#define GetPlayerTargetActor sampgdk_GetPlayerTargetActor

#undef  SetPlayerTeam
#define SetPlayerTeam sampgdk_SetPlayerTeam

#undef  GetPlayerTeam
#define GetPlayerTeam sampgdk_GetPlayerTeam

#undef  SetPlayerScore
#define SetPlayerScore sampgdk_SetPlayerScore

#undef  GetPlayerScore
#define GetPlayerScore sampgdk_GetPlayerScore

#undef  GetPlayerDrunkLevel
#define GetPlayerDrunkLevel sampgdk_GetPlayerDrunkLevel

#undef  SetPlayerDrunkLevel
#define SetPlayerDrunkLevel sampgdk_SetPlayerDrunkLevel

#undef  SetPlayerColor
#define SetPlayerColor sampgdk_SetPlayerColor

#undef  GetPlayerColor
#define GetPlayerColor sampgdk_GetPlayerColor

#undef  SetPlayerSkin
#define SetPlayerSkin sampgdk_SetPlayerSkin

#undef  GetPlayerSkin
#define GetPlayerSkin sampgdk_GetPlayerSkin

#undef  GivePlayerWeapon
#define GivePlayerWeapon sampgdk_GivePlayerWeapon

#undef  ResetPlayerWeapons
#define ResetPlayerWeapons sampgdk_ResetPlayerWeapons

#undef  SetPlayerArmedWeapon
#define SetPlayerArmedWeapon sampgdk_SetPlayerArmedWeapon

#undef  GetPlayerWeaponData
#define GetPlayerWeaponData sampgdk_GetPlayerWeaponData

#undef  GivePlayerMoney
#define GivePlayerMoney sampgdk_GivePlayerMoney

#undef  ResetPlayerMoney
#define ResetPlayerMoney sampgdk_ResetPlayerMoney

#undef  SetPlayerName
#define SetPlayerName sampgdk_SetPlayerName

#undef  GetPlayerMoney
#define GetPlayerMoney sampgdk_GetPlayerMoney

#undef  GetPlayerState
#define GetPlayerState sampgdk_GetPlayerState

#undef  GetPlayerIp
#define GetPlayerIp sampgdk_GetPlayerIp

#undef  GetPlayerPing
#define GetPlayerPing sampgdk_GetPlayerPing

#undef  GetPlayerWeapon
#define GetPlayerWeapon sampgdk_GetPlayerWeapon

#undef  GetPlayerKeys
#define GetPlayerKeys sampgdk_GetPlayerKeys

#undef  GetPlayerName
#define GetPlayerName sampgdk_GetPlayerName

#undef  SetPlayerTime
#define SetPlayerTime sampgdk_SetPlayerTime

#undef  GetPlayerTime
#define GetPlayerTime sampgdk_GetPlayerTime

#undef  TogglePlayerClock
#define TogglePlayerClock sampgdk_TogglePlayerClock

#undef  SetPlayerWeather
#define SetPlayerWeather sampgdk_SetPlayerWeather

#undef  ForceClassSelection
#define ForceClassSelection sampgdk_ForceClassSelection

#undef  SetPlayerWantedLevel
#define SetPlayerWantedLevel sampgdk_SetPlayerWantedLevel

#undef  GetPlayerWantedLevel
#define GetPlayerWantedLevel sampgdk_GetPlayerWantedLevel

#undef  SetPlayerFightingStyle
#define SetPlayerFightingStyle sampgdk_SetPlayerFightingStyle

#undef  GetPlayerFightingStyle
#define GetPlayerFightingStyle sampgdk_GetPlayerFightingStyle

#undef  SetPlayerVelocity
#define SetPlayerVelocity sampgdk_SetPlayerVelocity

#undef  GetPlayerVelocity
#define GetPlayerVelocity sampgdk_GetPlayerVelocity

#undef  PlayCrimeReportForPlayer
#define PlayCrimeReportForPlayer sampgdk_PlayCrimeReportForPlayer

#undef  PlayAudioStreamForPlayer
#define PlayAudioStreamForPlayer sampgdk_PlayAudioStreamForPlayer

#undef  StopAudioStreamForPlayer
#define StopAudioStreamForPlayer sampgdk_StopAudioStreamForPlayer

#undef  SetPlayerShopName
#define SetPlayerShopName sampgdk_SetPlayerShopName

#undef  SetPlayerSkillLevel
#define SetPlayerSkillLevel sampgdk_SetPlayerSkillLevel

#undef  GetPlayerSurfingVehicleID
#define GetPlayerSurfingVehicleID sampgdk_GetPlayerSurfingVehicleID

#undef  GetPlayerSurfingObjectID
#define GetPlayerSurfingObjectID sampgdk_GetPlayerSurfingObjectID

#undef  RemoveBuildingForPlayer
#define RemoveBuildingForPlayer sampgdk_RemoveBuildingForPlayer

#undef  GetPlayerLastShotVectors
#define GetPlayerLastShotVectors sampgdk_GetPlayerLastShotVectors

#undef  SetPlayerAttachedObject
#define SetPlayerAttachedObject sampgdk_SetPlayerAttachedObject

#undef  RemovePlayerAttachedObject
#define RemovePlayerAttachedObject sampgdk_RemovePlayerAttachedObject

#undef  IsPlayerAttachedObjectSlotUsed
#define IsPlayerAttachedObjectSlotUsed sampgdk_IsPlayerAttachedObjectSlotUsed

#undef  EditAttachedObject
#define EditAttachedObject sampgdk_EditAttachedObject

#undef  CreatePlayerTextDraw
#define CreatePlayerTextDraw sampgdk_CreatePlayerTextDraw

#undef  PlayerTextDrawDestroy
#define PlayerTextDrawDestroy sampgdk_PlayerTextDrawDestroy

#undef  PlayerTextDrawLetterSize
#define PlayerTextDrawLetterSize sampgdk_PlayerTextDrawLetterSize

#undef  PlayerTextDrawTextSize
#define PlayerTextDrawTextSize sampgdk_PlayerTextDrawTextSize

#undef  PlayerTextDrawAlignment
#define PlayerTextDrawAlignment sampgdk_PlayerTextDrawAlignment

#undef  PlayerTextDrawColor
#define PlayerTextDrawColor sampgdk_PlayerTextDrawColor

#undef  PlayerTextDrawUseBox
#define PlayerTextDrawUseBox sampgdk_PlayerTextDrawUseBox

#undef  PlayerTextDrawBoxColor
#define PlayerTextDrawBoxColor sampgdk_PlayerTextDrawBoxColor

#undef  PlayerTextDrawSetShadow
#define PlayerTextDrawSetShadow sampgdk_PlayerTextDrawSetShadow

#undef  PlayerTextDrawSetOutline
#define PlayerTextDrawSetOutline sampgdk_PlayerTextDrawSetOutline

#undef  PlayerTextDrawBackgroundColor
#define PlayerTextDrawBackgroundColor sampgdk_PlayerTextDrawBackgroundColor

#undef  PlayerTextDrawFont
#define PlayerTextDrawFont sampgdk_PlayerTextDrawFont

#undef  PlayerTextDrawSetProportional
#define PlayerTextDrawSetProportional sampgdk_PlayerTextDrawSetProportional

#undef  PlayerTextDrawSetSelectable
#define PlayerTextDrawSetSelectable sampgdk_PlayerTextDrawSetSelectable

#undef  PlayerTextDrawShow
#define PlayerTextDrawShow sampgdk_PlayerTextDrawShow

#undef  PlayerTextDrawHide
#define PlayerTextDrawHide sampgdk_PlayerTextDrawHide

#undef  PlayerTextDrawSetString
#define PlayerTextDrawSetString sampgdk_PlayerTextDrawSetString

#undef  PlayerTextDrawSetPreviewModel
#define PlayerTextDrawSetPreviewModel sampgdk_PlayerTextDrawSetPreviewModel

#undef  PlayerTextDrawSetPreviewRot
#define PlayerTextDrawSetPreviewRot sampgdk_PlayerTextDrawSetPreviewRot

#undef  PlayerTextDrawSetPreviewVehCol
#define PlayerTextDrawSetPreviewVehCol sampgdk_PlayerTextDrawSetPreviewVehCol

#undef  SetPVarInt
#define SetPVarInt sampgdk_SetPVarInt

#undef  GetPVarInt
#define GetPVarInt sampgdk_GetPVarInt

#undef  SetPVarString
#define SetPVarString sampgdk_SetPVarString

#undef  GetPVarString
#define GetPVarString sampgdk_GetPVarString

#undef  SetPVarFloat
#define SetPVarFloat sampgdk_SetPVarFloat

#undef  GetPVarFloat
#define GetPVarFloat sampgdk_GetPVarFloat

#undef  DeletePVar
#define DeletePVar sampgdk_DeletePVar

#undef  GetPVarsUpperIndex
#define GetPVarsUpperIndex sampgdk_GetPVarsUpperIndex

#undef  GetPVarNameAtIndex
#define GetPVarNameAtIndex sampgdk_GetPVarNameAtIndex

#undef  GetPVarType
#define GetPVarType sampgdk_GetPVarType

#undef  SetPlayerChatBubble
#define SetPlayerChatBubble sampgdk_SetPlayerChatBubble

#undef  PutPlayerInVehicle
#define PutPlayerInVehicle sampgdk_PutPlayerInVehicle

#undef  GetPlayerVehicleID
#define GetPlayerVehicleID sampgdk_GetPlayerVehicleID

#undef  GetPlayerVehicleSeat
#define GetPlayerVehicleSeat sampgdk_GetPlayerVehicleSeat

#undef  RemovePlayerFromVehicle
#define RemovePlayerFromVehicle sampgdk_RemovePlayerFromVehicle

#undef  TogglePlayerControllable
#define TogglePlayerControllable sampgdk_TogglePlayerControllable

#undef  PlayerPlaySound
#define PlayerPlaySound sampgdk_PlayerPlaySound

#undef  ApplyAnimation
#define ApplyAnimation sampgdk_ApplyAnimation

#undef  ClearAnimations
#define ClearAnimations sampgdk_ClearAnimations

#undef  GetPlayerAnimationIndex
#define GetPlayerAnimationIndex sampgdk_GetPlayerAnimationIndex

#undef  GetAnimationName
#define GetAnimationName sampgdk_GetAnimationName

#undef  GetPlayerSpecialAction
#define GetPlayerSpecialAction sampgdk_GetPlayerSpecialAction

#undef  SetPlayerSpecialAction
#define SetPlayerSpecialAction sampgdk_SetPlayerSpecialAction

#undef  DisableRemoteVehicleCollisions
#define DisableRemoteVehicleCollisions sampgdk_DisableRemoteVehicleCollisions

#undef  SetPlayerCheckpoint
#define SetPlayerCheckpoint sampgdk_SetPlayerCheckpoint

#undef  DisablePlayerCheckpoint
#define DisablePlayerCheckpoint sampgdk_DisablePlayerCheckpoint

#undef  SetPlayerRaceCheckpoint
#define SetPlayerRaceCheckpoint sampgdk_SetPlayerRaceCheckpoint

#undef  DisablePlayerRaceCheckpoint
#define DisablePlayerRaceCheckpoint sampgdk_DisablePlayerRaceCheckpoint

#undef  SetPlayerWorldBounds
#define SetPlayerWorldBounds sampgdk_SetPlayerWorldBounds

#undef  SetPlayerMarkerForPlayer
#define SetPlayerMarkerForPlayer sampgdk_SetPlayerMarkerForPlayer

#undef  ShowPlayerNameTagForPlayer
#define ShowPlayerNameTagForPlayer sampgdk_ShowPlayerNameTagForPlayer

#undef  SetPlayerMapIcon
#define SetPlayerMapIcon sampgdk_SetPlayerMapIcon

#undef  RemovePlayerMapIcon
#define RemovePlayerMapIcon sampgdk_RemovePlayerMapIcon

#undef  AllowPlayerTeleport
#define AllowPlayerTeleport sampgdk_AllowPlayerTeleport

#undef  SetPlayerCameraPos
#define SetPlayerCameraPos sampgdk_SetPlayerCameraPos

#undef  SetPlayerCameraLookAt
#define SetPlayerCameraLookAt sampgdk_SetPlayerCameraLookAt

#undef  SetCameraBehindPlayer
#define SetCameraBehindPlayer sampgdk_SetCameraBehindPlayer

#undef  GetPlayerCameraPos
#define GetPlayerCameraPos sampgdk_GetPlayerCameraPos

#undef  GetPlayerCameraFrontVector
#define GetPlayerCameraFrontVector sampgdk_GetPlayerCameraFrontVector

#undef  GetPlayerCameraMode
#define GetPlayerCameraMode sampgdk_GetPlayerCameraMode

#undef  EnablePlayerCameraTarget
#define EnablePlayerCameraTarget sampgdk_EnablePlayerCameraTarget

#undef  GetPlayerCameraTargetObject
#define GetPlayerCameraTargetObject sampgdk_GetPlayerCameraTargetObject

#undef  GetPlayerCameraTargetVehicle
#define GetPlayerCameraTargetVehicle sampgdk_GetPlayerCameraTargetVehicle

#undef  GetPlayerCameraTargetPlayer
#define GetPlayerCameraTargetPlayer sampgdk_GetPlayerCameraTargetPlayer

#undef  GetPlayerCameraTargetActor
#define GetPlayerCameraTargetActor sampgdk_GetPlayerCameraTargetActor

#undef  GetPlayerCameraAspectRatio
#define GetPlayerCameraAspectRatio sampgdk_GetPlayerCameraAspectRatio

#undef  GetPlayerCameraZoom
#define GetPlayerCameraZoom sampgdk_GetPlayerCameraZoom

#undef  AttachCameraToObject
#define AttachCameraToObject sampgdk_AttachCameraToObject

#undef  AttachCameraToPlayerObject
#define AttachCameraToPlayerObject sampgdk_AttachCameraToPlayerObject

#undef  InterpolateCameraPos
#define InterpolateCameraPos sampgdk_InterpolateCameraPos

#undef  InterpolateCameraLookAt
#define InterpolateCameraLookAt sampgdk_InterpolateCameraLookAt

#undef  IsPlayerConnected
#define IsPlayerConnected sampgdk_IsPlayerConnected

#undef  IsPlayerInVehicle
#define IsPlayerInVehicle sampgdk_IsPlayerInVehicle

#undef  IsPlayerInAnyVehicle
#define IsPlayerInAnyVehicle sampgdk_IsPlayerInAnyVehicle

#undef  IsPlayerInCheckpoint
#define IsPlayerInCheckpoint sampgdk_IsPlayerInCheckpoint

#undef  IsPlayerInRaceCheckpoint
#define IsPlayerInRaceCheckpoint sampgdk_IsPlayerInRaceCheckpoint

#undef  SetPlayerVirtualWorld
#define SetPlayerVirtualWorld sampgdk_SetPlayerVirtualWorld

#undef  GetPlayerVirtualWorld
#define GetPlayerVirtualWorld sampgdk_GetPlayerVirtualWorld

#undef  EnableStuntBonusForPlayer
#define EnableStuntBonusForPlayer sampgdk_EnableStuntBonusForPlayer

#undef  EnableStuntBonusForAll
#define EnableStuntBonusForAll sampgdk_EnableStuntBonusForAll

#undef  TogglePlayerSpectating
#define TogglePlayerSpectating sampgdk_TogglePlayerSpectating

#undef  PlayerSpectatePlayer
#define PlayerSpectatePlayer sampgdk_PlayerSpectatePlayer

#undef  PlayerSpectateVehicle
#define PlayerSpectateVehicle sampgdk_PlayerSpectateVehicle

#undef  StartRecordingPlayerData
#define StartRecordingPlayerData sampgdk_StartRecordingPlayerData

#undef  StopRecordingPlayerData
#define StopRecordingPlayerData sampgdk_StopRecordingPlayerData

#undef  CreateExplosionForPlayer
#define CreateExplosionForPlayer sampgdk_CreateExplosionForPlayer

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_A_PLAYERS_H */

#ifndef SAMPGDK_A_VEHICLES_H
#define SAMPGDK_A_VEHICLES_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */

#define CARMODTYPE_SPOILER (0)
#define CARMODTYPE_HOOD (1)
#define CARMODTYPE_ROOF (2)
#define CARMODTYPE_SIDESKIRT (3)
#define CARMODTYPE_LAMPS (4)
#define CARMODTYPE_NITRO (5)
#define CARMODTYPE_EXHAUST (6)
#define CARMODTYPE_WHEELS (7)
#define CARMODTYPE_STEREO (8)
#define CARMODTYPE_HYDRAULICS (9)
#define CARMODTYPE_FRONT_BUMPER (10)
#define CARMODTYPE_REAR_BUMPER (11)
#define CARMODTYPE_VENT_RIGHT (12)
#define CARMODTYPE_VENT_LEFT (13)
#define VEHICLE_PARAMS_UNSET (-1)
#define VEHICLE_PARAMS_OFF (0)
#define VEHICLE_PARAMS_ON (1)
#define VEHICLE_MODEL_INFO_SIZE (1)
#define VEHICLE_MODEL_INFO_FRONTSEAT (2)
#define VEHICLE_MODEL_INFO_REARSEAT (3)
#define VEHICLE_MODEL_INFO_PETROLCAP (4)
#define VEHICLE_MODEL_INFO_WHEELSFRONT (5)
#define VEHICLE_MODEL_INFO_WHEELSREAR (6)
#define VEHICLE_MODEL_INFO_WHEELSMID (7)
#define VEHICLE_MODEL_INFO_FRONT_BUMPER_Z (8)
#define VEHICLE_MODEL_INFO_REAR_BUMPER_Z (9)

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidVehicle">IsValidVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidVehicle(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleDistanceFromPoint">GetVehicleDistanceFromPoint on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetVehicleDistanceFromPoint(int vehicleid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CreateVehicle">CreateVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(int, CreateVehicle(int vehicletype, float x, float y, float z, float rotation, int color1, int color2, int respawn_delay, bool addsiren));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DestroyVehicle">DestroyVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DestroyVehicle(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsVehicleStreamedIn">IsVehicleStreamedIn on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsVehicleStreamedIn(int vehicleid, int forplayerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehiclePos">GetVehiclePos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehiclePos(int vehicleid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehiclePos">SetVehiclePos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehiclePos(int vehicleid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleZAngle">GetVehicleZAngle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleZAngle(int vehicleid, float * z_angle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleRotationQuat">GetVehicleRotationQuat on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleRotationQuat(int vehicleid, float * w, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleZAngle">SetVehicleZAngle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleZAngle(int vehicleid, float z_angle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleParamsForPlayer">SetVehicleParamsForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleParamsForPlayer(int vehicleid, int playerid, int objective, int doorslocked));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ManualVehicleEngineAndLights">ManualVehicleEngineAndLights on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ManualVehicleEngineAndLights());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleParamsEx">SetVehicleParamsEx on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleParamsEx(int vehicleid, int engine, int lights, int alarm, int doors, int bonnet, int boot, int objective));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleParamsEx">GetVehicleParamsEx on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleParamsEx(int vehicleid, int * engine, int * lights, int * alarm, int * doors, int * bonnet, int * boot, int * objective));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleParamsSirenState">GetVehicleParamsSirenState on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleParamsSirenState(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleParamsCarDoors">SetVehicleParamsCarDoors on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleParamsCarDoors(int vehicleid, int driver, int passenger, int backleft, int backright));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleParamsCarDoors">GetVehicleParamsCarDoors on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleParamsCarDoors(int vehicleid, int * driver, int * passenger, int * backleft, int * backright));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleParamsCarWindows">SetVehicleParamsCarWindows on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleParamsCarWindows(int vehicleid, int driver, int passenger, int backleft, int backright));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleParamsCarWindows">GetVehicleParamsCarWindows on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleParamsCarWindows(int vehicleid, int * driver, int * passenger, int * backleft, int * backright));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleToRespawn">SetVehicleToRespawn on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleToRespawn(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/LinkVehicleToInterior">LinkVehicleToInterior on open.mp</a>
 */
SAMPGDK_NATIVE(bool, LinkVehicleToInterior(int vehicleid, int interiorid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AddVehicleComponent">AddVehicleComponent on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AddVehicleComponent(int vehicleid, int componentid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/RemoveVehicleComponent">RemoveVehicleComponent on open.mp</a>
 */
SAMPGDK_NATIVE(bool, RemoveVehicleComponent(int vehicleid, int componentid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ChangeVehicleColor">ChangeVehicleColor on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ChangeVehicleColor(int vehicleid, int color1, int color2));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ChangeVehiclePaintjob">ChangeVehiclePaintjob on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ChangeVehiclePaintjob(int vehicleid, int paintjobid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleHealth">SetVehicleHealth on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleHealth(int vehicleid, float health));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleHealth">GetVehicleHealth on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleHealth(int vehicleid, float * health));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AttachTrailerToVehicle">AttachTrailerToVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AttachTrailerToVehicle(int trailerid, int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DetachTrailerFromVehicle">DetachTrailerFromVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DetachTrailerFromVehicle(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsTrailerAttachedToVehicle">IsTrailerAttachedToVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsTrailerAttachedToVehicle(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleTrailer">GetVehicleTrailer on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleTrailer(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleNumberPlate">SetVehicleNumberPlate on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleNumberPlate(int vehicleid, const char * numberplate));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleModel">GetVehicleModel on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleModel(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleComponentInSlot">GetVehicleComponentInSlot on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleComponentInSlot(int vehicleid, int slot));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleComponentType">GetVehicleComponentType on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleComponentType(int component));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/RepairVehicle">RepairVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, RepairVehicle(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleVelocity">GetVehicleVelocity on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleVelocity(int vehicleid, float * X, float * Y, float * Z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleVelocity">SetVehicleVelocity on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleVelocity(int vehicleid, float X, float Y, float Z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleAngularVelocity">SetVehicleAngularVelocity on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleAngularVelocity(int vehicleid, float X, float Y, float Z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleDamageStatus">GetVehicleDamageStatus on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleDamageStatus(int vehicleid, int * panels, int * doors, int * lights, int * tires));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/UpdateVehicleDamageStatus">UpdateVehicleDamageStatus on open.mp</a>
 */
SAMPGDK_NATIVE(bool, UpdateVehicleDamageStatus(int vehicleid, int panels, int doors, int lights, int tires));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleVirtualWorld">SetVehicleVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleVirtualWorld(int vehicleid, int worldid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleVirtualWorld">GetVehicleVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleVirtualWorld(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleModelInfo">GetVehicleModelInfo on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleModelInfo(int model, int infotype, float * X, float * Y, float * Z));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool IsValidVehicle(int vehicleid) {
  return sampgdk_IsValidVehicle(vehicleid);
}

inline float GetVehicleDistanceFromPoint(int vehicleid, float x, float y, float z) {
  return sampgdk_GetVehicleDistanceFromPoint(vehicleid, x, y, z);
}

inline int CreateVehicle(int vehicletype, float x, float y, float z, float rotation, int color1, int color2, int respawn_delay, bool addsiren = false) {
  return sampgdk_CreateVehicle(vehicletype, x, y, z, rotation, color1, color2, respawn_delay, addsiren);
}

inline bool DestroyVehicle(int vehicleid) {
  return sampgdk_DestroyVehicle(vehicleid);
}

inline bool IsVehicleStreamedIn(int vehicleid, int forplayerid) {
  return sampgdk_IsVehicleStreamedIn(vehicleid, forplayerid);
}

inline bool GetVehiclePos(int vehicleid, float * x, float * y, float * z) {
  return sampgdk_GetVehiclePos(vehicleid, x, y, z);
}

inline bool SetVehiclePos(int vehicleid, float x, float y, float z) {
  return sampgdk_SetVehiclePos(vehicleid, x, y, z);
}

inline bool GetVehicleZAngle(int vehicleid, float * z_angle) {
  return sampgdk_GetVehicleZAngle(vehicleid, z_angle);
}

inline bool GetVehicleRotationQuat(int vehicleid, float * w, float * x, float * y, float * z) {
  return sampgdk_GetVehicleRotationQuat(vehicleid, w, x, y, z);
}

inline bool SetVehicleZAngle(int vehicleid, float z_angle) {
  return sampgdk_SetVehicleZAngle(vehicleid, z_angle);
}

inline bool SetVehicleParamsForPlayer(int vehicleid, int playerid, int objective, int doorslocked) {
  return sampgdk_SetVehicleParamsForPlayer(vehicleid, playerid, objective, doorslocked);
}

inline bool ManualVehicleEngineAndLights() {
  return sampgdk_ManualVehicleEngineAndLights();
}

inline bool SetVehicleParamsEx(int vehicleid, int engine, int lights, int alarm, int doors, int bonnet, int boot, int objective) {
  return sampgdk_SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
}

inline bool GetVehicleParamsEx(int vehicleid, int * engine, int * lights, int * alarm, int * doors, int * bonnet, int * boot, int * objective) {
  return sampgdk_GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
}

inline int GetVehicleParamsSirenState(int vehicleid) {
  return sampgdk_GetVehicleParamsSirenState(vehicleid);
}

inline bool SetVehicleParamsCarDoors(int vehicleid, int driver, int passenger, int backleft, int backright) {
  return sampgdk_SetVehicleParamsCarDoors(vehicleid, driver, passenger, backleft, backright);
}

inline bool GetVehicleParamsCarDoors(int vehicleid, int * driver, int * passenger, int * backleft, int * backright) {
  return sampgdk_GetVehicleParamsCarDoors(vehicleid, driver, passenger, backleft, backright);
}

inline bool SetVehicleParamsCarWindows(int vehicleid, int driver, int passenger, int backleft, int backright) {
  return sampgdk_SetVehicleParamsCarWindows(vehicleid, driver, passenger, backleft, backright);
}

inline bool GetVehicleParamsCarWindows(int vehicleid, int * driver, int * passenger, int * backleft, int * backright) {
  return sampgdk_GetVehicleParamsCarWindows(vehicleid, driver, passenger, backleft, backright);
}

inline bool SetVehicleToRespawn(int vehicleid) {
  return sampgdk_SetVehicleToRespawn(vehicleid);
}

inline bool LinkVehicleToInterior(int vehicleid, int interiorid) {
  return sampgdk_LinkVehicleToInterior(vehicleid, interiorid);
}

inline bool AddVehicleComponent(int vehicleid, int componentid) {
  return sampgdk_AddVehicleComponent(vehicleid, componentid);
}

inline bool RemoveVehicleComponent(int vehicleid, int componentid) {
  return sampgdk_RemoveVehicleComponent(vehicleid, componentid);
}

inline bool ChangeVehicleColor(int vehicleid, int color1, int color2) {
  return sampgdk_ChangeVehicleColor(vehicleid, color1, color2);
}

inline bool ChangeVehiclePaintjob(int vehicleid, int paintjobid) {
  return sampgdk_ChangeVehiclePaintjob(vehicleid, paintjobid);
}

inline bool SetVehicleHealth(int vehicleid, float health) {
  return sampgdk_SetVehicleHealth(vehicleid, health);
}

inline bool GetVehicleHealth(int vehicleid, float * health) {
  return sampgdk_GetVehicleHealth(vehicleid, health);
}

inline bool AttachTrailerToVehicle(int trailerid, int vehicleid) {
  return sampgdk_AttachTrailerToVehicle(trailerid, vehicleid);
}

inline bool DetachTrailerFromVehicle(int vehicleid) {
  return sampgdk_DetachTrailerFromVehicle(vehicleid);
}

inline bool IsTrailerAttachedToVehicle(int vehicleid) {
  return sampgdk_IsTrailerAttachedToVehicle(vehicleid);
}

inline int GetVehicleTrailer(int vehicleid) {
  return sampgdk_GetVehicleTrailer(vehicleid);
}

inline bool SetVehicleNumberPlate(int vehicleid, const char * numberplate) {
  return sampgdk_SetVehicleNumberPlate(vehicleid, numberplate);
}

inline int GetVehicleModel(int vehicleid) {
  return sampgdk_GetVehicleModel(vehicleid);
}

inline int GetVehicleComponentInSlot(int vehicleid, int slot) {
  return sampgdk_GetVehicleComponentInSlot(vehicleid, slot);
}

inline int GetVehicleComponentType(int component) {
  return sampgdk_GetVehicleComponentType(component);
}

inline bool RepairVehicle(int vehicleid) {
  return sampgdk_RepairVehicle(vehicleid);
}

inline bool GetVehicleVelocity(int vehicleid, float * X, float * Y, float * Z) {
  return sampgdk_GetVehicleVelocity(vehicleid, X, Y, Z);
}

inline bool SetVehicleVelocity(int vehicleid, float X, float Y, float Z) {
  return sampgdk_SetVehicleVelocity(vehicleid, X, Y, Z);
}

inline bool SetVehicleAngularVelocity(int vehicleid, float X, float Y, float Z) {
  return sampgdk_SetVehicleAngularVelocity(vehicleid, X, Y, Z);
}

inline bool GetVehicleDamageStatus(int vehicleid, int * panels, int * doors, int * lights, int * tires) {
  return sampgdk_GetVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
}

inline bool UpdateVehicleDamageStatus(int vehicleid, int panels, int doors, int lights, int tires) {
  return sampgdk_UpdateVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
}

inline bool SetVehicleVirtualWorld(int vehicleid, int worldid) {
  return sampgdk_SetVehicleVirtualWorld(vehicleid, worldid);
}

inline int GetVehicleVirtualWorld(int vehicleid) {
  return sampgdk_GetVehicleVirtualWorld(vehicleid);
}

inline bool GetVehicleModelInfo(int model, int infotype, float * X, float * Y, float * Z) {
  return sampgdk_GetVehicleModelInfo(model, infotype, X, Y, Z);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  IsValidVehicle
#define IsValidVehicle sampgdk_IsValidVehicle

#undef  GetVehicleDistanceFromPoint
#define GetVehicleDistanceFromPoint sampgdk_GetVehicleDistanceFromPoint

#undef  CreateVehicle
#define CreateVehicle sampgdk_CreateVehicle

#undef  DestroyVehicle
#define DestroyVehicle sampgdk_DestroyVehicle

#undef  IsVehicleStreamedIn
#define IsVehicleStreamedIn sampgdk_IsVehicleStreamedIn

#undef  GetVehiclePos
#define GetVehiclePos sampgdk_GetVehiclePos

#undef  SetVehiclePos
#define SetVehiclePos sampgdk_SetVehiclePos

#undef  GetVehicleZAngle
#define GetVehicleZAngle sampgdk_GetVehicleZAngle

#undef  GetVehicleRotationQuat
#define GetVehicleRotationQuat sampgdk_GetVehicleRotationQuat

#undef  SetVehicleZAngle
#define SetVehicleZAngle sampgdk_SetVehicleZAngle

#undef  SetVehicleParamsForPlayer
#define SetVehicleParamsForPlayer sampgdk_SetVehicleParamsForPlayer

#undef  ManualVehicleEngineAndLights
#define ManualVehicleEngineAndLights sampgdk_ManualVehicleEngineAndLights

#undef  SetVehicleParamsEx
#define SetVehicleParamsEx sampgdk_SetVehicleParamsEx

#undef  GetVehicleParamsEx
#define GetVehicleParamsEx sampgdk_GetVehicleParamsEx

#undef  GetVehicleParamsSirenState
#define GetVehicleParamsSirenState sampgdk_GetVehicleParamsSirenState

#undef  SetVehicleParamsCarDoors
#define SetVehicleParamsCarDoors sampgdk_SetVehicleParamsCarDoors

#undef  GetVehicleParamsCarDoors
#define GetVehicleParamsCarDoors sampgdk_GetVehicleParamsCarDoors

#undef  SetVehicleParamsCarWindows
#define SetVehicleParamsCarWindows sampgdk_SetVehicleParamsCarWindows

#undef  GetVehicleParamsCarWindows
#define GetVehicleParamsCarWindows sampgdk_GetVehicleParamsCarWindows

#undef  SetVehicleToRespawn
#define SetVehicleToRespawn sampgdk_SetVehicleToRespawn

#undef  LinkVehicleToInterior
#define LinkVehicleToInterior sampgdk_LinkVehicleToInterior

#undef  AddVehicleComponent
#define AddVehicleComponent sampgdk_AddVehicleComponent

#undef  RemoveVehicleComponent
#define RemoveVehicleComponent sampgdk_RemoveVehicleComponent

#undef  ChangeVehicleColor
#define ChangeVehicleColor sampgdk_ChangeVehicleColor

#undef  ChangeVehiclePaintjob
#define ChangeVehiclePaintjob sampgdk_ChangeVehiclePaintjob

#undef  SetVehicleHealth
#define SetVehicleHealth sampgdk_SetVehicleHealth

#undef  GetVehicleHealth
#define GetVehicleHealth sampgdk_GetVehicleHealth

#undef  AttachTrailerToVehicle
#define AttachTrailerToVehicle sampgdk_AttachTrailerToVehicle

#undef  DetachTrailerFromVehicle
#define DetachTrailerFromVehicle sampgdk_DetachTrailerFromVehicle

#undef  IsTrailerAttachedToVehicle
#define IsTrailerAttachedToVehicle sampgdk_IsTrailerAttachedToVehicle

#undef  GetVehicleTrailer
#define GetVehicleTrailer sampgdk_GetVehicleTrailer

#undef  SetVehicleNumberPlate
#define SetVehicleNumberPlate sampgdk_SetVehicleNumberPlate

#undef  GetVehicleModel
#define GetVehicleModel sampgdk_GetVehicleModel

#undef  GetVehicleComponentInSlot
#define GetVehicleComponentInSlot sampgdk_GetVehicleComponentInSlot

#undef  GetVehicleComponentType
#define GetVehicleComponentType sampgdk_GetVehicleComponentType

#undef  RepairVehicle
#define RepairVehicle sampgdk_RepairVehicle

#undef  GetVehicleVelocity
#define GetVehicleVelocity sampgdk_GetVehicleVelocity

#undef  SetVehicleVelocity
#define SetVehicleVelocity sampgdk_SetVehicleVelocity

#undef  SetVehicleAngularVelocity
#define SetVehicleAngularVelocity sampgdk_SetVehicleAngularVelocity

#undef  GetVehicleDamageStatus
#define GetVehicleDamageStatus sampgdk_GetVehicleDamageStatus

#undef  UpdateVehicleDamageStatus
#define UpdateVehicleDamageStatus sampgdk_UpdateVehicleDamageStatus

#undef  SetVehicleVirtualWorld
#define SetVehicleVirtualWorld sampgdk_SetVehicleVirtualWorld

#undef  GetVehicleVirtualWorld
#define GetVehicleVirtualWorld sampgdk_GetVehicleVirtualWorld

#undef  GetVehicleModelInfo
#define GetVehicleModelInfo sampgdk_GetVehicleModelInfo

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_A_VEHICLES_H */

#ifndef SAMPGDK_OMP_ACTOR_H
#define SAMPGDK_OMP_ACTOR_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetActorAnimation">GetActorAnimation on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetActorAnimation(int actorid, char * animationLibrary, int librarySize, char * animationName, int nameSize, float * delta, bool * loop, bool * lockX, bool * lockY, bool * freeze, int * time));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetActorSkin">GetActorSkin on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetActorSkin(int actorid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetActorSpawnInfo">GetActorSpawnInfo on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetActorSpawnInfo(int actorid, int * skin, float * spawnX, float * spawnY, float * spawnZ, float * spawnAngle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetActors">GetActors on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetActors(char * actors, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetActorSkin">SetActorSkin on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetActorSkin(int actorid, int skin));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool GetActorAnimation(int actorid, char * animationLibrary, int librarySize, char * animationName, int nameSize, float * delta, bool * loop, bool * lockX, bool * lockY, bool * freeze, int * time) {
  return sampgdk_GetActorAnimation(actorid, animationLibrary, librarySize, animationName, nameSize, delta, loop, lockX, lockY, freeze, time);
}

inline int GetActorSkin(int actorid) {
  return sampgdk_GetActorSkin(actorid);
}

inline bool GetActorSpawnInfo(int actorid, int * skin, float * spawnX, float * spawnY, float * spawnZ, float * spawnAngle) {
  return sampgdk_GetActorSpawnInfo(actorid, skin, spawnX, spawnY, spawnZ, spawnAngle);
}

inline int GetActors(char * actors, int size) {
  return sampgdk_GetActors(actors, size);
}

inline bool SetActorSkin(int actorid, int skin) {
  return sampgdk_SetActorSkin(actorid, skin);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  GetActorAnimation
#define GetActorAnimation sampgdk_GetActorAnimation

#undef  GetActorSkin
#define GetActorSkin sampgdk_GetActorSkin

#undef  GetActorSpawnInfo
#define GetActorSpawnInfo sampgdk_GetActorSpawnInfo

#undef  GetActors
#define GetActors sampgdk_GetActors

#undef  SetActorSkin
#define SetActorSkin sampgdk_SetActorSkin

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_ACTOR_H */

#ifndef SAMPGDK_OMP_CHECKPOINT_H
#define SAMPGDK_OMP_CHECKPOINT_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCheckpoint">GetPlayerCheckpoint on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerCheckpoint(int playerid, float * centreX, float * centreY, float * centreZ, float * radius));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerRaceCheckpoint">GetPlayerRaceCheckpoint on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerRaceCheckpoint(int playerid, float * centreX, float * centreY, float * centreZ, float * nextX, float * nextY, float * nextZ, float * radius));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerCheckpointActive">IsPlayerCheckpointActive on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerCheckpointActive(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerRaceCheckpointActive">IsPlayerRaceCheckpointActive on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerRaceCheckpointActive(int playerid));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool GetPlayerCheckpoint(int playerid, float * centreX, float * centreY, float * centreZ, float * radius) {
  return sampgdk_GetPlayerCheckpoint(playerid, centreX, centreY, centreZ, radius);
}

inline bool GetPlayerRaceCheckpoint(int playerid, float * centreX, float * centreY, float * centreZ, float * nextX, float * nextY, float * nextZ, float * radius) {
  return sampgdk_GetPlayerRaceCheckpoint(playerid, centreX, centreY, centreZ, nextX, nextY, nextZ, radius);
}

inline bool IsPlayerCheckpointActive(int playerid) {
  return sampgdk_IsPlayerCheckpointActive(playerid);
}

inline bool IsPlayerRaceCheckpointActive(int playerid) {
  return sampgdk_IsPlayerRaceCheckpointActive(playerid);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  GetPlayerCheckpoint
#define GetPlayerCheckpoint sampgdk_GetPlayerCheckpoint

#undef  GetPlayerRaceCheckpoint
#define GetPlayerRaceCheckpoint sampgdk_GetPlayerRaceCheckpoint

#undef  IsPlayerCheckpointActive
#define IsPlayerCheckpointActive sampgdk_IsPlayerCheckpointActive

#undef  IsPlayerRaceCheckpointActive
#define IsPlayerRaceCheckpointActive sampgdk_IsPlayerRaceCheckpointActive

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_CHECKPOINT_H */

#ifndef SAMPGDK_OMP_CLASS_H
#define SAMPGDK_OMP_CLASS_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/EditPlayerClass">EditPlayerClass on open.mp</a>
 */
SAMPGDK_NATIVE(bool, EditPlayerClass(int classid, int team, int skin, float spawnX, float spawnY, float spawnZ, float angle, int weapon1, int ammo1, int weapon2, int ammo2, int weapon3, int ammo3));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetAvailableClasses">GetAvailableClasses on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetAvailableClasses());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerClass">GetPlayerClass on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerClass(int classid, int * team, int * skin, float * spawnX, float * spawnY, float * spawnZ, float * angle, int * weapon1, int * ammo1, int * weapon2, int * ammo2, int * weapon3, int * ammo3));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetSpawnInfo">GetSpawnInfo on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetSpawnInfo(int playerid, int * team, int * skin, float * spawnX, float * spawnY, float * spawnZ, float * angle, int * weapon1, int * ammo1, int * weapon2, int * ammo2, int * weapon3, int * ammo3));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool EditPlayerClass(int classid, int team, int skin, float spawnX, float spawnY, float spawnZ, float angle, int weapon1, int ammo1, int weapon2, int ammo2, int weapon3, int ammo3) {
  return sampgdk_EditPlayerClass(classid, team, skin, spawnX, spawnY, spawnZ, angle, weapon1, ammo1, weapon2, ammo2, weapon3, ammo3);
}

inline int GetAvailableClasses() {
  return sampgdk_GetAvailableClasses();
}

inline bool GetPlayerClass(int classid, int * team, int * skin, float * spawnX, float * spawnY, float * spawnZ, float * angle, int * weapon1, int * ammo1, int * weapon2, int * ammo2, int * weapon3, int * ammo3) {
  return sampgdk_GetPlayerClass(classid, team, skin, spawnX, spawnY, spawnZ, angle, weapon1, ammo1, weapon2, ammo2, weapon3, ammo3);
}

inline bool GetSpawnInfo(int playerid, int * team, int * skin, float * spawnX, float * spawnY, float * spawnZ, float * angle, int * weapon1, int * ammo1, int * weapon2, int * ammo2, int * weapon3, int * ammo3) {
  return sampgdk_GetSpawnInfo(playerid, team, skin, spawnX, spawnY, spawnZ, angle, weapon1, ammo1, weapon2, ammo2, weapon3, ammo3);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  EditPlayerClass
#define EditPlayerClass sampgdk_EditPlayerClass

#undef  GetAvailableClasses
#define GetAvailableClasses sampgdk_GetAvailableClasses

#undef  GetPlayerClass
#define GetPlayerClass sampgdk_GetPlayerClass

#undef  GetSpawnInfo
#define GetSpawnInfo sampgdk_GetSpawnInfo

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_CLASS_H */

#ifndef SAMPGDK_OMP_CORE_H
#define SAMPGDK_OMP_CORE_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AddServerRule">AddServerRule on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AddServerRule(const char * rule, const char * format));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AllowNickNameCharacter">AllowNickNameCharacter on open.mp</a>
 */
SAMPGDK_NATIVE(int, AllowNickNameCharacter(int character, bool allow));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AreAllAnimationsEnabled">AreAllAnimationsEnabled on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AreAllAnimationsEnabled());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AreInteriorWeaponsAllowed">AreInteriorWeaponsAllowed on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AreInteriorWeaponsAllowed());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CallLocalFunction">CallLocalFunction on open.mp</a>
 */
SAMPGDK_NATIVE(int, CallLocalFunction(const char * functionName, const char * specifiers));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CallRemoteFunction">CallRemoteFunction on open.mp</a>
 */
SAMPGDK_NATIVE(int, CallRemoteFunction(const char * functionName, const char * specifiers));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ChatTextReplacementToggled">ChatTextReplacementToggled on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ChatTextReplacementToggled());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CountRunningTimers">CountRunningTimers on open.mp</a>
 */
SAMPGDK_NATIVE(int, CountRunningTimers());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/EnableAllAnimations">EnableAllAnimations on open.mp</a>
 */
SAMPGDK_NATIVE(int, EnableAllAnimations(bool enable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetConsoleVarAsFloat">GetConsoleVarAsFloat on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetConsoleVarAsFloat(const char * cvar));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetModeRestartTime">GetModeRestartTime on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetModeRestartTime());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetTimerInterval">GetTimerInterval on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetTimerInterval(int timerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetTimerRemaining">GetTimerRemaining on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetTimerRemaining(int timerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetWeaponSlot">GetWeaponSlot on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetWeaponSlot(int weaponid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetWeather">GetWeather on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetWeather());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetWorldTime">GetWorldTime on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetWorldTime());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/HideGameTextForAll">HideGameTextForAll on open.mp</a>
 */
SAMPGDK_NATIVE(bool, HideGameTextForAll(int style));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsAdminTeleportAllowed">IsAdminTeleportAllowed on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsAdminTeleportAllowed());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsNickNameCharacterAllowed">IsNickNameCharacterAllowed on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsNickNameCharacterAllowed(int character));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsRepeatingTimer">IsRepeatingTimer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsRepeatingTimer(int timerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidNickName">IsValidNickName on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidNickName(const char * name));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidServerRule">IsValidServerRule on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidServerRule(const char * rule));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidTimer">IsValidTimer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidTimer(int timerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/RemoveServerRule">RemoveServerRule on open.mp</a>
 */
SAMPGDK_NATIVE(bool, RemoveServerRule(const char * rule));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetModeRestartTime">SetModeRestartTime on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetModeRestartTime(float seconds));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetServerRule">SetServerRule on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetServerRule(const char * rule, const char * format));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetTimerEx">SetTimerEx on open.mp</a>
 */
SAMPGDK_NATIVE(int, SetTimerEx(const char * functionName, int interval, bool repeating, const char * specifiers));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ToggleChatTextReplacement">ToggleChatTextReplacement on open.mp</a>
 */
SAMPGDK_NATIVE(int, ToggleChatTextReplacement(bool enable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/__open_mp_unused_print">__open_mp_unused_print on open.mp</a>
 */
SAMPGDK_NATIVE(int, __open_mp_unused_print(const char * string_));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool AddServerRule(const char * rule, const char * format) {
  return sampgdk_AddServerRule(rule, format);
}

inline int AllowNickNameCharacter(int character, bool allow) {
  return sampgdk_AllowNickNameCharacter(character, allow);
}

inline bool AreAllAnimationsEnabled() {
  return sampgdk_AreAllAnimationsEnabled();
}

inline bool AreInteriorWeaponsAllowed() {
  return sampgdk_AreInteriorWeaponsAllowed();
}

inline int CallLocalFunction(const char * functionName, const char * specifiers) {
  return sampgdk_CallLocalFunction(functionName, specifiers);
}

inline int CallRemoteFunction(const char * functionName, const char * specifiers) {
  return sampgdk_CallRemoteFunction(functionName, specifiers);
}

inline bool ChatTextReplacementToggled() {
  return sampgdk_ChatTextReplacementToggled();
}

inline int CountRunningTimers() {
  return sampgdk_CountRunningTimers();
}

inline int EnableAllAnimations(bool enable) {
  return sampgdk_EnableAllAnimations(enable);
}

inline float GetConsoleVarAsFloat(const char * cvar) {
  return sampgdk_GetConsoleVarAsFloat(cvar);
}

inline float GetModeRestartTime() {
  return sampgdk_GetModeRestartTime();
}

inline int GetTimerInterval(int timerid) {
  return sampgdk_GetTimerInterval(timerid);
}

inline int GetTimerRemaining(int timerid) {
  return sampgdk_GetTimerRemaining(timerid);
}

inline int GetWeaponSlot(int weaponid) {
  return sampgdk_GetWeaponSlot(weaponid);
}

inline int GetWeather() {
  return sampgdk_GetWeather();
}

inline int GetWorldTime() {
  return sampgdk_GetWorldTime();
}

inline bool HideGameTextForAll(int style) {
  return sampgdk_HideGameTextForAll(style);
}

inline bool IsAdminTeleportAllowed() {
  return sampgdk_IsAdminTeleportAllowed();
}

inline bool IsNickNameCharacterAllowed(int character) {
  return sampgdk_IsNickNameCharacterAllowed(character);
}

inline bool IsRepeatingTimer(int timerid) {
  return sampgdk_IsRepeatingTimer(timerid);
}

inline bool IsValidNickName(const char * name) {
  return sampgdk_IsValidNickName(name);
}

inline bool IsValidServerRule(const char * rule) {
  return sampgdk_IsValidServerRule(rule);
}

inline bool IsValidTimer(int timerid) {
  return sampgdk_IsValidTimer(timerid);
}

inline bool RemoveServerRule(const char * rule) {
  return sampgdk_RemoveServerRule(rule);
}

inline bool SetModeRestartTime(float seconds) {
  return sampgdk_SetModeRestartTime(seconds);
}

inline bool SetServerRule(const char * rule, const char * format) {
  return sampgdk_SetServerRule(rule, format);
}

inline int SetTimerEx(const char * functionName, int interval, bool repeating, const char * specifiers) {
  return sampgdk_SetTimerEx(functionName, interval, repeating, specifiers);
}

inline int ToggleChatTextReplacement(bool enable) {
  return sampgdk_ToggleChatTextReplacement(enable);
}

inline int __open_mp_unused_print(const char * string_) {
  return sampgdk___open_mp_unused_print(string_);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  AddServerRule
#define AddServerRule sampgdk_AddServerRule

#undef  AllowNickNameCharacter
#define AllowNickNameCharacter sampgdk_AllowNickNameCharacter

#undef  AreAllAnimationsEnabled
#define AreAllAnimationsEnabled sampgdk_AreAllAnimationsEnabled

#undef  AreInteriorWeaponsAllowed
#define AreInteriorWeaponsAllowed sampgdk_AreInteriorWeaponsAllowed

#undef  CallLocalFunction
#define CallLocalFunction sampgdk_CallLocalFunction

#undef  CallRemoteFunction
#define CallRemoteFunction sampgdk_CallRemoteFunction

#undef  ChatTextReplacementToggled
#define ChatTextReplacementToggled sampgdk_ChatTextReplacementToggled

#undef  CountRunningTimers
#define CountRunningTimers sampgdk_CountRunningTimers

#undef  EnableAllAnimations
#define EnableAllAnimations sampgdk_EnableAllAnimations

#undef  GetConsoleVarAsFloat
#define GetConsoleVarAsFloat sampgdk_GetConsoleVarAsFloat

#undef  GetModeRestartTime
#define GetModeRestartTime sampgdk_GetModeRestartTime

#undef  GetTimerInterval
#define GetTimerInterval sampgdk_GetTimerInterval

#undef  GetTimerRemaining
#define GetTimerRemaining sampgdk_GetTimerRemaining

#undef  GetWeaponSlot
#define GetWeaponSlot sampgdk_GetWeaponSlot

#undef  GetWeather
#define GetWeather sampgdk_GetWeather

#undef  GetWorldTime
#define GetWorldTime sampgdk_GetWorldTime

#undef  HideGameTextForAll
#define HideGameTextForAll sampgdk_HideGameTextForAll

#undef  IsAdminTeleportAllowed
#define IsAdminTeleportAllowed sampgdk_IsAdminTeleportAllowed

#undef  IsNickNameCharacterAllowed
#define IsNickNameCharacterAllowed sampgdk_IsNickNameCharacterAllowed

#undef  IsRepeatingTimer
#define IsRepeatingTimer sampgdk_IsRepeatingTimer

#undef  IsValidNickName
#define IsValidNickName sampgdk_IsValidNickName

#undef  IsValidServerRule
#define IsValidServerRule sampgdk_IsValidServerRule

#undef  IsValidTimer
#define IsValidTimer sampgdk_IsValidTimer

#undef  RemoveServerRule
#define RemoveServerRule sampgdk_RemoveServerRule

#undef  SetModeRestartTime
#define SetModeRestartTime sampgdk_SetModeRestartTime

#undef  SetServerRule
#define SetServerRule sampgdk_SetServerRule

#undef  SetTimerEx
#define SetTimerEx sampgdk_SetTimerEx

#undef  ToggleChatTextReplacement
#define ToggleChatTextReplacement sampgdk_ToggleChatTextReplacement

#undef  __open_mp_unused_print
#define __open_mp_unused_print sampgdk___open_mp_unused_print

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_CORE_H */

#ifndef SAMPGDK_OMP_DATABASE_H
#define SAMPGDK_OMP_DATABASE_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_Close_Modern">DB_Close on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DB_Close(int db));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_ExecuteQuery">DB_ExecuteQuery on open.mp</a>
 */
SAMPGDK_NATIVE(int, DB_ExecuteQuery(int db, const char * query));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_FreeResultSet">DB_FreeResultSet on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DB_FreeResultSet(int result));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetDatabaseConnectionCount">DB_GetDatabaseConnectionCount on open.mp</a>
 */
SAMPGDK_NATIVE(int, DB_GetDatabaseConnectionCount());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetDatabaseResultSetCount">DB_GetDatabaseResultSetCount on open.mp</a>
 */
SAMPGDK_NATIVE(int, DB_GetDatabaseResultSetCount());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetFieldCount">DB_GetFieldCount on open.mp</a>
 */
SAMPGDK_NATIVE(int, DB_GetFieldCount(int result));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetFieldFloat">DB_GetFieldFloat on open.mp</a>
 */
SAMPGDK_NATIVE(float, DB_GetFieldFloat(int result, int field));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetFieldFloatByName">DB_GetFieldFloatByName on open.mp</a>
 */
SAMPGDK_NATIVE(float, DB_GetFieldFloatByName(int result, const char * field));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetFieldInt">DB_GetFieldInt on open.mp</a>
 */
SAMPGDK_NATIVE(int, DB_GetFieldInt(int result, int field));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetFieldIntByName">DB_GetFieldIntByName on open.mp</a>
 */
SAMPGDK_NATIVE(int, DB_GetFieldIntByName(int result, const char * field));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetFieldName">DB_GetFieldName on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DB_GetFieldName(int result, int field, char * output, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetFieldString">DB_GetFieldString on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DB_GetFieldString(int result, int field, char * output, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetFieldStringByName">DB_GetFieldStringByName on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DB_GetFieldStringByName(int result, const char * field, char * output, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetLegacyDBResult">DB_GetLegacyDBResult on open.mp</a>
 */
SAMPGDK_NATIVE(int, DB_GetLegacyDBResult(int result));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetMemHandle">DB_GetMemHandle on open.mp</a>
 */
SAMPGDK_NATIVE(int, DB_GetMemHandle(int db));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_GetRowCount">DB_GetRowCount on open.mp</a>
 */
SAMPGDK_NATIVE(int, DB_GetRowCount(int result));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_Open_Modern">DB_Open on open.mp</a>
 */
SAMPGDK_NATIVE(int, DB_Open(const char * name, int flags));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DB_SelectNextRow">DB_SelectNextRow on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DB_SelectNextRow(int result));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool DB_Close(int db) {
  return sampgdk_DB_Close(db);
}

inline int DB_ExecuteQuery(int db, const char * query) {
  return sampgdk_DB_ExecuteQuery(db, query);
}

inline bool DB_FreeResultSet(int result) {
  return sampgdk_DB_FreeResultSet(result);
}

inline int DB_GetDatabaseConnectionCount() {
  return sampgdk_DB_GetDatabaseConnectionCount();
}

inline int DB_GetDatabaseResultSetCount() {
  return sampgdk_DB_GetDatabaseResultSetCount();
}

inline int DB_GetFieldCount(int result) {
  return sampgdk_DB_GetFieldCount(result);
}

inline float DB_GetFieldFloat(int result, int field = 0) {
  return sampgdk_DB_GetFieldFloat(result, field);
}

inline float DB_GetFieldFloatByName(int result, const char * field) {
  return sampgdk_DB_GetFieldFloatByName(result, field);
}

inline int DB_GetFieldInt(int result, int field = 0) {
  return sampgdk_DB_GetFieldInt(result, field);
}

inline int DB_GetFieldIntByName(int result, const char * field) {
  return sampgdk_DB_GetFieldIntByName(result, field);
}

inline bool DB_GetFieldName(int result, int field, char * output, int size) {
  return sampgdk_DB_GetFieldName(result, field, output, size);
}

inline bool DB_GetFieldString(int result, int field, char * output, int size) {
  return sampgdk_DB_GetFieldString(result, field, output, size);
}

inline bool DB_GetFieldStringByName(int result, const char * field, char * output, int size) {
  return sampgdk_DB_GetFieldStringByName(result, field, output, size);
}

inline int DB_GetLegacyDBResult(int result) {
  return sampgdk_DB_GetLegacyDBResult(result);
}

inline int DB_GetMemHandle(int db) {
  return sampgdk_DB_GetMemHandle(db);
}

inline int DB_GetRowCount(int result) {
  return sampgdk_DB_GetRowCount(result);
}

inline int DB_Open(const char * name, int flags) {
  return sampgdk_DB_Open(name, flags);
}

inline bool DB_SelectNextRow(int result) {
  return sampgdk_DB_SelectNextRow(result);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  DB_Close
#define DB_Close sampgdk_DB_Close

#undef  DB_ExecuteQuery
#define DB_ExecuteQuery sampgdk_DB_ExecuteQuery

#undef  DB_FreeResultSet
#define DB_FreeResultSet sampgdk_DB_FreeResultSet

#undef  DB_GetDatabaseConnectionCount
#define DB_GetDatabaseConnectionCount sampgdk_DB_GetDatabaseConnectionCount

#undef  DB_GetDatabaseResultSetCount
#define DB_GetDatabaseResultSetCount sampgdk_DB_GetDatabaseResultSetCount

#undef  DB_GetFieldCount
#define DB_GetFieldCount sampgdk_DB_GetFieldCount

#undef  DB_GetFieldFloat
#define DB_GetFieldFloat sampgdk_DB_GetFieldFloat

#undef  DB_GetFieldFloatByName
#define DB_GetFieldFloatByName sampgdk_DB_GetFieldFloatByName

#undef  DB_GetFieldInt
#define DB_GetFieldInt sampgdk_DB_GetFieldInt

#undef  DB_GetFieldIntByName
#define DB_GetFieldIntByName sampgdk_DB_GetFieldIntByName

#undef  DB_GetFieldName
#define DB_GetFieldName sampgdk_DB_GetFieldName

#undef  DB_GetFieldString
#define DB_GetFieldString sampgdk_DB_GetFieldString

#undef  DB_GetFieldStringByName
#define DB_GetFieldStringByName sampgdk_DB_GetFieldStringByName

#undef  DB_GetLegacyDBResult
#define DB_GetLegacyDBResult sampgdk_DB_GetLegacyDBResult

#undef  DB_GetMemHandle
#define DB_GetMemHandle sampgdk_DB_GetMemHandle

#undef  DB_GetRowCount
#define DB_GetRowCount sampgdk_DB_GetRowCount

#undef  DB_Open
#define DB_Open sampgdk_DB_Open

#undef  DB_SelectNextRow
#define DB_SelectNextRow sampgdk_DB_SelectNextRow

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_DATABASE_H */

#ifndef SAMPGDK_OMP_DIALOG_H
#define SAMPGDK_OMP_DIALOG_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerDialogData">GetPlayerDialogData on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerDialogData(int playerid, int * style, char * title, int titleSize, char * body, int bodySize, char * button1, int button1Size, char * button2, int button2Size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerDialogID">GetPlayerDialogID on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerDialogID(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/HidePlayerDialog">HidePlayerDialog on open.mp</a>
 */
SAMPGDK_NATIVE(bool, HidePlayerDialog(int playerid));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool GetPlayerDialogData(int playerid, int * style, char * title, int titleSize, char * body, int bodySize, char * button1, int button1Size, char * button2, int button2Size) {
  return sampgdk_GetPlayerDialogData(playerid, style, title, titleSize, body, bodySize, button1, button1Size, button2, button2Size);
}

inline int GetPlayerDialogID(int playerid) {
  return sampgdk_GetPlayerDialogID(playerid);
}

inline bool HidePlayerDialog(int playerid) {
  return sampgdk_HidePlayerDialog(playerid);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  GetPlayerDialogData
#define GetPlayerDialogData sampgdk_GetPlayerDialogData

#undef  GetPlayerDialogID
#define GetPlayerDialogID sampgdk_GetPlayerDialogID

#undef  HidePlayerDialog
#define HidePlayerDialog sampgdk_HidePlayerDialog

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_DIALOG_H */

#ifndef SAMPGDK_OMP_GANGZONE_H
#define SAMPGDK_OMP_GANGZONE_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CreatePlayerGangZone">CreatePlayerGangZone on open.mp</a>
 */
SAMPGDK_NATIVE(int, CreatePlayerGangZone(int playerid, float minx, float miny, float maxx, float maxy));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneGetColourForPlayer">GangZoneGetColourForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(int, GangZoneGetColourForPlayer(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneGetFlashColourForPlayer">GangZoneGetFlashColourForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(int, GangZoneGetFlashColourForPlayer(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GangZoneGetPos">GangZoneGetPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GangZoneGetPos(int zoneid, float * minX, float * minY, float * maxX, float * maxY));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsGangZoneFlashingForPlayer">IsGangZoneFlashingForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsGangZoneFlashingForPlayer(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsGangZoneVisibleForPlayer">IsGangZoneVisibleForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsGangZoneVisibleForPlayer(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerGangZoneFlashing">IsPlayerGangZoneFlashing on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerGangZoneFlashing(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerGangZoneVisible">IsPlayerGangZoneVisible on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerGangZoneVisible(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerInGangZone">IsPlayerInGangZone on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerInGangZone(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerInPlayerGangZone">IsPlayerInPlayerGangZone on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerInPlayerGangZone(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidGangZone">IsValidGangZone on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidGangZone(int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidPlayerGangZone">IsValidPlayerGangZone on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidPlayerGangZone(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerGangZoneDestroy">PlayerGangZoneDestroy on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerGangZoneDestroy(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerGangZoneFlash">PlayerGangZoneFlash on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerGangZoneFlash(int playerid, int zoneid, int flashColour));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerGangZoneGetColour">PlayerGangZoneGetColour on open.mp</a>
 */
SAMPGDK_NATIVE(int, PlayerGangZoneGetColour(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerGangZoneGetFlashColour">PlayerGangZoneGetFlashColour on open.mp</a>
 */
SAMPGDK_NATIVE(int, PlayerGangZoneGetFlashColour(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerGangZoneGetPos">PlayerGangZoneGetPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerGangZoneGetPos(int playerid, int zoneid, float * minX, float * minY, float * maxX, float * maxY));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerGangZoneHide">PlayerGangZoneHide on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerGangZoneHide(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerGangZoneShow">PlayerGangZoneShow on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerGangZoneShow(int playerid, int zoneid, int colour));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerGangZoneStopFlash">PlayerGangZoneStopFlash on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerGangZoneStopFlash(int playerid, int zoneid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/UseGangZoneCheck">UseGangZoneCheck on open.mp</a>
 */
SAMPGDK_NATIVE(bool, UseGangZoneCheck(int zoneid, bool enable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/UsePlayerGangZoneCheck">UsePlayerGangZoneCheck on open.mp</a>
 */
SAMPGDK_NATIVE(bool, UsePlayerGangZoneCheck(int playerid, int zoneid, bool enable));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline int CreatePlayerGangZone(int playerid, float minx, float miny, float maxx, float maxy) {
  return sampgdk_CreatePlayerGangZone(playerid, minx, miny, maxx, maxy);
}

inline int GangZoneGetColourForPlayer(int playerid, int zoneid) {
  return sampgdk_GangZoneGetColourForPlayer(playerid, zoneid);
}

inline int GangZoneGetFlashColourForPlayer(int playerid, int zoneid) {
  return sampgdk_GangZoneGetFlashColourForPlayer(playerid, zoneid);
}

inline bool GangZoneGetPos(int zoneid, float * minX, float * minY, float * maxX, float * maxY) {
  return sampgdk_GangZoneGetPos(zoneid, minX, minY, maxX, maxY);
}

inline bool IsGangZoneFlashingForPlayer(int playerid, int zoneid) {
  return sampgdk_IsGangZoneFlashingForPlayer(playerid, zoneid);
}

inline bool IsGangZoneVisibleForPlayer(int playerid, int zoneid) {
  return sampgdk_IsGangZoneVisibleForPlayer(playerid, zoneid);
}

inline bool IsPlayerGangZoneFlashing(int playerid, int zoneid) {
  return sampgdk_IsPlayerGangZoneFlashing(playerid, zoneid);
}

inline bool IsPlayerGangZoneVisible(int playerid, int zoneid) {
  return sampgdk_IsPlayerGangZoneVisible(playerid, zoneid);
}

inline bool IsPlayerInGangZone(int playerid, int zoneid) {
  return sampgdk_IsPlayerInGangZone(playerid, zoneid);
}

inline bool IsPlayerInPlayerGangZone(int playerid, int zoneid) {
  return sampgdk_IsPlayerInPlayerGangZone(playerid, zoneid);
}

inline bool IsValidGangZone(int zoneid) {
  return sampgdk_IsValidGangZone(zoneid);
}

inline bool IsValidPlayerGangZone(int playerid, int zoneid) {
  return sampgdk_IsValidPlayerGangZone(playerid, zoneid);
}

inline bool PlayerGangZoneDestroy(int playerid, int zoneid) {
  return sampgdk_PlayerGangZoneDestroy(playerid, zoneid);
}

inline bool PlayerGangZoneFlash(int playerid, int zoneid, int flashColour) {
  return sampgdk_PlayerGangZoneFlash(playerid, zoneid, flashColour);
}

inline int PlayerGangZoneGetColour(int playerid, int zoneid) {
  return sampgdk_PlayerGangZoneGetColour(playerid, zoneid);
}

inline int PlayerGangZoneGetFlashColour(int playerid, int zoneid) {
  return sampgdk_PlayerGangZoneGetFlashColour(playerid, zoneid);
}

inline bool PlayerGangZoneGetPos(int playerid, int zoneid, float * minX, float * minY, float * maxX, float * maxY) {
  return sampgdk_PlayerGangZoneGetPos(playerid, zoneid, minX, minY, maxX, maxY);
}

inline bool PlayerGangZoneHide(int playerid, int zoneid) {
  return sampgdk_PlayerGangZoneHide(playerid, zoneid);
}

inline bool PlayerGangZoneShow(int playerid, int zoneid, int colour) {
  return sampgdk_PlayerGangZoneShow(playerid, zoneid, colour);
}

inline bool PlayerGangZoneStopFlash(int playerid, int zoneid) {
  return sampgdk_PlayerGangZoneStopFlash(playerid, zoneid);
}

inline bool UseGangZoneCheck(int zoneid, bool enable) {
  return sampgdk_UseGangZoneCheck(zoneid, enable);
}

inline bool UsePlayerGangZoneCheck(int playerid, int zoneid, bool enable) {
  return sampgdk_UsePlayerGangZoneCheck(playerid, zoneid, enable);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  CreatePlayerGangZone
#define CreatePlayerGangZone sampgdk_CreatePlayerGangZone

#undef  GangZoneGetColourForPlayer
#define GangZoneGetColourForPlayer sampgdk_GangZoneGetColourForPlayer

#undef  GangZoneGetFlashColourForPlayer
#define GangZoneGetFlashColourForPlayer sampgdk_GangZoneGetFlashColourForPlayer

#undef  GangZoneGetPos
#define GangZoneGetPos sampgdk_GangZoneGetPos

#undef  IsGangZoneFlashingForPlayer
#define IsGangZoneFlashingForPlayer sampgdk_IsGangZoneFlashingForPlayer

#undef  IsGangZoneVisibleForPlayer
#define IsGangZoneVisibleForPlayer sampgdk_IsGangZoneVisibleForPlayer

#undef  IsPlayerGangZoneFlashing
#define IsPlayerGangZoneFlashing sampgdk_IsPlayerGangZoneFlashing

#undef  IsPlayerGangZoneVisible
#define IsPlayerGangZoneVisible sampgdk_IsPlayerGangZoneVisible

#undef  IsPlayerInGangZone
#define IsPlayerInGangZone sampgdk_IsPlayerInGangZone

#undef  IsPlayerInPlayerGangZone
#define IsPlayerInPlayerGangZone sampgdk_IsPlayerInPlayerGangZone

#undef  IsValidGangZone
#define IsValidGangZone sampgdk_IsValidGangZone

#undef  IsValidPlayerGangZone
#define IsValidPlayerGangZone sampgdk_IsValidPlayerGangZone

#undef  PlayerGangZoneDestroy
#define PlayerGangZoneDestroy sampgdk_PlayerGangZoneDestroy

#undef  PlayerGangZoneFlash
#define PlayerGangZoneFlash sampgdk_PlayerGangZoneFlash

#undef  PlayerGangZoneGetColour
#define PlayerGangZoneGetColour sampgdk_PlayerGangZoneGetColour

#undef  PlayerGangZoneGetFlashColour
#define PlayerGangZoneGetFlashColour sampgdk_PlayerGangZoneGetFlashColour

#undef  PlayerGangZoneGetPos
#define PlayerGangZoneGetPos sampgdk_PlayerGangZoneGetPos

#undef  PlayerGangZoneHide
#define PlayerGangZoneHide sampgdk_PlayerGangZoneHide

#undef  PlayerGangZoneShow
#define PlayerGangZoneShow sampgdk_PlayerGangZoneShow

#undef  PlayerGangZoneStopFlash
#define PlayerGangZoneStopFlash sampgdk_PlayerGangZoneStopFlash

#undef  UseGangZoneCheck
#define UseGangZoneCheck sampgdk_UseGangZoneCheck

#undef  UsePlayerGangZoneCheck
#define UsePlayerGangZoneCheck sampgdk_UsePlayerGangZoneCheck

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_GANGZONE_H */

#ifndef SAMPGDK_OMP_HTTP_H
#define SAMPGDK_OMP_HTTP_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_HTTP_H */

#ifndef SAMPGDK_OMP_MENU_H
#define SAMPGDK_OMP_MENU_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetMenuColumnHeader">GetMenuColumnHeader on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetMenuColumnHeader(int menuid, int column, char * header, int len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetMenuColumnWidth">GetMenuColumnWidth on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetMenuColumnWidth(int menuid, float * column1Width, float * column2Width));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetMenuColumns">GetMenuColumns on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetMenuColumns(int menuid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetMenuItem">GetMenuItem on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetMenuItem(int menuid, int column, int row, char * cell_, int len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetMenuItems">GetMenuItems on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetMenuItems(int menuid, int column));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetMenuPos">GetMenuPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetMenuPos(int menuid, float * x, float * y));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsMenuDisabled">IsMenuDisabled on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsMenuDisabled(int menuid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsMenuRowDisabled">IsMenuRowDisabled on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsMenuRowDisabled(int menuid, int row));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool GetMenuColumnHeader(int menuid, int column, char * header, int len) {
  return sampgdk_GetMenuColumnHeader(menuid, column, header, len);
}

inline bool GetMenuColumnWidth(int menuid, float * column1Width, float * column2Width) {
  return sampgdk_GetMenuColumnWidth(menuid, column1Width, column2Width);
}

inline int GetMenuColumns(int menuid) {
  return sampgdk_GetMenuColumns(menuid);
}

inline bool GetMenuItem(int menuid, int column, int row, char * cell_, int len) {
  return sampgdk_GetMenuItem(menuid, column, row, cell_, len);
}

inline int GetMenuItems(int menuid, int column) {
  return sampgdk_GetMenuItems(menuid, column);
}

inline bool GetMenuPos(int menuid, float * x, float * y) {
  return sampgdk_GetMenuPos(menuid, x, y);
}

inline bool IsMenuDisabled(int menuid) {
  return sampgdk_IsMenuDisabled(menuid);
}

inline bool IsMenuRowDisabled(int menuid, int row) {
  return sampgdk_IsMenuRowDisabled(menuid, row);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  GetMenuColumnHeader
#define GetMenuColumnHeader sampgdk_GetMenuColumnHeader

#undef  GetMenuColumnWidth
#define GetMenuColumnWidth sampgdk_GetMenuColumnWidth

#undef  GetMenuColumns
#define GetMenuColumns sampgdk_GetMenuColumns

#undef  GetMenuItem
#define GetMenuItem sampgdk_GetMenuItem

#undef  GetMenuItems
#define GetMenuItems sampgdk_GetMenuItems

#undef  GetMenuPos
#define GetMenuPos sampgdk_GetMenuPos

#undef  IsMenuDisabled
#define IsMenuDisabled sampgdk_IsMenuDisabled

#undef  IsMenuRowDisabled
#define IsMenuRowDisabled sampgdk_IsMenuRowDisabled

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_MENU_H */

#ifndef SAMPGDK_OMP_NETWORK_H
#define SAMPGDK_OMP_NETWORK_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ClearBanList">ClearBanList on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ClearBanList());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerRawIp">GetPlayerRawIp on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerRawIp(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsBanned">IsBanned on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsBanned(const char * ipaddress));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SendClientCheck">SendClientCheck on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SendClientCheck(int playerid, int type, int memoryAddress, int memoryOffset, int byteCount));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerAdmin">SetPlayerAdmin on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerAdmin(int playerid, bool admin));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool ClearBanList() {
  return sampgdk_ClearBanList();
}

inline int GetPlayerRawIp(int playerid) {
  return sampgdk_GetPlayerRawIp(playerid);
}

inline bool IsBanned(const char * ipaddress) {
  return sampgdk_IsBanned(ipaddress);
}

inline bool SendClientCheck(int playerid, int type, int memoryAddress, int memoryOffset, int byteCount) {
  return sampgdk_SendClientCheck(playerid, type, memoryAddress, memoryOffset, byteCount);
}

inline bool SetPlayerAdmin(int playerid, bool admin) {
  return sampgdk_SetPlayerAdmin(playerid, admin);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  ClearBanList
#define ClearBanList sampgdk_ClearBanList

#undef  GetPlayerRawIp
#define GetPlayerRawIp sampgdk_GetPlayerRawIp

#undef  IsBanned
#define IsBanned sampgdk_IsBanned

#undef  SendClientCheck
#define SendClientCheck sampgdk_SendClientCheck

#undef  SetPlayerAdmin
#define SetPlayerAdmin sampgdk_SetPlayerAdmin

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_NETWORK_H */

#ifndef SAMPGDK_OMP_NPC_H
#define SAMPGDK_OMP_NPC_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_AddPointToPath">NPC_AddPointToPath on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_AddPointToPath(int pathid, float x, float y, float z, float stopRange));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_AimAt">NPC_AimAt on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_AimAt(int npcid, float pointX, float pointY, float pointZ, bool shoot, int shootDelay, bool updateAngle, float offsetFromX, float offsetFromY, float offsetFromZ, int checkInBetweenFlags));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_AimAtPlayer">NPC_AimAtPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_AimAtPlayer(int npcid, int playerid, bool shoot, int shootDelay, bool updateAngle, float offsetX, float offsetY, float offsetZ, float offsetFromX, float offsetFromY, float offsetFromZ, int checkInBetweenFlags));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_ApplyAnimation">NPC_ApplyAnimation on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_ApplyAnimation(int npcid, const char * animlib, const char * animname, float delta, bool loop, bool lockX, bool lockY, bool freeze, int time));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_ChangeNode">NPC_ChangeNode on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_ChangeNode(int npcid, int nodeid, int linkid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_ClearAnimations">NPC_ClearAnimations on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_ClearAnimations(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_ClearPath">NPC_ClearPath on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_ClearPath(int pathid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_CloseNode">NPC_CloseNode on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_CloseNode(int nodeid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_Create">NPC_Create on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_Create(const char * name));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_CreatePath">NPC_CreatePath on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_CreatePath());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_Destroy">NPC_Destroy on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_Destroy(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_DestroyAllPath">NPC_DestroyAllPath on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_DestroyAllPath());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_DestroyPath">NPC_DestroyPath on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_DestroyPath(int pathid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_EnableInfiniteAmmo">NPC_EnableInfiniteAmmo on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_EnableInfiniteAmmo(int npcid, bool enable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_EnableReloading">NPC_EnableReloading on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_EnableReloading(int npcid, bool enable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_EnterVehicle">NPC_EnterVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_EnterVehicle(int npcid, int vehicleid, int seatid, int moveType));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_ExitVehicle">NPC_ExitVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_ExitVehicle(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetAll">NPC_GetAll on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetAll(char * npcs, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetAmmo">NPC_GetAmmo on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetAmmo(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetAmmoInClip">NPC_GetAmmoInClip on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetAmmoInClip(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetAnimation">NPC_GetAnimation on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetAnimation(int npcid, int * animationId, float * delta, bool * loop, bool * lockX, bool * lockY, bool * freeze, int * time));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetArmour">NPC_GetArmour on open.mp</a>
 */
SAMPGDK_NATIVE(float, NPC_GetArmour(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetCurrentPathPointIndex">NPC_GetCurrentPathPointIndex on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetCurrentPathPointIndex(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetCustomSkin">NPC_GetCustomSkin on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetCustomSkin(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetEnteringVehicle">NPC_GetEnteringVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetEnteringVehicle(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetEnteringVehicleID">NPC_GetEnteringVehicleID on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetEnteringVehicleID(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetEnteringVehicleSeat">NPC_GetEnteringVehicleSeat on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetEnteringVehicleSeat(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetFacingAngle">NPC_GetFacingAngle on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetFacingAngle(int npcid, float * angle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetFightingStyle">NPC_GetFightingStyle on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetFightingStyle(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetHealth">NPC_GetHealth on open.mp</a>
 */
SAMPGDK_NATIVE(float, NPC_GetHealth(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetInterior">NPC_GetInterior on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetInterior(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetKeys">NPC_GetKeys on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetKeys(int npcid, int * upAnddown, int * leftandRight, int * keys));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetNodeInfo">NPC_GetNodeInfo on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_GetNodeInfo(int nodeid, int * vehnodes, int * pednodes, int * navinode));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetNodePointCount">NPC_GetNodePointCount on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetNodePointCount(int nodeid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetNodePointPosition">NPC_GetNodePointPosition on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_GetNodePointPosition(int nodeid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetNodeType">NPC_GetNodeType on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetNodeType(int nodeid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetPathCount">NPC_GetPathCount on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetPathCount());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetPathPoint">NPC_GetPathPoint on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_GetPathPoint(int pathid, int pointIndex, float * x, float * y, float * z, float * stopRange));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetPathPointCount">NPC_GetPathPointCount on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetPathPointCount(int pathid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetPlayerAimingAt">NPC_GetPlayerAimingAt on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetPlayerAimingAt(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetPlayerMovingTo">NPC_GetPlayerMovingTo on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetPlayerMovingTo(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetPos">NPC_GetPos on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetPos(int npcid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetPosMovingTo">NPC_GetPosMovingTo on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetPosMovingTo(int npcid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetRecordCount">NPC_GetRecordCount on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetRecordCount());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetRot">NPC_GetRot on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetRot(int npcid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetSkin">NPC_GetSkin on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetSkin(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetSpecialAction">NPC_GetSpecialAction on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetSpecialAction(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetSurfingObject">NPC_GetSurfingObject on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetSurfingObject(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetSurfingOffsets">NPC_GetSurfingOffsets on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetSurfingOffsets(int npcid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetSurfingPlayerObject">NPC_GetSurfingPlayerObject on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetSurfingPlayerObject(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetSurfingVehicle">NPC_GetSurfingVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetSurfingVehicle(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetVehicle">NPC_GetVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetVehicle(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetVehicleGearState">NPC_GetVehicleGearState on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetVehicleGearState(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetVehicleHealth">NPC_GetVehicleHealth on open.mp</a>
 */
SAMPGDK_NATIVE(float, NPC_GetVehicleHealth(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetVehicleHydraThrusters">NPC_GetVehicleHydraThrusters on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetVehicleHydraThrusters(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetVehicleID">NPC_GetVehicleID on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetVehicleID(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetVehicleSeat">NPC_GetVehicleSeat on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetVehicleSeat(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetVehicleTrainSpeed">NPC_GetVehicleTrainSpeed on open.mp</a>
 */
SAMPGDK_NATIVE(float, NPC_GetVehicleTrainSpeed(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetVelocity">NPC_GetVelocity on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetVelocity(int npcid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetVirtualWorld">NPC_GetVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetVirtualWorld(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetWeapon">NPC_GetWeapon on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetWeapon(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetWeaponAccuracy">NPC_GetWeaponAccuracy on open.mp</a>
 */
SAMPGDK_NATIVE(float, NPC_GetWeaponAccuracy(int npcid, int weaponid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetWeaponActualClipSize">NPC_GetWeaponActualClipSize on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetWeaponActualClipSize(int npcid, int weaponid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetWeaponActualReloadTime">NPC_GetWeaponActualReloadTime on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetWeaponActualReloadTime(int npcid, int weaponid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetWeaponClipSize">NPC_GetWeaponClipSize on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetWeaponClipSize(int npcid, int weaponid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetWeaponReloadTime">NPC_GetWeaponReloadTime on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetWeaponReloadTime(int npcid, int weaponid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetWeaponShootTime">NPC_GetWeaponShootTime on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetWeaponShootTime(int npcid, int weaponid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetWeaponSkillLevel">NPC_GetWeaponSkillLevel on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetWeaponSkillLevel(int npcid, int skill));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_GetWeaponState">NPC_GetWeaponState on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_GetWeaponState(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_HasPathPointInRange">NPC_HasPathPointInRange on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_HasPathPointInRange(int pathId, float x, float y, float z, float radius));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsAiming">NPC_IsAiming on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsAiming(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsAimingAtPlayer">NPC_IsAimingAtPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsAimingAtPlayer(int npcid, int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsAnyStreamedIn">NPC_IsAnyStreamedIn on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsAnyStreamedIn(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsDead">NPC_IsDead on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsDead(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsEnteringVehicle">NPC_IsEnteringVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsEnteringVehicle(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsInfiniteAmmoEnabled">NPC_IsInfiniteAmmoEnabled on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsInfiniteAmmoEnabled(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsInvulnerable">NPC_IsInvulnerable on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsInvulnerable(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsMeleeAttacking">NPC_IsMeleeAttacking on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsMeleeAttacking(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsMoving">NPC_IsMoving on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsMoving(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsMovingToPlayer">NPC_IsMovingToPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsMovingToPlayer(int npcid, int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsNodeOpen">NPC_IsNodeOpen on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsNodeOpen(int nodeid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsPlaybackPaused">NPC_IsPlaybackPaused on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsPlaybackPaused(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsPlayingNode">NPC_IsPlayingNode on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsPlayingNode(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsPlayingNodePaused">NPC_IsPlayingNodePaused on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsPlayingNodePaused(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsPlayingPlayback">NPC_IsPlayingPlayback on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsPlayingPlayback(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsReloadEnabled">NPC_IsReloadEnabled on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsReloadEnabled(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsReloading">NPC_IsReloading on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsReloading(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsShooting">NPC_IsShooting on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsShooting(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsSpawned">NPC_IsSpawned on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsSpawned(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsStreamedIn">NPC_IsStreamedIn on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsStreamedIn(int npcid, int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsValid">NPC_IsValid on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsValid(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsValidPath">NPC_IsValidPath on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsValidPath(int pathid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsValidRecord">NPC_IsValidRecord on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsValidRecord(int recordId));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_IsVehicleSirenUsed">NPC_IsVehicleSirenUsed on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_IsVehicleSirenUsed(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_Kill">NPC_Kill on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_Kill(int npcid, int killerid, int reason));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_LoadRecord">NPC_LoadRecord on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_LoadRecord(const char * filePath));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_MeleeAttack">NPC_MeleeAttack on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_MeleeAttack(int npcid, int time, bool secondaryAttack));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_Move">NPC_Move on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_Move(int npcid, float x, float y, float z, int moveType, float moveSpeed, float stopRange));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_MoveByPath">NPC_MoveByPath on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_MoveByPath(int npcid, int pathid, int moveType, float moveSpeed, bool reversed));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_MoveToPlayer">NPC_MoveToPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_MoveToPlayer(int npcid, int playerid, int moveType, float moveSpeed, float stopRange, int updateDelayMS, int autoRestart));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_OpenNode">NPC_OpenNode on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_OpenNode(int nodeid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_PausePlayback">NPC_PausePlayback on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_PausePlayback(int npcid, bool paused));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_PausePlayingNode">NPC_PausePlayingNode on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_PausePlayingNode(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_PlayNode">NPC_PlayNode on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_PlayNode(int npcid, int nodeid, int moveType, float speed, float radius, bool setAngle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_PutInVehicle">NPC_PutInVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_PutInVehicle(int npcid, int vehicleid, int seatid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_RemoveFromVehicle">NPC_RemoveFromVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_RemoveFromVehicle(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_RemovePointFromPath">NPC_RemovePointFromPath on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_RemovePointFromPath(int pathid, int pointIndex));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_ResetAnimation">NPC_ResetAnimation on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_ResetAnimation(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_ResetSurfingData">NPC_ResetSurfingData on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_ResetSurfingData(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_Respawn">NPC_Respawn on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_Respawn(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_ResumePlayingNode">NPC_ResumePlayingNode on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_ResumePlayingNode(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetAmmo">NPC_SetAmmo on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetAmmo(int npcid, int ammo));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetAmmoInClip">NPC_SetAmmoInClip on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetAmmoInClip(int npcid, int ammo));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetAngleToPlayer">NPC_SetAngleToPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetAngleToPlayer(int npcid, int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetAngleToPos">NPC_SetAngleToPos on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetAngleToPos(int npcid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetAnimation">NPC_SetAnimation on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetAnimation(int npcid, int animationId, float delta, bool loop, bool lockX, bool lockY, bool freeze, int time));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetArmour">NPC_SetArmour on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetArmour(int npcid, float armour));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetFacingAngle">NPC_SetFacingAngle on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetFacingAngle(int npcid, float angle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetFightingStyle">NPC_SetFightingStyle on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetFightingStyle(int npcid, int style));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetHealth">NPC_SetHealth on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetHealth(int npcid, float health));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetInterior">NPC_SetInterior on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetInterior(int npcid, int interiorid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetInvulnerable">NPC_SetInvulnerable on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetInvulnerable(int npcid, bool toggle));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetKeys">NPC_SetKeys on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetKeys(int npcid, int upAndDown, int leftAndRight, int keys));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetNodePoint">NPC_SetNodePoint on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_SetNodePoint(int nodeid, int pointid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetPos">NPC_SetPos on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetPos(int npcid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetRot">NPC_SetRot on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetRot(int npcid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetSkin">NPC_SetSkin on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetSkin(int npcid, int skinid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetSpecialAction">NPC_SetSpecialAction on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetSpecialAction(int npcid, int actionid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetSurfingObject">NPC_SetSurfingObject on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetSurfingObject(int npcid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetSurfingOffsets">NPC_SetSurfingOffsets on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetSurfingOffsets(int npcid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetSurfingPlayerObject">NPC_SetSurfingPlayerObject on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetSurfingPlayerObject(int npcid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetSurfingVehicle">NPC_SetSurfingVehicle on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetSurfingVehicle(int npcid, int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetVehicleGearState">NPC_SetVehicleGearState on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetVehicleGearState(int npcid, int gearState));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetVehicleHealth">NPC_SetVehicleHealth on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetVehicleHealth(int npcid, float health));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetVehicleHydraThrusters">NPC_SetVehicleHydraThrusters on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetVehicleHydraThrusters(int npcid, int direction));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetVehicleTrainSpeed">NPC_SetVehicleTrainSpeed on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetVehicleTrainSpeed(int npcid, float speed));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetVelocity">NPC_SetVelocity on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetVelocity(int npcid, float x, float y, float z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetVirtualWorld">NPC_SetVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetVirtualWorld(int npcid, int virtualWorld));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetWeapon">NPC_SetWeapon on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetWeapon(int npcid, int weaponid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetWeaponAccuracy">NPC_SetWeaponAccuracy on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetWeaponAccuracy(int npcid, int weaponid, float accuracy));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetWeaponClipSize">NPC_SetWeaponClipSize on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetWeaponClipSize(int npcid, int weaponid, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetWeaponReloadTime">NPC_SetWeaponReloadTime on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetWeaponReloadTime(int npcid, int weaponid, int time));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetWeaponShootTime">NPC_SetWeaponShootTime on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetWeaponShootTime(int npcid, int weaponid, int time));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetWeaponSkillLevel">NPC_SetWeaponSkillLevel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_SetWeaponSkillLevel(int npcid, int skill, int level));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_SetWeaponState">NPC_SetWeaponState on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_SetWeaponState(int npcid, int weaponState));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_Shoot">NPC_Shoot on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_Shoot(int npcid, int weaponid, int hitId, int hitType, float endPointX, float endPointY, float endPointZ, float offsetX, float offsetY, float offsetZ, bool isHit, int checkInBetweenFlags));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_Spawn">NPC_Spawn on open.mp</a>
 */
SAMPGDK_NATIVE(int, NPC_Spawn(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_StartPlayback">NPC_StartPlayback on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_StartPlayback(int npcid, const char * recordName, bool autoUnload, float startX, float startY, float startZ, float rotX, float rotY, float rotZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_StartPlaybackEx">NPC_StartPlaybackEx on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_StartPlaybackEx(int npcid, int recordId, bool autoUnload, float startX, float startY, float startZ, float rotX, float rotY, float rotZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_StopAim">NPC_StopAim on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_StopAim(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_StopMeleeAttack">NPC_StopMeleeAttack on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_StopMeleeAttack(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_StopMove">NPC_StopMove on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_StopMove(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_StopPlayback">NPC_StopPlayback on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_StopPlayback(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_StopPlayingNode">NPC_StopPlayingNode on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_StopPlayingNode(int npcid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_UnloadAllRecords">NPC_UnloadAllRecords on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_UnloadAllRecords());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_UnloadRecord">NPC_UnloadRecord on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_UnloadRecord(int recordId));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_UpdateNodePoint">NPC_UpdateNodePoint on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_UpdateNodePoint(int npcid, int pointid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/NPC_UseVehicleSiren">NPC_UseVehicleSiren on open.mp</a>
 */
SAMPGDK_NATIVE(bool, NPC_UseVehicleSiren(int npcid, bool use));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool NPC_AddPointToPath(int pathid, float x, float y, float z, float stopRange = 0.2) {
  return sampgdk_NPC_AddPointToPath(pathid, x, y, z, stopRange);
}

inline int NPC_AimAt(int npcid, float pointX, float pointY, float pointZ, bool shoot, int shootDelay, bool updateAngle, float offsetFromX, float offsetFromY, float offsetFromZ, int checkInBetweenFlags) {
  return sampgdk_NPC_AimAt(npcid, pointX, pointY, pointZ, shoot, shootDelay, updateAngle, offsetFromX, offsetFromY, offsetFromZ, checkInBetweenFlags);
}

inline int NPC_AimAtPlayer(int npcid, int playerid, bool shoot, int shootDelay, bool updateAngle, float offsetX, float offsetY, float offsetZ, float offsetFromX, float offsetFromY, float offsetFromZ, int checkInBetweenFlags) {
  return sampgdk_NPC_AimAtPlayer(npcid, playerid, shoot, shootDelay, updateAngle, offsetX, offsetY, offsetZ, offsetFromX, offsetFromY, offsetFromZ, checkInBetweenFlags);
}

inline int NPC_ApplyAnimation(int npcid, const char * animlib, const char * animname, float delta, bool loop, bool lockX, bool lockY, bool freeze, int time) {
  return sampgdk_NPC_ApplyAnimation(npcid, animlib, animname, delta, loop, lockX, lockY, freeze, time);
}

inline int NPC_ChangeNode(int npcid, int nodeid, int linkid) {
  return sampgdk_NPC_ChangeNode(npcid, nodeid, linkid);
}

inline int NPC_ClearAnimations(int npcid) {
  return sampgdk_NPC_ClearAnimations(npcid);
}

inline bool NPC_ClearPath(int pathid) {
  return sampgdk_NPC_ClearPath(pathid);
}

inline bool NPC_CloseNode(int nodeid) {
  return sampgdk_NPC_CloseNode(nodeid);
}

inline int NPC_Create(const char * name) {
  return sampgdk_NPC_Create(name);
}

inline int NPC_CreatePath() {
  return sampgdk_NPC_CreatePath();
}

inline int NPC_Destroy(int npcid) {
  return sampgdk_NPC_Destroy(npcid);
}

inline bool NPC_DestroyAllPath() {
  return sampgdk_NPC_DestroyAllPath();
}

inline bool NPC_DestroyPath(int pathid) {
  return sampgdk_NPC_DestroyPath(pathid);
}

inline bool NPC_EnableInfiniteAmmo(int npcid, bool enable = true) {
  return sampgdk_NPC_EnableInfiniteAmmo(npcid, enable);
}

inline bool NPC_EnableReloading(int npcid, bool enable = true) {
  return sampgdk_NPC_EnableReloading(npcid, enable);
}

inline bool NPC_EnterVehicle(int npcid, int vehicleid, int seatid, int moveType) {
  return sampgdk_NPC_EnterVehicle(npcid, vehicleid, seatid, moveType);
}

inline bool NPC_ExitVehicle(int npcid) {
  return sampgdk_NPC_ExitVehicle(npcid);
}

inline int NPC_GetAll(char * npcs, int size) {
  return sampgdk_NPC_GetAll(npcs, size);
}

inline int NPC_GetAmmo(int npcid) {
  return sampgdk_NPC_GetAmmo(npcid);
}

inline int NPC_GetAmmoInClip(int npcid) {
  return sampgdk_NPC_GetAmmoInClip(npcid);
}

inline int NPC_GetAnimation(int npcid, int * animationId, float * delta, bool * loop, bool * lockX, bool * lockY, bool * freeze, int * time) {
  return sampgdk_NPC_GetAnimation(npcid, animationId, delta, loop, lockX, lockY, freeze, time);
}

inline float NPC_GetArmour(int npcid) {
  return sampgdk_NPC_GetArmour(npcid);
}

inline int NPC_GetCurrentPathPointIndex(int npcid) {
  return sampgdk_NPC_GetCurrentPathPointIndex(npcid);
}

inline int NPC_GetCustomSkin(int npcid) {
  return sampgdk_NPC_GetCustomSkin(npcid);
}

inline int NPC_GetEnteringVehicle(int npcid) {
  return sampgdk_NPC_GetEnteringVehicle(npcid);
}

inline int NPC_GetEnteringVehicleID(int npcid) {
  return sampgdk_NPC_GetEnteringVehicleID(npcid);
}

inline int NPC_GetEnteringVehicleSeat(int npcid) {
  return sampgdk_NPC_GetEnteringVehicleSeat(npcid);
}

inline int NPC_GetFacingAngle(int npcid, float * angle) {
  return sampgdk_NPC_GetFacingAngle(npcid, angle);
}

inline int NPC_GetFightingStyle(int npcid) {
  return sampgdk_NPC_GetFightingStyle(npcid);
}

inline float NPC_GetHealth(int npcid) {
  return sampgdk_NPC_GetHealth(npcid);
}

inline int NPC_GetInterior(int npcid) {
  return sampgdk_NPC_GetInterior(npcid);
}

inline int NPC_GetKeys(int npcid, int * upAnddown, int * leftandRight, int * keys) {
  return sampgdk_NPC_GetKeys(npcid, upAnddown, leftandRight, keys);
}

inline bool NPC_GetNodeInfo(int nodeid, int * vehnodes, int * pednodes, int * navinode) {
  return sampgdk_NPC_GetNodeInfo(nodeid, vehnodes, pednodes, navinode);
}

inline int NPC_GetNodePointCount(int nodeid) {
  return sampgdk_NPC_GetNodePointCount(nodeid);
}

inline bool NPC_GetNodePointPosition(int nodeid, float * x, float * y, float * z) {
  return sampgdk_NPC_GetNodePointPosition(nodeid, x, y, z);
}

inline int NPC_GetNodeType(int nodeid) {
  return sampgdk_NPC_GetNodeType(nodeid);
}

inline int NPC_GetPathCount() {
  return sampgdk_NPC_GetPathCount();
}

inline bool NPC_GetPathPoint(int pathid, int pointIndex, float * x, float * y, float * z, float * stopRange) {
  return sampgdk_NPC_GetPathPoint(pathid, pointIndex, x, y, z, stopRange);
}

inline int NPC_GetPathPointCount(int pathid) {
  return sampgdk_NPC_GetPathPointCount(pathid);
}

inline int NPC_GetPlayerAimingAt(int npcid) {
  return sampgdk_NPC_GetPlayerAimingAt(npcid);
}

inline int NPC_GetPlayerMovingTo(int npcid) {
  return sampgdk_NPC_GetPlayerMovingTo(npcid);
}

inline int NPC_GetPos(int npcid, float * x, float * y, float * z) {
  return sampgdk_NPC_GetPos(npcid, x, y, z);
}

inline int NPC_GetPosMovingTo(int npcid, float * x, float * y, float * z) {
  return sampgdk_NPC_GetPosMovingTo(npcid, x, y, z);
}

inline int NPC_GetRecordCount() {
  return sampgdk_NPC_GetRecordCount();
}

inline int NPC_GetRot(int npcid, float * x, float * y, float * z) {
  return sampgdk_NPC_GetRot(npcid, x, y, z);
}

inline int NPC_GetSkin(int npcid) {
  return sampgdk_NPC_GetSkin(npcid);
}

inline int NPC_GetSpecialAction(int npcid) {
  return sampgdk_NPC_GetSpecialAction(npcid);
}

inline int NPC_GetSurfingObject(int npcid) {
  return sampgdk_NPC_GetSurfingObject(npcid);
}

inline int NPC_GetSurfingOffsets(int npcid, float * x, float * y, float * z) {
  return sampgdk_NPC_GetSurfingOffsets(npcid, x, y, z);
}

inline int NPC_GetSurfingPlayerObject(int npcid) {
  return sampgdk_NPC_GetSurfingPlayerObject(npcid);
}

inline int NPC_GetSurfingVehicle(int npcid) {
  return sampgdk_NPC_GetSurfingVehicle(npcid);
}

inline int NPC_GetVehicle(int npcid) {
  return sampgdk_NPC_GetVehicle(npcid);
}

inline int NPC_GetVehicleGearState(int npcid) {
  return sampgdk_NPC_GetVehicleGearState(npcid);
}

inline float NPC_GetVehicleHealth(int npcid) {
  return sampgdk_NPC_GetVehicleHealth(npcid);
}

inline int NPC_GetVehicleHydraThrusters(int npcid) {
  return sampgdk_NPC_GetVehicleHydraThrusters(npcid);
}

inline int NPC_GetVehicleID(int npcid) {
  return sampgdk_NPC_GetVehicleID(npcid);
}

inline int NPC_GetVehicleSeat(int npcid) {
  return sampgdk_NPC_GetVehicleSeat(npcid);
}

inline float NPC_GetVehicleTrainSpeed(int npcid) {
  return sampgdk_NPC_GetVehicleTrainSpeed(npcid);
}

inline int NPC_GetVelocity(int npcid, float * x, float * y, float * z) {
  return sampgdk_NPC_GetVelocity(npcid, x, y, z);
}

inline int NPC_GetVirtualWorld(int npcid) {
  return sampgdk_NPC_GetVirtualWorld(npcid);
}

inline int NPC_GetWeapon(int npcid) {
  return sampgdk_NPC_GetWeapon(npcid);
}

inline float NPC_GetWeaponAccuracy(int npcid, int weaponid) {
  return sampgdk_NPC_GetWeaponAccuracy(npcid, weaponid);
}

inline int NPC_GetWeaponActualClipSize(int npcid, int weaponid) {
  return sampgdk_NPC_GetWeaponActualClipSize(npcid, weaponid);
}

inline int NPC_GetWeaponActualReloadTime(int npcid, int weaponid) {
  return sampgdk_NPC_GetWeaponActualReloadTime(npcid, weaponid);
}

inline int NPC_GetWeaponClipSize(int npcid, int weaponid) {
  return sampgdk_NPC_GetWeaponClipSize(npcid, weaponid);
}

inline int NPC_GetWeaponReloadTime(int npcid, int weaponid) {
  return sampgdk_NPC_GetWeaponReloadTime(npcid, weaponid);
}

inline int NPC_GetWeaponShootTime(int npcid, int weaponid) {
  return sampgdk_NPC_GetWeaponShootTime(npcid, weaponid);
}

inline int NPC_GetWeaponSkillLevel(int npcid, int skill) {
  return sampgdk_NPC_GetWeaponSkillLevel(npcid, skill);
}

inline int NPC_GetWeaponState(int npcid) {
  return sampgdk_NPC_GetWeaponState(npcid);
}

inline bool NPC_HasPathPointInRange(int pathId, float x, float y, float z, float radius) {
  return sampgdk_NPC_HasPathPointInRange(pathId, x, y, z, radius);
}

inline bool NPC_IsAiming(int npcid) {
  return sampgdk_NPC_IsAiming(npcid);
}

inline bool NPC_IsAimingAtPlayer(int npcid, int playerid) {
  return sampgdk_NPC_IsAimingAtPlayer(npcid, playerid);
}

inline bool NPC_IsAnyStreamedIn(int npcid) {
  return sampgdk_NPC_IsAnyStreamedIn(npcid);
}

inline bool NPC_IsDead(int npcid) {
  return sampgdk_NPC_IsDead(npcid);
}

inline bool NPC_IsEnteringVehicle(int npcid) {
  return sampgdk_NPC_IsEnteringVehicle(npcid);
}

inline bool NPC_IsInfiniteAmmoEnabled(int npcid) {
  return sampgdk_NPC_IsInfiniteAmmoEnabled(npcid);
}

inline bool NPC_IsInvulnerable(int npcid) {
  return sampgdk_NPC_IsInvulnerable(npcid);
}

inline bool NPC_IsMeleeAttacking(int npcid) {
  return sampgdk_NPC_IsMeleeAttacking(npcid);
}

inline bool NPC_IsMoving(int npcid) {
  return sampgdk_NPC_IsMoving(npcid);
}

inline bool NPC_IsMovingToPlayer(int npcid, int playerid) {
  return sampgdk_NPC_IsMovingToPlayer(npcid, playerid);
}

inline bool NPC_IsNodeOpen(int nodeid) {
  return sampgdk_NPC_IsNodeOpen(nodeid);
}

inline bool NPC_IsPlaybackPaused(int npcid) {
  return sampgdk_NPC_IsPlaybackPaused(npcid);
}

inline bool NPC_IsPlayingNode(int npcid) {
  return sampgdk_NPC_IsPlayingNode(npcid);
}

inline bool NPC_IsPlayingNodePaused(int npcid) {
  return sampgdk_NPC_IsPlayingNodePaused(npcid);
}

inline bool NPC_IsPlayingPlayback(int npcid) {
  return sampgdk_NPC_IsPlayingPlayback(npcid);
}

inline bool NPC_IsReloadEnabled(int npcid) {
  return sampgdk_NPC_IsReloadEnabled(npcid);
}

inline bool NPC_IsReloading(int npcid) {
  return sampgdk_NPC_IsReloading(npcid);
}

inline bool NPC_IsShooting(int npcid) {
  return sampgdk_NPC_IsShooting(npcid);
}

inline bool NPC_IsSpawned(int npcid) {
  return sampgdk_NPC_IsSpawned(npcid);
}

inline bool NPC_IsStreamedIn(int npcid, int playerid) {
  return sampgdk_NPC_IsStreamedIn(npcid, playerid);
}

inline bool NPC_IsValid(int npcid) {
  return sampgdk_NPC_IsValid(npcid);
}

inline bool NPC_IsValidPath(int pathid) {
  return sampgdk_NPC_IsValidPath(pathid);
}

inline bool NPC_IsValidRecord(int recordId) {
  return sampgdk_NPC_IsValidRecord(recordId);
}

inline bool NPC_IsVehicleSirenUsed(int npcid) {
  return sampgdk_NPC_IsVehicleSirenUsed(npcid);
}

inline int NPC_Kill(int npcid, int killerid, int reason) {
  return sampgdk_NPC_Kill(npcid, killerid, reason);
}

inline int NPC_LoadRecord(const char * filePath) {
  return sampgdk_NPC_LoadRecord(filePath);
}

inline bool NPC_MeleeAttack(int npcid, int time = -1, bool secondaryAttack = false) {
  return sampgdk_NPC_MeleeAttack(npcid, time, secondaryAttack);
}

inline int NPC_Move(int npcid, float x, float y, float z, int moveType, float moveSpeed, float stopRange = 0.2) {
  return sampgdk_NPC_Move(npcid, x, y, z, moveType, moveSpeed, stopRange);
}

inline bool NPC_MoveByPath(int npcid, int pathid, int moveType, float moveSpeed, bool reversed = false) {
  return sampgdk_NPC_MoveByPath(npcid, pathid, moveType, moveSpeed, reversed);
}

inline int NPC_MoveToPlayer(int npcid, int playerid, int moveType, float moveSpeed, float stopRange = 0.2, int updateDelayMS = 500, int autoRestart = false) {
  return sampgdk_NPC_MoveToPlayer(npcid, playerid, moveType, moveSpeed, stopRange, updateDelayMS, autoRestart);
}

inline bool NPC_OpenNode(int nodeid) {
  return sampgdk_NPC_OpenNode(nodeid);
}

inline bool NPC_PausePlayback(int npcid, bool paused = true) {
  return sampgdk_NPC_PausePlayback(npcid, paused);
}

inline bool NPC_PausePlayingNode(int npcid) {
  return sampgdk_NPC_PausePlayingNode(npcid);
}

inline bool NPC_PlayNode(int npcid, int nodeid, int moveType, float speed, float radius = 0.0, bool setAngle = true) {
  return sampgdk_NPC_PlayNode(npcid, nodeid, moveType, speed, radius, setAngle);
}

inline bool NPC_PutInVehicle(int npcid, int vehicleid, int seatid) {
  return sampgdk_NPC_PutInVehicle(npcid, vehicleid, seatid);
}

inline bool NPC_RemoveFromVehicle(int npcid) {
  return sampgdk_NPC_RemoveFromVehicle(npcid);
}

inline bool NPC_RemovePointFromPath(int pathid, int pointIndex) {
  return sampgdk_NPC_RemovePointFromPath(pathid, pointIndex);
}

inline int NPC_ResetAnimation(int npcid) {
  return sampgdk_NPC_ResetAnimation(npcid);
}

inline int NPC_ResetSurfingData(int npcid) {
  return sampgdk_NPC_ResetSurfingData(npcid);
}

inline int NPC_Respawn(int npcid) {
  return sampgdk_NPC_Respawn(npcid);
}

inline bool NPC_ResumePlayingNode(int npcid) {
  return sampgdk_NPC_ResumePlayingNode(npcid);
}

inline int NPC_SetAmmo(int npcid, int ammo) {
  return sampgdk_NPC_SetAmmo(npcid, ammo);
}

inline int NPC_SetAmmoInClip(int npcid, int ammo) {
  return sampgdk_NPC_SetAmmoInClip(npcid, ammo);
}

inline int NPC_SetAngleToPlayer(int npcid, int playerid) {
  return sampgdk_NPC_SetAngleToPlayer(npcid, playerid);
}

inline int NPC_SetAngleToPos(int npcid, float x, float y, float z) {
  return sampgdk_NPC_SetAngleToPos(npcid, x, y, z);
}

inline int NPC_SetAnimation(int npcid, int animationId, float delta, bool loop, bool lockX, bool lockY, bool freeze, int time) {
  return sampgdk_NPC_SetAnimation(npcid, animationId, delta, loop, lockX, lockY, freeze, time);
}

inline int NPC_SetArmour(int npcid, float armour) {
  return sampgdk_NPC_SetArmour(npcid, armour);
}

inline int NPC_SetFacingAngle(int npcid, float angle) {
  return sampgdk_NPC_SetFacingAngle(npcid, angle);
}

inline int NPC_SetFightingStyle(int npcid, int style) {
  return sampgdk_NPC_SetFightingStyle(npcid, style);
}

inline int NPC_SetHealth(int npcid, float health) {
  return sampgdk_NPC_SetHealth(npcid, health);
}

inline int NPC_SetInterior(int npcid, int interiorid) {
  return sampgdk_NPC_SetInterior(npcid, interiorid);
}

inline int NPC_SetInvulnerable(int npcid, bool toggle = true) {
  return sampgdk_NPC_SetInvulnerable(npcid, toggle);
}

inline int NPC_SetKeys(int npcid, int upAndDown, int leftAndRight, int keys) {
  return sampgdk_NPC_SetKeys(npcid, upAndDown, leftAndRight, keys);
}

inline bool NPC_SetNodePoint(int nodeid, int pointid) {
  return sampgdk_NPC_SetNodePoint(nodeid, pointid);
}

inline int NPC_SetPos(int npcid, float x, float y, float z) {
  return sampgdk_NPC_SetPos(npcid, x, y, z);
}

inline int NPC_SetRot(int npcid, float x, float y, float z) {
  return sampgdk_NPC_SetRot(npcid, x, y, z);
}

inline int NPC_SetSkin(int npcid, int skinid) {
  return sampgdk_NPC_SetSkin(npcid, skinid);
}

inline int NPC_SetSpecialAction(int npcid, int actionid) {
  return sampgdk_NPC_SetSpecialAction(npcid, actionid);
}

inline int NPC_SetSurfingObject(int npcid, int objectid) {
  return sampgdk_NPC_SetSurfingObject(npcid, objectid);
}

inline int NPC_SetSurfingOffsets(int npcid, float x, float y, float z) {
  return sampgdk_NPC_SetSurfingOffsets(npcid, x, y, z);
}

inline int NPC_SetSurfingPlayerObject(int npcid, int objectid) {
  return sampgdk_NPC_SetSurfingPlayerObject(npcid, objectid);
}

inline int NPC_SetSurfingVehicle(int npcid, int vehicleid) {
  return sampgdk_NPC_SetSurfingVehicle(npcid, vehicleid);
}

inline int NPC_SetVehicleGearState(int npcid, int gearState) {
  return sampgdk_NPC_SetVehicleGearState(npcid, gearState);
}

inline int NPC_SetVehicleHealth(int npcid, float health) {
  return sampgdk_NPC_SetVehicleHealth(npcid, health);
}

inline int NPC_SetVehicleHydraThrusters(int npcid, int direction) {
  return sampgdk_NPC_SetVehicleHydraThrusters(npcid, direction);
}

inline int NPC_SetVehicleTrainSpeed(int npcid, float speed) {
  return sampgdk_NPC_SetVehicleTrainSpeed(npcid, speed);
}

inline int NPC_SetVelocity(int npcid, float x, float y, float z) {
  return sampgdk_NPC_SetVelocity(npcid, x, y, z);
}

inline int NPC_SetVirtualWorld(int npcid, int virtualWorld) {
  return sampgdk_NPC_SetVirtualWorld(npcid, virtualWorld);
}

inline int NPC_SetWeapon(int npcid, int weaponid) {
  return sampgdk_NPC_SetWeapon(npcid, weaponid);
}

inline int NPC_SetWeaponAccuracy(int npcid, int weaponid, float accuracy) {
  return sampgdk_NPC_SetWeaponAccuracy(npcid, weaponid, accuracy);
}

inline int NPC_SetWeaponClipSize(int npcid, int weaponid, int size) {
  return sampgdk_NPC_SetWeaponClipSize(npcid, weaponid, size);
}

inline int NPC_SetWeaponReloadTime(int npcid, int weaponid, int time) {
  return sampgdk_NPC_SetWeaponReloadTime(npcid, weaponid, time);
}

inline int NPC_SetWeaponShootTime(int npcid, int weaponid, int time) {
  return sampgdk_NPC_SetWeaponShootTime(npcid, weaponid, time);
}

inline bool NPC_SetWeaponSkillLevel(int npcid, int skill, int level) {
  return sampgdk_NPC_SetWeaponSkillLevel(npcid, skill, level);
}

inline int NPC_SetWeaponState(int npcid, int weaponState) {
  return sampgdk_NPC_SetWeaponState(npcid, weaponState);
}

inline int NPC_Shoot(int npcid, int weaponid, int hitId, int hitType, float endPointX, float endPointY, float endPointZ, float offsetX, float offsetY, float offsetZ, bool isHit, int checkInBetweenFlags) {
  return sampgdk_NPC_Shoot(npcid, weaponid, hitId, hitType, endPointX, endPointY, endPointZ, offsetX, offsetY, offsetZ, isHit, checkInBetweenFlags);
}

inline int NPC_Spawn(int npcid) {
  return sampgdk_NPC_Spawn(npcid);
}

inline bool NPC_StartPlayback(int npcid, const char * recordName, bool autoUnload = false, float startX = 0.0, float startY = 0.0, float startZ = 0.0, float rotX = 0.0, float rotY = 0.0, float rotZ = 0.0) {
  return sampgdk_NPC_StartPlayback(npcid, recordName, autoUnload, startX, startY, startZ, rotX, rotY, rotZ);
}

inline bool NPC_StartPlaybackEx(int npcid, int recordId, bool autoUnload = false, float startX = 0.0, float startY = 0.0, float startZ = 0.0, float rotX = 0.0, float rotY = 0.0, float rotZ = 0.0) {
  return sampgdk_NPC_StartPlaybackEx(npcid, recordId, autoUnload, startX, startY, startZ, rotX, rotY, rotZ);
}

inline bool NPC_StopAim(int npcid) {
  return sampgdk_NPC_StopAim(npcid);
}

inline bool NPC_StopMeleeAttack(int npcid) {
  return sampgdk_NPC_StopMeleeAttack(npcid);
}

inline bool NPC_StopMove(int npcid) {
  return sampgdk_NPC_StopMove(npcid);
}

inline bool NPC_StopPlayback(int npcid) {
  return sampgdk_NPC_StopPlayback(npcid);
}

inline bool NPC_StopPlayingNode(int npcid) {
  return sampgdk_NPC_StopPlayingNode(npcid);
}

inline bool NPC_UnloadAllRecords() {
  return sampgdk_NPC_UnloadAllRecords();
}

inline bool NPC_UnloadRecord(int recordId) {
  return sampgdk_NPC_UnloadRecord(recordId);
}

inline bool NPC_UpdateNodePoint(int npcid, int pointid) {
  return sampgdk_NPC_UpdateNodePoint(npcid, pointid);
}

inline bool NPC_UseVehicleSiren(int npcid, bool use = true) {
  return sampgdk_NPC_UseVehicleSiren(npcid, use);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  NPC_AddPointToPath
#define NPC_AddPointToPath sampgdk_NPC_AddPointToPath

#undef  NPC_AimAt
#define NPC_AimAt sampgdk_NPC_AimAt

#undef  NPC_AimAtPlayer
#define NPC_AimAtPlayer sampgdk_NPC_AimAtPlayer

#undef  NPC_ApplyAnimation
#define NPC_ApplyAnimation sampgdk_NPC_ApplyAnimation

#undef  NPC_ChangeNode
#define NPC_ChangeNode sampgdk_NPC_ChangeNode

#undef  NPC_ClearAnimations
#define NPC_ClearAnimations sampgdk_NPC_ClearAnimations

#undef  NPC_ClearPath
#define NPC_ClearPath sampgdk_NPC_ClearPath

#undef  NPC_CloseNode
#define NPC_CloseNode sampgdk_NPC_CloseNode

#undef  NPC_Create
#define NPC_Create sampgdk_NPC_Create

#undef  NPC_CreatePath
#define NPC_CreatePath sampgdk_NPC_CreatePath

#undef  NPC_Destroy
#define NPC_Destroy sampgdk_NPC_Destroy

#undef  NPC_DestroyAllPath
#define NPC_DestroyAllPath sampgdk_NPC_DestroyAllPath

#undef  NPC_DestroyPath
#define NPC_DestroyPath sampgdk_NPC_DestroyPath

#undef  NPC_EnableInfiniteAmmo
#define NPC_EnableInfiniteAmmo sampgdk_NPC_EnableInfiniteAmmo

#undef  NPC_EnableReloading
#define NPC_EnableReloading sampgdk_NPC_EnableReloading

#undef  NPC_EnterVehicle
#define NPC_EnterVehicle sampgdk_NPC_EnterVehicle

#undef  NPC_ExitVehicle
#define NPC_ExitVehicle sampgdk_NPC_ExitVehicle

#undef  NPC_GetAll
#define NPC_GetAll sampgdk_NPC_GetAll

#undef  NPC_GetAmmo
#define NPC_GetAmmo sampgdk_NPC_GetAmmo

#undef  NPC_GetAmmoInClip
#define NPC_GetAmmoInClip sampgdk_NPC_GetAmmoInClip

#undef  NPC_GetAnimation
#define NPC_GetAnimation sampgdk_NPC_GetAnimation

#undef  NPC_GetArmour
#define NPC_GetArmour sampgdk_NPC_GetArmour

#undef  NPC_GetCurrentPathPointIndex
#define NPC_GetCurrentPathPointIndex sampgdk_NPC_GetCurrentPathPointIndex

#undef  NPC_GetCustomSkin
#define NPC_GetCustomSkin sampgdk_NPC_GetCustomSkin

#undef  NPC_GetEnteringVehicle
#define NPC_GetEnteringVehicle sampgdk_NPC_GetEnteringVehicle

#undef  NPC_GetEnteringVehicleID
#define NPC_GetEnteringVehicleID sampgdk_NPC_GetEnteringVehicleID

#undef  NPC_GetEnteringVehicleSeat
#define NPC_GetEnteringVehicleSeat sampgdk_NPC_GetEnteringVehicleSeat

#undef  NPC_GetFacingAngle
#define NPC_GetFacingAngle sampgdk_NPC_GetFacingAngle

#undef  NPC_GetFightingStyle
#define NPC_GetFightingStyle sampgdk_NPC_GetFightingStyle

#undef  NPC_GetHealth
#define NPC_GetHealth sampgdk_NPC_GetHealth

#undef  NPC_GetInterior
#define NPC_GetInterior sampgdk_NPC_GetInterior

#undef  NPC_GetKeys
#define NPC_GetKeys sampgdk_NPC_GetKeys

#undef  NPC_GetNodeInfo
#define NPC_GetNodeInfo sampgdk_NPC_GetNodeInfo

#undef  NPC_GetNodePointCount
#define NPC_GetNodePointCount sampgdk_NPC_GetNodePointCount

#undef  NPC_GetNodePointPosition
#define NPC_GetNodePointPosition sampgdk_NPC_GetNodePointPosition

#undef  NPC_GetNodeType
#define NPC_GetNodeType sampgdk_NPC_GetNodeType

#undef  NPC_GetPathCount
#define NPC_GetPathCount sampgdk_NPC_GetPathCount

#undef  NPC_GetPathPoint
#define NPC_GetPathPoint sampgdk_NPC_GetPathPoint

#undef  NPC_GetPathPointCount
#define NPC_GetPathPointCount sampgdk_NPC_GetPathPointCount

#undef  NPC_GetPlayerAimingAt
#define NPC_GetPlayerAimingAt sampgdk_NPC_GetPlayerAimingAt

#undef  NPC_GetPlayerMovingTo
#define NPC_GetPlayerMovingTo sampgdk_NPC_GetPlayerMovingTo

#undef  NPC_GetPos
#define NPC_GetPos sampgdk_NPC_GetPos

#undef  NPC_GetPosMovingTo
#define NPC_GetPosMovingTo sampgdk_NPC_GetPosMovingTo

#undef  NPC_GetRecordCount
#define NPC_GetRecordCount sampgdk_NPC_GetRecordCount

#undef  NPC_GetRot
#define NPC_GetRot sampgdk_NPC_GetRot

#undef  NPC_GetSkin
#define NPC_GetSkin sampgdk_NPC_GetSkin

#undef  NPC_GetSpecialAction
#define NPC_GetSpecialAction sampgdk_NPC_GetSpecialAction

#undef  NPC_GetSurfingObject
#define NPC_GetSurfingObject sampgdk_NPC_GetSurfingObject

#undef  NPC_GetSurfingOffsets
#define NPC_GetSurfingOffsets sampgdk_NPC_GetSurfingOffsets

#undef  NPC_GetSurfingPlayerObject
#define NPC_GetSurfingPlayerObject sampgdk_NPC_GetSurfingPlayerObject

#undef  NPC_GetSurfingVehicle
#define NPC_GetSurfingVehicle sampgdk_NPC_GetSurfingVehicle

#undef  NPC_GetVehicle
#define NPC_GetVehicle sampgdk_NPC_GetVehicle

#undef  NPC_GetVehicleGearState
#define NPC_GetVehicleGearState sampgdk_NPC_GetVehicleGearState

#undef  NPC_GetVehicleHealth
#define NPC_GetVehicleHealth sampgdk_NPC_GetVehicleHealth

#undef  NPC_GetVehicleHydraThrusters
#define NPC_GetVehicleHydraThrusters sampgdk_NPC_GetVehicleHydraThrusters

#undef  NPC_GetVehicleID
#define NPC_GetVehicleID sampgdk_NPC_GetVehicleID

#undef  NPC_GetVehicleSeat
#define NPC_GetVehicleSeat sampgdk_NPC_GetVehicleSeat

#undef  NPC_GetVehicleTrainSpeed
#define NPC_GetVehicleTrainSpeed sampgdk_NPC_GetVehicleTrainSpeed

#undef  NPC_GetVelocity
#define NPC_GetVelocity sampgdk_NPC_GetVelocity

#undef  NPC_GetVirtualWorld
#define NPC_GetVirtualWorld sampgdk_NPC_GetVirtualWorld

#undef  NPC_GetWeapon
#define NPC_GetWeapon sampgdk_NPC_GetWeapon

#undef  NPC_GetWeaponAccuracy
#define NPC_GetWeaponAccuracy sampgdk_NPC_GetWeaponAccuracy

#undef  NPC_GetWeaponActualClipSize
#define NPC_GetWeaponActualClipSize sampgdk_NPC_GetWeaponActualClipSize

#undef  NPC_GetWeaponActualReloadTime
#define NPC_GetWeaponActualReloadTime sampgdk_NPC_GetWeaponActualReloadTime

#undef  NPC_GetWeaponClipSize
#define NPC_GetWeaponClipSize sampgdk_NPC_GetWeaponClipSize

#undef  NPC_GetWeaponReloadTime
#define NPC_GetWeaponReloadTime sampgdk_NPC_GetWeaponReloadTime

#undef  NPC_GetWeaponShootTime
#define NPC_GetWeaponShootTime sampgdk_NPC_GetWeaponShootTime

#undef  NPC_GetWeaponSkillLevel
#define NPC_GetWeaponSkillLevel sampgdk_NPC_GetWeaponSkillLevel

#undef  NPC_GetWeaponState
#define NPC_GetWeaponState sampgdk_NPC_GetWeaponState

#undef  NPC_HasPathPointInRange
#define NPC_HasPathPointInRange sampgdk_NPC_HasPathPointInRange

#undef  NPC_IsAiming
#define NPC_IsAiming sampgdk_NPC_IsAiming

#undef  NPC_IsAimingAtPlayer
#define NPC_IsAimingAtPlayer sampgdk_NPC_IsAimingAtPlayer

#undef  NPC_IsAnyStreamedIn
#define NPC_IsAnyStreamedIn sampgdk_NPC_IsAnyStreamedIn

#undef  NPC_IsDead
#define NPC_IsDead sampgdk_NPC_IsDead

#undef  NPC_IsEnteringVehicle
#define NPC_IsEnteringVehicle sampgdk_NPC_IsEnteringVehicle

#undef  NPC_IsInfiniteAmmoEnabled
#define NPC_IsInfiniteAmmoEnabled sampgdk_NPC_IsInfiniteAmmoEnabled

#undef  NPC_IsInvulnerable
#define NPC_IsInvulnerable sampgdk_NPC_IsInvulnerable

#undef  NPC_IsMeleeAttacking
#define NPC_IsMeleeAttacking sampgdk_NPC_IsMeleeAttacking

#undef  NPC_IsMoving
#define NPC_IsMoving sampgdk_NPC_IsMoving

#undef  NPC_IsMovingToPlayer
#define NPC_IsMovingToPlayer sampgdk_NPC_IsMovingToPlayer

#undef  NPC_IsNodeOpen
#define NPC_IsNodeOpen sampgdk_NPC_IsNodeOpen

#undef  NPC_IsPlaybackPaused
#define NPC_IsPlaybackPaused sampgdk_NPC_IsPlaybackPaused

#undef  NPC_IsPlayingNode
#define NPC_IsPlayingNode sampgdk_NPC_IsPlayingNode

#undef  NPC_IsPlayingNodePaused
#define NPC_IsPlayingNodePaused sampgdk_NPC_IsPlayingNodePaused

#undef  NPC_IsPlayingPlayback
#define NPC_IsPlayingPlayback sampgdk_NPC_IsPlayingPlayback

#undef  NPC_IsReloadEnabled
#define NPC_IsReloadEnabled sampgdk_NPC_IsReloadEnabled

#undef  NPC_IsReloading
#define NPC_IsReloading sampgdk_NPC_IsReloading

#undef  NPC_IsShooting
#define NPC_IsShooting sampgdk_NPC_IsShooting

#undef  NPC_IsSpawned
#define NPC_IsSpawned sampgdk_NPC_IsSpawned

#undef  NPC_IsStreamedIn
#define NPC_IsStreamedIn sampgdk_NPC_IsStreamedIn

#undef  NPC_IsValid
#define NPC_IsValid sampgdk_NPC_IsValid

#undef  NPC_IsValidPath
#define NPC_IsValidPath sampgdk_NPC_IsValidPath

#undef  NPC_IsValidRecord
#define NPC_IsValidRecord sampgdk_NPC_IsValidRecord

#undef  NPC_IsVehicleSirenUsed
#define NPC_IsVehicleSirenUsed sampgdk_NPC_IsVehicleSirenUsed

#undef  NPC_Kill
#define NPC_Kill sampgdk_NPC_Kill

#undef  NPC_LoadRecord
#define NPC_LoadRecord sampgdk_NPC_LoadRecord

#undef  NPC_MeleeAttack
#define NPC_MeleeAttack sampgdk_NPC_MeleeAttack

#undef  NPC_Move
#define NPC_Move sampgdk_NPC_Move

#undef  NPC_MoveByPath
#define NPC_MoveByPath sampgdk_NPC_MoveByPath

#undef  NPC_MoveToPlayer
#define NPC_MoveToPlayer sampgdk_NPC_MoveToPlayer

#undef  NPC_OpenNode
#define NPC_OpenNode sampgdk_NPC_OpenNode

#undef  NPC_PausePlayback
#define NPC_PausePlayback sampgdk_NPC_PausePlayback

#undef  NPC_PausePlayingNode
#define NPC_PausePlayingNode sampgdk_NPC_PausePlayingNode

#undef  NPC_PlayNode
#define NPC_PlayNode sampgdk_NPC_PlayNode

#undef  NPC_PutInVehicle
#define NPC_PutInVehicle sampgdk_NPC_PutInVehicle

#undef  NPC_RemoveFromVehicle
#define NPC_RemoveFromVehicle sampgdk_NPC_RemoveFromVehicle

#undef  NPC_RemovePointFromPath
#define NPC_RemovePointFromPath sampgdk_NPC_RemovePointFromPath

#undef  NPC_ResetAnimation
#define NPC_ResetAnimation sampgdk_NPC_ResetAnimation

#undef  NPC_ResetSurfingData
#define NPC_ResetSurfingData sampgdk_NPC_ResetSurfingData

#undef  NPC_Respawn
#define NPC_Respawn sampgdk_NPC_Respawn

#undef  NPC_ResumePlayingNode
#define NPC_ResumePlayingNode sampgdk_NPC_ResumePlayingNode

#undef  NPC_SetAmmo
#define NPC_SetAmmo sampgdk_NPC_SetAmmo

#undef  NPC_SetAmmoInClip
#define NPC_SetAmmoInClip sampgdk_NPC_SetAmmoInClip

#undef  NPC_SetAngleToPlayer
#define NPC_SetAngleToPlayer sampgdk_NPC_SetAngleToPlayer

#undef  NPC_SetAngleToPos
#define NPC_SetAngleToPos sampgdk_NPC_SetAngleToPos

#undef  NPC_SetAnimation
#define NPC_SetAnimation sampgdk_NPC_SetAnimation

#undef  NPC_SetArmour
#define NPC_SetArmour sampgdk_NPC_SetArmour

#undef  NPC_SetFacingAngle
#define NPC_SetFacingAngle sampgdk_NPC_SetFacingAngle

#undef  NPC_SetFightingStyle
#define NPC_SetFightingStyle sampgdk_NPC_SetFightingStyle

#undef  NPC_SetHealth
#define NPC_SetHealth sampgdk_NPC_SetHealth

#undef  NPC_SetInterior
#define NPC_SetInterior sampgdk_NPC_SetInterior

#undef  NPC_SetInvulnerable
#define NPC_SetInvulnerable sampgdk_NPC_SetInvulnerable

#undef  NPC_SetKeys
#define NPC_SetKeys sampgdk_NPC_SetKeys

#undef  NPC_SetNodePoint
#define NPC_SetNodePoint sampgdk_NPC_SetNodePoint

#undef  NPC_SetPos
#define NPC_SetPos sampgdk_NPC_SetPos

#undef  NPC_SetRot
#define NPC_SetRot sampgdk_NPC_SetRot

#undef  NPC_SetSkin
#define NPC_SetSkin sampgdk_NPC_SetSkin

#undef  NPC_SetSpecialAction
#define NPC_SetSpecialAction sampgdk_NPC_SetSpecialAction

#undef  NPC_SetSurfingObject
#define NPC_SetSurfingObject sampgdk_NPC_SetSurfingObject

#undef  NPC_SetSurfingOffsets
#define NPC_SetSurfingOffsets sampgdk_NPC_SetSurfingOffsets

#undef  NPC_SetSurfingPlayerObject
#define NPC_SetSurfingPlayerObject sampgdk_NPC_SetSurfingPlayerObject

#undef  NPC_SetSurfingVehicle
#define NPC_SetSurfingVehicle sampgdk_NPC_SetSurfingVehicle

#undef  NPC_SetVehicleGearState
#define NPC_SetVehicleGearState sampgdk_NPC_SetVehicleGearState

#undef  NPC_SetVehicleHealth
#define NPC_SetVehicleHealth sampgdk_NPC_SetVehicleHealth

#undef  NPC_SetVehicleHydraThrusters
#define NPC_SetVehicleHydraThrusters sampgdk_NPC_SetVehicleHydraThrusters

#undef  NPC_SetVehicleTrainSpeed
#define NPC_SetVehicleTrainSpeed sampgdk_NPC_SetVehicleTrainSpeed

#undef  NPC_SetVelocity
#define NPC_SetVelocity sampgdk_NPC_SetVelocity

#undef  NPC_SetVirtualWorld
#define NPC_SetVirtualWorld sampgdk_NPC_SetVirtualWorld

#undef  NPC_SetWeapon
#define NPC_SetWeapon sampgdk_NPC_SetWeapon

#undef  NPC_SetWeaponAccuracy
#define NPC_SetWeaponAccuracy sampgdk_NPC_SetWeaponAccuracy

#undef  NPC_SetWeaponClipSize
#define NPC_SetWeaponClipSize sampgdk_NPC_SetWeaponClipSize

#undef  NPC_SetWeaponReloadTime
#define NPC_SetWeaponReloadTime sampgdk_NPC_SetWeaponReloadTime

#undef  NPC_SetWeaponShootTime
#define NPC_SetWeaponShootTime sampgdk_NPC_SetWeaponShootTime

#undef  NPC_SetWeaponSkillLevel
#define NPC_SetWeaponSkillLevel sampgdk_NPC_SetWeaponSkillLevel

#undef  NPC_SetWeaponState
#define NPC_SetWeaponState sampgdk_NPC_SetWeaponState

#undef  NPC_Shoot
#define NPC_Shoot sampgdk_NPC_Shoot

#undef  NPC_Spawn
#define NPC_Spawn sampgdk_NPC_Spawn

#undef  NPC_StartPlayback
#define NPC_StartPlayback sampgdk_NPC_StartPlayback

#undef  NPC_StartPlaybackEx
#define NPC_StartPlaybackEx sampgdk_NPC_StartPlaybackEx

#undef  NPC_StopAim
#define NPC_StopAim sampgdk_NPC_StopAim

#undef  NPC_StopMeleeAttack
#define NPC_StopMeleeAttack sampgdk_NPC_StopMeleeAttack

#undef  NPC_StopMove
#define NPC_StopMove sampgdk_NPC_StopMove

#undef  NPC_StopPlayback
#define NPC_StopPlayback sampgdk_NPC_StopPlayback

#undef  NPC_StopPlayingNode
#define NPC_StopPlayingNode sampgdk_NPC_StopPlayingNode

#undef  NPC_UnloadAllRecords
#define NPC_UnloadAllRecords sampgdk_NPC_UnloadAllRecords

#undef  NPC_UnloadRecord
#define NPC_UnloadRecord sampgdk_NPC_UnloadRecord

#undef  NPC_UpdateNodePoint
#define NPC_UpdateNodePoint sampgdk_NPC_UpdateNodePoint

#undef  NPC_UseVehicleSiren
#define NPC_UseVehicleSiren sampgdk_NPC_UseVehicleSiren

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_NPC_H */

#ifndef SAMPGDK_OMP_OBJECT_H
#define SAMPGDK_OMP_OBJECT_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AttachPlayerObjectToObject">AttachPlayerObjectToObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AttachPlayerObjectToObject(int playerid, int objectid, int parentid, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ, bool syncRotation));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/BeginObjectEditing">BeginObjectEditing on open.mp</a>
 */
SAMPGDK_NATIVE(bool, BeginObjectEditing(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/BeginObjectSelecting">BeginObjectSelecting on open.mp</a>
 */
SAMPGDK_NATIVE(bool, BeginObjectSelecting(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/BeginPlayerObjectEditing">BeginPlayerObjectEditing on open.mp</a>
 */
SAMPGDK_NATIVE(bool, BeginPlayerObjectEditing(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/EndObjectEditing">EndObjectEditing on open.mp</a>
 */
SAMPGDK_NATIVE(bool, EndObjectEditing(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetCustomModelPath">GetCustomModelPath on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetCustomModelPath(int modelid, char * dffPath, int dffSize, char * txdPath, int txdSize));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectAttachedData">GetObjectAttachedData on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetObjectAttachedData(int objectid, int * parentVehicle, int * parentObject, int * parentPlayer));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectAttachedOffset">GetObjectAttachedOffset on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetObjectAttachedOffset(int objectid, float * offsetX, float * offsetY, float * offsetZ, float * rotationX, float * rotationY, float * rotationZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectDrawDistance">GetObjectDrawDistance on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetObjectDrawDistance(int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectMaterial">GetObjectMaterial on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetObjectMaterial(int objectid, int materialIndex, int * modelid, char * textureLibrary, int textureLibrarySize, char * textureName, int textureNameSize, int * materialColour));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectMaterialText">GetObjectMaterialText on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetObjectMaterialText(int objectid, int materialIndex, char * text, int textSize, int * materialSize, char * fontFace, int fontFaceSize, int * fontSize, bool * bold, int * fontColour, int * backgroundColour, int * textAlignment));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectMoveSpeed">GetObjectMoveSpeed on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetObjectMoveSpeed(int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectMovingTargetPos">GetObjectMovingTargetPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetObjectMovingTargetPos(int objectid, float * targetX, float * targetY, float * targetZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectMovingTargetRot">GetObjectMovingTargetRot on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetObjectMovingTargetRot(int objectid, float * rotationX, float * rotationY, float * rotationZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectSyncRotation">GetObjectSyncRotation on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetObjectSyncRotation(int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetObjectType">GetObjectType on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetObjectType(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerAttachedObject">GetPlayerAttachedObject on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerAttachedObject(int playerid, int index, int * modelid, int * bone, float * offsetX, float * offsetY, float * offsetZ, float * rotationX, float * rotationY, float * rotationZ, float * scaleX, float * scaleY, float * scaleZ, int * materialColour1, int * materialColour2));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCameraTargetPlayerObj">GetPlayerCameraTargetPlayerObj on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerCameraTargetPlayerObj(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCameraTargetPlayerObject">GetPlayerCameraTargetPlayerObject on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerCameraTargetPlayerObject(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerObjectAttachedData">GetPlayerObjectAttachedData on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerObjectAttachedData(int playerid, int objectid, int * parentVehicle, int * parentObject, int * parentPlayer));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerObjectAttachedOffset">GetPlayerObjectAttachedOffset on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerObjectAttachedOffset(int playerid, int objectid, float * offsetX, float * offsetY, float * offsetZ, float * rotationX, float * rotationY, float * rotationZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerObjectDrawDistance">GetPlayerObjectDrawDistance on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetPlayerObjectDrawDistance(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerObjectMaterial">GetPlayerObjectMaterial on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerObjectMaterial(int playerid, int objectid, int materialIndex, int * modelid, char * textureLibrary, int textureLibrarySize, char * textureName, int textureNameSize, int * materialColour));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerObjectMaterialText">GetPlayerObjectMaterialText on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerObjectMaterialText(int playerid, int objectid, int materialIndex, char * text, int textSize, int * materialSize, char * fontFace, int fontFaceSize, int * fontSize, bool * bold, int * fontColour, int * backgroundColour, int * textAlignment));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerObjectMoveSpeed">GetPlayerObjectMoveSpeed on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetPlayerObjectMoveSpeed(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerObjectMovingTargetPos">GetPlayerObjectMovingTargetPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerObjectMovingTargetPos(int playerid, int objectid, float * targetX, float * targetY, float * targetZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerObjectMovingTargetRot">GetPlayerObjectMovingTargetRot on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerObjectMovingTargetRot(int playerid, int objectid, float * rotationX, float * rotationY, float * rotationZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerObjectSyncRotation">GetPlayerObjectSyncRotation on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerObjectSyncRotation(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerSurfingPlayerObjectID">GetPlayerSurfingPlayerObjectID on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerSurfingPlayerObjectID(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsObjectMaterialSlotUsed">IsObjectMaterialSlotUsed on open.mp</a>
 */
SAMPGDK_NATIVE(int, IsObjectMaterialSlotUsed(int objectid, int materialIndex));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsObjectNoCameraCol">IsObjectNoCameraCol on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsObjectNoCameraCol(int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerObjectMaterialSlotUsed">IsPlayerObjectMaterialSlotUsed on open.mp</a>
 */
SAMPGDK_NATIVE(int, IsPlayerObjectMaterialSlotUsed(int playerid, int objectid, int materialIndex));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerObjectNoCameraCol">IsPlayerObjectNoCameraCol on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerObjectNoCameraCol(int playerid, int objectid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidCustomModel">IsValidCustomModel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidCustomModel(int modelid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetObjectMoveSpeed">SetObjectMoveSpeed on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetObjectMoveSpeed(int objectid, float speed));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetObjectsDefaultCameraCollision">SetObjectsDefaultCameraCollision on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetObjectsDefaultCameraCollision(bool disable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerObjectMoveSpeed">SetPlayerObjectMoveSpeed on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerObjectMoveSpeed(int playerid, int objectid, float speed));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerObjectNoCameraCollision">SetPlayerObjectNoCameraCollision on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerObjectNoCameraCollision(int playerid, int objectid));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool AttachPlayerObjectToObject(int playerid, int objectid, int parentid, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ, bool syncRotation = true) {
  return sampgdk_AttachPlayerObjectToObject(playerid, objectid, parentid, offsetX, offsetY, offsetZ, rotationX, rotationY, rotationZ, syncRotation);
}

inline bool BeginObjectEditing(int playerid, int objectid) {
  return sampgdk_BeginObjectEditing(playerid, objectid);
}

inline bool BeginObjectSelecting(int playerid) {
  return sampgdk_BeginObjectSelecting(playerid);
}

inline bool BeginPlayerObjectEditing(int playerid, int objectid) {
  return sampgdk_BeginPlayerObjectEditing(playerid, objectid);
}

inline bool EndObjectEditing(int playerid) {
  return sampgdk_EndObjectEditing(playerid);
}

inline bool GetCustomModelPath(int modelid, char * dffPath, int dffSize, char * txdPath, int txdSize) {
  return sampgdk_GetCustomModelPath(modelid, dffPath, dffSize, txdPath, txdSize);
}

inline bool GetObjectAttachedData(int objectid, int * parentVehicle, int * parentObject, int * parentPlayer) {
  return sampgdk_GetObjectAttachedData(objectid, parentVehicle, parentObject, parentPlayer);
}

inline bool GetObjectAttachedOffset(int objectid, float * offsetX, float * offsetY, float * offsetZ, float * rotationX, float * rotationY, float * rotationZ) {
  return sampgdk_GetObjectAttachedOffset(objectid, offsetX, offsetY, offsetZ, rotationX, rotationY, rotationZ);
}

inline float GetObjectDrawDistance(int objectid) {
  return sampgdk_GetObjectDrawDistance(objectid);
}

inline bool GetObjectMaterial(int objectid, int materialIndex, int * modelid, char * textureLibrary, int textureLibrarySize, char * textureName, int textureNameSize, int * materialColour) {
  return sampgdk_GetObjectMaterial(objectid, materialIndex, modelid, textureLibrary, textureLibrarySize, textureName, textureNameSize, materialColour);
}

inline bool GetObjectMaterialText(int objectid, int materialIndex, char * text, int textSize, int * materialSize, char * fontFace, int fontFaceSize, int * fontSize, bool * bold, int * fontColour, int * backgroundColour, int * textAlignment) {
  return sampgdk_GetObjectMaterialText(objectid, materialIndex, text, textSize, materialSize, fontFace, fontFaceSize, fontSize, bold, fontColour, backgroundColour, textAlignment);
}

inline float GetObjectMoveSpeed(int objectid) {
  return sampgdk_GetObjectMoveSpeed(objectid);
}

inline bool GetObjectMovingTargetPos(int objectid, float * targetX, float * targetY, float * targetZ) {
  return sampgdk_GetObjectMovingTargetPos(objectid, targetX, targetY, targetZ);
}

inline bool GetObjectMovingTargetRot(int objectid, float * rotationX, float * rotationY, float * rotationZ) {
  return sampgdk_GetObjectMovingTargetRot(objectid, rotationX, rotationY, rotationZ);
}

inline bool GetObjectSyncRotation(int objectid) {
  return sampgdk_GetObjectSyncRotation(objectid);
}

inline int GetObjectType(int playerid, int objectid) {
  return sampgdk_GetObjectType(playerid, objectid);
}

inline bool GetPlayerAttachedObject(int playerid, int index, int * modelid, int * bone, float * offsetX, float * offsetY, float * offsetZ, float * rotationX, float * rotationY, float * rotationZ, float * scaleX, float * scaleY, float * scaleZ, int * materialColour1, int * materialColour2) {
  return sampgdk_GetPlayerAttachedObject(playerid, index, modelid, bone, offsetX, offsetY, offsetZ, rotationX, rotationY, rotationZ, scaleX, scaleY, scaleZ, materialColour1, materialColour2);
}

inline int GetPlayerCameraTargetPlayerObj(int playerid) {
  return sampgdk_GetPlayerCameraTargetPlayerObj(playerid);
}

inline int GetPlayerCameraTargetPlayerObject(int playerid) {
  return sampgdk_GetPlayerCameraTargetPlayerObject(playerid);
}

inline bool GetPlayerObjectAttachedData(int playerid, int objectid, int * parentVehicle, int * parentObject, int * parentPlayer) {
  return sampgdk_GetPlayerObjectAttachedData(playerid, objectid, parentVehicle, parentObject, parentPlayer);
}

inline bool GetPlayerObjectAttachedOffset(int playerid, int objectid, float * offsetX, float * offsetY, float * offsetZ, float * rotationX, float * rotationY, float * rotationZ) {
  return sampgdk_GetPlayerObjectAttachedOffset(playerid, objectid, offsetX, offsetY, offsetZ, rotationX, rotationY, rotationZ);
}

inline float GetPlayerObjectDrawDistance(int playerid, int objectid) {
  return sampgdk_GetPlayerObjectDrawDistance(playerid, objectid);
}

inline bool GetPlayerObjectMaterial(int playerid, int objectid, int materialIndex, int * modelid, char * textureLibrary, int textureLibrarySize, char * textureName, int textureNameSize, int * materialColour) {
  return sampgdk_GetPlayerObjectMaterial(playerid, objectid, materialIndex, modelid, textureLibrary, textureLibrarySize, textureName, textureNameSize, materialColour);
}

inline bool GetPlayerObjectMaterialText(int playerid, int objectid, int materialIndex, char * text, int textSize, int * materialSize, char * fontFace, int fontFaceSize, int * fontSize, bool * bold, int * fontColour, int * backgroundColour, int * textAlignment) {
  return sampgdk_GetPlayerObjectMaterialText(playerid, objectid, materialIndex, text, textSize, materialSize, fontFace, fontFaceSize, fontSize, bold, fontColour, backgroundColour, textAlignment);
}

inline float GetPlayerObjectMoveSpeed(int playerid, int objectid) {
  return sampgdk_GetPlayerObjectMoveSpeed(playerid, objectid);
}

inline bool GetPlayerObjectMovingTargetPos(int playerid, int objectid, float * targetX, float * targetY, float * targetZ) {
  return sampgdk_GetPlayerObjectMovingTargetPos(playerid, objectid, targetX, targetY, targetZ);
}

inline bool GetPlayerObjectMovingTargetRot(int playerid, int objectid, float * rotationX, float * rotationY, float * rotationZ) {
  return sampgdk_GetPlayerObjectMovingTargetRot(playerid, objectid, rotationX, rotationY, rotationZ);
}

inline bool GetPlayerObjectSyncRotation(int playerid, int objectid) {
  return sampgdk_GetPlayerObjectSyncRotation(playerid, objectid);
}

inline int GetPlayerSurfingPlayerObjectID(int playerid) {
  return sampgdk_GetPlayerSurfingPlayerObjectID(playerid);
}

inline int IsObjectMaterialSlotUsed(int objectid, int materialIndex) {
  return sampgdk_IsObjectMaterialSlotUsed(objectid, materialIndex);
}

inline bool IsObjectNoCameraCol(int objectid) {
  return sampgdk_IsObjectNoCameraCol(objectid);
}

inline int IsPlayerObjectMaterialSlotUsed(int playerid, int objectid, int materialIndex) {
  return sampgdk_IsPlayerObjectMaterialSlotUsed(playerid, objectid, materialIndex);
}

inline bool IsPlayerObjectNoCameraCol(int playerid, int objectid) {
  return sampgdk_IsPlayerObjectNoCameraCol(playerid, objectid);
}

inline bool IsValidCustomModel(int modelid) {
  return sampgdk_IsValidCustomModel(modelid);
}

inline bool SetObjectMoveSpeed(int objectid, float speed) {
  return sampgdk_SetObjectMoveSpeed(objectid, speed);
}

inline bool SetObjectsDefaultCameraCollision(bool disable) {
  return sampgdk_SetObjectsDefaultCameraCollision(disable);
}

inline bool SetPlayerObjectMoveSpeed(int playerid, int objectid, float speed) {
  return sampgdk_SetPlayerObjectMoveSpeed(playerid, objectid, speed);
}

inline bool SetPlayerObjectNoCameraCollision(int playerid, int objectid) {
  return sampgdk_SetPlayerObjectNoCameraCollision(playerid, objectid);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  AttachPlayerObjectToObject
#define AttachPlayerObjectToObject sampgdk_AttachPlayerObjectToObject

#undef  BeginObjectEditing
#define BeginObjectEditing sampgdk_BeginObjectEditing

#undef  BeginObjectSelecting
#define BeginObjectSelecting sampgdk_BeginObjectSelecting

#undef  BeginPlayerObjectEditing
#define BeginPlayerObjectEditing sampgdk_BeginPlayerObjectEditing

#undef  EndObjectEditing
#define EndObjectEditing sampgdk_EndObjectEditing

#undef  GetCustomModelPath
#define GetCustomModelPath sampgdk_GetCustomModelPath

#undef  GetObjectAttachedData
#define GetObjectAttachedData sampgdk_GetObjectAttachedData

#undef  GetObjectAttachedOffset
#define GetObjectAttachedOffset sampgdk_GetObjectAttachedOffset

#undef  GetObjectDrawDistance
#define GetObjectDrawDistance sampgdk_GetObjectDrawDistance

#undef  GetObjectMaterial
#define GetObjectMaterial sampgdk_GetObjectMaterial

#undef  GetObjectMaterialText
#define GetObjectMaterialText sampgdk_GetObjectMaterialText

#undef  GetObjectMoveSpeed
#define GetObjectMoveSpeed sampgdk_GetObjectMoveSpeed

#undef  GetObjectMovingTargetPos
#define GetObjectMovingTargetPos sampgdk_GetObjectMovingTargetPos

#undef  GetObjectMovingTargetRot
#define GetObjectMovingTargetRot sampgdk_GetObjectMovingTargetRot

#undef  GetObjectSyncRotation
#define GetObjectSyncRotation sampgdk_GetObjectSyncRotation

#undef  GetObjectType

#undef  GetPlayerAttachedObject
#define GetPlayerAttachedObject sampgdk_GetPlayerAttachedObject

#undef  GetPlayerCameraTargetPlayerObj
#define GetPlayerCameraTargetPlayerObj sampgdk_GetPlayerCameraTargetPlayerObj

#undef  GetPlayerCameraTargetPlayerObject
#define GetPlayerCameraTargetPlayerObject sampgdk_GetPlayerCameraTargetPlayerObject

#undef  GetPlayerObjectAttachedData
#define GetPlayerObjectAttachedData sampgdk_GetPlayerObjectAttachedData

#undef  GetPlayerObjectAttachedOffset
#define GetPlayerObjectAttachedOffset sampgdk_GetPlayerObjectAttachedOffset

#undef  GetPlayerObjectDrawDistance
#define GetPlayerObjectDrawDistance sampgdk_GetPlayerObjectDrawDistance

#undef  GetPlayerObjectMaterial
#define GetPlayerObjectMaterial sampgdk_GetPlayerObjectMaterial

#undef  GetPlayerObjectMaterialText
#define GetPlayerObjectMaterialText sampgdk_GetPlayerObjectMaterialText

#undef  GetPlayerObjectMoveSpeed
#define GetPlayerObjectMoveSpeed sampgdk_GetPlayerObjectMoveSpeed

#undef  GetPlayerObjectMovingTargetPos
#define GetPlayerObjectMovingTargetPos sampgdk_GetPlayerObjectMovingTargetPos

#undef  GetPlayerObjectMovingTargetRot
#define GetPlayerObjectMovingTargetRot sampgdk_GetPlayerObjectMovingTargetRot

#undef  GetPlayerObjectSyncRotation
#define GetPlayerObjectSyncRotation sampgdk_GetPlayerObjectSyncRotation

#undef  GetPlayerSurfingPlayerObjectID
#define GetPlayerSurfingPlayerObjectID sampgdk_GetPlayerSurfingPlayerObjectID

#undef  IsObjectMaterialSlotUsed
#define IsObjectMaterialSlotUsed sampgdk_IsObjectMaterialSlotUsed

#undef  IsObjectNoCameraCol
#define IsObjectNoCameraCol sampgdk_IsObjectNoCameraCol

#undef  IsPlayerObjectMaterialSlotUsed
#define IsPlayerObjectMaterialSlotUsed sampgdk_IsPlayerObjectMaterialSlotUsed

#undef  IsPlayerObjectNoCameraCol
#define IsPlayerObjectNoCameraCol sampgdk_IsPlayerObjectNoCameraCol

#undef  IsValidCustomModel
#define IsValidCustomModel sampgdk_IsValidCustomModel

#undef  SetObjectMoveSpeed
#define SetObjectMoveSpeed sampgdk_SetObjectMoveSpeed

#undef  SetObjectsDefaultCameraCollision
#define SetObjectsDefaultCameraCollision sampgdk_SetObjectsDefaultCameraCollision

#undef  SetPlayerObjectMoveSpeed
#define SetPlayerObjectMoveSpeed sampgdk_SetPlayerObjectMoveSpeed

#undef  SetPlayerObjectNoCameraCollision
#define SetPlayerObjectNoCameraCollision sampgdk_SetPlayerObjectNoCameraCollision

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_OBJECT_H */

#ifndef SAMPGDK_OMP_PICKUP_H
#define SAMPGDK_OMP_PICKUP_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CreatePlayerPickup">CreatePlayerPickup on open.mp</a>
 */
SAMPGDK_NATIVE(int, CreatePlayerPickup(int playerid, int model, int type, float x, float y, float z, int virtualWorld));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/DestroyPlayerPickup">DestroyPlayerPickup on open.mp</a>
 */
SAMPGDK_NATIVE(bool, DestroyPlayerPickup(int playerid, int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPickupModel">GetPickupModel on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPickupModel(int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPickupPos">GetPickupPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPickupPos(int pickupid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPickupType">GetPickupType on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPickupType(int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPickupVirtualWorld">GetPickupVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPickupVirtualWorld(int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerPickupModel">GetPlayerPickupModel on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerPickupModel(int playerid, int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerPickupPos">GetPlayerPickupPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerPickupPos(int playerid, int pickupid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerPickupType">GetPlayerPickupType on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerPickupType(int playerid, int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerPickupVirtualWorld">GetPlayerPickupVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerPickupVirtualWorld(int playerid, int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/HidePickupForPlayer">HidePickupForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, HidePickupForPlayer(int playerid, int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPickupHiddenForPlayer">IsPickupHiddenForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPickupHiddenForPlayer(int playerid, int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPickupStreamedIn">IsPickupStreamedIn on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPickupStreamedIn(int playerid, int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerPickupStreamedIn">IsPlayerPickupStreamedIn on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerPickupStreamedIn(int playerid, int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidPickup">IsValidPickup on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidPickup(int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidPlayerPickup">IsValidPlayerPickup on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidPlayerPickup(int playerid, int pickupid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPickupModel">SetPickupModel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPickupModel(int pickupid, int model, bool update));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPickupPos">SetPickupPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPickupPos(int pickupid, float x, float y, float z, bool update));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPickupType">SetPickupType on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPickupType(int pickupid, int type, bool update));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPickupVirtualWorld">SetPickupVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPickupVirtualWorld(int pickupid, int virtualWorld));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerPickupModel">SetPlayerPickupModel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerPickupModel(int playerid, int pickupid, int model, bool update));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerPickupPos">SetPlayerPickupPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerPickupPos(int playerid, int pickupid, float x, float y, float z, bool update));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerPickupType">SetPlayerPickupType on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerPickupType(int playerid, int pickupid, int type, bool update));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerPickupVirtualWorld">SetPlayerPickupVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerPickupVirtualWorld(int playerid, int pickupid, int virtualWorld));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ShowPickupForPlayer">ShowPickupForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ShowPickupForPlayer(int playerid, int pickupid));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline int CreatePlayerPickup(int playerid, int model, int type, float x, float y, float z, int virtualWorld = 0) {
  return sampgdk_CreatePlayerPickup(playerid, model, type, x, y, z, virtualWorld);
}

inline bool DestroyPlayerPickup(int playerid, int pickupid) {
  return sampgdk_DestroyPlayerPickup(playerid, pickupid);
}

inline int GetPickupModel(int pickupid) {
  return sampgdk_GetPickupModel(pickupid);
}

inline bool GetPickupPos(int pickupid, float * x, float * y, float * z) {
  return sampgdk_GetPickupPos(pickupid, x, y, z);
}

inline int GetPickupType(int pickupid) {
  return sampgdk_GetPickupType(pickupid);
}

inline int GetPickupVirtualWorld(int pickupid) {
  return sampgdk_GetPickupVirtualWorld(pickupid);
}

inline int GetPlayerPickupModel(int playerid, int pickupid) {
  return sampgdk_GetPlayerPickupModel(playerid, pickupid);
}

inline bool GetPlayerPickupPos(int playerid, int pickupid, float * x, float * y, float * z) {
  return sampgdk_GetPlayerPickupPos(playerid, pickupid, x, y, z);
}

inline int GetPlayerPickupType(int playerid, int pickupid) {
  return sampgdk_GetPlayerPickupType(playerid, pickupid);
}

inline int GetPlayerPickupVirtualWorld(int playerid, int pickupid) {
  return sampgdk_GetPlayerPickupVirtualWorld(playerid, pickupid);
}

inline bool HidePickupForPlayer(int playerid, int pickupid) {
  return sampgdk_HidePickupForPlayer(playerid, pickupid);
}

inline bool IsPickupHiddenForPlayer(int playerid, int pickupid) {
  return sampgdk_IsPickupHiddenForPlayer(playerid, pickupid);
}

inline bool IsPickupStreamedIn(int playerid, int pickupid) {
  return sampgdk_IsPickupStreamedIn(playerid, pickupid);
}

inline bool IsPlayerPickupStreamedIn(int playerid, int pickupid) {
  return sampgdk_IsPlayerPickupStreamedIn(playerid, pickupid);
}

inline bool IsValidPickup(int pickupid) {
  return sampgdk_IsValidPickup(pickupid);
}

inline bool IsValidPlayerPickup(int playerid, int pickupid) {
  return sampgdk_IsValidPlayerPickup(playerid, pickupid);
}

inline bool SetPickupModel(int pickupid, int model, bool update = true) {
  return sampgdk_SetPickupModel(pickupid, model, update);
}

inline bool SetPickupPos(int pickupid, float x, float y, float z, bool update = true) {
  return sampgdk_SetPickupPos(pickupid, x, y, z, update);
}

inline bool SetPickupType(int pickupid, int type, bool update = true) {
  return sampgdk_SetPickupType(pickupid, type, update);
}

inline bool SetPickupVirtualWorld(int pickupid, int virtualWorld) {
  return sampgdk_SetPickupVirtualWorld(pickupid, virtualWorld);
}

inline bool SetPlayerPickupModel(int playerid, int pickupid, int model, bool update = true) {
  return sampgdk_SetPlayerPickupModel(playerid, pickupid, model, update);
}

inline bool SetPlayerPickupPos(int playerid, int pickupid, float x, float y, float z, bool update = true) {
  return sampgdk_SetPlayerPickupPos(playerid, pickupid, x, y, z, update);
}

inline bool SetPlayerPickupType(int playerid, int pickupid, int type, bool update = true) {
  return sampgdk_SetPlayerPickupType(playerid, pickupid, type, update);
}

inline bool SetPlayerPickupVirtualWorld(int playerid, int pickupid, int virtualWorld) {
  return sampgdk_SetPlayerPickupVirtualWorld(playerid, pickupid, virtualWorld);
}

inline bool ShowPickupForPlayer(int playerid, int pickupid) {
  return sampgdk_ShowPickupForPlayer(playerid, pickupid);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  CreatePlayerPickup
#define CreatePlayerPickup sampgdk_CreatePlayerPickup

#undef  DestroyPlayerPickup
#define DestroyPlayerPickup sampgdk_DestroyPlayerPickup

#undef  GetPickupModel
#define GetPickupModel sampgdk_GetPickupModel

#undef  GetPickupPos
#define GetPickupPos sampgdk_GetPickupPos

#undef  GetPickupType
#define GetPickupType sampgdk_GetPickupType

#undef  GetPickupVirtualWorld
#define GetPickupVirtualWorld sampgdk_GetPickupVirtualWorld

#undef  GetPlayerPickupModel
#define GetPlayerPickupModel sampgdk_GetPlayerPickupModel

#undef  GetPlayerPickupPos
#define GetPlayerPickupPos sampgdk_GetPlayerPickupPos

#undef  GetPlayerPickupType
#define GetPlayerPickupType sampgdk_GetPlayerPickupType

#undef  GetPlayerPickupVirtualWorld
#define GetPlayerPickupVirtualWorld sampgdk_GetPlayerPickupVirtualWorld

#undef  HidePickupForPlayer
#define HidePickupForPlayer sampgdk_HidePickupForPlayer

#undef  IsPickupHiddenForPlayer
#define IsPickupHiddenForPlayer sampgdk_IsPickupHiddenForPlayer

#undef  IsPickupStreamedIn
#define IsPickupStreamedIn sampgdk_IsPickupStreamedIn

#undef  IsPlayerPickupStreamedIn
#define IsPlayerPickupStreamedIn sampgdk_IsPlayerPickupStreamedIn

#undef  IsValidPickup
#define IsValidPickup sampgdk_IsValidPickup

#undef  IsValidPlayerPickup
#define IsValidPlayerPickup sampgdk_IsValidPlayerPickup

#undef  SetPickupModel
#define SetPickupModel sampgdk_SetPickupModel

#undef  SetPickupPos
#define SetPickupPos sampgdk_SetPickupPos

#undef  SetPickupType
#define SetPickupType sampgdk_SetPickupType

#undef  SetPickupVirtualWorld
#define SetPickupVirtualWorld sampgdk_SetPickupVirtualWorld

#undef  SetPlayerPickupModel
#define SetPlayerPickupModel sampgdk_SetPlayerPickupModel

#undef  SetPlayerPickupPos
#define SetPlayerPickupPos sampgdk_SetPlayerPickupPos

#undef  SetPlayerPickupType
#define SetPlayerPickupType sampgdk_SetPlayerPickupType

#undef  SetPlayerPickupVirtualWorld
#define SetPlayerPickupVirtualWorld sampgdk_SetPlayerPickupVirtualWorld

#undef  ShowPickupForPlayer
#define ShowPickupForPlayer sampgdk_ShowPickupForPlayer

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_PICKUP_H */

#ifndef SAMPGDK_OMP_PLAYER_H
#define SAMPGDK_OMP_PLAYER_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/AllowPlayerWeapons">AllowPlayerWeapons on open.mp</a>
 */
SAMPGDK_NATIVE(bool, AllowPlayerWeapons(int playerid, bool allow));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ArePlayerWeaponsAllowed">ArePlayerWeaponsAllowed on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ArePlayerWeaponsAllowed(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ClearPlayerWorldBounds">ClearPlayerWorldBounds on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ClearPlayerWorldBounds(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetDefaultPlayerColour">GetDefaultPlayerColour on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetDefaultPlayerColour(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetGameText">GetGameText on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetGameText(int playerid, int style, char * message, int len, int time, int remaining));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerAnimFlags">GetPlayerAnimFlags on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerAnimFlags(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerBuildingsRemoved">GetPlayerBuildingsRemoved on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerBuildingsRemoved(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerCustomSkin">GetPlayerCustomSkin on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerCustomSkin(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerGhostMode">GetPlayerGhostMode on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerGhostMode(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerGravity">GetPlayerGravity on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetPlayerGravity(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerMarkerForPlayer">GetPlayerMarkerForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerMarkerForPlayer(int playerid, int targetid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerRotationQuat">GetPlayerRotationQuat on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerRotationQuat(int playerid, float * w, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerSkillLevel">GetPlayerSkillLevel on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerSkillLevel(int playerid, int skill));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerSpectateID">GetPlayerSpectateID on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerSpectateID(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerSpectateType">GetPlayerSpectateType on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerSpectateType(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerSurfingOffsets">GetPlayerSurfingOffsets on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerSurfingOffsets(int playerid, float * offsetX, float * offsetY, float * offsetZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerWeather">GetPlayerWeather on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerWeather(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerWorldBounds">GetPlayerWorldBounds on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerWorldBounds(int playerid, float * maxX, float * minX, float * maxY, float * minY));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerZAim">GetPlayerZAim on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetPlayerZAim(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayers">GetPlayers on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayers(char * players, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/HasGameText">HasGameText on open.mp</a>
 */
SAMPGDK_NATIVE(bool, HasGameText(int playerid, int style));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/HideGameTextForPlayer">HideGameTextForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, HideGameTextForPlayer(int playerid, int style));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerCameraTargetEnabled">IsPlayerCameraTargetEnabled on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerCameraTargetEnabled(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerControllable">IsPlayerControllable on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerControllable(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerCuffed">IsPlayerCuffed on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerCuffed(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerInDriveByMode">IsPlayerInDriveByMode on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerInDriveByMode(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerSpawned">IsPlayerSpawned on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerSpawned(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerTeleportAllowed">IsPlayerTeleportAllowed on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerTeleportAllowed(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerUsingOfficialClient">IsPlayerUsingOfficialClient on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerUsingOfficialClient(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerUsingOmp">IsPlayerUsingOmp on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerUsingOmp(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerWidescreenToggled">IsPlayerWidescreenToggled on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerWidescreenToggled(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerHasClockEnabled">PlayerHasClockEnabled on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerHasClockEnabled(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/RemovePlayerWeapon">RemovePlayerWeapon on open.mp</a>
 */
SAMPGDK_NATIVE(bool, RemovePlayerWeapon(int playerid, int weaponid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayerGravity">SetPlayerGravity on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayerGravity(int playerid, float gravity));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TogglePlayerGhostMode">TogglePlayerGhostMode on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TogglePlayerGhostMode(int playerid, bool enable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TogglePlayerWidescreen">TogglePlayerWidescreen on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TogglePlayerWidescreen(int playerid, bool wide));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool AllowPlayerWeapons(int playerid, bool allow) {
  return sampgdk_AllowPlayerWeapons(playerid, allow);
}

inline bool ArePlayerWeaponsAllowed(int playerid) {
  return sampgdk_ArePlayerWeaponsAllowed(playerid);
}

inline bool ClearPlayerWorldBounds(int playerid) {
  return sampgdk_ClearPlayerWorldBounds(playerid);
}

inline int GetDefaultPlayerColour(int playerid) {
  return sampgdk_GetDefaultPlayerColour(playerid);
}

inline bool GetGameText(int playerid, int style, char * message, int len, int time, int remaining) {
  return sampgdk_GetGameText(playerid, style, message, len, time, remaining);
}

inline int GetPlayerAnimFlags(int playerid) {
  return sampgdk_GetPlayerAnimFlags(playerid);
}

inline int GetPlayerBuildingsRemoved(int playerid) {
  return sampgdk_GetPlayerBuildingsRemoved(playerid);
}

inline int GetPlayerCustomSkin(int playerid) {
  return sampgdk_GetPlayerCustomSkin(playerid);
}

inline bool GetPlayerGhostMode(int playerid) {
  return sampgdk_GetPlayerGhostMode(playerid);
}

inline float GetPlayerGravity(int playerid) {
  return sampgdk_GetPlayerGravity(playerid);
}

inline int GetPlayerMarkerForPlayer(int playerid, int targetid) {
  return sampgdk_GetPlayerMarkerForPlayer(playerid, targetid);
}

inline bool GetPlayerRotationQuat(int playerid, float * w, float * x, float * y, float * z) {
  return sampgdk_GetPlayerRotationQuat(playerid, w, x, y, z);
}

inline int GetPlayerSkillLevel(int playerid, int skill) {
  return sampgdk_GetPlayerSkillLevel(playerid, skill);
}

inline int GetPlayerSpectateID(int playerid) {
  return sampgdk_GetPlayerSpectateID(playerid);
}

inline int GetPlayerSpectateType(int playerid) {
  return sampgdk_GetPlayerSpectateType(playerid);
}

inline bool GetPlayerSurfingOffsets(int playerid, float * offsetX, float * offsetY, float * offsetZ) {
  return sampgdk_GetPlayerSurfingOffsets(playerid, offsetX, offsetY, offsetZ);
}

inline int GetPlayerWeather(int playerid) {
  return sampgdk_GetPlayerWeather(playerid);
}

inline bool GetPlayerWorldBounds(int playerid, float * maxX, float * minX, float * maxY, float * minY) {
  return sampgdk_GetPlayerWorldBounds(playerid, maxX, minX, maxY, minY);
}

inline float GetPlayerZAim(int playerid) {
  return sampgdk_GetPlayerZAim(playerid);
}

inline int GetPlayers(char * players, int size) {
  return sampgdk_GetPlayers(players, size);
}

inline bool HasGameText(int playerid, int style) {
  return sampgdk_HasGameText(playerid, style);
}

inline bool HideGameTextForPlayer(int playerid, int style) {
  return sampgdk_HideGameTextForPlayer(playerid, style);
}

inline bool IsPlayerCameraTargetEnabled(int playerid) {
  return sampgdk_IsPlayerCameraTargetEnabled(playerid);
}

inline bool IsPlayerControllable(int playerid) {
  return sampgdk_IsPlayerControllable(playerid);
}

inline bool IsPlayerCuffed(int playerid) {
  return sampgdk_IsPlayerCuffed(playerid);
}

inline bool IsPlayerInDriveByMode(int playerid) {
  return sampgdk_IsPlayerInDriveByMode(playerid);
}

inline bool IsPlayerSpawned(int playerid) {
  return sampgdk_IsPlayerSpawned(playerid);
}

inline bool IsPlayerTeleportAllowed(int playerid) {
  return sampgdk_IsPlayerTeleportAllowed(playerid);
}

inline bool IsPlayerUsingOfficialClient(int playerid) {
  return sampgdk_IsPlayerUsingOfficialClient(playerid);
}

inline bool IsPlayerUsingOmp(int playerid) {
  return sampgdk_IsPlayerUsingOmp(playerid);
}

inline bool IsPlayerWidescreenToggled(int playerid) {
  return sampgdk_IsPlayerWidescreenToggled(playerid);
}

inline bool PlayerHasClockEnabled(int playerid) {
  return sampgdk_PlayerHasClockEnabled(playerid);
}

inline bool RemovePlayerWeapon(int playerid, int weaponid) {
  return sampgdk_RemovePlayerWeapon(playerid, weaponid);
}

inline bool SetPlayerGravity(int playerid, float gravity) {
  return sampgdk_SetPlayerGravity(playerid, gravity);
}

inline bool TogglePlayerGhostMode(int playerid, bool enable) {
  return sampgdk_TogglePlayerGhostMode(playerid, enable);
}

inline bool TogglePlayerWidescreen(int playerid, bool wide) {
  return sampgdk_TogglePlayerWidescreen(playerid, wide);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  AllowPlayerWeapons
#define AllowPlayerWeapons sampgdk_AllowPlayerWeapons

#undef  ArePlayerWeaponsAllowed
#define ArePlayerWeaponsAllowed sampgdk_ArePlayerWeaponsAllowed

#undef  ClearPlayerWorldBounds
#define ClearPlayerWorldBounds sampgdk_ClearPlayerWorldBounds

#undef  GetDefaultPlayerColour
#define GetDefaultPlayerColour sampgdk_GetDefaultPlayerColour

#undef  GetGameText
#define GetGameText sampgdk_GetGameText

#undef  GetPlayerAnimFlags
#define GetPlayerAnimFlags sampgdk_GetPlayerAnimFlags

#undef  GetPlayerBuildingsRemoved
#define GetPlayerBuildingsRemoved sampgdk_GetPlayerBuildingsRemoved

#undef  GetPlayerCustomSkin
#define GetPlayerCustomSkin sampgdk_GetPlayerCustomSkin

#undef  GetPlayerGhostMode
#define GetPlayerGhostMode sampgdk_GetPlayerGhostMode

#undef  GetPlayerGravity
#define GetPlayerGravity sampgdk_GetPlayerGravity

#undef  GetPlayerMarkerForPlayer
#define GetPlayerMarkerForPlayer sampgdk_GetPlayerMarkerForPlayer

#undef  GetPlayerRotationQuat
#define GetPlayerRotationQuat sampgdk_GetPlayerRotationQuat

#undef  GetPlayerSkillLevel
#define GetPlayerSkillLevel sampgdk_GetPlayerSkillLevel

#undef  GetPlayerSpectateID
#define GetPlayerSpectateID sampgdk_GetPlayerSpectateID

#undef  GetPlayerSpectateType
#define GetPlayerSpectateType sampgdk_GetPlayerSpectateType

#undef  GetPlayerSurfingOffsets
#define GetPlayerSurfingOffsets sampgdk_GetPlayerSurfingOffsets

#undef  GetPlayerWeather
#define GetPlayerWeather sampgdk_GetPlayerWeather

#undef  GetPlayerWorldBounds
#define GetPlayerWorldBounds sampgdk_GetPlayerWorldBounds

#undef  GetPlayerZAim
#define GetPlayerZAim sampgdk_GetPlayerZAim

#undef  GetPlayers
#define GetPlayers sampgdk_GetPlayers

#undef  HasGameText
#define HasGameText sampgdk_HasGameText

#undef  HideGameTextForPlayer
#define HideGameTextForPlayer sampgdk_HideGameTextForPlayer

#undef  IsPlayerCameraTargetEnabled
#define IsPlayerCameraTargetEnabled sampgdk_IsPlayerCameraTargetEnabled

#undef  IsPlayerControllable
#define IsPlayerControllable sampgdk_IsPlayerControllable

#undef  IsPlayerCuffed
#define IsPlayerCuffed sampgdk_IsPlayerCuffed

#undef  IsPlayerInDriveByMode
#define IsPlayerInDriveByMode sampgdk_IsPlayerInDriveByMode

#undef  IsPlayerSpawned
#define IsPlayerSpawned sampgdk_IsPlayerSpawned

#undef  IsPlayerTeleportAllowed
#define IsPlayerTeleportAllowed sampgdk_IsPlayerTeleportAllowed

#undef  IsPlayerUsingOfficialClient
#define IsPlayerUsingOfficialClient sampgdk_IsPlayerUsingOfficialClient

#undef  IsPlayerUsingOmp
#define IsPlayerUsingOmp sampgdk_IsPlayerUsingOmp

#undef  IsPlayerWidescreenToggled
#define IsPlayerWidescreenToggled sampgdk_IsPlayerWidescreenToggled

#undef  PlayerHasClockEnabled
#define PlayerHasClockEnabled sampgdk_PlayerHasClockEnabled

#undef  RemovePlayerWeapon
#define RemovePlayerWeapon sampgdk_RemovePlayerWeapon

#undef  SetPlayerGravity
#define SetPlayerGravity sampgdk_SetPlayerGravity

#undef  TogglePlayerGhostMode
#define TogglePlayerGhostMode sampgdk_TogglePlayerGhostMode

#undef  TogglePlayerWidescreen
#define TogglePlayerWidescreen sampgdk_TogglePlayerWidescreen

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_PLAYER_H */

#ifndef SAMPGDK_OMP_TEXTDRAW_H
#define SAMPGDK_OMP_TEXTDRAW_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerTextDrawVisible">IsPlayerTextDrawVisible on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerTextDrawVisible(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsTextDrawVisibleForPlayer">IsTextDrawVisibleForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsTextDrawVisibleForPlayer(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidPlayerTextDraw">IsValidPlayerTextDraw on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidPlayerTextDraw(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidTextDraw">IsValidTextDraw on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidTextDraw(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetAlignment">PlayerTextDrawGetAlignment on open.mp</a>
 */
SAMPGDK_NATIVE(int, PlayerTextDrawGetAlignment(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetBackgroundCol">PlayerTextDrawGetBackgroundCol on open.mp</a>
 */
SAMPGDK_NATIVE(int, PlayerTextDrawGetBackgroundCol(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetBoxColor">PlayerTextDrawGetBoxColor on open.mp</a>
 */
SAMPGDK_NATIVE(int, PlayerTextDrawGetBoxColor(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetColor">PlayerTextDrawGetColor on open.mp</a>
 */
SAMPGDK_NATIVE(int, PlayerTextDrawGetColor(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetFont">PlayerTextDrawGetFont on open.mp</a>
 */
SAMPGDK_NATIVE(int, PlayerTextDrawGetFont(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetLetterSize">PlayerTextDrawGetLetterSize on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawGetLetterSize(int playerid, int textid, float * width, float * height));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetOutline">PlayerTextDrawGetOutline on open.mp</a>
 */
SAMPGDK_NATIVE(int, PlayerTextDrawGetOutline(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetPos">PlayerTextDrawGetPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawGetPos(int playerid, int textid, float * x, float * y));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetPreviewModel">PlayerTextDrawGetPreviewModel on open.mp</a>
 */
SAMPGDK_NATIVE(int, PlayerTextDrawGetPreviewModel(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetPreviewRot">PlayerTextDrawGetPreviewRot on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawGetPreviewRot(int playerid, int textid, float * rotationX, float * rotationY, float * rotationZ, float * zoom));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetPreviewVehCol">PlayerTextDrawGetPreviewVehCol on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawGetPreviewVehCol(int playerid, int textid, int * colour1, int * colour2));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetShadow">PlayerTextDrawGetShadow on open.mp</a>
 */
SAMPGDK_NATIVE(int, PlayerTextDrawGetShadow(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetString">PlayerTextDrawGetString on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawGetString(int playerid, int textid, char * string_, int stringSize));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawGetTextSize">PlayerTextDrawGetTextSize on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawGetTextSize(int playerid, int textid, float * width, float * height));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawIsBox">PlayerTextDrawIsBox on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawIsBox(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawIsProportional">PlayerTextDrawIsProportional on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawIsProportional(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawIsSelectable">PlayerTextDrawIsSelectable on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawIsSelectable(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/PlayerTextDrawSetPos">PlayerTextDrawSetPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, PlayerTextDrawSetPos(int playerid, int textid, float x, float y));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetAlignment">TextDrawGetAlignment on open.mp</a>
 */
SAMPGDK_NATIVE(int, TextDrawGetAlignment(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetBackgroundColor">TextDrawGetBackgroundColor on open.mp</a>
 */
SAMPGDK_NATIVE(int, TextDrawGetBackgroundColor(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetBoxColor">TextDrawGetBoxColor on open.mp</a>
 */
SAMPGDK_NATIVE(int, TextDrawGetBoxColor(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetColor">TextDrawGetColor on open.mp</a>
 */
SAMPGDK_NATIVE(int, TextDrawGetColor(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetFont">TextDrawGetFont on open.mp</a>
 */
SAMPGDK_NATIVE(int, TextDrawGetFont(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetLetterSize">TextDrawGetLetterSize on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawGetLetterSize(int textid, float * width, float * height));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetOutline">TextDrawGetOutline on open.mp</a>
 */
SAMPGDK_NATIVE(int, TextDrawGetOutline(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetPos">TextDrawGetPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawGetPos(int textid, float * x, float * y));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetPreviewModel">TextDrawGetPreviewModel on open.mp</a>
 */
SAMPGDK_NATIVE(int, TextDrawGetPreviewModel(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetPreviewRot">TextDrawGetPreviewRot on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawGetPreviewRot(int textid, float * rotationX, float * rotationY, float * rotationZ, float * zoom));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetPreviewVehCol">TextDrawGetPreviewVehCol on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawGetPreviewVehCol(int textid, int * colour1, int * colour2));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetShadow">TextDrawGetShadow on open.mp</a>
 */
SAMPGDK_NATIVE(int, TextDrawGetShadow(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetString">TextDrawGetString on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawGetString(int textid, char * string_, int stringSize));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawGetTextSize">TextDrawGetTextSize on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawGetTextSize(int textid, float * width, float * height));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawIsBox">TextDrawIsBox on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawIsBox(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawIsProportional">TextDrawIsProportional on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawIsProportional(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawIsSelectable">TextDrawIsSelectable on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawIsSelectable(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawSetPos">TextDrawSetPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawSetPos(int textid, float x, float y));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/TextDrawSetStringForPlayer">TextDrawSetStringForPlayer on open.mp</a>
 */
SAMPGDK_NATIVE(bool, TextDrawSetStringForPlayer(int textid, int playerid, const char * format));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool IsPlayerTextDrawVisible(int playerid, int textid) {
  return sampgdk_IsPlayerTextDrawVisible(playerid, textid);
}

inline bool IsTextDrawVisibleForPlayer(int playerid, int textid) {
  return sampgdk_IsTextDrawVisibleForPlayer(playerid, textid);
}

inline bool IsValidPlayerTextDraw(int playerid, int textid) {
  return sampgdk_IsValidPlayerTextDraw(playerid, textid);
}

inline bool IsValidTextDraw(int textid) {
  return sampgdk_IsValidTextDraw(textid);
}

inline int PlayerTextDrawGetAlignment(int playerid, int textid) {
  return sampgdk_PlayerTextDrawGetAlignment(playerid, textid);
}

inline int PlayerTextDrawGetBackgroundCol(int playerid, int textid) {
  return sampgdk_PlayerTextDrawGetBackgroundCol(playerid, textid);
}

inline int PlayerTextDrawGetBoxColor(int playerid, int textid) {
  return sampgdk_PlayerTextDrawGetBoxColor(playerid, textid);
}

inline int PlayerTextDrawGetColor(int playerid, int textid) {
  return sampgdk_PlayerTextDrawGetColor(playerid, textid);
}

inline int PlayerTextDrawGetFont(int playerid, int textid) {
  return sampgdk_PlayerTextDrawGetFont(playerid, textid);
}

inline bool PlayerTextDrawGetLetterSize(int playerid, int textid, float * width, float * height) {
  return sampgdk_PlayerTextDrawGetLetterSize(playerid, textid, width, height);
}

inline int PlayerTextDrawGetOutline(int playerid, int textid) {
  return sampgdk_PlayerTextDrawGetOutline(playerid, textid);
}

inline bool PlayerTextDrawGetPos(int playerid, int textid, float * x, float * y) {
  return sampgdk_PlayerTextDrawGetPos(playerid, textid, x, y);
}

inline int PlayerTextDrawGetPreviewModel(int playerid, int textid) {
  return sampgdk_PlayerTextDrawGetPreviewModel(playerid, textid);
}

inline bool PlayerTextDrawGetPreviewRot(int playerid, int textid, float * rotationX, float * rotationY, float * rotationZ, float * zoom) {
  return sampgdk_PlayerTextDrawGetPreviewRot(playerid, textid, rotationX, rotationY, rotationZ, zoom);
}

inline bool PlayerTextDrawGetPreviewVehCol(int playerid, int textid, int * colour1, int * colour2) {
  return sampgdk_PlayerTextDrawGetPreviewVehCol(playerid, textid, colour1, colour2);
}

inline int PlayerTextDrawGetShadow(int playerid, int textid) {
  return sampgdk_PlayerTextDrawGetShadow(playerid, textid);
}

inline bool PlayerTextDrawGetString(int playerid, int textid, char * string_, int stringSize) {
  return sampgdk_PlayerTextDrawGetString(playerid, textid, string_, stringSize);
}

inline bool PlayerTextDrawGetTextSize(int playerid, int textid, float * width, float * height) {
  return sampgdk_PlayerTextDrawGetTextSize(playerid, textid, width, height);
}

inline bool PlayerTextDrawIsBox(int playerid, int textid) {
  return sampgdk_PlayerTextDrawIsBox(playerid, textid);
}

inline bool PlayerTextDrawIsProportional(int playerid, int textid) {
  return sampgdk_PlayerTextDrawIsProportional(playerid, textid);
}

inline bool PlayerTextDrawIsSelectable(int playerid, int textid) {
  return sampgdk_PlayerTextDrawIsSelectable(playerid, textid);
}

inline bool PlayerTextDrawSetPos(int playerid, int textid, float x, float y) {
  return sampgdk_PlayerTextDrawSetPos(playerid, textid, x, y);
}

inline int TextDrawGetAlignment(int textid) {
  return sampgdk_TextDrawGetAlignment(textid);
}

inline int TextDrawGetBackgroundColor(int textid) {
  return sampgdk_TextDrawGetBackgroundColor(textid);
}

inline int TextDrawGetBoxColor(int textid) {
  return sampgdk_TextDrawGetBoxColor(textid);
}

inline int TextDrawGetColor(int textid) {
  return sampgdk_TextDrawGetColor(textid);
}

inline int TextDrawGetFont(int textid) {
  return sampgdk_TextDrawGetFont(textid);
}

inline bool TextDrawGetLetterSize(int textid, float * width, float * height) {
  return sampgdk_TextDrawGetLetterSize(textid, width, height);
}

inline int TextDrawGetOutline(int textid) {
  return sampgdk_TextDrawGetOutline(textid);
}

inline bool TextDrawGetPos(int textid, float * x, float * y) {
  return sampgdk_TextDrawGetPos(textid, x, y);
}

inline int TextDrawGetPreviewModel(int textid) {
  return sampgdk_TextDrawGetPreviewModel(textid);
}

inline bool TextDrawGetPreviewRot(int textid, float * rotationX, float * rotationY, float * rotationZ, float * zoom) {
  return sampgdk_TextDrawGetPreviewRot(textid, rotationX, rotationY, rotationZ, zoom);
}

inline bool TextDrawGetPreviewVehCol(int textid, int * colour1, int * colour2) {
  return sampgdk_TextDrawGetPreviewVehCol(textid, colour1, colour2);
}

inline int TextDrawGetShadow(int textid) {
  return sampgdk_TextDrawGetShadow(textid);
}

inline bool TextDrawGetString(int textid, char * string_, int stringSize) {
  return sampgdk_TextDrawGetString(textid, string_, stringSize);
}

inline bool TextDrawGetTextSize(int textid, float * width, float * height) {
  return sampgdk_TextDrawGetTextSize(textid, width, height);
}

inline bool TextDrawIsBox(int textid) {
  return sampgdk_TextDrawIsBox(textid);
}

inline bool TextDrawIsProportional(int textid) {
  return sampgdk_TextDrawIsProportional(textid);
}

inline bool TextDrawIsSelectable(int textid) {
  return sampgdk_TextDrawIsSelectable(textid);
}

inline bool TextDrawSetPos(int textid, float x, float y) {
  return sampgdk_TextDrawSetPos(textid, x, y);
}

inline bool TextDrawSetStringForPlayer(int textid, int playerid, const char * format) {
  return sampgdk_TextDrawSetStringForPlayer(textid, playerid, format);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  IsPlayerTextDrawVisible
#define IsPlayerTextDrawVisible sampgdk_IsPlayerTextDrawVisible

#undef  IsTextDrawVisibleForPlayer
#define IsTextDrawVisibleForPlayer sampgdk_IsTextDrawVisibleForPlayer

#undef  IsValidPlayerTextDraw
#define IsValidPlayerTextDraw sampgdk_IsValidPlayerTextDraw

#undef  IsValidTextDraw
#define IsValidTextDraw sampgdk_IsValidTextDraw

#undef  PlayerTextDrawGetAlignment
#define PlayerTextDrawGetAlignment sampgdk_PlayerTextDrawGetAlignment

#undef  PlayerTextDrawGetBackgroundCol
#define PlayerTextDrawGetBackgroundCol sampgdk_PlayerTextDrawGetBackgroundCol

#undef  PlayerTextDrawGetBoxColor
#define PlayerTextDrawGetBoxColor sampgdk_PlayerTextDrawGetBoxColor

#undef  PlayerTextDrawGetColor
#define PlayerTextDrawGetColor sampgdk_PlayerTextDrawGetColor

#undef  PlayerTextDrawGetFont
#define PlayerTextDrawGetFont sampgdk_PlayerTextDrawGetFont

#undef  PlayerTextDrawGetLetterSize
#define PlayerTextDrawGetLetterSize sampgdk_PlayerTextDrawGetLetterSize

#undef  PlayerTextDrawGetOutline
#define PlayerTextDrawGetOutline sampgdk_PlayerTextDrawGetOutline

#undef  PlayerTextDrawGetPos
#define PlayerTextDrawGetPos sampgdk_PlayerTextDrawGetPos

#undef  PlayerTextDrawGetPreviewModel
#define PlayerTextDrawGetPreviewModel sampgdk_PlayerTextDrawGetPreviewModel

#undef  PlayerTextDrawGetPreviewRot
#define PlayerTextDrawGetPreviewRot sampgdk_PlayerTextDrawGetPreviewRot

#undef  PlayerTextDrawGetPreviewVehCol
#define PlayerTextDrawGetPreviewVehCol sampgdk_PlayerTextDrawGetPreviewVehCol

#undef  PlayerTextDrawGetShadow
#define PlayerTextDrawGetShadow sampgdk_PlayerTextDrawGetShadow

#undef  PlayerTextDrawGetString
#define PlayerTextDrawGetString sampgdk_PlayerTextDrawGetString

#undef  PlayerTextDrawGetTextSize
#define PlayerTextDrawGetTextSize sampgdk_PlayerTextDrawGetTextSize

#undef  PlayerTextDrawIsBox
#define PlayerTextDrawIsBox sampgdk_PlayerTextDrawIsBox

#undef  PlayerTextDrawIsProportional
#define PlayerTextDrawIsProportional sampgdk_PlayerTextDrawIsProportional

#undef  PlayerTextDrawIsSelectable
#define PlayerTextDrawIsSelectable sampgdk_PlayerTextDrawIsSelectable

#undef  PlayerTextDrawSetPos
#define PlayerTextDrawSetPos sampgdk_PlayerTextDrawSetPos

#undef  TextDrawGetAlignment
#define TextDrawGetAlignment sampgdk_TextDrawGetAlignment

#undef  TextDrawGetBackgroundColor
#define TextDrawGetBackgroundColor sampgdk_TextDrawGetBackgroundColor

#undef  TextDrawGetBoxColor
#define TextDrawGetBoxColor sampgdk_TextDrawGetBoxColor

#undef  TextDrawGetColor
#define TextDrawGetColor sampgdk_TextDrawGetColor

#undef  TextDrawGetFont
#define TextDrawGetFont sampgdk_TextDrawGetFont

#undef  TextDrawGetLetterSize
#define TextDrawGetLetterSize sampgdk_TextDrawGetLetterSize

#undef  TextDrawGetOutline
#define TextDrawGetOutline sampgdk_TextDrawGetOutline

#undef  TextDrawGetPos
#define TextDrawGetPos sampgdk_TextDrawGetPos

#undef  TextDrawGetPreviewModel
#define TextDrawGetPreviewModel sampgdk_TextDrawGetPreviewModel

#undef  TextDrawGetPreviewRot
#define TextDrawGetPreviewRot sampgdk_TextDrawGetPreviewRot

#undef  TextDrawGetPreviewVehCol
#define TextDrawGetPreviewVehCol sampgdk_TextDrawGetPreviewVehCol

#undef  TextDrawGetShadow
#define TextDrawGetShadow sampgdk_TextDrawGetShadow

#undef  TextDrawGetString
#define TextDrawGetString sampgdk_TextDrawGetString

#undef  TextDrawGetTextSize
#define TextDrawGetTextSize sampgdk_TextDrawGetTextSize

#undef  TextDrawIsBox
#define TextDrawIsBox sampgdk_TextDrawIsBox

#undef  TextDrawIsProportional
#define TextDrawIsProportional sampgdk_TextDrawIsProportional

#undef  TextDrawIsSelectable
#define TextDrawIsSelectable sampgdk_TextDrawIsSelectable

#undef  TextDrawSetPos
#define TextDrawSetPos sampgdk_TextDrawSetPos

#undef  TextDrawSetStringForPlayer
#define TextDrawSetStringForPlayer sampgdk_TextDrawSetStringForPlayer

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_TEXTDRAW_H */

#ifndef SAMPGDK_OMP_TEXTLABEL_H
#define SAMPGDK_OMP_TEXTLABEL_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Get3DTextLabelAttachedData">Get3DTextLabelAttachedData on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Get3DTextLabelAttachedData(int textid, int * parentPlayerid, int * parentVehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Get3DTextLabelColor">Get3DTextLabelColor on open.mp</a>
 */
SAMPGDK_NATIVE(int, Get3DTextLabelColor(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Get3DTextLabelDrawDistance">Get3DTextLabelDrawDistance on open.mp</a>
 */
SAMPGDK_NATIVE(float, Get3DTextLabelDrawDistance(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Get3DTextLabelLOS">Get3DTextLabelLOS on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Get3DTextLabelLOS(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Get3DTextLabelPos">Get3DTextLabelPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Get3DTextLabelPos(int textid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Get3DTextLabelText">Get3DTextLabelText on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Get3DTextLabelText(int textid, char * text, int len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Get3DTextLabelVirtualWorld">Get3DTextLabelVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(int, Get3DTextLabelVirtualWorld(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayer3DTextLabelAttached">GetPlayer3DTextLabelAttached on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayer3DTextLabelAttached(int playerid, int textid, int * parentPlayerid, int * parentVehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayer3DTextLabelColor">GetPlayer3DTextLabelColor on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayer3DTextLabelColor(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayer3DTextLabelDrawDist">GetPlayer3DTextLabelDrawDist on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetPlayer3DTextLabelDrawDist(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayer3DTextLabelLOS">GetPlayer3DTextLabelLOS on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayer3DTextLabelLOS(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayer3DTextLabelPos">GetPlayer3DTextLabelPos on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayer3DTextLabelPos(int playerid, int textid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayer3DTextLabelText">GetPlayer3DTextLabelText on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayer3DTextLabelText(int playerid, int textid, char * text, int len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayer3DTextLabelVirtualW">GetPlayer3DTextLabelVirtualW on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayer3DTextLabelVirtualW(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Is3DTextLabelStreamedIn">Is3DTextLabelStreamedIn on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Is3DTextLabelStreamedIn(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValid3DTextLabel">IsValid3DTextLabel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValid3DTextLabel(int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsValidPlayer3DTextLabel">IsValidPlayer3DTextLabel on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsValidPlayer3DTextLabel(int playerid, int textid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Set3DTextLabelDrawDistance">Set3DTextLabelDrawDistance on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Set3DTextLabelDrawDistance(int textid, float drawDistance));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Set3DTextLabelLOS">Set3DTextLabelLOS on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Set3DTextLabelLOS(int textid, bool enable));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/Set3DTextLabelVirtualWorld">Set3DTextLabelVirtualWorld on open.mp</a>
 */
SAMPGDK_NATIVE(bool, Set3DTextLabelVirtualWorld(int textid, int virtualWorld));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayer3DTextLabelDrawDist">SetPlayer3DTextLabelDrawDist on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayer3DTextLabelDrawDist(int playerid, int textid, float drawDistance));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetPlayer3DTextLabelLOS">SetPlayer3DTextLabelLOS on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetPlayer3DTextLabelLOS(int playerid, int textid, bool enable));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline bool Get3DTextLabelAttachedData(int textid, int * parentPlayerid, int * parentVehicleid) {
  return sampgdk_Get3DTextLabelAttachedData(textid, parentPlayerid, parentVehicleid);
}

inline int Get3DTextLabelColor(int textid) {
  return sampgdk_Get3DTextLabelColor(textid);
}

inline float Get3DTextLabelDrawDistance(int textid) {
  return sampgdk_Get3DTextLabelDrawDistance(textid);
}

inline bool Get3DTextLabelLOS(int textid) {
  return sampgdk_Get3DTextLabelLOS(textid);
}

inline bool Get3DTextLabelPos(int textid, float * x, float * y, float * z) {
  return sampgdk_Get3DTextLabelPos(textid, x, y, z);
}

inline bool Get3DTextLabelText(int textid, char * text, int len) {
  return sampgdk_Get3DTextLabelText(textid, text, len);
}

inline int Get3DTextLabelVirtualWorld(int textid) {
  return sampgdk_Get3DTextLabelVirtualWorld(textid);
}

inline bool GetPlayer3DTextLabelAttached(int playerid, int textid, int * parentPlayerid, int * parentVehicleid) {
  return sampgdk_GetPlayer3DTextLabelAttached(playerid, textid, parentPlayerid, parentVehicleid);
}

inline int GetPlayer3DTextLabelColor(int playerid, int textid) {
  return sampgdk_GetPlayer3DTextLabelColor(playerid, textid);
}

inline float GetPlayer3DTextLabelDrawDist(int playerid, int textid) {
  return sampgdk_GetPlayer3DTextLabelDrawDist(playerid, textid);
}

inline bool GetPlayer3DTextLabelLOS(int playerid, int textid) {
  return sampgdk_GetPlayer3DTextLabelLOS(playerid, textid);
}

inline bool GetPlayer3DTextLabelPos(int playerid, int textid, float * x, float * y, float * z) {
  return sampgdk_GetPlayer3DTextLabelPos(playerid, textid, x, y, z);
}

inline bool GetPlayer3DTextLabelText(int playerid, int textid, char * text, int len) {
  return sampgdk_GetPlayer3DTextLabelText(playerid, textid, text, len);
}

inline int GetPlayer3DTextLabelVirtualW(int playerid, int textid) {
  return sampgdk_GetPlayer3DTextLabelVirtualW(playerid, textid);
}

inline bool Is3DTextLabelStreamedIn(int playerid, int textid) {
  return sampgdk_Is3DTextLabelStreamedIn(playerid, textid);
}

inline bool IsValid3DTextLabel(int textid) {
  return sampgdk_IsValid3DTextLabel(textid);
}

inline bool IsValidPlayer3DTextLabel(int playerid, int textid) {
  return sampgdk_IsValidPlayer3DTextLabel(playerid, textid);
}

inline bool Set3DTextLabelDrawDistance(int textid, float drawDistance) {
  return sampgdk_Set3DTextLabelDrawDistance(textid, drawDistance);
}

inline bool Set3DTextLabelLOS(int textid, bool enable) {
  return sampgdk_Set3DTextLabelLOS(textid, enable);
}

inline bool Set3DTextLabelVirtualWorld(int textid, int virtualWorld) {
  return sampgdk_Set3DTextLabelVirtualWorld(textid, virtualWorld);
}

inline bool SetPlayer3DTextLabelDrawDist(int playerid, int textid, float drawDistance) {
  return sampgdk_SetPlayer3DTextLabelDrawDist(playerid, textid, drawDistance);
}

inline bool SetPlayer3DTextLabelLOS(int playerid, int textid, bool enable) {
  return sampgdk_SetPlayer3DTextLabelLOS(playerid, textid, enable);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  Get3DTextLabelAttachedData
#define Get3DTextLabelAttachedData sampgdk_Get3DTextLabelAttachedData

#undef  Get3DTextLabelColor
#define Get3DTextLabelColor sampgdk_Get3DTextLabelColor

#undef  Get3DTextLabelDrawDistance
#define Get3DTextLabelDrawDistance sampgdk_Get3DTextLabelDrawDistance

#undef  Get3DTextLabelLOS
#define Get3DTextLabelLOS sampgdk_Get3DTextLabelLOS

#undef  Get3DTextLabelPos
#define Get3DTextLabelPos sampgdk_Get3DTextLabelPos

#undef  Get3DTextLabelText
#define Get3DTextLabelText sampgdk_Get3DTextLabelText

#undef  Get3DTextLabelVirtualWorld
#define Get3DTextLabelVirtualWorld sampgdk_Get3DTextLabelVirtualWorld

#undef  GetPlayer3DTextLabelAttached
#define GetPlayer3DTextLabelAttached sampgdk_GetPlayer3DTextLabelAttached

#undef  GetPlayer3DTextLabelColor
#define GetPlayer3DTextLabelColor sampgdk_GetPlayer3DTextLabelColor

#undef  GetPlayer3DTextLabelDrawDist
#define GetPlayer3DTextLabelDrawDist sampgdk_GetPlayer3DTextLabelDrawDist

#undef  GetPlayer3DTextLabelLOS
#define GetPlayer3DTextLabelLOS sampgdk_GetPlayer3DTextLabelLOS

#undef  GetPlayer3DTextLabelPos
#define GetPlayer3DTextLabelPos sampgdk_GetPlayer3DTextLabelPos

#undef  GetPlayer3DTextLabelText
#define GetPlayer3DTextLabelText sampgdk_GetPlayer3DTextLabelText

#undef  GetPlayer3DTextLabelVirtualW
#define GetPlayer3DTextLabelVirtualW sampgdk_GetPlayer3DTextLabelVirtualW

#undef  Is3DTextLabelStreamedIn
#define Is3DTextLabelStreamedIn sampgdk_Is3DTextLabelStreamedIn

#undef  IsValid3DTextLabel
#define IsValid3DTextLabel sampgdk_IsValid3DTextLabel

#undef  IsValidPlayer3DTextLabel
#define IsValidPlayer3DTextLabel sampgdk_IsValidPlayer3DTextLabel

#undef  Set3DTextLabelDrawDistance
#define Set3DTextLabelDrawDistance sampgdk_Set3DTextLabelDrawDistance

#undef  Set3DTextLabelLOS
#define Set3DTextLabelLOS sampgdk_Set3DTextLabelLOS

#undef  Set3DTextLabelVirtualWorld
#define Set3DTextLabelVirtualWorld sampgdk_Set3DTextLabelVirtualWorld

#undef  SetPlayer3DTextLabelDrawDist
#define SetPlayer3DTextLabelDrawDist sampgdk_SetPlayer3DTextLabelDrawDist

#undef  SetPlayer3DTextLabelLOS
#define SetPlayer3DTextLabelLOS sampgdk_SetPlayer3DTextLabelLOS

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_TEXTLABEL_H */

#ifndef SAMPGDK_OMP_VARIABLE_H
#define SAMPGDK_OMP_VARIABLE_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_VARIABLE_H */

#ifndef SAMPGDK_OMP_VEHICLE_H
#define SAMPGDK_OMP_VEHICLE_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CarColIndexToColour">CarColIndexToColour on open.mp</a>
 */
SAMPGDK_NATIVE(int, CarColIndexToColour(int index, int alpha));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/CountVehicleOccupants">CountVehicleOccupants on open.mp</a>
 */
SAMPGDK_NATIVE(int, CountVehicleOccupants(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerHydraReactorAngle">GetPlayerHydraReactorAngle on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerHydraReactorAngle(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerLandingGearState">GetPlayerLandingGearState on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetPlayerLandingGearState(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerSirenState">GetPlayerSirenState on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetPlayerSirenState(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetPlayerTrainSpeed">GetPlayerTrainSpeed on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetPlayerTrainSpeed(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetRandomCarColPair">GetRandomCarColPair on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetRandomCarColPair(int modelid, int * colour1, int * colour2, int * colour3, int * colour4));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleCab">GetVehicleCab on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleCab(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleColor">GetVehicleColor on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleColor(int vehicleid, int * color1, int * color2));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleDriver">GetVehicleDriver on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleDriver(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleHydraReactorAngle">GetVehicleHydraReactorAngle on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleHydraReactorAngle(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleInterior">GetVehicleInterior on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleInterior(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleLandingGearState">GetVehicleLandingGearState on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleLandingGearState(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleLastDriver">GetVehicleLastDriver on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleLastDriver(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleMatrix">GetVehicleMatrix on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleMatrix(int vehicleid, float * rightX, float * rightY, float * rightZ, float * upX, float * upY, float * upZ, float * atX, float * atY, float * atZ));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleModelCount">GetVehicleModelCount on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleModelCount(int modelid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleModelsUsed">GetVehicleModelsUsed on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleModelsUsed());

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleNumberPlate">GetVehicleNumberPlate on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleNumberPlate(int vehicleid, char * plate, int len));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleOccupant">GetVehicleOccupant on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleOccupant(int vehicleid, int seatid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleOccupiedTick">GetVehicleOccupiedTick on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleOccupiedTick(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehiclePaintjob">GetVehiclePaintjob on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehiclePaintjob(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleRespawnDelay">GetVehicleRespawnDelay on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleRespawnDelay(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleRespawnTick">GetVehicleRespawnTick on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleRespawnTick(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleRotation">GetVehicleRotation on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleRotation(int vehicleid, float * x, float * y, float * z));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleSeats">GetVehicleSeats on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleSeats(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleSirenState">GetVehicleSirenState on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleSirenState(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleSpawnInfo">GetVehicleSpawnInfo on open.mp</a>
 */
SAMPGDK_NATIVE(bool, GetVehicleSpawnInfo(int vehicleid, float * spawnX, float * spawnY, float * spawnZ, float * angle, int * colour1, int * colour2));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleTower">GetVehicleTower on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicleTower(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicleTrainSpeed">GetVehicleTrainSpeed on open.mp</a>
 */
SAMPGDK_NATIVE(float, GetVehicleTrainSpeed(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/GetVehicles">GetVehicles on open.mp</a>
 */
SAMPGDK_NATIVE(int, GetVehicles(char * vehicles, int size));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/HasVehicleBeenOccupied">HasVehicleBeenOccupied on open.mp</a>
 */
SAMPGDK_NATIVE(bool, HasVehicleBeenOccupied(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsPlayerInModShop">IsPlayerInModShop on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsPlayerInModShop(int playerid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsVehicleDead">IsVehicleDead on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsVehicleDead(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsVehicleOccupied">IsVehicleOccupied on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsVehicleOccupied(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/IsVehicleSirenEnabled">IsVehicleSirenEnabled on open.mp</a>
 */
SAMPGDK_NATIVE(bool, IsVehicleSirenEnabled(int vehicleid));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleParamsSirenState">SetVehicleParamsSirenState on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleParamsSirenState(int vehicleid, bool enabled));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleRespawnDelay">SetVehicleRespawnDelay on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleRespawnDelay(int vehicleid, int respawnDelay));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/SetVehicleSpawnInfo">SetVehicleSpawnInfo on open.mp</a>
 */
SAMPGDK_NATIVE(bool, SetVehicleSpawnInfo(int vehicleid, int modelid, float spawnX, float spawnY, float spawnZ, float angle, int colour1, int colour2, int respawnDelay, int interior));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/ToggleVehicleSirenEnabled">ToggleVehicleSirenEnabled on open.mp</a>
 */
SAMPGDK_NATIVE(bool, ToggleVehicleSirenEnabled(int vehicleid, bool enabled));

/**
 * \ingroup natives
 * \see <a href="https://open.mp/docs/scripting/functions/VehicleCanHaveComponent">VehicleCanHaveComponent on open.mp</a>
 */
SAMPGDK_NATIVE(bool, VehicleCanHaveComponent(int modelid, int componentid));

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

inline int CarColIndexToColour(int index, int alpha = 0xFF) {
  return sampgdk_CarColIndexToColour(index, alpha);
}

inline int CountVehicleOccupants(int vehicleid) {
  return sampgdk_CountVehicleOccupants(vehicleid);
}

inline int GetPlayerHydraReactorAngle(int playerid) {
  return sampgdk_GetPlayerHydraReactorAngle(playerid);
}

inline int GetPlayerLandingGearState(int playerid) {
  return sampgdk_GetPlayerLandingGearState(playerid);
}

inline bool GetPlayerSirenState(int playerid) {
  return sampgdk_GetPlayerSirenState(playerid);
}

inline float GetPlayerTrainSpeed(int playerid) {
  return sampgdk_GetPlayerTrainSpeed(playerid);
}

inline bool GetRandomCarColPair(int modelid, int * colour1, int * colour2, int * colour3, int * colour4) {
  return sampgdk_GetRandomCarColPair(modelid, colour1, colour2, colour3, colour4);
}

inline int GetVehicleCab(int vehicleid) {
  return sampgdk_GetVehicleCab(vehicleid);
}

inline bool GetVehicleColor(int vehicleid, int * color1, int * color2) {
  return sampgdk_GetVehicleColor(vehicleid, color1, color2);
}

inline int GetVehicleDriver(int vehicleid) {
  return sampgdk_GetVehicleDriver(vehicleid);
}

inline int GetVehicleHydraReactorAngle(int vehicleid) {
  return sampgdk_GetVehicleHydraReactorAngle(vehicleid);
}

inline int GetVehicleInterior(int vehicleid) {
  return sampgdk_GetVehicleInterior(vehicleid);
}

inline int GetVehicleLandingGearState(int vehicleid) {
  return sampgdk_GetVehicleLandingGearState(vehicleid);
}

inline int GetVehicleLastDriver(int vehicleid) {
  return sampgdk_GetVehicleLastDriver(vehicleid);
}

inline bool GetVehicleMatrix(int vehicleid, float * rightX, float * rightY, float * rightZ, float * upX, float * upY, float * upZ, float * atX, float * atY, float * atZ) {
  return sampgdk_GetVehicleMatrix(vehicleid, rightX, rightY, rightZ, upX, upY, upZ, atX, atY, atZ);
}

inline int GetVehicleModelCount(int modelid) {
  return sampgdk_GetVehicleModelCount(modelid);
}

inline int GetVehicleModelsUsed() {
  return sampgdk_GetVehicleModelsUsed();
}

inline bool GetVehicleNumberPlate(int vehicleid, char * plate, int len) {
  return sampgdk_GetVehicleNumberPlate(vehicleid, plate, len);
}

inline int GetVehicleOccupant(int vehicleid, int seatid) {
  return sampgdk_GetVehicleOccupant(vehicleid, seatid);
}

inline int GetVehicleOccupiedTick(int vehicleid) {
  return sampgdk_GetVehicleOccupiedTick(vehicleid);
}

inline int GetVehiclePaintjob(int vehicleid) {
  return sampgdk_GetVehiclePaintjob(vehicleid);
}

inline int GetVehicleRespawnDelay(int vehicleid) {
  return sampgdk_GetVehicleRespawnDelay(vehicleid);
}

inline int GetVehicleRespawnTick(int vehicleid) {
  return sampgdk_GetVehicleRespawnTick(vehicleid);
}

inline bool GetVehicleRotation(int vehicleid, float * x, float * y, float * z) {
  return sampgdk_GetVehicleRotation(vehicleid, x, y, z);
}

inline int GetVehicleSeats(int vehicleid) {
  return sampgdk_GetVehicleSeats(vehicleid);
}

inline bool GetVehicleSirenState(int vehicleid) {
  return sampgdk_GetVehicleSirenState(vehicleid);
}

inline bool GetVehicleSpawnInfo(int vehicleid, float * spawnX, float * spawnY, float * spawnZ, float * angle, int * colour1, int * colour2) {
  return sampgdk_GetVehicleSpawnInfo(vehicleid, spawnX, spawnY, spawnZ, angle, colour1, colour2);
}

inline int GetVehicleTower(int vehicleid) {
  return sampgdk_GetVehicleTower(vehicleid);
}

inline float GetVehicleTrainSpeed(int vehicleid) {
  return sampgdk_GetVehicleTrainSpeed(vehicleid);
}

inline int GetVehicles(char * vehicles, int size) {
  return sampgdk_GetVehicles(vehicles, size);
}

inline bool HasVehicleBeenOccupied(int vehicleid) {
  return sampgdk_HasVehicleBeenOccupied(vehicleid);
}

inline bool IsPlayerInModShop(int playerid) {
  return sampgdk_IsPlayerInModShop(playerid);
}

inline bool IsVehicleDead(int vehicleid) {
  return sampgdk_IsVehicleDead(vehicleid);
}

inline bool IsVehicleOccupied(int vehicleid) {
  return sampgdk_IsVehicleOccupied(vehicleid);
}

inline bool IsVehicleSirenEnabled(int vehicleid) {
  return sampgdk_IsVehicleSirenEnabled(vehicleid);
}

inline bool SetVehicleParamsSirenState(int vehicleid, bool enabled) {
  return sampgdk_SetVehicleParamsSirenState(vehicleid, enabled);
}

inline bool SetVehicleRespawnDelay(int vehicleid, int respawnDelay) {
  return sampgdk_SetVehicleRespawnDelay(vehicleid, respawnDelay);
}

inline bool SetVehicleSpawnInfo(int vehicleid, int modelid, float spawnX, float spawnY, float spawnZ, float angle, int colour1, int colour2, int respawnDelay = -2, int interior = -2) {
  return sampgdk_SetVehicleSpawnInfo(vehicleid, modelid, spawnX, spawnY, spawnZ, angle, colour1, colour2, respawnDelay, interior);
}

inline bool ToggleVehicleSirenEnabled(int vehicleid, bool enabled) {
  return sampgdk_ToggleVehicleSirenEnabled(vehicleid, enabled);
}

inline bool VehicleCanHaveComponent(int modelid, int componentid) {
  return sampgdk_VehicleCanHaveComponent(modelid, componentid);
}

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#undef  CarColIndexToColour
#define CarColIndexToColour sampgdk_CarColIndexToColour

#undef  CountVehicleOccupants
#define CountVehicleOccupants sampgdk_CountVehicleOccupants

#undef  GetPlayerHydraReactorAngle
#define GetPlayerHydraReactorAngle sampgdk_GetPlayerHydraReactorAngle

#undef  GetPlayerLandingGearState
#define GetPlayerLandingGearState sampgdk_GetPlayerLandingGearState

#undef  GetPlayerSirenState
#define GetPlayerSirenState sampgdk_GetPlayerSirenState

#undef  GetPlayerTrainSpeed
#define GetPlayerTrainSpeed sampgdk_GetPlayerTrainSpeed

#undef  GetRandomCarColPair
#define GetRandomCarColPair sampgdk_GetRandomCarColPair

#undef  GetVehicleCab
#define GetVehicleCab sampgdk_GetVehicleCab

#undef  GetVehicleColor
#define GetVehicleColor sampgdk_GetVehicleColor

#undef  GetVehicleDriver
#define GetVehicleDriver sampgdk_GetVehicleDriver

#undef  GetVehicleHydraReactorAngle
#define GetVehicleHydraReactorAngle sampgdk_GetVehicleHydraReactorAngle

#undef  GetVehicleInterior
#define GetVehicleInterior sampgdk_GetVehicleInterior

#undef  GetVehicleLandingGearState
#define GetVehicleLandingGearState sampgdk_GetVehicleLandingGearState

#undef  GetVehicleLastDriver
#define GetVehicleLastDriver sampgdk_GetVehicleLastDriver

#undef  GetVehicleMatrix
#define GetVehicleMatrix sampgdk_GetVehicleMatrix

#undef  GetVehicleModelCount
#define GetVehicleModelCount sampgdk_GetVehicleModelCount

#undef  GetVehicleModelsUsed
#define GetVehicleModelsUsed sampgdk_GetVehicleModelsUsed

#undef  GetVehicleNumberPlate
#define GetVehicleNumberPlate sampgdk_GetVehicleNumberPlate

#undef  GetVehicleOccupant
#define GetVehicleOccupant sampgdk_GetVehicleOccupant

#undef  GetVehicleOccupiedTick
#define GetVehicleOccupiedTick sampgdk_GetVehicleOccupiedTick

#undef  GetVehiclePaintjob
#define GetVehiclePaintjob sampgdk_GetVehiclePaintjob

#undef  GetVehicleRespawnDelay
#define GetVehicleRespawnDelay sampgdk_GetVehicleRespawnDelay

#undef  GetVehicleRespawnTick
#define GetVehicleRespawnTick sampgdk_GetVehicleRespawnTick

#undef  GetVehicleRotation
#define GetVehicleRotation sampgdk_GetVehicleRotation

#undef  GetVehicleSeats
#define GetVehicleSeats sampgdk_GetVehicleSeats

#undef  GetVehicleSirenState
#define GetVehicleSirenState sampgdk_GetVehicleSirenState

#undef  GetVehicleSpawnInfo
#define GetVehicleSpawnInfo sampgdk_GetVehicleSpawnInfo

#undef  GetVehicleTower
#define GetVehicleTower sampgdk_GetVehicleTower

#undef  GetVehicleTrainSpeed
#define GetVehicleTrainSpeed sampgdk_GetVehicleTrainSpeed

#undef  GetVehicles
#define GetVehicles sampgdk_GetVehicles

#undef  HasVehicleBeenOccupied
#define HasVehicleBeenOccupied sampgdk_HasVehicleBeenOccupied

#undef  IsPlayerInModShop
#define IsPlayerInModShop sampgdk_IsPlayerInModShop

#undef  IsVehicleDead
#define IsVehicleDead sampgdk_IsVehicleDead

#undef  IsVehicleOccupied
#define IsVehicleOccupied sampgdk_IsVehicleOccupied

#undef  IsVehicleSirenEnabled
#define IsVehicleSirenEnabled sampgdk_IsVehicleSirenEnabled

#undef  SetVehicleParamsSirenState
#define SetVehicleParamsSirenState sampgdk_SetVehicleParamsSirenState

#undef  SetVehicleRespawnDelay
#define SetVehicleRespawnDelay sampgdk_SetVehicleRespawnDelay

#undef  SetVehicleSpawnInfo
#define SetVehicleSpawnInfo sampgdk_SetVehicleSpawnInfo

#undef  ToggleVehicleSirenEnabled
#define ToggleVehicleSirenEnabled sampgdk_ToggleVehicleSirenEnabled

#undef  VehicleCanHaveComponent
#define VehicleCanHaveComponent sampgdk_VehicleCanHaveComponent

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_VEHICLE_H */

#ifndef SAMPGDK_OMP_CONSTS_H
#define SAMPGDK_OMP_CONSTS_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */

#define CAM_MODE_1STPERSON (16)
#define CAM_MODE_AIMWEAPON (53)
#define CAM_MODE_AIMWEAPON_FROMCAR (55)
#define CAM_MODE_BEHINDBOAT (22)
#define CAM_MODE_BEHINDCAR (3)
#define CAM_MODE_CAMERA (46)
#define CAM_MODE_CAM_ON_A_STRING (18)
#define CAM_MODE_DISCONNECTED (-1)
#define CAM_MODE_DW_HELI_CHASE (56)
#define CAM_MODE_FIXED (15)
#define CAM_MODE_FOLLOWPED (4)
#define CAM_MODE_NONE (0)
#define CAM_MODE_ROCKETLAUNCHER (8)
#define CAM_MODE_ROCKETLAUNCHER_HS (51)
#define CAM_MODE_SNIPER (7)
#define CARDOOR_BONNET_DAMAGED (0x2)
#define CARDOOR_BONNET_MISSING (0x4)
#define CARDOOR_BONNET_OPEN (0x1)
#define CARDOOR_BOOT_DAMAGED (0x200)
#define CARDOOR_BOOT_MISSING (0x400)
#define CARDOOR_BOOT_OPEN (0x100)
#define CARDOOR_DRIVER_DAMAGED (0x20000)
#define CARDOOR_DRIVER_MISSING (0x40000)
#define CARDOOR_DRIVER_OPEN (0x10000)
#define CARDOOR_FRONT_LEFT_DAMAGED (0x20000)
#define CARDOOR_FRONT_LEFT_MISSING (0x40000)
#define CARDOOR_FRONT_LEFT_OPEN (0x10000)
#define CARDOOR_FRONT_RIGHT_DAMAGED (0x2000000)
#define CARDOOR_FRONT_RIGHT_MISSING (0x4000000)
#define CARDOOR_FRONT_RIGHT_OPEN (0x1000000)
#define CARDOOR_HOOD_DAMAGED (0x2)
#define CARDOOR_HOOD_MISSING (0x4)
#define CARDOOR_HOOD_OPEN (0x1)
#define CARDOOR_NONE (0)
#define CARDOOR_PASSENGER_DAMAGED (0x2000000)
#define CARDOOR_PASSENGER_MISSING (0x4000000)
#define CARDOOR_PASSENGER_OPEN (0x1000000)
#define CARDOOR_TRUNK_DAMAGED (0x200)
#define CARDOOR_TRUNK_MISSING (0x400)
#define CARDOOR_TRUNK_OPEN (0x100)
#define CARLIGHT_DRIVER_BROKEN (1)
#define CARLIGHT_FRONT_LEFT_BROKEN (1)
#define CARLIGHT_FRONT_RIGHT_BROKEN (4)
#define CARLIGHT_PASSENGER_BROKEN (4)
#define CARLIGHT_REAR_BROKEN (64)
#define CARMODTYPE_FRONT_BULLBAR (14)
#define CARMODTYPE_NONE (-1)
#define CARMODTYPE_REAR_BULLBAR (15)
#define CARTIRE_FRONT_LEFT_POPPED (8)
#define CARTIRE_FRONT_RIGHT_POPPED (2)
#define CARTIRE_REAR_LEFT_POPPED (4)
#define CARTIRE_REAR_RIGHT_POPPED (1)
#define CARTYRE_FRONT_LEFT_POPPED (8)
#define CARTYRE_FRONT_RIGHT_POPPED (2)
#define CARTYRE_REAR_LEFT_POPPED (4)
#define CARTYRE_REAR_RIGHT_POPPED (1)
#define CONNECTION_STATUS_CONNECTED (8)
#define CONNECTION_STATUS_DISCONNECT_ASAP (1)
#define CONNECTION_STATUS_DISCONNECT_ASAP_SILENTLY (2)
#define CONNECTION_STATUS_DISCONNECT_ON_NO_ACK (3)
#define CONNECTION_STATUS_HANDLING_CONNECTION_REQUEST (5)
#define CONNECTION_STATUS_NO_ACTION (0)
#define CONNECTION_STATUS_REQUESTED_CONNECTION (4)
#define CONNECTION_STATUS_SET_ENCRYPTION_ON_MULTIPLE_16_BYTE_PACKET (7)
#define CONNECTION_STATUS_UNVERIFIED_SENDER (6)
#define CONNSTAT_CONNECTED (8)
#define CONNSTAT_DISCONNECT_ASAP (1)
#define CONNSTAT_DISCONNECT_ASAP_SILNT (2)
#define CONNSTAT_DISCONNECT_ON_NO_ACK (3)
#define CONNSTAT_HANDLING_CONN_REQ (5)
#define CONNSTAT_NO_ACTION (0)
#define CONNSTAT_REQUESTED_CONNECTION (4)
#define CONNSTAT_SET_ENCRYPTION (7)
#define CONNSTAT_UNVERIFIED_SENDER (6)
#define CP_TYPE_AIR_BOBBING (8)
#define CP_TYPE_AIR_FINISH (4)
#define CP_TYPE_AIR_NORMAL (3)
#define CP_TYPE_AIR_ROTATING (5)
#define CP_TYPE_AIR_STROBING (6)
#define CP_TYPE_AIR_SWINGING (7)
#define CP_TYPE_GROUND_EMPTY (2)
#define CP_TYPE_GROUND_FINISH (1)
#define CP_TYPE_GROUND_NORMAL (0)
#define INVALID_DIALOG_ID (-1)
#define INVALID_NODE_ID (-1)
#define INVALID_NPC_ID (-1)
#define INVALID_PATH_ID (-1)
#define INVALID_PICKUP (-1)
#define INVALID_PLAYER_3DTEXT_ID (65535)
#define INVALID_PLAYER_TEXT_DRAW (65535)
#define INVALID_RECORD_ID (-1)
#define INVALID_TIMER (0)
#define KEY_AIM (0x80)
#define KEY_NONE (0x0)
#define LANDING_GEAR_STATE_DOWN (0)
#define LANDING_GEAR_STATE_UP (1)
#define MAX_DIALOG (32768)
#define NPC_ENTITY_CHECK_ACTOR (4)
#define NPC_ENTITY_CHECK_ALL (255)
#define NPC_ENTITY_CHECK_MAP (128)
#define NPC_ENTITY_CHECK_NONE (0)
#define NPC_ENTITY_CHECK_NPC (2)
#define NPC_ENTITY_CHECK_OBJECT (16)
#define NPC_ENTITY_CHECK_PLAYER (1)
#define NPC_ENTITY_CHECK_POBJECT_ORIG (32)
#define NPC_ENTITY_CHECK_POBJECT_TARG (64)
#define NPC_ENTITY_CHECK_VEHICLE (8)
#define NPC_MAX_NODES (64)
#define NPC_MOVE_TYPE_AUTO (5)
#define NPC_MOVE_TYPE_DRIVE (4)
#define NPC_MOVE_TYPE_JOG (2)
#define NPC_MOVE_TYPE_NONE (0)
#define NPC_MOVE_TYPE_SPRINT (3)
#define NPC_MOVE_TYPE_WALK (1)
#define OBJECT_MATERIAL_TEXT_ALIGN_CENT (1)
#define OBJECT_MATERIAL_TEXT_ALIGN_CENTRE (1)
#define OBJECT_MATERIAL_TEXT_ALIGN_RIGT (2)
#define PLAYER_STATE_ENTER_VEHICLE_DRV (5)
#define PLAYER_STATE_ENTER_VEHICLE_PASS (6)
#define PLAYER_STATE_EXIT_VEH (4)
#define REASON_COLLISION (54)
#define REASON_CONNECT (200)
#define REASON_DISCONNECT (201)
#define REASON_DROWN (53)
#define REASON_EXPLOSION (51)
#define REASON_HELICOPTER_BLADES (50)
#define REASON_SPLAT (54)
#define REASON_SUICIDE (255)
#define REASON_VEHICLE (49)
#define SQLITE_OPEN_AUTOPROXY (0x20)
#define SQLITE_OPEN_CREATE (0x4)
#define SQLITE_OPEN_DELETEONCLOSE (0x8)
#define SQLITE_OPEN_EXCLUSIVE (0x10)
#define SQLITE_OPEN_EXRESCODE (0x2000000)
#define SQLITE_OPEN_FULLMUTEX (0x10000)
#define SQLITE_OPEN_MAIN_DB (0x100)
#define SQLITE_OPEN_MAIN_JOURNAL (0x800)
#define SQLITE_OPEN_MEMORY (0x80)
#define SQLITE_OPEN_NOFOLLOW (0x1000000)
#define SQLITE_OPEN_NOMUTEX (0x8000)
#define SQLITE_OPEN_PRIVATECACHE (0x40000)
#define SQLITE_OPEN_READONLY (0x1)
#define SQLITE_OPEN_READWRITE (0x2)
#define SQLITE_OPEN_SHAREDCACHE (0x20000)
#define SQLITE_OPEN_SUBJOURNAL (0x2000)
#define SQLITE_OPEN_SUPER_JOURNAL (0x4000)
#define SQLITE_OPEN_TEMP_DB (0x200)
#define SQLITE_OPEN_TEMP_JOURNAL (0x1000)
#define SQLITE_OPEN_TRANSIENT_DB (0x400)
#define SQLITE_OPEN_URI (0x40)
#define SQLITE_OPEN_WAL (0x80000)
#define SYNC_ALL (1)
#define SYNC_NONE (0)
#define SYNC_OTHER (2)
#define TEXT_DRAW_ALIGN_CENTER (2)
#define TEXT_DRAW_ALIGN_CENTRE (2)
#define TEXT_DRAW_ALIGN_LEFT (1)
#define TEXT_DRAW_ALIGN_RIGHT (3)
#define TEXT_DRAW_FONT_0 (0)
#define TEXT_DRAW_FONT_1 (1)
#define TEXT_DRAW_FONT_2 (2)
#define TEXT_DRAW_FONT_3 (3)
#define TEXT_DRAW_FONT_AHARONI_BOLD (1)
#define TEXT_DRAW_FONT_BANK (0)
#define TEXT_DRAW_FONT_BANK_GOTHIC (2)
#define TEXT_DRAW_FONT_BECKETT_REGULAR (0)
#define TEXT_DRAW_FONT_HEADING (3)
#define TEXT_DRAW_FONT_PREVIEW (5)
#define TEXT_DRAW_FONT_PRICEDOWN (3)
#define TEXT_DRAW_FONT_SPACEAGE (2)
#define TEXT_DRAW_FONT_SPRITE (4)
#define TEXT_DRAW_FONT_STANDARD (1)
#define UNKNOWN_BULLET_HIT_TYPE (-1)
#define UNKNOWN_CAM_MODE (-1)
#define UNKNOWN_CAM_MOVE (-1)
#define UNKNOWN_CARMODTYPE (-1)
#define UNKNOWN_CLICK_SOURCE (-1)
#define UNKNOWN_CONNECTION_STATUS (-1)
#define UNKNOWN_CP_TYPE (-1)
#define UNKNOWN_DIALOG_STYLE (-1)
#define UNKNOWN_DOWNLOAD_REQUEST (-1)
#define UNKNOWN_EDIT_RESPONSE (-1)
#define UNKNOWN_FIGHT_STYLE (-1)
#define UNKNOWN_FORCE_SYNC (-1)
#define UNKNOWN_HTTP_ERROR (-1)
#define UNKNOWN_HTTP_METHOD (-1)
#define UNKNOWN_KEY (-1)
#define UNKNOWN_MAPICON (-1)
#define UNKNOWN_NPC_MOVE_TYPE (-1)
#define UNKNOWN_OBJECT_MATERIAL_SIZE (-1)
#define UNKNOWN_OBJECT_MATERIAL_TEXT_ALIGN (-1)
#define UNKNOWN_PLAYER_MARKERS_MODE (-1)
#define UNKNOWN_PLAYER_RECORDING_TYPE (-1)
#define UNKNOWN_PLAYER_STATE (-1)
#define UNKNOWN_SELECT_OBJECT (-1)
#define UNKNOWN_SPECIAL_ACTION (-1)
#define UNKNOWN_SPECTATE_MODE (-1)
#define UNKNOWN_SQLITE_OPEN (-1)
#define UNKNOWN_TEXT_DRAW_ALIGN (-1)
#define UNKNOWN_TEXT_DRAW_FONT (-1)
#define UNKNOWN_VARTYPE (-1)
#define UNKNOWN_VEHICLE_DOOR_STATUS (-1)
#define UNKNOWN_VEHICLE_LIGHT_STATUS (0)
#define UNKNOWN_VEHICLE_MODEL_INFO (-1)
#define UNKNOWN_VEHICLE_PANEL_STATUS (-1)
#define UNKNOWN_VEHICLE_TYRE_STATUS (-1)
#define UNKNOWN_WEAPON (-1)
#define UNKNOWN_WEAPONSKILL (-1)
#define UNKNOWN_WEAPONSTATE (-1)
#define UNKNOWN_WEAPON_SLOT (-1)
#define VARTYPE_FLOAT (3)
#define VARTYPE_INT (1)
#define VARTYPE_NONE (0)
#define VARTYPE_STRING (2)
#define VEHICLE_DOOR_STATUS_BONNET_DAMAGED (0x2)
#define VEHICLE_DOOR_STATUS_BONNET_MISSING (0x4)
#define VEHICLE_DOOR_STATUS_BONNET_OPEN (0x1)
#define VEHICLE_DOOR_STATUS_BOOT_DAMAGED (0x200)
#define VEHICLE_DOOR_STATUS_BOOT_MISSING (0x400)
#define VEHICLE_DOOR_STATUS_BOOT_OPEN (0x100)
#define VEHICLE_DOOR_STATUS_DRIVER_DAMAGED (0x20000)
#define VEHICLE_DOOR_STATUS_DRIVER_MISSING (0x40000)
#define VEHICLE_DOOR_STATUS_DRIVER_OPEN (0x10000)
#define VEHICLE_DOOR_STATUS_FRONT_LEFT_DAMAGED (0x20000)
#define VEHICLE_DOOR_STATUS_FRONT_LEFT_MISSING (0x40000)
#define VEHICLE_DOOR_STATUS_FRONT_LEFT_OPEN (0x10000)
#define VEHICLE_DOOR_STATUS_FRONT_RIGHT_DAMAGED (0x2000000)
#define VEHICLE_DOOR_STATUS_FRONT_RIGHT_MISSING (0x4000000)
#define VEHICLE_DOOR_STATUS_FRONT_RIGHT_OPEN (0x1000000)
#define VEHICLE_DOOR_STATUS_HOOD_DAMAGED (0x2)
#define VEHICLE_DOOR_STATUS_HOOD_MISSING (0x4)
#define VEHICLE_DOOR_STATUS_HOOD_OPEN (0x1)
#define VEHICLE_DOOR_STATUS_NONE (0)
#define VEHICLE_DOOR_STATUS_PASSENGER_DAMAGED (0x2000000)
#define VEHICLE_DOOR_STATUS_PASSENGER_MISSING (0x4000000)
#define VEHICLE_DOOR_STATUS_PASSENGER_OPEN (0x1000000)
#define VEHICLE_DOOR_STATUS_TRUNK_DAMAGED (0x200)
#define VEHICLE_DOOR_STATUS_TRUNK_MISSING (0x400)
#define VEHICLE_DOOR_STATUS_TRUNK_OPEN (0x100)
#define VEHICLE_LIGHT_STATUS_DRIVER_BROKEN (1)
#define VEHICLE_LIGHT_STATUS_FRONT_LEFT_BROKEN (1)
#define VEHICLE_LIGHT_STATUS_FRONT_RIGHT_BROKEN (4)
#define VEHICLE_LIGHT_STATUS_NONE (0)
#define VEHICLE_LIGHT_STATUS_PASSENGER_BROKEN (4)
#define VEHICLE_LIGHT_STATUS_REAR_BROKEN (64)
#define VEHICLE_MODEL_INFO_FRONT_BUMPER (8)
#define VEHICLE_MODEL_INFO_FRONT_SEAT (2)
#define VEHICLE_MODEL_INFO_PETROL_CAP (4)
#define VEHICLE_MODEL_INFO_REAR_BUMPER (9)
#define VEHICLE_MODEL_INFO_REAR_SEAT (3)
#define VEHICLE_MODEL_INFO_WHEELS_FRONT (5)
#define VEHICLE_MODEL_INFO_WHEELS_MID (7)
#define VEHICLE_MODEL_INFO_WHEELS_REAR (6)
#define VEHICLE_PANEL_STATUS_NONE (0)
#define VEHICLE_TIRE_STATUS_FRONT_LEFT_POPPED (8)
#define VEHICLE_TIRE_STATUS_FRONT_RIGHT_POPPED (2)
#define VEHICLE_TIRE_STATUS_REAR_LEFT_POPPED (4)
#define VEHICLE_TIRE_STATUS_REAR_RIGHT_POPPED (1)
#define VEHICLE_TYRE_STATUS_FRONT_LEFT_POPPED (8)
#define VEHICLE_TYRE_STATUS_FRONT_RIGHT_POPPED (2)
#define VEHICLE_TYRE_STATUS_NONE (0)
#define VEHICLE_TYRE_STATUS_REAR_LEFT_POPPED (4)
#define VEHICLE_TYRE_STATUS_REAR_RIGHT_POPPED (1)
#define WEAPON_MOLOTOV (18)
#define WEAPON_NIGHTSTICK (3)
#define WEAPON_NIGHT_VISION_GOGGLES (44)
#define WEAPON_SLOT_ARTILLERY (7)
#define WEAPON_SLOT_ASSAULT_RIFLE (5)
#define WEAPON_SLOT_DETONATOR (12)
#define WEAPON_SLOT_EQUIPMENT (9)
#define WEAPON_SLOT_EXPLOSIVES (8)
#define WEAPON_SLOT_GADGET (11)
#define WEAPON_SLOT_GIFT (10)
#define WEAPON_SLOT_LONG_RIFLE (6)
#define WEAPON_SLOT_MACHINE_GUN (4)
#define WEAPON_SLOT_MELEE (1)
#define WEAPON_SLOT_PISTOL (2)
#define WEAPON_SLOT_SHOTGUN (3)
#define WEAPON_SLOT_UNARMED (0)
#define WEAPON_THERMAL_GOGGLES (45)
#define degrees (1)
#define floatround_ceil (2)
#define floatround_floor (1)
#define floatround_round (0)
#define floatround_tozero (3)
#define floatround_unbiased (4)
#define grades (2)
#define io_read (0)
#define radian (0)
#define seek_current (1)
#define seek_end (2)
#define seek_start (0)

#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

#endif /* !SAMPGDK_OMP_CONSTS_H */

