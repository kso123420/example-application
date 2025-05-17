#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/usb/class/hid.h>
#include <zephyr/usb/usb_device.h>

#define PMW3360_REG_PID 0x00
#define SPI_NODE DT_NODELABEL(spi1)

static const struct device *spi_dev;
static const struct spi_cs_control cs_ctrl = {
    .gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(pmw3360)),
    .delay = 0,
};

static const struct spi_config spi_cfg = {
    .frequency = 2000000,
    .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
    .cs = &cs_ctrl,
};

uint8_t pmw3360_read(uint8_t reg) {
    uint8_t tx_buf[2] = {reg & 0x7F, 0};
    uint8_t rx_buf[2];

    const struct spi_buf tx_bufs[] = {
        { .buf = tx_buf, .len = 2 }
    };
    const struct spi_buf rx_bufs[] = {
        { .buf = rx_buf, .len = 2 }
    };

    spi_transceive(spi_dev, &spi_cfg, tx_bufs, rx_bufs);
    return rx_buf[1];
}

void main(void) {
    usb_enable(NULL);
    spi_dev = DEVICE_DT_GET(SPI_NODE);
    if (!device_is_ready(spi_dev)) {
        printk("SPI device not ready\n");
        return;
    }

    printk("Testing PMW3360...\n");
    while (1) {
        uint8_t pid = pmw3360_read(PMW3360_REG_PID);
        printk("Product ID: 0x%02X\n", pid);
        k_sleep(K_MSEC(1000));
    }
}
