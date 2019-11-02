################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FUSE/src/SAC-Cli.c \
../FUSE/src/SAC-servidor.c 

OBJS += \
./FUSE/src/SAC-Cli.o \
./FUSE/src/SAC-servidor.o 

C_DEPS += \
./FUSE/src/SAC-Cli.d \
./FUSE/src/SAC-servidor.d 


# Each subdirectory must supply rules for building sources it contributes
FUSE/src/%.o: ../FUSE/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


