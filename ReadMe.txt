[Spoofer v0.7]

[プラグインについて]
{
	Total_Noob氏の[DayViewerプラグイン]とDaave氏の[Davee's Unlimited Character
		Version and Mac Spoofer v3プラグイン]の機能を統合したものです。

	[DayViewer]の機能は、XMBの日付・時計の文字列を自由なフォーマットで表示させることができます。
	※ 最大で255文字まで表示することができます。

	[Davee's Unlimited Character Version and Mac Spoofer v3]の機能は、PSPの本体情報の
		[システムソフトウェア]と[MACアドレス]の表示を変更することができます。
}

[インストール]
{
	MSにsepluginsフォルダをコピーしてプラグインを有効にしてください。
}

[設定ファイルについて]
{
	・設定ファイルのパス
	{
		設定ファイルはプラグインと同じフォルダに配置してください。
	}

	・設定ファイルの文字コードについて
	{
		2Byte文字を使う場合は設定ファイルの形式をUnicodeに変更してください。
	}

	・フォーマット文字一覧
	{
		%weekday		spoofer_config.txtに記述した曜日を表示します。
		%monthname		spoofer_config.txtに記述した月を表示します。
		%year			現在の年を表示します。
		%month			現在の月を表示します。
		%day			現在の日を表示します。
		%hour			現在の時間を24時間表示で表示します。
		%hour12			現在の時間を12時間表示で表示します。
		%min			現在の分を表示します。
		%sec			現在の秒を表示します。
		%ampm			午前だと"AM"、午後だと"PM"と表示します。
		%batpercent		バッテリーが残り何%なのかを表示します。
		%batlifehour	バッテリーが残り何時間なのかを表示します。
		%batlifemin		バッテリーが残り何分なのかを表示します。
	}

	・[FormatString]について
	{
		XMBの右上にある日付・時計の表示文字を設定できます。

		※ '\'で改行することができます。
		※ 最大で255文字ほど表示できます。
	}

	・[WeekNames]と[MonthNames]について
	{
		[WeekNames]の項目では自由に曜日名を設定できます。
		[MonthNames]の項目では自由に月名を設定できます。
	}

	・[VersionSpoof]と[MacAddressSpoof]について
	{
		[VersionSpoof]の項目では[システムソフトウェア]で表示する文字列を設定することができます。
		[MacAddressSpoof]の項目では[MACアドレス]で表示する文字列を設定することができます。

		※ フォーマット文字を使用できるようにしました。
		※ (null)としておくと機能を無効化できます。
	}
}

[更新内容]
{
	・v0.7.1
	{
		%monthnameのバグを修正しました
	}
	・v0.7
	{
		PSP-GOでバッテリーの残り時間が正しく取得できなかった場合は"--"と表示するようにしました
		PSP-GOでVSHMENUを表示している時だけ動作を停止するようにして、VSHMENUがちらつくバグを修正しました
		[VersionSpoof],[MacAddressSpoof]の項目でもフォーマット文字を使用できるようにしました
	}
	・v0.6
	{
		DayViewer機能を改善したことによって動作が非常に軽くなった
		設定ファイルの仕様を変更した
		ソースコードをすべて書きなおした
	}
	・v0.5fix
	{
		プラグインが動作しなかったバグを修正
		dayviewer_config.txtがなかった場合にクラッシュしていたバグを修正
	}
	・v0.5
	{
		DayViewer機能の動作の改善
	}
	・v0.4
	{
		曜日がずれていたバグを修正
		曜日 & 日付が固定されていたバグを修正
		ソースコードをすべて書き直した
	}
	・v0.3fix
	{
		PSPがフリーズしやすくなっていた不具合を修正
	}
	・v0.3
	{
		DayViewer部分のパッチを変更し、最大表示文字数を100->1024に変更
		他のプラグインとの互換性を調整
	}
	・v0.2fix
	{
		DayViewerの表示部分のバグを修正
	}
	・v0.2
	{
		DayViewer機能の追加
	}
	・v0.1
	{
		公開 & ソースコード公開
	}
}

[対応FW]
{
	CFW 5.00 - CFW 6.60
}

[Credits]
{
	plum
}

