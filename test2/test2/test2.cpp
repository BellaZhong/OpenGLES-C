
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include <iostream>

#define _CRT_SECURE_NO_DEPRECATE
//#define AddSet(val) Setstr(#val)  

struct a {
	int arr[5];
	int b;
};

struct stru {
	int y1;
	int y2;
	int y3;
};

enum MyEnum
{
	enum1,
	enum2,
	enum3,
	enum4
};

struct aa {
	int c;
	struct a asample;
	enum MyEnum enumx;
};

std::string mystr;

void Setstr(const char * val)
{
	mystr = val;
}

#define AddSet(val) Setstr(#val)  
#define ENUM_CHIP_TYPE_CASE(x)   (#x)

// 把字符串转换为小写
char* str2lower(char *str)
{
	char *p;
	char *sir2 = (char *)malloc(strlen(str) + 1);
	p = sir2;
	while (*str)
	{
		*sir2++ = tolower(*str++);
	}
	*sir2 = '\0';
	return p;
}

struct stru *fun_init(struct stru *pstru1)
{
	pstru1->y1 = 22;
	pstru1->y2 = 33;
	pstru1->y3 = 44;
	return pstru1;
}

//char *init_arr[] = {"enum1","enum2","enum3","enum4"};

int main()
{

	char *path = "C:\\Users\\belzho01\\Desktop\\ad-calib-cfg_ad5.dat";
	char buffer[1024];//以一个字符数组为缓存,按行读取内容,每次按一个字符串来处理
	int num = 1;
	char *pdest;
	char *valdest;
	int val = 0;
	int flag = 0;
	uint32_t enumnum = 0;
	int i = 0;
	FILE *pf = fopen(path, "r");
	char *str[] = { "enum1","enum2","enum3","enum4" };
	int enumindex = 0;
	char *init_arr[] = { "p_frmt_mode","p_frmt_dbuf_ctrl","p_frmt_frame_width","p_frmt_frame_height","p_calc_psr_delta_change","p_calc_psr_delta_settle","s_calc_al_scale","s_calc_al_tf_step_sample","s_calc_al_tf_step_wait","s_calc_al_tf_step_waitup","s_calc_al_tf_step_size","s_calc_al_tf_limit","s_calc_al_tf_alpha","s_calc_al_tf_alpha_up","s_calc_al_tf_noise","s_calc_al_tf_m_inc","s_calc_al_tf_k_inc","s_calc_al_tf_m_dec","s_calc_al_tf_k_dec","s_calc_al_rtf_filter_b","s_calc_al_rtf_filter_c","s_calc_bl_min","s_calc_ab_al_knee1","s_calc_ab_al_knee2","s_calc_ab_bl_knee1","s_calc_ab_bl_knee2","s_calc_bl_panel_max","s_calc_sbc1_tf_asym","s_calc_sbc1_tf_asym_log","s_calc_sbc2_tf_asym","s_calc_sbc2_tf_asym_log","s_calc_backlight_scale","s_calc_gain_aa_tf_asym","s_calc_icut_bl_min","s_calc_gain_ca_tf_asym","s_calc_bpr_correct","s_calc_al_delta_settle","s_calc_bl_delta_settle","p_core1_irdx_control_0","p_core1_irdx_control_1","s_core1_irdx_variance","s_core1_irdx_slope_max","s_core1_irdx_slope_min","s_core1_irdx_black_level","s_core1_irdx_white_level","s_core1_irdx_limit_ampl","s_core1_irdx_dither","s_core1_irdx_filter_ctrl","s_core1_irdx_svariance","s_core1_dthr_control","s_core1_logo_top","s_core1_logo_left","s_core1_ca_d_artithresh","s_core1_reg0","s_core1_reg1","s_core1_reg2","s_core1_reg3","s_core1_reg4","s_core1_reg5","p_frmt_top_control","p_core1_irdx_alpha_manual","p_core1_irdx_beta_manual","p_core1_irdx_r_filter","p_core1_irdx_g_filter","p_core1_irdx_b_filter","p_core1_tile_frame_start","p_core1_tile_frame_end","p_core1_tile_procs_start","p_core1_tile_procs_end","p_core1_anti_flckr_control","p_core1_anti_flckr_rfd_thr","p_core1_anti_flckr_frd_thr","p_core1_anti_flckr_scd_thr","p_core1_anti_flckr_fd3_sc_dly","p_core1_anti_flckr_al_m","p_core1_anti_flckr_t_duration" };
	char *cfg_arr[] = {"p_frmt_roi_hor_start","p_frmt_roi_hor_end","p_frmt_roi_ver_start","p_frmt_roi_ver_end","p_calc_control_0","p_calc_control_1","p_calc_assertiveness","p_calc_tf_control","p_calc_strength_manual","p_calc_gain_aa_manual","p_calc_roi_factor_in","p_calc_roi_factor_out","s_calc_al_tf_aggressiveness","s_calc_al_rtf_filter_a","s_calc_bl_offset","s_calc_bl_atten_alpha","s_calc_sbc1_tf_depth","s_calc_sbc1_tf_step","s_calc_sbc1_tf_depth_log","s_calc_sbc1_tf_step_log","s_calc_sbc2_tf_depth","s_calc_sbc2_tf_step","s_calc_sbc2_tf_depth_log","s_calc_sbc2_tf_step_log","s_calc_calibration_a","s_calc_calibration_b","s_calc_calibration_c","s_calc_calibration_d","s_calc_calibration_e","s_calc_gain_aa_tf_depth","s_calc_gain_aa_tf_step","s_calc_strength_limit","s_calc_icut_hist_min","s_calc_gain_ca_tf_depth","s_calc_gain_ca_tf_step","s_calc_gain_max","s_calc_gain_middle","s_calc_brightpr","p_cabc_icut_select","p_cabc_icut_manual","s_cabc_intensity","p_core1_vc_control_0","p_core1_irdx_strength_inroi","p_core1_irdx_strength_outroi","p_core1_irdx_brightpr","s_core1_irdx_contrast","p_core1_irdx_darkenh","frame_pushes"};
	int init_end = 0;
	int cfg_end = 0;
	uint32_t initval_arr[80] = { 0 };
	uint32_t cfgval_arr[80] = { 0 };
	struct stru stru2;
	struct stru *stru1;
	stru1 = &stru2;

	stru1 = fun_init(stru1);
	printf("y1 = %d\n", stru1->y1);
	printf("y2 = %d\n", stru1->y2);
	printf("y3 = %d\n", stru1->y3);

	if (pf == NULL)
	{
		perror("file not open ");

	}

	//for (i = 0; i < 4; i++)
	//{
	//	printf("%s\n", init_arr[i]);
	//}



	//AddSet(enum1);
	//printf("%s\n", mystr.c_str());
	pdest = str2lower("p_calc_psr_delta_change");

	while (fgets(buffer, 1024, pf) != NULL)
	{

		//printf("No.%d:%s\n", num, buffer);
		//num++;


		if (flag == 1 && init_end == 0)
		{
			valdest = strstr(buffer, "0x");
			valdest = strtok(valdest, "\n");
			val = (uint32_t)strtol(valdest, NULL, 16);
			initval_arr[enumindex] = val;
			printf("val = %x\n", initval_arr[enumindex]);

			if (init_arr[enumindex] == "p_core1_anti_flckr_t_duration")
			{
				init_end = 1;
				enumindex = 0;
			}
			if (init_end == 0)
				enumindex++;
			flag = 0;
			continue;
		}

		if (flag == 1 && cfg_end == 0)
		{
			valdest = strstr(buffer, "0x");
			valdest = strtok(valdest, "\n");
			val = (uint32_t)strtol(valdest, NULL, 16);
			cfgval_arr[enumindex] = val;
			printf("val = %x\n", cfgval_arr[enumindex]);

			if (cfg_arr[enumindex] == "frame_pushes")
			{
				cfg_end = 1;
				enumindex = 0;
			}
			if (cfg_end == 0)
				enumindex++;
			flag = 0;
			continue;
		}


		if (init_end == 0)
		{
			if (strstr(buffer, init_arr[enumindex]) != NULL)
			{
				flag = 1;
				printf("%s\n", init_arr[enumindex]);
				continue;
			}
		}
		

		if (cfg_end == 0)
		{
			if (strstr(buffer, cfg_arr[enumindex]) != NULL)
			{
				flag = 1;
				printf("enumindex = %d\n",enumindex);
				printf("%s\n", cfg_arr[enumindex]);
				continue;
			}
		}

		//if (flag == 1)
		//{
		//	if (strstr(buffer, "0x") != NULL)
		//	{
		//		valdest = strstr(buffer, "0x");
		//		valdest = strtok(valdest, "\n");
		//		val = (int)strtol(valdest, NULL, 16);
		//		printf("val = %x\n", val);
		//	}
		//	flag = 0;
		//}
		//if (strstr(buffer, pdest) != NULL)
		//{
			//printf("No.%d:%s\n", num, buffer);
		//	flag = 1;
		//}
		//num++;

	}


	fclose(pf);



	//a a1;
	//aa aa1;
	//int i = 0;
	//int arr1[5] = {1,2,3,4,5};
	//int c = 10;
	//a1.b = c;
	//memset(arr1, 0, sizeof(arr1));
	//for (i = 0; i < 5; i++)
	//	printf("%d\n", arr1[i]);
	//memcpy(a1.arr, arr1, 5 * sizeof(int));
	//for (i = 0; i < 5; i++)
	//	printf("%d\n",a1.arr[i]);

	//aa1.c = 0x02;
	//aa1.asample.b = 0x10;
	//aa1.enumx = enum3;
	//printf("aa1.enumx = %x\n",aa1.enumx);




	system("pause");
	return 0;
}