object frmTranscription: TfrmTranscription
  Left = 479
  Top = 165
  Caption = 'Transcription'
  ClientHeight = 400
  ClientWidth = 600
  Color = clBtnFace
  Constraints.MinHeight = 250
  Constraints.MinWidth = 250
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object memoMain: TRichEdit
    Left = 0
    Top = 33
    Width = 600
    Height = 367
    TabStop = False
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    HideScrollBars = False
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
    WantTabs = True
  end
  object pnlTop: TPanel
    Left = 0
    Top = 0
    Width = 600
    Height = 33
    Align = alTop
    TabOrder = 1
    ExplicitWidth = 444
    DesignSize = (
      600
      33)
    object lblFileName: TLabel
      Left = 8
      Top = 8
      Width = 16
      Height = 13
      Caption = 'File'
    end
    object edFile: TEdit
      Left = 40
      Top = 5
      Width = 549
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      Color = clBtnFace
      ReadOnly = True
      TabOrder = 0
      ExplicitWidth = 393
    end
  end
end
