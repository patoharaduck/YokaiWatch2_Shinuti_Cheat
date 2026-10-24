#include "cheats.hpp"

// ============================================================
//  真打 追加チート (Rzera 氏 / けいと村長 / まにまに氏 / 各公開コードより)
//   ・妖怪エディタ (スロット選択式)
//   ・移動系 (座標移動・ムーンジャンプ・スピード・スタミナ)
//   ・バトル系 / 装備系 / その他
//  オンライン対戦の番付・勝ち点・KP を書き換えるコードは意図的に入れていない。
// ============================================================

namespace CTRPluginFramework
{
    static const char *TITLE = "Yokai Watch 2 Tools";

    static bool IsShinuchi(void) { return (u32)(Process::GetTitleID() & 0xFFFFFFFFULL) == 0x00155100; }
    static bool RequireShinuchi(void)
    {
        if (!IsShinuchi()) { MessageBox(TITLE, "この機能は「真打」専用です。")(); return false; }
        return true;
    }
    static void Report(bool ok, const std::string &msg)
    {
        MessageBox(TITLE, ok ? msg : std::string("書き込みに失敗しました。"))();
    }
    static bool InputDec(const std::string &title, long minV, long maxV, long &out, long start)
    {
        Keyboard kb(title + "\n(" + std::to_string(minV) + "〜" + std::to_string(maxV) + ")");
        std::string s;
        if (kb.Open(s, std::to_string(start)) != 0) return false;
        char *end = nullptr; long v = strtol(s.c_str(), &end, 10);
        if (end == s.c_str()) { MessageBox(TITLE, "数値として認識できませんでした。")(); return false; }
        if (v < minV) v = minV;
        if (v > maxV) v = maxV;
        out = v; return true;
    }
    static bool InputHex(const std::string &title, u32 &out, u32 start)
    {
        char buf[16]; snprintf(buf, sizeof(buf), "%08X", (unsigned)start);
        Keyboard kb(title + "\n(16進数 8桁)");
        kb.IsHexadecimal(true);
        std::string s;
        if (kb.Open(s, std::string(buf)) != 0) return false;
        char *end = nullptr; unsigned long v = strtoul(s.c_str(), &end, 16);
        if (end == s.c_str()) { MessageBox(TITLE, "16進数として認識できませんでした。")(); return false; }
        out = (u32)v; return true;
    }

    // HID キーマスク (CTRPF の Key 列挙と同じ値)
    enum {
        K_A = 0x1, K_B = 0x2, K_SELECT = 0x4, K_START = 0x8,
        K_DRIGHT = 0x10, K_DLEFT = 0x20, K_DUP = 0x40, K_DDOWN = 0x80,
        K_R = 0x100, K_L = 0x200, K_X = 0x400, K_Y = 0x800,
        K_CRIGHT = 0x10000000, K_CLEFT = 0x20000000, K_CUP = 0x40000000, K_CDOWN = 0x80000000
    };

    // ============================================================
    //  妖怪エディタ
    //   スロット: 0x0870DBB0 + n*0xC4 (0x1A1 個)
    // ============================================================
    static const u32 YW_BASE = 0x0870DBB0, YW_STRIDE = 0xC4, YW_COUNT = 0x1A1;
    enum {
        O_ID = 0x08, O_NAME = 0x0C, O_TECH1 = 0x2E, O_TECH2 = 0x32, O_TECH3 = 0x36, O_HPCUR = 0x3C,
        O_IV = 0x44, O_TRAIN = 0x44 /* 旅時ステ改 16 bytes */, O_LV = 0x53, O_POSE = 0x55, O_ORIGIN = 0x59,
        O_HP = 0x64, O_STR = 0x66, O_SPR = 0x68, O_DEF = 0x6A, O_SPD = 0x6C, O_PHYS = 0x7C
    };

    static u32 g_editSlot = 0;

    static bool SlotUsed(u32 slot)
    {
        u16 hp = 0; u32 id = 0;
        return Process::Read16(slot + O_HP, hp) && hp != 0 && Process::Read32(slot + O_ID, id) && id != 0;
    }

    static std::string SlotLabel(u32 index, u32 slot)
    {
        u32 id = 0; u8 lv = 0;
        Process::Read32(slot + O_ID, id);
        Process::Read8(slot + O_LV, lv);
        const char *n = YokaiNameFromId(id);
        char buf[32]; snprintf(buf, sizeof(buf), "%3u: ", (unsigned)(index + 1));
        return std::string(buf) + (n ? n : "(不明)") + " Lv." + std::to_string(lv);
    }

    static bool RequireEditSlot(void)
    {
        if (!RequireShinuchi()) return false;
        if (g_editSlot == 0 || !SlotUsed(g_editSlot))
        {
            MessageBox(TITLE, "先に「編集する妖怪を選ぶ」で妖怪を選んでください。")();
            return false;
        }
        return true;
    }

    static void EdSelectCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        std::vector<std::string> names;
        std::vector<u32> slots;
        for (u32 i = 0; i < YW_COUNT; ++i)
        {
            u32 slot = YW_BASE + i * YW_STRIDE;
            if (!SlotUsed(slot)) continue;
            names.push_back(SlotLabel(i, slot));
            slots.push_back(slot);
        }
        if (names.empty()) { MessageBox(TITLE, "妖怪が見つかりませんでした。")(); return; }
        Keyboard kb("編集する妖怪を選んでください (" + std::to_string(names.size()) + " 体)");
        kb.Populate(names);
        int k = kb.Open();
        if (k < 0 || (u32)k >= slots.size()) return;
        g_editSlot = slots[k];
        EditorUiSetSlot(g_editSlot, (g_editSlot - YW_BASE) / YW_STRIDE);
        MessageBox(TITLE, "選択: " + names[k])();
    }

    static void EdShowCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        u32 id = 0, phys = 0; u8 lv = 0, t1 = 0, t2 = 0, t3 = 0, org = 0, pose = 0; u16 hp = 0, st = 0, sp = 0, df = 0, sd = 0, hc = 0;
        u8 iv[5] = {0};
        Process::Read32(g_editSlot + O_ID, id);   Process::Read32(g_editSlot + O_PHYS, phys);
        Process::Read8(g_editSlot + O_LV, lv);    Process::Read8(g_editSlot + O_ORIGIN, org); Process::Read8(g_editSlot + O_POSE, pose);
        Process::Read8(g_editSlot + O_TECH1, t1); Process::Read8(g_editSlot + O_TECH2, t2); Process::Read8(g_editSlot + O_TECH3, t3);
        Process::Read16(g_editSlot + O_HPCUR, hc); Process::Read16(g_editSlot + O_HP, hp); Process::Read16(g_editSlot + O_STR, st);
        Process::Read16(g_editSlot + O_SPR, sp);  Process::Read16(g_editSlot + O_DEF, df); Process::Read16(g_editSlot + O_SPD, sd);
        for (u32 k = 0; k < 5; ++k) Process::Read8(g_editSlot + O_IV + k, iv[k]);
        const char *n = YokaiNameFromId(id);
        char buf[512];
        snprintf(buf, sizeof(buf),
            "%s (ID %08X)\nLv.%u  HP %u/%u  ちから %u  ようりょく %u  まもり %u  すばやさ %u\n個体値 %u/%u/%u/%u/%u  技Lv %u/%u/%u\n物理技 %08X  元祖/本家 %02X  ポーズ %02X",
            n ? n : "(不明)", (unsigned)id, lv, hc, hp, st, sp, df, sd, iv[0], iv[1], iv[2], iv[3], iv[4], t1, t2, t3, (unsigned)phys, org, pose);
        MessageBox(TITLE, std::string(buf))();
    }

    static void EdSpeciesCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        u32 id = 0; std::string name;
        if (!PickYokaiId(id, name)) return;
        Report(Process::Write32(g_editSlot + O_ID, id), "種族を " + name + " に変更しました。");
    }

    static void EdSpeciesHexCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        u32 cur = 0; Process::Read32(g_editSlot + O_ID, cur);
        u32 id = 0;
        if (!InputHex("妖怪IDを入力", id, cur)) return;
        Report(Process::Write32(g_editSlot + O_ID, id), "妖怪IDを変更しました。");
    }

    static void EdLevelCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        u8 cur = 0; Process::Read8(g_editSlot + O_LV, cur);
        long v = 0; if (!InputDec("レベル", 1, 255, v, cur)) return;
        Report(Process::Write8(g_editSlot + O_LV, (u8)v), "レベルを " + std::to_string(v) + " にしました。");
    }

    template <u32 Off, u32 Max>
    static void EdStat16Callback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        u16 cur = 0; Process::Read16(g_editSlot + Off, cur);
        long v = 0; if (!InputDec("値を入力", 0, Max, v, cur)) return;
        bool ok = Process::Write16(g_editSlot + Off, (u16)v);
        if (Off == O_HP) Process::Write16(g_editSlot + O_HPCUR, (u16)v);
        Report(ok, "変更しました: " + std::to_string(v));
    }

    static void EdAllStatsCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        long v = 0; if (!InputDec("HP/ちから/ようりょく/まもり/すばやさ 全て", 1, 65535, v, 65535)) return;
        Process::Write16(g_editSlot + O_HP, (u16)v);  Process::Write16(g_editSlot + O_HPCUR, (u16)v);
        Process::Write16(g_editSlot + O_STR, (u16)v); Process::Write16(g_editSlot + O_SPR, (u16)v);
        Process::Write16(g_editSlot + O_DEF, (u16)v); Process::Write16(g_editSlot + O_SPD, (u16)v);
        MessageBox(TITLE, "全ステータスを " + std::to_string(v) + " にしました。")();
    }

    static void EdIvCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        static const char *labels[] = { "体力", "ちから", "ようりょく", "まもり", "すばやさ" };
        for (u32 k = 0; k < 5; ++k)
        {
            u8 cur = 0; Process::Read8(g_editSlot + O_IV + k, cur);
            long v = 0; if (!InputDec(std::string("個体値: ") + labels[k], 0, 127, v, cur)) return;
            Process::Write8(g_editSlot + O_IV + k, (u8)v);
        }
        MessageBox(TITLE, "個体値を変更しました。\n※セーブ→タイトルに戻る→再読込で反映されます。")();
    }

    static void EdTechCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        long v = 0; if (!InputDec("技レベル (3つとも)", 1, 10, v, 10)) return;
        Process::Write8(g_editSlot + O_TECH1, (u8)v); Process::Write8(g_editSlot + O_TECH2, (u8)v); Process::Write8(g_editSlot + O_TECH3, (u8)v);
        MessageBox(TITLE, "技レベルを " + std::to_string(v) + " にしました。")();
    }

    // 技・スキル (kind: 0 攻撃技 1 術 2 必殺技 3 ガード 4 とりつき 5 スキル)
    template <u32 Kind>
    static void EdTechCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        u32 id = 0, ptr = 0; std::string label;
        if (!PickTech(Kind, id, ptr, label)) return;
        if (ApplyTech(g_editSlot, Kind, id, ptr))
            MessageBox(TITLE, std::string(TechKindName(Kind)) + " を「" + label + "」にしました。")();
    }

    static void EdSkillShowCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        BuildTechCatalog();
        std::string out;
        for (u32 k = 0; k < 6; ++k)
        {
            u32 v = 0; Process::Read32(g_editSlot + TechKindOffset(k), v);
            out += std::string(TechKindName(k)) + ": " + TechLabel(k, v) + "\n";
        }
        MessageBox(TITLE, out)();
    }

    static void ReloadSkillTableCallback(MenuEntry *entry)
    {
        u32 n = LoadSkillTable();
        MessageBox(TITLE, "技ID表を読み込みました: " + std::to_string(n) + " 件\n(/YW2Tools/skills.txt)")();
    }

    template <u8 Value>
    static void EdOriginCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        Report(Process::Write8(g_editSlot + O_ORIGIN, Value), Value == 0x08 ? "元祖の妖怪にしました。" : "本家の妖怪にしました。");
    }

    static void EdPoseCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        Report(Process::Write8(g_editSlot + O_POSE, 0xAB), "決めポーズを全開放しました (要検証)。");
    }

    static void EdTrainMaxCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        // 旅時ステ改 (真打): 0870DBF4 7F7F7F7F / 0870DBF8 7F7F7F7F / 0870DBFC 7A7F7F7F / 0870DC00 FF7F7F7F
        bool ok = Process::Write32(g_editSlot + 0x44, 0x7F7F7F7F);
        ok = Process::Write32(g_editSlot + 0x48, 0x7F7F7F7F) && ok;
        ok = Process::Write32(g_editSlot + 0x4C, 0x7A7F7F7F) && ok;
        ok = Process::Write32(g_editSlot + 0x50, 0xFF7F7F7F) && ok;
        Report(ok, "肉体改造(ステ改)値を最大にしました。\nこの後スポクラで「ちから」の肉体改造を1回行い、リフレッシュしてください。");
    }

    static void EdCopyToNuneCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        MessageBox(TITLE, "この妖怪は「ぬね」系の機能でも扱えます。\n(ニックネームを「ぬね」にすると他フォルダの「ぬね」機能の対象になります)")();
    }


    // ---- 妖怪データの保存 / リストア (SD: /YW2Tools/yokai_N.bin, 0xC4 バイト丸ごと) ----
    static std::string SavePath(u32 n) { return "/YW2Tools/yokai_" + std::to_string(n) + ".bin"; }

    template <u32 N>
    static void EdSaveCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        u8 buf[YW_STRIDE];
        for (u32 i = 0; i < YW_STRIDE; ++i) Process::Read8(g_editSlot + i, buf[i]);
        Directory::Create("/YW2Tools");
        File f;
        if (File::Open(f, SavePath(N), File::WRITE | File::CREATE | File::TRUNCATE) != 0)
        { MessageBox(TITLE, "ファイルを開けませんでした。\n" + SavePath(N))(); return; }
        int r = f.Write(buf, YW_STRIDE);
        f.Close();
        MessageBox(TITLE, r == 0 ? "妖怪データをスロット " + std::to_string(N) + " に保存しました。\n" + SavePath(N)
                                 : "保存に失敗しました。")();
    }

    template <u32 N>
    static void EdRestoreCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        if (!File::Exists(SavePath(N))) { MessageBox(TITLE, "スロット " + std::to_string(N) + " に保存データがありません。")(); return; }
        u8 buf[YW_STRIDE];
        File f;
        if (File::Open(f, SavePath(N), File::READ) != 0) { MessageBox(TITLE, "ファイルを開けませんでした。")(); return; }
        int r = f.Read(buf, YW_STRIDE);
        f.Close();
        if (r != 0) { MessageBox(TITLE, "読み込みに失敗しました。")(); return; }
        for (u32 i = 0; i < YW_STRIDE; ++i) Process::Write8(g_editSlot + i, buf[i]);
        MessageBox(TITLE, "スロット " + std::to_string(N) + " のデータを選択中の妖怪に書き戻しました。")();
    }

    // ---- 技まわりの生データ表示 (オフセット特定用) ----
    static void EdRawCallback(MenuEntry *entry)
    {
        if (!RequireEditSlot()) return;
        std::string out = "オフセット +0x70〜+0xC0 の 32bit 値:\n";
        for (u32 off = 0x70; off < YW_STRIDE; off += 4)
        {
            u32 v = 0; Process::Read32(g_editSlot + off, v);
            char buf[24]; snprintf(buf, sizeof(buf), "+%02X: %08X%s", (unsigned)off, (unsigned)v, ((off - 0x70) / 4) % 2 ? "\n" : "   ");
            out += buf;
        }
        MessageBox(TITLE, out)();
    }

    // ---- 画面上に選択中の妖怪の情報を常時表示 (OSD) ----
    static bool EditorOsd(const Screen &screen)
    {
        if (!screen.IsTop || g_editSlot == 0) return false;
        u32 id = 0; u8 lv = 0; u16 hp = 0, hc = 0, st = 0, sp = 0, df = 0, sd = 0;
        Process::Read32(g_editSlot + O_ID, id); Process::Read8(g_editSlot + O_LV, lv);
        Process::Read16(g_editSlot + O_HP, hp); Process::Read16(g_editSlot + O_HPCUR, hc);
        Process::Read16(g_editSlot + O_STR, st); Process::Read16(g_editSlot + O_SPR, sp);
        Process::Read16(g_editSlot + O_DEF, df); Process::Read16(g_editSlot + O_SPD, sd);
        const char *n = YokaiNameFromId(id);
        char l1[96], l2[96], l3[96];
        snprintf(l1, sizeof(l1), "Yokai Editor: %s (ID %08X)", n ? n : "?", (unsigned)id);
        snprintf(l2, sizeof(l2), "Lv.%u  HP %u/%u", lv, hc, hp);
        snprintf(l3, sizeof(l3), "ちから %u  ようりょく %u  まもり %u  すばやさ %u", st, sp, df, sd);
        // Draw() は内蔵の英数字フォントで日本語が化けるため、システムフォント描画を使う
        screen.DrawRect(6, 6, 388, 52, Color(0, 0, 0, 200), true);
        screen.DrawSysfont(l1, 10, 8);
        screen.DrawSysfont(l2, 10, 24);
        screen.DrawSysfont(l3, 10, 40);
        return true;
    }

    static MenuEntry *g_osdEntry = nullptr;
    static bool       g_osdOn = false;

    static void EdOsdGameFunc(MenuEntry *entry)
    {
        g_osdEntry = entry;
        if (!g_osdOn) { OSD::Run(EditorOsd); g_osdOn = true; }
    }

    MenuFolder *CreateEditorFolder(void)
    {
        MenuFolder *f = new MenuFolder("妖怪エディタ (真打)", "まず「編集する妖怪を選ぶ」で対象を選び、各項目を変更します。");
        *f += CreateEditorUiEntry();
        *f += new MenuEntry("編集する妖怪を選ぶ",             nullptr, EdSelectCallback);
        *f += new MenuEntry("選択中の妖怪の情報を表示",        nullptr, EdShowCallback);
        *f += new MenuEntry("種族を一覧から変更",              nullptr, EdSpeciesCallback);
        *f += new MenuEntry("種族IDを直接入力",                nullptr, EdSpeciesHexCallback);
        *f += new MenuEntry("レベル",                          nullptr, EdLevelCallback);
        *f += new MenuEntry("HP (最大/現在)",                  nullptr, EdStat16Callback<O_HP, 65535>);
        *f += new MenuEntry("ちから",                          nullptr, EdStat16Callback<O_STR, 65535>);
        *f += new MenuEntry("ようりょく",                      nullptr, EdStat16Callback<O_SPR, 65535>);
        *f += new MenuEntry("まもり",                          nullptr, EdStat16Callback<O_DEF, 65535>);
        *f += new MenuEntry("すばやさ",                        nullptr, EdStat16Callback<O_SPD, 65535>);
        *f += new MenuEntry("全ステータスをまとめて設定",       nullptr, EdAllStatsCallback);
        *f += new MenuEntry("個体値 (5項目)",                  nullptr, EdIvCallback);
        *f += new MenuEntry("技レベル (3つ)",                  nullptr, EdTechCallback);
        *f += new MenuEntry("技・スキルを表示",                 nullptr, EdSkillShowCallback);
        *f += new MenuEntry("攻撃技を変更",                     nullptr, EdTechCallback<0>);
        *f += new MenuEntry("術を変更",                         nullptr, EdTechCallback<1>);
        *f += new MenuEntry("必殺技を変更",                     nullptr, EdTechCallback<2>);
        *f += new MenuEntry("とりつきを変更",                   nullptr, EdTechCallback<4>);
        *f += new MenuEntry("スキルを変更",                     nullptr, EdTechCallback<5>);
        *f += new MenuEntry("技カタログを作り直す",             nullptr, RebuildTechCatalogCallback,
                            "定義表(全種族)と手持ちから技の一覧を作ります。妖怪を増やした後に実行。");
        *f += new MenuEntry("技ID表を再読み込み (skills.txt)",  nullptr, ReloadSkillTableCallback,
                            "SDの /YW2Tools/skills.txt に「8桁ID 技名」。あれば名前で表示されます。");
        *f += new MenuEntry("元祖の妖怪にする",                nullptr, EdOriginCallback<0x08>);
        *f += new MenuEntry("本家の妖怪にする",                nullptr, EdOriginCallback<0x10>);
        *f += new MenuEntry("決めポーズ全開放 (要検証)",       nullptr, EdPoseCallback);
        *f += new MenuEntry("肉体改造(ステ改)値を最大に",       nullptr, EdTrainMaxCallback,
                            "実行後、スポクラで「ちから」の肉体改造を1回してリフレッシュ。");
        *f += new MenuEntry("技まわりの生データを表示 (+0x70〜)", nullptr, EdRawCallback,
                            "技IDのオフセット特定用。技を付け替えてから見比べてください。");
        *f += new MenuEntry("妖怪データを保存 (スロット1)",    nullptr, EdSaveCallback<1>);
        *f += new MenuEntry("妖怪データを保存 (スロット2)",    nullptr, EdSaveCallback<2>);
        *f += new MenuEntry("妖怪データを保存 (スロット3)",    nullptr, EdSaveCallback<3>);
        *f += new MenuEntry("妖怪データをリストア (スロット1)", nullptr, EdRestoreCallback<1>);
        *f += new MenuEntry("妖怪データをリストア (スロット2)", nullptr, EdRestoreCallback<2>);
        *f += new MenuEntry("妖怪データをリストア (スロット3)", nullptr, EdRestoreCallback<3>);
        *f += new MenuEntry("選択中の妖怪を上画面に常時表示",   EdOsdGameFunc,
                            "チェックを入れると上画面左上に情報を描画します。");
        return f;
    }

    // ============================================================
    //  汎用パッチセット (チェックボックス)
    //   kind 0 = コード領域 (Process::Patch)  1 = 32bit Write  2 = 8bit Write
    // ============================================================
    struct PatchLine { u32 address; u32 value; u8 kind; };
    struct PatchSet2 { const char *name; const PatchLine *lines; u32 count; u32 original[10]; bool applied; MenuEntry *entry; };

    static void ApplySet(PatchSet2 &ps)
    {
        for (u32 i = 0; i < ps.count; ++i)
        {
            const PatchLine &l = ps.lines[i];
            u32 orig = 0;
            if (l.kind == 2) { u8 b = 0; Process::Read8(l.address, b); orig = b; }
            else Process::Read32(l.address, orig);
            ps.original[i] = orig;
            if (l.kind == 0)      Process::Patch(l.address, l.value);
            else if (l.kind == 1) Process::Write32(l.address, l.value);
            else                  Process::Write8(l.address, (u8)l.value);
        }
        ps.applied = true;
    }
    static void RestoreSet(PatchSet2 &ps)
    {
        for (u32 i = 0; i < ps.count; ++i)
        {
            const PatchLine &l = ps.lines[i];
            if (l.kind == 0)      Process::Patch(l.address, ps.original[i]);
            else if (l.kind == 1) Process::Write32(l.address, ps.original[i]);
            else                  Process::Write8(l.address, (u8)ps.original[i]);
        }
        ps.applied = false;
    }

    // ---- 移動系 ----
    static const PatchLine L_speed1[]   = { { 0x08001EFC, 0x4E800000, 1 } };
    static const PatchLine L_speed2[]   = { { 0x08001F00, 0x4F800000, 1 } };
    static const PatchLine L_stamina[]  = { { 0x08001F2C, 0x4F800000, 1 } };
    static const PatchLine L_norun[]    = { { 0x08001EFC, 0x2F800000, 1 } };
    static const PatchLine L_stairs[]   = { { 0x0039D928, 0xE3A00000, 0 }, { 0x0039D92C, 0xE58511A4, 0 } };
    static const PatchLine L_bikein[]   = { { 0x0800205C, 0x33, 2 } };
    // ---- アクション ----
    static const PatchLine L_actFun[]   = { { 0x15444B10, 0x3F, 2 }, { 0x15444B11, 0x3F, 2 } };
    static const PatchLine L_actUki[]   = { { 0x15444B10, 0x3C, 2 } };
    static const PatchLine L_actRobo[]  = { { 0x15444B0C, 0x3E, 2 }, { 0x15444B08, 0x02, 2 } };
    static const PatchLine L_actSit[]   = { { 0x15444B05, 0x3F, 2 } };
    static const PatchLine L_actLadder[]= { { 0x15444B08, 0x5CE, 1 } };
    // ---- バトル ----
    static const PatchLine L_dmg[]      = { { 0x00568C94, 0x0000FFFF, 0 } };
    static const PatchLine L_special[]  = { { 0x00563114, 0xE1C608BE, 0 }, { 0x00575B80, 0xE3A000FF, 0 } };
    static const PatchLine L_crit[]     = { { 0x00880040, 0xE3A00000, 0 }, { 0x00880044, 0xE3570000, 0 }, { 0x00880048, 0x012FFF1E, 0 },
                                            { 0x0088004C, 0xE5D7B38E, 0 }, { 0x00880050, 0xE35B0000, 0 }, { 0x00880054, 0x13A00001, 0 },
                                            { 0x00880058, 0xE12FFF1E, 0 }, { 0x00569F6C, 0xEB0C5833, 0 } };
    static const PatchLine L_dodge[]    = { { 0x0088005C, 0xE3A00000, 0 }, { 0x00880060, 0xE3590000, 0 }, { 0x00880064, 0x012FFF1E, 0 },
                                            { 0x00880068, 0xE5D9B38E, 0 }, { 0x0088006C, 0xE35B0000, 0 }, { 0x00880070, 0x13A00001, 0 },
                                            { 0x00880074, 0xE12FFF1E, 0 }, { 0x00567A9C, 0xEB0C616E, 0 } };
    static const PatchLine L_boss2[]    = { { 0x003F5900, 0xE3A00000, 0 } };
    // ---- その他 ----
    static const PatchLine L_sell[]     = { { 0x004A3364, 0xE1A00000, 0 } };
    static const PatchLine L_chat[]     = { { 0x0020EF18, 0xE3A02001, 0 }, { 0x0020EF1C, 0xE5C5207F, 0 }, { 0x0020EF20, 0xEA000001, 0 } };
    static const PatchLine L_fps60[]    = { { 0x08857BCD, 0x01, 2 } };
    static const PatchLine L_daily[]    = { { 0x005AA834, 0xE3A00001, 0 } };
    static const PatchLine L_armlet[]   = { { 0x08595E4C, 0x7F7F7F7F, 1 }, { 0x08595E50, 0x0000007F, 1 } };

    #define PS(n, arr) { n, arr, (u32)(sizeof(arr) / sizeof(arr[0])), {0}, false, nullptr }
    static PatchSet2 g_sets[] = {
        PS("スピードハック 1",              L_speed1),     // 0
        PS("スピードハック 2",              L_speed2),     // 1
        PS("スタミナ無限",                  L_stamina),    // 2
        PS("走れなくなる",                  L_norun),      // 3
        PS("階段でも自転車",                L_stairs),     // 4
        PS("自転車で室内に入れる",          L_bikein),     // 5
        PS("歩くのが楽しいアクション",      L_actFun),     // 6
        PS("ケータがウキウキになる",        L_actUki),     // 7
        PS("ケータ ロボット化",             L_actRobo),    // 8
        PS("座りながら移動",                L_actSit),     // 9
        PS("ハシゴを昇るアクション",        L_actLadder),  // 10
        PS("ダメージ最大 65535",            L_dmg),        // 11
        PS("必殺技無限",                    L_special),    // 12
        PS("クリティカル確定",              L_crit),       // 13
        PS("攻撃全回避",                    L_dodge),      // 14
        PS("ボス保存 (別版 003F5900)",      L_boss2),      // 15
        PS("無限売却",                      L_sell),       // 16
        PS("チャット加速化",                L_chat),       // 17
        PS("FPS 60",                        L_fps60),      // 18
        PS("1日1回イベント無限",            L_daily),      // 19
        PS("そまつなうでわ 全能力+127",      L_armlet),     // 20
    };
    static const u32 g_setCount = sizeof(g_sets) / sizeof(g_sets[0]);

    template <u32 Index>
    static void SetGameFunc(MenuEntry *entry)
    {
        PatchSet2 &ps = g_sets[Index];
        ps.entry = entry;
        if (!ps.applied)
        {
            if (!IsShinuchi()) { entry->Disable(); return; }
            ApplySet(ps);
        }
    }

    // ---- 座標移動 (Rzera 氏コード): A+十字キー で水平移動、B で上昇、X でゆっくり上昇 ----
    static const u32 POS_X = 0x08852E10, POS_Y = 0x08852E14, POS_Z = 0x08852E18;
    static const u32 WHISPER_X = 0x0880A448, WHISPER_Y = 0x0880A44C, WHISPER_Z = 0x0880A450;

    static float g_moveStep = 7.5f;    // A+十字/スティック 1フレームあたりの移動量
    static float g_riseStep = 22.0f;   // B 上昇 1フレームあたり
    static float g_jumpA    = 13.0f;   // A ジャンプ
    static float g_jumpX    = 3.25f;   // X ゆっくり上昇
    static float g_whisperStep = 88.0f;

    static void SetFloatCallback(float &target, const char *what)
    {
        Keyboard kb(std::string(what) + " (1フレームあたりの移動量)");
        float v = target;
        if (kb.Open(v, target) == 0) { target = v; MessageBox(TITLE, std::string(what) + " を " + std::to_string(v) + " にしました。")(); }
    }
    static void MoveStepCallback(MenuEntry *e)    { SetFloatCallback(g_moveStep, "座標移動の速度"); }
    static void RiseStepCallback(MenuEntry *e)    { SetFloatCallback(g_riseStep, "B 上昇の速度"); }
    static void JumpAStepCallback(MenuEntry *e)   { SetFloatCallback(g_jumpA, "A ジャンプの高さ"); }
    static void JumpXStepCallback(MenuEntry *e)   { SetFloatCallback(g_jumpX, "X 上昇の速度"); }
    static void WhisperStepCallback(MenuEntry *e) { SetFloatCallback(g_whisperStep, "ウィスパー移動の速度"); }

    static void AddFloat(u32 addr, float d)
    {
        float v = 0;
        if (Process::ReadFloat(addr, v)) Process::WriteFloat(addr, v + d);
    }

    static void MoveDpadGameFunc(MenuEntry *entry)
    {
        if (!IsShinuchi()) { entry->Disable(); return; }
        if (!Controller::IsKeyDown((Key)K_A)) return;
        if (Controller::IsKeyDown((Key)K_DDOWN))  AddFloat(POS_Z,  g_moveStep);
        if (Controller::IsKeyDown((Key)K_DUP))    AddFloat(POS_Z, -g_moveStep);
        if (Controller::IsKeyDown((Key)K_DRIGHT)) AddFloat(POS_X,  g_moveStep);
        if (Controller::IsKeyDown((Key)K_DLEFT))  AddFloat(POS_X, -g_moveStep);
    }

    static void MoveStickGameFunc(MenuEntry *entry)
    {
        if (!IsShinuchi()) { entry->Disable(); return; }
        if (!Controller::IsKeyDown((Key)K_A)) return;
        if (Controller::IsKeyDown((Key)K_CDOWN))  AddFloat(POS_Z,  g_moveStep);
        if (Controller::IsKeyDown((Key)K_CUP))    AddFloat(POS_Z, -g_moveStep);
        if (Controller::IsKeyDown((Key)K_CRIGHT)) AddFloat(POS_X,  g_moveStep);
        if (Controller::IsKeyDown((Key)K_CLEFT))  AddFloat(POS_X, -g_moveStep);
    }

    static void MoonJumpBGameFunc(MenuEntry *entry)
    {
        if (!IsShinuchi()) { entry->Disable(); return; }
        if (Controller::IsKeyDown((Key)K_B)) Process::WriteFloat(POS_Y, 144.0f);   // 43100000
    }

    static void RiseBGameFunc(MenuEntry *entry)
    {
        if (!IsShinuchi()) { entry->Disable(); return; }
        if (Controller::IsKeyDown((Key)K_B)) AddFloat(POS_Y, g_riseStep);          // 元コード 41B00000 = 22.0
    }

    static void JumpAXGameFunc(MenuEntry *entry)
    {
        if (!IsShinuchi()) { entry->Disable(); return; }
        if (Controller::IsKeyDown((Key)K_A)) AddFloat(POS_Y, g_jumpA);             // 元コード 41500000 = 13.0
        if (Controller::IsKeyDown((Key)K_X)) AddFloat(POS_Y, g_jumpX);             // 元コード 40500000 = 3.25
    }

    static void WhisperMoveGameFunc(MenuEntry *entry)
    {
        if (!IsShinuchi()) { entry->Disable(); return; }
        if (!Controller::IsKeyDown((Key)K_A)) return;
        if (Controller::IsKeyDown((Key)K_DRIGHT)) AddFloat(WHISPER_X,  g_whisperStep);
        if (Controller::IsKeyDown((Key)K_DLEFT))  AddFloat(WHISPER_X, -g_whisperStep);
        if (Controller::IsKeyDown((Key)K_DDOWN))  AddFloat(WHISPER_Z,  g_whisperStep);
        if (Controller::IsKeyDown((Key)K_DUP))    AddFloat(WHISPER_Z, -g_whisperStep);
    }

    static void WarpAGameFunc(MenuEntry *entry)
    {
        if (!IsShinuchi()) { entry->Disable(); return; }
        if (Controller::IsKeyDown((Key)K_A)) AddFloat(0x08852E17, 1408.0f);        // 元コードそのまま (要検証)
    }

    MenuFolder *CreateMoveFolder(void)
    {
        MenuFolder *f = new MenuFolder("移動系 (真打)", "チェックを入れている間だけ有効。座標コードは Rzera 氏のものを移植。");
        *f += new MenuEntry("座標移動: A + 十字キー",          MoveDpadGameFunc);
        *f += new MenuEntry("座標移動: A + スティック",        MoveStickGameFunc);
        *f += new MenuEntry("ムーンジャンプ: B で高さ固定",     MoonJumpBGameFunc);
        *f += new MenuEntry("B で上昇",                        RiseBGameFunc);
        *f += new MenuEntry("A でジャンプ / X でゆっくり上昇",   JumpAXGameFunc);
        *f += new MenuEntry("ウィスパー座標移動: A + 十字キー", WhisperMoveGameFunc);
        *f += new MenuEntry("A でワープ? (要検証)",             WarpAGameFunc);
        *f += new MenuEntry("設定: 座標移動の速度 (初期 7.5)",      nullptr, MoveStepCallback);
        *f += new MenuEntry("設定: B 上昇の速度 (初期 22)",         nullptr, RiseStepCallback);
        *f += new MenuEntry("設定: A ジャンプの高さ (初期 13)",     nullptr, JumpAStepCallback);
        *f += new MenuEntry("設定: X 上昇の速度 (初期 3.25)",      nullptr, JumpXStepCallback);
        *f += new MenuEntry("設定: ウィスパー移動の速度 (初期 88)", nullptr, WhisperStepCallback);
        *f += new MenuEntry("スピードハック 1",                SetGameFunc<0>);
        *f += new MenuEntry("スピードハック 2",                SetGameFunc<1>);
        *f += new MenuEntry("スタミナ無限",                    SetGameFunc<2>);
        *f += new MenuEntry("走れなくなる (ネタ)",             SetGameFunc<3>);
        *f += new MenuEntry("階段でも自転車",                  SetGameFunc<4>);
        *f += new MenuEntry("自転車で室内に入れる",            SetGameFunc<5>, "外で自転車に乗ったまま室内へ。えんえんトンネルでひょっとこが出るとフリーズ注意。");
        *f += new MenuEntry("アクション: 歩くのが楽しい",       SetGameFunc<6>);
        *f += new MenuEntry("アクション: ウキウキ",             SetGameFunc<7>);
        *f += new MenuEntry("アクション: ロボット化",           SetGameFunc<8>);
        *f += new MenuEntry("アクション: 座りながら移動",       SetGameFunc<9>);
        *f += new MenuEntry("アクション: ハシゴ昇り",           SetGameFunc<10>);
        return f;
    }

    MenuFolder *CreateBattleFolder(void)
    {
        MenuFolder *f = new MenuFolder("バトル系 (真打)", "チェックを入れている間だけ有効。");
        *f += new MenuEntry("ダメージ最大 65535",     SetGameFunc<11>);
        *f += new MenuEntry("必殺技無限",             SetGameFunc<12>);
        *f += new MenuEntry("クリティカル確定",       SetGameFunc<13>);
        *f += new MenuEntry("攻撃全回避",             SetGameFunc<14>);
        *f += new MenuEntry("ボス保存 (別版)",        SetGameFunc<15>);
        return f;
    }

    // ---- 装備系 (ワンショット) ----
    static void Equip2SlotCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 n = 0;
        for (u32 i = 0; i < 0x2C3; ++i) if (Process::Write8(0x085767BC + i * 0x84, 0x2D)) ++n;
        MessageBox(TITLE, "装備を2枠にしました (" + std::to_string(n) + " 件)。")();
    }
    static void EquipNoCheckCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 n = 0;
        for (u32 i = 0; i < 0x4A; ++i) if (Process::Write8(0x08595E51 + i * 0x38, 0x00)) ++n;
        MessageBox(TITLE, "装備の判定を削除しました (" + std::to_string(n) + " 件)。")();
    }
    static void SoulLv10Callback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 n = 0;
        for (u32 i = 0; i < 0x64; ++i)
        {
            u8 cur = 0;
            if (Process::Read8(0x08726212 + i * 0x14, cur) && cur != 0 && Process::Write8(0x08726212 + i * 0x14, 0x0A)) ++n;
        }
        MessageBox(TITLE, "所持している魂のレベルを 10 にしました (" + std::to_string(n) + " 件)。")();
    }
    static void GodExpCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        Report(Process::Write32(0x0859D464, 0xFFFFFFFF), "神けいけんちだま 1個でレベル99になるようにしました。");
    }

    MenuFolder *CreateEquipFolder(void)
    {
        MenuFolder *f = new MenuFolder("装備・魂 (真打)");
        *f += new MenuEntry("そまつなうでわ 全能力+127",     SetGameFunc<20>);
        *f += new MenuEntry("装備を2枠にする",               nullptr, Equip2SlotCallback);
        *f += new MenuEntry("装備判定を削除 (誰でも装備可)",  nullptr, EquipNoCheckCallback);
        *f += new MenuEntry("所持している魂を全てレベル10に", nullptr, SoulLv10Callback);
        *f += new MenuEntry("神けいけんちだま1個でレベル99",  nullptr, GodExpCallback);
        return f;
    }

    // ---- その他2 ----
    static void JpMaxCallback(MenuEntry *entry) { if (!RequireShinuchi()) return; Report(Process::Write32(0x08722BFC, 999999), "JP (ジャングルポイント) を 999999 にしました。"); }
    static void GpMaxCallback(MenuEntry *entry) { if (!RequireShinuchi()) return; Report(Process::Write32(0x08722C00, 999999), "GP を 999999 にしました。"); }
    static void LegendReopenCallback(MenuEntry *entry) { if (!RequireShinuchi()) return; Report(Process::Write32(0x08727108, 0), "レジェンドを再開放しました。"); }
    static void Gasha99Callback(MenuEntry *entry) { if (!RequireShinuchi()) return; Report(Process::Write8(0x08726C18, 99), "ガシャの残り回数を 99 にしました。"); }

    static void AllYokaiOrigin(u8 v, const char *msg)
    {
        u32 n = 0;
        for (u32 i = 0; i < YW_COUNT; ++i)
        {
            u32 slot = YW_BASE + i * YW_STRIDE;
            if (!SlotUsed(slot)) continue;
            if (Process::Write8(slot + O_ORIGIN, v)) ++n;
        }
        MessageBox(TITLE, std::string(msg) + " (" + std::to_string(n) + " 体)")();
    }
    static void AllHonkeCallback(MenuEntry *entry) { if (!RequireShinuchi()) return; AllYokaiOrigin(0x10, "全妖怪を本家にしました。"); }
    static void AllGansoCallback(MenuEntry *entry) { if (!RequireShinuchi()) return; AllYokaiOrigin(0x08, "全妖怪を元祖にしました。"); }
    static void AllPhysCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 id = 0;
        if (!InputHex("全妖怪の物理技IDを入力", id, 0)) return;
        u32 n = 0;
        for (u32 i = 0; i < YW_COUNT; ++i)
        {
            u32 slot = YW_BASE + i * YW_STRIDE;
            if (!SlotUsed(slot)) continue;
            if (Process::Write32(slot + O_PHYS, id)) ++n;
        }
        MessageBox(TITLE, "全妖怪の物理技を変更しました (" + std::to_string(n) + " 体)。")();
    }
    static void AllPoseCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 n = 0;
        for (u32 i = 0; i < YW_COUNT; ++i)
        {
            u32 slot = YW_BASE + i * YW_STRIDE;
            if (!SlotUsed(slot)) continue;
            if (Process::Write8(slot + O_POSE, 0xAB)) ++n;
        }
        MessageBox(TITLE, "全妖怪の決めポーズを全開放しました (要検証, " + std::to_string(n) + " 体)。")();
    }

    MenuFolder *CreateMore2Folder(void)
    {
        MenuFolder *f = new MenuFolder("その他2 (真打)");
        *f += new MenuEntry("JP (ジャングルポイント) MAX",  nullptr, JpMaxCallback);
        *f += new MenuEntry("GP MAX",                       nullptr, GpMaxCallback);
        *f += new MenuEntry("レジェンド再開放",              nullptr, LegendReopenCallback);
        *f += new MenuEntry("ガシャ残り回数 99",             nullptr, Gasha99Callback);
        *f += new MenuEntry("全妖怪を本家にする",            nullptr, AllHonkeCallback);
        *f += new MenuEntry("全妖怪を元祖にする",            nullptr, AllGansoCallback);
        *f += new MenuEntry("全妖怪の物理技を変更 (ID入力)",  nullptr, AllPhysCallback);
        *f += new MenuEntry("全妖怪の決めポーズ全開放 (要検証)", nullptr, AllPoseCallback);
        *f += new MenuEntry("無限売却",                     SetGameFunc<16>);
        *f += new MenuEntry("チャット加速化",               SetGameFunc<17>);
        *f += new MenuEntry("FPS 60",                       SetGameFunc<18>);
        *f += new MenuEntry("1日1回イベント無限",           SetGameFunc<19>);
        return f;
    }

    // 毎フレーム: チェックが外れたセットを元に戻す (CheatsFrameCallback から呼ばれる)
    void MoreFrameCallback(void)
    {
        EditorUiFrame();
        if (g_osdOn && g_osdEntry && !g_osdEntry->IsActivated()) { OSD::Stop(EditorOsd); g_osdOn = false; }
        for (u32 i = 0; i < g_setCount; ++i)
        {
            PatchSet2 &ps = g_sets[i];
            if (ps.applied && ps.entry && !ps.entry->IsActivated())
                RestoreSet(ps);
        }
    }
}
