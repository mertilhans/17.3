#!/bin/bash

echo "Testing heredoc with automatic Ctrl+C simulation..."

# Test script that automatically sends SIGINT
{
    sleep 0.5
    pkill -SIGINT minishell
} &

echo "cat << eof" | timeout 3s ./minishell
echo "Minishell exit status: $?"
