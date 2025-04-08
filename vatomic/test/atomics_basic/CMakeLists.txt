# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT
# ##############################################################################
# basic atomics test
# ##############################################################################

# functions to test
set(TC_FUNCS
    read
    write
    init
    add
    sub
    inc
    dec
    or
    xor
    and
    max
    xchg
    cmpxchg)

# number of arguments beyond the pointer to the atomic variable
set(NARGS_init 1)
set(NARGS_write 1)
set(NARGS_read 0)
set(NARGS_xchg 1)
set(NARGS_cmpxchg 2)
set(NARGS_add 1)
set(NARGS_sub 1)
set(NARGS_or 1)
set(NARGS_xor 1)
set(NARGS_and 1)
set(NARGS_max 1)
set(NARGS_inc 0)
set(NARGS_dec 0)

# types of the function
#
# * READ: returns the old value #
# * WRITE: writes the value, returns nothing
# * X: applies RMW operation, eg, sub, add, ...
# * GETX: applies the operation and returns old value
# * XGET: applies the operation and returns new value
set(FTYPE_xchg READ)
set(FTYPE_cmpxchg READ)
set(FTYPE_read READ)
set(FTYPE_write WRITE)
set(FTYPE_init WRITE)
set(FTYPE_add X GETX XGET)
set(FTYPE_sub X GETX XGET)
set(FTYPE_inc X GETX XGET)
set(FTYPE_dec X GETX XGET)
set(FTYPE_or X GETX XGET)
set(FTYPE_xor X GETX XGET)
set(FTYPE_and X GETX XGET)
set(FTYPE_max X GETX XGET)

# variants of the function (atomic types accepted)
set(FATOMIC_xchg atomic8 atomic16 atomic32 atomic64 atomicsz atomicptr)
set(FATOMIC_cmpxchg atomic8 atomic16 atomic32 atomic64 atomicsz atomicptr)
set(FATOMIC_read atomic8 atomic16 atomic32 atomic64 atomicsz atomicptr)
set(FATOMIC_write atomic8 atomic16 atomic32 atomic64 atomicsz atomicptr)
set(FATOMIC_init atomic8 atomic16 atomic32 atomic64 atomicsz atomicptr)
set(FATOMIC_add atomic8 atomic16 atomic32 atomic64 atomicsz)
set(FATOMIC_sub atomic8 atomic16 atomic32 atomic64 atomicsz)
set(FATOMIC_inc atomic8 atomic16 atomic32 atomic64 atomicsz)
set(FATOMIC_dec atomic8 atomic16 atomic32 atomic64 atomicsz)
set(FATOMIC_or atomic8 atomic16 atomic32 atomic64 atomicsz)
set(FATOMIC_xor atomic8 atomic16 atomic32 atomic64 atomicsz)
set(FATOMIC_and atomic8 atomic16 atomic32 atomic64 atomicsz)
set(FATOMIC_max atomic8 atomic16 atomic32 atomic64 atomicsz)

# interface variants
#
# * core: main interface with vatomic32, vatomic64 and vatomicptr
# * dispatch: core + dispatcher-based vatomic_ interface
# * compat: dispatch + backwards-compatible atomic*_ interface
if(LIBVSYNC_OPEN_DISTRO_TESTING)
    set(TC_VARIANTS core)
elseif(LIBVSYNC_DRC_DISTRO_TESTING)
    set(TC_VARIANTS core dispatch)
else()
    set(TC_VARIANTS core dispatch compat)
endif()

set(HEADER_core vsync/atomic/core.h)
set(HEADER_dispatch vsync/atomic/dispatch.h)
set(HEADER_compat vsync/atomic/compat.h)

foreach(TC_VARIANT ${TC_VARIANTS})
    set(TC_ATOMIC_HEADER ${HEADER_${TC_VARIANT}})

    foreach(TC_FUNC ${TC_FUNCS})
        foreach(TC_ATOMIC ${FATOMIC_${TC_FUNC}})

            if(${TC_VARIANT} STREQUAL compat)
                if(${TC_ATOMIC} STREQUAL atomic8)
                    continue()
                endif()
                if(${TC_ATOMIC} STREQUAL atomic16)
                    continue()
                endif()
                if(${TC_ATOMIC} STREQUAL atomicsz)
                    continue()
                endif()
                if(${TC_FUNC} STREQUAL max)
                    continue()
                endif()
                set(TC_TYPE_PRFX ${TC_ATOMIC})
                set(TC_FVARIANT ${TC_ATOMIC})
            elseif(${TC_VARIANT} STREQUAL core)
                set(TC_TYPE_PRFX v${TC_ATOMIC})
                set(TC_FVARIANT v${TC_ATOMIC})
            else()
                if(${TC_ATOMIC} STREQUAL atomic8)
                    continue()
                endif()
                if(${TC_ATOMIC} STREQUAL atomic16)
                    continue()
                endif()
                if(${TC_ATOMIC} STREQUAL atomicsz)
                    continue()
                endif()
                set(TC_TYPE_PRFX v${TC_ATOMIC})
                set(TC_FVARIANT vatomic)
            endif()

            foreach(FT ${FTYPE_${TC_FUNC}})
                set(TC_FUNC_SUFX)
                set(TC_FUNC_PRFX)
                set(TC_XGET 0)
                set(TC_GETX 0)
                set(TC_NARGS ${NARGS_${TC_FUNC}})

                if(${FT} STREQUAL GETX)
                    set(TC_FUNC_PRFX get_)
                    set(TC_GETX 1)
                elseif(${FT} STREQUAL XGET)
                    set(TC_FUNC_SUFX _get)
                    set(TC_XGET 1)
                elseif(${FT} STREQUAL READ)
                    set(TC_GETX 1)
                endif()

                if(${TC_VARIANT} STREQUAL compat)
                    if(${FT} STREQUAL XGET)
                        continue()
                    elseif(${FT} STREQUAL X)
                        set(TC_XGET 1)
                    endif()
                endif()

                set(TC
                    ${TC_VARIANT}-v${TC_ATOMIC}_${TC_FUNC_PRFX}${TC_FUNC}${TC_FUNC_SUFX}
                )
                configure_file(vatomic_basic.c.in ${TC}.c)
                add_executable(${TC} ${TC}.c)
                if(${TC_VARIANT} STREQUAL compat)
                    target_compile_definitions(
                        ${TC} PRIVATE VSYNC_DISABLE_DEPRECATED_WARNING)
                endif()
                target_link_libraries(${TC} vatomic pthread)

                v_add_bin_test(NAME ${TC} COMMAND ${TC})
            endforeach()
        endforeach()
    endforeach()
endforeach()
