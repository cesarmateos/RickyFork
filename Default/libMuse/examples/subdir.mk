################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libMuse/examples/libmuse.c 

OBJS += \
./libMuse/examples/libmuse.o 

C_DEPS += \
./libMuse/examples/libmuse.d 


# Each subdirectory must supply rules for building sources it contributes
libMuse/examples/%.o: ../libMuse/examples/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


