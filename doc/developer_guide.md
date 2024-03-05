# libvsync Developer Guide

[[_TOC_]]

libvsync is a header-only library. Any new algorithm needs to be in a header file.

Let us assume you want to add a new algorithm called `algorithm.h`. You then add `algorithm.h`
to one of the subdirectories of `include` folder. The subdirectories determine the group
to which the algorithm belongs. If `algorithm.h` belongs to a new category, then you need to add a new
subdirectory with a descriptive name of that category.

`algorithm.h` shall contain the main functions the user is going to call. Some algorithms
are be too long for one header file, or they can be modularized. If that is the case with
`algorithm.h`, you can add extra headers that you include in `algorithm.h` and place under
`internal` subdirectory of the category. For example if `algorithm.h` is a queue implementation
it will belong to `<vsync/queue/algorithm.h>` and other internal modules will be under
`<vsync/queue/internal/algorithm/algorithm.h>`.

Check [the current categories.](api/vsync/README.md).

### Header guard

Your whole header file should be guarded with a header macro to avoid double includes in user code:

```c
#ifndef VALGO_H
#define VALGO_H

// all functions and types

#endif
```

### Add header documentation as follows:

Your header documentation should include:

- `@file algorithm.h`
- `@brief text` with a brief description of the library. Text starts with a noun or adjective in caps and ends with period.
- A description with some more detailed information about the library. Starts with caps; ends with period.
- `@ingroup` list the groups that your algorithm belong to.
- `@cite` followed by a new line and one reference per line after that. Authors and reference name must be present. If book, give precise reference using `[X.Y.Z]`. If paper, give a link to the paper using `Authors - [Paper name](link)`. E.g.,
```c
@cite
- Authors 1 - Book [X.Y.Z]
- Authors 2 - [Paper](http://...)
```
A full example of what is expected to exist in your header file
```c
/*******************************************************************************
 * @file  algorithm.h
 * @brief one sentence that describes the algorithm.
 *
 * @ingroup lock_free // if your algo belongs to certain groups you
 *                    // can list them here separated with spaces
 *
 *
 * // add detailed information about the algorithm
 * // operating conditions etc.
 *
 *
 * @example
 * @include eg_filename.c
 *
 * @example
 * ```c
 * #include <vsync/dir/algorithm.h>
 *
 * ```
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 10.5]
 ******************************************************************************/
```

## Code style and documentation

- Variable names, types and function names follow [snake case](https://en.wikipedia.org/wiki/Snake_case). Use descriptive names that indicate what the functions/variables are used for.
- "Global names to be precise, local names to be concise", avoid one `char` names but keep local names short (8 char max]).
- Function names and types should have a common prefix that makes them unique, in addition to `v` our trademark prefix.
```C
typedef struct valog_s {
    // fields
} valog_t;

static inline void valgo_init(valog_t *algo) {

    algo->... = ...;
}
- Mark header functions as `static inline`. Current libvsync defines all header functions as `static inline`, however, this might change in the future.
- Do **not** define global or thread local variables (e.g. using `__thread`) in your header.
- All additional debugging related code must be guarded by macros that are turned off by default. For printing use `<vsync/commong/dbg.h>`.

```
- Private functions, which users should not use directly, must have an additional `_` prefix.
```c
static inline void _valgo_calc(valog_t *algo) { }
```
- Functions must be documented using ` /** ... */ `. The description should start with a third person singular verb in caps (e.g. "Creates and initializes the required ...") and end with a period.
- Parameter must be prefixed with `@param` and the description must end with period.
- Functions having a return type must be prefixed with `@return` and the description must end with period.
- If the API should be used (or not used) in some special way, this must be documented with `@note`. Other remarks can be used using the same annotation.
```c
/**
 * Initializes ... .
 *
 * // add extra details leave an empty line from the above.
 * @note not thread safe.
 * @param algo address of valog_t object.
 * @return true ... successful.
 * @return false ... failed.
 */
static inline vbool_t valgo_init(valog_t *algo) {
}
```
- Always use curly braces for single statement blocks
```c
if(cond) {
    print(...);
}

if(con) {
    return ...;
}
```
- Use `vsize_t` for array index whenever possible
- Do not use magic numbers, define a macro for such numbers
```c
#define V_ARR_LEN
vuint32_t g_arr[V_ARR_LEN];
for(vsize_t i = 0; i <V_ARR_LEN ; i++) {
    g_arr[i] = 0;
}
```
- For assertions inside header files use `ASSERT` from `<vsync/common/assert.h>` do not use `assert` directly
- It is recommended to satisfy the following limits (this is not a strict requirement):
  - maximum number of local variables 7
  - maximum number of parameters 6
  - maximum number of lines per function 50
  - maximum number of lines per header file 500
  - avoid nesting a code block more than four times within a function


## Using Macros

Simple rules about macros:

- Macros must be all in caps and words separated with `_`, e.g., `MACRO_NAME`.
- Multiline macro bodies should be surrounded with `do{A;B;C} while(0)`, if it is to be evaluated in a condition or used as a return statement use  `({A;B;C;})`

```c
#define VALGO_STMT(_p_) do{ ...; fun(_p_) } while(0)
#define VALGO_COND(_p_) ({...; fun(_p_) == 0})
```

- Undefine the macro at the end, if its usage don't go beyond the scope of the file
```c
#undef VALGO_STMT
#undef VALGO_COND
```

- Macro params should be surrounded with `(p)`.
- Don't add `;` at the end of macros


## Standard headers

Developers can only use a subset of standard headers, check the bellow list:

- Check allowed [standard headers](http://port70.net/~nsz/c/c99/n1256.html#4p6)
- Exception: use `<vsync/vtypes.h>` instead of <font color="red"> `<stdint.h>, <stdbool.h> <inttypes.h> <stddef.h>` </font>
- Exception: use `<vsync/utils/string.h>` instead of  <font color="red"> `<string.h>` </font>
- Exception: use `ASSERT` from `<vsync/common/assert.h>` instead of  <font color="red"> `assert` from `<assert.h>` </font>
- do NOT include  <font color="red"> `<stdlib.h>` </font>, for allocation use the abstraction `vmem_lib_t` in `<vsync/common/alloc.h>`
- do NOT include  <font color="red"> `<stdio.h>` </font>, for debugging messages you can use  `<vsync/common/dbg.h>`
- do NOT include system dependent headers like `time.h` or `pthread.h` and so on.


## Types

- For integer types use known width types e.g. don't use `unsigned int`, use `vuint32_t` or `vuint16_t` depending on your needs.
- Avoid using `void*` as much as possible and use well defined pointer types instead.

## Memory Allocation

- Avoid memory allocation at all costs. You can assume the memory objects given to you as a parameter by
the user. In libvsync we rely on the embedding  approach. Your data structure object will be a part of
the user object. Users then can use it as follows:

```c
typedef struct user_obj_s {
    // other stuff
    valog_node_t embededd_obj;
} user_obj_t;

int main(void) {
    valog_t algo;
    user_obj_t obj = // init;
    valgo_push(&algo,  &obj.embedded_obj);
    valog_node_t *node = valgo_pop(&algo);
    user_obj_t out_obj = container_of(node, user_obj_t, embededd_obj);
    return 0;
}
```

- If memory allocation cannot be avoided with the above approach, then abstract from the allocator.
Instead of using `malloc` or `free` directly, accept function pointers to use in their place and ask the user to implement the specification. see <vsync/utils/alloc.h> you can accept a parameter of type `vmem_lib_t *`.

## Code Format
- The accepted line width for the code is `80` characters. Instead of bothering how to format your code you can run
```bash
# to format code
make clang-format-apply
# to format cmake files
make cmake-format-apply
```
- Comment chars should be followed/preceded with a space
instead of:
```c
//comment
/*comment*/
```
do
```c
// comment
/* comment */
```

- Do not add blank lines at the end of code block defined by braces.
instead of
```c
while(true) {
    // code

}
```
do
```c
while(true) {
    // code
}
```

- You run the following script to address the above two points
```bash
scripts/sanitize.sh
```

- Do not use multiple empty lines, if you need to separate the code use comments e.g.
```c
/******************************************************************************
 * The following section is for internal functions
 *****************************************************************************/
```

- Assert one thing at a time
```c
// instead of
ASSERT(cond1 && cond2);
// assert each alone
ASSERT(cond1);
ASSERT(cond2);
```

## Newline at end of file
Your files should end with a newline, you can configure your editor to add it automatically on save.
```
#undef /*the last line of your header followed by a newline */

```

## Testing
Please add unit and stress tests under libvsync/test folder. The test folder mirrors include folder
in its structure. Thus, you should add your tests to the appropriate subdirectory of test.


## Contributing

Please contact us at `vsync AT huawei DOT com` if you wish to merge contributions.
