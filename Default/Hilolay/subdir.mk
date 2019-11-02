################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Hilolay/example.c \
../Hilolay/hilolay.c 

OBJS += \
./Hilolay/example.o \
./Hilolay/hilolay.o 

C_DEPS += \
./Hilolay/example.d \
./Hilolay/hilolay.d 


# Each subdirectory must supply rules for building sources it contributes
Hilolay/%.o: ../Hilolay/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


