INCL = ./include
CFLAGS = `pkg-config --cflags opencv` -I$(INCL) #-D_RASPBERRY
LDFLAGS = -L/usr/local/lib
LIBS = `pkg-config --libs opencv`  -lpthread -lwiringPi -lwiringPiDev

#-lraspicam -lraspicam_cv

% : %.cpp
	g++ -c $(CFLAGS) $(LIBS)  $<

all: library principal

library: src/main src/FaceRecognition src/KNN src/UserManager src/MxtAppRecFacial src/LBP src/Features

principal: 
	g++  $(CFLAGS) $(LIBS) *.o -o facerec

clean:
	rm -rf *.o facerec
