#!/bin/bash
set -e
cd "$(dirname "$0")"

echo "== 古い成果物を削除 =="
rm -f workspace.elf workspace.3gx BlankTemplate.elf BlankTemplate.3gx

echo "== Dockerコンテナ内でビルド (libctrpf 0.8.0と同じツールチェーン) =="
docker run --rm -v "$PWD":/workspace -w /workspace pablomk7/libctrpf:0.8.0 make

mv workspace.elf BlankTemplate.elf
mv workspace.3gx BlankTemplate.3gx

echo "== v2形式(3GX\$0002)で再パッケージ =="
3gxtool -s BlankTemplate.elf CTRPluginFramework.plgInfo BlankTemplate.3gx

echo "== 完了 =="
xxd -l 8 BlankTemplate.3gx
echo "BlankTemplate.3gx を SDカードの luma/plugins/<TitleID>/ にコピーしてください。"
