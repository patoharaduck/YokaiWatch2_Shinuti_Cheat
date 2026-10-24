#include "cheats.hpp"

// ============================================================
//  技ID表 (SD: /YW2Tools/skills.txt) と、技の一覧選択
//   ファイル形式: 1行に「8桁の16進ID 半角スペース 技名」 例) 1D3F3776 満を持す
//   「#」で始まる行は無視。技のオフセットは +0x7C〜+0x8C (技1〜技5) と仮定。
// ============================================================

namespace CTRPluginFramework
{
    struct SkillEntry2 { u32 id; std::string name; };
    static std::vector<SkillEntry2> g_skills;
    static bool g_skillsLoaded = false;

    static u32 HexVal(const char *p, u32 n, bool &ok)
    {
        u32 v = 0; ok = n > 0;
        for (u32 i = 0; i < n; ++i)
        {
            char c = p[i]; u32 d;
            if (c >= '0' && c <= '9') d = c - '0';
            else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
            else { ok = false; return 0; }
            v = (v << 4) | d;
        }
        return v;
    }

    u32 LoadSkillTable(void)
    {
        g_skills.clear();
        g_skillsLoaded = true;
        const std::string path = "/YW2Tools/skills.txt";
        if (!File::Exists(path)) return 0;
        File f;
        if (File::Open(f, path, File::READ) != 0) return 0;
        u64 size = f.GetSize();
        if (size == 0 || size > 65536) { f.Close(); return 0; }
        std::string data((size_t)size, '\0');
        if (f.Read(&data[0], (u32)size) != 0) { f.Close(); return 0; }
        f.Close();

        size_t pos = 0;
        while (pos < data.size())
        {
            size_t eol = data.find('\n', pos);
            if (eol == std::string::npos) eol = data.size();
            std::string line = data.substr(pos, eol - pos);
            pos = eol + 1;
            while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.pop_back();
            if (line.size() < 9 || line[0] == '#') continue;
            bool ok = false;
            u32 id = HexVal(line.c_str(), 8, ok);
            if (!ok) continue;
            size_t s = 8;
            while (s < line.size() && (line[s] == ' ' || line[s] == '\t')) ++s;
            if (s >= line.size()) continue;
            SkillEntry2 e; e.id = id; e.name = line.substr(s);
            g_skills.push_back(e);
        }
        return (u32)g_skills.size();
    }

    const char *SkillNameFromId(u32 id)
    {
        if (!g_skillsLoaded) LoadSkillTable();
        for (size_t i = 0; i < g_skills.size(); ++i)
            if (g_skills[i].id == id) return g_skills[i].name.c_str();
        return nullptr;
    }

    bool PickSkillId(u32 &outId, std::string &outName)
    {
        if (!g_skillsLoaded) LoadSkillTable();
        if (g_skills.empty())
        {
            MessageBox("Yokai Watch 2 Tools", "技ID表がありません。\nSDカードの /YW2Tools/skills.txt に\n「8桁ID 技名」を1行ずつ書いて入れてください。")();
            return false;
        }
        std::vector<std::string> names;
        for (size_t i = 0; i < g_skills.size(); ++i) names.push_back(g_skills[i].name);
        Keyboard kb("技を選んでください (" + std::to_string(g_skills.size()) + " 件)");
        kb.Populate(names);
        int k = kb.Open();
        if (k < 0 || (size_t)k >= g_skills.size()) return false;
        outId = g_skills[k].id; outName = g_skills[k].name;
        return true;
    }

    bool SkillTableAvailable(void)
    {
        if (!g_skillsLoaded) LoadSkillTable();
        return !g_skills.empty();
    }
}
