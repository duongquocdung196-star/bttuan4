TARGET = firmware

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CPU = -mcpu=cortex-m3 -mthumb

CFLAGS = $(CPU) \
         -O1 \
         -Wall \
         -g \
         -ffreestanding \
         -fdata-sections \
         -ffunction-sections \
         -DSTM32F103xB \
         -DUSE_HAL_DRIVER

LDFLAGS = $(CPU) \
          -Tlinker.ld \
          -nostartfiles \
          -Wl,--gc-sections \
          -Wl,-Map=$(TARGET).map \
          -specs=nosys.specs
INCLUDES = \
    -ICore/Inc \
    -IDrivers/CMSIS/Core/Include \
    -IDrivers/CMSIS/Device/ST/STM32F1xx/Include \
    -IDrivers/STM32F1xx_HAL_Driver/Inc \
    -IFreeRTOS-Kernel/include \
    -IFreeRTOS-Kernel/portable/GCC/ARM_CM3

C_SOURCES = \
    main.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
    Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c \
    FreeRTOS-Kernel/tasks.c \
    FreeRTOS-Kernel/queue.c \
    FreeRTOS-Kernel/list.c \
    FreeRTOS-Kernel/croutine.c \
    FreeRTOS-Kernel/event_groups.c \
    FreeRTOS-Kernel/stream_buffer.c \
    FreeRTOS-Kernel/timers.c \
    FreeRTOS-Kernel/portable/GCC/ARM_CM3/port.c \
    FreeRTOS-Kernel/portable/MemMang/heap_4.c

ASM_SOURCES = \
    startup/startup_stm32f103xb.s

OBJECTS = $(C_SOURCES:.c=.o) \
          $(ASM_SOURCES:.s=.o)

all: $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.s
	$(CC) $(CPU) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET).elf $(TARGET).bin $(TARGET).map

flash: $(TARGET).elf
	openocd -f interface/stlink.cfg \
	        -f target/stm32f1x.cfg \
	        -c "program $(TARGET).elf verify reset exit"
