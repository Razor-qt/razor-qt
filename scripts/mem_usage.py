#!/usr/bin/env python
#
# Results are in kB
#

import sys
import re

def permute(args):
    ret = []
    if args:
        first = args.pop(0)
        for y in permute(args):
            for x in first:
                ret.append(x + y)
    else:
        ret.append('')
    return ret

def parsed_groups(match, *types):
    groups = match.groups()
    assert len(groups) == len(types)
    return tuple([type(group) for group, type in zip(groups, types)])

class VMA(dict):
    def __init__(self, *args):
        (self.start, self.end, self.perms, self.offset,
         self.major, self.minor, self.inode, self.filename) = args

def parse_smaps(pid):
    maps = open('/proc/' + pid + '/smaps', 'r')
    
    hex = lambda s: int(s, 16)
    
    ret = []
    header = re.compile(r'^([0-9a-f]+)-([0-9a-f]+) (....) ([0-9a-f]+) '
                        r'(..):(..) (\d+) *(.*)$')
    detail = re.compile(r'^(.*): +(\d+) kB')
    for line in maps:
        m = header.match(line)
        if m:
            vma = VMA(*parsed_groups(m, hex, hex, str, hex, str, str, int, str))
            ret.append(vma)
        else:
            m = detail.match(line)
            if m:
                k, v = parsed_groups(m, str, int)
                assert k not in vma
                vma[k] = v
            else:
                print 'unparseable line:', line
    return ret

perms = permute(['r-', 'w-', 'x-', 'ps'])

def make_summary_dicts(vmas):
    mapped = {}
    anon = {}
    for d in mapped, anon:
        # per-perm
        for k in perms:
            d[k] = {}
            d[k]['Size'] = 0
            for y in 'Shared', 'Private':
                d[k][y] = {}
                for z in 'Clean', 'Dirty':
                    d[k][y][z] = 0
        # totals
        for y in 'Shared', 'Private':
            d[y] = {}
            for z in 'Clean', 'Dirty':
                d[y][z] = 0

    for vma in vmas:
        if vma.major == '00' and vma.minor == '00':
            d = anon
        else:
            d = mapped
        for y in 'Shared', 'Private':
            for z in 'Clean', 'Dirty':
                d[vma.perms][y][z] += vma.get(y + '_' + z, 0)
                d[y][z] += vma.get(y + '_' + z, 0)
        d[vma.perms]['Size'] += vma.get('Size', 0)
    return mapped, anon
    
def values(d, args):
    if args:
        ret = ()
        first = args[0]
        for k in first:
            ret += values(d[k], args[1:])
        return ret
    else:
        return (d,)

def print_summary(dicts_and_titles):
    def desc(title, perms):
        ret = {('Anonymous', 'rw-p'): 'Data (malloc, mmap)',
               ('Anonymous', 'rwxp'): 'Writable code (stack)',
               ('Mapped', 'r-xp'): 'Code',
               ('Mapped', 'rwxp'): 'Writable code (jump tables)',
               ('Mapped', 'r--p'): 'Read-only data',
               ('Mapped', 'rw-p'): 'Data'}.get((title, perms), None)
        if ret:
            return '  -- ' + ret
        else:
            return ''
    
    for d, title in dicts_and_titles:
        print title, 'memory:'
        print '               Shared            Private'
        print '           Clean    Dirty    Clean    Dirty'
        for k in perms:
            if d[k]['Size']:
                print ('    %s %7d  %7d  %7d  %7d%s'
                       % ((k,)
                          + values(d[k], (('Shared', 'Private'),
                                          ('Clean', 'Dirty')))
                          + (desc(title, k),)))
        print ('   total %7d  %7d  %7d  %7d'
               % values(d, (('Shared', 'Private'),
                            ('Clean', 'Dirty'))))
    
    print '   ' + '-' * 40
    print ('   total %7d  %7d  %7d  %7d'
           % tuple(map(sum, zip(*[values(d, (('Shared', 'Private'),
                                             ('Clean', 'Dirty')))
                                  for d, title in dicts_and_titles]))))

def main(pid):
    vmas = parse_smaps(pid)
    mapped, anon = make_summary_dicts(vmas)
    print_summary(((mapped, "Mapped"), (anon, "Anonymous")))

if __name__ == '__main__':
    sys.exit(main(*sys.argv[1:]))
