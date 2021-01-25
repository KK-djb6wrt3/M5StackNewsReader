# M5Stack News Reader
It gets RSS from Yahoo! news, and shows them on M5Stack.
![Image](view.jpg)
* [ReadMe.ja.md](/ReadMe.ja.md)

# Features
It gets RSS from Yahoo! news, and shows title & descrition with ticker.
And it can display any images, and change it by each topics.
It also shows battery status.

# Requirement
* [efont](https://github.com/tanakamasayuki/efont.git)

# Usage
You must place 'Source.xml' on root of microSD.
You must change 'Preference.xml', and place it on root of microSD.
```
<SSID> #SSID of an access point which you want to connect
<Password> #Password
<File> #File name of backgroun
<Balloon> #Position of balloon LT:Left-Top, RT:Right-Top, LB:Left-Bottom, RB:Right-Bottom
```
It doesn't limit the number of access points, and background. But if it's too much, it'll be insufficient RAM.

* When you push BtnA, it shows the current topic from head.
* When you hold BtnA, it goes to power off.
* When you push BtnB, it goes to menu. It can choose category of news. BtnA,C change the selection, and BtnB decides the category.
* When you push BtnC, it show the next topic.
* When you hold BtnC, it shows a QR code of the link of topic which is displayed.

# License
"M5Stack NewsReader" is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).
