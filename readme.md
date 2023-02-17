# VGM Player v3.0
* DPTP System - VGM Player v3.0 YM2612 + SN76489
* [neo-geo.hu/VGM Player v3.0](http://neo-geo.hu/index.php?modul=news&news=44#nwe)
* STM32F103RF, W25Q64, SD, I2C Oled
* SEGA GAME GEAR, SEGA MESTER SYSTEM and SEGA MEGA DRIVE
* VGM v1.50, v1.51, v1.60
* 44100Hz - 48000Hz megzsakításban kiszolgálva, 22.7uS-20.83uS

![alt text](http://neo-geo.hu/news/don_peter/new44/vgm_v2_0.jpg "VGM Player v3.0")

# Hardver használata
- A programban az 1 gombos kezeléssel a további funkciók érhetőek el:
1. 1mp vagy az alatt tartott gombnyomásra: következő fájlra ugrás (SD kártya esetén, ha van)
2. 1mp-nél nagyobb, de 3mp-nél kevesebb ideig tartott gombnyomásra: 44.1KHz és 48KHz-es váltás
3. 3mp-nél nagyobb de 4mp-nél kevesebb ideig tartott gombnyomásra: 72MHz és 128MHz-es CPU frekvenica váltása
4. 4mp-nél tovább tartott gombnyomás esetén: aktuális fájl feltöltése
- Minden esetben a kijelző első sorában az aktuális menüpont a megadott időintervallumban látható, KHz és MHz esetében az alapértelmezett vagy az előzőleg
beállított érték lesz látható nem az aktuálisan beállítani kívánt érték. Értsd úgy, ha 44.1KHz van beállítva, akkor 48KHz-re vált vagy ennek az ellenkezője.
Ugyan ez elmondható az MHz esetében is, ha 72MHz van beállítva, akkor 128MHz lesz és így fordítva, ha 128MHz, akkor 72MHz lesz.
