# BOO-OTA
Bootloader program enabling OTA-via-BT software upgrade feature for FRDM-KL46Z board
# Opis w języku polskim (zdecydowanie bardziej rzeczowy i przejrzystszy niż ten przedstawiony na filmie, zachęcam do lektury)
Tematem mojego projektu jest program bootloader'a, umożliwiający uaktualnienie oprogramowania na mikrokontrolerze bez konieczności podłączania programatora, a zatem umożliwiający tzw. konfigurację IAP (in-application-programming). Może się to odbywać drogą bezprzewodową, np. z wykorzystaniem popularnego modułu HC-05/06 (stąd nazwa, Boot Over The Air). Niestety, problem z moją stacją roboczą uniemożliwił mi przedstawienie tego w taki sposób, natomiast połąćzenie z urządzeniem wysyłającym nasze oprogramowanie odbywa się poprzez protokół UART, zatem wystarczy zrealizować same połączenie bezprzewodowe właśnie z wykorzystaniem tego protokołu. Początkowo, plan zakładał zapisywanie nowej wersji programu docelowego w pamięci nieulotnej (tutaj Flash), natomiast moduły KL05Z jak i KL46Z nie umożliwiają sprzętowo opcji pisania do pamięci FLASH z poziomu aplikacji (skutkuje to błędem szyny, tzw. Bus Error). Skutkowało to zmianą układu pierwszego na drugi, posiadający fizycznie większą ilość pamięci RAM, potrzebnej do pomieszczenia dodatkowo kodu wykonywalnego programu (instrukcji, tablicy wektorów przerwania). Plusem takiego rozwiązania jest większa prędkość wykonywania kodu programu, natomiast wadą jest utrata konfiguracji w momencie deficytu zasilania. Bootloader został napisany, idąc za angielskim słownictwem quasi-technicznym, "from scratch" (zaczynając od pustej kartki). Jedyne pliki, które zostały wygenerowane przez IDE a dołączone do projektu to kilka plików (wrapper'ów) umożliwiających dostęp do rejestrów po swojej nazwie, jeden plik konfigurujący zegar oraz jeden opisujący kolejność wektorów przerwań. Nie użyto programu "startowego" (zawierającego Reset_Handler()), pliku crt0 (C runtime zero) oraz automatycznie wygenerowanego skryptu Linkera. Ich funkcjonalność, jak i cały program bootloader'a, zrealizowano samodzielnie, w celu stworzenia programu o jak najmniejszej ilości zajmowanej w pamięci jak i możliwości dogodnej alokacji poszczególnych segmentów pamięci (zmiana rozmiaru, ilosć poszczególnych sekcji). Program bootloader'a musi być minimalnie skomplikowany będąc jednocześnie maksymalnie stabilnym. Z tego powodu do jego implementacji nie użyto mechanizmu przerwań jak i trybów energoosczędnych mikroprocesora. Pętla główna programu oczekuje na nadchodzący plik binarny, zapisując (po otrzymaniu) kolejne bajty pliku w oddzielnej przestrzeni RAM_USERSPACE przechowującej kod programu (userspace_map.ld oraz mem.ld). Po zakończeniu transmisji następuje deinicjalizacja użytych peryferiów (celem uniknięcia problemu z ich konfiguracją w aplikacji docelowej) oraz "skok" do programu docelowego (z użyciem wstawski assemblerowej, ładującej nową wartość program counter'a jak i inicjalizująca stack pointer). Program docelowy musi zostać skompilowany z użyciem odpowiednio zmodyfikowanego Reset_Handler'a, dodatkowo wyłączającego dostęp do pamięci FLASH (celem ograniczenia zużycia energii) jak i realokującego tablicę wektorów przerwań w inne miejsce w pamięci (rejestr określający offset VTOR_REG, dostępny w Cortex'ach serii M0+). Skompilowany wcześniej plik należy zalinkować z odpowiednim skryptem linkera. Tak przygotowany plik binarny można z powodzeniem wysłać na mikrokontroler z działającym bootloader'em. Bootloader jest w pełni przezroczysty, tj. nie ingeruje w działanie aplikacji docelowej, co właściwie pozwala na brak konieczności brania go pod uwagę w trakcie projektowania aplikacji docelowej (oprócz obsługi żądania soft-resetu). Pozwala to zatem na zaprojektowanie aplikacji o dowolnej funkcjonalności (czego nie udało mi się z powodzeniem zaprezentować a ma największą wartość dla potencjalnego użytkownika). Segment pamięci RAM_SHARED został wydzielony dla testów, nie udało się go jeszcze w pełni wykorzystać.
# Struktura plików
main.c - program bootloader'a, implementujący Reset_Handler() oraz funkcjonalność crt0               
app.c - program docelowy (użytkownika), implementujący odpowednio zmodyfikowany Reset_Handler()      
mem.ld - mapa pamięci, alokacja poszczególnych segmentów w pamięci                                   
bootloader_mem_map.ld - skrypt linker'a dla programu bootloader'a                                    
userspace_map.ld - skrypt linker'a dla programu użytkownika                                         
bootloader_mem_map.h - plik nagłówkowy, zawierający deklaracje odpowiednich symboli linker'a         
MKL46Z4.h - deklaracje rejestrów\
system_MKL46Z4.* - konfiguracja zegara\
core_... - konfiguracja mikroprocesora\
MKL46Z4_Vectors.s - układ wektorów w pamięci


# Toolchain:
Compiler: armclang\
Linker: GNU\
C Language Standard: c99\
IDE: SEGGER Embedded Studio for ARM v5.30

# Coding guidelines and style:
https://www.kernel.org/doc/html/v4.10/process/coding-style.html
