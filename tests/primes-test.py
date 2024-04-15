#!/usr/bin/env python3

import re
import sys

from os import getpid
from resource import prlimit, RLIMIT_NPROC, RLIMIT_NOFILE 
from subprocess import PIPE, run

from utils import VALGRIND_COMMAND, are_equal, format_result, run_command

TESTS = [
    {
        'description': 'correct primes up to 10',
        'number': 10,
        'valgrind_enabled': True
    },
    {
        'description': 'correct primes up to 100',
        'number': 100,
        'valgrind_enabled': False
    },
    {
        'description': 'correct primes up to 1000',
        'number': 1000,
        'valgrind_enabled': False
    },
    {
        'description': 'correct primes up to 10000',
        'number': 10000,
        'valgrind_enabled': False
    }
]

def exec_command(args, run_valgrind=False):
    # limits the number of open file descriptor
    # that the process or any of its child can have
    #
    # it has to be set to one more that the desired number
    prlimit(getpid(), RLIMIT_NOFILE, (10, 100))

    # limits the number of process that can be created
    # by a given process
    #
    # for the number 10000, a threshold of 3000 its OK
    #   (found it empirically -pic)
    prlimit(getpid(), RLIMIT_NPROC, (1900, 3000))

    output, valgrind_report, errors = run_command(args, run_valgrind=run_valgrind)

    if not run_valgrind and errors:
        raise Exception(errors)

    return set(filter(lambda l: l != '', output.split('\n'))), valgrind_report

def test_primes(binary_path, max_number, run_valgrind):
    output, valgrind_report = exec_command([binary_path, str(max_number)], run_valgrind)

    # - the `filter` removes lines not matching with the
    # pattern `primo %d`
    # - the `map` transforms the previous pattern leaving
    # just the number part `%d`
    return set(
        map(
            lambda s: int(s.split(' ')[1]),
            filter(
                lambda x: re.search(r'primo \d{1,4}', x, re.IGNORECASE),
                output
            )
        )
    ), valgrind_report

def generate_primes(number):
    # JOS code (grade-lab5) to calculate primes in a given range
    rest = range(2, number)
    while rest:
        yield rest[0]
        rest = [n for n in rest if n % rest[0]]

def run_test(binary_path, test_config, run_valgrind=False):
    description = test_config['description']
    number = test_config['number']
    valgrind_enabled = test_config['valgrind_enabled']

    expected_lines = set(generate_primes(number))

    resource_msg = None

    try:
        result_lines, valgrind_report = test_primes(binary_path, number, run_valgrind and valgrind_enabled)
        res = are_equal(expected_lines, result_lines)
    except Exception as e:
        resource_msg = f'Resource error - {e}'
        res = False

    print(f'  {description}: {format_result(res)}')

    if resource_msg is not None:
        print(resource_msg)
        return res

    if not res:
        diff_res = expected_lines ^ result_lines
        if not (expected_lines <= result_lines):
            # missing prime numbers in result
            assertion_msg = f"""
Prime numbers missing:
---------------------
{diff_res}
            """
        else:
            # not prime numbers in result
            assertion_msg = f"""
NOT prime numbers:
-----------------
{diff_res}
            """
        print(assertion_msg)

    if run_valgrind and valgrind_enabled:
        print(valgrind_report)

    return res

def execute_tests(binary_path, tests, run_valgrind=False):
    success = 0
    total = len(tests)

    for test_config in tests:
        res = run_test(binary_path, test_config, run_valgrind)
        if res:
            success += 1

    print(f'{success}/{total} passed')

def main(binary_path, run_valgrind):
    print('COMMAND: primes')

    execute_tests(binary_path, TESTS, run_valgrind)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: primes-test.py PRIMES_BIN_PATH [-v]')
        sys.exit(1)

    binary_path = sys.argv[1]
    run_valgrind = True if len(sys.argv) > 2 and sys.argv[2] == '-v' else False

    main(binary_path, run_valgrind)
