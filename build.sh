#!/usr/bin/env bash
gcc -o rust.so rust.c $(yed --print-cflags) $(yed --print-ldflags)
