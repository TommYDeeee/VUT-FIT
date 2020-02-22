/**********************************************************************/
/*   ____  ____                                                       */
/*  /   /\/   /                                                       */
/* /___/  \  /                                                        */
/* \   \   \/                                                       */
/*  \   \        Copyright (c) 2003-2009 Xilinx, Inc.                */
/*  /   /          All Right Reserved.                                 */
/* /---/   /\                                                         */
/* \   \  /  \                                                      */
/*  \___\/\___\                                                    */
/***********************************************************************/

/* This file is designed for use with ISim build 0x79f3f3a8 */

#define XSI_HIDE_SYMBOL_SPEC true
#include "xsi.h"
#include <memory.h>
#ifdef __GNUC__
#include <stdlib.h>
#else
#include <malloc.h>
#define alloca _alloca
#endif
extern char *IEEE_P_3499444699;
static const char *ng1 = "C:/FitkitSVN/apps/inp/inp2019-proj2/fpga/ram.vhd";
extern char *IEEE_P_3620187407;

char *ieee_p_3499444699_sub_2213602152_3536714472(char *, char *, int , int );
int ieee_p_3620187407_sub_514432868_3965413181(char *, char *, char *);


char *work_a_2676192072_3212880686_sub_1320479637_3057020925(char *t1, char *t2, char *t3)
{
    char t4[72];
    char t5[16];
    char t12[32];
    char t21[65536];
    char t40[16];
    char *t0;
    char *t6;
    char *t7;
    unsigned int t8;
    char *t9;
    unsigned char t10;
    unsigned int t11;
    char *t13;
    char *t14;
    int t15;
    unsigned int t16;
    char *t17;
    int t18;
    char *t19;
    char *t20;
    char *t22;
    char *t23;
    char *t24;
    char *t25;
    char *t26;
    char *t27;
    unsigned char t28;
    char *t29;
    char *t30;
    int t31;
    char *t32;
    int t33;
    char *t34;
    int t35;
    int t36;
    int t37;
    int t38;
    int t39;
    char *t41;
    int t42;
    char *t43;
    int t44;
    int t45;
    unsigned int t46;
    unsigned int t47;
    char *t48;
    unsigned char t49;
    char *t50;
    char *t51;
    char *t52;
    int t53;
    int t54;
    unsigned int t55;
    unsigned int t56;
    unsigned int t57;
    char *t58;
    unsigned int t59;

LAB0:    t6 = xsi_get_transient_memory(65536U);
    memset(t6, 0, 65536U);
    t7 = t6;
    t8 = (8U * 1U);
    t9 = t7;
    memset(t9, (unsigned char)2, t8);
    t10 = (t8 != 0);
    if (t10 == 1)
        goto LAB2;

LAB3:    t13 = (t12 + 0U);
    t14 = (t13 + 0U);
    *((int *)t14) = 0;
    t14 = (t13 + 4U);
    *((int *)t14) = 8191;
    t14 = (t13 + 8U);
    *((int *)t14) = 1;
    t15 = (8191 - 0);
    t16 = (t15 * 1);
    t16 = (t16 + 1);
    t14 = (t13 + 12U);
    *((unsigned int *)t14) = t16;
    t14 = (t12 + 16U);
    t17 = (t14 + 0U);
    *((int *)t17) = 7;
    t17 = (t14 + 4U);
    *((int *)t17) = 0;
    t17 = (t14 + 8U);
    *((int *)t17) = -1;
    t18 = (0 - 7);
    t16 = (t18 * -1);
    t16 = (t16 + 1);
    t17 = (t14 + 12U);
    *((unsigned int *)t17) = t16;
    t17 = (t4 + 4U);
    t19 = (t1 + 2720);
    t20 = (t17 + 52U);
    *((char **)t20) = t19;
    t22 = (t17 + 36U);
    *((char **)t22) = t21;
    memcpy(t21, t6, 65536U);
    t23 = (t17 + 40U);
    t24 = (t19 + 44U);
    t25 = *((char **)t24);
    *((char **)t23) = t25;
    t26 = (t17 + 48U);
    *((unsigned int *)t26) = 65536U;
    t27 = (t5 + 4U);
    t28 = (t2 != 0);
    if (t28 == 1)
        goto LAB5;

LAB4:    t29 = (t5 + 8U);
    *((char **)t29) = t3;
    t30 = (t3 + 8U);
    t31 = *((int *)t30);
    t32 = (t3 + 4U);
    t33 = *((int *)t32);
    t34 = (t3 + 0U);
    t35 = *((int *)t34);
    t36 = t35;
    t37 = t33;

LAB6:    t38 = (t37 * t31);
    t39 = (t36 * t31);
    if (t39 <= t38)
        goto LAB7;

LAB9:    t6 = (t17 + 36U);
    t7 = *((char **)t6);
    t10 = (65536U != 65536U);
    if (t10 == 1)
        goto LAB11;

LAB12:    t0 = xsi_get_transient_memory(65536U);
    memcpy(t0, t7, 65536U);

LAB1:    return t0;
LAB2:    t11 = (65536U / t8);
    xsi_mem_set_data(t7, t7, t8, t11);
    goto LAB3;

LAB5:    *((char **)t27) = t2;
    goto LAB4;

LAB7:    t41 = (t3 + 0U);
    t42 = *((int *)t41);
    t43 = (t3 + 8U);
    t44 = *((int *)t43);
    t45 = (t36 - t42);
    t16 = (t45 * t44);
    t46 = (1U * t16);
    t47 = (0 + t46);
    t48 = (t2 + t47);
    t49 = *((unsigned char *)t48);
    t50 = ieee_p_3499444699_sub_2213602152_3536714472(IEEE_P_3499444699, t40, ((int)(t49)), 8);
    t51 = (t17 + 36U);
    t52 = *((char **)t51);
    t53 = (t36 - 1);
    t54 = (t53 - 0);
    t55 = (t54 * 1);
    xsi_vhdl_check_range_of_index(0, 8191, 1, t53);
    t56 = (8U * t55);
    t57 = (0 + t56);
    t51 = (t52 + t57);
    t58 = (t40 + 12U);
    t59 = *((unsigned int *)t58);
    t59 = (t59 * 1U);
    memcpy(t51, t50, t59);

LAB8:    if (t36 == t37)
        goto LAB9;

LAB10:    t15 = (t36 + t31);
    t36 = t15;
    goto LAB6;

LAB11:    xsi_size_not_matching(65536U, 65536U, 0);
    goto LAB12;

LAB13:;
}

static void work_a_2676192072_3212880686_p_0(char *t0)
{
    char *t1;
    char *t2;
    char *t3;
    char *t4;
    char *t5;
    char *t6;
    char *t7;

LAB0:    xsi_set_current_line(49, ng1);

LAB3:    t1 = (t0 + 1236U);
    t2 = *((char **)t1);
    t1 = (t0 + 2372);
    t3 = (t1 + 32U);
    t4 = *((char **)t3);
    t5 = (t4 + 40U);
    t6 = *((char **)t5);
    memcpy(t6, t2, 8U);
    xsi_driver_first_trans_fast_port(t1);

LAB2:    t7 = (t0 + 2320);
    *((int *)t7) = 1;

LAB1:    return;
LAB4:    goto LAB2;

}

static void work_a_2676192072_3212880686_p_1(char *t0)
{
    unsigned char t1;
    char *t2;
    unsigned char t3;
    char *t4;
    char *t5;
    unsigned char t6;
    unsigned char t7;
    char *t8;
    unsigned char t9;
    unsigned char t10;
    char *t11;
    unsigned char t12;
    unsigned char t13;
    char *t14;
    char *t15;
    int t16;
    int t17;
    unsigned int t18;
    unsigned int t19;
    unsigned int t20;
    char *t21;
    char *t22;
    char *t23;
    char *t24;
    char *t25;

LAB0:    xsi_set_current_line(54, ng1);
    t2 = (t0 + 568U);
    t3 = xsi_signal_has_event(t2);
    if (t3 == 1)
        goto LAB5;

LAB6:    t1 = (unsigned char)0;

LAB7:    if (t1 != 0)
        goto LAB2;

LAB4:
LAB3:    t2 = (t0 + 2328);
    *((int *)t2) = 1;

LAB1:    return;
LAB2:    xsi_set_current_line(55, ng1);
    t4 = (t0 + 1052U);
    t8 = *((char **)t4);
    t9 = *((unsigned char *)t8);
    t10 = (t9 == (unsigned char)3);
    if (t10 != 0)
        goto LAB8;

LAB10:
LAB9:    goto LAB3;

LAB5:    t4 = (t0 + 592U);
    t5 = *((char **)t4);
    t6 = *((unsigned char *)t5);
    t7 = (t6 == (unsigned char)3);
    t1 = t7;
    goto LAB7;

LAB8:    xsi_set_current_line(56, ng1);
    t4 = (t0 + 960U);
    t11 = *((char **)t4);
    t12 = *((unsigned char *)t11);
    t13 = (t12 == (unsigned char)3);
    if (t13 != 0)
        goto LAB11;

LAB13:    xsi_set_current_line(60, ng1);
    t2 = (t0 + 1144U);
    t4 = *((char **)t2);
    t2 = (t0 + 684U);
    t5 = *((char **)t2);
    t2 = (t0 + 4200U);
    t16 = ieee_p_3620187407_sub_514432868_3965413181(IEEE_P_3620187407, t5, t2);
    t17 = (t16 - 0);
    t18 = (t17 * 1);
    xsi_vhdl_check_range_of_index(0, 8191, 1, t16);
    t19 = (8U * t18);
    t20 = (0 + t19);
    t8 = (t4 + t20);
    t11 = (t0 + 2444);
    t14 = (t11 + 32U);
    t15 = *((char **)t14);
    t21 = (t15 + 40U);
    t22 = *((char **)t21);
    memcpy(t22, t8, 8U);
    xsi_driver_first_trans_fast(t11);

LAB12:    goto LAB9;

LAB11:    xsi_set_current_line(57, ng1);
    t4 = (t0 + 776U);
    t14 = *((char **)t4);
    t4 = (t0 + 684U);
    t15 = *((char **)t4);
    t4 = (t0 + 4200U);
    t16 = ieee_p_3620187407_sub_514432868_3965413181(IEEE_P_3620187407, t15, t4);
    t17 = (t16 - 0);
    t18 = (t17 * 1);
    t19 = (8U * t18);
    t20 = (0U + t19);
    t21 = (t0 + 2408);
    t22 = (t21 + 32U);
    t23 = *((char **)t22);
    t24 = (t23 + 40U);
    t25 = *((char **)t24);
    memcpy(t25, t14, 8U);
    xsi_driver_first_trans_delta(t21, t20, 8U, 0LL);
    xsi_set_current_line(58, ng1);
    t2 = (t0 + 776U);
    t4 = *((char **)t2);
    t2 = (t0 + 2444);
    t5 = (t2 + 32U);
    t8 = *((char **)t5);
    t11 = (t8 + 40U);
    t14 = *((char **)t11);
    memcpy(t14, t4, 8U);
    xsi_driver_first_trans_fast(t2);
    goto LAB12;

}


extern void work_a_2676192072_3212880686_init()
{
	static char *pe[] = {(void *)work_a_2676192072_3212880686_p_0,(void *)work_a_2676192072_3212880686_p_1};
	static char *se[] = {(void *)work_a_2676192072_3212880686_sub_1320479637_3057020925};
	xsi_register_didat("work_a_2676192072_3212880686", "isim/inp_isim.exe.sim/work/a_2676192072_3212880686.didat");
	xsi_register_executes(pe);
	xsi_register_subprogram_executes(se);
}
