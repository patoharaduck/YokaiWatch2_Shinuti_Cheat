#include "cheats.hpp"

// ============================================================
//  全妖怪入手 / 全アイテム入手 (真打)  ― れれれ氏の ASM コードを C++ に忠実に移植
//
//  ● アイテム (たべもの/いきもの/道具, 装備, だいじなもの)
//    mgr     = [[0x0080D1E8] + 0x10]
//    定義表  = [[[0x0080D2B0] + 0x10] - 0x4C] .. +0x20F0, 1件 8 バイト { u32 id; u16 ?; u16 category; }
//    道具    : category 10/20/30  slot = mgr+0xDC + n*0x10 { u32 vt=0x007F613C; u16 idx; u16 idx+1; u32 id; u32 count }
//              件数 [mgr+0x10],[mgr+0x14]  所持ビットマップ mgr+0x1BE4
//    装備    : category 0x32      slot = mgr+0x1C44 + n*0x14 (idx は 0x1000 始まり) 件数 [mgr+0x3C],[mgr+0x40]  ビットマップ mgr+0x2374
//    だいじ  : category 0x3C      slot = mgr+0x23A8 + n*0x0C { vt=0x007F6164; u16 idx(0x2000+n); u16 2+n; u32 id }
//              件数 [mgr+0x68],[mgr+0x6C]  ビットマップ mgr+0x2C40
//  ● 妖怪
//    mgr     = [[0x0080D1D8] + 0x10]   slot表 = [mgr+8] (=0x0870DBB0)   ビットマップ = [mgr+0xC]
//    定義表  = [[[0x0080D280] + 0x10] + 0x14], 終端 = 定義表 + [定義表 - 0x10], 1件 0x84 バイト
//    条件    : [def+0x64](u16) != 0 && ([def+0x60](u8) & 4) && [def+0x20] != 0
//    slot    : { u32 vt=0x007F3048; u16 idx; u16 idx+1; u32 id=[def]; ... u8 level(+0x53)=99 }
//    ついでに [[[0x0080D1E0]+0x10] + 0x314] に idx,idx+1 を u16 で並べる。[[[0x0080D1F8]+0x10] + 0x158] = 0
//  どちらも「テーブル全体を定義表の順に作り直す」ため、既存データは上書きされる。実行前にバックアップ必須。
// ============================================================

namespace CTRPluginFramework
{
    static const char *TITLE = "Yokai Watch 2 Tools";
    static bool RequireShinuchi(void)
    {
        if ((u32)(Process::GetTitleID() & 0xFFFFFFFFULL) != 0x00155100) { MessageBox(TITLE, "この機能は「真打」専用です。")(); return false; }
        return true;
    }
    static bool PtrOk(u32 p) { return p >= 0x08000000 && p < 0x10000000; }
    static bool Deref(u32 addr, u32 &out) { return Process::Read32(addr, out) && PtrOk(out); }

    static bool GetMgr(u32 ptrAddr, u32 &mgr)
    {
        u32 p = 0;
        return Deref(ptrAddr, p) && Deref(p + 0x10, mgr);
    }

    static void SetBitmap(u32 base, u32 count)
    {
        // ASM: for (i = 0; i != count; ++i) word[i>>5] |= 1 << (i & 31)   ※ 事前クリアはしない (元コード準拠)
        for (u32 i = 0; i < count; ++i)
        {
            u32 a = base + (i >> 5) * 4, v = 0;
            Process::Read32(a, v);
            Process::Write32(a, v | (1u << (i & 31)));
        }
    }

    // ---------------- アイテム ----------------
    static bool GetItemDefTable(u32 &base, u32 &end)
    {
        u32 p = 0, q = 0;
        if (!Deref(0x0080D2B0, p) || !Deref(p + 0x10, q)) return false;
        if (!Process::Read32(q - 0x4C, base) || !PtrOk(base)) return false;
        end = base + 0x20F0;
        return true;
    }

    struct ItemKind { const char *name; u32 slotOff; u32 stride; u32 vt; u32 idxBase; u32 idx2Start; u32 cntOff; u32 bmpOff; bool hasCount; };
    static const ItemKind K_TOOL  = { "たべもの・いきもの・道具", 0x00DC, 0x10, 0x007F613C, 0x0000, 1, 0x10, 0x1BE4, true  };
    static const ItemKind K_EQUIP = { "装備",                     0x1C44, 0x14, 0x007F613C, 0x1000, 1, 0x3C, 0x2374, true  };
    static const ItemKind K_KEY   = { "だいじなもの",             0x23A8, 0x0C, 0x007F6164, 0x2000, 2, 0x68, 0x2C40, false };

    static bool CatMatch(const ItemKind &k, u32 cat)
    {
        if (&k == &K_TOOL)  return cat == 10 || cat == 20 || cat == 30;
        if (&k == &K_EQUIP) return cat == 0x32;
        return cat == 0x3C;
    }

    static void BuildItems(const ItemKind &k)
    {
        u32 mgr = 0, base = 0, end = 0;
        if (!GetMgr(0x0080D1E8, mgr)) { MessageBox(TITLE, "アイテム管理情報を取得できませんでした。\nゲーム中(マップ上)で実行してください。")(); return; }
        if (!GetItemDefTable(base, end)) { MessageBox(TITLE, "アイテム定義表を取得できませんでした。")(); return; }

        // ポインタ値はゲームのバージョンで変わる (wiki: 007F613C / 実機: 007F5C9C) ので、
        // 固定値ではなく「今持っている 1 件目」の値を流用する。0 件だと流用元が無いので中止。
        u32 curCount = 0, vt0 = 0;
        Process::Read32(mgr + k.cntOff, curCount);
        Process::Read32(mgr + k.slotOff, vt0);
        if (&k == &K_EQUIP && (curCount == 0 || vt0 < 0x00100000 || vt0 >= 0x00800000))
        {
            // 装備は道具と同じポインタ値 (wiki コードでは両方 007F613C) なので、道具の 1 件目から流用する
            u32 tc = 0, tv = 0;
            Process::Read32(mgr + K_TOOL.cntOff, tc);
            Process::Read32(mgr + K_TOOL.slotOff, tv);
            if (tc != 0 && tv >= 0x00100000 && tv < 0x00800000) { vt0 = tv; curCount = 1; }
        }
        if (&k == &K_KEY && (curCount == 0 || vt0 < 0x00100000 || vt0 >= 0x00800000))
        {
            // だいじなものは wiki コードで 道具 + 0x28 (007F6164 - 007F613C) だったので同じ差分で推定
            u32 tc = 0, tv = 0;
            Process::Read32(mgr + K_TOOL.cntOff, tc);
            Process::Read32(mgr + K_TOOL.slotOff, tv);
            if (tc != 0 && tv >= 0x00100000 && tv < 0x00800000) { vt0 = tv + 0x28; curCount = 1; }
        }
        if (curCount == 0 || vt0 < 0x00100000 || vt0 >= 0x00800000)
        {
            char buf[96]; snprintf(buf, sizeof(buf), "1件目のポインタを流用できません (所持 %u 件, vt=%08X)。\nこの種類を1つ以上持った状態で実行してください。", (unsigned)curCount, (unsigned)vt0);
            MessageBox(TITLE, std::string(k.name) + ": " + buf)(); return;
        }
        const u32 vt = vt0;

        u32 n = 0;
        for (u32 def = base; def < end; def += 8)
        {
            u16 cat = 0; u32 id = 0;
            if (!Process::Read16(def + 6, cat) || !CatMatch(k, cat)) continue;
            Process::Read32(def, id);
            u32 slot = mgr + k.slotOff + n * k.stride;
            Process::Write32(slot + 0, vt);
            Process::Write16(slot + 4, (u16)(k.idxBase + n));
            Process::Write16(slot + 6, (u16)(k.idx2Start + n));
            Process::Write32(slot + 8, id);
            if (k.hasCount) Process::Write32(slot + 0xC, 99);
            ++n;
        }
        Process::Write32(mgr + k.cntOff, n);
        Process::Write32(mgr + k.cntOff + 4, k.idx2Start + n);
        SetBitmap(mgr + k.bmpOff, n);
        MessageBox(TITLE, std::string(k.name) + " を全部入手しました (" + std::to_string(n) + " 種)。")();
    }

    static void AllToolsCallback(MenuEntry *e) { if (RequireShinuchi()) BuildItems(K_TOOL); }
    static void AllEquipCallback(MenuEntry *e) { if (RequireShinuchi()) BuildItems(K_EQUIP); }
    static void AllKeyCallback(MenuEntry *e)   { if (RequireShinuchi()) BuildItems(K_KEY); }

    // ---------------- 妖怪 ----------------
    static const u32 YW_BASE = 0x0870DBB0, YW_STRIDE = 0xC4, YW_COUNT = 0x1A1;

    static void AllYokaiCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 mgr = 0, table = 0, bmp = 0, defp = 0, defBase = 0, defLen = 0, listMgr = 0, flagMgr = 0;
        if (!GetMgr(0x0080D1D8, mgr) || !Deref(mgr + 8, table) || !Deref(mgr + 0xC, bmp))
        { MessageBox(TITLE, "妖怪管理情報を取得できませんでした。\nゲーム中(マップ上)で実行してください。")(); return; }
        if (table != YW_BASE)
        {
            char buf[64]; snprintf(buf, sizeof(buf), "妖怪テーブルが想定と違います (%08X)。中止しました。", (unsigned)table);
            MessageBox(TITLE, buf)(); return;
        }
        if (!Deref(0x0080D280, defp) || !Deref(defp + 0x10, defp) || !Deref(defp + 0x14, defBase) || !Process::Read32(defBase - 0x10, defLen))
        { MessageBox(TITLE, "妖怪定義表を取得できませんでした。")(); return; }
        u32 defEnd = defBase + defLen;
        if (defLen == 0 || defLen > 0x100000) { MessageBox(TITLE, "妖怪定義表の長さが不正です。中止しました。")(); return; }
        GetMgr(0x0080D1E0, listMgr);
        GetMgr(0x0080D1F8, flagMgr);

        u32 curCount = 0, vt0 = 0;
        Process::Read32(mgr + 0x10, curCount);
        Process::Read32(table, vt0);
        if (curCount == 0 || vt0 < 0x00100000 || vt0 >= 0x00800000)
        {
            char buf[96]; snprintf(buf, sizeof(buf), "1体目のポインタを流用できません (所持 %u 体, vt=%08X)。中止しました。", (unsigned)curCount, (unsigned)vt0);
            MessageBox(TITLE, buf)(); return;
        }
        const u32 vt = vt0;

        u32 n = 0, listPos = listMgr ? listMgr + 0x314 : 0;
        for (u32 def = defBase; def < defEnd; def += 0x84)
        {
            u16 f64 = 0; u8 f60 = 0; u32 f20 = 0, id = 0;
            Process::Read16(def + 0x64, f64);
            if (f64 == 0) continue;
            Process::Read8(def + 0x60, f60);
            if (!(f60 & 4)) continue;
            Process::Read32(def + 0x20, f20);
            if (f20 == 0) continue;
            if (n >= YW_COUNT) break;
            Process::Read32(def, id);
            u32 slot = table + n * YW_STRIDE;
            Process::Write32(slot + 0, vt);
            Process::Write16(slot + 4, (u16)n);
            Process::Write16(slot + 6, (u16)(n + 1));
            Process::Write32(slot + 8, id);
            Process::Write8(slot + 0x53, 99);
            if (listPos) { Process::Write16(listPos, (u16)n); Process::Write16(listPos + 2, (u16)(n + 1)); listPos += 4; }
            ++n;
        }
        Process::Write32(mgr + 0x10, n);
        Process::Write32(mgr + 0x14, n + 1);
        SetBitmap(bmp, n);
        if (flagMgr) Process::Write8(flagMgr + 0x158, 0);
        MessageBox(TITLE, "全妖怪入手: " + std::to_string(n) + " 体を作成しました。\n"
                          "※メダルアプリを開く前に、にっきで記録→タイトルに戻る→同じデータを始めてもう一度セーブしてください。")();
    }

    // ---------------- バックアップ / リストア ----------------
    static const u32 YOKAI_DUMP_SIZE = YW_COUNT * YW_STRIDE;
    static const u32 ITEM_DUMP_SIZE  = 0x2E00;

    static bool DumpRegion(u32 addr, u32 size, const std::string &path)
    {
        Directory::Create("/YW2Tools");
        File f;
        if (File::Open(f, path, File::WRITE | File::CREATE | File::TRUNCATE) != 0) return false;
        u8 buf[256];
        for (u32 off = 0; off < size; off += 256)
        {
            u32 n = (size - off < 256) ? size - off : 256;
            for (u32 i = 0; i < n; ++i) Process::Read8(addr + off + i, buf[i]);
            if (f.Write(buf, n) != 0) { f.Close(); return false; }
        }
        f.Close();
        return true;
    }
    static bool LoadRegion(u32 addr, u32 size, const std::string &path)
    {
        if (!File::Exists(path)) return false;
        File f;
        if (File::Open(f, path, File::READ) != 0) return false;
        u8 buf[256];
        for (u32 off = 0; off < size; off += 256)
        {
            u32 n = (size - off < 256) ? size - off : 256;
            if (f.Read(buf, n) != 0) { f.Close(); return false; }
            for (u32 i = 0; i < n; ++i) Process::Write8(addr + off + i, buf[i]);
        }
        f.Close();
        return true;
    }

    static void BackupCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 mgr = 0, imgr = 0, bmp = 0;
        bool y = GetMgr(0x0080D1D8, mgr) && DumpRegion(YW_BASE, YOKAI_DUMP_SIZE, "/YW2Tools/backup_yokai.bin")
                 && DumpRegion(mgr, 0x20, "/YW2Tools/backup_yokai_mgr.bin")
                 && Deref(mgr + 0xC, bmp) && DumpRegion(bmp, 0x40, "/YW2Tools/backup_yokai_bmp.bin");
        bool i = GetMgr(0x0080D1E8, imgr) && DumpRegion(imgr, ITEM_DUMP_SIZE, "/YW2Tools/backup_items.bin");
        MessageBox(TITLE, std::string("バックアップ: 妖怪 ") + (y ? "OK" : "失敗") + " / アイテム " + (i ? "OK" : "失敗") + "\n/YW2Tools/backup_*.bin")();
    }
    static void RestoreCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 mgr = 0, imgr = 0, bmp = 0;
        bool y = GetMgr(0x0080D1D8, mgr) && LoadRegion(YW_BASE, YOKAI_DUMP_SIZE, "/YW2Tools/backup_yokai.bin")
                 && LoadRegion(mgr, 0x20, "/YW2Tools/backup_yokai_mgr.bin")
                 && Deref(mgr + 0xC, bmp) && LoadRegion(bmp, 0x40, "/YW2Tools/backup_yokai_bmp.bin");
        bool i = GetMgr(0x0080D1E8, imgr) && LoadRegion(imgr, ITEM_DUMP_SIZE, "/YW2Tools/backup_items.bin");
        MessageBox(TITLE, std::string("リストア: 妖怪 ") + (y ? "OK" : "失敗/未保存") + " / アイテム " + (i ? "OK" : "失敗/未保存"))();
    }

    static void ShowMgrCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 mgr = 0, table = 0, count = 0, imgr = 0, icount = 0, ecount = 0, kcount = 0, vt0 = 0, dbase = 0, dend = 0;
        bool y = GetMgr(0x0080D1D8, mgr) && Process::Read32(mgr + 8, table) && Process::Read32(mgr + 0x10, count);
        bool i = GetMgr(0x0080D1E8, imgr);
        if (i) { Process::Read32(imgr + 0x10, icount); Process::Read32(imgr + 0x3C, ecount); Process::Read32(imgr + 0x68, kcount); Process::Read32(imgr + 0xDC, vt0); }
        bool d = GetItemDefTable(dbase, dend);
        char buf[320];
        snprintf(buf, sizeof(buf), "妖怪: mgr %08X table %08X 所持 %u\nアイテム: mgr %08X 道具 %u 装備 %u だいじ %u\nslot0 vt %08X (1件目から流用)\n定義表 %08X%s",
                 (unsigned)mgr, (unsigned)table, (unsigned)count, (unsigned)imgr, (unsigned)icount, (unsigned)ecount, (unsigned)kcount,
                 (unsigned)vt0, (unsigned)dbase, d ? "" : " (取得失敗)");
        MessageBox(TITLE, std::string(y ? "" : "(妖怪の管理情報が取得できません)\n") + buf)();
    }


    // ---------------- 解析用: 魂テーブル ----------------
    // 「全魂レベル10」コード (28726212, stride 0x14, 100件) → レベルは mgr+0x2C8E 付近。スロット先頭やカテゴリを特定するための表示。
    static void SoulDiagCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 mgr = 0, base = 0, end = 0;
        if (!GetMgr(0x0080D1E8, mgr)) { MessageBox(TITLE, "アイテム管理情報を取得できませんでした。")(); return; }

        // 1) 定義表のカテゴリ別件数 (魂のカテゴリ番号を探す)
        std::string out = "定義表 カテゴリ別件数:\n";
        if (GetItemDefTable(base, end))
        {
            u32 cats[64] = {0}, cnts[64] = {0}, ncat = 0;
            for (u32 def = base; def < end; def += 8)
            {
                u16 c = 0; Process::Read16(def + 6, c);
                u32 k = 0;
                for (; k < ncat; ++k) if (cats[k] == c) break;
                if (k == ncat && ncat < 64) { cats[ncat] = c; cnts[ncat] = 0; ++ncat; }
                if (k < 64) ++cnts[k];
            }
            for (u32 k = 0; k < ncat; ++k)
            {
                char b[32]; snprintf(b, sizeof(b), "cat 0x%02X: %u件  ", (unsigned)cats[k], (unsigned)cnts[k]);
                out += b;
                if (k % 3 == 2) out += "\n";
            }
        }
        else out += "(取得失敗)\n";
        MessageBox(TITLE, out)();

        // 2) 管理情報の件数フィールド候補 (+0x10〜+0xD8 を 4 バイトずつ)
        out = "mgr の 32bit 値 (+00〜+D8):\n";
        for (u32 off = 0; off < 0xDC; off += 4)
        {
            u32 v = 0; Process::Read32(mgr + off, v);
            char b[24]; snprintf(b, sizeof(b), "+%02X:%08X ", (unsigned)off, (unsigned)v);
            out += b;
            if ((off / 4) % 4 == 3) out += "\n";
        }
        MessageBox(TITLE, out)();

        // 3) 魂スロット付近 (mgr+0x2C40 〜 +0x2CE0)
        out = "mgr+0x2C40〜 (魂スロット候補):\n";
        for (u32 off = 0x2C40; off < 0x2CE0; off += 4)
        {
            u32 v = 0; Process::Read32(mgr + off, v);
            char b[24]; snprintf(b, sizeof(b), "+%04X:%08X ", (unsigned)off, (unsigned)v);
            out += b;
            if (((off - 0x2C40) / 4) % 3 == 2) out += "\n";
        }
        MessageBox(TITLE, out)();
    }


    // ---------------- 魂 (コンテナ記述子の自動検出) ----------------
    // 管理情報の中に 0x2C バイト単位の記述子が並ぶ:
    //   { +0 型ptr(007F5D68), +4 先頭, +8 ビットマップ, +C 件数, +10 件数+1, +14 (要素サイズ<<16 | 上限), ... }
    // 魂 = 定義表カテゴリ 0x28、要素 0x14 バイト、上限 100 → flags 0x00140064 を探す
    struct Container { u32 desc, begin, bitmap, count, stride, cap; };

    static bool FindContainerByFlags(u32 mgr, u32 flags, Container &c, u32 scanSize = 0x200)
    {
        u32 vtDesc = 0; Process::Read32(mgr + 4, vtDesc);     // 道具記述子の型ポインタ
        for (u32 off = 0x18; off < scanSize; off += 4)
        {
            u32 v = 0;
            if (!Process::Read32(mgr + off, v) || v != flags) continue;
            u32 d = mgr + off - 0x14, vt = 0, begin = 0, bmp = 0, cnt = 0;
            Process::Read32(d, vt);
            if (vt != vtDesc) continue;
            if (!Deref(d + 4, begin) || !Deref(d + 8, bmp)) continue;
            Process::Read32(d + 0xC, cnt);
            c.desc = d; c.begin = begin; c.bitmap = bmp; c.count = cnt; c.stride = flags >> 16; c.cap = flags & 0xFFFF;
            return true;
        }
        return false;
    }

    static void AllSoulsCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 mgr = 0, base = 0, end = 0;
        if (!GetMgr(0x0080D1E8, mgr)) { MessageBox(TITLE, "アイテム管理情報を取得できませんでした。")(); return; }
        if (!GetItemDefTable(base, end)) { MessageBox(TITLE, "アイテム定義表を取得できませんでした。")(); return; }
        Container c;
        if (!FindContainerByFlags(mgr, 0x00140064, c))
        {
            MessageBox(TITLE, "魂のコンテナ (要素0x14×上限100) が見つかりませんでした。\n「解析: コンテナ一覧」の結果を送ってください。")(); return;
        }
        if (c.count == 0)
        {
            MessageBox(TITLE, "魂を1つも持っていないため、雛形にする1件目がありません。\n魂を1つ入手してから実行してください。")(); return;
        }
        // 1件目を雛形にする (id 以外の未知のフィールドを保つ)
        u8 tmpl[0x14];
        for (u32 i = 0; i < c.stride; ++i) Process::Read8(c.begin + i, tmpl[i]);
        u16 idx0 = 0; Process::Read16(c.begin + 4, idx0);
        u32 idxBase = idx0 & 0xF000;

        u32 n = 0;
        for (u32 def = base; def < end && n < c.cap; def += 8)
        {
            u16 cat = 0; u32 id = 0;
            if (!Process::Read16(def + 6, cat) || cat != 0x28) continue;
            Process::Read32(def, id);
            u32 slot = c.begin + n * c.stride;
            for (u32 i = 0; i < c.stride; ++i) Process::Write8(slot + i, tmpl[i]);
            Process::Write16(slot + 4, (u16)(idxBase + n));
            Process::Write16(slot + 6, (u16)(n + 1));
            Process::Write32(slot + 8, id);
            ++n;
        }
        Process::Write32(c.desc + 0xC, n);
        Process::Write32(c.desc + 0x10, n + 1);
        SetBitmap(c.bitmap, n);
        MessageBox(TITLE, "魂を " + std::to_string(n) + " 種作成しました (上限 " + std::to_string(c.cap) + ")。\n※定義は415件あるため上限100までしか入りません。")();
    }

    static void ContainerListCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 mgr = 0;
        if (!GetMgr(0x0080D1E8, mgr)) { MessageBox(TITLE, "アイテム管理情報を取得できませんでした。")(); return; }
        u32 vtDesc = 0; Process::Read32(mgr + 4, vtDesc);
        std::string out = "記述子一覧 (型ptr一致):\n";
        u32 found = 0;
        for (u32 off = 4; off < 0x400 && found < 8; off += 4)
        {
            u32 v = 0; Process::Read32(mgr + off, v);
            if (v != vtDesc) continue;
            u32 begin = 0, bmp = 0, cnt = 0, flags = 0;
            Process::Read32(mgr + off + 4, begin); Process::Read32(mgr + off + 8, bmp);
            Process::Read32(mgr + off + 0xC, cnt); Process::Read32(mgr + off + 0x14, flags);
            char b[96]; snprintf(b, sizeof(b), "+%03X: 先頭 %08X bmp %08X 件数 %u サイズ 0x%X 上限 %u\n", (unsigned)off, (unsigned)begin, (unsigned)bmp, (unsigned)cnt, (unsigned)(flags >> 16), (unsigned)(flags & 0xFFFF));
            out += b; ++found;
        }
        MessageBox(TITLE, out)();
    }


    // ---------------- 妖怪を1体作成 (「妖怪作成」コードの代替: 一覧から選んで末尾に追加) ----------------
    static void CreateOneYokaiCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 mgr = 0, table = 0, bmp = 0, count = 0, listMgr = 0;
        if (!GetMgr(0x0080D1D8, mgr) || !Deref(mgr + 8, table) || !Deref(mgr + 0xC, bmp) || !Process::Read32(mgr + 0x10, count))
        { MessageBox(TITLE, "妖怪管理情報を取得できませんでした。\nゲーム中(マップ上)で実行してください。")(); return; }
        if (table != YW_BASE) { MessageBox(TITLE, "妖怪テーブルが想定と違います。中止しました。")(); return; }
        if (count == 0 || count >= YW_COUNT) { MessageBox(TITLE, "所持数が 0 か上限のため作成できません。")(); return; }
        u32 vt = 0; Process::Read32(table, vt);
        if (vt < 0x00100000 || vt >= 0x00800000) { MessageBox(TITLE, "1体目のポインタを流用できません。中止しました。")(); return; }

        u32 id = 0; std::string name;
        if (!PickYokaiId(id, name)) return;
        u8 lv = 1;
        Keyboard kb("作成する " + name + " のレベル (1〜99)");
        kb.IsHexadecimal(false);
        if (kb.Open(lv, lv) != 0) return;
        if (lv < 1) lv = 1;
        if (lv > 99) lv = 99;

        u32 slot = table + count * YW_STRIDE;
        for (u32 i = 0; i < YW_STRIDE; ++i) Process::Write8(slot + i, 0);   // 元コード同様、未指定の項目は 0 (再読込で再計算される)
        Process::Write32(slot + 0, vt);
        Process::Write16(slot + 4, (u16)count);
        Process::Write16(slot + 6, (u16)(count + 1));
        Process::Write32(slot + 8, id);
        Process::Write8(slot + 0x53, lv);
        GetMgr(0x0080D1E0, listMgr);
        if (listMgr) { Process::Write16(listMgr + 0x314 + count * 4, (u16)count); Process::Write16(listMgr + 0x314 + count * 4 + 2, (u16)(count + 1)); }
        u32 a = bmp + (count >> 5) * 4, v = 0;
        Process::Read32(a, v); Process::Write32(a, v | (1u << (count & 31)));
        Process::Write32(mgr + 0x10, count + 1);
        Process::Write32(mgr + 0x14, count + 2);
        MessageBox(TITLE, name + " (Lv." + std::to_string(lv) + ") を作成しました (所持 " + std::to_string(count + 1) + " 体)。\n"
                          "※ボス妖怪は「ボス保存」を有効にしていないと消えることがあります。\n※にっきで記録→タイトルへ→再読込→再セーブ を推奨。")();
    }


    // ---------------- 解析用: 定義表 / 手持ちデータの書き出し ----------------
    static void DumpTablesCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        std::string out;

        // 1) 手持ち妖怪テーブル (0x1A1 * 0xC4)
        out += DumpRegion(YW_BASE, YOKAI_DUMP_SIZE, "/YW2Tools/dump_yokai_party.bin") ? "手持ち妖怪: OK\n" : "手持ち妖怪: 失敗\n";

        // 2) 妖怪定義表 (1件 0x84)
        u32 defp = 0, defBase = 0, defLen = 0;
        if (Deref(0x0080D280, defp) && Deref(defp + 0x10, defp) && Deref(defp + 0x14, defBase) && Process::Read32(defBase - 0x10, defLen)
            && defLen > 0 && defLen < 0x100000)
        {
            // ヘッダ 0x10 バイトも含めて書き出す
            bool ok = DumpRegion(defBase - 0x10, defLen + 0x10, "/YW2Tools/dump_yokai_def.bin");
            char b[64]; snprintf(b, sizeof(b), "妖怪定義表: %s (%08X, %u バイト)\n", ok ? "OK" : "失敗", (unsigned)defBase, (unsigned)defLen);
            out += b;
        }
        else out += "妖怪定義表: 取得失敗\n";

        // 3) アイテム定義表 (1件 8)
        u32 ib = 0, ie = 0;
        if (GetItemDefTable(ib, ie))
            out += DumpRegion(ib, ie - ib, "/YW2Tools/dump_item_def.bin") ? "アイテム定義表: OK\n" : "アイテム定義表: 失敗\n";
        else out += "アイテム定義表: 取得失敗\n";

        // 4) 妖怪管理情報まわり (0x0080D1D8 / D1E0 / D1F8 / D280 の先 0x400 バイトずつ)
        static const u32 ptrs[] = { 0x0080D1D8, 0x0080D1E0, 0x0080D1F8, 0x0080D280 };
        for (u32 i = 0; i < 4; ++i)
        {
            u32 m = 0;
            if (GetMgr(ptrs[i], m))
            {
                char name[64]; snprintf(name, sizeof(name), "/YW2Tools/dump_mgr_%08X.bin", (unsigned)ptrs[i]);
                DumpRegion(m, 0x400, name);
            }
        }
        out += "管理情報: dump_mgr_*.bin\n/YW2Tools/ に保存しました。";
        MessageBox(TITLE, out)();
    }

    MenuFolder *CreateBulkFolder(void)
    {
        MenuFolder *f = new MenuFolder("全妖怪入手 / 全アイテム入手 (真打)",
            "ASMコードの移植版。テーブル全体を作り直すので実行前に必ずバックアップを。ゲーム中(マップ上)で実行。");
        *f += new MenuEntry("バックアップ (妖怪+アイテムをSDへ)",     nullptr, BackupCallback);
        *f += new MenuEntry("リストア (バックアップから戻す)",         nullptr, RestoreCallback);
        *f += new MenuEntry("管理情報を表示 (確認用)",                 nullptr, ShowMgrCallback);
        *f += new MenuEntry("たべもの・いきもの・道具 全部入手 (99個)", nullptr, AllToolsCallback);
        *f += new MenuEntry("装備 全部入手",                          nullptr, AllEquipCallback);
        *f += new MenuEntry("だいじなもの 全部入手",                   nullptr, AllKeyCallback);
        *f += new MenuEntry("魂 全部入手 (上限100まで)",               nullptr, AllSoulsCallback,
                            "魂を1つ以上持った状態で。1件目を雛形にして定義表(カテゴリ0x28)から作成。");
        *f += new MenuEntry("解析: コンテナ一覧",                      nullptr, ContainerListCallback);
        *f += new MenuEntry("解析: 妖怪/アイテムの定義表と手持ちをSDへ書き出す", nullptr, DumpTablesCallback,
                            "/YW2Tools/dump_*.bin に書き出します。技・スキルの対応表を作るための解析用。");
        *f += new MenuEntry("解析: 魂テーブルを表示 (3画面)",          nullptr, SoulDiagCallback);
        *f += new MenuEntry("妖怪を1体作成 (一覧から選択)",             nullptr, CreateOneYokaiCallback,
                            "「妖怪作成」コードの代替。選んだ妖怪を手持ちの末尾に追加します。");
        *f += new MenuEntry("妖怪 全部入手 (Lv99)",                   nullptr, AllYokaiCallback,
                            "既存の妖怪データも作り直されます。実行後: にっきで記録→タイトルへ→再読込→再セーブ。");
        return f;
    }
}
