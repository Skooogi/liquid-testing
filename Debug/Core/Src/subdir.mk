################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
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
./Core/Src/stm32h7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h7xx.d \
./Core/Src/telecommands.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/buffer.d ./Core/Src/buffer.o ./Core/Src/buffer.su ./Core/Src/bus.d ./Core/Src/bus.o ./Core/Src/bus.su ./Core/Src/dac.d ./Core/Src/dac.o ./Core/Src/dac.su ./Core/Src/decoder.d ./Core/Src/decoder.o ./Core/Src/decoder.su ./Core/Src/error.d ./Core/Src/error.o ./Core/Src/error.su ./Core/Src/filter.d ./Core/Src/filter.o ./Core/Src/filter.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/sdr.d ./Core/Src/sdr.o ./Core/Src/sdr.su ./Core/Src/stm32h7xx_hal_msp.d ./Core/Src/stm32h7xx_hal_msp.o ./Core/Src/stm32h7xx_hal_msp.su ./Core/Src/stm32h7xx_it.d ./Core/Src/stm32h7xx_it.o ./Core/Src/stm32h7xx_it.su ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32h7xx.d ./Core/Src/system_stm32h7xx.o ./Core/Src/system_stm32h7xx.su ./Core/Src/telecommands.d ./Core/Src/telecommands.o ./Core/Src/telecommands.su

.PHONY: clean-Core-2f-Src

