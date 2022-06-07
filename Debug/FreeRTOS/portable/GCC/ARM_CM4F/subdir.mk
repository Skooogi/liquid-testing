################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FreeRTOS/portable/GCC/ARM_CM4F/port.c 

OBJS += \
./FreeRTOS/portable/GCC/ARM_CM4F/port.o 

C_DEPS += \
./FreeRTOS/portable/GCC/ARM_CM4F/port.d 


# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/portable/GCC/ARM_CM4F/%.o FreeRTOS/portable/GCC/ARM_CM4F/%.su: ../FreeRTOS/portable/GCC/ARM_CM4F/%.c FreeRTOS/portable/GCC/ARM_CM4F/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H743xx -c -I"/home/lndn/Documents/Work/KvarkenSat/Kvarkensat/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"/home/lndn/Documents/Work/KvarkenSat/Kvarkensat/Drivers/STM32H7xx_HAL_Driver/Inc" -I"/home/lndn/Documents/Work/KvarkenSat/Kvarkensat/USB_DEVICE/App" -I"/home/lndn/Documents/Work/KvarkenSat/Kvarkensat/USB_DEVICE/Target" -I"/home/lndn/Documents/Work/KvarkenSat/Kvarkensat/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"/home/lndn/Documents/Work/KvarkenSat/Kvarkensat/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/home/lndn/Documents/Work/KvarkenSat/Kvarkensat/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"/home/lndn/Documents/Work/KvarkenSat/Kvarkensat/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/lndn/Documents/Work/KvarkenSat/Kvarkensat/Drivers/CMSIS/Include" -I"/home/lndn/Documents/Work/KvarkenSat/Kvarkensat/FreeRTOS/include" -I"/home/lndn/Documents/Work/KvarkenSat/Kvarkensat/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/lndn/Documents/Work/KvarkenSat/Kvarkensat/Core/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FreeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F

clean-FreeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F:
	-$(RM) ./FreeRTOS/portable/GCC/ARM_CM4F/port.d ./FreeRTOS/portable/GCC/ARM_CM4F/port.o ./FreeRTOS/portable/GCC/ARM_CM4F/port.su

.PHONY: clean-FreeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F

