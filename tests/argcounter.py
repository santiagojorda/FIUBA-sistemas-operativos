#!/usr/bin/env python3

import sys

def main():
    for index in range(1, len(sys.argv)):
        print(f'arg[{index}]: {sys.argv[index]}')

if __name__ == '__main__':
    main()