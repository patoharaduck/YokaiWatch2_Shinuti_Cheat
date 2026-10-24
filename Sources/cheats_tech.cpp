#include "cheats.hpp"

// ============================================================
//  技・スキル (実機ダンプの解析結果に基づく)
//
//  手持ち妖怪スロット (0xC4):
//    +0x7C 攻撃技  { u32 id; u32 ptr(技定義へ); u32 値 }
//    +0x88 術      { id, ptr, 値 }
//    +0x94 必殺技  { id, ptr, 値 }
//    +0xA0 ガード  { id (全員 61930F85), ptr=0, 0 }
//    +0xAC とりつき{ id, ptr, 値 }
//    +0xB8 スキル  { id, 0, 0 }
//  妖怪定義表 (0x84): +0x24 攻撃技 +0x28 術 +0x2C 必殺技 +0x30 ガード +0x34 とりつき +0x38 スキル
//  技の ptr は「同じ技を持つ手持ち妖怪」から流用する (id→ptr は 1 対 1 だった)
// ============================================================

namespace CTRPluginFramework
{
    static const char *TITLE = "Yokai Watch 2 Tools";
    static const u32 YW_BASE = 0x0870DBB0, YW_STRIDE = 0xC4, YW_COUNT = 0x1A1;

    static const char *KIND_NAME[6] = { "攻撃技", "術", "必殺技", "ガード", "とりつき", "スキル" };
    static const u32   KIND_OFF[6]  = { 0x7C, 0x88, 0x94, 0xA0, 0xAC, 0xB8 };

    const char *TechKindName(u32 kind) { return kind < 6 ? KIND_NAME[kind] : ""; }
    u32         TechKindOffset(u32 kind) { return kind < 6 ? KIND_OFF[kind] : 0; }

    struct TechEntry { u32 id; u32 ptr; std::string label; };
    static std::vector<TechEntry> g_catalog[6];
    static bool g_catalogBuilt = false;

    static bool PtrOk(u32 p) { return p >= 0x08000000 && p < 0x10000000; }
    static bool Deref(u32 a, u32 &o) { return Process::Read32(a, o) && PtrOk(o); }

    static bool GetDefTable(u32 &base, u32 &end)
    {
        u32 p = 0, len = 0;
        if (!Deref(0x0080D280, p) || !Deref(p + 0x10, p) || !Deref(p + 0x14, base)) return false;
        if (!Process::Read32(base - 0x10, len) || len == 0 || len > 0x100000) return false;
        end = base + len;
        return true;
    }

    static TechEntry *FindEntry(u32 kind, u32 id)
    {
        for (size_t i = 0; i < g_catalog[kind].size(); ++i)
            if (g_catalog[kind][i].id == id) return &g_catalog[kind][i];
        return nullptr;
    }

    // 定義表 (全種族) + 手持ち (ptr) からカタログを作る
    bool BuildTechCatalog(void)
    {
        for (u32 k = 0; k < 6; ++k) g_catalog[k].clear();
        u32 base = 0, end = 0;
        if (!GetDefTable(base, end)) return false;

        for (u32 def = base; def < end; def += 0x84)
        {
            u32 yid = 0; Process::Read32(def, yid);
            if (yid == 0) continue;
            const char *yn = YokaiNameFromId(yid);
            for (u32 k = 0; k < 6; ++k)
            {
                u32 tid = 0; Process::Read32(def + 0x24 + k * 4, tid);
                if (tid == 0 || FindEntry(k, tid)) continue;
                TechEntry e; e.id = tid; e.ptr = 0;
                const char *sn = BuiltinTechName(k, tid);
                if (!sn && k == 5) sn = SkillNameFromId(tid);
                if (sn) e.label = sn;
                else
                {
                    char b[16]; snprintf(b, sizeof(b), "%08X", (unsigned)tid);
                    e.label = std::string(yn ? yn : b) + " の " + KIND_NAME[k];
                }
                g_catalog[k].push_back(e);
            }
        }
        // 手持ちから ptr を拾う
        for (u32 i = 0; i < YW_COUNT; ++i)
        {
            u32 slot = YW_BASE + i * YW_STRIDE, yid = 0;
            if (!Process::Read32(slot + 8, yid) || yid == 0) continue;
            for (u32 k = 0; k < 6; ++k)
            {
                u32 tid = 0, ptr = 0;
                Process::Read32(slot + KIND_OFF[k], tid);
                Process::Read32(slot + KIND_OFF[k] + 4, ptr);
                TechEntry *e = tid ? FindEntry(k, tid) : nullptr;
                if (e && e->ptr == 0 && PtrOk(ptr)) e->ptr = ptr;
            }
        }
        g_catalogBuilt = true;
        return true;
    }

    static bool EnsureCatalog(void)
    {
        if (g_catalogBuilt) return true;
        if (!BuildTechCatalog()) { MessageBox(TITLE, "技のカタログを作れませんでした。\nゲーム中(マップ上)で実行してください。")(); return false; }
        return true;
    }

    std::string TechLabel(u32 kind, u32 id)
    {
        if (kind >= 6) return "";
        if (g_catalogBuilt)
        {
            TechEntry *e = FindEntry(kind, id);
            if (e) return e->label;
        }
        { const char *n = BuiltinTechName(kind, id); if (n) return n; }
        if (kind == 5) { const char *n = SkillNameFromId(id); if (n) return n; }
        char b[16]; snprintf(b, sizeof(b), "%08X", (unsigned)id);
        return b;
    }

    // 一覧から技を選ぶ。ptr は手持ちに同じ技があればそこから、無ければ 0 (ガード/スキルは常に 0)
    bool PickTech(u32 kind, u32 &outId, u32 &outPtr, std::string &outLabel)
    {
        if (kind >= 6 || !EnsureCatalog()) return false;
        std::vector<TechEntry> &cat = g_catalog[kind];
        if (cat.empty()) { MessageBox(TITLE, "候補がありません。")(); return false; }
        std::vector<std::string> names;
        for (size_t i = 0; i < cat.size(); ++i) names.push_back(cat[i].label);
        Keyboard kb(std::string(KIND_NAME[kind]) + " を選んでください (" + std::to_string(cat.size()) + " 件)");
        kb.Populate(names);
        int k = kb.Open();
        if (k < 0 || (size_t)k >= cat.size()) return false;
        outId = cat[k].id; outPtr = cat[k].ptr; outLabel = cat[k].label;
        return true;
    }

    // スロットの技を書き換える (id と ptr)。ptr が不明な技 (手持ちに無い) は書けない
    bool ApplyTech(u32 slot, u32 kind, u32 id, u32 ptr)
    {
        if (kind >= 6) return false;
        u32 off = KIND_OFF[kind];
        bool needPtr = (kind != 3 && kind != 5);
        if (needPtr && ptr == 0)
        {
            MessageBox(TITLE, "この技の定義ポインタが手持ちの妖怪から見つかりませんでした。\nその技を持つ妖怪を1体入手してから再度お試しください。")();
            return false;
        }
        bool ok = Process::Write32(slot + off, id);
        ok = Process::Write32(slot + off + 4, needPtr ? ptr : 0) && ok;
        return ok;
    }

    void RebuildTechCatalogCallback(MenuEntry *entry)
    {
        if (BuildTechCatalog())
        {
            std::string s = "技カタログを作成しました。\n";
            for (u32 k = 0; k < 6; ++k) s += std::string(KIND_NAME[k]) + ": " + std::to_string(g_catalog[k].size()) + " 件\n";
            MessageBox(TITLE, s)();
        }
        else MessageBox(TITLE, "作成に失敗しました。ゲーム中(マップ上)で実行してください。")();
    }
}
