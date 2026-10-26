#!/bin/sh
# gcc：纯函数单测（-Wall -Wextra -Werror）
cd "$(dirname "$0")" && gcc -std=c99 -Wall -Wextra -Werror -I../../code/src test_target_alias.c ../../code/src/target_alias.c -o test_target_alias && ./test_target_alias
