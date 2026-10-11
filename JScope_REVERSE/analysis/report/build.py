#!/usr/bin/env python3
"""Assemble report.html from the section fragments in this directory."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.abspath(os.path.join(HERE, '..', '..', 'report.html'))

ORDER = [
    '00a.html',            # CSS
    '00b_masthead.html',   # masthead + TOC + §0
    '01_binary.html',
    '02_arch.html',
    'old_s3.html',         # J-Link API 清单（沿用，已核对）
    '04_connect.html',
    '05_hss.html',
    '06_datamodel.html',
    '07_rtt.html',
    '08_typeid.html',
    '09_struct.html',
    '10_files.html',
    '11_cli.html',
    '12_trigger.html',
    '13_graph.html',
    '14_elf.html',
    '15_infra.html',
    '16_session.html',
    '17_unknowns.html',
    '18_openscope.html',
    '19_outputs.html',
    '99_tail.html',
]


def main():
    parts = []
    missing = []
    for name in ORDER:
        path = os.path.join(HERE, name)
        if not os.path.exists(path):
            missing.append(name)
            continue
        with open(path, encoding='utf-8') as f:
            parts.append(f.read().rstrip('\n'))
    if missing:
        print('MISSING:', ', '.join(missing), file=sys.stderr)
        return 1
    prefix = ('<!doctype html>\n<html lang="zh-CN">\n<head>\n'
              '<meta charset="utf-8">\n'
              '<meta name="viewport" content="width=device-width, initial-scale=1">\n')
    suffix = '\n</body>\n</html>\n'
    doc = prefix + '\n\n'.join(parts) + suffix
    # sanity checks
    for tok in ('</main>', '</body>', '</html>', '</head>'):
        if tok not in doc:
            print(f'WARNING: {tok} not found in output', file=sys.stderr)
    doc = doc.replace('<header class="masthead">', '</head>\n<body>\n\n<header class="masthead">', 1)
    with open(OUT, 'w', encoding='utf-8') as f:
        f.write(doc)
    print(f'wrote {OUT}  ({len(doc):,} bytes, {doc.count("<section"):} sections)')
    return 0


if __name__ == '__main__':
    sys.exit(main())
