/***********************************************************
** Copyright (C), 2009-2019, OPPO Mobile Comm Corp., Ltd.
** VENDOR_EDIT
** File: - oppo_nwpower.h
** Description: BugID:2120730, Add for FEATURE_DATA_NWPOWER
**
** Version: 1.0
** Date : 2019/07/31
** Author: Asiga@PSW.NW.DATA
**
** ------------------ Revision History:------------------------
** <author> <data> <version > <desc>
** Asiga 2019/07/31 1.0 build this module
****************************************************************/
#ifndef __OPPO_NWPOWER_H_
#define __OPPO_NWPOWER_H_

#define OPPO_TOTAL_IP_SUM                  60
#define OPPO_TRANSMISSION_INTERVAL         3 * 1000//3s
#define OPPO_TCP_RETRANSMISSION_INTERVAL   1 * 1000//1s

//Add for IPA wakeup
struct oppo_tcp_hook_struct {
	kuid_t uid;
	bool is_ipv6;
	u32 ipv4_addr;
	u64 ipv6_addr1;
	u64 ipv6_addr2;
	u64 set[OPPO_TOTAL_IP_SUM*3];
};

//Add for QMI wakeup
extern void oppo_match_qrtr_service_port(int type, int id, int port);
extern void oppo_match_qrtr_wakeup(int src_node, int src_port, int dst_port, unsigned int arg1, unsigned int arg2);

extern void oppo_nwpower_hook_on(bool normal);
extern void oppo_nwpower_hook_off(bool normal, bool unsl);

#endif /* __OPPO_NWPOWER_H_ */