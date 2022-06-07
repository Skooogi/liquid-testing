################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FreeRTOS/croutine.c \
../FreeRTOS/event_groups.c \
../FreeRTOS/list.c \
../FreeRTOS/queue.c \
../FreeRTOS/stream_buffer.c \
../FreeRTOS/tasks.c \
../FreeRTOS/timers.c 

OBJS += \
./FreeRTOS/croutine.o \
./FreeRTOS/event_groups.o \
./FreeRTOS/list.o \
./FreeRTOS/queue.o \
./FreeRTOS/stream_buffer.o \
./FreeRTOS/tasks.o \
./FreeRTOS/timers.o 

C_DEPS += \
./FreeRTOS/croutine.d \
./FreeRTOS/event_groups.d \
./FreeRTOS/list.d \
./FreeRTOS/queue.d \
./FreeRTOS/stream_buffer.d \
./FreeRTOS/tasks.d \
./FreeRTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/%.o: ../FreeRTOS/%.c FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H743xx -c -I"C:/Users/topir/DEV/Kvarkensat/Drivers/CMSIS/Device/ST/STM32H7xx/Include" -I"C:/Users/topir/DEV/Kvarkensat/Drivers/STM32H7xx_HAL_Driver/Inc" -I"C:/Users/topir/DEV/Kvarkensat/USB_DEVICE/App" -I"C:/Users/topir/DEV/Kvarkensat/USB_DEVICE/Target" -I"C:/Users/topir/DEV/Kvarkensat/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/topir/DEV/Kvarkensat/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/topir/DEV/Kvarkensat/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/topir/DEV/Kvarkensat/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/topir/DEV/Kvarkensat/Drivers/CMSIS/Include" -I"C:/Users/topir/DEV/Kvarkensat/FreeRTOS/include" -I"C:/Users/topir/DEV/Kvarkensat/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/topir/DEV/Kvarkensat/Core/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FreeRTOS

clean-FreeRTOS:
	-$(RM) ./FreeRTOS/croutine.d ./FreeRTOS/croutine.o ./FreeRTOS/event_groups.d ./FreeRTOS/event_groups.o ./FreeRTOS/list.d ./FreeRTOS/list.o ./FreeRTOS/queue.d ./FreeRTOS/queue.o ./FreeRTOS/stream_buffer.d ./FreeRTOS/stream_buffer.o ./FreeRTOS/tasks.d ./FreeRTOS/tasks.o ./FreeRTOS/timers.d ./FreeRTOS/timers.o

.PHONY: clean-FreeRTOS

