これは、https：//www.sigbus.info/compilerbookのリファレンス実装です。


関数とローカル変数　の直前まで出来た。

consume_ident()が無いことでハマった。
consume_ident()をgithub rui314より拝借

ステップ10の直前まで来た。

program()関数を実行すると結果がおかしくなる。

tokenize()は文字列の配列を返すべきではなかろうか?

chibicc の セミコロンで区切られた複数のステートメントを受け入れる
を参照し、同様の挙動を得ることが出来た。


2020-4-16


「return」ステートメントを追加

2020-4-17

式ステートメントの概念を追加する
1文字のローカル変数をサポート
複数文字のローカル変数をサポート
「if」ステートメントを追加
「while」ステートメントを追加
「for」ステートメント追加
追加 { ... }
		    
2020-4-19
