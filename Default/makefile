################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm

# All of the sources participating in the build are defined here
-include sources.mk
-include libMuse/examples/subdir.mk
-include kemmens/subdir.mk
-include SUSE/src/subdir.mk
-include MUSE/src/subdir.mk
-include Hilolay/subdir.mk
-include FUSE/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: tp-2019-2c-ricky-fork

# Tool invocations
tp-2019-2c-ricky-fork: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "tp-2019-2c-ricky-fork" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) tp-2019-2c-ricky-fork
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
