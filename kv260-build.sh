#!/bin/bash

set -e

[ -e .config ] || {
	make xilinx_zynqmp_kria_defconfig
}

make -j$(nproc) CROSS_COMPILE="ccache aarch64-linux-gnu-"

cp u-boot.elf kv260-bootgen
cd kv260-bootgen
./bootgen -image bootgen.bif -arch zynqmp -r -w -o xilinx_boot.bin
cd ..
echo "KV260 image is ready and may be flashed using the KV260 embedded web server:"
ls -l kv260-bootgen/xilinx_boot.bin

