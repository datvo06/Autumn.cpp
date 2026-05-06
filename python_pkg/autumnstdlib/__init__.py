"""Autumn standard library — bundled stdlib.sexp loaded as a string."""

from importlib.resources import read_text

autumnstdlib = read_text(__name__, "stdlib.sexp")

__all__ = ["autumnstdlib"]
