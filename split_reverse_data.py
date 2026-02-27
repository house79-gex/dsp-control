#!/usr/bin/env python3
"""
Script per dividere il file docs/reverse _data.md in parti più piccole
in modo che possano essere analizzate da GitHub Copilot.

Uso:
    python split_reverse_data.py [percorso_file] [dimensione_max_kb]

Esempio:
    python split_reverse_data.py "docs/reverse _data.md" 400
"""

import os
import sys
import re


def get_file_size_info(filepath):
    """Mostra informazioni sul file originale."""
    size_bytes = os.path.getsize(filepath)
    size_kb = size_bytes / 1024
    size_mb = size_kb / 1024
    print(f"📄 File: {filepath}")
    print(f"   Dimensione: {size_bytes:,} bytes ({size_mb:.2f} MB)")
    return size_bytes


def find_split_point(text, target_pos):
    """
    Trova il punto migliore dove dividere il testo, cercando
    (in ordine di preferenza):
    1. Un header Markdown (## o ###)
    2. Una riga vuota
    3. Un fine riga qualsiasi
    """
    # Cerca l'header Markdown più vicino (entro 5000 caratteri prima del target)
    search_start = max(0, target_pos - 5000)
    search_region = text[search_start:target_pos]

    # Cerca headers markdown (## o ###)
    headers = list(re.finditer(r'\n(?=#{1,3} )', search_region))
    if headers:
        return search_start + headers[-1].start() + 1  # +1 per saltare il \n

    # Cerca righe vuote
    blank_lines = list(re.finditer(r'\n\n', search_region))
    if blank_lines:
        return search_start + blank_lines[-1].start() + 1

    # Cerca qualsiasi fine riga
    newlines = list(re.finditer(r'\n', search_region))
    if newlines:
        return search_start + newlines[-1].start() + 1

    # Ultimo resort: taglia al target_pos esatto
    return target_pos


def split_file(filepath, max_size_kb=400):
    """Divide il file in parti più piccole."""
    max_size_bytes = max_size_kb * 1024

    # Leggi il file
    print(f"\n📖 Lettura file in corso...")
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    total_size = len(content.encode('utf-8'))
    num_parts_estimate = max(1, (total_size // max_size_bytes) + 1)
    print(f"   Caratteri totali: {len(content):,}")
    print(f"   Parti stimate: ~{num_parts_estimate}")

    # Crea la directory di output
    base_dir = os.path.dirname(filepath)
    output_dir = os.path.join(base_dir, "reverse_data_parts")
    os.makedirs(output_dir, exist_ok=True)
    print(f"📁 Directory output: {output_dir}")

    # Dividi il file
    parts = []
    pos = 0
    part_num = 1

    while pos < len(content):
        # Calcola la posizione target per il taglio
        # Usiamo i caratteri come approssimazione (UTF-8 può variare)
        chunk = content[pos:pos + max_size_bytes]
        actual_size = len(chunk.encode('utf-8'))

        # Se il chunk è troppo grande, riduci
        while actual_size > max_size_bytes and len(chunk) > 1000:
            chunk = chunk[:int(len(chunk) * 0.9)]
            actual_size = len(chunk.encode('utf-8'))

        if pos + len(chunk) >= len(content):
            # Ultimo pezzo
            end_pos = len(content)
        else:
            # Trova un punto di taglio intelligente
            end_pos = find_split_point(content, pos + len(chunk))

        part_content = content[pos:end_pos]
        part_size = len(part_content.encode('utf-8'))

        # Aggiungi intestazione alla parte
        header = f"<!-- PARTE {part_num} di ~{num_parts_estimate} | "
        header += f"Caratteri: {len(part_content):,} | "
        header += f"Bytes: {part_size:,} -->\n\n"

        # Scrivi la parte
        part_filename = f"reverse_data_part{part_num:03d}.md"
        part_path = os.path.join(output_dir, part_filename)

        with open(part_path, 'w', encoding='utf-8') as f:
            f.write(header + part_content)

        parts.append({
            'num': part_num,
            'filename': part_filename,
            'path': part_path,
            'size_bytes': part_size,
            'chars': len(part_content),
        })

        print(f"   ✅ Parte {part_num:3d}: {part_filename} "
              f"({part_size / 1024:.1f} KB, {len(part_content):,} caratteri)")

        pos = end_pos
        part_num += 1

    # Riepilogo
    print(f"\n{'=' * 60}")
    print(f"✅ COMPLETATO!")
    print(f"   File originale: {total_size / 1024 / 1024:.2f} MB")
    print(f"   Parti create:   {len(parts)}")
    print(f"   Directory:      {output_dir}")
    print(f"{'=' * 60}")

    # Genera indice
    index_path = os.path.join(output_dir, "INDEX.md")
    with open(index_path, 'w', encoding='utf-8') as f:
        f.write(f"# Indice reverse_data - {len(parts)} parti\n\n")
        f.write(f"File originale: `{os.path.basename(filepath)}` "
                f"({total_size / 1024 / 1024:.2f} MB)\n\n")
        f.write(f"| # | File | Dimensione | Caratteri |\n")
        f.write(f"|---|------|-----------|----------|\n")
        for p in parts:
            f.write(f"| {p['num']} | [{p['filename']}]({p['filename']}) | "
                    f"{p['size_bytes'] / 1024:.1f} KB | {p['chars']:,} |\n")

    print(f"📋 Indice creato: {index_path}")
    print(f"\n💡 Ora puoi caricare le singole parti sulla repository")
    print(f"   o incollarle in chat per l'analisi del protocollo RS-485.")

    return parts


def main():
    # Argomenti da linea di comando
    if len(sys.argv) >= 2:
        filepath = sys.argv[1]
    else:
        # Default: cerca il file nella posizione standard
        filepath = os.path.join("docs", "reverse _data.md")

    if len(sys.argv) >= 3:
        max_size_kb = int(sys.argv[2])
    else:
        max_size_kb = 400  # 400 KB default (ben sotto il limite GitHub API)

    # Verifica che il file esista
    if not os.path.exists(filepath):
        print(f"❌ File non trovato: {filepath}")
        print(f"\nUso: python {sys.argv[0]} [percorso_file] [dimensione_max_kb]")
        print(f'Esempio: python {sys.argv[0]} "docs/reverse _data.md" 400')
        sys.exit(1)

    # Info file
    get_file_size_info(filepath)

    # Esegui split
    split_file(filepath, max_size_kb)


if __name__ == "__main__":
    main()
