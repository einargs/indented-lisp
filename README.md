# Indented Lisp
Indented Lisp consists of a handwritten lexer and parser for a vaguely lisp-like
language that allows you to use indentation (two spaces) as well as parentheses
to indicate association.

This isn't intended to be any kind of actual language; I plan to cease working
on this once I finish writing the parser. If I get interested in working on a
lisp-like in the future, I may come back to this, but I doubt it.

Both the lexer and the parser are written in C, which I do not claim to be any
kind of expert on. Part of the motivation behind this project is to teach myself
more about C, though the primary thrust is just to have something to blow some
steam off with.

## The Lexer
The Lexer consists of a fairly simple state machine that reads characters in
from an input buffer (which I'm sure is implemented terribly).

I plan to experiment with something along the lines of a "meta-lexer" as a way
to seperate the contextual state needed to generate the open and close tokens
needed to allow for a context-free grammar in the parser.

In other words, the "meta-lexer" will wrap around the lexer. The parser will
make calls to this "meta-lexer" through a function that will transparently
handle both a "meta-lexer" wrapper around a lexer as well as a raw lexer.

The meta-lexer will maintain a pointer to the raw lexer (or another meta-lexer)
and two pieces of internal state: a single integer and a token buffer. The
integer willrepresent the level of indentation on the previous line. When the
meta-lexer is asked for a token it will pull a token from the raw lexer it is
wrapping.

If the meta-lexer pulls a newline token from the lexer it wraps, it will start
pulling tokens until it hits a non-indent token. Each time it pulls an indent
token the meta-lexer will increment a counter. Once it finally pulls a non-
indent token, the meta-lexer will compare the value of the counter to the
indentation level of the previous line and generate the appropriate amount
of scope tokens (the scope tokens, or whatever I call them, unify the
parentheses and indentation layers together at a lexical level).

These scope tokens will then be pushed onto a First-In Last-Out stack that is
serving as the aforementioned token buffer. Last, the non-indentation token
that was pulled from wrapped lexer is pushed onto that buffer. The meta-lexer
will then simply provide tokens from that buffer when asked for a token by the
parser (or what have you) until the buffer is empty.

If the meta-lexer encounters an EOF token in the process of this, then it will
discard the indentation-counting process and emit an EOF.
