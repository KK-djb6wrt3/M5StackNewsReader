# M5Stack News Reader
It gets RSS from Yahoo! news, and shows them on M5Stack.

# Features
It gets RSS from Yahoo! news, and shows title & descrition with ticker.
And it can display any images, and change it by each topics.
It also shows battery status.

# Requirement
* [efont](https://github.com/tanakamasayuki/efont.git)

# Usage
You must change 'Preference.xml', and place it on root of microSD.
```
<SSID> #SSID of an access point which you want to connect
<Password> #Password
<File> #File name of backgroun
<Balloon> #Position of balloon LT:Left-Top RT:Right-Top, LB:Left-Bottom, RB:Right-Bottom
```
It doesn't limit the number of access points, and background. But if it's too much, it'll be insufficient RAM.

When you pushes BtnA, it shows the current news from head.
