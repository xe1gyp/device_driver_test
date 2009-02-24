neon_test.s

1) Use below compile options to cretae executable.
arm-none-linux-gnueabi-gcc neon_test.s -march=armv7a -mfpu=neon -mfloat-abi=softfp -o neon_test.out

2) Copy this "neon_test.out" to your file system.

3) Execute it :  ./neon_test.out

If this test runs without throwing any illegal instruction or page falut error means NEON engine is active and can perform different neon istrution in the system.


NOTE:
Kernel should be build with NEON enabled. Also use gcc version 4.2.3 (Sourcery
G++ Lite 2008q1-126) and above for NEON

To enable NEON support in kernel.

1)  make CROSS_COMPILE=arm-none-linux-gnueabi- omap_3430sdp_defconfig

2)Enable NEON supprt.

  make menuconfig

	 Floating point emulation  --->
		[*]   Advanced SIMD (NEON) Extension support
                [*]   NEON L1 cache bug workaround (erratum 451034)

3) make CROSS_COMPILE=arm-none-linux-gnueabi- uImage

   Your Kernel has now neon support
