################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Inc/RCC/RCC_Init.cpp 

OBJS += \
./Inc/RCC/RCC_Init.o 

CPP_DEPS += \
./Inc/RCC/RCC_Init.d 


# Each subdirectory must supply rules for building sources it contributes
Inc/RCC/RCC_Init.o: ../Inc/RCC/RCC_Init.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DNUCLEO_F401RE -DSTM32 -DSTM32F401RETx -DSTM32F4 -DDEBUG -c -I../Inc -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"Inc/RCC/RCC_Init.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

