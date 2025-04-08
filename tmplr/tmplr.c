//'usr/bin/env' cc -xc -DSCRIPT -o tmplr.bin "$0" && exec ./tmplr.bin "$@"
/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2022-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * Description: template replacement tool
 * Author: Huawei Dresden Research Center
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>

/*******************************************************************************
 * tmplr - a template replacement tool
 *
 * tmplr is a simple tool to achieve a minimum level of genericity in libvsync
 * without resorting to C preprocessor macros.
 *
 * ## Template blocks
 *
 * tmplr reads input files and replaces mappings in template blocks. Template
 * blocks are marked with TMPL_BEGIN/TMPL_END commands (see "Template commands"
 * below).
 *
 * For example:
 *
 *     TMPL_BEGIN(key=value)
 *     The following word, key, will be replaced by value.
 *     TMPL_END
 *
 * ## Template mappings
 *
 * The mappings given to TMPL_BEGIN are called *template mappings*.
 *
 * Iteration mappings may take a single value as in keyA = value1 or multiple
 * values as in keyA = [[value1; value2]]. The list of values is separated by
 * semicolumn and optionally sorrounded by [[ ]]. The list of template mappings
 * is separated by commas, for example:
 *
 *     TMPL_BEGIN(keyA=[[val1;val2]], keyB=[[val3;val4]])
 *     ...
 *     TMPL_END
 *
 * ## Block iterations
 *
 * If template mappings contain multiple values, the template block is repeated
 * for each combination of template mappings. Each such instance of template
 * mappings is called *iteration mapping* in the source code.
 *
 * Consider this block example:
 *
 *     TMPL_BEGIN(key=[[val1;val2]])
 *     Key --> key
 *     TMPL_END
 *
 * The template mapping consists of key=[[val1;val2]]. In the first iteration of
 * the block, the iteration mapping is key=val1, in the second iteration, the
 * mapping is key=val2.
 *
 * ## Persistent mappings
 *
 * Beyond template mappings, tmplr also support persistent mappings. Outside
 * template blocks, the user can use the command TMPL_MAP(key, value) to add
 * persistent mappings to the tmplr state.
 *
 * During the processing of each line in a block iteration, *after* exhausting
 * the application of iteration mappings, tmplr applies any persistent mapping
 * matches.
 *
 * ## Command line and mapping override
 *
 * tmplr is a CLI program and takes as input a list of files. It provides two
 * flags: -v for verbose output and -D to select a single value for an iteration
 * mapping. For example, -DkeyA=value1. Other values will be ignored.
 *
 * ## Valid keys and values
 *
 * tmplr **does not** tokenize the input. Hence, a key "two words" is a
 * perfectly valid key. Characters such as $ can also be used in keys and
 * values.
 *
 * The only restriction is that keys cannot contain
 * - new line: \n
 * - parenthesis: ( )
 * - comma: ,
 * - semicolon: ;
 * - nor any tmplr commands
 *
 * Values cannot contain parenthesis, commas nor semicolon.
 *
 * Disclaimer:
 * We are aware of similar, more powerful tools such as Jinja, Mustache and M4.
 * tmplr follows three design principles:
 *
 * - simplicity: as simple and maintainable as possible
 * - dependency freedom: no additonal language which will get deprecated
 * - c-syntax transperency: annotation should not interfer with the LSP
 *   servers such as clangd
 ******************************************************************************/

/*******************************************************************************
 * Template commands
 ******************************************************************************/

#define TMPL_MAP    "_tmpl_map"
#define TMPL_BEGIN  "_tmpl_begin"
#define TMPL_END    "_tmpl_end"
#define TMPL_MUTE   "_tmpl_mute"
#define TMPL_UNMUTE "_tmpl_unmute"

#define TMPL_ABORT  "_tmpl_abort"
#define TMPL_SKIP   "_tmpl_skip"
#define TMPL_DL     "_tmpl_dl"
#define TMPL_NL     "_tmpl_nl"
#define TMPL_UPCASE "_tmpl_upcase"
#define TMPL_HOOK   "_tmpl_hook"

/*******************************************************************************
 * Maximum line lengths and buffer sizes
 ******************************************************************************/

/* maximum length of a line */
#define MAX_SLEN 256
/* maximum number of lines in a block */
#define MAX_BLEN 100
/* maximum number of keys */
#define MAX_KEYS 1024
/* maximum length of a value */
#define MAX_VLEN 256
/* Buffer to hold the value */
#define V_BUF_LEN ((MAX_VLEN) + 1)
/* maximum length of a key */
#define MAX_KLEN 64
/* Buffer to hold the key */
#define K_BUF_LEN ((MAX_KLEN) + 1)
/* maximum number of replacements per line */
#define MAX_APPLY 32

/*******************************************************************************
 * Type definitions
 ******************************************************************************/

/* pair_t is a key-value pair. Key and value are 0-terminated char arrays. */
typedef struct {
    char key[K_BUF_LEN];
    char val[V_BUF_LEN];
} pair_t;

/* err_t represents an error message */
typedef struct {
    const char *msg;
} err_t;

#define NO_ERROR                                                               \
    (err_t)                                                                    \
    {                                                                          \
        0                                                                      \
    }
#define ERROR(m)                                                               \
    (err_t)                                                                    \
    {                                                                          \
        .msg = m                                                               \
    }
#define IS_ERROR(err) (err).msg != NULL

/*******************************************************************************
 * Logging
 ******************************************************************************/

bool _verbose;
#define debugf(fmt, ...)                                                       \
    do {                                                                       \
        if (_verbose)                                                          \
            printf("// " fmt, ##__VA_ARGS__);                                  \
    } while (0)

/*******************************************************************************
 * String functions
 ******************************************************************************/

void
trim(char *s, char c)
{
    assert(s);
    /* remove trailing space */
    while (s[strlen(s) - 1] == c)
        s[strlen(s) - 1] = '\0';

    /* remove leading space */
    while (s[0] == c)
        /* use len of s to include \0 */
        memmove(s, s + 1, strlen(s));
}

void
trims(char *s, char *chars)
{
    for (char *c = chars; *c; c++)
        trim(s, *c);
}


/*******************************************************************************
 * Mappings
 ******************************************************************************/

/* template mappings: key -> value lists
 *
 * Passed as arguments to TMPL_BEGIN.
 */
pair_t template_map[MAX_KEYS];

/* template override mappings : key -> value
 *
 * Given via command line -D option. These overwrite template mapping values
 */
pair_t override_map[MAX_KEYS];

/* iteration mappings: key -> value
 *
 * These are the single values of the template mappings, potentially overriden
 * by override mappings. They are set at each iteration of a template block.
 *
 * They precede the persistent mappings.
 */
pair_t iteration_map[MAX_KEYS];

/* persistent mappings: key -> value
 *
 * Given via TMPL_MAP(key, value) commands outside of template blocks.
 *
 * These succeed the iteration mappings.
 */
pair_t persistent_map[MAX_KEYS];

/* block hooks */
pair_t block_hooks[MAX_KEYS];

void
remap(pair_t *map, const char *key, const char *val)
{
    if (key == NULL)
        return;
    for (int i = 0; i < MAX_KEYS; i++) {
        pair_t *p = map + i;
        if (!p->key[0] || strcmp(p->key, key) == 0) {
            memset(p->key, 0, MAX_KLEN);
            strcat(p->key, key);
            trim(p->key, ' ');

            memset(p->val, 0, MAX_VLEN);
            strcat(p->val, val);
            trim(p->val, ' ');
            debugf("[REMAP] %s = %s\n", p->key, p->val);
            return;
        }
    }
}

pair_t *
find(pair_t *map, const char *key)
{
    for (int i = 0; i < MAX_KEYS; i++) {
        pair_t *p = map + i;
        if (strcmp(p->key, key) == 0)
            return p;
    }
    return NULL;
}

void
unmap(pair_t *map, char *key)
{
    pair_t *p = find(map, key);
    if (p == NULL)
        return;

    memset(p->key, 0, MAX_KLEN);
    memset(p->val, 0, MAX_VLEN);
}

void
show(pair_t *map, const char *name)
{
    debugf("[SHOW MAP] %s\n", name);
    for (int i = 0; i < MAX_KEYS; i++) {
        pair_t *p = map + i;
        if (p->key[0]) {
            debugf("\t%s = %s\n", p->key, p->val);
        }
    }
}

void
clean(pair_t *map)
{
    memset(map, 0, sizeof(pair_t) * MAX_KEYS);
}

/*******************************************************************************
 * parse functions
 ******************************************************************************/

err_t
parse_assign(pair_t *p, char *start, char *end)
{
    char key[K_BUF_LEN] = {0};
    char val[V_BUF_LEN] = {0};

    char *comma = strstr(start, ",");
    if (comma == NULL)
        return ERROR("expected ','");
    start++;
    strncat(key, start, comma - start);
    comma++;
    strncat(val, comma, end - comma);
    remap(p, key, val);
    return NO_ERROR;
}

err_t
parse_template_map(char *start, char *end)
{
    char *next, *values;
    start++;
    *end = '\0';

again:
    next = strstr(start, ",");
    if (next) {
        *next = '\0';
        next++;
    }
    values = strstr(start, "=");
    if (values == NULL)
        return ERROR("expected '='");
    *values = '\0';
    values++;

    char key[K_BUF_LEN] = {0};
    strncat(key, start, MAX_KLEN);

    char val[V_BUF_LEN] = {0};
    strncat(val, values, strlen(values));
    trims(val, " []");

    remap(template_map, key, val);

    if (next) {
        start = next;
        goto again;
    }

    return NO_ERROR;
}

/*******************************************************************************
 * Line-based processing
 ******************************************************************************/
bool muted = false;

void
line_add_nl(char *line)
{
    const size_t len = strlen(line);
    if (line[len - 1] != '\n') {
        assert(len + 1 < MAX_SLEN);
        line[len]     = '\n';
        line[len + 1] = '\0';
    }
}

bool
line_apply(char *line, const char *key, const char *val)
{
    char *cur;

    if (!key[0] || (cur = strstr(line, key)) == NULL)
        return false;

    const size_t vlen = strlen(val);
    const size_t klen = strlen(key);
    const size_t slen = strlen(cur);

    const bool is_nl = strcmp(key, TMPL_NL) == 0;
    if (!is_nl)
        debugf("[APPLY] KEY: %s(%lu) VAL: %s(%lu)\n", key, klen, val, vlen);

    /* make space for value */
    if (!is_nl)
        debugf("\tBEFORE: %s", line);
    memmove(cur + vlen, cur + klen, slen);
    memcpy(cur, val, vlen);
    if (!is_nl)
        debugf("\tAFTER:  %s", line);

    return true;
}

bool
process_block_line(char *line)
{
    bool applied;
    char *cur;

    char buf[MAX_SLEN] = {0};
    strcat(buf, line);
    line_add_nl(buf);

    debugf("[LINE] %s", buf);
    int cnt = 0;
again:
    applied = false;

    for (int i = 0; i < MAX_KEYS && cnt < MAX_APPLY; i++) {
        /* should delete line? */
        if (strstr(buf, TMPL_DL)) {
            strcpy(buf, "");
            goto end;
        }
        if (strstr(buf, TMPL_SKIP))
            return false;

        const pair_t *pi = iteration_map + i;
        const pair_t *pp = persistent_map + i;
        if (!line_apply(buf, pi->key, pi->val) &&
            !line_apply(buf, pp->key, pp->val))
            continue;
        applied = true;
        cnt++;

        /* if one mapping is applied, restart testing all mappings */
        i = -1;
    }
    if (applied)
        goto again;

end:

    /* apply UPCASE */
    while ((cur = strstr(buf, TMPL_UPCASE))) {
        char *start = cur + strlen(TMPL_UPCASE);
        char sep    = start[0] == '(' ? ')' : start[0];
        char ssep[] = {sep, 0};
        char *end   = strstr(start + 1, ssep);
        assert(start && end && end > start);
        char *ch = start + 1;
        while (ch < end) {
            if (*ch >= 'a' && *ch <= 'z')
                *ch -= ('a' - 'A');
            ch++;
        }
        size_t len = (end - start) - 1;
        memmove(cur, start + 1, len);
        /* include the end of line */
        memmove(cur + len, end + 1, strlen(end));
    }

    /* apply NL */
    while (line_apply(buf, TMPL_NL, "\n"))
        ;

    /* output and return */
    printf("%s", buf);
    assert(cnt < MAX_APPLY);
    return true;
}

/*******************************************************************************
 * Block processing
 ******************************************************************************/

void
process_begin()
{
    debugf("============================\n");
    debugf("[BLOCK_BEGIN]\n");
    show(persistent_map, "persistent_map");
    show(block_hooks, "block_hooks");
    show(template_map, "template_map");
    debugf("----------------------------\n");
}

/*******************************************************************************
 * Block buffer
 *
 * Inside a template block, we buffer the whole block and then output the
 * content of hte buffer with mappings applied for each value of the mapping
 * iterators.
 ******************************************************************************/
char save_block[MAX_BLEN][MAX_SLEN];
int save_k;

const char *
sticking(const char *key)
{
    for (int i = 0; i < MAX_KEYS && strlen(override_map[i].key) != 0; i++)
        if (strcmp(override_map[i].key, key) == 0)
            return override_map[i].val;
    return NULL;
}

void
process_block(int i, const int nvars)
{
    pair_t *hook = NULL;
    if (i == nvars) {
        if ((hook = find(block_hooks, "begin")))
            if (!process_block_line(hook->val))
                return;

        for (int k = 0; k < save_k && process_block_line(save_block[k]); k++)
            ;

        if ((hook = find(block_hooks, "end")))
            (void)process_block_line(hook->val);

        return;
    }
    pair_t *p           = template_map + i;
    char val[V_BUF_LEN] = {0};
    strcat(val, p->val);

    const char *sval = sticking(p->key);
    const char *sep  = ";";
    char *saveptr    = NULL;
    char *tok        = strtok_r(val, sep, &saveptr);
    int c            = 0;
    while (tok) {
        trims(tok, " ");
        if (sval == NULL || strcmp(sval, tok) == 0) {
            (void)c;
            remap(iteration_map, p->key, tok);
            process_block(i + 1, nvars);
            unmap(iteration_map, p->key);
        }
        tok = strtok_r(0, sep, &saveptr);
    }
    if (i == 0 && (hook = find(block_hooks, "final"))) {
        (void)process_block_line(hook->val);
    }
}

/*******************************************************************************
 * File processing
 ******************************************************************************/

/* processing state */
enum state {
    TEXT,
    IGNORE_BLOCK,
    BLOCK_BEGIN,
    BLOCK_BEGIN_ARGS,
    BLOCK_TEXT,
    BLOCK_END,

    MAP,
    HOOK,
} S = TEXT;


err_t
process_line(char *line)
{
    char *cur = NULL;
    char *end = NULL;
    err_t err;

again:
    switch (S) {
        case TEXT:
            if (!muted && strstr(line, TMPL_ABORT)) {
                fflush(stdout);
                abort();
            }
            if (!muted && strstr(line, TMPL_BEGIN "(")) {
                S = BLOCK_BEGIN;
                goto again;
            }
            if (!muted && strstr(line, TMPL_MAP)) {
                S = MAP;
                goto again;
            }
            if (!muted && strstr(line, TMPL_HOOK)) {
                S = HOOK;
                goto again;
            }
            if (!muted && strstr(line, TMPL_MUTE)) {
                debugf("[OUTPUT] muted\n");
                muted = true;
                break;
            }
            if (strstr(line, TMPL_UNMUTE)) {
                debugf("[OUTPUT] unmuted\n");
                muted = false;
                break;
            }
            if (!muted && strstr(line, TMPL_DL) == NULL)
                printf("%s", line);
            break;

        case BLOCK_BEGIN:
            clean(template_map);
            cur = strstr(line, "(");
            if (cur == NULL)
                return ERROR("expected '('");
            S    = BLOCK_BEGIN_ARGS;
            line = cur;
            goto again;

        case BLOCK_BEGIN_ARGS:
            if ((end = strstr(line, ")"))) {
                err_t err = parse_template_map(line, end);
                if (IS_ERROR(err))
                    return err;
                S = BLOCK_TEXT;
                process_begin();
            } else {
                if ((end = strstr(line, ",")) == NULL)
                    return ERROR("expected ','");
                for (char *e = NULL; (e = strstr(end + 1, ","), e && e != end);
                     end     = e)
                    ;
                parse_template_map(line, end);
            }
            break;

        case BLOCK_TEXT:
            cur = strstr(line, TMPL_END);
            if (cur != NULL) {
                S = BLOCK_END;
                goto again;
            }
            if (save_k >= MAX_BLEN)
                return ERROR("block too long");

            memcpy(save_block[save_k++], line, strlen(line) + 1);
            break;
        case BLOCK_END:
            /* consume */
            {
                int nvars = 0;
                for (int i = 0; i < MAX_KEYS; i++)
                    if (template_map[i].key[0])
                        nvars++;
                process_block(0, nvars);
            }
            save_k = 0;
            S      = TEXT;
            break;

        case MAP:
            if ((cur = strstr(line, "(")) == NULL)
                return ERROR("expected '('");
            if ((end = strstr(cur, ")")) == NULL)
                return ERROR("expected ')'");
            for (char *e = NULL; (e = strstr(end + 1, ")"), e && e != end);
                 end     = e)
                ;
            err = parse_assign(persistent_map, cur, end);
            if (IS_ERROR(err))
                return err;
            S = TEXT;
            break;

        case HOOK:
            if ((cur = strstr(line, "(")) == NULL)
                return ERROR("expected '('");
            if ((end = strstr(cur, ")")) == NULL)
                return ERROR("expected ')'");
            err = parse_assign(block_hooks, cur, end);
            if (IS_ERROR(err))
                return err;
            S = TEXT;
            break;

        default:
            assert(0 && "invalid");
    }
    return NO_ERROR;
}

/*******************************************************************************
 * File processing
 ******************************************************************************/

void
process_file(const char *fn)
{
    FILE *fp = fopen(fn, "r+");
    assert(fp);

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    err_t err;
    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        assert(line);
        err = process_line(line);
        if (IS_ERROR(err)) {
            fprintf(stderr, "%s:%d: error: %s\n", fn, i + 1, err.msg);
            abort();
        }
        if (line) {
            free(line);
            line = NULL;
        }
        i++;
    }
    if (line)
        free(line);

    fclose(fp);
}

/*******************************************************************************
 * main function with options
 *
 * $0 [-v] <FILE> [FILE]...
 ******************************************************************************/
int
main(int argc, char *argv[])
{
    debugf("vatomic generator\n");
    int c;
    char *k;
    while ((c = getopt(argc, argv, "hvD:")) != -1) {
        switch (c) {
            case 'D':
                k    = strstr(optarg, "=");
                *k++ = '\0';
                remap(override_map, optarg, k);
                break;
            case 'v':
                _verbose = true;
                break;
            case 'h':
                printf("tmplr - a simple templating tool\n\n");
                printf("Usage:\n\ttmplr [FLAGS] <FILE> [FILE ...]\n\n");
                printf("Flags:\n");
                printf("\t-v            verbose\n");
                printf(
                    "\t-Dkey=value   override template map assignement of "
                    "key\n");
                exit(0);
            case '?':
                printf("error");
                exit(1);
            default:
                break;
        }
    }
    for (int i = optind; i < argc; i++)
        process_file(argv[i]);

// if started as script, remove tmplr file
#ifdef SCRIPT
    return remove(argv[0]);
#else
    return 0;
#endif
}
