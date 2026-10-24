#ifndef CHEATS_H
#define CHEATS_H

#include <CTRPluginFramework.hpp>
#include "Helpers.hpp"
#include "Unicode.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace CTRPluginFramework
{
    using StringVector = std::vector<std::string>;

    // ------------------------------------------------------------
    // バージョン差分
    //   元祖(0x0012F900) / 真打(0x00155100) : オフセット 0
    //   本家(0x0012F800)                    : オフセット +0x2000
    // 対応外のタイトルなら false を返す
    // ------------------------------------------------------------
    bool    GetVersionOffset(u32 &offset);

    // 各バージョンのアドレスを求めるヘルパー (base は元祖/真打基準)
    // 対応外なら 0 を返す
    u32     VersionAddress(u32 baseAddress);

    // ---- お金 ----
    void    MaxMoneyCallback(MenuEntry *entry);

    // ---- アイテム ----
    void    AllItemsX99Callback(MenuEntry *entry);

    // ---- 妖怪 (先頭 254 体) ----
    void    YokaiMaxHPCallback(MenuEntry *entry);
    void    YokaiMaxAllStatsCallback(MenuEntry *entry);
    void    YokaiMaxStrengthCallback(MenuEntry *entry);
    void    YokaiMaxSpiritCallback(MenuEntry *entry);
    void    YokaiMaxDefenseCallback(MenuEntry *entry);
    void    YokaiMaxSpeedCallback(MenuEntry *entry);
    void    YokaiMasterTechniquesCallback(MenuEntry *entry);

    // フォルダ生成 (cheats.cpp)
    MenuFolder  *CreateItemFolder(void);
    MenuFolder  *CreateYokaiFolder(void);

    // 毎フレーム呼ぶ監視 (main.cpp から menu->Callback で登録)
    void        CheatsFrameCallback(void);

    // フォルダ生成 (cheats_wiki.cpp, 真打専用)
    MenuFolder  *CreateWarpFolder(void);
    MenuFolder  *CreateSkillFolder(void);
    MenuFolder  *CreateYokaiExtFolder(void);
    MenuFolder  *CreateModelFolder(void);
    MenuFolder  *CreatePatchFolder(void);
    MenuFolder  *CreateMiscFolder(void);
    MenuFolder  *CreatePosFolder(void);

    // cheats_ids.cpp: ID テーブル
    bool        PickYokaiId(u32 &outId, std::string &outName);
    bool        PickItemId(u32 &outId, std::string &outName);
    const char *YokaiNameFromId(u32 id);
    const char *ItemNameFromId(u32 id);
    u32         GetYokaiGroupCount(void);
    const char *GetYokaiGroupName(u32 g);
    u32         GetYokaiGroupSize(u32 g);
    void        GetYokaiGroupEntry(u32 g, u32 k, const char *&name, u32 &id);
    u32         GetItemGroupCount(void);
    const char *GetItemGroupName(u32 g);
    u32         GetItemGroupSize(u32 g);
    void        GetItemGroupEntry(u32 g, u32 k, const char *&name, u32 &id);

    // cheats_skills.cpp: 技ID表 (/YW2Tools/skills.txt)
    u32         LoadSkillTable(void);
    const char *SkillNameFromId(u32 id);
    bool        PickSkillId(u32 &outId, std::string &outName);
    bool        SkillTableAvailable(void);

    // cheats_technames.cpp: 攻撃技/術/スキルの名前 (game-cap.com + 実機ダンプ)
    const char *BuiltinTechName(u32 kind, u32 id);

    // cheats_tech.cpp: 技・スキル (攻撃技/術/必殺技/ガード/とりつき/スキル = kind 0..5)
    const char *TechKindName(u32 kind);
    u32         TechKindOffset(u32 kind);
    bool        BuildTechCatalog(void);
    std::string TechLabel(u32 kind, u32 id);
    bool        PickTech(u32 kind, u32 &outId, u32 &outPtr, std::string &outLabel);
    bool        ApplyTech(u32 slot, u32 kind, u32 id, u32 ptr);
    void        RebuildTechCatalogCallback(MenuEntry *entry);

    // cheats_bulk.cpp
    MenuFolder  *CreateBulkFolder(void);

    // フォルダ生成 (cheats_ids.cpp, 真打専用)
    MenuFolder  *CreateIdFolder(void);

    // フォルダ生成 (cheats_more.cpp, 真打専用)
    MenuFolder  *CreateEditorFolder(void);
    MenuFolder  *CreateMoveFolder(void);
    MenuFolder  *CreateBattleFolder(void);
    MenuFolder  *CreateEquipFolder(void);
    MenuFolder  *CreateMore2Folder(void);
    void        MoreFrameCallback(void);

    // cheats_ui.cpp: 画面版エディタ
    MenuEntry  *CreateEditorUiEntry(void);
    void        EditorUiFrame(void);
    void        EditorUiSetSlot(u32 slot, u32 index);
}
#endif
