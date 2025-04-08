#!/bin/bash
set -e

CC_ARM32_BE=$ARMEB_PATH/armeb-linux-gnueabi-gcc
CC_ARM32=arm-linux-gnueabi-gcc
CC_AARCH64=aarch64-linux-gnu-gcc
CC_X86_64=gcc

QEMU_ARM="qemu-arm -L /usr/arm-linux-gnueabi"
QEMU_ARM_BE="qemu-armeb -L /usr/armeb-linux-gnueabi"
QEMU_X86_64="qemu-x86_64 -L /usr/bin/gcc"
QEMU_ARRCH64="qemu-aarch64 -L /usr/aarch64-linux-gnu"


TEST_TEMPLATE="atomics-test-master.c"
TIMEOUT_MSG="TIMEOUT"


CFLAGS="-I../include -g -std=c99 -Wall -Wextra -pedantic -ffreestanding -nostdlib -Wl,--defsym,_start=main -Wa,-L -O2"
CFLAGS="${CFLAGS} -DVSYNC_DISABLE_POLITE_AWAIT"
CFLAGS="${CFLAGS} -DVSYNC_ENABLE_ATOMIC_MAPPING"


# supported arch names, used in output files' names
declare -a archs
archs=("arm7" \
	"arm8" \
	"aarch64" \
	"aarch64-lse" \
	"aarch64-c11" \
	"aarch64-builtins" \
	"x86-c11" \
	"x86-builtins" \
	"arm7-be" \
	"arm8-be" \
	"armv7-le-builtins" \
	"armv7-be-builtins" \
)

arch_len=${#archs[@]}


# compilers
declare -a compilers
declare -a run
declare -a defines

compilers=(\
	$CC_ARM32 \
	$CC_ARM32 \
	$CC_AARCH64 \
	$CC_AARCH64 \
	$CC_AARCH64 \
	$CC_AARCH64 \
	$CC_X86_64 \
	$CC_X86_64 \
	$CC_ARM32_BE \
	$CC_ARM32_BE \
	$CC_ARM32 \
	$CC_ARM32_BE \
)
run[0]=$QEMU_ARM
run[1]=$QEMU_ARM
run[2]=$QEMU_ARRCH64
run[3]=$QEMU_ARRCH64
run[4]=$QEMU_ARRCH64
run[5]=$QEMU_ARRCH64
run[6]=$QEMU_X86_64
run[7]=$QEMU_X86_64
run[8]=$QEMU_ARM_BE
run[9]=$QEMU_ARM_BE
run[10]=$QEMU_ARM
run[11]=$QEMU_ARM_BE


defines=("-DVSYNC_ATOMIC_ARMV7"      \
	"-DVSYNC_ATOMIC_ARMV8"       \
	""                           \
	"-DVSYNC_ATOMIC_AARCH64_LSE" \
	"-DVSYNC_STDATOMIC"          \
	"-DVSYNC_BUILTINS"           \
	"-DVSYNC_STDATOMIC"          \
	""                           \
	"-DVSYNC_ATOMIC_ARMV7"       \
	"-DVSYNC_ATOMIC_ARMV8"       \
	"-DVSYNC_BUILTINS"           \
	"-DVSYNC_BUILTINS")
# compiler options for the used arch.


# -marm option is necessary to not get this error Error: thumb conditional instruction should be in IT block -- `cmpeq r7,fp'
# -static is necessary so that it does not complain about ld-linux.so.3 etc

declare -a options
options[0]="-O2 -march=armv7-a -mno-thumb-interwork -mfpu=vfp -mcpu=cortex-a9 -pthread"
options[1]="-O2 -march=armv8-a -mno-thumb-interwork -mfpu=vfp -pthread"
options[2]="-O2 -pthread"
options[3]="-O2 -pthread -march=armv8.2-a+lse"
options[4]="-O2 -pthread "
options[5]="-O2 -pthread "
options[6]="-O2 -pthread "
options[7]="-O2 -pthread "
options[8]="-static -O2 -march=armv7-a -marm -mno-thumb-interwork -mfpu=vfp  -mcpu=cortex-a9 -msoft-float -pthread -mbig-endian"
options[9]="-static -O2 -march=armv8-a -marm -mno-thumb-interwork -mfpu=vfp  -pthread -mbig-endian"
options[10]="-O2 -march=armv7-a -mno-thumb-interwork -mfpu=vfp  -pthread -mtune=cortex-a9 -mlittle-endian"
options[11]="-static -O2 -march=armv7-a -marm -mno-thumb-interwork -mfpu=vfp  -pthread -mtune=cortex-a9 -mbig-endian"

case "$1" in
	"-clean")
	rm -rf build
	mkdir build
;;
	"-compile")
	PRINT_CSV=""
	case "$2" in
		"-csv")
		PRINT_CSV="-DPRINT_CSV"
	;;
	esac
	rm -rf build
	mkdir build
	for ((i=0; i< arch_len; i++)); do
		for t in atomic32 atomic64 atomicptr
		do
			echo "Compiling ${archs[$i]}-$t"
			#expand
			expansion_input_file=$TEST_TEMPLATE
			expansion_output_file=build/test-$t-${archs[$i]}.c
			expansion_asm_file=build/test-$t-${archs[$i]}.s

			${compilers[$i]} -E $CFLAGS -DTYPE=$t $PRINT_CSV -DTYPE_IS_$t ${defines[$i]} $expansion_input_file -o $expansion_output_file
			#compile
			compilation_output_file=build/test-$t-${archs[$i]}.out
			${compilers[$i]} -DTYPE=$t ${options[$i]} $expansion_output_file  -o $compilation_output_file

			compilation_output_file=build/test-$t-${archs[$i]}.out
			${compilers[$i]} -DTYPE=$t ${options[$i]} $expansion_output_file  -S -o $expansion_asm_file
		done
	done
	chmod +x build/*.out
;;
	"-run-all")
		NO_NUMA=1
		NO_CPU=12
		NO_ITERATIONS=1000
		OFFSET=0
		NO_THREAD=5
		for thrdno in 1 2 5; do # threads
			for i in {0..6}; do # ARCHs
				for file in build/*${archs[$i]}.out
				do
					timeout 10s  ${run[$i]} ./$file $thrdno $NO_ITERATIONS $NO_CPU $NO_NUMA $OFFSET $api $mo || echo $TIMEOUT_MSG

				done # files
			done # archs
		done #threads
;;
	"-run-sanity")
		for thrdcount in 1 10; do
			for iter in 1 15 30 31 32 60 61 64 1000; do
				for ((i=0; i< arch_len; i++)); do # ARCHs
					for file in build/*${archs[$i]}.out
					do
						timeout 10s  ${run[$i]} ./$file $thrdcount $iter || echo $TIMEOUT_MSG
					done
				done
			done
		done
;;
	"-run-max")
		for ((i=0; i< arch_len; i++)); do # ARCHs
			for file in build/*${archs[$i]}.out
			do
# 				# timeout 120s
				${run[$i]} ./$file 500 5
				#|| echo -n $TIMEOUT_MSG
			done
		done
;;
	"-run-min-32")
		for ((i=0; i< arch_len; i++)); do # ARCHs
			for file in build/test-atomic32-${archs[$i]}.out
			do
				timeout 10s  ${run[$i]} ./$file 1 1 || echo $TIMEOUT_MSG
			done
		done
;;
	"-test-few")
		for thrdcount in 1 3; do
			for ((i=0; i< arch_len; i++)); do
				for file in build/*${archs[$i]}.out
				do
					err=0
					if ! timeout 10s  ${run[$i]} ./$file $thrdcount 1000; then
						echo $TIMEOUT_MSG file=$file threads=$thrdcount
						exit 1
					fi
				done
			done
		done
;;
	"-run-sub")
		suit=20
		for thrdcount in 10; do
			for iter in 1 15 30 31 32 60 61 64 1000; do
				for ((i=0; i< arch_len; i++)); do
					for file in build/test-atomic64-${archs[$i]}.out
					do
					timeout 10s	${run[$i]} ./$file $thrdcount $iter 12 1 0 $suit || echo $TIMEOUT_MSG
					done
				done
			done
		done
;;
	"-run-be")
		suit=20
		for thrdcount in 10; do
			for iter in 64; do
				for i in {6..7}; do
					for file in build/test-atomic64-${archs[$i]}.out
					do
						${run[$i]} ./$file $thrdcount $iter 12 1 0 $suit
					done
				done
			done
		done
;;
	*)
	echo "Unknown Command! available commands -clean, -compile <-csv>, -run-all, -run-sanity, -run-min-32, -run-sub, -run-be"
esac
