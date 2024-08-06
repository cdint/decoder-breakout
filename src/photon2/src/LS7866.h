/*
   LS7866.h - Library for using the LSI/CSI LS7866 quadrature encoder counter with I2C interface
   Created by Tin Chiang
   License: MIT license
*/

#ifndef LS7866_h
#define LS7866_h


/** Necessary library for STM32 HAL */
// #include "stm32l4xx_hal.h" /* Swap the HAL library based on you hardware*/

/** Define Register address size and register size in byte*/
#define LS7866_REG_ADD_SIZE 0x1
#define LS7866_DATA_SIZE_1 0x1 
#define LS7866_DATA_SIZE_2 0x2
#define LS7866_DATA_SIZE_3 0x3
#define LS7866_DATA_SIZE_4 0x4

/** Default LS7866 I2C register address. */
#define LS7866_MCR0 0x00 /* RD&WR */
#define LS7866_MCR1 0x01 /* RD&WR */
#define LS7866_FCR  0x02 /* RD&WR */
#define LS7866_IDR0 0x03 /* RD&WR */
#define LS7866_IDR1 0x04 /* RD&WR */
#define LS7866_TPR  0x05 /* WR */
#define LS7866_ODR  0x06 /* RD */
#define LS7866_CNTR 0x07 /* RD */
#define LS7866_SSTR 0x08 /* RD */

/* MDR0 configuration data
   The configutation byte is formed by choosing one element from each group.
   
   *Note: At power-up the MCR0 is cleared to 0. 

   *Definition of running modes:
    *Non-Recycle mode: 
        CNTR freezes at CNTR = IDR1+1 with generation of EQL1 in up and at CNTR = -1 with generation of BW in down count modes. Counting re-enabled with Load CNTR or Reset CNTR.

    *Range-Limit mode:
        p and Down count ranges are limited between IDR1 (high) and IDR0 (low) respectively. CNTR freezes at these limits resuming counting when the direction reverses.

    *Modulo-n:
        nput count clock frequency fc is divided by a factor of (n+1) in both up and down directions, where n = IDR1. The resultant frequency, fo = fc/(n+1) is made available at the FLAG/ output when FCR register bits B[1] and B[3] are set to 1. A frequency of fo = fc/[2*(n+1)] results at the FLAG/ output when the FCR register bits B[5] and B[6] are set to 1.
*/

//Count modes, B[1:0] 
#define NQUAD 0x00u                //non-quadrature mode (A = count clock, B = up/down control) 
#define QUADRX1 0x01u              //X1 quadrature mode (1 count per quad)
#define QUADRX2 0x02u              //X2 quadrature mode (2 count per quad) 
#define QUADRX4 0x03u              //X4 quadrature mode (4 count per quad)

//Running modes, B[3:2] 
#define FREE_RUN 0x00u << 2
#define SINGE_CYCLE 0x01u << 2 
#define RANGE_LIMIT 0x02u << 2 
#define MODULO_N 0x03u << 2

//Pin Z modes, B[6:4]
#define Z_DISABLE   0x00u << 4
#define LCI0_INPUT  0x01u << 4      //CNTR <= IDR0
#define LCI1_INPUT  0x02u << 4      //CNTR <= IDR1
#define RCNT_INPUT  0x03u << 4      //reset CNTR
#define RDST_INPUT  0x04u << 4      //reset DSTR
#define LSST_INPUT  0x05u << 4      //SSTR <= DSTR
#define LODC_INPUT  0x06u << 4      //ODR <= CNTR
#define RCNT_LODC_INPUT 0x07u << 4  /* In this mode Z input is edge sensitive; high-to-low   transition performs LODC function and low-to-high transition performs RCNT function. This configuration overrides B[7] setting.*/

//Pin Z trigger modes
#define HIGH_TO_LOW 0x00 << 7
#define LOW_ACTIVE  0x01 << 7


/* MDR1 configuration data
   The configutation byte is formed by choosing one element from each group.
   Note: At power-up the MCR1 is cleared to 0.
*/

//1 to 4 bytes data-width, B[1:0]
#define BYTE_4 0x00         //four byte mode (Affects IDR0, IDR1, ODR, CNTR) 
#define BYTE_3 0x01         //three byte mode (Affects IDR0, IDR1, ODR, CNTR)
#define BYTE_2 0x02         //two byte mode (Affects IDR0, IDR1, ODR, CNTR)
#define BYTE_1 0x03         //one byte mode (Affects IDR0, IDR1, ODR, CNTR)

//Enable or disable counter, B[2]
#define EN_CNTR 0x00u << 2  //counting enabled 
#define DIS_CNTR 0x01 << 2  //counting disabled

/*  Note for B[6] and B[7]:
    With B[6] = 1 and B[7] = 0, a RD_CNTR operation also simultaneously uploads DSTR into SSTR followed by a reset of DSTR.
*/

//Reset or not reset the DSTR when CNTR is read, B[6]
#define DSTR_NRST_CNTR  0x00 << 6
#define DSTR_RST_CNTR   0x01 << 6

//Load or not load the SSTR with DSTR when CNTR is read, B[7]
#define SSTR_NLOAD_CNTR_RD  0x00 << 7
#define SSTR_LOAD_CNTR_RD   0x01 << 7

/* FCR configuration data
   The configutation byte is formed by choosing one element from each group.
   Note: At power-up the FCR is cleared to 0.
*/
// FCR register bits
#define ENABLE_CY       0x01u  // Bit 0: Enable CY (CNTR Overflow)
#define ENABLE_BW       0x02u  // Bit 1: Enable BW (CNTR Underflow)
#define ENABLE_EQL0     0x04u  // Bit 2: Enable EQL0 (CNTR = IDR0)
#define ENABLE_EQL1     0x08u  // Bit 3: Enable EQL1 (CNTR = IDR1)
#define ENABLE_IDX      0x10u  // Bit 4: Enable IDX (Z Input Active)
#define ENABLE_EQL1TGL  0x20u  // Bit 5: Enable EQL1TGL (Toggle at CNTR = IDR1)
#define ENABLE_BWTGL    0x40u  // Bit 6: Enable BWTGL (Toggle at BW)
#define ENABLE_DIR      0x80u  // Bit 7: Enable Count Direction (DFLAG: 0=DOWN, 1=UP)

/* TPR configuration data
   The configutation byte is formed by choosing one element from each group.
   Note: Data written into TPR are not stored, thereby releasing the referenced registers for normal operation following a write to TPR.
*/
#define RCNT            0x01u  // Bit 0: Reset CNTR (CNTR <= 0)( B[0] = 1 overrides B[1], B[2] and B[6] )
#define LCI0            0x02u  // Bit 1: Load CNTR with IDR0 (CNTR <= IDR0)
#define LCI1            0x04u  // Bit 2: Load CNTR with IDR1 (CNTR <= IDR1)
#define LODC            0x08u  // Bit 3: Load ODR with CNTR (ODR <= CNTR)
#define LSST            0x10u  // Bit 4: Load SSTR with DSTR (SSTR <= DSTR)
#define RDST            0x20u  // Bit 5: Reset DSTR (DSTR <= 0) and LFLAG/ 
// no function for Bit 6
#define MRST            0x80u  // Bit 7: Resets IDR0, IDR1, CNTR, ODR, DSTR, SSTR and LFLAG/(MRST overrides B[1] through B[4] and B[6].) 

/* SSTR bit definition
   Use the definition as bit mask
   Note: SSTR stores the instataneous snap-shot of the DSTR, and is reset to 0 at power-up

   Ways to capture the DSTR in SSTR
    * Event control such as Z input
    * Program control with LSST command
    * Configure MCR1 so that DSTR is capture when CNTR is read (LSST command automatically executed after LODC command 
*/
#define PLS             0x01u  // Bit 0: Set to one upon power up. "Power loss" indicator
#define CE              0x02u  // Bit 1: High indicates "counting enabled"
#define IDX             0x04u  // Bit 2: High indicates Z input in active state
#define EQL0            0x08u  // Bit 3: High indicates CNTR = IDR0
#define EQL1            0x10u  // Bit 4: High indicates CNTR = IDR1
#define BW              0x20u  // Bit 5: High indicates CNTR underflow
#define CY              0x40u  // Bit 6: High indicates CNTR overflow
#define UD              0x80u  // Bit 7: High indicates count up, low indicates count down


class LS7866
{
private:
    /* data */
    // HAL handler
    I2C_HandleTypeDef *i2c_hal;
    uint8_t device_address;

    uint8_t data_width = 0; // data can be from 1 to 4 bytes


public:
    /*  deviceAddress = 0x70 .. 0x77 
        device address is set by pin A0, A1, and A2
        
        *A2 A1 A0 = 000 -> 0x70
        *A2 A1 A0 = 001 -> 0x71
        *A2 A1 A0 = 010 -> 0x72
        *A2 A1 A0 = 011 -> 0x73
        *A2 A1 A0 = 100 -> 0x74
        *A2 A1 A0 = 101 -> 0x75
        *A2 A1 A0 = 110 -> 0x76
        *A2 A1 A0 = 111 -> 0x77
    */
    LS7866(uint8_t deviceAddress, I2C_HandleTypeDef *hi2c);
    bool begin();

    // data modify
    bool write_MDR0(uint8_t data);
    bool write_MDR1(uint8_t data);
    bool write_FCR(uint8_t data);

    // data access
    uint8_t read_MCR0();
    uint8_t read_MCR1();
    uint32_t read_CNTR();
    uint32_t read_ODR();

    // command
    void reset_CNTR();
    void reset_DSTR();
    void load_ODR();


    // Hardware specific layer, HAL functions are called here
    bool write_reg(uint8_t address, uint8_t data, uint8_t num_bytes);
    bool read_reg(uint8_t reg_address, uint8_t* data_buff, uint16_t num_bytes);

    
};





#endif