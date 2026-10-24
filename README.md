# Yokai Watch 2 Tools (妖怪ウォッチ2 真打 用 CTRPF プラグイン)

3DS の Luma3DS で使う、妖怪ウォッチ2 真打 用の個人プラグインです。
[CTRPluginFramework BlankTemplate](https://github.com/PabloMK7/CTRPluginFramework-BlankTemplate) をもとに作っています。

## できること
- 妖怪・アイテム・技などの編集メニュー(オフライン用)
- 個体値・レベルなどの一括変更

他のプレイヤーに影響するオンライン向けの機能は入れていません。

## ビルド
詳しい手順は [README_ビルド手順.md](README_ビルド手順.md) を見てください。

```sh
./build.sh
```

Docker (`pablomk7/libctrpf:0.8.0`) と、ホストの `3gxtool` が必要です。

## 注意
- 非公式の個人プラグインです。ゲームや任天堂とは関係ありません。
- 使うときは必ずセーブデータのバックアップを取ってください。
- 自己責任で、オフラインの自分のセーブだけで使ってください。
- `.3gx` / `.elf` などのビルド生成物と、他人製のバイナリはリポジトリに含めていません。
