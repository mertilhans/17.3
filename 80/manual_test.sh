#!/bin/bash

echo "Manual heredoc test"
echo "1. Run: ./minishell"
echo "2. Type: cat << eof" 
echo "3. Press Ctrl+C"
echo "4. Type: echo \$?"
echo "Expected result: 130"
echo ""
echo "Starting minishell now..."

exec ./minishell
