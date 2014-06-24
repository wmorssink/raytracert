################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OBJ_SRCS += \
../Balls.obj \
../cube.obj \
../dodgeColorTest.obj 

CPP_SRCS += \
../box.cpp \
../element.cpp \
../main.cpp \
../mesh.cpp \
../node.cpp \
../raytracing.cpp \
../trianglelist.cpp 

OBJS += \
./box.o \
./element.o \
./main.o \
./mesh.o \
./node.o \
./raytracing.o \
./trianglelist.o 

CPP_DEPS += \
./box.d \
./element.d \
./main.d \
./mesh.d \
./node.d \
./raytracing.d \
./trianglelist.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


