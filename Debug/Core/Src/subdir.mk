################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/buffer.c \
../Core/Src/bus.c \
../Core/Src/dac.c \
../Core/Src/decoder.c \
../Core/Src/error.c \
../Core/Src/filter.c \
../Core/Src/main.c \
../Core/Src/sdr.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_hal_timebase_tim.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h7xx.c \
../Core/Src/telecommands.c 

OBJS += \
./Core/Src/adc.o \
./Core/Src/buffer.o \
./Core/Src/bus.o \
./Core/Src/dac.o \
./Core/Src/decoder.o \
./Core/Src/error.o \
./Core/Src/filter.o \
./Core/Src/main.o \
./Core/Src/sdr.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_hal_timebase_tim.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h7xx.o \
./Core/Src/telecommands.o 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/buffer.d \
./Core/Src/bus.d \
./Core/Src/dac.d \
./Core/Src/decoder.d \
./Core/Src/error.d \
./Core/Src/filter.d \
./Core/Src/main.d \
./Core/Src/sdr.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_hal_timebase_tim.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h7xx.d \
./Core/Src/telecommands.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H743xx -c -I"C:/Users/topir/DEV/Kvarkensat/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"C:/Users/topir/DEV/Kvarkensat/Drivers/STM32H7xx_HAL_Driver/Inc" -I"C:/Users/topir/DEV/Kvarkensat/USB_DEVICE/App" -I"C:/Users/topir/DEV/Kvarkensat/USB_DEVICE/Target" -I"C:/Users/topir/DEV/Kvarkensat/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/topir/DEV/Kvarkensat/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/topir/DEV/Kvarkensat/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/topir/DEV/Kvarkensat/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/topir/DEV/Kvarkensat/Drivers/CMSIS/Include" -I"C:/Users/topir/DEV/Kvarkensat/FreeRTOS/include" -I"C:/Users/topir/DEV/Kvarkensat/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/topir/DEV/Kvarkensat/Core/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/buffer.d ./Core/Src/buffer.o ./Core/Src/bus.d ./Core/Src/bus.o ./Core/Src/dac.d ./Core/Src/dac.o ./Core/Src/decoder.d ./Core/Src/decoder.o ./Core/Src/error.d ./Core/Src/error.o ./Core/Src/filter.d ./Core/Src/filter.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/sdr.d ./Core/Src/sdr.o ./Core/Src/stm32h7xx_hal_msp.d ./Core/Src/stm32h7xx_hal_msp.o ./Core/Src/stm32h7xx_hal_timebase_tim.d ./Core/Src/stm32h7xx_hal_timebase_tim.o ./Core/Src/stm32h7xx_it.d ./Core/Src/stm32h7xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32h7xx.d ./Core/Src/system_stm32h7xx.o ./Core/Src/telecommands.d ./Core/Src/telecommands.o

.PHONY: clean-Core-2f-Src

