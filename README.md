# M5StackNewsReader
It displays Yahoo! news from RSS on M5Stack.
Yahoo!ニュースをRSSから取得し、M5Stackに表示します。

It depends on efont(https://github.com/tanakamasayuki/efont.git).
You must to import it to use this project.
efont(https://github.com/tanakamasayuki/efont.git)に依存しているので、ビルド前にインストールしておいてください。

To execute this proect, you need to modify & place Preference.xml into root of microSD.
1) Write your SSID of access point to <SSID> area.
2) Write your password of step1 to <Password> area.
3) Put your background image to microSD, and describe it to <File> aera. And the <Balloon> area indicates the position of balloon to show news text. LT:Left-Top, RT:Right-Top, LB:Left-Bottom, RB:Right-Bottom.
実行するにはPreference.xmlを変更してmicroSDのルートフォルダに置く必要があります。
1) アクセスポイントのSSIDを<SSID>欄に記述する。
2) ステップ1のアクセスポイントのパスワードを<Password>欄に記述する。
3) 背景画像をmicroSDに置き、<File>欄に記述する。<Balloon>欄は吹き出しの位置を指定する。LT:左上 RT:右上, LB:左下, RB:右下
