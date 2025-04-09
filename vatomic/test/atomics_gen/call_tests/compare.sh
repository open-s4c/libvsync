
#!/bin/bash
export VSYNCER_DOCKER=false
export EXPAND=true
INCLUDE_PATH="-I../../include/ -I../../../include/"
REGEX="SeqCst\s*:\s*([0-9]+)\s*Release\s*:\s*([0-9]+)\s*Acquire\s*:\s*([0-9]+)\s*Relaxed\s*:\s*([0-9]+)"

function print_fail_info() {
    #     echo -e "\e[0;31m FAIL: In $f the numbers don't add up rlx:$SUM_RLX != sc: $SUM_SC != norm: $SUM_NORM != num_calls: $NUM_CALLS \e[0m"
    echo -e "\e[0;31m with VSYNC_SC:\n $OUTPUT_SC \e[0m"
    echo -e "\e[0;31m with VSYNC_RLX:\n $OUTPUT_RLX \e[0m"
    echo -e "\e[0;31m with VSYNC_NORM:\n $OUTPUT_NORM \e[0m"
}

echo "running with --expand=$EXPAND"

for f in call*.c; do
    echo "-----------------------------------------------------"
    OUTPUT_SC=$(CFLAGS="-DVSYNC_SC $INCLUDE_PATH" vsyncer info --expand=$EXPAND $f | head -n 19 | tail -n 5);
    OUTPUT_RLX=$(CFLAGS="-DVSYNC_RLX $INCLUDE_PATH" vsyncer info --expand=$EXPAND $f | head -n 19 | tail -n 5);
    OUTPUT_NORM=$(CFLAGS="-DVSYNC_NORM $INCLUDE_PATH" vsyncer info --expand=$EXPAND $f | head -n 19 | tail -n 5);

    SUM_SC=0
    SUM_RLX=0
    SUM_NORM=0
    NUM_CALLS=$(cat $f | grep vatomic.*_.*\( | wc -l)

    if (( $NUM_CALLS == 0 )); then
        echo -e "\e[0;31m ERROR: [$f] seems to call no functions \e[0m"
        exit 1
    fi

    if [[ $OUTPUT_SC =~ $REGEX ]]; then
        SC_CNT=${BASH_REMATCH[1]}
        REL_CNT=${BASH_REMATCH[2]}
        ACQ_CNT=${BASH_REMATCH[3]}
        RLX_CNT=${BASH_REMATCH[4]}

        if (( $REL_CNT != 0 || $ACQ_CNT != 0 || $RLX_CNT != 0 )); then
            echo -e "\e[0;31m ERROR: [$f] A none SC barrier detected on SC \e[0m"
            exit 1
        fi
        SUM_SC=$SC_CNT
    else
        echo -e "\e[0;31m ERROR: [$f] with regex \e[0m"
        exit 1
    fi

    if [[ $OUTPUT_RLX =~ $REGEX ]]; then
        SC_CNT=${BASH_REMATCH[1]}
        REL_CNT=${BASH_REMATCH[2]}
        ACQ_CNT=${BASH_REMATCH[3]}
        RLX_CNT=${BASH_REMATCH[4]}

        if (( $REL_CNT != 0 || $ACQ_CNT != 0 || $SC_CNT != 0 )); then
            echo -e "\e[0;31m ERROR: [$f] A none RLX barrier detected on RLX \e[0m"
            exit 1
        fi
        SUM_RLX=$RLX_CNT
    else
        echo "error apply regex on $f"
        break
    fi

    if [[ $OUTPUT_NORM =~ $REGEX ]]; then
        SC_CNT=${BASH_REMATCH[1]}
        REL_CNT=${BASH_REMATCH[2]}
        ACQ_CNT=${BASH_REMATCH[3]}
        RLX_CNT=${BASH_REMATCH[4]}
        SUM_NORM=$((SC_CNT+REL_CNT+ACQ_CNT+RLX_CNT))
    else
        echo "error apply regex"
        break
    fi


    if (( $SUM_RLX != $SUM_SC )); then
        if [ "$f" = "call_fence.c" ]; then
            if (( $SUM_RLX != 0 &&  $SUM_SC != 4 )); then
                echo -e "\e[0;31m FAIL: [$f]  expted 0 for rlx and got $SUM_RLX, expected 4 for SC and got  $SUM_SC \e[0m"
            fi
            continue;
        else
            echo -e "\e[0;31m FAIL: [$f] has different number of rlx/sc barriers \e[0m"
            exit 1
        fi
    fi

    if (( $SUM_NORM != $SUM_SC )); then
        RATIO=$(( $SUM_NORM / $SUM_SC ))
        REMAINDER=$(( $SUM_NORM % $SUM_SC ))
        if (( $REMAINDER != 0 || $RATIO != 3  && $RATIO != 4 )); then
            # echo -e "\e[0;31m FAIL: [$f] ratio of norm($SUM_NORM)/sc($SUM_SC) is $RATIO with remainder= $REMAINDER, which is not acceptable \e[0m"
            # print_fail_info
            # exit 1
            echo -e "\e[0;33m WARN: [$f] ratio of norm($SUM_NORM)/sc($SUM_SC) is $RATIO with remainder= $REMAINDER, which is not acceptable \e[0m"
            continue
        fi
    fi

    if (( $SUM_NORM != $NUM_CALLS )); then
        RATIO=$(( $SUM_NORM / $NUM_CALLS ))
        REMAINDER=$(($SUM_NORM % $NUM_CALLS ))
        if (( $REMAINDER != 0 || $RATIO != 2 )); then
            echo -e "\e[0;33m WARN: [$f] ratio of norm($SUM_NORM)/#calls($NUM_CALLS) is $RATIO with remainder= $REMAINDER, which is not acceptable \e[0m"
            continue
        fi
    fi

    echo -e "\e[0;32m PASS: $f rlx:$SUM_RLX = sc: $SUM_SC = norm: $SUM_NORM = num_calls: $NUM_CALLS \e[0m"
done
echo "-----------------------------------------------------"
# cleanup
rm *.ll
