text-client: text-client.cpp
	g++ text-client.cpp -o $@

text-server: text-server.cpp
	g++ text-server.cpp -o $@

clean:
	rm *.o text-client text-server
	