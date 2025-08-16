# 🐚 MiniShell Projesi - Kapsamlı Teknik Dokümantasyon ve Kavramsal Analiz

## Giriş: MiniShell Nedir ve Bizden Ne Bekleniyor?

MiniShell, 42 Okulu müfredatının en önemli projelerinden biridir. Temel amacı, UNIX benzeri bir işletim sisteminin temel taşı olan "kabuk" (shell) programının basitleştirilmiş bir versiyonunu C dilinde kodlamaktır. Bu proje bizden sadece `fork`, `pipe`, `execve` gibi sistem çağrılarını kullanmayı değil, aynı zamanda bir programın baştan sona nasıl bir mantık silsilesiyle çalıştığını anlamamızı bekler.

Kullanıcıdan alınan basit bir metin girdisinin (`ls -l | grep a > out.txt`) nasıl ayrıştırıldığını, yorumlandığını, proseslere bölündüğünü ve işletim sistemi tarafından nasıl çalıştırıldığını en temel seviyede tecrübe etmemizi sağlar. Bu süreç, modern yazılım geliştirmenin temelini oluşturan **veri yapıları, algoritma tasarımı, bellek yönetimi ve proses yönetimi** gibi kritik konularda derin bir anlayış kazandırır.

---

## Bölüm 1: UNIX Proses Yönetiminin Temelleri: `fork`, `process` ve `pid`

Minishell'in nasıl çalıştığını anlamak için önce UNIX'in prosesleri nasıl yönettiğini bilmemiz gerekir. Bu bölüm, projenin temelini oluşturan bu üç anahtar kavramı detaylıca açıklar.

### Proses (Process) Nedir?

Bir **proses**, en basit tanımıyla, çalışmakta olan bir programdır. Bir programı (`./minishell` gibi) çalıştırdığınızda, işletim sistemi o program için bellekte bir alan ayırır ve programın komutlarını (kodunu) bu alana yükler. Bu "canlı" hale gelmiş programa proses denir. Her prosesin kendine ait kaynakları vardır:

-   **Bellek Alanı**: Kod, global değişkenler ve `stack` (yığın) gibi verilerin tutulduğu özel bir bellek bölgesi.
-   **Process ID (PID)**: Onu diğer proseslerden ayıran benzersiz bir kimlik numarası.
-   **File Descriptors (Dosya Tanımlayıcıları)**: `STDIN (0)`, `STDOUT (1)`, `STDERR (2)` gibi standart I/O kanalları başta olmak üzere, açık dosyaları temsil eden numaralar.
-   **Durum (State)**: Çalışıyor (running), bekliyor (waiting), uyuyor (sleeping) gibi o anki durumu.

### `fork()` Sistem Çağrısı: Klonlama Sanatı

`fork()`, UNIX'teki en temel ve güçlü sistem çağrılarından biridir. Görevi, kendisini çağıran prosesin **birebir aynısı** olan yeni bir proses (bir klon) oluşturmaktır.

-   **Parent Process (Ana Proses)**: `fork()`'u çağıran orijinal prosestir.
-   **Child Process (Çocuk Proses)**: `fork()` tarafından yeni oluşturulan prosestir.

`fork()` çağrıldığında işletim sistemi şunları yapar:
1.  Parent prosesin bellek alanını, dosya tanımlayıcılarını ve diğer tüm kaynaklarını kopyalar.
2.  Yeni bir PID atayarak child prosesi oluşturur.
3.  Bu andan itibaren, iki proses de **bağımsız** olarak çalışmaya devam eder. Kodun aynı satırından (`fork()`'un dönüşünden) yollarına devam ederler.

**`fork()`'un Geri Dönüş Değeri Neden Kritik?**

`fork()` her iki prosese de bir değer döndürür, ancak bu değer farklıdır. Bu fark, hangi prosesin parent, hangisinin child olduğunu anlamamızı sağlar:

-   **Parent Prosese**: Oluşturduğu child prosesin PID'sini (pozitif bir sayı) döndürür.
-   **Child Prosese**: `0` değerini döndürür.
-   **Hata Durumunda**: `-1` döndürür.

Minishell'de bu ayrımı şöyle kullanırız:

```c
pid_t pid = fork();

if (pid == -1) {
    // Hata oluştu
}
else if (pid == 0) {
    // Bu kod bloğu SADECE child process tarafından çalıştırılır.
    // execve() burada çağrılır.
}
else {
    // Bu kod bloğu SADECE parent process tarafından çalıştırılır.
    // waitpid() ile child'ın bitmesi burada beklenir.
}
```

### PID ve PPID Nedir?

**PID (Process ID)**: Her prosesin, işletim sistemi tarafından atanan benzersiz bir tam sayı kimliğidir. `getpid()` sistem çağrısı ile bir proses kendi PID'sini öğrenebilir. Minishell'de `$$` değişkeni bu değeri temsil eder.

**PPID (Parent Process ID)**: Bir prosesi oluşturan ana prosesin PID'sidir. `getppid()` ile öğrenilir. Her child prosesin bir parent'ı vardır. Eğer bir parent proses, child'ı beklemeden sonlanırsa, bu child "yetim" (orphan) kalır ve genellikle sistemin ilk prosesi olan `init` (PID 1) tarafından evlat edinilir.

Bu üç kavram, Minishell'in temel çalışma mantığını oluşturur: Shell (parent), çalıştıracağı her komut için bir child proses `fork()`'lar. Bu child proses, `execve` ile kendini yeni komutla değiştirir ve işini bitirince sonlanır. Shell ise bu sırada `waitpid` ile child'ın bitmesini bekler.

---

## Bölüm 2: Proje Mimarisi ve Veri Akışı

Minishell, kullanıcı girdisini adım adım işleyen modüler bir "pipeline" (boru hattı) mimarisi üzerine kuruludur. Bu, karmaşıklığı yönetmeyi ve her bir bileşenin sorumluluğunu net bir şekilde ayırmayı sağlar.

### Veri Akış Şeması:

**Girdi (char *)**: `readline` ile alınan ham metin.

```
"echo 'hello $USER' | wc -c > output.txt"
```

**➡️ Modül 1: LEXER**

Çıktı: `t_token` yapılarından oluşan bir bağlı liste.

```
[WORD:"echo"] -> [WORD:"'hello $USER'"] -> [PIPE:"|"] -> [WORD:"wc"] -> [WORD:"-c"] -> [REDIR_OUT:">"] -> [WORD:"output.txt"]
```

**➡️ Modül 2: PARSER**

Çıktı: `t_parser` yapılarından oluşan, pipe ile birbirine bağlanmış bir komut listesi.

```
Komut 1 (t_parser):
    argv: ["echo", "'hello $USER'"]
    redirs: NULL
    next: -> Komut 2

Komut 2 (t_parser):
    argv: ["wc", "-c"]
    redirs: [type:REDIR_OUT, filename:"output.txt"]
    next: NULL
```

**➡️ Modül 3: EXPANDER**

İşlem: Parser'dan gelen `t_parser` listesindeki `argv` ve `redirs->filename` alanlarını işler.

```
Komut 1 (t_parser):
    argv: expand("'hello $USER'") -> ["echo", "hello $USER"] (tek tırnak yüzünden $USER genişletilmez).

Komut 2 (t_parser):
    argv: Değişiklik yok.
    redirs: expand("output.txt") -> [type:REDIR_OUT, filename:"output.txt"]
```

**➡️ Modül 4: EXECUTOR**

İşlem: Genişletilmiş komut listesini alır ve çalıştırır.

1. `pipe()` oluşturulur.
2. Komut 1 için `fork()`: Child 1, `echo`'yu çalıştırır, çıktısını pipe'ın yazma ucuna yönlendirir.
3. Komut 2 için `fork()`: Child 2, pipe'ın okuma ucunu stdin olarak ayarlar, `output.txt` dosyasını stdout olarak açar ve `wc -c` komutunu çalıştırır.
4. Parent Process: Her iki child process'in de bitmesini bekler.

---

## Bölüm 3: Ana Döngü ve Başlatma (shell.c)

Shell'in ana iskeleti ve başlangıç yapılandırması bu dosyada yer alır.

### `main()` Fonksiyonu:

1. **`set_last_exit_status(0)`**: Programın başlangıç çıkış kodunu 0 olarak ayarlar.

2. **`initialize_shell(env)`**: Shell'i kullanıma hazırlar. Bu kritik fonksiyon şunları yapar:
   - **`init_env(env)`**: `main` fonksiyonundan gelen `char **env` dizisini, yönetimi daha kolay olan `t_env` bağlı listesine dönüştürür.
   - **`get_export_list()` ve `init_export_from_env()`**: `export` komutunun kullanacağı ayrı bir `t_export` listesi oluşturur ve başlangıçta env listesiyle doldurur.
   - **`init_shell_variables(&env_list)`**: `SHLVL`, `PWD` ve `OLDPWD` gibi temel shell değişkenlerini ayarlar. Örneğin `SHLVL`'i bir artırır.
   - **`setup_interactive_signals()`**: Sinyal yöneticilerini interaktif mod için ayarlar.

3. **`shell_loop(env_list, env)`**: Shell'in ana döngüsünü başlatır.

4. **Temizlik**: Döngüden çıkıldığında `rl_clear_history()`, `gb_free_all()` ve `env_gb_free_all()` çağrılarak tüm bellek sızıntıları önlenir.

### `shell_loop(t_env *env_list, char **env)`:

- Sonsuz bir `while(1)` döngüsü içinde çalışır.
- **`readline()`**: Kullanıcıdan girdi bekler. Girdi satırı (`line`) alındığında, geçmişe (history) eklenir. Ctrl+D basılırsa `readline` `NULL` döner, exit mesajı basılır ve döngü kırılır.
- **`process_command_line(line, &env_list, env)`**: Alınan komut satırını işlemek için ana işlem hattını tetikler. Bu fonksiyon, Lexer, Parser ve Executor modüllerini sırayla çağırır.

---

## Bölüm 4: Modül 1: Lexer (lexer*.c)

**Amaç**: Ham metin girdisini, anlamlı birimler olan "token"lara ayırmak.

### Anahtar Fonksiyonlar ve Çalışma Prensibi

#### `t_token *tokenize_input(char *input)`:

**Görevi**: Lexer modülünün ana giriş noktasıdır. String'i alır ve token listesini döndürür.

**Adım Adım Çalışması**:

1. **`ft_tokenize_control(input)`**: Girdide bariz syntax hataları olup olmadığını kontrol eder (örneğin, komutun sadece `|` olması). Eğer hata varsa `NULL` döner.

2. **`tokenizer_init(input)`**: Girdi string'i ile bir `t_tokenizer` yapısı oluşturur. Bu yapı, string üzerinde bir imleç gibi davranarak mevcut pozisyonu (`pos`) ve karakteri (`current`) takip eder.

3. **`token_input_2(...)`**: Bir döngü içinde `tokenizer_get_next_token`'ı çağırarak tüm token'ları alır ve bunları birbirine bağlayarak listeyi oluşturur.

#### `t_token *tokenizer_get_next_token(t_tokenizer *tokenizer)`:

**Görevi**: Girdideki bir sonraki token'ı bulup döndürür.

**Adım Adım Çalışması**:

1. **`ft_skip_space(tokenizer)`**: Anlamsız boşlukları atlar.

2. **Özel Karakter Kontrolü**: `>` karakterini kontrol eder. Eğer bir sonraki karakter de `>` ise (`lexer_peek`), `TOKEN_REDIR_APPEND` (`>>`) oluşturur, değilse `TOKEN_REDIR_OUT` (`>`) oluşturur.

3. **`tokenizer_get_next_token_2(tokenizer)`**: `|` ve `<` karakterlerini benzer şekilde işler. `<`'den sonra tekrar `<` geliyorsa `TOKEN_HEREDOC` (`<<`) oluşturulur.

4. **Kelime (Word) Tespiti**: Yukarıdakilerden hiçbiri değilse, bunun bir kelime olduğunu varsayar ve `extract_complex_word(tokenizer)`'ı çağırır.

#### `char *extract_complex_word(t_tokenizer *tokenizer)`:

**Görevi**: Tırnak işaretlerini (`'` ve `"`) dikkate alarak tek bir token kelimesi çıkarır. Lexer'ın en karmaşık mantığı buradadır.

**Adım Adım Çalışması**:

1. **`ft_token_data_init(&data)`**: Bir kelimeyi geçici olarak oluşturmak için `t_token_data` yapısını başlatır. `data.in_quote` (char) hangi tırnak içinde olunduğunu (`'` veya `"` veya `\0`) tutar.

2. **`while (tokenizer->current && (data.in_quote || (!is_special_char(...) && !is_whitespace(...))))`**: Bu koşul, kelimenin nerede bittiğini belirler. Eğer bir tırnak içindeysek (`data.in_quote` doluysa), boşluklar ve özel karakterler de kelimenin bir parçasıdır. Tırnak içinde değilsek, boşluk veya özel karakter gördüğümüz an kelime biter.

3. **`process_character(...)`**: Döngünün her adımında bu fonksiyon çağrılır:
   - Gelen karakter tırnak ise `handle_quote` ile `data.in_quote` durumu güncellenir.
   - Tüm karakterler `append_char` ile `data.word`'e eklenir.

4. **Hata Kontrolü**: Döngü bittiğinde `data.in_quote` hala doluysa, bu kapatılmamış bir tırnak vardır ve hata verilir.

---

## Bölüm 5: Modül 2: Parser (parser*.c)

**Amaç**: Token listesini, pipe'larla birbirine bağlanmış, `t_parser` komut yapılarına dönüştürmek.

### Anahtar Fonksiyonlar ve Çalışma Prensibi

#### `t_parser *parse_tokens(t_token *tokens, t_env *env_list)`:

**Görevi**: Parser modülünün ana fonksiyonu.

**Adım Adım Çalışması**:

1. **`ft_all_init(...)`**: `t_all` yapısını oluşturur. Bu yapı, hem oluşturulan komut listesini (`cmd_list`) hem de environment listesini (`env_list`) bir arada tutarak diğer fonksiyonlara kolayca geçilmesini sağlar.

2. **`ft_syn(tokens)`**: Yönlendirme token'larından sonra bir kelime gelip gelmediği gibi temel syntax kontrollerini yapar.

3. **`while (tokens && tokens->type != TOKEN_EOF)`**: Token listesi bitene kadar döner. Her döngüde `ft_control_token`'ı çağırarak bir pipe'lık komut bloğunu işler.

#### `t_token *ft_control_token(t_token *tokens, t_all *data)`:

**Görevi**: Bir pipe görene kadar olan token'ları tek bir `t_parser` yapısına dönüştürür.

**Adım Adım Çalışması**:

1. **`init_parser_cmd(&cmd)`**: Yeni, boş bir `t_parser` yapısı oluşturur. Argümanlar için `argv` dizisini ve yönlendirmeler için `redirs` listesini hazırlar.

2. **`while (tokens && tokens->type != TOKEN_PIPE && tokens->type != TOKEN_EOF)`**: Pipe görene kadar `ft_loop`'u çağırarak her token'ı işler.

3. **`finalize_cmd(...)`**: Komutun `argv` dizisini `NULL` ile sonlandırır ve oluşturulan komutu ana komut listesine (`data->cmd_list`) ekler.

#### `void ft_loop(t_token **tokens, ...)`:

**Görevi**: Tek bir token'ı işler ve türüne göre ilgili fonksiyonu çağırır.

**Mantığı**:

- **`TOKEN_WORD` ise**: `ft_loop_3`'ü çağırır. Bu fonksiyon, kelimeyi `expand_with_quotes` ile anında genişletir ve `cmd->argv`'a ekler.
- **`TOKEN_REDIR_IN`, `_OUT`, `_APPEND` ise**: `redir_control_state`'i çağırır. Bu fonksiyon `process_redirection_expansion`'ı tetikler, dosya adını genişletir ve `add_redirection` ile `cmd->redirs` listesine ekler.
- **`TOKEN_HEREDOC` ise**: `ft_redir_heredoc` çağrılır. Bu, delimiter'ı işler ve `add_redirection_with_expansion` ile `cmd->redirs` listesine ekler.

---

## Bölüm 6: Modül 3: Expander (expander*.c)

**Amaç**: Değişkenleri (`$VAR`), özel ifadeleri (`$?`, `$$`) ve tırnakları işleyerek nihai komut argümanlarını oluşturmak.

### Anahtar Fonksiyonlar ve Çalışma Prensibi

#### `char *expand_with_quotes(char *str, t_env *env_list)`:

**Görevi**: Tırnak kurallarını dikkate alarak bir string'i genişleten ana fonksiyondur.

**Adım Adım Çalışması**:

1. **`init_quote_data(&data)`**: Genişletilmiş sonucu biriktirmek için bir `t_quote_data` yapısı oluşturur.

2. **`while (str[i])`**: String üzerinde karakter karakter ilerler.

3. **`process_quote_loop(...)`** döngü içinde çağrılır:
   - Eğer karakter `"` veya `'` ise, `handle_quotes` ile tırnak durumu (`quote_char`) güncellenir. Tırnak karakterleri sonuçtan çıkarılır.
   - Eğer karakter `$` ise ve `quote_char != '\''` (tek tırnak içinde değilsek), `handle_expansion` -> `handle_dollar` çağrılır.
   - Diğer tüm karakterler `push_char_res` ile doğrudan sonuca eklenir.

#### `char *handle_dollar(char *str, int *i, t_env *env_list)`:

**Görevi**: `$` ile başlayan bir ifadenin değerini bulur.

**Adım Adım Çalışması**:

1. **`*i`'yi bir artırarak `$`'ı geçer.

2. **`str[*i]`'nin ne olduğuna bakar**:
   - **`?` veya `$`**: `handle_special_vars` çağrılır, bu da `get_last_exit_status()` veya `getpid()`'nin sonucunu string olarak döner.
   - **Harf veya `_`**: Geçerli bir değişken adının başlangıcı. `extract_var_name` ile değişkenin tam adını (`USER`, `PATH` vb.) okur.

3. **`search_variable`**: Bu isimle önce `env_list`'te (`search_env_variable`), bulunamazsa `export_list`'te (`search_export_variable`) arama yapar.

4. Bulunan değeri (`value`) veya bulunamazsa boş string'i (`""`) döndürür.

---

## Bölüm 7: Modül 4: Executor (execute*.c)

**Amaç**: Hazırlanmış komut listesini çalıştırmak, pipe'ları kurmak, I/O yönlendirmelerini yapmak ve prosesleri yönetmek.

### Anahtar Fonksiyonlar ve Çalışma Prensibi

#### `int execute_cmds(t_parser *cmd_list, ...)`:

**Görevi**: Yürütme sürecini başlatan ve yöneten ana fonksiyondur.

**Adım Adım Çalışması**:

1. **`data_init(...)`**: `t_exec_data` yapısını başlatır. `STDIN` ve `STDOUT`'un `dup()` ile yedeğini alır.

2. **`heredoc_handle(...)`**: **Çok Kritik Adım**. Diğer hiçbir şey yapmadan önce, tüm heredoc'ları okur. `process_heredocs`'u çağırır, o da `read_heredoc_with_expand` veya `read_single_heredoc_block` ile kullanıcıdan girdiyi alır, bir `pipe()`'a yazar ve pipe'ın okuma fd'sini `cmd->heredoc_fd`'ye kaydeder.

3. **`if (*pid_len == 1 && is_builtin(cmd_list))`**: Eğer tek komut varsa ve bu `cd`, `exit` gibi bir built-in ise fork yapmaz. `n_next_or_built`'i çağırır. Bu fonksiyon I/O yönlendirmelerini yapar (`ft_redir_ctrl`), built-in'i çalıştırır (`ft_builtin_call`) ve yedeklenmiş `STDIN`/`STDOUT`'u geri yükler (`ft_in_or_out`).

4. **Diğer Durumlar**: `setup_parent_execution_signals()` ile ana shell'i sinyallere karşı korur. `ft_data_pids` -> `execute_loop`'u çağırarak fork ve pipe sürecini başlatır.

5. **`finish_fd(...)`**: Tüm işlemler bittikten sonra yedeklenmiş `STDIN`/`STDOUT`'u geri yükler.

#### `void child_process_exec(t_parser *cmd, ...)`:

**Görevi**: Bir child process'in yapması gereken tüm kurulumu yapar.

**Adım Adım Çalışması**:

1. **Sinyal Ayarı**: `setup_child_signals` ile sinyalleri varsayılan hale getirir.

2. **Pipe Bağlantıları**: `dup2` kullanarak `data->in_fd`'yi `STDIN_FILENO`'ya, `data->pipefd[1]`'i (eğer varsa) `STDOUT_FILENO`'ya bağlar.

3. **Dosya Yönlendirmeleri**: `ft_redir_ctrl` çağrılır. Bu fonksiyon, komutun `redirs` listesindeki `< file`, `> file` gibi yönlendirmeleri `open()` ve `dup2()` kullanarak uygular. Bu, pipe bağlantılarından sonra çalıştığı için önceliklidir.

4. **Komutu Çalıştır**: `ft_exec_start` çağrılır.

---

## Bölüm 8: Yardımcı Modüller ve Fonksiyonlar

### Sinyal Yönetimi (signal_handler.c, signal_utils.c)

- **`handle_sigint`**: İnteraktif modda Ctrl+C basıldığında tetiklenir. `rl_*` fonksiyonları ile readline'a yeni ve boş bir satır olduğunu bildirir, prompt'u yeniden çizer.

- **`handle_sigint_heredoc`**: Heredoc okunurken Ctrl+C basıldığında tetiklenir. Sadece yeni satır basar ve `$?`'ı 130 yapar, bu da heredoc okuma döngüsünü kırar.

- **`calculate_exit_status`**: `waitpid`'den dönen `status`'u analiz eder. `WIFEXITED` ile normal çıkış kodunu, `WIFSIGNALED` ile sinyal kaynaklı çıkış kodunu (128 + sinyal no) döndürür.

### Built-in Fonksiyonları (builtin_*.c)

- **`built_cd`**: `chdir()` sistem çağrısını kullanır. Başarılı olursa, `getcwd()` ile yeni `PWD`'yi alır ve `setenv()` ile hem `PWD` hem de `OLDPWD` ortam değişkenlerini doğrudan günceller.

- **`builtin_export`**: Argümansız çağrıldığında `export_print_all` ile listeyi basar. `export KEY=VALUE` şeklinde çağrıldığında, `is_valid_identifier` ile `KEY`'i kontrol eder ve hem `set_export_value` hem de `set_env_value` ile her iki listeyi de günceller.

- **`builtin_exit`**: `is_numeric_string` ile argümanın sayısal olup olmadığını kontrol eder. `ft_atoi` ile sayıyı alır ve `exit()` sistem çağrısını bu değerle yapar.

### Environment ve Bellek Yönetimi (env_*.c, gb_*.c)

- **`init_env`**: `main`'den gelen `char **env`'i `ft_strchr` ile `'='` karakterinden bölerek key ve value'yu ayırır ve `t_env` listesini oluşturur.

- **`env_list_to_array`**: `execve`'nin istediği `char *[]` formatına `t_env` listesini dönüştürür. `KEY` ve `VALUE`'yu `'='` ile birleştirerek yeni string'ler oluşturur.

- **`gb_malloc`**: `malloc` için bir "wrapper"dır. `malloc` sonrası dönen adresi `gc_safe` aracılığıyla küresel bir `t_gb` listesine kaydeder.

- **`gb_free_all`**: Program sonunda bu listeyi dolaşarak tüm belleği serbest bırakır ve sızıntıları önler. `env_gb_*` fonksiyonları da aynı mantıkla, ancak sadece ortam değişkenleri için ayrı bir liste üzerinde çalışır.
