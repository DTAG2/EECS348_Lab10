C := g++

main.exe: main.cpp
	$(C) main.cpp -o main.exe

clean:
	rm -f main.exe