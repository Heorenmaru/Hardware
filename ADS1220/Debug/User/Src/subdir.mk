################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Src/crclib.c \
../User/Src/handlers.c \
../User/Src/kernel.c \
../User/Src/usb_snd.c 

OBJS += \
./User/Src/crclib.o \
./User/Src/handlers.o \
./User/Src/kernel.o \
./User/Src/usb_snd.o 

C_DEPS += \
./User/Src/crclib.d \
./User/Src/handlers.d \
./User/Src/kernel.d \
./User/Src/usb_snd.d 


# Each subdirectory must supply rules for building sources it contributes
User/Src/%.o User/Src/%.su: ../User/Src/%.c User/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xB -c -I../Core/Inc -I../User/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-User-2f-Src

clean-User-2f-Src:
	-$(RM) ./User/Src/crclib.d ./User/Src/crclib.o ./User/Src/crclib.su ./User/Src/handlers.d ./User/Src/handlers.o ./User/Src/handlers.su ./User/Src/kernel.d ./User/Src/kernel.o ./User/Src/kernel.su ./User/Src/usb_snd.d ./User/Src/usb_snd.o ./User/Src/usb_snd.su

.PHONY: clean-User-2f-Src

