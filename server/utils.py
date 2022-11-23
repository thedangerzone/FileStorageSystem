import struct, collections

class CStruct(object):

    def __init__(self, typename, format_defn, lead_char="<"):
        self.names = []
        fmts = [lead_char]
        for line in format_defn.splitlines():
            name, fmt = line.split()
            self.names.append(name)
            fmts.append(fmt)
        self.formatstr = ''.join(fmts)
        self.struct = struct.Struct(self.formatstr)
        self.named_tuple_class = collections.namedtuple(typename, self.names)

    def object_from_bytes(self, byte_str):
        atuple = self.struct.unpack(byte_str)
        return self.named_tuple_class._make(atuple)