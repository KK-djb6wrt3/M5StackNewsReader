# M5Stack News Reader
Yahoo!ニュースをRSSから取得し、M5Stackに表示します。
* [ReadMe.md](ReadMe.md)

# 機能
Yahoo!ニュースの主要トピックを取得し、タイトルと詳細をスクロールして表示します。
また、任意の背景画像を表示することができ、1トピック毎に変更することができます。
また、バッテリー状態も表示します。

# 依存関係
* [efont](https://github.com/tanakamasayuki/efont.git)

# 使用方法
Preference.xmlを変更し、microSDのルートフォルダに置く必要があります。
```
<SSID> #アクセスポイントのSSID
<Password> #パスワード
<File> #背景画像のファイル名
<Balloon> #吹き出しの位置。LT:左上 RT:右上, LB:左下, RB:右下
```
アクセスポイントや背景画像の数に制限は設けていませんが、あまり多いとメモリが足りなくなる可能性があります。

BtnAを押すと現在表示しているニュースを先頭から表示します。

# ライセンス
M5Stack News Readerは[MITライセンス](https://en.wikipedia.org/wiki/MIT_License)で配布されています
