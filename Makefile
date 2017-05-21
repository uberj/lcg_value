all:
	gcc lcg-state-forward.c -o lcg-state-forward
	gcc lcg-state-forward-orig-with-modification.c -o lcg-state-forward-orig-with-modification

test:
	./test.sh

clean:
	rm -f lcg-state-forward
	rm -f lcg-state-forward-orig-with-modification
