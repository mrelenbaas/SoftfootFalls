# SoftfootFalls

A town builder game.

## Valgrind

#### Check for memory leaks

valgrind --leak-check=full --show-leak-kinds=definite,indirect,possible --track-origins=yes ./softfoot-falls

## Git

#### Check for untracked files

git ls-files --others --exclude-standard

#### Track a file type with Git Large File System (LFS)

git lfs track "*.txt"