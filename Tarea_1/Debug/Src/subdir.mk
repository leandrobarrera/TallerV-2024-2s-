################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/CheckSourcesMain.c \
../Src/magic.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/CheckSourcesMain.o \
./Src/magic.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/CheckSourcesMain.d \
./Src/magic.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE=1 -c -I"//home/leandro/Documentos/GitHub/TallerV-2024-2s-/Tarea_1/Inc" -I/home/leandro/STM32CubeIDE/workspace_1.10.1/CMSIS-repo/STM32Cube_FW_F4_V1.27.0/Drivers/CMSIS/Include/ -I/home/leandro/CMSIS-repo/STM32Cube_FW_F4_V1.27.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/CheckSourcesMain.cyclo ./Src/CheckSourcesMain.d ./Src/CheckSourcesMain.o ./Src/CheckSourcesMain.su ./Src/magic.cyclo ./Src/magic.d ./Src/magic.o ./Src/magic.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

