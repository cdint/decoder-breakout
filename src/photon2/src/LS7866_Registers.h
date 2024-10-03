//==============================================================================
//
// Title:		LS7866_Registers.h
// Purpose: Define LS7866 Registers and Bit Fields
//
// Created on:  9/13/23
// Copyright:	  LSI Computer Systems Inc. All Rights Reserved.
//
//==============================================================================

#ifndef __LS7866_Registers_H__
#define __LS7866_Registers_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Constants

// LS7866 high-order address bits are fixed at 0x70
#define LS7866_I2C_FIXED_ADDR 0x70
#define LS7866_REG_AUTO_INC   0x10

#define MCR0_ADDR			0x00
#define MCR0_NON_QUAD		0x00
#define MCR0_QUAD_X1		0x01
#define MCR0_QUAD_X2		0x02
#define MCR0_QUAD_X4		0x03
#define MCR0_FREE_RUN		0x00
#define MCR0_NON_RECYCLE	0x04
#define MCR0_RANGE_LIMIT	0x06		
#define MCR0_MODULO_N		0x0C
#define MCR0_DISABLE_Z		0x00
#define MCR0_Z_LCI0			0x10
#define MCR0_Z_LCI1			0x20
#define MCR0_Z_RCNT			0x30
#define MCR0_Z_RDST			0x40
#define MCR0_Z_LSST			0x50		
#define MCR0_Z_LODC			0x60
#define MCR0_Z_RCNT_LODC	0x70
#define MCR0_Z_EDGE_FE		0x00
#define MCR0_Z_ACT_LOW		0x80
		
		
#define MCR1_ADDR			0x01
#define MCR1_CNTR_4BYTE		0x00
#define MCR1_CNTR_3BYTE		0x01
#define MCR1_CNTR_2BYTE		0x02
#define MCR1_CNTR_1BYTE		0x03
#define MCR1_COUNT_ENABLE	0x00
#define MCR1_COUNT_DISABLE	0x04
#define MCR1_nCLR_DSTR_ON_RD	0x00
#define MCR1_CLR_DSTR_ON_RD	0x40
#define MCR1_SSTR_ON_RD		0x00
#define MCR1_NO_SSTR_ON_RD	0x80

		
#define FCR_ADDR			0x02
#define FCR_CY	  	  0x01
#define FCR_BW		    0x02
#define FCR_EQL0			0x04
#define FCR_EQL1			0x08
#define FCR_IDX		    0x10
#define FCR_EQL1TGL		0x20		
#define FCR_BWTGL		  0x40
#define FCR_DIR 			0x80
		
#define TPR_ADDR			0x05
#define TPR_RCNT			0x01
#define TPR_LCI0			0x02
#define TPR_LCI1			0x04
#define TPR_LODC			0x08
#define TPR_LSST			0x10
#define TPR_RDST			0x20
#define TPR_DSTR_S			0x40
#define TPR_MRST			0x80
		
#define DSTR
#define DSTR_PLS			0x01
#define DSTR_CE				0x02
#define DSTR_IDX			0x04
#define DSTR_EQL0			0x08
#define DSTR_EQL1			0x10
#define DSTR_BW				0x20
#define DSTR_CY				0x40
#define DSTR_S				0x80
		
		
#define SSTR_ADDR			0x08
#define SSTR_PLS			0x01
#define SSTR_CE				0x02
#define SSTR_IDX			0x04
#define SSTR_EQL0			0x08
#define SSTR_EQL1			0x10
#define SSTR_BW				0x20
#define SSTR_CY				0x40
#define SSTR_UD				0x80
		
#define IDR0_ADDR			0x03
#define IDR1_ADDR			0x04
#define ODR_ADDR			0x06
#define CNTR_ADDR			0x07
		

#ifdef __cplusplus
    }
#endif

#endif  // End of file.
