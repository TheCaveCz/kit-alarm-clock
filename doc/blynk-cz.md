# Nastavení

## Aplikace Blynk

* Stáhněte a nainstalujte si aplikaci Blynk (https://blynk.io/en/getting-started)
* Vytvořte si v aplikaci Blynk nový účet/přihlašte se pokud už účet máte
* Vytvořte v aplikaci nový projekt (New project), device je `Wemos D1 mini`, connection type je `WiFi`
* Blynk odešle na mail zadaný při registraci **Auth token**, ten si zkopírujte nebo opište, budeme jej potřebovat v dalším kroku

<img src="pictures/setup-blynkproject.png" width=320>

## Připojení hodin k WiFi

Na displeji hodin musí svítit **AP** a je potřeba mít Blynk **Auth token**. Připojte se na WiFi `alarm-clock-xxxxxx` (xxxxxx je číslo hodin, může se měnit).

<img src="pictures/setup-wifi.png" width=320>

Objeví se nastavovací obrazovka (to může chvíli trvat, na některých Android telefonech je potřeba ji ručně otevřít). Pokud se obrazovka neobjeví, zadejte do prohlížeče adresu http://192.168.4.1/, poté zvolte možnost Configure WiFi a počkejte, dokud se neobjeví výběr WiFi sítí.

<img src="pictures/setup-wifiscreen.png" width=320>

Vyberte vaši WiFi síť a do prostředního políčka napište její heslo. Do posledního políčka vložte Auth token a stiskněte tlačítko `Save`

<img src="pictures/setup-wifidetail.png" width=320>

Po připojení k WiFi na hodinách svítí nuly.

## Nastavení projektu Blynk

Otevřete aplikaci Blynk a zvolte projekt budíku. Obrazovka by měla vypadat podobně.

<img src="pictures/setup-p-empty.png" width=320>

V aplikaci Blynk otevřete projekt a přidejte první kompnentu (na Androidu klepnutím na plus v kolečku, na iOS swipem doleva).

<img src="pictures/setup-p-picker.png" width=320>

První komponenta, kterou přidáme je **Real-time Clock**. Klepněte na ikonu Play v pravém horním rohu a restartujte hodiny (stiskem tlačítka reset nebo odpojením a připojením kabelu). Pokud vše proběhlo v pořádku, na displeji je aktuální čas.

<img src="pictures/setup-p-running.png" width=320>

Klepněte na ikonu Stop a přidejte další komponenty: **Time Input**, .**Button** a **Notification**. Klepnutím na každou komponentu Time Input otevřete její nastavení a nastavte ji podle následujícího obrázku:

<img src="pictures/setup-p-timeinput1.png" width=320>
<img src="pictures/setup-p-timeinput2.png" width=320>

Další komponenta, kterou nastavíme je Button:

<img src="pictures/setup-p-button.png" width=320>

Další komponenta je Notification:

<img src="pictures/setup-p-notification.png" width=320>



