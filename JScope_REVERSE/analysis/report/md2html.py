#!/usr/bin/env python3
"""Convert an analysis .md report into a <section> fragment matching report.html's design system.

Usage: md2html.py <input.md> <out.html> <secnum> <secid> [<h2 title override>]
"""
import html
import re
import sys

BADGE = {
    '[观测]': '<span class="badge b-obs">观测</span>',
    '[推导]': '<span class="badge b-der">推导</span>',
    '[推断]': '<span class="badge b-inf">推断</span>',
    '[未知]': '<span class="badge b-unk">未知</span>',
}


def inline(text: str) -> str:
    """Inline markup: escape HTML, then apply code/bold/badges."""
    # protect inline code spans first
    spans = []

    def stash(m):
        spans.append(m.group(1))
        return f'\x00{len(spans)-1}\x00'

    text = re.sub(r'`([^`]+)`', stash, text)
    text = html.escape(text, quote=False)
    for k, v in BADGE.items():
        text = text.replace(html.escape(k, quote=False), v)
    text = re.sub(r'\*\*([^*]+)\*\*', r'<strong>\1</strong>', text)
    text = re.sub(r'(?<![\w*])\*([^*\n]+)\*(?![\w*])', r'<em>\1</em>', text)
    # links [t](u)
    text = re.sub(r'\[([^\]]+)\]\((https?://[^)]+)\)', r'<a href="\2">\1</a>', text)

    def unstash(m):
        return '<code>' + html.escape(spans[int(m.group(1))], quote=False) + '</code>'

    text = re.sub(r'\x00(\d+)\x00', unstash, text)
    return text


def convert(md: str, secnum: str, secid: str, title_override=None) -> str:
    lines = md.split('\n')
    out = []
    i = 0
    first_h1 = True
    title = title_override
    while i < len(lines):
        line = lines[i]
        # fenced code
        if line.startswith('```'):
            lang = line[3:].strip()
            i += 1
            buf = []
            while i < len(lines) and not lines[i].startswith('```'):
                buf.append(lines[i])
                i += 1
            i += 1
            code = html.escape('\n'.join(buf), quote=False)
            cap = f'<p class="cap">{html.escape(lang)}</p>' if lang and lang not in ('c', 'text') else ''
            out.append('<div class="code">\n' + cap + '<pre><code>' + code + '</code></pre>\n</div>')
            continue
        # table
        if line.lstrip().startswith('|') and i + 1 < len(lines) and re.match(r'^\s*\|[\s:|-]+\|\s*$', lines[i+1]):
            def cells(row):
                # honour \| escapes inside table cells
                parts = row.strip().strip('|').replace('\\|', '\x01').split('|')
                return [c.strip().replace('\x01', '|') for c in parts]

            header = cells(line)
            i += 2
            rows = []
            while i < len(lines) and lines[i].lstrip().startswith('|'):
                rows.append(cells(lines[i]))
                i += 1
            t = ['<div class="tw">', '<table>', '<thead><tr>']
            t += [f'<th>{inline(h)}</th>' for h in header]
            t += ['</tr></thead>', '<tbody>']
            for r in rows:
                t.append('<tr>' + ''.join(f'<td>{inline(c)}</td>' for c in r) + '</tr>')
            t += ['</tbody>', '</table>', '</div>']
            out.append('\n'.join(t))
            continue
        # headings
        m = re.match(r'^(#{1,6})\s+(.*)$', line)
        if m:
            level, text = len(m.group(1)), m.group(2)
            if level == 1 and first_h1:
                first_h1 = False
                title = title or text
                out.append(f'<h2><span class="sec-n">{secnum}</span>{inline(title)}</h2>')
            elif level == 1:
                out.append(f'<h3>{inline(text)}</h3>')
            elif level == 2:
                out.append(f'<h3>{inline(text)}</h3>')
            elif level == 3:
                out.append(f'<h4>{inline(text)}</h4>')
            else:
                out.append(f'<h4>{inline(text)}</h4>')
            i += 1
            continue
        # blockquote
        if line.startswith('>'):
            buf = []
            while i < len(lines) and lines[i].startswith('>'):
                buf.append(lines[i].lstrip('>').strip())
                i += 1
            body = '\n'.join(buf)
            paras = [inline(p.strip()) for p in body.split('\n') if p.strip()]
            out.append('<div class="callout">\n' + '\n'.join(f'<p>{p}</p>' for p in paras) + '\n</div>')
            continue
        # lists
        if re.match(r'^\s*([-*]|\d+\.)\s+', line):
            ordered = bool(re.match(r'^\s*\d+\.\s+', line))
            items = []
            while i < len(lines) and re.match(r'^\s*([-*]|\d+\.)\s+', lines[i]):
                item = re.sub(r'^\s*([-*]|\d+\.)\s+', '', lines[i])
                i += 1
                # continuation lines (indented)
                while i < len(lines) and lines[i].startswith('  ') and lines[i].strip() and not re.match(r'^\s*([-*]|\d+\.)\s+', lines[i]):
                    item += ' ' + lines[i].strip()
                    i += 1
                items.append(f'<li>{inline(item)}</li>')
            tag = 'ol' if ordered else 'ul'
            out.append(f'<{tag}>\n' + '\n'.join(items) + f'\n</{tag}>')
            continue
        # hr
        if re.match(r'^\s*---+\s*$', line):
            out.append('<hr>')
            i += 1
            continue
        # blank
        if not line.strip():
            i += 1
            continue
        # paragraph (join until blank)
        buf = [line]
        i += 1
        while i < len(lines) and lines[i].strip() and not re.match(r'^(#{1,6}\s|```|\s*[-*]\s|\s*\d+\.\s|>|\s*\|)', lines[i]):
            buf.append(lines[i])
            i += 1
        out.append(f'<p>{inline(" ".join(x.strip() for x in buf))}</p>')

    body = '\n      '.join(out)
    return f'    <!-- ══ {secnum} ══ -->\n    <section id="{secid}">\n      {body}\n    </section>\n'


if __name__ == '__main__':
    src, dst, secnum, secid = sys.argv[1:5]
    title = sys.argv[5] if len(sys.argv) > 5 else None
    md = open(src, encoding='utf-8').read()
    open(dst, 'w', encoding='utf-8').write(convert(md, secnum, secid, title))
    print('wrote', dst)
