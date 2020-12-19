/***************************************************
 * File:touch.c
 * VENDOR_EDIT
 * Copyright (c)  2008- 2030  Oppo Mobile communication Corp.ltd.
 * Description:
 *             tp dev
 * Version:1.0:
 * Date created:2016/09/02
 * Author: hao.wang@Bsp.Driver
 * TAG: BSP.TP.Init
*/


#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/serio.h>
#include "oppo_touchscreen/tp_devices.h"
#include "oppo_touchscreen/touchpanel_common.h"

#include <soc/oppo/oppo_project.h>
#include "touch.h"

#define MAX_LIMIT_DATA_LENGTH         100

struct tp_dev_name tp_dev_names[] = {
    {TP_BOE,"BOE"},
    {TP_INNOLUX, "INNOLUX"},
    {TP_BOE_90HZ, "90HZ_BOE"},
    {TP_INNOLUX_90HZ, "90HZ_INNOLUX"},
    {TP_HLT_90HZ, "90HZ_HLT"},
    {TP_TXD_90HZ_82n, "90HZ_TXD"},
    {TP_BOE_90HZ_82n, "90HZ_BOE"},
    {TP_SAMSUNG, "SAMSUNG"},
    {TP_UNKNOWN, "UNKNOWN"},
};

#define GET_TP_DEV_NAME(tp_type) ((tp_dev_names[tp_type].type == (tp_type))?tp_dev_names[tp_type].name:"UNMATCH")

int g_tp_dev_vendor = TP_UNKNOWN;
int g_chip_name = UNKNOWN_IC;   //add by wanglongfei  distinguish IC
typedef enum {
    TP_INDEX_NULL,
    ili9881_boe,
    nt36525b_inx,
    ili9881_90hz_boe,
    nt36525b_90hz_inx,
    ili9881_90hz_hlt,
    ili9882n_90hz_txd,
    ili9882n_90hz_boe,
    gt9886_samsung,
} TP_USED_INDEX;
TP_USED_INDEX tp_used_index  = TP_INDEX_NULL;
extern char* saved_command_line;

/*
* this function is used to judge whether the ic driver should be loaded
* For incell module, tp is defined by lcd module, so if we judge the tp ic
* by the boot command line of containing lcd string, we can also get tp type.
*/

TP_USED_IC __init tp_judge_ic_match(void)
{
    pr_err("[TP] saved_command_line = %s \n", saved_command_line);
//#ifdef ODM_HQ_EDIT
/* dongfeiju, 2020/05/15, Add for NOVATEK bringup */
    if (strstr(saved_command_line, "mdss_dsi_ili9882n_90hz_txd_video")) {
        g_tp_dev_vendor = TP_TXD_90HZ_82n;
        tp_used_index = ili9882n_90hz_txd;
        g_chip_name =  ILI9882N;
        printk("[TP] g_tp_dev_vendor: %d, tp_used_index: %d\n", g_tp_dev_vendor, tp_used_index);
        return g_chip_name;
    }
    if (strstr(saved_command_line, "mdss_dsi_ili9882n_90hz_boe_video")) {
        g_tp_dev_vendor = TP_BOE_90HZ_82n;
        tp_used_index = ili9882n_90hz_boe;
        g_chip_name =  ILI9882N;
        printk("[TP] g_tp_dev_vendor: %d, tp_used_index: %d\n", g_tp_dev_vendor, tp_used_index);
        return g_chip_name;
    }

    if (strstr(saved_command_line, "mdss_dsi_ili9881h_boe_video")) {
        g_tp_dev_vendor = TP_BOE;
        tp_used_index = ili9881_boe;
        g_chip_name = ILI9881H;
        printk("g_tp_dev_vendor: %d, tp_used_index: %d\n", g_tp_dev_vendor, tp_used_index);
        return g_chip_name;
    }

    if (strstr(saved_command_line, "mdss_dsi_nt36525b_inx_video")) {
        g_tp_dev_vendor = TP_INNOLUX;
        tp_used_index = nt36525b_inx;
        g_chip_name =  NT36525B;
        printk("[TP]touchpanel: g_tp_dev_vendor: %d, tp_used_index: %d\n", g_tp_dev_vendor, tp_used_index);
        return g_chip_name;
    }
    if (strstr(saved_command_line, "mdss_dsi_ili9881h_90hz_boe_video")) {
        g_tp_dev_vendor = TP_BOE_90HZ;
        tp_used_index = ili9881_90hz_boe;
        g_chip_name = ILI9881H;
        printk("g_tp_dev_vendor: %d, tp_used_index: %d\n", g_tp_dev_vendor, tp_used_index);
        return g_chip_name;
    }
    if (strstr(saved_command_line, "mdss_dsi_ili9881h_90hz_hlt_video")) {
        g_tp_dev_vendor = TP_HLT_90HZ;
        tp_used_index = ili9881_90hz_hlt;
        g_chip_name = ILI9881H;
        printk("g_tp_dev_vendor: %d, tp_used_index: %d\n", g_tp_dev_vendor, tp_used_index);
        return g_chip_name;
    }
    if (strstr(saved_command_line, "mdss_dsi_nt36525b_90hz_inx_video")) {
        g_tp_dev_vendor = TP_INNOLUX_90HZ;
        tp_used_index = nt36525b_90hz_inx;
        g_chip_name = NT36525B;
        printk("g_tp_dev_vendor: %d, tp_used_index: %d\n", g_tp_dev_vendor, tp_used_index);
        return g_chip_name;
    }
//#endif
    if (strstr(saved_command_line, "mdss_s6e8fc1_samsung_video")) {
        g_tp_dev_vendor = TP_SAMSUNG;
        tp_used_index = gt9886_samsung;
        g_chip_name = GT9886;
        printk("g_tp_dev_vendor: %d, tp_used_index: %d\n", g_tp_dev_vendor, tp_used_index);
        return g_chip_name;
    }

    pr_err("Lcd module not found\n");
    return UNKNOWN_IC;
}

int tp_util_get_vendor(struct hw_resource *hw_res, struct panel_info *panel_data)
{

    char *vendor = NULL;
    panel_data->test_limit_name = kzalloc(MAX_LIMIT_DATA_LENGTH, GFP_KERNEL);
    if (panel_data->test_limit_name == NULL) {
        pr_err("[TP]panel_data.test_limit_name kzalloc error\n");
    }
    panel_data->tp_type = g_tp_dev_vendor;
    vendor = GET_TP_DEV_NAME(panel_data->tp_type);
    strncpy(panel_data->manufacture_info.manufacture, vendor, MAX_DEVICE_MANU_LENGTH);

    if (panel_data->tp_type == TP_UNKNOWN) {
        pr_err("[TP]%s type is unknown\n", __func__);
        return 0;
    }

    panel_data->vid_len = 7;
//#ifdef ODM_HQ_EDIT
/*Hao.Jiang@BSP.TP.Function, 2020/05/14, add for Rum TP FW*/
    switch(get_project()) {
    case 20021:
    case 20022:
    case 20229:
    case 20228:
    case 20227:
    case 20226:
    case 20225:
    case 20224:
    case 20223:
    case 20222:
    case 20221:
    case 20202:
    case 20203:
    case 20204:
    case 20207:
    case 20208:
        if (panel_data->fw_name) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/20221/FW_%s_%s.bin",
                     panel_data->chip_name, vendor);
        }

        if (panel_data->test_limit_name) {
            snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                     "/tp/20221/LIMIT_%s_%s.img",
                     panel_data->chip_name, vendor);
            printk("panel_data->test_limit_name = %s\n", panel_data->test_limit_name);
        }
        break;
    case 20215:
    case 20214:
    case 20213:
    case 20212:
    case 20211:
        if (panel_data->fw_name) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/20211/FW_%s_%s.bin",
                     panel_data->chip_name, vendor);
        }

        if (panel_data->test_limit_name) {
            snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                     "/tp/20211/LIMIT_%s_%s.img",
                     panel_data->chip_name, vendor);
            printk("panel_data->test_limit_name = %s\n", panel_data->test_limit_name);
        }
        break;
    default:
        if (panel_data->fw_name) {
            snprintf(panel_data->fw_name, MAX_FW_NAME_LENGTH,
                     "tp/%d/FW_%s_%s.bin",
                     get_project(), panel_data->chip_name, vendor);
        }

        if (panel_data->test_limit_name) {
            snprintf(panel_data->test_limit_name, MAX_LIMIT_DATA_LENGTH,
                     "/tp/%d/LIMIT_%s_%s.img",
                     get_project(), panel_data->chip_name, vendor);
            printk("panel_data->test_limit_name = %s\n", panel_data->test_limit_name);
        }
        break;
    }
//#endif

   if (tp_used_index == ili9881_boe) {
        memcpy(panel_data->manufacture_info.version, "FA218BI", 7);
        panel_data->firmware_headfile.firmware_data = FW_20221_ILI9881H_BOE;
        panel_data->firmware_headfile.firmware_size = sizeof(FW_20221_ILI9881H_BOE);
    } else if (tp_used_index == nt36525b_inx) {
        memcpy(panel_data->manufacture_info.version, "FA218IN", 7);
        panel_data->firmware_headfile.firmware_data = FW_20221_NT36525B_INNOLUX;
        panel_data->firmware_headfile.firmware_size = sizeof(FW_20221_NT36525B_INNOLUX);
//#ifdef ODM_HQ_EDIT
/*Hao.Jiang@BSP.TP.Function, 2020/05/14, add for Rum TP FW*/
    } else if (tp_used_index == gt9886_samsung) {
        memcpy(panel_data->manufacture_info.version, "FA218ON", 7);
//#endif
    } else if (tp_used_index == ili9881_90hz_boe) {
        panel_data->vid_len = 8;
        memcpy(panel_data->manufacture_info.version, "FA218BI9", 8);
        panel_data->firmware_headfile.firmware_data = FW_20221_ILI9881H_90HZ_BOE;
        panel_data->firmware_headfile.firmware_size = sizeof(FW_20221_ILI9881H_90HZ_BOE);
    } else if (tp_used_index == ili9881_90hz_hlt) {
        panel_data->vid_len = 8;
        memcpy(panel_data->manufacture_info.version, "FA218HI9", 8);
        panel_data->firmware_headfile.firmware_data = FW_20221_ILI9881H_90HZ_HLT;
        panel_data->firmware_headfile.firmware_size = sizeof(FW_20221_ILI9881H_90HZ_HLT);
    } else if (tp_used_index == ili9882n_90hz_txd) {
        panel_data->vid_len = 9;
        memcpy(panel_data->manufacture_info.version, "FA218TI9N", 9);
        panel_data->firmware_headfile.firmware_data = FW_20221_ILI9882N_TXD;
        panel_data->firmware_headfile.firmware_size = sizeof(FW_20221_ILI9882N_TXD);
    } else if (tp_used_index == ili9882n_90hz_boe) {
        panel_data->vid_len = 9;
        memcpy(panel_data->manufacture_info.version, "FA218BI9N", 9);
        panel_data->firmware_headfile.firmware_data = FW_20221_ILI9882N_BOE;
        panel_data->firmware_headfile.firmware_size = sizeof(FW_20221_ILI9882N_BOE);
    } else if (tp_used_index == nt36525b_90hz_inx) {
        panel_data->vid_len = 8;
        memcpy(panel_data->manufacture_info.version, "FA218IN9", 8);
        panel_data->firmware_headfile.firmware_data = FW_20221_NT36525B_90HZ_INNOLUX;
        panel_data->firmware_headfile.firmware_size = sizeof(FW_20221_NT36525B_90HZ_INNOLUX);
    } else {
        panel_data->firmware_headfile.firmware_data = NULL;
        panel_data->firmware_headfile.firmware_size = 0;
    }


    panel_data->manufacture_info.fw_path = panel_data->fw_name;

    pr_info("fw_path: %s\n", panel_data->manufacture_info.fw_path);
    pr_info("[TP]vendor:%s fw:%s limit:%s\n",
            vendor, panel_data->fw_name,
            panel_data->test_limit_name == NULL ? "NO Limit" : panel_data->test_limit_name);

    return 0;

}
