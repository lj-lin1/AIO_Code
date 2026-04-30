#include "w25qxx.h"

/* 片选控制（你自己根据工程实现） */
#define CS_LOW() HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET)
#define CS_HIGH() HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET)

/* ================= 私有变量 ================= */

static SPI_HandleTypeDef *w25_spi;

/* ================= SPI封装 ================= */

static void SPI_Write(uint8_t *tx, uint16_t len)
{
    HAL_SPI_Transmit(w25_spi, tx, len, HAL_MAX_DELAY);
}

static uint8_t SPI_RW(uint8_t data)
{
    uint8_t rx;
    HAL_SPI_TransmitReceive(w25_spi, &data, &rx, 1, HAL_MAX_DELAY);
    return rx;
}

static void SPI_Read(uint8_t *rx, uint32_t len)
{
    while (len--)
    {
        *rx++ = SPI_RW(0xFF);
    }
}

/* ================= 内部函数 ================= */

static void WriteEnable(void)
{
    uint8_t cmd = CMD_WRITE_ENABLE;

    CS_LOW();
    SPI_Write(&cmd, 1);
    CS_HIGH();
}

static void WaitBusy(void)
{
    uint8_t cmd = CMD_READ_STATUS1;
    uint8_t status;

    CS_LOW();
    SPI_Write(&cmd, 1);

    do
    {
        status = SPI_RW(0xFF);
    } while (status & 0x01);

    CS_HIGH();
}

static void SendAddr(uint32_t addr)
{
    uint8_t a[4] =
        {
            (addr >> 24) & 0xFF,
            (addr >> 16) & 0xFF,
            (addr >> 8) & 0xFF,
            addr & 0xFF};

    SPI_Write(a, 4);
}

/* ================= 初始化 ================= */

void W25Q256_Init(SPI_HandleTypeDef *hspi)
{
    w25_spi = hspi;

    /* 进入4字节地址模式 */
    uint8_t cmd = CMD_ENTER_4BYTE;

    CS_LOW();
    SPI_Write(&cmd, 1);
    CS_HIGH();
}

/* ================= 基本功能 ================= */

uint32_t W25Q256_ReadJEDECID(void)
{
    uint8_t cmd = CMD_JEDEC_ID;
    uint8_t id[3];

    CS_LOW();
    SPI_Write(&cmd, 1);
    SPI_Read(id, 3);
    CS_HIGH();

    return (id[0] << 16) | (id[1] << 8) | id[2];
}

void W25Q256_ReadUniqueID(uint8_t *id)
{
    uint8_t cmd = CMD_UNIQUE_ID;

    CS_LOW();
    SPI_Write(&cmd, 1);

    /* 4字节 dummy */
    for (int i = 0; i < 4; i++)
        SPI_RW(0xFF);

    SPI_Read(id, 8);

    CS_HIGH();
}

void W25Q256_Read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t cmd = CMD_READ_DATA;

    CS_LOW();
    SPI_Write(&cmd, 1);
    SendAddr(addr);
    SPI_Read(buf, len);
    CS_HIGH();
}

void W25Q256_Write(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint32_t chunk;

    while (len)
    {
        chunk = 256 - (addr % 256);
        if (chunk > len)
            chunk = len;

        WriteEnable();

        uint8_t cmd = CMD_PAGE_PROGRAM;

        CS_LOW();
        SPI_Write(&cmd, 1);
        SendAddr(addr);
        SPI_Write(buf, chunk);
        CS_HIGH();

        WaitBusy();

        addr += chunk;
        buf += chunk;
        len -= chunk;
    }
}

/* ================= 擦除 ================= */

void W25Q256_SectorErase(uint32_t addr)
{
    WriteEnable();

    uint8_t cmd = CMD_SECTOR_ERASE;

    CS_LOW();
    SPI_Write(&cmd, 1);
    SendAddr(addr);
    CS_HIGH();

    WaitBusy();
}

void W25Q256_BlockErase64K(uint32_t addr)
{
    WriteEnable();

    uint8_t cmd = CMD_BLOCK_ERASE_64K;

    CS_LOW();
    SPI_Write(&cmd, 1);
    SendAddr(addr);
    CS_HIGH();

    WaitBusy();
}

void W25Q256_ChipErase(void)
{
    WriteEnable();

    uint8_t cmd = CMD_CHIP_ERASE;

    CS_LOW();
    SPI_Write(&cmd, 1);
    CS_HIGH();

    WaitBusy();
}

/* ================= 智能擦除 ================= */

static void EraseRange(uint32_t addr, uint32_t len)
{
    uint32_t end = addr + len;

    while (addr < end)
    {
        /* 优先64K */
        if ((addr % 0x10000 == 0) && (end - addr >= 0x10000))
        {
            W25Q256_BlockErase64K(addr);
            addr += 0x10000;
        }
#if W25Q256_USE_32K_ERASE
        /* 次优32K */
        else if ((addr % 0x8000 == 0) && (end - addr >= 0x8000))
        {
            WriteEnable();

            uint8_t cmd = CMD_BLOCK_ERASE_32K;

            CS_LOW();
            SPI_Write(&cmd, 1);
            SendAddr(addr);
            CS_HIGH();

            WaitBusy();

            addr += 0x8000;
        }
#endif
        /* 最小4K */
        else
        {
            W25Q256_SectorErase(addr);
            addr += 0x1000;
        }
    }
}

/* ================= 自动擦除写入 ================= */

void W25Q256_WriteAutoErase(uint32_t addr, uint8_t *buf, uint32_t len)
{
    /* 先擦除 */
    EraseRange(addr, len);

    /* 再写入 */
    W25Q256_Write(addr, buf, len);
}

/* ================= 状态/低功耗 ================= */

uint8_t W25Q256_ReadStatus1(void)
{
    uint8_t cmd = CMD_READ_STATUS1;
    uint8_t status;

    CS_LOW();
    SPI_Write(&cmd, 1);
    status = SPI_RW(0xFF);
    CS_HIGH();

    return status;
}

void W25Q256_PowerDown(void)
{
    uint8_t cmd = CMD_POWER_DOWN;

    CS_LOW();
    SPI_Write(&cmd, 1);
    CS_HIGH();
}

void W25Q256_WakeUp(void)
{
    uint8_t cmd = CMD_RELEASE_POWERDOWN;

    CS_LOW();
    SPI_Write(&cmd, 1);
    CS_HIGH();
}