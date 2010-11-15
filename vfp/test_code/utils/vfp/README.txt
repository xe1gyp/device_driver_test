vfptest.c

1) Use below compile options to create executable
arm-none-linux-gnueabi-gcc vfptest.c -mfpu=vfpv3 -mfloat-abi=softfp -o vfptest.out clear

2) Copy this "vfptest.out" to your file system.

3) Execute it :  ./vfptest.out

If this test runs without throwing any illegal instruction or page falut error means NEON engine is active and can perform different neon istrution in the system.


NOTE: Kernel should be built with VFP enabled.

To enable VFP support in kernel.

1) make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- omap_3630sdp_defconfig

2) Enable VFP support

  make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- menuconfig
	 Floating point emulation  --->
		[*] VFP-format floating point maths

3) make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- uImage -j4

   Your Kernel has now VFP support
