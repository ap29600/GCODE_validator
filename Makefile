default: test

test: bin/test
	./bin/test

bin/test: $(wildcard test/* src/* lib/*)
	gcc test/test.c -o bin/test -Wall -Wno-unused-result -Wextra --pedantic --std=c11 -O3 -ggdb
