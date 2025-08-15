################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../STM32CubeIDE/Application/User/Core/syscalls.c \
../STM32CubeIDE/Application/User/Core/sysmem.c 

OBJS += \
./STM32CubeIDE/Application/User/Core/syscalls.o \
./STM32CubeIDE/Application/User/Core/sysmem.o 

C_DEPS += \
./STM32CubeIDE/Application/User/Core/syscalls.d \
./STM32CubeIDE/Application/User/Core/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
STM32CubeIDE/Application/User/Core/%.o STM32CubeIDE/Application/User/Core/%.su STM32CubeIDE/Application/User/Core/%.cyclo: ../STM32CubeIDE/Application/User/Core/%.c STM32CubeIDE/Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../Core/Inc -I../../USB_DEVICE/App -I../../USB_DEVICE/Target -I../../Drivers/STM32F1xx_HAL_Driver/Inc -I../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../Drivers/CMSIS/Include -IC:/Users/sargs/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Drivers/STM32F1xx_HAL_Driver/Inc -IC:/Users/sargs/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -IC:/Users/sargs/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -IC:/Users/sargs/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -IC:/Users/sargs/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Drivers/CMSIS/Device/ST/STM32F1xx/Include -IC:/Users/sargs/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-STM32CubeIDE-2f-Application-2f-User-2f-Core

clean-STM32CubeIDE-2f-Application-2f-User-2f-Core:
	-$(RM) ./STM32CubeIDE/Application/User/Core/syscalls.cyclo ./STM32CubeIDE/Application/User/Core/syscalls.d ./STM32CubeIDE/Application/User/Core/syscalls.o ./STM32CubeIDE/Application/User/Core/syscalls.su ./STM32CubeIDE/Application/User/Core/sysmem.cyclo ./STM32CubeIDE/Application/User/Core/sysmem.d ./STM32CubeIDE/Application/User/Core/sysmem.o ./STM32CubeIDE/Application/User/Core/sysmem.su

.PHONY: clean-STM32CubeIDE-2f-Application-2f-User-2f-Core

