# 🐚 MiniShell Projesi - Kapsamlı Teknik Dokümantasyon

Bu doküman, `minishell` projesinin mimarisini, temel modüllerini ve bu modülleri oluşturan anahtar fonksiyonların çalışma prensiplerini derinlemesine incelemektedir. Amaç, kodun her bir parçasının görevini ve projenin bütününe olan katkısını net bir şekilde ortaya koymaktır.

## 🗂️ İçindekiler
1.  [Proje Mimarisi ve Veri Akışı](#-proje-mimarisi-ve-veri-akışı)
2.  [Ana Döngü ve Başlatma (`shell.c`)](#-ana-döngü-ve-başlatma-shellc)
3.  [**Modül 1: Lexer** - Girdiyi Token'lara Ayırma](#-modül-1-lexer---girdiyi-tokenlara-ayırma)
4.  [**Modül 2: Parser** - Token'ları Komut Yapılarına Dönüştürme](#-modül-2-parser---tokenları-komut-yapılarına-dönüştürme)
5.  [**Modül 3: Expander** - Değişken ve Tırnak Genişletme](#-modül-3-expander---değişken-ve-tırnak-genişletme)
6.  [**Modül 4: Executor** - Komutları Çalıştırma](#-modül-4-executor---komutları-çalıştırma)
7.  [**Yardımcı Modüller**](#-yardımcı-modüller)
    - [Sinyal Yönetimi (`signal_handler.c`)](#sinyal-yönetimi-signal_handlerc)
    - [Built-in Komutlar (`builtin_*.c`)](#built-in-komutlar-builtin_c)
    - [Environment ve Export Yönetimi (`env_*.c`, `export_*.c`)](#environment-ve-export-yönetimi-env_c-export_c)
    - [Garbage Collector (`gb_*.c`)](#garbage-collector-gb_c)

---

## 🏗️ Proje Mimarisi ve Veri Akışı

Minishell, kullanıcıdan alınan bir komut satırını çalıştırılabilir bir eyleme dönüştürmek için modüler bir "pipeline" (boru hattı) mimarisi kullanır.

`Girdi (char *)` -> **[LEXER]** -> `Token Listesi (t_token *)` -> **[PARSER]** -> `Komut Listesi (t_parser *)` -> **[EXPANDER]** -> `Genişletilmiş Komut Listesi` -> **[EXECUTOR]** -> `Çıktı`

Bu yapı, her modülün belirli bir sorumluluğa sahip olmasını sağlayarak kodun okunabilirliğini ve yönetilebilirliğini artırır.

---

## 🔄 Ana Döngü ve Başlatma (`shell.c`)

Shell'in kalbi, kullanıcıdan sürekli olarak girdi alan ve işleyen ana döngüdür.

-   **`main()`**: Programın giriş noktasıdır.
    1.  `initialize_shell()` fonksiyonunu çağırarak ortam değişkenlerini (`env`) yükler, `export` listesini oluşturur ve sinyal yöneticilerini (signal handlers) ayarlar.
    2.  `shell_loop()` fonksiyonunu başlatır.
    3.  Döngü bittiğinde (`exit` komutu veya `Ctrl+D` ile), `gb_free_all()` ve `env_gb_free_all()` çağrılarak tüm tahsis edilmiş bellek serbest bırakılır ve program sonlanır.

-   **`shell_loop()`**:
    1.  `readline("MiniShell->>> ")` ile kullanıcıdan satır satır girdi okur. `Ctrl+D` basıldığında `readline` `NULL` döndürür ve döngü sonlanır.
    2.  Alınan satır boş değilse, `process_command_line()` fonksiyonuna gönderir.
    3.  Her döngüden sonra, interaktif sinyal yöneticileri `setup_interactive_signals()` ile yeniden ayarlanır.

-   **`process_command_line()`**: Gelen komut satırını işleyen ana mantığı içerir.
    1.  `tokenize_input()` -> Lexer'ı çağırarak token listesini oluşturur.
    2.  `parse_tokens()` -> Parser'ı çağırarak komut listesini (`t_parser`) oluşturur.
    3.  `execute_cmds()` -> Executor'ı çağırarak komutları çalıştırır.

---

## 📝 **Modül 1: Lexer** - Girdiyi Token'lara Ayırma

Lexer'ın görevi, `char*` tipindeki ham girdiyi, Parser'ın işleyebileceği `t_token` yapılarından oluşan bir bağlı listeye dönüştürmektir.

**Ana Dosyalar**: `lexer.c`, `lexer_extension.c`, `lexer_utils.c`

### Fonksiyonel Analiz

-   **`tokenize_input(char *input)`**: Lexer'ın giriş noktasıdır.
    -   `ft_tokenize_control()` ile girdinin başında `|` gibi bariz syntax hatalarını kontrol eder.
    -   `tokenizer_init()` ile `t_tokenizer` yapısını oluşturur. Bu yapı, girdi string'i üzerinde pozisyon takibi yapar.
    -   `token_input_2()` döngüsü içinde `tokenizer_get_next_token()`'ı tekrar tekrar çağırarak tüm token'ları alır ve bir bağlı liste oluşturur.

-   **`tokenizer_get_next_token(t_tokenizer *tokenizer)`**: Lexer'ın kalbidir. Her çağrıldığında bir sonraki token'ı döndürür.
    -   `ft_skip_space()` ile tokenlar arasındaki boşlukları atlar.
    -   `|`, `<`, `>` gibi özel karakterleri doğrudan tanır ve ilgili `t_token`'ı oluşturur (`TOKEN_PIPE`, `TOKEN_REDIR_IN`, vb.).
    -   `lexer_peek()` fonksiyonu ile `>>` ve `<<` gibi iki karakterli operatörleri ayırt eder (bir sonraki karaktere "göz atarak").
    -   Bu karakterlerden hiçbiri değilse, bir "kelime" (word) ile karşılaştığını varsayar ve `extract_complex_word()`'u çağırır.

-   **`extract_complex_word(t_tokenizer *tokenizer)`**: Tırnak işaretleri ve özel karakterler içermeyen standart kelimeleri işler.
    -   Boşluk veya özel bir karakter (`|`, `<`, `>`) görene kadar karakterleri biriktirir.
    -   **Tırnak Yönetimi**: Tek (`'`) veya çift (`"`) tırnak gördüğünde, `in_quote` durumunu değiştirir. Tırnak açık olduğu sürece, boşluklar ve özel karakterler de kelimenin bir parçası olarak kabul edilir. Bu, `echo "selam dunya | cat"` gibi bir komutun doğru token'lara ayrılmasını sağlar.
    -   Eğer tırnak kapatılmazsa, bir syntax hatası olarak işaretlenir (`quote error`).

---

## 🔗 **Modül 2: Parser** - Token'ları Komut Yapılarına Dönüştürme

Parser, Lexer'dan gelen token listesini alır ve `t_parser` yapılarından oluşan mantıksal bir komut zinciri oluşturur. Her `t_parser` bir `pipe` (`|`) ile ayrılmış bir komutu temsil eder.

**Ana Dosyalar**: `parser_main.c`, `parser_handle.c`, `parser_redirect.c`, `parser_heredoc.c`, `parser_syntax.c`

### Fonksiyonel Analiz

-   **`parse_tokens(t_token *tokens, t_env *env_list)`**: Parser'ın ana fonksiyonudur.
    -   `ft_syn()` ile `>` veya `<` gibi bir yönlendirme token'ından sonra bir dosya adı gelip gelmediğini kontrol eder.
    -   Bir döngü içinde `ft_control_token()`'ı çağırarak token listesini tüketir ve `t_parser` listesini oluşturur.

-   **`ft_control_token(t_token *tokens, t_all *data)`**: Bir `pipe` görene kadar veya tokenlar bitene kadar olan kısmı tek bir `t_parser` komutuna dönüştürür.
    -   `gb_malloc` ile yeni bir `t_parser` (cmd) nesnesi yaratır ve `init_parser_cmd` ile başlatır.
    -   Bir döngü içinde `ft_loop()`'u çağırarak tokenları işler.
    -   `finalize_cmd()` ile komutun `argv` dizisini `NULL` ile sonlandırır ve ana komut listesine (`data->cmd_list`) ekler.

-   **`ft_loop(t_token **tokens, t_parser *cmd, int *argc, t_all *all)`**: Tek bir token'ı işler ve ne yapacağına karar verir.
    -   **`TOKEN_WORD` ise**: Bu bir komut veya argümandır. `ft_loop_3` içinde `expand_with_quotes` çağrılır (Expander modülü) ve sonuç `cmd->argv` dizisine eklenir. `expand_argv_capacity` ile `argv` dizisi gerektiğinde büyütülür.
    -   **Yönlendirme Token'ları ise (`<`, `>>`, vb.)**: `redir_control_state` veya `ft_redir_heredoc` çağrılır. Bu fonksiyonlar, yönlendirmenin türünü ve dosya adını (veya heredoc delimiter'ını) içeren bir `t_redirection` yapısı oluşturur ve `cmd->redirs` listesine ekler.

-   **`ft_redir_heredoc(t_token *tokens, t_parser *cmd)`**: Heredoc (`<<`) işlemleri için özel mantık içerir.
    -   Delimiter'ı alır (`tokens->next->value`).
    -   `has_quote_chars()` ile delimiter'ın tırnak içerip içermediğini kontrol eder. Eğer içeriyorsa, heredoc içindeki değişkenler expand edilmeyecektir (`no_expand = 1`).
    -   `remove_quotes()` ile delimiter'ı tırnaklardan temizler.
    -   `add_redirection_with_expansion()` ile `REDIR_HEREDOC` tipinde bir yönlendirme oluşturur.

---

## 💥 **Modül 3: Expander** - Değişken ve Tırnak Genişletme

Expander, Parser tarafından oluşturulan `argv` ve `redirection` içindeki metinlerde bulunan `$`'lı değişkenleri ve tırnakları işler.

**Ana Dosyalar**: `expander_variable.c`, `expander_quotes.c`, `expander_quotes2.c`, `expander_heredoc.c`, `expander_utils.c`

### Fonksiyonel Analiz

-   **`expand_with_quotes(char *str, t_env *env_list)`**: Ana genişletme fonksiyonudur. String'i karakter karakter işler.
    -   `quote_char` değişkeni ile tırnak (`'` veya `"`) içinde olup olmadığını takip eder.
    -   `$` karakteriyle karşılaştığında ve tek tırnak (`'`) içinde değilse, `handle_dollar()`'ı çağırır.
    -   Diğer karakterleri `push_char_res` ile sonuç string'ine ekler.

-   **`handle_dollar(char *str, int *i, t_env *env_list)`**: `$` ile başlayan ifadeleri işler.
    -   `$?`: `get_last_exit_status()`'tan dönen değeri `ft_itoa` ile string'e çevirir.
    -   `$$`: `getpid()` ile işlem ID'sini alır ve string'e çevirir.
    -   `$VAR`: Değişkenin ismini (`VAR`) okur, `find_env` ile ortam değişkenleri listesinde arar. Bulamazsa `find_export_value` ile `export` listesinde arar ve değerini döndürür. Değişken yoksa boş bir string (`""`) döndürür.

-   **`split_expanded_string(char *str)`**: Tırnak içinde olmayan bir değişkenin değeri boşluk içeriyorsa (`VAR="a b"` -> `echo $VAR`), bu değeri birden fazla argümana böler. `bash`'in "word splitting" davranışını taklit eder.

-   **`expand_heredoc_line(char *str, t_env *env_list)`**: Heredoc satırları için özel genişletme yapar. Normal genişletmeden farkı, tırnak karakterlerini metnin bir parçası olarak görmesidir.

---

## 🚀 **Modül 4: Executor** - Komutları Çalıştırma

Executor, Parser'dan gelen ve Expander tarafından işlenen komut listesini alır ve çalıştırır.

**Ana Dosyalar**: `execute_main.c`, `execute_child.c`, `execute_fork.c`, `execute_redirections.c`, `execute_finder.c`, `execute_error_exit.c`

### Fonksiyonel Analiz

-   **`execute_cmds(t_parser *cmd_list, ...)`**: Yürütme işleminin ana giriş noktasıdır.
    1.  `data_init()` ile `t_exec_data` yapısını (pipe'lar, PID'ler, standart I/O'nun yedekleri) hazırlar.
    2.  `heredoc_handle()`'ı çağırır. Bu çok önemli bir adımdır; **tüm heredoc'lar diğer komutlar çalıştırılmadan önce okunur** ve içerikleri geçici pipe'lara yazılır. `cmd->heredoc_fd` bu pipe'ın okuma ucunu tutar.
    3.  **Özel Durum**: Eğer komut zincirinde tek bir komut varsa ve bu bir built-in ise (`is_builtin`), `fork` yapmadan doğrudan ana shell process'inde `n_next_or_built` aracılığıyla çalıştırılır. Bu, `cd` ve `export` gibi shell'in kendi durumunu değiştirmesi gereken komutlar için zorunludur.
    4.  **Genel Durum**: Diğer tüm durumlar için `execute_loop`'u çağırır.

-   **`execute_loop(...)`**: Komut listesi (`t_parser`) üzerinde bir döngü başlatır.
    -   Her komut için `process_command()`'ı çağırır.

-   **`process_command(...)`**: Tek bir komutu işler.
    1.  Eğer bir sonraki komut varsa (`cmd->next`), `pipe()` sistem çağrısı ile bir pipe oluşturur.
    2.  `setup_and_fork()` aracılığıyla `fork()`'u çağırarak yeni bir alt process (child process) yaratır.
    3.  **Parent Process**: Pipe'ın yazma ucunu kapatır (`close(data->pipefd[1])`) ve okuma ucunu bir sonraki komutun girişi olması için `data->in_fd`'ye atar.
    4.  **Child Process**: `child_process_exec()` fonksiyonu çalışır.

-   **`child_process_exec(...)`**: Alt process içinde çalışır.
    1.  `dup2()` sistem çağrılarını kullanarak I/O yönlendirmelerini yapar. Gerekliyse, bir önceki komutun pipe çıkışını (`data->in_fd`) `STDIN_FILENO`'ya bağlar. Eğer bir sonraki komut varsa, mevcut pipe'ın yazma ucunu (`data->pipefd[1]`) `STDOUT_FILENO`'ya bağlar.
    2.  `ft_redir_ctrl()` ile `<` ve `>` gibi dosya yönlendirmelerini uygular.
    3.  `ft_exec_start()`'ı çağırır.

-   **`ft_exec_start(...)`**: Komutu fiilen çalıştırır.
    -   Eğer komut built-in ise, `ft_builtin_call` ile çalıştırır ve `exit` ile alt process'i sonlandırır.
    -   Değilse, `find_executable()` ile komutun tam yolunu (`/bin/ls` gibi) bulur.
    -   `execve()` sistem çağrısını yapar. `execve` başarılı olursa, alt process'in kodu, çalıştırılan komutun koduyla değiştirilir. Başarısız olursa, hata mesajı basılır ve process sonlandırılır.

-   **`find_executable(...)`**: `$PATH` ortam değişkenini `:` karakterine göre böler ve her bir dizinde komut adında çalıştırılabilir bir dosya arar.

---

## 🛠️ **Yardımcı Modüller**

### Sinyal Yönetimi (`signal_handler.c`)

-   **`setup_interactive_signals()`**: Ana döngüde, kullanıcıdan girdi beklenirken aktiftir. `Ctrl+C` (`SIGINT`) `handle_sigint`'i tetikler, bu da yeni bir prompt satırı basar. `Ctrl+\` (`SIGQUIT`) görmezden gelinir.
-   **`heredoc_signals()`**: Heredoc girdisi okunurken `Ctrl+C`, `handle_sigint_heredoc`'u tetikleyerek sadece heredoc okumasını iptal eder.
-   **`setup_parent_execution_signals()` / `setup_child_signals()`**: Komut çalışırken, ana shell sinyalleri görmezden gelirken (`SIG_IGN`), alt process sinyalleri varsayılan şekilde ele alır (`SIG_DFL`). Bu, `Ctrl+C`'nin sadece çalışan komutu (örneğin `sleep 100`) sonlandırmasını, shell'in kendisini değil, sağlar.

### Built-in Komutlar (`builtin_*.c`)

-   Her bir `builtin_*.c` dosyası, `is_builtin()` tarafından tanınan bir komutun mantığını içerir.
-   `ft_builtin_call()` fonksiyonu, komutun adına göre doğru built-in fonksiyonunu çağırır.
-   `set_last_exit_status()` ile `$?` için çıkış kodunu ayarlarlar.

### Environment ve Export Yönetimi (`env_*.c`, `export_*.c`)

-   **`t_env` ve `t_export` Listeleri**: `env`, `KEY=VALUE` çiftlerini tutarken, `export` sadece `export` edilmiş değişkenleri tutar (değeri olmayanlar dahil).
-   **`init_env()`**: Program başlangıcında `main`'in `envp` argümanından `env_list`'i oluşturur.
-   **`set_env_value()` ve `set_export_value()`**: Bu fonksiyonlar, ilgili listelere değişken ekler veya günceller. `export` komutu her iki listeyi de günceller.
-   **`env_list_to_array()`**: `execve`'nin ihtiyaç duyduğu `char *[]` formatını oluşturmak için `env_list`'i bir diziye dönüştürür.

### Garbage Collector (`gb_*.c`)

-   **Amaç**: Bellek sızıntılarını (`leak`) önlemek için basit bir bellek yönetim sistemi.
-   **`gb_malloc(size_t size)`**: `malloc`'u çağırdıktan sonra, dönen pointer'ı `gc_safe` ile küresel bir bağlı listeye (`t_gb *list`) ekler.
-   **`gc_free(void *ptr)`**: Bir pointer'ı hem bellekten `free` eder hem de küresel listeden çıkarır.
-   **`gb_free_all()`**: Program sonunda, küresel listedeki tüm pointer'ları `free` eder.
-   **`env_gb_*` Fonksiyonları**: Ortam değişkenleri için ayrı bir garbage collector listesi tutulur. Bu, `unset` gibi komutlarla sadece env değişkenlerinin belleğini yönetmeyi kolaylaştırır.