import sys
import baf.baf
from baf.baf.datatypes import *
from ascii import ascii_list


class ChunkHeader(Block):
    magic = Bytes(8)

    def set_magic(self, data: dict):
        return data['magic'].encode('UTF8')


class CharDef(Block):
    character = U8()
    glyph = Bytes(7)

    def set_character(self, data: dict[str | int, list]):
        ch_model = self.character
        datum = ch_model.instantiate(self)
        for k, _ in data.items():
            if isinstance(k, str):
                datum.build(ord(k))
            else:
                datum.build(k)
        return datum

    def set_glyph(self, data: dict[str | int, list]):
        gl_model = self.glyph
        datum = gl_model.instantiate(self)
        for _, v in data.items():
            datum.build(bytes(v))
        return datum


class ASCII(Block):
    header = ChunkHeader()
    c_size = U32()
    pad = Align(8)
    data = Array(CharDef())

    def set_c_size(self, data):
        return self.data.size()

    def set_header(self, data):
        model = self.header
        datum = model.instantiate(self)
        datum.build({"magic": "ASCIIFNT"})
        return datum

    def set_data(self, data):
        model = self.data
        datum = model.instantiate(self)
        datum.build([{k: v} for k, v in ascii_list.items()])
        return datum


class MarqueeROM(Block):
    magic = Bytes(8)
    version = U32()
    size = U32()
    pad1 = Align(8)
    ascii = ASCII()

    def set_size(self, data):
        return self.ascii.size()

    def set_magic(self, data):
        return bytes(data['magic'], 'UTF8')

    def set_ascii(self, data):
        asc = self.ascii
        datum = asc.instantiate(self)
        datum.build(data)
        return datum


if __name__ == '__main__':
    try:
        rom = baf.baf.build_toml(MarqueeROM, "rom.toml")
    except Exception as e:
        print(f"ERROR: {e}\nTraceback:")
        print('\n'.join(e.__notes__))
        sys.exit(1)

    with open('MARQUEE.ROM', 'wb') as fp:
        fp.write(rom.get_bytes())
