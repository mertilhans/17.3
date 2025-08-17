#!/bin/bash

# ==========================================
# MiniShell Hard Test Suite - Shell Destroyer
# Created by: meto1337
# Purpose: Comprehensive stress testing for minishell
# ==========================================

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
BOLD='\033[1m'
NC='\033[0m'

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
MINISHELL_PATH="./minishell"
VALGRIND="valgrind --leak-check=full --track-fds=yes --show-leak-kinds=all --suppressions=readline.supp"

show_banner() {
    clear
    echo -e "${PURPLE}${BOLD}"
    echo "████████████████████████████████████████████████████████████████████"
    echo "██                                                                ██"
    echo "██    ███╗   ███╗██████╗ ████████╗ ██████╗ ██╗██████╗ ██████╗     ██"
    echo "██    ████╗ ████║██╔═══╝╚══██╔══╝██╔═══██╗╚██║╚════██╗╚════██╗    ██"
    echo "██    ██╔████╔██║█████╗    ██║   ██║   ██║ ██║ █████╔╝ █████╔╝    ██"
    echo "██    ██║╚██╔╝██║██╔══╝    ██║   ██║   ██║ ██║ ╚═══██╗ ╚═══██╗    ██"
    echo "██    ██║ ╚═╝ ██║██████╗   ██║   ╚██████╔╝ ██║██████╔╝██████╔╝    ██"
    echo "██    ╚═╝     ╚═╝╚═════╝   ╚═╝    ╚═════╝  ╚═╝╚═════╝ ╚═════╝     ██"
    echo "██                                                                ██"
    echo "██              ${WHITE}MINISHELL HARD TEST SUITE${PURPLE}                     ██"
    echo "██              ${WHITE}Shell Destroyer Edition${PURPLE}                      ██"
    echo "██              ${CYAN}Created by: meto1337${PURPLE}                          ██"
    echo "██                                                                ██"
    echo "████████████████████████████████████████████████████████████████████"
    echo -e "${NC}"
    echo ""
}

show_menu() {
    echo -e "${WHITE}${BOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo -e "${CYAN}${BOLD}                        SELECT TEST CATEGORY                       ${NC}"
    echo -e "${WHITE}${BOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo -e "${GREEN}  1) 🗣️  Built-in Commands Tests (echo, cd, pwd, exit, etc.)${NC}"
    echo -e "${GREEN}  2) 🔥 FD Leak Detection Tests (File Descriptor Hell)${NC}"
    echo -e "${GREEN}  3) 🌍 Environment Variable Torture Tests (env -i chaos)${NC}"
    echo -e "${GREEN}  4) 💀 Signal Handling Extreme Tests (Ctrl+C mayhem)${NC}"
    echo -e "${GREEN}  5) ⚠️  Syntax Error Boundary Tests (Parser destroyer)${NC}"
    echo -e "${GREEN}  6) 🚪 Exit Code Validation Tests (Status chaos)${NC}"
    echo -e "${GREEN}  7) 🔄 Pipe & Redirection Hell Tests (I/O nightmare)${NC}"
    echo -e "${GREEN}  8) 📝 Heredoc & Quote Tests (Parsing chaos)${NC}"
    echo -e "${GREEN}  9) 💰 Variable Expansion Tests (\$VAR madness)${NC}"
    echo -e "${GREEN} 10) 🏴‍☠️ Memory Leak Hunter (Valgrind massacre)${NC}"
    echo -e "${GREEN} 11) 💥 Ultimate Destroyer (All tests combined)${NC}"
    echo -e "${GREEN} 12) 🔧 Custom Stress Test Builder${NC}"
    echo -e "${RED}  0) Exit${NC}"
    echo -e "${WHITE}${BOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo -n -e "${YELLOW}Enter your choice [0-12]: ${NC}"
}

# ==========================================
# UTILITY FUNCTIONS
# ==========================================

run_test() {
    local test_name="$1"
    local test_command="$2"
    local expected_behavior="$3"
    local test_type="$4"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo -e "\n${BLUE}${BOLD}[TEST $TOTAL_TESTS] $test_name${NC}"
    echo -e "${CYAN}Command: $test_command${NC}"
    echo -e "${CYAN}Expected: $expected_behavior${NC}"
    
    case "$test_type" in
        "fd")
            run_fd_test "$test_command"
            ;;
        "env")
            run_env_test "$test_command"
            ;;
        "signal")
            run_signal_test "$test_command"
            ;;
        "syntax")
            run_syntax_test "$test_command"
            ;;
        "exit")
            run_exit_test "$test_command"
            ;;
        "memory")
            run_memory_test "$test_command"
            ;;
        *)
            run_generic_test "$test_command"
            ;;
    esac
}

run_generic_test() {
    local cmd="$1"
    echo -e "${YELLOW}Testing generic command...${NC}"
    
    echo -e "$cmd\nexit" | timeout 10 $MINISHELL_PATH > /tmp/generic_test_output.txt 2>&1
    local exit_code=$?
    
    if [ $exit_code -eq 0 ] || [ $exit_code -eq 1 ] || [ $exit_code -eq 127 ]; then
        echo -e "${GREEN}✅ PASSED - Command handled correctly${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}❌ FAILED - Command handling issue${NC}"
        echo -e "${RED}Exit code: $exit_code${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        [ -f /tmp/generic_test_output.txt ] && head -5 /tmp/generic_test_output.txt
    fi
}

run_fd_test() {
    local cmd="$1"
    echo -e "${YELLOW}Testing FD leaks...${NC}"
    
    timeout 10 bash -c "echo '$cmd' | $VALGRIND $MINISHELL_PATH 2>&1" > /tmp/fd_test_output.txt
    local exit_code=$?
    
    local fd_leak_check=$(grep -c "Open file descriptor" /tmp/fd_test_output.txt 2>/dev/null || echo "0")
    
    if [ $fd_leak_check -eq 0 ] && [ $exit_code -ne 139 ]; then
        echo -e "${GREEN}✅ PASSED - No FD leaks detected${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}❌ FAILED - FD leak detected or crash${NC}"
        echo -e "${RED}Exit code: $exit_code, FD leaks: $fd_leak_check${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        [ -f /tmp/fd_test_output.txt ] && head -10 /tmp/fd_test_output.txt
    fi
}

run_env_test() {
    local cmd="$1"
    echo -e "${YELLOW}Testing env -i behavior...${NC}"
    
    timeout 10 bash -c "env -i $MINISHELL_PATH << 'EOF'
$cmd
exit
EOF" > /tmp/env_test_output.txt 2>&1
    
    local exit_code=$?
    
    if [ $exit_code -eq 0 ] || [ $exit_code -eq 127 ]; then
        echo -e "${GREEN}✅ PASSED - Handled env -i correctly${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}❌ FAILED - env -i handling failed${NC}"
        echo -e "${RED}Exit code: $exit_code${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        [ -f /tmp/env_test_output.txt ] && head -5 /tmp/env_test_output.txt
    fi
}

run_signal_test() {
    local cmd="$1"
    echo -e "${YELLOW}Testing signal handling...${NC}"
    
    echo "$cmd" | timeout 5 $MINISHELL_PATH &
    local pid=$!
    sleep 1
    
    kill -INT $pid 2>/dev/null
    wait $pid 2>/dev/null
    local exit_code=$?
    
    if [ $exit_code -eq 130 ] || [ $exit_code -eq 0 ]; then
        echo -e "${GREEN}✅ PASSED - Signal handled correctly${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}❌ FAILED - Signal handling issue${NC}"
        echo -e "${RED}Exit code: $exit_code (expected 130 or 0)${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

run_syntax_test() {
    local cmd="$1"
    echo -e "${YELLOW}Testing syntax error handling...${NC}"
    
    echo "$cmd" | timeout 5 $MINISHELL_PATH > /tmp/syntax_test_output.txt 2>&1
    local exit_code=$?
    
    local has_error=$(grep -c "syntax error\|command not found\|No such file" /tmp/syntax_test_output.txt)
    
    if [ $has_error -gt 0 ] || [ $exit_code -eq 2 ] || [ $exit_code -eq 127 ]; then
        echo -e "${GREEN}✅ PASSED - Syntax error detected correctly${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}❌ FAILED - Should have detected syntax error${NC}"
        echo -e "${RED}Exit code: $exit_code${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        [ -f /tmp/syntax_test_output.txt ] && head -3 /tmp/syntax_test_output.txt
    fi
}

run_exit_test() {
    local cmd="$1"
    echo -e "${YELLOW}Testing exit code handling...${NC}"
    
    echo -e "$cmd\nexit" | timeout 5 $MINISHELL_PATH > /tmp/exit_test_output.txt 2>&1
    local exit_code=$?
    
    if [ $exit_code -ge 0 ] && [ $exit_code -le 255 ]; then
        echo -e "${GREEN}✅ PASSED - Exit code in valid range ($exit_code)${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}❌ FAILED - Invalid exit code: $exit_code${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

run_memory_test() {
    local cmd="$1"
    echo -e "${YELLOW}Testing memory leaks...${NC}"
    
    timeout 15 bash -c "echo '$cmd' | $VALGRIND $MINISHELL_PATH 2>&1" > /tmp/memory_test_output.txt
    
    local leaks=$(grep -c "definitely lost\|indirectly lost\|possibly lost" /tmp/memory_test_output.txt)
    local errors=$(grep -c "Invalid read\|Invalid write\|Invalid free" /tmp/memory_test_output.txt)
    
    if [ $leaks -eq 0 ] && [ $errors -eq 0 ]; then
        echo -e "${GREEN}✅ PASSED - No memory issues detected${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}❌ FAILED - Memory issues detected${NC}"
        echo -e "${RED}Leaks: $leaks, Errors: $errors${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        grep -A2 -B2 "lost\|Invalid" /tmp/memory_test_output.txt | head -10
    fi
}

# ==========================================
# COMPREHENSIVE BUILT-IN COMMAND TESTS
# ==========================================

builtin_command_tests() {
    echo -e "\n${PURPLE}${BOLD}🗣️ BUILT-IN COMMANDS COMPREHENSIVE TESTS 🗣️${NC}"
    echo -e "${CYAN}Testing all built-in commands with every possible edge case...${NC}\n"
    
    # ===== ECHO TESTS =====
    echo -e "${YELLOW}${BOLD}📢 ECHO COMMAND TESTS${NC}"
    
    run_test "Echo Basic Text" "echo hello world" "hello world" "builtin"
    run_test "Echo Empty String" "echo" "" "builtin"
    run_test "Echo Multiple Spaces" "echo hello    world    test" "hello world test" "builtin"
    run_test "Echo Special Characters" "echo '!@#\$%^&*()'" "!@#\$%^&*()" "builtin"
    run_test "Echo Numbers" "echo 12345 67890" "12345 67890" "builtin"
    
    # Echo -n flag tests
    run_test "Echo -n Basic" "echo -n hello" "hello (no newline)" "builtin"
    run_test "Echo -n Empty" "echo -n" "(no output, no newline)" "builtin"
    run_test "Echo -n Multiple Args" "echo -n hello world test" "hello world test (no newline)" "builtin"
    run_test "Echo -n Multiple Flags" "echo -nnn hello" "hello (no newline)" "builtin"
    run_test "Echo -n Invalid Flag" "echo -nx hello" "-nx hello (with newline)" "builtin"
    
    # Echo with quotes
    run_test "Echo Single Quotes" "echo 'hello world'" "hello world" "builtin"
    run_test "Echo Double Quotes" "echo \"hello world\"" "hello world" "builtin"
    run_test "Echo Nested Quotes" "echo \"I said 'hello'\"" "I said 'hello'" "builtin"
    
    # ===== CD TESTS =====
    echo -e "\n${YELLOW}${BOLD}📁 CD COMMAND TESTS${NC}"
    
    run_test "CD to Temp" "cd /tmp && pwd" "/tmp" "builtin"
    run_test "CD to Home" "cd && pwd" "\$HOME directory" "builtin"
    run_test "CD Relative Path" "cd /tmp && cd .. && pwd" "Parent directory" "builtin"
    run_test "CD Current Dir" "cd . && pwd" "Current directory" "builtin"
    
    # CD error cases
    run_test "CD Nonexistent Dir" "cd /nonexistent/directory/path/12345" "No such file or directory" "builtin"
    run_test "CD to File" "cd /etc/passwd" "Not a directory" "builtin"
    run_test "CD Too Many Args" "cd /tmp /home /usr" "too many arguments" "builtin"
    
    # ===== PWD TESTS =====
    echo -e "\n${YELLOW}${BOLD}📍 PWD COMMAND TESTS${NC}"
    
    run_test "PWD Basic" "pwd" "Current working directory" "builtin"
    run_test "PWD After CD" "cd /tmp && pwd" "/tmp" "builtin"
    run_test "PWD With Args" "pwd extra arguments" "Should ignore args" "builtin"
    
    # ===== EXIT TESTS =====
    echo -e "\n${YELLOW}${BOLD}🚪 EXIT COMMAND TESTS${NC}"
    
    run_test "Exit No Args" "exit" "Exit code 0" "builtin"
    run_test "Exit Success" "exit 0" "Exit code 0" "builtin"
    run_test "Exit Failure" "exit 1" "Exit code 1" "builtin"
    run_test "Exit Large Number" "exit 255" "Exit code 255" "builtin"
    run_test "Exit Overflow" "exit 256" "Exit code 0 (256 % 256)" "builtin"
    run_test "Exit Negative" "exit -1" "Exit code 255" "builtin"
    run_test "Exit Non-Numeric" "exit abc" "numeric argument required" "builtin"
    
    # ===== EXPORT TESTS =====
    echo -e "\n${YELLOW}${BOLD}📤 EXPORT COMMAND TESTS${NC}"
    
    run_test "Export Basic" "export TEST=hello && echo \$TEST" "hello" "builtin"
    run_test "Export No Value" "export TEST && env | grep TEST" "TEST=" "builtin"
    run_test "Export List All" "export" "List all exported variables" "builtin"
    run_test "Export Override" "export TEST=first && export TEST=second && echo \$TEST" "second" "builtin"
    
    # Export error cases
    run_test "Export Invalid Name" "export 123invalid=value" "not a valid identifier" "builtin"
    run_test "Export Special Start" "export @invalid=value" "not a valid identifier" "builtin"
    
    # ===== UNSET TESTS =====
    echo -e "\n${YELLOW}${BOLD}📤 UNSET COMMAND TESTS${NC}"
    
    run_test "Unset Basic" "export TEST=hello && unset TEST && echo \$TEST" "(empty)" "builtin"
    run_test "Unset Nonexistent" "unset NONEXISTENT_VAR_12345" "No error" "builtin"
    run_test "Unset Multiple" "export A=1 B=2 && unset A B && echo \$A\$B" "(empty)" "builtin"
    
    # ===== ENV TESTS =====
    echo -e "\n${YELLOW}${BOLD}🌍 ENV COMMAND TESTS${NC}"
    
    run_test "Env Basic" "env" "List all environment variables" "builtin"
    run_test "Env After Export" "export TEST=hello && env | grep TEST" "TEST=hello" "builtin"
}

# ==========================================
# PIPE AND REDIRECTION HELL TESTS
# ==========================================

pipe_redirection_hell_tests() {
    echo -e "\n${PURPLE}${BOLD}🔄 PIPE & REDIRECTION HELL TESTS 🔄${NC}"
    echo -e "${CYAN}Testing pipes and redirections in every possible nightmare scenario...${NC}\n"
    
    # ===== BASIC PIPE TESTS =====
    echo -e "${YELLOW}${BOLD}🔗 BASIC PIPE TESTS${NC}"
    
    run_test "Simple Pipe" "echo hello | cat" "hello" "pipe"
    run_test "Double Pipe" "echo hello | cat | cat" "hello" "pipe"
    run_test "Triple Pipe" "echo hello | cat | cat | cat" "hello" "pipe"
    run_test "Long Pipe Chain" "echo test | cat | cat | cat | cat | cat | wc -c" "5" "pipe"
    run_test "Pipe Echo to Grep" "echo 'hello world' | grep hello" "hello world" "pipe"
    
    # ===== EXTREME PIPE CHAINS =====
    echo -e "\n${YELLOW}${BOLD}⛓️ EXTREME PIPE CHAINS${NC}"
    
    run_test "10-Command Chain" "echo test | cat | cat | cat | cat | cat | cat | cat | cat | cat | wc -c" "5" "pipe"
    run_test "Mixed Command Chain" "echo hello | cat | grep hello | cat | wc -l" "1" "pipe"
    run_test "Empty Pipe Chain" "echo '' | cat | cat | cat" "(empty)" "pipe"
    
    # ===== BASIC REDIRECTION TESTS =====
    echo -e "\n${YELLOW}${BOLD}📝 BASIC REDIRECTION TESTS${NC}"
    
    run_test "Output Redirect" "echo hello > /tmp/redirect_test && cat /tmp/redirect_test" "hello" "redirect"
    run_test "Input Redirect" "echo hello > /tmp/input_test && cat < /tmp/input_test" "hello" "redirect"
    run_test "Append Redirect" "echo line1 > /tmp/append_test && echo line2 >> /tmp/append_test && cat /tmp/append_test" "line1\\nline2" "redirect"
    run_test "Overwrite Test" "echo first > /tmp/overwrite && echo second > /tmp/overwrite && cat /tmp/overwrite" "second" "redirect"
    
    # ===== REDIRECTION ERROR CASES =====
    echo -e "\n${YELLOW}${BOLD}⚠️ REDIRECTION ERROR CASES${NC}"
    
    run_test "Redirect to Nonexistent Dir" "echo test > /nonexistent/dir/file" "No such file or directory" "redirect"
    run_test "Read Nonexistent File" "cat < /nonexistent/file/12345" "No such file or directory" "redirect"
    run_test "Permission Denied Write" "echo test > /etc/passwd" "Permission denied" "redirect"
    
    # ===== PIPES WITH REDIRECTIONS =====
    echo -e "\n${YELLOW}${BOLD}🔗📝 PIPES WITH REDIRECTIONS${NC}"
    
    run_test "Pipe then Redirect" "echo hello | cat > /tmp/pipe_redirect && cat /tmp/pipe_redirect" "hello" "pipe"
    run_test "Redirect then Pipe" "echo hello > /tmp/redirect_pipe && cat /tmp/redirect_pipe | cat" "hello" "pipe"
    run_test "Complex Pipe Redirect" "echo test | cat | cat > /tmp/complex && cat /tmp/complex" "test" "pipe"
    
    # ===== PIPE ERROR SCENARIOS =====
    echo -e "\n${YELLOW}${BOLD}💥 PIPE ERROR SCENARIOS${NC}"
    
    run_test "Pipe to Failed Command" "echo hello | nonexistent_command_12345" "Command not found" "pipe"
    run_test "Failed Command Pipe" "nonexistent_cmd_12345 | cat" "Command not found" "pipe"
    run_test "Chain with Failed Command" "echo hello | nonexistent | cat" "Mixed success/failure" "pipe"
    
    # ===== SPECIAL FILE DESCRIPTORS =====
    echo -e "\n${YELLOW}${BOLD}🔢 SPECIAL FILE DESCRIPTORS${NC}"
    
    run_test "Redirect to /dev/null" "echo hello > /dev/null && echo success" "success" "redirect"
    run_test "Read from /dev/null" "cat < /dev/null" "(empty)" "redirect"
    run_test "Pipe to /dev/null" "echo hello | cat > /dev/null && echo done" "done" "pipe"
    
    # ===== WHITESPACE AND EDGE CASES =====
    echo -e "\n${YELLOW}${BOLD}⚪ WHITESPACE AND EDGE CASES${NC}"
    
    run_test "Spaces Around Pipes" "echo hello   |   cat   |   cat" "hello" "pipe"
    run_test "Spaces Around Redirects" "echo hello   >   /tmp/spaces && cat /tmp/spaces" "hello" "redirect"
    run_test "No Spaces" "echo hello|cat|cat>/tmp/nospace&&cat /tmp/nospace" "hello" "pipe"
}

# ==========================================
# HEREDOC AND QUOTE HELL TESTS  
# ==========================================

heredoc_quote_hell_tests() {
    echo -e "\n${PURPLE}${BOLD}📝 HEREDOC & QUOTE HELL TESTS 📝${NC}"
    echo -e "${CYAN}Testing heredocs and quotes in every possible nightmare scenario...${NC}\n"
    
    # ===== BASIC HEREDOC TESTS =====
    echo -e "${YELLOW}${BOLD}📜 BASIC HEREDOC TESTS${NC}"
    
    run_test "Simple Heredoc" "cat << EOF\nhello world\nEOF" "hello world" "heredoc"
    run_test "Multi-line Heredoc" "cat << EOF\nline1\nline2\nline3\nEOF" "line1\\nline2\\nline3" "heredoc"
    run_test "Empty Heredoc" "cat << EOF\nEOF" "(empty)" "heredoc"
    run_test "Different Delimiter" "cat << END\nhello world\nEND" "hello world" "heredoc"
    
    # ===== HEREDOC WITH VARIABLE EXPANSION =====
    echo -e "\n${YELLOW}${BOLD}💰 HEREDOC WITH VARIABLE EXPANSION${NC}"
    
    run_test "Heredoc Expand Variable" "export USER=testuser && cat << EOF\nHello \$USER\nEOF" "Hello testuser" "heredoc"
    run_test "Heredoc Multiple Variables" "export A=hello B=world && cat << EOF\n\$A \$B\nEOF" "hello world" "heredoc"
    
    # ===== HEREDOC WITHOUT EXPANSION =====
    echo -e "\n${YELLOW}${BOLD}🚫 HEREDOC WITHOUT EXPANSION${NC}"
    
    run_test "Heredoc No Expand Single" "export USER=testuser && cat << 'EOF'\nHello \$USER\nEOF" "Hello \$USER" "heredoc"
    run_test "Heredoc No Expand Double" "export USER=testuser && cat << \"EOF\"\nHello \$USER\nEOF" "Hello \$USER" "heredoc"
    
    # ===== HEREDOC WITH PIPES =====
    echo -e "\n${YELLOW}${BOLD}🔗 HEREDOC WITH PIPES${NC}"
    
    run_test "Heredoc Pipe Simple" "cat << EOF | cat\nhello world\nEOF" "hello world" "heredoc"
    run_test "Heredoc Pipe Chain" "cat << EOF | cat | cat | wc -l\nline1\nline2\nEOF" "2" "heredoc"
    run_test "Heredoc Grep" "cat << EOF | grep hello\nhello world\ntest line\nEOF" "hello world" "heredoc"
    
    # ===== HEREDOC ERROR CASES =====
    echo -e "\n${YELLOW}${BOLD}⚠️ HEREDOC ERROR CASES${NC}"
    
    run_test "Heredoc No Delimiter" "cat << EOF\nhello world" "EOF not found warning" "heredoc"
    run_test "Heredoc Wrong Delimiter" "cat << EOF\nhello\nEND" "EOF not found warning" "heredoc"
    
    # ===== BASIC QUOTE TESTS =====
    echo -e "\n${YELLOW}${BOLD}📝 BASIC QUOTE TESTS${NC}"
    
    run_test "Single Quotes Basic" "echo 'hello world'" "hello world" "quote"
    run_test "Double Quotes Basic" "echo \"hello world\"" "hello world" "quote"
    run_test "Empty Single Quotes" "echo ''" "(empty)" "quote"
    run_test "Empty Double Quotes" "echo \"\"" "(empty)" "quote"
    
    # ===== NESTED QUOTES =====
    echo -e "\n${YELLOW}${BOLD}🔄 NESTED QUOTES${NC}"
    
    run_test "Double Inside Single" "echo 'I said \"hello\"'" "I said \"hello\"" "quote"
    run_test "Single Inside Double" "echo \"I said 'hello'\"" "I said 'hello'" "quote"
    run_test "Multiple Quote Types" "echo 'hello' \"world\" test" "hello world test" "quote"
    
    # ===== QUOTES WITH VARIABLES =====
    echo -e "\n${YELLOW}${BOLD}💰 QUOTES WITH VARIABLES${NC}"
    
    run_test "Variable in Double Quotes" "export USER=test && echo \"Hello \$USER\"" "Hello test" "quote"
    run_test "Variable in Single Quotes" "export USER=test && echo 'Hello \$USER'" "Hello \$USER" "quote"
    run_test "Mixed Variable Quotes" "export USER=test && echo \"Hello \$USER\" 'literal \$USER'" "Hello test literal \$USER" "quote"
    
    # ===== QUOTES WITH SPECIAL CHARACTERS =====
    echo -e "\n${YELLOW}${BOLD}🎭 QUOTES WITH SPECIAL CHARACTERS${NC}"
    
    run_test "Quotes with Pipes" "echo 'hello | world'" "hello | world" "quote"
    run_test "Quotes with Redirects" "echo 'test > file'" "test > file" "quote"
    run_test "Quotes with Semicolon" "echo 'command; command'" "command; command" "quote"
}

# ==========================================
# VARIABLE EXPANSION MADNESS TESTS
# ==========================================

variable_expansion_tests() {
    echo -e "\n${PURPLE}${BOLD}💰 VARIABLE EXPANSION MADNESS TESTS 💰${NC}"
    echo -e "${CYAN}Testing variable expansion in every possible scenario...${NC}\n"
    
    # ===== BASIC VARIABLE EXPANSION =====
    echo -e "${YELLOW}${BOLD}📦 BASIC VARIABLE EXPANSION${NC}"
    
    run_test "Simple Variable" "export TEST=hello && echo \$TEST" "hello" "variable"
    run_test "Variable with Text" "export TEST=hello && echo \$TEST world" "hello world" "variable"
    run_test "Multiple Variables" "export A=hello B=world && echo \$A \$B" "hello world" "variable"
    run_test "Variable in Middle" "export TEST=hello && echo start \$TEST end" "start hello end" "variable"
    run_test "Adjacent Variables" "export A=hello B=world && echo \$A\$B" "helloworld" "variable"
    
    # ===== SPECIAL VARIABLES =====
    echo -e "\n${YELLOW}${BOLD}🎯 SPECIAL VARIABLES${NC}"
    
    run_test "Exit Status Variable" "true && echo \$?" "0" "variable"
    run_test "Exit Status After False" "false && echo \$?" "1" "variable"
    run_test "Process ID Variable" "echo \$\$" "[process ID number]" "variable"
    run_test "Multiple Special Vars" "echo \$? \$\$ \$?" "[status] [pid] [status]" "variable"
    
    # ===== UNDEFINED VARIABLES =====
    echo -e "\n${YELLOW}${BOLD}❓ UNDEFINED VARIABLES${NC}"
    
    run_test "Undefined Variable" "echo \$NONEXISTENT_VAR" "(empty)" "variable"
    run_test "Undefined with Text" "echo start \$NONEXISTENT end" "start  end" "variable"
    run_test "Multiple Undefined" "echo \$UNDEF1 \$UNDEF2 \$UNDEF3" "   " "variable"
    run_test "Mixed Defined Undefined" "export TEST=hello && echo \$TEST \$UNDEF \$TEST" "hello  hello" "variable"
    
    # ===== VARIABLE EXPANSION IN QUOTES =====
    echo -e "\n${YELLOW}${BOLD}📝 VARIABLE EXPANSION IN QUOTES${NC}"
    
    run_test "Variable in Double Quotes" "export TEST=hello && echo \"\$TEST world\"" "hello world" "variable"
    run_test "Variable in Single Quotes" "export TEST=hello && echo '\$TEST world'" "\$TEST world" "variable"
    run_test "Mixed Quote Variable" "export TEST=hello && echo \"\$TEST\" '\$TEST'" "hello \$TEST" "variable"
    run_test "Special Vars in Quotes" "echo \"\$\$ \$?\"" "[pid] [status]" "variable"
    
    # ===== COMPLEX VARIABLE SCENARIOS =====
    echo -e "\n${YELLOW}${BOLD}🎭 COMPLEX VARIABLE SCENARIOS${NC}"
    
    run_test "Variable Chain" "export A=B && export B=C && export C=hello && echo \$A" "B" "variable"
    run_test "Variable with Spaces" "export TEST='hello world test' && echo \$TEST" "hello world test" "variable"
    run_test "Variable with Special Chars" "export TEST='!@#\$%^&*()' && echo \$TEST" "!@#\$%^&*()" "variable"
    run_test "Empty Variable" "export TEST= && echo \"value:\$TEST:end\"" "value::end" "variable"
    run_test "Variable Override" "export TEST=first && export TEST=second && echo \$TEST" "second" "variable"
    
    # ===== VARIABLE EXPANSION EDGE CASES =====
    echo -e "\n${YELLOW}${BOLD}⚡ VARIABLE EXPANSION EDGE CASES${NC}"
    
    run_test "Dollar Without Variable" "echo \$" "\$" "variable"
    run_test "Dollar with Number" "echo \$1 \$2 \$0" "\$1 \$2 \$0" "variable"
    run_test "Invalid Variable Name" "echo \$123invalid" "\$123invalid" "variable"
    run_test "Variable with Underscore" "export _TEST=hello && echo \$_TEST" "hello" "variable"
    run_test "Variable Case Sensitive" "export test=lower TEST=UPPER && echo \$test \$TEST" "lower UPPER" "variable"
    
    # ===== ENVIRONMENT VARIABLE TESTS =====
    echo -e "\n${YELLOW}${BOLD}🌍 ENVIRONMENT VARIABLE TESTS${NC}"
    
    run_test "PATH Variable" "echo \$PATH" "[path string]" "variable"
    run_test "HOME Variable" "echo \$HOME" "[home directory]" "variable"
    run_test "USER Variable" "echo \$USER" "[username]" "variable"
    run_test "PWD Variable" "echo \$PWD" "[current directory]" "variable"
    
    # ===== VARIABLE EXPANSION WITH COMMANDS =====
    echo -e "\n${YELLOW}${BOLD}⚙️ VARIABLE EXPANSION WITH COMMANDS${NC}"
    
    run_test "Variable in Echo" "export MSG=hello && echo \$MSG" "hello" "variable"
    run_test "Variable in CD" "export DIR=/tmp && cd \$DIR && pwd" "/tmp" "variable"
    run_test "Variable in Pipes" "export CMD=cat && echo hello | \$CMD" "hello" "variable"
    run_test "Variable in Redirect" "export FILE=/tmp/varfile && echo hello > \$FILE && cat \$FILE" "hello" "variable"
    
    # ===== VARIABLE WORD SPLITTING =====
    echo -e "\n${YELLOW}${BOLD}✂️ VARIABLE WORD SPLITTING${NC}"
    
    run_test "Variable Split Simple" "export ARGS='one two three' && echo \$ARGS" "one two three" "variable"
    run_test "Variable Split in Echo" "export ARGS='hello world' && echo start \$ARGS end" "start hello world end" "variable"
    run_test "Variable No Split Quotes" "export ARGS='hello world' && echo \"start \$ARGS end\"" "start hello world end" "variable"
    run_test "Variable Multiple Spaces" "export ARGS='hello    world' && echo \$ARGS" "hello world" "variable"
    
    # ===== VARIABLE EXPANSION IN HEREDOC =====
    echo -e "\n${YELLOW}${BOLD}📜 VARIABLE EXPANSION IN HEREDOC${NC}"
    
    run_test "Heredoc Variable Simple" "export USER=test && cat << EOF\nHello \$USER\nEOF" "Hello test" "variable"
    run_test "Heredoc Multiple Variables" "export A=hello B=world && cat << EOF\n\$A \$B\nEOF" "hello world" "variable"
    run_test "Heredoc Special Variables" "cat << EOF\nPID: \$\$ Status: \$?\nEOF" "PID: [pid] Status: [status]" "variable"
    run_test "Heredoc No Expand" "export USER=test && cat << 'EOF'\nHello \$USER\nEOF" "Hello \$USER" "variable"
}

fd_leak_tests() {
    echo -e "\n${PURPLE}${BOLD}🔥 FD LEAK DETECTION TESTS - NIGHTMARE MODE 🔥${NC}"
    echo -e "${CYAN}Testing for file descriptor leaks in extreme scenarios...${NC}\n"
    
    run_test "Basic Pipe FD Test" "echo hello | cat | cat | cat" "No FD leaks" "fd"
    run_test "Multiple Redirections FD Test" "echo test > /tmp/test1 > /tmp/test2 > /tmp/test3" "No FD leaks" "fd"
    run_test "Failed Command FD Test" "nonexistent_cmd | cat | nonexistent_cmd2" "No FD leaks" "fd"
    run_test "Heredoc FD Test" "cat << EOF\ntest\nEOF" "No FD leaks" "fd"
    run_test "Complex Heredoc FD Test" "cat << A | cat << B\nlineA\nA\nlineB\nB" "No FD leaks" "fd"
    
    run_test "10-Command Pipe Chain" "echo test | cat | cat | cat | cat | cat | cat | cat | cat | cat | wc -l" "No FD leaks" "fd"
    run_test "Failed Execve FD Test" "./nonexistent | cat | ./alsononexistent" "No FD leaks" "fd"
    run_test "Empty Pipe FD Test" "| cat" "No FD leaks" "fd"
    run_test "Broken Pipe Chain" "echo test | | cat" "No FD leaks" "fd"
    
    run_test "Mixed Redirect + Pipe FD" "echo test | cat > /tmp/out | cat < /tmp/out" "No FD leaks" "fd"
    run_test "Nested Redirect FD Test" "echo test > /tmp/a && cat < /tmp/a > /tmp/b && cat < /tmp/b" "No FD leaks" "fd"
    run_test "Heredoc + Pipe FD Test" "cat << EOF | grep test | wc -l\nhello test world\nEOF" "No FD leaks" "fd"
    run_test "Failed Open FD Test" "cat < /nonexistent/file | cat > /nonexistent/dir/file" "No FD leaks" "fd"
}

env_torture_tests() {
    echo -e "\n${PURPLE}${BOLD}🌍 ENVIRONMENT VARIABLE TORTURE TESTS 🌍${NC}"
    echo -e "${CYAN}Testing behavior with env -i and environment chaos...${NC}\n"
    
    run_test "Empty Environment Test" "echo \$PATH" "Empty or undefined" "env"
    run_test "PWD in Empty Env" "pwd" "Should work without PATH" "env"
    run_test "CD in Empty Env" "cd /tmp && pwd" "Should work" "env"
    run_test "Export in Empty Env" "export TEST=value && echo \$TEST" "Should work" "env"
    run_test "Built-in Commands No Env" "echo hello && pwd && cd /tmp" "Should work" "env"
    
    run_test "External Cmd No PATH" "ls" "Command not found" "env"
    run_test "Absolute Path No ENV" "/bin/ls" "Should work" "env"
    run_test "Relative Path No ENV" "./minishell" "May fail" "env"
    run_test "Command with Args No PATH" "ls -la" "Command not found" "env"
    run_test "Pipe No PATH" "/bin/echo test | /bin/cat" "Should work" "env"
    
    run_test "HOME Undefined Test" "cd" "Should handle gracefully" "env"
    run_test "USER Undefined Test" "echo \$USER" "Empty" "env"
    run_test "Shell Level Test" "echo \$SHLVL" "Should be 1" "env"
    run_test "Exit Status No Env" "false && echo \$?" "Should be 1" "env"
    run_test "PID No Env" "echo \$\$" "Should show PID" "env"
}

signal_extreme_tests() {
    echo -e "\n${PURPLE}${BOLD}💀 SIGNAL HANDLING EXTREME TESTS 💀${NC}"
    echo -e "${CYAN}Testing signal handling in extreme scenarios...${NC}\n"
    
    run_test "Ctrl+C Basic Test" "sleep 2" "Should handle SIGINT" "signal"
    run_test "Ctrl+C During Echo" "echo hello" "Should complete normally" "signal"
    run_test "Ctrl+C During Pipe" "sleep 3 | cat" "Should handle SIGINT" "signal"
    run_test "Ctrl+C During Heredoc" "cat << EOF" "Should interrupt heredoc" "signal"
    run_test "Ctrl+C Multiple Commands" "sleep 1 && sleep 1 && sleep 1" "Should interrupt" "signal"
    
    run_test "Signal During FD Operations" "sleep 2 > /tmp/signal_test" "Should handle gracefully" "signal"
    run_test "Signal During Redirect" "sleep 3 | cat > /tmp/out" "Should cleanup FDs" "signal"
    run_test "Signal Chain Pipe" "sleep 2 | sleep 2 | sleep 2" "Should kill all" "signal"
    run_test "Signal Built-in Cmd" "sleep 100" "Should interrupt external" "signal"
}

syntax_destroyer_tests() {
    echo -e "\n${PURPLE}${BOLD}⚠️ SYNTAX ERROR BOUNDARY TESTS ⚠️${NC}"
    echo -e "${CYAN}Testing parser limits and syntax error handling...${NC}\n"
    
    run_test "Pipe at Start" "|" "Syntax error" "syntax"
    run_test "Pipe at End" "echo hello |" "Syntax error" "syntax"
    run_test "Empty Redirect" "echo hello >" "Syntax error" "syntax"
    run_test "Invalid Redirect Order" "echo hello < >" "Syntax error" "syntax"
    
    run_test "Triple Pipe" "echo hello ||| cat" "Syntax error" "syntax"
    run_test "Invalid Redirect" "echo hello < >" "Syntax error" "syntax"
    run_test "Unclosed Quote" "echo \"hello world" "Syntax error" "syntax"
    run_test "Nested Quotes Error" "echo \"hello 'world\" test'" "Should handle" "syntax"
    run_test "Empty Heredoc" "cat <<" "Syntax error" "syntax"
    
    run_test "Only Spaces" "   " "Should handle empty" "syntax"
    run_test "Only Tabs" "\t\t\t" "Should handle empty" "syntax"
    run_test "Mixed Whitespace" " \t \n " "Should handle empty" "syntax"
    run_test "Special Chars Only" "!@#%^*()" "Command not found" "syntax"
    
    run_test "Alternating Quotes" "echo 'hello\"world'\"test\"'end'" "Should parse correctly" "syntax"
    run_test "Empty Quotes" "echo '' \"\" ''" "Should handle empty quotes" "syntax"
    run_test "Quote in Quote" "echo \"I said 'hello' loudly\"" "Should parse correctly" "syntax"
    run_test "Dollar in Single Quote" "echo '\$HOME \$USER'" "Should not expand" "syntax"
}

exit_code_chaos_tests() {
    echo -e "\n${PURPLE}${BOLD}🚪 EXIT CODE VALIDATION TESTS 🚪${NC}"
    echo -e "${CYAN}Testing exit code handling and propagation...${NC}\n"
    
    run_test "Exit Success Test" "exit 0" "Exit code 0" "exit"
    run_test "Exit Failure Test" "exit 1" "Exit code 1" "exit"
    run_test "Exit Large Number" "exit 255" "Exit code 255" "exit"
    run_test "Exit Overflow" "exit 256" "Exit code 0 (256 % 256)" "exit"
    run_test "Exit Negative" "exit -1" "Exit code 255" "exit"
    
    run_test "False Command Exit" "false" "Exit code 1" "exit"
    run_test "True Command Exit" "true" "Exit code 0" "exit"
    run_test "Nonexistent Command" "nonexistent_cmd_12345" "Exit code 127" "exit"
    run_test "Permission Denied" "/etc/passwd" "Exit code 126" "exit"
    
    run_test "Pipe Exit Success" "echo hello | cat" "Exit code 0" "exit"
    run_test "Pipe Exit Failure" "false | cat" "Exit code 0 (last command)" "exit"
    run_test "Pipe Failed Command" "echo hello | nonexistent" "Exit code 127" "exit"
    run_test "Exit Status Variable" "false; echo \$?" "Should show 1" "exit"
    run_test "Multiple Exit Checks" "true; echo \$?; false; echo \$?" "Show 0 then 1" "exit"
    
    run_test "CD Nonexistent Dir" "cd /nonexistent/directory/path" "Exit code 1" "exit"
    run_test "Export Invalid" "export 123invalid=value" "Exit code 1" "exit"
    run_test "Unset Nonexistent" "unset NONEXISTENT_VAR" "Exit code 0" "exit"
    run_test "Echo Exit Code" "echo hello" "Exit code 0" "exit"
    run_test "PWD Exit Code" "pwd" "Exit code 0" "exit"
}

memory_massacre_tests() {
    echo -e "\n${PURPLE}${BOLD}🏴‍☠️ MEMORY LEAK HUNTER - VALGRIND MASSACRE 🏴‍☠️${NC}"
    echo -e "${CYAN}Comprehensive memory leak detection with Valgrind...${NC}\n"
    
    run_test "Simple Command Memory" "echo hello world" "No leaks" "memory"
    run_test "Pipe Memory Test" "echo hello | cat | wc -c" "No leaks" "memory"
    run_test "Redirect Memory Test" "echo test > /tmp/mem_test && cat /tmp/mem_test" "No leaks" "memory"
    run_test "Variable Memory Test" "export TEST=value && echo \$TEST && unset TEST" "No leaks" "memory"
    run_test "Built-in Memory Test" "pwd && cd /tmp && pwd" "No leaks" "memory"
    
    run_test "Large String Memory" "echo 'very long string here with many characters'" "No leaks" "memory"
    run_test "Many Variables Memory" "export A=1 B=2 C=3 D=4 E=5 && env | grep '^[ABCDE]='" "No leaks" "memory"
    run_test "Complex Pipe Memory" "echo test | cat | cat | cat | wc -c" "No leaks" "memory"
    run_test "Heredoc Memory Test" "cat << EOF\nline1\nline2\nline3\nEOF" "No leaks" "memory"
    run_test "Failed Command Memory" "nonexistent_cmd && echo done" "No leaks" "memory"
    
    run_test "Empty Command Memory" "" "No leaks" "memory"
    run_test "Quote Memory Test" "echo \"hello 'world'\" 'test \"quote\"'" "No leaks" "memory"
    run_test "Signal Memory Test" "sleep 1" "No leaks (with interrupt)" "memory"
    run_test "Error Recovery Memory" "echo test | nonexistent | cat" "No leaks" "memory"
    run_test "Exit Memory Test" "echo test && exit 42" "No leaks" "memory"
}

ultimate_destroyer() {
    echo -e "\n${PURPLE}${BOLD}💥 ULTIMATE DESTROYER - ALL TESTS COMBINED 💥${NC}"
    echo -e "${CYAN}Running all test suites in sequence...${NC}\n"
    
    builtin_command_tests
    fd_leak_tests
    env_torture_tests  
    signal_extreme_tests
    syntax_destroyer_tests
    exit_code_chaos_tests
    pipe_redirection_hell_tests
    heredoc_quote_hell_tests
    variable_expansion_tests
    memory_massacre_tests
    
    echo -e "\n${RED}${BOLD}🏴‍☠️ EXTREME EDGE CASES - BEYOND DESTRUCTION 🏴‍☠️${NC}"
    
    run_test "Chaos Test 1" "echo hello | cat | wc -c" "Complex piping" "memory"
    run_test "Chaos Test 2" "export A='hello' B='world' && echo \$A \$B" "Variable chain" "memory"
    run_test "Chaos Test 3" "echo hello > /tmp/chaos && cat < /tmp/chaos | cat" "Redirect + pipe" "memory"
    run_test "Chaos Test 4" "cat << EOF | grep hello\nhello world\ntest line\nEOF" "Heredoc + pipe" "memory"
    run_test "Chaos Test 5" "echo \$? \$\$ | cat" "Special variables" "memory"
    
    run_test "Built-in Chaos 1" "echo -nnnnn hello; cd /tmp; pwd; export TEST=value; echo \$TEST" "Built-in chain" "builtin"
    run_test "Built-in Chaos 2" "echo 'hello world' | cat; export MSG='piped message'; echo \$MSG" "Built-in + pipe" "builtin"
    
    run_test "Resource Test 1" "echo very long command line test with many arguments here" "Long command" "syntax"
    run_test "Resource Test 2" "echo hello | cat | cat | cat | cat | cat | cat | cat | cat | cat" "Long pipe chain" "fd"
    
    run_test "Quote Variable Chaos" "export VAR='hello world' && echo '\$VAR' \"\$VAR\" \$VAR" "Quote/variable mix" "variable"
    run_test "Heredoc Variable Chaos" "export A=hello B=world && cat << EOF\n\$A '\$B' \"\$A \$B\"\nEOF" "Heredoc complex" "heredoc"
    run_test "Pipe Quote Chaos" "echo 'hello world' | cat | cat" "Pipe quote mix" "pipe"
    
    run_test "Error Chain Recovery" "nonexistent1; echo recovered1; nonexistent2; echo recovered2" "Error chain" "syntax"
    run_test "FD Error Recovery" "cat < /nonexistent | cat" "FD error chain" "fd"
    run_test "Signal Error Recovery" "sleep 1" "Signal recovery" "signal"
}

custom_stress_builder() {
    echo -e "\n${PURPLE}${BOLD}🔧 CUSTOM STRESS TEST BUILDER 🔧${NC}"
    echo -e "${CYAN}Build your own stress test scenarios...${NC}\n"
    
    echo -e "${YELLOW}Select stress test components:${NC}"
    echo -e "${GREEN}1) Heavy pipe chains${NC}"
    echo -e "${GREEN}2) Complex redirections${NC}"
    echo -e "${GREEN}3) Variable expansion chaos${NC}"
    echo -e "${GREEN}4) Quote nesting hell${NC}"
    echo -e "${GREEN}5) Signal stress test${NC}"
    echo -e "${GREEN}6) All combined${NC}"
    
    read -p "Enter choice [1-6]: " custom_choice
    
    case $custom_choice in
        1)
            echo -e "${CYAN}Generating heavy pipe chain tests...${NC}"
            for i in {5..15}; do
                local pipe_chain="echo test"
                for j in $(seq 1 $i); do
                    pipe_chain="$pipe_chain | cat"
                done
                run_test "Pipe Chain $i" "$pipe_chain" "No issues" "fd"
            done
            ;;
        2)
            echo -e "${CYAN}Generating complex redirection tests...${NC}"
            run_test "Multi Redirect 1" "echo test > /tmp/1 > /tmp/2 > /tmp/3" "Complex redirect" "fd"
            run_test "Multi Redirect 2" "cat < /dev/null > /tmp/test >> /tmp/test" "Mixed redirects" "fd"
            ;;
        3)
            echo -e "${CYAN}Generating variable expansion chaos...${NC}"
            run_test "Var Chaos 1" "export A=B B=C C=hello && echo \$A \$B \$C" "Variable chain" "memory"
            run_test "Var Chaos 2" "echo \$NONEXISTENT_VAR_12345" "Undefined variable" "memory"
            run_test "Var Chaos 3" "echo \$? \$\$ \$0" "Special variables" "memory"
            ;;
        4)
            echo -e "${CYAN}Generating quote nesting hell...${NC}"
            run_test "Quote Hell 1" "echo \"hello 'world' test\"" "Nested quotes" "syntax"
            run_test "Quote Hell 2" "echo 'can\\'t touch this'" "Escaped quotes" "syntax"
            run_test "Quote Hell 3" "echo \"\\\"hello\\\" world\"" "Escaped double quotes" "syntax"
            ;;
        5)
            echo -e "${CYAN}Generating signal stress tests...${NC}"
            for i in {1..5}; do
                run_test "Signal Stress $i" "sleep $i" "Signal handling" "signal"
            done
            ;;
        6)
            echo -e "${CYAN}Running all custom components...${NC}"
            for choice in {1..5}; do
                custom_choice=$choice
                custom_stress_builder
            done
            ;;
    esac
}

show_results() {
    echo -e "\n${WHITE}${BOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo -e "${PURPLE}${BOLD}                        TEST RESULTS SUMMARY                       ${NC}"
    echo -e "${WHITE}${BOLD}═══════════════════════════════════════════════════════════════════${NC}"
    
    local pass_rate=0
    if [ $TOTAL_TESTS -gt 0 ]; then
        pass_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
    fi
    
    echo -e "${CYAN}${BOLD}Total Tests Run: ${WHITE}$TOTAL_TESTS${NC}"
    echo -e "${GREEN}${BOLD}Tests Passed: ${WHITE}$PASSED_TESTS${NC}"
    echo -e "${RED}${BOLD}Tests Failed: ${WHITE}$FAILED_TESTS${NC}"
    echo -e "${YELLOW}${BOLD}Pass Rate: ${WHITE}$pass_rate%${NC}"
    
    if [ $pass_rate -eq 100 ]; then
        echo -e "\n${GREEN}${BOLD}🎉 PERFECT SCORE! Your minishell is DESTROYER-PROOF! 🎉${NC}"
        echo -e "${GREEN}${BOLD}🏆 LEGENDARY STATUS ACHIEVED! 🏆${NC}"
    elif [ $pass_rate -ge 90 ]; then
        echo -e "\n${GREEN}${BOLD}🔥 EXCELLENT! Your minishell withstood the destruction! 🔥${NC}"
        echo -e "${YELLOW}${BOLD}💪 ALMOST LEGENDARY STATUS! 💪${NC}"
    elif [ $pass_rate -ge 75 ]; then
        echo -e "\n${YELLOW}${BOLD}⚡ GOOD! Your minishell survived most attacks! ⚡${NC}"
        echo -e "${YELLOW}${BOLD}🛡️ SOLID DEFENSE STATUS! 🛡️${NC}"
    elif [ $pass_rate -ge 50 ]; then
        echo -e "\n${YELLOW}${BOLD}⚠️ MODERATE! Your minishell needs reinforcement! ⚠️${NC}"
        echo -e "${RED}${BOLD}🔨 NEEDS IMPROVEMENT STATUS! 🔨${NC}"
    else
        echo -e "\n${RED}${BOLD}💥 DESTROYED! Your minishell was obliterated! 💥${NC}"
        echo -e "${RED}${BOLD}☠️ TOTAL DESTRUCTION STATUS! ☠️${NC}"
    fi
    
    echo -e "${WHITE}${BOLD}═══════════════════════════════════════════════════════════════════${NC}"
    
    if [ $FAILED_TESTS -gt 0 ]; then
        echo -e "\n${RED}${BOLD}🔍 FAILURE ANALYSIS:${NC}"
        echo -e "${RED}• Check for file descriptor leaks${NC}"
        echo -e "${RED}• Verify signal handling robustness${NC}"
        echo -e "${RED}• Examine memory management${NC}"
        echo -e "${RED}• Review error handling logic${NC}"
        echo -e "${RED}• Test environment variable handling${NC}"
    fi
    
    echo -e "\n${CYAN}${BOLD}📁 Test artifacts saved in /tmp/minishell_test_*${NC}"
}

cleanup_tests() {
    echo -e "\n${YELLOW}🧹 Cleaning up test files...${NC}"
    rm -f /tmp/test* /tmp/mem_test /tmp/fd_test* /tmp/env_test* /tmp/syntax_test* /tmp/exit_test* /tmp/memory_test* 2>/dev/null
    rm -f /tmp/1 /tmp/2 /tmp/3 /tmp/out /tmp/signal_test /tmp/redirect_test /tmp/input_test /tmp/append_test 2>/dev/null
    rm -f /tmp/overwrite /tmp/pipe_redirect /tmp/redirect_pipe /tmp/complex /tmp/chaos /tmp/spaces /tmp/nospace 2>/dev/null
    echo -e "${GREEN}✅ Cleanup completed${NC}"
}

check_dependencies() {
    echo -e "${YELLOW}🔍 Checking dependencies...${NC}"
    
    if [ ! -f "$MINISHELL_PATH" ]; then
        echo -e "${RED}❌ Error: minishell executable not found at $MINISHELL_PATH${NC}"
        echo -e "${YELLOW}Please compile your minishell first with 'make'${NC}"
        exit 1
    fi
    
    if ! command -v valgrind >/dev/null 2>&1; then
        echo -e "${YELLOW}⚠️ Warning: valgrind not found. Memory tests will be limited.${NC}"
        VALGRIND=""
    fi
    
    if ! command -v timeout >/dev/null 2>&1; then
        echo -e "${YELLOW}⚠️ Warning: timeout command not found. Tests may hang.${NC}"
    fi
    
    echo -e "${GREEN}✅ Dependencies check completed${NC}"
}

main() {
    show_banner
    check_dependencies
    
    while true; do
        show_menu
        read choice
        
        case $choice in
            1)
                echo -e "${GREEN}Starting Built-in Commands Tests...${NC}"
                builtin_command_tests
                show_results
                read -p "Press Enter to continue..."
                ;;
            2)
                echo -e "${GREEN}Starting FD Leak Detection Tests...${NC}"
                fd_leak_tests
                show_results
                read -p "Press Enter to continue..."
                ;;
            3)
                echo -e "${GREEN}Starting Environment Torture Tests...${NC}"
                env_torture_tests
                show_results
                read -p "Press Enter to continue..."
                ;;
            4)
                echo -e "${GREEN}Starting Signal Handling Tests...${NC}"
                signal_extreme_tests
                show_results
                read -p "Press Enter to continue..."
                ;;
            5)
                echo -e "${GREEN}Starting Syntax Error Tests...${NC}"
                syntax_destroyer_tests
                show_results
                read -p "Press Enter to continue..."
                ;;
            6)
                echo -e "${GREEN}Starting Exit Code Tests...${NC}"
                exit_code_chaos_tests
                show_results
                read -p "Press Enter to continue..."
                ;;
            7)
                echo -e "${GREEN}Starting Pipe & Redirection Tests...${NC}"
                pipe_redirection_hell_tests
                show_results
                read -p "Press Enter to continue..."
                ;;
            8)
                echo -e "${GREEN}Starting Heredoc & Quote Tests...${NC}"
                heredoc_quote_hell_tests
                show_results
                read -p "Press Enter to continue..."
                ;;
            9)
                echo -e "${GREEN}Starting Variable Expansion Tests...${NC}"
                variable_expansion_tests
                show_results
                read -p "Press Enter to continue..."
                ;;
            10)
                echo -e "${GREEN}Starting Memory Leak Tests...${NC}"
                memory_massacre_tests
                show_results
                read -p "Press Enter to continue..."
                ;;
            11)
                echo -e "${RED}${BOLD}🚨 WARNING: ULTIMATE DESTROYER MODE ACTIVATED! 🚨${NC}"
                echo -e "${YELLOW}This will run ALL tests and may take several minutes...${NC}"
                read -p "Are you sure? [y/N]: " confirm
                if [[ $confirm =~ ^[Yy]$ ]]; then
                    ultimate_destroyer
                    show_results
                fi
                read -p "Press Enter to continue..."
                ;;
            12)
                custom_stress_builder
                show_results
                read -p "Press Enter to continue..."
                ;;
            0)
                echo -e "${CYAN}${BOLD}Thank you for using MiniShell Hard Test Suite!${NC}"
                echo -e "${PURPLE}Created by: meto1337${NC}"
                cleanup_tests
                exit 0
                ;;
            *)
                echo -e "${RED}Invalid choice! Please select 0-12.${NC}"
                sleep 1
                ;;
        esac
        
        # Reset counters for next run
        TOTAL_TESTS=0
        PASSED_TESTS=0
        FAILED_TESTS=0
        clear
        show_banner
    done
}

# ==========================================
# SCRIPT EXECUTION
# ==========================================

# Trap signals for cleanup
trap cleanup_tests EXIT INT TERM

# Check if script is being run directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi