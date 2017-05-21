all:
	gcc -g lcg-state-forward.c -o lcg-state-forward
	gcc -g lcg-state-forward-orig-with-modification.c -o lcg-state-forward-orig-with-modification
	gcc -g lcg-state-forward-orig.c -o lcg-state-forward-orig

test:
	./test.sh

clean:
	rm -f lcg-state-forward
	rm -f lcg-state-forward-orig-with-modification
	rm -f lcg-state-forward-orig
