# VGM Player v3.0
* DPTP System - VGM Player v3.0 YM2612 + SN76489
* [neo-geo.hu/VGM Player v3.0](http://neo-geo.hu/index.php?modul=news&news=45#nwe)
* STM32F103RF, W25Q64, SD, I2C Oled
* SEGA GAME GEAR, SEGA MESTER SYSTEM and SEGA MEGA DRIVE
* VGM v1.50, v1.51, v1.60
* 44100Hz - 48000Hz megzsakításban kiszolgálva, 22.7uS-20.83uS

![VGM Player v3.0](http://neo-geo.hu/news/don_peter/new45/vgm_v300_3.jpg "VGM Player v3.0")

![Listen to authentic music.](https://youtu.be/WJZeTfQz4C4)

# Mi is ez az eszköz?
* Az eszköz képes a feltöltött SEGA VGM zene fájlokat fogadni USB-n vagy SD (FAT32) kártyáról, majd azt feltölteni a saját W25Q64-es Flash memóriájára
majd onnan közvetlen feldolgozni és lejátszani azt. Az eszköz redelkezik egy erősítő áramkörrel, amely elegendő fülhallgatókhoz, vagy külön erősítő meghajtására.
Külön Oled kijelző gondoskodik az aktuális állapotot kiírásáról, illetve a YM2612 és PSG bináris és vezérlő bitek állapotát LED-es fényjelzések segítségével
 olvashatjuk le.

# Eszköz használata
- A programban az 1 gombos kezeléssel a további funkciók érhetőek el:
- - 1. 1mp vagy az alatt tartott gombnyomásra: következő fájlra ugrás (SD kártya esetén, ha van)
- - 2. 1mp-nél nagyobb, de 3mp-nél kevesebb ideig tartott gombnyomásra: 44.1KHz és 48KHz-es váltás
- - 3. 3mp-nél nagyobb de 4mp-nél kevesebb ideig tartott gombnyomásra: 72MHz és 128MHz-es CPU frekvenica váltása
- - 4. 4mp-nél tovább tartott gombnyomás esetén: aktuális fájl feltöltése
- Minden esetben a kijelző első sorában az aktuális menüpont a megadott időintervallumban látható, KHz és MHz esetében az alapértelmezett vagy az előzőleg
beállított érték lesz látható nem az aktuálisan beállítani kívánt érték. Értsd úgy, ha 44.1KHz van beállítva, akkor 48KHz-re vált vagy ennek az ellenkezője.
Ugyan ez elmondható az MHz esetében is, ha 72MHz van beállítva, akkor 128MHz lesz és így fordítva, ha 128MHz, akkor 72MHz lesz.

# Kompatibilitás
* A program kezeli az 1.50, 1.51-es és az 1.60-os SEGA MD VGM zenéket (DAC STREAM még nem teljes) és kezeli a PSG vagy is a SEGA MS és SEGA GG zenéket is.

# VGM fájlok feltöltése az eszközre
* Töltsd le a vgm_sent_release_STM32_2023_02_17.zip fájlt, majd tömörítsd ki
- a programmal azonos mappába másolj be pár VGM fájlt, amely a felsorólt konzolokra készült
- ha nem a program könyvtárába másolod be a VGM fájlokat, akkor meg kell adni a teljes utvonalat

	`vgm_sent COM? filename.vgm`
	
- a fentebb látható módon índítható a program, a vgm_sent maga az alkalmazás, nem kötelező a kiterjesztést megadni
- COM? - a kérdőjel helyére a windows álltal az eszközhöz rendelt csatorna száma kerül, pl.: COM7
- Az alkalmazás Win64 operációs rendszeren volt fejlesztve .NET 4.5-ös keretrendszerrel.

# Fejlesztésre várnak
* A program nincs mindere felkészítve, amelyeket később, ha időm is engedi javítok, ilyen hibák lehetnek a következők:
- - nincs kezelve, ha nincs SD kártya behelyezve és léptni akarjuk a fájlokat
- - nincs kész a DAC STREAM rész, 0x93-as parancs kidolgozásának befejezése, ameddig ez a rész nincs kész, addig nem képes az 1.60-os VGM zenéket jól kezelni.

# Hiba javítások listálya
- még nincs ilyen