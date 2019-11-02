################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kemmens/CommandInterpreter.c \
../kemmens/Serialization.c \
../kemmens/SocketClient.c \
../kemmens/SocketCommons.c \
../kemmens/SocketServer.c \
../kemmens/StringUtils.c \
../kemmens/ThreadManager.c \
../kemmens/ThreadPool.c \
../kemmens/config.c \
../kemmens/logger.c \
../kemmens/megekemmen.c 

OBJS += \
./kemmens/CommandInterpreter.o \
./kemmens/Serialization.o \
./kemmens/SocketClient.o \
./kemmens/SocketCommons.o \
./kemmens/SocketServer.o \
./kemmens/StringUtils.o \
./kemmens/ThreadManager.o \
./kemmens/ThreadPool.o \
./kemmens/config.o \
./kemmens/logger.o \
./kemmens/megekemmen.o 

C_DEPS += \
./kemmens/CommandInterpreter.d \
./kemmens/Serialization.d \
./kemmens/SocketClient.d \
./kemmens/SocketCommons.d \
./kemmens/SocketServer.d \
./kemmens/StringUtils.d \
./kemmens/ThreadManager.d \
./kemmens/ThreadPool.d \
./kemmens/config.d \
./kemmens/logger.d \
./kemmens/megekemmen.d 


# Each subdirectory must supply rules for building sources it contributes
kemmens/%.o: ../kemmens/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


