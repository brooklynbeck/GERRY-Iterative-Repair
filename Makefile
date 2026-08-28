#Usage:
#make		#compile all binary
#make clean	#remove all binaries and objects

.PHONY = all clean

all: 
	@echo "Creating objects..."
	gcc -c helpers.c
	gcc -c initializeDomain.c
	gcc -c initializeSchedule.c
	gcc -c iterativeRepair.c
	gcc -c main.c
	gcc -c manageCircularBuffer.c
	gcc -c manageLog.c
	gcc -c manageSchedule.c
	gcc -c manageTasks.c
	gcc -c montecarlosimulation.c
	@echo "Compiling all..."
	gcc -o gerry helpers.c initializeDomain.c initializeSchedule.c iterativeRepair.c main.c manageCircularBuffer.c manageLog.c manageSchedule.c manageTasks.c montecarlosimulation.c -lm

clean:
	@echo "Cleaning up..."
	rm *.o
	mkdir -p GerryBin
	mv *.c GerryBin
