################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sqlite3.c 

CPP_SRCS += \
../bncsutilinterface.cpp \
../bnet.cpp \
../bnetprotocol.cpp \
../bnlsclient.cpp \
../bnlsprotocol.cpp \
../commandpacket.cpp \
../config.cpp \
../crc32.cpp \
../csvparser.cpp \
../game.cpp \
../game_admin.cpp \
../game_base.cpp \
../gameplayer.cpp \
../gameprotocol.cpp \
../gameslot.cpp \
../ghost.cpp \
../ghostdb.cpp \
../ghostdbmysql.cpp \
../ghostdbsqlite.cpp \
../language.cpp \
../map.cpp \
../packed.cpp \
../replay.cpp \
../savegame.cpp \
../sha1.cpp \
../socket.cpp \
../stats.cpp \
../statsdota.cpp \
../statsw3mmd.cpp \
../util.cpp 

O_SRCS += \
../bncsutilinterface.o \
../bnet.o \
../bnetprotocol.o \
../bnlsclient.o \
../bnlsprotocol.o \
../commandpacket.o \
../config.o \
../crc32.o \
../csvparser.o \
../game.o \
../game_admin.o \
../game_base.o \
../gameplayer.o \
../gameprotocol.o \
../gameslot.o \
../ghost.o \
../ghostdb.o \
../ghostdbmysql.o \
../ghostdbsqlite.o \
../language.o \
../map.o \
../packed.o \
../replay.o \
../savegame.o \
../sha1.o \
../socket.o \
../sqlite3.o \
../stats.o \
../statsdota.o \
../statsw3mmd.o \
../util.o 

OBJS += \
./bncsutilinterface.o \
./bnet.o \
./bnetprotocol.o \
./bnlsclient.o \
./bnlsprotocol.o \
./commandpacket.o \
./config.o \
./crc32.o \
./csvparser.o \
./game.o \
./game_admin.o \
./game_base.o \
./gameplayer.o \
./gameprotocol.o \
./gameslot.o \
./ghost.o \
./ghostdb.o \
./ghostdbmysql.o \
./ghostdbsqlite.o \
./language.o \
./map.o \
./packed.o \
./replay.o \
./savegame.o \
./sha1.o \
./socket.o \
./sqlite3.o \
./stats.o \
./statsdota.o \
./statsw3mmd.o \
./util.o 

C_DEPS += \
./sqlite3.d 

CPP_DEPS += \
./bncsutilinterface.d \
./bnet.d \
./bnetprotocol.d \
./bnlsclient.d \
./bnlsprotocol.d \
./commandpacket.d \
./config.d \
./crc32.d \
./csvparser.d \
./game.d \
./game_admin.d \
./game_base.d \
./gameplayer.d \
./gameprotocol.d \
./gameslot.d \
./ghost.d \
./ghostdb.d \
./ghostdbmysql.d \
./ghostdbsqlite.d \
./language.d \
./map.d \
./packed.d \
./replay.d \
./savegame.d \
./sha1.d \
./socket.d \
./stats.d \
./statsdota.d \
./statsw3mmd.d \
./util.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


