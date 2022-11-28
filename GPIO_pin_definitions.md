# GPIO pin definitions

## ESP32 + INA3221 module

|GPIO|Bus|module|function|
|----|---|------|--------|
|01|UART_0|CP2102 USB|TX|
|02|VSPI|n/a|Built-in LED|
|03|UART_0|CP2102 USB|RX|
|04|n/a|n/a|(not in use)|
|05|VSPI|n/a|CS (not in use)|
|06|SPI|FLASH|(reserved!)|
|07|SPI|FLASH|(reserved!)|
|08|SPI|FLASH|(reserved!)|
|09|SPI|FLASH|(reserved!)|
|10|SPI|FLASH|(reserved!)|
|11|SPI|FLASH|(reserved!)|
|12|HSPI|n/a|MISO (not in use)|
|13|HSPI|n/a|MOSI (not in use)|
|14|HSPI|n/a|SCK (not in use)|
|15|HSPI|n/a|CS (not in use)|
|16|UART_2|n/a|RX (not in use)|
|17|UART_2|n/a|TX (not in use)|
|18|VSPI|n/a|SCK (not in use)|
|19|VSPI|n/a|MISO (not in use)|
|21|I2C_0|INA3221|SDA|
|22|I2C_0|INA3221|SCL|
|23|SPI|n/a|MOSI|
|25|n/a|Reed switch|Reed switch|
|26|n/a|n/a|(not in use)|
|27|n/a|n/a|(not in use)|
|32|I2C_1|BH1750|SDA (not in use)|
|33|I2C_1|BH1750|SCL (not in use)|
|34|n/a|n/a|(not in use)(input only)|
|35|n/a|n/a|(not in use)(input only)|
|36|n/a|n/a|(input only)|
|39|n/a|n/a|(input only)|
