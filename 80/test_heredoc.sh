#!/bin/bash

# Test heredoc Ctrl+C durumu
echo "Testing heredoc Ctrl+C..."

# Heredoc başlat ve Ctrl+C simüle et
(echo "cat << eof"; sleep 0.1; echo -e "\x03") | ./minishell

# Exit status kontrol et
echo "Exit status after Ctrl+C: $?"

# Karşılaştırma için bash'te test
echo ""
echo "Testing with bash for comparison:"
(echo "cat << eof"; sleep 0.1; echo -e "\x03") | bash
echo "Bash exit status after Ctrl+C: $?"
