#include "LS7866.h"

LS7866::LS7866(uint8_t deviceAddress, I2C_HandleTypeDef *hi2c)
{
    //assume the I2c module initialization has been done in the main.c
    i2c_hal = hi2c;    
    device_address = deviceAddress;    
}

// Data access
uint8_t LS7866::read_MCR0()
{
    uint8_t data;
    bool err;
    err = read_reg(LS7866_MCR0, &data, LS7866_DATA_SIZE_1);
    if(err){
        return 0;        
    }
    return data;
}
uint8_t LS7866::read_MCR1()
{
    uint8_t data;
    bool err;
    err = read_reg(LS7866_MCR1, &data, LS7866_DATA_SIZE_1);
    if(err){
        return 0;        
    }
    return data;
}
uint8_t LS7866::read_FCR()
{
    uint8_t data;
    bool err;
    err = read_reg(LS7866_FCR, &data, LS7866_DATA_SIZE_1);
    if(err){
        return 0;        
    }
    return data;
}

uint32_t LS7866::read_CNTR()
{
    uint32_t data; // for combine the data
    uint8_t data_buffer[data_width]
    bool err;
    err = read_reg(LS7866_CNTR, &data_buffer, data_width);
    if(err){
        return 0;        
    }
    // TODO:
    // Need to figure out how the read n bytes work: how does byte 1-4 fit into the data buffer?
    // How to construct the data from data buffer?
    // if(data_width == LS7866_DATA_SIZE_1)
    // {
    //     data = (uint32_t)(data_buffer[0])
    // }
    return data;
}

uint32_t LS7866::read_ODR()
{
    // TODO:
    // Need to figure out how the read n bytes work: how does byte 1-4 fit into the data buffer?
     // How to construct the data from data buffer?
}

// Hardware specific layer, HAL functions are called here
bool LS7866::write_reg(uint8_t reg_address, uint8_t* data_buff, uint16_t num_bytes)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_Mem_Write(i2c_hal, (uint16_t)(device_address<<1), (uint16_t)(reg_address), LS7866_REG_ADD_SIZE, data_buff, num_bytes, HAL_MAX_DELAY);

    // check if the transmission is successful
    if(ret != HAL_OK){
        return false;
    }
    return true;
}
bool read_reg(uint8_t reg_address, uint8_t* data_buff, uint16_t num_bytes)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_Mem_Read(i2c_hal, (uint16_t)(device_address <<1), (uint16_t)(reg_address), LS7866_REG_ADD_SIZE, data_buff, num_bytes, HAL_MAX_DELAY);

    // check if the data receive is successful
    if(ret != HAL_OK){
        return false;
    }
    return true;
}