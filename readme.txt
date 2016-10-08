This is an implementation of a simple shell using process forking and input/output redirection.

shell.c - main executable
test.c - unit tests
commands.c - base command-types
automaton.c - string matching primitives
match.c - string matching specifications
parse.c - simple command input
history.c - command history implementations
jobs.c - implementation of `jobs` and `fg`
builtin.c - implementations of builtin ocommands (cd, pwd, etc.)
