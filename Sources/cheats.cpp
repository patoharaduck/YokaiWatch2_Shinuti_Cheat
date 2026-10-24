#include "cheats.hpp"

// ============================================================
//  妖怪ウォッチ2 チート実装
//
//  アドレスの出典: JourneyOver/CTRPF-AR-CHEAT-CODES
//    Cheats/Yo-Kai Watch 2 - Ganso (JPN)/000400000012F900.txt
//    Cheats/Yo-Kai Watch 2 - Honke (JPN)/000400000012F800.txt
//  ・元祖と本家は全アドレスがちょうど +0x2000 ずれているだけ
//  ・真打は「お金」のアドレスが元祖と同一なのを実機で確認済み
//    → 他のアドレスも元祖と同じ配置と推定 (要検証)
// ============================================================

namespace CTRPluginFramework
{
    static const char *TITLE = "Yokai Watch 2 Tools";

    // ---------------- バージョン差分 ----------------
    bool    GetVersionOffset(u32 &offset)
    {
        u32 lowTitleID = (u32)(Process::GetTitleID() & 0xFFFFFFFFULL);

        switch (lowTitleID)
        {
            case 0x0012F900: // 元祖
            case 0x00155100: // 真打
                offset = 0;
                return true;
            case 0x0012F800: // 本家
                offset = 0x2000;
                return true;
            default:
                return false;
        }
    }

    u32     VersionAddress(u32 baseAddress)
    {
        u32 offset = 0;
        if (!GetVersionOffset(offset))
            return 0;
        return baseAddress + offset;
    }

    static void ShowUnsupported(void)
    {
        MessageBox(TITLE, "このタイトルIDには対応していません。")();
    }

    // ---------------- お金 ----------------
    static const u32 MONEY_BASE = 0x08722BF8;   // 32bit, 最大 999999 (0x000F423F)

    void    MaxMoneyCallback(MenuEntry *entry)
    {
        u32 address = VersionAddress(MONEY_BASE);
        if (address == 0) { ShowUnsupported(); return; }

        if (Process::Write32(address, 999999))
            MessageBox(TITLE, "お金を 999999 にしました。")();
        else
            MessageBox(TITLE, "書き込みに失敗しました。")();
    }

    // ---------------- アイテム ----------------
    // 真打 (wiki 掲載コード):
    //   C0000000 000001AE / 2872366C 00000063 / DC000000 00000010  → 0x08723668 + n*0x10 の +4 が所持数
    //   C0000000 0000005A / 287251D4 00000063 / DC000000 00000014  → 第2テーブル
    // 元祖 (JourneyOver): 28723668 (所持数 +0, 0xFE 個) と配置が異なるため真打専用にする
    static const u32 ITEM_BASE    = 0x08723668;
    static const u32 ITEM_CNT_OFF = 0x04;
    static const u32 ITEM_STRIDE  = 0x10;
    static const u32 ITEM_COUNT   = 0x1AE;
    static const u32 ITEM2_BASE   = 0x087251D4;
    static const u32 ITEM2_STRIDE = 0x14;
    static const u32 ITEM2_COUNT  = 0x5A;

    void    AllItemsX99Callback(MenuEntry *entry)
    {
        u32 low = (u32)(Process::GetTitleID() & 0xFFFFFFFFULL);
        if (low != 0x00155100) { MessageBox(TITLE, "この機能は「真打」専用です。")(); return; }

        u32 changed = 0;
        for (u32 i = 0; i < ITEM_COUNT; ++i)
        {
            u32 addr = ITEM_BASE + i * ITEM_STRIDE + ITEM_CNT_OFF;
            u8  cur  = 0;
            // 空きスロット(所持数0)には書かない = 元ARコードより安全側
            if (!Process::Read8(addr, cur) || cur == 0)
                continue;
            if (Process::Write8(addr, 99))
                ++changed;
        }
        for (u32 i = 0; i < ITEM2_COUNT; ++i)
        {
            u32 addr = ITEM2_BASE + i * ITEM2_STRIDE;
            u8  cur  = 0;
            if (!Process::Read8(addr, cur) || cur == 0)
                continue;
            if (Process::Write8(addr, 99))
                ++changed;
        }

        MessageBox(TITLE, "所持中のアイテム " + std::to_string(changed) + " 種を 99 個にしました。")();
    }

    // ---------------- 妖怪 ----------------
    // ARコード群 (元祖基準):
    //   HP        1870DBEC / 1870DC14 = 0x03E7 (16bit)
    //   技レベル  2870DBDE / 2870DBE2 / 2870DBE6 = 0x0A (8bit)
    //   ちから    1870DC16 = 0x03E7 (16bit)
    //   ようりょく1870DC18
    //   まもり    1870DC1A
    //   すばやさ  1870DC1C
    //   1体あたり 0xC4 バイト、先頭 254 体
    static const u32 YOKAI_BASE    = 0x0870DBDE;   // 技レベル1 (構造体内で最初に使う項目)
    static const u32 YOKAI_STRIDE  = 0xC4;
    static const u32 YOKAI_COUNT   = 0xFE;

    static const u32 OFF_TECH1   = 0x0870DBDE - YOKAI_BASE;
    static const u32 OFF_TECH2   = 0x0870DBE2 - YOKAI_BASE;
    static const u32 OFF_TECH3   = 0x0870DBE6 - YOKAI_BASE;
    static const u32 OFF_HP_CUR  = 0x0870DBEC - YOKAI_BASE;
    static const u32 OFF_HP_MAX  = 0x0870DC14 - YOKAI_BASE;
    static const u32 OFF_STR     = 0x0870DC16 - YOKAI_BASE;
    static const u32 OFF_SPR     = 0x0870DC18 - YOKAI_BASE;
    static const u32 OFF_DEF     = 0x0870DC1A - YOKAI_BASE;
    static const u32 OFF_SPD     = 0x0870DC1C - YOKAI_BASE;

    // 妖怪スロットが使用中か (最大HPが 0 なら空きとみなす)
    static bool IsYokaiSlotUsed(u32 slotBase)
    {
        u16 hpMax = 0;
        return Process::Read16(slotBase + OFF_HP_MAX, hpMax) && hpMax != 0;
    }

    // 妖怪全員に対して 16bit 値を書き込む共通処理
    static void ForEachYokaiWrite16(const u32 *offsets, u32 offsetCount, u16 value, const std::string &what)
    {
        u32 base = VersionAddress(YOKAI_BASE);
        if (base == 0) { ShowUnsupported(); return; }

        u32 changed = 0;
        for (u32 i = 0; i < YOKAI_COUNT; ++i)
        {
            u32 slot = base + i * YOKAI_STRIDE;
            if (!IsYokaiSlotUsed(slot))
                continue;

            for (u32 k = 0; k < offsetCount; ++k)
                Process::Write16(slot + offsets[k], value);
            ++changed;
        }

        MessageBox(TITLE, what + " (" + std::to_string(changed) + " 体)")();
    }

    void    YokaiMaxHPCallback(MenuEntry *entry)
    {
        static const u32 offs[] = { OFF_HP_CUR, OFF_HP_MAX };
        ForEachYokaiWrite16(offs, 2, 999, "妖怪のHPを 999 にしました。");
    }

    void    YokaiMaxAllStatsCallback(MenuEntry *entry)
    {
        static const u32 offs[] = { OFF_STR, OFF_SPR, OFF_DEF, OFF_SPD };
        ForEachYokaiWrite16(offs, 4, 999, "妖怪の全ステータスを 999 にしました。");
    }

    void    YokaiMaxStrengthCallback(MenuEntry *entry)
    {
        static const u32 offs[] = { OFF_STR };
        ForEachYokaiWrite16(offs, 1, 999, "妖怪の「ちから」を 999 にしました。");
    }

    void    YokaiMaxSpiritCallback(MenuEntry *entry)
    {
        static const u32 offs[] = { OFF_SPR };
        ForEachYokaiWrite16(offs, 1, 999, "妖怪の「ようりょく」を 999 にしました。");
    }

    void    YokaiMaxDefenseCallback(MenuEntry *entry)
    {
        static const u32 offs[] = { OFF_DEF };
        ForEachYokaiWrite16(offs, 1, 999, "妖怪の「まもり」を 999 にしました。");
    }

    void    YokaiMaxSpeedCallback(MenuEntry *entry)
    {
        static const u32 offs[] = { OFF_SPD };
        ForEachYokaiWrite16(offs, 1, 999, "妖怪の「すばやさ」を 999 にしました。");
    }

    void    YokaiMasterTechniquesCallback(MenuEntry *entry)
    {
        u32 base = VersionAddress(YOKAI_BASE);
        if (base == 0) { ShowUnsupported(); return; }

        u32 changed = 0;
        for (u32 i = 0; i < YOKAI_COUNT; ++i)
        {
            u32 slot = base + i * YOKAI_STRIDE;
            if (!IsYokaiSlotUsed(slot))
                continue;

            Process::Write8(slot + OFF_TECH1, 10);
            Process::Write8(slot + OFF_TECH2, 10);
            Process::Write8(slot + OFF_TECH3, 10);
            ++changed;
        }

        MessageBox(TITLE, "妖怪の技レベルを最大にしました。 (" + std::to_string(changed) + " 体)")();
    }

    // ---------------- フォルダ ----------------
    MenuFolder  *CreateItemFolder(void)
    {
        MenuFolder *folder = new MenuFolder("アイテム");
        *folder += new MenuEntry("所持アイテムを全て 99 個にする", nullptr, AllItemsX99Callback,
                                 "所持数が 1 以上のアイテムだけを 99 個にします。");
        return folder;
    }

    MenuFolder  *CreateYokaiFolder(void)
    {
        MenuFolder *folder = new MenuFolder("妖怪 (先頭254体)",
                                            "空きスロットは自動的にスキップします。");
        *folder += new MenuEntry("HP を 999 にする",           nullptr, YokaiMaxHPCallback);
        *folder += new MenuEntry("全ステータスを 999 にする",   nullptr, YokaiMaxAllStatsCallback);
        *folder += new MenuEntry("ちから を 999 にする",        nullptr, YokaiMaxStrengthCallback);
        *folder += new MenuEntry("ようりょく を 999 にする",    nullptr, YokaiMaxSpiritCallback);
        *folder += new MenuEntry("まもり を 999 にする",        nullptr, YokaiMaxDefenseCallback);
        *folder += new MenuEntry("すばやさ を 999 にする",      nullptr, YokaiMaxSpeedCallback);
        *folder += new MenuEntry("技レベルを全て最大にする",    nullptr, YokaiMasterTechniquesCallback);
        return folder;
    }
}
