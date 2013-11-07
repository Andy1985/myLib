all: 
	g++ -Wall -g -o memIPOpt memIPOpt.cpp CMemcache.cpp CIPParse.cpp -lmemcached
	g++ -Wall -g -o memOption memOption.cpp CMemcache.cpp -lmemcached
	g++ -Wall -g -o FileToMem FileToMem.cpp CMemcache.cpp CIPParse.cpp -lmemcached
clean: 
	rm -f memOption
	rm -f memIPOpt
	rm -f FileToMem
