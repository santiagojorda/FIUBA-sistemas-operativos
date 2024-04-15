from subprocess import PIPE, run

COLORS = {
    'default': "\033[0m",
    'red': "\033[31m",
    'green': "\033[32m"
}

VALGRIND_COMMAND = ['valgrind', '--track-fds=yes', '--leak-check=full', '--show-leak-kinds=all']

def color(text, color_name):
    return COLORS[color_name] + text + COLORS["default"]

def format_result(result):
    return color('OK', 'green') if result else color('FAIL', 'red')

def are_equal(expected, current):
    # ^ symmetric difference operator
    diff = expected ^ current

    return len(diff) == 0

def run_command(args, input=None, run_valgrind=False, cwd=None):
    if run_valgrind:
        args = VALGRIND_COMMAND + args

    proc = run(args, stdout=PIPE, stderr=PIPE, input=input, cwd=cwd, universal_newlines=True)

    if run_valgrind:
        valgrind_report = '  VALGRIND OUTPUT:\n' + '\t' + '\t'.join(map(lambda l: l + '\n', proc.stderr.split('\n')))
        errors = None
    else:
        valgrind_report = None
        errors = proc.stderr if proc.stderr != '' else None

    return proc.stdout, valgrind_report, errors