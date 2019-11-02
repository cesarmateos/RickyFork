################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SUSE/src/suse.c 

OBJS += \
./SUSE/src/suse.o 

C_DEPS += \
./SUSE/src/suse.d 


# Each subdirectory must supply rules for building sources it contributes
SUSE/src/%.o: ../SUSE/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


