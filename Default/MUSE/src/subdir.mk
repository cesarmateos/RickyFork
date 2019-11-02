################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MUSE/src/muse.c 

OBJS += \
./MUSE/src/muse.o 

C_DEPS += \
./MUSE/src/muse.d 


# Each subdirectory must supply rules for building sources it contributes
MUSE/src/%.o: ../MUSE/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


