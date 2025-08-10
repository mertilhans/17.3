#!/bin/bash

echo "Manual test process:"
echo "1. Starting minishell in background"
echo "2. Sending commands"
echo "3. Sending SIGINT"
echo

# Start minishell with input
(
    echo "cat << eof"
    sleep 1
    echo "test line"
    sleep 1
    # Send Ctrl+C (ASCII 3)
    printf "\003"
    sleep 1
    echo "echo \$?"
    sleep 1
    echo "exit"
) | ./minishell

echo "Test completed."
