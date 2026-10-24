#include "3ds.h"
#include "csvc.h"
#include "CTRPluginFramework.hpp"
#include "cheats.hpp"

#include <string>
#include <cstdlib>

namespace CTRPluginFramework
{
    // NOTE: テンプレート付属の NFC 用タッチスクリーンパッチ (ToggleTouchscreenForceOn) は
    //       static std::vector のデストラクタがプロセス終了時にクラッシュを起こしたため削除済み。
    //       amiibo を使う場合は元テンプレートを参照して std::vector を固定長配列に置き換えて復活させること。

    // This function is called before main and before the game starts
    // Useful to do code edits safely
    void    PatchProcess(FwkSettings &settings)
    {
    }

    // This function is called when the process exits
    // Useful to save settings, undo patchs or clean up things
    void    OnProcessExit(void)
    {
    }

    // ------------------------------------------------------------
    // お金を任意の値に設定 (キーボード入力)
    //   元祖/真打: 0x08722BF8   本家: 0x08724BF8   (32bit, 最大 999999)
    //   真打は実機で動作確認済み (2026/09/15)
    // ------------------------------------------------------------
    static const u32 MONEY_ADDRESS_BASE = 0x08722BF8;

    static void SetMoneyCallback(MenuEntry *entry)
    {
        u32 address = VersionAddress(MONEY_ADDRESS_BASE);

        if (address == 0)
        {
            MessageBox("Yokai Watch 2 Tools", "このタイトルIDには対応していません。")();
            return;
        }

        u32 currentValue = 0;
        if (!Process::Read32(address, currentValue))
        {
            MessageBox("Yokai Watch 2 Tools", "現在の値を読み込めませんでした。\nアドレスが間違っている可能性があります。")();
            return;
        }

        Keyboard keyboard("設定したいお金の金額を入力してください (0〜999999)\n現在の値: " + std::to_string(currentValue));

        std::string input;
        int ret = keyboard.Open(input, std::to_string(currentValue));

        if (ret != 0)
            return; // キャンセル、または閉じられた

        // 例外を使わず strtol で安全にパース (-fno-exceptions でビルドされるため)
        const char *cstr = input.c_str();
        char *endptr = nullptr;
        long value = strtol(cstr, &endptr, 10);

        if (endptr == cstr)
        {
            MessageBox("Yokai Watch 2 Tools", "数値として認識できませんでした。")();
            return;
        }

        if (value < 0)
            value = 0;
        if (value > 999999)
            value = 999999;

        if (!Process::Write32(address, (u32)value))
        {
            MessageBox("Yokai Watch 2 Tools", "書き込みに失敗しました。")();
            return;
        }

        MessageBox("Yokai Watch 2 Tools", "お金を " + std::to_string(value) + " に設定しました。")();
    }

    // ------------------------------------------------------------
    // 次のバトルの敵を変更
    //   元祖/真打: 0x08722CF0   本家: +0x2000   (ユーザー提供の AR コードより。要検証)
    // ------------------------------------------------------------
    static const u32 ENEMY_ADDRESS_BASE = 0x08722CF0;

    // テンプレート関数: Value を敵アドレスに書き込むコールバックを敵の数だけ生成する
    template <u32 Value>
    static void SetEnemyGeneric(MenuEntry *entry)
    {
        u32 address = VersionAddress(ENEMY_ADDRESS_BASE);
        if (address == 0)
        {
            MessageBox("Yokai Watch 2 Tools", "このタイトルIDには対応していません。")();
            return;
        }

        if (Process::Write32(address, Value))
            MessageBox("Yokai Watch 2 Tools", "次のバトルの敵を変更しました。")();
        else
            MessageBox("Yokai Watch 2 Tools", "書き込みに失敗しました。")();
    }

    static MenuFolder *CreateEnemyFolder(void)
    {
        MenuFolder *folder = new MenuFolder("バトル時の敵変更", "選んだ妖怪が次のバトルの相手になります。");

        *folder += new MenuEntry("あやとり様",         nullptr, SetEnemyGeneric<0xDC33C7C1>);
        *folder += new MenuEntry("なまはげ (69レベ)",   nullptr, SetEnemyGeneric<0x4DF80DFF>);
        *folder += new MenuEntry("なまはげ (85レベ)",   nullptr, SetEnemyGeneric<0x1A60C04E>);
        *folder += new MenuEntry("ツチノコパンダ",       nullptr, SetEnemyGeneric<0x713FE778>);
        *folder += new MenuEntry("赤鬼",               nullptr, SetEnemyGeneric<0x02B99B38>);
        *folder += new MenuEntry("ブリー隊長",          nullptr, SetEnemyGeneric<0x63775BAF>);
        *folder += new MenuEntry("ジェットニャン",       nullptr, SetEnemyGeneric<0xEDBCE582>);
        *folder += new MenuEntry("黒鬼",               nullptr, SetEnemyGeneric<0x62B1B06C>);
        *folder += new MenuEntry("(名前不明 CDEF7AFC)",   nullptr, SetEnemyGeneric<0xCDEF7AFC>);

        return folder;
    }

    void    InitMenu(PluginMenu &menu)
    {
        // ---- お金 ----
        MenuFolder *money = new MenuFolder("お金");
        *money += new MenuEntry("お金を設定する (入力)",  nullptr, SetMoneyCallback);
        *money += new MenuEntry("お金を 999999 にする",   nullptr, MaxMoneyCallback);
        menu += money;

        // ---- アイテム / 妖怪 (cheats.cpp) ----
        menu += CreateItemFolder();
        menu += CreateYokaiFolder();

        // ---- バトル ----
        menu += CreateEnemyFolder();

        // ---- 真打専用 (cheats_wiki.cpp) ----
        menu += CreateYokaiExtFolder();
        menu += CreateSkillFolder();
        menu += CreateWarpFolder();
        menu += CreateModelFolder();
        menu += CreatePatchFolder();
        menu += CreateMiscFolder();
        menu += CreateIdFolder();
        menu += CreatePosFolder();
        menu += CreateEditorFolder();
        menu += CreateMoveFolder();
        menu += CreateBattleFolder();
        menu += CreateEquipFolder();
        menu += CreateMore2Folder();
        menu += CreateBulkFolder();
    }

    int     main(void)
    {
        PluginMenu *menu = new PluginMenu("Yokai Watch 2 Tools", 0, 3, 0,
                                            "Yokai Watch 2 (Ganso/Honke/Shinuchi) 向けカスタムプラグイン。\nAction Replayや各種ツールにアクセスできます。");

        // Synchronize the menu with frame event
        menu->SynchronizeWithFrame(true);

        // Init our menu entries & folders
        InitMenu(*menu);

        // 毎フレーム: チェックを外したコードパッチを元に戻す監視
        menu->Callback(CheatsFrameCallback);

        // Launch menu and mainloop
        menu->Run();

        // NOTE: 以前はここで delete menu; していたが、ゲーム終了時に
        //       ヒープ解放 (_free_r) で data abort が起きるため、
        //       終了時の解放は OS に任せる (プロセス終了で全て回収される)。

        // Exit plugin
        return (0);
    }
}
