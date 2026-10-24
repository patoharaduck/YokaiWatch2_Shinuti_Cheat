#include "cheats.hpp"

// ============================================================
//  妖怪ウォッチ2 真打 専用チート (wikiwiki.jp/3ds_codes 掲載コードより)
//
//  ・ここのアドレスは「真打 / old (旧3DS向けメモリ配置)」のもの。
//    ユーザー実機で 0x0872xxxx 系が動作しているのでこの配置で実装。
//  ・真打以外 (元祖/本家) では動作保証がないため、真打以外では実行を拒否する。
//  ・ASM 注入 (F0F0 系) のコードは再現していない。
// ============================================================

namespace CTRPluginFramework
{
    static const char *TITLE = "Yokai Watch 2 Tools";

    // ---------------- 共通ヘルパー ----------------
    static bool RequireShinuchi(void)
    {
        u32 low = (u32)(Process::GetTitleID() & 0xFFFFFFFFULL);
        if (low != 0x00155100)
        {
            MessageBox(TITLE, "この機能は「真打」専用です。")();
            return false;
        }
        return true;
    }

    // リストから 1 つ選ばせる。キャンセル時は -1
    static int SelectFromList(const std::string &title, std::vector<std::string> &items)
    {
        Keyboard kb(title);
        kb.Populate(items);
        return kb.Open();
    }

    // 10 進数入力。キャンセル/不正時は false
    static bool InputDecimal(const std::string &title, long minV, long maxV, long &out, long start)
    {
        Keyboard kb(title + "\n(" + std::to_string(minV) + "〜" + std::to_string(maxV) + ")");
        std::string s;
        if (kb.Open(s, std::to_string(start)) != 0)
            return false;
        char *end = nullptr;
        long v = strtol(s.c_str(), &end, 10);
        if (end == s.c_str())
        {
            MessageBox(TITLE, "数値として認識できませんでした。")();
            return false;
        }
        if (v < minV) v = minV;
        if (v > maxV) v = maxV;
        out = v;
        return true;
    }

    // 16 進数入力 (8桁まで)。キャンセル/不正時は false
    static bool InputHex32(const std::string &title, u32 &out, u32 start)
    {
        char buf[16];
        snprintf(buf, sizeof(buf), "%08X", (unsigned)start);
        Keyboard kb(title + "\n(16進数 8桁で入力)");
        kb.IsHexadecimal(true);
        std::string s;
        if (kb.Open(s, std::string(buf)) != 0)
            return false;
        char *end = nullptr;
        unsigned long v = strtoul(s.c_str(), &end, 16);
        if (end == s.c_str())
        {
            MessageBox(TITLE, "16進数として認識できませんでした。")();
            return false;
        }
        out = (u32)v;
        return true;
    }

    static void ReportWrite(bool ok, const std::string &okMsg)
    {
        if (ok) MessageBox(TITLE, okMsg)();
        else    MessageBox(TITLE, "書き込みに失敗しました。")();
    }

    // ============================================================
    //  ワープ
    // ============================================================
    static const u32 WARP_WRITE_ADDR   = 0x08727284;  // ここにマップIDを書く (A+B 押下時)
    static const u32 CURRENT_MAP_ADDR  = 0x08853FA8;  // 現在のマップID

    struct MapEntry { const char *name; u32 id; };
    static const MapEntry g_maps[] = {
        { "さくら町",                         0x91487158 },
        { "桜住宅街",                         0x17DC03F6 },
        { "桜EXツリー",                        0x32B75C2A },
        { "桜EXツリーの展望台",                 0x1157F7E3 },
        { "桜EXツリー エレベーター",            0xD627100C },
        { "おつかい横丁",                      0x98BE94A1 },
        { "そよ風",                           0x8A0B3B4F },
        { "ナギサき",                          0x6D1C5096 },
        { "だんだん坂",                        0xBDD5CB7D },
        { "大森山",                           0x0569AC18 },
        { "山頂",                             0xE921736D },
        { "女湯",                             0xE5BA0D10 },
        { "過去のひょうたん池",                 0x0C9F49E1 },
        { "過去のケマモト村",                   0xD6E80B88 },
        { "過去のケマモト村駅",                 0xC45DA466 },
        { "過去の大森山",                      0x83FDDEB6 },
        { "しのびの森",                        0xAB711614 },
        { "キウチ山山頂",                      0xA4C4415E },
        { "妖魔界 世界の境目",                  0xCDAD9B98 },
        { "妖魔界 大王の門",                    0x54A4CA22 },
        { "妖魔界 はらぺこ峠",                  0xCAC05F81 },
        { "妖魔界 うそつき山",                  0x23A3FAB4 },
        { "妖魔界 荒くれ街道",                  0xBDC76F17 },
        { "あやかし通り",                      0x2A6BBCDB },
        { "あやかし通り ランプウェイ",           0x2D0678C2 },
        { "あやかし通り 新元祖道",              0xED7CDAC6 },
        { "あやかし通り 新本家道",              0x6836D077 },
        { "金ぴか都市高",                      0x33708D9A },
        { "金ぴか都市高のガチャ",               0x85820729 },
        { "海マップ",                          0x7377FA8D },
        { "佐和山城",                          0x5A14A2FD },
        { "佐和山城 城内",                     0x48A10D13 },
        { "ゲラゲラ温泉",                      0xAEAC9A85 },
        { "満腹おたふく",                      0xA21D4487 },
        { "福福特急",                          0x495C332E },
        { "マンションのサーチマップ",           0xF4EFE642 },
        { "アミダ極楽 第6階層 (あやとり様)",     0xB28E75EE },
        { "歌舞伎座 スーパーステージ",           0xD01DE638 },
        { "電車のなか?",                       0xB10079BA },
        { "車の下 (コンビニから)",              0x90832346 },
        { "なまはげのバトルマップ (コンビニから)", 0x3AD23311 },
        { "虫取りの木 (コンビニから)",          0xA3772BF3 },
        { "信号の間 (コンビニから)",            0x3EE4FBDD },
        { "大森山山頂から見るマップ",            0xD9B77893 },
        { "ケマモト村バトルマップ",             0x21B08B5B },
    };
    static const u32 g_mapCount = sizeof(g_maps) / sizeof(g_maps[0]);

    static void WarpSelectCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;

        std::vector<std::string> names;
        for (u32 i = 0; i < g_mapCount; ++i)
            names.push_back(g_maps[i].name);

        int idx = SelectFromList("ワープ先を選んでください", names);
        if (idx < 0 || (u32)idx >= g_mapCount)
            return;

        ReportWrite(Process::Write32(WARP_WRITE_ADDR, g_maps[idx].id),
                    std::string("ワープ先を「") + g_maps[idx].name + "」に設定しました。\nメニューを閉じて A+B を押しながらマップ移動してください。");
    }

    static void WarpInputCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 id = 0;
        if (!InputHex32("マップIDを入力してください", id, 0x91487158))
            return;
        ReportWrite(Process::Write32(WARP_WRITE_ADDR, id), "ワープ先マップIDを設定しました。");
    }

    static void ShowCurrentMapCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 id = 0;
        if (!Process::Read32(CURRENT_MAP_ADDR, id))
        {
            MessageBox(TITLE, "読み込みに失敗しました。")();
            return;
        }
        char buf[64];
        snprintf(buf, sizeof(buf), "現在のマップID: %08X", (unsigned)id);
        const char *name = "(一覧に無いマップ)";
        for (u32 i = 0; i < g_mapCount; ++i)
            if (g_maps[i].id == id) { name = g_maps[i].name; break; }
        MessageBox(TITLE, std::string(buf) + "\n" + name)();
    }

    MenuFolder *CreateWarpFolder(void)
    {
        MenuFolder *f = new MenuFolder("ワープ (真打)", "マップIDを書き込みます。A+B を押しながらマップ移動で反映。");
        *f += new MenuEntry("ワープ先を一覧から選ぶ",      nullptr, WarpSelectCallback);
        *f += new MenuEntry("ワープ先マップIDを直接入力",  nullptr, WarpInputCallback);
        *f += new MenuEntry("現在のマップIDを表示",        nullptr, ShowCurrentMapCallback);
        return f;
    }

    // ============================================================
    //  妖怪スキル変更 (特定の妖怪 10 体)
    // ============================================================
    struct SkillYokai { const char *name; u32 address; u32 original; };
    static const SkillYokai g_skillYokai[] = {
        { "呪野花子",  0x08578F48, 0x286491ED },
        { "しゅらコマ", 0x0857A4F0, 0x251DFDE2 },
        { "なまはげ",  0x085810B4, 0xBC14AC58 },
        { "心オバア",  0x0858AE7C, 0x3B6B08BA },
        { "大くだん",  0x0857791C, 0x9D2D578D },
        { "ホリュウ",  0x0858AA5C, 0x6E934B01 },
        { "万尾獅子",  0x085863B8, 0x1D3F3776 },
        { "ブシニャン", 0x08583D0C, 0x67416BEF },
        { "吹雪姫",    0x085837E4, 0xB109AC92 },
        { "百鬼姫",    0x08583238, 0x79379EB7 },
    };
    static const u32 g_skillYokaiCount = sizeof(g_skillYokai) / sizeof(g_skillYokai[0]);

    struct SkillEntry { const char *name; u32 id; };
    static const SkillEntry g_skills[] = {
        { "満を持す",        0x1D3F3776 },
        { "かたながり",      0xBC14AC58 },
        { "いっせん",        0x833FCE10 },
        { "から傘シールド",  0xF9419289 },
        { "ねこばば",        0x8A6E3A16 },
        { "未来予知",        0x9D2D578D },
        { "ギャンブラー",    0xF438FE86 },
        { "おいのり",        0x3B6B08BA },
        { "トリプルヘッド",  0x361264B5 },
        { "呪いの天才",      0x286491ED },
        { "保留",            0x6E934B01 },
        { "超クリティカル",  0x67416BEF },
        { "もえるとうし",    0x251DFDE2 },
        { "吹雪姫のスキル",  0xB109AC92 },
        { "百鬼姫のスキル",  0x79379EB7 },
        { "光オーラ",        0xA4A99BEB },
    };
    static const u32 g_skillCount = sizeof(g_skills) / sizeof(g_skills[0]);

    static void SkillChangeCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;

        std::vector<std::string> yokai;
        for (u32 i = 0; i < g_skillYokaiCount; ++i)
            yokai.push_back(g_skillYokai[i].name);
        int y = SelectFromList("スキルを変更する妖怪", yokai);
        if (y < 0 || (u32)y >= g_skillYokaiCount) return;

        std::vector<std::string> skills;
        for (u32 i = 0; i < g_skillCount; ++i)
            skills.push_back(g_skills[i].name);
        int s = SelectFromList(std::string(g_skillYokai[y].name) + " の新しいスキル", skills);
        if (s < 0 || (u32)s >= g_skillCount) return;

        ReportWrite(Process::Write32(g_skillYokai[y].address, g_skills[s].id),
                    std::string(g_skillYokai[y].name) + " のスキルを「" + g_skills[s].name + "」にしました。\n※セーブ前に必ず「元に戻す」を実行してください。");
    }

    static void SkillRestoreAllCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 ok = 0;
        for (u32 i = 0; i < g_skillYokaiCount; ++i)
            if (Process::Write32(g_skillYokai[i].address, g_skillYokai[i].original))
                ++ok;
        MessageBox(TITLE, "スキルを元に戻しました (" + std::to_string(ok) + " 体)。")();
    }

    MenuFolder *CreateSkillFolder(void)
    {
        MenuFolder *f = new MenuFolder("妖怪スキル変更 (真打)", "セーブする前に必ず「元に戻す」を実行してください。");
        *f += new MenuEntry("スキルを変更する",         nullptr, SkillChangeCallback);
        *f += new MenuEntry("全員のスキルを元に戻す",   nullptr, SkillRestoreAllCallback);
        return f;
    }

    // ============================================================
    //  妖怪データ (手持ち/預かり 全スロット)
    //   構造体先頭 0x0870DBB0、1 体 0xC4 バイト、0x1A1 スロット
    //   +0x08 妖怪ID  +0x0C 名前  +0x44〜48 個体値(体力/ちから/ようりょく/まもり/すばやさ)
    //   +0x53 レベル  +0x64 HP  +0x66 ちから  +0x68 ようりょく  +0x6A まもり  +0x6C すばやさ
    // ============================================================
    static const u32 YW_BASE   = 0x0870DBB0;
    static const u32 YW_STRIDE = 0xC4;
    static const u32 YW_COUNT  = 0x1A1;
    static const u32 YW_NAME_MARK = 0xCB82CA82;   // 名前「ぬね」

    enum { O_ID = 0x08, O_NAME = 0x0C, O_IV = 0x44, O_LV = 0x53,
           O_HP = 0x64, O_STR = 0x66, O_SPR = 0x68, O_DEF = 0x6A, O_SPD = 0x6C };

    static bool SlotUsed(u32 slot)
    {
        u16 hp = 0;
        return Process::Read16(slot + O_HP, hp) && hp != 0;
    }

    // 名前が「ぬね」のスロットを探す (見つからなければ 0)
    static u32 FindNuneSlot(void)
    {
        for (u32 i = 0; i < YW_COUNT; ++i)
        {
            u32 slot = YW_BASE + i * YW_STRIDE;
            u32 v = 0;
            if (Process::Read32(slot + O_NAME, v) && v == YW_NAME_MARK)
                return slot;
        }
        MessageBox(TITLE, "名前が「ぬね」の妖怪が見つかりません。\n変更したい妖怪のニックネームを「ぬね」にしてください。")();
        return 0;
    }

    static void AllLevelCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        long lv = 0;
        if (!InputDecimal("全妖怪のレベル", 1, 255, lv, 99)) return;

        u32 n = 0;
        for (u32 i = 0; i < YW_COUNT; ++i)
        {
            u32 slot = YW_BASE + i * YW_STRIDE;
            if (!SlotUsed(slot)) continue;
            if (Process::Write8(slot + O_LV, (u8)lv)) ++n;
        }
        MessageBox(TITLE, "レベルを " + std::to_string(lv) + " にしました (" + std::to_string(n) + " 体)。")();
    }

    static void AllStatsInputCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        long v = 0;
        if (!InputDecimal("全妖怪の HP/ちから/ようりょく/まもり/すばやさ", 1, 65535, v, 65535)) return;

        u32 n = 0;
        for (u32 i = 0; i < YW_COUNT; ++i)
        {
            u32 slot = YW_BASE + i * YW_STRIDE;
            if (!SlotUsed(slot)) continue;
            Process::Write16(slot + O_HP,  (u16)v);
            Process::Write16(slot + O_STR, (u16)v);
            Process::Write16(slot + O_SPR, (u16)v);
            Process::Write16(slot + O_DEF, (u16)v);
            Process::Write16(slot + O_SPD, (u16)v);
            ++n;
        }
        MessageBox(TITLE, "全ステータスを " + std::to_string(v) + " にしました (" + std::to_string(n) + " 体)。")();
    }

    // 個体値プリセット
    struct IvPreset { const char *name; u8 v[5]; };
    static const IvPreset g_ivPresets[] = {
        { "体力に最大振り",     { 0x30, 0x00, 0x00, 0x00, 0x00 } },
        { "ちからに最大振り",   { 0x00, 0x28, 0x00, 0x00, 0x00 } },
        { "ようりょくに最大振り", { 0x00, 0x00, 0x28, 0x00, 0x00 } },
        { "まもりに最大振り",   { 0x00, 0x00, 0x00, 0x28, 0x00 } },
        { "すばやさに最大振り", { 0x00, 0x00, 0x00, 0x00, 0x28 } },
        { "バランス振り",       { 0x10, 0x0A, 0x0A, 0x0A, 0x0A } },
    };
    static const u32 g_ivPresetCount = sizeof(g_ivPresets) / sizeof(g_ivPresets[0]);

    static void WriteIv(u32 slot, const u8 *v)
    {
        for (u32 k = 0; k < 5; ++k)
            Process::Write8(slot + O_IV + k, v[k]);
    }

    static void AllIvPresetCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        std::vector<std::string> names;
        for (u32 i = 0; i < g_ivPresetCount; ++i) names.push_back(g_ivPresets[i].name);
        int p = SelectFromList("全妖怪の個体値プリセット", names);
        if (p < 0 || (u32)p >= g_ivPresetCount) return;

        u32 n = 0;
        for (u32 i = 0; i < YW_COUNT; ++i)
        {
            u32 slot = YW_BASE + i * YW_STRIDE;
            if (!SlotUsed(slot)) continue;
            WriteIv(slot, g_ivPresets[p].v);
            ++n;
        }
        MessageBox(TITLE, std::string("個体値を「") + g_ivPresets[p].name + "」にしました (" + std::to_string(n) + " 体)。\n※セーブ→タイトルに戻る→再読込で反映されます。")();
    }

    // ---- 名前「ぬね」の妖怪 ----
    static void NuneChangeIdCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 slot = FindNuneSlot();
        if (!slot) return;
        u32 cur = 0;
        Process::Read32(slot + O_ID, cur);
        u32 id = 0;
        if (!InputHex32("「ぬね」の妖怪IDを入力", id, cur)) return;
        ReportWrite(Process::Write32(slot + O_ID, id), "妖怪IDを変更しました。");
    }

    static void NuneLevelCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 slot = FindNuneSlot();
        if (!slot) return;
        u8 cur = 0;
        Process::Read8(slot + O_LV, cur);
        long lv = 0;
        if (!InputDecimal("「ぬね」のレベル", 1, 255, lv, cur)) return;
        ReportWrite(Process::Write8(slot + O_LV, (u8)lv), "レベルを " + std::to_string(lv) + " にしました。");
    }

    static void NuneStatsCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 slot = FindNuneSlot();
        if (!slot) return;

        static const char *labels[] = { "HP", "ちから", "ようりょく", "まもり", "すばやさ" };
        static const u32   offs[]   = { O_HP, O_STR, O_SPR, O_DEF, O_SPD };
        for (u32 k = 0; k < 5; ++k)
        {
            u16 cur = 0;
            Process::Read16(slot + offs[k], cur);
            long v = 0;
            if (!InputDecimal(std::string("「ぬね」の ") + labels[k], 1, 65535, v, cur)) return;
            Process::Write16(slot + offs[k], (u16)v);
        }
        MessageBox(TITLE, "ステータスを変更しました。")();
    }

    static void NuneIvCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 slot = FindNuneSlot();
        if (!slot) return;
        std::vector<std::string> names;
        for (u32 i = 0; i < g_ivPresetCount; ++i) names.push_back(g_ivPresets[i].name);
        int p = SelectFromList("「ぬね」の個体値プリセット", names);
        if (p < 0 || (u32)p >= g_ivPresetCount) return;
        WriteIv(slot, g_ivPresets[p].v);
        MessageBox(TITLE, "個体値を変更しました。\n※セーブ→タイトルに戻る→再読込で反映されます。")();
    }

    static void NuneTechCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 slot = FindNuneSlot();
        if (!slot) return;
        long lv = 0;
        if (!InputDecimal("「ぬね」の技レベル (3つとも)", 1, 10, lv, 10)) return;
        Process::Write8(slot + 0x2E, (u8)lv);
        Process::Write8(slot + 0x32, (u8)lv);
        Process::Write8(slot + 0x36, (u8)lv);
        MessageBox(TITLE, "技レベルを " + std::to_string(lv) + " にしました。")();
    }

    static void NuneIvInputCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 slot = FindNuneSlot();
        if (!slot) return;
        static const char *labels[] = { "体力", "ちから", "ようりょく", "まもり", "すばやさ" };
        u8 v[5];
        for (u32 k = 0; k < 5; ++k)
        {
            u8 cur = 0;
            Process::Read8(slot + O_IV + k, cur);
            long x = 0;
            if (!InputDecimal(std::string("「ぬね」の個体値: ") + labels[k], 0, 48, x, cur)) return;
            v[k] = (u8)x;
        }
        WriteIv(slot, v);
        MessageBox(TITLE, "個体値を変更しました。\n※セーブ→タイトルに戻る→再読込で反映されます。")();
    }

    MenuFolder *CreateYokaiExtFolder(void)
    {
        MenuFolder *f = new MenuFolder("妖怪データ (真打・全スロット)",
            "特定の1体だけ変えたい時は、その妖怪のニックネームを「ぬね」にしてください。");
        *f += new MenuEntry("全妖怪のレベルを変更",              nullptr, AllLevelCallback);
        *f += new MenuEntry("全妖怪のステータスを変更 (最大65535)", nullptr, AllStatsInputCallback);
        *f += new MenuEntry("全妖怪の個体値プリセット",           nullptr, AllIvPresetCallback);
        *f += new MenuEntry("「ぬね」の妖怪IDを変更",             nullptr, NuneChangeIdCallback);
        *f += new MenuEntry("「ぬね」のレベルを変更",             nullptr, NuneLevelCallback);
        *f += new MenuEntry("「ぬね」のステータスを変更",         nullptr, NuneStatsCallback);
        *f += new MenuEntry("「ぬね」の個体値プリセット",         nullptr, NuneIvCallback);
        *f += new MenuEntry("「ぬね」の個体値を手入力",           nullptr, NuneIvInputCallback);
        *f += new MenuEntry("「ぬね」の技レベルを変更",           nullptr, NuneTechCallback);
        return f;
    }

    // ============================================================
    //  主人公 / お供 / モデル
    // ============================================================
    static const u32 GENDER_ADDR   = 0x08736220;
    static const u32 PARTNER_ADDR  = 0x08722BEC;   // お供のモデルID
    static const u32 FACEICON_ADDR = 0x0872E4B0;   // プロフィール顔アイコンで選んだモデルID
    static const u32 MODEL_CODE    = 0x00412AB4;   // 主人公モデル (コード領域)

    static void GenderKeitaCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        ReportWrite(Process::Write32(GENDER_ADDR, 0x999), "主人公をケータにしました。\n※タイトル画面で実行してください。");
    }
    static void GenderFumikaCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        ReportWrite(Process::Write32(GENDER_ADDR, 0xFFF), "主人公をフミカにしました。\n※タイトル画面で実行してください。");
    }

    static void PartnerFromIconCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 id = 0;
        if (!Process::Read32(FACEICON_ADDR, id)) { MessageBox(TITLE, "読み込みに失敗しました。")(); return; }
        ReportWrite(Process::Write32(PARTNER_ADDR, id), "お供を変更しました。\n※セーブ→タイトルに戻る→再読込で反映されます。");
    }

    static void PartnerInputCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 cur = 0;
        Process::Read32(PARTNER_ADDR, cur);
        u32 id = 0;
        if (!InputHex32("お供のモデルIDを入力", id, cur)) return;
        ReportWrite(Process::Write32(PARTNER_ADDR, id), "お供を変更しました。\n※セーブ→タイトルに戻る→再読込で反映されます。");
    }

    static void HeroModelFromIconCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 id = 0;
        if (!Process::Read32(FACEICON_ADDR, id)) { MessageBox(TITLE, "読み込みに失敗しました。")(); return; }
        ReportWrite(Process::Patch(MODEL_CODE, id), "主人公モデルを変更しました。\n※タイトル画面に戻ると反映されます。");
    }

    static void HeroModelInputCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 cur = 0;
        Process::Read32(MODEL_CODE, cur);
        u32 id = 0;
        if (!InputHex32("主人公のモデルIDを入力", id, cur)) return;
        ReportWrite(Process::Patch(MODEL_CODE, id), "主人公モデルを変更しました。\n※タイトル画面に戻ると反映されます。");
    }

    MenuFolder *CreateModelFolder(void)
    {
        MenuFolder *f = new MenuFolder("主人公・お供 (真打)");
        *f += new MenuEntry("主人公をケータにする (タイトル画面で)",   nullptr, GenderKeitaCallback);
        *f += new MenuEntry("主人公をフミカにする (タイトル画面で)",   nullptr, GenderFumikaCallback);
        *f += new MenuEntry("お供を顔アイコンで選んだ妖怪にする",     nullptr, PartnerFromIconCallback,
                            "プロフィールの顔アイコンを変更してからこの項目を実行");
        *f += new MenuEntry("お供のモデルIDを直接入力",              nullptr, PartnerInputCallback);
        *f += new MenuEntry("主人公モデルを顔アイコンで選んだ妖怪にする", nullptr, HeroModelFromIconCallback);
        *f += new MenuEntry("主人公モデルIDを直接入力",              nullptr, HeroModelInputCallback);
        return f;
    }

    // ============================================================
    //  コードパッチ (ゲーム本体の命令を書き換えるタイプ)  チェックボックス式
    //   チェック ON  : 元の命令を保存してパッチを当てる
    //   チェック OFF : 毎フレームの監視 (CheatsFrameCallback) が元の命令に戻す
    // ============================================================
    struct CodePatch { u32 address; u32 value; };

    struct PatchSet
    {
        const char       *name;
        const CodePatch  *patches;
        u32               count;
        u32               original[8];
        bool              applied;
        MenuEntry        *entry;
    };

    static const CodePatch p_friend[]  = { { 0x00652F48, 0xE3A00001 } };
    static const CodePatch p_boss[]    = { { 0x0040CBC4, 0xE3A03801 }, { 0x0041537C, 0xE3A00001 }, { 0x00415380, 0xE12FFF1E },
                                           { 0x0055ACEC, 0xE3A01020 }, { 0x00652F5C, 0xE3A00001 } };
    static const CodePatch p_dict[]    = { { 0x00412738, 0xE3A00001 }, { 0x004181CC, 0xEA000006 } };
    static const CodePatch p_noitem[]  = { { 0x004F49BC, 0xE1A00000 } };

    static PatchSet g_patchSets[] = {
        { "友達チャンス確定",                    p_friend, 1, {0}, false, nullptr },
        { "ボス保存 + 友達チャンス可能",          p_boss,   5, {0}, false, nullptr },
        { "大辞典に全妖怪を表示",                p_dict,   2, {0}, false, nullptr },
        { "アイテム未所持でも限定妖怪が出現",      p_noitem, 1, {0}, false, nullptr },
    };
    static const u32 g_patchSetCount = sizeof(g_patchSets) / sizeof(g_patchSets[0]);

    static bool IsShinuchiSilent(void)
    {
        return (u32)(Process::GetTitleID() & 0xFFFFFFFFULL) == 0x00155100;
    }

    static void ApplyPatchSet(PatchSet &ps)
    {
        for (u32 i = 0; i < ps.count; ++i)
        {
            u32 orig = 0;
            Process::Read32(ps.patches[i].address, orig);
            ps.original[i] = orig;
            Process::Patch(ps.patches[i].address, ps.patches[i].value);
        }
        ps.applied = true;
    }

    static void RestorePatchSet(PatchSet &ps)
    {
        for (u32 i = 0; i < ps.count; ++i)
            Process::Patch(ps.patches[i].address, ps.original[i]);
        ps.applied = false;
    }

    // チェック ON の間、毎フレーム呼ばれる (MenuEntry の GameFunc)
    template <u32 Index>
    static void PatchSetGameFunc(MenuEntry *entry)
    {
        PatchSet &ps = g_patchSets[Index];
        ps.entry = entry;
        if (!ps.applied)
        {
            if (!IsShinuchiSilent())
            {
                entry->Disable();
                return;
            }
            ApplyPatchSet(ps);
        }
    }

    // ============================================================
    //  常時系チート (チェックボックス)
    // ============================================================
    static const u32 MONEY_ADDR_SHINUCHI = 0x08722BF8;

    static void MoneyLockGameFunc(MenuEntry *entry)
    {
        if (!IsShinuchiSilent()) { entry->Disable(); return; }
        Process::Write32(MONEY_ADDR_SHINUCHI, 999999);
    }

    // 「ぬね」の妖怪の HP を常に最大に (戦闘中の回復用)
    static u32 g_nuneSlotCache = 0;
    static void NuneHpLockGameFunc(MenuEntry *entry)
    {
        if (!IsShinuchiSilent()) { entry->Disable(); return; }
        if (entry->WasJustActivated())
            g_nuneSlotCache = 0;
        if (g_nuneSlotCache == 0)
        {
            for (u32 i = 0; i < YW_COUNT; ++i)
            {
                u32 slot = YW_BASE + i * YW_STRIDE, v = 0;
                if (Process::Read32(slot + O_NAME, v) && v == YW_NAME_MARK) { g_nuneSlotCache = slot; break; }
            }
            if (g_nuneSlotCache == 0) { entry->Disable(); return; }
        }
        u16 cur = 0, mx = 0;
        if (Process::Read16(g_nuneSlotCache + O_HP, mx) && mx != 0)
        {
            Process::Read16(g_nuneSlotCache + 0x3C, cur);   // 現在HP (元祖ARコードの 0x0870DBEC)
            if (cur != mx) Process::Write16(g_nuneSlotCache + 0x3C, mx);
        }
    }

    // 毎フレーム呼ばれる監視: チェックを外されたパッチを元に戻す
    void CheatsFrameCallback(void)
    {
        MoreFrameCallback();
        for (u32 i = 0; i < g_patchSetCount; ++i)
        {
            PatchSet &ps = g_patchSets[i];
            if (ps.applied && ps.entry && !ps.entry->IsActivated())
                RestorePatchSet(ps);
        }
    }

    MenuFolder *CreatePatchFolder(void)
    {
        MenuFolder *f = new MenuFolder("ゲーム動作の変更 (真打)", "チェックを入れている間だけ有効。外すと元に戻ります。");
        *f += new MenuEntry("友達チャンス確定",                  PatchSetGameFunc<0>);
        *f += new MenuEntry("ボス保存 + 友達チャンス可能",        PatchSetGameFunc<1>);
        *f += new MenuEntry("大辞典に全妖怪を表示",              PatchSetGameFunc<2>);
        *f += new MenuEntry("アイテム未所持でも限定妖怪が出現",   PatchSetGameFunc<3>);
        *f += new MenuEntry("お金が減らない (常に 999999)",       MoneyLockGameFunc);
        *f += new MenuEntry("「ぬね」の妖怪の HP が減らない",     NuneHpLockGameFunc,
                            "ニックネーム「ぬね」の妖怪の現在HPを毎フレーム最大に戻します。");
        return f;
    }

    // ============================================================
    //  ポインタ経由 (S系妖怪出現 / 限定クエスト)
    // ============================================================
    static bool FollowPtr(u32 base, u32 &out)
    {
        u32 p = 0, q = 0;
        if (!Process::Read32(base, p) || p < 0x08000000 || p >= 0x10000000) return false;
        if (!Process::Read32(p + 0x10, q) || q < 0x08000000 || q >= 0x10000000) return false;
        out = q;
        return true;
    }

    template <u8 Value>
    static void SpawnSCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 q = 0;
        if (!FollowPtr(0x0080D1F8, q)) { MessageBox(TITLE, "ポインタの解決に失敗しました。\nゲーム中(マップ上)で実行してください。")(); return; }
        ReportWrite(Process::Write8(q + 0x14B, Value),
                    Value == 1 ? "ジバニャンS が出現するようにしました。" : "コマさんS が出現するようにしました。");
    }

    template <u32 Offset>
    static void QuestUnlockCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 q = 0;
        if (!FollowPtr(0x0080D228, q)) { MessageBox(TITLE, "ポインタの解決に失敗しました。\nゲーム中(マップ上)で実行してください。")(); return; }
        u32 n = 0;
        for (u32 i = 0; i < 5; ++i)
        {
            u8 cur = 0;
            if (Process::Read8(q + Offset + i, cur) && cur == 0)
                if (Process::Write8(q + Offset + i, 1)) ++n;
        }
        MessageBox(TITLE, "限定クエストを受注可能にしました (" + std::to_string(n) + " 件)。")();
    }


    // ---- メモ (RAM書き換え用アドレス 0x031xxxxx は +0x055FF000 で 0x087xxxxx に対応: 消費アイテム 03124668 → 08723668 で検証) ----
    static const u32 GASHA_COUNT_ADDR = 0x08726C18;   // 23127C18 000000FF
    static const u32 DAILY_SAISEN_ADDR = 0x08726C02;  // 23127C02 00000010
    static const u32 DAILY_OMIKUJI_ADDR = 0x08726C04; // 23127C04 00000000

    static void GashaCountCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        ReportWrite(Process::Write8(GASHA_COUNT_ADDR, 0xFF), "ガシャの残り回数を 255 にしました。");
    }

    static void DailyResetCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        bool ok = Process::Write8(DAILY_SAISEN_ADDR, 0x10);
        ok = Process::Write8(DAILY_OMIKUJI_ADDR, 0x00) && ok;
        ReportWrite(ok, "1日制限 (賽銭 / 地獄おみくじ / えんえんトンネル 等) を解除しました。");
    }

    MenuFolder *CreateMiscFolder(void)
    {
        MenuFolder *f = new MenuFolder("その他 (真打)");
        *f += new MenuEntry("ジバニャンS を出現させる",   nullptr, SpawnSCallback<1>);
        *f += new MenuEntry("コマさんS を出現させる",     nullptr, SpawnSCallback<2>);
        *f += new MenuEntry("元祖限定クエストを受注可能に", nullptr, QuestUnlockCallback<0x6F>);
        *f += new MenuEntry("本家限定クエストを受注可能に", nullptr, QuestUnlockCallback<0x74>);
        *f += new MenuEntry("ガシャの残り回数を 255 にする",  nullptr, GashaCountCallback);
        *f += new MenuEntry("1日制限を解除 (賽銭/おみくじ/トンネル)", nullptr, DailyResetCallback);
        return f;
    }
    // ============================================================
    //  座標ツール
    //   主人公の座標アドレスは未判明のため、プラグイン内蔵の「Search」で見つけて登録する方式。
    //   前提: X, Y, Z が float で連続 (12 バイト) に並んでいること。
    //   探し方: Search → float → 初回は Unknown → 少し歩いて「変化した」で絞り込み → 立ち止まって「変化なし」
    //           → 候補が数個になったら値を書き換えて主人公が動く/飛ぶアドレスが座標。
    // ============================================================
    static u32   g_posAddr = 0x08852E10;   // Rzera 氏の座標コードより: 08852E10 / 08852E14(高さ) / 08852E18 の float 3 つ
    static float g_posSave[3][3];
    static bool  g_posSaved[3] = { false, false, false };

    static bool ReadPos(float *xyz)
    {
        return Process::ReadFloat(g_posAddr, xyz[0]) && Process::ReadFloat(g_posAddr + 4, xyz[1]) && Process::ReadFloat(g_posAddr + 8, xyz[2]);
    }
    static bool WritePos(const float *xyz)
    {
        return Process::WriteFloat(g_posAddr, xyz[0]) && Process::WriteFloat(g_posAddr + 4, xyz[1]) && Process::WriteFloat(g_posAddr + 8, xyz[2]);
    }
    static bool RequirePosAddr(void)
    {
        if (!RequireShinuchi()) return false;
        if (g_posAddr == 0)
        {
            MessageBox(TITLE, "座標アドレスが未設定です。\n「座標アドレスを設定」で X 座標(float)のアドレスを入力してください。")();
            return false;
        }
        return true;
    }
    static std::string PosToString(const float *xyz)
    {
        char buf[96];
        snprintf(buf, sizeof(buf), "X: %.2f\nY: %.2f\nZ: %.2f", xyz[0], xyz[1], xyz[2]);
        return std::string(buf);
    }

    static void PosSetAddrCallback(MenuEntry *entry)
    {
        u32 a = 0;
        if (!InputHex32("主人公の X 座標 (float) のアドレスを入力\n※Search で見つけたアドレス", a, g_posAddr)) return;
        g_posAddr = a;
        float xyz[3];
        if (ReadPos(xyz))
            MessageBox(TITLE, "座標アドレスを設定しました。\n" + PosToString(xyz))();
        else
            MessageBox(TITLE, "設定しましたが、そのアドレスを読めませんでした。")();
    }

    static void PosShowCallback(MenuEntry *entry)
    {
        if (!RequirePosAddr()) return;
        float xyz[3];
        if (ReadPos(xyz)) MessageBox(TITLE, PosToString(xyz))();
        else MessageBox(TITLE, "読み込みに失敗しました。")();
    }

    template <u32 Slot>
    static void PosSaveCallback(MenuEntry *entry)
    {
        if (!RequirePosAddr()) return;
        if (!ReadPos(g_posSave[Slot])) { MessageBox(TITLE, "読み込みに失敗しました。")(); return; }
        g_posSaved[Slot] = true;
        MessageBox(TITLE, "位置 " + std::to_string(Slot + 1) + " を記憶しました。\n" + PosToString(g_posSave[Slot]))();
    }

    template <u32 Slot>
    static void PosLoadCallback(MenuEntry *entry)
    {
        if (!RequirePosAddr()) return;
        if (!g_posSaved[Slot]) { MessageBox(TITLE, "位置 " + std::to_string(Slot + 1) + " はまだ記憶していません。")(); return; }
        ReportWrite(WritePos(g_posSave[Slot]), "位置 " + std::to_string(Slot + 1) + " へ移動しました。");
    }

    template <u32 Axis, int Delta>
    static void PosNudgeCallback(MenuEntry *entry)
    {
        if (!RequirePosAddr()) return;
        float v = 0;
        if (!Process::ReadFloat(g_posAddr + Axis * 4, v)) { MessageBox(TITLE, "読み込みに失敗しました。")(); return; }
        v += (float)Delta;
        Process::WriteFloat(g_posAddr + Axis * 4, v);
    }

    static void PosInputCallback(MenuEntry *entry)
    {
        if (!RequirePosAddr()) return;
        float xyz[3];
        if (!ReadPos(xyz)) { MessageBox(TITLE, "読み込みに失敗しました。")(); return; }
        static const char *labels[] = { "X", "Y", "Z" };
        for (u32 k = 0; k < 3; ++k)
        {
            Keyboard kb(std::string("座標 ") + labels[k] + " を入力");
            float v = xyz[k];
            if (kb.Open(v, xyz[k]) != 0) return;
            xyz[k] = v;
        }
        ReportWrite(WritePos(xyz), "座標を書き込みました。\n" + PosToString(xyz));
    }

    MenuFolder *CreatePosFolder(void)
    {
        MenuFolder *f = new MenuFolder("座標ツール (真打)",
            "既定は 0x08852E10 (公開コードより)。合わない場合は Search で見つけたアドレスを登録してください。");
        *f += new MenuEntry("座標アドレスを設定",        nullptr, PosSetAddrCallback);
        *f += new MenuEntry("現在の座標を表示",          nullptr, PosShowCallback);
        *f += new MenuEntry("座標を直接入力",            nullptr, PosInputCallback);
        *f += new MenuEntry("位置 1 を記憶",             nullptr, PosSaveCallback<0>);
        *f += new MenuEntry("位置 2 を記憶",             nullptr, PosSaveCallback<1>);
        *f += new MenuEntry("位置 3 を記憶",             nullptr, PosSaveCallback<2>);
        *f += new MenuEntry("位置 1 へ移動",             nullptr, PosLoadCallback<0>);
        *f += new MenuEntry("位置 2 へ移動",             nullptr, PosLoadCallback<1>);
        *f += new MenuEntry("位置 3 へ移動",             nullptr, PosLoadCallback<2>);
        *f += new MenuEntry("X +10",  nullptr, PosNudgeCallback<0, 10>);
        *f += new MenuEntry("X -10",  nullptr, PosNudgeCallback<0, -10>);
        *f += new MenuEntry("Y +10 (上)",  nullptr, PosNudgeCallback<1, 10>);
        *f += new MenuEntry("Y -10 (下)",  nullptr, PosNudgeCallback<1, -10>);
        *f += new MenuEntry("Z +10",  nullptr, PosNudgeCallback<2, 10>);
        *f += new MenuEntry("Z -10",  nullptr, PosNudgeCallback<2, -10>);
        return f;
    }
}
