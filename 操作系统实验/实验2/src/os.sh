#!/bin/bash

output_file="Condor_os.img"
asm_files=("bootloader" "oskernel" "b" "a" "c" "d")


rm -f ${output_file}

for asm_file in ${asm_files[@]}
do
	nasm ${asm_file}.asm -o ${asm_file}.img
    cat ${asm_file}.img >> "${output_file}"
    rm -f ${asm_file}.img
    echo "${asm_file} finished"
done

echo "${output_file} finished."