#!/usr/local/bin/python
# -*- coding: utf-8 -*-
'''TestBezier
'''

import wx

class TestBezierPath(object):
  def __init__(self):
    pass

  def paint(self, gc):
    print 'drawing path'
    gc.SetPen(wx.Pen(wx.BLACK, 1))
    gc.SetBrush(wx.Brush(wx.BLACK, style=wx.SOLID))
    path = gc.CreatePath()
    path.MoveToPoint(wx.Point2D(10, 10))
    # path.AddQuadCurveToPoint(10, 150, 100, 100)
    path.AddCurveToPoint(
      wx.Point2D(10, 150), wx.Point2D(100, 150), wx.Point2D(100, 100))
    path.MoveToPoint(wx.Point2D(50, 10))
    path.AddCurveToPoint(
      wx.Point2D(50, 150), wx.Point2D(140, 150), wx.Point2D(140, 100))
    path.AddLineToPoint(10, 10) # (auto close when fill)
    gc.DrawPath(path) # FillPath and StrokePath

class TestBezierPane(wx.Panel):
  def __init__(self, *args, **kwargs):
    super(TestBezierPane, self).__init__(*args, **kwargs)
    self.path = TestBezierPath()
    self.SetBackgroundColour(wx.WHITE)
    # self.SetDoubleBuffered(True) # must use wx.MemoryDC and blit to dc
    self.Bind(wx.EVT_PAINT, self.OnPaint)

  def OnPaint(self, event):
    # event.Skip()
    dc = wx.PaintDC(self)
    dc.BeginDrawing()
    self.drawTestRects(dc)
    gc = wx.GraphicsContext.Create(dc)
    gc.PushState()
    self.path.paint(gc)
    gc.PopState()
    dc.EndDrawing()

  def drawTestRects(self, dc):
    dc.SetBrush(wx.Brush(wx.BLACK, style=wx.SOLID))
    dc.DrawRectangle(50, 50, 50, 50)
    dc.DrawRectangle(100, 100, 100, 100)

class TestBezierFrame(wx.Frame):
  def __init__(self, *args, **kwargs):
    super(TestBezierFrame, self).__init__(*args, **kwargs)
    sz = wx.BoxSizer(wx.HORIZONTAL)
    self.pnl = TestBezierPane(self, wx.NewId(), style=wx.TAB_TRAVERSAL)
    sz.Add(self.pnl, 1, wx.EXPAND)
    self.SetSizer(sz)
    self.Show()

if __name__ == '__main__':
  app = wx.App(False)
  frm = TestBezierFrame(parent=None, id=wx.NewId(),
    title=u'Test Bezier', size=(640, 480), pos=(320, 240))
  app.MainLoop()
