/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef TMPLR_MACROS_H
#define TMPLR_MACROS_H

/**
 * Marks the begin of a template block.
 *
 * Takes a comma-separated list of key value pairs, where values can be lists of
 * the form [[val1;val2;val3]] or single values. For example:
 *
 * ```c
 * _tmpl_begin(KEY1 = VALUE1, KEY2 = [[VALUE2; VALUE3]]);
 * KEY1 = KEY2;
 * _tmpl_end;
 * ```
 */

#define _tmpl_begin(...)

/**
 * Marks the end of a template block.
 */
#define _tmpl_end

/**
 * Adds a string to begin or end hook.
 *
 * @param HOOK either begin or end
 *
 * The string argument may contain commas but no parenthesis.
 */
#define _tmpl_hook(HOOK, ...)

/**
 * Stops tmplr processing and output.
 *
 * Until the matching _tmpl_unmute, all text is discarded and all tmplr command
 * ignored. A muted-block is useful to add includes that help LSP servers.
 */
#define _tmpl_mute

/**
 * Restarts tmplr processing output.
 */
#define _tmpl_mute

/**
 *  Maps a key K to a value which may contain commas
 */
#define _tmpl_map(K, ...)

/**
 * Skips template block iteration.
 *
 * @note This can only be called within _tmpl_begin and _tmpl_end.
 */
#define _tmpl_skip

/**
 * Deletes the line from the template output.
 */
#define _tmpl_dl

/**
 * Adds a new line.
 */
#define _tmpl_nl

/**
 * Aborts tmplr execution and exits with error code 1.
 */
#define _tmpl_abort

/**
 * Makes content uppercase.
 *
 * @note This can only be called within _tmpl_begin and _tmpl_end.
 */
#define _tmpl_upcase(...)

#endif
