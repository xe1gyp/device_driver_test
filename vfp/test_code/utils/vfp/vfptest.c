
/*********************************************************************
 * ARM Vector Floating Point  Test Scope
 *********************************************************************
 *
 * 1. Register accesses
 *    - VFP to ARM register transfer
 *    - ARM to VFP register transfer
 *
 * 2. IEEE Standard mode Single and Double precision floating point operations
 *    ABS, NEG, ADD, SUB, MUL, NMUL, DIV, SQRT, MAC, NMAC, MSC, NMSC
 *    - Scalar operation
 *    - Vector operation
 *
 * 3. VFPv3 specific operations
 *    FCONST, FSHTO, FSLTO, FUHTO, FULTO
 *
 * 4. VFP operations which cause bounces
 *
 * 5. VFP rounding mode
 *
 * 6. VFP context save
 *
 * 7. VFP mode inheritance
 *
 * 8. VFP RunFast mode
 */
#include <linux/types.h>
#include <stdio.h>

#define __VFPV3_SUPPORTED__

enum {
	vfpAbs = 0,
	vfpNeg,
	vfpSqrt,
	vfpAdd,
	vfpSub,
	vfpMul,
	vfpNMul,
	vfpDiv,
	vfpMac,
	vfpNMac,
	vfpMsc,
	vfpNMsc,
};

static float single_expected_result[] = {
	[vfpAbs]	=	0.03,
	[vfpNeg]	=	-0.03423,
	[vfpSqrt]	=	0.00215174,
	[vfpAdd]	=	2e-39,
	[vfpSub]	=	-2e-31,
	[vfpMul]	=	9.99825e-40,
	[vfpNMul]	=	-9.99825e-40,
	[vfpDiv]	=	3.82371e-40,
	[vfpMac]	=	6.76f,
	[vfpNMac]	=	-6.76f,
	[vfpMsc]	=	6.76f,
	[vfpNMsc]	=	-6.76f,
};

static double double_expected_result[] = {
	[vfpAbs]        =       9.631e-21,
	[vfpNeg]        =       -3.423e-29,
	[vfpSqrt]       =       -3.423e-29,
	[vfpAdd]        =       5.2371e-322,
	[vfpSub]        =       0,
	[vfpMul]        =       3.11261e-322,
	[vfpNMul]       =       -6.5618244e-92,
	[vfpDiv]        =       1,
	[vfpMac]        =       6.76f,
	[vfpNMac]       =       -6.76f,
	[vfpMsc]        =       6.76f,
	[vfpNMsc]       =       -6.76f,
};



void VfpRegWrite32(float value, __u32 reg)
{
	switch(reg){
		case 0:
			__asm__ __volatile__ ("flds s0, %[val] \n"::[val] "X" (value));
			break;
		case 1:
			__asm__ __volatile__ ("flds s1, %[val] \n"::[val] "X" (value));
			break;
		case 2:
			__asm__ __volatile__ ("flds s2, %[val] \n"::[val] "X" (value));
			break;
		case 3:
			__asm__ __volatile__ ("flds s3, %[val] \n"::[val] "X" (value));
			break;
		case 4:
			__asm__ __volatile__ ("flds s4, %[val] \n"::[val] "X" (value));
			break;
		case 5:
			__asm__ __volatile__ ("flds s5, %[val] \n"::[val] "X" (value));
			break;
		case 6:
			__asm__ __volatile__ ("flds s6, %[val] \n"::[val] "X" (value));
			break;
		case 7:
			__asm__ __volatile__ ("flds s7, %[val] \n"::[val] "X" (value));
			break;
		case 8:
			__asm__ __volatile__ ("flds s8, %[val] \n"::[val] "X" (value));
			break;
		case 9:
			__asm__ __volatile__ ("flds s9, %[val] \n"::[val] "X" (value));
			break;
		case 10:
			__asm__ __volatile__ ("flds s10, %[val] \n"::[val] "X" (value));
			break;
		case 11:
			__asm__ __volatile__ ("flds s11, %[val] \n"::[val] "X" (value));
			break;
		case 12:
			__asm__ __volatile__ ("flds s12, %[val] \n"::[val] "X" (value));
			break;
		case 13:
			__asm__ __volatile__ ("flds s13, %[val] \n"::[val] "X" (value));
			break;
		case 14:
			__asm__ __volatile__ ("flds s14, %[val] \n"::[val] "X" (value));
			break;
		case 15:
			__asm__ __volatile__ ("flds s15, %[val] \n"::[val] "X" (value));
			break;
		case 16:
			__asm__ __volatile__ ("flds s16, %[val] \n"::[val] "X" (value));
			break;
		case 17:
			__asm__ __volatile__ ("flds s17, %[val] \n"::[val] "X" (value));
			break;
		case 18:
			__asm__ __volatile__ ("flds s18, %[val] \n"::[val] "X" (value));
			break;
		case 19:
			__asm__ __volatile__ ("flds s19, %[val] \n"::[val] "X" (value));
			break;
		case 20:
			__asm__ __volatile__ ("flds s20, %[val] \n"::[val] "X" (value));
			break;
		case 21:
			__asm__ __volatile__ ("flds s21, %[val] \n"::[val] "X" (value));
			break;
		case 22:
			__asm__ __volatile__ ("flds s22, %[val] \n"::[val] "X" (value));
			break;
		case 23:
			__asm__ __volatile__ ("flds s23, %[val] \n"::[val] "X" (value));
			break;
		case 24:
			__asm__ __volatile__ ("flds s24, %[val] \n"::[val] "X" (value));
			break;
		case 25:
			__asm__ __volatile__ ("flds s25, %[val] \n"::[val] "X" (value));
			break;
		case 26:
			__asm__ __volatile__ ("flds s26, %[val] \n"::[val] "X" (value));
			break;
		case 27:
			__asm__ __volatile__ ("flds s27, %[val] \n"::[val] "X" (value));
			break;
		case 28:
			__asm__ __volatile__ ("flds s28, %[val] \n"::[val] "X" (value));
			break;
		case 29:
			__asm__ __volatile__ ("flds s29, %[val] \n"::[val] "X" (value));
			break;
		case 30:
			__asm__ __volatile__ ("flds s30, %[val] \n"::[val] "X" (value));
			break;
		case 31:
			__asm__ __volatile__ ("flds s31, %[val] \n"::[val] "X" (value));
			break;
		default:
			return;
		}
}

void VfpRegWrite64(double value, __u32 reg)
{
	switch(reg){
		case 0:
			__asm__ __volatile__ ("fldd d0, %[val] \n"::[val] "X" (value));
			break;
		case 1:
			__asm__ __volatile__ ("fldd d1, %[val] \n"::[val] "X" (value));
			break;
		case 2:
			__asm__ __volatile__ ("fldd d2, %[val] \n"::[val] "X" (value));
			break;
		case 3:
			__asm__ __volatile__ ("fldd d3, %[val] \n"::[val] "X" (value));
			break;
		case 4:
			__asm__ __volatile__ ("fldd d4, %[val] \n"::[val] "X" (value));
			break;
		case 5:
			__asm__ __volatile__ ("fldd d5, %[val] \n"::[val] "X" (value));
			break;
		case 6:
			__asm__ __volatile__ ("fldd d6, %[val] \n"::[val] "X" (value));
			break;
		case 7:
			__asm__ __volatile__ ("fldd d7, %[val] \n"::[val] "X" (value));
			break;
		case 8:
			__asm__ __volatile__ ("fldd d8, %[val] \n"::[val] "X" (value));
			break;
		case 9:
			__asm__ __volatile__ ("fldd d9, %[val] \n"::[val] "X" (value));
			break;
		case 10:
			__asm__ __volatile__ ("fldd d10, %[val] \n"::[val] "X" (value));
			break;
		case 11:
			__asm__ __volatile__ ("fldd d11, %[val] \n"::[val] "X" (value));
			break;
		case 12:
			__asm__ __volatile__ ("fldd d12, %[val] \n"::[val] "X" (value));
			break;
		case 13:
			__asm__ __volatile__ ("fldd d13, %[val] \n"::[val] "X" (value));
			break;
		case 14:
			__asm__ __volatile__ ("fldd d14, %[val] \n"::[val] "X" (value));
			break;
		case 15:
			__asm__ __volatile__ ("fldd d15, %[val] \n"::[val] "X" (value));
			break;
#ifdef __VFPV3_SUPPORTED__
		case 16:
			__asm__ __volatile__ ("fldd d16, %[val] \n"::[val] "X" (value));
			break;
		case 17:
			__asm__ __volatile__ ("fldd d17, %[val] \n"::[val] "X" (value));
			break;
		case 18:
			__asm__ __volatile__ ("fldd d18, %[val] \n"::[val] "X" (value));
			break;
		case 19:
			__asm__ __volatile__ ("fldd d19, %[val] \n"::[val] "X" (value));
			break;
		case 20:
			__asm__ __volatile__ ("fldd d20, %[val] \n"::[val] "X" (value));
			break;
		case 21:
			__asm__ __volatile__ ("fldd d21, %[val] \n"::[val] "X" (value));
			break;
		case 22:
			__asm__ __volatile__ ("fldd d22, %[val] \n"::[val] "X" (value));
			break;
		case 23:
			__asm__ __volatile__ ("fldd d23, %[val] \n"::[val] "X" (value));
			break;
		case 24:
			__asm__ __volatile__ ("fldd d24, %[val] \n"::[val] "X" (value));
			break;
		case 25:
			__asm__ __volatile__ ("fldd d25, %[val] \n"::[val] "X" (value));
			break;
		case 26:
			__asm__ __volatile__ ("fldd d26, %[val] \n"::[val] "X" (value));
			break;
		case 27:
			__asm__ __volatile__ ("fldd d27, %[val] \n"::[val] "X" (value));
			break;
		case 28:
			__asm__ __volatile__ ("fldd d28, %[val] \n"::[val] "X" (value));
			break;
		case 29:
			__asm__ __volatile__ ("fldd d29, %[val] \n"::[val] "X" (value));
			break;
		case 30:
			__asm__ __volatile__ ("fldd d30, %[val] \n"::[val] "X" (value));
			break;
		case 31:
			__asm__ __volatile__ ("fldd d31, %[val] \n"::[val] "X" (value));
			break;
#endif /* __VFPV3_SUPPORTED__ */
		default:
			return;
		}
}

float VfpRegRead32(__u32 reg)
{
	float retval;

	switch(reg){
		case 0:
			__asm__ __volatile__ ("fmrs %[x], s0 \n":[x] "=r" (retval));
			break;
		case 1:
			__asm__ __volatile__ ("fmrs %[x], s1 \n":[x] "=r" (retval));
			break;
		case 2:
			__asm__ __volatile__ ("fmrs %[x], s2 \n":[x] "=r" (retval));
			break;
		case 3:
			__asm__ __volatile__ ("fmrs %[x], s3 \n":[x] "=r" (retval));
			break;
		case 4:
			__asm__ __volatile__ ("fmrs %[x], s4 \n":[x] "=r" (retval));
			break;
		case 5:
			__asm__ __volatile__ ("fmrs %[x], s5 \n":[x] "=r" (retval));
			break;
		case 6:
			__asm__ __volatile__ ("fmrs %[x], s6 \n":[x] "=r" (retval));
			break;
		case 7:
			__asm__ __volatile__ ("fmrs %[x], s7 \n":[x] "=r" (retval));
			break;
		case 8:
			__asm__ __volatile__ ("fmrs %[x], s8 \n":[x] "=r" (retval));
			break;
		case 9:
			__asm__ __volatile__ ("fmrs %[x], s9 \n":[x] "=r" (retval));
			break;
		case 10:
			__asm__ __volatile__ ("fmrs %[x], s10 \n":[x] "=r" (retval));
			break;
		case 11:
			__asm__ __volatile__ ("fmrs %[x], s11 \n":[x] "=r" (retval));
			break;
		case 12:
			__asm__ __volatile__ ("fmrs %[x], s12 \n":[x] "=r" (retval));
			break;
		case 13:
			__asm__ __volatile__ ("fmrs %[x], s13 \n":[x] "=r" (retval));
			break;
		case 14:
			__asm__ __volatile__ ("fmrs %[x], s14 \n":[x] "=r" (retval));
			break;
		case 15:
			__asm__ __volatile__ ("fmrs %[x], s15 \n":[x] "=r" (retval));
			break;
		case 16:
			__asm__ __volatile__ ("fmrs %[x], s16 \n":[x] "=r" (retval));
			break;
		case 17:
			__asm__ __volatile__ ("fmrs %[x], s17 \n":[x] "=r" (retval));
			break;
		case 18:
			__asm__ __volatile__ ("fmrs %[x], s18 \n":[x] "=r" (retval));
			break;
		case 19:
			__asm__ __volatile__ ("fmrs %[x], s19 \n":[x] "=r" (retval));
			break;
		case 20:
			__asm__ __volatile__ ("fmrs %[x], s20 \n":[x] "=r" (retval));
			break;
		case 21:
			__asm__ __volatile__ ("fmrs %[x], s21 \n":[x] "=r" (retval));
			break;
		case 22:
			__asm__ __volatile__ ("fmrs %[x], s22 \n":[x] "=r" (retval));
			break;
		case 23:
			__asm__ __volatile__ ("fmrs %[x], s23 \n":[x] "=r" (retval));
			break;
		case 24:
			__asm__ __volatile__ ("fmrs %[x], s24 \n":[x] "=r" (retval));
			break;
		case 25:
			__asm__ __volatile__ ("fmrs %[x], s25 \n":[x] "=r" (retval));
			break;
		case 26:
			__asm__ __volatile__ ("fmrs %[x], s26 \n":[x] "=r" (retval));
			break;
		case 27:
			__asm__ __volatile__ ("fmrs %[x], s27 \n":[x] "=r" (retval));
			break;
		case 28:
			__asm__ __volatile__ ("fmrs %[x], s28 \n":[x] "=r" (retval));
			break;
		case 29:
			__asm__ __volatile__ ("fmrs %[x], s29 \n":[x] "=r" (retval));
			break;
		case 30:
			__asm__ __volatile__ ("fmrs %[x], s30 \n":[x] "=r" (retval));
			break;
		case 31:
			__asm__ __volatile__ ("fmrs %[x], s31 \n":[x] "=r" (retval));
			break;
		default:
			break;
		}
		return retval;
}

double VfpRegRead64(__u32 reg)
{
	switch(reg){
		case 0:
			__asm__ __volatile__ ("fmrrd r0, r1, d0 \n");
			break;
		case 1:
			__asm__ __volatile__ ("fmrrd r0, r1, d1 \n");
			break;
		case 2:
			__asm__ __volatile__ ("fmrrd r0, r1, d2 \n");
			break;
		case 3:
			__asm__ __volatile__ ("fmrrd r0, r1, d3 \n");
			break;
		case 4:
			__asm__ __volatile__ ("fmrrd r0, r1, d4 \n");
			break;
		case 5:
			__asm__ __volatile__ ("fmrrd r0, r1, d5 \n");
			break;
		case 6:
			__asm__ __volatile__ ("fmrrd r0, r1, d6 \n");
			break;
		case 7:
			__asm__ __volatile__ ("fmrrd r0, r1, d7 \n");
			break;
		case 8:
			__asm__ __volatile__ ("fmrrd r0, r1, d8 \n");
			break;
		case 9:
			__asm__ __volatile__ ("fmrrd r0, r1, d9 \n");
			break;
		case 10:
			__asm__ __volatile__ ("fmrrd r0, r1, d10 \n");
			break;
		case 11:
			__asm__ __volatile__ ("fmrrd r0, r1, d11 \n");
			break;
		case 12:
			__asm__ __volatile__ ("fmrrd r0, r1, d12 \n");
			break;
		case 13:
			__asm__ __volatile__ ("fmrrd r0, r1, d13 \n");
			break;
		case 14:
			__asm__ __volatile__ ("fmrrd r0, r1, d14 \n");
			break;
		case 15:
			__asm__ __volatile__ ("fmrrd r0, r1, d15 \n");
			break;
#ifdef __VFPV3_SUPPORTED__
		case 16:
			__asm__ __volatile__ ("fmrrd r0, r1, d16 \n");
			break;
		case 17:
			__asm__ __volatile__ ("fmrrd r0, r1, d17 \n");
			break;
		case 18:
			__asm__ __volatile__ ("fmrrd r0, r1, d18 \n");
			break;
		case 19:
			__asm__ __volatile__ ("fmrrd r0, r1, d19 \n");
			break;
		case 20:
			__asm__ __volatile__ ("fmrrd r0, r1, d20 \n");
			break;
		case 21:
			__asm__ __volatile__ ("fmrrd r0, r1, d21 \n");
			break;
		case 22:
			__asm__ __volatile__ ("fmrrd r0, r1, d22 \n");
			break;
		case 23:
			__asm__ __volatile__ ("fmrrd r0, r1, d23 \n");
			break;
		case 24:
			__asm__ __volatile__ ("fmrrd r0, r1, d24 \n");
			break;
		case 25:
			__asm__ __volatile__ ("fmrrd r0, r1, d25 \n");
			break;
		case 26:
			__asm__ __volatile__ ("fmrrd r0, r1, d26 \n");
			break;
		case 27:
			__asm__ __volatile__ ("fmrrd r0, r1, d27 \n");
			break;
		case 28:
			__asm__ __volatile__ ("fmrrd r0, r1, d28 \n");
			break;
		case 29:
			__asm__ __volatile__ ("fmrrd r0, r1, d29 \n");
			break;
		case 30:
			__asm__ __volatile__ ("fmrrd r0, r1, d30 \n");
			break;
		case 31:
			__asm__ __volatile__ ("fmrrd r0, r1, d31 \n");
			break;
#endif /* __VFPV3_SUPPORTED__ */
		default:
			break;
		}
		return;
}


int vfpAbsS(const float* arg)
{
	int retval = 0;

	VfpRegWrite32(arg[0], 0); /* s0 = first param */
	__asm__ __volatile__ ("fabss s1, s0 \n"); /* s1 = abs(s0) */
	printf("vfpAbsS(%g) = %g RESULT = ", arg[0], VfpRegRead32(1));
	if (single_expected_result[vfpAbs] == VfpRegRead32(1))
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

int vfpNegS(const float* arg)
{
	int retval = 0;

	VfpRegWrite32(arg[0], 0); /* s0 = first param */
	__asm__ __volatile__ ("fnegs s1, s0 \n"); /* s1 = abs(s0) */
	printf("vfpNegS(%g) = %g RESULT = ", arg[0], VfpRegRead32(1));
	if (single_expected_result[vfpNeg] == VfpRegRead32(1))
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

int vfpSqrtS(const float* arg)
{
	float v;
	int retval = 0;

	VfpRegWrite32(arg[0], 31);
	__asm__ __volatile__ ("fsqrts s11, s31 \n");
	v = VfpRegRead32(11);
	single_expected_result[vfpSqrt] = v; /* this is temporary */
	printf("vfpSqrtS(%g) = %g RESULT = ", arg[0], v);
	if (single_expected_result[vfpSqrt] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

int vfpAddS(const float* arg)
{
	float v;
	int retval = 0;

	VfpRegWrite32(arg[0], 10);
	VfpRegWrite32(arg[1], 11);
	__asm__ __volatile__ ("fadds s24, s10, s11");
	v = VfpRegRead32(24);
	printf("vfpAddS(%g,%g) = %g RESULT = ", arg[0], arg[1], v);
	if (single_expected_result[vfpAdd] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

int vfpSubS(const float* arg)
{
	float v;
	int retval = 0;

	VfpRegWrite32(arg[0], 7);
	VfpRegWrite32(arg[1], 8);
	__asm__ __volatile__ ("fsubs s9, s7, s8");
	v = VfpRegRead32(9);
	printf("vfpSubS(%g,%g) = %g RESULT = ", arg[0], arg[1], v);
	if (single_expected_result[vfpSub] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

int vfpMulS(const float* arg)
{
	float v;
	int retval = 0;

	VfpRegWrite32(arg[0], 2);
	VfpRegWrite32(arg[1], 3);
	__asm__ __volatile__ ("fmuls s1, s2, s3");
	v = VfpRegRead32(1);
	printf("vfpMulS(%g,%g) = %g RESULT = ", arg[0], arg[1], v);
	if (single_expected_result[vfpMul] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

int vfpNMulS(const float *arg)
{
	float v;
	int retval = 0;

	VfpRegWrite32(arg[0], 11);
	VfpRegWrite32(arg[1], 12);
	__asm__ __volatile__ ("fnmuls s13, s11, s12");
	v = VfpRegRead32(13);
	printf("vfpNMulS(%g,%g) = %g RESULT = ", arg[0], arg[1], v);
	if (single_expected_result[vfpNMul] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

int vfpDivS(const float* arg)
{
	float v;
	int retval = 0;

	VfpRegWrite32(arg[0], 14);
	VfpRegWrite32(arg[1], 15);
	__asm__ __volatile__ ("fdivs s13, s14, s15");
	v = VfpRegRead32(13);
	printf("vfpDivS(%g,%g) = %g RESULT = ", arg[0], arg[1], v);
	if (single_expected_result[vfpDiv] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

/**
 * Description:
 * FMACS{<cond>} <Sd>, <Sn>, <Sm>
 * for i = 0 to vec_len-1
 * Sd[i] = Sd[i] + Sn[i] * Sm[i]
 */
int vfpMacS(const float *arg)
{
	float v;
	int retval = 0;

        VfpRegWrite32(arg[0], 13);
        VfpRegWrite32(arg[1], 14);
        VfpRegWrite32(arg[2], 15);
        __asm__ __volatile__ ("fmacs s13, s14, s15");
	v =  VfpRegRead32(13);
	single_expected_result[vfpMac] = v; /* this is temporary */
	printf("vfpMacS(%g,%g,%g) = %g RESULT = ", arg[0], arg[1], arg[2], v);
	if (single_expected_result[vfpMac] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

/**
 * Description:
 * FNMACS{<cond>} <Sd>, <Sn>, <Sm>
 * for i = 0 to vec_len-1
 * Sd[i] = Sd[i] + neg(Sn[i] * Sm[i])
 */
int vfpNMacS(const float *arg)
{
	float v;
	int retval = 0;

        VfpRegWrite32(arg[0], 13);
        VfpRegWrite32(arg[1], 14);
        VfpRegWrite32(arg[2], 15);
        __asm__ __volatile__ ("fnmacs s13, s14, s15");
	v = VfpRegRead32(13);
	single_expected_result[vfpNMac] = v; /* this is temporary */
	printf("vfpNMacS(%g,%g,%g) = %g RESULT = ", arg[0], arg[1], arg[2], v);
	if (single_expected_result[vfpNMac] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

/**
 * Description:
 * FMSCS{<cond>} <Sd>, <Sn>, <Sm>
 * for i = 0 to vec_len-1
 * Sd[i] = neg(Sd[i]) + Sn[i] * Sm[i]
 */
int vfpMscS(const float *arg)
{
	float v;
	int retval = 0;

        VfpRegWrite32(arg[0], 13);
        VfpRegWrite32(arg[1], 14);
        VfpRegWrite32(arg[2], 15);
        __asm__ __volatile__ ("fmscs s13, s14, s15");
	v = VfpRegRead32(13);
	single_expected_result[vfpMsc] = v; /* this is temporary */
	printf("vfpMscS(%g,%g,%g) = %g RESULT = ", arg[0], arg[1], arg[2], v);
	if (single_expected_result[vfpMsc] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}
/**
 * Description:
 * FNMSCS{<cond>} <Sd>, <Sn>, <Sm>
 * for i = 0 to vec_len-1
 * Sd[i] = neg(Sd[i]) + neg(Sn[i] * Sm[i])
 */
int vfpNMscS(const float *arg)
{
	float v;
	int retval = 0;

        VfpRegWrite32(arg[0], 13);
        VfpRegWrite32(arg[1], 14);
        VfpRegWrite32(arg[2], 15);
        __asm__ __volatile__ ("fnmscs s13, s14, s15");
	v = VfpRegRead32(13);
	single_expected_result[vfpNMsc] = v; /* this is temporary */
	printf("vfpNMscS(%g,%g,%g) = %g RESULT = ", arg[0], arg[1], arg[2], v);

	if (single_expected_result[vfpNMsc] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}


int vfpAbsD(const double* arg)
{
	double v;
	int retval = 0;

	VfpRegWrite64(arg[0], 10); /* s10 = first param */
	__asm__ __volatile__ ("fabsd d3, d10 \n"); /* d3 = abs(d10) */
	v = VfpRegRead64(3);
	printf("vfpAbsD(%g) = %g RESULT = ", arg[0], v);
	if (double_expected_result[vfpAbs] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

int vfpNegD(const double* arg)
{
	double v;
	int retval = 0;

	VfpRegWrite64(arg[0], 10); /* s10 = first param */
	__asm__ __volatile__ ("fnegd d3, d10 \n"); /* d3 = abs(d10) */
	v = VfpRegRead64(3);
	printf("vfpNegD(%g) = %g RESULT = ", arg[0], v);
	if (double_expected_result[vfpNeg] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

int vfpSqrtD(const double* arg)
{
	double v;
	int retval = 0;

	VfpRegWrite64(arg[0], 6); /* d6 = first param */
	__asm__ __volatile__ ("fsqrtd d7, d6 \n"); /* d7 = sqrt(d6) */
	v = VfpRegRead64(3);
	printf("vfpSqrtD(%g) = %g RESULT = ", arg[0], v);
	if (double_expected_result[vfpSqrt] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

int vfpAddD(const double* arg)
{
	double v;
	int retval = 0;

	VfpRegWrite64(arg[0], 4);
	VfpRegWrite64(arg[1], 5);
	__asm__ __volatile__ ("faddd d14, d4, d5");
	v = VfpRegRead64(14);
	printf("vfpAddD(%g,%g) = %g RESULT = ", arg[0], arg[1], v);
	if (double_expected_result[vfpAdd] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}

int vfpSubD(const double* arg)
{
	double v;
	int retval = 0;

	VfpRegWrite64(arg[0], 7);
	VfpRegWrite64(arg[1], 8);
	__asm__ __volatile__ ("fsubd d9, d7, d8");
	v = VfpRegRead64(9);
	printf("vfpSubD(%g,%g) = %g RESULT = ", arg[0], arg[1], v);
	if (double_expected_result[vfpSub] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}


int vfpMulD(const double* arg)
{
	double v;
	int retval = 0;

	VfpRegWrite64(arg[0], 2);
	VfpRegWrite64(arg[1], 3);
	__asm__ __volatile__ ("fmuld d1, d2, d3");
	v = VfpRegRead64(1);
	printf("vfpMulD(%g,%g) = %g RESULT = ", arg[0], arg[1], v);
	if (double_expected_result[vfpMul] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}


int vfpNMulD(const double *arg)
{
	double v;
	int retval = 0;

        VfpRegWrite64(arg[0], 11);
        VfpRegWrite64(arg[1], 12);
        __asm__ __volatile__ ("fnmuld d13, d11, d12");
	v = VfpRegRead64(13);
	printf("vfpNMulD(%g,%g) = %g RESULT = ", arg[0], arg[1], v);
	if (double_expected_result[vfpNMul] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}
	return retval;
}


int vfpDivD(const double* arg)
{
	double v;
	int retval = 0;

	VfpRegWrite64(arg[0], 14);
	VfpRegWrite64(arg[1], 15);
	__asm__ __volatile__ ("fdivd d13, d14, d15");
	v = VfpRegRead64(13);
	printf("vfpDivD(%g,%g) = %g RESULT = ", arg[0], arg[1], v);
	if (double_expected_result[vfpDiv] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}

	return retval;
}

/**
 * Description:
 * FMACD{<cond>} <Dd>, <Dn>, <Dm>
 * for i = 0 to vec_len-1
 * Dd[i] = neg(Dd[i]) + Dn[i] * Dm[i]
 */
int vfpMacD(const double *arg)
{
	double v;
	int retval = 0 ;

	VfpRegWrite64(arg[0], 13);
	VfpRegWrite64(arg[1], 14);
	VfpRegWrite64(arg[2], 15);
	__asm__ __volatile__ ("fmacd d13, d14, d15");
	v = VfpRegRead64(13);
	double_expected_result[vfpMac] = v; /* this is temporary */
	printf("vfpMacD(%g,%g,%g) = %g RESULT = ", arg[0], arg[1], arg[2], v);
	if (double_expected_result[vfpMac] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}

	return retval;
}

/**
 * Description:
 * FNMACD{<cond>} <Dd>, <Dn>, <Dm>
 * for i = 0 to vec_len-1
 * Dd[i] = Dd[i] + neg(Dn[i] * Dm[i])
 */
int vfpNMacD(const double *arg)
{
	double v;
	int retval = 0;

        VfpRegWrite64(arg[0], 13);
        VfpRegWrite64(arg[1], 14);
        VfpRegWrite64(arg[2], 15);
        __asm__ __volatile__ ("fnmacd d13, d14, d15");
	v = VfpRegRead64(13);
	double_expected_result[vfpNMac] = v; /* this is temporary */
	printf("vfpNMacD(%g,%g,%g) = %g RESULT = ", arg[0], arg[1], arg[2], v);
	if (double_expected_result[vfpNMac] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}

	return retval;
}


/**
 * Description:
 * FMSCD{<cond>} <Dd>, <Dn>, <Dm>
 * for i = 0 to vec_len-1
 * Dd[i] = neg(Dd[i]) + Dn[i] * Dm[i]
 */
int vfpMscD(const double *arg)
{
	double v;
	int retval = 0;

        VfpRegWrite64(arg[0], 13);
        VfpRegWrite64(arg[1], 14);
        VfpRegWrite64(arg[2], 15);
        __asm__ __volatile__ ("fmscd d13, d14, d15");
	v = VfpRegRead64(13);
	double_expected_result[vfpMsc] = v; /* this is temporary */
	printf("vfpMscD(%g,%g,%g) = %g RESULT = ", arg[0], arg[1], arg[2], v);
	if (double_expected_result[vfpMsc] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}

	return retval;
}


/**
 * Description:
 * FNMSCD{<cond>} <Dd>, <Dn>, <Dm>
 * for i = 0 to vec_len-1
 * Dd[i] = neg(Dd[i]) + neg(Dn[i] * Dm[i])
 */
int vfpNMscD(const double *arg)
{
	double v;
	int retval = 0;

        VfpRegWrite64(arg[0], 13);
        VfpRegWrite64(arg[1], 14);
        VfpRegWrite64(arg[2], 15);
        __asm__ __volatile__ ("fnmscd d13, d14, d15");
	v = VfpRegRead64(13);
	double_expected_result[vfpNMsc] = v;	/* this is temporary
						* and will be removed
						* after pre-computed
						* value populated in
						* array
						*/
	printf("vfpNMscD(%g,%g,%g) = %g RESULT = ", arg[0], arg[1], arg[2], v);
	if (double_expected_result[vfpNMsc] == v)
		printf(" PASSED\n");
	else {
		printf(" FAILED\n");
		retval = 1;
	}

	return retval;
}

#define L_DD_VFP_0001_0001      1
#define L_DD_VFP_0002_0001      2
#define L_DD_VFP_0003_0001      3
typedef int (funcS)(const float* param);
typedef int (funcD)(const double* param);

int branch_to_scalar_sgl_op(funcS f, float a1, float a2, float a3)
{
	float param[3] = {a1, a2, a3};

	return((*f)(param));
}

int branch_to_scalar_dbl_op(funcD f, double a1, double a2, double a3)
{
	double param[3] = {a1, a2, a3};

	return((*f)(param));
}



void vfptest_vector_operations()
{
	return;
}

void vfptest_vfpv3_specific_op()
{
	return;
}

void vfptest_rounding_modes()
{
	return;
}

void vfptest_contextsave()
{
	return;
}


void vfptest_mode_inheritence()
{
	return;
}

int vfptest_scalar_operations(int scenario)
{
	switch(scenario) {
	case L_DD_VFP_0002_0001:
		printf("\n\nSingle precision [Scalar operations]...\n");
		printf("############################################\n");
		if (branch_to_scalar_sgl_op(&vfpAbsS, 3e-2f, 0, 0))
			return 1;
		if (branch_to_scalar_sgl_op(&vfpNegS, 3.423e-2f, 0, 0))
			return 1;
		if (branch_to_scalar_sgl_op(&vfpSqrtS, 4.63e-6f, 0, 0))
			return 1;
		if (branch_to_scalar_sgl_op(&vfpAddS, 1e-39f, 1e-39f, 0))
			return 1;
		if (branch_to_scalar_sgl_op(&vfpSubS, 7e-39f, 2e-31f, 0))
			return 1;
		if (branch_to_scalar_sgl_op(&vfpMulS, 3.162e-20f, 3.162e-20f, 0))
			return 1;
		if (branch_to_scalar_sgl_op(&vfpNMulS, 3.162e-20f, 3.162e-20f, 0))
			return 1;
		if (branch_to_scalar_sgl_op(&vfpDivS, 1e-39f, 2.61526152f, 0))
			return 1;
		if (branch_to_scalar_sgl_op(&vfpMacS, 1e-39f, 2.6f, 2.6f))
			return 1;
		if (branch_to_scalar_sgl_op(&vfpNMacS, 1e-39f, 2.6f, 2.6f))
			return 1;
		if (branch_to_scalar_sgl_op(&vfpMscS, 1e-39f, 2.6f, 2.6f))
			return 1;
		if (branch_to_scalar_sgl_op(&vfpNMacS, 1e-39f, 2.6f, 2.6f))
			return 1;
		break;

	case L_DD_VFP_0003_0001:
		printf("\n\nDouble precision [Scalar operations]...\n");
		printf("############################################\n");
		if (branch_to_scalar_dbl_op(&vfpAbsD, 9.631e-21, 0, 0))
			return 1;
		if (branch_to_scalar_dbl_op(&vfpNegD, 3.423e-29, 0, 0))
			return 1;
		if (branch_to_scalar_dbl_op(&vfpSqrtD, 5.863e-310, 0, 0))
			return 1;
		if (branch_to_scalar_dbl_op(&vfpAddD, 3.1234e-322, 2.1234e-322, 0))
			return 1;
		if (branch_to_scalar_dbl_op(&vfpSubD, 3.1234e-322, 3.1234e-322, 0))
			return 1;
		if (branch_to_scalar_dbl_op(&vfpMulD, 1.767e-161, 1.767e-161, 0))
			return 1;
		if (branch_to_scalar_dbl_op(&vfpNMulD, 9.162e-33, 7.162e-60, 0))
			return 1;
		if (branch_to_scalar_dbl_op(&vfpDivD, 3.1234e-122, 3.1234e-122, 0))
			return 1;
		if (branch_to_scalar_dbl_op(&vfpMacD, 1e-39f, 2.6f, 2.6f))
			return 1;
		if (branch_to_scalar_dbl_op(&vfpNMacD, 1e-39f, 2.6f, 2.6f))
			return 1;
		if (branch_to_scalar_dbl_op(&vfpMscD, 1e-39f, 2.6f, 2.6f))
			return 1;
		if (branch_to_scalar_dbl_op(&vfpNMacD, 1e-39f, 2.6f, 2.6f))
			return 1;
		break;
	}
	return 0;
}


int vfptest_registers_write_read_test()
{
	int i = 31;
	int retval = 0;

	printf("Single-precicion registers write/read: ");
	for (; i >= 0; i--){
		VfpRegWrite32(i * 0x5A5A5A, i);
		if (VfpRegRead32(i) != i * 0x5A5A5A){
			printf(" FAILED\n");
			retval = 1;
			goto dp_test;
		}
	}
	printf(" PASSED\n");
dp_test:

#ifdef __VFPV3_SUPPORTED__
	i = 31;
#else
	i = 15;
#endif
	printf("Double-precision registers write/read: ");
	for (; i >= 0; i--){
		VfpRegWrite64(i * 0xA5A5A5A5, i);
		if (VfpRegRead64(i) != i * 0xA5A5A5A5){
			printf(" FAILED\n");
			retval = 1;
			goto end;
		}
	}
	printf(" PASSED\n");
end:
	return retval;
}

int main(int argc, char *argv[])
{
	int scenario = atoi(argv[1]);
	int retval = 0;

	printf("\n*** VFP FUNCTIONAL TESTS ***\n");
	switch(scenario) {
		case L_DD_VFP_0001_0001:
			retval = vfptest_registers_write_read_test();
			break;
		case L_DD_VFP_0002_0001:
			retval = vfptest_scalar_operations(L_DD_VFP_0002_0001);
			break;
		case L_DD_VFP_0003_0001:
			retval = vfptest_scalar_operations(L_DD_VFP_0003_0001);
			break;
		default:
			printf("Invalid Option: %d \n", scenario);
			retval = 1;
	}

#if 0
	vfptest_vector_operations();
	vfptest_vfpv3_specific_op();
	vfptest_rounding_modes();
	vfptest_contextsave();
	vfptest_mode_inheritence();
#endif
	return retval;
}
