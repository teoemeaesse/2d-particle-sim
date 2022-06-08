make:
	g++ -g -c simulator.c -lm -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -DGL_GLEXT_PROTOTYPES
	g++ simulator.o -g -o simulator common.c concurrency.c particle.c compute.c -lm -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -DGL_GLEXT_PROTOTYPES

clean:
	rm *.o simulator