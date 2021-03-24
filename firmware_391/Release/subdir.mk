################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../audio.cpp \
../main.cpp \
../parser.cpp \
../rs232.cpp 

OBJS += \
./audio.o \
./main.o \
./parser.o \
./rs232.o 

CPP_DEPS += \
./audio.d \
./main.d \
./parser.d \
./rs232.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Arm C Compiler 5'
	armcc -O2 -g --md --depend_format=unix_escaped --no_depend_system_headers -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


