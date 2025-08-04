/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 00:00:00 by mertilhan13       #+#    #+#             */
/*   Updated: 2025/08/04 17:02:04 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <signal.h>

volatile sig_atomic_t g_signal_received = 0;

void handle_sigint(int signum)
{
    (void)signum;
    g_signal_received = SIGINT;
    
    printf("\n");
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}
void    handle_sigint_heredoc(int signum)
{
    (void)signum;
    g_signal_received = SIGINT;
    printf("\n");
    exit(130);
}
void    heredoc_signals(void)
{
    struct sigaction sa_int;
    struct sigaction sa_quit;
    
    //sa_int.sa_handler = heredoc_signals;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);
    
    sa_quit.sa_handler = SIG_IGN;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit,NULL);
}
void setup_interactive_signals(void)
{
    struct sigaction sa_int;
    struct sigaction sa_quit;
    
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);
    
    sa_quit.sa_handler = SIG_IGN;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);
}
void setup_parent_execution_signals(void)
{
    struct sigaction sa_int, sa_quit;
    
    sa_int.sa_handler = SIG_IGN;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);
    
    sa_quit.sa_handler = SIG_IGN;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);
}
void setup_child_signals(void)
{
    signal(SIGINT, SIG_DFL); 
    signal(SIGQUIT, SIG_DFL); 
}

void check_and_handle_signal(int *last_exit_status)
{
    if(g_signal_received == SIGINT)
    {
        g_signal_received = 0;
        *last_exit_status = 130; 
    }
}
int calculate_exit_status(int status)
{
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
    {
        int sig = WTERMSIG(status);
        if (sig == SIGINT)
            return (130);
        else if (sig == SIGQUIT)
            return (131);
        else
            return (128 + sig);
    }
    return (1);
}

// Etkileşimli Shell'de:

// SIGTERM'i tamamen yok sayar (kill 0 ile kapanmaz)
// SIGINT gelince (Ctrl+C) çalışan döngülerden çıkar ve wait komutunu keser
// SIGQUIT'i her durumda yok sayar
// Job control varsa SIGTTIN, SIGTTOU, SIGTSTP'yi de yok sayar

// Alt Süreç Sinyal Yönetimi:

// Dış komutlar parent'tan miras aldığı sinyal handler'ları kullanır
// trap ile ignore edilmişse alt süreç de ignore eder
// Job control yoksa asenkron komutlar SIGINT/SIGQUIT'i ignore eder
// Command substitution'da çalışan komutlar keyboard job control sinyallerini(SIGTTIN SIGTTOU SIGTSTP(CTRL + Z)) ignore eder

// SIGHUP İşleme Süreci:

// SIGHUP alınca shell kapanmaya hazırlanır
// Tüm running/stopped job'lara SIGHUP gönderir
// Stopped job'lara önce SIGCONT gönderir (SIGHUP'ı alsınlar diye)
// disown komutuyla job'lar bu süreçten çıkarılabilir

// Komut Bekleme Sırasında Sinyal İşleme:

// Komut çalışırken trap'li sinyal gelirse trap bekler, komut bitince çalışır
// wait builtin'de trap'li sinyal gelirse hemen exit status >128 ile döner, sonra trap çalışır

// Process Group Yönetimi:

// Job control kapalı: Shell ve komut aynı group'ta → Ctrl+C ikisine de gider
// Job control açık: Komut ayrı group'ta → Sadece komut alır sinyali

// SIGINT Karar Verme Süreci (Job control kapalı):

// Komut SIGINT'ten öldüyse → Shell de SIGINT işler (trap çalıştır/çık/yeni komut bekle)
// Komut SIGINT'i handle ettiyse → Shell fatal kabul etmez, sadece trap çalıştırır