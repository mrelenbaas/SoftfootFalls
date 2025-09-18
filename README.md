# SoftfootFalls

A town builder game.

## Valgrind

#### Check for memory leaks

valgrind --leak-check=full --track-origins=yes --suppressions=valgrind_suppressions.supp --gen-suppressions=all ./softfoot-falls

## Git

#### Track a file type with Git Large File System (LFS)

git lfs track "*.txt"
