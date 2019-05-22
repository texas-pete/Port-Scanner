portScan: portScanner.o
	g++ -o portScan portScanner.o -std=c++11
	
portScanner.o: portScanner.cpp 
	g++ -c -g portScanner.cpp -std=c++11

clean:
	rm *.o portScan

