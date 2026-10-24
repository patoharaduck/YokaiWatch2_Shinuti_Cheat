#include "cheats.hpp"

// ============================================================
//  妖怪エディタ 画面版 (真打)
//   チェックを入れてメニューを閉じると、下画面にタッチ操作できるフォームを描画し、
//   上画面に選択中の妖怪の要約を表示する。
//   描画: OSD::Run のコールバック内で Screen に描く / 入力: GameFunc (毎フレーム) で Touch を見る
// ============================================================

namespace CTRPluginFramework
{
    static const char *TITLE = "Yokai Watch 2 Tools";

    static const u32 YW_BASE = 0x0870DBB0, YW_STRIDE = 0xC4, YW_COUNT = 0x1A1;
    enum {
        O_ID = 0x08, O_TECH1 = 0x2E, O_TECH2 = 0x32, O_TECH3 = 0x36, O_HPCUR = 0x3C, O_IV = 0x44,
        O_LV = 0x53, O_POSE = 0x55, O_ORIGIN = 0x59,
        O_HP = 0x64, O_STR = 0x66, O_SPR = 0x68, O_DEF = 0x6A, O_SPD = 0x6C, O_PHYS = 0x7C
    };

    // ---------- 状態 ----------
    static u32   g_uiSlot   = 0;      // 編集対象スロット (0 = 未選択)
    static u32   g_uiIndex  = 0;      // 表示用の番号
    static int   g_uiPage   = 0;      // 0..2
    static bool  g_uiOsdOn  = false;
    static bool  g_uiTouchWas = false;
    static MenuEntry *g_uiEntry = nullptr;
    static u32   g_uiFlash  = 0;      // 押した直後のハイライト用フレーム数
    static std::string g_uiMsg;       // 下部メッセージ

    static bool IsShinuchi(void) { return (u32)(Process::GetTitleID() & 0xFFFFFFFFULL) == 0x00155100; }

    static bool SlotUsed(u32 slot)
    {
        u16 hp = 0; u32 id = 0;
        return Process::Read16(slot + O_HP, hp) && hp != 0 && Process::Read32(slot + O_ID, id) && id != 0;
    }

    // 使用中スロットを index 方向に探す (dir = +1 / -1)。見つからなければ現状維持
    static void StepSlot(int dir)
    {
        u32 i = g_uiIndex;
        for (u32 n = 0; n < YW_COUNT; ++n)
        {
            i = (u32)(((int)i + dir + (int)YW_COUNT) % (int)YW_COUNT);
            u32 slot = YW_BASE + i * YW_STRIDE;
            if (SlotUsed(slot)) { g_uiIndex = i; g_uiSlot = slot; return; }
        }
    }

    // ---------- 描画部品 ----------
    struct UiRect { u32 x, y, w, h; };
    static bool Hit(const UiRect &r, u32 px, u32 py) { return px >= r.x && px < r.x + r.w && py >= r.y && py < r.y + r.h; }

    static const Color C_BG(0, 0, 0, 200);
    static const Color C_PANEL(30, 30, 40, 230);
    static const Color C_BOX(250, 250, 250, 255);
    static const Color C_BTN(70, 110, 200, 255);
    static const Color C_BTN2(120, 120, 130, 255);
    static const Color C_TXT(255, 255, 255, 255);
    static const Color C_TXTD(20, 20, 20, 255);
    static const Color C_ON(220, 60, 60, 255);

    static void Label(const Screen &s, const std::string &t, u32 x, u32 y) { s.DrawSysfont(t, x, y, C_TXT); }
    static void Box(const Screen &s, const UiRect &r, const std::string &t)
    {
        s.DrawRect(r.x, r.y, r.w, r.h, C_BOX, true);
        s.DrawSysfont(t, r.x + 4, r.y + 1, C_TXTD);
    }
    static void Button(const Screen &s, const UiRect &r, const std::string &t, bool alt = false)
    {
        s.DrawRect(r.x, r.y, r.w, r.h, alt ? C_BTN2 : C_BTN, true);
        s.DrawSysfont(t, r.x + 4, r.y + 1, C_TXT);
    }
    static void Check(const Screen &s, const UiRect &r, const std::string &t, bool on)
    {
        s.DrawRect(r.x, r.y, 14, 14, C_BOX, true);
        if (on) s.DrawRect(r.x + 3, r.y + 3, 8, 8, C_ON, true);
        s.DrawSysfont(t, r.x + 18, r.y - 1, C_TXT);
    }

    // ---------- レイアウト (下画面 320x240) ----------
    static const UiRect R_CLOSE = { 262, 3, 54, 16 };
    static const UiRect R_PREV  = { 4, 214, 40, 22 };
    static const UiRect R_NEXT  = { 276, 214, 40, 22 };
    static const UiRect R_SLOTM = { 200, 24, 26, 16 };   // 前の妖怪
    static const UiRect R_SLOTP = { 290, 24, 26, 16 };   // 次の妖怪
    // page 0
    static const UiRect R_LV    = { 100, 46, 50, 16 };
    static const UiRect R_PICK  = { 170, 44, 146, 20 };
    static const UiRect R_STAT[5] = { {90, 90, 70, 16}, {90, 110, 70, 16}, {90, 130, 70, 16}, {90, 150, 70, 16}, {90, 170, 70, 16} };
    static const UiRect R_HONKE = { 190, 92, 14, 14 };
    static const UiRect R_GANSO = { 190, 112, 14, 14 };
    static const UiRect R_HEAL  = { 190, 140, 120, 20 };
    static const UiRect R_ALL   = { 190, 166, 120, 20 };
    // page 1
    static const UiRect R_TECH[3] = { {120, 50, 50, 16}, {120, 70, 50, 16}, {120, 90, 50, 16} };
    static const UiRect R_PHYS  = { 120, 116, 90, 16 };
    static const UiRect R_TK[6] = { {80, 48, 236, 16}, {80, 70, 236, 16}, {80, 92, 236, 16}, {80, 114, 236, 16}, {80, 136, 236, 16}, {80, 158, 236, 16} };
    static const UiRect R_RAW[6] = { {120, 142, 90, 14}, {120, 158, 90, 14}, {120, 174, 90, 14}, {216, 142, 90, 14}, {216, 158, 90, 14}, {216, 174, 90, 14} };
    // page 2
    static const UiRect R_IV[5] = { {110, 50, 50, 16}, {110, 70, 50, 16}, {110, 90, 50, 16}, {110, 110, 50, 16}, {110, 130, 50, 16} };
    static const UiRect R_POSE  = { 180, 50, 130, 20 };
    static const UiRect R_TRAIN = { 180, 76, 130, 20 };
    static const UiRect R_SAVE[3] = { {10, 160, 90, 20}, {110, 160, 90, 20}, {210, 160, 90, 20} };
    static const UiRect R_LOAD[3] = { {10, 186, 90, 20}, {110, 186, 90, 20}, {210, 186, 90, 20} };

    static const char *STAT_LABEL[5] = { "HP:", "ちから:", "ようりょく:", "まもり:", "すばやさ:" };
    static const u32   STAT_OFF[5]   = { O_HP, O_STR, O_SPR, O_DEF, O_SPD };
    static const char *IV_LABEL[5]   = { "体力:", "ちから:", "ようりょく:", "まもり:", "すばやさ:" };

    static std::string U(u32 v) { return std::to_string(v); }
    static std::string H(u32 v) { char b[12]; snprintf(b, sizeof(b), "%08X", (unsigned)v); return std::string(b); }
    static u16 R16(u32 off) { u16 v = 0; Process::Read16(g_uiSlot + off, v); return v; }
    static u8  R8(u32 off)  { u8 v = 0;  Process::Read8(g_uiSlot + off, v);  return v; }
    static u32 R32(u32 off) { u32 v = 0; Process::Read32(g_uiSlot + off, v); return v; }

    // ---------- 描画 ----------
    static bool EditorUiDraw(const Screen &screen)
    {
        if (g_uiSlot == 0) return false;
        u32 id = R32(O_ID);
        const char *nm = YokaiNameFromId(id);
        std::string name = nm ? nm : "(不明)";

        if (screen.IsTop)
        {
            screen.DrawRect(20, 8, 360, 96, C_BG, true);
            Label(screen, "Yokai Editor", 28, 10);
            Label(screen, "妖怪: " + name + " (ID: " + H(id) + ")", 28, 28);
            Label(screen, "レベル: " + U(R8(O_LV)) + "   HP: " + U(R16(O_HPCUR)) + "/" + U(R16(O_HP)), 28, 44);
            Label(screen, "ちから: " + U(R16(O_STR)) + "  ようりょく: " + U(R16(O_SPR)) + "  まもり: " + U(R16(O_DEF)) + "  すばやさ: " + U(R16(O_SPD)), 28, 60);
            Label(screen, "個体値: " + U(R8(O_IV)) + "/" + U(R8(O_IV+1)) + "/" + U(R8(O_IV+2)) + "/" + U(R8(O_IV+3)) + "/" + U(R8(O_IV+4))
                          + "   技Lv: " + U(R8(O_TECH1)) + "/" + U(R8(O_TECH2)) + "/" + U(R8(O_TECH3)), 28, 76);
            return true;
        }

        // ---- 下画面 ----
        screen.DrawRect(0, 0, 320, 240, C_PANEL, true);
        Label(screen, "妖怪エディタ  " + U(g_uiPage + 1) + "/3", 6, 2);
        Button(screen, R_CLOSE, "閉じる", true);
        Label(screen, "妖怪: " + name, 6, 22);
        Button(screen, R_SLOTM, "<", true);
        Label(screen, "#" + U(g_uiIndex + 1), 232, 24);
        Button(screen, R_SLOTP, ">", true);

        if (g_uiPage == 0)
        {
            Label(screen, "レベル:", 10, 46);
            Box(screen, R_LV, U(R8(O_LV)));
            Button(screen, R_PICK, "一覧から種族変更");
            Label(screen, "ステータス", 10, 72);
            for (u32 i = 0; i < 5; ++i) { Label(screen, STAT_LABEL[i], 10, 90 + i * 20); Box(screen, R_STAT[i], U(R16(STAT_OFF[i]))); }
            Label(screen, "フラグ", 190, 72);
            u8 org = R8(O_ORIGIN);
            Check(screen, R_HONKE, "本家の妖怪", org == 0x10);
            Check(screen, R_GANSO, "元祖の妖怪", org == 0x08);
            Button(screen, R_HEAL, "HP全回復");
            Button(screen, R_ALL,  "全ステ MAX(65535)");
        }
        else if (g_uiPage == 1)
        {
            Label(screen, "技", 10, 30);
            for (u32 i = 0; i < 3; ++i) { Label(screen, "技レベル" + U(i + 1) + ":", 10, 50 + i * 20); Box(screen, R_TECH[i], U(R8(O_TECH1 + i * 4))); }
            Label(screen, "技・スキル (タッチで変更)", 10, 30);
            for (u32 k = 0; k < 6; ++k)
            {
                if (k == 3) continue;   // ガードは全員共通
                u32 v = R32(TechKindOffset(k));
                Label(screen, std::string(TechKindName(k)) + ":", 10, 50 + k * 22);
                Box(screen, R_TK[k], TechLabel(k, v));
            }
            Label(screen, "※一覧は定義表と手持ちから自動生成。ptr が無い技は変更不可", 10, 200);
        }
        else
        {
            Label(screen, "個体値", 10, 30);
            for (u32 i = 0; i < 5; ++i) { Label(screen, IV_LABEL[i], 10, 50 + i * 20); Box(screen, R_IV[i], U(R8(O_IV + i))); }
            Button(screen, R_POSE,  "決めポーズ全開放");
            Button(screen, R_TRAIN, "肉体改造値MAX");
            Label(screen, "妖怪データ (SD)", 10, 144);
            for (u32 i = 0; i < 3; ++i) { Button(screen, R_SAVE[i], "保存 " + U(i + 1)); Button(screen, R_LOAD[i], "リストア " + U(i + 1), true); }
        }

        Button(screen, R_PREV, "<<", true);
        Button(screen, R_NEXT, ">>", true);
        if (!g_uiMsg.empty()) Label(screen, g_uiMsg, 50, 218);
        return true;
    }

    // ---------- 入力 ----------
    static void EditU8(u32 off, u8 maxV, const std::string &what)
    {
        u8 v = R8(off);
        Keyboard kb(what + "\n10進数で入力 (0〜" + U(maxV) + ")  現在: " + U(v));
        kb.IsHexadecimal(false);   // CTRPF の数値キーボードは既定が16進なので10進に切替
        if (kb.Open(v, v) == 0) { if (v > maxV) v = maxV; Process::Write8(g_uiSlot + off, v); g_uiMsg = what + " を変更しました"; }
    }
    static void EditU16(u32 off, u16 maxV, const std::string &what, bool alsoCur = false)
    {
        u16 v = R16(off);
        Keyboard kb(what + "\n10進数で入力 (0〜" + U(maxV) + ")  現在: " + U(v));
        kb.IsHexadecimal(false);
        if (kb.Open(v, v) == 0)
        {
            if (v > maxV) v = maxV;
            Process::Write16(g_uiSlot + off, v);
            if (alsoCur) Process::Write16(g_uiSlot + O_HPCUR, v);
            g_uiMsg = what + " を変更しました";
        }
    }
    static void EditHex32(u32 off, const std::string &what)
    {
        u32 v = R32(off);
        Keyboard kb(what + " (16進)");
        kb.IsHexadecimal(true);
        if (kb.Open(v, v) == 0) { Process::Write32(g_uiSlot + off, v); g_uiMsg = what + " を変更しました"; }
    }

    static std::string SavePath(u32 n) { return "/YW2Tools/yokai_" + std::to_string(n) + ".bin"; }
    static void SaveSlot(u32 n)
    {
        u8 buf[YW_STRIDE];
        for (u32 i = 0; i < YW_STRIDE; ++i) Process::Read8(g_uiSlot + i, buf[i]);
        Directory::Create("/YW2Tools");
        File f;
        if (File::Open(f, SavePath(n), File::WRITE | File::CREATE | File::TRUNCATE) != 0) { g_uiMsg = "保存に失敗 (open)"; return; }
        int r = f.Write(buf, YW_STRIDE); f.Close();
        g_uiMsg = (r == 0) ? "スロット " + U(n) + " に保存しました" : "保存に失敗しました";
    }
    static void LoadSlot(u32 n)
    {
        if (!File::Exists(SavePath(n))) { g_uiMsg = "スロット " + U(n) + " は空です"; return; }
        u8 buf[YW_STRIDE];
        File f;
        if (File::Open(f, SavePath(n), File::READ) != 0) { g_uiMsg = "読込に失敗 (open)"; return; }
        int r = f.Read(buf, YW_STRIDE); f.Close();
        if (r != 0) { g_uiMsg = "読込に失敗しました"; return; }
        for (u32 i = 0; i < YW_STRIDE; ++i) Process::Write8(g_uiSlot + i, buf[i]);
        g_uiMsg = "スロット " + U(n) + " をリストアしました";
    }

    static void HandleTouch(u32 x, u32 y)
    {
        if (Hit(R_CLOSE, x, y)) { if (g_uiEntry) g_uiEntry->Disable(); return; }
        if (Hit(R_PREV, x, y))  { g_uiPage = (g_uiPage + 2) % 3; return; }
        if (Hit(R_NEXT, x, y))  { g_uiPage = (g_uiPage + 1) % 3; return; }
        if (Hit(R_SLOTM, x, y)) { StepSlot(-1); g_uiMsg.clear(); return; }
        if (Hit(R_SLOTP, x, y)) { StepSlot(+1); g_uiMsg.clear(); return; }

        if (g_uiPage == 0)
        {
            if (Hit(R_LV, x, y))   { EditU8(O_LV, 255, "レベル"); return; }
            if (Hit(R_PICK, x, y))
            {
                u32 id = 0; std::string name;
                if (PickYokaiId(id, name)) { Process::Write32(g_uiSlot + O_ID, id); g_uiMsg = name + " に変更しました"; }
                return;
            }
            for (u32 i = 0; i < 5; ++i)
                if (Hit(R_STAT[i], x, y)) { EditU16(STAT_OFF[i], 65535, STAT_LABEL[i], i == 0); return; }
            if (Hit(R_HONKE, x, y)) { Process::Write8(g_uiSlot + O_ORIGIN, 0x10); g_uiMsg = "本家の妖怪にしました"; return; }
            if (Hit(R_GANSO, x, y)) { Process::Write8(g_uiSlot + O_ORIGIN, 0x08); g_uiMsg = "元祖の妖怪にしました"; return; }
            if (Hit(R_HEAL, x, y))  { Process::Write16(g_uiSlot + O_HPCUR, R16(O_HP)); g_uiMsg = "HPを全回復しました"; return; }
            if (Hit(R_ALL, x, y))
            {
                for (u32 i = 0; i < 5; ++i) Process::Write16(g_uiSlot + STAT_OFF[i], 65535);
                Process::Write16(g_uiSlot + O_HPCUR, 65535);
                g_uiMsg = "全ステータスを 65535 にしました"; return;
            }
        }
        else if (g_uiPage == 1)
        {
            for (u32 i = 0; i < 3; ++i)
                if (Hit(R_TECH[i], x, y)) { EditU8(O_TECH1 + i * 4, 10, "技レベル" + U(i + 1)); return; }
            for (u32 k = 0; k < 6; ++k)
            {
                if (k == 3) continue;
                if (Hit(R_TK[k], x, y))
                {
                    u32 id = 0, ptr = 0; std::string label;
                    if (PickTech(k, id, ptr, label) && ApplyTech(g_uiSlot, k, id, ptr))
                        g_uiMsg = std::string(TechKindName(k)) + " を " + label + " にしました";
                    return;
                }
            }
        }
        else
        {
            for (u32 i = 0; i < 5; ++i)
                if (Hit(R_IV[i], x, y)) { EditU8(O_IV + i, 127, std::string("個体値 ") + IV_LABEL[i]); return; }
            if (Hit(R_POSE, x, y))  { Process::Write8(g_uiSlot + O_POSE, 0xAB); g_uiMsg = "決めポーズを全開放しました"; return; }
            if (Hit(R_TRAIN, x, y))
            {
                Process::Write32(g_uiSlot + 0x44, 0x7F7F7F7F); Process::Write32(g_uiSlot + 0x48, 0x7F7F7F7F);
                Process::Write32(g_uiSlot + 0x4C, 0x7A7F7F7F); Process::Write32(g_uiSlot + 0x50, 0xFF7F7F7F);
                g_uiMsg = "肉体改造値をMAXにしました"; return;
            }
            for (u32 i = 0; i < 3; ++i)
            {
                if (Hit(R_SAVE[i], x, y)) { SaveSlot(i + 1); return; }
                if (Hit(R_LOAD[i], x, y)) { LoadSlot(i + 1); return; }
            }
        }
    }

    // チェック ON の間、毎フレーム
    static void EditorUiGameFunc(MenuEntry *entry)
    {
        g_uiEntry = entry;
        if (!IsShinuchi()) { entry->Disable(); return; }

        if (entry->WasJustActivated() || g_uiSlot == 0)
        {
            // 最初の使用中スロットを選ぶ
            g_uiIndex = YW_COUNT - 1;
            StepSlot(+1);
            if (g_uiSlot == 0) { entry->Disable(); return; }
            g_uiPage = 0; g_uiMsg = "項目をタッチで編集できます";
        }
        if (!g_uiOsdOn) { OSD::Run(EditorUiDraw); g_uiOsdOn = true; }

        bool down = Touch::IsDown();
        if (down && !g_uiTouchWas)
        {
            UIntVector p = Touch::GetPosition();
            HandleTouch(p.x, p.y);
        }
        g_uiTouchWas = down;
    }

    // チェック OFF になったら OSD を止める (MoreFrameCallback から毎フレーム呼ばれる)
    void EditorUiFrame(void)
    {
        if (g_uiOsdOn && g_uiEntry && !g_uiEntry->IsActivated()) { OSD::Stop(EditorUiDraw); g_uiOsdOn = false; g_uiTouchWas = false; }
    }

    // 妖怪エディタ(メニュー版)で選んだスロットを画面版にも反映させたい場合に使う
    void EditorUiSetSlot(u32 slot, u32 index) { g_uiSlot = slot; g_uiIndex = index; }

    MenuEntry *CreateEditorUiEntry(void)
    {
        return new MenuEntry("エディタ画面を開く (下画面タッチ操作)", EditorUiGameFunc,
                             "チェックを入れてメニューを閉じると、下画面にエディタ画面が出ます。「閉じる」で終了。");
    }
}
