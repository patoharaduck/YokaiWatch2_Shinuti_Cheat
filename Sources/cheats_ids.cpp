#include "cheats.hpp"

// ============================================================
//  妖怪ID / アイテムID テーブル (真打)  出典: wikiwiki.jp/3ds_codes
//  値はすべて「入力用 (ビッグエンディアン表記)」= Write32 でそのまま書く値
// ============================================================

namespace CTRPluginFramework
{
    static const char *TITLE = "Yokai Watch 2 Tools";

    struct IdEntry { const char *name; u32 id; };

    // ---------------- 妖怪 ----------------
    static const IdEntry Y_ISAMASHI[] = {
        {"ぶようじん坊",0x79F3AA36},{"がらあきん坊",0x6B4605D8},{"さきがけの助",0x725D3499},{"ダラケ刀",0xE424928F},
        {"ザンバラ刀",0x5C98F5EA},{"ダララだんびら",0x4583C4AB},{"かたづ家来",0xAA720DA6},{"アライ魔将",0x7D738AA7},
        {"獅子まる",0xB8C7A248},{"万尾獅子",0xB25B193D},{"おこ武者",0x12CE6AC3},{"くしゃ武者",0x61F8DC5A},
        {"なまはげ",0x78E3ED1B},{"ちからモチ",0xF6913D61},{"やきもチ",0xEF8A0C20},{"あかなめ",0x37A5351F},
        {"おにぎり侍【鬼斬り】",0x8F19527A},{"おにぎり侍【おにぎり】",0xF26EA63F},{"焼きおに斬り【鬼斬り】",0x0AE77E58},{"焼きおに斬り【防火壁】",0x77908A1D},
        {"カブトさん",0x44938386},{"ゲンマ将軍",0x56262C68},{"黄泉ゲンスイ",0x4F3D1D29},{"泥田坊",0x9DACFD94},
        {"ブリー隊長",0xC5CFEDC2},{"メラメライオン",0xEE9A4B0D},{"グラグライオン",0xF7817A4C},{"とどろき獅子",0xDCAC298F},
        {"しょうブシ",0x04FBE5BC},{"まさむね",0xBC4782D9},{"むらまさ",0xFC2FE4E3},{"くさなぎ",0xA55CB398},
        {"ベンケイ",0xD944BB3F},{"からくりベンケイ",0xC05F8A7E},{"やまと",0x118656B8},{"カブキ猿",0x089D67F9},
        {"クワノ武士【くそまじめ】",0x734D73B4},{"クワノ武士【闘将】",0x0E3A87F1},{"クワガ大将【いあつかん】",0xCBF114D1},{"クワガ大将【闘将】",0xB686E094},
        {"オオクワノ神",0xD2EA2590},{"鬼食い",0x1852D1B6},{"いのちとり",0x0149E0F7},
    };
    static const IdEntry Y_FUSHIGI[] = {
        {"さかさっ傘",0xA0EEB6D3},{"化け草履",0xF277BAE9},{"ズルズルづる",0x399BCC0C},{"かぜカモ",0x2080FD4D},
        {"つづかな僧",0xD3FA62BD},{"やめたい師",0x4E2D5A04},{"もうせん和尚",0x57366B45},{"わすれん帽",0xBB8F9E33},
        {"メゾン・ドワスレ",0x77EF31D2},{"ばか頭巾",0xA294AF72},{"のっぺら坊",0x554C72E4},{"わらえ姉",0x0333F956},
        {"青くちびる",0x2748E1A7},{"バクロ婆",0x8C516E01},{"ババァーン",0x9EE4C1EF},{"ドクロ婆",0x954A5F40},
        {"モテモ天",0xA93A31DD},{"モテマクール",0x34ED0964},{"モテヌス",0x2DF63825},{"一旦ゴメン",0x8585E90F},
        {"あやまり倒し",0x378757E8},{"こえんら",0x3D398E6A},{"えんらえんら",0xCF5356B7},{"でんぱく小僧",0x7E3BB6DC},
        {"でんじん",0xC687D1B9},{"はつでんしん",0xDF9CE0F8},{"おでんじん",0x8F3B308D},{"U.S.O.",0x2F8C2184},
        {"アペリカン",0xC1BBADFD},{"うんがい鏡",0x86EFB783},{"うんがい三面鏡",0x2532F806},{"ヤミ鏡",0x9FF486C2},
        {"まぼ老師",0x3E53D0E6},{"かげ老師",0x1A28C817},{"ほう老師",0x0C65B264},{"さとりちゃん",0x8127AB69},
        {"よつめ",0x983C9A28},{"セバスチャン",0xF6655DCF},{"天狗",0x6B64672F},{"ほむら天狗",0x727F566E},
        {"キュウビ",0x098D20D4},{"犬神",0x10961195},{"ぬえ",0x9D8E9F63},{"風魔猿",0x8495AE22},
    };
    static const IdEntry Y_GOKETSU[] = {
        {"ドキ土器",0x0059A7DA},{"だるだるま",0xB13147B1},{"だるまっちょ",0xA384E85F},{"ゴリだるま",0xBA9FD91E},
        {"うらや飯",0xB8E5C0BF},{"プライ丼",0xC89B4A5D},{"迷い車",0x4A8F1862},{"だっせんしゃ",0x70272D38},
        {"ムリカベ【ブロッカー】",0x34CF6B93},{"ムリカベ【がまん】",0x49B89FD6},{"むりだ城【ブロッカー】",0x8C730CF6},{"むりだ城【大地の壁】",0xF104F8B3},
        {"シロカベ",0x95683DB7},{"グレるりん",0x8C37C918},{"ゴクドー",0xCC5FAF22},{"アニ鬼",0xFE69CDA0},
        {"ヨロイさん",0x267AC47D},{"モノマネキン",0x973046E1},{"トオセンボン",0x9EC6A318},{"くろがねセンボン",0x87DD9259},
        {"モレゾウ",0xC16DAFA4},{"プルファント",0xD8769EE5},{"ガマンモス",0xFF45BA6F},{"雷蔵",0x12EC0834},
        {"ヒライ神",0x47F9DD0A},{"あせっか鬼",0xAA506F51},{"ふじのやま",0xBBADFCC4},{"さくらのじま",0xA2B6CD85},
        {"ドウカク",0xB003626B},{"ギンカク",0x9B2E31A8},{"キンカク",0xA918532A},{"プラチナカク",0x823500E9},
        {"武者かぶと【ガードくずし】",0x03119BA1},{"武者かぶと【闘将】",0x7E666FE4},{"かぶと無双【ガードくずし】",0x11A4344F},{"かぶと無双【闘将】",0x6CD3C00A},
        {"オオツノノ神",0x08BF050E},{"ロボニャン",0x3E71B211},{"ゴルニャン",0x276A8350},{"だいだらぼっち",0xF17B437C},
        {"うみぼうず",0xE860723D},{"大ガマ",0xEDF01581},{"大やもり",0xF4EB24C0},
    };
    static const IdEntry Y_PRETTY[] = {
        {"びきゃく",0xB59305D7},{"しゃれこ婦人",0x435B9F6C},{"カラカラさん",0xFBE7F809},{"ほね美人",0xE2FCC948},
        {"セミまる",0x49C72419},{"カゲまる",0x09AF4223},{"ヒグラシまる",0x10B47362},{"さむガリ【氷の牙】",0x9971043D},
        {"さむガリ【かんつう】",0xE406F078},{"ガリ王子【アイスブロック】",0x21CD6358},{"ガリ王子【かんつう】",0x5CBA971D},{"あつガルル",0x45A1A65C},
        {"つらがわり",0x5FF2AB66},{"一つ目小僧",0x0D2F62B2},{"ジバニャン",0xA3A68AA8},{"トゲニャン",0x1B1AEDCD},
        {"ワルニャン",0x86CDD574},{"ブチニャン",0x6BB26576},{"ノガッパ【さぼりじょうず】",0x94787A9A},{"ノガッパ【水あそび】",0xE90F8EDF},
        {"たびガッパ【かんつう】",0xCC1B6ACC},{"たびガッパ【かっぱのさら】",0xB16C9E89},{"なみガッパ",0xD5005B8D},{"コマさん",0x74A70DA9},
        {"ししコマ",0x4E0F38F3},{"コマじろう【うん】",0x6DBC3CE8},{"コマじろう【雷の拳】",0x09D0F9EC},{"とらじろう【うん】",0x7C395A71},
        {"とらじろう【雷の拳】",0x3378CCB6},{"バク",0x2658A68A},{"おバク様",0xE74ECC03},{"ハク【こううん】",0x3F4397CB},
        {"ハク【パーフェクトドリーム】",0x5B2F52CF},{"雨女",0x4D470488},{"ばいそく",0xF5FB63ED},{"キュン太郎",0x44B1E171},
        {"ズキュキュン太",0x5604E19F},{"裏キュン太",0x4F1F7FDE},{"ケマモン",0x3E85D2BF},{"ゆきおんな",0x2CE67F08},
        {"ふぶき姫",0x945A186D},{"百鬼姫",0x8D41292C},{"河童",0x7A99F4BA},{"水虎",0x6382C5FB},{"マスターニャーダ",0xC22593DF},
    };
    static const IdEntry Y_POKAPOKA[] = {
        {"うらやましろう",0xDED27776},{"つまみぐいのすけ",0xF4C94637},{"ワカメくん",0xFC0D8614},{"ワカメ☆スター",0x651E5BD2},
        {"コンブさん",0xE516B755},{"メカブちゃん",0xCE3BE496},{"ひも爺【ひもじすと】",0xD3D8004A},{"ひも爺【おにぎり】",0xAEAFF40F},
        {"くいい爺【ひもじすと】",0x8BBB101C},{"くいい爺【おにぎり】",0xF6CCE459},{"こめ爺",0xCAC3310B},{"キズナメコ",0xD0903C31},
        {"キズナース",0xF8C9636B},{"砂夫【すなお】",0x304F4B02},{"砂夫【さらさらボディー】",0x4D38BF47},{"大山砂夫【すなお】",0x4075040E},
        {"大山砂夫【さらさらボディー】",0x3D02F04B},{"から傘お化け",0x682C5B54},{"から傘魔人",0x52C0ABE0},{"びんボーイ",0x70054FCF},
        {"お金ナイダー",0xB21FDCD3},{"らくてん童",0x691E7E8E},{"ざしきわらし",0xCF179359},{"ざしきわら神",0xEA7CCC85},
        {"ぜっこう蝶",0xD966D9C8},{"ゼッコウ蝶",0xC07DE889},{"サイコウ蝶",0x61DABEAD},{"アゲアゲハ",0x78C18FEC},
        {"くだん",0x77ABF43C},{"大くだん",0x0AA3BBB6},{"どんちゃん",0x88F32C67},{"晴れ男",0xDDA23CB7},
        {"ホノボーノ",0xEEB829FA},{"ヒョウヘンナ",0xAFC008ED},{"ヒョウヘンヌ",0xB6DB39AC},{"サンタク老師",0x0E675EC9},
        {"ふくろじじい",0x177C6F88},{"さいの目入道",0x79D1C8C1},{"ゾロ目大明神",0x60CAF980},{"雷オトン",0x4983E1F7},
        {"絶オジイ",0x5098D0B6},{"聖オカン",0xF13F8692},{"心オバア",0xE824B7D3},{"麒麟",0x4ACBDD8C},{"イッカク",0x53D0ECCD},
    };
    static const IdEntry Y_USURAKAGE[] = {
        {"みちび鬼",0xD454D94E},{"えこひい鬼",0xCD4FE80F},{"かたのり小僧",0x5B364E19},{"かたのり親方",0xE38A297C},
        {"ぎっくり男",0xFA91183D},{"TETSUYA",0xE0C21507},{"KANTETSU",0x343B0B3D},{"草くいおとこ",0xC5A94ADB},
        {"肉くいおとこ",0xDA2EE5B3},{"ネガティブーン",0x7E5D11C5},{"ジャネガブーン",0xC6E176A0},{"かゆかゆ",0xDFFA47E1},
        {"ジミー【おんみつ】",0x5188979B},{"ジミー【ひらひらボディー】",0x2CFF63DE},{"カゲロー【おんみつ】",0x09EB87CD},{"カゲロー【ひらひらボディー】",0x749C7388},
        {"カイム",0x10F0B68C},{"ぎしんあん鬼【うたがいのめ】",0xDA0C8744},{"ぎしんあん鬼【しりょくA】",0xA77B7301},{"ガ鬼",0xC317B605},
        {"あまのじゃく",0xE83AE5C6},{"ヒキコウモリ",0x556E1013},{"トジコウモリ",0xC8B928AA},{"ヤドコウモリ",0xD1A219EB},
        {"かりパックン",0x587E7262},{"泥ボックン",0xF2337F07},{"まてんし",0x7D152DBE},{"ひとまか仙人",0x6FA08250},
        {"りもこんかくし",0xD71CE535},{"ジコチュウ",0x7907CA98},{"ネクラマテング",0x62B0E021},{"ヨミテング",0x7BABD160},
        {"リー夫人",0x37C39206},{"ミツマタノヅチ",0x8F7FF563},{"ガブニャン",0x8C876C58},{"魔ガサス",0x74E3C847},
        {"ウ魔",0x6DF8F906},{"デビビル",0x826F9712},{"デビビラン",0x3AD3F077},{"虫歯伯爵",0x23C8C136},
        {"百々目鬼",0x629282D6},{"きらめ鬼",0x7B89B397},{"ドケチング",0x7ABB964D},{"ヤミまろ",0x63A0A70C},{"どんどろ",0x268EA4D3},
    };
    static const IdEntry Y_BUKIMI[] = {
        {"イガイガグリ",0x1B388F3A},{"チクチクウニ",0x0223BE7B},{"じめりんぼう",0xB157E0A8},{"雨ふらし",0x47DBBFFD},
        {"こおりんぼう",0xA84CD1FD},{"化けあられ",0x5EC08EBC},{"たらりん",0x8361822A},{"ヨコドリ",0x6CE8BE2B},
        {"ぶんぶん鳥",0x493344B7},{"はらおドリ【つやつやボディー】",0x75F38F6A},{"はらおドリ【ながもち】",0x119F4A6E},{"おともらし",0x2C307D51},
        {"爆音ならし",0xB1E745E8},{"トホホギス",0xB1132546},{"ブカッコウ",0xA8081407},{"死神鳥",0x832547C4},
        {"じんめん犬",0x33077779},{"かおベロス",0x736F1143},{"三途の犬",0x6A742002},{"けうけげん",0x9E32C3B6},
        {"ふさふさん",0x095B228D},{"認MEN",0x03E5FB0F},{"ドンヨリーヌ",0xFF67D898},{"口だけおんな",0x348BAE7D},
        {"おつぼね様【さぼりじょうず】",0x2D909F3C},{"おつぼね様【くそまじめ】",0x49FC5A38},{"じがじぃさん",0xBB599C6A},{"のらりくらり",0x115054E1},
        {"おならず者",0x433D3875},{"へこ鬼神",0x5A260934},{"花子さん",0xA9EC3384},{"呪野花子",0x1BEE8D63},
        {"のぼせトンマン",0xF18F23D2},{"おすべり様",0xFB815F10},{"おしっしょう",0xE29A6E51},{"しわくちゃん",0x665667A9},
        {"老いらん",0xDEEA00CC},{"ふじみ御前",0xC7F1318D},{"フゥミン",0xE934F0FE},{"おねむの精",0xF02FC1BF},
        {"土蜘蛛",0xA352EA06},{"女郎蜘蛛",0xBA49DB47},
    };
    static const IdEntry Y_NYORORON[] = {
        {"ねちがえる",0x8639B5DA},{"ツチノコ",0xC207F128},{"フクリュウ",0xD0B25EC6},{"ツチノコパンダ",0xDB1CC069},
        {"からみぞん",0x948C1A34},{"こんがらギャル",0xD32C60E4},{"笑ウツボ",0xCBD37626},{"歌ウナギ",0xD2C84767},
        {"大蛇のツボ",0xF9E514A4},{"キライギョ",0x4D65667F},{"ゾウオ",0x5FD0C991},{"ギャクジョウオ",0x46CBF8D0},
        {"おもいだスッポン",0x74536D07},{"おもいだ神",0xC199CF0A},{"ムカムカデ",0xF6473F38},{"激ドラゴン",0x7925A86F},
        {"りゅーくん",0x680E396F},{"龍神",0xF5D9911A},{"青龍",0xECC2305B},{"にんぎょ",0x4EFB585D},
        {"八百比丘尼",0x6B900781},{"イザナミ",0x728B36C0},{"ろくろ首",0x5C4EF7B3},{"ボー坊",0xF6B35F96},
        {"しどろもどろ",0xEFA86ED7},{"ミチクサメ",0xE76CAEF4},{"トーシロザメ",0xFE779FB5},{"ムダヅカイ",0xBF0FBEA2},
        {"はらわシェル",0xA6148FE3},{"ナガバナ",0x07B3D9C7},{"ナガバナナ",0x1EA8E886},{"ホリュウ",0xE4F290D6},
        {"つられたろう丸",0xCCEF0A62},{"しきるん蛇",0x3D1BEC9D},{"すねスネーク",0x0F2D8E1F},{"まむし行司",0x2400DDDC},
        {"オロチ",0x85A78BF8},{"影オロチ",0x9CBCBAB9},
    };
    static const IdEntry Y_SPECIAL[] = {
        {"ブシニャン",0x97122416},{"しゅらコマ",0x2FAE4373},{"山吹鬼",0x1870B341},{"ネタバレリーナ",0x8B4F70B2},
        {"花さか爺",0x0AC51CAF},{"うんちく魔",0xCB271688},{"イケメン犬",0xB2797BCA},{"やまタン",0x739B71ED},
        {"サファイニャン",0x35DF2CBE},{"エメラルニャン",0x1EF27F7D},{"ルビーニャン",0x07E94E3C},{"トパニャン",0x48A8D8FB},
        {"ダイヤニャン",0xFF1DC42C},{"メロンニャン",0x5A088D1C},{"ミカンニャン",0x4313BC5D},{"キウイニャン",0x683EEF9E},
        {"ブドウニャン",0x7125DEDF},{"イチゴニャン",0x3E644818},{"スイカニャン",0x277F7959},{"ロボガッパ",0x577E4874},{"ロボコマ",0x6DD67D2E},
        {"ロボG",0xD56A1A4B},{"ロボメン犬",0xC7DFB5A5},{"ロボノコ",0x7F63D2C0},{"ロボりゅーくん",0xE2B4EA79},{"ワンダーニャン",0x4467E328},
        {"ロボニャンF型",0x4ED93AAA},{"セーラーニャン",0x5C6C9544},{"マスクドニャン",0xE4D0F221},{"フユニャン",0x25109AF1},
        {"ダークニャン",0xD30E0213},{"ジバコマ",0xBCB3E277},{"ジェットニャン",0x040F8512},{"厄怪",0xFCF9E6BA},{"不怪",0xEE4C4954},
        {"豪怪",0x56F02E31},{"難怪",0x0E933E67},{"破怪",0xB62F5902},
        {"あかなめ・怪",0x316EF7F4},{"泥田坊・怪",0x9B673F7F},{"えんらえんら・怪",0xC998945C},{"河童・怪",0x7C523651},
        {"から傘お化け・怪",0x6EE799BF},{"ざしきわらし・怪",0xC9DC51B2},{"くだん・怪",0x716036D7},{"けうけげん・怪",0x98F9015D},
        {"にんぎょ・怪",0x48309AB6},{"ろくろ首・怪",0x5A853558},{"ジンギスギスカン",0xAFAA492B},
        {"チンギスギスハン",0xBD1FE6C5},{"寝ブタ",0x05A381A0},{"ねぶた",0x9874B919},{"江戸っ子パンダ",0x20C8DE7C},
        {"大江戸忍者パンダ",0x327D7192},{"ひつま武士",0x8AC116F7},{"ひまつ武士",0xD2A206A1},{"たこやっ鬼",0x6A1E61F7},
        {"たこや鬼神",0x2A7607FE},{"砂ン丘入道",0x92CA609B},{"ノー砂ン丘入道",0x807FCF75},{"すもうどん",0x38C3A810},
        {"横綱うどん",0xA51490A9},{"あっそう山",0x1DA8F7CC},{"あそっ火山",0x0F1D5822},{"ウキウキビ",0xB7A13F47},
        {"トキメキビ",0xEFC22F11},
        {"ハナホ人",0x56D24CC6},{"妖怪ガッツK",0xCB05747F},{"妖怪ガッツF",0x73B9131A},{"ジバニャンS",0xAC77B127},
        {"コマさんS",0x7B763626},{"コマじろうS",0x626D0767},{"ヤミキュウビ",0x3BBB4256},{"ヒカリオロチ",0xB791E97A},
        {"赤鬼",0x03C799F8},{"青鬼",0xBB7BFE9D},{"黒鬼",0xA9CE5173},
    };

    static const IdEntry Y_BOSS[] = {
        {"ガシャどくろ",0x924C0089},{"デカニャン",0x33F317D7},{"大後悔船長",0x5AAC8FF9},{"台風の目",0x67CCA649},
        {"キン",0xE88C53E9},{"ギン",0xFA39FC07},{"トキヲ・ウバウネ",0xAF2C2939},{"スベテ・ウバウネ",0x206CDC99},
        {"ミツマタノヅチ",0xDF84A182},{"地獄大山椒",0xC69F90C3},{"のぼせトンマン",0xE2E48832},{"おのぼり黒トン",0xFBFFB973},
        {"つられたろう丸",0xA544F2E2},{"第八三途丸",0xBC5FC3A3},{"おぼろ入道",0x9824DB52},{"月影入道",0x813FEA13},
        {"亡霊武者",0x2A040742},{"国宝・影正",0x331F3603},{"鬼くももん",0x17642EF2},{"やぶれかぶれ院長",0x50C45422},
        {"キモナシ先生",0x49DF6563},{"イカカモネ議長",0x6DA47D92},{"イカカモネ議長 (2)",0xEFF4EA43},{"ゲソヒゲール大臣",0xF6EFDB02},
        {"あおべえあかべえ",0x5910D32C},{"つづらチュン太夫",0x6470FA9C},{"どんどろ",0xD294C3F3},{"モテアマス",0xD5EC7A59},
        {"ゲートキーパー",0x7ED79708},{"でるくいたたき",0x1D0CF529},{"泥船ヨーソ郎",0x43B7BEB8},{"いくよね",0xF19762A8},
        {"くるよね",0xE322CD46},{"熟魔女ソラミ",0xB6371878},{"あやとりさま",0xA23C4B48},{"ガシャどくろG",0x8B5731C8},
        {"ギヤマンどくろ",0xA07A620B},{"オモテナス",0xCCF74B18},{"おびすさま",0x55FAC4CB},{"赤鬼",0x14D87227},
        {"青鬼",0x066DDDC9},{"黒鬼",0xBED1BAAC},{"レッドJ",0xC8E94AE1},{"マイティードッグ",0xF5896351},
        {"日ノ神",0x7AC996F1},{"ドウ",0x42859B62},{"ぶんぶく茶ヶ丸",0x8F493031},{"枝魔目さま",0xB2291981},
        {"暴走マッタナ氏",0x47A9BF41},{"カブキロイド",0x0009C591},
    };
    static const IdEntry Y_ENEMY[] = {
        {"おびすさま",0x002A5F32},{"いかサマ士",0x0076990E},{"亡霊武者",0x016535F1},{"スベテ・ウバウネ",0x02C2DC9E},
        {"枝魔目さま",0x034DF33F},{"台風の目",0x03FDA58A},{"クサビ",0x0417C468},{"クロカブト",0x079CE713},
        {"ゲソヒゲール大臣",0x097E0876},{"ふかづめ小僧",0x09A21E00},{"あきす老師",0x0AEA227B},{"あおべえあかべえ",0x0BCFE28E},
        {"月影入道",0x0C149895},{"あおべえあかべえ (2)",0x0CA22697},{"ゲートキーパー",0x0CABEEB8},{"むだむだん",0x0DD65E3F},
        {"ゲソヒゲール大臣 (2)",0x0E13CC6F},{"怒りナマズの一郎",0x0F4D3A69},{"どんどろ",0x10D8A0BE},{"ゲートキーパー (2)",0x129FF0DC},
        {"じゃまと",0x150B2A0A},{"紅蓮ナマズのお凛",0x16560B28},{"青鬼",0x16DBDC92},{"どんどろ (2)",0x17B564A7},
        {"あやとりさま",0x1A84CD5B},{"台風の目 (2)",0x1AE694CB},{"ドウ",0x1D325232},{"国宝・影正",0x20D0032E},
        {"のぼせトンマン",0x2337EED9},{"赤鬼",0x2733C60F},{"ゲートキーパー (3)",0x2786BD7B},{"国宝・影正 (2)",0x27BDC737},
        {"がきおんな",0x286B03BA},{"イカカモネ議長",0x28CB3EA9},{"鉄砲ナマズの五郎",0x2A2665B5},{"おぼろ入道",0x2DA1AE4A},
        {"泥船ヨーソ郎",0x2DE1AAD2},{"イカカモネ議長 (2)",0x2FA6FAB0},{"台風の目 (3)",0x31CBC708},{"ごきげんななめ",0x332849AD},
        {"台風の目 (4)",0x332AC603},{"黄泉ナマズの権三",0x333D54F4},{"悪メン犬",0x378A0BCB},{"ギン",0x3860E0BB},
        {"鬼くももん",0x3976ABE6},{"荒らすん蛇",0x3996902F},{"きこえんら",0x39B4F2D8},{"ペテン老師",0x3ADEAC54},
        {"ゲートキーパー (4)",0x3E9D8C3A},{"腹黒郎",0x403C9DC3},{"イカカモネ議長 (3)",0x40E7DBE3},{"たくらむ怪魔",0x444581DF},
        {"おびすさま 雲",0x474F6B25},{"どろこ婦人",0x47D6E3DE},{"ろうにんぎょ",0x4A7624EF},{"だまししコマ",0x4A824441},
        {"おびすさま (2)",0x4B0B9468},{"悪オトン",0x4D0E9D45},{"デマまる",0x4D4A58AB},{"オモテナス",0x4E319FE8},
        {"鬼くももん (2)",0x4E719B70},{"ギン (2)",0x4F67D02D},{"国宝・影正 (3)",0x50BAF7A1},{"キン",0x50FAF339},
        {"ほっぺた坊",0x51C10E56},{"とんま将軍",0x52AB50DA},{"国宝・影正 (4)",0x57D733B8},{"サギ王子",0x5837EBAF},
        {"イカカモネ議長 (4)",0x58A1CA26},{"とぐろ首",0x58C38B01},{"ガシャどくろ",0x5E6CB3AA},{"イカカモネ議長 (5)",0x5FCC0E3F},
        {"おびすさま (3)",0x6026C7AB},{"どんどろ (3)",0x60B25431},{"ゆらめく怪魔",0x612EDE03},{"いわくちゃん",0x62DB1B1B},
        {"無茶むしゃ",0x6575A0E8},{"ざわめき",0x661FFE64},{"立ちヨミテング",0x663D9C93},{"ドウ (2)",0x6A3562A4},
        {"ゲートキーパー 雲",0x6C6238E6},{"わりゅーくん",0x6C834511},{"地獄大山椒",0x6C965848},{"ガサガサお化け",0x6CA127E6},
        {"ギヤマンどくろ",0x6D6C2A9B},{"あやとりさま (2)",0x6D83FDCD},{"くるよね",0x6ED2E6F2},{"モテアマス",0x6F84A937},
        {"キザ天狗",0x6FE91B9D},{"ダマさん",0x702A711B},{"通り馬",0x706EB4F5},{"亡霊武者 (2)",0x710FC17E},
        {"くだらん",0x7326888E},{"枝魔目さま (2)",0x744AC3A9},{"どろボーイ",0x7488337D},{"台風の目 雲",0x757909A7},
        {"ミツマタノヅチ",0x758D6909},{"亡霊武者 (3)",0x76620567},{"大きいつづら",0x76C55572},{"クロノブシ",0x77C00F06},
        {"おびすさま (4)",0x793DF6EA},{"てぐせブーン",0x7AD06D77},{"あおべえあかべえ (3)",0x7BA51601},{"だましんぼう",0x7D7ED684},
        {"へのかっぱ",0x7E148808},{"のぞキング",0x7E36EAFF},{"ゲソヒゲール大臣 (3)",0x7E7938E0},{"ガシャどくろG",0x7FD98575},
        {"台風の目 (5)",0x83EFC571},{"ドウ (3)",0x843B0388},{"あやとりさま (3)",0x84E058F8},{"もぐりちゃん",0x85AAD7DB},
        {"イビビル",0x86E2EBA0},{"ブレルりん",0x88BAB5AA},{"パクロ婆",0x88DC12B3},{"どんどろ (4)",0x89D1F104},
        {"冥土ナマズの佐吉",0x8B813391},{"ゲートキーパー (5)",0x8B96A166},{"ゲソヒゲール大臣 (4)",0x907759CC},{"えせガッパ",0x90F50628},
        {"悟りナマズの三郎",0x929A02D0},{"あおべえあかべえ (4)",0x92C6B334},{"月影入道 (2)",0x951DC92F},{"大後悔船長",0x955DCDB7},
        {"ゲートキーパー (6)",0x95A2BF02},{"あおべえあかべえ (5)",0x95AB772D},{"ゲソヒゲール大臣 (5)",0x971A9DD5},{"亡霊武者 (4)",0x986C644B},
        {"ソーダ坊",0x99218126},{"おびすさま (5)",0x99230E88},{"枝魔目さま (3)",0x9A44A285},{"ねぐせげん",0x9ABFBF04},
        {"台風の目 (6)",0x9AF4F430},{"おのぼり黒トン",0x9B8B89BC},{"スベテ・ウバウネ (2)",0x9BCB8D24},{"泥船ヨーソ郎 (2)",0x9C2B8D73},
        {"枝魔目さま (4)",0x9D29669C},{"亡霊武者 (5)",0x9F01A052},{"大後悔船長 (2)",0xA14BA4C3},{"鬼くももん (3)",0xA7123E45},
        {"黒ニャン",0xA72BF61A},{"ゲートキーパー (7)",0xA794DD80},{"黒鬼",0xA7CA8BED},{"台風の目 (7)",0xA8C296B2},
        {"台風の目 (8)",0xAA2397B9},{"シッカク",0xAD952F98},{"パチモ天",0xADB74D6F},{"地獄ナマズの平次",0xAEEA6C4D},
        {"イカカモネ議長 (6)",0xB1C26F13},{"おぼろ入道 (2)",0xB4A8FFF0},{"ワルスギス",0xB59E59F4},{"イカカモネ議長 (7)",0xB6AFAB0A},
        {"暴れナマズの次郎",0xB7F15D0C},{"なまくら",0xB8CAFE6B},{"国宝・影正 (5)",0xB9D95294},{"まきあげ貝",0xBB82C210},
        {"ゲートキーパー (8)",0xBE8FECC1},{"キン (2)",0xBEF49215},{"こそどろ帽",0xBF02E281},{"ニセノコ",0xC68A8D9A},
        {"イカカモネ議長 (8)",0xC6C55F85},{"ガシャどくろ (2)",0xC765E210},{"じゃまガッパ",0xC896167E},{"国宝・影正 (6)",0xC9B3A61B},
        {"キン (3)",0xC9F3A283},{"ざしき荒らし",0xCB9AEFEB},{"なんやかんや",0xCBDE2A05},{"ミツマタノヅチ (2)",0xCD310E6C},
        {"ぶんぶく茶ヶ丸",0xCDA0EB09},{"小さいつづら",0xCE793217},{"国宝・影正 (7)",0xCEDE6202},{"ねつぞウナギ",0xCF5E0A94},
        {"おびすさま (6)",0xD202C5D2},{"地獄大山椒 (2)",0xD42A3F2D},{"ギン (3)",0xD66E8197},{"オモテナス (2)",0xD738CE52},
        {"キモ爺",0xD7557CF8},{"鬼くももん (4)",0xD778CACA},{"さまよう怪魔",0xD992B966},{"イカカモネ議長 (9)",0xD9EE8A59},
        {"青いらん",0xDA677C7E},{"絶不蝶",0xDDEBA57A},{"ゲソヒゲール大臣 (6)",0xE01DAD43},{"おびすさま (7)",0xE034A750},
        {"ほらふきザメ",0xE3E1D246},{"あおべえあかべえ (6)",0xE5C183A2},{"ガシャどくろG (2)",0xE6D0D4CF},{"ゲソヒゲール大臣 (7)",0xE770695A},
        {"亡霊武者 (6)",0xE80690C4},{"いくよね",0xE846945C},{"ミツマタノヅチ (3)",0xE85A51B0},{"うすっぺライオン",0xEA1737BF},
        {"枝魔目さま (5)",0xEA2E560A},{"枝魔目さま (6)",0xED439213},{"フゥビン",0xEDB98C4C},{"亡霊武者 (7)",0xEF6B54DD},
        {"地獄大山椒 (3)",0xF14160F1},{"くされモチ",0xF21C41D3},{"ボーどろ",0xF23E2324},{"ドウ (4)",0xF33C331E},
        {"ギヤマンどくろ (2)",0xF4657B21},{"あやとりさま (4)",0xF48AAC77},{"モテアマス (2)",0xF68DF88D},{"ばか草履",0xF6FAC65B},
        {"どろこんぶ",0xF880FAA6},{"おびすさま (8)",0xF92F9611},{"どんどろ (5)",0xFED6C192},{"ペテン師匠",0xFF0C23A2},
    };

    struct IdGroup { const char *name; const IdEntry *list; u32 count; };
    #define GROUP(n, arr) { n, arr, (u32)(sizeof(arr) / sizeof(arr[0])) }
    static const IdGroup g_yokaiGroups[] = {
        GROUP("イサマシ族",   Y_ISAMASHI),
        GROUP("フシギ族",     Y_FUSHIGI),
        GROUP("ゴーケツ族",   Y_GOKETSU),
        GROUP("プリチー族",   Y_PRETTY),
        GROUP("ポカポカ族",   Y_POKAPOKA),
        GROUP("ウスラカゲ族", Y_USURAKAGE),
        GROUP("ブキミー族",   Y_BUKIMI),
        GROUP("ニョロロン族", Y_NYORORON),
        GROUP("レジェンド/限定/真打", Y_SPECIAL),
        GROUP("ボス妖怪 (No.388〜448)", Y_BOSS),
        GROUP("敵専用・図鑑外", Y_ENEMY),
    };
    static const u32 g_yokaiGroupCount = sizeof(g_yokaiGroups) / sizeof(g_yokaiGroups[0]);

    // ---------------- アイテム ----------------
    static const IdEntry I_COIN[] = {
        {"五つ星コイン",0xCB567D2C},{"どきどきコイン(怪)",0x4C29D9CE},{"水色コイン",0xE35507D8},{"青コイン",0x73EA1A49},
        {"緑コイン",0x04ED2ADF},{"桃コイン",0xDAE34BF3},{"オレンジコイン",0x7487DE50},{"黄色コイン",0x0380EEC6},
        {"赤コイン",0x9A89BF7C},{"赤い箱",0x47874761},{"金の手形",0xB2E3D021},{"あやかしコイン",0xE716EAF6},
        {"わくわくコイン(鳥)",0x67048A0D},{"うきうきコイン",0xE07B2EEF},{"ご当地コイン(北海道)",0x60694E14},{"ご当地コイン(東北)",0x176E7E82},
        {"ご当地コイン(関東)",0x8E672F38},{"ご当地コイン(中部)",0xF9601FAE},{"ご当地コイン(近畿)",0x69DF023F},{"ご当地コイン(中国)",0x1ED832A9},
        {"ご当地コイン(四国)",0x7E1FBB4C},{"ご当地コイン(九州)",0x09188BDA},{"ご当地コイン(沖縄)",0x9011DA60},{"わくわくコイン(風)",0x1003BA9B},
        {"わくわくコイン(月)",0xB70FDB21},{"きらきらコイン",0xFE0DDBB7},{"ミュージックカード",0x4C9B4455},{"ダンシングスター",0xAC952579},
        {"ゲートボール",0x46452D56},{"まじめにいきる",0x1BF87993},{"どきどきコインTSUTAYA",0x3B2EE958},{"どきどきコインNAMCO",0xA227B8E2},
        {"超エラベールコイン",0x40817721},{"エラベールコイン",0x39835393},
    };
    static const IdEntry I_TOOL[] = {
        {"ぜっぴん牛乳",0x19A98AAC},{"くじ引き券",0x359C74C3},{"ちからのおふだ",0x15EB282B},{"ようりょくのおふだ",0x8CE27991},
        {"まもりのおふだ",0xFBE54907},{"すばやさのおふだ",0x6581DCA4},{"まつたけ",0xA29139A7},{"スペシャルラーメン",0x1EA02270},
        {"モグモグバーガー",0x1D24F61E},{"りんごあめ",0xF66581B7},{"アッカンベーグル",0x1A2D5EC2},{"あいじょう天むす",0x1BEF34F5},
        {"ナマステカレー",0xA45AFB4C},{"特上しもふり",0xA0D787FE},{"特大ジャンボパフェ",0xA598917B},{"雪かきぴー",0xAD400228},
        {"大トロ",0x15FC654D},{"攻めの秘伝書",0x10A43EAE},{"術の秘伝書",0x89AD6F14},{"必殺の秘伝書",0xFEAA5F82},
        {"まず〜い漢方",0x1429421C},{"にが〜い漢方",0x8D2013A6},{"ふかーい漢方",0xFA272330},{"神けいけんちだま",0x144D2ED9},
        {"ミニけいけんちだま",0x8D447F63},{"小けいけんちだま",0x1320EAC0},{"中けいけんちだま",0x8A29BB7A},{"大けいけんちだま",0xFD2E8BEC},
        {"超けいけんちだま",0x634A1E4F},{"スタミナムアルファ",0x8EA4C7C8},{"プラチナこけし",0x160B9080},{"ざるそば",0xD614B19A},
        {"極上マグロ",0xA115EDC9},{"ヨキシマムゴッド",0x1CE69C29},{"伝説のバッジ",0xA1154BA9},{"伝説のゆびわ",0x2ACFC1C6},
        {"伝説のうでわ",0x2B0DABF1},{"伝説のおまもり",0x294B15A9},{"愛情おむすび",0x69966FD3},{"砂スーツ",0x367A847E},
        {"禍々しい砥石",0x97357395},{"神々しい砥石",0x046E8A07},{"ギガヘルツ玉",0x1D57D7BD},{"白銀のかみどめ",0xF359B691},
        {"プラチナインゴッド",0x9A28735F},{"大将の魂",0xF4347288},{"無双の魂",0x63E6AB00},{"龍神玉",0x9D45B746},
        {"神通力水",0xAF73D5C4},{"人魚の宝玉",0x0936E636},{"サビれた刀",0xDA7339B9},{"いやしの草",0xE75F871A},
        {"親方ダンベル",0x032122E5},{"呪いの日記帳",0xD830E652},{"サポートライフ7月号",0x9BEA1968},{"ガードばんざい",0xF220DCA6},
        {"習得空手講座",0x3E3046D2},{"スーパー黒みつ",0x345E1EF4},{"チーズバーガー",0xF447532B},{"チキンカレー",0xD4300FC3},
        {"やわらか牛すじ",0x39BABA81},{"ジャガジャガチップス",0xDD2AF6A7},{"バリうまスナック",0x4433A71D},{"とんこつラーメン",0xF7C38745},
        {"ハンバーガー",0xF6E6D538},{"ほっこり大根",0xD7B4DBAD},{"マトンカレー",0x4D395E79},{"シーフードカレー",0x3A3E6EEF},
        {"ダブルバーガー",0x834063BD},{"チーズケーキ",0xD5F265F4},{"ショートケーキ",0x4CFB344E},{"ロイヤルパンケーキ",0x3BFC04D8},
        {"極上おでん",0xA7DE2F22},{"エビチリ",0x143E0F7A},{"緑コインの欠片",0xD5499DDD},{"青コインの欠片",0xA24EAD4B},
        {"呪言の刀",0xDAD33977},{"呪言の袴",0x44B7ACD4},{"呪言の盾",0x33B09C42},{"呪言の杖",0xADD409E1},
    };
    static const IdEntry I_SOUL[] = {
        {"モテモ天の魂",0x42008321},{"心オバアの魂",0x7232036C},{"絶オジイの魂",0x9C3C6240},{"くさなぎの魂",0x90E308BD},
        {"モレゾウの魂",0x9121628A},{"虫歯伯爵の魂",0x29921C0D},{"アライ魔将の魂",0x3F352BF2},{"メゾンドワスレの魂",0x7074BD35},
        {"イザナミの魂",0x37410DEE},{"ロボニャンF型の魂",0x81CE4376},{"影オロチの魂",0x04B5055B},{"ふじみ御前の魂",0xAE0A00C2},
        {"モテモテ魂",0x041FF802},{"たらりんの魂",0x50A91C84},{"死神鳥の魂",0xC133D30F},{"ヒグラシまるの魂",0x464C77ED},
        {"ほむら天狗の魂",0x06F3BB02},{"ホリュウの魂",0xA093D466},{"おねむの精の魂",0xB07CF59A},{"スパルタ魂",0xDF4A3FB5},
        {"閃光魂",0x412EAA16},{"よびよせ魂",0xEA11992E},{"ブロッカー魂",0x9D16A9B8},{"ガード魂",0x31445E99},
        {"ちょうはつ魂",0xD827FBAC},{"おんみつ魂",0xA84D0F23},{"壺ガード魂",0x46436E0F},{"かんつう魂",0xAF20CB3A},
        {"なめらかオイル魂",0x36299A80},{"こじらせ魂",0xA6968711},{"大滝魂",0xB1563E62},{"雷神魂",0xC6510EF4},
        {"火の魂",0x58359B57},{"氷の魂",0x9A7B6DA1},{"土の塊",0xC88A86C6},{"雷の塊",0xBF8DB650},
        {"水の魂",0x2F32ABC1},{"風の塊",0xED7C5D37},{"煉獄魂",0xD191B787},{"吹雪魂",0x285F6FD8},
        {"いん石魂",0x5F585F4E},{"嵐魂",0xB63BFA7B},{"黄泉魂",0x03723C1B},{"ヤミ魂",0x74750C8D},{"無の魂",0x7318C894},
    };
    // 「だいじなもの」はメモリ上のバイト列で掲載されていたので逆順 (入力用) に変換済み
    static const IdEntry I_KEY[] = {
        {"廃屋のカギ",0xB15A8104},{"王国の鈴",0x22889F7C},{"ニャーKBの生写真2",0x37FDDB52},{"ニャーKBの生写真3",0xA1CDDC25},
        {"ニャーKBの生写真4",0x44441B45},{"ニャーKBの生写真5",0xD2741C32},{"ニャーKBの生写真6",0x682515AB},{"ニャーKBの生写真7",0xFE1512DC},
        {"ニャーKBの生写真8",0x5D807642},{"ニャーKBのサイン",0xFF718BB5},{"1日乗車券",0x20473E94},{"らくらくフリーパス",0x0C26307A},
        {"スタンプラリーの台紙",0xC2E34113},{"小屋のカギ",0xAFB354E4},{"コマさんの歯車",0xCDBABCB9},{"ひも爺の歯車",0x6E2FD827},
        {"ミカンの種",0x954FCAA7},{"ブドウの種",0x92627240},{"キウイの種",0x04527537},{"スイカの種",0xE1DBB257},
        {"コマニャチの巻物",0xC7FFC499},{"思い出のフタ",0xAD7CF50C},{"ノガッパの歯車",0x5B8ABBCE},{"ラッコちゃん",0x480BAEE9},
        {"天地まんじゅう",0xF2E7D7F7},{"A-201号室のカギ",0xE0FCC73C},{"妖怪ウォッチ・零式",0xF06B9A1D},{"ロックスターの衣装",0x86CEDF80},
        {"ヨカコーラの瓶",0xDE3BA99E},{"A-203号室のカギ",0x8717366E},{"妖魔界へのカギ",0xB67739E3},{"お宝の地図",0x0987C577},
        {"お花見の自転車",0x65B349E9},{"おばあちゃん家メモ",0x7209AC2F},{"釣りざお",0x34150FD1},{"ニャイーンの巻物",0x51CFC3EE},
        {"宿題ドリル",0xFAD07EB8},{"校内のカギ",0xE439AB58},{"妖怪大辞典",0xBBE165A1},{"妖怪ウォッチ",0x2DD162D6},
        {"虫あみ",0x8E440648},{"ゴールドカードキー",0xC742B41E},{"レトロでキレイなおもちゃ",0x25E6CB99},{"レトロでふるびたおもちゃ",0xB3D6CCEE},
        {"カメカメラの人形",0xF25AA770},{"びしょびしょの地図",0x9FB7C200},{"しわくちゃの地図",0x3C22A69E},{"アヒルのおまる",0x646AA007},
        {"シミつきの地図",0xAA12A1E9},{"屋敷のカギ 裏口",0x83D25A0A},{"屋敷のカギ 小",0xEE824FFD},{"屋敷のカギ 大",0x54D34664},
        {"どんどろの素",0x7D13BD87},{"新たなるお宝の地図",0x1043A870},{"新たなるお宝のカギ",0x8673AF07},{"最後のお宝の地図",0x176E1097},
        {"ホース(クリア後)",0x1874013F},
    };
    static const IdGroup g_itemGroups[] = {
        GROUP("コイン系",     I_COIN),
        GROUP("道具・食べ物", I_TOOL),
        GROUP("魂",           I_SOUL),
        GROUP("だいじなもの", I_KEY),
    };
    static const u32 g_itemGroupCount = sizeof(g_itemGroups) / sizeof(g_itemGroups[0]);

    // ---------------- 共通 ----------------
    static bool RequireShinuchi(void)
    {
        u32 low = (u32)(Process::GetTitleID() & 0xFFFFFFFFULL);
        if (low != 0x00155100) { MessageBox(TITLE, "この機能は「真打」専用です。")(); return false; }
        return true;
    }

    // グループ → 項目 の 2 段階選択。選ばれた ID を返す (キャンセルは false)
    static bool PickId(const char *what, const IdGroup *groups, u32 groupCount, u32 &outId, std::string &outName)
    {
        std::vector<std::string> gnames;
        for (u32 i = 0; i < groupCount; ++i) gnames.push_back(groups[i].name);
        Keyboard kg(std::string(what) + " の種類を選んでください");
        kg.Populate(gnames);
        int g = kg.Open();
        if (g < 0 || (u32)g >= groupCount) return false;

        std::vector<std::string> names;
        for (u32 i = 0; i < groups[g].count; ++i) names.push_back(groups[g].list[i].name);
        Keyboard ki(std::string(groups[g].name) + " から選んでください");
        ki.Populate(names);
        int k = ki.Open();
        if (k < 0 || (u32)k >= groups[g].count) return false;

        outId   = groups[g].list[k].id;
        outName = groups[g].list[k].name;
        return true;
    }

    static const char *LookupName(const IdGroup *groups, u32 groupCount, u32 id)
    {
        for (u32 g = 0; g < groupCount; ++g)
            for (u32 i = 0; i < groups[g].count; ++i)
                if (groups[g].list[i].id == id) return groups[g].list[i].name;
        return nullptr;
    }

    // ---- 他ファイル向け公開ヘルパー ----
    u32  GetYokaiGroupCount(void) { return g_yokaiGroupCount; }
    const char *GetYokaiGroupName(u32 g) { return g < g_yokaiGroupCount ? g_yokaiGroups[g].name : ""; }
    u32  GetYokaiGroupSize(u32 g) { return g < g_yokaiGroupCount ? g_yokaiGroups[g].count : 0; }
    void GetYokaiGroupEntry(u32 g, u32 k, const char *&name, u32 &id)
    { name = g_yokaiGroups[g].list[k].name; id = g_yokaiGroups[g].list[k].id; }
    u32  GetItemGroupCount(void) { return g_itemGroupCount; }
    const char *GetItemGroupName(u32 g) { return g < g_itemGroupCount ? g_itemGroups[g].name : ""; }
    u32  GetItemGroupSize(u32 g) { return g < g_itemGroupCount ? g_itemGroups[g].count : 0; }
    void GetItemGroupEntry(u32 g, u32 k, const char *&name, u32 &id)
    { name = g_itemGroups[g].list[k].name; id = g_itemGroups[g].list[k].id; }

    bool PickYokaiId(u32 &outId, std::string &outName) { return PickId("妖怪", g_yokaiGroups, g_yokaiGroupCount, outId, outName); }
    bool PickItemId(u32 &outId, std::string &outName)  { return PickId("アイテム", g_itemGroups, g_itemGroupCount, outId, outName); }
    const char *YokaiNameFromId(u32 id) { return LookupName(g_yokaiGroups, g_yokaiGroupCount, id); }
    const char *ItemNameFromId(u32 id)  { return LookupName(g_itemGroups, g_itemGroupCount, id); }

    // ---------------- 妖怪: 「ぬね」の妖怪を一覧から選んだ妖怪に変える ----------------
    static const u32 YW_BASE = 0x0870DBB0, YW_STRIDE = 0xC4, YW_COUNT = 0x1A1, YW_NAME_MARK = 0xCB82CA82;

    static u32 FindNune(void)
    {
        for (u32 i = 0; i < YW_COUNT; ++i)
        {
            u32 slot = YW_BASE + i * YW_STRIDE, v = 0;
            if (Process::Read32(slot + 0x0C, v) && v == YW_NAME_MARK) return slot;
        }
        MessageBox(TITLE, "名前が「ぬね」の妖怪が見つかりません。\n変更したい妖怪のニックネームを「ぬね」にしてください。")();
        return 0;
    }

    static void NunePickYokaiCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 slot = FindNune();
        if (!slot) return;
        u32 id = 0; std::string name;
        if (!PickId("妖怪", g_yokaiGroups, g_yokaiGroupCount, id, name)) return;
        if (Process::Write32(slot + 0x08, id))
            MessageBox(TITLE, "「ぬね」を " + name + " に変更しました。")();
        else
            MessageBox(TITLE, "書き込みに失敗しました。")();
    }

    static void NuneShowYokaiCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 slot = FindNune();
        if (!slot) return;
        u32 id = 0;
        Process::Read32(slot + 0x08, id);
        char buf[48]; snprintf(buf, sizeof(buf), "ID: %08X", (unsigned)id);
        const char *n = LookupName(g_yokaiGroups, g_yokaiGroupCount, id);
        MessageBox(TITLE, std::string("「ぬね」の妖怪 ") + buf + "\n" + (n ? n : "(一覧に無い妖怪)"))();
    }

    // ---------------- アイテム: 持ち物でカーソルを合わせたアイテムを操作 ----------------
    static const u32 CURSOR_ITEM_ADDR = 0x08A68C94;
    static const u32 ITEM_BASE = 0x08723668, ITEM_STRIDE = 0x10, ITEM_COUNT = 0x1AE;
    static const u32 ITEM_COUNT_OFF = 0x04;   // 真打: 所持数バイトはスロット +4 (ARコード 2872366C より)

    // カーソル中のアイテムIDを持つスロットを探す。ID が入っているスロット内オフセットも返す
    static bool FindCursorItemSlot(u32 &slot, u32 &idOffset, u32 &id)
    {
        if (!Process::Read32(CURSOR_ITEM_ADDR, id) || id == 0)
        {
            MessageBox(TITLE, "カーソル中のアイテムIDを読めませんでした。\n「もちもの」画面でアイテムにカーソルを合わせてから実行してください。")();
            return false;
        }
        static const u32 offs[] = { 0x00, 0x08, 0x0C };
        for (u32 i = 0; i < ITEM_COUNT; ++i)
        {
            u32 s = ITEM_BASE + i * ITEM_STRIDE;
            for (u32 k = 0; k < 3; ++k)
            {
                u32 v = 0;
                if (Process::Read32(s + offs[k], v) && v == id) { slot = s; idOffset = offs[k]; return true; }
            }
        }
        char buf[48]; snprintf(buf, sizeof(buf), "ID %08X", (unsigned)id);
        MessageBox(TITLE, std::string("持ち物データの中に ") + buf + " のスロットが見つかりませんでした。")();
        return false;
    }

    static void CursorItemShowCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 id = 0;
        if (!Process::Read32(CURSOR_ITEM_ADDR, id)) { MessageBox(TITLE, "読み込みに失敗しました。")(); return; }
        char buf[48]; snprintf(buf, sizeof(buf), "ID: %08X", (unsigned)id);
        const char *n = LookupName(g_itemGroups, g_itemGroupCount, id);
        MessageBox(TITLE, std::string("カーソル中のアイテム ") + buf + "\n" + (n ? n : "(一覧に無いアイテム)"))();
    }

    static void CursorItemCountCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 slot = 0, off = 0, id = 0;
        if (!FindCursorItemSlot(slot, off, id)) return;
        u8 cur = 0; Process::Read8(slot + ITEM_COUNT_OFF, cur);
        Keyboard kb("個数を入力してください (1〜99)\n現在: " + std::to_string(cur));
        std::string s;
        if (kb.Open(s, std::to_string(cur)) != 0) return;
        char *end = nullptr; long v = strtol(s.c_str(), &end, 10);
        if (end == s.c_str()) return;
        if (v < 1) v = 1;
        if (v > 99) v = 99;
        if (Process::Write8(slot + ITEM_COUNT_OFF, (u8)v)) MessageBox(TITLE, "個数を " + std::to_string(v) + " にしました。")();
        else MessageBox(TITLE, "書き込みに失敗しました。")();
    }

    static void CursorItemConvertCallback(MenuEntry *entry)
    {
        if (!RequireShinuchi()) return;
        u32 slot = 0, off = 0, id = 0;
        if (!FindCursorItemSlot(slot, off, id)) return;
        u32 newId = 0; std::string name;
        if (!PickId("アイテム", g_itemGroups, g_itemGroupCount, newId, name)) return;
        if (Process::Write32(slot + off, newId))
            MessageBox(TITLE, "カーソル中のアイテムを「" + name + "」に変換しました。\n※画面を開き直すと表示が更新されます。")();
        else
            MessageBox(TITLE, "書き込みに失敗しました。")();
    }

    MenuFolder *CreateIdFolder(void)
    {
        MenuFolder *f = new MenuFolder("妖怪・アイテム 一覧から変更 (真打)",
            "妖怪: ニックネーム「ぬね」の妖怪が対象。アイテム: もちもの画面でカーソルを合わせたものが対象。");
        *f += new MenuEntry("「ぬね」の妖怪を一覧から選んだ妖怪にする", nullptr, NunePickYokaiCallback);
        *f += new MenuEntry("「ぬね」の妖怪の現在のIDを表示",         nullptr, NuneShowYokaiCallback);
        *f += new MenuEntry("カーソル中のアイテムIDを表示",           nullptr, CursorItemShowCallback);
        *f += new MenuEntry("カーソル中のアイテムの個数を変更",        nullptr, CursorItemCountCallback);
        *f += new MenuEntry("カーソル中のアイテムを別のアイテムに変換", nullptr, CursorItemConvertCallback,
                            "選んだアイテムのIDで上書きします。だいじなものへの変換は自己責任で。");
        return f;
    }
}
