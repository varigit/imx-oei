/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2022 NXP
 */

#ifndef __ASM_ARCH_IMX9_DDR_H
#define __ASM_ARCH_IMX9_DDR_H
#include <asm/arch/soc_memory_map.h>
#include <asm/io.h>

#if defined(CONFIG_IMX95)
#if defined(CA55)
#define DDRMIX_BCTRL_BASE	DDRMIX_BLK_CTRL_BASE
#define DDR_CTL_BASE		DDRC_BASE
#define DDR_PHY_BASE		DDRPHY_BASE
#define SRC_BASE_ADDR		SRC_RBASE
#elif defined(CM33)
#define DDRMIX_BCTRL_BASE	DDRMIX_BLK_CTRL_BASE
#define DDR_CTL_BASE		DDRC_BASE
#define DDR_PHY_BASE		DDRPHY_BASE
#define SRC_BASE_ADDR		SRC_RBASE
#else
#error "Please specify either -DCA55 or -DCM33 !"
#endif
#else
#error "Please define CONFIG_<SoC> !"
#endif

#define REG_DDR_SDRAM_MD_CNTL	(DDR_CTL_BASE + 0x120)
#define REG_DDR_CS0_BNDS        (DDR_CTL_BASE + 0x0)
#define REG_DDR_CS1_BNDS        (DDR_CTL_BASE + 0x8)
#define REG_DDRDSR_2		(DDR_CTL_BASE + 0xB24)
#define REG_DDR_TIMING_CFG_0	(DDR_CTL_BASE + 0x104)
#define REG_DDR_SDRAM_CFG	(DDR_CTL_BASE + 0x110)
#define REG_DDR_SDRAM_CFG2      (DDR_CTL_BASE + 0x114)
#define REG_DDR_TIMING_CFG_4	(DDR_CTL_BASE + 0x160)
#define REG_DDR_DEBUG_19	(DDR_CTL_BASE + 0xF48)
#define REG_DDR_SDRAM_CFG_3     (DDR_CTL_BASE + 0x260)
#define REG_DDR_SDRAM_CFG_4     (DDR_CTL_BASE + 0x264)
#define REG_DDR_SDRAM_MD_CNTL_2 (DDR_CTL_BASE + 0x270)
#define REG_DDR_SDRAM_MPR4      (DDR_CTL_BASE + 0x28C)
#define REG_DDR_SDRAM_MPR5      (DDR_CTL_BASE + 0x290)
#define REG_DDR_MTCR            (DDR_CTL_BASE + 0xd00)

#define REG_DDR_ERR_EN        	(DDR_CTL_BASE + 0x1000)

#define IP2APB_DDRPHY_IPS_BASE_ADDR(X)		(DDR_PHY_BASE + (X * 0x2000000))
#define DDRPHY_MEM(X)				(DDR_PHY_BASE + (X * 0x2000000) + 0x50000)

#define IMEM_LEN 0x10000 /* 65536 bytes */
#define DMEM_LEN 0x10000 /* 65536 bytes */

#define IMEM_OFFSET_ADDR 0x00050000
#define DMEM_OFFSET_ADDR 0x00058000

/* PHY State */
enum pstate {
	PS0,
	PS1,
	PS2,
	PS3,
};

enum msg_response {
	TRAIN_SUCCESS = 0x7,
	TRAIN_STREAM_START = 0x8,
	TRAIN_FAIL = 0xff,
};

/* user data type */
enum fw_type {
	FW_1D_IMAGE,
	FW_2D_IMAGE,
};

enum mem_type {
	DMEM,
	IMEM,
};

struct ddrc_cfg_param {
	unsigned int reg;
	unsigned int val;
};

struct __attribute__((__packed__)) ddrphy_cfg_param {
	unsigned int   reg;
	unsigned short val;
};

struct dram_fsp_cfg {
	struct ddrc_cfg_param *ddrc_cfg;
	unsigned int ddrc_cfg_num;
	struct ddrc_cfg_param *mr_cfg;
	unsigned int mr_cfg_num;
	unsigned int bypass;
};

struct dram_fsp_msg {
	unsigned int drate;
	enum fw_type fw_type;
	/* pstate ddrphy config */
	struct ddrphy_cfg_param *fsp_phy_cfg;
	unsigned int fsp_phy_cfg_num;
	/* pstate message block(header) */
	struct ddrphy_cfg_param *fsp_phy_msgh_cfg;
	unsigned int fsp_phy_msgh_cfg_num;
	/* pstate PIE */
	struct ddrphy_cfg_param *fsp_phy_pie_cfg;
	unsigned int fsp_phy_pie_cfg_num;

	/* for simulation */
	struct ddrphy_cfg_param *fsp_phy_prog_csr_ps_cfg;
	unsigned int fsp_phy_prog_csr_ps_cfg_num;
};

struct dram_timing_info {
	/* ddrc config */
	struct ddrc_cfg_param *ddrc_cfg;
	unsigned int ddrc_cfg_num;
	/* ddrc pstate config */
	struct dram_fsp_cfg *fsp_cfg;
	unsigned int fsp_cfg_num;
	/* ddrphy config */
	struct ddrphy_cfg_param *ddrphy_cfg;
	unsigned int ddrphy_cfg_num;
	/* ddr fsp train info */
	struct dram_fsp_msg *fsp_msg;
	unsigned int fsp_msg_num;
	/* ddr phy trained CSR */
	struct ddrphy_cfg_param *ddrphy_trained_csr;
	unsigned int ddrphy_trained_csr_num;
	/* ddr phy common PIE */
	struct ddrphy_cfg_param *ddrphy_pie;
	unsigned int ddrphy_pie_num;
	/* initialized drate table */
	unsigned int fsp_table[4];
};

extern struct dram_timing_info dram_timing;

void ddr_load_train_firmware(struct dram_fsp_msg *fsp_msg, enum mem_type type);
int ddr_init(struct dram_timing_info *timing_info);
int ddr_cfg_phy(struct dram_timing_info *timing_info);

/* utils function for ddr phy training */
int wait_ddrphy_training_complete(void);
void ddrphy_init_set_dfi_clk(unsigned int drate);
void ddrphy_init_read_msg_block(void);
void ddrphy_cfg_set(struct ddrphy_cfg_param *cfg, unsigned int num);

#define ddrphy_addr_remap(paddr_apb_from_ctlr)	(paddr_apb_from_ctlr << 2)

#define dwc_ddrphy_apb_wr(addr, data) \
	writel(data, IP2APB_DDRPHY_IPS_BASE_ADDR(0) + ddrphy_addr_remap(addr))
#define dwc_ddrphy_apb_rd(addr) \
	readl(IP2APB_DDRPHY_IPS_BASE_ADDR(0) + ddrphy_addr_remap(addr))

/* Quick Boot related */
#if (!defined(DDR5) && !defined(DDR4x))
#error "Please specify either -DDDR5 or -DDDR4x !"
#endif

#define DDRPHY_QB_CSR_SIZE	5168
#define DDRPHY_QB_ACSM_SIZE	4 * 1024
#define DDRPHY_QB_MSB_SIZE	0x200

/**
 * No PSTATE SRAM is saved by default, one Pstate defined.
 * PSTATE SRAM to be saved if 3 and more Pstates are defined.
 */
#if (defined(DDRPHY_PSTATES) && DDRPHY_PSTATES >= 3)
#define DDRPHY_QB_PSTATES	DDRPHY_PSTATES
#else
#define DDRPHY_QB_PSTATES	0
#endif
#define DDRPHY_QB_PST_SIZE	DDRPHY_QB_PSTATES * 4 * 1024

#define ACSM_SRAM_BASE_ADDR	0x41000
#define PSTATE_SRAM_BASE_ADDR	0xA0000

#define QB_STATE_MEM		0x4aaf4000
typedef struct {
	u32 crc;
	u8 TrainedVREFCA_A0;
	u8 TrainedVREFCA_A1;
	u8 TrainedVREFCA_B0;
	u8 TrainedVREFCA_B1;
	u8 TrainedVREFDQ_A0;
	u8 TrainedVREFDQ_A1;
	u8 TrainedVREFDQ_B0;
	u8 TrainedVREFDQ_B1;
	u8 TrainedVREFDQU_A0;
	u8 TrainedVREFDQU_A1;
	u8 TrainedVREFDQU_B0;
	u8 TrainedVREFDQU_B1;
	u8 TrainedDRAMDFE_A0;
	u8 TrainedDRAMDFE_A1;
	u8 TrainedDRAMDFE_B0;
	u8 TrainedDRAMDFE_B1;
	u8 TrainedDRAMDCA_A0;
	u8 TrainedDRAMDCA_A1;
	u8 TrainedDRAMDCA_B0;
	u8 TrainedDRAMDCA_B1;
	u16 csr[DDRPHY_QB_CSR_SIZE];
	u16 acsm[DDRPHY_QB_ACSM_SIZE];
	u16 pst[DDRPHY_QB_PST_SIZE];
} ddrphy_qb_state;

#if defined(CONFIG_DDR_QBOOT)
int ddr_cfg_phy_qb(struct dram_timing_info *timing_info, int fsp_id);
void ddr_load_DMEM(u16 *msg_blk, ddrphy_qb_state *qb_state);
u32 ddr_get_qb_state_addr(void);
#else
void ddrphy_qb_save(void);
#endif
#endif
