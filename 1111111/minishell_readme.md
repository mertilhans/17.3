# 🐚 MiniShell - Bash Clone Implementation

Kendi bash clone'umuz! Bu proje, shell'in nasıl çalıştığını anlamak ve kendi shell implementasyonumuzu yapmak için geliştirildi.

## 📋 İçindekiler
- [Genel Bakış](#genel-bakış)
- [Algoritma Akışı](#algoritma-akışı)
- [Kod Mimarisi](#kod-mimarisi)
- [Özellikler](#özellikler)
- [Kullanım](#kullanım)
- [Test Edilebilir Özellikler](#test-edilebilir-özellikler)

## 🎯 Genel Bakış

MiniShell, bash'in temel özelliklerini destekleyen bir shell implementasyonudur. Komut satırından girdi alır, parse eder ve çalıştırır.

### Ana Bileşenler:
1. **Lexer** - Girdiyi token'lara böler
2. **Parser** - Token'ları komut yapısına dönüştürür  
3. **Expander** - Değişkenleri genişletir
4. **Executor** - Komutları çalıştırır

## 🔄 Algoritma Akışı

### 1. INPUT ALMA
```
Kullanıcı: echo "Hello $USER"
↓
readline() ile input alınır
```

### 2. TOKENIZATION (Lexer)
```
Input: echo "Hello $USER"
↓
Tokens: [WORD:"echo"] [WORD:"Hello $USER"]
```

**Lexer nasıl çalışır:**
- Karakterleri tek tek okur
- Boşlukları atlayarak token'ları ayırır
- Quote'ları (`"` ve `'`) handle eder
- Özel karakterleri (`|`, `<`, `>`, `>>`, `<<`) tanır

### 3. PARSING
```
Tokens: [WORD:"echo"] [WORD:"Hello $USER"]
↓
Command Structure:
- argv[0] = "echo"
- argv[1] = "Hello $USER" (henüz expand edilmemiş)
- redirs = NULL
```

**Parser nasıl çalışır:**
- Token'ları sırayla işler
- Her pipe (`|`) arasında yeni komut oluşturur
- Redirection'ları (`<`, `>`, `>>`, `<<`) ayrı ayrı handle eder
- Heredoc'lar için özel işlem yapar

### 4. EXPANSION
```
"Hello $USER" → "Hello mertilhan1337"
```

**Expansion kuralları:**
- Single quote (`'`) içinde expansion YOK
- Double quote (`"`) içinde expansion VAR
- Quote dışında expansion VAR + word splitting

### 5. EXECUTION
```
Command: echo "Hello mertilhan1337"
↓
Fork → execve → Output: Hello mertilhan1337
```

## 🏗️ Kod Mimarisi

### Dosya Yapısı
```
minishell/
├── shell.c          # Main loop ve initialization
├── lexer.c          # Tokenization
├── parser.c         # Command structure oluşturma
├── expander.c       # Variable expansion
├── execute.c        # Command execution
├── builtin_*.c      # Built-in commands
├── env_*.c          # Environment management
├── gb_*.c           # Garbage collector
└── utils.c          # Helper functions
```

### Veri Yapıları

#### Token
```c
typedef struct s_token {
    t_token_type type;    // WORD, PIPE, REDIR_IN, etc.
    char *value;          // "echo", "|", "<", etc.
    struct s_token *next; // Linked list
} t_token;
```

#### Command (Parser)
```c
typedef struct s_parser {
    char **argv;              // ["echo", "hello", NULL]
    t_redirection *redirs;    // < > >> << redirection'ları
    struct s_parser *next;    // Pipe'lı komutlar için
    int heredoc_fd;           // Heredoc file descriptor
} t_parser;
```

#### Environment
```c
typedef struct s_env {
    char *key;            // "USER"
    char *value;          // "mertilhan1337"
    struct s_env *next;   // Linked list
} t_env;
```

## ✨ Özellikler

### ✅ Desteklenen Özellikler
- **Built-in Commands**: `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`
- **Pipes**: `cmd1 | cmd2 | cmd3`
- **Redirections**: `<`, `>`, `>>`
- **Heredoc**: `<<` (expansion ile ve olmadan)
- **Variable Expansion**: `$USER`, `$?`, `$$`
- **Quote Handling**: `'single'`, `"double"`
- **Signal Handling**: Ctrl+C, Ctrl+\
- **Exit Status**: Her komutun exit code'u

### 🎯 Echo Özel Durumları
- `echo -n`: Newline yazdırmaz
- `echo -nnnn`: Multiple n flag'ları
- `echo -nnnx`: Invalid flag kombinasyonu

### 📝 Heredoc Özel Durumları
```bash
# Expansion VAR
cat << EOF
Hello $USER
EOF

# Expansion YOK  
cat << "EOF"
Hello $USER  # Literal olarak yazılır
EOF
```

## 🚀 Kullanım

### Derleme
```bash
make
```

### Çalıştırma
```bash
./minishell
MiniShell->>>   echo "Hello World"
Hello World
MiniShell->>>   exit
```

## 🧪 Test Edilebilir Özellikler

### Basic Commands
```bash
echo hello world
pwd
cd ..
export TEST=value
echo $TEST
unset TEST
env | grep USER
```

### Pipes ve Redirections
```bash
echo test | cat
echo test > file.txt
cat < file.txt
echo line1 >> file.txt
ls | grep .c | cat
```

### Heredoc Tests
```bash
# Normal heredoc (expansion var)
cat << EOF
Hello $USER
Current dir: $PWD
EOF

# Quoted heredoc (expansion yok)
cat << "EOF"
Hello $USER
Current dir: $PWD  
EOF
```

### Advanced Echo Tests
```bash
echo -n test
echo -nnnn hello
echo -nnnx invalid    # Invalid flag
echo "quoted string"
echo 'single quotes'
```

### Error Handling
```bash
|                     # Syntax error
cat <                 # Missing file
nonexistent_command   # Command not found
```

### Environment Variables
```bash
export USER=test
echo $USER
echo $?               # Exit status
echo $$               # Process ID
echo $NONEXISTENT     # Empty expansion
```

## 🔧 Algoritma Detayları

### Lexer Algoritması
1. Input string'i karakter karakter oku
2. Whitespace'leri atla
3. Quote başlangıcı görürsen, quote bitene kadar oku
4. Özel karakter görürsen (`|`, `<`, `>`) token oluştur
5. Normal karakterleri word olarak topla

### Parser Algoritması
1. Token listesini sırayla işle
2. PIPE görene kadar token'ları bir komuta ekle
3. Redirection token'larını ayrı handle et
4. Heredoc için delimiter'ı kaydet
5. Pipe gördüğünde yeni komut başlat

### Expansion Algoritması
1. String'i karakter karakter oku
2. `$` gördüğünde:
   - `$?` → exit status
   - `$$` → process ID  
   - `$VAR` → environment variable
3. Quote context'ini koru:
   - Single quote: expansion yok
   - Double quote: expansion var
   - No quote: expansion + word splitting

### Execution Algoritması
1. Heredoc'ları önceden işle
2. Her komut için fork()
3. Child process'te:
   - Pipe'ları setup et
   - Redirection'ları uygula
   - Built-in ise builtin çalıştır
   - Değilse execve() çağır
4. Parent'ta child'ları bekle

## 🎯 Önemli Noktalar

### Memory Management
- **Garbage Collector** kullanılıyor
- Her malloc'lu pointer track ediliyor
- Program sonunda otomatik temizlik

### Signal Handling
- **Interactive mode**: Ctrl+C yeni prompt
- **Heredoc mode**: Ctrl+C heredoc'u sonlandır
- **Execution mode**: Child process'e signal gönder

### Error Handling
- Syntax error'lar bash uyumlu
- File error'ları perror ile
- Exit status'lar doğru set ediliyor

## 🏆 Sonuç

Bu MiniShell, bash'in core özelliklerini implement ederek shell'in nasıl çalıştığını gösteriyor. Lexer → Parser → Expander → Executor pipeline'ı ile modüler bir yapıda tasarlandı.

**En zorlu kısımlar:**
1. Quote handling ve expansion logic
2. Heredoc expansion kuralları  
3. Pipe chain'lerinde FD management
4. Signal handling'in doğru yapılması

**En keyifli kısım:**
Kendi shell'inde kendi komutlarını çalıştırmak! 🚀