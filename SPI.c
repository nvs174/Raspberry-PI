#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() 
{
    // 1. Инициализация библиотеки pigpio
    if (gpioInitialise() < 0) {
        fprintf(stderr, "Ошибка инициализации pigpio\n");
        return 1;
    }

    // 2. Конфигурация SPI
    const int spi_channel = 0;     // SPI0 (использует CE0 - физический пин 24)
    const int spi_baud = 100000;  // Скорость 100 кбит/с
    const int spi_flags = 0;       // Режим SPI: 
                                   //   биты 1-0: CPOL и CPHA (0 = режим 0)
                                   //   бит 8: CS активен низким уровнем (по умолчанию)
                                   //   другие биты: 0 = значения по умолчанию

    // 3. Открытие SPI устройства
    int spi_handle = spiOpen(spi_channel, spi_baud, spi_flags);
    if (spi_handle < 0) {
        fprintf(stderr, "Ошибка открытия SPI: %d\n", spi_handle);
        gpioTerminate();
        return 1;
    }

    // 4. Подготовка данных (8-битные значения)
    unsigned char tx_data[] = {0xAA, 0x55, 0x01, 0x75}; // Чёткие шаблоны для анализа
    const size_t data_size = sizeof(tx_data); // = 4 байта

    printf("SPI конфигурапарпарация:\n");
    printf("  Канал: %d (CE%d)\n", spi_channel, spi_channel);
    printf("  Скорость: %d бод\n", spi_baud);
    printf("  Режим: %d (CPOL=%d, CPHA=%d)\n", 
           spi_flags & 3, 
           (spi_flags >> 3) & 1, 
           (spi_flags >> 2) & 1);
    printf("  Размер данных: %zu байт\n\n", data_size);

    // 5. Основной цикл передачи
    while(1) {
        // Отправка данных
        int status = spiWrite(spi_handle, (char*)tx_data, data_size);
        
        if (status < 0) {
            fprintf(stderr, "Ошибка SPI передачи: %d\n", status);
            break;
        } else {
            printf("Отправлено %zu байт: ", data_size);
            for (size_t i = 0; i < data_size; i++) {
                printf("%02X ", tx_data[i]);
            }
            printf("\n");
        }
        
        // Задержка 500 мс
        usleep(500000);
    }

    // 6. Корректное завершение
    spiClose(spi_handle);
    gpioTerminate();
    printf("\nSPI соединение закрыто\n");
    return 0;
}