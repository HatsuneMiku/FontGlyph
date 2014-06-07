#!/usr/local/bin/python
# -*- coding: utf-8 -*-
'''drawstrokes
http://deztec.jp/x/10/faireal/d70520.xml
prj/tools/fonts/Typography/fontforge/fonttools/ showttf.c stripttc.c
 3. showttf http://d.hatena.ne.jp/project_the_tower2/20080530/1212147881
 9. format2 http://d.hatena.ne.jp/project_the_tower2/20080910/1221039009
10. format4 http://d.hatena.ne.jp/project_the_tower2/20080912/1221181313
11. format4 http://d.hatena.ne.jp/project_the_tower2/20080913/1221276002
12. format6 http://d.hatena.ne.jp/project_the_tower2/20080913/1221295285
18. showttf http://d.hatena.ne.jp/project_the_tower2/20080922/1222101267
20. py glyf http://d.hatena.ne.jp/project_the_tower2/20100630/1277838854
20. tables http://f.hatena.ne.jp/project_the_tower2/20100619232059
'''

import sys, os
import struct
import StringIO
import wx
import logging

class Glyph(object):
  glyphs_flg = ''.join((
    '022652', 'a924949f49f5c98ff99320', # Q
    '010b', 'fff0',                     # H
    '02181f', 'c93924c9',               # e
    '0103', 'f0',                       # l
    '020b17', '924924',                 # o
    '0103', 'f0',                       # ,
    '010c', 'fff8',                     # w
    '0112', 'c9f920',                   # r
    '021320', 'e4927c9200',             # d
    '020307', 'ff'))                    # !
  glyphs_str = ''.join((
    '00530051', # Q
    '00c9fff6ffcbfff6ffe4002dffe6001bfff4002dffeb0044ffe4009cffff0053',
    '00010059001700a3001f003c0024003f0051004f0031fffa000efffa001cffe7',
    '001800040014fffc0024ffd6000fffe1003aff98fffcff3c0004ffa4ffd7ff6f',
    'ffe8ffc2000bfff1001affeb000f0000001f002000100004000dfff10005ffdb',
    'ffecffe1ffd9ffddffe60000ffd80029ffe9000fffdfffc8ffb6ffd100300067',
    '0021002afffe002afff00028fff9000bffcb0026ffcfffb0fffdffceffe20004',
    'fffa002a0015003e0017002000330019001c0005000ffff6001cffe6000affe1',
    '0015ffd1001e004e000900d0fff400bbfff20029ffe4002affe6000bfff1ffe7',
    'ffeb000effe7fff6fff9001a00050015ffee0005ffe1fffbffc9ffd1ffe6ffd7',
    'ffdcff9dfff6ffccfff3ff920003ffc20007ffbc001aff880014ffc8001dffd6',
    '0013ffec00360000001a0000',
    '000c0048', # H
    '053b0000ff3a0000000002d9fd1900000000fd27ff3a0000000005d100c60000',
    '0000fdb802e700000000024800c60000',
    '00200065', # e
    '0460021bfcc900000000ff99003eff670036ffcf0034ffd0008fffd000560000',
    '0072000000e7005b0031002c000a00000000ff33ffa1ffd8ff3affcaff930000',
    'feea0000fec8012d0000011500000112012b014200f4000000e2000000f5fef8',
    '0000ff0dff49002affff0094ff6d00a2ff6a0000ff690000ff4dff4efff4ff7c',
    '0004006c', # l
    '01770000ff4400000000061400bc0000',
    '0018006f', # o
    '0471022e0000feeffee8fec4ff150000ff130000fee9013c0000011100000111',
    '0117013d00ed000000eb00000118fec3ff3efeef000000d9ff5600d3ff690000',
    'ff670000ff57ff2d0000ff270000ff2e00aaff27009800000096000000ab00d7',
    '0004002c', # ,
    '023f011dfee6fd71ff6e000000ae028f',
    '000d0077', # w
    '0636045dfeddfba3ff520000fee1035dfee3fca3ff530000feda045d00c40000',
    '00cdfc9f01170361009b0000011efc9f00c20361',
    '00130072', # r
    '03720390fff60000ffd6000affb10009ffca0000ffa90000ff5effb3ffb5ffc3',
    '0000fce7ff4400000000045d00bc00000000ff5b0070005a00ab004b00590000',
    '00310000002cfffb002cfff9',
    '00210064', # d
    '04430000ff44000000000075ffafffbaff50ffb2ff990000ff380000ff150134',
    '000001110000008e005100de0045004e0044004c00b500500061000000580000',
    '0088ffdb004bffd9000001e400bc0000ff44faff00000281ffb40022ff88001a',
    'ffb90000ff620000ff50ff240000ff360000ff390088ff310096000000500000',
    '00a40047',
    '00080021', # !
    '020305d1ffe6fbc9ff560000ffe4043700d6fa2fff340000000000d300cc0000'))

  def __init__(self, ttf):
    self.ttf, self.glyphs = ttf, []
    self.dc, self.sz = None, (0, 0)
    self.offset, self.space, self.scale = (0, 0), (0, 0), 1
    if self.ttf.fontfilename is not None: return
    if len(self.glyphs_str) % 8: lg.error('bad glyphs_str length\a\n')
    s = StringIO.StringIO(self.glyphs_str)
    it = iter(lambda: (s.read(4), s.read(4)), ('', ''))
    self.glyphs = [(self.signedshort(i), self.signedshort(j)) for i, j in it]

  def scalex(self, x):
    return self.offset[0] + x / self.scale

  def scaley(self, y):
    return self.sz[1] - (self.offset[1] + y / self.scale)

  def signedshort(self, h):
    i = int(h, 16)
    return i if i < 32768 else (i - 65536)

  def unsignedshort(self, i):
    return '%04x' % (i if i >= 0 else (i + 65536))

  def getDummyGlyph(self, ch):
    pos, glyf, fpos, curvePoints, epoc = 0, [], 0, [], []
    while pos < len(self.glyphs):
      num, code = self.glyphs[pos]
      epnum, fnum = int(self.glyphs_flg[fpos:fpos+2], 16), (num + 7) / 8
      if code == ord(ch):
        glyf = self.glyphs[pos+1:pos+num+1]
        for i in xrange(epnum):
          epoc.append(int(self.glyphs_flg[fpos+(i+1)*2:fpos+(i+1)*2+2], 16))
        for i in xrange(fnum):
          b = int(self.glyphs_flg[fpos+(i+epnum+1)*2:fpos+(i+epnum+1)*2+2], 16)
          m = 0x0080
          for j in xrange(8 - ((fnum * 8 - num) if i == fnum - 1 else 0)):
            curvePoints.append(CurvePoint(1 if b & m else 0))
            m >>= 1
        for i, (x, y) in enumerate(glyf):
          px, py = x + (px if i else 0), y + (py if i else 0)
          '''
          print '(%6d, %6d) -> (%6d, %6d)' % (x, y, px, py)
          print '(0x%s, 0x%s)' % (self.unsignedshort(x), self.unsignedshort(y))
          '''
          curvePoints[i].x, curvePoints[i].y = px, py
        return (curvePoints, epoc)
      pos += num + 1
      fpos += (epnum + fnum + 1) * 2
    return (curvePoints, epoc)

  def drawGlyph(self, ch):
    w, h, curvePoints, endPtsOfContours = 80, 160, [], []
    if self.ttf.fontfilename is not None:
      glyf = self.ttf.get_glyph(ch)
      if glyf is not None:
        curvePoints, endPtsOfContours = glyf.curvePoints, glyf.endPtsOfContours
    else:
      curvePoints, endPtsOfContours = self.getDummyGlyph(ch)
    if len(curvePoints) == 0: return (w, h)
    gc = wx.GraphicsContext.Create(self.dc)
    gc.PushState()
    gc.SetPen(wx.Pen(wx.GREEN, 1))
    gc.SetBrush(wx.Brush('green', wx.SOLID))
    self.path = gc.CreatePath()
    self.state = 0
    for j in xrange(len(endPtsOfContours)):
      k = (endPtsOfContours[j-1] + 1) if j else 0
      for i in xrange(k, endPtsOfContours[j] + 1):
        cur = curvePoints[i]
        self.state = i - k
        if i == k and not (cur.flg & 0x01): lg.error('off curve first\a\n')
        if i != k:
          nxt = curvePoints[k if i == endPtsOfContours[j] else (i + 1)]
          if self.curve(prv, cur, nxt): continue
        prv = CurvePoint(cur.flg, cur.x, cur.y) # copy.copy(cur)
      self.state = i - k
      self.curve(prv, curvePoints[k], curvePoints[k + 1])
    gc.DrawPath(self.path)
    gc.PopState()
    if True:
      for j in xrange(len(endPtsOfContours)):
        k = (endPtsOfContours[j-1] + 1) if j else 0
        for i in xrange(k, endPtsOfContours[j] + 1):
          cur = curvePoints[i]
          r = 4 if i == k else 3
          col = wx.BLUE if cur.flg & 0x01 else wx.RED # On Curve / Off Curve
          self.dc.SetPen(wx.Pen(col, 1))
          if i == k: self.dc.SetBrush(wx.Brush('green', wx.SOLID))
          self.dc.DrawCircle(self.scalex(cur.x), self.scaley(cur.y), r)
          if i == k: self.dc.SetBrush(wx.Brush('white', wx.SOLID))
          self.dc.SetPen(wx.Pen(wx.BLACK, 1))
    return (w, h)

  def bezier(self, px, py, x, y, nx, ny):
    if self.state == 1:
      self.path.MoveToPoint(wx.Point2D(self.scalex(px), self.scaley(py)))
    self.path.AddQuadCurveToPoint(
      self.scalex(x), self.scaley(y), self.scalex(nx), self.scaley(ny))

  def curve(self, prv, cur, nxt):
    if cur.flg & 0x01:
      if not (prv.flg & 0x01): lg.error('off curve bug\a\n')
      self.stroke(prv.x, prv.y, cur.x, cur.y)
      return False
    if nxt.flg & 0x01:
      self.bezier(prv.x, prv.y, cur.x, cur.y, nxt.x, nxt.y)
      prv.flg, prv.x, prv.y = nxt.flg, nxt.x, nxt.y # not copy.copy(nxt)
      return True
    else:
      mx, my = (cur.x + nxt.x) / 2, (cur.y + nxt.y) / 2
      self.bezier(prv.x, prv.y, cur.x, cur.y, mx, my)
      prv.flg, prv.x, prv.y = 1, mx, my
      return True

  def stroke(self, xs, ys, xe, ye):
    if self.state == 1:
      self.path.MoveToPoint(wx.Point2D(self.scalex(xs), self.scaley(ys)))
    self.path.AddLineToPoint(self.scalex(xe), self.scaley(ye))

  def drawStrokes(self, u):
    ox, oy = self.offset[0], self.offset[1] # copy.copy(self.offset)
    for ch in u:
      w, h = self.drawGlyph(ch)
      if ch == '\n':
        self.offset[0] = ox
        self.offset[1] -= h + self.space[1]
      else:
        self.offset[0] += w + self.space[0]

  def setDrawInfo(self, dc, offset, space, scale):
    self.dc, self.sz = dc, dc.GetSize()
    self.offset, self.space, self.scale = offset, space, scale

class MyPanel(wx.Panel):
  def __init__(self, ttf, *args, **kwargs):
    super(MyPanel, self).__init__(*args, **kwargs)
    self.glyph = Glyph(ttf)
    self.offset, self.space, self.scale = [10, 240], [30, 60], 10
    self.Bind(wx.EVT_PAINT, self.OnPaint)

  def OnPaint(self, event=None):
    dc = wx.PaintDC(self) # event.GetEventObject()
    dc.BeginDrawing()
    dc.Clear()
    dc.SetPen(wx.Pen(wx.BLACK, 1))
    dc.SetBrush(wx.Brush('white', wx.SOLID))
    self.glyph.setDrawInfo(dc, self.offset, self.space, self.scale)
    self.glyph.drawStrokes('Hello,\nworld!')
    dc.EndDrawing()

class MyFrame(wx.Frame):
  def __init__(self, ttf, *args, **kwargs):
    super(MyFrame, self).__init__(*args, **kwargs)
    sz = wx.BoxSizer(wx.HORIZONTAL)
    self.pnl = MyPanel(ttf=ttf, parent=self)
    sz.Add(self.pnl, 1, wx.EXPAND)
    self.SetSizer(sz)

class MyApp(wx.App):
  def __init__(self, ttf, *args, **kwargs):
    super(MyApp, self).__init__(*args, **kwargs)
    self.frm = MyFrame(ttf=ttf, parent=None, id=wx.NewId(),
      title='font', size=(640, 480), pos=(320, 240))
    self.SetTopWindow(self.frm)
    self.frm.Show()

class TTFBaseTable(object):
  propinfo = []

  def __init__(self, f):
    self.setattrs_from_info(f, self.propinfo)
    self.appendix = []

  def __repr__(self):
    return '\n'.join(' %s: %s' % (p, self.get_str(q, p)) \
      for (q, p) in self.propinfo + self.appendix)

  def get_str(self, q, p):
    v = getattr(self, p)
    if isinstance(v, str): return v
    return '%016x' % v

  def set_appendix(self, f, lst):
    self.setattrs_from_info(f, lst)
    self.appendix += lst

  def setattrs_from_info(self, f, info):
    if len(info) == 0: return
    fmt = ''.join(['>'] + [q for (q, p) in info])
    d = struct.unpack(fmt, f.read(struct.calcsize(fmt)))
    for i, (q, p) in enumerate(info): setattr(self, p, d[i])

class TTFCmap(TTFBaseTable):
  def __init__(self, *args, **kwargs):
    self.propinfo = [
      ('H', 'table_version'), ('H', 'numEncodingTables')]
    super(TTFCmap, self).__init__(*args, **kwargs)

class TTFCmapEncodingTable(TTFBaseTable):
  def __init__(self, *args, **kwargs):
    self.propinfo = [
      ('H', 'platformId'), ('H', 'specificEncodingId'), ('L', 'offset')]
    super(TTFCmapEncodingTable, self).__init__(*args, **kwargs)

class TTFCmapSubTableFormat(TTFBaseTable):
  def __init__(self, *args, **kwargs):
    self.propinfo = [
      ('H', 'format'), ('H', 'length'), ('H', 'version')]
    super(TTFCmapSubTableFormat, self).__init__(*args, **kwargs)

class TTFMaxp(TTFBaseTable):
  def __init__(self, *args, **kwargs):
    self.propinfo = [
      ('L', 'table_version'), ('H', 'numGlyphs')]
    super(TTFMaxp, self).__init__(*args, **kwargs)

class TTFHead(TTFBaseTable):
  def __init__(self, *args, **kwargs):
    self.propinfo = [
      ('L', 'table_version'), ('L', 'fontRevision'),
      ('L', 'checkSumAdjustment'), ('L', 'magicNumber'),
      ('H', 'flags'), ('H', 'unitsPerEm'), ('Q', 'created'), ('Q', 'modified'),
      ('H', 'xMin'), ('H', 'yMin'), ('H', 'xMax'), ('H', 'yMax'), # hhhh
      ('H', 'macStyle'), ('H', 'lowestRecPPEM'), ('h', 'fontDirectionHint'),
      ('h', 'indexToLocFormat'), ('h', 'glyphDataFormat')]
    super(TTFHead, self).__init__(*args, **kwargs)

class TTFGlyfHead(TTFBaseTable):
  def __init__(self, *args, **kwargs):
    self.propinfo = [
      ('h', 'numContours'), # positive: single-glyph, negative: multi-glyph
      ('h', 'xMin'), ('h', 'yMin'), ('h', 'xMax'), ('h', 'yMax')]
    super(TTFGlyfHead, self).__init__(*args, **kwargs)

class CurvePoint(object):
  def __init__(self, flg=0, x=0, y=0):
    self.flg, self.x, self.y = flg, x, y

class TTFGlyfSingle(TTFBaseTable):
  def __init__(self, glyfHead, *args, **kwargs):
    super(TTFGlyfSingle, self).__init__(*args, **kwargs)
    f = args[0]
    self.glyfHead = glyfHead
    fmt = '>%dH' % self.glyfHead.numContours
    self.endPtsOfContours = struct.unpack(fmt, f.read(struct.calcsize(fmt)))
    self.instructionLength = struct.unpack('>H', f.read(2))[0]
    fmt = '>%dB' % self.instructionLength
    self.instructions = struct.unpack(fmt, f.read(struct.calcsize(fmt)))
    r, numFlags = 0, self.endPtsOfContours[-1] + 1
    self.curvePoints = [CurvePoint() for _ in xrange(numFlags)]
    for i in xrange(numFlags):
      if r > 0: flg, r = self.curvePoints[i-1].flg, r - 1
      else:
        b = struct.unpack('>B', f.read(1))[0]
        if r == -1: flg, r = self.curvePoints[i-1].flg, b - 1
        else: flg, r = b, -1 if b & 0x08 else 0
      self.curvePoints[i].flg = flg
    for n, coords in enumerate(('x', 'y')):
      for i in xrange(numFlags):
        p = self.curvePoints[i].flg & (0x10 << n)
        w = 'B' if self.curvePoints[i].flg & (0x02 << n) else 'h'
        fmt = '>%s' % w
        if w != 'B' and p: v = 0
        else: v = struct.unpack(fmt, f.read(struct.calcsize(fmt)))[0]
        if w == 'B' and not p: v = -v
        setattr(self.curvePoints[i], coords,
          v + (getattr(self.curvePoints[i-1], coords) if i else 0))

  def __repr__(self):
    s = [''.join(' %04x' % p for p in self.endPtsOfContours)]
    s.append(' instructionLength: %d' % self.instructionLength)
    s.append(' instructions: ...') # self.instructions
    for i, cp in enumerate(self.curvePoints):
      flgname = ('YDual/P', 'XDual/P', 'Repeat', 'Y-Short', 'X-Short', 'On')
      sflg = [(m if cp.flg & (1 << (len(flgname) - 1 - n)) else '')
        for n, m in enumerate(flgname)]
      s.append(' flag[%3d]: %s' % (i, ''.join('%8s' % _ for _ in sflg)))
    for i, cp in enumerate(self.curvePoints):
      s.append(' coords[%3d]: (%6d, %6d)' % (i, cp.x, cp.y))
    return '\n'.join(s)

class TTFTag(TTFBaseTable):
  def __init__(self, *args, **kwargs):
    self.propinfo = [
      ('4s', 'tag'), ('L', 'checkSum'), ('L', 'offset'), ('L', 'length')]
    super(TTFTag, self).__init__(*args, **kwargs)

class TTFInfo(TTFBaseTable):
  def __init__(self, *args, **kwargs):
    self.propinfo = [
      ('L', 'sfntVersion'), # 2H
      ('H', 'numTables'), ('H', 'searchRange'),
      ('H', 'entrySelector'), ('H', 'rangeShift')]
    super(TTFInfo, self).__init__(*args, **kwargs)

class TTFont(object):
  def __init__(self, fontfilename):
    self.fontfilename = fontfilename
    lg.debug('font file: %s' % self.fontfilename)
    if self.fontfilename is None: return
    if not os.path.exists(self.fontfilename):
      lg.error('font does not exist.\a\n')
      self.fontfilename = None
      return
    f = open(self.fontfilename, 'rb')
    self.info = TTFInfo(f)
    lg.debug(self.info)
    lg.debug('tables: (No.: tag, checkSum, offset, length)')
    self.offsets = {}
    for i in xrange(self.info.numTables):
      tag = TTFTag(f)
      # lg.debug(tag)
      lg.debug(' %02d: %s %08x, %08x, %8d' % (
        i, tag.tag, tag.checkSum, tag.offset, tag.length))
      self.offsets[tag.tag] = tag.offset
    for tag in ('cmap', 'maxp', 'head', 'loca'):
      lg.debug('%s table: %08x' % (tag, self.offsets[tag]))
      f.seek(self.offsets[tag], 0)
      setattr(self, '_%s' % tag, getattr(self, 'proc_%s' % tag)(tag, f))
    f.close()

  def get_glyph(self, ch):
    idx = self._cmap[0][ord(ch)]
    if self._loca[idx] == self._loca[idx+1]: return None
    f = open(self.fontfilename, 'rb')
    w = 1 if self._head.indexToLocFormat else 2
    f.seek(self.offsets['glyf'] + w * self._loca[idx], 0)
    glyfHead = TTFGlyfHead(f)
    lg.debug(glyfHead)
    if glyfHead.numContours < 0:
      lg.error('multi-glyph is not supported\a\n')
      return None
    glyfSingle = TTFGlyfSingle(glyfHead, f)
    lg.debug(glyfSingle)
    f.close()
    return glyfSingle

  def show_dmp(self, s, m, d, w):
    dmp = ['%s%s:' % (' ' * s, m)]
    r = 32 / w
    for j in xrange(len(d) / r):
      dmps = ['%s%04x:' % (' ' * (s + 2), r * j)]
      for i in xrange(r):
        if w == 2 or 4 or 8: dmps.append((' %%0%dx' % w) % d[r * j + i])
        else: dmps.append(' ..')
      dmp.append(''.join(dmps))
    lg.debug('\n'.join(dmp))

  def proc_cmap(self, tag, f):
    idx = {}
    cmap = TTFCmap(f)
    lg.debug(cmap)
    for i in xrange(cmap.numEncodingTables):
      f.seek(self.offsets[tag] + 4 + 8 * i, 0) # 2H + (2HL * i)
      cet = TTFCmapEncodingTable(f)
      lg.debug('  encoding table %02d:\n%s' % (i, cet))
      f.seek(self.offsets[tag] + cet.offset, 0)
      cstf = TTFCmapSubTableFormat(f)
      lg.debug('   format: %d length: %d version: %d' % (
        cstf.format, cstf.length, cstf.version))
      if cstf.format == 0:
        glyphIdArray = struct.unpack('>256B', f.read(256))
        self.show_dmp(4, 'glyphIdArray', glyphIdArray, 2)
        idx[cet.specificEncodingId] = glyphIdArray
      elif cstf.format == 2:
        subHeaderKeys = struct.unpack('>256H', f.read(512))
        '''
        subHeaders = [struct.unpack('>4H', f.read(8)) for j in xrange(m)]
        for j in xrange(m):
          firstCode, entryCount, idDelta, idRangeOffset = subHeaders[j]
        glyphIndexArray = struct.unpack('>%dH' % n, f.read(2 * n))
        '''
        # self.show_dmp(4, 'glyphIndexArray', glyphIndexArray, 4)
        # idx[cet.specificEncodingId] = glyphIndexArray
      elif cstf.format == 4:
        p = struct.unpack('>4H', f.read(8))
        segCountX2, searchRange, entrySelector, rangeShift = p
        fmt = '>%dH' % (segCountX2 / 2)
        endCount = struct.unpack(fmt, f.read(segCountX2))
        reservedPad = struct.unpack('>H', f.read(2))[0]
        startCount = struct.unpack(fmt, f.read(segCountX2))
        idDelta = struct.unpack(fmt, f.read(segCountX2))
        idRangeOffset = struct.unpack(fmt, f.read(segCountX2))
        # glyphIdArray = struct.unpack('>%dH' % n, f.read(2 * n))
        lg.debug('    segCountX2: %d' % segCountX2)
        lg.debug('    searchRange: %d' % searchRange)
        lg.debug('    entrySelector: %d' % entrySelector)
        lg.debug('    rangeShift: %d' % rangeShift)
        lg.debug('    endCount[%d]: (segCount): -' % (segCountX2 / 2))
        lg.debug('    reservedPad: %d' % reservedPad)
        lg.debug('    startCount[%d]: (segCount): -' % (segCountX2 / 2))
        lg.debug('    idDelta[%d]: (segCount): -' % (segCountX2 / 2))
        lg.debug('    idRangeOffset[%d]: (segCount): -' % (segCountX2 / 2))
        lg.debug('    glyphIdArray[]: (arbitrary length): -')
        # self.show_dmp(4, 'glyphIdArray', glyphIdArray, 4)
        # idx[cet.specificEncodingId] = glyphIdArray
      elif cstf.format == 6:
        firstCode, entryCount = struct.unpack('>2H', f.read(4))
        fmt = '>%dH' % entryCount
        glyphIdArray = struct.unpack(fmt, f.read(struct.calcsize(fmt)))
        lg.debug('    firstCode: %d' % firstCode)
        lg.debug('    entryCount: %d' % entryCount)
        self.show_dmp(4, 'glyphIdArray', glyphIdArray, 4)
        idx[cet.specificEncodingId] = ((0, ) * firstCode) + glyphIdArray
      elif cstf.format == 8:
        lg.error('\nnot supported format (%d)\a\n' % cstf.format)
      elif cstf.format == 10:
        lg.error('\nnot supported format (%d)\a\n' % cstf.format)
      elif cstf.format == 12:
        lg.error('\nnot supported format (%d)\a\n' % cstf.format)
      elif cstf.format == 14:
        lg.error('\nnot supported format (%d)\a\n' % cstf.format)
      else: lg.error('\nbad cstf.format\a\n')
    return idx

  def proc_maxp(self, tag, f):
    maxp = TTFMaxp(f)
    if maxp.table_version != 0x00000005:
      lst = [
        ('H', 'maxPoints'), ('H', 'maxContours'),
        ('H', 'maxCompositePoints'), ('H', 'maxCompositeContours'),
        # ('H', 'maxZones'),
        ('H', 'maxTwilightPoints'), ('H', 'maxStorage'),
        ('H', 'maxFunctionDefs'), ('H', 'maxInstructionDefs'),
        ('H', 'maxStackElements'), ('H', 'maxSizeOfInstructions'),
        ('H', 'maxComponentElements'), ('H', 'maxComponentDepth')]
      maxp.set_appendix(f, lst)
    lg.debug(maxp)
    return maxp

  def proc_head(self, tag, f):
    head = TTFHead(f)
    lg.debug(head)
    return head

  def proc_loca(self, tag, f):
    if len(self._cmap) == 0: return None
    w = 'L' if self._head.indexToLocFormat else 'H'
    fmt = '>%d%c' % (self._maxp.numGlyphs + 1, w)
    loca = struct.unpack(fmt, f.read(struct.calcsize(fmt)))
    self.show_dmp(4, tag, loca, 8 if w == 'L' else 4)
    return loca

if __name__ == '__main__':
  global lg
  lg = logging.getLogger()
  console_log = logging.StreamHandler()
  console_log.setLevel(logging.ERROR)
  console_log.setFormatter(
    logging.Formatter('%(asctime)s %(levelname)s %(message)s'))
  lg.addHandler(console_log)
  file_log = logging.FileHandler(filename='log.log', mode='w')
  file_log.setLevel(logging.DEBUG)
  file_log.setFormatter(
    logging.Formatter('%(asctime)s %(levelname)s %(message)s'))
  lg.addHandler(file_log)
  lg.setLevel(logging.DEBUG) # for root logger
  fontfilename = None # './verdana.ttf' # './mikaP.ttf'
  if len(sys.argv) > 1: fontfilename = sys.argv[1]
  app = MyApp(ttf=TTFont(fontfilename), redirect=False)
  app.MainLoop()
