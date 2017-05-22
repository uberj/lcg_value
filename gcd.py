import sys

big = int(sys.argv[1])
small = int(sys.argv[2])

def divide(big, small):
    whole_count = 0
    while True:
        whole_count += 1
        big = big - small
        if big < 0:
            return whole_count - 1, big + small

print divide(big, small)
